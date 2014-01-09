include.block ("BPhysTagTools/BPhysTagTool_jobOptions.py")

from EventTagAlgs.EventTagGlobal import EventTagGlobal

if not EventTagGlobal.InitEventTagBuilder:
    raise RunTimeError('EventTagBuilder has not been initialized')

########### BPhysics analysis Tag options ################

from BPhysTagTools.BPhysTagToolsConf import BPhysTagTool as ConfiguredBPhysTagTool
BPhysTagTool = ConfiguredBPhysTagTool(
	)     
ToolSvc += BPhysTagTool
