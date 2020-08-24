# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigParticle )

# Component(s) in the package:
atlas_add_library( TrigParticle
                   src/*.cxx
                   PUBLIC_HEADERS TrigParticle
                   LINK_LIBRARIES AthContainers AthLinks AthenaKernel EventKernel FourMom Navigation Particle TrigCaloEvent TrigInDetEvent TrigNavigationLib TrigSerializeCnvSvcLib VxVertex egammaEvent tauEvent
                   PRIVATE_LINK_LIBRARIES CaloGeoHelpers JetEvent TrigNavigationLib xAODBTagging xAODEgamma xAODJet xAODTau xAODTrigEgamma )

atlas_add_sercnv_library( TrigParticleSerCnv
                          FILES TrigParticle/TrigElectron.h TrigParticle/TrigElectronContainer.h TrigParticle/TrigPhoton.h TrigParticle/TrigPhotonContainer.h TrigParticle/TrigTau.h TrigParticle/TrigTauContainer.h TrigParticle/TrigL2Bjet.h TrigParticle/TrigL2BjetContainer.h TrigParticle/TrigEFBjet.h TrigParticle/TrigEFBjetContainer.h TrigParticle/TrigL2Bphys.h TrigParticle/TrigL2BphysContainer.h TrigParticle/TrigEFBphys.h TrigParticle/TrigEFBphysContainer.h egammaEvent/egammaContainer.h egammaEvent/egDetailContainer.h tauEvent/TauJetContainer.h tauEvent/TauDetailsContainer.h JetEvent/JetCollection.h JetEvent/JetMomentMap.h JetEvent/JetMomentMapCollection.h JetEvent/JetKeyDescriptor.h JetEvent/JetKeyDescriptorCollection.h
                          TYPES_WITH_NAMESPACE Analysis::TauJetContainer Analysis::TauDetailsContainer
                          LINK_LIBRARIES TrigParticle )

atlas_add_sercnv_library( TrigParticlexAODSerCnv
                          FILES xAODTau/TauJetContainer.h xAODTau/TauJetAuxContainer.h xAODTau/TauTrackContainer.h xAODTau/TauTrackAuxContainer.h xAODJet/JetContainer.h xAODJet/JetTrigAuxContainer.h xAODTrigEgamma/TrigPhotonContainer.h xAODTrigEgamma/TrigPhotonAuxContainer.h xAODTrigEgamma/TrigElectronContainer.h xAODTrigEgamma/TrigElectronAuxContainer.h xAODTrigEgamma/ElectronTrigAuxContainer.h xAODTrigEgamma/PhotonTrigAuxContainer.h xAODEgamma/ElectronContainer.h xAODEgamma/ElectronAuxContainer.h xAODEgamma/PhotonContainer.h xAODEgamma/PhotonAuxContainer.h xAODBTagging/BTaggingContainer.h xAODBTagging/BTaggingTrigAuxContainer.h xAODBTagging/BTagVertexContainer.h xAODBTagging/BTagVertexAuxContainer.h
                          TYPES_WITH_NAMESPACE xAOD::TauJetContainer xAOD::TauJetAuxContainer xAOD::TauTrackContainer xAOD::TauTrackAuxContainer xAOD::JetContainer xAOD::JetTrigAuxContainer xAOD::TrigPhotonContainer xAOD::TrigPhotonAuxContainer xAOD::TrigElectronContainer xAOD::TrigElectronAuxContainer xAOD::ElectronContainer xAOD::ElectronAuxContainer xAOD::PhotonContainer xAOD::PhotonAuxContainer xAOD::ElectronTrigAuxContainer xAOD::PhotonTrigAuxContainer xAOD::BTaggingContainer xAOD::BTaggingTrigAuxContainer xAOD::BTagVertexContainer xAOD::BTagVertexAuxContainer
                          CNV_PFX xAOD
                          LINK_LIBRARIES TrigParticle )

atlas_add_dictionary( TrigParticleDict
                      TrigParticle/TrigParticleDict.h
                      TrigParticle/selection.xml
                      LINK_LIBRARIES TrigParticle
                      DATA_LINKS TrigTau TrigPhotonContainer TrigEFBjetContainer TrigL2BjetContainer
                      ELEMENT_LINKS TrigL2BphysContainer TrigEFBphysContainer TrigElectronContainer )

