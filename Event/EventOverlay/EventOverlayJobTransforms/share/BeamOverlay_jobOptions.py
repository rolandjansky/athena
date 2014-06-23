
include.block ( "EventOverlayJobTransforms/BeamOverlay_jobOptions.py" )

from OverlayCommonAlgs.OverlayFlags import OverlayFlags

from AthenaCommon.Resilience import treatException,protectedInclude

if OverlayFlags.doBCM():
    protectedInclude( "InDetBCM_EventAthenaPool/InDetBCM_EventAthenaPool_joboptions.py" )
    from AthenaCommon.AlgSequence import AlgSequence
    job = AlgSequence()
    from AthenaCommon import CfgGetter
    job += CfgGetter.getAlgorithm("BCM_Digitization/BCM_Digitization", tryDefaultConfigurable=True)
    job.BCM_Digitization.DigitizationTool.EvtStore = "BkgEvent_0_SG"

