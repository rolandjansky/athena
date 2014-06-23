
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
if BTaggingFlags.IP1D:
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'IP1D')
if BTaggingFlags.IP2D:
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'IP2D')
if BTaggingFlags.IP3D:
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'IP3D')
if BTaggingFlags.SV1:
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'SV1')
if BTaggingFlags.SV2:
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'SV2')
if BTaggingFlags.SoftMu:
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'SoftMu')
if BTaggingFlags.SoftEl:
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'SoftEl')
if BTaggingFlags.JetFitterTag|BTaggingFlags.JetFitterNN: 
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'JetFitter')
if BTaggingFlags.JetProb:
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'JetProb')
if BTaggingFlags.MV1 | BTaggingFlags.MV1Flip:
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'MV1')
if BTaggingFlags.MV1c | BTaggingFlags.MV1cFlip:
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'MV1c')
if BTaggingFlags.MV2:
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'MV2')
if BTaggingFlags.MV2c00 | BTaggingFlags.MV2c00Flip:
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'MV2c00')
if BTaggingFlags.MV2c10 | BTaggingFlags.MV2c10Flip:
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'MV2c10')
if BTaggingFlags.MV2c20 | BTaggingFlags.MV2c20Flip:
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'MV2c20')
if BTaggingFlags.MV3_bVSu | BTaggingFlags.MV3_bVSc | BTaggingFlags.MV3_cVSu | BTaggingFlags.MV3_bVSuFlip | BTaggingFlags.MV3_bVScFlip | BTaggingFlags.MV3_cVSuFlip:
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'MV3')
if BTaggingFlags.JetFitterCharm | BTaggingFlags.JetFitterCharmNeg :
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'JetFitterCharm')
if BTaggingFlags.MVb:
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'MVb')
if BTaggingFlags.Gaia | BTaggingFlags.GaiaNeg :
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'Gaia')
if BTaggingFlags.MVbPrime |  BTaggingFlags.MVbPrimeFlip :
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'MVbPrime')
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'MVbCharm')


JetCollectionForCalib = []
JetCollectionForCalib+= BTaggingFlags.Jets
if "AntiKt4TopoEM" not in JetCollectionForCalib:
  JetCollectionForCalib+=["AntiKt4TopoEM"]

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
  theFolders.append(BTaggingFlags.CalibrationFolderRoot+'ALL')

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

