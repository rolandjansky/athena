################################################################################
# Package: MuonCompetingRIOsOnTrack
################################################################################

# Declare the package name:
atlas_subdir( MuonCompetingRIOsOnTrack )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonCompetingRIOsOnTrack
                   src/*.cxx
                   PUBLIC_HEADERS MuonCompetingRIOsOnTrack
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CxxUtils Identifier MuonRIO_OnTrack TrkSurfaces TrkCompetingRIOsOnTrack TrkRIO_OnTrack
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_dictionary( MuonCompetingRIOsOnTrackDict
                      MuonCompetingRIOsOnTrack/MuonCompetingRIOsOnTrackDict.h
                      MuonCompetingRIOsOnTrack/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} CxxUtils Identifier MuonRIO_OnTrack TrkSurfaces TrkCompetingRIOsOnTrack TrkRIO_OnTrack GaudiKernel MuonCompetingRIOsOnTrack )

