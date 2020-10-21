# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkCompetingRIOsOnTrack )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( TrkCompetingRIOsOnTrack
                   src/*.cxx
                   PUBLIC_HEADERS TrkCompetingRIOsOnTrack
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES GeoPrimitives EventPrimitives TrkEventPrimitives TrkMeasurementBase
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} TrkRIO_OnTrack )

atlas_add_dictionary( TrkCompetingRIOsOnTrackDict
                      TrkCompetingRIOsOnTrack/TrkCompetingRIOsOnTrackDict.h
                      TrkCompetingRIOsOnTrack/selection.xml
                      LINK_LIBRARIES TrkCompetingRIOsOnTrack )
