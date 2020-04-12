##########################################################################################################
##													##
##													##
##     Script to read .db and .pool files and create .txt with the 					##
##     calibration contants, to compare with the original ones, and check				##
##     if the conversion has being done properly 							##
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


T0Tag="Textt0"
RtTag="Textrt"
contants= "calibout.267359.txt"
calib="mycool.db"


from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock("atlas")
globalflags.DataSource.set_Value_and_Lock("geant4")
#globalflags.DetDescrVersion ="ATLAS-CSC-02-01-00"
from IOVDbSvc.CondDB import conddb

include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )

from AthenaCommon.DetFlags import DetFlags
DetFlags.TRT_setOn()
DetFlags.detdescr.TRT_setOn()

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit


include("AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py")
include("RegistrationServices/RegistrationServices_jobOptions.py")
#
# --- Load necessary TRT conditions folders
#

if not conddb.folderRequested('/TRT/Calib/RT'):
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/RT","/TRT/Calib/RT",className='TRTCond::RtRelationMultChanContainer')

if not conddb.folderRequested('/TRT/Calib/T0'):
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/T0","/TRT/Calib/T0",className='TRTCond::StrawT0MultChanContainer')

if not conddb.folderRequested('/TRT/Calib/errors2d'):
    TRTErrorsFolder = conddb.addFolderSplitOnline ("TRT","/TRT/Onl/Calib/errors2d","/TRT/Calib/errors2d",className='TRTCond::RtRelationMultChanContainer')

if not conddb.folderRequested('/TRT/Calib/slopes'):
    TRTSlopesFolder = conddb.addFolderSplitOnline ("TRT","/TRT/Onl/Calib/slopes","/TRT/Calib/slopes",className='TRTCond::RtRelationMultChanContainer')


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
#svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema="+calib+";dbname=COMP200"
#svcMgr.dbConnection="sqlite://;schema=mycool.db;dbname=COMP200"

conddb.setGlobalTag("OFLCOND-MC16-SDR-27")
#conddb.setGlobalTag("CONDBR2-BLKPA-2018-14")
#svcMgr.IOVDbSvc.forceRunNumber=227000

# in case you want to read your own mycool.db
#conddb.blockFolder("/TRT/Calib/T0")
#conddb.blockFolder("/TRT/Calib/RT")
#conddb.blockFolder("/TRT/Calib/errors")


#svcMgr.IOVDbSvc.Folders+= ["<db>sqlite://;schema=mycool.db;dbname=CONDBR2 </db> /TRT/Calib/T0  <tag>"+T0Tag+"</tag>"]
#svcMgr.IOVDbSvc.Folders+= ["<db>sqlite://;schema=mycool.db;dbname=CONDBR2 </db> /TRT/Calib/RT  <tag>"+RtTag+"</tag>"]
#svcMgr.IOVDbSvc.Folders+= ["<db>COOLOFL_TRT/OFLP200 </db> /TRT/Calib/errors  <tag>TrtCalibErrors-mc-01</tag>"]


svcMgr.IOVSvc.preLoadData = True
svcMgr.MessageSvc.OutputLevel      = INFO


from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTCondWrite
TRTCondWrite = TRTCondWrite( name = "TRTCondWrite",
                             CalibOutputFile = "calibout_1.txt")
topSequence+=TRTCondWrite

theApp.EvtMax = 1

