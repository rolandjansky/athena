# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RatesAnalysis )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Package library:
atlas_add_library( RatesAnalysisLib
                   src/Rates*.cxx
                   PUBLIC_HEADERS RatesAnalysis
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthAnalysisBaseCompsLib AthenaBaseComps TrigDecisionToolLib EnhancedBiasWeighterLib
                   PRIVATE_LINK_LIBRARIES EventInfo TrigConfData TrigConfL1Data xAODEgamma xAODEventInfo )

# Component(s) in the package:
atlas_add_component( RatesAnalysis
                     src/FullMenu.cxx
                     src/components/RatesAnalysis_entries.cxx
                     LINK_LIBRARIES RatesAnalysisLib )

atlas_add_test( RatesAnalysis_test
                SOURCES test/RatesAnalysis_test.cxx
                LINK_LIBRARIES RatesAnalysisLib CxxUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( share/RatesAnalysisFullMenu.py share/RatesEmulationExample.py share/RatesAnalysisPostProcessing.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
