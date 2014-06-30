include.block ("EventTagUtils/EventInfoTagTool_jobOptions.py")

########### Event Tag Options ################
#####################################################

from EventTagUtils.EventTagUtilsConf import EventInfoTagTool as ConfiguredEventInfoTagTool
EventInfoTagTool=ConfiguredEventInfoTagTool(
     IncludeEventFlag = True,
     IncludeExtras = True)
ToolSvc = Service( "ToolSvc" )
ToolSvc += EventInfoTagTool
