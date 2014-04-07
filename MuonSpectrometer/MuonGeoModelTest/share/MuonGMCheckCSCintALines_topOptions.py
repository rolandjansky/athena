#----
from AthenaCommon.AthenaCommonFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import jobproperties,athenaCommonFlags
# import service manager
from AthenaCommon.AppMgr import ServiceMgr
import AthenaCommon.AtlasUnixStandardJob


# Turn off all detector systems except the Muon Spectrometer
from AthenaCommon.DetFlags import DetFlags
#DetFlags.detdescr.all_setOff()
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOff()
DetFlags.detdescr.Muon_setOn()

# Select geometry version
if not 'DetDescrVersion' in dir():
    DetDescrVersion="ATLAS-GEO-16-00-01"

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = DetDescrVersion

if not 'ConditionsTag' in dir():
    ConditionsTag="OFLCOND-SIM-00-00-00"
    # to test conditions with misalignments 
    # ConditionsTag="OFLCOND-DR-BS900-A1-01"
import IOVDbSvc.IOVDb
ServiceMgr.IOVDbSvc.GlobalTag=ConditionsTag


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

# Modify A line parameters
# GeneralControlAlines = I1 * 100000 + I2 * 10000 + I3 * 1000 + I4 * 100 + I5 * 10 + I6
# I1 = 0/1 zerofy or not the s translation
# I2 = 0/1 zerofy or not the z translation
# I3 = 0/1 zerofy or not the t translation
# I4 = 0/1 zerofy or not the s rotation
# I5 = 0/1 zerofy or not the z rotation
# I6 = 0/1 zerofy or not the t rotation
GeneralControlAlines = 111111

#from AthenaCommon.AppMgr import ServiceMgr as svcMgr;
#svcMgr.MessageSvc.countInactive=True'

# Set output level threshold 
#(0=NIL 1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel = INFO
MessageSvc.defaultLimit = 99999999
MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
#MessageSvc.Format = "% F%30W%S%7W%R%T %0W%M"
MessageSvc.useColors = True 
MessageSvc.countInactive=True

#***************************************************** HERE override MuonSpectrometer tag
from AtlasGeoModel import SetGeometryVersion
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
#GeoModelSvc.MuonVersionOverride = "MuonSpectrometer-R.01.01.Initial.Light"

print GeoModelSvc

#***************************************************** HERE define alignment data from cond. tags
if not 'useAlign' in dir():
    useAlign = True

if not 'IAcsc' in dir():
    IAcsc = False

if useAlign:
    from IOVDbSvc.CondDB import conddb
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
    MGM_AlignmentDbTool.OutputLevel = DEBUG

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
MuonDetectorTool.SelectedStations  = [ "CSL" ] # irrelevant if stationselection is 0
MuonDetectorTool.SelectedStJzz = [-1, 1]
MuonDetectorTool.SelectedStJff = [3]
# alignment control 
MuonDetectorTool.ControlAlines     = GeneralControlAlines         # default is 111111
MuonDetectorTool.DumpAlines = True
if useAlign:
    MuonDetectorTool.UseConditionDb = 1; #default is 1 
else:
    MuonDetectorTool.UseConditionDb = 0; # override default 
MuonDetectorTool.UseAsciiConditionData = 0; #default is 0 
#MuonDetectorTool.AlternateASZTFile='asztfile.txt' #if enabled the Alines in Oracle (if any) will not be used
MuonDetectorTool.EnableCscInternalAlignment = IAcsc
MuonDetectorTool.DumpCscInternalAlines = True
MuonDetectorTool.ControlCscInternalAlines = 111111
MuonDetectorTool.AlternateCscIntAlignFile='IACSCtest.txt' #if enabled the CSC int Alines in Oracle (if any) will not be used
# cutouts 
MuonDetectorTool.IncludeCutouts = 1;
MuonDetectorTool.IncludeCutoutsBog = 1;
MuonDetectorTool.IncludeCtbBis = 1;
MuonDetectorTool.EnableFineClashFixing = 0
# inert materials
#MuonDetectorTool.ForceSwitchOnOff_BUILDINERTMATERIALS = 1000;# avoid building the dead matter
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
 
#MessageSvc.setDebug+=[ "MuonGM::MuonChamber" ]
#MessageSvc.setDebug+=[ "MuonGM::MdtReadoutElement" ]
#MessageSvc.setDebug+=[ "MuonGM::CscReadoutElement" ]
#MessageSvc.setDebug+=[ "MuonGM::TgcReadoutElement" ]
#MessageSvc.setDebug+=[ "MuonGM::RpcReadoutElement" ]
MessageSvc.setDebug+=["MGM::MuonDetectorManager::updateAlignment"]

print MuonDetectorTool

#***************************************************** HERE setup MuonGMCheck
from MuonGeoModelTest.MuonGeoModelTestConf import MuonGMCheck
MuonGMCheck = MuonGMCheck()
MuonGMCheck.EventLoopMode = GMCheckLoop
MuonGMCheck.check_mdt = 0
MuonGMCheck.check_rpc = 0
MuonGMCheck.check_tgc = 0
MuonGMCheck.check_csc = 1
MuonGMCheck.check_surfaces = 1
MuonGMCheck.minimalChecks = 0
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


