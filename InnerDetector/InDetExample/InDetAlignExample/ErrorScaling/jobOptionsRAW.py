# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# --- read BS 
doReadBS        = True

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
import os

#if not doReadBS:
#  athenaCommonFlags.FilesInput = ["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/perf-idtracking/InDetRecExample/mc09_7TeV.105200.T1_McAtNlo_Jimmy.digit.RDO.e510_s624_s633_d287_tid112426_00/RDO.112426._000007.pool.root.1"] 
#else:
#  athenaCommonFlags.FilesInput = ["rfio:/castor/cern.ch/grid/atlas/DAQ/2011/00180481/calibration_IDTracks/data11_7TeV.00180481.calibration_IDTracks.daq.RAW._lb0000._SFO-10._0002.data"]

#athenaCommonFlags.FilesInput = [ 'root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/mc10_7TeV/DESD_MBIAS/e574_s932_s946_r2094/mc10_7TeV.105001.pythia_minbias.recon.DESD_MBIAS.e574_s932_s946_r2094_tid280668_00/DESD_MBIAS.280668._016228.pool.root.1']

#athenaCommonFlags.FilesInput = [ 'root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/data11_7TeV/DRAW_ZMUMU/f368_m716/data11_7TeV.00180164.physics_Muons.merge.DRAW_ZMUMU.f368_m716/data11_7TeV.00180164.physics_Muons.merge.DRAW_ZMUMU.f368_m716._0002.1' ]

#athenaCommonFlags.FilesInput = [ 'root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/user/lacuesta/data11_7TeV/user.lacuesta.data11_7TeV.00191425.calibration_IDTracks.daq.RAW_der1324561622/data11_7TeV.00191425.calibration_IDTracks.daq.RAW._lb0000._SFO-10._0006.data' ]

#athenaCommonFlags.FilesInput = [ '/afs/cern.ch/user/a/atlidali/eos/atlas/atlasdatadisk/user/lacuesta/data11_7TeV/user.lacuesta.data11_7TeV.00191425.calibration_IDTracks.daq.RAW_der1324561622/data11_7TeV.00191425.calibration_IDTracks.daq.RAW._lb0000._SFO-10._0006.data','/afs/cern.ch/user/a/atlidali/eos/atlas/atlasdatadisk/user/lacuesta/data11_7TeV/user.lacuesta.data11_7TeV.00191425.calibration_IDTracks.daq.RAW_der1324561622/data11_7TeV.00191425.calibration_IDTracks.daq.RAW._lb0000._SFO-10._0011.data','/afs/cern.ch/user/a/atlidali/eos/atlas/atlasdatadisk/user/lacuesta/data11_7TeV/user.lacuesta.data11_7TeV.00191425.calibration_IDTracks.daq.RAW_der1324561622/data11_7TeV.00191425.calibration_IDTracks.daq.RAW._lb0000._SFO-11._0003.data','/afs/cern.ch/user/a/atlidali/eos/atlas/atlasdatadisk/user/lacuesta/data11_7TeV/user.lacuesta.data11_7TeV.00191425.calibration_IDTracks.daq.RAW_der1324561622/data11_7TeV.00191425.calibration_IDTracks.daq.RAW._lb0000._SFO-11._0004.data','/afs/cern.ch/user/a/atlidali/eos/atlas/atlasdatadisk/user/lacuesta/data11_7TeV/user.lacuesta.data11_7TeV.00191425.calibration_IDTracks.daq.RAW_der1324561622/data11_7TeV.00191425.calibration_IDTracks.daq.RAW._lb0000._SFO-4._0009.data','/afs/cern.ch/user/a/atlidali/eos/atlas/atlasdatadisk/user/lacuesta/data11_7TeV/user.lacuesta.data11_7TeV.00191425.calibration_IDTracks.daq.RAW_der1324561622/data11_7TeV.00191425.calibration_IDTracks.daq.RAW._lb0000._SFO-4._0010.data','/afs/cern.ch/user/a/atlidali/eos/atlas/atlasdatadisk/user/lacuesta/data11_7TeV/user.lacuesta.data11_7TeV.00191425.calibration_IDTracks.daq.RAW_der1324561622/data11_7TeV.00191425.calibration_IDTracks.daq.RAW._lb0000._SFO-5._0003.data','/afs/cern.ch/user/a/atlidali/eos/atlas/atlasdatadisk/user/lacuesta/data11_7TeV/user.lacuesta.data11_7TeV.00191425.calibration_IDTracks.daq.RAW_der1324561622/data11_7TeV.00191425.calibration_IDTracks.daq.RAW._lb0000._SFO-5._0007.data','/afs/cern.ch/user/a/atlidali/eos/atlas/atlasdatadisk/user/lacuesta/data11_7TeV/user.lacuesta.data11_7TeV.00191425.calibration_IDTracks.daq.RAW_der1324561622/data11_7TeV.00191425.calibration_IDTracks.daq.RAW._lb0000._SFO-5._0012.data','/afs/cern.ch/user/a/atlidali/eos/atlas/atlasdatadisk/user/lacuesta/data11_7TeV/user.lacuesta.data11_7TeV.00191425.calibration_IDTracks.daq.RAW_der1324561622/data11_7TeV.00191425.calibration_IDTracks.daq.RAW._lb0000._SFO-9._0004.data','/afs/cern.ch/user/a/atlidali/eos/atlas/atlasdatadisk/user/lacuesta/data11_7TeV/user.lacuesta.data11_7TeV.00191425.calibration_IDTracks.daq.RAW_der1324561622/data11_7TeV.00191425.calibration_IDTracks.daq.RAW._lb0000._SFO-9._0007.data','/afs/cern.ch/user/a/atlidali/eos/atlas/atlasdatadisk/user/lacuesta/data11_7TeV/user.lacuesta.data11_7TeV.00191425.calibration_IDTracks.daq.RAW_der1324561622/data11_7TeV.00191425.calibration_IDTracks.daq.RAW._lb0000._SFO-9._0011.data']

#athenaCommonFlags.FilesInput = ["root://eosatlas.cern.ch//eos/atlas/atlasgroupdisk/perf-egamma/dq2/mc11_valid/RDO/e825_s1310_s1300_d578/mc11_valid.106052.PythiaZtautau.digit.RDO.e825_s1310_s1300_d578_tid512904_00/RDO.512904._000001.pool.root.1"]

#use vicente's RAW files
#os.system('eosforceumount ${HOME}/eos')
#os.system('eosmount ${HOME}/eos')
#a = os.popen('eosforceumount ${HOME}/eos')
#b = os.popen('eosmount ${HOME}/eos')

#rawDatasets = open('rawDatasets','r')
#files = rawDatasets.readlines()
#localFiles = []
#for ii in files:
#  tt = '/afs/cern.ch/user/a/atlidali' + ii[16:]
#  localFiles.append(tt.rstrip('\n'))

#athenaCommonFlags.FilesInput = localFiles
#rawDatasets.close()

#athenaCommonFlags.FilesInput = [ 'data11_7TeV.00191425.calibration_IDTracks.daq.RAW._lb0000._SFO-10._0006.data' ]

#if doReadBS:

os.environ['STAGE_HOST']="castoratlas"
os.environ['STAGE_SVCCLASS']="atlcal"


#os.environ['STAGE_HOST']="castoratlast3"
#os.environ['STAGE_SVCCLASS']="atlascerngroupdisk"      
#//athenaCommonFlags.FilesInput = ["rfio:/castor/cern.ch/atlas/atlascerngroupdisk/perf-idtracking/InDetRecExample/data10_7TeV.00154817.physics_MinBias.merge.RAW/data10_7TeV.00154817.physics_MinBias.merge.RAW._lb0100._0001.1"]
from AthenaCommon.GlobalFlags import globalflags
#globalflags.ConditionsTag.set_Value_and_Lock("COMCOND-BLKPST-004-05")
notMC = False
if notMC:
    globalflags.ConditionsTag.set_Value_and_Lock("COMCOND-BLKPST-005-06")
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

#theApp.EvtMax = 100
  
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

InDetFlags.doPRDFormation = True
#InDetFlags.doPixelClusterSplitting = False
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
