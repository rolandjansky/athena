atlas_subdir( ZdcNtuple )

atlas_depends_on_subdirs (
  PhysicsAnalysis/D3PDTools/AnaAlgorithm
  PhysicsAnalysis/Interfaces/AsgAnalysisInterfaces
  Control/AthToolSupport/AsgTools
  PhysicsAnalysis/Interfaces/EgammaAnalysisInterfaces
  PhysicsAnalysis/AnalysisCommon/IsolationSelection
  Reconstruction/Jet/JetInterface
  PhysicsAnalysis/Interfaces/MuonAnalysisInterfaces
  PhysicsAnalysis/AnalysisCommon/PATInterfaces
  Trigger/TrigConfiguration/TrigConfInterfaces
  Trigger/TrigAnalysis/TrigDecisionTool
  Trigger/TrigAnalysis/TriggerMatchingTool
  ForwardDetectors/ZDC/ZdcAnalysis
  Event/xAOD/xAODCaloEvent
  Event/xAOD/xAODEgamma
  Event/xAOD/xAODEventInfo
  Event/xAOD/xAODForward
  Event/xAOD/xAODHIEvent
  Event/xAOD/xAODJet
  Event/xAOD/xAODMuon
  Event/xAOD/xAODTracking
  Event/xAOD/xAODTrigL1Calo
  Event/xAOD/xAODTrigMinBias
  Event/xAOD/xAODTrigger
  Event/xAOD/xAODTruth
  PRIVATE
  Event/xAOD/xAODCore
  Control/xAODRootAccess
  )


atlas_add_library (ZdcNtupleLib
                   ZdcNtuple/*.h Root/*.cxx
		   PUBLIC_HEADERS ZdcNtuple
		   LINK_LIBRARIES AnaAlgorithmLib AsgAnalysisInterfaces
                   AsgTools EgammaAnalysisInterfacesLib IsolationSelectionLib
                   JetInterface MuonAnalysisInterfacesLib PATInterfaces
                   TrigConfInterfaces TrigDecisionToolLib TriggerMatchingToolLib
                   ZdcAnalysisLib xAODCaloEvent xAODEgamma xAODEventInfo
                   xAODForward xAODHIEvent xAODJet xAODMuon xAODTracking
                   xAODTrigL1Calo xAODTrigMinBias xAODTrigger xAODTruth
                   PRIVATE_LINK_LIBRARIES xAODCore xAODRootAccess)

atlas_add_dictionary (ZdcNtupleDict
			ZdcNtuple/ZdcNtupleDict.h
			ZdcNtuple/selection.xml
			LINK_LIBRARIES ZdcNtupleLib)
