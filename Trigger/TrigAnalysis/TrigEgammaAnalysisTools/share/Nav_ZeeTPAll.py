# job options for NavZeeTPxAODValidation
# Ryan Mackenzie White <ryan.white@cern.ch>

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSecVertexFinder.set_Value_and_Lock(False)
from AthenaCommon.AppMgr import ToolSvc

import os

#if not 'DIR' in dir():
#     dirtouse='valid1.147806.PowhegPythia8_AU2CT10_Zee.recon.AOD.e2658_s1967_s1964_r5787_tid01572823_00'
#else :
#     dirtouse=DIR

dirtouse='/afs/cern.ch/user/r/rwhite/workspace/xAOD/DC14/19.1.1.5/valid1.147806.PowhegPythia8_AU2CT10_Zee.recon.AOD.e2658_s1967_s1964_r5787_tid01572823_00'

# To run
# athena -l DEBUG -c "DIR='/afs/cern.ch/user/r/rwhite/workspace/egamma/mc/DC14Val/mc14_13TeV.147406.PowhegPythia8_AZNLO_Zee.recon.AOD.e3059_s1982_s2008_r5787_tid01572494_00'" Nav_ZeeTPVal.py
listfiles=os.listdir(dirtouse)
finallist=[]
for ll in listfiles:
      finallist.append(dirtouse+'/'+ll)
#print finallist


athenaCommonFlags.FilesInput=finallist

athenaCommonFlags.EvtMax=1000
#athenaCommonFlags.EvtMax=-1
rec.readAOD=True
# switch off detectors
rec.doForwardDet=False
rec.doInDet=False
rec.doCalo=False
rec.doMuon=False
rec.doEgamma=False
rec.doTrigger = True; recAlgs.doTrigger=False # disable trigger (maybe necessary if detectors switched of)
rec.doMuon=False
rec.doMuonCombined=False
rec.doWriteAOD=False
rec.doWriteESD=False
rec.doDPD=False
rec.doTruth=False


# autoconfiguration might trigger undesired feature
rec.doESD.set_Value_and_Lock(False) # uncomment if do not run ESD making algorithms
rec.doWriteESD.set_Value_and_Lock(False) # uncomment if do not write ESD
rec.doAOD.set_Value_and_Lock(False) # uncomment if do not run AOD making algorithms
rec.doWriteAOD.set_Value_and_Lock(False) # uncomment if do not write AOD
rec.doWriteTAG.set_Value_and_Lock(False) # uncomment if do not write TAG

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output += ["file1 DATAFILE='output.root' OPT='RECREATE'"]

#topSequence += TrigDecChecker
import PyUtils.RootUtils as ru
ROOT = ru.import_root()
import PyCintex
PyCintex.loadDictionary('ElectronPhotonSelectorToolsDict')
from ROOT import LikeEnum
from ROOT import egammaPID

from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaNavZeeTPCounts, TrigEgammaNavZeeTPEff, TrigEgammaNavZeeTPRes, TrigEgammaNavZeeTPIneff
ZeeCounts = TrigEgammaNavZeeTPCounts("ZeeCounts",OutputLevel=INFO)
ZeeTPEff = TrigEgammaNavZeeTPEff("ZeeTPEff")
ZeeRes = TrigEgammaNavZeeTPRes("ZeeRes")
ZeeTPIneff = TrigEgammaNavZeeTPIneff("ZeeTPIneff")
ToolSvc += ZeeCounts
ToolSvc += ZeeRes
ToolSvc += ZeeTPIneff
ToolSvc += ZeeTPEff

# Following loads the online selectors
from TrigEgammaHypo.TrigEgammaPidTools import ElectronPidTools
from TrigEgammaHypo.TrigEgammaPidTools import ElectronToolName
ElectronPidTools()

from ElectronPhotonSelectorTools.ElectronIsEMSelectorMapping import electronPIDmenu

# Offline ++ selectors
from ElectronPhotonSelectorTools.ConfiguredAsgElectronIsEMSelectors import ConfiguredAsgElectronIsEMSelector
LooseElectronSelector = ConfiguredAsgElectronIsEMSelector("LooseElectronSelector", egammaPID.ElectronIDLoosePP)
LooseElectronSelector.ConfigFile = "ElectronPhotonSelectorTools/offline/dc14b_20141031/ElectronIsEMLooseSelectorCutDefs.conf"  
ToolSvc+=LooseElectronSelector
MediumElectronSelector = ConfiguredAsgElectronIsEMSelector("MediumElectronSelector", egammaPID.ElectronIDMediumPP)
MediumElectronSelector.ConfigFile = "ElectronPhotonSelectorTools/offline/dc14b_20141031/ElectronIsEMMediumSelectorCutDefs.conf"  
ToolSvc+=MediumElectronSelector
TightElectronSelector = ConfiguredAsgElectronIsEMSelector("TightElectronSelector", egammaPID.ElectronIDTightPP)
TightElectronSelector.ConfigFile = "ElectronPhotonSelectorTools/offline/dc14b_20141031/ElectronIsEMTightSelectorCutDefs.conf"  
ToolSvc+=TightElectronSelector
from egammaMVACalib.egammaMVACalibConf import egammaMVATool 
mvatool = egammaMVATool("mvatool",folder="egammaMVACalib/v1")
ToolSvc += mvatool

from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaAnalysisAlg
topSequence += TrigEgammaAnalysisAlg("alg",OutputLevel=VERBOSE)
alg = topSequence.alg
ZeeTPEff.ElectronPPSelector =[ ToolSvc.TightElectronSelector,ToolSvc.MediumElectronSelector,ToolSvc.LooseElectronSelector]
ZeeTPEff.minTrigList=["e28_tight","e28_tight_iloose"]
ZeeTPEff.ProbeTriggerList=["e28_tight_iloose"]

ZeeCounts.ElectronPPSelector =[ ToolSvc.TightElectronSelector,ToolSvc.MediumElectronSelector,ToolSvc.LooseElectronSelector]
ZeeCounts.minTrigList=["e28_tight","e28_tight_iloose"]
ZeeCounts.ProbeTriggerList=["e28_tight_iloose","e28_tight","e24_medium_iloose"]

ZeeRes.ElectronPPSelector =[ ToolSvc.TightElectronSelector,ToolSvc.MediumElectronSelector,ToolSvc.LooseElectronSelector]
ZeeRes.MVACalibTool = mvatool
ZeeRes.minTrigList=["e28_tight","e28_tight_iloose"]
ZeeRes.isEMofflineTag=egammaPID.ElectronTightPP
ZeeRes.ProbeTriggerList=["e28_tight_iloose"]

ZeeTPIneff.IneffLabels=["ClusterEtaRange","ConversionMatch","ClusterHadronicLeakage","ClusterMiddleEnergy","ClusterMiddleEratio37","ClusterMiddleEratio33","ClusterMiddleWidth","f3","ClusterStripsEratio","ClusterStripsDeltaEmax2","ClusterStripsDeltaE","ClusterStripsWtot","ClusterStripsFracm","ClusterStripsWeta1c","empty14","ClusterStripsDEmaxs1","TrackBlayer","TrackPixel","TrackSi","TrackA0","TrackMatchEta","TrackMatchPhi","TrackMatchEoverP","empty23","TrackTRThits","TrackTRTratio","TrackTRTratio90","TrackA0Tight","TrackMatchEtaTight","Isolation","ClusterIsolation","TrackIsolation"]
ZeeTPIneff.ElectronPPSelector =[ ToolSvc.TightElectronSelector,ToolSvc.MediumElectronSelector,ToolSvc.LooseElectronSelector]
ZeeTPIneff.ElectronOnlPPSelector=[ ToolSvc.AsgElectronIsEMTightSelector,ToolSvc.AsgElectronIsEMMediumSelector,ToolSvc.AsgElectronIsEMLooseSelector]
ZeeTPIneff.minTrigList=["e28_tight","e28_tight_iloose"]
ZeeTPIneff.ProbeTriggerList=["e28_tight_iloose"]

alg.Tools += [ ZeeCounts, ZeeRes, ZeeTPEff, ZeeTPIneff ] 


# setup TTree registration Service
# save ROOT histograms and Tuple
THistSvc = Service ( "THistSvc" )
THistSvc.Output += ["Validation_Zee DATAFILE='Validation_Zee.root' OPT='RECREATE'"]

svcMgr.MessageSvc.OutputLevel=ERROR

