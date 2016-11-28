from JetTagDQA.JetTagDQAConf import JetTagDQA__PhysValBTag
tool1 = JetTagDQA__PhysValBTag()
tool1.EnableLumi = False
tool1.OutputLevel = WARNING
tool1.DetailLevel = 10
tool1.isData = True

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += tool1

monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.AthenaMonTools += [ tool1 ]
