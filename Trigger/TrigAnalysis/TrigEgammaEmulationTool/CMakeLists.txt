################################################################################
# Package: TrigEgammaEmulationTool
################################################################################

# Declare the package name:
atlas_subdir( TrigEgammaEmulationTool )


# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
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
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
                          LumiBlock/LumiBlockComps
                          InnerDetector/InDetRecTools/InDetTrackSelectionTool
			  Reconstruction/RecoTools/IsolationTool
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigHypothesis/TrigMultiVarHypo
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          Trigger/TrigAnalysis/TrigEgammaMatchingTool )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigEgammaEmulationTool
                     src/*.cxx
                     Root/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers AsgTools xAODBase xAODCaloEvent xAODEgamma  xAODTracking xAODTrigCalo xAODTrigEgamma xAODTrigRinger xAODTrigger PATCoreLib ElectronPhotonSelectorToolsLib TrigDecisionToolLib TrigConfHLTData TrigSteeringEvent TrigMultiVarHypoLib AthenaBaseComps StoreGateLib SGtests GaudiKernel TrigEgammaMatchingToolLib LumiBlockCompsLib IsolationToolLib) 


# Install files from the package:
atlas_install_headers( TrigEgammaEmulationTool )
atlas_install_python_modules( python/TrigEgamma*.py )
atlas_install_joboptions( share/test*.py )

