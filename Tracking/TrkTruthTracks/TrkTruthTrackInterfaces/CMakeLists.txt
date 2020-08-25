################################################################################
# Package: TrkTruthTrackInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrkTruthTrackInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Tracking/TrkEvent/TrkSegment )

atlas_add_library( TrkTruthTrackInterfaces
                   TrkTruthTrackInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrkTruthTrackInterfaces
                   LINK_LIBRARIES GaudiKernel AtlasHepMCLib TrkSegment )

