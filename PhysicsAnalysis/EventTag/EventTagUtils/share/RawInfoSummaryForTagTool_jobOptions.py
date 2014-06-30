include.block ("EventTagUtils/RawInfoSummaryForTagTool_jobOptions.py")

########### Raw Summary Tag Options ################
# Raw Summary Object Not in the AOD yet!

from EventTagUtils.EventTagUtilsConf import \
RawInfoSummaryForTagTool as ConfiguredRawInfoSummaryForTagTool
RawInfoSummaryForTagTool=ConfiguredRawInfoSummaryForTagTool(
     RawInfoSummaryKey = "")
ToolSvc += RawInfoSummaryForTagTool
