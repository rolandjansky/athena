from TauDQA.TauDQAConf import PhysValTau
tool1 = PhysValTau()
tool1.EnableLumi = False
tool1.OutputLevel = WARNING
tool1.DetailLevel = 10
from RecExConfig.RecFlags import rec as recFlags
if recFlags.doTruth():
    tool1.isMC = True

tool1.TauContainerName = "TauJets"
tool1.TruthParticleContainerName = "TruthParticles"

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += tool1

monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.AthenaMonTools += [ tool1 ]

