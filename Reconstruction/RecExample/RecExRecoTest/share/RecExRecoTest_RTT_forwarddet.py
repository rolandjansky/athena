include ("RecExRecoTest/RecExRecoTest_RTT_common.py")

jp.AthenaCommonFlags.PoolESDOutput="idcalo_ESD.pool.root"
rec.doTrigger=False
#rec.doWriteESD=False # uncomment if do not write ESD
rec.doAOD=False
rec.doWriteAOD=False # uncomment if do not write AOD
rec.doWriteTAG=False # uncomment if do not write TAG


from RecExConfig.RecFlags import rec
rec.doForwardDet=True
rec.doInDet=False
rec.doMuon=False
rec.doCalo=False

# If Calo/tracking is off, this needs to be off too.
from IsolationAlgs.IsoGetter import isoGetter
isoGetter (disable=True)


include ("RecExCond/RecExCommon_flags.py")
rec.AutoConfiguration = ['FieldAndGeo']
import RecExConfig.AutoConfiguration as auto
auto.ConfigureFromListOfKeys(rec.AutoConfiguration())

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

include ("RecExRecoTest/RecExRecoTest_RTT_common_postOptions.py")
