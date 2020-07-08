#--------------------------------------------------------------
# Template jobOptions: SLHC 
#  - Tracking 
#--------------------------------------------------------------

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

if not athenaCommonFlags.PoolRDOInput.isDefault():
    athenaCommonFlags.FilesInput = athenaCommonFlags.PoolRDOInput()
elif not athenaCommonFlags.PoolESDInput.isDefault():
    athenaCommonFlags.FilesInput = athenaCommonFlags.PoolESDInput()  

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
# jobproperties.Global.DetDescrVersion='ATLAS-SLHC-01-00-00'
jobproperties.Global.DetDescrVersion='ATLAS-SLHC-01-03-00'

# --- printout
globalflags.print_JobProperties()
    
from AthenaCommon.BeamFlags import jobproperties
# --- default is high luminosity for SLHC
jobproperties.Beam.numberOfCollisions = 40.0  

from AthenaCommon.GlobalFlags import globalflags
# globalflags.ConditionsTag = "OFLCOND-SDR-BS14T-ATLAS-00"
globalflags.ConditionsTag = "OFLCOND-ATLAS-HL-LHC-00"

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
# --- no TRT for SLHC
DetFlags.TRT_setOff()
DetFlags.detdescr.TRT_setOff()
DetFlags.makeRIO.TRT_setOff()
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
if (hasattr(InDetFlags,"doPixelClusterSplitting")) :
    InDetFlags.doPixelClusterSplitting=False
#---slhc
InDetFlags.doTrackSegmentsPixel = False
InDetFlags.doTrackSegmentsSCT   = False
InDetFlags.doTrackSegmentsTRT   = False

# switch of tracking and vertex finding for AOD and ESD making
printfunc ('RecoStep:',rec.OutputFileNameForRecoStep())
if ( rec.OutputFileNameForRecoStep() == 'ESDtoAOD' or
     rec.OutputFileNameForRecoStep() == 'ESDtoDPD' ):
    printfunc ('switching off tracking and vertex finding for AOD and ESD making')
    InDetFlags.doNewTracking  =         False
    InDetFlags.doVertexFinding =        False

else:
    printfunc ('setting flags for RAWtoESD')
    InDetFlags.doLowPt        =         False
    InDetFlags.doBackTracking =         False
    InDetFlags.doTRTStandalone =        False
    #InDetFlags.doSingleSpBackTracking = False
    InDetFlags.postProcessing =         True
    InDetFlags.doBeamGas      =         False
    InDetFlags.doBeamHalo     =         False
    InDetFlags.doCosmics      =         False
    InDetFlags.doSharedHits   =         True
    # --- Turn off track slimming
    InDetFlags.doSlimming = False
    #---endslhc

    # --- Turn on Brem-Recovery
    InDetFlags.doBremRecovery.set_Value_and_Lock(True);
    InDetFlags.doCaloSeededBrem.set_Value_and_Lock(False);
    # --- Turn on Forward tracking
    InDetFlags.doForwardTracks.set_Value_and_Lock(True);

    # --- de-activate monitorings
    InDetFlags.doMonitoringGlobal    = False
    InDetFlags.doMonitoringPixel     = False
    InDetFlags.doMonitoringSCT       = False
    InDetFlags.doMonitoringTRT       = False
    InDetFlags.doMonitoringAlignment = False
    

    # --- activate creation of ntuples, standard plots
    #InDetFlags.doTrkNtuple      = False
    InDetFlags.doPhysValMon     = False
    InDetFlags.doSGDeletion     = False

# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon = True
from PerfMonComps.PerfMonFlags import jobproperties as jp
jp.PerfMonFlags.OutputFile=rec.OutputFileNameForRecoStep()+'.pmon.gz'
jp.PerfMonFlags.doMonitoring=True


#InDetFlags.doTrkD3PD        = True

from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails     = 2
TrackD3PDFlags.storeTrackPredictionAtBLayer                     = False
TrackD3PDFlags.storeTrackSummary                                = True
TrackD3PDFlags.storeHitTruthMatching                            = True
TrackD3PDFlags.storeDetailedTruth                               = True

from InDetRecExample.InDetKeys import InDetKeys
InDetKeys.trkValidationNtupleName = 'myTrkValidation.root'
#InDetKeys.trkD3PDFileName = 'myInDetTrackD3PD.root'

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = True


# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# turn on SLHC tracking
#--------------------------------------------------------------

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSLHC = True

# --- do not use DCS
InDetFlags.useDCS  = False

# --- turn off conditions
# ... missing

from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
SLHC_Flags.SLHC_Version = ''

# ---- setup of pixel tool
from PixelGeoModel.PixelGeoModelConf import PixelDetectorTool
pixelTool           = PixelDetectorTool()
# --- turn off alignment
pixelTool.Alignable = False

# --- setup of tracking geometry
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.SLHC_Geometry                   = True
TrkDetFlags.PixelBuildingOutputLevel        = VERBOSE
TrkDetFlags.SCT_BuildingOutputLevel         = VERBOSE
TrkDetFlags.TRT_BuildingOutputLevel         = VERBOSE
TrkDetFlags.MagneticFieldCallbackEnforced   = False
TrkDetFlags.TRT_BuildStrawLayers            = False
try:
    TrkDetFlags.MaterialSource                  = 'COOL'
except:
    TrkDetFlags.MaterialFromCool                = True ## old syntax for back-compatibility
TrkDetFlags.MaterialDatabaseLocal           = False and TrkDetFlags.MaterialFromCool()
TrkDetFlags.MaterialStoreGateKey            = '/GLOBAL/TrackingGeo/SLHC_LayerMaterial'
TrkDetFlags.MaterialTagBase                 = 'SLHC_LayerMat_v'
TrkDetFlags.MaterialVersion                 = 5
if SLHC_Flags.SLHC_Version() is '' :
    TrkDetFlags.MaterialMagicTag                = jobproperties.Global.DetDescrVersion()
else :
    TrkDetFlags.MaterialMagicTag                = SLHC_Flags.SLHC_Version() 
if TrkDetFlags.MaterialDatabaseLocal() is True :
    TrkDetFlags.MaterialDatabaseLocalPath    = ''
    TrkDetFlags.MaterialDatabaseLocalName    = 'SLHC_LayerMaterial-'+SLHC_Flags.SLHC_Version()+'.db'
TrkDetFlags.MagneticFieldCallbackEnforced         = False
TrkDetFlags.LArUseMaterialEffectsOnTrackProvider  = False
TrkDetFlags.TileUseMaterialEffectsOnTrackProvider = False



# --- do tracking D3PD
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
#ServiceMgr.EventSelector.SkipEvents = 2
#ServiceMgr.StoreGateSvc.Dump = True

#ServiceMgr.PoolSvc.AttemptCatalogPatch=True

# --- default SLHC test file from digit output
#ServiceMgr.EventSelector.InputCollections = [ 'singleMuon_test_slhc_Digits.pool.root' ]

# set the path variables consistently
from InDetSLHC_Example.SLHC_Setup import SLHC_Setup
SLHC_Setup = SLHC_Setup()

