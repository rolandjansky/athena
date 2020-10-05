# read Pool
include("AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py")

# Pool Converters
include("EventAthenaPool/EventAthenaPool_joboptions.py")
include("TrigT1EventAthenaPool/TrigT1EventAthenaPool_joboptions.py")
include("AnalysisTriggerEventAthenaPool/AnalysisTriggerEventAthenaPool_joboptions.py")

# input 
EventSelector = Service( "EventSelector" )
#EventSelector.InputCollections = [ "/afs/cern.ch/atlas/offline/data/testfile/dc2.002867.pyt_z_ee.g4dig802._0001.pool.root" ]
EventSelector.InputCollections = [ "rfio:/castor/cern.ch/atlas/project/dc2/preprod/g4dig805/dc2.002883.pyt_z_mumu.g4dig805/data/dc2.002883.pyt_z_mumu.g4dig805._0001.pool.root" ]

# LVL1 config
include("TrigT1Config/TrigT1ConfigJobOptions.py")
L1Config = Service( "L1Config" )
L1Config.triggerMenuFileLocation   = "LVL1triggermenuDC2.xml"
L1Config.thresholdListFileLocation = "LVL1triggerthresholdsDC2.xml"

# for RecMuonRoI svc
DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DecodeIdDict = TRUE
DetDescrCnvSvc.ReadFromNova = TRUE
include ( "AtlasGeoModel/GeoModelCommon.py" )
include ( "AmdcAth/AmdcAth_jobOptions.py" )
include ( "AtlasGeoModel/MuonGeoModel.py" )
include ( "MuonByteStreamCnvTest/MuonRdoDigit_jobOptions.py" )
include ( "TrigT1TGCRecRoiSvc/TrigT1TGCRecRoiSvcJobOptions.py" )
theApp.Dlls   += ["TrigT1RPCRecRoiSvc"]
theApp.ExtSvc += ["LVL1RPC::RPCRecRoiSvc"]

# RoIB result to AOD conversion
include("AnalysisTriggerAlgs/AnalysisTrigger_jobOptions.py")

# write to Pool
theApp.OutStream += [ "Stream1" ]
theApp.OutStreamType = "AthenaOutputStream"
Stream1 = Algorithm( "Stream1" )
Stream1.EvtConversionSvc = "AthenaPoolCnvSvc"
                                 
# Output file name
Stream1.OutputFile = "AOD_Trigger.root"

# Item list
Stream1.ItemList+=["EventInfo#*"]
Stream1.ItemList+=["CTP_Decision#*"]
Stream1.ItemList+=["LVL1_ROI#*"]

# Message level
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO

RoIBResultToAOD = Algorithm( "RoIBResultToAOD" )
RoIBResultToAOD.OutputLevel = DEBUG

# Number of events to be processed
theApp.EvtMax = 10

