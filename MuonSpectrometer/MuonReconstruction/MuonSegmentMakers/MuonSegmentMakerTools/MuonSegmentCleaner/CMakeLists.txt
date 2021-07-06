################################################################################
# Package: MuonSegmentCleaner
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentCleaner )

# Component(s) in the package:
atlas_add_library( MuonSegmentCleanerLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonSegmentCleaner
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonRecToolInterfaces MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES CxxUtils EventPrimitives MuonReadoutGeometry MuonCompetingRIOsOnTrack MuonPrepRawData MuonRIO_OnTrack MuonSegment TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkPrepRawData TrkRIO_OnTrack )

atlas_add_component( MuonSegmentCleaner
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonRecToolInterfaces CxxUtils EventPrimitives MuonReadoutGeometry MuonIdHelpersLib MuonCompetingRIOsOnTrack MuonPrepRawData MuonRIO_OnTrack MuonSegment TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkPrepRawData TrkRIO_OnTrack MuonSegmentCleanerLib )

