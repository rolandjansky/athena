from EgammaPhysValMonitoring.EgammaPhysValMonitoringConf import EgammaPhysValMonitoring__EgammaPhysValMonitoringTool
tool1 = EgammaPhysValMonitoring__EgammaPhysValMonitoringTool()
tool1.EnableLumi = False
tool1.OutputLevel = WARNING
tool1.DetailLevel = 10

tool1.PhotonContainerName = "Photons"
tool1.ElectronContainerName = "Electrons"
tool1.ElectronContainerFrwdName = "ForwardElectrons"
tool1.TruthParticleContainerName = "TruthParticles"

tool1.isMC = rec.doTruth()
from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifierCalo
tool1.MCTruthClassifier = MCTruthClassifierCalo

monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.AthenaMonTools += [ tool1 ]
