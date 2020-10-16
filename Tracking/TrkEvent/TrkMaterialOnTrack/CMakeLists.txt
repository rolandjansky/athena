# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkMaterialOnTrack )

# Component(s) in the package:
atlas_add_library( TrkMaterialOnTrack
                   src/*.cxx
                   PUBLIC_HEADERS TrkMaterialOnTrack
                   LINK_LIBRARIES TrkEventPrimitives
                   PRIVATE_LINK_LIBRARIES GaudiKernel TrkSurfaces )

atlas_add_dictionary( TrkMaterialOnTrackDict
                      TrkMaterialOnTrack/TrkMaterialOnTrackDict.h
                      TrkMaterialOnTrack/selection.xml
                      LINK_LIBRARIES TrkMaterialOnTrack )
