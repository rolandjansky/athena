from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='data':
  BTaggingFlags.CalibrationFolderRoot=BTaggingFlags.CalibrationFolderRoot.replace("/GLOBAL/BTagCalib","/GLOBAL/Onl/BTagCalib")
  connSchema="GLOBAL"
  message = "#BTAG (AODFix)# accessing online conditions DB"
  if not athenaCommonFlags.isOnline():
    message = message + " (via offline replica)"
  print message
else:
  connSchema="GLOBAL_OFL"
  print "#BTAG (AODFix)# accessing offline conditions DB"

print "#BTAG (AODFix)# running in AODFix mode -> btag calibration root folder is CalibrationFolderRoot =", BTaggingFlags.CalibrationFolderRoot

theFolders = []
# Here were defined the calibration folder root tagger per tagger (obsolete scheme in previous release < 21)

JetCollectionForCalib = []
JetCollectionForCalib+= BTaggingFlags.Jets
if "AntiKt4EMTopo" not in JetCollectionForCalib:
  JetCollectionForCalib+=["AntiKt4EMTopo"]

from JetTagCalibration.JetTagCalibrationConf import Analysis__CalibrationBroker
BTagCalibrationBrokerTool_AODFix = Analysis__CalibrationBroker(
  name = "BTagCalibrationBrokerTool_AODFix",
  folderRoot = BTaggingFlags.CalibrationFolderRoot,
  folders = theFolders,
  channels = JetCollectionForCalib,
  channelAliases = BTaggingFlags.CalibrationChannelAliases,
  shadowFoldersAndChannels = BTaggingFlags.CalibrationSingleFolder,
  OutputLevel = BTaggingFlags.OutputLevel
  )
ToolSvc += BTagCalibrationBrokerTool_AODFix
theApp.Dlls+=['DetDescrCondExample','DetDescrCondTools']

from IOVDbSvc.CondDB import conddb 

# with new scheme, only one actual COOL folder:
if BTaggingFlags.CalibrationSingleFolder:
  theFolders = []
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'RUN12')

for folder in theFolders:
  if BTaggingFlags.CalibrationFromLocalReplica:
    dbname="OFLP200"
    #For data, use COMP200 for Run 1, CONDBR2 for Run 2
    if globalflags.DataSource()=='data':
      dbname=conddb.dbdata
    conddb.addFolder("","<dbConnection>sqlite://X;schema=mycool.db;dbname="+dbname+"</dbConnection> "+folder+" <tag>"+BTaggingFlags.CalibrationTag+"</tag>")
  else:
    if BTaggingFlags.CalibrationFromCERN:
      conddb.addFolder("","<dbConnection>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_GLOBAL;dbname=OFLP200</dbConnection> "+folder+" <tag>"+BTaggingFlags.CalibrationTag+"</tag>") 
    else: 
      if BTaggingFlags.CalibrationTag == "":
        conddb.addFolder(connSchema,folder) 
      else:
        conddb.addFolder(connSchema,folder+" <tag>"+BTaggingFlags.CalibrationTag+"</tag>") 

if BTaggingFlags.OutputLevel < 3: 
  print BTagCalibrationBrokerTool_AODFix

