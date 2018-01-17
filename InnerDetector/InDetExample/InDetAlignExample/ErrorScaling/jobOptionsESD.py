# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

include("datasets.py")

# --- read BS 
doReadBS        = True

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

#if not doReadBS:
#  athenaCommonFlags.FilesInput = ["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/perf-idtracking/InDetRecExample/mc09_7TeV.105200.T1_McAtNlo_Jimmy.digit.RDO.e510_s624_s633_d287_tid112426_00/RDO.112426._000007.pool.root.1"] 
#else:
#  athenaCommonFlags.FilesInput = ["rfio:/castor/cern.ch/grid/atlas/DAQ/2011/00180481/calibration_IDTracks/data11_7TeV.00180481.calibration_IDTracks.daq.RAW._lb0000._SFO-10._0002.data"]

#athenaCommonFlags.FilesInput = [ 'root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/mc10_7TeV/DESD_MBIAS/e574_s932_s946_r2094/mc10_7TeV.105001.pythia_minbias.recon.DESD_MBIAS.e574_s932_s946_r2094_tid280668_00/DESD_MBIAS.280668._016228.pool.root.1']

#if doReadBS:
import os
os.environ['STAGE_HOST']="castoratlas"
os.environ['STAGE_SVCCLASS']="atlcal"


#os.environ['STAGE_HOST']="castoratlast3"
#os.environ['STAGE_SVCCLASS']="atlascerngroupdisk"      
#//athenaCommonFlags.FilesInput = ["rfio:/castor/cern.ch/atlas/atlascerngroupdisk/perf-idtracking/InDetRecExample/data10_7TeV.00154817.physics_MinBias.merge.RAW/data10_7TeV.00154817.physics_MinBias.merge.RAW._lb0100._0001.1"]
from AthenaCommon.GlobalFlags import globalflags
#globalflags.ConditionsTag.set_Value_and_Lock("COMCOND-BLKPST-004-05")
#globalflags.ConditionsTag.set_Value_and_Lock("COMCOND-BLKPST-005-06")
#globalflags.ConditionsTag.set_Value_and_Lock("COMCOND-BLKPST-005-07")

print athenaCommonFlags.FilesInput()

import AthenaPython.ConfigLib as apcl
cfg = apcl.AutoCfg(name = 'InDetRecExampleAutoConfig', input_files=athenaCommonFlags.FilesInput())
cfg.configure_job()

if doReadBS:
  from AthenaCommon.GlobalFlags import globalflags

  if len(globalflags.ConditionsTag())!=0:
    from IOVDbSvc.CondDB import conddb
    conddb.setGlobalTag(globalflags.ConditionsTag())

#theApp.EvtMax = 10
  
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
doWriteESD      = False
doWriteAOD      = False
# --- do auditors ?
doAuditors      = False

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
#DetFlags.makeRIO.pixel_setOff()
#DetFlags.makeRIO.SCT_setOff()
#DetFlags.makeRIO.TRT_setOff()

# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

# ---- switch parts of ID off/on as follows (always use both lines)
#DetFlags.pixel_setOff()
#DetFlags.detdescr.pixel_setOn()
#DetFlags.SCT_setOff()
#DetFlags.detdescr.SCT_setOn()
#DetFlags.TRT_setOff()
#DetFlags.detdescr.TRT_setOn()

# --- switch off DCS
#DetFlags.dcs.pixel_setOff()
#DetFlags.dcs.SCT_setOff()
#DetFlags.dcs.TRT_setOff()

# --- printout
DetFlags.Print()

# --- output level
#OutputLevel          = DEBUG

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth       = (globalflags.DataSource == 'geant4' and globalflags.InputFormat() == 'pool')

# --- uncomment to change the default of one of the following options:
#InDetFlags.doNewTracking          = False
#InDetFlags.doLowPt                = True
#InDetFlags.doBeamGas              = True
#InDetFlags.doBeamHalo             = True
InDetFlags.doxKalman              = False
InDetFlags.doiPatRec              = False
#InDetFlags.doBackTracking         = False
#InDetFlags.doSingleSpBackTracking = True
#InDetFlags.doTRTStandalone        = False
#InDetFlags.doLowBetaFinder        = False

# --- Turn off track slimming
#InDetFlags.doSlimming = False

#InDetFlags.cutLevel = 3

# --- possibility to run tracking on subdetectors separately (and independent from each other)
#InDetFlags.doTrackSegmentsPixel = True
#InDetFlags.doTrackSegmentsSCT   = True
#InDetFlags.doTrackSegmentsTRT   = True

# --- possibility to change the trackfitter
#InDetFlags.trackFitterType = 'KalmanFitter'

# --- activate monitorings
InDetFlags.doMonitoringGlobal    = False
InDetFlags.doMonitoringPrimaryVertexingEnhanced = False
InDetFlags.doMonitoringPixel     = False
InDetFlags.doMonitoringSCT       = False
InDetFlags.doMonitoringTRT       = False
InDetFlags.doMonitoringAlignment = False

InDetFlags.doMonitoringGlobal    = True
InDetFlags.doMonitoringPixel     = True
InDetFlags.doMonitoringSCT       = False
InDetFlags.doMonitoringTRT       = False
InDetFlags.doMonitoringAlignment = True

InDetFlags.doPixelClusterSplitting = False

# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon        = True
# --- activate creation of standard plots
#InDetFlags.doStandardPlots  = True
# --- active storegate delection
#InDetFlags.doSGDeletion  = True

# --- produce various ntuples (all in one root file)
#InDetFlags.doTrkNtuple      = True
#InDetFlags.doPixelTrkNtuple = True
#InDetFlags.doSctTrkNtuple   = True
#InDetFlags.doTrtTrkNtuple   = True
#InDetFlags.doSctClusterNtuple   = True
#InDetFlags.doVtxNtuple      = True
#InDetFlags.doConvVtxNtuple  = True
#InDetFlags.doV0VtxNtuple    = True

#InDetFlags.doMinBias   = True

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = True

if True:
  redoPatternRecoAndTracking = True
  from InDetRecExample.InDetJobProperties import InDetFlags
  InDetFlags.doTruth            = (globalflags.DataSource == 'geant4' and globalflags.InputFormat == "pool")
  InDetFlags.preProcessing      = redoPatternRecoAndTracking
  InDetFlags.doPRDFormation        = False                       # those two will be (later) automatically false if
  InDetFlags.doSpacePointFormation = redoPatternRecoAndTracking  # preProcessing is false
  InDetFlags.doNewTracking      = redoPatternRecoAndTracking
  InDetFlags.doiPatRec          = False
  InDetFlags.doxKalman          = False
  InDetFlags.doLowPt            = False
  InDetFlags.doCTBTracking      = False
  InDetFlags.doBackTracking     = redoPatternRecoAndTracking
  InDetFlags.doTRTStandalone    = redoPatternRecoAndTracking
  InDetFlags.doTrtSegments      = redoPatternRecoAndTracking
  InDetFlags.doParticleCreation = redoPatternRecoAndTracking
  InDetFlags.doSlimming    = False
# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include("InDetRecExample/InDetRec_all.py")
#include("/afs/cern.ch/work/c/cerio/private/17.6.0.1/InnerDetector/InDetExample/InDetAlignExample/ErrorScaling/InDetRec_all.py")
#include("/afs/cern.ch/work/c/cerio/private/17.6.0.1/InnerDetector/InDetExample/InDetRecExample/share/InDetRec_all.py")

# --- dump MC truth into logfile
#from TruthExamples.TruthExamplesConf import DumpMC
#topSequence += DumpMC(McEventKey = "TruthEvent")
