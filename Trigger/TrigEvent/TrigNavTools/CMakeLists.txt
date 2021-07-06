# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigNavTools )

# Component(s) in the package:
atlas_add_library( TrigNavToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigNavTools
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps AthenaKernel CxxUtils TrigConfHLTData TrigSteeringEvent xAODTrigger
                   LINK_LIBRARIES GaudiKernel TrigDecisionToolLib TrigNavigationLib )

atlas_add_component( TrigNavTools
                     src/components/*.cxx
                     LINK_LIBRARIES TrigNavToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/*.py )

atlas_add_test(NavConverterTest
    SCRIPT python -m TrigNavTools.NavConverterConfig
    POST_EXEC_SCRIPT nopost.sh)