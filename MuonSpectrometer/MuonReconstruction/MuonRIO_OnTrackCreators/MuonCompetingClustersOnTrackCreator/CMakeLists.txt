################################################################################
# Package: MuonCompetingClustersOnTrackCreator
################################################################################

# Declare the package name:
atlas_subdir( MuonCompetingClustersOnTrackCreator )

# Component(s) in the package:
atlas_add_component( MuonCompetingClustersOnTrackCreator
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonReadoutGeometry MuonIdHelpersLib MuonCompetingRIOsOnTrack MuonRIO_OnTrack MuonRecToolInterfaces TrkSurfaces TrkEventPrimitives TrkPrepRawData )

