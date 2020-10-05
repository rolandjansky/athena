################################################################################
# Package: IDC_OverlayBase
################################################################################

# Declare the package name:
atlas_subdir( IDC_OverlayBase )

# Component(s) in the package:
atlas_add_library( IDC_OverlayBase
                   PUBLIC_HEADERS IDC_OverlayBase
                   LINK_LIBRARIES AthenaBaseComps Identifier )
