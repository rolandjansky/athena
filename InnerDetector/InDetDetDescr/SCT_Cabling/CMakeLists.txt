# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SCT_Cabling )

# Component(s) in the package:
atlas_add_library( SCT_CablingLib
                   src/*.cxx
                   PUBLIC_HEADERS SCT_Cabling
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel AthenaPoolUtilities GaudiKernel Identifier
                   PRIVATE_LINK_LIBRARIES InDetIdentifier PathResolver StoreGateLib )

atlas_add_component( SCT_Cabling
                     src/components/*.cxx
                     LINK_LIBRARIES SCT_CablingLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Test(s) in the package:
atlas_add_test( TestSCT_CablingCfg
                SCRIPT python -m SCT_Cabling.TestSCT_CablingCfg
                PROPERTIES TIMEOUT 600
                ENVIRONMENT THREADS=1 )
