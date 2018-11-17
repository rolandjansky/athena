atlas_subdir( ZdcNtuple )

atlas_add_library (ZdcNtupleLib
		  ZdcNtuple/*.h Root/*.cxx
		  PUBLIC_HEADERS ZdcNtuple
		  LINK_LIBRARIES xAODRootAccess xAODEventInfo xAODTruth xAODJet JetInterface xAODMuon MuonAnalysisInterfacesLib xAODEgamma EgammaAnalysisInterfacesLib TrigDecisionToolLib TrigConfInterfaces xAODForward xAODHIEvent xAODTracking xAODParticleEvent xAODCaloEvent IsolationSelectionLib PATInterfaces xAODTrigMinBias xAODTrigL1Calo AsgTools PathResolver AnaAlgorithmLib AsgAnalysisInterfaces ZdcAnalysisLib TriggerMatchingToolLib)

atlas_add_dictionary (ZdcNtupleDict
			ZdcNtuple/ZdcNtupleDict.h
			ZdcNtuple/selection.xml
			LINK_LIBRARIES ZdcNtupleLib)


if (NOT XAOD_STANDALONE)

#   atlas_add_component( ZdcNtuple
#      		     src/components/*.cxx
#     	             INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
#     		     LINK_LIBRARIES ${ROOT_LIBRARIES} EventLoop EventLoopGrid EventLoopAlgs xAODRootAccess GoodRunsLists xAODEventInfo TrigDecisionToolLib TrigConfInterfaces TrigConfxAOD xAODForward xAODHIEvent xAODTracking xAODJet JetInterface InDetTrackSelectionTool xAODMuon MuonAnalysisInterfacesLib HIEventUtils xAODParticleEvent xAODTruth xAODCaloEvent MuonMomentumCorrections PATInterfaces ZdcAnalysis xAODTrigMinBias MuonEfficiencyCorrections xAODEgamma EgammaAnalysisInterfaces xAODTrigL1Calo xAODBTaggingEfficiency TrigMuonMatching TrigEgammaMatchingTool IsolationSelection AsgTools PathResolver AnaAlgorithmLib AsgAnalysisInterfaces)

endif()

atlas_install_joboptions( share/*.py )

