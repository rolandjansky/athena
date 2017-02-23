################################################################################
# Package: MuonSimValUtils
################################################################################

# Declare the package name:
atlas_subdir( MuonSimValUtils )

# Component(s) in the package:
atlas_add_library( MuonSimValUtils
                   src/*.cxx
                   PUBLIC_HEADERS MuonSimValUtils )

