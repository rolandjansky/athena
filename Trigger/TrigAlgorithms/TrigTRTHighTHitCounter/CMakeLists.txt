# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigTRTHighTHitCounter )

# Component(s) in the package:
atlas_add_component( TrigTRTHighTHitCounter
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthViews AthenaBaseComps AthenaMonitoringKernelLib CxxUtils DecisionHandlingLib GaudiKernel Identifier InDetIdentifier InDetPrepRawData StoreGateLib TrigCompositeUtilsLib TrigSteeringEvent xAODTrigRinger xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select=ATL900,ATL901 ) 
