################################################################################
# Package: L1CommonCore
################################################################################

# Declare the package name:
atlas_subdir( L1CommonCore )

# External dependencies:
find_package( tdaq-common COMPONENTS CTPfragment )

# Install files from the package:
atlas_install_headers( L1CommonCore )

