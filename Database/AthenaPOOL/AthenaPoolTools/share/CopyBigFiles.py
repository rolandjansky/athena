include("AthenaPoolTools/CopyFiles.py")

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.AthenaPoolCnvSvc.MaxFileSizes = [ "15000000000" ]

