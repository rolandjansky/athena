# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file TrackD3PDMaker/python/xAODTrackSummaryFiller.py
# @author scott snyder <snyder@bnl.gov>
# @date Jul 2014
# @brief Configure filler for xAOD track summary data.
#


import D3PDMakerCoreComps


HitSum        = 'HitSum'
HoleSum       = 'HoleSum'
IDHits        = 'IDHits'
IDHoles       = 'IDHoles'
IDSharedHits  = 'IDSharedHits'
IDOutliers    = 'IDOutliers'
PixelInfoPlus = 'PixelInfoPlus'
SCTInfoPlus   = 'SCTInfoPlus'
TRTInfoPlus   = 'TRTInfoPlus'
InfoPlus      = 'InfoPlus'
ExpectBLayer  = 'ExpectBLayer'
MuonHits      = 'MuonHits'
MuonHoles     = 'MuonHoles'
Outliers      = 'Outliers'
StdDev        = 'StdDev'


sumvars = [
    # ID hits
    [  IDHits,                'nBLHits  = numberOfInnermostPixelLayerHits'],
    [ [IDHits, HitSum],       'nPixHits = numberOfPixelHits'],
    [ [IDHits, HitSum],       'nSCTHits = numberOfSCTHits'],
    [ [IDHits, HitSum],       'nTRTHits = numberOfTRTHits'],
    [  IDHits,                'nTRTHighTHits = numberOfTRTHighThresholdHits'],
    [  IDHits,                'nTRTXenonHits = numberOfTRTXenonHits'],

    # ID holes + dead sensors - needed for appropriate cutting
    [ [IDHoles, HoleSum],     'nPixHoles = numberOfPixelHoles'],
    [ [IDHoles, HoleSum],     'nSCTHoles = numberOfSCTHoles'],
    [ [IDHoles, HoleSum],     'nTRTHoles = numberOfTRTHoles'],
    [  IDHoles,               'nPixelDeadSensors = numberOfPixelDeadSensors'],
    [  IDHoles,               'nSCTDeadSensors = numberOfSCTDeadSensors'],

    # ID shared & Split hits
    [  IDSharedHits,          'nBLSharedHits = numberOfInnermostPixelLayerSharedHits'],
    [  IDSharedHits,          'nPixSharedHits = numberOfPixelSharedHits'],
    [  IDSharedHits,          'nSCTSharedHits = numberOfSCTSharedHits'],
    [  IDSharedHits,          'nBLayerSplitHits = numberOfInnermostPixelLayerSplitHits'],
    [  IDSharedHits,          'nPixSplitHits = numberOfPixelSplitHits'],

    # ID outliers
    [  IDOutliers,            'nBLayerOutliers = numberOfInnermostPixelLayerOutliers'],
    [  IDOutliers,            'nPixelOutliers = numberOfPixelOutliers'],
    [  IDOutliers,            'nSCTOutliers =  numberOfSCTOutliers'],
    [  IDOutliers,            'nTRTOutliers = numberOfTRTOutliers'],
    [  IDOutliers,            'nTRTHighTOutliers = numberOfTRTHighThresholdOutliers'],

    # Pixel info plus
    [  PixelInfoPlus,         'nContribPixelLayers =numberOfContribPixelLayers'],
    [  PixelInfoPlus,         'nGangedPixels = numberOfGangedPixels'],
    [  PixelInfoPlus,         'nGangedFlaggedFakes = numberOfGangedFlaggedFakes'],
    [  PixelInfoPlus,         'nPixelSpoiltHits = numberOfPixelSpoiltHits'],

    # SCT info plus
    [  SCTInfoPlus,           'nSCTDoubleHoles = numberOfSCTDoubleHoles'],
    [  SCTInfoPlus,           'nSCTSpoiltHits = numberOfSCTSpoiltHits'],

    # TRT info plus
    [  TRTInfoPlus,           'nTRTDeadStraws = numberOfTRTDeadStraws'],
    [  TRTInfoPlus,           'nTRTTubeHits = numberOfTRTTubeHits'],

    # Info plus                                                
    [  InfoPlus,              'nOutliersOnTrack = numberOfOutliersOnTrack'],
    [  InfoPlus,              'standardDeviationOfChi2OS =standardDeviationOfChi2OS'],

    # Expect BLayer hit
    [  ExpectBLayer,          'expectInnermostPixelLayerHit = expectInnermostPixelLayerHit'],

    # Muon hits
    [ [MuonHits, HitSum],     'nprecisionLayers  = numberOfPrecisionLayers'],
    [ [MuonHits, HitSum],     'nphiLayers        = numberOfPhiLayers'],
    [ [MuonHits, HitSum],     'ntrigEtaLayers    = numberOfTriggerEtaLayers'],

    # Muon holes
    [ [MuonHoles, HoleSum],   'nprecisionHoleLayers = numberOfPrecisionHoleLayers'],
    [ [MuonHoles, HoleSum],   'nphiHoleLayers       = numberOfPhiHoleLayers'],
    [ [MuonHoles, HoleSum],   'ntrigEtaHoleLayers   = numberOfTriggerEtaHoleLayers'],

    # Other
    [ Outliers,               'nOutliersOnTrack = numberOfOutliersOnTrack'],
    [ StdDev,                 'standardDeviationOf Chi2OS'],

    ]



def xAODTrackSummaryFiller (obj, lod, blockName,
                            IDHits = True,
                            IDHoles = True,
                            IDSharedHits = True,
                            IDOutliers = False,
                            PixelInfoPlus = False,
                            SCTInfoPlus = False,
                            TRTInfoPlus = False,
                            InfoPlus = False,
                            MuonHits = True,
                            MuonHoles = False,
                            ExpectBLayer = True,
                            HitSum = True,
                            HoleSum = True,
                            FullInfo = False,
                            Outliers = False,
                            StdDev = False,
                            **kw):
    varlist = []
    for tags, v in sumvars:
        if type(tags) != type([]): tags = [tags]
        sel = sum ([locals()[t] for t in tags], FullInfo)
        if sel:
            v = v + '< unsigned char:0'
            varlist.append (v)

    obj.defineBlock (lod, blockName,
                     D3PDMakerCoreComps.AuxDataFillerTool,
                     Vars = varlist,
                     **kw)
    return

        
