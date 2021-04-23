# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigCostAnalysis )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist RIO )

# Athena algorithm to do cost analysis and produce histograms
atlas_add_component( TrigCostAnalysis
                     src/*.cxx src/monitors/*.cxx src/counters/*.cxx src/components/TrigCostAnalysis_entries.cxx
                     PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthAnalysisBaseCompsLib TrigDataAccessMonitoringLib TrigDecisionToolLib EnhancedBiasWeighterLib xAODEventInfo PathResolver TrigConfData TrigCompositeUtilsLib )


atlas_install_scripts( share/RunTrigCostAnalysis.py share/CostAnalysisPostProcessing.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_python_modules( python/*.py python/TableConstructors/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
