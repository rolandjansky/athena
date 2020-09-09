################################################################################
# Package: MuonSegmentCleaner
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentCleaner )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( MuonSegmentCleanerLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonSegmentCleaner
                   PRIVATE_INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonRecToolInterfaces MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES ${EIGEN_LIBRARIES} CxxUtils EventPrimitives MuonReadoutGeometry MuonCompetingRIOsOnTrack MuonPrepRawData MuonRIO_OnTrack MuonSegment TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkPrepRawData TrkRIO_OnTrack )

atlas_add_component( MuonSegmentCleaner
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GaudiKernel MuonRecToolInterfaces CxxUtils EventPrimitives MuonReadoutGeometry MuonIdHelpersLib MuonCompetingRIOsOnTrack MuonPrepRawData MuonRIO_OnTrack MuonSegment TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkPrepRawData TrkRIO_OnTrack MuonSegmentCleanerLib )

