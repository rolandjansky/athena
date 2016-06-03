################################################################################
# Package: FadsPackageLoader
################################################################################

# Declare the package name:
atlas_subdir( FadsPackageLoader )

find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_library( FadsPackageLoader
                   src/*.cxx
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PUBLIC_HEADERS FadsPackageLoader
                   LINK_LIBRARIES dl )

