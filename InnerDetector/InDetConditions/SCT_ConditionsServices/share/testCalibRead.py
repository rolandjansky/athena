################################################################################
# Job options file to test the ReadCalibDataSvc
################################################################################

#--------------------------------------------------------------
# Set some basic options
#--------------------------------------------------------------
DoTestmyConditionsSummary             = True  # Test return bool conditionsSummary?
DoTestmyDefectIsGood                  = True  # Test return defect type summary?
DoTestmyDefectType                    = False  # Test return defect type summary?
DoTestmyDefectsSummary                = False  # Test return module defects summary?
DoTestmyDefectList                    = False  # Test return module defect list?
PrintOutCalibDefectMaps               = False # Print out the calib defect maps

#--------------------------------------------------------------
# use auditors
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ServiceMgr

from GaudiSvc.GaudiSvcConf import AuditorSvc
from AthenaCommon.AppMgr import theApp

ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
#ChronoStatSvc = Service ( "ChronoStatSvc")
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
#MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )
theApp.AuditAlgorithms=True


#--------------------------------------------------------------
# PerfMon Setup
#--------------------------------------------------------------
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
jobproperties.PerfMonFlags.OutputFile = "perfmon.root"

#--------------------------------------------------------------
# Load Geometry
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion="ATLAS-R1-2012-03-00-00"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="data"
print globalflags

eventInfoKey = "ByteStreamEventInfo"
if globalflags.DataSource()=="geant4":
    eventInfoKey = "McEventInfo"

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------
# --- switch on InnerDetector
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.SCT_setOn()
DetFlags.pixel_setOff()
DetFlags.TRT_setOff()

# ---- switch parts of ID off/on as follows
#switch off tasks
DetFlags.pileup.all_setOff()
DetFlags.simulate.all_setOff()
DetFlags.makeRIO.all_setOff()
DetFlags.writeBS.all_setOff()
DetFlags.readRDOBS.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.writeRIOPool.all_setOff()


import AtlasGeoModel.SetGeometryVersion
import AtlasGeoModel.GeoModelInit

# Disable SiLorentzAngleSvc to remove
# ERROR ServiceLocatorHelper::createService: wrong interface id IID_665279653 for service
ServiceMgr.GeoModelSvc.DetectorTools['SCT_DetectorTool'].LorentzAngleSvc=""

#--------------------------------------------------------------
# Load ReadCalibData Alg and Service
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ReadCalibDataTestAlg
topSequence+= SCT_ReadCalibDataTestAlg()

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ReadCalibDataSvc
ServiceMgr += SCT_ReadCalibDataSvc(EventInfoKey=eventInfoKey)

SCT_ReadCalibDataSvc=ServiceMgr.SCT_ReadCalibDataSvc
#SCT_ReadCalibDataSvc.RecoOnly = False
# <-999 setting ignores the defect, otherwise it will be checked against the set value
SCT_ReadCalibDataSvc.IgnoreDefects = ["NOISE_SLOPE","OFFSET_SLOPE","GAIN_SLOPE","BAD_OPE"]
SCT_ReadCalibDataSvc.IgnoreDefectsParameters = [-1000,-1000,-1000,-1000]
#SCT_ReadCalibDataSvc.IgnoreDefects = ["BADFIT","NOISE_SLOPE","OFFSET_SLOPE","GAIN_SLOPE","BAD_OPE"]
#SCT_ReadCalibDataSvc.IgnoreDefectsParameters = [-1000,-1000,-1000,-1000,-1000]
#SCT_ReadCalibDataSvc.IgnoreDefects = ["NOISE_SLOPE","OFFSET_SLOPE","GAIN_SLOPE"]
#SCT_ReadCalibDataSvc.IgnoreDefectsParameters = [-1000,-1000,-1000]
#SCT_ReadCalibDataSvc.IgnoreDefects = ["DEAD","STUCKON","UNDER","OVER","BADFIT","VLO_GAIN","LO_GAIN","HI_GAIN","LO_OFFSET","HI_OFFSET","UNBONDED","PARTBONDED","NOISY","V_NOISY","NOISE_SLOPE","OFFSET_SLOPE","GAIN_SLOPE","BAD_OPE","NO_HI"]
#SCT_ReadCalibDataSvc.IgnoreDefectsParameters = [-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1,0.015]
#SCT_ReadCalibDataSvc.IgnoreDefects = ["NO_IGNORE"]
#SCT_ReadCalibDataSvc.IgnoreDefectsParameters = [-1000]

##Modules to test:
##136523776, strips 0-255 BAD_OPE=good
#SCT_ReadCalibDataTestAlg.ModuleOfflinePosition = [-2, 1, 0, 6, 0, 111] #(EC/B, Disk/Layer, eta, phi, side, strip)
##134592512, strips 640-767 slope=good, strip 767-61 dead=bad
##SCT_ReadCalibDataTestAlg.ModuleOfflinePosition = [-2, 0, 1, 11, 0, 706] #(EC/B, Disk/Layer, eta, phi, side, strip)
SCT_ReadCalibDataTestAlg.ModuleOfflinePosition = [2, 1, 2, 12, 1, 464] #(EC/B, Disk/Layer, eta, phi, side, strip)
##Module not at all in DB?! cooling loop disc 9 EC:
#SCT_ReadCalibDataTestAlg.ModuleOfflinePosition = [-2, 8, 0, 26, 1, 384] #(EC/B, Disk/Layer, eta, phi, side, strip)


#--------------------------------------------------------------
# Event selector settings. Use McEventSelector
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob
#ServiceMgr+= EventSelector()
#ServiceMgr.EventSelector.FirstEvent = 1
#ServiceMgr.EventSelector.EventsPerRun = 5
#ServiceMgr.EventSelector.RunNumber = 0
theApp.EvtMax                    = 1

#For real data, earliest timestamp is 0
#ServiceMgr.EventSelector.InitialTimeStamp = 1228950000
ServiceMgr.EventSelector.InitialTimeStamp = 1530617600
ServiceMgr.EventSelector.RunNumber = 198232

#--------------------------------------------------------------
# Set output lvl (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.SCT_ReadCalibDataSvc.OutputLevel = INFO
topSequence.SCT_ReadCalibDataTestAlg.OutputLevel = INFO


#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
#include("IOVDbSvc/IOVDbSvc_jobOptions.py")
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
IOVDbSvc.GlobalTag='COMCOND-BLKPA-RUN1-09'
IOVDbSvc.OutputLevel = DEBUG
#ToolSvc = Service( "ToolSvc" )

# Not clear why these tags are not resolved from global tag
conddb.blockFolder("/Indet/Align")
conddb.addFolderWithTag("INDET_OFL","/Indet/Align","InDetAlign-BLK-UPD4-09")

#For testing against the DEVDB10
#DBname='<dbConnection>oracle://DEVDB10;schema=ATLAS_SCT_COMMCOND_DEV;dbname=ACALTEST;user=ATLAS_SCT_COMMCOND_DEV;password=********</dbConnection>'
#IOVDbSvc.Folders += [ DBname + '/SCT/DAQ/Calibration/NPtGainDefects' ]
#IOVDbSvc.Folders += [ DBname + '/SCT/DAQ/Calibration/NoiseOccupancyDefects' ]

#For real Pit one data in comp200
#conddb.addFolder("SCT",CoolFldrPathNoiseOcc)
#conddb.addFolder("SCT",CoolFldrPathNPtGain)
#Now multi-version in comp200:

conddb.addFolder("SCT","/SCT/DAQ/Calibration/NoiseOccupancyDefects <tag>HEAD</tag>")
conddb.addFolder("SCT","/SCT/DAQ/Calibration/NPtGainDefects <tag>HEAD</tag>")
conddb.addFolder("SCT","/SCT/DAQ/Configuration/ROD <tag>HEAD</tag>")
conddb.addFolder("SCT","/SCT/DAQ/Configuration/Geog <tag>HEAD</tag>")
conddb.addFolder("SCT","/SCT/DAQ/Configuration/RODMUR <tag>HEAD</tag>")
conddb.addFolder("SCT","/SCT/DAQ/Configuration/MUR <tag>HEAD</tag>")


#--------------------------------------------------------------
# Set the correct flags
#--------------------------------------------------------------
# Test return ConditionsSummary?
if DoTestmyConditionsSummary:
 SCT_ReadCalibDataTestAlg.DoTestmyConditionsSummary = True
else:
 SCT_ReadCalibDataTestAlg.DoTestmyConditionsSummary = False

# Test my isGood method
if DoTestmyDefectIsGood:
 SCT_ReadCalibDataTestAlg.DoTestmyDefectIsGood = True
else:
 SCT_ReadCalibDataTestAlg.DoTestmyDefectIsGood = False

# Test return DefectType?
if DoTestmyDefectType:
 SCT_ReadCalibDataTestAlg.DoTestmyDefectType = True
else:
 SCT_ReadCalibDataTestAlg.DoTestmyDefectType = False

# Test return DefectsSummary?
if DoTestmyDefectsSummary:
 SCT_ReadCalibDataTestAlg.DoTestmyDefectsSummary = True
else:
 SCT_ReadCalibDataTestAlg.DoTestmyDefectsSummary = False

# Test return DefectsList?
if DoTestmyDefectList:
 SCT_ReadCalibDataTestAlg.DoTestmyDefectList = True
else:
 SCT_ReadCalibDataTestAlg.DoTestmyDefectList = False

#Print out defects maps
if PrintOutCalibDefectMaps:
 ServiceMgr.SCT_ReadCalibDataSvc.PrintCalibDefectMaps = True

