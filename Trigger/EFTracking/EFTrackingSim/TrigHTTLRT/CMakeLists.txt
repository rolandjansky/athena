################################################################################
# Package: TrigHTTLRT
################################################################################

# Declare the package name:
atlas_subdir( TrigHTTLRT )


# External dependencies:
find_package( Boost )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( lwtnn )

# Component(s) in the package:
atlas_add_library(
    TrigHTTLRTLib        src/*.cxx TrigHTTLRT/*.h
    PUBLIC_HEADERS              TrigHTTLRT
    INCLUDE_DIRS                ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${LWTNN_INCLUDE_DIRS}
    LINK_LIBRARIES              ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${EIGEN_LIBRARIES} ${LWTNN_LIBRARIES}
                                AthenaBaseComps GaudiKernel TrigHTTBanksLib TrigHTTConfToolsLib TrigHTTInputLib TrigHTTMapsLib TrigHTTObjectsLib TrigHTTHoughLib
    PRIVATE_LINK_LIBRARIES      ${ROOT_LIBRARIES}
)

atlas_add_component(
    TrigHTTLRT           src/components/*.cxx
    INCLUDE_DIRS                ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
    LINK_LIBRARIES              TrigHTTLRTLib
)

# Install files from the package:
atlas_install_python_modules( python/*.py )

# Unit tests
