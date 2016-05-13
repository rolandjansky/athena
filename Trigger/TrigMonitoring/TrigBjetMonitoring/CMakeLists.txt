################################################################################
# Package: TrigBjetMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TrigBjetMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigMonitoring/TrigHLTMonitoring
                          Trigger/TrigTools/TrigTrackJetFinderTool
                          PRIVATE
                          Control/AthenaMonitoring
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Event/FourMomUtils
                          Event/xAOD/xAODBTagging
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          Reconstruction/Jet/JetEvent
                          Reconstruction/Jet/JetUtils
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_library( TrigBjetMonitoringLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigBjetMonitoring
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel TrigDecisionToolLib TrigHLTMonitoringLib AthenaMonitoringLib
                   PRIVATE_LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives EventPrimitives FourMomUtils xAODBTagging xAODEventInfo xAODTracking JetEvent JetUtils muonEvent Particle TrigInDetEvent TrigMuonEvent TrigParticle TrigSteeringEvent )

atlas_add_component( TrigBjetMonitoring
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} GaudiKernel TrigDecisionToolLib TrigHLTMonitoringLib AthenaMonitoringLib GeoPrimitives EventPrimitives FourMomUtils xAODBTagging xAODEventInfo xAODTracking JetEvent JetUtils muonEvent Particle TrigInDetEvent TrigMuonEvent TrigParticle TrigSteeringEvent TrigBjetMonitoringLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

