from MissingEtDQA.MissingEtDQAConf import MissingEtDQA__PhysValMET
tool1 = MissingEtDQA__PhysValMET()
tool1.EnableLumi = False
tool1.OutputLevel = WARNING
tool1.DetailLevel = 10
tool1.METContainerName = "MET_Reference_AntiKt4LCTopo"
from RecExConfig.RecFlags import rec as recFlags
if recFlags.doTruth():
    tool1.DoTruth = True

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += tool1

monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.AthenaMonTools += [ tool1 ]
