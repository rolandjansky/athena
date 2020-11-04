# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetCompetingRIOsOnTrackTool )

atlas_add_library( InDetCompetingRIOsOnTrackToolLib
                   InDetCompetingRIOsOnTrackTool/*.h
                   INTERFACE
                   PUBLIC_HEADERS InDetCompetingRIOsOnTrackTool
                   LINK_LIBRARIES GaudiKernel AthenaBaseComps TrkParameters EventPrimitives GeoPrimitives TrkToolInterfaces InDetCompetingRIOsOnTrack )

# Component(s) in the package:
atlas_add_component( InDetCompetingRIOsOnTrackTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES InDetCompetingRIOsOnTrack InDetCompetingRIOsOnTrackToolLib InDetPrepRawData TrkEventPrimitives TrkExInterfaces TrkSurfaces TrkToolInterfaces )
