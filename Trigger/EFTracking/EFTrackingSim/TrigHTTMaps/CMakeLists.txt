# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

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
    INCLUDE_DIRS        ${LWTNN_INCLUDE_DIRS}
    LINK_LIBRARIES      ${LWTNN_LIBRARIES} GaudiKernel TrigHTTConfToolsLib TrigHTTObjectsLib
    PRIVATE_INCLUDE_DIRS   ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
    PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AsgMessagingLib AthenaBaseComps PathResolver
)

atlas_add_component(
    TrigHTTMaps         src/components/*.cxx
    LINK_LIBRARIES      TrigHTTMapsLib TrigHTTConfToolsLib
)

# Install files from the package:
atlas_install_python_modules( python/*.py )

# Tests in the package:
atlas_add_test(         HTTPlaneMap
    SOURCES             test/HTTPlaneMap_test.cxx
    LINK_LIBRARIES      AthenaKernel TrigHTTMapsLib TestTools
    PROPERTIES
        TIMEOUT         120
)

atlas_add_test(         HTTRegionMap
    SOURCES             test/HTTRegionMap_test.cxx
    LINK_LIBRARIES      AthenaKernel TrigHTTMapsLib TestTools
    PROPERTIES
        TIMEOUT         120
)


