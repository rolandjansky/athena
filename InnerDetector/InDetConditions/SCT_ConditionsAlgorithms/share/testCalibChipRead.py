################################################################################
# Job options file to test the ReadCalibDataSvc
################################################################################

#--------------------------------------------------------------
# Thread-specific setup
#--------------------------------------------------------------
from AthenaCommon.ConcurrencyFlags import jobproperties
if jobproperties.ConcurrencyFlags.NumThreads() > 0:
    from AthenaCommon.AlgScheduler import AlgScheduler
    AlgScheduler.CheckDependencies( True )
    AlgScheduler.ShowControlFlow( True )
    AlgScheduler.ShowDataDependencies( True )

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
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
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
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.SCT_setOn()

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

if not hasattr(topSequence, "xAODMaker::EventInfoCnvAlg"):
    from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
    topSequence +=xAODMaker__EventInfoCnvAlg(OutputLevel=2)

from SCT_ConditionsTools.SCT_ReadCalibChipDataToolSetup import SCT_ReadCalibChipDataToolSetup
sct_ReadCalibChipDataToolSetup = SCT_ReadCalibChipDataToolSetup()
sct_ReadCalibChipDataToolSetup.setNoiseFolderTag("SctDaqCalibrationChipNoise-UPD1-002-00")
sct_ReadCalibChipDataToolSetup.setGainFolderTag("SctDaqCalibrationChipGain-UPD1-002-00")
sct_ReadCalibChipDataToolSetup.setup()

SCT_ReadCalibChipDataTool=sct_ReadCalibChipDataToolSetup.getTool()

from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_ReadCalibChipDataTestAlg
topSequence+= SCT_ReadCalibChipDataTestAlg(SCT_ReadCalibChipDataTool=SCT_ReadCalibChipDataTool)

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
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.EventSelector.RunNumber = 215643
theApp.EvtMax                    = 20

#--------------------------------------------------------------
# Set output lvl (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = INFO
SCT_ReadCalibChipDataTool.OutputLevel = INFO
SCT_ReadCalibChipDataTestAlg.OutputLevel = INFO

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
    SCT_ReadCalibChipDataSvc.PrintCalibDataMaps = True

