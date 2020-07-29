# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigValAlgs )

# Component(s) in the package:
atlas_add_component( TrigValAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AnalysisTriggerEvent AthAnalysisBaseCompsLib AthContainers AthViews AthenaBaseComps AthenaKernel CxxUtils EventInfo GaudiKernel MuonCombinedToolInterfaces Particle StoreGateLib TrigCaloEvent TrigCompositeUtilsLib TrigConfHLTData TrigConfInterfaces TrigDecisionToolLib TrigInDetEvent TrigInDetTruthEvent TrigMissingEtEvent TrigMuonEvent TrigNavStructure TrigNavigationLib TrigParticle TrigRoiConversionLib TrigSteeringEvent TrigT1Interfaces VxSecVertex tauEvent xAODBTagging xAODCore xAODEgamma xAODJet xAODMuon xAODTau xAODTracking xAODTrigBphys xAODTrigCalo xAODTrigEgamma xAODTrigMinBias xAODTrigMissingET xAODTrigMuon xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
