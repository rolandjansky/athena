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

ByteStreamInputSvc.FullFileName = \
["root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/data11_hi.00193403.physics_HardProbes.merge.RAW._lb0012._SFO-9._0001.1"]
#["/tmp/leite/test_map/data_test.00146536.calibration_map.daq.RAW._lb0000._ROSEventBuilder._0001.data"]
#["/castor/cern.ch/grid/atlas/DAQ/2009/00115405/physics_ZDCStream/" + \
#"data09_cos.00115405.physics_ZDCStream.daq.RAW._lb0000._SFO-1._0064.data"]

#include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )

from AthenaCommon.GlobalFlags import GlobalFlags
#GlobalFlags.InputFormat.set_bytestream()


from AthenaCommon.JobProperties import jobproperties
#jobproperties.Global.DetDescrVersion="ATLAS-CSC-01-02-00"
#jobproperties.Global.DetDescrVersion="ATLAS-CSC-02-00-00"

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

# Read LArRawChannel
#include( "LArByteStream/ReadLArBS_jobOptions.py" )

from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
svcMgr += ByteStreamAddressProviderSvc()
svcMgr.ByteStreamAddressProviderSvc.TypeNames += ["ZdcDigitsCollection"]

# ToolSvc.LArRodDecoder.Print=true;
ByteStreamCnvSvc = Service( "ByteStreamCnvSvc" )
ByteStreamCnvSvc.InitCnvs += [  "ZdcDigitsCollection"]

theApp.ExtSvc += [ "ByteStreamCnvSvc"]

from ZdcByteStream.ZdcByteStreamConf import ZdcByteStreamTool
from ZdcByteStream.ZdcByteStreamConf import ZdcByteStreamTester

ToolSvc=Service("ToolSvc")
ToolSvc += ZdcByteStreamTool("ZdcByteStreamTool")
theApp.Dlls += ["ZdcByteStream"]
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 2
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1000
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------

AthenaEventLoopMgr = Service ("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel=2

theApp.TopAlg+=["ZdcByteStreamTester"]
ZdcByteStreamTester=Algorithm("ZdcByteStreamTester")
#ZdcByteStreamTester.ZdcDigitsCollectionLocation = "ZdcDigitsCollection"

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("ChronoAuditor")()
theApp.AuditAlgorithms=True
theApp.AuditServices=True
theApp.AuditTools=True


