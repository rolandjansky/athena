################################################################################
# Package: TriggerMenuXML
################################################################################
# This package creates the trigger menu xml files during the release build.
# The menus are defined below via 'atlas_build_trigger_menu'.
#
# make                       : Build all menus defined
# make build_menu_<menu>     : Build only <menu> (e.g. Physics_pp_v6)
#
# For cmake testing:
# TMXML_BUILD_TEST=1 make    : Create dummy XML files without running athena
#

# Declare the package name:
atlas_subdir( TriggerMenuXML )

# Function to build trigger menu:
function ( atlas_build_trigger_menu menu)
  # Get menu xml file names
  execute_process ( COMMAND python -c "from TriggerJobOpts.TriggerFlags import TriggerFlags as TF;TF.triggerMenuSetup='${menu}';print ('%s %s %s' % (TF.outputLVL1configFile(),TF.outputL1TopoConfigFile(),TF.outputHLTconfigFile()))"
    OUTPUT_VARIABLE stdout
    OUTPUT_STRIP_TRAILING_WHITESPACE )
  # Create list and prepend directory name
  string (REPLACE " " ";" menu_files ${stdout})
  foreach ( p ${menu_files} )
    list( APPEND XMLs ${CMAKE_XML_OUTPUT_DIRECTORY}/TriggerMenuXML/${p} )
  endforeach()
  # Command to build trigger menu
  add_custom_command ( OUTPUT ${XMLs}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_XML_OUTPUT_DIRECTORY}/TriggerMenuXML
    COMMAND ${CMAKE_BINARY_DIR}/atlas_build_run.sh ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/XMLDumperFromAthena.sh ${menu} ${CMAKE_XML_OUTPUT_DIRECTORY}/TriggerMenuXML
    DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/XMLDumperFromAthena.sh )
  # Create custom target and add it to package dependencies
  add_custom_target ( build_menu_${menu} ALL SOURCES ${XMLs} )
  add_dependencies ( Package_TriggerMenuXML build_menu_${menu} )
  # Install files
  install ( FILES ${XMLs}
    DESTINATION ${CMAKE_INSTALL_XMLDIR}/TriggerMenuXML )
endfunction ( atlas_build_trigger_menu )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/CreateTriggerMenuXML.sh )
atlas_install_scripts( scripts/XMLDumperFromAthena.sh )
atlas_install_xmls( data/*.xml )

# List of menus to be created:
atlas_build_trigger_menu ( Physics_pp_v6 )
atlas_build_trigger_menu ( MC_pp_v6 )
atlas_build_trigger_menu ( MC_pp_v6_loose_mc_prescale )
atlas_build_trigger_menu ( MC_pp_v6_tight_mc_prescale )
atlas_build_trigger_menu ( MC_pp_v6_tightperf_mc_prescale )

atlas_build_trigger_menu ( Physics_HI_v3 )
atlas_build_trigger_menu ( MC_HI_v3 )
atlas_build_trigger_menu ( MC_HI_v3_tight_mc_prescale )
