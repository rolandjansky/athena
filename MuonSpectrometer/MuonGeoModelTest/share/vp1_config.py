useABLinesData=False


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
    useBLines    = 1
    GeneralControlAlines = 0
    

if useAlignData:
    useAlign = True

######################################################################## setup GeoModel
from AthenaCommon.GlobalFlags import GlobalFlags
#GlobalFlags.DetGeo.set_atlas()
#GlobalFlags.DataSource.set_geant4()
#GlobalFlags.Luminosity.set_zero()

# Turn off all detector systems except the Muon Spectrometer
from AthenaCommon.DetFlags import DetFlags
#DetFlags.detdescr.all_setOff()
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOff()
DetFlags.detdescr.Muon_setOn()

from IOVDbSvc.CondDB import conddb
if not 'ConditionsTag' in dir():
    #ConditionsTag="COMCOND-BLKPST-004-00" ### remember to activate the options IOVDbSvc.CondDB options for data like conditions -> COMP200  with useAlignData
    ConditionsTag="OFLCOND-SIM-00-00-00" ### for MC condition tags OFLP200 (default DB choice) will work out of teh box

if useAlignData:
    print "Setting up DB name / Instance to COMP200 (default is OFLP200)"
    conddb.dbname = "COMP200"
    conddb.iovdbsvc.DBInstance="COMP200"

import IOVDbSvc.IOVDb
ServiceMgr.IOVDbSvc.GlobalTag=ConditionsTag

# Select geometry version
if not 'DetDescrVersion' in dir():
    DetDescrVersion="ATLAS-GEO-10-00-01"

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = DetDescrVersion


#***************************************************** HERE override MuonSpectrometer tag
from AtlasGeoModel import SetGeometryVersion
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
#GeoModelSvc.MuonVersionOverride = "MuonSpectrometer-R.03.14"

print GeoModelSvc

#***************************************************** HERE define alignment data from cond. tags

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
MuonDetectorTool.SelectedStations  = [ "X" ] # irrelevant if stationselection is 0 - this will switch off all stations
#MuonDetectorTool.SelectedStations  = [ "EIL", "T", "CSS", "BMS" ] # irrelevant if stationselection is 0
#MuonDetectorTool.SelectedStations  = [ "BIS" ] # irrelevant if stationselection is 0
MuonDetectorTool.SelectedStations  = [ "T1E", "T4E" ] # irrelevant if stationselection is 0
MuonDetectorTool.SelectedStJzz = [1, 2]
MuonDetectorTool.SelectedStJff = [3]
# alignment control 
MuonDetectorTool.ControlAlines     = GeneralControlAlines         # default is 111111
MuonDetectorTool.DumpAlines = True
if useAlign:
    MuonDetectorTool.UseConditionDb = 1; #default is 1 
else:
    MuonDetectorTool.UseConditionDb = 0; # override default 
MuonDetectorTool.UseAsciiConditionData = 0; #default is 0 
MuonDetectorTool.EnableMdtDeformations = useBLines
#MuonDetectorTool.AlternateASZTFile='TGCALinesT1E3station' #if enabled the Alines in Oracle (if any) will not be used
MuonDetectorTool.AlternateASZTFile='TGCALinesVP1' #if enabled the Alines in Oracle (if any) will not be used
# cutouts 
#MuonDetectorTool.IncludeCutouts = 1;
#MuonDetectorTool.IncludeCutoutsBog = 1;
#MuonDetectorTool.IncludeCtbBis = 1;
#MuonDetectorTool.EnableFineClashFixing = 0
# inert materials
MuonDetectorTool.ForceSwitchOnOff_BUILDINERTMATERIALS = 1000;# avoid building the dead matter
#MuonDetectorTool.ForceSwitchOnOff_BUILDINERTMATERIALS = 1001;# avoid building the dead matter
#MuonDetectorTool.ForceSwitchOnOff_MINIMALGEO          = 1000;    
MuonDetectorTool.ForceSwitchOnOff_BUILDENDCAP         = 1000;
MuonDetectorTool.ForceSwitchOnOff_BUILDCALOSADDLE     = 1000;     
MuonDetectorTool.ForceSwitchOnOff_BUILDBARRELTOROID   = 1000;
MuonDetectorTool.ForceSwitchOnOff_BUILDENDCAPTOROID   = 1000;
MuonDetectorTool.ForceSwitchOnOff_BUILDFEET           = 1000;
MuonDetectorTool.ForceSwitchOnOff_BUILDDISKSHIELD     = 1000;
MuonDetectorTool.ForceSwitchOnOff_BUILDTOROIDSHIELD   = 1000;
MuonDetectorTool.ForceSwitchOnOff_BUILDFORWARDSHIELD  = 1000;

MuonDetectorTool.OutputLevel=DEBUG
 
#MessageSvc.setDebug+=[ "MuonGM::MuonChamber" ]
#MessageSvc.setDebug+=[ "MuonGM::MdtReadoutElement" ]
#MessageSvc.setDebug+=[ "MuonGM::CscReadoutElement" ]
#MessageSvc.setDebug+=[ "MuonGM::TgcReadoutElement" ]
#MessageSvc.setDebug+=[ "MuonGM::RpcReadoutElement" ]
MessageSvc.setDebug+=["MGM::MuonDetectorManager::updateAlignment"]


