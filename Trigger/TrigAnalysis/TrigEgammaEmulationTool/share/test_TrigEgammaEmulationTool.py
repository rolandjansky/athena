from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSecVertexFinder.set_Value_and_Lock(False)
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

import os

dirtouse=str()
finallist = []

if 'FILE' in dir():
  finallist.append(FILE)
elif 'DIR' in dir():
  #dirtouse='/afs/cern.ch/work/j/jolopezl/datasets/data15_13TeV.00284285.physics_Main.merge.AOD.f662_m1453_r7851_p2565/'
  dirtouse=DIR
  while(dirtouse.endswith('/')):
    dirtouse=dirtouse.rstrip('/')
  listfiles=os.listdir(dirtouse)
  for ll in listfiles:
    finallist.append(dirtouse+'/'+ll)
elif 'RTT' in dir():
  rttfile='root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/rtt/prod/rtt/'+RTT+'/x86_64-slc6-gcc49-opt/offline/TrigEgammaValidation/RDOtoAOD_MC_transform_Zee_25ns_pileup/AOD.Zee.25ns.pileup.pool.root'
  finallist.append(rttfile)
else:
  default_file='/afs/cern.ch/work/j/jolopezl/datasets/egamma/valid1.147406.PowhegPythia8_AZNLO_Zee.recon.AOD.e3099_s2578_r7514/AOD.07499854._000097.pool.root.1' 
  finallist.append(default_file)

if 'NOV' in dir():
  nov=NOV
else:
  nov=-1

## Athena
athenaCommonFlags.FilesInput=finallist
athenaCommonFlags.EvtMax=nov
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
# triggerList = ['e24_lhmedium_L1EM18VH', 'e24_lhmedium_L1EM20VH']
validation = [
              #'HLT_e24_lhmedium_L1EM18VH',
              #'HLT_e24_lhmedium_iloose_L1EM18VH',
              'HLT_e24_lhmedium_nod0_iloose',
              #'HLT_e28_lhtight_nod0_iloose',
              #'HLT_e28_lhtight_nod0_ringer_iloose',
              #'HLT_e17_etcut_L1EM15',
              #'HLT_e0_perf_L1EM15'
              ]
triggerList = validation

include("RecExCommon/RecExCommon_topOptions.py")

from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# TDT
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
ToolSvc.TrigDecisionTool.TrigDecisionKey='xTrigDecision'

from TrigEgammaMatchingTool.TrigEgammaMatchingToolConf import Trig__TrigEgammaMatchingTool
EgammaMatchTool = Trig__TrigEgammaMatchingTool("MatchingTool")
ToolSvc += EgammaMatchTool

from egammaRec.Factories  import ToolFactory,FcnWrapper,AlgFactory, getPropertyValue
from TrigEgammaEmulationTool.TrigEgammaEmulationToolConfig import TrigEgammaEmulationTool

Emulator = TrigEgammaEmulationTool(name = "TrigEgammaEmulationTool", 
                                   TriggerList = triggerList,
                                   OutputLevel = 0)

from TrigEgammaEmulationTool.TrigEgammaEmulationToolConf import Trig__TrigEgammaEmulationToolTest
test = Trig__TrigEgammaEmulationToolTest()
test.TrigEgammaEmulationTool = Emulator
test.TrigEgammaMatchingTool  = EgammaMatchTool
test.TriggerList             = triggerList
test.OutputLevel             = 1
theJob += test




from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.MessageSvc.defaultLimit = 9999999
 
include("TriggerTest/TriggerTestCommon.py")

