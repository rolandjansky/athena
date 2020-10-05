###############################################################
#
# Job options for converting NOVA/MySQLCondDB to POOL/COOL
#   use the correct authentication.xml for creating scheme and writing
#   to COOL 
#==============================================================

# DetDescrVersion = "DC2" 
# DetDescrVersion = "Rome-Initial" 
# DetDescrVersion = "ATLAS-DC3-02"
DetDescrVersion = "ATLAS-DC3-04"

# Switches: 
# items 
RunNumber = 1
RecreateFolder = False
SchemaPrivilege = False
WriteIOV=False

# use McEventSelector
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

theApp.Dlls += [ "GaudiAud" ] 
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors =  [ "ChronoAuditor" ] 

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
theApp.Dlls += [ "LArRawUtils" ]
theApp.Dlls += [ "LArTools" ]

# Other LAr related 
include( "LArIdCnv/LArIdCnv_joboptions.py" )

include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )

include( "LArCondCnv/LArCondCnv_G4_jobOptions.py" )

LArDC2IOVDbTag = LArDC2IOVDbTag+"-IdFix1"

OutputPOOLFileName = "LArRawConditions.MC."+LArDC2IOVDbTag+".001.pool.root" 
# ATLAS 
include( "LArCondCnv/LArCondCnv_IdMapAtlas_jobOptions.py" )

# Objects  and its tag
ObjectList = []
TagList = []


ObjectList += ["LArRampMC#LArRamp#/LAR/ElecCalibMC/Ramp"]
TagList += ["LARElecCalibMCRamp-"+LArDC2IOVDbTag]

ObjectList += ["LArAutoCorrMC#LArAutoCorr#/LAR/ElecCalibMC/AutoCorr"]
TagList += ["LARElecCalibMCAutoCorr-"+LArDC2IOVDbTag]

ObjectList += ["LArDAC2uAMC#LArDAC2uA#/LAR/ElecCalibMC/DAC2uA"]
TagList += ["LARElecCalibMCDAC2uA-"+LArDC2IOVDbTag]

ObjectList += ["LArPedestalMC#LArPedestal#/LAR/ElecCalibMC/Pedestal"]
TagList += ["LARElecCalibMCPedestal-"+LArDC2IOVDbTag]

ObjectList += ["LArNoiseMC#LArNoise#/LAR/ElecCalibMC/Noise"]
TagList += ["LARElecCalibMCNoise-"+LArDC2IOVDbTag]

ObjectList += ["LArfSamplMC#LArfSampl#/LAR/ElecCalibMC/fSampl"]
TagList += ["LARElecCalibMCfSampl-"+LArDC2IOVDbTag]

ObjectList += ["LAruA2MeVMC#LAruA2MeV#/LAR/ElecCalibMC/uA2MeV"]
TagList += ["LARElecCalibMCuA2MeV-"+LArDC2IOVDbTag]

ObjectList += ["LArMinBiasMC#LArMinBias#/LAR/ElecCalibMC/MinBias"]
TagList += ["LARElecCalibMCMinBias-"+LArDC2IOVDbTag]

ObjectList += ["LArShape32MC#LArShape#/LAR/ElecCalibMC/Shape"]
TagList += ["LARElecCalibMCShape-"+LArDC2IOVDbTag]

#/--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = DEBUG
#--------------------------------------------------------------
EventSelector = Service( "EventSelector" )
EventSelector.RunNumber=1
#EventSelector.EventsPerRun=10;
EventSelector.EventsPerRun=2
EventSelector.FirstEvent=1

include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )

include( "LArCondAthenaPool/LArCondAthenaPool_joboptions.py" )

theApp.EvtMax=1 

# include regstration alg (default is WriteIOV = False)
include("RegistrationServices/OutputConditionsAlg_jobOptions.py")

# List of objects container type#key#foldername
OutputConditionsAlg.ObjectList = ObjectList 
OutputConditionsAlg.IOVTagList = TagList

ToolSvc = Service("ToolSvc") 
ToolSvc.ConditionsAlgStream.OutputFile = OutputPOOLFileName

# Set flag to register and run interval Run1/Event1 to Run2/Event2
#   Usually, only need to set Run1, others go to default
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
	
AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel = INFO

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel  = DEBUG
MessageSvc.defaultLimit = 10000;
MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

theApp.Dlls += [ "GaudiAud" ] 
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors =  [ "ChronoAuditor" ]

###########################################################################

#11.5.0 fix for MuonIDFileName
DetDescrCnvSvc.MuonIDFileName = "IdDictMuonSpectrometer_R.01.xml"
