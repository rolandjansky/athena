################################################################################
# Package: TrkAlignInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrkAlignInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModel
                          GaudiKernel
                          Tracking/TrkAlignment/TrkAlignEvent
                          Tracking/TrkEvent/TrkTrack )

# Install files from the package:
atlas_install_headers( TrkAlignInterfaces )

