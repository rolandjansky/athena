# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkDistortedSurfaces )

# Component(s) in the package:
atlas_add_library( TrkDistortedSurfaces
                   src/*.cxx
                   PUBLIC_HEADERS TrkDistortedSurfaces
                   LINK_LIBRARIES GeoPrimitives GaudiKernel TrkSurfaces
                   PRIVATE_LINK_LIBRARIES TrkEventPrimitives )

atlas_add_dictionary( TrkDistortedSurfacesDict
                      TrkDistortedSurfaces/TrkDistortedSurfacesDict.h
                      TrkDistortedSurfaces/selection.xml
                      LINK_LIBRARIES TrkDistortedSurfaces )
