##########################################################################################################
##													##
##													##
##     Script to read .db and .pool files and create .txt with the StatusHT contants                    ##
##													##
##########################################################################################################


from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock("atlas")
globalflags.DataSource.set_Value_and_Lock("geant4")
from IOVDbSvc.CondDB import conddb


from AthenaCommon.GlobalFlags import globalflags
include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )

from AthenaCommon.GlobalFlags import globalflags


from AthenaCommon.DetFlags import DetFlags
DetFlags.TRT_setOn()
DetFlags.detdescr.TRT_setOn()

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit


include("RegistrationServices/RegistrationServices_jobOptions.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

conddb.setGlobalTag("OFLCOND-MC16-SDR-26")
svcMgr.IOVDbSvc.forceRunNumber=282483  # write here a number within the IoV of interest

svcMgr.IOVSvc.preLoadData 	   = True
svcMgr.MessageSvc.OutputLevel      = INFO

InDetTRTConditionsServices=[]
from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummarySvc
InDetTRTStrawStatusSummarySvc = TRT_StrawStatusSummarySvc(name = "InDetTRTStrawStatusSummarySvc")
ServiceMgr += InDetTRTStrawStatusSummarySvc
print InDetTRTStrawStatusSummarySvc
InDetTRTConditionsServices.append(InDetTRTStrawStatusSummarySvc)

# TRT Conditions Summary Service
from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_ConditionsSummarySvc
InDetTRTConditionsSummaryService = TRT_ConditionsSummarySvc(name        = "InDetTRTConditionsSummaryService",
									ServiceList = InDetTRTConditionsServices)
ServiceMgr += InDetTRTConditionsSummaryService
print InDetTRTConditionsSummaryService 

from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTStrawStatusWrite
TSW=TRTStrawStatusWrite(name="TSW")
TSW.StrawStatusTool 		=InDetTRTStrawStatusSummarySvc
TSW.StatusInputFileHT		=""
topSequence +=TSW

theApp.EvtMax = 1

conddb.blockFolder("/TRT/Cond/StatusHT")
conddb.addFolder("","<dbConnection>sqlite://;schema=TRT_Cond_StatusHT.db;dbname=OFLP200</dbConnection>/TRT/Cond/StatusHT<tag>TrtStrawStatusHT-MC-run2-scenario5_00-03</tag>",force=True)
# or in case you have a sqLite file
#conddb.addFolder("","<dbConnection>sqlite://;schema=mycool.db;dbname=CONDBR2</dbConnection>/TRT/Cond/StatusHT<tag>TrtStrawStatusHT-MC-run2-scenario5_00-03</tag>",force=True)




