# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigNavTools )

# Component(s) in the package:
atlas_add_library( TrigNavToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigNavTools
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps AthenaKernel TrigConfHLTData TrigSteeringEvent xAODTrigger
                   LINK_LIBRARIES GaudiKernel TrigDecisionToolLib TrigNavigationLib )

atlas_add_component( TrigNavTools
                     src/components/*.cxx
                     LINK_LIBRARIES TrigNavToolsLib )

# Install files from the package:
atlas_install_python_modules( python/__init__.py python/TrigNavToolsConfig.py )
atlas_install_joboptions( share/*.py )

