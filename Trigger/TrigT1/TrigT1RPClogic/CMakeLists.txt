# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1RPClogic )

# Component(s) in the package:
atlas_add_library( TrigT1RPClogicLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1RPClogic
                   LINK_LIBRARIES MuonCablingTools GaudiKernel RPC_CondCablingLib RPCcablingInterfaceLib TrigT1RPChardwareLib )
