


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolRDOInput      = [ "step1.x.RDO.pool.root" ]
athenaCommonFlags.BSRDOInput        = [ "step3.x.RDOBS.data" ]
athenaCommonFlags.PoolRDOOutput     =   "step4.x.RDO.pool.root"

include("TrigSimTransforms/MergingEventLoopMgr_TriggerBSandRDOtoRDO.py")



