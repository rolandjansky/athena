# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1RPClogic )

# Component(s) in the package:
atlas_add_library( TrigT1RPClogicLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1RPClogic
                   LINK_LIBRARIES MuonCablingTools RPC_CondCablingLib TrigT1RPChardwareLib )
