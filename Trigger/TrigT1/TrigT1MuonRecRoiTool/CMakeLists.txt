# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1MuonRecRoiTool )

# Component(s) in the package:
atlas_add_library( TrigT1MuonRecRoiToolLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1MuonRecRoiTool
                   LINK_LIBRARIES AthenaKernel GaudiKernel MuonIdHelpersLib MuonReadoutGeometry TGCcablingInterfaceLib RPCcablingInterfaceLib RPC_CondCablingLib )

atlas_add_component( TrigT1MuonRecRoiTool
                     src/components/*.cxx
                     LINK_LIBRARIES TrigT1MuonRecRoiToolLib )

