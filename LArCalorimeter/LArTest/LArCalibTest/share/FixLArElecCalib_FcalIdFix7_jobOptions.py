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

DetDescrVersion = "ATLAS-DC3-05"

# Switches: 
# items 
RunNumber = 1

# flags . turn them on when ready to write to COOL
RecreateFolder  = False
SchemaPrivilege = False
WriteIOV = False

# Objects  and its tag
ObjectList = []
TagList = []


# FIX 2
TagNameForFix = "DC3-B-IdFix7"
ObjectList += ["LArDAC2uAMC#LArDAC2uA#/LAR/ElecCalibMC/DAC2uA"]
TagList += ["LARElecCalibMCDAC2uA-"+TagNameForFix]

ObjectList += ["LAruA2MeVMC#LAruA2MeV#/LAR/ElecCalibMC/uA2MeV"]
TagList += ["LARElecCalibMCuA2MeV-"+TagNameForFix]

ObjectList += ["LArAutoCorrMC#LArAutoCorr#/LAR/ElecCalibMC/AutoCorr"]
TagList += ["LARElecCalibMCAutoCorr-"+TagNameForFix]

ObjectList += ["LArNoiseMC#LArNoise#/LAR/ElecCalibMC/Noise"]
TagList += ["LARElecCalibMCNoise-"+TagNameForFix]

ObjectList += ["LArRampMC#LArRamp#/LAR/ElecCalibMC/Ramp"]
TagList += ["LARElecCalibMCRamp-"+TagNameForFix]

ObjectList += ["LArShape32MC#LArShape#/LAR/ElecCalibMC/Shape"]
TagList += ["LARElecCalibMCShape-"+TagNameForFix]

ObjectList += ["LArMinBiasMC#LArMinBias#/LAR/ElecCalibMC/MinBias"]
TagList += ["LARElecCalibMCMinBias-"+TagNameForFix]

ObjectList += ["LArfSamplMC#LArfSampl#/LAR/ElecCalibMC/fSampl"]
TagList += ["LARElecCalibMCfSampl-"+TagNameForFix]

ObjectList += ["LArPedestalMC#LArPedestal#/LAR/ElecCalibMC/Pedestal"]
TagList += ["LARElecCalibMCPedestal-"+TagNameForFix]

OutputPOOLFileName = "LArConditionData_"+TagNameForFix+"_v1.pool.root" 

#/--------------------------------------------------------------
# Algorithm to fix the LAr Id, if needed
#/------------------------------- 
theApp.Dlls += [ "LArConditionsTest" ]
theApp.TopAlg += [ "FixLArElecCalib" ]
FixLArElecCalib = Algorithm("FixLArElecCalib")
FixLArElecCalib.FixFlag =2
# FixLArElecCalib.FixFlag =0 

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
theApp.Dlls += [ "LArTools" ]

LArIdMapFix=7

# Other LAr related
include( "LArIdCnv/LArIdCnv_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )

#include( "LArCondCnv/LArCondCnv_G4_jobOptions.py" )
#include( "LArCondCnv/LArCondCnv_IdMapAtlas_jobOptions.py" )

include( "LArRawConditions/LArRawConditions_MC_jobOptions.py" )
#include( "LArRawConditions/LArIdMap_ATLAS_jobOptions.py" )
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py")

DetDescrCnvSvc.LArIDFileName ="IdDictLArCalorimeter_DC3-05-Comm-01.xml"
DetDescrCnvSvc.IdDictFromRDB=False

#--------------------------------------------------------------
EventSelector = Service( "EventSelector" )
EventSelector.RunNumber=1
#EventSelector.EventsPerRun=10;
EventSelector.EventsPerRun=2
EventSelector.FirstEvent=1

include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )

theApp.EvtMax=1 

AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel = DEBUG

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel  = DEBUG
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
 OutputConditionsAlg.Event1   = 0

 # Set the connection string
 include ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )
 IOVDbSvc = Service( "IOVDbSvc" )

 if (SchemaPrivilege):
        # To recreate folder or add DB, need to use schema account:
        IOVDbSvc.dbConnection="impl=cool;techno=oracle;schema=ATLAS_COOL_LAR;ATLAS_COOLPROD:OFLP130:ATLAS_COOL_LAR:"
 else:
        # For db writing
        IOVDbSvc.dbConnection="impl=cool;techno=oracle;schema=ATLAS_COOL_LAR;ATLAS_COOLPROD:OFLP130:ATLAS_COOL_LAR_W:"


 # For schema creation - only should be used when creating the folder,
 # i.e. the first time
 IOVRegSvc = Service( "IOVRegistrationSvc" )
 IOVRegSvc.OutputLevel  = DEBUG
 if (RecreateFolder):
      IOVRegSvc.RecreateFolders = True	
###########################################################################
	
#11.5.0 fix for MuonIDFileName
DetDescrCnvSvc.MuonIDFileName = "IdDictMuonSpectrometer_R.01.xml"
# DetDescrCnvSvc.DoIdChecks = True;

