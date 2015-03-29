include.block ("JetMissingEtTagTools/JetMissingEtTagTool_jobOptions.py")

########### Jet and MissingET Tag options ################

from JetRec.JetRecCalibrationFinder import jrcf
JetCalibrationTool = jrcf.find("AntiKt", 0.4, "LCTopo", "aj", "reco", "auto")
#if not hasattr(ToolSvc, JetCalibrationTool):
ToolSvc += JetCalibrationTool 

from JetMissingEtTagTools.JetMissingEtTagToolsConf import \
JetMissingEtIdentificationTagTool as ConfiguredJetIdTagTool
JetMissingEtIdentificationTagTool = ConfiguredJetIdTagTool(
     JetContainer    = "AntiKt4LCTopoJets",
     BadEtCut           = 20.0*GeV,
     MissingEtObject = "MET_RefFinal",
     JetCalibrationTool = JetCalibrationTool)

ToolSvc += JetMissingEtIdentificationTagTool
