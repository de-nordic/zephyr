#!/usr/bin/env python3

# Copyright (c) 2025 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

"""
Generate flash_map C code from devicetree information.

This script reads a pickled EDT (Edtlib DeviceTree) object and generates C code
for the flash map subsystem, including:
- Array of flash_area structures (default_flash_map)
- Individual partition objects (global_fixed_partition_ORD_<N>)
- Individual subpartition objects (global_fixed_subpartition_ORD_<N>)

The generated C code uses devicetree macros (DT_FIXED_PARTITION_ID, DEVICE_DT_GET, etc.)
to ensure consistency with the devicetree system and proper macro expansion at compile time.

This replaces the use of DT foreach macros in flash_map_default.c with direct C code generation.
"""

import argparse
import os
import pickle
import sys

# Add path to devicetree library
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python-devicetree', 'src'))

import edtlib_logger
from devicetree import edtlib


def parse_args():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument('--edt-pickle', required=True,
                        help='Path to pickled EDT object')
    parser.add_argument('--output', required=True,
                        help='Path to output C file')
    parser.add_argument('--labels', action='store_true',
                        help='Enable flash map labels support (CONFIG_FLASH_MAP_LABELS)')

    return parser.parse_args()


def node_z_path_id(node):
    """Get the DT_ identifier for a node (similar to gen_defines.py)."""
    # Based on how gen_defines.py creates path IDs
    # We need the exact format used in devicetree_generated.h
    path = node.path
    # Remove leading slash
    path = path.lstrip('/')
    # Replace path separators and special characters
    path = path.replace('/', '_S_')
    path = path.replace('@', '_')
    path = path.replace(',', '_')
    path = path.replace('-', '_')
    path = path.replace('.', '_')
    path = path.replace('+', '_')
    
    if path:
        return f"N_{path}"
    return "N"


def get_dt_node_id(node):
    """Get the full DT node identifier including the DT_ prefix."""
    return f"DT_{node_z_path_id(node)}"


def get_mtd_macro_for_partition(node):
    """Get the appropriate MTD macro for a partition node."""
    if node.parent and 'fixed-partitions' in node.parent.compats:
        return f"DT_MTD_FROM_FIXED_PARTITION({get_dt_node_id(node)})"
    elif node.parent and 'fixed-subpartitions' in node.parent.compats:
        return f"DT_MTD_FROM_FIXED_SUBPARTITION({get_dt_node_id(node)})"
    return None


def get_partition_offset_macro(node):
    """Get the macro to compute partition offset."""
    dt_node_id = get_dt_node_id(node)
    
    if node.parent and 'fixed-subpartitions' in node.parent.compats:
        # For subpartitions, use FIXED_PARTITION_NODE_OFFSET which handles address translation
        return f"FIXED_PARTITION_NODE_OFFSET({dt_node_id})"
    else:
        # For regular partitions, also use FIXED_PARTITION_NODE_OFFSET
        return f"FIXED_PARTITION_NODE_OFFSET({dt_node_id})"


def get_mtd_device_node(node):
    """
    Get the MTD (memory technology device) node for a partition or subpartition.
    
    For fixed-partitions: MTD is typically the flash controller (grandparent)
    For fixed-subpartitions: MTD is the flash controller of the parent partition
    """
    if not node.parent:
        return None
        
    if 'fixed-partitions' in node.parent.compats:
        # Regular partition: MTD is grandparent (flash controller)
        # Structure: flash-controller -> flash -> partitions -> partition
        if node.parent.parent:
            # Check if grandparent is soc-nv-flash, if so, MTD is great-grandparent
            if node.parent.parent.matching_compat == 'soc-nv-flash':
                return node.parent.parent.parent if node.parent.parent.parent else None
            else:
                return node.parent.parent
        return None
        
    elif 'fixed-subpartitions' in node.parent.compats:
        # Subpartition: MTD is the flash controller of the parent partition
        # Structure: ... -> partition (with fixed-subpartitions) -> subpartition
        parent_partition = node.parent.parent
        if parent_partition and parent_partition.parent:
            if parent_partition.parent.parent:
                # Check if it's soc-nv-flash
                if parent_partition.parent.parent.matching_compat == 'soc-nv-flash':
                    return parent_partition.parent.parent.parent if parent_partition.parent.parent.parent else None
                else:
                    return parent_partition.parent.parent
        return None
    
    return None


def get_partition_nodes(edt):
    """
    Get all partition and subpartition nodes from the devicetree.
    
    Returns a tuple of (partition_nodes, subpartition_nodes), where each
    is a list of nodes sorted by dependency order.
    """
    partition_nodes = []
    subpartition_nodes = []
    
    for node in edt.nodes:
        # Check if this is a partition node
        if node.parent and 'fixed-partitions' in node.parent.compats:
            # This is a regular partition
            # Check if the MTD device is enabled
            mtd_device = get_mtd_device_node(node)
            if mtd_device and mtd_device.status == 'okay':
                partition_nodes.append(node)
                
        elif node.parent and 'fixed-subpartitions' in node.parent.compats:
            # This is a subpartition
            # Check if parent partition's MTD device is enabled
            mtd_device = get_mtd_device_node(node)
            if mtd_device and mtd_device.status == 'okay':
                subpartition_nodes.append(node)
    
    # Sort by dependency order
    partition_nodes.sort(key=lambda n: n.dep_ordinal)
    subpartition_nodes.sort(key=lambda n: n.dep_ordinal)
    
    return partition_nodes, subpartition_nodes


def get_flash_device(node):
    """Get the flash device for a partition or subpartition node."""
    return get_mtd_device_node(node)


def get_device_macro(node):
    """Get the DEVICE_DT_GET macro call for a node's MTD device."""
    mtd_macro = get_mtd_macro_for_partition(node)
    if not mtd_macro:
        return "NULL"
    
    # Return the DEVICE_DT_GET macro call with the MTD macro
    return f"DEVICE_DT_GET({mtd_macro})"


def get_partition_id_macro(node):
    """Get the DT_FIXED_PARTITION_ID macro for a node."""
    dt_node_id = get_dt_node_id(node)
    return f"DT_FIXED_PARTITION_ID({dt_node_id})"


def get_partition_size_macro(node):
    """Get the DT_REG_SIZE macro for a node."""
    dt_node_id = get_dt_node_id(node)
    return f"DT_REG_SIZE({dt_node_id})"


def get_partition_label_macro(node):
    """Get the DT_PROP_OR macro for the label property."""
    dt_node_id = get_dt_node_id(node)
    return f"DT_PROP_OR({dt_node_id}, label, NULL)"


def get_partition_size(node):
    """Get the partition size from its reg property."""
    if 'reg' in node.props:
        reg = node.props['reg'].val
        if reg and len(reg) >= 2:
            return reg[1]
    return 0


def write_file_header(f, edt):
    """Write the file header with copyright and includes."""
    f.write("""/*
 * Copyright (c) 2017-2025 Nordic Semiconductor ASA
 * Copyright (c) 2015 Runtime Inc
 * Copyright (c) 2023 Sensorfy B.V.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This file is auto-generated by scripts/dts/gen_flash_map.py
 * DO NOT EDIT MANUALLY
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>
#include <zephyr/storage/flash_map.h>

""")


def write_flash_area_array(f, partition_nodes, subpartition_nodes, labels_enabled):
    """Write the default_flash_map array using DT macros."""
    f.write("/* Flash area array containing all partitions and subpartitions */\n")
    f.write("const struct flash_area default_flash_map[] = {\n")
    
    # Write all partitions
    for node in partition_nodes:
        partition_id_macro = get_partition_id_macro(node)
        offset_macro = get_partition_offset_macro(node)
        size_macro = get_partition_size_macro(node)
        device_macro = get_device_macro(node)
        
        f.write(f"\t{{\n")
        f.write(f"\t\t.fa_id = {partition_id_macro},\n")
        f.write(f"\t\t.fa_off = {offset_macro},\n")
        f.write(f"\t\t.fa_size = {size_macro},\n")
        f.write(f"\t\t.fa_dev = {device_macro},\n")
        
        if labels_enabled:
            label_macro = get_partition_label_macro(node)
            f.write(f"\t\t.fa_label = {label_macro},\n")
        
        f.write(f"\t}},\n")
    
    # Write all subpartitions
    for node in subpartition_nodes:
        partition_id_macro = get_partition_id_macro(node)
        offset_macro = get_partition_offset_macro(node)
        size_macro = get_partition_size_macro(node)
        device_macro = get_device_macro(node)
        
        f.write(f"\t{{\n")
        f.write(f"\t\t.fa_id = {partition_id_macro},\n")
        f.write(f"\t\t.fa_off = {offset_macro},\n")
        f.write(f"\t\t.fa_size = {size_macro},\n")
        f.write(f"\t\t.fa_dev = {device_macro},\n")
        
        if labels_enabled:
            label_macro = get_partition_label_macro(node)
            f.write(f"\t\t.fa_label = {label_macro},\n")
        
        f.write(f"\t}},\n")
    
    f.write("};\n\n")
    
    total_areas = len(partition_nodes) + len(subpartition_nodes)
    f.write(f"const int flash_map_entries = {total_areas};\n")
    f.write("const struct flash_area *flash_map = default_flash_map;\n\n")


def write_partition_objects(f, partition_nodes, subpartition_nodes, labels_enabled):
    """Write individual partition objects using DT macros."""
    # Write regular partition objects
    if partition_nodes:
        f.write("/* Individual partition objects */\n")
        for node in partition_nodes:
            partition_id_macro = get_partition_id_macro(node)
            offset_macro = get_partition_offset_macro(node)
            size_macro = get_partition_size_macro(node)
            device_macro = get_device_macro(node)
            
            var_name = f"global_fixed_partition_ORD_{node.dep_ordinal}"
            
            f.write(f"const struct flash_area {var_name} = {{\n")
            f.write(f"\t.fa_id = {partition_id_macro},\n")
            f.write(f"\t.fa_off = {offset_macro},\n")
            f.write(f"\t.fa_size = {size_macro},\n")
            f.write(f"\t.fa_dev = {device_macro},\n")
            
            if labels_enabled:
                label_macro = get_partition_label_macro(node)
                f.write(f"\t.fa_label = {label_macro},\n")
            
            f.write("};\n\n")
    
    # Write subpartition objects
    if subpartition_nodes:
        f.write("/* Individual subpartition objects */\n")
        for node in subpartition_nodes:
            partition_id_macro = get_partition_id_macro(node)
            offset_macro = get_partition_offset_macro(node)
            size_macro = get_partition_size_macro(node)
            device_macro = get_device_macro(node)
            
            var_name = f"global_fixed_subpartition_ORD_{node.dep_ordinal}"
            
            f.write(f"const struct flash_area {var_name} = {{\n")
            f.write(f"\t.fa_id = {partition_id_macro},\n")
            f.write(f"\t.fa_off = {offset_macro},\n")
            f.write(f"\t.fa_size = {size_macro},\n")
            f.write(f"\t.fa_dev = {device_macro},\n")
            
            if labels_enabled:
                label_macro = get_partition_label_macro(node)
                f.write(f"\t.fa_label = {label_macro},\n")
            
            f.write("};\n\n")


def main():
    """Main entry point."""
    args = parse_args()
    
    # Setup logging
    edtlib_logger.setup_edtlib_logging()
    
    # Load the pickled EDT
    with open(args.edt_pickle, 'rb') as f:
        edt = pickle.load(f)
    
    # Get partition nodes
    partition_nodes, subpartition_nodes = get_partition_nodes(edt)
    
    # Generate the C file
    with open(args.output, 'w', encoding='utf-8') as f:
        write_file_header(f, edt)
        write_flash_area_array(f, partition_nodes, subpartition_nodes, args.labels)
        write_partition_objects(f, partition_nodes, subpartition_nodes, args.labels)
    
    print(f"Generated flash map C file: {args.output}")
    print(f"  Partitions: {len(partition_nodes)}")
    print(f"  Subpartitions: {len(subpartition_nodes)}")
    print(f"  Total flash areas: {len(partition_nodes) + len(subpartition_nodes)}")


if __name__ == '__main__':
    main()

