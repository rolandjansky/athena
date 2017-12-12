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
globalflags.DetDescrVersion="ATLAS-R1-2012-03-00-00"
globalflags.ConditionsTag="COMCOND-BLKPA-RUN1-09"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="data"
print globalflags

#--------------------------------------------------------------
# Set up conditions
#--------------------------------------------------------------
from RecExConfig.RecFlags import rec
rec.projectName.set_Value_and_Lock("data12_8TeV")

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
#include("IOVDbSvc/IOVDbSvc_jobOptions.py")
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
conddb.dbdata="COMP200"
IOVDbSvc.GlobalTag=globalflags.ConditionsTag()
IOVDbSvc.OutputLevel = DEBUG

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

from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
topSequence +=xAODMaker__EventInfoCnvAlg(OutputLevel=2)

from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")

sctGainFolder = "/SCT/DAQ/Calibration/ChipGain"
sctGainCondAlg = "SCT_ReadCalibChipGainCondAlg"
if not conddb.folderRequested(sctGainFolder):
    conddb.addFolderWithTag("SCT","/SCT/DAQ/Calibration/ChipGain","SctDaqCalibrationChipGain-UPD1-002-00", className="CondAttrListCollection")
if not hasattr(condSeq, sctGainCondAlg):
    from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ReadCalibChipGainCondAlg
    condSeq += SCT_ReadCalibChipGainCondAlg(name=sctGainCondAlg, ReadKey=sctGainFolder)

sctNoiseFolder = "/SCT/DAQ/Calibration/ChipNoise"
sctNoiseCondAlg = "SCT_ReadCalibChipNoiseCondAlg"
if not conddb.folderRequested(sctNoiseFolder):
    conddb.addFolderWithTag("SCT","/SCT/DAQ/Calibration/ChipNoise","SctDaqCalibrationChipNoise-UPD1-002-00", className="CondAttrListCollection")
if not hasattr(condSeq, sctNoiseCondAlg):
    from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ReadCalibChipNoiseCondAlg
    condSeq += SCT_ReadCalibChipNoiseCondAlg(name=sctNoiseCondAlg, ReadKey=sctNoiseFolder)

from AthenaCommon.AppMgr import ServiceMgr

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
ServiceMgr.EventSelector.RunNumber = 215643
theApp.EvtMax                    = 1

#--------------------------------------------------------------
# Set output lvl (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.SCT_ReadCalibChipDataSvc.OutputLevel = INFO
topSequence.SCT_ReadCalibChipDataTestAlg.OutputLevel = INFO

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

