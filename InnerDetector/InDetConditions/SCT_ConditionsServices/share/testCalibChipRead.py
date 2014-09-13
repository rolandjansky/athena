################################################################################
# Job options file to test the ReadCalibDataSvc
################################################################################

#--------------------------------------------------------------
# Set some basic options
#--------------------------------------------------------------
DoTestmyConditionsSummary             = True  # Test return bool conditionsSummary?
DoTestmyDataSummary                   = True  # Test return data summary?
PrintOutCalibDataMaps                 = False # Print out the calib defect maps

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
# Load Geometry
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion="ATLAS-GEO-16-00-00"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="geant4"
print globalflags

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


#--------------------------------------------------------------
# Load ReadCalibData Alg and Service
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ReadCalibChipDataTestAlg
topSequence+= SCT_ReadCalibChipDataTestAlg()

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ReadCalibChipDataSvc
ServiceMgr += SCT_ReadCalibChipDataSvc()

SCT_ReadCalibChipDataSvc=ServiceMgr.SCT_ReadCalibChipDataSvc


##Modules to test:
##136523776, strips 0-255 BAD_OPE=good
#SCT_ReadCalibDataTestAlg.ModuleOfflinePosition = [-2, 1, 0, 6, 0, 111] #(EC/B, Disk/Layer, eta, phi, side, strip)
##134592512, strips 640-767 slope=good, strip 767-61 dead=bad
SCT_ReadCalibChipDataTestAlg.ModuleOfflinePosition = [-2, 0, 1, 11, 0, 706] #(EC/B, Disk/Layer, eta, phi, side, strip)
##Module not at all in DB?! cooling loop disc 9 EC:
#SCT_ReadCalibChipDataTestAlg.ModuleOfflinePosition = [-2, 8, 0, 26, 1, 384] #(EC/B, Disk/Layer, eta, phi, side, strip)


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
ServiceMgr.EventSelector.InitialTimeStamp = 1228950000


#--------------------------------------------------------------
# Set output lvl (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.SCT_ReadCalibChipDataSvc.OutputLevel = INFO
topSequence.SCT_ReadCalibChipDataTestAlg.OutputLevel = INFO


#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
#include("IOVDbSvc/IOVDbSvc_jobOptions.py")
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
IOVDbSvc.GlobalTag='COMCOND-ES1C-000-00'
IOVDbSvc.OutputLevel = DEBUG
#ToolSvc = Service( "ToolSvc" )

#For testing against the DEVDB10
#DBname='<dbConnection>oracle://DEVDB10;schema=ATLAS_SCT_COMMCOND_DEV;dbname=ACALTEST;user=ATLAS_SCT_COMMCOND_DEV;password=********</dbConnection>'
#IOVDbSvc.Folders += [ DBname + '/SCT/DAQ/Calibratsion/NPtGainDefects' ]
#IOVDbSvc.Folders += [ DBname + '/SCT/DAQ/Calibration/NoiseOccupancyDefects' ]

#For real Pit one data in comp200
#conddb.addFolder("SCT",CoolFldrPathNoiseOcc)
#conddb.addFolder("SCT",CoolFldrPathNPtGain)
#Now multi-version in comp200:
conddb.addFolder("SCT","/SCT/DAQ/Calibration/ChipNoise <tag>HEAD</tag>")
conddb.addFolder("SCT","/SCT/DAQ/Calibration/ChipGain <tag>HEAD</tag>")

#--------------------------------------------------------------
# Set the correct flags
#--------------------------------------------------------------
# Test return ConditionsSummary?
if DoTestmyConditionsSummary:
 SCT_ReadCalibChipDataTestAlg.DoTestmyConditionsSummary = True
else:
 SCT_ReadCalibChipDataTestAlg.DoTestmyConditionsSummary = False

# Test return DataSummary?
if DoTestmyDataSummary:
 SCT_ReadCalibChipDataTestAlg.DoTestmyDataSummary = True
else:
 SCT_ReadCalibChipDataTestAlg.DoTestmyDataSummary = False

#Print out defects maps
if PrintOutCalibDataMaps:
 ServiceMgr.SCT_ReadCalibChipDataSvc.PrintCalibDataMaps = True

