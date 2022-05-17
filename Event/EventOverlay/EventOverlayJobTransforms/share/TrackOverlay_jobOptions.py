include.block ( "EventOverlayJobTransforms/TrackOverlay_jobOptions.py" )

from AthenaCommon import CfgGetter
from OverlayCommonAlgs.OverlayFlags import overlayFlags

job += CfgGetter.getAlgorithm('CopyPixelClusterContainer')
job += CfgGetter.getAlgorithm('CopySCT_ClusterContainer')
job += CfgGetter.getAlgorithm('CopyTRT_DriftCircleContainer')

trackContainers=['CombinedInDetTracks','DisappearingTracks','ResolvedForwardTracks','ResolvedLargeD0Tracks']
for container in trackContainers:
    job += CfgGetter.getAlgorithmClone('CopyTrackCollection'+container, 'CopyTrackCollection', collectionName=container)

truthContainers=['DisappearingTracksDetailedTruth','ResolvedForwardTracksDetailedTruth','CombinedInDetTracksDetailedTruth','ResolvedLargeD0TracksDetailedTruth']
for container in truthContainers:
    job += CfgGetter.getAlgorithmClone('CopyDetailedTrackTruthCollection'+container, 'CopyDetailedTrackTruthCollection', collectionName=container)

prdTruthContainers=['PRD_MultiTruthTRT','PRD_MultiTruthPixel','PRD_MultiTruthSCT']
for container in prdTruthContainers:
    job += CfgGetter.getAlgorithmClone('CopyPRD_MultiTruthCollection'+container, 'CopyPRD_MultiTruthCollection', collectionName=container)

from TrkEventCnvTools.TrkEventCnvToolsConf import Trk__EventCnvSuperTool
TrkEventCnvSuperTool = Trk__EventCnvSuperTool(name="EventCnvSuperTool")
TrkEventCnvSuperTool.DoTrackOverlay=True
ToolSvc+=TrkEventCnvSuperTool
