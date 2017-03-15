loadInDetRec_Options = {"detectorDescription" : ""
                        ,"applyLBibldistTweak" : True
                        ,"globalTag" : ""
                        ,"siPoolFile":""
                        ,"siAlignmentTag":""
                        ,"trtAlignmentTag":""
                        ,"trtAlignmentTagL3":""
                        ,"dynamicL1IDTag":""
                        ,"dynamicL2PIXTag":""
                        ,"dynamicL2SCTTag":""
                        ,"dynamicL1TRTag":""
                        ,"dynamicL3SiTag":""
                        ,"dynamicL2TRTTag":""
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
                        ,"inputDynamicGlobalDatabase" : "" #cool file to read the dynamic global folders from
                        ,"useDynamicAlignFolders": False 
                        }


# If not defined the defaults given above are used
for var in loadInDetRec_Options:
  if var in dir():
    loadInDetRec_Options[var] = eval(var)

from IOVDbSvc.CondDB import conddb

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
  print 'Loading initial alignment File'
  ServiceMgr.CondProxyProvider.InputCollections = [ loadInDetRec_Options["siPoolFile"] ]
  ServiceMgr.CondProxyProvider.OutputLevel=INFO
  print ServiceMgr.CondProxyProvider
  IOVSvc = Service("IOVSvc")
  IOVSvc.preLoadData = True

else:
  # The global folders for dynamic scheme are loaded below
  if loadInDetRec_Options["dynamicL3SiTag"]:
    conddb.addOverride('/Indet/AlignL3',loadInDetRec_Options["dynamicL3SiTag"])

  if loadInDetRec_Options["dynamicL2TRTTag"]:
    conddb.addOverride('/TRT/AlignL2',loadInDetRec_Options["dynamicL2TRTTag"])

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
    conddb.blockFolder("/Indet/AlignL3")

  if loadInDetRec_Options["readTRT"]:
    conddb.blockFolder("/TRT/Align")
    conddb.blockFolder("/TRT/AlignL2")
      
  if loadInDetRec_Options["readTRTL3"]:
    conddb.blockFolder("/TRT/Calib/DX")

    
  from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
  from AthenaCommon.AppMgr import ServiceMgr
  ServiceMgr += CondProxyProvider()
  ServiceMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
  # set this to the file containing AlignableTransform objects
  ServiceMgr.CondProxyProvider.InputCollections += loadInDetRec_Options["inputPoolFiles"]
  print "INPUT POOL FILES COLLECTION", ServiceMgr.CondProxyProvider.InputCollections 
  ServiceMgr.CondProxyProvider.OutputLevel = DEBUG
  print ServiceMgr.CondProxyProvider
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
    print "INFO:: ErrorScaling from local Database"
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
print InDetRotErrorScalingTool

  
# Correct TRT calibration for cosmics
if loadInDetRec_Options["TRTCalibTextFile"] and loadInDetRec_Options["Cosmics"]:
  from AthenaCommon.AppMgr import ToolSvc
  from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
  TRTCalibDBSvc=TRT_CalDbSvc()
  ServiceMgr+=TRTCalibDBSvc
  
  conddb.blockFolder("/TRT/Calib/RT" )
  conddb.blockFolder("/TRT/Calib/T0" )
  TRTCalibDBSvc.calibTextFile = loadInDetRec_Options["TRTCalibTextFile"]

if loadInDetRec_Options["TRTCalibT0TagCos"]:
  conddb.addOverride('/TRT/Calib/T0',loadInDetRec_Options["TRTCalibT0TagCos"])
if loadInDetRec_Options["TRTCalibRtTagCos"]:
  conddb.addOverride('/TRT/Calib/RT',loadInDetRec_Options["TRTCalibRtTagCos"])

#Trying to fix the Database problem
if loadInDetRec_Options["globalTag"] == "OFLCOND-RUN12-SDR-14":    
  conddb.addOverride('/PIXEL/PixdEdx','PixdEdx_January2011_mc-000-00')
  from AthenaCommon.CfgGetter import getService
  getService("AtlasFieldSvc").UseDCS = False

doJiveXML=False  
if doJiveXML:
  ToolSvc.TrackRetriever.OtherTrackCollections =["CombinedInDetTracks", "CombinedInDetTracks_CTB"]



#Run Digital
if loadInDetRec_Options["DigitalClustering"]:
    print "##### INFO: Running with Digital clustering"
    ToolSvc.InDetPixelClusterOnTrackTool.PositionStrategy=0
    ToolSvc.InDetPixelClusterOnTrackTool.ErrorStrategy=1



if loadInDetRec_Options["inputBowingDatabase"]:
  print "INFO:: parsed: ",loadInDetRec_Options["inputBowingDatabase"]
  if ".db" in loadInDetRec_Options["inputBowingDatabase"]:
    print "INFO:: blocking IBLDist Folder"
    conddb.blockFolder("/Indet/IBLDist")
    print "INFO:: Adding folder with tag"
    from IOVDbSvc.CondDB import conddb
    conddb.addFolderWithTag('','<dbConnection>sqlite://X;schema='+loadInDetRec_Options["inputBowingDatabase"]+';dbname=CONDBR2</dbConnection>/Indet/IBLDist','IndetIBLDist',True);
    #conddb.addFolderWithTag(loadInDetRec_Options["errorScalingTag"],'/Indet/TrkErrorScaling','IndetTrkErrorScaling_nominal',True );
    print "INFO:: IBL Bowing from local Database"
  else:
    print "INFO:: Overriding Database Tag"
    conddb.addOverride('/Indet/IBLDist',loadInDetRec_Options["inputBowingDatabase"])

if loadInDetRec_Options["useDynamicAlignFolders"]:
  if loadInDetRec_Options["inputDynamicGlobalDatabase"]:
    print "INFO:: parsed: ",loadInDetRec_Options["inputDynamicGlobalDatabase"]
    if ".db" in loadInDetRec_Options["inputDynamicGlobalDatabase"]:
      print "INFO:: blocking dynamic Global Folders"
      conddb.blockFolder("/Indet/AlignL1/ID")
      conddb.blockFolder("/Indet/AlignL2/PIX")
      conddb.blockFolder("/Indet/AlignL2/SCT")    
      conddb.blockFolder("/TRT/AlignL1/TRT")

      print "INFO:: Adding folders with tag"
      from IOVDbSvc.CondDB import conddb
      conddb.addFolderWithTag('','<dbConnection>sqlite://X;schema='+loadInDetRec_Options["inputDynamicGlobalDatabase"]+';dbname=CONDBR2</dbConnection>/Indet/AlignL1/ID','IndetL1Test',True);
      conddb.addFolderWithTag('','<dbConnection>sqlite://X;schema='+loadInDetRec_Options["inputDynamicGlobalDatabase"]+';dbname=CONDBR2</dbConnection>/Indet/AlignL2/PIX','IndetL2PIXTest',True);
      conddb.addFolderWithTag('','<dbConnection>sqlite://X;schema='+loadInDetRec_Options["inputDynamicGlobalDatabase"]+';dbname=CONDBR2</dbConnection>/Indet/AlignL2/SCT','IndetL2SCTTest',True);
      conddb.addFolderWithTag('','<dbConnection>sqlite://X;schema='+loadInDetRec_Options["inputDynamicGlobalDatabase"]+';dbname=CONDBR2</dbConnection>/TRT/AlignL1/TRT','IndetL1TRTTest',True);
      print "INFO:: Dynamic global folders from local Database"

  elif loadInDetRec_Options["inputDynamicGlobalDatabase"]=="":
    print "INFO:: Attempting to override Database tags for global folders in Run2 DB scheme"
    if loadInDetRec_Options["dynamicL1IDTag"]:
      conddb.addOverride('/Indet/AlignL1/ID',loadInDetRec_Options["dynamicL1IDTag"])
    if loadInDetRec_Options["dynamicL2PIXTag"]:
      conddb.addOverride('/Indet/AlignL2/PIX',loadInDetRec_Options["dynamicL2PIXTag"])
    if loadInDetRec_Options["dynamicL2SCTTag"]:
      conddb.addOverride('/Indet/AlignL2/SCT',loadInDetRec_Options["dynamicL2SCTTag"])
    if loadInDetRec_Options["dynamicL1TRTag"]:
      conddb.addOverride('/TRT/AlignL1/TRT',loadInDetRec_Options["dynamicL1TRTag"])



