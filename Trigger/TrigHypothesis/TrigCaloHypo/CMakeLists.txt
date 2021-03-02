# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigCaloHypo )

# Component(s) in the package:
atlas_add_library( TrigCaloHypoLib
                   TrigCaloHypo/*.h
                   INTERFACE
                   PUBLIC_HEADERS TrigCaloHypo
                   LINK_LIBRARIES CaloEvent DecisionHandlingLib GaudiKernel Identifier TrigSteeringEvent )

atlas_add_component( TrigCaloHypo
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaMonitoringKernelLib CaloEvent CaloInterfaceLib DecisionHandlingLib GaudiKernel Identifier LArRecConditions LArRecEvent StoreGateLib TrigCaloHypoLib TrigCompositeUtilsLib TrigSteeringEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )
