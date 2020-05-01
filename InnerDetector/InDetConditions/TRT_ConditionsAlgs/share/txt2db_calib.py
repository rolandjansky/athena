##########################################################################################################
##													##
##													##
##     Script to read text file and create .db and .pool files with calibration constants 		##
##													##
##													##
##													##
##     You have to set up the trt tag that you use to write mycool.db and pooloutputfile.root		## 
##													##
##													##
##########################################################################################################

T0Tag="Textt0"
RtTag="Textrt"
contants= "dbconst_HTcorrection.txt"
calib="mycool.db"

#Read the .db and .pool. Use pool_insertFileToCatalog <poolfilename>
from AthenaCommon.GlobalFlags import globalflags

globalflags.DetGeo.set_Value_and_Lock("atlas")
#GlobalFlags.DataSource 	= 'data'
globalflags.DataSource.set_Value_and_Lock("geant4")

from IOVDbSvc.CondDB import conddb

# This line can be excluded and it will by default connect to SQlite file mycool.db
# IOVDbSvc.dbConnection="impl=cool;techno=sqlite;schema=mycool.db;X:OFLP200"
include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )

#from AthenaCommon.GlobalFlags import globalflags
#GlobalFlags.DetDescrVersion ="ATLAS-CSC-02-01-00"


from AthenaCommon.DetFlags import DetFlags
DetFlags.TRT_setOn()
DetFlags.detdescr.TRT_setOn()

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit


include("RegistrationServices/RegistrationServices_jobOptions.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 1

svcMgr.MessageSvc.OutputLevel      = 1
conddb.setGlobalTag("OFLCOND-MC16-SDR-28")
#svcMgr.IOVDbSvc.forceRunNumber=290000


from AthenaServices.AthenaServicesConf import AthenaOutputStreamTool
TRTCondStream=AthenaOutputStreamTool(name="CondStream1",OutputFile="trtcalibout.pool.root")
ToolSvc += TRTCondStream
print TRTCondStream

conddb.blockFolder("/TRT/Calib/RT" )
conddb.blockFolder("/TRT/Calib/T0" )

from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTCondWrite
TRTCondWrite=TRTCondWrite( name = "TRTCondWrite",
                           CalibInputFile=contants)
topSequence+=TRTCondWrite

outputFile = "pooloutputfile.root"
objectList = [ "TRTCond::RtRelationMultChanContainer#/TRT/Calib/RT","TRTCond::StrawT0MultChanContainer#/TRT/Calib/T0"]
tagList = [RtTag,T0Tag]

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
myOCA=OutputConditionsAlg(outputFile=outputFile)
myOCA.ObjectList=objectList
myOCA.IOVTagList=tagList
myOCA.WriteIOV=True

myOCA.Run1=0
myOCA.LB1=0
myOCA.Run2=9999999
myOCA.LB2=0

topSequence+=myOCA

