#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# --- read BS 
doReadBS =  False
doCopyRDO = True

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

#  athenaCommonFlags.FilesInput = ["/afs/cern.ch/work/m/mdiamond/public/digiOUT/OUTdigi1247_2.pool.root"]
#  athenaCommonFlags.FilesInput = ["/tmp/mdiamond/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.RDO.e3099_s2578_r6221_tid05191880_00/RDO.05191880._000170.pool.root.1"]
#  athenaCommonFlags.FilesInput = ["/tmp/mdiamond/RDO.05191880._000080.pool.root.1", "/tmp/mdiamond/RDO.05191880._000232.pool.root.1", "/tmp/mdiamond/RDO.05191880._000235.pool.root.1", "/tmp/mdiamond/RDO.05191880._000243.pool.root.1" ]
#  athenaCommonFlags.FilesInput = ["/tmp/mdiamond/ns25/RDO.05191878._000042.pool.root.1", "/tmp/mdiamond/ns25/RDO.05191878._000110.pool.root.1", "/tmp/mdiamond/ns25/RDO.05191878._000121.pool.root.1", "/tmp/mdiamond/ns25/RDO.05191878._000141.pool.root.1", "/tmp/mdiamond/ns25/RDO.05191878._000022.pool.root.1", "/tmp/mdiamond/ns25/RDO.05191878._000140.pool.root.1", "/tmp/mdiamond/ns25/RDO.05191878._000148.pool.root.1", "/tmp/mdiamond/ns25/RDO.05191878._000189.pool.root.1", "/tmp/mdiamond/ns25/RDO.05191878._000156.pool.root.1", "/tmp/mdiamond/ns25/RDO.05191878._000012.pool.root.1","/tmp/mdiamond/ns25/RDO.05191878._000041.pool.root.1", "/tmp/mdiamond/ns25/RDO.05191878._000102.pool.root.1"]
#  athenaCommonFlags.FilesInput = ["/tmp/mdiamond/ns25/RDO.05191878._000042.pool.root.1"]
if hasattr(runArgs, "inputRDOFile"):
  athenaCommonFlags.FilesInput = runArgs.inputRDOFile

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.ConditionsTag='OFLCOND-RUN12-SDR-25'
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag(globalflags.ConditionsTag())

  
import AthenaPython.ConfigLib as apcl
cfg = apcl.AutoCfg(name = 'InDetRecExampleAutoConfig', input_files=athenaCommonFlags.FilesInput())
cfg.configure_job()

from AthenaCommon.GlobalFlags import jobproperties
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag(globalflags.ConditionsTag())

if len(globalflags.ConditionsTag())!=0:
  conddb.setGlobalTag(globalflags.ConditionsTag())
import MagFieldServices.SetupField


theApp.EvtMax = -1
#theApp.EvtMax = 25

#--------------------------------------------------------------
# Control
#--------------------------------------------------------------

# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = INFO
# --- produce an atlantis data file
doJiveXML       = False
# --- run the Virtual Point 1 event visualisation
doVP1           = False
# --- controls what is written out. ESD includes AOD, so it's normally enough
doWriteESD      = True
doWriteAOD      = False
# --- do auditors ?
doAuditors      = True

import os
if os.environ['CMTCONFIG'].endswith('-dbg'):
  # --- do EDM monitor (debug mode only)
  doEdmMonitor    = True 
  # --- write out a short message upon entering or leaving each algorithm
  doNameAuditor   = True
else:
  doEdmMonitor    = False
  doNameAuditor   = False

#--------------------------------------------------------------
# Additional Detector Setup
#--------------------------------------------------------------

from RecExConfig.RecFlags import rec
rec.Commissioning=False

from AthenaCommon.DetFlags import DetFlags 
# --- switch on InnerDetector
DetFlags.ID_setOn()
DetFlags.makeRIO.SCT_setOff()
DetFlags.makeRIO.TRT_setOff()

# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

# ---- switch parts of ID off/on as follows (always use both lines)
DetFlags.SCT_setOff()
DetFlags.detdescr.SCT_setOn()
DetFlags.TRT_setOff()
DetFlags.detdescr.TRT_setOn()

# --- switch off DCS
DetFlags.dcs.SCT_setOff()
DetFlags.dcs.TRT_setOff()

# --- printout
#DetFlags.Print()

# --- output level
#OutputLevel          = DEBUG

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doDBMstandalone = True
InDetFlags.doTruth = True

# --- uncomment to change the default of one of the following options:
InDetFlags.doLowPt                = False
InDetFlags.doTRTPhaseCalculation = False

InDetFlags.cutLevel       = 1 # test for TRTonly in PU

# --- Turn off track slimming
InDetFlags.doSlimming = False

InDetFlags.doIBL = True
InDetFlags.doTrackSegmentsPixelPrdAssociation = False 
InDetFlags.doTrackSegmentsPixelFourLayer = False 
InDetFlags.doTrackSegmentsPixelThreeLayer = False 

# --- activate monitorings
InDetFlags.doMonitoringGlobal    = False
InDetFlags.doMonitoringPrimaryVertexingEnhanced = False
InDetFlags.doMonitoringPixel     = False
InDetFlags.doMonitoringSCT       = False
InDetFlags.doMonitoringTRT       = False
InDetFlags.doMonitoringAlignment = False

# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon        = True
# --- activate creation of standard plots
InDetFlags.doStandardPlots  = True
# --- activate physics validation monitoring
InDetFlags.doPhysValMon  = True
# --- active storegate delection
#InDetFlags.doSGDeletion  = True

# --- produce various ntuples (all in one root file)
InDetFlags.doTrkNtuple      = False
InDetFlags.doPixelTrkNtuple = False
InDetFlags.doPixelClusterNtuple = False

#InDetFlags.doMinBias   = True

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = True

#DBM changes
InDetFlags.doPixelClusterSplitting.set_Value_and_Lock(False)
InDetFlags.doNewTracking.set_Value_and_Lock(True)
InDetFlags.useBroadPixClusterErrors.set_Value_and_Lock(True)
InDetFlags.selectSCTIntimeHits = False
InDetFlags.doForwardTracks = False
InDetFlags.doVertexFinding = True
InDetFlags.doSCT_PRDFormation = False
InDetFlags.doTRT_PRDFormation = False
InDetFlags.doHolesOnTrack = False
InDetFlags.doTIDE_Ambi = False
InDetFlags.useBeamConstraint = False
InDetFlags.doCaloSeededBrem = False
InDetFlags.doVertexFindingForMonitoring = False
InDetFlags.doSplitVertexFindingForMonitoring = False
InDetFlags.doParticleCreation = True

#InDetFlags.doFatras = True

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include("InDetRecExample/InDetRec_all.py")

# --- dump MC truth into logfile
#from TruthExamples.TruthExamplesConf import DumpMC
#topSequence += DumpMC(McEventKey = "TruthEvent")
