# run with >>>>>>> athena -c 'sect5=True;useBLinesData=True' MuonGMCheckApplyBLines_topOptions.py >&! log
if not 'sect5' in dir():
    sect5 = False
    
if not 'useAlign' in dir():
    useAlign = False
if not 'useAlignData' in dir():
    useAlignData = False
if not 'useABLinesData' in dir():
    useABLinesData = False

if not 'useBLines' in dir():
    useBLines = 0

if useABLinesData: 
    useAlignData = True
    useBLines    = 1

# Modify A line parameters
# GeneralControlAlines = I1 * 100000 + I2 * 10000 + I3 * 1000 + I4 * 100 + I5 * 10 + I6
# I1 = 0/1 zerofy or not the s translation
# I2 = 0/1 zerofy or not the z translation
# I3 = 0/1 zerofy or not the t translation
# I4 = 0/1 zerofy or not the s rotation
# I5 = 0/1 zerofy or not the z rotation
# I6 = 0/1 zerofy or not the t rotation
GeneralControlAlines = 111111
#GeneralControlAlines = 0

if not 'useBLinesData' in dir():
    useBLinesData = False

if useBLinesData: 
    useAlignData = True
    #useBLines    = 1100000 # sp 
    #useBLines    = 1010000 # sn 
    #useBLines    = 1001000 # tw 
    #useBLines    = 1000100 # eg 
    #useBLines    = 1000010 # ep 
    #useBLines    = 1000001 # en
    useBLines     = 1100000 ###### my test case 
    GeneralControlAlines = 0
    

if useAlignData:
    useAlign = True

include ("RecJobTransforms/UseOracle.py")
#----
from AthenaCommon.AthenaCommonFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import jobproperties,athenaCommonFlags
# import service manager
from AthenaCommon.AppMgr import ServiceMgr
import AthenaCommon.AtlasUnixStandardJob

######################################################################## setup GeoModel
from AthenaCommon.GlobalFlags import GlobalFlags
#GlobalFlags.DetGeo.set_atlas()
#GlobalFlags.DataSource.set_geant4()
#GlobalFlags.Luminosity.set_zero()

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.DataSource.set_Value_and_Lock('geant4')

# Turn off all detector systems except the Muon Spectrometer
from AthenaCommon.DetFlags import DetFlags
#DetFlags.detdescr.all_setOff()
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOff()
DetFlags.detdescr.Muon_setOn()

from IOVDbSvc.CondDB import conddb
if not 'ConditionsTag' in dir():
    if useAlignData:
        ConditionsTag="COMCOND-BLKPA-006-01" ### remember to activate the options IOVDbSvc.CondDB options for data like conditions -> COMP200  with useAlignData
        #ConditionsTag="COMCOND-BLKPST-004-00" ### remember to activate the options IOVDbSvc.CondDB options for data like conditions -> COMP200  with useAlignData
    else:
        ConditionsTag="OFLCOND-SIM-00-00-00" ### for MC condition tags OFLP200 (default DB choice) will work out of teh box

if useAlignData:
    print "Setting up DB name / Instance to COMP200 (default is OFLP200)"
    conddb.dbname = "COMP200"
    conddb.iovdbsvc.DBInstance="COMP200"

import IOVDbSvc.IOVDb
ServiceMgr.IOVDbSvc.GlobalTag=ConditionsTag

# Select geometry version
if not 'DetDescrVersion' in dir():
    DetDescrVersion="ATLAS-GEO-20-00-01"

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = DetDescrVersion


from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
######################################################################## end setup GeoModel

# 0 to run MuonGM checks init time only
# 1 to run MuonGM checks at each event
GMCheckLoop = 1

if GMCheckLoop:
    #import AthenaPoolCnvSvc.ReadAthenaPool
    #ServiceMgr.EventSelector.InputCollections = [ "/afs/le.infn.it/user/s/spagnolo/projAtlas/hlt/stef/data/mc12.csc120.007218.digit.muminus_pt20GeV._00001.pool.root" ]
    theApp.EvtMax = 1
else:
    theApp.EvtSel = "NONE"
    theApp.EvtMax = 0


# Set output level threshold 
#(0=NIL 1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel = INFO
MessageSvc.defaultLimit = 99999999
MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
#Messagesvc.Format = "% F%30W%S%7W%R%T %0W%M"
#MessageSvc.useColors = True 

#***************************************************** HERE override MuonSpectrometer tag
from AtlasGeoModel import SetGeometryVersion
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
#GeoModelSvc.MuonVersionOverride = "MuonSpectrometer-R.01.01.Initial.Light"

print GeoModelSvc


#***************************************************** HERE define alignment data from cond. tags
if useAlign:
    from IOVDbSvc.CondDB import conddb
    conddb.dbname = "COMP200"
    conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/BARREL','/MUONALIGN/MDT/BARREL')
    conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEA','/MUONALIGN/MDT/ENDCAP/SIDEA')
    conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEC','/MUONALIGN/MDT/ENDCAP/SIDEC')
    conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEA','/MUONALIGN/TGC/SIDEA')
    conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEC','/MUONALIGN/TGC/SIDEC')
    from MuonCondTool.MuonCondToolConf import MuonAlignmentDbTool
    MuonAlignmentDbTool = MuonAlignmentDbTool("MGM_AlignmentDbTool")
    MuonAlignmentDbTool.ParlineFolders = ["/MUONALIGN/MDT/BARREL",
                                          "/MUONALIGN/MDT/ENDCAP/SIDEA",
                                          "/MUONALIGN/MDT/ENDCAP/SIDEC",
                                          "/MUONALIGN/TGC/SIDEA",
                                          "/MUONALIGN/TGC/SIDEC"]
    ToolSvc += MuonAlignmentDbTool
    MGM_AlignmentDbTool = ToolSvc.MGM_AlignmentDbTool
    MGM_AlignmentDbTool.OutputLevel = INFO
    MGM_AlignmentDbTool.DumpALines = False
    
#***************************************************** HERE setup MuonDetectorManager
from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
MuonDetectorTool = MuonDetectorTool()
# memory management 
MuonDetectorTool.FillCacheInitTime = 0 # default is 0
#MuonDetectorTool.DumpMemoryBreakDown = True # default is false
# selection of active stations 
MuonDetectorTool.StationSelection  = 1 # default is 0
#MuonDetectorTool.SelectedStations  = [ "X" ] # irrelevant if stationselection is 0 - this will switch off all stations
#MuonDetectorTool.SelectedStations  = [ "EIL", "T", "CSS", "BMS" ] # irrelevant if stationselection is 0
if sect5:
    MuonDetectorTool.StationSelection  = 1 # default is 0
    MuonDetectorTool.SelectedStations  = [ "BOL" , "EML" ] # irrelevant if stationselection is 0
    #MuonDetectorTool.SelectedStations  = [ "BOL" ] # irrelevant if stationselection is 0
    #MuonDetectorTool.SelectedStations  = [ "EML" ] # irrelevant if stationselection is 0
    MuonDetectorTool.SelectedStJzz = [-1, 1]
    MuonDetectorTool.SelectedStJff = [3]
    #MuonDetectorTool.SelectedStJzz = [-1, 1]
    #MuonDetectorTool.SelectedStJff = [6]
   
#MuonDetectorTool.SelectedStations  = [ "BIR" ] # irrelevant if stationselection is 0
#MuonDetectorTool.SelectedStJzz = [4]
#MuonDetectorTool.SelectedStJff = [6]
# alignment control 
MuonDetectorTool.ControlAlines     = GeneralControlAlines         # default is 111111
MuonDetectorTool.DumpAlines = False
if useAlign:
    MuonDetectorTool.UseConditionDb = 1; #default is 1 
else:
    MuonDetectorTool.UseConditionDb = 0; # override default 
MuonDetectorTool.EnableMdtDeformations = useBLines
MuonDetectorTool.UseAsciiConditionData = 0; #default is 0 
#MuonDetectorTool.AlternateASZTFile='asztfile.txt' #if enabled the Alines in Oracle (if any) will not be used
# cutouts 
MuonDetectorTool.IncludeCutouts = 1;
MuonDetectorTool.IncludeCutoutsBog = 1;
MuonDetectorTool.IncludeCtbBis = 1;
MuonDetectorTool.EnableFineClashFixing = 0
# inert materials
MuonDetectorTool.ForceSwitchOnOff_BUILDINERTMATERIALS = 1000;# avoid building the dead matter
#MuonDetectorTool.ForceSwitchOnOff_BUILDINERTMATERIALS = 1001;# avoid building the dead matter
#MuonDetectorTool.ForceSwitchOnOff_MINIMALGEO          = 1000;    
#MuonDetectorTool.ForceSwitchOnOff_BUILDENDCAP         = 1000;
#MuonDetectorTool.ForceSwitchOnOff_BUILDCALOSADDLE     = 1000;     
#MuonDetectorTool.ForceSwitchOnOff_BUILDBARRELTOROID   = 1000;
#MuonDetectorTool.ForceSwitchOnOff_BUILDENDCAPTOROID   = 1000;
#MuonDetectorTool.ForceSwitchOnOff_BUILDFEET           = 1001;
#MuonDetectorTool.ForceSwitchOnOff_BUILDDISKSHIELD     = 1000;
#MuonDetectorTool.ForceSwitchOnOff_BUILDTOROIDSHIELD   = 1000;
#MuonDetectorTool.ForceSwitchOnOff_BUILDFORWARDSHIELD  = 1000;

MuonDetectorTool.OutputLevel=DEBUG
 
#MessageSvc.setDebug+=[ "MuGM:MuonStation" ]
#MessageSvc.setDebug+=[ "MuonGM::MuonChamber" ]
#MessageSvc.setDebug+=[ "MuonGM::MdtReadoutElement" ]
#MessageSvc.setDebug+=[ "MuonGM::CscReadoutElement" ]
#MessageSvc.setDebug+=[ "MuonGM::TgcReadoutElement" ]
#MessageSvc.setDebug+=[ "MuonGM::RpcReadoutElement" ]
MessageSvc.setDebug+=["MGM::MuonDetectorManager::updateAlignment"]
MessageSvc.setDebug+=["MGM::MuonDetectorManager::updateDeformations"]

print MuonDetectorTool

#***************************************************** HERE setup MuonGMCheck
from MuonGeoModelTest.MuonGeoModelTestConf import MuonGMCheck
MuonGMCheck = MuonGMCheck()
MuonGMCheck.EventLoopMode = GMCheckLoop
MuonGMCheck.check_mdt = 1
MuonGMCheck.check_rpc = 0
MuonGMCheck.check_tgc = 0
MuonGMCheck.check_csc = 0
MuonGMCheck.minimalChecks = 1
MuonGMCheck.check_first_last = 2 #//0
MuonGMCheck.check_Blines = 1
MuonGMCheck.testMdtCache = 0
MuonGMCheck.testRpcCache = 0
MuonGMCheck.testTgcCache = 0
MuonGMCheck.testCscCache = 0
MuonGMCheck.testMdtDetectorElementHash = 0
MuonGMCheck.testRpcDetectorElementHash = 0
MuonGMCheck.testTgcDetectorElementHash = 0
MuonGMCheck.testCscDetectorElementHash = 0

MuonGMCheck.OutputLevel = INFO

print MuonGMCheck


######################################################
############### HERE DEFINE THE SEQUENCE #############
######################################################

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()
topSequence += MuonGMCheck

print " "
print "List of all Dlls"
print theApp.Dlls
print " "
print "List of all Ext services"
print theApp.ExtSvc
print " "
print "List of all top algorithms"
print theApp.TopAlg

#print "Print here Top Sequence" 
#print topSequence
#print "Print here Service Manager" 
#print ServiceMgr


