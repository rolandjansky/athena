################################################################################
# Package: MuonClusterOnTrackCreator
################################################################################

# Declare the package name:
atlas_subdir( MuonClusterOnTrackCreator )

# Component(s) in the package:
atlas_add_component( MuonClusterOnTrackCreator
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps EventPrimitives GaudiKernel MuonReadoutGeometry MuonIdHelpersLib CscClusterizationLib MuonPrepRawData MuonRIO_OnTrack MuonRecToolInterfaces TrkSurfaces TrkEventPrimitives TrkParameters TrkPrepRawData TrkRIO_OnTrack )

# Install files from the package:
atlas_install_joboptions( share/*.py )

