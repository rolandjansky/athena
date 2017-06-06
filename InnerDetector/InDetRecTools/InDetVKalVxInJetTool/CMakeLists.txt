################################################################################
# Package: InDetVKalVxInJetTool
################################################################################

# Declare the package name:
atlas_subdir( InDetVKalVxInJetTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Reconstruction/Particle
                          Tracking/TrkVertexFitter/TrkVKalVrtFitter
                          PRIVATE
                          DetectorDescription/GeoPrimitives
                          PhysicsAnalysis/AnalysisCommon/AnalysisUtils
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/VxSecVertex
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces )
#InnerDetector/InDetRecTools/InDetMaterialRejTool

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( InDetVKalVxInJetTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps xAODTracking GaudiKernel InDetRecToolInterfaces Particle TrkVKalVrtFitterLib GeoPrimitives AnalysisUtilsLib TrkNeutralParameters TrkParticleBase TrkTrackSummary VxSecVertex VxVertex TrkToolInterfaces TrkVertexFitterInterfaces )

# Install files from the package:
atlas_install_headers( InDetVKalVxInJetTool )
atlas_install_python_modules( python/*.py )

