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

foreach (hanfile IN LISTS han_config_files)
  add_custom_command(
    OUTPUT ${CMAKE_DATA_OUTPUT_DIRECTORY}/DataQualityConfigurations/${hanfile}.hcfg
    COMMENT "merge & build han configuration ${hanfile}"
    # Merge
    COMMAND ${CMAKE_COMMAND} -E env GIT_COMMIT_HASH=${GIT_COMMIT_HASH}
      ${CMAKE_CURRENT_SOURCE_DIR}/scripts/MergeConfigs.py ${hanfile}.config
      ${CMAKE_CURRENT_SOURCE_DIR}/config ${configOutputFile}
      # Build binary config
      COMMAND ${CMAKE_BINARY_DIR}/atlas_build_run.sh han-config-gen
      ${configOutputFile}
      # Deploy binary config
      COMMAND ${CMAKE_COMMAND} -E copy
      ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${hanfile}.hcfg
      ${outputFile}
      # Dependencies for the command:
      DEPENDS scripts/MergeConfigs.py ${configInputFiles} )

   # Remember the generated file's name:
   list( APPEND hcfgfiles ${outputFile} )

   # Install the generated file:
   install( FILES ${outputFile}
      DESTINATION ${CMAKE_INSTALL_DATADIR}/DataQualityConfigurations OPTIONAL )

   # Clean up:
   unset( outputFile )
   unset( configInputFiles )
   unset( configOutputFile )

endforeach()

# Set up the custom target generating the configurations:
add_custom_target( DataQualityConfigurations_han_configs
   ALL SOURCES ${hcfgfiles} )
add_dependencies( DataQualityConfigurations_han_configs
   han-config-gen )
add_dependencies( Package_DataQualityConfigurations
   DataQualityConfigurations_han_configs )
set_target_properties( DataQualityConfigurations_han_configs PROPERTIES
   LABELS DataQualityConfigurations
   FOLDER DataQuality/DataQualityConfigurations )

# Clean up:
unset( hcfgfiles )
