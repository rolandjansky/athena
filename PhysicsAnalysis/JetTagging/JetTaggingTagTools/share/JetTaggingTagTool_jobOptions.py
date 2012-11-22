include.block ("JetTaggingTagTools/JetTaggingTagTool_jobOptions.py")


########### Flavor tagging analysis tag options ################

from JetTaggingTagTools.JetTaggingTagToolsConf import \
JetTaggingTagTool as ConfiguredJetTaggingTagTool
JetTaggingTagTool=ConfiguredJetTaggingTagTool(
     JetContainer = "AntiKt4TopoJets",
     EtCut        = 15.0*GeV)
ToolSvc += JetTaggingTagTool
