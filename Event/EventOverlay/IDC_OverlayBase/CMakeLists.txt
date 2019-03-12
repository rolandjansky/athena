################################################################################
# Package: IDC_OverlayBase
################################################################################

# Declare the package name:
atlas_subdir( IDC_OverlayBase )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/Identifier
                          Event/EventOverlay/OverlayAlgBase )

# Component(s) in the package:
atlas_add_library( IDC_OverlayBase
                   PUBLIC_HEADERS IDC_OverlayBase
                   LINK_LIBRARIES Identifier OverlayAlgBase )
