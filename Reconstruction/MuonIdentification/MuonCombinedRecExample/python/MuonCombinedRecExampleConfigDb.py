# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addToolClone, addService, addAlgorithm, \
     addTypesToExcludeIfDefaultValue, addNamesToExcludeIfDefaultValue, addFullNamesToExcludeIfDefaultValue, \
     addPropertiesToExcludeIfDefault, \
     addTypesToSkipIfNotAvailable, addNamesToSkipIfNotAvailable, addFullNamesToSkipIfNotAvailable, \
     addTypesOnlyToSkip

from AthenaCommon.Constants import *  # FATAL,ERROR etc.
from InDetRecExample.InDetKeys import InDetKeys
from MuonCombinedRecExample.MuonCombinedKeys import MuonCombinedKeys as MuonCbKeys
# combined tools
addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCombinedTool","MuonCombinedTool")

addTool("MuonCombinedRecExample.MuGirlTagTool.MuonInsideOutRecoTool","MuonInsideOutRecoTool")
addTool("MuonCombinedRecExample.MuGirlTagTool.MuonCandidateTrackBuilderTool","MuonCandidateTrackBuilderTool")
addTool("MuonCombinedRecExample.MuGirlTagTool.MuonLayerSegmentMatchingTool","MuonLayerSegmentMatchingTool")
addTool("MuonCombinedRecExample.MuGirlTagTool.MuonLayerAmbiguitySolverTool","MuonLayerAmbiguitySolverTool")
addTool("MuonCombinedRecExample.MuGirlTagTool.MuonRecoValidationTool","MuonRecoValidationTool")
addTool("MuonCombinedRecExample.MuGirlTagTool.DCMathStauSegmentMaker","DCMathStauSegmentMaker")
addTool("MuonCombinedRecExample.MuGirlTagTool.MuonStauChamberHoleRecoveryTool","MuonStauChamberHoleRecoveryTool")
addTool("MuonCombinedRecExample.MuGirlTagTool.MuonStauSeededSegmentFinder","MuonStauSeededSegmentFinder")
addTool("MuonCombinedRecExample.MuGirlTagTool.MuonStauSegmentRegionRecoveryTool","MuonStauSegmentRegionRecoveryTool")
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
addTool("MuonCombinedRecExample.MuonCombinedFitTools.MuonMaterialProviderTool","MuonMaterialProviderTool")

addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuGirlAlg","MuGirlAlg")
addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonCaloTagAlg","MuonCaloTagAlg")
addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonInsideOutRecoAlg","MuonInsideOutRecoAlg")
addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuGirlStauAlg","MuGirlStauAlg")

addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonCombinedInDetCandidateAlg","MuonCombinedInDetCandidateAlg")
addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonCombinedInDetCandidateAlg",
                                    "MuonCombinedInDetCandidateAlg_LargeD0",
                                    TrackParticleLocation=[InDetKeys.xAODLargeD0TrackParticleContainer()],
                                    InDetCandidateLocation=MuonCbKeys.InDetTrackParticlesLargeD0(),
                                    DoSiliconAssocForwardMuons=False)



addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonCombinedMuonCandidateAlg","MuonCombinedMuonCandidateAlg")
addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonCombinedAlg","MuonCombinedAlg")
addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonCombinedAlg", "MuonCombinedAlg_LargeD0",
                                                                        InDetCandidateLocation=MuonCbKeys.InDetTrackParticlesLargeD0(),
                                                                        CombinedTagMaps=["muidcoTagMap_LargeD0","stacoTagMap_LargeD0"],
                                                                        MuidCombinedTracksLocation="MuidCombinedTracks_LargeD0",
                                                                        MuidMETracksLocation="MuidMETracks_LargeD0")

addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonCombinedInDetExtensionAlg","MuonCombinedInDetExtensionAlg")

addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonSegmentTagAlg","MuonSegmentTagAlg")
addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonSegmentTagAlg","MuonSegmentTagAlg_LargeD0",
                                                                        InDetCandidateLocation=MuonCbKeys.InDetTrackParticlesLargeD0(),
                                                                        TagMap="segmentTagMap_LargeD0",
                                                                        MuonSegmentLocation="MuonSegments")





addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonCreatorAlg","MuonCreatorAlg")
addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.MuonCreatorAlg","MuonCreatorAlg_LargeD0",
                                                                      MuonContainerLocation=MuonCbKeys.FinalMuonsLargeD0(),
                                                                      InDetCandidateLocation=MuonCbKeys.InDetTrackParticlesLargeD0(),
                                                                      ExtrapolatedLocation="ExtraPolatedMuon_LargeD0",
                                                                      ExtrapolatedTrackLocation="ExtraPolatedMuonTrack_LargeD0",
                                                                      MSOnlyExtrapolatedLocation="MSOnlyExtraPolatedMuons_LargeD0",
                                                                      MSOnlyExtrapolatedTrackLocation="MSOnlyExtraPolatedMuonTrack_LargeD0",
                                                                      CombinedLocation=MuonCbKeys.CombinedFitTracksLargeD0(),
                                                                      SegmentContainerName="MuonSegments_LargeD0",
                                                                      TrackSegmentContainerName="TrakMuonSegments_LargeD0",
                                                                      TagMaps=["muidcoTagMap_LargeD0","stacoTagMap_LargeD0","segmentTagMap_LargeD0"],
                                                                      BuildSlowMuon= False,
                                                                      MakeClusters=False    )


addAlgorithm("MuonCombinedRecExample.MuonCombinedAlgs.StauCreatorAlg","StauCreatorAlg")

# tracking tools
addTool("TrkTrackSummaryTool.AtlasTrackSummaryTool.AtlasTrackSummaryTool", "AtlasTrackSummaryTool" )
addTool("Trk::ParticleCaloExtensionTool", "MuonParticleCaloExtensionTool", Extrapolator = "AtlasExtrapolator")

# put this here until it is set in the appropriate package
addTool("MuonCombinedRecExample.MuonSegmentTaggerTools.MuTagMatchingTool","MuTagMatchingTool")
addTool("MuonCombinedRecExample.MuonSegmentTaggerTools.MuTagAmbiguitySolverTool","MuTagAmbiguitySolverTool")
addTool("MuonCombinedRecExample.MuonSegmentTaggerTools.MuonSegmentTagTool","MuonSegmentTagTool")

addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCombinedInDetDetailedTrackSelectorTool","MuonCombinedInDetDetailedTrackSelectorTool")
addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCombinedInDetDetailedTrackSelectorTool","MuonCombinedInDetDetailedForwardTrackSelectorTool", nHitSct=0)
addTool("MuonCombinedRecExample.MuonCombinedTools.MuonInDetForwardCandidateTool","MuonInDetForwardCandidateTool")
addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCombinedParticleCreator","MuonCombinedParticleCreator")
addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCaloParticleCreator","MuonCaloParticleCreator")
addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCreatorTool","MuonCreatorTool")
addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCreatorTool","StauCreatorTool",BuildStauContainer=True)
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
addService("MuonCombinedRecExample.MuonCaloTagTool.CaloMuonScoreONNXRuntimeSvc","CaloMuonScoreONNXRuntimeSvc")

####### muid tools
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
addTool("MuonCombinedRecExample.MuonCombinedFitTools.CombinedMuonTrackBuilder","CombinedMuonTrackBuilder")
addTool("MuonCombinedRecExample.MuonCombinedFitTools.CombinedMuonTrackBuilderFit","CombinedMuonTrackBuilderFit")
