# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DecisionHandling )

atlas_add_library( DecisionHandlingLib
                   src/ComboHypo.cxx
                   src/ComboHypoToolBase.cxx
                   src/DebugComboHypoTool.cxx
                   src/DumpDecisions.cxx
                   src/HypoBase.cxx
                   src/InputMakerBase.cxx
		           src/ITestHypoTool.cxx
                   PUBLIC_HEADERS DecisionHandling
                   LINK_LIBRARIES AthenaBaseComps AthenaMonitoringKernelLib GaudiKernel StoreGateLib TrigCompositeUtilsLib TrigCostMonitorLib TrigSteeringEvent TrigTimeAlgsLib
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
                     LINK_LIBRARIES DecisionHandlingLib AthenaKernel xAODTrigCalo AthViews xAODTracking xAODJet )

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
#atlas_install_joboptions( share/*.py )
atlas_install_data( share/*.ref)

# Tests

atlas_add_test( test_ComboHypoTool
    SOURCES test/test_ComboHypoTool.cxx
    LINK_LIBRARIES TestTools TrigCompositeUtilsLib DecisionHandlingLib )
