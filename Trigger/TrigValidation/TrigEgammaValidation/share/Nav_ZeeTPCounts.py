# job options for NavZeeTPxAODValidation
# Ryan Mackenzie White <ryan.white@cern.ch>

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSecVertexFinder.set_Value_and_Lock(False)
from AthenaCommon.AppMgr import ToolSvc

import os
#dirtouse='/afs/cern.ch/user/r/rwhite/workspace/xAOD/DC14/19.1.1.5/valid1.147806.PowhegPythia8_AU2CT10_Zee.recon.AOD.e2658_s1967_s1964_r5787_tid01572823_00'
dirtouse='valid1.147806.PowhegPythia8_AU2CT10_Zee.recon.AOD.e2658_s1967_s1964_r5787_tid01572823_00'
listfiles=os.listdir(dirtouse)
finallist=[]
for ll in listfiles:
      finallist.append(dirtouse+'/'+ll)
      print finallist


athenaCommonFlags.FilesInput=finallist

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

from TrigEgammaValidation.TrigEgammaValidationConf import NavZeeTPCounts
zee = NavZeeTPCounts()
#zee.Chains = ['e24_medium1_iloose','e24_medium_iloose','e28_tight1_L2Star_idperf','e28_tight1_idperf']
#zee.Chains = ['e24_medium1_L1EM20V','e24_medium_L1EM20V','e24_medium1_iloose','e24_medium_iloose','e28_tight1_iloose_L2StarA','e28_tight_iloose_L2Star','e28_tight_iloose','e0_perf_L1EM15VH','e60_medium1']
#zee.IsEMloose = SelectionDefElectron.ElectronLoose1
#zee.IsEMloose1 = SelectionDefElectron.ElectronLoose1
#zee.IsEMloosePP = egammaPID.ElectronLoosePP
zee.minTrigList=["e28_tight","e28_tight_iloose"]

#print zee.IsEMloose
#print zee.IsEMloose1
#print zee.IsEMloosePP

#zee.IsEMmedium = SelectionDefElectron.ElectronMedium1
#zee.IsEMmedium1 = SelectionDefElectron.ElectronMedium1
#zee.IsEMmediumPP = egammaPID.ElectronMediumPP

#zee.IsEMtight = SelectionDefElectron.ElectronTight1
#zee.IsEMtight1 = SelectionDefElectron.ElectronTight1
#zee.IsEMtightPP = egammaPID.ElectronTightPP

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
zee.ElectronPPSelector =[ theelectroncuttightid, theelectroncutmediumid, theelectroncutlooseid]

topSequence += zee

# setup TTree registration Service
# save ROOT histograms and Tuple
THistSvc = Service ( "THistSvc" )
THistSvc.Output += ["Validation DATAFILE='Validation_zee.root' OPT='RECREATE'"]

svcMgr.MessageSvc.OutputLevel=DEBUG

