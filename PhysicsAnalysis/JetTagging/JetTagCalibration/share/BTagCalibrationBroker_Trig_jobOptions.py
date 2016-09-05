from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='data':
  BTaggingFlags.TrigCalibrationFolderRoot=BTaggingFlags.TrigCalibrationFolderRoot.replace("/GLOBAL/TrigBTagCalib","/GLOBAL/Onl/TrigBTagCalib")
  connSchema="GLOBAL"
  message = "#BTAG (Trig)# accessing online conditions DB"
  if not athenaCommonFlags.isOnline():
    message = message + " (via offline replica)"
  print message
else:
  connSchema="GLOBAL_OFL"
  print "#BTAG (Trig)# accessing offline conditions DB"

print "#BTAG (Trig)# running in trigger mode -> btag calibration root folder is TrigCalibrationFolderRoot =", BTaggingFlags.TrigCalibrationFolderRoot

theFolders = []
# Here were defined the calibration folder root tagger per tagger (obsolete scheme in previous release < 21)

JetCollectionForCalib = []
JetCollectionForCalib+= BTaggingFlags.Jets
if "AntiKt4EMTopo" not in JetCollectionForCalib:
  JetCollectionForCalib+=["AntiKt4EMTopo"]

from JetTagCalibration.JetTagCalibrationConf import Analysis__CalibrationBroker
BTagCalibrationBrokerTool_Trig = Analysis__CalibrationBroker(
  name = "BTagCalibrationBrokerTool_Trig",
  folderRoot = BTaggingFlags.TrigCalibrationFolderRoot,
  folders = theFolders,
  channels = JetCollectionForCalib,
  channelAliases = BTaggingFlags.CalibrationChannelAliases,
  shadowFoldersAndChannels = BTaggingFlags.CalibrationSingleFolder,
  OutputLevel = BTaggingFlags.OutputLevel
  )
ToolSvc += BTagCalibrationBrokerTool_Trig
theApp.Dlls+=['DetDescrCondExample','DetDescrCondTools']

from IOVDbSvc.CondDB import conddb 

# with new scheme, only one actual COOL folder:
if BTaggingFlags.CalibrationSingleFolder:
  theFolders = []
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'RUN12')

for folder in theFolders:
  if BTaggingFlags.CalibrationFromLocalReplica:
    dbname="OFLP200"
    #For data, use COMP200 for Run 1, CONDBR2 for Run 2
    if globalflags.DataSource()=='data':
      dbname=conddb.dbdata
    conddb.addFolder("","<dbConnection>sqlite://X;schema=mycool.db;dbname="+dbname+"</dbConnection> "+folder+" <tag>"+BTaggingFlags.TrigCalibrationTag+"</tag>")
  else:
    if BTaggingFlags.CalibrationFromCERN:
      conddb.addFolder("","<dbConnection>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_GLOBAL;dbname=OFLP200</dbConnection> "+folder+" <tag>"+BTaggingFlags.TrigCalibrationTag+"</tag>") 
    else: 
      if BTaggingFlags.TrigCalibrationTag == "":
        conddb.addFolder(connSchema,folder) 
      else:
        conddb.addFolder(connSchema,folder+" <tag>"+BTaggingFlags.TrigCalibrationTag+"</tag>") 

if BTaggingFlags.OutputLevel < 3: 
  print BTagCalibrationBrokerTool_Trig

