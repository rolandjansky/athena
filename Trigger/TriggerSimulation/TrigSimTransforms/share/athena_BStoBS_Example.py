

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.BSRDOInput.set_Value_and_Lock([os.getenv('INPUTBS',"digiBS.data")])
athenaCommonFlags.BSRDOOutput.set_Value_and_Lock(os.getenv('OUTPUTBS',"trigBS.data"))

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.triggerConfig.set_Value_and_Lock("MCRECO:MC_pp_v2_loose_mc_prescale")

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag   = os.getenv('DIGICON',"OFLCOND-MC12-SDR-06")
globalflags.DetDescrVersion = os.getenv('DIGIGEO',"ATLAS-GEO-18-01-01")


include("TrigSimTransforms/athena_BStoBS.py")




