# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

### JobOptions to run MuonCaloTag in xAOD

from AthenaCommon import CfgMgr
from AthenaCommon.CfgGetter import getPublicTool,getService

###logfile
from AthenaCommon.Logging import log

###############################################################################
## Helper function for defining THistSvc inputs
###############################################################################
# Finds the root file @rootFileName and creates a one-item list for THistSvc.Input
###############################################################################

### Track Selector for CaloTrkMuIdAlg
def CaloTrkMuIdAlgTrackSelectorTool( name='CaloTrkMuIdAlgTrackSelectorTool', **kwargs ):
    import MuonCombinedRecExample.CombinedMuonTrackSummary
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
    return CfgMgr.TrackDepositInCaloTool(name,**kwargs)

def CaloMuonTagLoose( name='CaloMuonTagLoose', **kwargs ):
    kwargs.setdefault("TagMode","Loose")
    return CfgMgr.CaloMuonTag(name,**kwargs)

### Configure CaloMuonTag (tight is default) ###
def CaloMuonTag( name='CaloMuonTag', **kwargs ):
    return CfgMgr.CaloMuonTag(name,**kwargs)

def CaloMuonLikelihoodTool(name='CaloMuonLikelihoodTool', **kwargs ):
    kwargs.setdefault("TrackEnergyInCaloTool", getPublicTool("TrackEnergyInCaloTool") )
    return CfgMgr.CaloMuonLikelihoodTool(name,**kwargs)

def MuonCaloTagTool( name='MuonCaloTagTool', **kwargs ):
    kwargs.setdefault("CaloMuonTagLoose",       getPublicTool("CaloMuonTagLoose") )
    kwargs.setdefault("CaloMuonTagTight",       getPublicTool("CaloMuonTag") )
    kwargs.setdefault("CaloMuonLikelihoodTool", getPublicTool("CaloMuonLikelihoodTool") )
    kwargs.setdefault("TrackDepositInCaloTool", getPublicTool("TrackDepositInCaloTool") )
    kwargs.setdefault("TrackSelectorTool",      getPublicTool("CaloTrkMuIdAlgTrackSelectorTool") )
    kwargs.setdefault("doCaloLR",               True )
    return CfgMgr.MuonCombined__MuonCaloTagTool(name,**kwargs )
