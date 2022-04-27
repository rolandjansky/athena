# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigInDetToolInterfaces )

# Component(s) in the package:
atlas_add_library( TrigInDetToolInterfacesLib
                   TrigInDetToolInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrigInDetToolInterfaces
                   LINK_LIBRARIES GaudiKernel IRegionSelector Identifier TrigInDetEvent TrigInDetPattRecoEvent TrkTrack )
