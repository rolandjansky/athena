################################################################################
# Package: MuonAlignErrorBase
################################################################################

# Declare the package name:
atlas_subdir( MuonAlignErrorBase )

# Component(s) in the package:
atlas_add_library( MuonAlignErrorBase
                   src/*.cxx
                   PUBLIC_HEADERS MuonAlignErrorBase
                   LINK_LIBRARIES GeoPrimitives )

