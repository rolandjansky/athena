include.block( "UserAnalysisUtils/UserAnalysisOverlapCheckingTool_jobOptions.py" )

from UserAnalysisUtils.UserAnalysisUtilsConf import \
     UserAnalysisOverlapCheckingTool as ConfiguredUserAnalysisOverlapCheckingTool
UserAnalysisOverlapCheckingTool = ConfiguredUserAnalysisOverlapCheckingTool(

  OverlapDeltaR=0.2,
  OverlapDeltaRWithJets=0.3

    )

ToolSvc += UserAnalysisOverlapCheckingTool
print      UserAnalysisOverlapCheckingTool
