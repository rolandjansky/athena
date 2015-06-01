# ----------- JobOptions fragments for Writing out ByteStream ----------
# 


# get a handle on the ServiceManager
svcMgr = theApp.serviceMgr()

# Conversion Service instance 
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc
svcMgr += ByteStreamCnvSvc()
# OutputSvc
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamFileOutputSvc
svcMgr += ByteStreamFileOutputSvc()

# Properties 
ByteStreamCnvSvc = svcMgr.ByteStreamCnvSvc
ByteStreamCnvSvc.ByteStreamOutputSvc ="ByteStreamFileOutputSvc"
ByteStreamFileOutputSvc = svcMgr.ByteStreamFileOutputSvc
ByteStreamFileOutputSvc.OutputFiles = ["RawEvent.re"] 
# OutputStream
from AthenaServices.AthenaServicesConf import AthenaOutputStream
oStream = AthenaOutputStream(
    "StreamBS",
    EvtConversionSvc = "ByteStreamCnvSvc",
    OutputFile = "ByteStreamFileOutputSvc"
    )
theApp.addOutputStream( oStream )
theApp.OutStreamType ="AthenaOutputStream"; 
