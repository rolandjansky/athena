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
    kwargs.setdefault("ptMin"            ,  15000 )
    kwargs.setdefault("ptMax"            ,  35000 )
    kwargs.setdefault("emb1Veto"         ,  ( 400,   800  ) )
    kwargs.setdefault("emb2Veto"         ,  ( 700,   1400 ) )
    kwargs.setdefault("emb3Veto"         ,  ( 500,   600  ) )
    kwargs.setdefault("eme1Veto"         ,  ( 1100,  1300 ) )
    kwargs.setdefault("eme2Veto"         ,  ( 2200,  2100 ) )
    kwargs.setdefault("eme3Veto"         ,  ( 1100,  1100 ) )
    kwargs.setdefault("tileBar0Sig"      ,  ( 0,    -400  ) )
    kwargs.setdefault("tileBar1Sig"      ,  ( 100,  -1500 ) )
    kwargs.setdefault("tileBar2Sig"      ,  ( 0,    -400  ) )
    kwargs.setdefault("tileExt0Sig"      ,  ( -50,  -500  ) )
    kwargs.setdefault("tileExt1Sig"      ,  ( 200,  -600  ) )
    kwargs.setdefault("tileExt2Sig"      ,  ( 150,  -1200 ) )
    kwargs.setdefault("tileBar0Veto"     ,  ( 3500,  8000 ) )
    kwargs.setdefault("tileBar1Veto"     ,  ( 3500,  8000 ) )
    kwargs.setdefault("tileBar2Veto"     ,  ( 3500,  8000 ) )
    kwargs.setdefault("tileExt0Veto"     ,  ( 3000,  7000 ) )
    kwargs.setdefault("tileExt1Veto"     ,  ( 3500,  9000 ) )
    kwargs.setdefault("tileExt2Veto"     ,  ( 3500,  9000 ) )
    kwargs.setdefault("tileBar0DiffLow"  ,  (-400,  -1100 ) )
    kwargs.setdefault("tileBar1DiffLow"  ,  (-650,  -1300 ) )
    kwargs.setdefault("tileBar2DiffLow"  ,  (-400,  -1100 ) )
    kwargs.setdefault("tileExt0DiffLow"  ,  (-600,  -1100 ) )
    kwargs.setdefault("tileExt1DiffLow"  ,  (-600,  -1500 ) )
    kwargs.setdefault("tileExt2DiffLow"  ,  (-600,  -1600 ) )
    kwargs.setdefault("hec0Veto"         ,  (2500,   9000 ) )
    kwargs.setdefault("hec1Veto"         ,  (3000,   9000 ) )
    kwargs.setdefault("hec2Veto"         ,  (3000,   9000 ) )
    kwargs.setdefault("hec3Veto"         ,  (3000,   9000 ) )
    kwargs.setdefault("hec0Sig"          ,  (-50,   -1100 ) )
    kwargs.setdefault("hec1Sig"          ,  (-50,   -1100 ) )
    kwargs.setdefault("hec2Sig"          ,  (0,     -1100 ) )
    kwargs.setdefault("hec3Sig"          ,  (150,   -1100 ) )
    return CfgMgr.CaloMuonTag(name,**kwargs)

### Configure CaloMuonTag (tight is default) ###
def CaloMuonTag( name='CaloMuonTag', **kwargs ):
    return CfgMgr.CaloMuonTag(name,**kwargs)

def CaloMuonLikelihoodTool(name='CaloMuonLikelihoodTool', **kwargs ):
    from AthenaCommon.AppMgr import ServiceMgr
    if not hasattr(ServiceMgr, 'THistSvc'):
        ServiceMgr += CfgMgr.THistSvc()

    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if athenaCommonFlags.isOnline == False:

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
    return CfgMgr.MuonCombined__MuonCaloTagTool(name,**kwargs )
