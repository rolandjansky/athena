from AthenaCommon.AppMgr import ServiceMgr
atlasFieldSvc = ServiceMgr.AtlasFieldSvc
try:
    atlasFieldSvc.UseMapsFromCOOL=True
except:
    print "WARNING postInclude.AtlasFieldSvcUseMapsFromCOOL.py will have no effect!"
#print atlasFieldSvc
