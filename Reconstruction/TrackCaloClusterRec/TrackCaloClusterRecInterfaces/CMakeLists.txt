################################################################################
# Package: TrackCaloClusterRecInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrackCaloClusterRecInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Event/xAOD/xAODBase
                          Tracking/TrkEvent/TrkCaloExtension
                          Tracking/TrkEvent/TrkEventPrimitives )

# Install files from the package:
atlas_install_headers( TrackCaloClusterRecInterfaces )