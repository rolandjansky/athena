################################################################################
# Package: TrigEgammaEmulationTool
################################################################################

# Declare the package name:
atlas_subdir( TrigEgammaEmulationTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODBase
   Event/xAOD/xAODCaloEvent
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODPrimitives
   Event/xAOD/xAODTracking
   Event/xAOD/xAODTrigCalo
   Event/xAOD/xAODTrigEgamma
   Event/xAOD/xAODTrigRinger
   Event/xAOD/xAODTrigger
   PhysicsAnalysis/AnalysisCommon/PATCore
   PhysicsAnalysis/Interfaces/EgammaAnalysisInterfaces
   LumiBlock/LumiBlockComps
   InnerDetector/InDetRecTools/InDetTrackSelectionTool
   Reconstruction/RecoTools/RecoToolInterfaces
   Trigger/TrigAnalysis/TrigDecisionTool
   Trigger/TrigConfiguration/TrigConfHLTData
   Trigger/TrigEvent/TrigSteeringEvent
   Trigger/TrigHypothesis/TrigMultiVarHypo
   Trigger/TrigAnalysis/TrigEgammaMatchingTool
   PRIVATE
   Control/AthenaBaseComps
   Control/StoreGate
   GaudiKernel )

# External dependencies:
find_package( Boost )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TrigEgammaEmulationToolLib
   TrigEgammaEmulationTool/*.h Root/*.cxx
   PUBLIC_HEADERS TrigEgammaEmulationTool
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthContainers AsgTools xAODBase
   xAODCaloEvent xAODEgamma xAODTracking xAODTrigCalo xAODTrigEgamma
   xAODTrigRinger xAODTrigger PATCoreLib EgammaAnalysisInterfacesLib
   LumiBlockCompsLib InDetTrackSelectionToolLib RecoToolInterfaces
   TrigDecisionToolLib TrigConfHLTData TrigSteeringEvent TrigMultiVarHypoLib
   TrigEgammaMatchingToolLib
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} )

atlas_add_component( TrigEgammaEmulationTool
   src/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps GaudiKernel TrigDecisionToolLib
   TrigEgammaMatchingToolLib xAODTrigger xAODTrigCalo xAODTrigRinger
   xAODTracking xAODCaloEvent xAODEgamma PATCoreLib TrigEgammaEmulationToolLib )

# Install files from the package:
atlas_install_python_modules( python/TrigEgamma*.py )
atlas_install_joboptions( share/test*.py )
