################################################################################
# Package: TrkTruthTrackInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrkTruthTrackInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Tracking/TrkEvent/TrkSegment )

# Install files from the package:
atlas_install_headers( TrkTruthTrackInterfaces )

