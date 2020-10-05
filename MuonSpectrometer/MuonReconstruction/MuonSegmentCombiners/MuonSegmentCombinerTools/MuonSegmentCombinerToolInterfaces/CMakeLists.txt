################################################################################
# Package: MuonSegmentCombinerToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentCombinerToolInterfaces )

atlas_add_library( MuonSegmentCombinerToolInterfaces
                   MuonSegmentCombinerToolInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonSegmentCombinerToolInterfaces
                   LINK_LIBRARIES GaudiKernel MuonSegment MuonPattern TrkSegment MuonPrepRawData MuonEDM_AssociationObjects  )


