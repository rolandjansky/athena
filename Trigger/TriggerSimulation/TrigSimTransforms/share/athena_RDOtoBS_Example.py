

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolRDOInput.set_Value_and_Lock(["step1.x.RDO.pool.root"])
athenaCommonFlags.BSRDOOutput.set_Value_and_Lock("step2.x.RDOBS.data")

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.triggerConfig.set_Value_and_Lock("MCRECO:MC_pp_v2_loose_mc_prescale")

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = "ATLAS-GEO-18-01-01"
globalflags.ConditionsTag = "OFLCOND-MC12-SDR-06"

include("TrigSimTransforms/athena_RDOtoBS.py")




