################################################################################
# Package: TrigHIHypo
################################################################################

# Declare the package name:
atlas_subdir( TrigHIHypo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloInterface
                          Control/AthenaBaseComps
                          Event/xAOD/xAODHIEvent
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODCaloEvent
                          GaudiKernel
                          Reconstruction/HeavyIonRec/HIGlobal
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkParameters
                          Trigger/TrigEvent/TrigMissingEtEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrigHIHypo
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} TrigCaloEvent TrigInDetEvent TrigSteeringEvent CaloEvent AthenaBaseComps xAODHIEvent xAODMuon xAODTracking xAODCaloEvent GaudiKernel Particle TrkParameters TrigMissingEtEvent TrigMuonEvent TrigParticle TrigInterfacesLib TrigT1Interfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py )

