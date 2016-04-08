################################################################################
# Package: TriggerD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( TriggerD3PDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CxxUtils
                          Event/EventKernel
                          Event/FourMomUtils
                          GaudiKernel
                          PhysicsAnalysis/D3PDMaker/D3PDMakerInterfaces
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigAnalysis/TrigObjectMatching
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Control/SGTools
                          Control/StoreGate
                          Event/xAOD/xAODTrigger
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          Trigger/TrigAnalysis/TrigAnalysisInterfaces
                          Trigger/TrigAnalysis/TriggerMenuAnalysis
                          Trigger/TrigAnalysis/TriggerMenuNtuple
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigConfiguration/TrigConfigSvc
                          Trigger/TrigEvent/TrigMonitoringEvent
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_library( TriggerD3PDMakerLib
                   src/*.cxx
                   PUBLIC_HEADERS TriggerD3PDMaker
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils EventKernel FourMomUtils GaudiKernel D3PDMakerUtils TrigSteeringEvent TrigDecisionToolLib TrigObjectMatchingLib StoreGateLib SGtests TriggerMenuAnalysisLib TrigConfigSvcLib
                   PRIVATE_LINK_LIBRARIES SGTools xAODTrigger AnalysisTriggerEvent TriggerMenuNtuple TrigConfHLTData TrigConfL1Data TrigMonitoringEvent TrigT1Interfaces TrigT1Result )

atlas_add_component( TriggerD3PDMaker
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils EventKernel FourMomUtils GaudiKernel D3PDMakerUtils TrigDecisionToolLib TrigObjectMatchingLib TrigSteeringEvent SGTools StoreGateLib SGtests xAODTrigger AnalysisTriggerEvent TriggerMenuAnalysisLib TriggerMenuNtuple TrigConfHLTData TrigConfL1Data TrigConfigSvcLib TrigMonitoringEvent TrigT1Interfaces TrigT1Result TriggerD3PDMakerLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

