# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import TrackD3PDMaker
import InDetD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
#import TruthD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerCoreComps.D3PDObject import DeferArg
from D3PDMakerCoreComps.flagTestLOD import flagTestLOD, deferFlag
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.IndexAssociation import IndexAssociation
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from TrackD3PDMaker import TrackD3PDMakerConf
from TrackD3PDMaker.PerigeeAssociation import PerigeeAssociation
from InDetD3PDMaker.TrackStateOnSurfaceAssociation import TrackStateOnSurfaceAssociation
from RecExConfig.RecFlags import rec
from TrackD3PDMaker.TrackD3PDMakerFlags import *
from AthenaCommon.AppMgr import ToolSvc


def _get_estimator (args, hookargs):
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc, 'AtlasExtrapolator'):
        AtlasExtrapolator = ToolSvc.AtlasExtrapolator
    else:
        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        AtlasExtrapolator = AtlasExtrapolator()
        ToolSvc += AtlasExtrapolator
    if hasattr(ToolSvc, 'TrackD3PDLinTrkFactory'):
        TrackD3PDLinTrkFactory = ToolSvc.TrackD3PDLinTrkFactory
    else:
        from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
        TrackD3PDLinTrkFactory = Trk__FullLinearizedTrackFactory(
            name              = "TrackD3PDFullLinearizedTrackFactory",
            Extrapolator      = AtlasExtrapolator)
        ToolSvc += TrackD3PDLinTrkFactory

    if hasattr(ToolSvc, 'TrackD3PDTrackToVertexIPEstimator'):
        TrackD3PDTrackToVertexIPEstimator = ToolSvc.TrackD3PDTrackToVertexIPEstimator
    else:
        from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator
        TrackD3PDTrackToVertexIPEstimator = Trk__TrackToVertexIPEstimator(
            name="TrackD3PDTrackToVertexIPEstimator",
            Extrapolator=AtlasExtrapolator,
            LinearizedTrackFactory=TrackD3PDLinTrkFactory)
        ToolSvc += TrackD3PDTrackToVertexIPEstimator

    args['TrackToVertexIPEstimator'] = TrackD3PDTrackToVertexIPEstimator
    return


def TrackD3PDObject(_label='trkTrack',
                    _prefix='trkTrack_',
                    _sgkey='Tracks',
                    _object_name='TrackD3PDObject',
                    typ=TrackParticleType,
                    vertexTarget='vx',
                    vertexPrefix='vx_',
                    vertexSGKey='VxPrimaryCandidate',
                    truthTarget='mc',
                    truthPrefix='mc_',
                    detailedTruthPrefix='detailed_mc_',
                    truthMapKey='TrackTruthCollection',
                    SGKeyForTruth='Tracks',
                    detailedTruthMapKey='DetailedTrackTruth',
                    SGKeyForDetailedTruth='Tracks',
                    flags=TrackD3PDFlags):

    object = make_SGDataVector_D3PDObject (
        typ.typeName,
        _sgkey,
        _prefix,
        _object_name,
        default_allowMissing = True,
        default_label = _label,
        allow_args = ['GenParticleTarget',
                      'TruthParticleTarget',
                      'storeTruthInfo',
                      'storeTrackMomentum',
                      'trackParametersAtGlobalPerigeeLevelOfDetails',
                      'trackParametersAtPrimaryVertexLevelOfDetails',
                      'trackParametersAtBeamSpotLevelOfDetails',
                      'trackParameterAtBeamLineLevelOfDetails',
                      'storeDiagonalCovarianceAsErrors',
                      'storeTrackParametersAtCalo',
                      'storeTrackParametersAtCalo2ndLayer',
                      'storeTrackFitQuality',
                      'storeTrackPredictionAtBLayer',
                      'storeTrackInfo',
                      'storeVertexAssociation',
                      'storeDetailedTruth',
                      'storeBLayerHitsOnTrack',
                      'storePixelHitsOnTrack',
                      'storeSCTHitsOnTrack',
                      'storeTRTHitsOnTrack',
                      'storeMDTHitsOnTrack',
                      'storeCSCHitsOnTrack',
                      'storeRPCHitsOnTrack',
                      'storeTGCHitsOnTrack',
                      'storeBLayerOutliersOnTrack',
                      'storePixelOutliersOnTrack',
                      'storeSCTOutliersOnTrack',
                      'storeTRTOutliersOnTrack',
                      'storeMDTOutliersOnTrack',
                      'storeCSCOutliersOnTrack',
                      'storeRPCOutliersOnTrack',
                      'storeTGCOutliersOnTrack',
                      'storeBLayerHolesOnTrack',
                      'storePixelHolesOnTrack',
                      'storeSCTHolesOnTrack',
                      'storeTRTHolesOnTrack',
                      'storeMDTHolesOnTrack',
                      'storeCSCHolesOnTrack',
                      'storeRPCHolesOnTrack',
                      'storeTGCHolesOnTrack',
                      'storePullsAndResiduals',
                      'storeTrackUnbiasedIPAtPV',
                      'storeHitTruthMatching',
                      'storeTrackSummary',
                      'doTruth',
                      ])

    # This generates ERROR messages.  Disabled for now.
    #object.defineBlock(1, "Isolation", TrackD3PDMaker.TrackIsolationFillerTool)


    ## default perigee (at (0,0,0))
    PerigeeAssoc = PerigeeAssociation\
                   (object,
                    typ.PerigeeAssociationTool, "GlobalPerigee",
                    fillMomName = 'storeTrackMomentum',
                    levelName = 'trackParametersAtGlobalPerigeeLevelOfDetails')

    # Unbiased impact parameters at PV
    PerigeeAssoc.defineBlock (flagTestLOD('storeTrackUnbiasedIPAtPV',
                                          flags, _get_estimator),
                              _prefix+"IPEstimate",
                              TrackD3PDMaker.PerigeeUnbiasedIPAtPVFillerTool,
                              # Filled in by LOD function.
                              TrackToVertexIPEstimator=None,
                              Prefix = 'IPEstimate_',
                              Suffix = '_wrtPV')

    # perigee at Primary Vertex
    PerigeeAtPVAssoc = PerigeeAssociation\
                       (object,
                        typ.PerigeeAtPVAssociationTool, "PerigeeAtPV",
                        suffix='_wrtPV',
                        levelName = 'trackParametersAtPrimaryVertexLevelOfDetails')

    # perigee at Beam Spot
    PerigeeAtBSAssoc = PerigeeAssociation\
                       (object,
                        typ.PerigeeAtBSAssociationTool, "PerigeeAtBS",
                        suffix='_wrtBS', 
                        levelName = 'trackParametersAtBeamSpotLevelOfDetails')

    # parameters at Calo
    object.defineBlock(flagTestLOD('storeTrackParametersAtCalo', flags),
                       _prefix+"ParametersAtCalo",
                       ParametersAtCaloFillerTool)

    # parameters at Calo 2nd layer
    object.defineBlock(flagTestLOD('storeTrackParametersAtCalo2ndLayer', flags),
                       _prefix+"ParametersAtCalo2ndLayer",
                       ParametersAtCaloFillerTool,
                       Sampling = 1,
                       Suffix = '2ndLayer')

    # Fit Quality
    FitQualityAssoc = SimpleAssociation\
                      (object,
                       typ.FitQualityAssociationTool,
                       level = flagTestLOD('storeTrackFitQuality', flags))
        
    FitQualityAssoc.defineBlock(flagTestLOD('storeTrackFitQuality', flags),
                                _prefix+'FitQuality',
                                InDetD3PDMaker.TrackFitQualityFillerTool)

    # Track Summary
    TrackSummaryAssoc = SimpleAssociation\
                        (object,
                         typ.TrackSummaryAssociationTool,
                         level = flagTestLOD('storeTrackSummary', flags))

    TrackSummaryAssoc.defineBlock(flagTestLOD('storeTrackSummary', flags),
                                  _prefix+'TrackSummary',
                                  InDetD3PDMaker.TrackTrackSummaryFillerTool,
                                  FullInfo = flags.storeTrackSummary.FullInfo,
                                  IDHits = flags.storeTrackSummary.IDHits,
                                  IDHoles = flags.storeTrackSummary.IDHoles,
                                  IDSharedHits = flags.storeTrackSummary.IDSharedHits,
                                  IDOutliers = flags.storeTrackSummary.IDOutliers,
                                  PixelInfoPlus = flags.storeTrackSummary.PixelInfoPlus,
                                  SCTInfoPlus = flags.storeTrackSummary.SCTInfoPlus,
                                  TRTInfoPlus = flags.storeTrackSummary.TRTInfoPlus,
                                  InfoPlus = flags.storeTrackSummary.InfoPlus,
                                  MuonHits = flags.storeTrackSummary.MuonHits,
                                  MuonHoles = flags.storeTrackSummary.MuonHoles,
                                  ExpectBLayer = flags.storeTrackSummary.ExpectBLayer,
                                  HitSum = flags.storeTrackSummary.HitSum,
                                  HoleSum = flags.storeTrackSummary.HoleSum,
                                  HitPattern = flags.storeTrackSummary.HitPattern,
                                  SiHits = flags.storeTrackSummary.SiHits,
                                  TRTRatio = flags.storeTrackSummary.TRTRatio,
                                  PixeldEdx = flags.storeTrackSummary.PixeldEdx,
                                  ElectronPID = flags.storeTrackSummary.ElectronPID)
        
    # Track Info
    TrackInfoAssoc = SimpleAssociation\
                     (object,
                      typ.TrackInfoAssociationTool,
                      level = flagTestLOD('storeTrackInfo', flags))
        
    TrackInfoAssoc.defineBlock(flagTestLOD('storeTrackInfo', flags),
                               _prefix+'TrackInfo',
                               InDetD3PDMaker.TrackInfoFillerTool)

    ## B-Layer predictions
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc, 'InDetRecTestBLayerTool'):
        BLayerPredictionAssoc = SimpleAssociation\
                                (object,
                                 typ.PerigeeAssociationTool,
                                 prefix = 'blayerPrediction_',
                                 blockname = _prefix+'BLayerInfoAssoc',
                                 level = flagTestLOD('storeTrackPredictionAtBLayer', flags))

        BLayerPredictionAssoc.defineBlock (flagTestLOD('storeTrackPredictionAtBLayer', flags),
                                           _prefix+'BLayerInfo',
                                           TrackD3PDMaker.PerigeeBLPredictionFillerTool,
                                           InDetTestBLayerTool = ToolSvc.InDetRecTestBLayerTool)

    tsos_table = [
        ('storeBLayerHitsOnTrack', 'BLayer_hit_', 'getBLayerMeasurements'),
        ('storePixelHitsOnTrack',  'Pixel_hit_',  'getPixelMeasurements'),
        ('storeSCTHitsOnTrack',    'SCT_hit_',    'getSCTMeasurements'),
        ('storeTRTHitsOnTrack',    'TRT_hit_',    'getTRTMeasurements'),
        ('storeMDTHitsOnTrack',    'MDT_hit_',    'getMDTMeasurements'),
        ('storeCSCHitsOnTrack',    'CSC_hit_',    'getCSCMeasurements'),
        ('storeRPCHitsOnTrack',    'RPC_hit_',    'getRPCMeasurements'),
        ('storeTGCHitsOnTrack',    'TGC_hit_',    'getTGCMeasurements'),

        ('storeBLayerOutliersOnTrack', 'BLayer_outlier_', 'getBLayerOutliers'),
        ('storePixelOutliersOnTrack',  'Pixel_outlier_',  'getPixelOutliers'),
        ('storeSCTOutliersOnTrack',    'SCT_outlier_',    'getSCTOutliers'),
        ('storeTRTOutliersOnTrack',    'TRT_outlier_',    'getTRTOutliers'),
        ('storeMDTOutliersOnTrack',    'MDT_outlier_',    'getMDTOutliers'),
        ('storeCSCOutliersOnTrack',    'CSC_outlier_',    'getCSCOutliers'),
        ('storeRPCOutliersOnTrack',    'RPC_outlier_',    'getRPCOutliers'),
        ('storeTGCOutliersOnTrack',    'TGC_outlier_',    'getTGCOutliers'),

        ('storeBLayerHolesOnTrack',    'BLayer_hole_',    'getBLayerHoles'),
        ('storePixelHolesOnTrack',     'Pixel_hole_',     'getPixelHoles'),
        ('storeSCTHolesOnTrack',       'SCT_hole_',       'getSCTHoles'),
        ('storeTRTHolesOnTrack',       'TRT_hole_',       'getTRTHoles'),
        ('storeMDTHolesOnTrack',       'MDT_hole_',       'getMDTHoles'),
        ('storeCSCHolesOnTrack',       'CSC_hole_',       'getCSCHoles'),
        ('storeRPCHolesOnTrack',       'RPC_hole_',       'getRPCHoles'),
        ('storeTGCHolesOnTrack',       'TGC_hole_',       'getTGCHoles'),
        ]

    for flag, pref, opt in tsos_table:
        TrackStateOnSurfaceAssociation (object,
                                        typ.HitAssociationTool,
                                        pref, _prefix + pref,
                                        flagTestLOD(flag, flags),
                                        fillPullsName = 'storePullsAndResiduals' if flag.find('Holes')<0 else 'False',
                                        **{opt : True})


    # Vertex association
    VertexAssoc = IndexAssociation  (
        object,
        typ.VertexAssociationTool, vertexTarget,
        prefix = vertexPrefix,
        VxSGKey = vertexSGKey,
        level = flagTestLOD('storeVertexAssociation', flags))

    # Truth matching
    if rec.doTruth():
        # Simple truth matching

        # Allow associating to either GenParticles or TruthParticles.
        def _levelAssocToGP (reqlev, args, hookargs):
            if reqlev < 1: return False
            if hookargs.get ('TruthParticleTarget'): return False
            if hookargs.get ('GenParticleTarget'):
                args['Target'] = hookargs.get ('GenParticleTarget')
            return True
        def _levelAssocToTP (reqlev, args, hookargs):
            if reqlev < 1: return False
            tpt = hookargs.get ('TruthParticleTarget')
            if not tpt: return False
            args['Target'] = tpt
            return True

        TruthAssoc = SimpleAssociation\
                     (object,
                      typ.TruthAssociationTool,
                      prefix = truthPrefix,
                      SGKey = SGKeyForTruth,
                      MapKey = truthMapKey,
                      level = flagTestLOD('doTruth and storeHitTruthMatching', flags))
        TruthAssoc.defineBlock (_levelAssocToGP,
                                'TruthAssocIndex',
                                D3PDMakerCoreComps.IndexFillerTool,
                                Target = truthTarget)


        # Detailed truth matching
        DetailedTruthAssoc = IndexMultiAssociation (
            object,
            typ.DetailedTruthAssociationTool, truthTarget,
            prefix = detailedTruthPrefix,
            level = flagTestLOD('doTruth and storeDetailedTruth and reqlev>=1', flags),
            SGKey = SGKeyForDetailedTruth,
            MapKey = detailedTruthMapKey)
    
    return object

