###############################################################
#
# Job options file 1
#
#==============================================================
#use McEventSelector
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )
from AthenaCommon.AlgSequence import AlgSequence
topSequence  = AlgSequence()

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DataSource.set_geant4()
GlobalFlags.InputFormat.set_pool()
GlobalFlags.DetGeo.set_atlas()

# G4Phys ="QGSP_EMV"
G4Phys ="QGSP_BERT"
LArfSamplG4Phys = G4Phys

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion="ATLAS-CSC-02-00-00"
#jobproperties.Global.DetDescrVersion = "ATLAS-DC3-05"
#LArIdMapFix=7 
 

# Switches: 
# items 
RunNumber = 1

#
RecreateFolder = False
WriteIOV = True

# Objects  and its tag
ObjectList = []
TagList = []


# FIX  
if jobproperties.Global.DetDescrVersion() == "ATLAS-CSC-02-00-00" :
  TagNameForFix = "CSC02-D-"+G4Phys
elif jobproperties.Global.DetDescrVersion() == "ATLAS-DC3-05" :
  TagNameForFix = "CSC01-D-"+G4Phys
else :
  TagNameForFix = "Wrong"
  print " ERROR: wrong DetDescrVersion" 


ObjectList += ["LArfSamplMC#LArfSampl#/LAR/ElecCalibMC/fSampl"]
TagList += ["LARElecCalibMCfSampl-"+TagNameForFix]

OutputPOOLFileName = "LArfSampl_1303_v1.pool.root" 

#/--------------------------------------------------------------
# Algorithm to fix the LAr Id, if needed
#/------------------------------- 

#from LArConditionsTest.LArConditionsTestConf import FixLArElecCalib
#topSequence  += FixLArElecCalib()

theApp.Dlls += [ "LArConditionsTest" ]
theApp.TopAlg += [ "FixLArElecCalib" ]
FixLArElecCalib = Algorithm("FixLArElecCalib")

# 1=
# 2=fix for IdMapFix=1
# 3=new fsample for CSC-02
# 7 = fsampl from 13.0.30
topSequence.FixLArElecCalib.FixFlag =7
topSequence.FixLArElecCalib.G4Phys =G4Phys
#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
theApp.Dlls += [ "LArTools" ]

include ("AtlasGeoModel/SetGeometryVersion.py")
include ("AtlasGeoModel/GeoModelInit.py")

# Other LAr related
include( "LArIdCnv/LArIdCnv_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )

include( "LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py" )
include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )

#--------------------------------------------------------------
EventSelector = svcMgr.EventSelector
EventSelector.RunNumber=1
#EventSelector.EventsPerRun=10;
EventSelector.EventsPerRun=2
EventSelector.FirstEvent=1

include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )

theApp.EvtMax=1 

AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel = DEBUG 

MessageSvc = svcMgr.MessageSvc
MessageSvc.OutputLevel  = INFO
MessageSvc.defaultLimit = 1000000;
MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

#theApp.Dlls += [ "GaudiAud" ] 
#theAuditorSvc = AuditorSvc()
#theAuditorSvc.Auditors =  [ "ChronoAuditor" ]

##############################################
# Writing POOL and COOL 
if len(ObjectList)>0 : 

 include( "RegistrationServices/IOVRegistrationSvc_jobOptions.py" )
 from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
 topSequence += OutputConditionsAlg(outputFile=OutputPOOLFileName)

 # List of objects container type#key#foldername
 topSequence.OutputConditionsAlg.ObjectList = ObjectList 
 topSequence.OutputConditionsAlg.IOVTagList = TagList

 # Set flag to register and run interval Run1/Event1 to Run2/Event2
 #   Usually, only need to set Run1, others go to default
 #### 

 topSequence.OutputConditionsAlg.WriteIOV = WriteIOV
 topSequence.OutputConditionsAlg.Run1     = 0
 topSequence.OutputConditionsAlg.LB1   = 0

 # Set the connection string
 include ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )
 IOVDbSvc = svcMgr.IOVDbSvc

 #IOVDbSvc.dbConnection="impl=cool;techno=sqlite;schema=LArElecCalib_13.0.30_v1.db;X:OFLP200"
 IOVDbSvc.dbConnection="sqlite://X;schema=LArElecCalib_13.0.30_v1.db;dbname=OFLP200"

 # For schema creation - only should be used when creating the folder
 # i.e. the first time
 IOVRegSvc = svcMgr.IOVRegistrationSvc
 IOVRegSvc.OutputLevel  = DEBUG
 IOVRegSvc.RecreateFolders = RecreateFolder 

 PoolSvc.FileOpen = "update"

AthenaEventLoopMgr.ClearStorePolicy = "BeginEvent"
 
###########################################################################
	

