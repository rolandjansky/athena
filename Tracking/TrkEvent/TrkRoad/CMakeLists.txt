# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkRoad )

# Component(s) in the package:
atlas_add_library( TrkRoad
                   src/*.cxx
                   PUBLIC_HEADERS TrkRoad
                   LINK_LIBRARIES AthContainers AthenaKernel GeoPrimitives EventPrimitives
                   PRIVATE_LINK_LIBRARIES GaudiKernel )
