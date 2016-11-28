from RecExConfig.RecFlags import rec as recFlags

from ZeeValidation.ZeeValidationConf import ZeeValidation__ZeeValidationMonitoringTool
tool1 = ZeeValidation__ZeeValidationMonitoringTool()
tool1.IsData = not recFlags.doTruth()
tool1.EnableLumi = False
tool1.OutputLevel = WARNING
tool1.DetailLevel = 10

tool1.PhotonContainerName = "Photons"
tool1.ElectronContainerName = "Electrons"
tool1.ElectronContainerFrwdName = "ForwardElectrons"
tool1.TruthParticleContainerName = "TruthParticles"
        
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += tool1

monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.AthenaMonTools += [ tool1 ]
