# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

### JobOptions to run MuonCaloTag in xAOD

from AthenaCommon import CfgMgr
from AthenaCommon.CfgGetter import getPublicTool,getService, getPrivateTool

###############################################################################
## Helper function for defining THistSvc inputs
###############################################################################
# Finds the root file \@rootFileName and creates a one-item list for THistSvc.Input
###############################################################################

### Track Selector for CaloTrkMuIdAlg
def CaloTrkMuIdAlgTrackSelectorTool( name='CaloTrkMuIdAlgTrackSelectorTool', **kwargs ):
    from AthenaCommon.AppMgr import ToolSvc
    kwargs.setdefault("pTMin", 5000.)
    kwargs.setdefault("IPd0Max", 7.)
    kwargs.setdefault("IPz0Max", 130)     # 130 (tuned on Z)
    kwargs.setdefault("nHitBLayer", 0)
    kwargs.setdefault("nHitPix", 1)
    kwargs.setdefault("nHitSct", 5)
    kwargs.setdefault("nHitSi", 7)
    kwargs.setdefault("nHitTrt", 0)
    kwargs.setdefault("TrackSummaryTool", ToolSvc.CombinedMuonTrackSummary )
    kwargs.setdefault("Extrapolator", getPublicTool("AtlasExtrapolator") )
    return CfgMgr.InDet__InDetDetailedTrackSelectorTool(name, **kwargs )

def TrackEnergyInCaloTool( name ='TrackEnergyInCaloTool', **kwargs ):
    kwargs.setdefault("ExtrapolatorHandle", getPublicTool("AtlasExtrapolator") )
    return CfgMgr.TrackEnergyInCaloTool(name,**kwargs)

def TrackDepositInCaloTool( name ='TrackDepositInCaloTool', **kwargs ):
    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    from TrackToCalo.TrackToCaloConf import Rec__ParticleCaloCellAssociationTool
    caloCellAssociationTool = Rec__ParticleCaloCellAssociationTool(ParticleCaloExtensionTool = getPublicTool("MuonParticleCaloExtensionTool"))
    kwargs.setdefault("ExtrapolatorHandle",       AtlasExtrapolator() )
    kwargs.setdefault("ParticleCaloExtensionTool",       getPublicTool("MuonParticleCaloExtensionTool") )
    kwargs.setdefault("ParticleCaloCellAssociationTool",       caloCellAssociationTool )
    return CfgMgr.TrackDepositInCaloTool(name,**kwargs)

def CaloMuonScoreONNXRuntimeSvc(name='CaloMuonScoreONNXRuntimeSvc', **kwargs):
    return CfgMgr.CaloMuonScoreONNXRuntimeSvc(name, **kwargs)

def CaloMuonLikelihoodTool(name='CaloMuonLikelihoodTool', **kwargs ):
    kwargs.setdefault("ParticleCaloExtensionTool",       getPublicTool("MuonParticleCaloExtensionTool") )
    return CfgMgr.CaloMuonLikelihoodTool(name,**kwargs)

def CaloMuonScoreTool(name='CaloMuonScoreTool', **kwargs ):
    from TrackToCalo.TrackToCaloConf import Rec__ParticleCaloCellAssociationTool
    caloCellAssociationTool = Rec__ParticleCaloCellAssociationTool(ParticleCaloExtensionTool = getPublicTool("MuonParticleCaloExtensionTool"))
    kwargs.setdefault("ParticleCaloCellAssociationTool",       caloCellAssociationTool )
    kwargs.setdefault("CaloMuonScoreONNXRuntimeSvc", getService("CaloMuonScoreONNXRuntimeSvc") )
    return CfgMgr.CaloMuonScoreTool(name,**kwargs)

def MuonCaloTagTool( name='MuonCaloTagTool', **kwargs ):  
    from CaloTrkMuIdTools.CaloTrkMuIdToolsConf import CaloMuonTag as ConfiguredCaloMuonTag
    CaloMuonTagLoose = ConfiguredCaloMuonTag(name = "CaloMuonTagLoose")
    CaloMuonTagLoose.TagMode="Loose"
    CaloMuonTagTight = ConfiguredCaloMuonTag(name = "CaloMuonTag")
    kwargs.setdefault("CaloMuonTagLoose",       CaloMuonTagLoose )
    kwargs.setdefault("CaloMuonTagTight",       CaloMuonTagTight )
    kwargs.setdefault("CaloMuonLikelihoodTool", getPrivateTool("CaloMuonLikelihoodTool") )
    kwargs.setdefault("CaloMuonScoreTool",      getPrivateTool("CaloMuonScoreTool") )
    kwargs.setdefault("TrackDepositInCaloTool", getPublicTool("TrackDepositInCaloTool") )
    kwargs.setdefault("TrackSelectorTool",      getPublicTool("CaloTrkMuIdAlgTrackSelectorTool") )
    kwargs.setdefault("doCaloLR",               True )
    return CfgMgr.MuonCombined__MuonCaloTagTool(name,**kwargs )
