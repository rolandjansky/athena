
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
if athenaCommonFlags.isOnline():
  BTaggingFlags.CalibrationFolderRoot="/GLOBAL/Onl/BTagCalib/"
  connSchema="GLOBAL"
  print "#BTAG# running online -> btag calib root folder is /GLOBAL/Onl/BTagCalib/"
else:
  BTaggingFlags.CalibrationFolderRoot="/GLOBAL/BTagCalib/"
  connSchema="GLOBAL_OFL"
  print "#BTAG# running offline -> btag calib root folder is /GLOBAL/BTagCalib/"

theFolders = []
#if BTaggingFlags.IP3D:
theFolders.append(BTaggingFlags.CalibrationFolderRoot+'IP3D')
theFolders.append(BTaggingFlags.CalibrationFolderRoot+'SV1')
theFolders.append(BTaggingFlags.CalibrationFolderRoot+'JetFitter')

JetCollectionForCalib = []
JetCollectionForCalib+= BTaggingFlags.Jets


from JetTagCalibration.JetTagCalibrationConf import Analysis__CalibrationBroker
BTagCalibrationBrokerTool = Analysis__CalibrationBroker(
                                                          name = "BTagCalibrationBrokerTool",
                                                          folderRoot = BTaggingFlags.CalibrationFolderRoot,
                                                          folders = theFolders,
                                                          channels = JetCollectionForCalib,
                                                          channelAliases = BTaggingFlags.CalibrationChannelAliases,
                                                          shadowFoldersAndChannels = BTaggingFlags.CalibrationSingleFolder,   
                                                          OutputLevel = BTaggingFlags.OutputLevel
                                                       )
ToolSvc += BTagCalibrationBrokerTool
theApp.Dlls+=['DetDescrCondExample','DetDescrCondTools']

from IOVDbSvc.CondDB import conddb 

# with new scheme, only one actual COOL folder:
if BTaggingFlags.CalibrationSingleFolder:
  theFolders = []
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'RUN12')

for folder in theFolders:
  if BTaggingFlags.CalibrationFromLocalReplica:
    conddb.addFolder("","<dbConnection>sqlite://X;schema=mycool.db;dbname=OFLP200</dbConnection> "+folder+" <tag>"+BTaggingFlags.CalibrationTag+"</tag>")
  else:
    if BTaggingFlags.CalibrationFromCERN:
      conddb.addFolder("","<dbConnection>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_GLOBAL;dbname=OFLP200</dbConnection> "+folder+" <tag>"+BTaggingFlags.CalibrationTag+"</tag>") 
    else: 
      if BTaggingFlags.CalibrationTag == "":
        conddb.addFolder(connSchema,folder) 
      else:
        conddb.addFolder(connSchema,folder+" <tag>"+BTaggingFlags.CalibrationTag+"</tag>") 

if BTaggingFlags.OutputLevel < 3: 
  print BTagCalibrationBrokerTool

