################################################################################
# Package: TriggerMatchingTool
################################################################################

# Declare the package name:
atlas_subdir( TriggerMatchingTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODBase
                          GaudiKernel
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigEvent/TrigNavStructure
                          PRIVATE
                          Control/AthAnalysisBaseComps
                          Event/FourMomUtils )


# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TriggerMatchingToolLib
                   Root/*.cxx
                   src/*.cxx
                   PUBLIC_HEADERS TriggerMatchingTool
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgTools xAODBase GaudiKernel TrigNavStructure TrigDecisionToolLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthAnalysisBaseCompsLib FourMomUtils )

atlas_add_component( TriggerMatchingTool
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AsgTools xAODBase GaudiKernel TrigDecisionToolLib TrigNavStructure AthAnalysisBaseCompsLib FourMomUtils TriggerMatchingToolLib )

atlas_add_dictionary( TriggerMatchingToolDict
                      TriggerMatchingTool/TriggerMatchingToolDict.h
                      TriggerMatchingTool/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AsgTools xAODBase GaudiKernel TrigDecisionToolLib TrigNavStructure AthAnalysisBaseCompsLib FourMomUtils TriggerMatchingToolLib )

