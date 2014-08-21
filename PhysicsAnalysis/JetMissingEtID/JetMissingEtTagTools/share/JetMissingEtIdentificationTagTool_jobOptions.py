include.block ("JetMissingEtTagTools/JetMissingEtTagTool_jobOptions.py")

########### Jet and MissingET Tag options ################

from JetMissingEtTagTools.JetMissingEtTagToolsConf import \
JetMissingEtIdentificationTagTool as ConfiguredJetIdTagTool
JetMissingEtIdentificationTagTool = ConfiguredJetIdTagTool(
     JetContainer    = "AntiKt4LCTopoJets",
     BadEtCut           = 20.0*GeV,
     MissingEtObject = "MET_RefFinal")
ToolSvc += JetMissingEtIdentificationTagTool
