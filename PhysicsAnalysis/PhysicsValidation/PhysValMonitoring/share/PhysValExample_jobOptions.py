from PhysValMonitoring.PhysValMonitoringConf import PhysVal__PhysValExample
tool1 = PhysVal__PhysValExample()
tool1.EnableLumi = False
tool1.OutputLevel = WARNING
tool1.DetailLevel = 10

tool1.TauContainerName = "TauJets"
tool1.PhotonContainerName = "Photons"
tool1.ElectronContainerName = "Electrons"
tool1.METContainerName = "MET_Reference_AntiKt4LCTopo"
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += tool1

monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.AthenaMonTools += [ tool1 ]
