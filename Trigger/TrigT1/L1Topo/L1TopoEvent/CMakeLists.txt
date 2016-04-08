################################################################################
# Package: L1TopoEvent
################################################################################

# Declare the package name:
atlas_subdir( L1TopoEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigConfiguration/TrigConfBase
                          Trigger/TrigT1/L1Topo/L1TopoCommon )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_library( L1TopoEvent
                   Root/*.cxx
                   PUBLIC_HEADERS L1TopoEvent
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfBase L1TopoCommon )

