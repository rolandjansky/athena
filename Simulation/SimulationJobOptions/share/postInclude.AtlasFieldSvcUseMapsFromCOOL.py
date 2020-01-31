from AthenaCommon.AppMgr import ServiceMgr
atlasFieldSvc = ServiceMgr.AtlasFieldSvc
try:
    atlasFieldSvc.UseMapsFromCOOL=True
except:
    printfunc ("WARNING postInclude.AtlasFieldSvcUseMapsFromCOOL.py will have no effect!")
#printfunc (atlasFieldSvc)
