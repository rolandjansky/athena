################################################################################
# Package: L1TopoInterfaces
################################################################################

# Declare the package name:
atlas_subdir( L1TopoInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigConfiguration/TrigConfBase
                          Trigger/TrigT1/L1Topo/L1TopoCommon
                          Trigger/TrigT1/L1Topo/L1TopoEvent )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree pthread RIO MathCore Hist )

# Component(s) in the package:
atlas_add_library( L1TopoInterfaces
                   Root/*.cxx
                   PUBLIC_HEADERS L1TopoInterfaces
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfBase L1TopoCommon L1TopoEvent ${ROOT_LIBRARIES} )

