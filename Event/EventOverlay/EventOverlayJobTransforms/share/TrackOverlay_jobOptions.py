include.block ( "EventOverlayJobTransforms/TrackOverlay_jobOptions.py" )

from AthenaCommon import CfgGetter
from OverlayCommonAlgs.OverlayFlags import overlayFlags

job += CfgGetter.getAlgorithm('CopyPixelClusterContainer')
job += CfgGetter.getAlgorithm('CopySCT_ClusterContainer')
job += CfgGetter.getAlgorithm('CopyTRT_DriftCircleContainer')

trackContainers=['CombinedInDetTracks','DisappearingTracks','ResolvedForwardTracks','ResolvedLargeD0Tracks']
for container in trackContainers:
    job += CfgGetter.getAlgorithmClone('CopyTrackCollection'+container, 'CopyTrackCollection', collectionName=container)

from TrkEventCnvTools.TrkEventCnvToolsConf import Trk__EventCnvSuperTool
TrkEventCnvSuperTool = Trk__EventCnvSuperTool(name="EventCnvSuperTool")
TrkEventCnvSuperTool.DoTrackOverlay=True
ToolSvc+=TrkEventCnvSuperTool
