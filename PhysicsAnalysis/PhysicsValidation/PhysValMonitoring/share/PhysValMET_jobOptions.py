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

from ROOT import TauAnalysisTools
SelectionCuts = TauAnalysisTools.SelectionCuts

tauseltool = CfgMgr.TauAnalysisTools__TauSelectionTool("TauSelectionTool")
tool1.TauSelectionTool = tauseltool

tool1.TauSelectionTool.ConfigPath = ""
tool1.TauSelectionTool.SelectionCuts \
    = int(SelectionCuts.CutPt | SelectionCuts.CutAbsEta |
          SelectionCuts.CutAbsCharge | SelectionCuts.CutNTrack | SelectionCuts.CutJetIDWP)
tool1.TauSelectionTool.PtMin = 20.0
tool1.TauSelectionTool.JetIDWP = TauAnalysisTools.JETIDRNNMEDIUM
tool1.TauSelectionTool.NTracks = (1, 3)
tool1.TauSelectionTool.AbsCharge = 1
tool1.TauSelectionTool.AbsEtaRegion = (0.0, 1.37, 1.52, 2.5)

#check if we are running over DAOD_PHYSVAL or AOD
from PyUtils.MetaReader import read_metadata
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
try:
    input_file = svcMgr.EventSelector.InputCollections[0]
except AttributeError:
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    input_file = athenaCommonFlags.FilesInput()[0]
metadata = read_metadata(input_file)
metadata = metadata[input_file]  # promote all keys one level up

isDAOD_PHYSVAL=False
for class_name, name in metadata['metadata_items'].items():
    if name == 'EventStreamInfo':
        if "DAOD_PHYSVAL" in class_name :
            print ("Running on DAOD_PHYSVAL - will not add TTVA decorations.")
            isDAOD_PHYSVAL=True

tool1.InputIsDAOD = isDAOD_PHYSVAL

from RecExConfig.AutoConfiguration import IsInInputFile
tool1.DoMETRefPlots=IsInInputFile('xAOD::MissingETContainer',"MET_Reference_AntiKt4EMTopo")

monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.AthenaMonTools += [ tool1 ]
