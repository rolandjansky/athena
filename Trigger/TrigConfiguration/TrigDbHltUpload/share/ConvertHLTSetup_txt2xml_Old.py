#!/usr/bin/env python
# $Id: ConvertHLTSetup_txt2xml.py,v 1.41 2009/05/20 20:29:36 stelzer Exp $

# converts the python style output from the HistorySvc into xml
# Joerg Stelzer, CERN, 10.Sept.2006

# PJB 21/11/08 Ignore the DataFlowConfig component (savannah 43616)
# but also need to ignore CalBufferName comp of MuFast (set to "")
# which is otherwise set by this

import getopt, sys, re, os.path
import xml.dom.minidom

flatComponentStructure = False
ConversionForPointOne = False # Sylvie 11/07/2008
                              # This is a tempory hack to set "OnlineRun" to "True" for muFast_Muon (muFast)
                              # when preparing configuration for Point1.
                              # This can be set to True by using the option --ForPointOne

typeLUT = {
    'AlignedBackExtrapolator'                                                                :'TrigMuonBackExtrapolator',
    'CscCalibTool'                                                                           :'CscCalibTool',
    'CscClusterOnTrackCreator.CalibCscStripFitter'                                           :'CalibCscStripFitter',
    'CscClusterOnTrackCreator.CalibCscStripFitter.CscCalibTool'                              :'CscCalibTool',
    'CscClusterOnTrackCreator.QratCscClusterFitter'                                          :'QratCscClusterFitter',
    'CscThresholdClusterBuilder.CscSplitClusterFitter'                                       :'CscSplitClusterFitter',
    'CscThresholdClusterBuilder.CscSplitClusterFitter.QratCscClusterFitter'                  :'QratCscClusterFitter',
    'CscThresholdClusterBuilder.CscSplitClusterFitter.SimpleCscClusterFitter'                :'SimpleCscClusterFitter',
    'DCMathSegmentMaker'                                                                     :'Muon::DCMathSegmentMaker',
    'DataFlowConfig'                                                                         :'IGNORE',
    'DefaultTrigMooreHisto'                                                                  :'TrigMooreHisto',
    'EFNoEFIDempid'                                                                          :'EMPIDBuilder',
    'EFconvemtrackmatch.ExtrapolateToCaloTool'                                               :'ExtrapolateToCaloTool',
    'EFegfourmom'                                                                            :'EMFourMomBuilder',
    'EFembremfit'                                                                            :'EMBremsstrahlungBuilder',
    'EFembremfit.CaloDepthToolshowerdefault'                                                 :'CaloDepthTool',
    'EFemconversion.ExtrapolateToCaloTool'                                                   :'ExtrapolateToCaloTool',
    'EFempid'                                                                                :'EMPIDBuilder',
    'EFemshower'                                                                             :'EMShowerBuilder',
    'EFemshower.egammaefconviso'                                                             :'egammaIso',
    'EFemshower.egammaefiso'                                                                 :'egammaIso',
    'EFemshower.egammaefnoefidIso'                                                           :'egammaIso',
    'EFemshower.egammashowershapeEFConv'                                                     :'egammaShowerShape',
    'EFemshower.egammashowershapeEFNoEFID'                                                   :'egammaShowerShape',
    'EFemshower.egammashowershapeEFeg'                                                       :'egammaShowerShape',
    'EFemshower.egefQgcld'                                                                   :'egammaqgcld',
    'EFemshower.egefQgcld.CaloDepthToolshowerdefault'                                        :'CaloDepthTool',
    'EFemshower.egefconvQgcld'                                                               :'egammaqgcld',
    'EFemshower.egefconvQgcld.CaloDepthToolshowerdefault'                                    :'CaloDepthTool',
    'EFemshower.egefconvqpoint'                                                              :'egammaqpoint',
    'EFemshower.egefconvqpoint.CaloDepthToolshowerdefault'                                   :'CaloDepthTool',
    'EFemshower.egefnoefidqpoint'                                                            :'egammaqpoint',
    'EFemshower.egefnoefidqpoint.CaloDepthToolshowerdefault'                                 :'CaloDepthTool',
    'EFemshower.egefqpoint'                                                                  :'egammaqpoint',
    'EFemshower.egefqpoint.CaloDepthToolshowerdefault'                                       :'CaloDepthTool',
    'EFemtrackmatch'                                                                         :'EMTrackMatchBuilder',
    'EFemtrackmatch.ExtrapolateToCaloTool'                                                   :'ExtrapolateToCaloTool',
    'EgammaEmEnFex_Ringer'                                                                   :'EgammaEmEnFex',
    'EgammaHadEnFex_Ringer'                                                                  :'EgammaHadEnFex',
    'EgammaSamp1Fex_Ringer'                                                                  :'EgammaSamp1Fex',
    'EgammaSamp2Fex_Ringer'                                                                  :'EgammaSamp2Fex',
    'EventCnvSuperTool'                                                                      :'Trk::EventCnvSuperTool',
    'ExtrapolTrackToCaloToolentrance'                                                        :'ExtrapolTrackToCaloTool',
    'ExtrapolTrackToCaloToolshowerdefault'                                                   :'ExtrapolTrackToCaloTool',
    'ExtrapolateToCaloTool'                                                                  :'ExtrapolateToCaloTool',
    'H1WeightCone4H1Tower'                                                                   :'H1WeightToolCSC12Generic',
    'IDScanHitFilter_Bphysics'                                                               :'IDScanHitFilter',
    'IDScanHitFilter_Cosmics'                                                                :'IDScanHitFilter',
    'IDScanHitFilter_FullScan'                                                               :'IDScanHitFilter',
    'IDScanHitFilter_Jet'                                                                    :'IDScanHitFilter',
    'IDScanHitFilter_Muon'                                                                   :'IDScanHitFilter',
    'IDScanHitFilter_Tau'                                                                    :'IDScanHitFilter',
    'IDScanHitFilter_Tile'                                                                   :'IDScanHitFilter',
    'IDScanHitFilter_eGamma'                                                                 :'IDScanHitFilter',
    'IDScanHitFilter_muonIso'                                                                :'IDScanHitFilter',
    'IDScanZFinder_Bphysics'                                                                 :'IDScanZFinder',
    'IDScanZFinder_Cosmics'                                                                  :'IDScanZFinder',
    'IDScanZFinder_FullScan'                                                                 :'IDScanZFinder',
    'IDScanZFinder_Jet'                                                                      :'IDScanZFinder',
    'IDScanZFinder_Muon'                                                                     :'IDScanZFinder',
    'IDScanZFinder_Tau'                                                                      :'IDScanZFinder',
    'IDScanZFinder_Tile'                                                                     :'IDScanZFinder',
    'IDScanZFinder_eGamma'                                                                   :'IDScanZFinder',
    'IDScanZFinder_muonIso'                                                                  :'IDScanZFinder',
    'InDetL2_TRT_TrackSegmentsMaker'                                                         :'InDet::TRT_TrackSegmentsMaker_ATLxk',
    'InDetTRTConditionsSummaryTool'                                                          :'TRT_ConditionsSummaryTool',
    'InDetTrigAdaptiveMultiPriVxFinderTool'                                                  :'InDet::InDetAdaptiveMultiPriVxFinderTool',
    'InDetTrigAmbiTrackSelectionToolBeamGas'                                                 :'InDet::InDetAmbiTrackSelectionTool',
    'InDetTrigAmbiTrackSelectionToolCosmicsN'                                                :'InDet::InDetAmbiTrackSelectionTool',
    'InDetTrigAmbiguityProcessor_bjet'                                                       :'Trk::SimpleAmbiguityProcessorTool',
    'InDetTrigAmbiguityProcessor_bphysics'                                                   :'Trk::SimpleAmbiguityProcessorTool',
    'InDetTrigAmbiguityProcessor_electron'                                                   :'Trk::SimpleAmbiguityProcessorTool',
    'InDetTrigAmbiguityProcessor_fullScan'                                                   :'Trk::SimpleAmbiguityProcessorTool',
    'InDetTrigAmbiguityProcessor_minBias'                                                    :'Trk::SimpleAmbiguityProcessorTool',
    'InDetTrigAmbiguityProcessor_muon'                                                       :'Trk::SimpleAmbiguityProcessorTool',
    'InDetTrigAmbiguityProcessor_tau'                                                        :'Trk::SimpleAmbiguityProcessorTool',
    'InDetTrigConversionPropagator'                                                          :'Trk::StraightLinePropagator',
    'InDetTrigExtScoringTool_bphysics'                                                       :'InDet::InDetAmbiScoringTool',
    'InDetTrigExtScoringTool_electron'                                                       :'InDet::InDetAmbiScoringTool',
    'InDetTrigExtScoringTool_muon'                                                           :'InDet::InDetAmbiScoringTool',
    'InDetTrigMergedPixelsTool'                                                              :'InDet::MergedPixelsTool',
    'InDetTrigParticleCreatorTool'                                                           :'Trk::ParticleCreatorTool',
    'InDetTrigPriVxFinderTool'                                                               :'InDet::InDetPriVxFinderTool',
    'InDetTrigSCT_ClusteringTool'                                                            :'InDet::SCT_ClusteringTool',
    'InDetTrigSiTrackMaker_bjet'                                                             :'InDet::SiTrackMaker_xk',
    'InDetTrigSiTrackMaker_bphysics'                                                         :'InDet::SiTrackMaker_xk',
    'InDetTrigSiTrackMaker_electron'                                                         :'InDet::SiTrackMaker_xk',
    'InDetTrigSiTrackMaker_fullScan'                                                         :'InDet::SiTrackMaker_xk',
    'InDetTrigSiTrackMaker_minBias'                                                          :'InDet::SiTrackMaker_xk',
    'InDetTrigSiTrackMaker_muon'                                                             :'InDet::SiTrackMaker_xk',
    'InDetTrigSiTrackMaker_tau'                                                              :'InDet::SiTrackMaker_xk',
    'InDetTrigTRT_DriftCircleTool'                                                           :'InDet::TRT_DriftCircleTool',
    'InDetTrigTRT_SeededTrackTool_photon'                                                    :'InDet::TRT_SeededTrackFinder_ATL',
    'InDetTrigTRT_StandaloneScoringTool'                                                     :'InDet::InDetTrtTrackScoringTool',
    'InDetTrigTRT_StandaloneScoringTool_electron'                                            :'InDet::InDetTrtTrackScoringTool',
    'InDetTrigTRT_StandaloneScoringTool_muon'                                                :'InDet::InDetTrtTrackScoringTool',
    'InDetTrigTRT_StandaloneScoringTool_tau'                                                 :'InDet::InDetTrtTrackScoringTool',
    'InDetTrigTRT_TrackSegmentsMaker'                                                        :'InDet::TRT_TrackSegmentsMaker_ATLxk',
    'InDetTrigTRT_TrackSegmentsMaker_electron'                                               :'InDet::TRT_TrackSegmentsMaker_ATLxk',
    'InDetTrigTRT_TrackSegmentsMaker_muon'                                                   :'InDet::TRT_TrackSegmentsMaker_ATLxk',
    'InDetTrigTRT_TrackSegmentsMaker_photon'                                                 :'InDet::TRT_TrackSegmentsMaker_ATLxk',
    'InDetTrigTRT_TrackSegmentsMaker_tau'                                                    :'InDet::TRT_TrackSegmentsMaker_ATLxk',
    'InDetTrigTrackFitterCosmics'                                                            :'Trk::GlobalChi2Fitter',
    'InDetTrigTrackSlimmingTool'                                                             :'Trk::TrackSlimmingTool',
    'InDetTrigTrackSummaryToolSharedHits'                                                    :'Trk::TrackSummaryTool',
    'InDetTrigZvertexMaker_bjet'                                                             :'InDet::SiZvertexMaker_xk',
    'InDetTrigZvertexMaker_bphysics'                                                         :'InDet::SiZvertexMaker_xk',
    'InDetTrigZvertexMaker_electron'                                                         :'InDet::SiZvertexMaker_xk',
    'InDetTrigZvertexMaker_fullScan'                                                         :'InDet::SiZvertexMaker_xk',
    'InDetTrigZvertexMaker_minBias'                                                          :'InDet::SiZvertexMaker_xk',
    'InDetTrigZvertexMaker_muon'                                                             :'InDet::SiZvertexMaker_xk',
    'InDetTrigZvertexMaker_tau'                                                              :'InDet::SiZvertexMaker_xk',
    'MCTBHitHandler'                                                                         :'Muon::MCTBHitHandler',
    'MCTBHitHandler.FixedErrorMuonClusterOnTrackCreator'                                     :'Muon::MuonClusterOnTrackCreator',
    'MdtDriftCircleOnTrackCreatorAdjustableT0Mboy'                                           :'Muon::MdtDriftCircleOnTrackCreator',
    'MdtDriftCircleOnTrackCreatorAdjustableT0Moore'                                          :'Muon::MdtDriftCircleOnTrackCreator',
    'MdtTubeHitOnTrackCreator'                                                               :'Muon::MdtDriftCircleOnTrackCreator',
    'MisalignedBackExtrapolator'                                                             :'TrigMuonBackExtrapolator',
    'MuGirlNS::CandidateTool'                                                                :'MuGirlNS::CandidateTool',
    'MuidCaloEnergyToolParam'                                                                :'Rec::MuidCaloEnergyTool',
    'MuonChamberHoleRecoveryTool.CscClusterOnTrackCreator'                                   :'Muon::CscClusterOnTrackCreator',
    'MuonChamberHoleRecoveryTool.CscClusterOnTrackCreator.CalibCscStripFitter'               :'CalibCscStripFitter',
    'MuonChamberHoleRecoveryTool.CscClusterOnTrackCreator.CalibCscStripFitter.CscCalibTool'  :'CscCalibTool',
    'MuonChamberHoleRecoveryTool.CscClusterOnTrackCreator.QratCscClusterFitter'              :'QratCscClusterFitter',
    'MuonChi2TrackFitter'                                                                    :'Trk::GlobalChi2Fitter',
    'MuonChi2SLTrackFitter'                                                                  :'Trk::GlobalChi2Fitter',
    'MuonCombiTrackMaker'                                                                    :'Muon::MuonCombiTrackMaker',
    'MuonCscRawDataProviderTool'                                                             :'Muon::CSC_RawDataProviderTool',
    'MuonEDMHelperTool'                                                                      :'Muon::MuonEDMHelperTool',
    'MuonIdHelper'                                                                           :'Rec::MuonIdentificationHelper',
    'MuonInertMaterialBuilder'                                                               :'Muon::MuonInertMaterialBuilder',
    'MuonKalmanTrackFitter'                                                                  :'Trk::KalmanFitter',
    'MuonMdtRawDataProviderTool'                                                             :'Muon::MDT_RawDataProviderTool',
    'MuonPhiHitSelector'                                                                     :'MuonPhiHitSelector',
    'MuonRK_Extrapolator'                                                                    :'Trk::Extrapolator',
    'MuonRpcRawDataProviderTool'                                                             :'Muon::RPC_RawDataProviderTool',
    'MuonSTEP_Extrapolator'                                                                  :'Trk::Extrapolator',
    'MuonSegmentRegionRecoveryTool'                                                          :'Muon::MuonSegmentRegionRecoveryTool',
    'MuonSegmentTrackBuilder'                                                                :'Muon::MuonSegmentTrackBuilder',
    'MuonSegmentTrackBuilderMBMat'                                                           :'Muon::MuonSegmentTrackBuilder',
    'MuonSegmentTrackBuilderTGBlendedMat'                                                    :'Muon::MuonSegmentTrackBuilder',
    'MuonSegmentTrackBuilderTGMat'                                                           :'Muon::MuonSegmentTrackBuilder',
    'MuonStationBuilder'                                                                     :'Muon::MuonStationBuilder',
    'MuonStraightLineExtrapolator'                                                           :'Trk::Extrapolator',
    'MuonTgcRawDataProviderTool'                                                             :'Muon::TGC_RawDataProviderTool',
    'MuonTrackCleaner'                                                                       :'Muon::MuonTrackCleaner',
    'OnlineSpacePointProviderTool'                                                           :'OnlineSpacePointProviderTool',
    'PIDExtrapolator'                                                                        :'Trk::Extrapolator',
    'ShifterTool_Cosmics'                                                                    :'ShifterTool',
    'SimpleCscClusterFitter'                                                                 :'SimpleCscClusterFitter',
    'Streams'                                                                                :'AthSequencer',
    'TMEFCombinedTrackBuilder'                                                               :'Rec::CombinedMuonTrackBuilder',
    'TMEFCombinedTrackBuilderCosmic'                                                         :'RecCombinedMuonTrackBuilder',
    'TMEF_MdtDriftCircleOnTrackCreatorAdjustableT0Cosmic'                                    :'Muon::MdtDriftCircleOnTrackCreator',
    'TMEF_MuonAmbiProcessorCosmic'                                                           :'Trk::SimpleAmbiguityProcessorTool',
    'TMEF_MuonChamberHoleRecoveryToolCosmic'                                                 :'Muon::MuonChamberHoleRecoveryTool',
    'TMEF_MuonCombinePatternToolCosmic'                                                      :'MuonCombinePatternTool',
    'TMEF_MuonCurvedSegmentCombinerCosmic'                                                   :'Muon::MuonCurvedSegmentCombiner',
    'TMEF_MuonHoughPatternToolCosmic'                                                        :'MuonHoughPatternTool',
    'TMEF_MuonPatternSegmentMakerCosmic'                                                     :'Muon::MuonPatternSegmentMaker',
    'TMEF_MuonSegmentCombinationCleanerToolCosmic'                                           :'Muon::MuonSegmentCombinationCleanerTool',
    'TMEF_MuonSegmentTrackBuilderCosmic'                                                     :'Muon::MuonSegmentTrackBuilder',
    'TRTSeededInDetTrigAmbiguityProcessor'                                                   :'Trk::SimpleAmbiguityProcessorTool',
    'TileCondToolNoiseSample.TileCondProxyCool_NoiseAutoCr'                                  :'TileCondProxyCool<TileCalibDrawerFlt>',
    'TileCondToolNoiseSample.TileCondProxyFile_NoiseAutoCr'                                  :'TileCondProxyFile<TileCalibDrawerFlt>',
    'TileCondToolOfc'                                                                        :'TileCondToolOfc',
    'TileCondToolPulseShape'                                                                 :'TileCondToolPulseShape',
    'TileCondToolPulseShape.TileCondProxyCool_PulseShapePhy'                                 :'TileCondProxyCool<TileCalibDrawerFlt>',
    'TileCondToolPulseShape.TileCondProxyFile_PulseShapePhy'                                 :'TileCondProxyFile<TileCalibDrawerFlt>',
    'Trig13pTauSeedNewtonTrkDistanceFinder'                                                  :'Trk::SeedNewtonTrkDistanceFinder',
    'Trig13pTauTrigTrkImpactPoint3dEstimator'                                                :'Trk::ImpactPoint3dEstimator',
    'TrigInDetTrackFitter'                                                                   :'TrigInDetTrackFitter',
    'TrigL2VertexFitter'                                                                     :'TrigL2VertexFitter',
    'TrigMooreBackTracker'                                                                   :'Rec::MuidBackTracker',
    'TrigMooreMatchMaker'                                                                    :'Rec::MuidMatchMaker',
    'TrigMuGirlGlobalFitTool'                                                                :'MuGirlNS::GlobalFitTool',
    'TrigMuGirlParticleCreatorTool'                                                          :'MuGirlNS::MuGirlParticleCreatorTool',
    'TrigMuGirlSelectionTool'                                                                :'MuGirlNS::ANNSelectionTool',
    'TrigPixRawDataProvider_EF'                                                              :'InDet::TrigPixRawDataProvider',
    'TrigSCTRawDataProvider_EF'                                                              :'InDet::TrigSCTRawDataProvider',
    'TrigTRTRawDataProvider_EF'                                                              :'InDet::TrigTRTRawDataProvider',
    'TrigTRT_TrackExtensionTool'                                                             :'TrigTRT_TrackExtensionTool',
    'TrigTau13pTrackLeading'                                                                 :'Tau1P3PTrackLeading',
    'TrigTau13pTrackLeading.TrigTau3PTrackSelect'                                            :'TauCommonTrackSelect',
    'TrigTau13pTrackLeading.TrigTau3PTrackSelectBQ'                                          :'TauCommonTrackSelect',
    'TrigTau1P3PAddCaloInfo'                                                                 :'Tau1P3PAddCaloInfo',
    'TrigTau1P3PTrackMatchCalo'                                                              :'Tau1P3PTrackMatchCalo',
    'TrigTau1P3PTrackMatchCells'                                                             :'Tau1P3PTrackMatchCells',
    'TrigTau1P3PTrackQualify'                                                                :'Tau1P3PTrackQualify',
    'TrigTau1P3PTrackQualify.TrigTau1PTrackSelect'                                           :'TauCommonTrackSelect',
    'TrigTau1p3pVertex'                                                                      :'TauCommonVertex',
    'TrigTauCalibrate'                                                                       :'tauCalibrateWeightTool',
    'TrigTauCells'                                                                           :'tauCellBuilder',
    'TrigTauRecVertex'                                                                       :'TauCommonVertex',
    'TrigTauSeedBuilder'                                                                     :'tauMergeSeedBuilder',
    'TrigTauSeedNewtonTrkDistanceFinder'                                                     :'Trk::SeedNewtonTrkDistanceFinder',
    'TrigTauSeeds'                                                                           :'tauSeedBuilder',
    'TrigTauTracks'                                                                          :'tauTrack',
    'TrigTauTracks.TrigTauCommonTrackSelect'                                                 :'TauCommonTrackSelect',
    'Trk::FastVertexFitter'                                                                  :'Trk::FastVertexFitter',
    'EFemtrackmatch.egammaExtrapolationTool'                                                 :'EMExtrapolationTools',
    'EFconvemtrackmatch.egammaExtrapolationTool'                                             :'EMExtrapolationTools',
    'egammaIso'                                                                              :'egammaIso',
    'egammaefconviso'                                                                        :'egammaIso',
    'egammaefiso'                                                                            :'egammaIso',
    'egammaefnoefidIso'                                                                      :'egammaIso',
    'egammashowershapeEFConv'                                                                :'egammaShowerShape',
    'egammashowershapeEFeg'                                                                  :'egammaShowerShape',
    'egefQgcld'                                                                              :'egammaqgcld',
    'egefQgcld.CaloDepthToolshowerdefault'                                                   :'CaloDepthTool',
    'egefconvQgcld.CaloDepthToolshowerdefault'                                               :'CaloDepthTool',
    'egefconvqpoint'                                                                         :'egammaqpoint',
    'egefconvqpoint.CaloDepthToolshowerdefault'                                              :'CaloDepthTool',
    'egefnoefidqpoint'                                                                       :'egammaqpoint',
    'egefnoefidqpoint.CaloDepthToolshowerdefault'                                            :'CaloDepthTool',
    'egefqpoint'                                                                             :'egammaqpoint',
    'egefqpoint.CaloDepthToolshowerdefault'                                                  :'CaloDepthTool',
    'EFemconversion.exToCalo'                                                                :'ExtrapolateToCaloTool',
    'EFconvemtrackmatch.exToCalo'                                                            :'ExtrapolateToCaloTool',
    'EFemtrackmatch.exToCalo'                                                                :'ExtrapolateToCaloTool',
    'ConfiguredOnlineSpacePointProviderTool':'OnlineSpacePointProviderTool',
    'Csc2dSegmentMaker':'Csc2dSegmentMaker',
    'InDetL2_TRT_TrackSegmentsMaker_BarrelCosmics':'InDet::TRT_TrackSegmentsMaker_BarrelCosmics',
    'InDetL2_TRT_TrackSegmentsMaker_BarrelCosmics_NoField':'InDet::TRT_TrackSegmentsMaker_BarrelCosmics',
    'InDetTrigAmbiTrackSelectionTool':'InDet::InDetAmbiTrackSelectionTool',
    'InDetTrigAmbiguityProcessorLowPt':'Trk::SimpleAmbiguityProcessorTool',
    'InDetTrigAmbiguityProcessor_cosmicsN':'Trk::SimpleAmbiguityProcessorTool',
    'InDetTrigAmbiguityProcessor_minBias2':'Trk::SimpleAmbiguityProcessorTool',
    'InDetTrigClusterMakerTool':'InDet::ClusterMakerTool',
    'InDetTrigConversionFinderTools':'InDet::InDetConversionFinderTools',
    'InDetTrigCosmicScoringTool_TRT':'InDet::InDetCosmicScoringTool',
    'InDetTrigExtScoringTool_cosmicsN':'InDet::InDetAmbiScoringTool',
    'InDetTrigParticleCreatorToolParams':'Trk::TrackParticleCreatorTool',
    'InDetTrigSCTSpacePointTool':'InDet::SCT_TrigSpacePointTool',
    'InDetTrigScoringTool_cosmicsN':'InDet::InDetAmbiScoringTool',
    'InDetTrigSiComTrackFinder':'InDet::SiCombinatorialTrackFinder_xk',
    'InDetTrigSiDetElementsRoadMaker':'InDet::SiDetElementsRoadMaker_xk',
    'InDetTrigSiSpacePointMakerTool':'InDet::SiSpacePointMakerTool',
    'InDetTrigSiTrackMakerLowPt':'InDet::SiTrackMaker_xk',
    'InDetTrigSiTrackMaker_cosmicsN':'InDet::SiTrackMaker_xk',
    'InDetTrigSiTrackMaker_minBias2':'InDet::SiTrackMaker_xk',
    'InDetTrigTRT_StandaloneScoringTool_cosmicsN':'InDet::InDetTrtTrackScoringTool',
    'InDetTrigTRT_StandaloneScoringTool_photon':'InDet::InDetTrtTrackScoringTool',
    'InDetTrigTRT_TrackSegmentsMaker_cosmicsN':'InDet::TRT_TrackSegmentsMaker_ATLxk',
    'InDetTrigTrackFitter':'Trk::GlobalChi2Fitter',
    'InDetTrigTrackFitterLowPt':'Trk::GlobalChi2Fitter',
    'InDetTrigTrackFitterTRT':'Trk::GlobalChi2Fitter',
    'InDetTrigTrackSlimmingToolParams':'Trk::TrackSlimmingTool',
    'InDetTrigTrackSummaryTool':'Trk::TrackSummaryTool',
    'InDetTrigZvertexMakerLowPt':'InDet::SiZvertexMaker_xk',
    'InDetTrigZvertexMaker_cosmicsN':'InDet::SiZvertexMaker_xk',
    'InDetTrigZvertexMaker_minBias2':'InDet::SiZvertexMaker_xk',
    'L2DataLoader':'L2DataLoader',
    'L2MdtTool':'L2MdtTool',
    'L2MdtTool.MdtTimingTool':'MdtTimingTool',
    'L2RpcTool':'L2RpcTool',
    'LArTrackingVolumeHelper':'Trk::TrackingVolumeHelper',
    'MCTBExtrapolatorBlendedMat':'Trk::Extrapolator',
    'MdtTimingTool':'MdtTimingTool',
    'MuGirlStauCombinedMuonTrackBuilder':'Rec::CombinedMuonTrackBuilder',
    'MuonHoleRecovery.CscClusterOnTrackCreator':'Muon::CscClusterOnTrackCreator',
    'MuonSegmentMatchingTool':'Muon::MuonSegmentMatchingTool',
    'MuonTrackExtrapolationTool':'Muon::MuonTrackExtrapolationTool',
    'MuonTrackSelectorTool':'Muon::MuonTrackSelectorTool',
    'PixelClusterCacheTool.PixelRodDecoder':'PixelRodDecoder',
    'SCT_ClusterCacheTool.SCT_RodDecoder':'SCT_RodDecoder',
    'TrigCosmicsTauExtrapolateToCaloTool':'ExtrapolateToCaloTool',
    'TrigMuGirlSAGlobalFitTool':'MuGirlNS::GlobalFitTool',
    'TrigMuGirlStauGlobalFitTool':'MuGirlNS::GlobalFitTool',
    'TrigMuGirlStauTool':'MuGirlNS::StauTool',
    'TrigTRT_DriftCircleProviderTool':'TrigTRT_DriftCircleProviderTool',
    'TrigTRT_TrackExtensionTool.TrigTRT_DetElementRoadTool':'TrigTRT_DetElementRoadTool',
    'TrigTauCommonCalcVar':'TauCommonCalcVars',
    'TrigTauCosmicsTrackSelect':'InDet::InDetDetailedTrackSelectorTool',
    'TrigTauVertex':'TauCommonVertex',
    'egammaQgcld_TrigEgammaRec_PhotonConversions.CaloDepthToolshowerdefault':'CaloDepthTool',
    'egammaQgcld_TrigEgammaRec_eGamma.CaloDepthToolshowerdefault':'CaloDepthTool',
    'egammaQpoint_CosmicEgamma_V2TrigEgammaRec_NoIDEF_eGamma.CaloDepthToolshowerdefault':'CaloDepthTool',
    'egammaQpoint_TrigEgammaRec_NoIDEF_eGamma.CaloDepthToolshowerdefault':'CaloDepthTool',
    'egammaQpoint_TrigEgammaRec_PhotonConversions.CaloDepthToolshowerdefault':'CaloDepthTool',
    'egammaQpoint_TrigEgammaRec_eGamma.CaloDepthToolshowerdefault':'CaloDepthTool',
    'egfourmom_CosmicEgamma_V2TrigEgammaRec_NoIDEF_eGamma':'EMFourMomBuilder',
    'egfourmom_TrigEgammaRec_NoIDEF_eGamma':'EMFourMomBuilder',
    'egfourmom_TrigEgammaRec_PhotonConversions':'EMFourMomBuilder',
    'egfourmom_TrigEgammaRec_eGamma':'EMFourMomBuilder',
    'emconversion_TrigEgammaRec_PhotonConversions':'EMConversionBuilder',
    'emconversion_TrigEgammaRec_PhotonConversions.exToCalo':'ExtrapolateToCaloTool',
    'empid_CosmicEgamma_V2TrigEgammaRec_NoIDEF_eGamma':'EMPIDBuilder',
    'empid_TrigEgammaRec_NoIDEF_eGamma':'EMPIDBuilder',
    'empid_TrigEgammaRec_PhotonConversions':'EMPIDBuilder',
    'empid_TrigEgammaRec_eGamma':'EMPIDBuilder',
    'emshower_CosmicEgamma_V2TrigEgammaRec_NoIDEF_eGamma':'EMShowerBuilder',
    'emshower_CosmicEgamma_V2TrigEgammaRec_NoIDEF_eGamma.egammashowershape_CosmicEgamma_V2TrigEgammaRec_NoIDEF_eGamma':'egammaShowerShape',
    'emshower_TrigEgammaRec_NoIDEF_eGamma':'EMShowerBuilder',
    'emshower_TrigEgammaRec_NoIDEF_eGamma.egammashowershape_TrigEgammaRec_NoIDEF_eGamma':'egammaShowerShape',
    'emshower_TrigEgammaRec_PhotonConversions':'EMShowerBuilder',
    'emshower_TrigEgammaRec_PhotonConversions.egammashowershape_TrigEgammaRec_PhotonConversions':'egammaShowerShape',
    'emshower_TrigEgammaRec_eGamma':'EMShowerBuilder',
    'emshower_TrigEgammaRec_eGamma.egammashowershape_TrigEgammaRec_eGamma':'egammaShowerShape',
    'emtrackmatch_TrigEgammaRec_PhotonConversions':'EMTrackMatchBuilder',
    'emtrackmatch_TrigEgammaRec_PhotonConversions.egammaExtrapolationTool':'EMExtrapolationTools',
    'emtrackmatch_TrigEgammaRec_PhotonConversions.exToCalo':'ExtrapolateToCaloTool',
    'emtrackmatch_TrigEgammaRec_eGamma':'EMTrackMatchBuilder',
    'emtrackmatch_TrigEgammaRec_eGamma.egammaExtrapolationTool':'EMExtrapolationTools',
    'emtrackmatch_TrigEgammaRec_eGamma.exToCalo':'ExtrapolateToCaloTool',
    'CosmicMuonOnly_V2TrigL2CosmicMuon.L2DataLoader':'L2DataLoader',
    'CosmicMuonOnly_V2TrigL2CosmicMuon.L2MdtTool':'L2MdtTool',
    'CosmicMuonOnly_V2TrigL2CosmicMuon.L2MdtTool.MdtTimingTool':'MdtTimingTool',
    'CosmicMuonOnly_V2TrigL2CosmicMuon.L2RpcTool':'L2RpcTool',
    'CosmicMuon_MuonEF_V2TrigL2CosmicMuon.L2DataLoader':'L2DataLoader',
    'CosmicMuon_MuonEF_V2TrigL2CosmicMuon.L2MdtTool':'L2MdtTool',
    'CosmicMuon_MuonEF_V2TrigL2CosmicMuon.L2MdtTool.MdtTimingTool':'MdtTimingTool',
    'CosmicMuon_MuonEF_V2TrigL2CosmicMuon.L2RpcTool':'L2RpcTool',
    'CosmicMuon_V2TrigL2CosmicMuon.L2DataLoader':'L2DataLoader',
    'CosmicMuon_V2TrigL2CosmicMuon.L2MdtTool':'L2MdtTool',
    'CosmicMuon_V2TrigL2CosmicMuon.L2MdtTool.MdtTimingTool':'MdtTimingTool',
    'FixedErrorMuonClusterOnTrackCreator':'Muon::MuonClusterOnTrackCreator',
    'InDetTrigExtScoringTool_bjet':'InDet::InDetAmbiScoringTool',
    'InDetVKalVxInJetTool':'InDet::InDetVKalVxInJetTool',
    'InDetVKalVxInJetTool.VertexFitterTool':'Trk::TrkVKalVrtFitter',
    'MuonCombinePatternTool':'MuonCombinePatternTool',
    'MuonHoughPatternTool':'MuonHoughPatternTool',
    'MuonSegmentMomentum':'MuonSegmentMomentum'
    }

blockRE  = re.compile('^(GLOBAL|SERVICES|ALGORITHMS|ALGTOOLS)$')
lineRE   = re.compile("([\w\.:\<\>\,\* \@\-]+?)\s*'([^']*)':(.*)")
propRE   = re.compile(" '(.*)':(.*)")
typeRE   = re.compile(".*?([\w:]+)/([\w:]+).*?")
numberRE = re.compile("[-+]?(\d+\.\d*|\.\d+)([eE][-+]?\d+)?")
ninesRE  = re.compile("([-+]?\d*\.\d*)999999[5-9]")
zerosRE  = re.compile("([-+]?\d*\.\d*)000000[0-6]")

lines = []
linecounter = 0

errorstate = 'SUCCESS'

class setup:
    def __init__(self, name="setup"):
        self.name  = name
        self.components = []
    def __str__(self):
        s  = '<?xml version="1.0" encoding="UTF-8"?>\n'
        s += '<setup version="1.00000" name="setup">\n'
        for c in self.components:
            s += str(c)
        s += '</setup>\n'
        return s
    def addComponent(self, component):
        alS = component.alias.split('.')
        if not component.alias.count('.'):
            for c in self.components:
                if c.alias==component.alias:
                    raise NameError, 'Trying to add component %s a second time to the setup' % component.alias
            self.components.append(component)
            #print "Adding component %s to setup" % component.alias
            return
        else:
            for c in self.components:
                if component.alias.startswith(c.alias+'.'):
                    c.addComponent(component)
                    return
            raise NameError, "Parent of component '%s' not found in the setup" % component.alias
    def __getitem__(self,alias):
        for c in self.components:
            if c.alias==alias: return c
            if alias.startswith(c.alias+'.'):
                return c[alias]
        raise IndexError, 'Component %s not in the setup' % alias
    def _getSA(self):
        return [c for c in self.components if c.classname=="HLT::TrigSteer"]
    def _getPrivTools(self):
        pt = []
        for c in self.components:
            pt += c.getPrivateTools()
        return pt
    def _getAllPropertyVals(self):
        vals = []
        for c in self.components:
            vals += c.getPropertyVals()
        return vals
    
    steeralg = property(_getSA)
    privtools = property(_getPrivTools)
    propvals = property(_getAllPropertyVals)
        
class component(object):
    count = 0
    def __init__(self, alias, classname, properties):
        self.__class__.count += 1
        self.alias      = alias
        self.classname  = classname
        self.isTopAlg   = 0
        self.isAlg      = 0
        self.isService  = 0
        self.isPubTool  = 0
        self.isPrivTool = 0
        self.isAuditor  = 0
        self.isSteerAlg = 0
        self.parameters = []
        self.components = []
        self.subalgs    = []
        self.indent     = 0
        self.addParameters(properties)
    def __str__(self):
        alias = self.alias.replace('&',' &amp; ')
        alias = alias.replace('>','&gt;')
        alias = alias.replace('<','&lt;')
        alias = alias.replace('\'','&#39;')
        alias = alias.replace('"','&quot;')
        
        classname = self.classname.replace('&',' &amp; ')
        classname = classname.replace('>','&gt;')
        classname = classname.replace('<','&lt;')
        classname = classname.replace('\'','&#39;')
        classname = classname.replace('"','&quot;')
        
        s  = self.indent*2*' '+'  <component alias="%s" name="%s" topalg="%i" alg="%i" steeralg="%i" svc="%i" pubT="%i" privT="%i" aud="%i">\n' % \
             (alias, classname, self.isTopAlg, self.isAlg, self.isSteerAlg, self.isService, self.isPubTool, self.isPrivTool, self.isAuditor)
        for p in self.parameters:
            p.indent = self.indent
            s += str(p)
        if flatComponentStructure:
            s += self.indent*2*' '+'  </component>\n'
            for c in self.components:
                c.indent = self.indent
                s += str(c)
        else:
            for c in self.components:
                c.indent = self.indent+1
                s += str(c)
            s += self.indent*2*' '+'  </component>\n'
        return s
    def __getitem__(self,k):
        p = self.getParameter(k)
        if p: return p.value
        for c in self.components:
            if c.alias==k: return c
            if k.startswith(c.alias+'.'):
                return c[k]
        raise IndexError, 'In component %s no parameter or component %s has been found' % (self.alias,k)

    def getParameter(self,pa):
        for p in self.parameters:
            if p.name == pa: return p
        return None 

    def getPrivateTools(self):
        pt = []
        if self.isPrivTool!=0:
            pt += [self.alias]
        for c in self.components:
            pt += c.getPrivateTools()
        return pt

    def getPropertyVals(self):
        vals = [x.value for x in self.parameters]
        for c in self.components:
            vals += c.getPropertyVals()
        return vals
    
    def addComponent(self, component):
        if map(lambda c:c.alias==component.alias,self.components).count(True):
            raise NameError, 'Trying to add component %s a second time to the component %s' % (component.alias,self.alias)
        for c in self.components:
            if component.alias.startswith(c.alias+'.'):
                c.addComponent(component)
                return
        self.components.append(component)
        #print "Adding component %s to component %s" % (component.alias,self.alias)
        return

    def addAliasOfSubAlg(self,subalg):
        self.subalgs.append(subalg)
        
    def addParameters(self, properties):
        for parameterName, parameterValue in zip(properties[0],properties[1]):
            self.addParameter(parameterName, parameterValue)
            
    def addParameter(self, parameterName, parameterValue):
        for p in self.parameters:
            if p.name == parameterName:
                if p.value != parameterValue:
                    pass
                return
        
        newParameter = parameter(parameterName, parameterValue)
        insert = self.checkParameter(newParameter)
        if not insert: return
        self.parameters.append(newParameter)

    def updateParameter(self, parameterName, parameterValue):
        for p in self.parameters:
            if p.name == parameterName:
                p.value = parameterValue
                p.removeRoundingError()
                self.checkParameter(p)
                return

    def checkParameter(self,parameter):
        insert = True
        value = parameter.value
        value = value.strip('"')          # strip the outer double-quotes
        if self.alias == 'HistorySvc':
            if parameter.name == 'Dump':       value = '0'
            if parameter.name == 'Activate':   value = '0'
            if parameter.name == 'OutputFile': value = ''
        if parameter.name == 'OutputLevel' and value < '3': value = '3'
        if parameter.name == 'rfac' and value=='': value = '""'
        if parameter.name == 'energies' and value=='': value = '""'
        if parameter.name == 'regions' and value=='': value = '""'
        if parameter.name == 'correction' and value=='': value = '""'
        if parameter.name == 'interp_barriers' and value=='': value = '""'
        #if parameter.name=='Folders': value = self.fixFolders(value)
        #more of these in jet calib
        if parameter.name == 'wtEMB0'  and value=='': value = '""'
        if parameter.name == 'wtEME0'  and value=='': value = '""'
        if parameter.name == 'wtEMB1'  and value=='': value = '""'
        if parameter.name == 'wtEMB2'  and value=='': value = '""'
        if parameter.name == 'wtEME1'  and value=='': value = '""'
        if parameter.name == 'wtEME2'  and value=='': value = '""'
        if parameter.name == 'wtTile1' and value=='': value = '""'
        if parameter.name == 'wtTile2' and value=='': value = '""'
        if parameter.name == 'wtHec1'  and value=='': value = '""'
        if parameter.name == 'wtHec2'  and value=='': value = '""'
        if parameter.name == 'wtFCal1' and value=='': value = '""'
        if parameter.name == 'wtFCal2' and value=='': value = '""'
        if parameter.name == 'etaFit'  and value=='': value = '""'
        if parameter.name == 'CorrSpecs'  and value=='': value = '[]'
        if parameter.name == 'KeepList'   and value=='': value = '[]'
        if parameter.name == 'sampling_depth' and value=='': value = '""'
        #PJB set the MuonCalBufferName property to ""
        if parameter.name == 'MuonCalBufferName': value = ''
        #PJB set the DoRecursiveLoad to ""
        #if parameter.name == 'DoRecursiveLoad': value = ''
        # Sylvie 11/07/2008
        # This is a tempory hack to set "OnlineRun" to "True" for muFast_Muon (muFast)
        # when preparing configuration for Point1.
        # This can be set to True by using the option --ForPointOne
        if self.alias == 'muFast_Muon' and self.classname == 'muFast' and parameter.name == 'OnlineRun':
            if ConversionForPointOne:
                print "::Warning:: Parameter '%s' from alias '%s' and classname '%s' will be set to 'True' since 'ForPointOne' option has been set. This is a temporary hack while waiting for the real fix" %(parameter.name,self.alias,self.classname)
                value = 'True'
                
        #if componentAlias == 'THistSvc':
        #    if parameterName == 'Input': parameterValue = parameterValue.replace('/afs/cern.ch/atlas/software/builds/','/sw/comm2/')

        #if len(value)>4000:
            #print "ERROR in line %i: parameter value for parameter %s has %i characters. Maximum is 4000 (Oracle limit)" % (linecounter, parameter.name, len(value))
            #global errorstate
            #errorstate = 'ERROR'
            #insert = False
        #value = value.replace('"',"&quot;")    # replace all other double-quotes by &quot;
        #value = value.replace('<','&lt;')
        value = value.replace('\\','\\\\')
        if value.lower() == 'nan': value = '0'
        parameter.value = value
        return insert
    def __eq__(self, other):
        return self.alias == other.alias

    def fixFolders(self,value):
        # split into folders
        origValue = value
        value = value.lstrip('[')
        value = value.rstrip(']')
        folders = value.split(',')
        folderRE = re.compile(".*<db>(.*)</db>(.*)")
        D = {}
        for f in folders:
            f = f.strip(' ')
            f = f.strip("'")
            m = folderRE.match(f)
            if m != None:
                conn = m.group(1)
                folder = m.group(2)
                D.setdefault(conn,set()).add(folder)
            else:
                print "ERROR in IOVdbSvc folder description: %s" % f
                global errorstate
                errorstate = 'ERROR'
        max_occur = 0
        max_size  = 0
        max_conn  = ''
        for conn in D:
            size = len(D[conn])*len(conn)
            if size>max_size:
                max_size = size
                max_occur = len(D[conn])
                max_conn = conn
        print "IOVdbSvc connection %s has largest size (%i, occurances %i) and is being replaced by empty string." % (max_conn,max_size,max_occur)

        newFolder = "["
        for conn in D:
            for f in D[conn]:
                if conn!=max_conn:
                    newFolder += "'<db>%s</db>%s'," % (conn,f)
                else:
                    newFolder += "'%s'," % f
        newFolder = newFolder.rstrip(",")
        newFolder += "]"

        # now set the default connection
        foundDefConn = False
        for p in self.parameters:
            if p.name == 'dbConnection':
                p.value = max_conn
                foundDefConn = True
        if not foundDefConn:
            self.parameters.append(parameter('dbConnection', max_conn))

        print 'Folder size:',len(newFolder)
        return newFolder

class AppMgrComponent(component):
    def __init__(self, properties):
        super(AppMgrComponent, self).__init__('ApplicationMgr', 'ApplicationMgr', properties)
    def _getTA(self):
        ta = [t.strip('"').strip("'") for t in self['TopAlg'].strip('[]').split(',')]
        return map(lambda x: x.strip("'").split("/")[-1], ta)
    def _getSvc(self):
        svc = map(lambda x: x.strip().strip("'").split('/'), self['ExtSvc'].strip('[]').split(','))
        svc += map(lambda x: x.strip().strip("'").split('/'), self['SvcMapping'].strip('[]').split(','))
        d = {}
        for s in svc:
            if len(s)<2:
                print "WARNING Service %s listed in ApplicationMgr without type" % s
                continue
            d[s[1]] = s[0]
        return d    
    topalg = property(_getTA)
    service = property(_getSvc)

    
class parameter:
    def __init__(self, name, value):
        self.isUserDefined = False
        self.name  = name
        self.value = value
        self.removeRoundingError()
        self.indent     = 0

    def removeRoundingError(self):
        val = self.value
        if val.startswith('[') and val.endswith(']'):
          try :
            val = eval(val)
          except :
            # Nothing happened just pass the value as string
            pass
          self.value = '%s' % (val)
        else:
            self.value=self.removeRoundingErrorFromNumber(val)

    def removeRoundingErrorFromNumber(self,val):
        orig = val
        m = numberRE.match(val)
        if not m: return val
        exp = m.group(2)
        m = ninesRE.match(val)
        if m:
            val = str(float(m.group(1)+'9999999999'))
            if exp:
                val += exp
            return val
        m = zerosRE.match(val)
        if m:
            val = str(float(m.group(1)+'0000000000'))
            if exp:
                val += exp
            return val
        return val


    def __str__(self):
        # Formating of special characters for xml format
        value = self.value.replace('&',' &amp; ')
        value = value.replace('>','&gt;')
        value = value.replace('<','&lt;')
        value = value.replace('\'','&#39;')
        value = value.replace('"','&quot;')
        return self.indent*2*' '+'    <parameter name="%s" value="%s" op="set"/>\n' % (self.name,value)
    def __eq__(self, other):
        return self.name == other.name


def usage():
    """Prints a help message"""
    print "\nusage:"
    print "%s [options] inputfile.txt" % sys.argv[0]
    print "options are:"
    print "  -h|--help        : prints this message"
    print "  --ForPointOne    : temporary hack to run at Point 1 (muFast_Muon)\n"
    print "Minimum required   : you have to specify the .txt file to be converted.\n"
    return


def splitTheLine(line):
    """ this function takes one input line:
    component  "parname": parvalue
    from the python file and splits it into
    three parts:
    - component
    - parameter name
    - parameter value
    In the python file the parameter name is always
    quoted, and followed by a colon"""

    if len(line)==0: return []

    if line in ['GLOBAL', 'SERVICES', 'ALGORITHMS', 'ALGTOOLS']: return [line]

    matchgroups = lineRE.match(line)
    
    if matchgroups==None: return None
    
    return list(matchgroups.groups())


def splitListOption(liststring):
    """splits a string of the form
    "['Classname1/InstanceName1', 'Classname2/InstanceName2', 'Classname3/InstanceName3']"
    into its components and fills a dictionary {"Classname1": "InstanceName1", ... } """
    dict = {}
    for s in liststring.strip("[]").split(','):
        item = s.strip(" ").strip('"').strip("'").split('/',1)
        if len(item) == 1:
            if item[0]!='': dict[item[0]] = item[0]
        else:
            dict[item[1]] = item[0]
    return dict




def getLine():
    global linecounter
    global lines
    line = lines[linecounter].rstrip('\n')
    linecounter+=1
    #print "Read line %04i: %s" % (linecounter,line)
    return line


def readComponent():
    while True:
        line = getLine()
        if line=='}': return None;
        line_items = line.split()
        #Remove possible empty space character behing the Type/Name and at the end of line
        if line.startswith('Type:'): Type = line[6:].strip()
        if line.startswith('Name:'): Name = line[6:].strip()
        if line.startswith('Properties:'): break
    line = getLine()
    properties = ([],[])
    while line!=']':
        m=propRE.match(line)
        if m:
            properties[0].append(m.group(1))
            properties[1].append(m.group(2))
        else:
            global linecounter
            raise NameError, 'Line %i: Property does not match format\n%s' % (linecounter,line)
        line = getLine()
    newComponent = component(alias=Name, classname=Type, properties=properties)
    return newComponent


def typeLookup(alias):
    return typeLUT[alias]

def valArrayLen(val):
    if not (val.startswith('[') and val.endswith(']')):
        return 1;
    val = val.strip('[]').strip()
    if len(val)==0:
        return 0;
    return val.count(',')+1

def updateExistingComponentWithChangedProperties(existingComponent,properties):
    for pName, pVal in zip(properties[0],properties[1]):
        if existingComponent.getParameter(pName) :
          existingVal = existingComponent.getParameter(pName).value
          #if valArrayLen(pVal) != valArrayLen(existingVal):
          #    print "ERROR in GLOBAL: %s::%s: %s --> %s" % (existingComponent.alias, pName, existingVal, pVal),valArrayLen(existingVal), valArrayLen(pVal)
          # assign the new parameter
          existingComponent.updateParameter(pName,pVal)
        else :
          existingComponent.addParameter(pName,pVal)
        



if __name__ == '__main__':

    theSetup = setup("test")

    short_opt = "h?"
    long_opt = ['help','ForPointOne']
    
    if len(sys.argv) <= 1:
        usage()
        sys.exit(1)
        
    try:
        opts, args = getopt.getopt(sys.argv[1:], short_opt, long_opt)
    except getopt.GetoptError:
        usage()
        sys.exit(2)

    # Read the options
    for o, a in opts:
        if o in ('-h', '-?', '--help'):
            usage()
            sys.exit(0)
        # Sylvie 11/07/2008
        # This is a tempory hack to set "OnlineRun" to "True" for muFast_Muon (muFast)
        # when preparing configuration for Point1.
        if o in ('--ForPointOne'):
            ConversionForPointOne = True
        else:
            ConversionForPointOne = False    

    infilename  = sys.argv[-1] # The input text file is always the last argument
    if len(infilename)-infilename.rfind(".txt") == 4:
        (root,ext) = os.path.splitext(infilename)
        if ext==".txt":
            outfilename = root+".xml"
        else:
            outfilename = root+ext+".xml"
    else:
        print "\nError, the file to be converted (last argument) is not a .txt file.\n"
        usage()
        sys.exit(1)


    print "Input   : ", infilename
    print "Output  : ", outfilename

    fin  = open(infilename, 'r')
    lines = fin.readlines()
    fin.close()


    currentPart = ''
    globalProp = {}
    readComponents = {}
    readsubalgs = {}
    
    while True:
        try:
            line = getLine()
        except IndexError:
            break

        if blockRE.match(line):
            currentPart = line.rstrip();
            print currentPart
            continue

        if currentPart=='GLOBAL':
            parts = splitTheLine(line.strip())
            if parts == None:
                print "Error in parsing line %i:\n%s" % (linecounter,line)
                sys.exit(1)
            if len(parts)==0: continue;        # empty line
            [componentAlias, parameterName, parameterValue] = parts
            if componentAlias == "CosmicEgammaTrigCaloClusterMaker_slw.trigslw":
                 print "Temporary hack for cosmics, ignore alias '",componentAlias,"'"
                 continue;
            if componentAlias == "DataFlowConfig":
                 print "Ignoring bizarre DataFlowConfig component"
                 continue;
            if not globalProp.has_key(componentAlias):
                globalProp[componentAlias] = ([],[])
            globalProp[componentAlias][0].append(parameterName)
            globalProp[componentAlias][1].append(parameterValue)
        else:
            if line.startswith('>>'):
                newComponent = readComponent()
                if currentPart=='SERVICES':
                    newComponent.isService=1
                elif currentPart=='ALGORITHMS':
                    newComponent.isAlg=1
                elif currentPart=='ALGTOOLS':
                    if newComponent.alias.startswith("ToolSvc."):
                        newComponent.isPubTool=1
                    else:
                        newComponent.isPrivTool=1
                #print "%3i:" % newComponent.count, newComponent
                ### theSetup.addComponent(newComponent)
                nDots = newComponent.alias.count('.')
                readComponents.setdefault(nDots,[]).append(newComponent)
                
            if line.startswith('-------'):
                tmpComponent = readComponent()
                if tmpComponent:
                    ### newComponent.addAliasOfSubAlg(tmpComponent.alias)
                    readsubalgs.setdefault(newComponent.alias,[]).append(tmpComponent.alias)

    # this is temporary until CaloClusterCorrection-00-07-06 is in the release
    


    # this part removes all CaloRunCluster subtools
    listOfMasterCaloClusterTools = []
    for ndots in readComponents:
        for c in readComponents[ndots]:
            if c.classname == 'CaloRunClusterCorrections':
                listOfMasterCaloClusterTools += [c.alias+'.']

    # removing calocluster sub tools from global list
    for mastertool in listOfMasterCaloClusterTools:
        subtools = [st for st in globalProp.keys() if st.startswith(mastertool)]
        for c in subtools:
            globalProp.pop(c)


    for ndots in readComponents:
        for c in readComponents[ndots]:
            if True in [c.alias.startswith(t) for t in listOfMasterCaloClusterTools]:
                print "Skipping calo correction sub-tool: %s" % c.alias
                continue
            print "Adding component %s" % c.alias
            theSetup.addComponent(c)
            if c.alias in readsubalgs:
                for csa in readsubalgs[c.alias]:
                    c.addAliasOfSubAlg(csa)



                
    appMgr = AppMgrComponent(globalProp['ApplicationMgr'])
    theSetup.addComponent(appMgr)


    # setting topAlg flag
    for c in appMgr.topalg:
        print "topalgs",c
        theSetup[c].isTopAlg=1

    # setting steerAlg flag
    for c in theSetup.steeralg:
        for sa in c.subalgs: theSetup[sa].isSteerAlg = 1

    # getting the services from ApplicationMgr
    svcDict = appMgr.service

    # getting all private tools from all components
    privtools = theSetup.privtools

    propvals = theSetup.propvals
    for p in globalProp.values():
        propvals += p[1]

    print "Number of components in GLOBAL: %i" % len(globalProp.keys())

    newComponents = []
    
    for ca in globalProp.keys():
        existingComponent = None
        properties = globalProp[ca]
        try:
            existingComponent = theSetup[ca]
            #print "Found existing component %s" % ca
        except IndexError:
            # the alias is not in the setup yet
            existingComponent = None
            if ca.startswith('ToolSvc.'):
                toolname = '.'.join(ca.split('.')[1:])
                for t in privtools:
                    if t.endswith('.'+toolname):
                        existingComponent = theSetup[t]
                        #print "Found tool %s in ToolSvc which is also private tool %s " % (ca, t)
                if not existingComponent:
                    #print "Found new tool %s in ToolSvc of unknown type in GLOBAL (create component)" % toolname
                    newComponents.append(component(alias=ca, classname='unknown', properties=properties))
            else:
                #print "Found new component %s of unknown type in GLOBAL (create component)" % toolname
                newComponents.append(component(alias=ca, classname='unknown', properties=properties))
        if existingComponent:
            # here we can implement a check if the properties have changed
            updateExistingComponentWithChangedProperties(existingComponent,properties)


    for c in newComponents:
        ca = c.alias
        if ca in svcDict:
            #print "Found new component of unknown type in GLOBAL but it is listed in AppMgr:",newComponent.alias
            c.classname = svcDict[ca]
            theSetup.addComponent(c)
        elif ca.startswith('ToolSvc.'):
            toolname = '.'.join(ca.split('.')[1:])
            #print toolname
            #print "Tool %s has no type" % ca
            m = None
            for pv in propvals:
                if pv.find(toolname)!=-1:
                    m = typeRE.match(pv)
                    if m:
                        break
                    else:
                        pass
                        #print 'found toolname %s, but did not match class/instance: %s' % (toolname,pv)
            if m:
                c.classname = m.group(1)
                theSetup.addComponent(c)
                #print 'matches class/instance: %s/%s' % (m.group(1),m.group(2))
            else:
                if typeLUT.has_key(toolname) :
                    c.classname = typeLUT[toolname]
                    print '--- using LUT: %s => %s' % (toolname,c.classname)
                else:
                    #c.classname = c.alias
                    print '::WARNING::','no way to determine type of tool %s from HistorySvc output and not contained in internal LUT; use the %s' % (toolname,c.classname)
                theSetup.addComponent(c)
        else:
            #if ca != 'DataFlowConfig': # special online thing from Werner (there is no such C++ class)
            if typeLUT.has_key(ca) :
                c.classname = typeLUT[ca]
                print '--- using LUT: %s => %s' % (ca,c.classname)
            else:
                print '::WARNING::','no way to determine type of tool %s from HistorySvc output and not contained in internal LUT; use the %s' % (ca,c.classname)
            theSetup.addComponent(c)





    fout2 = open(outfilename, 'w')
    print "writing",outfilename
    fout2.write(str(theSetup))
    fout2.close()



    if errorstate=='SUCCESS' or errorstate=='WARNING':
        print "Conversion successful without errors!"
    elif errorstate=='ERROR':
        print "Conversion finished with errors!"
        
    sys.exit(0)
