# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TriggerMenuMT )

# Install files from the package:
atlas_install_python_modules( python/*.py
                              python/L1
                              python/HLT
                              python/TriggerAPI
                              python/CFtest
                              POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select=ATL900,ATL901 )

atlas_install_scripts( scripts/generateL1MenuRun3.py
                       scripts/verify_menu_config.py
                       scripts/generateBunchGroupSetFromOldKey.py
                       scripts/trigmenu_modify_prescale_json.py
                       scripts/extract_chain_from_json.py
                       scripts/runTriggerAPIExample.py
                       scripts/generateUnprescaledLists.py
                       scripts/test_menu_dump.py
                       POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Shell scripts without flake8 checking:
atlas_install_scripts( scripts/test_HLTmenu.sh
                       scripts/test_manual_menu_cf.sh
                       scripts/test_emu_step_menu_processing_cf.sh
                       scripts/test_emu_step_processing_cf.sh )

atlas_install_joboptions( share/*.py
                          POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F821 )

# Control flow tests
foreach(test emu_step_processing_cf emu_step_menu_processing_cf)
   atlas_add_test( ${test}
                   SCRIPT test_${test}.sh
                   LOG_SELECT_PATTERN "TrigSignatureMoni.*INFO HLT_.*|TrigSignatureMoni.*-- #[0-9]+ (Events|Features).*|TriggerSummaryStep.* chains passed:|TriggerSummaryStep.*+++ HLT_.*|TriggerSummaryStep.*+++ leg.*"
                   PRIVATE_WORKING_DIRECTORY )
endforeach()

atlas_add_test( manual_menu_cf
                SCRIPT test_manual_menu_cf.sh
                PRIVATE_WORKING_DIRECTORY
                POST_EXEC_SCRIPT noerror.sh )

atlas_add_test( emu_step_processing_cf_newJO
                SCRIPT python -m TriggerMenuMT.CFtest.test_menu_cf_newJO
                LOG_SELECT_PATTERN "TrigSignatureMoni.*INFO HLT_.*|TrigSignatureMoni.*-- #[0-9]+ (Events|Features).*|TriggerSummaryStep.* chains passed:|TriggerSummaryStep.*+++ HLT_.*|TriggerSummaryStep.*+++ leg.*"
                PRIVATE_WORKING_DIRECTORY  )

# Unit tests:
atlas_add_test( ViewCFTest
                SCRIPT python -m unittest -v TriggerMenuMT.HLT.Test.ViewCFTest
                POST_EXEC_SCRIPT noerror.sh )

atlas_add_test( EventBuildingSequences
                SCRIPT python -m TriggerMenuMT.HLT.CommonSequences.EventBuildingSequences
                POST_EXEC_SCRIPT noerror.sh )

atlas_add_test( checkMenuPrimaries SCRIPT scripts/checkMenuPrimaries.py 
                POST_EXEC_SCRIPT "check_log.py --errors --config checklogTriggerTest.conf checkMenuPrimaries.log" )

atlas_add_test( RecoFragmentsPool
                SCRIPT python -m unittest -v TriggerMenuMT.HLT.Test.RecoFragmentsPoolTest
                POST_EXEC_SCRIPT noerror.sh )

# Test L1/HLT trigger menus:
function( atlas_test_lvl1_trigger_menu menu )
   atlas_add_test( "L1_${menu}"
                   SCRIPT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/generateL1MenuRun3.py ${menu}
                   PRIVATE_WORKING_DIRECTORY
                   POST_EXEC_SCRIPT "check_log.py --errors --config checklogTriggerTest.conf ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/unitTestRun_L1_${menu}/L1_${menu}.log" )
endfunction()

function( atlas_test_hlt_trigger_menu menu )
   atlas_add_test( "HLT_${menu}"
                   SCRIPT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_HLTmenu.sh ${menu}
                   PROPERTIES TIMEOUT 500
                   PRIVATE_WORKING_DIRECTORY
                   POST_EXEC_SCRIPT "check_log.py --errors --config checklogTriggerTest.conf ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/unitTestRun_HLT_${menu}/HLT_${menu}.log" )
endfunction()

# For other menus, fast check that HLT does not include non-existent L1
function( atlas_test_l1defined_menu menu )
   atlas_add_test( "L1Defined_${menu}"
                   SCRIPT test_menu_dump.py -l -m ${menu}
                   POST_EXEC_SCRIPT noerror.sh )
endfunction()

# Test all L1 menus (as this is fast):
atlas_test_lvl1_trigger_menu( Dev_pp_run3_v1 )
atlas_test_lvl1_trigger_menu( Physics_pp_run3_v1 )
atlas_test_lvl1_trigger_menu( PhysicsP1_pp_run3_v1 )
atlas_test_lvl1_trigger_menu( PhysicsP1_pp_lowMu_run3_v1 )
atlas_test_lvl1_trigger_menu( MC_pp_run3_v1 )
atlas_test_lvl1_trigger_menu( Cosmic_run3_v1 )
atlas_test_lvl1_trigger_menu( PhysicsP1_HI_run3_v1 )
atlas_test_lvl1_trigger_menu( Dev_HI_run3_v1 )

# Test only most comprehensive HLT menus:
atlas_test_hlt_trigger_menu( Dev_pp_run3_v1_HLTReprocessing_prescale )
atlas_test_hlt_trigger_menu( Cosmic_run3_v1 )
atlas_test_hlt_trigger_menu( Dev_HI_run3_v1 )

# Test other HLT menus not included above
atlas_test_l1defined_menu( PhysicsP1_pp_run3_v1 )
atlas_test_l1defined_menu( PhysicsP1_pp_lowMu_run3_v1 )

# New job options test:
atlas_add_test(generateMenuMT_newJO
   SCRIPT python -m TriggerMenuMT.HLT.Menu.Dev_pp_run3_v1_newJO 
   PRIVATE_WORKING_DIRECTORY
   PROPERTIES TIMEOUT 500
   POST_EXEC_SCRIPT noerror.sh )

# test for menu name consistency
atlas_add_test( test_menu_dump
  SCRIPT test_menu_dump.py -p
  POST_EXEC_SCRIPT noerror.sh )
