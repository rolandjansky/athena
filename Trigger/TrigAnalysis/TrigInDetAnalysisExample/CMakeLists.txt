################################################################################
# Package: TrigInDetAnalysisExample
################################################################################


# Declare the package name:
atlas_subdir( TrigInDetAnalysisExample )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/EventInfo
			  Event/xAOD/xAODEventInfo
                          Generators/GeneratorObjects
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/tauEvent
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkTools/TrkParticleCreator
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigAnalysis/TrigInDetAnalysis
                          Trigger/TrigAnalysis/TrigInDetAnalysisUtils
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigMonitoring/TrigHLTMonitoring
                          PRIVATE
                          Control/AthenaMonitoring
                          GaudiKernel
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigSteer/TrigCompositeUtils )

# External dependencies:
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigInDetAnalysisExampleLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigInDetAnalysisExample
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} EventInfo GeneratorObjects McParticleEvent muonEvent egammaEvent tauEvent VxVertex TrigInDetAnalysis TrigInDetAnalysisUtils TrigInDetEvent TrigSteeringEvent TrigDecisionToolLib TrigHLTMonitoringLib AthenaMonitoringLib TrigCompositeUtilsLib 
                   PRIVATE_LINK_LIBRARIES GaudiKernel TrkParameters TrkTrack )

atlas_add_component( TrigInDetAnalysisExample
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} EventInfo GeneratorObjects McParticleEvent muonEvent egammaEvent tauEvent VxVertex TrigDecisionToolLib TrigInDetAnalysis TrigInDetAnalysisUtils TrigInDetEvent TrigSteeringEvent TrigHLTMonitoringLib AthenaMonitoringLib GaudiKernel TrkParameters TrkTrack TrigInDetAnalysisExampleLib )

# Disable naming convention checker.
# FIXME: This should be fixed properly once run2 is finished.
if( ${CMAKE_CXX_FLAGS} MATCHES "libchecker_gccplugins" )
  set_target_properties( TrigInDetAnalysisExampleLib PROPERTIES COMPILE_FLAGS -fplugin-arg-libchecker_gccplugins-checkers=no-naming )
  set_target_properties( TrigInDetAnalysisExample PROPERTIES COMPILE_FLAGS -fplugin-arg-libchecker_gccplugins-checkers=no-naming )
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py )

