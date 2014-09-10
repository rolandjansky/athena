# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TrackD3PDMaker
from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
import D3PDMakerCoreComps
from D3PDMakerCoreComps.flagTestLOD import deferFlag
from D3PDMakerCoreComps.SimpleAssociation import SimpleAssociation
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation

def _get_res():
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc, 'TrackD3PDResidualPullCalculator'):
        return ToolSvc.TrackD3PDResidualPullCalculator

    from TrkResidualPullCalculator.TrkResidualPullCalculatorConf import Trk__ResidualPullCalculator
    print 'xxx', Trk__ResidualPullCalculator
    res = Trk__ResidualPullCalculator( name = "TrackD3PDResidualPullCalculator")
    ToolSvc += res
    return res


def TrackStateOnSurfaceAssociation (parent, associator, _prefix = '',
                                    _blocknameprefix = '', _level = 0,
                                    getBLayerMeasurements = False,
                                    getPixelMeasurements = False,
                                    getSCTMeasurements = False,
                                    getTRTMeasurements = False,
                                    getMDTMeasurements = False,
                                    getCSCMeasurements = False,
                                    getRPCMeasurements = False,
                                    getTGCMeasurements = False,
                                    getBLayerOutliers = False,
                                    getPixelOutliers = False,
                                    getSCTOutliers = False,
                                    getTRTOutliers = False,
                                    getMDTOutliers = False,
                                    getCSCOutliers = False,
                                    getRPCOutliers = False,
                                    getTGCOutliers = False,
                                    getBLayerHoles = False,
                                    getPixelHoles = False,
                                    getSCTHoles = False,
                                    getTRTHoles = False,
                                    getMDTHoles = False,
                                    getCSCHoles = False,
                                    getRPCHoles = False,
                                    getTGCHoles = False,
                                    fillPullsName = 'False',
                                    flags = TrackD3PDFlags):

    assoc = ContainedVectorMultiAssociation\
            (parent, associator,
             prefix = _prefix, level = _level,
             blockname = _blocknameprefix+'HitsOnTrackAssoc',
             GetBLayerMeasurements = getBLayerMeasurements,
             GetPixelMeasurements = getPixelMeasurements,
             GetSCTMeasurements = getSCTMeasurements,
             GetTRTMeasurements = getTRTMeasurements,
             GetMDTMeasurements = getMDTMeasurements,
             GetCSCMeasurements = getCSCMeasurements,
             GetRPCMeasurements = getRPCMeasurements,
             GetTGCMeasurements = getTGCMeasurements,
             GetBLayerOutliers = getBLayerOutliers,
             GetPixelOutliers = getPixelOutliers,
             GetSCTOutliers = getSCTOutliers,
             GetTRTOutliers = getTRTOutliers,
             GetMDTOutliers = getMDTOutliers,
             GetCSCOutliers = getCSCOutliers,
             GetRPCOutliers = getRPCOutliers,
             GetTGCOutliers = getTGCOutliers,
             GetBLayerHoles = getBLayerHoles,
             GetPixelHoles = getPixelHoles,
             GetSCTHoles = getSCTHoles,
             GetTRTHoles = getTRTHoles,
             GetMDTHoles = getMDTHoles,
             GetCSCHoles = getCSCHoles,
             GetRPCHoles = getRPCHoles,
             GetTGCHoles = getTGCHoles)


    assoc.defineBlock(0, _blocknameprefix+'HitsOnTrack',
                      TrackD3PDMaker.TrackStateOnSurfaceFillerTool,
                      FillPixelHits = getPixelMeasurements or getPixelOutliers or getBLayerMeasurements or getBLayerOutliers,
                      FillSCTHits = getSCTMeasurements or getSCTOutliers,
                      FillTRTHits = getTRTMeasurements or getTRTOutliers,
                      FillMDTHits = getMDTMeasurements or getMDTOutliers,
                      FillCSCHits = getCSCMeasurements or getCSCOutliers,
                      FillRPCHits = getRPCMeasurements or getRPCOutliers,
                      FillTGCHits = getTGCMeasurements or getTGCOutliers,
                      FillPixelHoles = getPixelHoles or getBLayerHoles,
                      FillSCTHoles = getSCTHoles,
                      FillTRTHoles = getTRTHoles,
                      FillMDTHoles = getMDTHoles,
                      FillCSCHoles = getCSCHoles,
                      FillRPCHoles = getRPCHoles,
                      FillTGCHoles = getTGCHoles,
                      FillPulls = deferFlag (fillPullsName, flags),
                      ResidualPullCalculator = deferFlag ('_get_res() if %s else None'%
                                                          fillPullsName,
                                                          flags,
                                                          {'_get_res':_get_res}))

    ## TODO - add Muon holes
    if not(getPixelHoles or getBLayerHoles or getSCTHoles or getTRTHoles):
        fqassoc = SimpleAssociation\
                  (assoc,
                   TrackD3PDMaker.TSOFitQualityAssociationTool,
                   prefix = '', level = 0, blockname = _blocknameprefix+'HitsOnTrack_FitQualityAssoc')

        fqassoc.defineBlock(0, _blocknameprefix+'HitsOnTrack_FitQuality',
                            TrackD3PDMaker.TrackFitQualityFillerTool)
    
    return assoc
