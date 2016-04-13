################################################################################
# Package: TrigBSExtraction
################################################################################

# Declare the package name:
atlas_subdir( TrigBSExtraction )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Event/xAOD/xAODJetCnv
                          Event/xAOD/xAODTrackingCnv
                          Event/xAOD/xAODTrigBphysCnv
                          Event/xAOD/xAODTrigCaloCnv
                          Event/xAOD/xAODTrigMinBiasCnv
                          Event/xAOD/xAODTrigMissingETCnv
                          Event/xAOD/xAODTrigMuonCnv
                          GaudiKernel
                          Trigger/TrigDataAccess/TrigSerializeCnvSvc
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigBphys
                          Event/xAOD/xAODTrigCalo
                          Event/xAOD/xAODTrigMinBias
                          Event/xAOD/xAODTrigMissingET
                          Event/xAOD/xAODTrigMuon
                          Reconstruction/Jet/JetEvent
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMissingEtEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigStorageDefinitions )

# Component(s) in the package:
atlas_add_component( TrigBSExtraction
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests GaudiKernel TrigSerializeCnvSvcLib TrigNavigationLib TrigSteeringEvent xAODJet xAODMuon xAODTracking xAODTrigBphys xAODTrigCalo xAODTrigMinBias xAODTrigMissingET xAODTrigMuon JetEvent Particle TrkTrack TrigCaloEvent TrigInDetEvent TrigMissingEtEvent TrigMuonEvent TrigParticle TrigStorageDefinitions )

# Install files from the package:
atlas_install_headers( TrigBSExtraction )
atlas_install_joboptions( share/*.py )

