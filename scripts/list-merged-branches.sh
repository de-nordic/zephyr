#!/bin/bash

# Copyright (c) 2026 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0

# Wrapper script for list_merged_branches.py
# Usage: ./scripts/list-merged-branches.sh [OPTIONS]

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

python3 "$SCRIPT_DIR/list_merged_branches.py" "$@"
