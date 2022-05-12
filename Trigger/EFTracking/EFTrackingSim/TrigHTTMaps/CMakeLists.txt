################################################################################
# Package: TrigHTTMaps
################################################################################

# Declare the package name:
atlas_subdir( TrigHTTMaps )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( Boost )
find_package( lwtnn )

# Component(s) in the package:
atlas_add_library(
    TrigHTTMapsLib      src/*.cxx TrigHTTMaps/*.h
    PUBLIC_HEADERS      TrigHTTMaps
    INCLUDE_DIRS        ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${LWTNN_INCLUDE_DIRS}
    LINK_LIBRARIES      ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${LWTNN_LIBRARIES}
                        AthContainers AthenaBaseComps AthenaKernel TrigHTTConfToolsLib TrigHTTObjectsLib PathResolver
)

atlas_add_component(
    TrigHTTMaps         src/components/*.cxx
    INCLUDE_DIRS        ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
    LINK_LIBRARIES      TrigHTTMapsLib TrigHTTConfToolsLib
)

# Install files from the package:
atlas_install_python_modules( python/*.py )

atlas_add_test(         HTTPlaneMap
    SOURCES             test/HTTPlaneMap_test.cxx
    LINK_LIBRARIES      TrigHTTMapsLib TestTools
    PROPERTIES
        TIMEOUT         120
)

atlas_add_test(         HTTRegionMap
    SOURCES             test/HTTRegionMap_test.cxx
    LINK_LIBRARIES      TrigHTTMapsLib TestTools
    PROPERTIES
        TIMEOUT         120
)


