include.block ("StandardModelPhysTagTools/StandardModelPhysTagTool_jobOptions.py")

########### StandardModelPhysics analysis tag options ################

from StandardModelPhysTagTools.StandardModelPhysTagToolsConf import \
StandardModelPhysTagTool as ConfiguredSMTagTool
StandardModelPhysTagTool=ConfiguredSMTagTool(
     JetContainer = "AntiKt4TopoJets",
     EtCut        = 15.0*GeV)
ToolSvc += StandardModelPhysTagTool




