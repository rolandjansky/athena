# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1RPCRecRoiSvc )

# Component(s) in the package:
atlas_add_library( TrigT1RPCRecRoiSvcLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1RPCRecRoiSvc
                   LINK_LIBRARIES GaudiKernel MuonIdHelpersLib MuonReadoutGeometry RPC_CondCablingLib StoreGateLib TrigT1Interfaces
                   PRIVATE_LINK_LIBRARIES RPCcablingInterfaceLib )

atlas_add_component( TrigT1RPCRecRoiSvc
                     src/components/*.cxx
                     LINK_LIBRARIES TrigT1RPCRecRoiSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.txt share/*.py )

