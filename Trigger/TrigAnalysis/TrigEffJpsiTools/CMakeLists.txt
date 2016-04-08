################################################################################
# Package: TrigEffJpsiTools
################################################################################

# Declare the package name:
atlas_subdir( TrigEffJpsiTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Event/EventInfo
                          Event/EventKernel
                          MuonSpectrometer/MuonCablings/RPCcablingInterface
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigAnalysis/TrigInDetAnalysis
                          Trigger/TrigAnalysis/TrigInDetAnalysisUtils
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigT1/TrigT1RPCRecRoiSvc
                          Trigger/TrigT1/TrigT1Result
                          Trigger/TrigT1/TrigT1TGCRecRoiSvc )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_component( TrigEffJpsiTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} GeoPrimitives GaudiKernel AthenaBaseComps AthenaKernel StoreGateLib SGtests EventInfo EventKernel RPCcablingInterfaceLib MuonReadoutGeometry MuonIdHelpersLib AnalysisTriggerEvent muonEvent Particle TrkSurfaces TrkEventUtils TrkParameters TrkRIO_OnTrack TrkTrack TrkExInterfaces TrigDecisionToolLib TrigInDetAnalysis TrigInDetAnalysisUtils TrigConfL1Data TrigMuonEvent TrigParticle TrigSteeringEvent TrigT1Interfaces TrigT1RPCRecRoiSvcLib TrigT1Result TrigT1TGCRecRoiSvcLib )

# Install files from the package:
atlas_install_headers( TrigEffJpsiTools )
atlas_install_joboptions( share/*.py )

