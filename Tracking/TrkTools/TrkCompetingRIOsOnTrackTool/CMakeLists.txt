# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkCompetingRIOsOnTrackTool )

atlas_add_library( TrkCompetingRIOsOnTrackToolLib
                   TrkCompetingRIOsOnTrackTool/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrkCompetingRIOsOnTrackTool
                   LINK_LIBRARIES GaudiKernel AthenaBaseComps TrkToolInterfaces TrkParameters )

# Component(s) in the package:
atlas_add_component( TrkCompetingRIOsOnTrackTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel TrkCompetingRIOsOnTrackToolLib AthenaBaseComps TrkToolInterfaces AtlasDetDescr TrkSurfaces TrkCompetingRIOsOnTrack TrkEventUtils TrkParameters TrkPrepRawData TrkRIO_OnTrack )
