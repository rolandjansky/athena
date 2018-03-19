################################################################################
# Package: L1TopoSimulationUtils
################################################################################

# Declare the package name:
atlas_subdir( L1TopoSimulationUtils )

# Trigger dependencies
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigT1/L1Topo/L1TopoEvent )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_library( L1TopoSimulationUtils
                   Root/*.cxx
                   PUBLIC_HEADERS L1TopoSimulationUtils
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} L1TopoEvent )

atlas_add_test( L1TopoSimulationUtils_test
                SOURCES
                test/L1TopoSimulationUtils_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} TestTools DataModel SGTools L1TopoEvent L1TopoSimulationUtils)
