from MissingEtDQA.MissingEtDQAConf import MissingEtDQA__PhysValMET
tool1 = MissingEtDQA__PhysValMET()
tool1.EnableLumi = False
tool1.OutputLevel = WARNING
tool1.DetailLevel = 10

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += tool1

monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.AthenaMonTools += [ tool1 ]
