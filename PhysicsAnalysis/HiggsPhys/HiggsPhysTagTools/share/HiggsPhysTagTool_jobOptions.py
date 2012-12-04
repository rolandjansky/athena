include.block ("HiggsPhysTagTools/HiggsPhysTagTool_jobOptions.py")

from EventTagAlgs.EventTagGlobal import EventTagGlobal

if not EventTagGlobal.InitEventTagBuilder:
    raise RunTimeError('EventTagBuilder has not been initialized')

########### HiggsPhysics analysis tag options ################

from HiggsPhysTagTools.HiggsPhysTagToolsConf import \
HiggsPhysTagTool as ConfiguredHiggsPhysTagTool
HiggsPhysTagTool=ConfiguredHiggsPhysTagTool(
     JetContainer = "AntiKt4TopoJets",
     EtCut        = 15.*GeV)
ToolSvc += HiggsPhysTagTool

