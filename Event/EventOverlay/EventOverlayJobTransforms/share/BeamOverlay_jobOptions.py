
include.block ( "EventOverlayJobTransforms/BeamOverlay_jobOptions.py" )

from AthenaCommon.DetFlags import DetFlags

from AthenaCommon.Resilience import treatException,protectedInclude

if DetFlags.overlay.BCM_on():
    protectedInclude( "InDetBCM_EventAthenaPool/InDetBCM_EventAthenaPool_joboptions.py" )
    from AthenaCommon.AlgSequence import AlgSequence
    job = AlgSequence()
    from AthenaCommon import CfgGetter
    job += CfgGetter.getAlgorithm("BCM_OverlayDigitization")


