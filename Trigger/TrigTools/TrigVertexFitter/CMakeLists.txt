# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigVertexFitter )

# Component(s) in the package:
atlas_add_component( TrigVertexFitter
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrigInDetEvent TrigInDetToolInterfacesLib TrigTimeAlgsLib TrkEventPrimitives TrkParameters TrkTrack )
