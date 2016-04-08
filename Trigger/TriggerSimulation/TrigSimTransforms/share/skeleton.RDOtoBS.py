import logging
log = logging.getLogger("RDOtoBS")

log.info("="*20 + "Beginning of RDOtoBS" + "="*20)

# Input and output
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolRDOInput.set_Value_and_Lock( runArgs.inputRDOFile )
athenaCommonFlags.BSRDOOutput.set_Value_and_Lock( runArgs.outputBS_OLDFile )
athenaCommonFlags.EvtMax = -1
athenaCommonFlags.SkipEvents.set_Value_and_Lock( runArgs.skipEvents )

# Trigger configuration
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.triggerConfig.set_Value_and_Lock(runArgs.triggerConfig)

# Detector geometry and conditions data
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = "ATLAS-GEO-18-01-01"
globalflags.ConditionsTag = "OFLCOND-MC12-SDR-06"

include("TrigSimTransforms/athena_RDOtoBS.py")


log.info("="*20 + "End of RDOtoBS" + "="*20)
