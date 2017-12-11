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

mettoolTopo = CfgMgr.met__METMaker('METMaker_AntiKt4Topo',
                                   JetSelection="Default",
                                   DoPFlow=False)
ToolSvc += mettoolTopo
tool1.METMakerTopo = mettoolTopo

mettoolPFlow = CfgMgr.met__METMaker('METMaker_AntiKt4PFlow',
                                    JetSelection="PFlow",
                                    DoPFlow=True)
ToolSvc += mettoolPFlow
tool1.METMakerPFlow = mettoolPFlow

museltool = CfgMgr.CP__MuonSelectionTool("MuonSelectionTool",
                                         MuQuality=1, # Medium
                                         MaxEta=2.4,
                                         TurnOffMomCorr = True)
ToolSvc += museltool
tool1.MuonSelectionTool = museltool

eseltool = CfgMgr.AsgElectronLikelihoodTool("ElectronLHSelectionTool",
                                            WorkingPoint="MediumLHElectron")
ToolSvc += eseltool
tool1.ElectronLHSelectionTool = eseltool

phoseltool = CfgMgr.AsgPhotonIsEMSelector("PhotonIsEMSelectionTool",
                                           WorkingPoint="TightPhoton")
ToolSvc += phoseltool
tool1.PhotonIsEMSelectionTool = phoseltool

tauseltool = CfgMgr.TauAnalysisTools__TauSelectionTool("TauSelectionTool")
ToolSvc += tauseltool
tool1.TauSelectionTool = tauseltool

ToolSvc += tool1

monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.AthenaMonTools += [ tool1 ]
