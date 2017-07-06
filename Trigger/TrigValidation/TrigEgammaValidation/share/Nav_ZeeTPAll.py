# job options for NavZeeTPxAODValidation
# Ryan Mackenzie White <ryan.white@cern.ch>

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSecVertexFinder.set_Value_and_Lock(False)
from AthenaCommon.AppMgr import ToolSvc

import os

if not 'DIR' in dir():
     dirtouse='valid1.147806.PowhegPythia8_AU2CT10_Zee.recon.AOD.e2658_s1967_s1964_r5787_tid01572823_00'
else :
     dirtouse=DIR

 

#dirtouse='/afs/cern.ch/user/r/rwhite/workspace/xAOD/DC14/19.1.1.5/valid1.147806.PowhegPythia8_AU2CT10_Zee.recon.AOD.e2658_s1967_s1964_r5787_tid01572823_00'


listfiles=os.listdir(dirtouse)
finallist=[]
for ll in listfiles:
      finallist.append(dirtouse+'/'+ll)
print finallist


athenaCommonFlags.FilesInput=finallist

#athenaCommonFlags.EvtMax=2000
athenaCommonFlags.EvtMax=-1
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
from ROOT import LikeEnum
from ROOT import egammaPID
from ROOT import egammaParameters

#from TrigEgammaHypo.TrigEGammaPIDdefs import SelectionDefElectron
from ElectronPhotonSelectorTools.TrigEGammaPIDdefs import SelectionDefElectron

from TrigEgammaHypo.TrigEGammaPIDdefs import TrigEgammaIDQuality
from TrigEgammaHypo.TrigEGammaPIDdefsDC14 import TrigEgammaIDQualityDC14

from TrigEgammaHypo.TrigEgammaElectronIsEMSelectorMapping import electronPIDmenu as electronTrigPIDmenu

from TrigEgammaValidation.TrigEgammaValidationConf import NavZeeTPCounts, NavZeeTPEff, NavZeeTPRes, NavZeeTPIneff
zee = NavZeeTPCounts()
zee1 = NavZeeTPEff()
zee2 = NavZeeTPRes()
zee3 = NavZeeTPIneff()

from ElectronPhotonSelectorTools.ElectronIsEMSelectorMapping import electronPIDmenu
try:
    from ElectronPhotonSelectorTools.ConfiguredAsgElectronIsEMSelectors import ConfiguredAsgElectronIsEMSelector
except:
    mlog = logging.getLogger(name+'::__init__')
    mlog.error("could not get handle to AthenaSelectorTool")

# Offline ++ selectors
theelectroncuttightid=ConfiguredAsgElectronIsEMSelector("AsgElectronIsEMTightPPSelector",egammaPID.ElectronIDTightPP,electronPIDmenu.menu2012)
ToolSvc+=theelectroncuttightid       

theelectroncutmediumid=ConfiguredAsgElectronIsEMSelector("AsgElectronIsEMMediumPPSelector",egammaPID.ElectronIDMediumPP,electronPIDmenu.menu2012)
ToolSvc+=theelectroncutmediumid       

theelectroncutlooseid=ConfiguredAsgElectronIsEMSelector("AsgElectronIsEMLoosePPSelector",egammaPID.ElectronIDLoosePP,electronPIDmenu.menu2012)
ToolSvc+=theelectroncutlooseid       

# Online ++ selectors
from TrigEgammaRec.TrigEgammaAthElectronIsEMSelectors import TrigEgammaAthElectronIsEMSelector

theOnlelectroncuttightid=TrigEgammaAthElectronIsEMSelector("AsgOnlElectronIsEMTightPPSelector",TrigEgammaIDQuality.ElectronIDTight1,electronTrigPIDmenu.menuTrig2012)
ToolSvc+=theOnlelectroncuttightid

theOnlelectroncutmediumid=TrigEgammaAthElectronIsEMSelector("AsgOnlElectronIsEMMediumPPSelector",TrigEgammaIDQuality.ElectronIDMedium1,electronTrigPIDmenu.menuTrig2012)
ToolSvc+=theOnlelectroncutmediumid

theOnlelectroncutlooseid=TrigEgammaAthElectronIsEMSelector("AsgOnlElectronIsEMLoosePPSelector",TrigEgammaIDQuality.ElectronIDLoose1,electronTrigPIDmenu.menuTrig2012)
ToolSvc+=theOnlelectroncutlooseid



zee.ElectronPPSelector =[ theelectroncuttightid, theelectroncutmediumid, theelectroncutlooseid]
zee1.ElectronPPSelector =[ theelectroncuttightid, theelectroncutmediumid, theelectroncutlooseid]
zee2.ElectronPPSelector =[ theelectroncuttightid, theelectroncutmediumid, theelectroncutlooseid]
zee3.ElectronPPSelector =[ theelectroncuttightid, theelectroncutmediumid, theelectroncutlooseid]
zee3.ElectronOnlPPSelector =[ theOnlelectroncuttightid, theOnlelectroncutmediumid, theOnlelectroncutlooseid]

zee.minTrigList=["e28_tight","e28_tight_iloose"]
zee1.minTrigList=["e28_tight","e28_tight_iloose"]
zee2.minTrigList=["e28_tight","e28_tight_iloose"]
zee3.minTrigList=["e28_tight","e28_tight_iloose"]

zee3.IneffLabels=["ClusterEtaRange","ConversionMatch","ClusterHadronicLeakage","ClusterMiddleEnergy","ClusterMiddleEratio37","ClusterMiddleEratio33","ClusterMiddleWidth","f3","ClusterStripsEratio","ClusterStripsDeltaEmax2","ClusterStripsDeltaE","ClusterStripsWtot","ClusterStripsFracm","ClusterStripsWeta1c","empty14","ClusterStripsDEmaxs1","TrackBlayer","TrackPixel","TrackSi","TrackA0","TrackMatchEta","TrackMatchPhi","TrackMatchEoverP","empty23","TrackTRThits","TrackTRTratio","TrackTRTratio90","TrackA0Tight","TrackMatchEtaTight","Isolation","ClusterIsolation","TrackIsolation"]

#topSequence += zee
topSequence += zee1
topSequence += zee2
topSequence += zee3

# setup TTree registration Service
# save ROOT histograms and Tuple
THistSvc = Service ( "THistSvc" )
THistSvc.Output += ["ValidationBase DATAFILE='Validation_zee.root' OPT='RECREATE'"]

svcMgr.MessageSvc.OutputLevel=ERROR

