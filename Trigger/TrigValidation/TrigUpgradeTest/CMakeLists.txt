################################################################################
# Package: TrigUpgradeTest
################################################################################

# Declare the package name:
atlas_subdir( TrigUpgradeTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          GaudiKernel
                          Control/AthenaBaseComps
                          Trigger/TrigSteer/DecisionHandling
                          Trigger/TrigEvent/TrigSteeringEvent
                          )

atlas_add_component( TrigUpgradeTest
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps TrigSteeringEvent DecisionHandlingLib
                     )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_data( share/*.ref share/*.conf )
atlas_install_python_modules( python/*.py )
atlas_install_scripts( test/exec*.sh test/test*.sh )

# Unit tests:
atlas_add_test( ViewSchedule1 SCRIPT test/test_view_schedule.sh
                ENVIRONMENT THREADS=1 )
atlas_add_test( ViewSchedule2 SCRIPT test/test_view_schedule.sh
                ENVIRONMENT THREADS=2 )
atlas_add_test( ViewSchedule64 SCRIPT test/test_view_schedule.sh
                ENVIRONMENT THREADS=64 )

atlas_add_test( merge
   SCRIPT test/test_merge.sh
   PROPERTIES TIMEOUT 1000
   )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_IDRunMC )
atlas_add_test( IDRunMC
   SCRIPT test/test_id_run_mc.sh
   PROPERTIES TIMEOUT 1000
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_IDRunMC
   )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_IDRunData )
atlas_add_test( IDRunData SCRIPT test/test_id_run_data.sh
   PROPERTIES TIMEOUT 1000
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_IDRunData
   )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_egammaRunData )
atlas_add_test( egammaRunData
   SCRIPT test/test_egamma_run_data.sh
   PROPERTIES TIMEOUT 1000   
   EXTRA_PATTERNS "-s TrigSignatureMoniMT.*HLT_.*|Fragment size"
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_egammaRunData
   )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_decodeBS )
atlas_add_test( decodeBS
   SCRIPT test/test_decodeBS.sh
   PROPERTIES TIMEOUT 1000
   EXTRA_PATTERNS "-s FillMissingEDM.*(present|absent)"
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_decodeBS
   DEPENDS egammaRunData
   )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_photonMenu )
atlas_add_test( photonMenu
   SCRIPT test/test_photon_menu.sh
   PROPERTIES TIMEOUT 1000
   EXTRA_PATTERNS "-s TriggerSummaryStep.*HLT_.*|TriggerMonitorFinal.*HLT_.*|TrigSignatureMoniMT.*HLT_.*"
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_photonMenu
   )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_electronMenu )
atlas_add_test( electronMenu
   SCRIPT test/test_electron_menu.sh
   EXTRA_PATTERNS "-s TriggerSummaryStep.*HLT_.*|TriggerMonitorFinal.*HLT_.*|TrigSignatureMoniMT.*HLT_.*"
   PROPERTIES TIMEOUT 1000
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_electronMenu
   )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_muRunData )
atlas_add_test( muRunData
   SCRIPT test/test_mu_run_data.sh
   PROPERTIES TIMEOUT 1000
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_muRunData
   )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_muMenu )
atlas_add_test( muMenu
   SCRIPT test/test_mu_menu.sh
   EXTRA_PATTERNS "-s TriggerSummaryStep.*HLT_.*|TriggerMonitorFinal.*HLT_.*|TrigSignatureMoniMT.*HLT_.*"
   PROPERTIES TIMEOUT 1000
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_muMenu
   )


file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_tauMenu )
atlas_add_test( tauMenu
   SCRIPT test/test_tau_menu.sh
   EXTRA_PATTERNS "-s TriggerSummaryStep.*HLT_.*|TriggerMonitorFinal.*HLT_.*|TrigSignatureMoniMT.*HLT_.*"
   PROPERTIES TIMEOUT 1000
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_tauMenu
   )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_jet )
atlas_add_test( jetRunData
   SCRIPT test/test_jet.sh
   PROPERTIES TIMEOUT 1000
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_jet
   )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_jetMenu )
atlas_add_test( jetMenu
   SCRIPT test/test_jet_menu.sh
   EXTRA_PATTERNS "-s TriggerSummaryStep.*HLT_.*|TriggerMonitorFinal.*HLT_.*|TrigSignatureMoniMT.*HLT_.*"
   PROPERTIES TIMEOUT 1000
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_jetMenu
   )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_fullMenu )
atlas_add_test( fullMenu
   SCRIPT test/test_full_menu_build.sh
   POST_EXEC_SCRIPT "trig-art-result-parser.sh fullMenu.log \"athena CheckLog RegTest\""
   PROPERTIES TIMEOUT 1000
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_fullMenu
   )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_idCaloRunData )
atlas_add_test( idCaloRunData
   SCRIPT test/test_id_calo_run_data.sh
   PROPERTIES TIMEOUT 1000
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_idCaloRunData
   )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_CaloOnlyRunData )
atlas_add_test( CaloOnlyRunData
   SCRIPT test/test_calo_only_data.sh
   EXTRA_PATTERNS "-s .*ERROR.*|FastCaloL2EgammaAlg.*REGTEST*"
   PROPERTIES TIMEOUT 1000
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_CaloOnlyRunData
   )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_emu_l1_decoding )
atlas_add_test( EmuL1Decoding
   SCRIPT test/test_emu_l1_decoding.sh
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_emu_l1_decoding
   )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_emu_step_processing )
atlas_add_test( EmuStepProcessing
   SCRIPT	test/test_emu_step_processing.sh
   EXTRA_PATTERNS "-s TrigSignatureMoniMT.*INFO HLT_.*"
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_emu_step_processing
   )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_NewJO )
atlas_add_test( NewJO
   SCRIPT test/test_newJO_build.sh
   POST_EXEC_SCRIPT "trig-art-result-parser.sh NewJO.log \"Configuration athena CheckLog RegTest\""
   PROPERTIES TIMEOUT 1000
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_NewJO
   )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_RunMenuTest )
atlas_add_test( RunMenuTest
   SCRIPT test/test_runMenuTest.sh
   EXTRA_PATTERNS "-s .*ERROR (?\!attempt to add a duplicate).*|.*FATAL.*|TrigSignatureMoniMT .*INFO.*"
   PROPERTIES TIMEOUT 1000
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_RunMenuTest
   )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_pebTest )
atlas_add_test( pebTest
   SCRIPT test/test_peb.sh
   PROPERTIES TIMEOUT 1000
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_pebTest
   EXTRA_PATTERNS "-s , robs=\[|adds PEBInfo|TriggerSummary.*HLT_"
   )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_dataScoutingTest )
atlas_add_test( dataScoutingTest
   SCRIPT test/test_dataScouting.sh
   PROPERTIES TIMEOUT 1000
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_dataScoutingTest
   EXTRA_PATTERNS "-s , robs=\[|adds PEBInfo|ROBFragments with IDs|{module.*words}|TriggerSummary.*HLT_"
   )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_l1sim )
atlas_add_test( l1sim
   SCRIPT test/test_l1sim.sh
   PROPERTIES TIMEOUT 1000
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_l1sim
   )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_bJetMenu )
atlas_add_test( bJetMenu
   SCRIPT test/test_bjet_menu.sh
   PROPERTIES TIMEOUT 1000
   EXTRA_PATTERNS "-s TrigSignatureMoniMT.*HLT_.*"
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_bJetMenu
   )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_bJetMenuALLTE )
atlas_add_test( bJetMenuALLTE
   SCRIPT test/test_bjet_menuALLTE.sh
   PROPERTIES TIMEOUT 1000
   EXTRA_PATTERNS "-s TrigSignatureMoniMT.*HLT_.*"
   PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_bJetMenuALLTE
   )


file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_met_standalone )
atlas_add_test( met_standalone
    SCRIPT test/test_met_standalone.sh
    PROPERTIES TIMEOUT 1000
   	EXTRA_PATTERNS "-s METHypoAlg.*MET.*value" 	   
    PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_met_standalone
    )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_met_menu )
atlas_add_test( met_menu
    SCRIPT test/test_met_menu.sh
    PROPERTIES TIMEOUT 1000
   	EXTRA_PATTERNS "-s TrigSignatureMoniMT.*HLT_.*" 	   
    PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_met_menu 
    )
