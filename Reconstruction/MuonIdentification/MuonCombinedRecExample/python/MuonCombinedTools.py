# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from MuonCombinedRecExample.MuonCombinedKeys import MuonCombinedKeys as MuonCbKeys
from MuonRecExample.MuonRecFlags import muonRecFlags
from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.Resilience import protectedInclude

from AthenaCommon import CfgMgr
from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,getService,getServiceClone
from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam

from AthenaCommon.GlobalFlags import globalflags
from RecExConfig.RecFlags import rec
from TriggerJobOpts.TriggerFlags import TriggerFlags

def MuonCombinedInDetDetailedTrackSelectorTool( name='MuonCombinedInDetDetailedTrackSelectorTool', **kwargs):
    if beamFlags.beamType() == 'cosmics':
        kwargs.setdefault("pTMin", 500 )
        kwargs.setdefault("IPd0Max", 19999.0 )
        kwargs.setdefault("IPz0Max", 19999.0 )
        kwargs.setdefault("z0Max", 19999.0 )
        kwargs.setdefault("useTrackSummaryInfo", False )
        kwargs.setdefault("useTrackQualityInfo", False )
    else:
        kwargs.setdefault("pTMin", 2000 )
        kwargs.setdefault("IPd0Max", 50.0 )
        kwargs.setdefault("IPz0Max", 9999.0 )
        kwargs.setdefault("z0Max", 9999.0 )
        kwargs.setdefault("useTrackSummaryInfo", True )
        kwargs.setdefault("nHitBLayer", 0 )
        kwargs.setdefault("nHitPix", 1 )
        kwargs.setdefault("nHitBLayerPlusPix", 0 )
        kwargs.setdefault("nHitSct", 3 )
        kwargs.setdefault("nHitSi", 4 )
        kwargs.setdefault("nHitTrt", 0 )
        kwargs.setdefault("useTrackQualityInfo", False )
    kwargs.setdefault("TrackSummaryTool", getPublicTool("AtlasTrackSummaryTool") )
    kwargs.setdefault("Extrapolator", getPublicTool("AtlasExtrapolator") )
    return CfgMgr.InDet__InDetDetailedTrackSelectorTool(name,**kwargs)


def MuonCombinedInDetDetailedTrackSelectorTool_LRT(name='MuonCombinedInDetDetailedTrackSelectorTool_LRT', **kwargs):
    kwargs.setdefault("pTMin", 2000 )
    kwargs.setdefault("IPd0Max", 1.e4 )
    kwargs.setdefault("IPz0Max",  1.e4 )
    kwargs.setdefault("z0Max",  1.e4  )
    kwargs.setdefault("useTrackSummaryInfo", True )
    kwargs.setdefault("nHitBLayer", 0 )
    kwargs.setdefault("nHitPix", 0 )
    kwargs.setdefault("nHitBLayerPlusPix", 0 )
    kwargs.setdefault("nHitSct", 4 )
    kwargs.setdefault("nHitSi", 4 )
    kwargs.setdefault("nHitTrt", 0 )
    kwargs.setdefault("useTrackQualityInfo", False )    
    kwargs.setdefault("TrackSummaryTool", getPublicTool("AtlasTrackSummaryTool") )
    kwargs.setdefault("Extrapolator", getPublicTool("AtlasExtrapolator") )
    return CfgMgr.InDet__InDetDetailedTrackSelectorTool(name,**kwargs)
    


def MuonInDetForwardCandidateTool( name = 'MuonInDetForwardCandidateTool', **kwargs):
   #import pdb ; pdb.set_trace()
   idCandTool = getPublicToolClone("InDetForwardCandidateTool","InDetCandidateTool", TrackSelector = getPublicTool("MuonCombinedInDetDetailedForwardTrackSelectorTool") )
   idCandTool.FlagCandidatesAsSiAssociated = True
   return idCandTool

def MuonCombinedParticleCreator(name="MuonCombinedParticleCreator",**kwargs):
    if TriggerFlags.MuonSlice.doTrigMuonConfig:
        kwargs.setdefault("TrackSummaryTool"              , getPublicTool("MuonTrackSummaryTool") )
    else:
        import MuonCombinedRecExample.CombinedMuonTrackSummary
        kwargs.setdefault("TrackSummaryTool", ToolSvc.CombinedMuonTrackSummary ) #getPublicTool("CombinedMuonTrackSummary") )
    kwargs.setdefault("Extrapolator", getPublicTool("AtlasExtrapolator") )
    kwargs.setdefault("KeepAllPerigee",True )
    kwargs.setdefault("UseMuonSummaryTool",True )
    if beamFlags.beamType() == 'cosmics':
        kwargs.setdefault("PerigeeExpression","Origin")
    return CfgMgr.Trk__TrackParticleCreatorTool(name,**kwargs)

def MuonCaloParticleCreator(name="MuonCaloParticleCreator",**kwargs):
    import MuonCombinedRecExample.CombinedMuonTrackSummary
    kwargs.setdefault("Extrapolator", getPublicTool("AtlasExtrapolator") )
    kwargs.setdefault("TrackSummaryTool", ToolSvc.CombinedMuonTrackSummary ) #getPublicTool("CombinedMuonTrackSummary") )
    kwargs.setdefault("KeepAllPerigee",True )
    kwargs.setdefault("PerigeeExpression","Origin")
    return CfgMgr.Trk__TrackParticleCreatorTool(name,**kwargs)

def InDetCandidateTool(name="InDetCandidateTool",**kwargs ):
    kwargs.setdefault("TrackSelector", getPublicTool("MuonCombinedInDetDetailedTrackSelectorTool") )
    return CfgMgr.MuonCombined__InDetCandidateTool(name,**kwargs)

def MuonPrintingTool(name="MuonPrintingTool",**kwargs ):
    kwargs.setdefault("MuonStationPrinter", getPublicTool("MuonEDMPrinterTool"))
    return CfgMgr.Rec__MuonPrintingTool(name,**kwargs)

def MuonCreatorTool(name="MuonCreatorTool",**kwargs):
    kwargs.setdefault("CaloMaterialProvider", getPublicTool("MuonMaterialProviderTool"))
    if TriggerFlags.MuonSlice.doTrigMuonConfig:
        kwargs.setdefault('MakeTrackAtMSLink',True)
        kwargs.setdefault("FillTimingInformation",False)
        kwargs.setdefault("MuonSelectionTool", "")
        kwargs.setdefault("UseCaloCells", False)
        kwargs.setdefault("TrackSegmentAssociationTool", "")
    else:
        getPublicTool("MuonMomentumBalanceSignificanceTool")
        getPublicTool("MuonScatteringAngleSignificanceTool")
        getPublicTool("MuonCaloParticleCreator")
    import MuonCombinedRecExample.CombinedMuonTrackSummary
    from AthenaCommon.AppMgr import ToolSvc
    kwargs.setdefault("TrackSummaryTool", ToolSvc.CombinedMuonTrackSummary)

    kwargs.setdefault("TrackParticleCreator", getPublicTool("MuonCombinedParticleCreator") )
    kwargs.setdefault("ParticleCaloExtensionTool", getPublicTool("MuonParticleCaloExtensionTool") )
    kwargs.setdefault("ParticleCaloExtensionToolID", getPublicTool("MuonParticleCaloExtensionTool") )
    kwargs.setdefault("MuonPrinter", getPublicTool("MuonPrintingTool") )
    return CfgMgr.MuonCombined__MuonCreatorTool(name,**kwargs)

def MuonCreatorTool_LRT(name="MuonCreatorTool_LRT",**kwargs):
    kwargs.setdefault("CaloMaterialProvider", getPublicTool("MuonMaterialProviderTool"))
    if TriggerFlags.MuonSlice.doTrigMuonConfig:
        kwargs.setdefault('MakeTrackAtMSLink',True)
        kwargs.setdefault("FillTimingInformation",False)
        kwargs.setdefault("MuonSelectionTool", "")
        kwargs.setdefault("UseCaloCells", False)
        kwargs.setdefault("TrackSegmentAssociationTool", "")
    else:
        getPublicTool("MuonMomentumBalanceSignificanceTool")
        getPublicTool("MuonScatteringAngleSignificanceTool")
        getPublicTool("MuonCaloParticleCreator")
    import MuonCombinedRecExample.CombinedMuonTrackSummary
    from AthenaCommon.AppMgr import ToolSvc
    kwargs.setdefault("TrackSummaryTool", ToolSvc.CombinedMuonTrackSummary)

    kwargs.setdefault("TrackParticleCreator", getPublicTool("MuonCombinedParticleCreator") )
    kwargs.setdefault("ParticleCaloExtensionTool", getPublicTool("MuonParticleCaloExtensionTool") )
    kwargs.setdefault("ParticleCaloExtensionToolID", getPublicTool("MuonParticleCaloExtensionTool") )
    kwargs.setdefault("MuonPrinter", getPublicTool("MuonPrintingTool") )
    kwargs.setdefault("RequireIDTrack", True)
    return CfgMgr.MuonCombined__MuonCreatorTool(name,**kwargs)
    
    



def ExtrapolateMuonToIPTool(name="ExtrapolateMuonToIPTool",**kwargs):
    if TriggerFlags.MuonSlice.doTrigMuonConfig:
        kwargs.setdefault("TrackSummaryTool", getPublicTool("MuonTrackSummaryTool"))
    else:
        import MuonCombinedRecExample.CombinedMuonTrackSummary
        from AthenaCommon.AppMgr import ToolSvc
        kwargs.setdefault("TrackSummaryTool", ToolSvc.CombinedMuonTrackSummary)
    return CfgMgr.ExtrapolateMuonToIPTool(name,**kwargs)

def MuonCandidateTool(name="MuonCandidateTool",**kwargs):
    if beamFlags.beamType() == 'cosmics':
        kwargs.setdefault("ExtrapolationStrategy", 1 )
        kwargs.setdefault("TrackExtrapolationTool", getPublicTool("ExtrapolateMuonToIPTool"))
    if TriggerFlags.MuonSlice.doTrigMuonConfig:
        trigTrackBuilder = getPublicToolClone("TrigCombinedMuonTrackBuilder","CombinedMuonTrackBuilder",
                                              TrackSummaryTool=getPublicTool("MuonTrackSummaryTool"))
        kwargs.setdefault("TrackBuilder", trigTrackBuilder)
    return CfgMgr.MuonCombined__MuonCandidateTool(name,**kwargs)

def MuonCombinedTool(name="MuonCombinedTool",**kwargs):
    tools = []
    if muonCombinedRecFlags.doCombinedFit():
        tools.append(getPublicTool("MuonCombinedFitTagTool"))
    if muonCombinedRecFlags.doStatisticalCombination() and beamFlags.beamType() != 'cosmics':
        tools.append(getPublicTool("MuonCombinedStacoTagTool"))
    kwargs.setdefault("MuonCombinedTagTools", tools )
    return CfgMgr.MuonCombined__MuonCombinedTool(name,**kwargs)

def MuonCombinedFitTagTool(name="MuonCombinedFitTagTool",**kwargs):
    from AthenaCommon.AppMgr import ToolSvc
    if TriggerFlags.MuonSlice.doTrigMuonConfig:
        from TrkExRungeKuttaIntersector.TrkExRungeKuttaIntersectorConf import Trk__IntersectorWrapper as Propagator
        TrigMuonPropagator = Propagator(name = 'TrigMuonPropagator')
        ToolSvc += TrigMuonPropagator
        kwargs.setdefault("TrackBuilder",         getPublicToolClone("TrigMuonTrackBuilder", "CombinedMuonTrackBuilder", Propagator=TrigMuonPropagator) )
        kwargs.setdefault("VertexContainer", "")
    else:
        kwargs.setdefault("TrackBuilder",         getPublicTool("CombinedMuonTrackBuilder") )
    kwargs.setdefault("TrackQuery",           getPublicTool("MuonTrackQuery") )
    kwargs.setdefault("MatchQuality",         getPublicTool("MuonMatchQuality") )
    return CfgMgr.MuonCombined__MuonCombinedFitTagTool(name,**kwargs)

def MuonCombinedStacoTagTool(name="MuonCombinedStacoTagTool",**kwargs):
    kwargs.setdefault("ParticleCaloExtensionTool",  getPublicTool("MuonParticleCaloExtensionTool") )

    return CfgMgr.MuonCombined__MuonCombinedStacoTagTool(name,**kwargs)

