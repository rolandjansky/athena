
include ("RecExRecoTest/RecExRecoTest_RTT_common.py")

jp.AthenaCommonFlags.PoolESDOutput="id_ESD.pool.root"
rec.doTrigger=False
#rec.doWriteESD=False # uncomment if do not write ESD
rec.doAOD=False
rec.doWriteAOD=False # uncomment if do not write AOD
rec.doWriteTAG=False # uncomment if do not write TAG

from RecExConfig.RecFlags import rec
rec.doForwardDet=False
rec.doInDet=True
rec.doMuon=False
rec.doCalo=False
rec.doEgamma=False
rec.doMuonCombined=False
rec.doJetMissingETTag=False
rec.doTau=False



# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")


include ("RecExRecoTest/RecExRecoTest_RTT_common_postOptions.py")
