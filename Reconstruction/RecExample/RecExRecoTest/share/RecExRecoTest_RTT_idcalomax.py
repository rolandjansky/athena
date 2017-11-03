include ("RecExRecoTest/RecExRecoTest_RTT_common.py")

jp.AthenaCommonFlags.PoolESDOutput="idcalo_ESD.pool.root"
rec.doTrigger=False
#rec.doWriteESD=False # uncomment if do not write ESD
rec.doAOD=False
rec.doWriteAOD=False # uncomment if do not write AOD
rec.doWriteTAG=False # uncomment if do not write TAG


from RecExConfig.RecFlags import rec
rec.doForwardDet=False
rec.doInDet=True
rec.doMuon=False
rec.doCalo=True

# If muons are not present, then eflow is not configured,
# which causes jet finding to fail.
rec.doJetMissingETTag = False


include ("RecExCond/RecExCommon_flags.py")
rec.AutoConfiguration = ['everything']
import RecExConfig.AutoConfiguration as auto
auto.ConfigureFromListOfKeys(rec.AutoConfiguration())


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")


include ("RecExRecoTest/RecExRecoTest_RTT_common_postOptions.py")
