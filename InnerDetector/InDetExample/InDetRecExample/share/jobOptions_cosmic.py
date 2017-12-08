#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# --- read BS 
doReadBS        = True

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

if not doReadBS:
  athenaCommonFlags.FilesInput = ["root://eosatlas//eos/atlas/atlascerngroupdisk/perf-idtracking/InDetRecExample/data10_cos.00151040.physics_IDCosmic.recon.ESD.f220/data10_cos.00151040.physics_IDCosmic.recon.ESD.f220._lb0002._0001.1"]

if doReadBS:
  # Since BS doesn't yet support xrootd need to set
  import os
  os.environ['STAGE_HOST']="castoratlast3"
  os.environ['STAGE_SVCCLASS']="atlascerngroupdisk"  
  athenaCommonFlags.FilesInput = ["root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/tct/rec_input/tracking/data10_cos.00151040.physics_IDCosmic.merge.RAW._lb0002._0001.1"]

import AthenaPython.ConfigLib as apcl
cfg = apcl.AutoCfg(name = 'InDetRecExampleAutoConfig', input_files=athenaCommonFlags.FilesInput())
cfg.configure_job()

if doReadBS:
  from AthenaCommon.GlobalFlags import globalflags
  if len(globalflags.ConditionsTag())!=0:
    from IOVDbSvc.CondDB import conddb
    conddb.setGlobalTag(globalflags.ConditionsTag())

# request TrackingGeo tag  AtlasLayerMat_v18_ATLAS-GEO-16
# but there are only   AtlasLayerMat_v16_ATLAS-GEO-16
#                   or AtlasLayerMat_v18_ATLAS-R1-2010-01
# @TODO switch to more recent data file ?
conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterialV2','AtlasLayerMat_v19s0_ATLAS-R1-2010-01')
#conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterialV2','AtlasLayerMat_v16_ATLAS-GEO-16')
import MagFieldServices.SetupField

theApp.EvtMax = 50

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
# --- read BS - please look at the default input file(s) to know the det descr and cond tag
doReadBS        = True
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
# Additional Detector setup
#--------------------------------------------------------------

from AthenaCommon.DetFlags import DetFlags 
# --- switch on InnerDetector
DetFlags.ID_setOn()

#DetFlags.makeRIO.pixel_setOff()
#DetFlags.readRIOPool.pixel_setOff()
#DetFlags.readRIOBS.pixel_setOff() 
#DetFlags.readRDOBS.pixel_setOff() 

#DetFlags.makeRIO.SCT_setOff()
#DetFlags.readRIOPool.SCT_setOff()
#DetFlags.readRIOBS.SCT_setOff() 
#DetFlags.readRDOBS.SCT_setOff() 

#DetFlags.TRT_setOff()
#DetFlags.detdescr.TRT_setOn()

# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

# --- switch off DCS
#DetFlags.dcs.pixel_setOff()
#DetFlags.dcs.SCT_setOff()
#DetFlags.dcs.TRT_setOff()

# --- printout
DetFlags.Print()

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags

from RecExConfig.RecFlags import rec
rec.Commissioning = True
rec.doMonitoring = True

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth       = (globalflags.DataSource == 'geant4' and globalflags.InputFormat == "pool")

#InDetFlags.doCosmics=True
# --- uncomment to change the default of one of the following options:
#InDetFlags.doNewTracking  = False
##InDetFlags.preProcessing  = True
#InDetFlags.doLowPt        = True
#InDetFlags.doBeamGas      = True
#InDetFlags.doBeamHalo     = True
##InDetFlags.doxKalman      = False
##InDetFlags.doiPatRec      = False
#InDetFlags.doBackTracking = True
#InDetFlags.doTRTStandalone = True
##InDetFlags.doVertexFinding=False
##InDetFlags.doConversions=False

# --- Turn off track slimming
#InDetFlags.doSlimming = False

# --- possibility to change the trackfitter
#InDetFlags.trackFitterType = 'KalmanFitter'

# --- deactivate monitorings
#InDetFlags.doMonitoringGlobal    = False
#InDetFlags.doMonitoringPixel     = False
#InDetFlags.doMonitoringSCT       = False
#InDetFlags.doMonitoringTRT       = False
#InDetFlags.doMonitoringAlignment = False


# --- Disable DCS usage
#InDetFlags.useDCS=False

# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon = True

# --- activate creation of standard plots
#InDetFlags.doStandardPlots  = True

# --- produce various ntuples (all in one root file)
#InDetFlags.doTrkNtuple      = True
#InDetFlags.doPixelTrkNtuple = True
#InDetFlags.doSctTrkNtuple   = True
#InDetFlags.doTrtTrkNtuple   = True
#InDetFlags.doSctClusterNtuple   = True
#InDetFlags.doVtxNtuple      = True
#InDetFlags.doConvVtxNtuple  = True
#InDetFlags.doV0VtxNtuple    = True

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = True

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include("InDetRecExample/InDetRec_all.py")
