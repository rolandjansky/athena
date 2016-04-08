outputFile = "trtstrawstatusperm-08_test.root"
objectList = [ "TRTCond::StrawStatusMultChanContainer#/TRT/Cond/StatusPermanent"]
tagList = ["TrtStrawStatusPerm-03_test"]

from AthenaCommon.GlobalFlags import GlobalFlags


include("IOVDbSvc/CondDBSetup.py")
from IOVDbSvc.CondDB import conddb

# This line can be excluded and it will by default connect to SQlite file mycool.db
from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
IOVDbSvc.dbConnection="impl=cool;techno=sqlite;schema=trtstrawstatusperm-08_test.db;X:COMP200"
 
include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )

IOVSvc = Service("IOVSvc")
IOVSvc.preLoadData = True



from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion ="ATLAS-CSC-05-00-00"


from AthenaCommon.DetFlags import DetFlags
DetFlags.TRT_setOn()
DetFlags.detdescr.TRT_setOn()

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit


include("RegistrationServices/RegistrationServices_jobOptions.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTStrawStatusWrite
TSW=TRTStrawStatusWrite(name="TSW")
#TSW.StatusInputFile="EKtest2.txt"
TSW.StatusInputFilePermanent="EKtest1.txt"
topSequence +=TSW


from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
myOCA=OutputConditionsAlg(outputFile=outputFile,name="myOCA")
myOCA.ObjectList=objectList
myOCA.IOVTagList=tagList
#myOCK.WriteIOV=True
myOCA.Run1=0
myOCA.LB1=0
myOCA.Run2=2147483647
myOCA.LB2=4294967295


topSequence+=myOCA

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = DEBUG
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1



include( "RegistrationServices/IOVRegistrationSvc_jobOptions.py" )
svcMgr = Service( "IOVRegistrationSvc" )
svcMgr.UseGlobalIOVForCollections = True
