################################################################################
# Package: DerivationFrameworkTrigger
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkTrigger )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          Trigger/TrigCost/EnhancedBiasWeighter
                          Trigger/TrigAnalysis/TrigAnalysisInterfaces
                          Trigger/TrigDecisionTool/TrigDecisionTool
                          Trigger/TrigEvent/TrigNavStructure
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODTrigger
                          Event/xAOD/xAODCaloEvent
                          Event/FourMomUtils
                          PRIVATE
                          Event/xAOD/xAODEventInfo
                          Tools/PathResolver
                          External/AtlasROOT)

find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_library( 
    DerivationFrameworkTriggerLib
    src/*.cxx
    NO_PUBLIC_HEADERS
    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
    LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaBaseComps xAODBase TrigDecisionToolLib TrigNavStructure xAODCore xAODTrigger xAODCaloEvent FourMomUtils
)

atlas_add_component(
    DerivationFrameworkTrigger
    src/components/*.cxx
    LINK_LIBRARIES DerivationFrameworkTriggerLib
)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

