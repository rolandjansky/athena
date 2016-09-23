
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
# input
athenaCommonFlags.PoolRDOInput = runArgs.inputRDOFile
athenaCommonFlags.BSRDOInput = runArgs.inputBS_TRIG_OLDFile
# output
athenaCommonFlags.PoolRDOOutput = runArgs.outputRDO_TRIG_OLDFile

include("TrigSimTransforms/MergingEventLoopMgr_TriggerBSandRDOtoRDO.py")
