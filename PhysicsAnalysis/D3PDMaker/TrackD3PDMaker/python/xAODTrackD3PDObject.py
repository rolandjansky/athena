# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import TrackD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.flagTestLOD import flagTestLOD
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.IndexAssociation import IndexAssociation
from TrackD3PDMaker.PerigeeAssociation import PerigeeAssociation
from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
from AthenaCommon.AppMgr import ToolSvc


def xAODTrackD3PDObject(_label='trkTrack',
                        _prefix='trkTrack_',
                        _sgkey='TrackParticleCandidate',
                        _object_name='TrackD3PDObject',
                        vertexTarget='vx',
                        vertexPrefix='vx_',
                        vertexSGKey='VxPrimaryCandidate',
                        flags=TrackD3PDFlags):

    object = make_SGDataVector_D3PDObject (
        'xAOD::TrackParticleContainer',
        _sgkey,
        _prefix,
        _object_name,
        default_allowMissing = True,
        default_label = _label,
        allow_args = ['storeTrackMomentum',
                      'trackParametersAtGlobalPerigeeLevelOfDetails',
                      'trackParametersAtPrimaryVertexLevelOfDetails',
                      'trackParametersAtBeamSpotLevelOfDetails',
                      'storeDiagonalCovarianceAsErrors',
                      'storeTrackFitQuality',
                      'storeTrackPredictionAtBLayer',
                      'storeTrackInfo',
                      'storeVertexAssociation',
                      'storeTrackSummary',
                      ])

    ## default perigee (at (0,0,0))
    PerigeeAssoc = PerigeeAssociation\
                   (object,
                    TrackD3PDMaker.TrackParticlePerigeeAtOOAssociationTool,
                    "GlobalPerigee",
                    fillMomName = 'storeTrackMomentum',
                    levelName = 'trackParametersAtGlobalPerigeeLevelOfDetails')

    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc, 'InDetRecTestBLayerTool'):
        PerigeeAssoc.defineBlock (flagTestLOD('storeTrackPredictionAtBLayer', flags),
                                  _prefix+'BLayerInfo',
                                  TrackD3PDMaker.PerigeeBLPredictionFillerTool,
                                  InDetTestBLayerTool = ToolSvc.InDetRecTestBLayerTool)

    # perigee at Primary Vertex
    PerigeeAtPVAssoc = PerigeeAssociation\
                       (object,
                        TrackD3PDMaker.TrackParticlePerigeeAtPVAssociationTool,
                        "PerigeeAtPV",
                        suffix='_wrtPV',
                        levelName = 'trackParametersAtPrimaryVertexLevelOfDetails')

    # perigee at Beam Spot
    PerigeeAtBSAssoc = PerigeeAssociation\
                       (object,
                        TrackD3PDMaker.TrackParticlePerigeeAtBSAssociationTool,
                        "PerigeeAtBS",
                        suffix='_wrtBS', 
                        levelName = 'trackParametersAtBeamSpotLevelOfDetails')

    object.defineBlock(flagTestLOD('storeTrackFitQuality', flags),
                       _prefix+'FitQuality',
                        D3PDMakerCoreComps.AuxDataFillerTool,
                        Vars = ['chiSquared',
                                'numberDoF'])

    # Track Summary
    class SumVars:
        HitSum         = 1 << 0
        HoleSum        = 1 << 1
        IDHits         = 1 << 2
        IDHoles        = 1 << 3
        IDSharedHits   = 1 << 4
        IDOutliers     = 1 << 5
        PixelInfoPlus  = 1 << 6
        SCTInfoPlus    = 1 << 7
        TRTInfoPlus    = 1 << 8
        InfoPlus       = 1 << 9
        ExpectBLayer   = 1 << 10
        MuonHits       = 1 << 11
        DBMHits        = 1 << 12


        varsTable = [
            # ID hits
            [IDHits,          'nBLHits',        'numberOfInnermostPixelLayerHits'],
            [IDHits + HitSum, 'nPixHits',       'numberOfPixelHits' ],
            [IDHits + HitSum, 'nSCTHits',       'numberOfSCTHits' ],
            [IDHits + HitSum, 'nTRTHits',       'numberOfTRTHits' ],
            [IDHits,          'nTRTHighTHits',  'numberOfTRTHighThresholdHits'],
            [IDHits,          'nTRTXenonHits',  'numberOfTRTXenonHits'],

            # ID holes + dead sensors - needed for appropriate cutting
            [IDHoles + HoleSum, 'nPixHoles',         'numberOfPixelHoles' ],
            [IDHoles + HoleSum, 'nSCTHoles',         'numberOfSCTHoles' ],
            [IDHoles + HoleSum, 'nTRTHoles',         'numberOfTRTHoles' ],
            [IDHoles,           'nPixelDeadSensors', 'numberOfPixelDeadSensors' ],
            [IDHoles,           'nSCTDeadSensors',   'numberOfSCTDeadSensors' ],

            # ID shared & Split hits
            [IDSharedHits,      'nBLSharedHits',     'numberOfInnermostPixelLayerSharedHits'],
            [IDSharedHits,      'nPixSharedHits',    'numberOfPixelSharedHits'],
            [IDSharedHits,      'nSCTSharedHits',    'numberOfSCTSharedHits'],
            [IDSharedHits,      'nBLayerSplitHits',  'numberOfInnermostPixelLayerSplitHits'],
            [IDSharedHits,      'nPixSplitHits',     'numberOfPixelSplitHits'],

            # ID outliers                                              
            [IDOutliers,        'nBLayerOutliers',   'numberOfInnermostPixelLayerOutliers' ],
            [IDOutliers,        'nPixelOutliers',    'numberOfPixelOutliers' ],
            [IDOutliers,        'nSCTOutliers',      'numberOfSCTOutliers' ],
            [IDOutliers,        'nTRTOutliers',      'numberOfTRTOutliers'],
            [IDOutliers,        'nTRTHighTOutliers', 'numberOfTRTHighThresholdOutliers'],

            # Pixel info plus                                          
            [PixelInfoPlus,     'nContribPixelLayers', 'numberOfContribPixelLayers' ],
            [PixelInfoPlus,     'nGangedPixels',       'numberOfGangedPixels' ],
            [PixelInfoPlus,     'nGangedFlaggedFakes', 'numberOfGangedFlaggedFakes' ],
            [PixelInfoPlus,     'nPixelSpoiltHits',    'numberOfPixelSpoiltHits' ],

            # SCT info plus                                            
            [SCTInfoPlus,       'nSCTDoubleHoles',     'numberOfSCTDoubleHoles' ],
            [SCTInfoPlus,       'nSCTSpoiltHits',      'numberOfSCTSpoiltHits' ],

            # TRT info plus                                            
            [TRTInfoPlus,       'nTRTDeadStraws',      'numberOfTRTDeadStraws' ],
            [TRTInfoPlus,       'nTRTTubeHits',        'numberOfTRTTubeHits' ],
            [TRTInfoPlus,       'nTRTSharedHits',      'numberOfTRTSharedHits' ],
            [TRTInfoPlus,       'nTRTHTHitsTotal',     'numberOfTRTHighThresholdHitsTotal' ],

            # Info plus                                                
            [InfoPlus,          'nOutliersOnTrack',           'numberOfOutliersOnTrack'],
            [InfoPlus,          'standardDeviationOfChi2OS',],

            # Expect BLayer hit                                        
            [ExpectBLayer,      'expectInnermostPixelLayerHit'],

            # Muon hits
            [MuonHits + HitSum, 'numberOfPrecisionLayers'],
            [MuonHits + HitSum, 'numberOfPrecisionHoleLayers'],
            [MuonHits + HitSum, 'numberOfPhiLayers'],
            [MuonHits + HitSum, 'numberOfPhiHoleLayers'],
            [MuonHits + HitSum, 'numberOfTriggerEtaLayers'],
            [MuonHits + HitSum, 'numberOfTriggerEtaHoleLayers'],
        ]


        @classmethod
        def varlist (cls,
                     FullInfo,
                     IDHits,
                     IDHoles,
                     IDSharedHits,
                     IDOutliers,
                     PixelInfoPlus,
                     SCTInfoPlus,
                     TRTInfoPlus,
                     InfoPlus,
                     ExpectBLayer,
                     MuonHits,
                     DBMHits,
                     HitSum,
                     HoleSum,
                     ElectronPID,
                     PixeldEdx):
            ret = []
            mask = 0
            if FullInfo: mask = ~0
            if IDHits:        mask |= cls.IDHits
            if IDHoles:       mask |= cls.IDHoles
            if IDSharedHits:  mask |= cls.IDSharedHits
            if IDOutliers:    mask |= cls.IDOutliers
            if PixelInfoPlus: mask |= cls.PixelInfoPlus
            if SCTInfoPlus:   mask |= cls.SCTInfoPlus
            if TRTInfoPlus:   mask |= cls.TRTInfoPlus
            if InfoPlus:      mask |= cls.InfoPlus
            if ExpectBLayer:  mask |= cls.ExpectBLayer
            if MuonHits:      mask |= cls.MuonHits
            if DBMHits:       mask |= cls.DBMHits
            if HitSum:        mask |= cls.HitSum
            if HoleSum:       mask |= cls.HoleSum

            for v in cls.varsTable:
                if (v[0] & mask) != 0:
                    var = v[1]
                    enum = v[2] if len(v) >= 3 else var
                    ret.append (var + ' = ' + enum)

            if ElectronPID or FullInfo:
                ret.append ('eProbabilityComb')
                ret.append ('eProbabilityHT')

            if PixeldEdx or FullInfo:
                ret.append ('pixeldEdx')
                ret.append ('numberOfUsedHitsdEdx')
                ret.append ('numberOfIBLOverflowsdEdx')
            return ret
            

    sumvarlist = SumVars.varlist (
        FullInfo = flags.storeTrackSummary.FullInfo,
        IDHits = flags.storeTrackSummary.IDHits,
        IDHoles = flags.storeTrackSummary.IDHoles,
        IDSharedHits = flags.storeTrackSummary.IDSharedHits,
        IDOutliers = flags.storeTrackSummary.IDOutliers,
        PixelInfoPlus = flags.storeTrackSummary.PixelInfoPlus,
        SCTInfoPlus = flags.storeTrackSummary.SCTInfoPlus,
        TRTInfoPlus = flags.storeTrackSummary.TRTInfoPlus,
        InfoPlus = flags.storeTrackSummary.InfoPlus,
        ExpectBLayer = flags.storeTrackSummary.ExpectBLayer,
        MuonHits = flags.storeTrackSummary.MuonHits,
        DBMHits = flags.storeTrackSummary.DBMHits,
        HitSum = flags.storeTrackSummary.HitSum,
        HoleSum = flags.storeTrackSummary.HoleSum,
        ElectronPID = flags.storeTrackSummary.ElectronPID,
        PixeldEdx = flags.storeTrackSummary.PixeldEdx,
        )

    object.defineBlock (flagTestLOD('storeTrackSummary', flags),
                        _prefix + 'TrackSummary',
                        D3PDMakerCoreComps.AuxDataFillerTool,
                        Vars = sumvarlist)


    # Track Info
    object.defineBlock (flagTestLOD('storeTrackInfo', flags),
                        _prefix + 'TrackInfo',
                        D3PDMakerCoreComps.AuxDataFillerTool,
                        Vars = ['trackFitter',
                                'particleHypothesis',
                                'trackProperties',
                                'patternRecoInfo'])

    # Vertex association
    VertexAssoc = IndexAssociation  (
        object,
        TrackD3PDMaker.TrackParticleVertexAssociationTool,
        vertexTarget,
        prefix = vertexPrefix,
        VxSGKey = vertexSGKey,
        level = flagTestLOD('storeVertexAssociation', flags))

    return object


xAODTrackParticleD3PDObject = xAODTrackD3PDObject()
