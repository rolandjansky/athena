from AthenaCommon.AppMgr import ServiceMgr
atlasFieldSvc = ServiceMgr.AtlasFieldSvc
try:
    atlasFieldSvc.UseMapsFromCOOL=False
except:
    print "WARNING postInclude.AtlasFieldSvcFallbackForOldCond.py will have no effect!"
#print atlasFieldSvc
