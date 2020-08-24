# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1CaloMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist )

# Component(s) in the package:
atlas_add_component( TrigT1CaloMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AnalysisTriggerEvent AthContainers AthenaMonitoringKernelLib AthenaMonitoringLib AthenaPoolUtilities CaloEvent CaloIdentifier EventInfo GaudiKernel Identifier JetInterface LWHists SGTools StoreGateLib TileConditionsLib TileEvent TrigAnalysisInterfaces TrigConfInterfaces TrigConfL1Data TrigDecisionToolLib TrigT1CaloCalibConditions TrigT1CaloCalibToolInterfaces TrigT1CaloCalibToolsLib TrigT1CaloCondSvcLib TrigT1CaloEventLib TrigT1CaloMonitoringToolsLib TrigT1CaloToolInterfaces TrigT1CaloUtilsLib TrigT1Interfaces VxVertex xAODEgamma xAODJet xAODTrigL1Calo )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
