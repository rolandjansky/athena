# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkVolumes )

# Component(s) in the package:
atlas_add_library( TrkVolumes
                   src/*.cxx
                   PUBLIC_HEADERS TrkVolumes
		           LINK_LIBRARIES AthenaKernel GeoPrimitives GaudiKernel TrkDetDescrUtils TrkGeometrySurfaces TrkSurfaces TrkEventPrimitives TrkParameters CxxUtils)
