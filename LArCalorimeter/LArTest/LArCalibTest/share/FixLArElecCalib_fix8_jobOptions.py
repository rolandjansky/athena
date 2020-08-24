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


from AthenaCommon.GlobalFlags import GlobalFlags
#GlobalFlags.DataSource.set_data()
#GlobalFlags.InputFormat.set_bytestream()
GlobalFlags.DetGeo.set_atlas()

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
jobproperties.Global.DetDescrVersion="ATLAS-CSC-02-00-00"

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit


LArIOVDbTag = "CSC02-H-QGSP_BERT"
# Switches: 
# items 
RunNumber = 1

#
RecreateFolder = False
WriteIOV = True

# Objects  and its tag
ObjectList = []
TagList = []
 
TagNameForFix = "CSC02-I"

ObjectList += ["LArNoiseMC#LArNoise#/LAR/ElecCalibMC/Noise"]
ObjectList += ["LAruA2MeVMC#LAruA2MeV#/LAR/ElecCalibMC/uA2MeV"]
ObjectList += ["LArDAC2uAMC#LArDAC2uA#/LAR/ElecCalibMC/DAC2uA"]
ObjectList += ["LArRampMC#LArRamp#/LAR/ElecCalibMC/Ramp"]
ObjectList += ["LArShape32MC#LArShape#/LAR/ElecCalibMC/Shape"]
ObjectList += ["LArAutoCorrMC#LArAutoCorr#/LAR/ElecCalibMC/AutoCorr"]
ObjectList += ["LArMphysOverMcalMC#LArMphysOverMcal#/LAR/ElecCalibMC/MphysOverMcal"]

TagList += ["LARElecCalibMCNoise-"+TagNameForFix]
TagList += ["LARElecCalibMCuA2MeV-"+TagNameForFix]
TagList += ["LARElecCalibMCDAC2uA-"+TagNameForFix]
TagList += ["LARElecCalibMCRamp-"+TagNameForFix]
TagList += ["LARElecCalibMCShape-"+TagNameForFix]
TagList += ["LARElecCalibMCAutoCorr-"+TagNameForFix]
TagList += ["LARElecCalibMCMphysOverMcal-"+TagNameForFix]

OutputPOOLFileName = "LArElecCalib_"+TagNameForFix+"_v1.pool.root" 

from LArConditionsTest.LArConditionsTestConf import FixLArElecCalib
topSequence += FixLArElecCalib()
FixLArElecCalib = topSequence.FixLArElecCalib
# fix8 for updating EM/HEC calib for rel 14.
FixLArElecCalib.FixFlag =8

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# Other LAr related
include( "LArIdCnv/LArIdCnv_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )

include( "LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py" )
include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )

#--------------------------------------------------------------
EventSelector.RunNumber=1
#EventSelector.EventsPerRun=10;
EventSelector.EventsPerRun=2
EventSelector.FirstEvent=1


include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )

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

 svcMgr.IOVDbSvc.dbConnection="impl=cool;techno=sqlite;schema=LArElecCalib_CSC02-I.db;X:OFLP200"

 # For schema creation - only should be used when creating the folder,
 # i.e. the first time
 IOVRegSvc = svcMgr.IOVRegistrationSvc
 IOVRegSvc.OutputLevel  = DEBUG
 IOVRegSvc.RecreateFolders = RecreateFolder 

###########################################################################
	
