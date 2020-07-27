# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigUpgradeTest )

#atlas_add_component( TrigUpgradeTest
#                     src/*.cxx
#                     src/components/*.cxx
#                     LINK_LIBRARIES AthenaBaseComps TrigSteeringEvent DecisionHandlingLib TrigCompositeUtilsLib
#                     )

#foreach(test emu_step_processing emu_step_menu_processing)
#    set( rundir ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_${test} )
#    file( REMOVE_RECURSE ${rundir} )
#    file( MAKE_DIRECTORY ${rundir} )
#    atlas_add_test( ${test}
#		SCRIPT test/test_${test}.sh
#		LOG_SELECT_PATTERN "TrigSignatureMoniMT.*INFO HLT_.*|TrigSignatureMoniMT.*-- #[0-9]+ (Events|Features).*|TriggerSummaryStep.* chains passed:|TriggerSummaryStep.*+++ HLT_.*|TriggerSummaryStep.*+++ leg.*"
#    PROPERTIES WORKING_DIRECTORY ${rundir}
#		)
#endforeach()


# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_data( share/*.ref share/*.conf )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( test/exec*.sh test/test*.sh )
