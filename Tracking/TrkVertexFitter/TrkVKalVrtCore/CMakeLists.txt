################################################################################
# Package: TrkVKalVrtCore
################################################################################

# Declare the package name:
atlas_subdir( TrkVKalVrtCore )

atlas_depends_on_subdirs( PUBLIC Control/CxxUtils )

# Component(s) in the package:
atlas_add_library( TrkVKalVrtCore
                   src/*.cxx
                   PUBLIC_HEADERS TrkVKalVrtCore )

