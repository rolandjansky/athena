# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkRIO_OnTrack )

# Component(s) in the package:
atlas_add_library( TrkRIO_OnTrack
                   src/*.cxx
                   PUBLIC_HEADERS TrkRIO_OnTrack
                   LINK_LIBRARIES AthenaKernel GeoPrimitives Identifier EventPrimitives TrkEventPrimitives TrkMeasurementBase
                   PRIVATE_LINK_LIBRARIES GaudiKernel TrkPrepRawData )

