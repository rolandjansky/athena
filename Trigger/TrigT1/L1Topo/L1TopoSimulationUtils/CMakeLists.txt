################################################################################
# Package: L1TopoSimulationUtils
################################################################################

# Declare the package name:
atlas_subdir( L1TopoSimulationUtils )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_library( L1TopoSimulationUtils
                   Root/*.cxx
                   PUBLIC_HEADERS L1TopoSimulationUtils
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} )

