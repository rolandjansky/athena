# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DecisionHandling )

atlas_add_library( DecisionHandlingLib
                   src/ComboHypo.cxx
                   src/ComboHypoToolBase.cxx
                   src/DumpDecisions.cxx
                   src/HypoBase.cxx
                   src/InputMakerBase.cxx
		           src/ITestHypoTool.cxx
                   PUBLIC_HEADERS DecisionHandling
                   LINK_LIBRARIES AthenaBaseComps AthenaMonitoringKernelLib GaudiKernel StoreGateLib TrigCompositeUtilsLib TrigCostMonitorMTLib TrigSteeringEvent TrigTimeAlgsLib
                   PRIVATE_LINK_LIBRARIES AthContainers AthViews xAODTrigger )

# Component(s) in the package:
atlas_add_component( DecisionHandling
                     src/components/*.cxx
                     src/DeltaRRoIComboHypoTool.cxx
                     src/InputMakerForRoI.cxx
                     src/TriggerSummaryAlg.cxx
                     src/RoRSeqFilter.cxx
                     src/ViewCreator*.cxx
                     src/TestRecoAlg.cxx
                     src/TestInputMaker.cxx
                     src/TestHypoTool.cxx
                     src/TestHypoAlg.cxx
                     src/PassFilter.cxx
                     LINK_LIBRARIES DecisionHandlingLib xAODTrigCalo AthViews xAODTracking xAODJet )

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_data( share/*.ref)





foreach(test emu_step_processing emu_step_menu_processing)
    set( rundir ${CMAKE_CURRENT_BINARY_DIR}/unitTestRun_${test} )
    file( REMOVE_RECURSE ${rundir} )
    file( MAKE_DIRECTORY ${rundir} )
    atlas_add_test( ${test}
        SCRIPT test/test_${test}.sh
        LOG_SELECT_PATTERN "TrigSignatureMoniMT.*INFO HLT_.*|TrigSignatureMoniMT.*-- #[0-9]+ (Events|Features).*|TriggerSummaryStep.* chains passed:|TriggerSummaryStep.*+++ HLT_.*|TriggerSummaryStep.*+++ leg.*"
    PROPERTIES WORKING_DIRECTORY ${rundir}
        )
endforeach()
