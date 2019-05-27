################################################################################
# Package: TrigCostAnalysis
################################################################################

# Declare the package name:
atlas_subdir( TrigCostAnalysis )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthAnalysisBaseComps
                          GaudiKernel
                          PRIVATE
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigCost/EnhancedBiasWeighter
                          Event/xAOD/xAODEventInfo )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( Boost )

# Athena algorithm to do cost analysis and produce histograms
atlas_add_component( TrigCostAnalysis
                     src/*.cxx src/monitors/*.cxx src/counters/*.cxx  src/components/TrigCostAnalysis_entries.cxx
                     PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthAnalysisBaseCompsLib TrigDecisionToolLib EnhancedBiasWeighterLib xAODEventInfo )

# Small helper library used by trigCostHistToCSV
atlas_add_library( TrigCostAnalysisLib
                   TrigCostAnalysis/*.h Root/*.cxx TrigCostAnalysis/TableConstructors/*.h Root/TableConstructors/*.cxx
                   PUBLIC_HEADERS TrigCostAnalysis
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel)

# Lightweight post processing of cost histograms to produce CSV output
atlas_add_executable( trigCostHistToCSV
                      util/trigCostHistToCSV.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel TrigCostAnalysisLib )

atlas_install_joboptions( share/*.py )
