################################################################################
# Package: RatesAnalysis
################################################################################

# Declare the package name:
atlas_subdir( RatesAnalysis )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthAnalysisBaseComps
                          GaudiKernel
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigCost/EnhancedBiasWeighter
                          PRIVATE
                          Event/xAOD/xAODEventInfo )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Package library:
atlas_add_library( RatesAnalysisLib
                   src/Rates*.cxx
                   PUBLIC_HEADERS RatesAnalysis
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthAnalysisBaseCompsLib TrigDecisionToolLib EnhancedBiasWeighterLib
                   PRIVATE_LINK_LIBRARIES xAODEventInfo )

# Component(s) in the package:
atlas_add_component( RatesAnalysis
                     src/FullMenu.cxx
                     src/components/RatesAnalysis_entries.cxx
                     LINK_LIBRARIES GaudiKernel RatesAnalysisLib )

atlas_add_test( RatesAnalysis_test
                SOURCES test/RatesAnalysis_test.cxx
                LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthAnalysisBaseCompsLib RatesAnalysisLib TrigDecisionToolLib EnhancedBiasWeighterLib xAODEventInfo )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( share/RatesAnalysisFullMenu.py )
