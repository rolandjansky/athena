from AthenaCommon.AppMgr import ServiceMgr
if (rec.doWriteESD):
    ServiceMgr.ByteStreamInputSvc.MaxBadEvents=1000
