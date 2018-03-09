################################################################################
# Package: JetTagMonitoring
################################################################################

# Declare the package name:
atlas_subdir( JetTagMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaMonitoring
                          Event/xAOD/xAODJet
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          Reconstruction/RecoTools/ITrackToVertex
                          PRIVATE
                          Event/xAOD/xAODBTagging
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          LArCalorimeter/LArRecEvent
                          PhysicsAnalysis/JetTagging/JetTagTools
                          Tools/LWHists
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/VxSecVertex
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          Trigger/TrigAnalysis/TrigDecisionTool )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( JetTagMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib xAODJet ITrackToVertex xAODBTagging xAODEventInfo xAODTracking GaudiKernel LArRecEvent JetTagToolsLib LWHists TrkParticleBase VxSecVertex TrkVertexFitterInterfaces TrigDecisionToolLib )

# Install files from the package:
atlas_install_headers( JetTagMonitoring )
atlas_install_joboptions( share/*.py )

