## copy from MuonGeoModelTest/share/runAmdcMGM.py

if not 'useAlign' in dir():
    useAlign = False
if not 'useAlignData' in dir():
    useAlignData = True

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
import AthenaCommon.AtlasUnixGeneratorJob

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
    printfunc ("Setting up DB name / Instance to COMP200 (default is OFLP200)")
    conddb.dbname = "CONDBR2"
    conddb.iovdbsvc.DBInstance="CONDBR2"

import IOVDbSvc.IOVDb
ServiceMgr.IOVDbSvc.GlobalTag='CONDBR2-BLKPA-RUN2-02'
#conddb.setGlobalTag("MuonAlignMDTBarrelAlign-RUN2-BA_0800-00")


# Select geometry version
if not 'DetDescrVersion' in dir():
    DetDescrVersion="ATLAS-R2-2015-03-01-00"
    printfunc ('MuonGeoModelTest/runAmdcMGM DetDescrVersion now set to ', DetDescrVersion)
else:
    printfunc ('MuonGeoModelTest/runAmdcMGM DetDescrVersion is already ', DetDescrVersion)
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = DetDescrVersion

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
######################################################################## end setup GeoModel

theApp.EvtSel = "EventSelector"
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

printfunc (GeoModelSvc)

#***************************************************** HERE define alignment data from cond. tags
if useAlign:
    from McEventSelector import McEventSelectorConf
    ServiceMgr+=McEventSelectorConf.McEventSelector('EventSelector',
                                                    RunNumber = 310809,
                                                    InitialTimeStamp = 1476741326)
    from IOVDbSvc.CondDB import conddb
    conddb.dbname = "CONDBR2"
    conddb.addFolderWithTag('MUONALIGN_OFL','/MUONALIGN/MDT/BARREL',"MuonAlignMDTBarrelAlign-RUN2-BA_0800-00",className='CondAttrListCollection')
    conddb.addFolderWithTag('MUONALIGN_OFL','/MUONALIGN/MDT/ENDCAP/SIDEA',"MuonAlignMDTEndCapAAlign-RUN2-ECA_ROLLING_2015_03_01-UPD4-02",className='CondAttrListCollection')
    conddb.addFolderWithTag('MUONALIGN_OFL','/MUONALIGN/MDT/ENDCAP/SIDEC',"MuonAlignMDTEndCapCAlign-RUN2-ECC_ROLLING_2015_03_01-UPD4-02",className='CondAttrListCollection')
    conddb.addFolderWithTag('MUONALIGN_OFL','/MUONALIGN/CSC/ILINES',"MuonAlignCscIlines-2020-00",className='CondAttrListCollection')
    conddb.addFolderWithTag('MUONALIGN_OFL','/MUONALIGN/TGC/SIDEA',"MuonAlignTGCEndCapAAlign-RUN2-TGCA_ROLLING_2011_01-ES1-UPD1-03",className='CondAttrListCollection')
    conddb.addFolderWithTag('MUONALIGN_OFL','/MUONALIGN/TGC/SIDEC',"MuonAlignTGCEndCapCAlign-RUN2-TGCC_ROLLING_2011_01-ES1-UPD1-03",className='CondAttrListCollection')
    #conddb.addFolderWithTag('','<dbConnection>oracle://INT8R;schema=ATLAS_COOLOFL_MUONALIGN;dbname=CONDBR2;user=ATLAS_COOLOFL_MUONALIGN_W;password=do5cM5Gmcfl0BbdKo3Myz_jAd04z</dbConnection>/MUONALIGN/MDT/BARREL',"MuonAlignMDTBarrelAlign-RUN2-BA_0800-00",className='CondAttrListCollection')
    #conddb.addFolderWithTag('','<dbConnection>oracle://INT8R;schema=ATLAS_COOLOFL_MUONALIGN;dbname=CONDBR2;user=ATLAS_COOLOFL_MUONALIGN_W;password=do5cM5Gmcfl0BbdKo3Myz_jAd04z</dbConnection>/MUONALIGN/MDT/ENDCAP/SIDEA',"MuonAlignMDTEndCapAAlign-RUN2-ECA_ROLLING_2015_03_01-UPD4-02",className='CondAttrListCollection')
    #conddb.addFolderWithTag('','<dbConnection>oracle://INT8R;schema=ATLAS_COOLOFL_MUONALIGN;dbname=CONDBR2;user=ATLAS_COOLOFL_MUONALIGN_W;password=do5cM5Gmcfl0BbdKo3Myz_jAd04z</dbConnection>/MUONALIGN/MDT/ENDCAP/SIDEC',"MuonAlignMDTEndCapCAlign-RUN2-ECC_ROLLING_2015_03_01-UPD4-02",className='CondAttrListCollection')
    #conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/BARREL','/MUONALIGN/MDT/BARREL',className='CondAttrListCollection')
    #conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEA','/MUONALIGN/MDT/ENDCAP/SIDEA',className='CondAttrListCollection')
    #conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/MDT/ENDCAP/SIDEC','/MUONALIGN/MDT/ENDCAP/SIDEC',className='CondAttrListCollection')
    #conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEA','/MUONALIGN/TGC/SIDEA',className='CondAttrListCollection')
    #conddb.addFolderSplitOnline('MUONALIGN','/MUONALIGN/Onl/TGC/SIDEC','/MUONALIGN/TGC/SIDEC',className='CondAttrListCollection')

    from AthenaCommon.AlgSequence import AthSequencer
    condSequence = AthSequencer("AthCondSeq")
    from MuonCondAlg.MuonCondAlgConf import MuonAlignmentCondAlg
    condSequence+=MuonAlignmentCondAlg('MuonAlignmentCondAlg')
    MuonAlignmentCondAlg.ParlineFolders = ["/MUONALIGN/MDT/BARREL",
                                           "/MUONALIGN/MDT/ENDCAP/SIDEA",
                                           "/MUONALIGN/MDT/ENDCAP/SIDEC",
                                           "/MUONALIGN/CSC/ILINES",
                                           "/MUONALIGN/TGC/SIDEA",
                                           "/MUONALIGN/TGC/SIDEC"
                                           ]
    MuonAlignmentCondAlg.OutputLevel = DEBUG

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

printfunc (MuonDetectorTool)
MessageSvc.setDebug+=["MGM::MuonDetectorManager::updateAlignment"]


#***************************************************** HERE setup Amdc 
include ("AmdcAth/AmdcAth_jobOptions.py")
from AmdcAth.AmdcAthConf import AmdcsimrecAthenaSvc
AmdcsimrecAthenaSvc = AmdcsimrecAthenaSvc()

AmdcsimrecAthenaSvc.ControlALine =  GeneralControlAlines
AmdcsimrecAthenaSvc.AlignmentSource=2 # COOL 
#AmdcsimrecAthenaSvc.ControlALine =  0
#Truncate A line rotation parameters
#1/0 truncate/ do not truncate
AmdcsimrecAthenaSvc.TruncateALine = 0;
#Truncate P line rotation parameters
#1/0 truncate/ do not truncate
AmdcsimrecAthenaSvc.TruncatePLine = 0 ;
AmdcsimrecAthenaSvc.OutputLevel=DEBUG

printfunc (AmdcsimrecAthenaSvc)

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

printfunc (AmdcDumpGeoModel)

######################################################
############### HERE DEFINE THE SEQUENCE #############
######################################################

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()
topSequence += AmdcDumpGeoModel

printfunc (" ")
printfunc ("List of all Dlls")
printfunc (theApp.Dlls)
printfunc (" ")
printfunc ("List of all Ext services")
printfunc (theApp.ExtSvc)
printfunc (" ")
printfunc ("List of all top algorithms")
printfunc (theApp.TopAlg)

#printfunc ("Print here Top Sequence" )
#printfunc (topSequence)
#printfunc ("Print here Service Manager" )
#printfunc (ServiceMgr)


