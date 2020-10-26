# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetCompetingRIOsOnTrack )

# Component(s) in the package:
atlas_add_library( InDetCompetingRIOsOnTrack
                   src/*.cxx
                   PUBLIC_HEADERS InDetCompetingRIOsOnTrack
                   LINK_LIBRARIES CxxUtils InDetRIO_OnTrack TrkCompetingRIOsOnTrack
                   PRIVATE_LINK_LIBRARIES GaudiKernel TrkSurfaces )
