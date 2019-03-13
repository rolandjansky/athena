################################################################################
# Package: TriggerMenuMT
################################################################################

# Declare the package name:
atlas_subdir( TriggerMenuMT )


# Declare the package's dependencies: 
atlas_depends_on_subdirs( PRIVATE
                          Trigger/TriggerCommon/TriggerJobOpts)

atlas_add_test ( pyflakesMenu
   SCRIPT scripts/pyflakesMenu.sh
   ENVIRONMENT "PYPATH=${CMAKE_CURRENT_SOURCE_DIR}/python ${CMAKE_CURRENT_SOURCE_DIR}/share ${CMAKE_CURRENT_SOURCE_DIR}/scripts"
)

atlas_add_test( flake8
   SCRIPT flake8 --select=F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python 
   POST_EXEC_SCRIPT nopost.sh )

#----------------------------------
# Function to build trigger menu:
function( atlas_build_lvl1_trigger_menu menu )

   # Don't do anything in release recompilation dryrun mode. In all other
   # modes, proceed as usual.
   if( ATLAS_RELEASE_RECOMPILE_DRYRUN )
      return()
   endif()

   # Command to build trigger menu. The idea is that ${menu}.stamp gets
   # created as the last command, should the menu generation succeed such that 
   # after a successful menu generation it wouldn't be attempted again.
   # In order for the installation step to not try to re-generate
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
      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/generateL1MenuMT.sh -r ${CMAKE_PROJECT_VERSION} ${menu}
      ${CMAKE_CURRENT_BINARY_DIR}/Menus/${menu}
      COMMAND ${CMAKE_COMMAND} -E make_directory
      ${CMAKE_XML_OUTPUT_DIRECTORY}/TriggerMenuMT
      COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_CURRENT_BINARY_DIR}/Menus/${menu}/
      ${CMAKE_XML_OUTPUT_DIRECTORY}/TriggerMenuMT
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
                        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/generateL1MenuMT.sh
                        -r ${CMAKE_PROJECT_VERSION} ${menu} ${CMAKE_CURRENT_BINARY_DIR}/Menus/${menu} )
                  endif()" )

   # Install the generated XML files. Note that this installation rule is
   # executed after the previous code. So by this time the files should be
   # in place, if they could be produced.
   install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Menus/${menu}/
      DESTINATION ${CMAKE_INSTALL_XMLDIR}/TriggerMenuMT
      USE_SOURCE_PERMISSIONS
      FILES_MATCHING PATTERN "*.xml" )

   # Create a target that will depend on all the other targets, and will
   # print the "right message" at the end of the build. Notice that
   # we can't rely on the Package_TriggerMenuXML target here, since
   # the XML generation depends on all package targets being ready before
   # it could start. So it would cause a circular dependency to make the
   # menu targets be dependencies of the package target.
   if( NOT TARGET TriggerMenuMTMain )
      add_custom_target( TriggerMenuMTMain ALL
         COMMAND ${CMAKE_COMMAND} -E echo
         "TriggerMenuMT: Package build succeeded" )
   endif()
   add_dependencies( TriggerMenuMTMain build_menu_${menu} )

endfunction ( atlas_build_lvl1_trigger_menu )



#----------------------------------
# Install files from the package:
atlas_install_python_modules( python/*.py 
			      python/LVL1MenuConfig
			      python/HLTMenuConfig
			      )
atlas_install_joboptions( share/*.py )

atlas_install_scripts( scripts/generateMenuMT.py )
atlas_install_scripts( scripts/generateL1MenuMT.sh )
atlas_install_scripts( scripts/generateLVL1MenuMT.py )
atlas_install_scripts( scripts/generateL1TopoMenuMT.py )
atlas_install_scripts( scripts/test_HLTmenu.sh )

atlas_install_xmls( data/*.dtd data/*.xml )

#atlas_add_test( generateMenuMT SCRIPT scripts/generateMenuMT.sh 
#                PROPERTIES TIMEOUT 500 
#                POST_EXEC_SCRIPT "check_log.pl --config checklogTriggerTest.conf generateMenuMT.log"
#              )


atlas_add_test( generateMenuMT_newJO SCRIPT python -m TriggerMenuMT.HLTMenuConfig.Menu.LS2_v1_newJO
                PROPERTIES TIMEOUT 500 
              )

atlas_add_test( generateMenuMT SCRIPT bash test_HLTmenu.sh
                PROPERTIES TIMEOUT 1000 
              )



#file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_generateMenu )
#atlas_add_test( generateMenu
#                SCRIPT scripts/test_HLTmenu.sh
#                EXTRA_PATTERNS "-s TriggerSummaryStep.*HLT_.*|TriggerMonitorFinal.*HLT_.*|TrigSignatureMoniMT.*HLT_.*"
#                PROPERTIES TIMEOUT 500
#                PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_generateMenu
#              )



#----------------------------------
# List of menus to be created:
atlas_build_lvl1_trigger_menu( LS2_v1 )



