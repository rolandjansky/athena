# run almost empty event loop
include ("RecExRecoTest/RecExRecoTest_RTT_common.py")

from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs

rec.doForwardDet=False
rec.doInDet=False
rec.doCalo=False
rec.doMuon=False

rec.doTrigger = False; recAlgs.doTrigger=False # disable trigger (maybe necessary if detectors switched of)


rec.doTruth=False
rec.doESD = False # if false, all algorithms are switched off by defaults 
rec.doWriteESD=False # uncomment if do not write ESD
rec.doAOD=False
rec.doWriteAOD=False # uncomment if do not write AOD
rec.doWriteTAG=False # uncomment if do not write TAG

include ("RecExCond/RecExCommon_flags.py")
rec.AutoConfiguration = ['FieldAndGeo']
import RecExConfig.AutoConfiguration as auto
auto.ConfigureFromListOfKeys(rec.AutoConfiguration())

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")


include ("RecExRecoTest/RecExRecoTest_RTT_common_postOptions.py")
