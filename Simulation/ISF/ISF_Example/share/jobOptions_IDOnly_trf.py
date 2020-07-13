#--------------------------------------------------------------
# Template jobOptions: SLHC 
#  - Tracking 
#--------------------------------------------------------------

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

from RecExConfig.RecFlags import rec
# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = INFO
# --- produce an atlantis data file
doJiveXML       = False
# --- run the Virtual Point 1 event visualisation
doVP1           = False
# --- controls what is written out. ESD includes AOD, so it's normally enough
doWriteESD      = rec.doWriteESD()
doWriteAOD      = rec.doWriteAOD()
# --- read BS - please look at the default input file(s) to know the det descr and cond tag
doReadBS        = False
# --- do auditors ?
doAuditors      = True

if os.environ['CMTCONFIG'].endswith('-dbg'):
  # --- do EDM monitor (debug mode only)
  doEdmMonitor    = True 
  # --- write out a short message upon entering or leaving each algorithm
  doNameAuditor   = True
else:
  doEdmMonitor    = False
  doNameAuditor   = False



#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
globalflags.DataSource = 'geant4'
# --- input is pool for SLHC 
globalflags.InputFormat = 'pool'

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-GEO-20-00-01'

# --- printout
globalflags.print_JobProperties()
    
from AthenaCommon.BeamFlags import jobproperties
# --- default is high luminosity for SLHC
jobproperties.Beam.numberOfCollisions = 40.0  

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-MC12-SDR-06"

# --- no conditions for SLHC
if len(globalflags.ConditionsTag())!=0:
   from IOVDbSvc.CondDB import conddb
   conddb.setGlobalTag(globalflags.ConditionsTag())

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------

from RecExConfig.RecFlags import rec
rec.Commissioning=False

from AthenaCommon.DetFlags import DetFlags 
# --- switch on InnerDetector
DetFlags.ID_setOn()
# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
# --- printout
DetFlags.Print()

#DetFlags.writeRDOPool.pixel_setOn()

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth       = (globalflags.InputFormat() == 'pool')

InDetFlags.doLowBetaFinder = False


# --- uncomment to change the default of one of the following options:
#OutputLevel          = DEBUG

# --- possibility to change the trackfitter
#InDetFlags.trackFitterType = 'KalmanFitter'
#if (hasattr(InDetFlags,"doPixelClusterSplitting")) :
#  InDetFlags.doPixelClusterSplitting=False
#---slhc
#InDetFlags.doTrackSegmentsPixel = False
#InDetFlags.doTrackSegmentsSCT   = False
#InDetFlags.doTrackSegmentsTRT   = False

#InDetFlags.doNewTracking  =         True
#InDetFlags.doLowPt        =         False
#InDetFlags.doxKalman      =         False
#InDetFlags.doiPatRec      =         False
#InDetFlags.doBackTracking =         False
#InDetFlags.doTRTStandalone =        False
#InDetFlags.doSingleSpBackTracking = False
InDetFlags.postProcessing =         True
InDetFlags.doBeamGas      =         False
InDetFlags.doBeamHalo     =         False
InDetFlags.doCosmics      =         False
InDetFlags.doSharedHits   =         True
# --- Turn off track slimming
InDetFlags.doSlimming = False
#---endslhc

# --- de-activate monitorings
InDetFlags.doMonitoringGlobal    = False
InDetFlags.doMonitoringPixel     = False
InDetFlags.doMonitoringSCT       = False
InDetFlags.doMonitoringTRT       = False
InDetFlags.doMonitoringAlignment = False

# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon = True

# --- activate creation of ntuples, standard plots
#InDetFlags.doTrkNtuple      = False
InDetFlags.doPhysValMon  = False
InDetFlags.doSGDeletion     = False
#InDetFlags.doTrkD3PD        = True

from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails     = 2
TrackD3PDFlags.storeTrackPredictionAtBLayer                     = False
TrackD3PDFlags.storeTrackSummary                                = True
TrackD3PDFlags.storeHitTruthMatching                            = True
TrackD3PDFlags.storeDetailedTruth                               = True


# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = True


# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# turn on SLHC tracking
#--------------------------------------------------------------

from InDetRecExample.InDetJobProperties import InDetFlags

# --- do tracking D3PD
from InDetRecExample.InDetKeys import InDetKeys
if hasattr(runArgs,"outputDESDM_TRACKFile"):
  InDetFlags.doTrkD3PD.set_Value_and_Lock(True)
  InDetKeys.trkD3PDFileName.set_Value_and_Lock(runArgs.outputDESDM_TRACKFile)

# --- Set output names such that they work with Reco_trf.py
if athenaCommonFlags.PoolESDOutput(): InDetKeys.OutputESDFileName = athenaCommonFlags.PoolESDOutput()
if athenaCommonFlags.PoolAODOutput(): InDetKeys.OutputAODFileName = athenaCommonFlags.PoolAODOutput()
#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include("InDetRecExample/InDetRec_all.py")


#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# Number of events to be processed 
theApp.EvtMax = athenaCommonFlags.EvtMax()

# --- default SLHC test file from digit output
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.PoolRDOInput()
athenaCommonFlags.FilesInput = athenaCommonFlags.PoolRDOInput()
