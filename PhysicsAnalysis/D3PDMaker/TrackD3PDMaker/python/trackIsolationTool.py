# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: trackIsolationTool.py 542462 2013-03-26 04:51:52Z ssnyder $
#
# @file TrackD3PDMaker/python/trackIsolationTool.py
# @author scott snyder <snyder@bnl.gov>
# @date Mar, 2013
# @brief Helpers for setting up TrackIsolationTool.
#


from RecExConfig.RecFlags import rec 
from TrackIsolationTools.TrackIsolationToolsConf import  TrackIsolationTool
from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
from AthenaCommon.SystemOfUnits import MeV, GeV, mm
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.AppMgr import ServiceMgr


from AthenaCommon.Include import include
include ( "TrackInCaloTools/TrackInCaloTools_jobOptions.py" )


# Create an initialized TrackIsolationTool.
def trackIsolationTool (name,
                        doTrackIsolation = False,
                        doSumConversionTracks = True,
                        trackParticleCollection = 'TrackParticleCandidate',
                        conversionContainer = 'ConversionCandidate',
                        calorimeterCellContainer = None,
                        onlyEM = False,
                        coreDeltaR = 0.05,
                        trackToVertexTool = None,
                        trackSummaryTool = None,
                        extrapolator = None,

                        trackSelector = None,
                        pTMin                = 1000*MeV,
                        IPd0Max              = 10.0*mm,
                        IPz0Max              = 10.0*mm,
                        z0Max                = 10.0*mm,
                        useTrackSummaryInfo  = True,
                        nHitBLayer           = 0,
                        nHitPix              = 0,
                        nHitBLayerPlusPix    = 0,
                        nHitSct              = 0,
                        nHitSi               = 4,
                        nHitTrt              = 0,
                        useTrackQualityInfo  = False,
                        **kw
                        ):

    from AthenaCommon.AppMgr import ToolSvc

    if calorimeterCellContainer == None:
        calorimeterCellContainer = \
          'AODCellContainer' if rec.readAOD() else 'AllCalo'

    if trackToVertexTool == None:
        from TrackToVertex.TrackToVertexConf import Reco__TrackToVertex
        trackToVertexTool = Reco__TrackToVertex()
        ToolSvc += trackToVertexTool

    if trackSummaryTool == None:
        from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
        trackSummaryTool = AtlasTrackSummaryTool()
        ToolSvc += trackSummaryTool

    if extrapolator == None:
        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        extrapolator = AtlasExtrapolator()
        ToolSvc += extrapolator


    if trackSelector == None:
        selname = name + 'TrackSelector'
        from AthenaCommon.BeamFlags import jobproperties
        if (jobproperties.Beam.beamType() == 'cosmics' or
            jobproperties.Beam.beamType() == 'singlebeam'):

            trackSelector = InDet__InDetDetailedTrackSelectorTool (
                name                 = selname,
                pTMin                = 0*MeV,
                IPd0Max              = 9999*mm,
                IPz0Max              = 9999*mm,
                z0Max                = 9999*mm,
                useTrackSummaryInfo  = False,
                useTrackQualityInfo  = False,
                TrackSummaryTool     = trackSummaryTool,
                Extrapolator         = extrapolator)
        else:
            trackSelector = InDet__InDetDetailedTrackSelectorTool (
                name                 = selname,
                pTMin                = pTMin,
                IPd0Max              = IPd0Max,
                IPz0Max              = IPz0Max,
                z0Max                = z0Max,
                useTrackSummaryInfo  = useTrackSummaryInfo,
                nHitBLayer           = nHitBLayer,
                nHitPix              = nHitPix,
                nHitBLayerPlusPix    = nHitBLayerPlusPix,
                nHitSct              = nHitSct,
                nHitSi               = nHitSi,
                nHitTrt              = nHitTrt,
                useTrackQualityInfo  = useTrackQualityInfo,
                TrackSummaryTool     = trackSummaryTool,
                Extrapolator         = extrapolator,
                **kw)
        ToolSvc += trackSelector


    tool = TrackIsolationTool(
        name,
        DoTrackIsolation         = doTrackIsolation,
        DoSumConversionTracks    = doSumConversionTracks,
        TrackParticleCollection  = trackParticleCollection,
        ConversionContainer      = conversionContainer,
        CaloCellContainer        = calorimeterCellContainer,

        OnlyEM                   = onlyEM,
        CoreDeltaR               = coreDeltaR,
        ExtrapolTrackToCaloTool  = ToolSvc.TrackInCaloTools,

        TrackToVertex            = trackToVertexTool,
        TrackSelector            = trackSelector)
    ToolSvc += tool
    return tool



#########################################################################
# Define some useful variations of TrackIsolationTool.
#


#
# Track cut of 500 MeV
#
def trackIsolationTool_500MeV():
    return trackIsolationTool (name = 'TrackIsolationTool_500MeV',
                               pTMin = 500*MeV)


#
# Track cut of 1 GeV
#
def trackIsolationTool_1GeV():
    return trackIsolationTool (name = 'TrackIsolationTool_1GeV',
                               pTMin = 1*GeV)


#
# Track cut of 2 GeV
#
def trackIsolationTool_2GeV():
    return trackIsolationTool (name = 'TrackIsolationTool_2GeV',
                               pTMin = 2*GeV)


#
# Track cut of 3 GeV
#
def trackIsolationTool_3GeV():
    return trackIsolationTool (name = 'TrackIsolationTool_3GeV',
                               pTMin = 3*GeV)


#
# Track cut of 4 GeV
#
def trackIsolationTool_4GeV():
    return trackIsolationTool (name = 'TrackIsolationTool_4GeV',
                               pTMin = 4*GeV)


#
# Track cut of 4 GeV
#
def trackIsolationTool_5GeV():
    return trackIsolationTool (name = 'TrackIsolationTool_5GeV',
                               pTMin = 5*GeV)

def trackIsolationTool_3GeV_hitschi():
    return trackIsolationTool (name = 'TrackIsolationTool_3GeV_hitschi',
                               pTMin = 3*GeV,
                               nHitPix              = 1,
                               nHitSct              = 4,
                               nHitTrt              = 10,
                               useTrackQualityInfo  = True,
                               fitChi2OnNdfMax      = 3.0,
                               )


def trackIsolationTool_trkelstyle():
    from AthenaCommon.AppMgr import ToolSvc

    IsolBuildTestBLayerTool = None
    if DetFlags.haveRIO.pixel_on():
        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        TrackIsolationExtrapolator = AtlasExtrapolator()
        ToolSvc+=TrackIsolationExtrapolator

        from InDetTestBLayer.InDetTestBLayerConf import \
             InDet__InDetTestBLayerTool
        IsolBuildTestBLayerTool = InDet__InDetTestBLayerTool \
          (name= "IsolBuildTestBLayerTool",
           PixelSummarySvc = ServiceMgr.PixelConditionsSummarySvc,
           Extrapolator    = TrackIsolationExtrapolator)
        ToolSvc += IsolBuildTestBLayerTool

    return trackIsolationTool (name = 'TrackIsolationTool_trkelstyle',
                               pTMin = 0.4*MeV,
                               IPd0Max = 1.5*mm,
                               IPz0Max = 1.0*mm,
                               nHitBLayer = 1,
                               nHitSi = 9,
                               nHolesPixel = 0,
                               nSharedBLayer = 999,
                               nSharedPix = 999,
                               nSharedSct = 999,
                               nSharedSi = 999,
                               d0MaxPreselection = 9999,
                               InDetTestBLayerTool = IsolBuildTestBLayerTool)
