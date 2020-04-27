##########################################################################################################
##													##
##													##
##     Script to read text and create .db and .pool files with straw status constants 			##
##													##
##     Poolfile should be inserted in the catalog:							##
##                                                 -pool_insertFileToCatalog  pooloutputfile.root	##
##													##
##     Output file:											##	
##						   -caliboutput.txt					##
##													##
##     You have to set up the trt tag that you use to write mycool.db and pooloutputfile.root		## 
##													##
##													##
##########################################################################################################

tagList         = [     "TRTCondStatus-TEST-00-00"                                              ]
objectList      = [     "TRTCond::StrawStatusMultChanContainer#/TRT/Cond/Status"                ]
outputFile 	= "StrawStatus.pool.root"


tagListHT       = [     "TrtStrawStatusHT-Scenario1"                                     ]
objectListHT    = [     "TRTCond::StrawStatusMultChanContainer#/TRT/Cond/StatusHT"              ]
outputFileHT = "StrawStatusHT.pool.root"



tagListPer      = [     "TRTCondStatusPermanent-TEST-00-00"                                     ]
objectListPer   = [     "TRTCond::StrawStatusMultChanContainer#/TRT/Cond/StatusPermanent"       ]
outputFilePer 	= "StrawStatusPer.pool.root"


inputfilePer = "/afs/cern.ch/user/i/idcalib/w0/TRT_Calibration/uploadedDB/testareas/testarea_20.1.4.9/StatusTest/StrawsToMaskPer.txt"
inputfileTmp = "/afs/cern.ch/user/i/idcalib/w0/TRT_Calibration/uploadedDB/testareas/testarea_20.1.4.9/StatusTest/StrawsToMaskTmp.txt"
inputfileHT  = "scenario7.txt"

from AthenaCommon.GlobalFlags import globalflags

globalflags.DetGeo.set_Value_and_Lock("atlas")
globalflags.DataSource.set_Value_and_Lock("data")

from IOVDbSvc.CondDB import conddb
include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )


from AthenaCommon.DetFlags import DetFlags
DetFlags.TRT_setOn()
DetFlags.detdescr.TRT_setOn()

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit



from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 1

svcMgr.MessageSvc.OutputLevel      = INFO
conddb.setGlobalTag("CONDBR2-BLKPA-2018-14")


from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTStrawStatusWrite
TSW=TRTStrawStatusWrite(name="TSW")
TSW.StatusInputFilePermanent	=inputfilePer
TSW.StatusInputFile		=inputfileTmp
TSW.StatusInputFileHT		=inputfileHT  
topSequence +=TSW

print "#########################################################################################################################################"
print "#########################################################################################################################################"

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
include("RegistrationServices/RegistrationServices_jobOptions.py")


myOCAHT=OutputConditionsAlg(outputFile=outputFileHT,name="myOCAHT")
myOCAHT.ObjectList=objectListHT
myOCAHT.IOVTagList=tagListHT
myOCAHT.Run1=0
myOCAHT.LB1=0
myOCAHT.Run2=2147483647
myOCAHT.LB2=0
myOCAHT.WriteIOV=True
topSequence+=myOCAHT


from AthenaServices.AthenaServicesConf import AthenaOutputStreamTool
TRTCondStream=AthenaOutputStreamTool('myOCATool')
TRTCondStream.PoolContainerPrefix='ConditionsTree'

TRTCondStream=AthenaOutputStreamTool('myOCAPerTool')
TRTCondStream.PoolContainerPrefix='ConditionsTree'

TRTCondStream=AthenaOutputStreamTool('myOCAHTTool')
TRTCondStream.PoolContainerPrefix='ConditionsTree'


ToolSvc += TRTCondStream
print TRTCondStream
svcMgr = Service( "IOVRegistrationSvc" )
svcMgr.UseGlobalIOVForCollections = True


print "#########################################################################################################################################"
print "#########################################################################################################################################"

