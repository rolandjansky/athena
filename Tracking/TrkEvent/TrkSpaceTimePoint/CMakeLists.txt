# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkSpaceTimePoint )

# Component(s) in the package:
atlas_add_library( TrkSpaceTimePoint
                   src/*.cxx
                   PUBLIC_HEADERS TrkSpaceTimePoint
                   LINK_LIBRARIES GeoPrimitives EventPrimitives TrkEventPrimitives
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

