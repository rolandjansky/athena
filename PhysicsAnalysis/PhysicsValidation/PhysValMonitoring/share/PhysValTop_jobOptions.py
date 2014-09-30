from TopPhysValMonitoring.TopPhysValMonitoringConf import TopPhysVal__TopPhysValMonitoring
tool1 = TopPhysVal__TopPhysValMonitoring()
tool1.EnableLumi = False
tool1.OutputLevel = WARNING
tool1.DetailLevel = 10

tool1.METContainerName = "" # do not try to read MET as long as the proxy is missing

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += tool1

monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.AthenaMonTools += [ tool1 ]
