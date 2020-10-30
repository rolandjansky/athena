# ----------- JobOptions fragments for sending ByteStream to ROBDataProvider
# 

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# get a handle on the ServiceManager
svcMgr = theApp.serviceMgr()

# Conversion Service instance 
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc
svcMgr += ByteStreamCnvSvc()
# OutputSvc
from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ROBDataProviderSvc
svcMgr += ROBDataProviderSvc()
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamRDP_OutputSvc
svcMgr += ByteStreamRDP_OutputSvc()
# Properties
ByteStreamCnvSvc = svcMgr.ByteStreamCnvSvc
ByteStreamCnvSvc.ByteStreamOutputSvc ="ByteStreamRDP_OutputSvc"
# This output stream is created as a regular OutputStream 
# OutputStream 
from AthenaServices.AthenaServicesConf import AthenaOutputStream
topSequence += AthenaOutputStream(
    "StreamBS",
    EvtConversionSvc = "ByteStreamCnvSvc",
    OutputFile = "ByteStreamRDP_OutputSvc",
    ExtraInputs = [( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' ) ],
    )
# ByteStreamCnvSvc is an input CnvSvc now. 
EventPersistencySvc = svcMgr.EventPersistencySvc
EventPersistencySvc.CnvServices += [ "ByteStreamCnvSvc" ]
