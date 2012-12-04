include.block ("SUSYPhysTagTools/SUSYPhysTagTool_jobOptions.py")

########### SUSYPhysics analysis Tag options ################

from SUSYPhysTagTools.SUSYPhysTagToolsConf import \
SUSYPhysTagTool as ConfiguredSUSYPhysTagTool
SUSYPhysTagTool=ConfiguredSUSYPhysTagTool(
     JetContainer = "AntiKt4TopoJets",
     EtCut        = 15.0*GeV)
ToolSvc += SUSYPhysTagTool




