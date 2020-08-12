
include.block ( "EventOverlayJobTransforms/BeamOverlay_jobOptions.py" )

from AthenaCommon.DetFlags import DetFlags

if DetFlags.overlay.BCM_on():
    if overlayFlags.isDataOverlay():
        if overlayFlags.isOverlayMT():
            job.InDetBCMRawDataProvider.RDOKey = overlayFlags.bkgPrefix() + "BCM_RDOs"
        else:
            job.InDetBCMRawDataProvider.RDOKey = overlayFlags.dataStore() + "+BCM_RDOs"

    from AthenaCommon import CfgGetter
    job += CfgGetter.getAlgorithm("BCM_OverlayDigitization")
    job += CfgGetter.getAlgorithm("BCMOverlay")
    if DetFlags.overlay.Truth_on():
        job += CfgGetter.getAlgorithm("BCMSDOOverlay")
