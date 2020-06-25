#####################################################################
# JobOption file to copy LAr conditons data from Nova to COOL/POOL  #
#                                                                   #
# If run 'as is', it will write all conditions for a run defind     #
# below. To convert many IOVs, these jobO can be invoked by shell-  #
# script that sets the necessary parameters.                        #
#                                                                   #
# Currently, the validation server is used (INTR)                   # 
#####################################################################


if not "FromRun" in dir():
  FromRun=1001046
  ToRun  =1001046
  FromEvent=0
  ToEvent=99999999
  NovaTag="TB04-Default"
  Tag=NovaTag
  doUpdate=False
  OutputPOOLFileName = "LArRawConditions_H8_OFC_v1.pool.root"

  useTCMOFC=False
  useRTMOFC=True

if not "ObjectList" in dir():
  #Syntax: Type#Key#Folder
  ObjectList = ["LArCalibParams#LArCalibParams#/LAR/LArElecCalibH8/LArCalibParams",
                "LArPedestalComplete#LArPedestal#/LAR/LArElecCalibH8/LArPedestal",
                "LArAutoCorrComplete#LArAutoCorr#/LAR/LArElecCalibH8/LArAutoCorr",
                "LArRampComplete#LArRamp#/LAR/LArElecCalibH8/LArRamp",
                "LAruA2MeVComplete#LAruA2MeV#/LAR/LArElecCalibH8/LAruA2MeV",
                "LArDAC2uAComplete#LArDAC2uA#/LAR/LArElecCalibH8/LArDAC2uA",
                "LArMphysOverMcalComplete#LArMphysOverMcal#/LAR/LArElecCalibH8/LArMphysOverMcal"
                ]
  #Must have the same order as the ObjectList and the tags must be globally unique
  # therefore, we add the name of each object to the tag
  TagList=["LArCalibParams-"+Tag,
           "LArPedestal-"+Tag,
           "LArAutoCorr-"+Tag,
           "LArRamp-"+Tag,
           "LAruA2MeV-"+Tag,
           "LArDAC2uA-"+Tag,
           "LArMPhysMcal-"+Tag
           ]
  
  if useRTMOFC:
    ObjectList+=["LArOFCComplete#LArOFC#/LAR/LArElecCalibH8/LArOFCPhys/RTM",
                 "LArPhysWaveContainer#LArPhysWave#/LAR/LArElecCalibH8/LArPhysWaveContainer/RTM",
                 "LArShapeComplete#LArShape#/LAR/LArElecCalibH8/LArShape/RTM"
                 ]
    
    TagList+=["LArOFCPhysRTM-"+Tag,
              "LArPhysWaveRTM-"+Tag,
              "LArShapeRTM-"+Tag
              ]

  if useTCMOFC:
    ObjectList+=["LArOFCComplete#LArOFC#/LAR/LArElecCalibH8/LArOFCPhys/TCM",
                 "LArPhysWaveContainer#LArPhysWave#/LAR/LArElecCalibH8/LArPhysWaveContainer/TCM",
                 "LArShapeComplete#LArShape#/LAR/LArElecCalibH8/LArShape/TCM",
                 ]
    TagList+=["LArOFCPhysTCM-"+Tag,
              "LArPhysWaveTCM-"+Tag,
              "LArShapeTCM-"+Tag
              ]


#Set to 'True' in order to write IOV to COOL
#A soft-link to the approriate authentication.xml must be present
if not "DoWriteCOOLIOV" in dir():
  DoWriteCOOLIOV=False

#Set to 'True' in order to create the schema in COOL
#A soft-link to the approriate authentication.xml must be present
if not "CreateSchema" in dir():
  CreateSchema=False


from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()
#DetDescrVersion = "CTB-H8"

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

EventSelector = Service( "EventSelector" )
if "RunNumber" in dir():
  EventSelector.RunNumber=RunNumber
else:
  EventSelector.RunNumber=FromRun
#EventSelector.EventsPerRun=10;
EventSelector.EventsPerRun=2
EventSelector.FirstEvent=FromEvent

LArCondCnvDbServer = 'atlobk02.cern.ch'
#include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H8_joboptions.py" )

include("LArIdCnv/LArIdCnv_joboptions.py")
include("LArCondCnv/LArCondCnv_IdMapH8_jobOptions.py")
include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )
#include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

useLArCTBCoolPool=False

include("LArCondCnv/LArCondCnv_TB04_jobOptions.py")
IOVDbSvc.Folders += LArTB04Folder_Calib   

# Number of events to be processed (default is 10)
theApp.EvtMax = 1


include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )

include( "LArCondAthenaPool/LArCondAthenaPool_joboptions.py" )

# include regstration alg (default is WriteIOV = False)
include("RegistrationServices/OutputConditionsAlg_jobOptions.py")

# List of objects container type#key#foldername
OutputConditionsAlg.ObjectList = ObjectList 
ToolSvc = Service("ToolSvc")

ToolSvc.ConditionsAlgStream.OutputFile = OutputPOOLFileName
if doUpdate:
  PoolSvc=Service("PoolSvc")
  PoolSvc.FileOpen="update"

## # Set flag to register and run interval Run1/Event1 to Run2/Event2
## #   Usually, only need to set Run1, others go to default
OutputConditionsAlg.WriteIOV   = DoWriteCOOLIOV
OutputConditionsAlg.Run1       = FromRun
OutputConditionsAlg.Event1     = FromEvent
OutputConditionsAlg.Run2       = ToRun
OutputConditionsAlg.Event2     = ToEvent
OutputConditionsAlg.IOVTagList = TagList
OutputConditionsAlg.OutputLevel= DEBUG

## if (CreateSchema):
## 	# To recreate folder or add DB, need to use schema account:
## 	IOVDbSvc.dbConnection = "impl=cool;techno=oracle;schema=ATLAS_COOL_LAR;INTR:LARCTB:ATLAS_COOL_LAR:"
## else:
## 	# For db writing
## 	IOVDbSvc.dbConnection = "impl=cool;techno=oracle;schema=ATLAS_COOL_LAR;INTR:LARCTB:ATLAS_COOL_LAR_W:"

if DoWriteCOOLIOV:
  # Set the connection string
  include ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )
  IOVDbSvc = Service( "IOVDbSvc" )
  IOVDbSvc.OutputLevel=DEBUG

  if CreateSchema:
    IOVDbSvc.dbConnection = "impl=cool;techno=oracle;schema=ATLAS_COOL_LAR;INTR:LARCTB:ATLAS_COOL_LAR:"
    IOVRegSvc = Service( "IOVRegistrationSvc" )
    IOVRegSvc.OutputLevel  = DEBUG
    IOVRegSvc.RecreateFolders = True
  else:
    IOVDbSvc.dbConnection = "impl=cool;techno=oracle;schema=ATLAS_COOL_LAR;INTR:LARCTB:ATLAS_COOL_LAR_W:"

	
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO

DetectorStore = Service( "DetectorStore" )
DetectorStore.Dump = TRUE
DetectorStore.OutputLevel = INFO


#IOVDBSvc=Service("IOVDBSvc")
#IOVDBSvc.OutputLevel=DEBUG






