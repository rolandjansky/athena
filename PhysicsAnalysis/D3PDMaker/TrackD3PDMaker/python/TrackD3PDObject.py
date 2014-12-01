# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TrackD3PDMaker
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
from TrackD3PDMaker.TrackStateOnSurfaceAssociation import TrackStateOnSurfaceAssociation
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
                    typeName='TrackCollection',
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
        typeName,
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

    if typeName=='TrackCollection':
        PerigeeAssociationTool       = TrackD3PDMaker.TrkTrackPerigeeAssociationTool
        PerigeeAtPVAssociationTool   = TrackD3PDMaker.TrkTrackPerigeeAtPVAssociationTool
        PerigeeAtBSAssociationTool   = TrackD3PDMaker.TrkTrackPerigeeAtBSAssociationTool
        FitQualityAssociationTool    = TrackD3PDMaker.TrkTrackFitQualityAssociationTool
        TrackSummaryAssociationTool  = TrackD3PDMaker.TrkTrackTrackSummaryAssociationTool
        TrackInfoAssociationTool     = TrackD3PDMaker.TrkTrackInfoAssociationTool
        HitAssociationTool           = TrackD3PDMaker.TrkTrackTSOAssociationTool
        VertexAssociationTool        = TrackD3PDMaker.TrkTrackVertexAssociationTool
        TruthAssociationTool         = TrackD3PDMaker.TrkTrackTruthAssociationTool
        DetailedTruthAssociationTool = TrackD3PDMaker.TrkTrackDetailedTruthAssociationTool
        ParametersAtBLFillerTool     = TrackD3PDMaker.TrkTrackParametersAtBLFillerTool
        
    if typeName=='Rec::TrackParticleContainer':
        PerigeeAssociationTool       = TrackD3PDMaker.TrackParticlePerigeeAtOOAssociationTool
        PerigeeAtPVAssociationTool   = TrackD3PDMaker.TrackParticlePerigeeAtPVAssociationTool
        PerigeeAtBSAssociationTool   = TrackD3PDMaker.TrackParticlePerigeeAtBSAssociationTool
        FitQualityAssociationTool    = TrackD3PDMaker.TrackParticleFitQualityAssociationTool
        TrackSummaryAssociationTool  = TrackD3PDMaker.TrackParticleTrackSummaryAssociationTool
        TrackInfoAssociationTool     = TrackD3PDMaker.TrackParticleInfoAssociationTool
        HitAssociationTool           = TrackD3PDMaker.TrackParticleTSOAssociationTool
        VertexAssociationTool        = TrackD3PDMaker.TrackParticleVertexAssociationTool
        TruthAssociationTool         = TrackD3PDMaker.TrackParticleTruthAssociationTool
        DetailedTruthAssociationTool = TrackD3PDMaker.TrackParticleDetailedTruthAssociationTool
        ParametersAtBLFillerTool     = TrackD3PDMaker.TrackParticleParametersAtBLFillerTool

    # This generates ERROR messages.  Disabled for now.
    #object.defineBlock(1, "Isolation", TrackD3PDMaker.TrackIsolationFillerTool)


    ## default perigee (at (0,0,0))
    PerigeeAssoc = PerigeeAssociation\
                   (object,
                    PerigeeAssociationTool, "GlobalPerigee",
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
                        PerigeeAtPVAssociationTool, "PerigeeAtPV",
                        suffix='_wrtPV',
                        levelName = 'trackParametersAtPrimaryVertexLevelOfDetails')

    # perigee at Beam Spot
    PerigeeAtBSAssoc = PerigeeAssociation\
                       (object,
                        PerigeeAtBSAssociationTool, "PerigeeAtBS",
                        suffix='_wrtBS', 
                        levelName = 'trackParametersAtBeamSpotLevelOfDetails')

    # perigee at Beam Line
    from TrackD3PDMaker.PerigeeAssociation import perigeeLOD
    # Not implemented in TrackToVertex.
    #object.defineBlock(perigeeLOD ('trackParametersAtBeamLineLevelOfDetails>0',
    #                               flags),
    #                   _prefix+"ParametersAtBL",
    #                   ParametersAtBLFillerTool,
    #                   levelOfDetails = deferFlag ('trackParametersAtBeamLineLevelOfDetails', flags))

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
                       FitQualityAssociationTool,
                       level = flagTestLOD('storeTrackFitQuality', flags))
        
    FitQualityAssoc.defineBlock(flagTestLOD('storeTrackFitQuality', flags),
                                _prefix+'FitQuality',
                                TrackD3PDMaker.TrackFitQualityFillerTool)

    # Track Summary
    TrackSummaryAssoc = SimpleAssociation\
                        (object,
                         TrackSummaryAssociationTool,
                         level = flagTestLOD('storeTrackSummary', flags))

    TrackSummaryAssoc.defineBlock(flagTestLOD('storeTrackSummary', flags),
                                  _prefix+'TrackSummary',
                                  TrackD3PDMaker.TrackTrackSummaryFillerTool,
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
                      TrackInfoAssociationTool,
                      level = flagTestLOD('storeTrackInfo', flags))
        
    TrackInfoAssoc.defineBlock(flagTestLOD('storeTrackInfo', flags),
                               _prefix+'TrackInfo',
                               TrackD3PDMaker.TrackInfoFillerTool)

    ## B-Layer predictions
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc, 'InDetRecTestBLayerTool'):
        BLayerPredictionAssoc = SimpleAssociation\
                                (object,
                                 PerigeeAssociationTool,
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
                                        HitAssociationTool,
                                        pref, _prefix + pref,
                                        flagTestLOD(flag, flags),
                                        fillPullsName = 'storePullsAndResiduals' if flag.find('Holes')<0 else 'False',
                                        **{opt : True})


    # Vertex association
    VertexAssoc = IndexAssociation  (
        object,
        VertexAssociationTool, vertexTarget,
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
                      TruthAssociationTool,
                      prefix = truthPrefix,
                      SGKey = SGKeyForTruth,
                      MapKey = truthMapKey,
                      level = flagTestLOD('doTruth and storeHitTruthMatching', flags))
        TruthAssoc.defineBlock (_levelAssocToGP,
                                'TruthAssocIndex',
                                D3PDMakerCoreComps.IndexFillerTool,
                                Target = truthTarget)

#        TruthAssocTP = SimpleAssociation\
#                       (TruthAssoc,
#                        TruthD3PDMaker.GenParticleTruthParticleAssociationTool,
#                        level = _levelAssocToTP)
#        TruthAssocTP.defineBlock (_levelAssocToTP,
#                                  'TruthAssocIndexTP',
#                                  D3PDMakerCoreComps.IndexFillerTool,
#                                  Target = truthTarget)

#        _levelTruthInfo = flagTestLOD ('doTruth and storeHitTruthMatching and reqlev>=2 and storeTruthInfo', flags)


#        TruthAssoc.defineBlock (_levelTruthInfo,
#                                'TruthPerigee',
#                                TruthD3PDMaker.GenParticlePerigeeFillerTool)
#
#        if typeName == 'Rec::TrackParticleContainer':
#            from TruthD3PDMaker.MCTruthClassifierConfig import \
#                 D3PDMCTruthClassifier
#            object.defineBlock \
#                   (_levelTruthInfo,
#                    'MCClassification',
#                    TrackD3PDMaker.TrackParticleMCClassifierFillerTool,
#                    prefix = 'mc_',
#                    Classifier = D3PDMCTruthClassifier)


        # Detailed truth matching
        DetailedTruthAssoc = IndexMultiAssociation (
            object,
            DetailedTruthAssociationTool, truthTarget,
            prefix = detailedTruthPrefix,
            level = flagTestLOD('doTruth and storeDetailedTruth and reqlev>=1', flags),
            SGKey = SGKeyForDetailedTruth,
            MapKey = detailedTruthMapKey)
    
#    if typeName == 'Rec::TrackParticleContainer':

#        from TrackD3PDMaker.trackIsolationTool import \
#             trackIsolationTool_500MeV, \
#             trackIsolationTool_1GeV, \
#             trackIsolationTool_2GeV, \
#             trackIsolationTool_3GeV, \
#             trackIsolationTool_4GeV, \
#             trackIsolationTool_5GeV, \
#             trackIsolationTool_3GeV_hitschi

#        from TrackD3PDMaker.addTrackIsolationCones import addTrackIsolationCones
#        addTrackIsolationCones (
#            object, 'Isolation', level=999,
#            sizes = [0.2, 0.3, 0.4],
#            caloIsoVar = 'cone%%_caloIso',
#            trackIsoVar = 'cone%%_trackIso',
#            nTrackIsoVar = 'cone%%_nTrackIso',
#            isolationTool = DeferArg('tool()',
#                                    tool=trackIsolationTool_500MeV))

#        iso4blocks = [[trackIsolationTool_1GeV, '1GeV', 'ptmin1gev_'],
#                      [trackIsolationTool_2GeV, '2GeV', 'ptmin2gev_'],
#                      [trackIsolationTool_3GeV, '3GeV', 'ptmin3gev_'],
#                      [trackIsolationTool_3GeV_hitschi, '3GeV_hitschi', 'ptmin3gev_hitschi_'],
#                      [trackIsolationTool_4GeV, '4GeV', 'ptmin4gev_'],
#                      [trackIsolationTool_5GeV, '5GeV', 'ptmin5gev_'],
#                      ]
#        for (tool, blksuff, varsuff) in iso4blocks:
#            object.defineBlock (999, 'Isolation_40_' + blksuff,
#                                TrackD3PDMaker.TrackIsolationFillerTool,
#                                ConeSize = 0.4,
#                                prefix = 'cone40_' + varsuff,
#                                TrackIsolationTool = DeferArg('tool()',
#                                                              tool=tool))

    return object

TrkTrackD3PDObject = TrackD3PDObject(_label='trkTrack',
                                     _prefix='trkTrack_',
                                     _sgkey='Tracks',
                                     _object_name='TrkTrackD3PDObject',
                                     typeName='TrackCollection',
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
                                     flags=TrackD3PDFlags)

PixelTrackD3PDObject = TrackD3PDObject(_label='trkPix',
                                       _prefix='trkPix_',
                                       _sgkey='ResolvedPixelTracks',
                                       _object_name='PixelTrackD3PDObject',
                                       typeName='TrackCollection',
                                       vertexTarget='vx',
                                       vertexPrefix='vx_',
                                       vertexSGKey='VxPrimaryCandidate',
                                       truthTarget='mc',
                                       truthPrefix='mc_',
                                       detailedTruthPrefix='detailed_mc_',
                                       truthMapKey='ResolvedPixelTracksTruthCollection',
                                       SGKeyForTruth='ResolvedPixelTracks',
                                       detailedTruthMapKey='ResolvedPixelTracksDetailedTruth',
                                       SGKeyForDetailedTruth='ResolvedPixelTracks',
                                       flags=TrackD3PDFlags)

SCTTrackD3PDObject = TrackD3PDObject(_label='trkSCT',
                                     _prefix='trkSCT_',
                                     _sgkey='ResolvedSCTTracks',
                                     _object_name='SCTTrackD3PDObject',
                                     typeName='TrackCollection',
                                     vertexTarget='vx',
                                     vertexPrefix='vx_',
                                     vertexSGKey='VxPrimaryCandidate',
                                     truthTarget='mc',
                                     truthPrefix='mc_',
                                     detailedTruthPrefix='detailed_mc_',
                                     truthMapKey='ResolvedSCTTracksTruthCollection',
                                     SGKeyForTruth='ResolvedSCTTracks',
                                     detailedTruthMapKey='ResolvedSCTTracksDetailedTruth',
                                     SGKeyForDetailedTruth='ResolvedSCTTracks',
                                     flags=TrackD3PDFlags)

TRTTrackD3PDObject = TrackD3PDObject(_label='trkTRT',
                                     _prefix='trkTRT_',
                                     _sgkey='StandaloneTRTTracks',
                                     _object_name='TRTTrackD3PDObject',
                                     typeName='TrackCollection',
                                     vertexTarget='vx',
                                     vertexPrefix='vx_',
                                     vertexSGKey='VxPrimaryCandidate',
                                     truthTarget='mc',
                                     truthPrefix='mc_',
                                     detailedTruthPrefix='detailed_mc_',
                                     truthMapKey='StandaloneTRTTracks',
                                     SGKeyForTruth='StandaloneTRTTracks',
                                     detailedTruthMapKey='StandaloneTRTTracksDetailedTruth',
                                     SGKeyForDetailedTruth='StandaloneTRTTracks',
                                     flags=TrackD3PDFlags)

TrackParticleD3PDObject = TrackD3PDObject(_label='trk',
                                          _prefix='trk_',
                                          _sgkey=D3PDMakerFlags.TrackSGKey(),
                                          _object_name='TrackParticleD3PDObject',
                                          typeName='Rec::TrackParticleContainer',
                                          vertexTarget='vx',
                                          vertexPrefix='vx_',
                                          vertexSGKey='VxPrimaryCandidate',
                                          truthTarget='mc',
                                          truthPrefix='mc_',
                                          detailedTruthPrefix='detailed_mc_',
                                          truthMapKey='TrackParticleTruthCollection',
                                          SGKeyForTruth=D3PDMakerFlags.TrackSGKey(),
                                          detailedTruthMapKey='DetailedTrackTruth',
                                          SGKeyForDetailedTruth='Tracks',
                                          flags=TrackD3PDFlags)

ResolvedTracksD3PDObject = TrackD3PDObject(_label='trkResolvedTrack',
                                           _prefix='trkResolvedTrack_',
                                           _sgkey='ResolvedTracks',
                                           _object_name='ResolvedTracksD3PDObject',
                                           typeName='TrackCollection',
                                           vertexTarget='vx',
                                           vertexPrefix='vx_',
                                           vertexSGKey='VxPrimaryCandidate',
                                           truthTarget='mc',
                                           truthPrefix='mc_',
                                           detailedTruthPrefix='detailed_mc_',
                                           truthMapKey='ResolvedTracksTruthCollection',
                                           SGKeyForTruth='ResolvedTracks',
                                           detailedTruthMapKey='ResolvedTracksDetailedTruth',
                                           SGKeyForDetailedTruth='ResolvedTracks',
                                           flags=TrackD3PDFlags)

GSFTrackParticleD3PDObject = TrackD3PDObject(_label='trk',
                                             _prefix='trk_',
                                             _sgkey=D3PDMakerFlags.TrackSGKey(),
                                             _object_name='GSFTrackParticleD3PDObject',
                                             typeName='Rec::TrackParticleContainer',
                                             vertexTarget='vx',
                                             vertexPrefix='vx_',
                                             vertexSGKey='VxPrimaryCandidate',
                                             truthTarget='mc',
                                             truthPrefix='mc_',
                                             detailedTruthPrefix='detailed_mc_',
                                             truthMapKey='GSFTrackParticleTruthCollection',
                                             SGKeyForTruth="GSFTrackParticleCandidate",
                                             detailedTruthMapKey='DetailedTrackTruth',
                                             SGKeyForDetailedTruth='Tracks',
                                             flags=TrackD3PDFlags) 
