import os

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolRDOInput.set_Value_and_Lock([ os.getenv('INPUTRDO',"../digiRDO.pool.root") ])
athenaCommonFlags.BSRDOOutput.set_Value_and_Lock( os.getenv('OUTPUTBS',"digiBS.data") )

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.triggerConfig.set_Value_and_Lock("MCRECO:MC_pp_v2_loose_mc_prescale")

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag   = os.getenv('DIGICON',"OFLCOND-MC12-SDR-06")
globalflags.DetDescrVersion = os.getenv('DIGIGEO',"ATLAS-GEO-18-01-01")

include("TrigSimTransforms/athena_RDOtoBS.py")




