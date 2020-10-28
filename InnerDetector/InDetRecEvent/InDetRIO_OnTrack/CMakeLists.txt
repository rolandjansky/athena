# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetRIO_OnTrack )

# Component(s) in the package:
atlas_add_library( InDetRIO_OnTrack
                   src/*.cxx
                   PUBLIC_HEADERS InDetRIO_OnTrack
                   LINK_LIBRARIES AthLinks AthenaKernel CxxUtils Identifier GaudiKernel InDetPrepRawData TrkEventPrimitives TrkRIO_OnTrack
                   PRIVATE_LINK_LIBRARIES InDetIdentifier InDetReadoutGeometry TRT_ReadoutGeometry TrkSurfaces )

atlas_add_dictionary( InDetRIO_OnTrackDict
                      InDetRIO_OnTrack/InDetRIO_OnTrackDict.h
                      InDetRIO_OnTrack/selection.xml
                      LINK_LIBRARIES InDetRIO_OnTrack )
