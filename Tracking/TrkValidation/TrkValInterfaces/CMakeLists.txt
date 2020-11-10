# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkValInterfaces )

# Component(s) in the package:
atlas_add_library( TrkValInterfaces
   TrkValInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS TrkValInterfaces
   LINK_LIBRARIES AtlasHepMCLib GeoPrimitives GaudiKernel TrkEventPrimitives TrkParameters )
