# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RPC_CondCabling )

# Component(s) in the package:
atlas_add_library( RPC_CondCablingLib
                   src/*.cxx
                   PUBLIC_HEADERS RPC_CondCabling
                   PRIVATE_LINK_LIBRARIES PathResolver
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel AthenaPoolUtilities GaudiKernel MuonCablingTools MuonIdHelpersLib StoreGateLib )

atlas_add_component( RPC_CondCabling
                     src/components/*.cxx
                     LINK_LIBRARIES RPC_CondCablingLib )
