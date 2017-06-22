# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addAlgorithm,addTool,addService

addTool("TrigMuonEF.TrigMuonEFConfig.TrigMuonEFStandaloneTrackToolConfig", "TrigMuonEFStandaloneTrackTool")

addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_CaloEnergyMeas", "TMEF_CaloEnergyMeas")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_CaloEnergyTool", "TMEF_CaloEnergyTool")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_CaloTrackStateOnSurface", "TMEF_CaloTrackStateOnSurface")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_MaterialEffectsUpdator", "TMEF_MaterialEffectsUpdator")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_Navigator", "TMEF_Navigator")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_Propagator", "TMEF_Propagator")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_Extrapolator", "TMEF_Extrapolator")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_TrackSummaryTool", "TMEF_TrackSummaryTool")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_CombinedMuonTag", "TMEF_CombinedMuonTag")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_MatchQuality", "TMEF_MatchQuality")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_MaterialAllocator", "TMEF_MaterialAllocator")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_iPatFitter", "TMEF_iPatFitter")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_iPatSLFitter", "TMEF_iPatSLFitter")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_TrackCleaner", "TMEF_TrackCleaner")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_CombinedMuonTrackBuilder", "TMEF_CombinedMuonTrackBuilder")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_MuonTrackQuery", "TMEF_MuonTrackQuery")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_MatchMaker", "TMEF_MatchMaker")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_TrackBuilderTool", "TMEF_TrackBuilderTool")

addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_TrackSummaryToolNoHole", "TMEF_TrackSummaryToolNoHole")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_TrkToTrackParticleConvTool", "TMEF_TrkToTrackParticleConvTool")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_MuonCombinedTool", "TMEF_MuonCombinedTool")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_MuidMuonRecovery","TMEF_MuidMuonRecovery")
addTool("TrigMuonEF.TrigMuonEFConfig.MuonHolesOnTrack","MuonHolesOnTrack")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_CombMuonMEOTPParam","TMEF_CombMuonMEOTPParam")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_MuonCombinedPropagator","TMEF_MuonCombinedPropagator")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_OutwardsMuonTrackCleaner","TMEF_OutwardsMuonTrackCleaner")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_MuonCombinedTrackFitter","TMEF_MuonCombinedTrackFitter")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_OutwardsCombinedMuonTrackBuilder","TMEF_OutwardsCombinedMuonTrackBuilder")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_MuonCombinedFitTagTool","TMEF_MuonCombinedFitTagTool")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_MuonCreatorTool","TMEF_MuonCreatorTool")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_MuonCandidateTool","TMEF_MuonCandidateTool")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_TrkMaterialProviderTool", "TMEF_TrkMaterialProviderTool")

addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_MuonCandidateTrackBuilderTool", "TMEF_MuonCandidateTrackBuilderTool")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_MuonInsideOutRecoTool", "TMEF_MuonInsideOutRecoTool")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_MuonPRDSelectionTool", "TMEF_MuonPRDSelectionTool")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_MuonLayerSegmentFinderTool", "TMEF_MuonLayerSegmentFinderTool")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_MuonClusterSegmentFinderTool", "TMEF_MuonClusterSegmentFinderTool")

addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_MuonStauRecoTool", "TMEF_MuonStauRecoTool")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_MuonStauInsideOutRecoTool", "TMEF_MuonStauInsideOutRecoTool")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_MuonStauCandidateTrackBuilderTool", "TMEF_MuonStauCandidateTrackBuilderTool")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_CombinedStauTrackBuilder", "TMEF_CombinedStauTrackBuilder")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_MuonStauSegmentRegionRecoveryTool", "TMEF_MuonStauSegmentRegionRecoveryTool")
addTool("TrigMuonEF.TrigMuonEFConfig.TMEF_CombinedStauTrackBuilderFit", "TMEF_CombinedStauTrackBuilderFit")

addAlgorithm("TrigMuonEF.TrigMuonEFConfig.TrigMuonEFTrackIsolationConfig", "TrigMuonEFTrackIsolation")

addAlgorithm("TrigMuonEF.TrigMuonEFConfig.TrigMuonEFFSRoiMakerConfig","TrigMuonEFFSRoiMaker")

addAlgorithm("TrigMuonEF.TrigMuonEFConfig.TrigMuonEFIDTrackRoiMaker","TrigMuonEFIDTrackRoiMaker")

addAlgorithm("TrigMuonEF.TrigMuonEFConfig.InDetTrkRoiMaker_Muon", "InDetTrkRoiMaker_Muon")

