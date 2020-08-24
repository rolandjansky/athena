#---------------------------------------------------------------
# Write RDO to BS
# --------------------------------------------------------------

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc


svcMgr = theApp.serviceMgr()

# Conversion Service instance
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc
svcMgr += ByteStreamCnvSvc()

# OutputSvc
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageOutputSvc
svcMgr += ByteStreamEventStorageOutputSvc()

# Properties
ByteStreamCnvSvc = svcMgr.ByteStreamCnvSvc
ByteStreamCnvSvc.IsSimulation = True
ByteStreamCnvSvc.ByteStreamOutputSvc ="ByteStreamEventStorageOutputSvc"

include("InDetRecExample/InDetRecCabling.py")

if DetFlags.haveRIO.pixel_on():
  ByteStreamCnvSvc.InitCnvs += ["PixelRDO_Container"]
if DetFlags.haveRIO.SCT_on():
  ByteStreamCnvSvc.InitCnvs += ["SCT_RDO_Container"]
if DetFlags.haveRIO.TRT_on():
  ByteStreamCnvSvc.InitCnvs += ["TRT_RDO_Container"]
if DetFlags.haveRIO.BCM_on():
  ByteStreamCnvSvc.InitCnvs += ["BCM_RDO_Container"]
print              (ByteStreamCnvSvc)

# OutputStream
from AthenaServices.AthenaServicesConf import AthenaOutputStream
oStream = AthenaOutputStream(
    "StreamBS",
    EvtConversionSvc = "ByteStreamCnvSvc",
    OutputFile = "ByteStreamEventStorageOutputSvc"
    )

if DetFlags.haveRIO.pixel_on():
  oStream.ItemList += [ "PixelRDO_Container#*" ]
if DetFlags.haveRIO.SCT_on():
  oStream.ItemList += [ "SCT_RDO_Container#*" ]
if DetFlags.haveRIO.TRT_on():
  oStream.ItemList += [ "TRT_RDO_Container#*" ]
if DetFlags.haveRIO.BCM_on():
  oStream.ItemList += [ "BCM_RDO_Container#*" ]
print (oStream)

# add stream
theApp.addOutputStream( oStream )
theApp.OutStreamType ="AthenaOutputStream"

ByteStreamEventStorageOutputSvc = svcMgr.ByteStreamEventStorageOutputSvc
ByteStreamEventStorageOutputSvc.OutputDirectory = "./"
ByteStreamEventStorageOutputSvc.AppName = "Athena"
ByteStreamEventStorageOutputSvc.FileTag = "csc13"

