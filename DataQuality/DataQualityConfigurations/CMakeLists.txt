# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DataQualityConfigurations )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( scripts/MergeConfigs.py scripts/UploadDQAMITag.py
   scripts/merge_all_han_configs.sh scripts/merge_some_han_configs.sh
   scripts/han-config-indent.sh scripts/han-config-check.sh
   scripts/han-config-parse.awk )

execute_process(
  COMMAND git log -1 --format=%H
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# The list of generated files:
set( hcfgfiles )

# Loop over all configurations:
foreach( hanfile collisions_run collisions_minutes10
      cosmics_run cosmics_minutes10 heavyions_run heavyions_minutes10 )

   # The name of the output file:
   set( outputFile
      ${CMAKE_DATA_OUTPUT_DIRECTORY}/DataQualityConfigurations/${hanfile}.hcfg )

   # Get a list of all files used by han-config-gen:
   file( GLOB configInputFiles
      ${CMAKE_CURRENT_SOURCE_DIR}/config/*/${hanfile}.config
      ${CMAKE_CURRENT_SOURCE_DIR}/config/*/*/${hanfile}.config )

   # The name of the generated config file:
   set( configOutputFile
      ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${hanfile}.config )

   # Set up the file generation command(s):
   add_custom_command(
      # The output to generate:
      OUTPUT ${outputFile}
      COMMENT "Merge & build han configuration ${hanfile}"
      # Merge
      COMMAND ${CMAKE_COMMAND} -E env GIT_COMMIT_HASH=${GIT_COMMIT_HASH}
      ${CMAKE_CURRENT_SOURCE_DIR}/scripts/MergeConfigs.py ${hanfile}.config
      ${CMAKE_CURRENT_SOURCE_DIR}/config ${configOutputFile}
      # Build binary config
      COMMAND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/atlas_build_run.sh
      han-config-gen ${configOutputFile} -b true
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
