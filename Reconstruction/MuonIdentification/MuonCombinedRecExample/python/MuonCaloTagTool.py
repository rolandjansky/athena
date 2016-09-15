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
def defineCaloLhrPdf(rootFileName, streamName=""):
   import os
   from AthenaCommon.Utils.unixtools import FindFile
   rootFile = FindFile(filename=rootFileName, pathlist=os.environ['DATAPATH'].split(os.pathsep), access=os.R_OK)
   if streamName=="":
       pos1 = rootFileName.find("CaloMuonLikelihood.PDF.")
       pos2 = rootFileName.find(".root")
       if pos1!=-1 and pos2!=-1:
         streamName = rootFileName.replace(".", "_")
         streamName = streamName[0:pos2]
       else:
           raise ValueError("in CaloTrkMuIdTools_jobOptions.py: defineCaloLhrPdf(...): rootFileName: " + rootFileName + " is not an expected value (should be CaloMuonLikelihood.PDF.xx.root).")
   return [streamName + " DATAFILE='" + str(rootFile) + "' TYP='ROOT' OPT='READ'"]

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

    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if athenaCommonFlags.isOnline == False:
       from AthenaCommon.AppMgr import ServiceMgr
       if not hasattr(ServiceMgr, 'THistSvc'):
          ServiceMgr += CfgMgr.THistSvc()

       ServiceMgr.THistSvc.Input += defineCaloLhrPdf("CaloMuonLikelihood.PDF.A0.root");            ### PDFs for barrel region low pT
       ServiceMgr.THistSvc.Input += defineCaloLhrPdf("CaloMuonLikelihood.PDF.A1.root");            ### PDFs for barrel region medium pT
       ServiceMgr.THistSvc.Input += defineCaloLhrPdf("CaloMuonLikelihood.PDF.A2.root");            ### PDFs for barrel region high pT
       ServiceMgr.THistSvc.Input += defineCaloLhrPdf("CaloMuonLikelihood.PDF.B0.root");            ### PDFs for transition region low pT
       ServiceMgr.THistSvc.Input += defineCaloLhrPdf("CaloMuonLikelihood.PDF.B1.root");            ### PDFs for transition region medium pT
       ServiceMgr.THistSvc.Input += defineCaloLhrPdf("CaloMuonLikelihood.PDF.B2.root");            ### PDFs for transition region high pT
       ServiceMgr.THistSvc.Input += defineCaloLhrPdf("CaloMuonLikelihood.PDF.C0.root");            ### PDFs for endcap region low pT
       ServiceMgr.THistSvc.Input += defineCaloLhrPdf("CaloMuonLikelihood.PDF.C1.root");            ### PDFs for endcap region medium pT
       ServiceMgr.THistSvc.Input += defineCaloLhrPdf("CaloMuonLikelihood.PDF.C2.root");            ### PDFs for endcap region high pT
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
