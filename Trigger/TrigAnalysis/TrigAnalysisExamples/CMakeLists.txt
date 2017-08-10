################################################################################
# Package: TrigAnalysisExamples
################################################################################

# Declare the package name:
atlas_subdir( TrigAnalysisExamples )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODTrigger
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODTau
   Event/xAOD/xAODTrigEgamma
   Event/xAOD/xAODTrigCalo
   Event/xAOD/xAODJet
   Trigger/TrigConfiguration/TrigConfHLTData
   Control/AthenaBaseComps
   Control/AthenaKernel
   Control/StoreGate
   Control/AthAnalysisBaseComps
   Control/AthToolSupport/AsgTools
   PhysicsAnalysis/POOLRootAccess
   Event/EventInfo
   GaudiKernel
   Trigger/TrigAnalysis/TrigAnalysisInterfaces
   Trigger/TrigAnalysis/TrigDecisionTool
   Trigger/TrigAnalysis/TriggerMatchingTool
   Trigger/TrigEvent/TrigSteeringEvent
   Trigger/TrigT1/TrigT1Interfaces
   Event/FourMomUtils )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree Hist )

# Component(s) in the package:
atlas_add_component( TrigAnalysisExamples
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} AthenaBaseComps
   AthenaKernel StoreGateLib AthAnalysisBaseCompsLib AsgTools EventInfo
   GaudiKernel xAODEventInfo xAODTrigger xAODEgamma xAODTau xAODTrigEgamma
   xAODTrigCalo xAODJet TrigConfHLTData TrigDecisionToolLib TriggerMatchingToolLib
   TrigSteeringEvent TrigT1Interfaces FourMomUtils POOLRootAccess )

atlas_add_executable( TrigAnalysisExApp
   src/apps/TrigAnalysisExApp.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthAnalysisBaseCompsLib POOLRootAccess
   GaudiKernel TrigDecisionToolLib )
                     
# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
