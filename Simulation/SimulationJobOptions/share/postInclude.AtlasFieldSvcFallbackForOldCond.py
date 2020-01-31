from AthenaCommon.AppMgr import ServiceMgr
atlasFieldSvc = ServiceMgr.AtlasFieldSvc
try:
    atlasFieldSvc.UseMapsFromCOOL=False
except:
    printfunc ("WARNING postInclude.AtlasFieldSvcFallbackForOldCond.py will have no effect!")
#printfunc (atlasFieldSvc)
