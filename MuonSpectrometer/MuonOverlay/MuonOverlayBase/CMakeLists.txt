################################################################################
# Package: MuonOverlayBase
################################################################################

# Declare the package name:
atlas_subdir( MuonOverlayBase )

# Component(s) in the package:
atlas_add_library( MuonOverlayBase
                   PUBLIC_HEADERS MuonOverlayBase
                   LINK_LIBRARIES IDC_OverlayBase )
