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
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( RatesAnalysis
                   src/*.cxx
                   PUBLIC_HEADERS RatesAnalysis
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthAnalysisBaseCompsLib TrigDecisionToolLib EnhancedBiasWeighterLib
                   PRIVATE_LINK_LIBRARIES EventInfo xAODEventInfo )

atlas_add_test( RatesAnalysis_test
                SOURCES test/RatesAnalysis_test.cxx
                LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthAnalysisBaseCompsLib RatesAnalysis TrigDecisionToolLib EnhancedBiasWeighterLib EventInfo xAODEventInfo )
