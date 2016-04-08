################################################################################
# Package: L1TopoCommon
################################################################################

# Declare the package name:
atlas_subdir( L1TopoCommon )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_library( L1TopoCommon
                   Root/*.cxx
                   PUBLIC_HEADERS L1TopoCommon
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} )

