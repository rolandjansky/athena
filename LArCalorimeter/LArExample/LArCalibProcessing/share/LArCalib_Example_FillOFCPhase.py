###############################################################
#
# Python script for accessing LAr Conditions data in DetectorStore
#
#    0. Select run number, and load Conditions jobOpt
#    1. Access Conditions Data using Identifier 
#         Retrieve container from DetectorStore
#         make identifier using onlineID or larCablingSvc
#         use Container's get method to access data object.
#         use LArCondDataDumper.printLarCondObject(obj) to print object
#    2. Print all channel
#         Define Type,Key to retrieve (ListofType, ListofKey)
#         print_all_lar_cond_data (ListofType,ListofKey) 
#         (set MessageSvc.OutputLevel = DEBUG for printing all channels)
#==============================================================

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )
#include ("LArConditionsCommon/LArMinimalSetup.py")

#theByteStreamInputSvc=svcMgr.ByteStreamInputSvc
#svcMgr.EventSelector.Input=["/data/pavol/athena_calib/daq.Ramp.0031077.No.Streaming.LB0000.EB-ECC._0001.data"]
#--------------------------------------------------------------
# pick a proper run number for conditions
#--------------------------------------------------------------
#from AthenaCommon.AppMgr import ServiceMgr
EventSelector = ServiceMgr.EventSelector
EventSelector.RunNumber=31077
EventSelector.EventsPerRun=100000;
#EventSelector.FirstEvent=1
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = WARNING
theApp.EvtMax = 1

#--------------------------------------------------------------
# pick a DetDescrVersion
#--------------------------------------------------------------
# DetDescrVersion = "DC2" 
# DetDescrVersion = "Rome-Initial" 
#DetDescrVersion = "ATLAS-DC3-05"
DetDescrTag = "ATLAS-CommNF-03-00-00"
IOVDbSvcGlobalTagData = 'COMCOND-002-00'

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DataSource.set_data()
GlobalFlags.InputFormat.set_bytestream()
GlobalFlags.DetGeo.set_commis()

#from AthenaCommon.JobProperties import jobproperties
#jobproperties.Global.DetDescrVersion = "ATLAS-CommNF-00-03-00"

#--------------------------------------------------------------
# load shared libs, converter, etc
#--------------------------------------------------------------
include ("LArConditionsCommon/DumpLArCondData_Config.py")

#--------------------------------------------------------------
# 0. load conditions data 
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
IOVDbSvc.OutputLevel = DEBUG
IOVDbSvc.GlobalTag = 'COMCOND-002-00'
include( "LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py" )
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

#PoolFileList = ["LArMphysOverMcal_RTM_31046_31047_31048_GAIN_0.pool.root"]
#PoolFileList = ["/afs/cern.ch/atlas/conditions/poolcond/vol3/comcond.000002.lar_conditions.recon.pool.v0000/comcond.000002.lar_conditions.recon.pool.v0000._0094.pool.root"]
#include("LArCalibProcessing/ReadLocalConditionsPool.py")
IOVSvc = Service( "IOVSvc" )
IOVSvc.updateInterval = "JOB"
IOVSvc.preLoadData=True
IOVSvc.OutputLevel = DEBUG
#
KeyOutput = "LArOFCPhase"
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from LArCalibUtils.LArCalibUtilsConf import LArOFPhaseFill
LArOFPhaseFill = LArOFPhaseFill("LArOFPhaseFill")
#LArOFPhaseFill.InputFile = ""
LArOFPhaseFill.keyOFCbin = KeyOutput
LArOFPhaseFill.GroupingType = "ExtendedSubDetector"
LArOFPhaseFill.OutputLevel = ERROR
topSequence += LArOFPhaseFill
#
# Make an output
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
Folder = "/LAR/OFC/OFCPhase"
ObjectSpec = [ "LArOFCBinComplete#"+KeyOutput+"#"+Folder ]
TagSpec = [ "LArOFCPhase-01" ]
OutputDB = "sqlite://X;schema=OFCPhase.db;dbname=CONDBR2"
OutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg","./ofc_phase.pool.root", 
                    ObjectSpec,TagSpec,True)
#OutputConditionsAlg.Run1 = 0
#OutputConditionsAlg.Run2 = 0

topSequence += OutputConditionsAlg


from AthenaCommon.AppMgr import ServiceMgr
from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
svcMgr.IOVRegistrationSvc.RecreateFolders = True
svcMgr.IOVDbSvc.dbConnection  = OutputDB

#--------------------------------------------------------------
# initialize Athena, then go to first event 
#--------------------------------------------------------------
#theApp.initialize() 
#theApp.nextEvent()
svcMgr.MessageSvc.OutputLevel  = DEBUG
svcMgr.MessageSvc.defaultLimit = 10000
svcMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"
   
svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = ERROR)


