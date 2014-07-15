include.block ("TauTagTools/TauJetTagTool_jobOptions.py")

########### TauJet tag options ################

from TauTagTools.TauTagToolsConf import \
TauJetTagTool as ConfiguredTauJetTagTool
TauJetTagTool=ConfiguredTauJetTagTool(
     Container          = "TauRecContainer",
     EtCut              = 20.0*GeV,
     TauDetailContainer = "TauRecDetailsContainer")
ToolSvc += TauJetTagTool
