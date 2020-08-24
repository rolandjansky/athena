###############################################################
#
# Job options file 1
#
#==============================================================
#use McEventSelector
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

EventSelector = svcMgr.EventSelector
EventSelector.RunNumber=1
EventSelector.EventsPerRun=10;
EventSelector.FirstEvent=1

theApp.Dlls += [ "GaudiAud" ] 
theAuditorSvc = svcMgr.AuditorSvc
theAuditorSvc.Auditors =  [ "ChronoAuditor" ] 

LArIOVDbTag = "OFLCOND-SDR-BS7T-05-12"
DetDescrVersion='ATLAS-GEO-18-01-03'

from AthenaCommon.GlobalFlags import globalflags
#GlobalFlags.DataSource.set_data()
#GlobalFlags.InputFormat.set_bytestream()
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.DataSource.set_Value_and_Lock('geant4')
globalflags.ConditionsTag.set_Value_and_Lock(LArIOVDbTag)
globalflags.DetDescrVersion.set_Value_and_Lock(DetDescrVersion)
# Set the connection string
include ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )
IOVDbSvc.GlobalTag=LArIOVDbTag


#--------------------------------------------------------------
# Detector Description
#--------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

from AthenaCommon.JobProperties import jobproperties
#jobproperties.Global.DetDescrVersion="ATLAS-CSC-02-00-00"
jobproperties.Global.DetDescrVersion=DetDescrVersion

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit


#LArIOVDbTag = "CSC02-I-QGSP_BERT"
# Switches: 
# items 
#RunNumber = 214989
RunNumber = 1

#
RecreateFolder = False
WriteIOV = True

# Objects  and its tag
ObjectList = []
TagList = []
 
#TagNameForFix = "CSC02-I2"

#ObjectList += ["LArAutoCorrMC#LArAutoCorr#/LAR/ElecCalibMC/AutoCorr"]

#TagList += ["LARElecCalibMCAutoCorr-"+TagNameForFix]

#OutputPOOLFileName = "LArElecCalib_"+TagNameForFix+"_v3.pool.root" 

from LArConditionsTest.LArConditionsTestConf import FixLArElecCalib
topSequence += FixLArElecCalib()
FixLArElecCalib = topSequence.FixLArElecCalib
# fix9 for FCAl AutoCorr 
FixLArElecCalib.FixFlag = 13

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# Other LAr related
include( "LArIdCnv/LArIdCnv_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )

include( "LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py" )
include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )
#include( "LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py" )
#include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

#--------------------------------------------------------------
EventSelector.RunNumber=RunNumber
#EventSelector.EventsPerRun=10;
EventSelector.EventsPerRun=2
EventSelector.FirstEvent=1


include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )

theApp.EvtMax=1 

MessageSvc = svcMgr.MessageSvc
MessageSvc.OutputLevel  = DEBUG
MessageSvc.defaultLimit = 1000000;
MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"



from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
OutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg","dummy.pool.root",
                                        ["AthenaAttributeList#/LAR/IdentifierOfl/OnOffIdMap_SC",
                                         ],
                                        ["LARIdentifierOflOnOffIdMap_SC-000",
                                         ],
                                        True)

OutputConditionsAlg.Run1     = 0


from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
svcMgr.IOVRegistrationSvc.RecreateFolders = True
svcMgr.IOVRegistrationSvc.SVFolder = False
svcMgr.IOVRegistrationSvc.userTags = False

svcMgr.IOVRegistrationSvc.OverrideNames += ["OnlineHashToOfflineId",]
types=[]
for i in range(len(svcMgr.IOVRegistrationSvc.OverrideNames)):
    types.append("Blob16M");
svcMgr.IOVRegistrationSvc.OverrideTypes += types;
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=LArSCIdentifierMaps.db;dbname=OFLP200"

