################################################################################
# Package: InDetSVWithMuonTool
################################################################################

# Declare the package name:
atlas_subdir( InDetSVWithMuonTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Reconstruction/Particle
                          Event/xAOD/xAODTracking
                          Tracking/TrkVertexFitter/TrkVKalVrtFitter
                          PRIVATE
                          PhysicsAnalysis/AnalysisCommon/AnalysisUtils
                          Event/xAOD/xAODBTagging )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( InDetSVWithMuonTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps xAODBTagging xAODTracking GaudiKernel InDetRecToolInterfaces Particle TrkVKalVrtFitterLib GeoPrimitives AnalysisUtilsLib TrkNeutralParameters TrkParticleBase TrkTrackSummary VxSecVertex VxVertex TrkToolInterfaces TrkVertexFitterInterfaces )

# Install files from the package:
atlas_install_headers( InDetSVWithMuonTool )
atlas_install_python_modules( python/*.py )

