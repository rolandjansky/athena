include.block ("EventTagUtils/GlobalEventTagTool_jobOptions.py")

########### Global Event Tag Options ################
# Independent from IncludeEventFlag, IncludeVertexFlag: event info (event #, run#, etc) 
# IncludeVertexFlag= true                             : event + vertex info
# IncludeEventFlag = true, IncludeVertexFlag= true    : event + vertex + 7 detector eventflags
#####################################################

include ("EventTagUtils/EventInfoTagTool_jobOptions.py")

from EventTagUtils.EventTagUtilsConf import GlobalEventTagTool as ConfiguredGlobalEventTagTool
GlobalEventTagTool=ConfiguredGlobalEventTagTool(
    CaloClusterContainer = "CaloCalTopoCluster",
    TrackContainer       = "TrackParticleCandidate",
    VertexContainer      = "VxPrimaryCandidate",
    IncludeEventFlag     = True,
    IncludeVertexFlag    = True,
    UseMC                = False,
    VertexTrackCut       = 4
    )
ToolSvc = Service( "ToolSvc" )
ToolSvc += GlobalEventTagTool
