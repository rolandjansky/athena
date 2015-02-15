#--------------------------------------------------------------
# Template jobOptions: SLHC 
#  - Tracking 
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

# AK-SCZ geometry - complain if not explicitly set
if not 'DetDescrVersion' in dir():
  DetDescrVersion = 'ATLAS-IBL-01-00-00'

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion=DetDescrVersion


# --- printout
globalflags.print_JobProperties()
    
from AthenaCommon.BeamFlags import jobproperties
# --- default is high luminosity for SLHC
jobproperties.Beam.numberOfCollisions = 40.0  

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
# AK-SCZ - set fitter - does not complain if not explicitly set, just lets default stand
if 'FitterType' in dir():
  InDetFlags.trackFitterType = FitterType

if (hasattr(InDetFlags,"doPixelClusterSplitting")) :
  InDetFlags.doPixelClusterSplitting=False
#---slhc
InDetFlags.doTrackSegmentsPixel = False
InDetFlags.doTrackSegmentsSCT   = False
InDetFlags.doTrackSegmentsTRT   = False

InDetFlags.doNewTracking  =         True
InDetFlags.doLowPt        =         False
InDetFlags.doxKalman      =         False
InDetFlags.doiPatRec      =         False
InDetFlags.doBackTracking =         False
InDetFlags.doTRTStandalone =        False
#InDetFlags.doSingleSpBackTracking = False
InDetFlags.postProcessing =         True
InDetFlags.doBeamGas      =         False
InDetFlags.doBeamHalo     =         False
InDetFlags.doCosmics      =         False
InDetFlags.doSharedHits   =         False
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
InDetFlags.doTrkNtuple      = True
InDetFlags.doStandardPlots  = True
InDetFlags.doSGDeletion     = False
InDetFlags.doTrkD3PD        = True

from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails     = 2
TrackD3PDFlags.storeTrackPredictionAtBLayer                     = False
TrackD3PDFlags.storeTrackSummary                                = True
TrackD3PDFlags.storeHitTruthMatching                            = True
TrackD3PDFlags.storeDetailedTruth                               = True

from InDetRecExample.InDetKeys import InDetKeys
InDetKeys.trkValidationNtupleName = 'myTrkValidation.root'
InDetKeys.trkD3PDFileName = 'myInDetTrackD3PD.root'

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = True

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# turn on IBL tracking
#--------------------------------------------------------------
include ("InDetIBL_Example/InDetIBLRecoPreInclude.py")

InDetFlags.doStandardPlots.unlock();
InDetFlags.doStandardPlots.set_Value_and_Lock(True);

from IOVDbSvc.CondDB import conddb
conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterial','AtlasLayerMat_v15_ATLAS-IBL-02-00-00')

#
# -- Truth jet creation
#
from JetRec.JetGetters import *
antiKt2alg = make_StandardJetGetter('AntiKt',0.4,'Truth').jetAlgorithmHandle()
cone2alg = make_StandardJetGetter('Cone',0.4,'Truth').jetAlgorithmHandle()

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include("InDetRecExample/InDetRec_all.py")

#--------------------------------------------------------------
# turn on IBL tracking
#--------------------------------------------------------------
include ("InDetIBL_Example/InDetIBLRecoPostInclude.py")

#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# Number of events to be processed (default is 10)
theApp.EvtMax = -1

ServiceMgr.PoolSvc.AttemptCatalogPatch=True

# --- default SLHC test file from digit output
ServiceMgr.EventSelector.InputCollections = [ 'root://castoratlas//castor/cern.ch/grid/atlas/atlasgroupdisk/det-slhc/group/det-slhc/mc11_14TeV/group.det-slhc.mc11_14TeV.105001.pythia_minbias.ibl02.ESD.D3PD.NTUP.val3.test.110713122344/group.det-slhc.25330_000665.EXT0._00009.ESD.root' ]

