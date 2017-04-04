################################################################################
# Package: DataQualityConfigurations
################################################################################

# Declare the package name:
atlas_subdir( DataQualityConfigurations )

# Requires DQI for binary build - this turns out not to do much, but keep for indication
# Actual heavy lifting done by DEPENDS in add_custom_command
atlas_depends_on_subdirs(PRIVATE DataQuality/DataQualityInterfaces)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( scripts/MergeConfigs.py scripts/UploadDQAMITag.py scripts/merge_all_han_configs.sh scripts/merge_some_han_configs.sh scripts/han-config-indent.sh scripts/han-config-check.sh scripts/han-config-parse.awk )

execute_process(
  COMMAND git log -1 --format=%H
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Merge all han configurations
set( han_config_files collisions_run collisions_minutes10
                      cosmics_run cosmics_minutes10
		      heavyions_run heavyions_minutes10 )
set( hcfgfiles )
set( BUILD_BIN_DIR ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/bin )
set( BUILD_LIB_DIR ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/lib )
foreach (hanfile IN LISTS han_config_files)
  add_custom_command(
    OUTPUT ${CMAKE_DATA_OUTPUT_DIRECTORY}/DataQualityConfigurations/${hanfile}.hcfg
    COMMENT "merge & build han configuration ${hanfile}"
    COMMAND env PATH=${BUILD_BIN_DIR}:$ENV{PATH} LD_LIBRARY_PATH=${BUILD_LIB_DIR}:$ENV{LD_LIBRARY_PATH} which han-config-gen
    COMMAND PATH=${BUILD_BIN_DIR}:$ENV{PATH} LD_LIBRARY_PATH=${BUILD_LIB_DIR}:$ENV{LD_LIBRARY_PATH} env ldd `which han-config-gen`
    # Merge
    COMMAND ${CMAKE_COMMAND} -E env GIT_COMMIT_HASH=${GIT_COMMIT_HASH}
      ${CMAKE_CURRENT_SOURCE_DIR}/scripts/MergeConfigs.py ${hanfile}.config
      ${CMAKE_CURRENT_SOURCE_DIR}/config
      ${CMAKE_CURRENT_BINARY_DIR}/${hanfile}.config
    # Build binary config
    COMMAND PATH=${BUILD_BIN_DIR}:$ENV{PATH} LD_LIBRARY_PATH=${BUILD_LIB_DIR}:$ENV{LD_LIBRARY_PATH} env han-config-gen
      ${CMAKE_CURRENT_BINARY_DIR}/${hanfile}.config
    # Deploy binary config
    COMMAND ${CMAKE_COMMAND} -E copy
      ${CMAKE_CURRENT_BINARY_DIR}/${hanfile}.hcfg
      ${CMAKE_DATA_OUTPUT_DIRECTORY}/DataQualityConfigurations/${hanfile}.hcfg
    DEPENDS scripts/MergeConfigs.py config/*/${hanfile}.config
  )
  set(hcfgfiles ${hcfgfiles} ${CMAKE_DATA_OUTPUT_DIRECTORY}/DataQualityConfigurations/${hanfile}.hcfg)
endforeach(hanfile)

add_custom_target( han_configs ALL SOURCES ${hcfgfiles} )
add_dependencies( han_configs han-config-gen )

add_dependencies( Package_DataQualityConfigurations han_configs )
