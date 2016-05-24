################################################################################
# Package: TrkVKalVrtCore
################################################################################

# Declare the package name:
atlas_subdir( TrkVKalVrtCore )

# Component(s) in the package:
atlas_add_library( TrkVKalVrtCore
                   src/*.cxx
                   PUBLIC_HEADERS TrkVKalVrtCore )

