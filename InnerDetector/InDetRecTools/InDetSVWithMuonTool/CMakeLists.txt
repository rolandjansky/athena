################################################################################
# Package: InDetSVWithMuonTool
################################################################################

# Declare the package name:
atlas_subdir( InDetSVWithMuonTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
    PUBLIC
    Control/AthenaBaseComps
    GaudiKernel
    Tracking/TrkVertexFitter/TrkVKalVrtFitter
    Event/xAOD/xAODTracking

    PRIVATE
    PhysicsAnalysis/AnalysisCommon/AnalysisUtils
    Event/xAOD/xAODBTagging
    )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_library(
    InDetSVWithMuonToolLib
    InDetSVWithMuonTool/*.h
    src/*.cxx

    PUBLIC_HEADERS
    InDetSVWithMuonTool

    INCLUDE_DIRS
    ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}

    LINK_LIBRARIES
    ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${EIGEN_LIBRARIES}
    AthenaBaseComps GaudiKernel TrkVKalVrtFitter xAODTracking

    PRIVATE_LINK_LIBRARIES
    AnalysisUtils xAODBTagging
    )


# Component(s) in the package:
atlas_add_component(
    InDetSVWithMuonTool
    src/components/*.cxx

    LINK_LIBRARIES
    GaudiKernel InDetSVWithMuonToolLib
    )

# Install files from the package:
atlas_install_python_modules( python/*.py )
