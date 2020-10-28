# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetCosmicsFilters )

# Component(s) in the package:
atlas_add_component( InDetCosmicsFilters
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AtlasDetDescr Identifier TrkRIO_OnTrack TrkTrack )
