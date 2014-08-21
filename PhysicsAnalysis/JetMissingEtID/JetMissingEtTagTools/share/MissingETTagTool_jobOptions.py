include.block ("JetMissingEtTagTools/MissingETTagTool_jobOptions.py")


########### MissingET tag options ################

from JetMissingEtTagTools.JetMissingEtTagToolsConf import JetMissingEtTagTool as ConfiguredMissingEtTool
JetMissingEtTagTool=ConfiguredMissingEtTool()
ToolSvc += JetMissingEtTagTool
