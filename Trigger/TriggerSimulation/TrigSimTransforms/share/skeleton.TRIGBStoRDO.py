
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
# input
athenaCommonFlags.PoolRDOInput      = runArgs.inputRDOFile
athenaCommonFlags.BSRDOInput        = runArgs.inputBS_TRIGFile
# output
athenaCommonFlags.PoolRDOOutput     = runArgs.outputRDO_TRIGFile

include("TrigSimTransforms/MergingEventLoopMgr_TriggerBSandRDOtoRDO.py")
