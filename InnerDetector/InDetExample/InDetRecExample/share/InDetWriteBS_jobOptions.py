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

# SCT cabling
from SCT_Cabling.SCT_CablingConf import SCT_CablingSvc
ServiceMgr+=SCT_CablingSvc()
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
conddb.addFolderSplitMC("SCT","/SCT/DAQ/Configuration/ROD","/SCT/DAQ/Configuration/ROD")
conddb.addFolderSplitMC("SCT","/SCT/DAQ/Configuration/MUR","/SCT/DAQ/Configuration/MUR")
conddb.addFolderSplitMC("SCT","/SCT/DAQ/Configuration/RODMUR","/SCT/DAQ/Configuration/RODMUR")
conddb.addFolderSplitMC("SCT","/SCT/DAQ/Configuration/Geog","/SCT/DAQ/Configuration/Geog")
ServiceMgr.SCT_CablingSvc.DataSource = "CORACOOL"
# hack to force MC calbing loading for FDR2
#if svcMgr.ByteStreamCnvSvc.IsSimulation:
#  ServiceMgr.SCT_CablingSvc.DataSource = "SCT_MC_FullCabling_svc.dat"

if DetFlags.haveRIO.pixel_on():
  ByteStreamCnvSvc.InitCnvs += ["PixelRDO_Container"]
if DetFlags.haveRIO.SCT_on():
  ByteStreamCnvSvc.InitCnvs += ["SCT_RDO_Container"]
if DetFlags.haveRIO.TRT_on():
  ByteStreamCnvSvc.InitCnvs += ["TRT_RDO_Container"]
if DetFlags.haveRIO.BCM_on():
  ByteStreamCnvSvc.InitCnvs += ["BCM_RDO_Container"]
print              ByteStreamCnvSvc

# OutputStream
from AthenaServices.AthenaServicesConf import AthenaOutputStream
oStream = AthenaOutputStream(
    "StreamBS",
    EvtConversionSvc = "ByteStreamCnvSvc",
    OutputFile = "ByteStreamEventStorageOutputSvc"
    )

oStream.ForceRead = TRUE
if DetFlags.haveRIO.pixel_on():
  oStream.ItemList += [ "PixelRDO_Container#*" ]
if DetFlags.haveRIO.SCT_on():
  oStream.ItemList += [ "SCT_RDO_Container#*" ]
if DetFlags.haveRIO.TRT_on():
  oStream.ItemList += [ "TRT_RDO_Container#*" ]
if DetFlags.haveRIO.BCM_on():
  oStream.ItemList += [ "BCM_RDO_Container#*" ]
print oStream

# add stream
theApp.addOutputStream( oStream )
theApp.OutStreamType ="AthenaOutputStream"

ByteStreamEventStorageOutputSvc = svcMgr.ByteStreamEventStorageOutputSvc
ByteStreamEventStorageOutputSvc.OutputDirectory = "./"
ByteStreamEventStorageOutputSvc.AppName = "Athena"
ByteStreamEventStorageOutputSvc.FileTag = "csc13"

