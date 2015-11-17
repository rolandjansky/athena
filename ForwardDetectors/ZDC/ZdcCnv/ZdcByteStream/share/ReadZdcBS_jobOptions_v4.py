#**************************************************************
# This came from LArCalorimeter/LArCnv/LArByteStream/trunk/share/jobOptions_ReadLArROD.py
# jopOptions file in LArByteStream package
#       Read in ByteStream events, and Access LArRawChannel
#
#==============================================================
include.block("ZdcByteStream/ReadZdcBS_jobOptions.py")

# eformat input
include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )

# EventStorage Input
ByteStreamInputSvc =  svcMgr.ByteStreamInputSvc

#ByteStreamInputSvc.FullFileName = ["root://eosatlas//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/data11_hi.00193403.physics_HardProbes.merge.RAW._lb0012._SFO-9._0001.1"]
#ByteStreamInputSvc.FullFileName = ["root://eosatlas//eos/atlas/atlascerngroupdisk/det-zdc/ZdcData/standalone/data15_calib.00283741.calibration_.daq.RAW._lb0000._ROS-FWD-ZDC-00._0001.data"]
ByteStreamInputSvc.FullFileName = ["root://eosatlas//eos/atlas/atlascerngroupdisk/det-zdc/ZdcData/calib/data15_calib.00285911.calibration_.daq.RAW._lb0000._ROS-FWD-ZDC-00._0001.data"]

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.InputFormat = "bytestream"


from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion="ATLAS-GEO-18-01-01"

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
svcMgr += ByteStreamAddressProviderSvc()
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerContainer/ZdcTriggerTowers"]
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "xAOD::TriggerTowerAuxContainer/ZdcTriggerTowersAux."]

# ToolSvc.LArRodDecoder.Print=true;
ByteStreamCnvSvc = Service( "ByteStreamCnvSvc" )
ByteStreamCnvSvc.InitCnvs += [  "xAOD::TriggerTowerContainer"]
ByteStreamCnvSvc.InitCnvs += [  "xAOD::TriggerTowerAuxContainer"]

theApp.ExtSvc += [ "ByteStreamCnvSvc"]

from ZdcByteStream.ZdcByteStreamConf import ZdcByteStreamReadV1V2Tool
from ZdcRec.ZdcRecConf import ZdcRecChannelToolV2

ToolSvc=Service("ToolSvc")
ToolSvc += ZdcByteStreamReadV1V2Tool("ZdcByteStreamReadV1V2Tool")
theApp.Dlls += ["ZdcByteStream"]
theApp.Dlls += ["ZdcRec"]
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 2
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 10
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------

AthenaEventLoopMgr = Service ("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel=2


from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from ZdcByteStream.ZdcByteStreamConf import ZdcByteStreamRawDataV2
job += ZdcByteStreamRawDataV2("ZdcByteStreamRawData2")
from ZdcRec.ZdcModuleGetter import ZdcModuleGetter
ZdcModuleGetter()

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("ChronoAuditor")()
theApp.AuditAlgorithms=True
theApp.AuditServices=True
theApp.AuditTools=True


