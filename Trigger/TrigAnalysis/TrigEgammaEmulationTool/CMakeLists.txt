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
   InnerDetector/InDetRecTools/InDetTrackSelectionTool
   Reconstruction/RecoTools/RecoToolInterfaces
   Tools/PathResolver
   Trigger/TrigAnalysis/TrigDecisionTool
   Trigger/TrigConfiguration/TrigConfHLTData
   Trigger/TrigEvent/TrigSteeringEvent
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
   InDetTrackSelectionToolLib RecoToolInterfaces
   TrigDecisionToolLib TrigConfHLTData TrigSteeringEvent 
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} )

atlas_add_component( TrigEgammaEmulationTool
    src/components/*.cxx
    LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES}
    AthContainers AsgTools xAODBase xAODCaloEvent xAODEgamma  xAODTracking
    xAODTrigCalo xAODTrigEgamma xAODTrigRinger xAODTrigger PATCoreLib
    ElectronPhotonSelectorToolsLib TrigDecisionToolLib TrigConfHLTData
    TrigSteeringEvent 
    AthenaBaseComps StoreGateLib SGtests
    GaudiKernel 
    IsolationToolLib
    TrigEgammaEmulationToolLib) 

