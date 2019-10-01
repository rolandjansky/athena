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
                          Control/AthViews
                          Control/CxxUtils
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
                          LumiBlock/LumiBlockComps
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          Generators/GeneratorObjects
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
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
                          Trigger/TrigTools/TrigTimeAlgs
                          Trigger/TrigSteer/DecisionHandling)

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigBphysHypo
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel AthLinks BeamSpotConditionsData DecisionHandlingLib FourMomUtils StoreGateLib SGtests GeoPrimitives EventInfo xAODEventInfo xAODMuon xAODTracking xAODTrigBphys xAODTrigMuon GaudiKernel GeneratorObjects Particle ElectronPhotonSelectorToolsLib TrkParameters TrkTrack VxVertex TrkVKalVrtFitterLib TrigBphysicsEvent TrigCaloEvent TrigInDetEvent TrigMuonEvent TrigNavigationLib TrigParticle TrigSteeringEvent TrigInterfacesLib TrigT1Interfaces TrigTimeAlgsLib AthViews )

# Install files from the package:
atlas_install_python_modules( python/*.py )

# Check Python syntax:
atlas_add_test( flake8
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 --enable-extension=ATL900,ATL901 ${CMAKE_CURRENT_SOURCE_DIR}/python
                POST_EXEC_SCRIPT nopost.sh )
