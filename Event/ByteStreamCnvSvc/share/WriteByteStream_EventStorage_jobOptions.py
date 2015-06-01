# ----------- JobOptions fragments for Writing out ByteStream ----------
# 

# get a handle on the ServiceManager
svcMgr = theApp.serviceMgr()

# Conversion Service instance 
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc
svcMgr += ByteStreamCnvSvc()
# OutputSvc
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageOutputSvc
svcMgr += ByteStreamEventStorageOutputSvc()
# Properties 
ByteStreamCnvSvc = svcMgr.ByteStreamCnvSvc
ByteStreamCnvSvc.ByteStreamOutputSvc ="ByteStreamEventStorageOutputSvc"
# OutputStream
from AthenaServices.AthenaServicesConf import AthenaOutputStream
oStream = AthenaOutputStream(
    "StreamBS",
    EvtConversionSvc = "ByteStreamCnvSvc",
    OutputFile = "ByteStreamEventStorageOutputSvc"
    )
theApp.addOutputStream( oStream )
theApp.OutStreamType ="AthenaOutputStream"; 
# Define the output as follows: 
ByteStreamEventStorageOutputSvc = svcMgr.ByteStreamEventStorageOutputSvc
ByteStreamEventStorageOutputSvc.OutputDirectory = "./" 
ByteStreamEventStorageOutputSvc.AppName = "Athena" 
ByteStreamEventStorageOutputSvc.FileTag = "csc13" 
