################################################################################
# Package: MuonOverlayBase
################################################################################

# Declare the package name:
atlas_subdir( MuonOverlayBase )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/EventOverlay/IDC_OverlayBase )

# Component(s) in the package:
atlas_add_library( MuonOverlayBase
                   PUBLIC_HEADERS MuonOverlayBase
                   LINK_LIBRARIES IDC_OverlayBase )
