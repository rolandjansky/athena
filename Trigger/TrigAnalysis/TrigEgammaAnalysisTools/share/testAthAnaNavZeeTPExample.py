# Default job options for TrigEgammaAnalysisTools
# Authors: 
# Ryan Mackenzie White <ryan.white@cern.ch>
# Denis Damazio <denis.damazio@cern.ch
# 
# Tool and algorithm configuration done using egamma Factories
# Default configurations found in TrigEgammaAnalysisToolsConfig


#from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#from AthenaCommon.AppMgr import ToolSvc
# Access the algorithm sequece:
import os

if not 'DIR' in dir():
     dirtouse='/afs/cern.ch/user/r/rwhite/workspace/xAOD/DC14/19.1.1.5/valid1.147806.PowhegPythia8_AU2CT10_Zee.recon.AOD.e2658_s1967_s1964_r5787_tid01572823_00'
else :
     dirtouse=DIR


# To run
# athena -l DEBUG -c "DIR='/afs/cern.ch/user/r/rwhite/workspace/egamma/mc/DC14Val/mc14_13TeV.147406.PowhegPythia8_AZNLO_Zee.recon.AOD.e3059_s1982_s2008_r5787_tid01572494_00'" test_NavZeeTPAll.py
listfiles=os.listdir(dirtouse)
finallist=[]
for ll in listfiles:
      finallist.append(dirtouse+'/'+ll)
#print finallist

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = finallist


from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()
from AthenaCommon.AppMgr import theApp
theApp.EvtMax=500
ToolSvc += CfgMgr.TrigConf__xAODConfigTool( "xAODConfigTool" )
ToolSvc += CfgMgr.Trig__TrigDecisionTool( "TrigDecisionTool", ConfigTool = ToolSvc.xAODConfigTool, TrigDecisionKey = "xTrigDecision" )
# Here we configure the output histogram
# And the athena algorithm, simply a loop over tools
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["Validation_Zee DATAFILE='Validation_Zee.root' OPT='RECREATE'"]

# Check the TDT results
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaTDToolTest
theJob+=TrigEgammaTDToolTest("TrigEgammaTDToolTest")

import PyUtils.RootUtils as ru
ROOT = ru.import_root()
import PyCintex
PyCintex.loadDictionary('ElectronPhotonSelectorToolsDict')
from ROOT import LikeEnum
from ROOT import egammaPID

# Analysis Code
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaNavZeeTPCounts
ZeeCounts = TrigEgammaNavZeeTPCounts("ZeeCounts",OutputLevel=INFO)
ToolSvc += ZeeCounts

ToolSvc += CfgMgr.TrigConf__xAODConfigTool( "xAODConfigTool" )
ToolSvc += CfgMgr.Trig__TrigDecisionTool( "TrigDecisionTool", ConfigTool = ToolSvc.xAODConfigTool, TrigDecisionKey = "xTrigDecision" )

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

ZeeCounts.OutputLevel=1
ZeeCounts.TrigDecisionTool = ToolSvc.TrigDecisionTool
ZeeCounts.DirectoryPath='NavZeeTPCounts'
ZeeCounts.ElectronKey = 'Electrons'
ZeeCounts.MVACalibTool=mvatool
ZeeCounts.ApplyMVACalib=False
ZeeCounts.ElectronIsEMSelector =[TightElectronSelector,MediumElectronSelector,LooseElectronSelector]
ZeeCounts.ElectronLikelihoodTool =[]
ZeeCounts.MinimumTriggerList=["e28_tight","e28_tight_iloose","e24_tight_iloose"]
ZeeCounts.ZeeLowerMass=80
ZeeCounts.ZeeUpperMass=100
ZeeCounts.OfflineTagIsEM=egammaPID.ElectronTightPPIso
ZeeCounts.OfflineTagSelector='Tight' # 1=tight, 2=medium, 3=loose
ZeeCounts.OppositeCharge=True
ZeeCounts.OfflineTagMinEt=25
ZeeCounts.TagTrigger="e24_tight_iloose"
ZeeCounts.ProbeTriggerList=['e28_tight_iloose','e24_medium_iloose','e28_lhtight_iloose','e24_tight_iloose']
       
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaAnalysisAlg
theJob += TrigEgammaAnalysisAlg("alg",OutputLevel=VERBOSE)
alg = theJob.alg

alg.Tools += [ ZeeCounts ]

