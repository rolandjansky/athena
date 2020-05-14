from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSecVertexFinder.set_Value_and_Lock(False)
from AthenaCommon.AppMgr import ToolSvc

import os

outputName = 'Validation_asdf'


athenaCommonFlags.FilesInput=['AOD.pool.root']
athenaCommonFlags.EvtMax=100
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
# main jobOption
triggerList = [
                  'HLT_e5_etcut',
                  ]
include ("RecExCommon/RecExCommon_topOptions.py")
ToolSvc.TrigDecisionTool.TrigDecisionKey='xTrigDecision'
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()
 
from TrigEgammaMatchingTool.TrigEgammaMatchingToolConf import Trig__TrigEgammaMatchingTool
matchtool = Trig__TrigEgammaMatchingTool("MatchingTool");
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += matchtool
 
from TrigEgammaMatchingTool.TrigEgammaMatchingToolConf import Trig__TrigEgammaMatchingToolTest
alg = Trig__TrigEgammaMatchingToolTest()
alg.TrigEgammaMatchingTool = matchtool
alg.TriggerList = triggerList
alg.OutputLevel = DEBUG
theJob += alg
 
include("TriggerTest/TriggerTestCommon.py")
