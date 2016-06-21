
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
# input
athenaCommonFlags.PoolRDOInput      = runArgs.inputRDOFile
athenaCommonFlags.BSRDOInput = runArgs.inputBS_TRIG_OLDFile
# output
athenaCommonFlags.PoolRDOOutput = runArgs.outputRDO_TRIG_OLDFile

# disable check of geometry tags
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.IgnoreTagSupport = True
GeoModelSvc.IgnoreTagDifference = True

include("TrigSimTransforms/MergingEventLoopMgr_TriggerBSandRDOtoRDO.py")
