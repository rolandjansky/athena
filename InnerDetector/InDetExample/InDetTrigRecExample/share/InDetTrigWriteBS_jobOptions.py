#---------------------------------------------------------------
# Write RDO to BS
# --------------------------------------------------------------

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc

# OutputSvc
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageOutputSvc
ByteStreamFileOutputSvc = ByteStreamEventStorageOutputSvc("ByteStreamFileOutputSvc",
						  OutputDirectory = "./",
                                                  AppName = "Athena", 
                                                  FileTag = "csc13")
ServiceMgr             += ByteStreamFileOutputSvc
print                     ByteStreamFileOutputSvc

# Conversion Service instance
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc
ByteStreamCnvSvc           = ByteStreamCnvSvc("ByteStreamCnvSvc",
                                              IsSimulation        = True,
                                              ByteStreamOutputSvc = "ByteStreamFileOutputSvc")
ByteStreamCnvSvc.InitCnvs += ["PixelRDO_Container"]
ByteStreamCnvSvc.InitCnvs += ["SCT_RDO_Container"]
ByteStreamCnvSvc.InitCnvs += ["TRT_RDO_Container"]
ServiceMgr      += ByteStreamCnvSvc
print              ByteStreamCnvSvc

# OutputStream
from AthenaServices.AthenaServicesConf import AthenaOutputStream
StreamBS = AthenaOutputStream("StreamBS",
                              EvtConversionSvc = "ByteStreamCnvSvc")
# Force reading of output stream (ostream set externally)
StreamBS.ForceRead = TRUE
StreamBS.ItemList += [ "TRT_RDO_Container#*" ]
StreamBS.ItemList += [ "SCT_RDO_Container#*" ]
StreamBS.ItemList += [ "PixelRDO_Container#*" ]
print StreamBS

# add stream
theApp.addOutputStream( StreamBS )
theApp.OutStreamType ="AthenaOutputStream"

