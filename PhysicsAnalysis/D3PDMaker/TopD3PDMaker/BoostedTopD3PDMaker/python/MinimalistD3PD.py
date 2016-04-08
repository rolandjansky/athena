# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags

D3PDMakerFlags.TruthWriteHadrons = True
D3PDMakerFlags.TruthDoPileup = False

from CaloD3PDMaker.ClusterD3PDObject          import ClusterD3PDObject

from egammaD3PDMaker.ElectronD3PDObject  import ElectronD3PDObject
from MuonD3PDMaker.MuonD3PDObject        import MuonD3PDObject
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
import JetSubstructureD3PDMaker
import D3PDMakerCoreComps

#setup tool service and include
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.Include import include

#################################################################################################
##NEW Mini isolation code which calls standard isolation tools
################################################################################################

# Get EMTrackIsolationTool
try:
   from egammaTools.EMTrackIsolationToolBase import EMTrackIsolationToolBase
   theemtrackisolationtool = EMTrackIsolationToolBase("emtrackisolationtool")
   theemtrackisolationtool.useBremAssoc = True
   ToolSvc+=theemtrackisolationtool
except Exception:
   print traceback.format_exc()

# Get BoostedTopD3PDMaker which contains MI filler tools
import BoostedTopD3PDMaker

#Electrons, 10 and 15
ElectronD3PDObject.defineBlock(99, "miniiso10", BoostedTopD3PDMaker.MiniIsolationElectronFillerTool, EMTrackIsolationTool=theemtrackisolationtool, KtDist=10., prefix="MI10_")

ElectronD3PDObject.defineBlock(99, "miniiso15", BoostedTopD3PDMaker.MiniIsolationElectronFillerTool, EMTrackIsolationTool=theemtrackisolationtool, KtDist=15., prefix="MI15_")

#Get Muon Isolation tool
# This is needed to initialize the MuonIsolationTool properly.
# Without this, there will be ERRORs!
include( "MuonIsolationTools/MuonIsolationTool_jobOptions.py" )

#Muons, 10 and 15
MuonD3PDObject.defineBlock(99, "miniiso10", BoostedTopD3PDMaker.MiniIsolationMuonFillerTool, MuIsolationTool=ToolSvc.MuonIsolationTool, KtDist=10., prefix="MI10_")

MuonD3PDObject.defineBlock(99, "miniiso15", BoostedTopD3PDMaker.MiniIsolationMuonFillerTool, MuIsolationTool=ToolSvc.MuonIsolationTool, KtDist=15., prefix="MI15_")

#########################################

def MinimalistD3PD(alg):

  from CaloD3PDMaker import ClusterMomentFillerTool as CMFT
  ClusterD3PDObject.defineBlock (0, 'CenterMagMoments', CMFT, Moments = [CMFT.CENTER_MAG, 'centermag'])
  alg += ClusterD3PDObject(level=0, sgkey='CaloCalTopoCluster', prefix='cl_')
 # alg.cl_Filler.cl_Filler_Kinematics.WriteE = True


  return



