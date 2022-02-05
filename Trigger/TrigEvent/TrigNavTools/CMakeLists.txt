# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigNavTools )

# Component(s) in the package:
atlas_add_library( TrigNavToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigNavTools
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps AthenaKernel CxxUtils TrigConfHLTData TrigConfHLTUtilsLib TrigSteeringEvent xAODTrigger
                   LINK_LIBRARIES GaudiKernel TrigDecisionToolLib TrigNavigationLib AnalysisTriggerEvent AthAnalysisBaseCompsLib AthContainers AthViews AthenaBaseComps AthenaKernel CxxUtils EventInfo GaudiKernel MuonCombinedToolInterfaces Particle StoreGateLib TrigCaloEvent TrigCompositeUtilsLib TrigConfHLTData TrigConfInterfaces TrigDecisionToolLib TrigInDetEvent TrigInDetTruthEvent TrigMissingEtEvent TrigMuonEvent TrigNavStructure TrigNavigationLib TrigParticle TrigRoiConversionLib TrigSteeringEvent TrigT1Interfaces VxSecVertex tauEvent xAODBTagging xAODCore xAODEgamma xAODJet xAODMuon xAODTau xAODTracking xAODTrigBphys xAODTrigCalo xAODTrigEgamma xAODTrigMinBias xAODTrigMissingET xAODTrigMuon xAODTrigger xAODEventInfo )

atlas_add_component( TrigNavTools
                     src/components/*.cxx
                     LINK_LIBRARIES TrigNavToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/*.py )
