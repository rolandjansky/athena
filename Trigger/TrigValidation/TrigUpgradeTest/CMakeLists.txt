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
atlas_add_test( ViewSchedule1  SCRIPT test/test_view_schedule.sh ENVIRONMENT THREADS=1 )
atlas_add_test( ViewSchedule2  SCRIPT test/test_view_schedule.sh ENVIRONMENT THREADS=2 )
atlas_add_test( ViewSchedule64 SCRIPT test/test_view_schedule.sh ENVIRONMENT THREADS=64 )
atlas_add_test( merge SCRIPT test/test_merge.sh PROPERTIES TIMEOUT 1000 )

# Helper to define unit tests running in a separate directory
# The test name is derived from the test script name: test/test_${name}.sh
function( _add_test name )
   set( rundir ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_${name} )
   file( REMOVE_RECURSE ${rundir} )   # cleanup to avoid interference with previous run
   file( MAKE_DIRECTORY ${rundir} )
   atlas_add_test( ${name}
      SCRIPT test/test_${name}.sh
      PROPERTIES TIMEOUT 1000
      PROPERTIES WORKING_DIRECTORY ${rundir}
      ${ARGN} )
endfunction( _add_test )

_add_test( id_run_mc )
_add_test( id_run_data )
_add_test( mu_run_data )
_add_test( id_calo_run_data )
_add_test( emu_l1_decoding )
_add_test( jet )
_add_test( l1sim )
_add_test( egamma_run_data EXTRA_PATTERNS "-s TrigSignatureMoniMT.*HLT_.*|Fragment size" )
_add_test( decodeBS EXTRA_PATTERNS "-s FillMissingEDM.*(present|absent)" PROPERTIES DEPENDS egammaRunData )
_add_test( photon_menu EXTRA_PATTERNS "-s TriggerSummaryStep.*HLT_.*|TriggerMonitorFinal.*HLT_.*|TrigSignatureMoniMT.*HLT_.*" )
_add_test( electron_menu EXTRA_PATTERNS "-s TriggerSummaryStep.*HLT_.*|TriggerMonitorFinal.*HLT_.*|TrigSignatureMoniMT.*HLT_.*" )
_add_test( mu_menu EXTRA_PATTERNS "-s TriggerSummaryStep.*HLT_.*|TriggerMonitorFinal.*HLT_.*|TrigSignatureMoniMT.*HLT_.*" )
_add_test( tau_menu EXTRA_PATTERNS "-s TriggerSummaryStep.*HLT_.*|TriggerMonitorFinal.*HLT_.*|TrigSignatureMoniMT.*HLT_.*" )
_add_test( jet_menu EXTRA_PATTERNS "-s TriggerSummaryStep.*HLT_.*|TriggerMonitorFinal.*HLT_.*|TrigSignatureMoniMT.*HLT_.*" )
_add_test( calo_only_data EXTRA_PATTERNS "-s .*ERROR.*|FastCaloL2EgammaAlg.*REGTEST*" )
_add_test( emu_step_processing EXTRA_PATTERNS "-s TrigSignatureMoniMT.*INFO HLT_.*" )
_add_test( emu_newjo EXTRA_PATTERNS "-s TrigSignatureMo.*INFO HLT_.*" )
_add_test( runMenuTest EXTRA_PATTERNS "-s .*ERROR (?\!attempt to add a duplicate).*|.*FATAL.*|TrigSignatureMoniMT .*INFO.*" )
_add_test( peb EXTRA_PATTERNS "-s , robs=\[|adds PEBInfo|TriggerSummary.*HLT_" )
_add_test( dataScouting EXTRA_PATTERNS "-s , robs=\[|adds PEBInfo|ROBFragments with IDs|{module.*words}|TriggerSummary.*HLT_" )
_add_test( bjet_menu EXTRA_PATTERNS "-s TrigSignatureMoniMT.*HLT_.*" )
_add_test( bjet_menuALLTE EXTRA_PATTERNS "-s TrigSignatureMoniMT.*HLT_.*" )
_add_test( met_fromCells EXTRA_PATTERNS "-s METHypoAlg.*MET.*value" )
_add_test( met_fromClusters EXTRA_PATTERNS "-s METHypoAlg.*MET.*value" )
_add_test( met_fromJets EXTRA_PATTERNS "-s METHypoAlg.*MET.*value" )
_add_test( met_menu EXTRA_PATTERNS "-s TrigSignatureMoniMT.*HLT_.*" )

# ART-based tests:
_add_test( full_menu_build POST_EXEC_SCRIPT "trig-art-result-parser.sh fullMenu.log \"athena CheckLog RegTest\"" )
_add_test( newJO_build POST_EXEC_SCRIPT "trig-art-result-parser.sh NewJO.log \"Configuration athena CheckLog RegTest\"" )
