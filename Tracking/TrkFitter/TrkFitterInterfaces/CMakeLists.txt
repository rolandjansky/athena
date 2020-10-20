# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkFitterInterfaces )

# Component(s) in the package:
atlas_add_library( TrkFitterInterfaces
                   PUBLIC_HEADERS TrkFitterInterfaces
                   LINK_LIBRARIES AthContainers EventPrimitives GaudiKernel TrkEventPrimitives TrkParameters TrkTrack TrkFitterUtils )

