# Digital clustering
# ToolSvc.InDetPixelClusterOnTrackTool.CorrectDigitalCentroid=True
# ToolSvc.InDetPixelClusterOnTrackTool.PositionStrategy=0
# ToolSvc.InDetPixelClusterOnTrackTool.ErrorStrategy=1
# ToolSvc.InDetPixelClusterOnTrackToolDigital.PositionStrategy=0
# ToolSvc.InDetPixelClusterOnTrackToolDigital.ErrorStrategy=1
# ToolSvc.InDetSCT_ClusterOnTrackTool.CorrectionStrategy=0
# ToolSvc.InDetSCT_ClusterOnTrackTool.ErrorStrategy=0

# Analogue Clustering

from AthenaCommon.AppMgr import ToolSvc

ToolSvc.InDetPixelClusterOnTrackTool.CorrectDigitalCentroid=False
ToolSvc.InDetPixelClusterOnTrackTool.PositionStrategy=1
ToolSvc.InDetPixelClusterOnTrackTool.ErrorStrategy=2
ToolSvc.InDetPixelClusterOnTrackTool.OutputLevel = VERBOSE
ToolSvc.InDetPixelClusterOnTrackTool.ITkAnalogueClustering = True
ToolSvc.InDetPixelClusterOnTrackToolDigital.PositionStrategy=1
ToolSvc.InDetPixelClusterOnTrackToolDigital.ErrorStrategy=2
ToolSvc.InDetPixelClusterOnTrackToolDigital.OutputLevel = VERBOSE
ToolSvc.InDetPixelClusterOnTrackToolDigital.ITkAnalogueClustering = True
ToolSvc.InDetSCT_ClusterOnTrackTool.CorrectionStrategy=1
ToolSvc.InDetSCT_ClusterOnTrackTool.ErrorStrategy=2
ToolSvc.InDetSCT_ClusterOnTrackTool.OutputLevel = VERBOSE

ServiceMgr.PixelOfflineCalibSvc.ITkAnalogueClustering = True
# This following line must be set, but it should be set via the command line, such that it is not hard-coded into a release here
#ServiceMgr.PixelOfflineCalibSvc.ITkAnalogueClusteringConstantsFile = "/afs/cern.ch/user/b/bsmart/public/ForITk/finalConfig.txt"

