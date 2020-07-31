################################################################################
# Package: InDetPriVxFinderTool
################################################################################

# Declare the package name:
atlas_subdir( InDetPriVxFinderTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          Control/CxxUtils
                          PRIVATE
                          Control/AthContainers
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          InnerDetector/InDetRecTools/InDetTrackSelectionTool
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkLinks
                          Tracking/TrkEvent/TrkTrackLink
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/VxMultiVertex
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          Tracking/TrkVertexFitter/TrkVertexFitters)

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( InDetPriVxFinderTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps xAODTracking 
                     GaudiKernel InDetRecToolInterfaces PATCoreLib TrkParameters TrkParticleBase TrkTrack CxxUtils AthContainers GeoPrimitives 
                     EventPrimitives TrkSurfaces TrkEventPrimitives TrkLinks TrkTrackSummary VxMultiVertex VxVertex 
                     TrkVertexFitterInterfaces TrkVertexFittersLib BeamSpotConditionsData StoreGateLib
                     InDetTrackSelectionToolLib )

# Install files from the package:
atlas_install_headers( InDetPriVxFinderTool )

