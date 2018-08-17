include.block ( "EventOverlayJobTransforms/TruthOverlay_jobOptions.py" )

# McEventCollection copying.  Alghough logically it belongs to
# overlaying that is done later, it has to be scheduled before
# digitization algorithms as a workaround for bug #35465
from OverlayCommonAlgs.OverlayFlags import overlayFlags

from OverlayCommonAlgs.OverlayCommonAlgsConf import CopyMcEventCollection
job += CopyMcEventCollection()
if overlayFlags.isDataOverlay():
    job.CopyMcEventCollection.RealData = True

if not overlayFlags.isDataOverlay():
    from OverlayCommonAlgs.OverlayCommonAlgsConf import CopyTruthInfo
    job += CopyTruthInfo()
