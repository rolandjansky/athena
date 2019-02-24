
include.block ( "EventOverlayJobTransforms/BeamOverlay_jobOptions.py" )

from AthenaCommon.DetFlags import DetFlags

if DetFlags.overlay.BCM_on():
    if overlayFlags.isDataOverlay():
        job.InDetBCMRawDataProvider.RDOKey = overlayFlags.dataStore() + "+BCM_RDOs"

    from AthenaCommon import CfgGetter
    job += CfgGetter.getAlgorithm("BCM_OverlayDigitization")
