#$Id $

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
  FromRun=1429
  ToRun  =1429
  FromEvent=0
  ToEvent=99999999
  NovaTag="EC04-Default"
  Tag=NovaTag
  doUpdate=False
  OutputPOOLFileName = "LArRawConditions_H6_v1.pool.root"

  useTCMOFC=False
  useRTMOFC=True

if not "ObjectList" in dir():
  #Syntax: Type#Key#Folder
  ObjectList = ["LArPedestalComplete#LArPedestal#/LAR/LArElecCalibEC04/LArPedestal",
                "LArH6OscillationComplete#LArH6Oscillation#/LAR/LArElecCalibEC04/LArH6Oscillation",
                "LArAutoCorrComplete#LArAutoCorr#/LAR/LArElecCalibEC04/LArAutoCorr",
                "LArRampComplete#LArRamp#/LAR/LArElecCalibEC04/LArRamp",
                "LArOFCComplete#LArOFC#/LAR/LArElecCalibEC04/LArOFC",
                "LAruA2MeVComplete#LAruA2MeV#/LAR/LArElecCalibEC04/LAruA2MeV",
                "LArDAC2uAComplete#LArDAC2uA#/LAR/LArElecCalibEC04/LArDAC2uA",
                "LArPhysWaveContainer#LArPhysWave#/LAR/LArElecCalibEC04/LArPhysWaveContainer/LArPhysWave",
                "LArShapeComplete#LArShape#/LAR/LArElecCalibEC04/LArShapeComplete/LArPhysWave"
                ]
  #Must have the same order as the ObjectList and the tags must be globally unique
  # therefore, we add the name of each object to the tag
  TagList=["LArPedestal-"+Tag,
           "LArH6Oscillation-"+Tag,
           "LArAutoCorr-"+Tag,
           "LArRamp-"+Tag,
           "LArOFC-"+Tag,
           "LAruA2MeV-"+Tag,
           "LArDAC2uA-"+Tag,
           "LArPhysWave-"+Tag,
           "LArShape-"+Tag
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

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

EventSelector = Service( "EventSelector" )
if "RunNumber" in dir():
  EventSelector.RunNumber=RunNumber
else:
  EventSelector.RunNumber=FromRun
#EventSelector.EventsPerRun=10;
EventSelector.EventsPerRun=2
EventSelector.FirstEvent=FromEvent

include("LArIdCnv/LArIdCnv_joboptions.py")
include("LArCondCnv/LArCondCnv_IdMapH6_jobOptions.py")
include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )


### Read old DB here ####

include( "LArCondCnv/LArCondCnv_Config_jobOptions.py" )
include( "LArCondCnv/LArCondCnv_IOVDbSvc_jobOptions.py" )
IOVDbSvc = Service( "IOVDbSvc" )
LArEC04FolderList=["/lar/LArElecCalibEC04/LArPedestal",
                   "/lar/LArElecCalibEC04/LArH6Oscillation",
                   "/lar/LArElecCalibEC04/LArAutoCorr",
                   "/lar/LArElecCalibEC04/LArOFC",
                   "/lar/LArElecCalibEC04/LArRamp",
                   "/lar/LArElecCalibEC04/LArDAC2uA",
                   "/lar/LArElecCalibEC04/LAruA2MeV",
                   "/lar/LArElecCalibEC04/LArPhysWaveContainer/LArPhysWave",
                   "/lar/LArElecCalibEC04/LArShapeComplete/LArPhysWave"
                   ]

ServerDB = "atlobk02.cern.ch:conditions_ctb_2004:conditions:conditions"
for i in range(len(LArEC04FolderList)) :
  LArEC04FolderList[i] += '<tag>'+ Tag + '</tag>'
  LArEC04FolderList[i] += '<dbConnection>'+  ServerDB + '</dbConnection>'

IOVDbSvc.Folders += LArEC04FolderList





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


if DoWriteCOOLIOV:
  # Set the connection string
  include ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )
  IOVDbSvc = Service( "IOVDbSvc" )
  IOVDbSvc.OutputLevel=DEBUG

  if CreateSchema:
    IOVDbSvc.dbConnection = "impl=cool;techno=oracle;schema=ATLAS_COOL_LAR;INTR:LAREC04:ATLAS_COOL_LAR:"
    IOVRegSvc = Service( "IOVRegistrationSvc" )
    IOVRegSvc.OutputLevel  = DEBUG
    IOVRegSvc.RecreateFolders = True
  else:
    IOVDbSvc.dbConnection = "impl=cool;techno=oracle;schema=ATLAS_COOL_LAR;INTR:LAREC04:ATLAS_COOL_LAR_W:"

	
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO

DetectorStore = Service( "DetectorStore" )
DetectorStore.Dump = TRUE
DetectorStore.OutputLevel = INFO

ToolSvc.ConditionsAlgStream.OutputLevel=DEBUG

#IOVDBSvc=Service("IOVDBSvc")
#IOVDBSvc.OutputLevel=DEBUG






