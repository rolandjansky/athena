################################################################################
# Package: TrigTauHypo
################################################################################

# Declare the package name:
atlas_subdir( TrigTauHypo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/StoreGate
                          Control/AthenaBaseComps
                          GaudiKernel
                          Reconstruction/MuonIdentification/MuidEvent
                          Reconstruction/Particle
                          Reconstruction/RecoTools/ITrackToVertex
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/tauEvent
                          Tracking/TrkEvent/VxVertex
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigEvent/TrigTopoEvent
                          Trigger/TrigSteer/TrigInterfaces
			                 Event/xAOD/xAODTrigger
                          PRIVATE
                          Calorimeter/CaloEvent
                          Control/AthViews
                          Control/AthContainers
                          Event/EventInfo
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODTracking
                          Tools/PathResolver
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigNavStructure 
                          Trigger/TrigSteer/DecisionHandling )

# External dependencies:
find_package( AIDA )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigTauHypoLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigTauHypo
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel MuidEvent Particle ITrackToVertex egammaEvent tauEvent xAODTrigger  VxVertex TrigInDetEvent TrigMuonEvent TrigParticle TrigSteeringEvent TrigTopoEvent StoreGateLib SGtests TrigInterfacesLib AthViews
                   PRIVATE_LINK_LIBRARIES CaloEvent AthContainers AthenaBaseComps EventInfo xAODCaloEvent xAODJet xAODTau xAODEgamma xAODTracking PathResolver TrkTrack TrkTrackSummary TrigCaloEvent TrigNavStructure DecisionHandlingLib )

atlas_add_component( TrigTauHypo
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} StoreGateLib SGtests GaudiKernel MuidEvent Particle ITrackToVertex egammaEvent tauEvent VxVertex TrigInDetEvent TrigMuonEvent TrigParticle TrigSteeringEvent TrigTopoEvent TrigInterfacesLib CaloEvent AthContainers AthenaBaseComps EventInfo xAODCaloEvent xAODJet xAODTau xAODEgamma xAODTracking PathResolver TrkTrack TrkTrackSummary TrigCaloEvent TrigNavStructure TrigTauHypoLib DecisionHandlingLib AthViews )

# Install files from the package:
atlas_install_python_modules( python/*.py )

