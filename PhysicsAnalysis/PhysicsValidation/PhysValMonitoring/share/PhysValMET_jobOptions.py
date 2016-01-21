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
from AthenaCommon import CfgMgr
jvtTool = CfgMgr.JetVertexTaggerTool('JVTTool')
ToolSvc += jvtTool
tool1.JVTTool = jvtTool

mettool = CfgMgr.met__METMaker('METMaker')
ToolSvc += mettool
tool1.METMaker = mettool

museltool = CfgMgr.CP__MuonSelectionTool("MuonSelectionTool",
                                       MuQuality=1, # Medium
                                       MaxEta=2.4)
ToolSvc += museltool
tool1.MuonSelectionTool = museltool

eseltool = CfgMgr.AsgElectronLikelihoodTool("ElectronLHSelectionTool",
                                            ConfigFile="ElectronPhotonSelectorTools/offline/mc15_20150429/ElectronLikelihoodMediumOfflineConfig2015.conf")
ToolSvc += eseltool
tool1.ElectronLHSelectionTool = eseltool

from ROOT import egammaPID
phoseltool = CfgMgr.AsgPhotonIsEMSelector("PhotonIsEMSelectionTool",
                                             isEMMask=egammaPID.PhotonTight,
                                             ConfigFile="ElectronPhotonSelectorTools/offline/mc15_20150429/PhotonIsEMTightSelectorCutDefs.conf")
ToolSvc += phoseltool
tool1.PhotonIsEMSelectionTool = phoseltool

tauseltool = CfgMgr.TauAnalysisTools__TauSelectionTool("TauSelectionTool")
ToolSvc += tauseltool
tool1.TauSelectionTool = tauseltool

ToolSvc += tool1

monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.AthenaMonTools += [ tool1 ]
