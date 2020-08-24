loadInDetRec_Options = {"detectorDescription" : ""
                        ,"applyLBibldistTweak" : True
                        ,"globalTag" : ""
                        ,"siPoolFile":""
                        ,"siAlignmentTag":""
                        ,"trtAlignmentTag":""
                        ,"trtAlignmentTagL3":""
                        ,"lorentzAngleTag":""
                        ,"beamSpotTag":""
                        ,"errorScalingTag":""
                        ,"DigitalClustering": False
                        ,"TRTCalibTextFile":""
                        ,"TRTCalibT0TagCos":""
                        ,"TRTCalibRtTagCos":""
                        ,"Cosmics":False
                        ,"BField":False
                        ,"readConstantsFromPool" : False       #  whether to read initial alignment constants from pool file
                        ,"readBowingFromCool" : False          #  whether to read initial IBL bowing from cool file
                        ,"readSilicon" : True                  #  whether to read initial Si alignment constants from pool file
                        ,"readTRT" : True                      #  whether to read initial TRT alignment constnats from pool file
                        ,"readTRTL3" : False                   #  whether to read initial TRT L3 alignment constnats from pool file
                        ,"inputPoolFiles" : ["IDalignment_nominal.pool.root"]  #  pool files to read the constants from,
                        ,"inputBowingDatabase"  : "" #cool file to read the bowing from
                        }


# If not defined the defaults given above are used
for var in loadInDetRec_Options:
  if var in dir():
    loadInDetRec_Options[var] = eval(var)

'''
# Enable LB-IOV sensitive L3 transform tweak (we don't want this in the solving job)
if not loadInDetRec_Options["applyLBibldistTweak"]:
  from PixelGeoModel.PixelGeoModelConf import PixelDetectorTool
  pixelTool =  PixelDetectorTool()
  pixelTool.TweakIBLDist = False
'''
    

from IOVDbSvc.CondDB import conddb
  #conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterial','AtlasLayerMat_v15_ATLAS-IBL3D25-04-00-01')
#conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterialV2','AtlasLayerMat_v18_ATLAS-R2-2015-01')


if not loadInDetRec_Options["BField"] and loadInDetRec_Options["Cosmics"]:
  InDetTrackFitter.GetMaterialFromTrack = False
  InDetTrackFitter.Momentum = 100000. *MeV


if loadInDetRec_Options["siPoolFile"]:
  include ("DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )
  from IOVDbSvc.CondDB import conddb
    # block folders that you want to override
  conddb.blockFolder("/Indet/Align")
  conddb.blockFolder("/TRT/Align")
  from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
  from AthenaCommon.AppMgr import ServiceMgr
  ServiceMgr += CondProxyProvider()
  ServiceMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
  printfunc ('Loading initial alignment File')
  ServiceMgr.CondProxyProvider.InputCollections = [ loadInDetRec_Options["siPoolFile"] ]
  ServiceMgr.CondProxyProvider.OutputLevel=INFO
  printfunc (ServiceMgr.CondProxyProvider)
  IOVSvc = Service("IOVSvc")
  IOVSvc.preLoadData = True

else:
  if loadInDetRec_Options["siAlignmentTag"]:
    conddb.addOverride('/Indet/Align',loadInDetRec_Options["siAlignmentTag"])
  if loadInDetRec_Options["trtAlignmentTag"]:
    conddb.addOverride('/TRT/Align',loadInDetRec_Options["trtAlignmentTag"])
  if loadInDetRec_Options["trtAlignmentTagL3"] and not loadInDetRec_Options["Cosmics"]:
    conddb.addOverride('/TRT/Calib/DX',loadInDetRec_Options["trtAlignmentTagL3"])


# Read in constants from Pool file
if loadInDetRec_Options["readConstantsFromPool"]:
  from IOVDbSvc.CondDB import conddb
  
  if loadInDetRec_Options["readSilicon"]:
    conddb.blockFolder("/Indet/Align")
    
  if loadInDetRec_Options["readTRT"]:
    conddb.blockFolder("/TRT/Align")
      
  if loadInDetRec_Options["readTRTL3"]:
    conddb.blockFolder("/TRT/Calib/DX")

  #if loadInDetRec_Options["readBowingFromCool"]:
  #  conddb.blockFolder("/Indet/IBLDist")
  #  conddb.addFolderWithTag('','<dbConnection>sqlite://X;schema='+loadInDetRec_Options["inputBowingDatabase"]+';dbname=CONDBR2</dbConnection>/Indet/IBLDist','IndetIBLDist',True);
    
  from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
  from AthenaCommon.AppMgr import ServiceMgr
  ServiceMgr += CondProxyProvider()
  ServiceMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
  # set this to the file containing AlignableTransform objects
  ServiceMgr.CondProxyProvider.InputCollections += loadInDetRec_Options["inputPoolFiles"]
  printfunc ("INPUT POOL FILES COLLECTION", ServiceMgr.CondProxyProvider.InputCollections )
  ServiceMgr.CondProxyProvider.OutputLevel = DEBUG
  printfunc (ServiceMgr.CondProxyProvider)
  # this preload causes callbacks for read in objects to be activated,
  # allowing GeoModel to pick up the transforms
  ServiceMgr.IOVSvc.preLoadData = True
  ServiceMgr.IOVSvc.OutputLevel = DEBUG
  


if loadInDetRec_Options["errorScalingTag"]:
  if ".db" in loadInDetRec_Options["errorScalingTag"]:
    conddb.blockFolder("/Indet/TrkErrorScaling")
    
    #conddb.addFolder("","<dbConnection>sqlite://X;schema="+loadInDetRec_Options["errorScalingTag"]+";dbname=CONDBR2</dbConnection> /Indet/TrkErrorScaling" + "<tag>IndetTrkErrorScaling_nominal</tag>", force=True)
    #conddb.addFolderWithTag('','<dbConnection>sqlite://X;schema='+loadInDetRec_Options["errorScalingTag"]+';dbname=OFLP200</dbConnection>/Indet/TrkErrorScaling','IndetTrkErrorScaling_nominal',True ); 
    conddb.addFolderWithTag('','<dbConnection>sqlite://X;schema='+loadInDetRec_Options["errorScalingTag"]+';dbname=CONDBR2</dbConnection>/Indet/TrkErrorScaling','IndetTrkErrorScaling_nominal',True );
    #conddb.addFolderWithTag(loadInDetRec_Options["errorScalingTag"],'/Indet/TrkErrorScaling','IndetTrkErrorScaling_nominal',True );
    printfunc ("INFO:: ErrorScaling from local Database")
  else:
    conddb.addOverride('/Indet/TrkErrorScaling',loadInDetRec_Options["errorScalingTag"])
#Added if you put an empty ErrorScalingTag
#else:
#  conddb.addOverride('/Indet/TrkErrorScaling','IndetTrkErrorScaling_nominal')


if loadInDetRec_Options["beamSpotTag"] and not loadInDetRec_Options["Cosmics"]:
  conddb.addOverride('/Indet/Beampos',loadInDetRec_Options["beamSpotTag"])

if loadInDetRec_Options["lorentzAngleTag"] and not loadInDetRec_Options["Cosmics"]:
  conddb.addOverride('/PIXEL/LorentzAngleScale',loadInDetRec_Options["lorentzAngleTag"])
  
from AthenaCommon.AppMgr import ToolSvc
from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackErrorScalingTool
InDetRotErrorScalingTool = Trk__RIO_OnTrackErrorScalingTool( name = 'RIO_OnTrackErrorScalingTool',
                                                             overrideDatabaseID = False,
                                                             overrideScalePixBarX = 1,
                                                             overrideScalePixBarY = 1,
                                                             overrideScalePixECsX = 1,
                                                             overrideScalePixECsY = 1,
                                                             overrideScaleSCTBar = 1,
                                                             overrideScaleSCTECs = 1,
                                                             overrideScaleTRTBar = 1,
                                                             overrideScaleTRTECs = 1,
                                                             OutputLevel = INFO )
#InDetRotErrorScalingTool.overrideDatabaseID=False
#InDetRotErrorScalingTool.overrideScaleTRT=3
#InDetRotErrorScalingTool.overrideScaleSCT=3
#InDetRotErrorScalingTool.overrideScalePix=30

ToolSvc += InDetRotErrorScalingTool
printfunc (InDetRotErrorScalingTool)

  
# Correct TRT calibration for cosmics
if loadInDetRec_Options["TRTCalibTextFile"] and loadInDetRec_Options["Cosmics"]:
  from AthenaCommon.AppMgr import ToolSvc
  from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbTool
  TRTCalibDBTool=TRT_CalDbTool(name="TRT_CalDbTool")
  
  conddb.blockFolder("/TRT/Calib/RT" )
  conddb.blockFolder("/TRT/Calib/T0" )
  TRTCalibDBSvc.calibTextFile = loadInDetRec_Options["TRTCalibTextFile"]

if loadInDetRec_Options["TRTCalibT0TagCos"]:
  conddb.addOverride('/TRT/Calib/T0',loadInDetRec_Options["TRTCalibT0TagCos"])
if loadInDetRec_Options["TRTCalibRtTagCos"]:
  conddb.addOverride('/TRT/Calib/RT',loadInDetRec_Options["TRTCalibRtTagCos"])

doJiveXML=False  
if doJiveXML:
  ToolSvc.TrackRetriever.OtherTrackCollections =["CombinedInDetTracks", "CombinedInDetTracks_CTB"]



#Run Digital
if loadInDetRec_Options["DigitalClustering"]:
    printfunc ("##### INFO: Running with Digital clustering")
    ToolSvc.InDetPixelClusterOnTrackTool.PositionStrategy=0
    ToolSvc.InDetPixelClusterOnTrackTool.ErrorStrategy=1



if loadInDetRec_Options["inputBowingDatabase"]:
  printfunc ("INFO:: parsed: ",loadInDetRec_Options["inputBowingDatabase"])
  if ".db" in loadInDetRec_Options["inputBowingDatabase"]:
    printfunc ("INFO:: blocking IBLDist Folder")
    conddb.blockFolder("/Indet/IBLDist")
    printfunc ("INFO:: Adding folder with tag")
    from IOVDbSvc.CondDB import conddb
    conddb.addFolderWithTag('','<dbConnection>sqlite://X;schema='+loadInDetRec_Options["inputBowingDatabase"]+';dbname=CONDBR2</dbConnection>/Indet/IBLDist','IndetIBLDist',True);
    #conddb.addFolderWithTag(loadInDetRec_Options["errorScalingTag"],'/Indet/TrkErrorScaling','IndetTrkErrorScaling_nominal',True );
    printfunc ("INFO:: IBL Bowing from local Database")
  else:
    printfunc ("INFO:: Overriding Database Tag")
    conddb.addOverride('/Indet/IBLDist',loadInDetRec_Options["inputBowingDatabase"])

