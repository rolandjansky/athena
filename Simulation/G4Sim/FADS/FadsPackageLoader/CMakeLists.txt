################################################################################
# Package: FadsPackageLoader
################################################################################

# Declare the package name:
atlas_subdir( FadsPackageLoader )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Simulation/G4Sim/FADS/FadsUtilities )

# Component(s) in the package:
atlas_add_library( FadsPackageLoader
                   src/*.cxx
                   PUBLIC_HEADERS FadsPackageLoader
                   PRIVATE_LINK_LIBRARIES FadsUtilities )

