################################################################################
# Package: TrigHTTObjects
################################################################################

# Declare the package name:
atlas_subdir( TrigHTTObjects )

# External dependencies:

find_package( ROOT )
find_package( Boost COMPONENTS iostreams program_options filesystem thread system )
find_package( Eigen )

# Component(s) in the package:
atlas_add_root_dictionary(
    TrigHTTObjectsLib       TrigHTTObjectsLibDictSource
    ROOT_HEADERS            TrigHTTObjects/*.h
    EXTERNAL_PACKAGES       Boost Eigen ROOT
)

# Component(s) in the package:
atlas_add_library(
    TrigHTTObjectsLib       src/*.cxx TrigHTTObjects/*.h ${TrigHTTObjectsLibDictSource}
    PUBLIC_HEADERS          TrigHTTObjects
    INCLUDE_DIRS            ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
    LINK_LIBRARIES          ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${EIGEN_LIBRARIES} AthenaKernel GaudiKernel AsgTools
)

atlas_add_test(             TrigHTTRoad_test
    SOURCES                 test/HTTRoad_test.cxx
    LINK_LIBRARIES          TrigHTTObjectsLib
)

atlas_add_test(             TrigHTTTrack_test
    SOURCES                 test/HTTTrack_test.cxx
    LINK_LIBRARIES          TrigHTTObjectsLib
)

atlas_add_test(             TrigHTTLogicalEventInputHeader_test
    SOURCES                 test/HTTLogicalEventInputHeader_test.cxx
    LINK_LIBRARIES          TrigHTTObjectsLib
)

atlas_add_test(             TrigHTTLogicalEventOutputHeader_test
    SOURCES                 test/HTTLogicalEventOutputHeader_test.cxx
    LINK_LIBRARIES          TrigHTTObjectsLib
)