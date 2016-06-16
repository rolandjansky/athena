from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSecVertexFinder.set_Value_and_Lock(False)
from AthenaCommon.AppMgr import ToolSvc
import os

localRunOnFolder='/tmp/jodafons/user.jodafons.mc14_13TeV.147406.PowhegPythia8_AZNLO_Zee.recon.RDO.rel20.1.0.4.e3059_s1982_s2008_r5993_rr0104_AOD/'

#localRunOnFolder='/tmp/jodafons/user.jodafons.mc14_13TeV.129160.Pythia8_AU2CTEQ6L1_perf_JF17.recon.RDO.rel20.1.0.4.e3084_s2044_s2008_r5988.rr0104_AOD/'
files=[]
# Put dir for your data here:
f = os.popen('ls '+localRunOnFolder)
for j in f:
  i = j[0:-1]
  files += [localRunOnFolder+i]
  files.sort()



DoNtuple=True
DoOfflineDump=False
IsBackground=False
athenaCommonFlags.FilesInput=files
athenaCommonFlags.EvtMax=100
rec.readAOD=True
# switch off detectors
rec.doForwardDet=False
rec.doInDet=False
rec.doCalo=False
rec.doMuon=False
rec.doEgamma=False
rec.doTrigger = True; recAlgs.doTrigger=False # disable trigger (maybe necessary if detectors switched off)
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

# TDT
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
ToolSvc.TrigDecisionTool.TrigDecisionKey='xTrigDecision'


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
monMan= AthenaMonManager( "PhysValMonManager")
monMan.FileKey = "PhysVal"
topSequence += monMan

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["PhysVal DATAFILE='PhysVal.root' OPT='RECREATE'"]



from AthenaCommon.AppMgr import ToolSvc
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaEmulationTool
from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaNavAnalysisTool,\
     TrigEgammaNavTPAnalysisTool, TrigEgammaNavNtuple, TrigEgammaNavTPNtuple
from TrigEgammaAnalysisTools.TrigEgammaProbelist import default, probeListLowMidPtPhysicsTriggers # to import probelist

#from TrigHLTMonitoring.HLTMonTriggerList import hltmonList # import MaM trigger list not available!!!!
import TriggerMenu.menu.Physics_pp_v5 as physics_menu
import TriggerMenu.menu.MC_pp_v5 as mc_menu 
from TriggerJobOpts.TriggerFlags import TriggerFlags

physics_menu.setupMenu()
mc_menu.setupMenu()
egammaMenu = TriggerFlags.EgammaSlice.signatures()
egammaChains = []
l1Items = []
addItems = []
for egchain in egammaMenu:
    egammaChains.append(egchain[0])
    l1Items.append(egchain[1])
    addItems.append(egchain[3])

# Set list to full menu
#probelist = egammaChains
probelist = ['e24_lhmedium_L1EM20VH','e24_lhmedium_ringer_L1EM20VH','e24_lhmedium_L1EM18VH','e24_lhmedium_ringer_L1EM18VH','e24_lhmedium_ringer_perf_L1EM18VH', 'e24_lhmedium_ringer_perf_L1EM20VH']
basePath = 'Trigger/HLT/Egamma/'
tagItems = ['e26_lhtight_iloose']


#from LumiBlockComps.LuminosityToolDefault import LuminosityToolOnline
#lumiOnlineTool = LuminosityToolOnline("LuminosityToolOnline")
#LumiTool = LuminosityToolOnline()
#ToolSvc += lumiOnlineTool
#ToolSvc += LumiTool

OutputLevel=1
if not IsBackground:
  Analysis = TrigEgammaNavAnalysisTool(name='NavSignalAnalysis',
          DirectoryPath=basePath+'SignalAnalysis',
          TriggerList=probelist, 
          UseLuminosityTool=False,
          #LuminosityTool=LumiTool,
          ForcePidSelection=False,
          ForceProbeIsolation=False,
          ForceEtThreshold=False,
          RemoveCrack=False,
          ForceFilterSelection=True,
          ElectronFilterType="LHTight",
          File="PhysVal",
          OutputLevel=OutputLevel)

  TPAnalysis = TrigEgammaNavTPAnalysisTool(name='NavTPAnalysis',
          DirectoryPath=basePath+'TPAnalysis',
          UseLuminosityTool=False,
          #LuminosityTool=LumiTool,
          TriggerList=probelist, 
          File="PhysVal",
          TagTriggerList=tagItems,
          OutputLevel=OutputLevel)

  Tools=["TrigEgammaNavAnalysisTool/NavSignalAnalysis", "TrigEgammaNavTPAnalysisTool/NavTPAnalysis"]

  if DoNtuple:
    #extraction data for training
    Ntuple    = TrigEgammaNavNtuple(name="NavSignalNtuple",
            DirectoryPath=basePath+'ZeeNtuple',
            TriggerList=probelist,
            UseLuminosityTool=False,
            #LuminosityTool=LumiTool,
            DoOfflineDump=DoOfflineDump,
            ForcePidSelection=False,
            ForceProbeIsolation=False,
            ForceEtThreshold=False,
            RemoveCrack=False,
            ForceFilterSelection=True,
            ElectronFilterType="LHTight",
            File="PhysVal",
            OutputLevel=OutputLevel)

    TPNtuple  = TrigEgammaNavTPNtuple(name="NavTPNtuple",
            DirectoryPath=basePath+'TPNtuple',
            TriggerList=probelist,
            UseLuminosityTool=False,
            #LuminosityTool=LumiTool,
            File="PhysVal",
            TagTriggerList=tagItems,
            OutputLevel=OutputLevel)

    Tools+=["TrigEgammaNavTPNtuple/NavTPNtuple","TrigEgammaNavNtuple/NavSignalNtuple"] 

else:
  Analysis = TrigEgammaNavAnalysisTool(name='NavBackgroundAnalysis',
        DirectoryPath=basePath+'BackgroundAnalysis',
        TriggerList=probelist, 
        ForcePidSelection=False,
        ForceProbeIsolation=False,
        UseLuminosityTool=False,
        #LuminosityTool=LumiTool,
        ForceEtThreshold=False,
        RemoveCrack=False,
        ForceFilterSelection=True,
        ElectronFilterType="!Truth",
        File="PhysVal",
        OutputLevel=OutputLevel)

  Tools=["TrigEgammaNavAnalysisTool/NavBackgroundAnalysis"] 

  if DoNtuple:
    Ntuple    = TrigEgammaNavNtuple(name="NavBackgroundNtuple",
        DirectoryPath=basePath+'BackgroundNtuple',
        TriggerList=probelist,
        DoOfflineDump=DoOfflineDump,
        UseLuminosityTool=False,
        #LuminosityTool=LumiTool,
        ForcePidSelection=False,
        ForceProbeIsolation=False,
        ForceEtThreshold=False,
        RemoveCrack=False,
        ForceFilterSelection=True,
        ElectronFilterType="!Truth",
        File="PhysVal",
        OutputLevel=OutputLevel)
    
    Tools+=["TrigEgammaNavNtuple/NavBackgroundNtuple"] 

from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaPhysValMonTool
TrigEgammaPhysValMonTool = TrigEgammaPhysValMonTool( name = "TrigEgammaPhysValMonTool", 
          histoPathBase=basePath,
          Tools=Tools)

ToolSvc += TrigEgammaPhysValMonTool
#!*****************************************************************************************
monMan.AthenaMonTools += [ "TrigEgammaPhysValMonTool" ]
    



