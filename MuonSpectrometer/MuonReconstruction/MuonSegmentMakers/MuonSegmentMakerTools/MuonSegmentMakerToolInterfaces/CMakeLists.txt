################################################################################
# Package: MuonSegmentMakerToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentMakerToolInterfaces )

# External dependencies:
find_package( Eigen )

atlas_add_library( MuonSegmentMakerToolInterfaces
                   MuonSegmentMakerToolInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonSegmentMakerToolInterfaces
                   LINK_LIBRARIES  GaudiKernel MuonPrepRawData TrkTruthData MuonRIO_OnTrack TrkSegment MuonPattern MuonSegment MuonLayerEvent MuonRecToolInterfaces Identifier MuonEDM_AssociationObjects GeoPrimitives EventPrimitives )

