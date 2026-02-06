# SPDX-License-Identifier: Apache-2.0

zephyr_sources(flash_map.c)

# Generate flash map C code from devicetree unless using custom flash map
if(NOT CONFIG_FLASH_MAP_CUSTOM)
  set(FLASH_MAP_GENERATED_C ${PROJECT_BINARY_DIR}/flash_map_generated.c)
  set(GEN_FLASH_MAP_SCRIPT ${ZEPHYR_BASE}/scripts/dts/gen_flash_map.py)
  
  # Determine if labels are enabled
  if(CONFIG_FLASH_MAP_LABELS)
    set(FLASH_MAP_LABELS_ARG --labels)
  else()
    set(FLASH_MAP_LABELS_ARG "")
  endif()
  
  # Add command to generate flash map C file
  add_custom_command(
    OUTPUT ${FLASH_MAP_GENERATED_C}
    COMMAND ${PYTHON_EXECUTABLE} ${GEN_FLASH_MAP_SCRIPT}
            --edt-pickle ${EDT_PICKLE}
            --output ${FLASH_MAP_GENERATED_C}
            ${FLASH_MAP_LABELS_ARG}
    DEPENDS ${EDT_PICKLE} ${GEN_FLASH_MAP_SCRIPT}
    COMMENT "Generating flash_map_generated.c from devicetree"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  )
  
  # Add the generated file as a source
  zephyr_library_sources(${FLASH_MAP_GENERATED_C})
else()
  # Use the default file for custom flash maps
  zephyr_sources(flash_map_default.c)
endif()

zephyr_sources_ifdef(CONFIG_FLASH_MAP_SHELL flash_map_shell.c)
zephyr_sources_ifdef(CONFIG_FLASH_PAGE_LAYOUT flash_map_layout.c)
zephyr_sources_ifdef(CONFIG_FLASH_AREA_CHECK_INTEGRITY flash_map_integrity.c)

zephyr_library_link_libraries_ifdef(CONFIG_MBEDTLS mbedTLS)
