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
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
theApp.AuditAlgorithms=True


#--------------------------------------------------------------
# PerfMon Setup (crash in finalize of AthenaMT)
#--------------------------------------------------------------
if jobproperties.ConcurrencyFlags.NumThreads() == 0:
    from PerfMonComps.PerfMonFlags import jobproperties
    jobproperties.PerfMonFlags.doMonitoring = True
    jobproperties.PerfMonFlags.OutputFile = "perfmon.root"

#--------------------------------------------------------------
# Load Geometry
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion="ATLAS-R2-2015-03-01-00"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="data"
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

# Set up SCT cabling
from AthenaCommon.Include import include
include('InDetRecExample/InDetRecCabling.py')

#--------------------------------------------------------------
# Load ReadCalibData Alg and Service
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
#include("IOVDbSvc/IOVDbSvc_jobOptions.py")
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
IOVDbSvc.GlobalTag='CONDBR2-BLKPA-2017-06'
IOVDbSvc.OutputLevel = DEBUG

from SCT_ConditionsTools.SCT_ReadCalibDataToolSetup import SCT_ReadCalibDataToolSetup
sct_ReadCalibDataToolSetup = SCT_ReadCalibDataToolSetup()
sct_ReadCalibDataToolSetup.setup()

SCT_ReadCalibDataCondAlg = sct_ReadCalibDataToolSetup.getAlg()
SCT_ReadCalibDataTool = sct_ReadCalibDataToolSetup.getTool()

from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_ReadCalibDataTestAlg
topSequence+= SCT_ReadCalibDataTestAlg(SCT_ReadCalibDataTool=SCT_ReadCalibDataTool)

# <-999 setting ignores the defect, otherwise it will be checked against the set value
SCT_ReadCalibDataCondAlg.IgnoreDefects = ["NOISE_SLOPE","OFFSET_SLOPE","GAIN_SLOPE","BAD_OPE"]
SCT_ReadCalibDataCondAlg.IgnoreDefectsParameters = [-1000,-1000,-1000,-1000]
#SCT_ReadCalibDataTool.IgnoreDefects = ["BADFIT","NOISE_SLOPE","OFFSET_SLOPE","GAIN_SLOPE","BAD_OPE"]
#SCT_ReadCalibDataTool.IgnoreDefectsParameters = [-1000,-1000,-1000,-1000,-1000]
#SCT_ReadCalibDataTool.IgnoreDefects = ["NOISE_SLOPE","OFFSET_SLOPE","GAIN_SLOPE"]
#SCT_ReadCalibDataTool.IgnoreDefectsParameters = [-1000,-1000,-1000]
#SCT_ReadCalibDataTool.IgnoreDefects = ["DEAD","STUCKON","UNDER","OVER","BADFIT","VLO_GAIN","LO_GAIN","HI_GAIN","LO_OFFSET","HI_OFFSET","UNBONDED","PARTBONDED","NOISY","V_NOISY","NOISE_SLOPE","OFFSET_SLOPE","GAIN_SLOPE","BAD_OPE","NO_HI"]
#SCT_ReadCalibDataTool.IgnoreDefectsParameters = [-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1,0.015]
#SCT_ReadCalibDataTool.IgnoreDefects = ["NO_IGNORE"]
#SCT_ReadCalibDataTool.IgnoreDefectsParameters = [-1000]

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
theApp.EvtMax                    = 20

#For real data, earliest timestamp is 0
#ServiceMgr.EventSelector.InitialTimeStamp = 1228950000
ServiceMgr.EventSelector.InitialTimeStamp = 1476741326 # LB 18 of run 310809, 10/17/2016 @ 9:55pm (UTC)
ServiceMgr.EventSelector.RunNumber = 310809

#--------------------------------------------------------------
# Set output lvl (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
SCT_ReadCalibDataTool.OutputLevel = INFO
topSequence.SCT_ReadCalibDataTestAlg.OutputLevel = INFO

conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/MUR", "/SCT/DAQ/Config/MUR")
conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/Geog", "/SCT/DAQ/Config/Geog")
conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/RODMUR", "/SCT/DAQ/Config/RODMUR")
conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/ROD", "/SCT/DAQ/Config/ROD")

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
    SCT_ReadCalibDataTool.PrintCalibDefectMaps = True

