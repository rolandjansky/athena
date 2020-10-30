# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( iPatTrackParameters )

# Component(s) in the package:
atlas_add_library( iPatTrackParameters
                   src/PerigeeParameters.cxx
                   src/ScattererParameters.cxx
                   PUBLIC_HEADERS iPatTrackParameters
                   LINK_LIBRARIES GeoPrimitives EventPrimitives
                   PRIVATE_LINK_LIBRARIES AthenaKernel )

