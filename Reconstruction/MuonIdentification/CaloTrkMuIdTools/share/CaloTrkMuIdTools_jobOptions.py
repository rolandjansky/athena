###############################################################################
## JobOptions for setting up CaloTrkMuIdTools 
###############################################################################
include.block("CaloTrkMuIdTools/CaloTrkMuIdTools_jobOptions.py")

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
    
###############################################################################
## jobOptions
###############################################################################
if not 'doMEOTP' in dir():
   doMEOTP=False

### Setup extrapolator ###
if(doMEOTP):
   ### Configure Material Effects On Track Provider (usually this is not done) ###
   include('CaloMaterialEffectsOnTrackProvider/CaloMaterialEffectsOnTrackProvider.py')
   from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
   CaloTrkMuId_MaterialUpdator            = Trk__MaterialEffectsUpdator()
   ToolSvc += CaloTrkMuId_MaterialUpdator
   
   from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator
   CaloTrkMuId_Propagator = Trk__RungeKuttaPropagator()
   ToolSvc += CaloTrkMuId_Propagator
   
   from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
   CaloTrkMuIdTools_Extrapolator = AtlasExtrapolator(name = 'CaloTrkMuIdTools_Extrapolator')
   CaloTrkMuIdTools_Extrapolator.DoCaloDynamic          = True
   CaloTrkMuIdTools_Extrapolator.Propagators            = [CaloTrkMuIdTools_Propagator]
   CaloTrkMuIdTools_Extrapolator.MaterialEffectsUpdator = CaloTrkMuIdTools_MaterialUpdator
   
else:
    ### Use the public AtlasExtrapolator ###
    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    CaloTrkMuIdTools_Extrapolator = AtlasExtrapolator()
    
ToolSvc += CaloTrkMuIdTools_Extrapolator
#print      CaloTrkMuIdTools_Extrapolator

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

### Configure TrackEnergyInCaloTool ###
from CaloTrkMuIdTools.CaloTrkMuIdToolsConf import TrackEnergyInCaloTool as ConfiguredTrackEnergyInCaloTool
TrackEnergyInCaloTool = ConfiguredTrackEnergyInCaloTool(name = "TrackEnergyInCaloTool", ExtrapolatorHandle = CaloTrkMuIdTools_Extrapolator)
ToolSvc += TrackEnergyInCaloTool

### Configure TrackDepositInCaloTool ###
from CaloTrkMuIdTools.CaloTrkMuIdToolsConf import TrackDepositInCaloTool as ConfiguredTrackDepositInCaloTool
TrackDepositInCaloTool = ConfiguredTrackDepositInCaloTool(name = "TrackDepositInCaloTool")
ToolSvc += TrackDepositInCaloTool

### Configure the loose CaloMuon tagger ###
from CaloTrkMuIdTools.CaloTrkMuIdToolsConf import CaloMuonTag as ConfiguredCaloMuonTag
CaloMuonTagLoose = ConfiguredCaloMuonTag(name = "CaloMuonTagLoose")
ToolSvc += CaloMuonTagLoose

### Sets loose cuts for CaloMuonTagLoose ###
include("CaloTrkMuIdTools/CaloMuonTag_Comm_jobOptions.py")
print CaloMuonTagLoose

### Configure CaloMuonTag (tight is default) ###
CaloMuonTag = ConfiguredCaloMuonTag(name = "CaloMuonTag")
ToolSvc += CaloMuonTag
print CaloMuonTag

### Configure CaloMuonLikelihoodTool ###
from CaloTrkMuIdTools.CaloTrkMuIdToolsConf import CaloMuonLikelihoodTool as ConfiguredCaloMuonLikelihoodTool
CaloMuonLikelihoodTool = ConfiguredCaloMuonLikelihoodTool(TrackEnergyInCaloTool = TrackEnergyInCaloTool)
ToolSvc += CaloMuonLikelihoodTool

