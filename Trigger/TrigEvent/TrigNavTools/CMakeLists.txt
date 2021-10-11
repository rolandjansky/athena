# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigNavTools )

# Component(s) in the package:
atlas_add_library( TrigNavToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigNavTools
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps AthenaKernel CxxUtils TrigConfHLTData TrigSteeringEvent xAODTrigger
                   LINK_LIBRARIES GaudiKernel TrigDecisionToolLib TrigNavigationLib AnalysisTriggerEvent AthAnalysisBaseCompsLib AthContainers AthViews AthenaBaseComps AthenaKernel CxxUtils EventInfo GaudiKernel MuonCombinedToolInterfaces Particle StoreGateLib TrigCaloEvent TrigCompositeUtilsLib TrigConfHLTData TrigConfInterfaces TrigDecisionToolLib TrigInDetEvent TrigInDetTruthEvent TrigMissingEtEvent TrigMuonEvent TrigNavStructure TrigNavigationLib TrigParticle TrigRoiConversionLib TrigSteeringEvent TrigT1Interfaces VxSecVertex tauEvent xAODBTagging xAODCore xAODEgamma xAODJet xAODMuon xAODTau xAODTracking xAODTrigBphys xAODTrigCalo xAODTrigEgamma xAODTrigMinBias xAODTrigMissingET xAODTrigMuon xAODTrigger xAODEventInfo )

atlas_add_component( TrigNavTools
                     src/components/*.cxx
                     LINK_LIBRARIES TrigNavToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/*.py )

atlas_add_test(NavConverterTest
    SCRIPT python -m TrigNavTools.NavConverterConfig
    POST_EXEC_SCRIPT nopost.sh)

# atlas_add_test(NavConverter_muon_chains_test
#     SCRIPT testTrigR2ToR3NavGraphConversion.py --evtMax=10 
#     TestNavConversion.Chains='["HLT_mu4"]' 
#     TestNavConversion.Collections='["xAOD::MuonContainer","xAOD::L2StandAloneMuonContainer","xAOD::TrigMissingET","xAOD::JetContainer"]' 
#     LOG_SELECT_PATTERN "REGTEST")

# atlas_add_test(NavConverter_muon_multichains_test
#     SCRIPT testTrigR2ToR3NavGraphConversion.py --evtMax=10 
#     TestNavConversion.Chains='["HLT_mu4","HLT_mu6","HLT_mu10","HLT_mu6_2mu4","HLT_mu22"]' 
#     TestNavConversion.Collections='["xAOD::MuonContainer","xAOD::L2StandAloneMuonContainer","xAOD::TrigMissingET","xAOD::JetContainer"]' 
#     LOG_SELECT_PATTERN "REGTEST")

    atlas_add_test(NavConverter_electron_multichains_test
    SCRIPT testTrigR2ToR3NavGraphConversion.py --evtMax=10 
    TestNavConversion.Chains='["HLT_e5_lhvloose_nod0","HLT_e9_etcut","HLT_e26_lhtight_nod0","HLT_e28_lhtight_nod0"]' 
    TestNavConversion.Collections='["xAOD::ElectronContainer","xAOD::TrigEMClusterContainer","xAOD::TrigEMCluster","xAOD::TrigElectron","xAOD::TrigElectronContainer","xAOD::CaloCluster","xAOD::CaloClusterContainer"]' 
    LOG_SELECT_PATTERN "REGTEST")