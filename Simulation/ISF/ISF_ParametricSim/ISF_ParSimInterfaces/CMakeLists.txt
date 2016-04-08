################################################################################
# Package: ISF_ParSimInterfaces
################################################################################

# Declare the package name:
atlas_subdir( ISF_ParSimInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Tracking/TrkEvent/TrkParameters )

# Install files from the package:
atlas_install_headers( ISF_ParSimInterfaces )

