# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkDigEvent )

# Component(s) in the package:
atlas_add_library( TrkDigEvent
                  src/*.cxx
                  PUBLIC_HEADERS TrkDigEvent
                  LINK_LIBRARIES GeoPrimitives TrkDetDescrUtils TrkSurfaces )

