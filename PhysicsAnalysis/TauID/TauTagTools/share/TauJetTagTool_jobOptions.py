
include.block ("TauTagTools/TauJetTagTool_jobOptions.py")

########### TauJet tag options ################

from TauAnalysisTools.TauAnalysisToolsConf import TauAnalysisTools__TauSelectionTool

# NB: numerical values for SelectionCuts argument are defined here:
#   https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-00-01-03/TauAnalysisTools/TauSelectionTool.h#L53

EleBDTLooseTauSelectionTool = TauAnalysisTools__TauSelectionTool("EleBDTLooseTauSelectionTool")
EleBDTLooseTauSelectionTool.SelectionCuts = 1<<8 # CutEleBDTWP
EleBDTLooseTauSelectionTool.EleBDTWP      = 2    # ELEIDBDTLOOSE
ToolSvc += EleBDTLooseTauSelectionTool

EleBDTMediumTauSelectionTool = TauAnalysisTools__TauSelectionTool("EleBDTMediumTauSelectionTool")
EleBDTMediumTauSelectionTool.SelectionCuts = 1<<8 # CutEleBDTWP
EleBDTMediumTauSelectionTool.EleBDTWP      = 3    # ELEIDBDTMEDIUM
ToolSvc += EleBDTMediumTauSelectionTool

EleBDTTightTauSelectionTool = TauAnalysisTools__TauSelectionTool("EleBDTTightTauSelectionTool")
EleBDTTightTauSelectionTool.SelectionCuts = 1<<8 # CutEleBDTWP
EleBDTTightTauSelectionTool.EleBDTWP      = 4    # ELEIDBDTTIGHT
ToolSvc += EleBDTTightTauSelectionTool

MuonVetoTauSelectionTool = TauAnalysisTools__TauSelectionTool("MuonVetoTauSelectionTool")
MuonVetoTauSelectionTool.SelectionCuts = 1<<9 # CutMuonVeto
ToolSvc += MuonVetoTauSelectionTool

JetBDTSigLooseTauSelectionTool = TauAnalysisTools__TauSelectionTool("JetBDTSigLooseTauSelectionTool")
JetBDTSigLooseTauSelectionTool.SelectionCuts = 1<<6 # CutJetIDWP
JetBDTSigLooseTauSelectionTool.JetIDWP       = 2    # JETIDBDTLOOSE
ToolSvc += JetBDTSigLooseTauSelectionTool

JetBDTSigMediumTauSelectionTool = TauAnalysisTools__TauSelectionTool("JetBDTSigMediumTauSelectionTool")
JetBDTSigMediumTauSelectionTool.SelectionCuts = 1<<6 # CutJetIDWP
JetBDTSigMediumTauSelectionTool.JetIDWP       = 3    # JETIDBDTMEDIUM
ToolSvc += JetBDTSigMediumTauSelectionTool

JetBDTSigTightTauSelectionTool = TauAnalysisTools__TauSelectionTool("JetBDTSigTightTauSelectionTool")
JetBDTSigTightTauSelectionTool.SelectionCuts = 1<<6 # CutJetIDWP
JetBDTSigTightTauSelectionTool.JetIDWP       = 4    # JETIDBDTTIGHT
ToolSvc += JetBDTSigTightTauSelectionTool

# JetLLHSigLooseTauSelectionTool = TauAnalysisTools__TauSelectionTool("JetLLHSigLooseTauSelectionTool")
# JetLLHSigLooseTauSelectionTool.SelectionCuts = 1<<6 # CutJetIDWP
# JetLLHSigLooseTauSelectionTool.JetIDWP       = 7    # JETIDLLHLOOSE
# ToolSvc += JetLLHSigLooseTauSelectionTool

# JetLLHSigMediumTauSelectionTool = TauAnalysisTools__TauSelectionTool("JetLLHSigMediumTauSelectionTool")
# JetLLHSigMediumTauSelectionTool.SelectionCuts = 1<<6 # CutJetIDWP
# JetLLHSigMediumTauSelectionTool.JetIDWP       = 8    # JETIDLLHMEDIUM
# ToolSvc += JetLLHSigMediumTauSelectionTool

# JetLLHSigTightTauSelectionTool = TauAnalysisTools__TauSelectionTool("JetLLHSigTightTauSelectionTool")
# JetLLHSigTightTauSelectionTool.SelectionCuts = 1<<6 # CutJetIDWP
# JetLLHSigTightTauSelectionTool.JetIDWP       = 9    # JETIDLLHTIGHT
# ToolSvc += JetLLHSigTightTauSelectionTool

EleOLRTauSelectionTool = TauAnalysisTools__TauSelectionTool("EleOLRTauSelectionTool")
EleOLRTauSelectionTool.SelectionCuts = 1<<10 # CutEleOLR
EleOLRTauSelectionTool.EleOLR        = True
ToolSvc += EleOLRTauSelectionTool

from TauTagTools.TauTagToolsConf import \
TauJetTagTool as ConfiguredTauJetTagTool
TauJetTagTool=ConfiguredTauJetTagTool(
     Container          = "TauJets",
     EtCut              = 20.0*GeV,
     EleBDTLooseTauSelectionTool     = EleBDTLooseTauSelectionTool,
     EleBDTMediumTauSelectionTool    = EleBDTMediumTauSelectionTool,
     EleBDTTightTauSelectionTool     = EleBDTTightTauSelectionTool,
     MuonVetoTauSelectionTool        = MuonVetoTauSelectionTool,
     JetBDTSigLooseTauSelectionTool  = JetBDTSigLooseTauSelectionTool,
     JetBDTSigMediumTauSelectionTool = JetBDTSigMediumTauSelectionTool,
     JetBDTSigTightTauSelectionTool  = JetBDTSigTightTauSelectionTool,
     # JetLLHSigLooseTauSelectionTool  = JetLLHSigLooseTauSelectionTool,
     # JetLLHSigMediumTauSelectionTool = JetLLHSigMediumTauSelectionTool,
     # JetLLHSigTightTauSelectionTool  = JetLLHSigTightTauSelectionTool,
     EleOLRTauSelectionTool          = EleOLRTauSelectionTool
     )
ToolSvc += TauJetTagTool
