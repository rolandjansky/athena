# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1TGCRecRoiSvc )

# Component(s) in the package:
atlas_add_library( TrigT1TGCRecRoiSvcLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1TGCRecRoiSvc
                   LINK_LIBRARIES GaudiKernel MuonIdHelpersLib MuonReadoutGeometry TGCcablingInterfaceLib TrigT1Interfaces
                   PRIVATE_LINK_LIBRARIES Identifier StoreGateLib )

atlas_add_component( TrigT1TGCRecRoiSvc
                     src/components/*.cxx
                     LINK_LIBRARIES TrigT1TGCRecRoiSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.txt share/*.py )

