# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addAlgorithm

# combined tools

addTool("MuonCombinedRecExample.MuonCombinedFitTools.ParticleCaloCellAssociationTool","MuonCaloCellAssociationTool")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuonCaloEnergyTool","MuonCaloEnergyTool")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuonMaterialProviderTool","MuonTrkMaterialProviderTool")

addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCombinedTool","MuonCombinedTool")

addTool("MuonCombinedRecExample.MuGirlTagTool.MuonInsideOutRecoTool","MuonInsideOutRecoTool")
addTool("MuonCombinedRecExample.MuGirlTagTool.MuonCandidateTrackBuilderTool","MuonCandidateTrackBuilderTool")
addTool("MuonCombinedRecExample.MuGirlTagTool.MuonLayerSegmentMatchingTool","MuonLayerSegmentMatchingTool")
addTool("MuonCombinedRecExample.MuGirlTagTool.MuonLayerAmbiguitySolverTool","MuonLayerAmbiguitySolverTool")
addTool("MuonCombinedRecExample.MuGirlTagTool.DCMathStauSegmentMaker","DCMathStauSegmentMaker")
addTool("MuonCombinedRecExample.MuGirlTagTool.MuonStauChamberHoleRecoveryTool","MuonStauChamberHoleRecoveryTool")
addTool("MuonCombinedRecExample.MuGirlTagTool.MuonStauSeededSegmentFinder","MuonStauSeededSegmentFinder")
addTool("MuonCombinedRecExample.MuGirlTagTool.MuonStauSegmentRegionRecoveryTool","MuonStauSegmentRegionRecoveryTool")
addTool("MuonCombinedRecExample.MuGirlTagTool.MuonRecoValidationTool","MuonRecoValidationTool")
addTool("MuonCombinedRecExample.MuGirlTagTool.CombinedStauTrackBuilderFit",'CombinedStauTrackBuilderFit')
addTool("MuonCombinedRecExample.MuGirlTagTool.CombinedStauTrackBuilder",'CombinedStauTrackBuilder')
addTool("MuonCombinedRecExample.MuGirlTagTool.MuonStauCandidateTrackBuilderTool","MuonStauCandidateTrackBuilderTool")
addTool("MuonCombinedRecExample.MuGirlTagTool.MuonStauInsideOutRecoTool","MuonStauInsideOutRecoTool")
addTool("MuonCombinedRecExample.MuGirlTagTool.MuonStauRecoTool","MuonStauRecoTool")

addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCombinedFitTagTool","MuonCombinedFitTagTool")
addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCombinedStacoTagTool","MuonCombinedStacoTagTool")
addTool("Trk::TrackParticleCreatorTool","TrackParticleCreatorTool")
addTool("MuonCombinedRecExample.MuonCaloTagTool.MuonCaloTagTool","MuonCaloTagTool")
addTool("Rec::MuonMomentumBalanceSignificanceTool","MuonMomentumBalanceSignificanceTool")
addTool("Rec::MuonScatteringAngleSignificanceTool","MuonScatteringAngleSignificanceTool")
addTool( "Muon::MuonSystemExtensionTool", "MuonSystemExtensionTool", ParticleCaloExtensionTool = "MuonParticleCaloExtensionTool", Extrapolator = "AtlasExtrapolator")




addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonCaloTagAlg","MuonCaloTagAlg")
addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonCaloTagAlg_LRT","MuonCaloTagAlg_LRT")



addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonInsideOutRecoAlg","MuonInsideOutRecoAlg")
addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuGirlAlg_LRT","MuGirlAlg_LRT")
addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuGirlStauAlg","MuGirlStauAlg")

from AthenaCommon.BeamFlags import jobproperties
useOnlyHitSectors = (jobproperties.Beam.beamType()=='collisions')
addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonCombinedInDetCandidateAlg","MuonCombinedInDetCandidateAlg")
addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonCombinedInDetCandidateAlg_LRT","MuonCombinedInDetCandidateAlg_LRT")

addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonInDetToMuonSystemExtensionAlg", "MuonInDetToMuonSystemExtensionAlg", UseOnlyHitSectors=useOnlyHitSectors)
addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonInDetToMuonSystemExtensionAlg_LRT","MuonInDetToMuonSystemExtensionAlg_LRT", UseOnlyHitSectors=useOnlyHitSectors)



addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonCombinedMuonCandidateAlg","MuonCombinedMuonCandidateAlg")

addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonCombinedAlg","MuonCombinedAlg")
addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonCombinedAlg_LRT", "MuonCombinedAlg_LRT")

addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonSegmentTagAlg","MuonSegmentTagAlg")
addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonSegmentTagAlg_LRT","MuonSegmentTagAlg_LRT")

addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonCreatorAlg","MuonCreatorAlg")
addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonCreatorAlg_LRT","MuonCreatorAlg_LRT" )
addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonCreatorAlg_EMEO", "MuonCreatorAlg_EMEO")


addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.StauCreatorAlg","StauCreatorAlg")

# tracking tools
addTool("TrkTrackSummaryTool.AtlasTrackSummaryTool.AtlasTrackSummaryTool", "AtlasTrackSummaryTool" )
addTool("Trk::ParticleCaloExtensionTool", "MuonParticleCaloExtensionTool", Extrapolator = "AtlasExtrapolator")

# put this here until it is set in the appropriate package
addTool("MuonCombinedRecExample.MuonSegmentTaggerTools.MuTagMatchingTool","MuTagMatchingTool")
addTool("MuonCombinedRecExample.MuonSegmentTaggerTools.MuTagAmbiguitySolverTool","MuTagAmbiguitySolverTool")
addTool("MuonCombinedRecExample.MuonSegmentTaggerTools.MuonSegmentTagTool","MuonSegmentTagTool")

addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCombinedInDetDetailedTrackSelectorTool","MuonCombinedInDetDetailedTrackSelectorTool")

addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCombinedInDetDetailedTrackSelectorTool_LRT", "MuonCombinedInDetDetailedTrackSelectorTool_LRT")

addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCombinedInDetDetailedTrackSelectorTool","MuonCombinedInDetDetailedForwardTrackSelectorTool", nHitSct=0)
addTool("MuonCombinedRecExample.MuonCombinedTools.MuonInDetForwardCandidateTool","MuonInDetForwardCandidateTool")
addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCombinedParticleCreator","MuonCombinedParticleCreator")
addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCaloParticleCreator","MuonCaloParticleCreator")
addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCreatorTool","MuonCreatorTool")
addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCreatorTool","StauCreatorTool",BuildStauContainer=True)
addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCreatorTool_LRT","MuonCreatorTool_LRT")

addTool("MuonCombinedRecExample.MuonCombinedTools.ExtrapolateMuonToIPTool","ExtrapolateMuonToIPTool")
addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCandidateTool","MuonCandidateTool")
addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCombinedTrackSummaryTool","MuonCombinedTrackSummaryTool")
addTool("MuonCombinedRecExample.MuonCombinedTools.MuonPrintingTool","MuonPrintingTool")

addTool("MuonCombinedRecExample.MuonCombinedFitTools.OutwardsCombinedMuonTrackBuilder","OutwardsCombinedMuonTrackBuilder")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuonCombinedPropagator","MuonCombinedPropagator")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.OutwardsSegmentRegionRecoveryTool","OutwardsSegmentRegionRecoveryTool")


####### calo tag
addTool("MuonCombinedRecExample.MuonCaloTagTool.CaloTrkMuIdAlgTrackSelectorTool","CaloTrkMuIdAlgTrackSelectorTool")
addTool("MuonCombinedRecExample.MuonCaloTagTool.TrackEnergyInCaloTool","TrackEnergyInCaloTool")
addTool("MuonCombinedRecExample.MuonCaloTagTool.TrackDepositInCaloTool","TrackDepositInCaloTool")
addTool("MuonCombinedRecExample.MuonCaloTagTool.CaloMuonTagLoose","CaloMuonTagLoose")
addTool("MuonCombinedRecExample.MuonCaloTagTool.CaloMuonTag","CaloMuonTag")
addTool("MuonCombinedRecExample.MuonCaloTagTool.CaloMuonLikelihoodTool","CaloMuonLikelihoodTool")
addTool("MuonCombinedRecExample.MuonCaloTagTool.CaloMuonScoreTool","CaloMuonScoreTool")

####### muid tools
addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuonAlignmentUncertToolTheta","MuonAlignmentUncertToolTheta")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuonAlignmentUncertToolPhi"  ,"MuonAlignmentUncertToolPhi")

addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuonMatchQuality","MuonMatchQuality")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuidMuonRecovery","MuidMuonRecovery")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuonCombinedTrackFitter","MuonCombinedTrackFitter")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.CombinedMuonTagTestTool","CombinedMuonTagTestTool")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuidMaterialAllocator","MuidMaterialAllocator")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.iPatFitter","iPatFitter")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.iPatSLFitter","iPatSLFitter")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuidTrackCleaner","MuidTrackCleaner")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.OutwardsTrackCleaner","OutwardsTrackCleaner")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuidCaloEnergyParam","MuidCaloEnergyParam")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuidCaloEnergyMeas","MuidCaloEnergyMeas")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuidCaloEnergyTool","MuidCaloEnergyTool")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuidCaloEnergyToolParam","MuidCaloEnergyToolParam")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuidCaloTrackStateOnSurface","MuidCaloTrackStateOnSurface")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuidCaloTrackStateOnSurfaceParam","MuidCaloTrackStateOnSurfaceParam")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuidMaterialEffectsOnTrackProvider","MuidMaterialEffectsOnTrackProvider")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuidMaterialEffectsOnTrackProviderParam","MuidMaterialEffectsOnTrackProviderParam")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuonTrackQuery","MuonTrackQuery")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuidSegmentRegionRecoveryTool","MuidSegmentRegionRecoveryTool")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuidErrorOptimisationTool","MuidErrorOptimisationTool")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.CombinedMuonTrackBuilder","CombinedMuonTrackBuilder")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.CombinedMuonTrackBuilderFit","CombinedMuonTrackBuilderFit")

addTool("MuonCombinedRecExample.MuonCombinedFitTools.CombinedTrackBuilderFit_EMEO","CombinedTrackBuilderFit_EMEO")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuonSegmentRegionRecoveryTool_EMEO","MuonSegmentRegionRecoveryTool_EMEO")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.CombinedMuonTrackBuilder_EMEO","CombinedMuonTrackBuilder_EMEO")
addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCandidateTool_EMEO", "MuonCandidateTool_EMEO")
addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonCombinedMuonCandidateAlg_EMEO","MuonCombinedMuonCandidateAlg_EMEO")
