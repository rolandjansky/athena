###############################################################
#
# Job options file 1
#
#==============================================================
#use McEventSelector
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

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


DetDescrVersion = "ATLAS-CSC-02-00-00"
# DetDescrVersion = "ATLAS-DC3-05"
# LArIdMapFix=7 
# G4Phys ="QGSP_EMV"
# G4Phys ="QGSP_BERT"

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
if DetDescrVersion == "ATLAS-CSC-02-00-00" :
  TagNameForFix = "CSC02-F"
else :
  TagNameForFix = "Wrong"
  print " ERROR: wrong DetDescrVersion" 

ObjectList += ["LArNoiseMC#LArNoise#/LAR/ElecCalibMC/Noise"]
ObjectList += ["LAruA2MeVMC#LAruA2MeV#/LAR/ElecCalibMC/uA2MeV"]
ObjectList += ["LArDAC2uAMC#LArDAC2uA#/LAR/ElecCalibMC/DAC2uA"]
ObjectList += ["LArRampMC#LArRamp#/LAR/ElecCalibMC/Ramp"]


TagList += ["LARElecCalibMCNoise-"+TagNameForFix]
TagList += ["LARElecCalibMCuA2MeV-"+TagNameForFix]
TagList += ["LARElecCalibMCDAC2uA-"+TagNameForFix]
TagList += ["LARElecCalibMCRamp-"+TagNameForFix]

OutputPOOLFileName = "LArFCalADC2MeV_13.0.30_v1.pool.root" 

#/--------------------------------------------------------------
# Algorithm to fix the LAr Id, if needed
#/------------------------------- 
theApp.Dlls += [ "LArConditionsTest" ]
theApp.TopAlg += [ "FixLArElecCalib" ]
FixLArElecCalib = Algorithm("FixLArElecCalib")
# 1=
# 2=fix for IdMapFix=1
# 3=new fsample for CSC-02
# 5=new FCAL noise and minbias
FixLArElecCalib.FixFlag =6
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
EventSelector = Service( "EventSelector" )
EventSelector.RunNumber=1
#EventSelector.EventsPerRun=10;
EventSelector.EventsPerRun=2
EventSelector.FirstEvent=1

include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )

theApp.EvtMax=1 

AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel = INFO

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel  = INFO
MessageSvc.defaultLimit = 1000000;
MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

theApp.Dlls += [ "GaudiAud" ] 
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors =  [ "ChronoAuditor" ]

##############################################
# Writing POOL and COOL 
if len(ObjectList)>0 : 

 # include regstration alg (default is WriteIOV = False)
 include("RegistrationServices/OutputConditionsAlg_jobOptions.py")

 # List of objects container type#key#foldername
 OutputConditionsAlg.ObjectList = ObjectList 
 OutputConditionsAlg.IOVTagList = TagList

 ToolSvc = Service("ToolSvc") 
 ToolSvc.ConditionsAlgStream.OutputFile = OutputPOOLFileName

 # Set flag to register and run interval Run1/Event1 to Run2/Event2
 #   Usually, only need to set Run1, others go to default
 #### 

 OutputConditionsAlg.WriteIOV = WriteIOV
 OutputConditionsAlg.Run1     = 0
 OutputConditionsAlg.LB1   = 0

 # Set the connection string
 include ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )
 IOVDbSvc = Service( "IOVDbSvc" )

 IOVDbSvc.dbConnection="impl=cool;techno=sqlite;schema=LArElecCalib_FCalADC2MeV.db;X:OFLP200"

 # For schema creation - only should be used when creating the folder,
 # i.e. the first time
 IOVRegSvc = Service( "IOVRegistrationSvc" )
 IOVRegSvc.OutputLevel  = DEBUG
 IOVRegSvc.RecreateFolders = RecreateFolder 

 #  PoolSvc.FileOpen = "update"
###########################################################################
	
