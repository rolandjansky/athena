# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkVertexOnTrack )

# Component(s) in the package:
atlas_add_library( TrkVertexOnTrack
                   src/*.cxx
                   PUBLIC_HEADERS TrkVertexOnTrack
                   LINK_LIBRARIES GeoPrimitives EventPrimitives GaudiKernel TrkSurfaces TrkMeasurementBase TrkParameters
                   PRIVATE_LINK_LIBRARIES VxVertex )

