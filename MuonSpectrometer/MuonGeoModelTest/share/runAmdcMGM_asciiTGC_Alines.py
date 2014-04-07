####### to be run with athena -c "DetDescrVersion='ATLAS-GEO-16-00-01'" runAmdcMGM_asciiTGC_Alines.py
####### for testing MGM vs Amdc with test A-lines (input test Alines files in the test directory)  
if not 'useAlign' in dir():
    useAlign = False
if not 'useAlignData' in dir():
    useAlignData = False

if not 'useAgdd2Geo' in dir():
    useAgdd2Geo = False

if useAlignData:
    useAlign = True
#----
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


from IOVDbSvc.CondDB import conddb
if not 'ConditionsTag' in dir():
    ConditionsTag="COMCOND-BLKPST-004-00" ### remember to activate the options IOVDbSvc.CondDB options for data like conditions -> COMP200 
    #ConditionsTag="OFLCOND-SIM-00-00-00" ### for MC condition tags OFLP200 (default DB choice) will work out of teh box

if useAlignData:
    print "Setting up DB name / Instance to COMP200 (default is OFLP200)"
    conddb.dbname = "COMP200"
    conddb.iovdbsvc.DBInstance="COMP200"

import IOVDbSvc.IOVDb
ServiceMgr.IOVDbSvc.GlobalTag=ConditionsTag


# Select geometry version
if not 'DetDescrVersion' in dir():
    DetDescrVersion="ATLAS-GEO-16-00-01"
    print 'MuonGeoModelTest/runAmdcMGM DetDescrVersion now set to ', DetDescrVersion
else:
    print 'MuonGeoModelTest/runAmdcMGM DetDescrVersion is already ', DetDescrVersion
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = DetDescrVersion

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
######################################################################## end setup GeoModel

theApp.EvtSel = "NONE"
theApp.EvtMax = 1
# Modify A line parameters
# GeneralControlAlines = I1 * 100000 + I2 * 10000 + I3 * 1000 + I4 * 100 + I5 * 10 + I6
# I1 = 0/1 zerofy or not the s translation
# I2 = 0/1 zerofy or not the z translation
# I3 = 0/1 zerofy or not the t translation
# I4 = 0/1 zerofy or not the s rotation
# I5 = 0/1 zerofy or not the z rotation
# I6 = 0/1 zerofy or not the t rotation
GeneralControlAlines = 111111

# Set output level threshold 
#(0=NIL 1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel = INFO


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
    #conddb.addOverride('/MUONALIGN/TGC/SIDEA','MuonAlignTGCEndCapAAlign-TEST-01')
    #conddb.addOverride('/MUONALIGN/TGC/SIDEC','MuonAlignTGCEndCapCAlign-TEST-01')
    conddb.addOverride('/MUONALIGN/TGC/SIDEA','MuonAlignTGCEndCapAAlign-REPRO-2010-03')
    conddb.addOverride('/MUONALIGN/TGC/SIDEC','MuonAlignTGCEndCapCAlign-REPRO-2010-03')
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
    MGM_AlignmentDbTool.DumpALines = True


#***************************************************** HERE setup MuonDetectorManager
from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
MuonDetectorTool = MuonDetectorTool()
MuonDetectorTool.StationSelection = 0
MuonDetectorTool.SelectedStations = [ "EIL", "B" ]
MuonDetectorTool.ControlAlines    = GeneralControlAlines
MuonDetectorTool.ForceSwitchOnOff_BUILDINERTMATERIALS = 1000; # avoid building the dead matter
MuonDetectorTool.ForceSwitchOnOff_MINIMALGEO = 1001;          # IMPORTANT: do not account for cutouts (Amdc does not)
if useAlign:
    MuonDetectorTool.UseConditionDb = 1; #default is 1 
else:
    MuonDetectorTool.UseConditionDb = 0; # override default 
MuonDetectorTool.UseAsciiConditionData = 0; #default is 0 
MuonDetectorTool.OutputLevel = DEBUG
#MuonDetectorTool.AlternateASZTFile = "TGCALinesT1E3"
MuonDetectorTool.AlternateASZTFile = "TGC_COMPsWithTILT"


print MuonDetectorTool
MessageSvc.setDebug+=["MGM::MuonDetectorManager::updateAlignment"]
MessageSvc.setDebug+=["MuGM:MuonStation"]
MessageSvc.defaultLimit=9999999

#***************************************************** HERE setup Amdc 
include ("AmdcAth/AmdcAth_jobOptions.py")
from AmdcAth.AmdcAthConf import AmdcsimrecAthenaSvc
AmdcsimrecAthenaSvc = AmdcsimrecAthenaSvc()

AmdcsimrecAthenaSvc.ControlALine =  GeneralControlAlines
AmdcsimrecAthenaSvc.NameOfTheSource = "ASCII" ;
AmdcsimrecAthenaSvc.AlignmentSource=3 # 2=COOL, 3=external(Oracle only) 
#AmdcsimrecAthenaSvc.NameOfTheAmdbFile='amdb_simrec.r.04.06_TGCALinesT1E3'
AmdcsimrecAthenaSvc.NameOfTheAmdbFile='amdb_simrec.r.04.06TGC_COMPsWithTILT'
#AmdcsimrecAthenaSvc.ControlALine =  0
#Truncate A line rotation parameters
#1/0 truncate/ do not truncate
AmdcsimrecAthenaSvc.TruncateALine = 0;
#Truncate P line rotation parameters
#1/0 truncate/ do not truncate
AmdcsimrecAthenaSvc.TruncatePLine = 0 ;
AmdcsimrecAthenaSvc.OutputLevel=DEBUG

print AmdcsimrecAthenaSvc

#***************************************************** HERE setup AmdcDumpGeoModel
from AmdcMGM.AmdcMGMConf import AmdcDumpGeoModel
AmdcDumpGeoModel = AmdcDumpGeoModel()

#if 1 do nothing 
AmdcDumpGeoModel.SwitchOff = 0
#
#if 1 print only warnings, 
#if 2 print warnings only for precise coordinates, 
#print all otherwise
AmdcDumpGeoModel.WarningLevel = 1
AmdcDumpGeoModel.OutputLevel = DEBUG
#
#CheckTEC if 1 perform the comparison for TEC 
#EpsLoTEC Min value on the position difference to output warning for TEC
#
AmdcDumpGeoModel.CheckMdt = 1
AmdcDumpGeoModel.EpsLoMdt = 0.010
#
AmdcDumpGeoModel.CheckRpc = 1
AmdcDumpGeoModel.EpsLoRpc = 0.010
#
AmdcDumpGeoModel.CheckTgc = 1
AmdcDumpGeoModel.EpsLoTgc = 0.010
#
AmdcDumpGeoModel.CheckCsc = 1
#AmdcDumpGeoModel.EpsLoCsc = 0.052
AmdcDumpGeoModel.EpsLoCsc = 0.010
#
#if 1 use mean value of strip corners position instead of center for CSC
AmdcDumpGeoModel.CenterCscMean = 0
#
#if 1 set on Station selection
AmdcDumpGeoModel.StationSelection = 0
#AmdcDumpGeoModel.StationSelected  = [ "BIL" , "BML", "BMS", "BOL" ]
#AmdcDumpGeoModel.StationSelected  = [ "CSS", "CSL" ]
#
#if 1 initialize() return FAILURE to speed up analysis
AmdcDumpGeoModel.EmergencyOut = 0

print AmdcDumpGeoModel

######################################################
############### HERE DEFINE THE SEQUENCE #############
######################################################

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()
topSequence += AmdcDumpGeoModel

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


