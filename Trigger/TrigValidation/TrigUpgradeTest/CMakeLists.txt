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
atlas_install_scripts( test/exec*.sh test/test*.sh test/test*.py )

# Check python syntax:
atlas_add_test( flake8
   SCRIPT flake8 --select=F,E7,E9,W6 --ignore=E701 ${CMAKE_CURRENT_SOURCE_DIR}/python
   POST_EXEC_SCRIPT nopost.sh )

# Unit tests (they test things from outside TrigUpgradeTest - should be moved or removed):
atlas_add_test( ViewSchedule1  SCRIPT test/test_view_schedule.sh ENVIRONMENT THREADS=1 POST_EXEC_SCRIPT nopost.sh )
atlas_add_test( ViewSchedule2  SCRIPT test/test_view_schedule.sh ENVIRONMENT THREADS=2 POST_EXEC_SCRIPT nopost.sh )
atlas_add_test( ViewSchedule64 SCRIPT test/test_view_schedule.sh ENVIRONMENT THREADS=64 POST_EXEC_SCRIPT nopost.sh )
atlas_add_test( merge SCRIPT test/test_merge.sh PROPERTIES TIMEOUT 1000 POST_EXEC_SCRIPT nopost.sh )


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


_add_test( emu_l1_decoding POST_EXEC_SCRIPT nopost.sh ) # should be moved to the L1Decoder package
_add_test( emu_scaling_CF  EXTRA_PATTERNS "-s TrigSignatureMoniMT.*INFO HLT_.*|TriggerSummaryStep.* chains passed:|TriggerSummaryStep.*+++ HLT_.*") # should be moved to TriggerMenuMT
_add_test( emu_step_processing EXTRA_PATTERNS "-s TrigSignatureMoniMT.*INFO HLT_.*|TriggerSummaryStep.* chains passed:|TriggerSummaryStep.*+++ HLT_.*") # should be moved to TriggerMenuMT
