################################################################################
# Package: MuonSegmentTaggerToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentTaggerToolInterfaces )

atlas_add_library( MuonSegmentTaggerToolInterfaces
                   MuonSegmentTaggerToolInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonSegmentTaggerToolInterfaces
                   LINK_LIBRARIES GaudiKernel MuonCombinedEvent TrkEventPrimitives MuonSegmentMakerToolInterfaces TrkParameters )


