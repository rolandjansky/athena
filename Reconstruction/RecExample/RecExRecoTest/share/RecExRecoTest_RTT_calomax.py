include ("RecExRecoTest/RecExRecoTest_RTT_common.py")


jp.AthenaCommonFlags.PoolESDOutput="muon_ESD.pool.root"
rec.doTrigger=False
#obsolete doAODLVL1False
#rec.doWriteESD=False # uncomment if do not write ESD
rec.doAOD=False
rec.doWriteAOD=False # uncomment if do not write AOD
rec.doWriteTAG=False # uncomment if do not write TAG


from RecExConfig.RecFlags import rec

rec.doForwardDet=False
rec.doInDet=False
rec.doMuon=False
rec.doCalo=True

# If tracking is off, this also needs to be off.
rec.doEgamma = False


include ("RecExCond/RecExCommon_flags.py")
rec.AutoConfiguration = ['FieldAndGeo']
import RecExConfig.AutoConfiguration as auto
auto.ConfigureFromListOfKeys(rec.AutoConfiguration())


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")


include ("RecExRecoTest/RecExRecoTest_RTT_common_postOptions.py")
