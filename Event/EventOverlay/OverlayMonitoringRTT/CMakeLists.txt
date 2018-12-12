################################################################################
# Package: OverlayMonitoringRTT
################################################################################

# Declare the package name:
atlas_subdir( OverlayMonitoringRTT )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_headers( OverlayMonitoringRTT )
