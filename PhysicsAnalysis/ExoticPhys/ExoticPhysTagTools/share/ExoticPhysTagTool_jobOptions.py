include.block ("ExoticPhysTagTools/ExoticPhysTagTool_jobOptions.py")

from EventTagAlgs.EventTagGlobal import EventTagGlobal

if not EventTagGlobal.InitEventTagBuilder:
    raise RunTimeError('EventTagBuilder has not been initialized')

########### ExoticPhysics analysis tag options ################

from ExoticPhysTagTools.ExoticPhysTagToolsConf import \
ExoticPhysTagTool as ConfiguredExoticPhysTagTool
ExoticPhysTagTool=ConfiguredExoticPhysTagTool(
     JetContainer = "AntiKt4TopoJets",
     EtCut        = 15.0*GeV)
ToolSvc += ExoticPhysTagTool
