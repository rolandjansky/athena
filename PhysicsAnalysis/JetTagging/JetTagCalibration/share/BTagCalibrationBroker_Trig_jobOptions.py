from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='data':
  BTaggingFlags.TrigCalibrationFolderRoot=BTaggingFlags.TrigCalibrationFolderRoot.replace("/GLOBAL","/GLOBAL/Onl")
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
if BTaggingFlags.IP1D:
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'IP1D')
if BTaggingFlags.IP2D:
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'IP2D')
if BTaggingFlags.IP3D:
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'IP3D')
if BTaggingFlags.SV1:
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'SV1')
if BTaggingFlags.SV2:
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'SV2')
if BTaggingFlags.SoftMu:
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'SoftMu')
if BTaggingFlags.SoftEl:
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'SoftEl')
if BTaggingFlags.JetFitterTag|BTaggingFlags.JetFitterNN: 
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'JetFitter')
if BTaggingFlags.JetProb:
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'JetProb')
if BTaggingFlags.MV1 | BTaggingFlags.MV1Flip:
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'MV1')
if BTaggingFlags.MV1c | BTaggingFlags.MV1cFlip:
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'MV1c')
if BTaggingFlags.MV2:
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'MV2')
if BTaggingFlags.MV2c00 | BTaggingFlags.MV2c00Flip:
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'MV2c00')
if BTaggingFlags.MV2c10 | BTaggingFlags.MV2c10Flip:
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'MV2c10')
if BTaggingFlags.MV2c20 | BTaggingFlags.MV2c20Flip:
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'MV2c20')
if BTaggingFlags.MV2c100 | BTaggingFlags.MV2c100Flip:
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'MV2c100')
if BTaggingFlags.MV2m | BTaggingFlags.MV2mFlip:
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'MV2m')
if BTaggingFlags.MV3_bVSu | BTaggingFlags.MV3_bVSc | BTaggingFlags.MV3_cVSu | BTaggingFlags.MV3_bVSuFlip | BTaggingFlags.MV3_bVScFlip | BTaggingFlags.MV3_cVSuFlip:
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'MV3')
if BTaggingFlags.JetFitterCharm | BTaggingFlags.JetFitterCharmNeg :
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'JetFitterCharm')
if BTaggingFlags.MVb:
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'MVb')
if BTaggingFlags.Gaia | BTaggingFlags.GaiaNeg :
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'Gaia')
if BTaggingFlags.MVbPrime |  BTaggingFlags.MVbPrimeFlip :
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'MVbPrime')
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'MVbCharm')
if BTaggingFlags.MultiSVbb1:
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'MultiSVbb1')
if BTaggingFlags.MultiSVbb2:
  theFolders.append(BTaggingFlags.TrigCalibrationFolderRoot+'MultiSVbb2')


JetCollectionForCalib = []
JetCollectionForCalib+= BTaggingFlags.Jets
if "AntiKt4TopoEM" not in JetCollectionForCalib:
  JetCollectionForCalib+=["AntiKt4TopoEM"]

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
      from AtlasGeoModel.InDetGMJobProperties import GeometryFlags as geoFlags
      # Run() only exists for ATLAS-R1(...) and ATLAS-R2(...) geo tags,
      # not for ATLAS-GEO(...) and ATLAS-IBL(...) ones.
      # Hence if Run() is undefined,
      # presence of IBL is used to switch between Run1/Run2
      if (geoFlags.Run() == "RUN1" or (geoFlags.Run() == "UNDEFINED" and geoFlags.isIBL() == False)):
        dbname="COMP200"
      else:
        dbname="CONDBR2"
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

