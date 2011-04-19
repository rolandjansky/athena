include.block ("DPDUtils/DPDTagTool_jobOptions.py")

########### DPD analysis Tag options ################

from DPDUtils.DPDUtilsConf import DPDTagTool as ConfiguredDPDTagTool
DPDTagTool=ConfiguredDPDTagTool(
   DESDSkimDecisionCollection="StreamESD_SkimDecisionsContainer",
#####   DAODSkimDecisionCollection="StreamAOD_SkimDecisionsContainer"
)

ToolSvc += DPDTagTool

