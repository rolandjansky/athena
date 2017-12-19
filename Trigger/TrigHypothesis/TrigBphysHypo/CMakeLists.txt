################################################################################
# Package: TrigBphysHypo
################################################################################

# Declare the package name:
atlas_subdir( TrigBphysHypo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/AthLinks
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          Event/EventInfo
                          Event/FourMomUtils
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigBphys
                          Event/xAOD/xAODTrigMuon
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          Generators/GeneratorObjects
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkVertexFitter/TrkVKalVrtFitter
                          Trigger/TrigEvent/TrigBphysicsEvent
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigNavigation
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigTools/TrigInDetToolInterfaces
                          Trigger/TrigTools/TrigTimeAlgs )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigBphysHypo
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel AthLinks StoreGateLib SGtests GeoPrimitives EventInfo xAODEventInfo xAODMuon xAODTracking xAODTrigBphys xAODTrigMuon GaudiKernel GeneratorObjects Particle TrkParameters TrkTrack VxVertex TrkVKalVrtFitterLib TrigBphysicsEvent TrigCaloEvent TrigInDetEvent TrigMuonEvent TrigNavigationLib TrigParticle TrigSteeringEvent TrigInterfacesLib TrigT1Interfaces TrigTimeAlgsLib )

# Install files from the package:
atlas_install_headers( TrigBphysHypo )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/TriggerConfig*.py share/jobO*.py )
atlas_install_runtime( share/*.xml )

