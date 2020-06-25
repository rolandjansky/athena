from AthenaCommon.AppMgr import ServiceMgr
if (rec.doWriteESD):
    ServiceMgr.EventSelector.MaxBadEvents=1000
