from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSecVertexFinder.set_Value_and_Lock(False)
from AthenaCommon.AppMgr import ToolSvc


athenaCommonFlags.FilesInput=["/afs/cern.ch/user/a/asgbase/patspace/xAODs/r6630/mc15_13TeV.361106.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zee.recon.AOD.e3601_s2576_s2132_r6630_tid05358812_00/AOD.05358812._000010.pool.root.1"]
athenaCommonFlags.EvtMax=10
#athenaCommonFlags.EvtMax=-1
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
include ("RecExCommon/RecExCommon_topOptions.py")
ToolSvc.TrigDecisionTool.TrigDecisionKey='xTrigDecision'
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ToolSvc
theJob = AlgSequence()
 
ToolSvc += CfgMgr.Trig__MatchingTool("MyMatchingTool",OutputLevel=DEBUG)


#algseq = CfgMgr.AthSequencer("AthAlgSeq")                              #gets the main AthSequencer
#algseq += CfgMgr.TestMatchingToolAlg()                                 #adds an instance of your alg to it

 
from TriggerMatchingTool.TriggerMatchingToolConf import TestMatchingToolAlg
alg = TestMatchingToolAlg()
theJob += alg
 
include("TriggerTest/TriggerTestCommon.py")

