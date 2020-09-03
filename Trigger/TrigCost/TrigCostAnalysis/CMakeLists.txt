# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigCostAnalysis )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist RIO )

# Athena algorithm to do cost analysis and produce histograms
atlas_add_component( TrigCostAnalysis
                     src/*.cxx src/monitors/*.cxx src/counters/*.cxx src/components/TrigCostAnalysis_entries.cxx
                     PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthAnalysisBaseCompsLib TrigDecisionToolLib EnhancedBiasWeighterLib xAODEventInfo PathResolver TrigConfData )

# Small helper library used by trigCostHistToCSV
atlas_add_library( TrigCostAnalysisLib
                   TrigCostAnalysis/*.h Root/*.cxx TrigCostAnalysis/TableConstructors/*.h Root/TableConstructors/*.cxx
                   PUBLIC_HEADERS TrigCostAnalysis
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools AthenaBaseComps )

# Lightweight post processing of cost histograms to produce CSV output
atlas_add_executable( trigCostHistToCSV
                      util/trigCostHistToCSV.cxx
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools AthenaBaseComps TrigCostAnalysisLib )

atlas_install_joboptions( share/TrigCostAnalysis_JobOptions.py )
atlas_install_scripts( share/TrigCostAnalysis.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
