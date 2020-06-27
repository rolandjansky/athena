# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DecisionHandling )

atlas_add_library( DecisionHandlingLib
                   src/*.cxx
                   PUBLIC_HEADERS DecisionHandling
                   LINK_LIBRARIES AthenaBaseComps AthenaMonitoringKernelLib GaudiKernel StoreGateLib TrigCompositeUtilsLib TrigCostMonitorMTLib TrigSteeringEvent TrigTimeAlgsLib
                   PRIVATE_LINK_LIBRARIES AthContainers AthViews xAODTrigger )

# Component(s) in the package:
atlas_add_component( DecisionHandling
                     src/components/*.cxx
                     LINK_LIBRARIES DecisionHandlingLib )

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
