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

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Trigger/TriggerCommon/TriggerJobOpts
                          Trigger/TriggerCommon/TriggerMenu )

# Function to build trigger menu:
function( atlas_build_trigger_menu menu )

   # Don't do anything in release recompilation dryrun mode. In all other
   # modes, proceed as usual.
   if( ATLAS_RELEASE_RECOMPILE_DRYRUN )
      return()
   endif()

   # Command to build trigger menu. The idea is that ${menu}.stamp gets
   # created as the last command, should the menu generation succeed. So
   # that after a successful menu generation it wouldn't be attempted
   # again. But in order for the installation step to not try to re-generate
   # the menu in case it was the generation itself that failed, another
   # stamp file, ${menu}.attempted.stamp is created as the first command.
   # The menu is then only generated as part of the installation step if
   # this ${menu}.attempted.stamp file doesn't even exist.
   add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${menu}.stamp
      COMMAND ${CMAKE_COMMAND} -E touch
      ${CMAKE_CURRENT_BINARY_DIR}/${menu}.attempted.stamp
      COMMAND ${CMAKE_COMMAND} -E make_directory
      ${CMAKE_CURRENT_BINARY_DIR}/Menus/${menu}
      COMMAND ${CMAKE_BINARY_DIR}/atlas_build_run.sh
      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/XMLDumperFromAthena.sh -r ${CMAKE_PROJECT_VERSION} ${menu}
      ${CMAKE_CURRENT_BINARY_DIR}/Menus/${menu}
      COMMAND ${CMAKE_COMMAND} -E make_directory
      ${CMAKE_XML_OUTPUT_DIRECTORY}/TriggerMenuXML
      COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_CURRENT_BINARY_DIR}/Menus/${menu}/
      ${CMAKE_XML_OUTPUT_DIRECTORY}/TriggerMenuXML
      COMMAND ${CMAKE_COMMAND} -E touch
      ${CMAKE_CURRENT_BINARY_DIR}/${menu}.stamp
      DEPENDS "Package_$<JOIN:$<TARGET_PROPERTY:ATLAS_PACKAGES_TARGET,ATLAS_PACKAGES>,;Package_>" )

   # Create custom target and add it to package dependencies
   add_custom_target( build_menu_${menu} ALL SOURCES
      ${CMAKE_CURRENT_BINARY_DIR}/${menu}.stamp )

   # In case the file generation failed, because it wasn't even attempted
   # (failure in another package), then try to run the generation as part
   # of the installation. Note that apparently chaining commands inside a
   # single execute_process(...) call doesn't work correctly during installation
   # for some reason. Hence it's taken apart into 3 separate calls.
   install( CODE "if( NOT EXISTS
                     ${CMAKE_CURRENT_BINARY_DIR}/${menu}.attempted.stamp )
                     message( WARNING \"Generating trigger menu ${menu}\"
                              \" during the installation\" )
                     execute_process( COMMAND ${CMAKE_COMMAND} -E touch
                        ${CMAKE_CURRENT_BINARY_DIR}/${menu}.attempted.stamp )
                     execute_process(
                        COMMAND ${CMAKE_COMMAND} -E make_directory
                        ${CMAKE_CURRENT_BINARY_DIR}/Menus/${menu} )
                     execute_process(
                        COMMAND ${CMAKE_BINARY_DIR}/atlas_build_run.sh
                        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/XMLDumperFromAthena.sh
                        -r ${CMAKE_PROJECT_VERSION} ${menu} ${CMAKE_CURRENT_BINARY_DIR}/Menus/${menu} )
                  endif()" )

   # Install the generated XML files. Note that this installation rule is
   # executed after the previous code. So by this time the files should be
   # in place, if they could be produced.
   install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Menus/${menu}/
      DESTINATION ${CMAKE_INSTALL_XMLDIR}/TriggerMenuXML
      USE_SOURCE_PERMISSIONS
      FILES_MATCHING PATTERN "*.xml" )

   # Create a target that will depend on all the other targets, and will
   # print the "right message" at the end of the build. Notice that
   # we can't rely on the Package_TriggerMenuXML target here, since
   # the XML generation depends on all package targets being ready before
   # it could start. So it would cause a circular dependency to make the
   # menu targets be dependencies of the package target.
   if( NOT TARGET TriggerMenuXMLMain )
      add_custom_target( TriggerMenuXMLMain ALL
         COMMAND ${CMAKE_COMMAND} -E echo
         "TriggerMenuXML: Package build succeeded" )
   endif()
   add_dependencies( TriggerMenuXMLMain build_menu_${menu} )

endfunction ( atlas_build_trigger_menu )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/CreateTriggerMenuXML.sh )
atlas_install_scripts( scripts/XMLDumperFromAthena.sh )
atlas_install_xmls( data/*.xml )

# List of menus to be created:

atlas_build_trigger_menu( Physics_pp_v8 )
atlas_build_trigger_menu( Physics_pp_v8_lowmu )
atlas_build_trigger_menu( MC_pp_v8 )
atlas_build_trigger_menu( MC_pp_v8_BulkMCProd_mc_prescale )

atlas_build_trigger_menu( Physics_pp_v7 )
atlas_build_trigger_menu( Physics_pp_v7_tight_physics_prescale )
atlas_build_trigger_menu( Physics_pp_v7_unprescaled_only_physics_prescale )
atlas_build_trigger_menu( Physics_pp_v7_primaries )
atlas_build_trigger_menu( MC_pp_v7 )
atlas_build_trigger_menu( MC_pp_v7_BulkMCProd_mc_prescale )
atlas_build_trigger_menu( MC_pp_v7_CPSampleProd_mc_prescale )
atlas_build_trigger_menu( MC_pp_v7_TriggerValidation_mc_prescale )

atlas_build_trigger_menu( Physics_HI_v5 )
atlas_build_trigger_menu( Physics_HI_v5_tight_physics_prescale )
atlas_build_trigger_menu( MC_HI_v5 )
atlas_build_trigger_menu( MC_HI_v5_tight_mc_prescale )

atlas_build_trigger_menu( Physics_HI_v4 )
atlas_build_trigger_menu( MC_HI_v4 )
atlas_build_trigger_menu( MC_HI_v4_tight_mc_prescale )

#atlas_build_trigger_menu( MC_PhaseII )
