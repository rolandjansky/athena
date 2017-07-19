################################################################################
# Package: TrigParticle
################################################################################

# Declare the package name:
atlas_subdir( TrigParticle )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/AthContainers
                          Control/AthLinks
                          Control/Navigation
                          Control/SGTools
                          Event/EventKernel
                          Event/FourMom
                          Reconstruction/Particle
                          Tracking/TrkEvent/VxVertex
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          PRIVATE
                          Calorimeter/CaloGeoHelpers
                          Event/xAOD/xAODBTagging
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODTrigEgamma
                          Reconstruction/Jet/JetEvent
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/tauEvent
                          Trigger/TrigDataAccess/TrigSerializeCnvSvc
                          Trigger/TrigEvent/TrigNavigation )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigParticle
                   src/*.cxx
                   PUBLIC_HEADERS TrigParticle
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers AthLinks Navigation SGTools EventKernel FourMom Particle VxVertex TrigCaloEvent TrigInDetEvent TrigSerializeCnvSvcLib TrigNavigationLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CaloGeoHelpers xAODBTagging xAODEgamma xAODJet xAODTau xAODTrigEgamma JetEvent egammaEvent tauEvent )

atlas_add_sercnv_library( TrigParticleSerCnv
                          FILES TrigParticle/TrigElectron.h TrigParticle/TrigElectronContainer.h TrigParticle/TrigPhoton.h TrigParticle/TrigPhotonContainer.h TrigParticle/TrigTau.h TrigParticle/TrigTauContainer.h TrigParticle/TrigL2Bjet.h TrigParticle/TrigL2BjetContainer.h TrigParticle/TrigEFBjet.h TrigParticle/TrigEFBjetContainer.h TrigParticle/TrigL2Bphys.h TrigParticle/TrigL2BphysContainer.h TrigParticle/TrigEFBphys.h TrigParticle/TrigEFBphysContainer.h egammaEvent/egammaContainer.h egammaEvent/egDetailContainer.h tauEvent/TauJetContainer.h tauEvent/TauDetailsContainer.h JetEvent/JetCollection.h JetEvent/JetMomentMap.h JetEvent/JetMomentMapCollection.h JetEvent/JetKeyDescriptor.h JetEvent/JetKeyDescriptorCollection.h
                          TYPES_WITH_NAMESPACE Analysis::TauJetContainer Analysis::TauDetailsContainer
                          INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                          LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks Navigation SGTools EventKernel FourMom Particle VxVertex TrigCaloEvent TrigInDetEvent CaloGeoHelpers xAODBTagging xAODEgamma xAODJet xAODTau xAODTrigEgamma JetEvent egammaEvent tauEvent TrigSerializeCnvSvcLib TrigNavigationLib TrigParticle )

atlas_add_sercnv_library( TrigParticlexAODSerCnv
                          FILES xAODTau/TauJetContainer.h xAODTau/TauJetAuxContainer.h xAODTau/TauTrackContainer.h xAODTau/TauTrackAuxContainer.h xAODJet/JetContainer.h xAODJet/JetTrigAuxContainer.h xAODTrigEgamma/TrigPhotonContainer.h xAODTrigEgamma/TrigPhotonAuxContainer.h xAODTrigEgamma/TrigElectronContainer.h xAODTrigEgamma/TrigElectronAuxContainer.h xAODTrigEgamma/ElectronTrigAuxContainer.h xAODTrigEgamma/PhotonTrigAuxContainer.h xAODEgamma/ElectronContainer.h xAODEgamma/ElectronAuxContainer.h xAODEgamma/PhotonContainer.h xAODEgamma/PhotonAuxContainer.h xAODBTagging/BTaggingContainer.h xAODBTagging/BTaggingTrigAuxContainer.h xAODBTagging/BTagVertexContainer.h xAODBTagging/BTagVertexAuxContainer.h
                          TYPES_WITH_NAMESPACE xAOD::TauJetContainer xAOD::TauJetAuxContainer xAOD::TauTrackContainer xAOD::TauTrackAuxContainer xAOD::JetContainer xAOD::JetTrigAuxContainer xAOD::TrigPhotonContainer xAOD::TrigPhotonAuxContainer xAOD::TrigElectronContainer xAOD::TrigElectronAuxContainer xAOD::ElectronContainer xAOD::ElectronAuxContainer xAOD::PhotonContainer xAOD::PhotonAuxContainer xAOD::ElectronTrigAuxContainer xAOD::PhotonTrigAuxContainer xAOD::BTaggingContainer xAOD::BTaggingTrigAuxContainer xAOD::BTagVertexContainer xAOD::BTagVertexAuxContainer
                          CNV_PFX xAOD
                          INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                          LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks Navigation SGTools EventKernel FourMom Particle VxVertex TrigCaloEvent TrigInDetEvent CaloGeoHelpers xAODBTagging xAODEgamma xAODJet xAODTau xAODTrigEgamma JetEvent egammaEvent tauEvent TrigSerializeCnvSvcLib TrigNavigationLib TrigParticle )

atlas_add_dictionary( TrigParticleDict
                      TrigParticle/TrigParticleDict.h
                      TrigParticle/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks Navigation SGTools EventKernel FourMom Particle VxVertex TrigCaloEvent TrigInDetEvent CaloGeoHelpers xAODBTagging xAODEgamma xAODJet xAODTau xAODTrigEgamma JetEvent egammaEvent tauEvent TrigSerializeCnvSvcLib TrigNavigationLib TrigParticle
                      DATA_LINKS TrigTau TrigPhotonContainer TrigEFBjetContainer TrigL2BjetContainer
                      ELEMENT_LINKS TrigL2BphysContainer TrigEFBphysContainer TrigElectronContainer )

