# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkEventUtils )

# Component(s) in the package:
atlas_add_library( TrkEventUtils
                   src/*.cxx
                   PUBLIC_HEADERS TrkEventUtils
                   LINK_LIBRARIES AthenaKernel GeoPrimitives EventPrimitives Identifier TrkSurfaces TrkEventPrimitives TrkMeasurementBase TrkParameters TrkPrepRawData TrkRIO_OnTrack TrkTrack
                   PRIVATE_LINK_LIBRARIES AtlasDetDescr TrkCompetingRIOsOnTrack TrkPseudoMeasurementOnTrack TrkSegment TrkSpacePoint TrkVertexOnTrack )

