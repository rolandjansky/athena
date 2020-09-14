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

# configuration of the truth matching tool
tool1.TauTruthMatchingTool.TruthElectronContainerName = "TruthElectrons"
tool1.TauTruthMatchingTool.TruthMuonContainerName = "MuonTruthParticles"
tool1.TauTruthMatchingTool.WriteTruthTaus = True

# Trigger loading of TauAnalysisTools to make ID enums visible
import cppyy
try:
    print("Successfully loaded TauAnalysisToolsDict")
    cppyy.load_library('libTauAnalysisToolsDict')
except:
    print("Could not load TauAnalysisToolsDict")
    pass
from ROOT import TauAnalysisTools

# configuration of the 'primitive' tau selection
tool1.PrimitiveTauSelectionTool.ConfigPath = ""
tool1.PrimitiveTauSelectionTool.SelectionCuts \
    = int(TauAnalysisTools.CutAbsEta | TauAnalysisTools.CutAbsCharge | 
          TauAnalysisTools.CutNTrack)
tool1.PrimitiveTauSelectionTool.PtMin = 0.0
tool1.PrimitiveTauSelectionTool.JetIDWP = TauAnalysisTools.JETIDNONE
tool1.PrimitiveTauSelectionTool.EleOLR = False
tool1.PrimitiveTauSelectionTool.NTracks = (0, 1, 2, 3, 4, 5)
tool1.PrimitiveTauSelectionTool.AbsCharges = (0, 1, 2, 3)
tool1.PrimitiveTauSelectionTool.AbsEtaRegion = (0.0, 10.0)

# configuration of the 'nominal' tau selection
tool1.NominalTauSelectionTool.ConfigPath = ""
tool1.NominalTauSelectionTool.SelectionCuts \
    = int(TauAnalysisTools.CutPt | TauAnalysisTools.CutAbsEta | 
          TauAnalysisTools.CutAbsCharge | TauAnalysisTools.CutNTrack)
tool1.NominalTauSelectionTool.PtMin = 20.0
tool1.NominalTauSelectionTool.JetIDWP = TauAnalysisTools.JETIDNONE
tool1.NominalTauSelectionTool.EleOLR = False
tool1.NominalTauSelectionTool.NTracks = (0, 1, 2, 3, 4, 5)
tool1.NominalTauSelectionTool.AbsCharges = (0, 1, 2, 3)
tool1.NominalTauSelectionTool.AbsEtaRegion = (0.0, 1.37, 1.52, 2.5)

monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.AthenaMonTools += [ tool1 ]

