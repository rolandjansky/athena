# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1MuonRecRoiTool )

atlas_add_component( TrigT1MuonRecRoiTool
                     src/*.cxx 
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaKernel GaudiKernel TrigT1Interfaces MuonIdHelpersLib MuonReadoutGeometry TGCcablingInterfaceLib RPCcablingInterfaceLib RPC_CondCablingLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
