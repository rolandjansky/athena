# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkAlignableSurfaces )

# Component(s) in the package:
atlas_add_library( TrkAlignableSurfaces
                   src/*.cxx
                   PUBLIC_HEADERS TrkAlignableSurfaces
                   LINK_LIBRARIES GeoPrimitives Identifier TrkSurfaces )
