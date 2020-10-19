# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkGeometrySurfaces )

# Component(s) in the package:
atlas_add_library( TrkGeometrySurfaces
                   src/*.cxx
                   PUBLIC_HEADERS TrkGeometrySurfaces
                   LINK_LIBRARIES GeoPrimitives EventPrimitives TrkDetDescrUtils TrkSurfaces TrkEventPrimitives TrkParametersBase
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

