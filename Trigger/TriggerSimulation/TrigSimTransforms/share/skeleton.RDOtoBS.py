import logging
log = logging.getLogger("RDOtoBS")

log.info("="*20 + "Beginning of RDOtoBS" + "="*20)

# Input and output
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolRDOInput.set_Value_and_Lock(runArgs.inputRDOFile)
athenaCommonFlags.BSRDOOutput.set_Value_and_Lock(runArgs.outputBS_OLDFile)
athenaCommonFlags.EvtMax = -1
if hasattr(runArgs, "skipEvents"): athenaCommonFlags.SkipEvents.set_Value_and_Lock(runArgs.skipEvents)

# Trigger configuration
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.triggerConfig.set_Value_and_Lock(runArgs.triggerConfig)

# Detector geometry and conditions data
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion.set_Value_and_Lock(runArgs.geometryVersion)
globalflags.ConditionsTag.set_Value_and_Lock(runArgs.conditionsTag)

from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageOutputSvc
if hasattr(runArgs, "outputBSEformatVersion"):
    ByteStreamEventStorageOutputSvc.EformatVersion = runArgs.outputBSEformatVersion
if hasattr(runArgs, "outputBSEventStorageVersion"):
    ByteStreamEventStorageOutputSvc.EventStorageVersion = runArgs.outputBSEventStorageVersion

# disable check of geometry tags
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.IgnoreTagDifference = True

include("TrigSimTransforms/athena_RDOtoBS.py")


log.info("="*20 + "End of RDOtoBS" + "="*20)
