# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TriggerMenuMT )

#----------------------------------
# Function to build trigger menu:
function( atlas_build_lvl1_trigger_menu menu )

   # Don't do anything in release recompilation dryrun mode. In all other
   # modes, proceed as usual.
   if( ATLAS_RELEASE_RECOMPILE_DRYRUN )
      return()
   endif()

   cmake_parse_arguments( ARG "" "DEPENDS" "" ${ARGN} )

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
      COMMAND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/atlas_build_run.sh
      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/generateL1MenuMT.sh -r ${CMAKE_PROJECT_VERSION} ${menu}
      ${CMAKE_CURRENT_BINARY_DIR}/Menus/${menu}
      COMMAND ${CMAKE_COMMAND} -E make_directory
      ${CMAKE_XML_OUTPUT_DIRECTORY}/TriggerMenuMT
      COMMAND strace -o ${CMAKE_CURRENT_BINARY_DIR}/Menus/${menu}.copydir.strace
      ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/Menus/${menu}/
      ${CMAKE_XML_OUTPUT_DIRECTORY}/TriggerMenuMT
      #COMMAND ${CMAKE_COMMAND} -E touch           # do not create stamp file to force rebuild every time
      #${CMAKE_CURRENT_BINARY_DIR}/${menu}.stamp
      DEPENDS "Package_$<JOIN:$<TARGET_PROPERTY:ATLAS_PACKAGES_TARGET,ATLAS_PACKAGES>,;Package_>" )

   # Create custom target and add it to package dependencies
   add_custom_target( build_menu_${menu} ALL SOURCES
      ${CMAKE_CURRENT_BINARY_DIR}/${menu}.stamp )

   # Optional dependency on other menu
   if ( ARG_DEPENDS )
      add_dependencies( build_menu_${menu} build_menu_${ARG_DEPENDS} )
   endif()

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
                        COMMAND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/atlas_build_run.sh
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
         COMMENT "Package build succeeded"
         COMMAND ${CMAKE_COMMAND} -E echo
         "TriggerMenuMT: Package build succeeded" )
   endif()
   add_dependencies( TriggerMenuMTMain build_menu_${menu} )

endfunction ( atlas_build_lvl1_trigger_menu )

#----------------------------------
# Install files from the package:
atlas_install_python_modules( python/*.py
                              python/LVL1MenuConfig
                              python/L1
                              python/HLTMenuConfig
                              python/TriggerAPI
                              POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )

atlas_install_scripts( scripts/generateLVL1MenuMT.py
                       scripts/generateL1TopoMenuMT.py
                       scripts/generateL1MenuRun3.py
                       scripts/trigCompareOldandNewL1Menus.py
                       scripts/verify_menu_config.py
                       scripts/test_full_menu_cf.py
                       scripts/generateBunchGroupSetFromOldKey.py
                       scripts/extract_chain_from_json.py
                       scripts/runTriggerAPIExample.py
                       scripts/generateUnprescaledLists.py
                       POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Shell scripts without flake8 checking:
atlas_install_scripts( scripts/generateL1MenuMT.sh
                       scripts/trigL1MenuMigrationCheck.sh
                       scripts/test_HLTmenu.sh )

atlas_install_joboptions( share/*.py
                          POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F821 )

# Temporary copy of LVL1 XML DTD (remove this and the file once XML menu is deprecated)
atlas_install_xmls( share/*.dtd )

# Tests:
file( REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_generateMenuMT_newJO )
file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_generateMenuMT_newJO )
atlas_add_test( generateMenuMT_newJO
                SCRIPT python -m TriggerMenuMT.HLTMenuConfig.Menu.LS2_v1_newJO
                PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_generateMenuMT_newJO
                PROPERTIES TIMEOUT 500
                POST_EXEC_SCRIPT nopost.sh )

file( REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_ViewCFTest )
file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_ViewCFTest )
atlas_add_test( ViewCFTest
                SCRIPT python -m unittest -v TriggerMenuMT.HLTMenuConfig.Test.ViewCFTest
                PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_ViewCFTest
                POST_EXEC_SCRIPT nopost.sh )

file( REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_generateMenuMT )
file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_generateMenuMT )
atlas_add_test( generateMenuMT
                SCRIPT test_HLTmenu.sh
                PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_generateMenuMT
                PROPERTIES TIMEOUT 4000
                POST_EXEC_SCRIPT nopost.sh )

file( REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_full_menu_cf )
file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_full_menu_cf )
atlas_add_test( full_menu_cf
                SCRIPT scripts/test_full_menu_cf.sh
                PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_full_menu_cf 
                PROPERTIES TIMEOUT 500
                POST_EXEC_SCRIPT nopost.sh )
                
file( REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_LS2_emu_menu_DH )
file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_LS2_emu_menu_DH )
atlas_add_test( LS2_emu_menu_DH
                SCRIPT test_emu_step_menu_processing.sh
                PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_LS2_emu_menu_DH
                PROPERTIES TIMEOUT 500
                POST_EXEC_SCRIPT nopost.sh )

file( REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_EventBuildingSequenceSetup )
file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_EventBuildingSequenceSetup )
atlas_add_test( EventBuildingSequenceSetup
                SCRIPT python -m TriggerMenuMT.HLTMenuConfig.CommonSequences.EventBuildingSequenceSetup
                PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_EventBuildingSequenceSetup
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( checkMenuPrimaries SCRIPT scripts/checkMenuPrimaries.py 
                PROPERTIES TIMEOUT 200 
                POST_EXEC_SCRIPT "check_log.py --errors --config checklogTriggerTest.conf checkMenuPrimaries.log" 
              )

#----------------------------------
# List of menus to be created:
atlas_build_lvl1_trigger_menu( Physics_pp_run3_v1 )
atlas_build_lvl1_trigger_menu( PhysicsP1_pp_run3_v1 )
atlas_build_lvl1_trigger_menu( MC_pp_run3_v1 )
atlas_build_lvl1_trigger_menu( Cosmic_run3_v1 )
atlas_build_lvl1_trigger_menu( PhysicsP1_HI_run3_v1 )
atlas_build_lvl1_trigger_menu( Dev_HI_run3_v1 )

# Build of different prescales for same menu need to be serialized (ATR-22387):
atlas_build_lvl1_trigger_menu( LS2_v1 )
atlas_build_lvl1_trigger_menu( LS2_v1_TriggerValidation_prescale DEPENDS LS2_v1 )
atlas_build_lvl1_trigger_menu( LS2_v1_BulkMCProd_prescale DEPENDS LS2_v1_TriggerValidation_prescale )
atlas_build_lvl1_trigger_menu( LS2_v1_CPSampleProd_prescale DEPENDS LS2_v1_BulkMCProd_prescale )
