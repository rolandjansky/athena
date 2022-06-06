# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#########################################################################

if 'InputFile' not in dir():
   InputFile='shape.txt'

if 'OutputDb' not in dir():
   OutputDb='LArElecCalib_NewShape_v2.db'

if 'Version' not in dir():
   Version='3'

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

EventSelector = svcMgr.EventSelector
EventSelector.RunNumber=308000
EventSelector.EventsPerRun=10;
EventSelector.FirstEvent=1

theApp.Dlls += [ "GaudiAud" ] 
theAuditorSvc = svcMgr.AuditorSvc
theAuditorSvc.Auditors =  [ "ChronoAuditor" ] 

DetDescrVersion='ATLAS-GEO-20-00-00'
LArIOVDbTag = "CONDBR2-ES1PA-2016-03"

from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.ConditionsTag.set_Value_and_Lock(LArIOVDbTag)
globalflags.DetDescrVersion.set_Value_and_Lock(DetDescrVersion)
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
jobproperties.Global.DetDescrVersion=DetDescrVersion

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit


RunNumber = 308000
#
RecreateFolder = False
WriteIOV = True

# Objects  and its tag
ObjectList = []
TagList = []
 
TagNameForFix = "SpecRun"

ObjectList += ["LArShape32MC#LArShape#/LAR/ElecCalibOfl/LArPileupShape"]

TagList += ["LARElecCalibOflLArPileupShape-RUN2-"+TagNameForFix]

OutputPOOLFileName = "LArElecCalib_"+TagNameForFix+"_v"+Version+".pool.root" 

from LArConditionsTest.LArConditionsTestConf import FixLArElecCalib
topSequence += FixLArElecCalib()
FixLArElecCalib = topSequence.FixLArElecCalib
FixLArElecCalib.FixFlag = 17
FixLArElecCalib.InputFile = InputFile

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# Other LAr related
include( "LArIdCnv/LArIdCnv_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )

include( "LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py" )
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

#--------------------------------------------------------------

include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )

theApp.Dlls   += [ "AthenaPoolCnvSvc" ]
theApp.Dlls   += [ "LArCondAthenaPoolPoolCnv" ]
include( "AthenaSealSvc/AthenaSealSvc_joboptions.py" )

conddb.addFolder("LAR_OFL","/LAR/ElecCalibOfl/LArPileupShape")

theApp.EvtMax=1 

MessageSvc = svcMgr.MessageSvc
MessageSvc.OutputLevel  = DEBUG
MessageSvc.defaultLimit = 1000000;
MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

##############################################
# Writing POOL and COOL 
if len(ObjectList)>0 : 

 include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")

 from AthenaCommon.AlgSequence import AlgSequence
 from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
 topSequence += OutputConditionsAlg()
 OutputConditionsAlg = topSequence.OutputConditionsAlg
 
 # List of objects container type#key#foldername
 OutputConditionsAlg.ObjectList = ObjectList 
 OutputConditionsAlg.IOVTagList = TagList
 svcMgr.ToolSvc.OutputConditionsAlgTool.OutputFile = OutputPOOLFileName

 # Set flag to register and run interval Run1/Event1 to Run2/Event2
 #   Usually, only need to set Run1, others go to default
 #### 

 OutputConditionsAlg.WriteIOV = WriteIOV
 OutputConditionsAlg.Run1     = 0
 OutputConditionsAlg.LB1   = 0

 # Set the connection string
 include ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )

 svcMgr.IOVDbSvc.dbConnection="sqlite://;schema="+OutputDb+";dbname=CONDBR2"

 # For schema creation - only should be used when creating the folder,
 # i.e. the first time
 IOVRegSvc = svcMgr.IOVRegistrationSvc
 IOVRegSvc.OutputLevel  = DEBUG
 IOVRegSvc.RecreateFolders = RecreateFolder 

###########################################################################

