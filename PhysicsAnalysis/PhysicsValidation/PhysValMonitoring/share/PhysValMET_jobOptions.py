from MissingEtDQA.MissingEtDQAConf import MissingEtDQA__PhysValMET
tool1 = MissingEtDQA__PhysValMET()
tool1.EnableLumi = False
tool1.OutputLevel = WARNING
tool1.DetailLevel = 10
from RecExConfig.RecFlags import rec as recFlags
if recFlags.doTruth():
    tool1.DoTruth = True

from AthenaCommon import CfgMgr
jvtToolEMTopo = CfgMgr.JetVertexTaggerTool('JVTToolEMTopo')
jvtToolPFlow = CfgMgr.JetVertexTaggerTool('JVTToolPFlow')
tool1.JVTToolEMTopo = jvtToolEMTopo
tool1.JVTToolEMPFlow = jvtToolPFlow
tool1.JVTToolEMTopo.JetContainer = "AntiKt4EMTopoJets"
tool1.JVTToolEMPFlow.JetContainer = "AntiKt4EMPFlowJets"

mettoolTopo = CfgMgr.met__METMaker('METMaker_AntiKt4Topo',
                                   JetSelection="Default",
                                   JetJvtMomentName="NewJvt",
                                   DoPFlow=False)
tool1.METMakerTopo = mettoolTopo

mettoolPFlow = CfgMgr.met__METMaker('METMaker_AntiKt4PFlow',
                                    JetSelection="PFlow",
                                    JetJvtMomentName="NewJvt",
                                    DoPFlow=True)
tool1.METMakerPFlow = mettoolPFlow

museltool = CfgMgr.CP__MuonSelectionTool("MuonSelectionTool",
                                         MuQuality=1, # Medium
                                         MaxEta=2.4,
                                         TurnOffMomCorr = True)
tool1.MuonSelectionTool = museltool

eseltool = CfgMgr.AsgElectronLikelihoodTool("ElectronLHSelectionTool",
                                            WorkingPoint="MediumLHElectron")
tool1.ElectronLHSelectionTool = eseltool

phoseltool = CfgMgr.AsgPhotonIsEMSelector("PhotonIsEMSelectionTool",
                                           WorkingPoint="TightPhoton")
tool1.PhotonIsEMSelectionTool = phoseltool

tauseltool = CfgMgr.TauAnalysisTools__TauSelectionTool("TauSelectionTool")
tool1.TauSelectionTool = tauseltool

monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.AthenaMonTools += [ tool1 ]
