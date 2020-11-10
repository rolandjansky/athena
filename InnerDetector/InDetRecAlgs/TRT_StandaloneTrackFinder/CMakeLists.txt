# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_StandaloneTrackFinder )

# Component(s) in the package:
atlas_add_component( TRT_StandaloneTrackFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel StoreGateLib TrkSegment TrkTrack InDetRecToolInterfaces StoreGateLib TrkEventPrimitives TrkParameters TrkEventUtils )
