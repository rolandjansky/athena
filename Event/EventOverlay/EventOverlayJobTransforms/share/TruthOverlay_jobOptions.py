include.block ( "EventOverlayJobTransforms/TruthOverlay_jobOptions.py" )

from AthenaCommon import CfgGetter
from OverlayCommonAlgs.OverlayFlags import overlayFlags

# Copy McEventCollection
job += CfgGetter.getAlgorithm("CopyMcEventCollection")

# Copy jet truth
if not overlayFlags.isDataOverlay():
    job += CfgGetter.getAlgorithm("CopyInTimeJetTruthInfo")
    job += CfgGetter.getAlgorithm("CopyOutOfTimeJetTruthInfo")
