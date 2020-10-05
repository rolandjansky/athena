# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigInDetToolInterfaces )

# Component(s) in the package:
atlas_add_library( TrigInDetToolInterfacesLib
                   TrigInDetToolInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrigInDetToolInterfaces
                   LINK_LIBRARIES LINK_LIBRARIES ByteStreamData GaudiKernel IRegionSelector Identifier InDetPrepRawData TrigInDetEvent TrigInDetPattRecoEvent TrkEventPrimitives TrkTrack )
