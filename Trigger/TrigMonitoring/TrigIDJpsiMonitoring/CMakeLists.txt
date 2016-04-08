################################################################################
# Package: TrigIDJpsiMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TrigIDJpsiMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/EventInfo
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          Reconstruction/Particle
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigAnalysis/TrigInDetAnalysis
                          Trigger/TrigAnalysis/TrigInDetAnalysisUtils
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigMonitoring/TrigHLTMonitoring
                          Trigger/TrigTools/TrigJPsiTools
                          PRIVATE
                          Control/AthenaKernel
                          Control/AthenaMonitoring
                          GaudiKernel
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/egamma/egammaEvent
                          Tracking/TrkEvent/VxVertex
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigTruthEvent/TrigInDetTruthEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_component( TrigIDJpsiMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} EventInfo Particle TrigDecisionToolLib TrigInDetAnalysis TrigInDetAnalysisUtils TrigMuonEvent TrigParticle TrigSteeringEvent TrigHLTMonitoringLib TrigJPsiTools AthenaKernel AthenaMonitoringLib GaudiKernel muonEvent egammaEvent VxVertex TrigInDetEvent TrigInDetTruthEvent )

# Install files from the package:
atlas_install_headers( TrigIDJpsiMonitoring )
atlas_install_python_modules( python/*.py )

