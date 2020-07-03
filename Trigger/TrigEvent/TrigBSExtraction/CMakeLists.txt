# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigBSExtraction )

# Component(s) in the package:
atlas_add_component( TrigBSExtraction
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps CaloEvent GaudiKernel JetEvent Particle StoreGateLib TrigCaloEvent TrigInDetEvent TrigMissingEtEvent TrigMuonEvent TrigNavigationLib TrigParticle TrigSerializeCnvSvcLib TrigSteeringEvent TrigStorageDefinitions TrkTrack egammaEvent tauEvent xAODBTagging xAODBTaggingCnvLib xAODCaloEvent xAODCaloEventCnvLib xAODEgamma xAODEgammaCnvLib xAODJet xAODJetCnvLib xAODMuon xAODTau xAODTauCnvLib xAODTracking xAODTrackingCnvLib xAODTrigBphys xAODTrigBphysCnvLib xAODTrigCalo xAODTrigCaloCnvLib xAODTrigMinBias xAODTrigMinBiasCnvLib xAODTrigMissingET xAODTrigMissingETCnvLib xAODTrigMuon xAODTrigMuonCnvLib xAODTrigger xAODTriggerCnvLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
