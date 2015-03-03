include.block ("TauTagTools/TauIdentificationTagTool_jobOptions.py")

########### Tau Identification tag options ################

from TauTagTools.TauTagToolsConf import \
TauIdentificationTagTool as ConfiguredTauIdTagTool
TauIdentificationTagTool=ConfiguredTauIdTagTool(
     Container     = "TauJets",
     EtCut         = 15.0*GeV,
     Likelihood    = -6.0,
     HadEnergyFrac = 0.9)
ToolSvc += TauIdentificationTagTool
