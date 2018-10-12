################################################################################
# Package: L1TopoRDO
################################################################################

# Declare the package name:
atlas_subdir( L1TopoRDO )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          Control/AthContainers)

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_library( L1TopoRDO
                   src/*.cxx
                   PUBLIC_HEADERS L1TopoRDO
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES TestTools AthContainers AthenaKernel
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} )

atlas_add_test( L1TopoRDO_test
                SOURCES
                test/L1TopoRDO_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} TestTools AthContainers AthenaKernel L1TopoRDO )

