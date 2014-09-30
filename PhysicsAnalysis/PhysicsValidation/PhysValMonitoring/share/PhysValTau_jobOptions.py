from TauDQA.TauDQAConf import PhysValTau
tool1 = PhysValTau()
tool1.EnableLumi = False
tool1.OutputLevel = WARNING
tool1.DetailLevel = 10

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += tool1

monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.AthenaMonTools += [ tool1 ]
