#--------------------------------------------------------------
# Template jobOptions: SLHC 
#  - Fatras
#--------------------------------------------------------------

#--------------------------------------------------------------
# Set to monte carlo
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixStandardJob    
from AthenaCommon import AthenaCommonFlags
from AthenaCommon.AppMgr import theApp    
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.GlobalFlags import GlobalFlags

# let Fatras build the geometry for you
from FatrasExample.FatrasJobProperties import FatrasFlags

# set the detectors on/off and steer the fast simulation flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
# --- no TRT for SLHC
DetFlags.TRT_setOff()
DetFlags.detdescr.TRT_setOff()
DetFlags.makeRIO.TRT_setOff()
# Calo and Muon off for SLHC
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

#from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
#GeoModelSvc.InDetVersionOverride="InnerDetector-SLHC-00"

from FastSimulationConfig.FastSimulationFlags import jobproperties
jobproperties.FastSimulation.doFatrasID   = DetFlags.ID_on()
jobproperties.FastSimulation.doFatrasMuon = DetFlags.Muon_on()

# run additional fast calo sim
FatrasFlags.FastCaloSim = False

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
DetDescrVersion = 'ATLAS-SLHC-01-00-00'
jobproperties.Global.DetDescrVersion = DetDescrVersion

# --- printout
globalflags.print_JobProperties()
    
from AthenaCommon.BeamFlags import jobproperties
# --- default is high luminosity for SLHC
jobproperties.Beam.numberOfCollisions = 40.0  

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-SDR-BS14T-ATLAS-00"

# --- no conditions for SLHC
if len(globalflags.ConditionsTag())!=0:
   from IOVDbSvc.CondDB import conddb
   conddb.setGlobalTag(globalflags.ConditionsTag())

# Temporary until BField tag for ATLAS-SLHC-01-00-00 put into COOL
from IOVDbSvc.CondDB import conddb
conddb.addOverride('/GLOBAL/BField/Map','BFieldMap-GEO-06-00-00')


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

# SLHC Flags -----------------------------------------------------
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
TrkDetFlags.MaterialFromCool                = True
TrkDetFlags.MaterialDatabaseLocal           = SLHC_Flags.SLHC_Version() != '' and TrkDetFlags.MaterialFromCool()
TrkDetFlags.MaterialStoreGateKey            = '/GLOBAL/TrackingGeo/SLHC_LayerMaterial'
TrkDetFlags.MaterialTagBase                 = 'SLHC_LayerMat_v'
TrkDetFlags.MaterialVersion                 = 6
TrkDetFlags.MaterialMagicTag                = DetDescrVersion
if TrkDetFlags.MaterialDatabaseLocal() is True :
   TrkDetFlags.MaterialMagicTag                = SLHC_Flags.SLHC_Version()
   TrkDetFlags.MaterialDatabaseLocalPath    = ''
   TrkDetFlags.MaterialDatabaseLocalName    = 'SLHC_LayerMaterial-'+SLHC_Flags.SLHC_Version()+'.db'
TrkDetFlags.MagneticFieldCallbackEnforced         = False
TrkDetFlags.LArUseMaterialEffectsOnTrackProvider  = False
TrkDetFlags.TileUseMaterialEffectsOnTrackProvider = False


# InDet Flags ----------------------------------------------------
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSLHC       = True
# --- possibility to change the trackfitter
#InDetFlags.trackFitterType = 'KalmanFitter'
#---slhc
InDetFlags.doTrackSegmentsPixel = False
InDetFlags.doTrackSegmentsSCT   = False
InDetFlags.doTrackSegmentsTRT   = False

InDetFlags.doNewTracking  =         True
InDetFlags.doLowPt        =         False
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

InDetFlags.doLowBetaFinder       = False

#--------------------------------------------------------------
# Number of events to be processed
#--------------------------------------------------------------
theApp.EvtMax = 10

#--------------------------------------------------------------
# General Job Steering
#--------------------------------------------------------------
FatrasFlags.BuildGeometry = True
# simulation flags are steered by FastSimulation flags
# refit/reconstruction can be changed
# (ID)
FatrasFlags.RefittingID      = False
FatrasFlags.ReconstructionID = True
# (MS)
FatrasFlags.RefittingMS       = False
FatrasFlags.DigitizationMS    = True
FatrasFlags.ReconstructionMS  = True
#--------------------------------------------------------------
# Output
#--------------------------------------------------------------
FatrasFlags.WriteESD = False
FatrasFlags.WriteAOD = False
#FatrasFlags.WriteJiveXML = True

#--------------------------------------------------------------
# Input
#--------------------------------------------------------------
# uncomment and change for single track simulaton
FatrasFlags.SingleTrackSimulation = True

from FatrasExample.FatrasSingleTrackSimulation \
    import FatrasSingleTrackSimulationFlags
FatrasSingleTrackSimulationFlags.MinimumMomentum = 100000.
FatrasSingleTrackSimulationFlags.MaximumMomentum = 100000.
FatrasSingleTrackSimulationFlags.MinimumEta      = -3.0
FatrasSingleTrackSimulationFlags.MaximumEta      =  3.0
FatrasSingleTrackSimulationFlags.ParticleType    = 13
FatrasSingleTrackSimulationFlags.UseDiscreteMomentumSteps = False
FatrasSingleTrackSimulationFlags.DiscreteMomentumSteps    = 10

# from pool and set up the event selector
if not FatrasFlags.SingleTrackSimulation() :
  import AthenaPoolCnvSvc.ReadAthenaPool 
  from AthenaCommon.AppMgr import ServiceMgr
  ServiceMgr.EventPersistencySvc.CnvServices += [ 'AthenaPoolCnvSvc' ] 

  ServiceMgr.EventSelector.InputCollections = [
    '/afs/cern.ch/atlas/groups/fatras/fatras_input/CSC.005145.PythiaZmumu.EVGEN.root'
    ]

  #ServiceMgr.EventSelector.InputCollections = [
  #  '/afs/cern.ch/atlas/groups/fatras/fatras_input/mc12.005200.T1_McAtNlo_Jimmy.evgen.EVNT.v12000604_tid007939._00036.pool.root.1'
  #  ]


#--------------------------------------------------------------
# Debug
#--------------------------------------------------------------
from FatrasExample.FatrasOutputLevels import FatrasOutputLevelFlags
#FatrasOutputLevelFlags.GeometryBuilding   = VERBOSE
#FatrasOutputLevelFlags.FatrasAlgs         = VERBOSE
#FatrasOutputLevelFlags.FatrasTools        = VERBOSE
#FatrasOutputLevelFlags.FatrasMuonTools    = VERBOSE
#FatrasOutputLevelFlags.FatrasMuonAlgs     = VERBOSE
#FatrasOutputLevelFlags.ExtrapolationTools = VERBOSE
#FatrasOutputLevelFlags.FittingTools       = VERBOSE

#--------------------------------------------------------------
# Tuning & Cut parameter optimization
#--------------------------------------------------------------
from FatrasExample.FatrasTuning import FatrasTuningFlags
#FatrasTuningFlags.MinimumParticlePtPrimaryTrackCreation    = 150.
#FatrasTuningFlags.MinimumMuonPtTrackCreation               = 1000.
#FatrasTuningFlags.MinimumParticleMomentum                  = 50.
#FatrasTuningFlags.MinimumBremPhotonMomentum                = 150.
#FatrasTuningFlags.BremCreationVolumeRadius                 = 1200.
#FatrasTuningFlags.BremCreationVolumeHalfZ                  = 4000.
#FatrasTuningFlags.BetheHeitlerScalor                       = 1.
#FatrasTuningFlags.ParticleDecayMinChildEnergy              = 250.
#FatrasTuningFlags.ConversionMinChildEnergy                 = 100.
#FatrasTuningFlags.ConversionChildEnergyScalor              = 2.
#FatrasTuningFlags.ConversionProbabilityScalor              = 0.98
#FatrasTuningFlags.ConversionVolumeRadius                   = 1200.
#FatrasTuningFlags.ConversionVolumeHalfZ                    = 4000.
#FatrasTuningFlags.HadronicInteractionProbabilityScalor     = 1.
#FatrasTuningFlags.HadronicInteractionMinMomentumIn         = 100.
#FatrasTuningFlags.HadronicInteractionMinMomentumOut        = 200.
#FatrasTuningFlags.PixNoiseLevel                            = 1e-6
#FatrasTuningFlags.SctNoiseLevel                            = 1e-6
#FatrasTuningFlags.TrtNoiseLevel                            = 0.02
#FatrasTuningFlags.PixelEfficiencies                        = 1.
#FatrasTuningFlags.SctEfficiencies                          = 1.
#FatrasTuningFlags.TrtEfficiencies                          = 1.
#FatrasTuningFlags.MaterialDistortionsX0                    = 0.02
#FatrasTuningFlags.MaterialDistortionsRho                   = 0.02

from FatrasExample.FatrasClusterCreation import FatrasClusterCreationFlags
FatrasClusterCreationFlags.UseConditionsSvc = False
#FatrasClusterCreationFlags.PixMinPathLength             = 0.05
#FatrasClusterCreationFlags.PixSmearPathSigma            = 0.05
#FatrasClusterCreationFlags.PixPathLengthTotConversion   = 250.
#FatrasClusterCreationFlags.SctMinPathLength             = 0.05
#FatrasClusterCreationFlags.SctSmearPathSigma            = 0.05
#FatrasClusterCreationFlags.TrtSigmaDriftRadius          = 0.14
#FatrasClusterCreationFlags.TrtSigmaDriftRadiusTail      = 0.400
#FatrasClusterCreationFlags.TrtDriftTailFraction         = 0.03

#--------------------------------------------------------------
# Auditors
#--------------------------------------------------------------
# FatrasFlags.RunAuditors = True

#--------------------------------------------------------------
# Event Display
#--------------------------------------------------------------
FatrasFlags.doVP1 = False

#--------------------------------------------------------------
# Validation
#--------------------------------------------------------------
from FatrasExample.FatrasValidation import FatrasValidationFlags
#FatrasValidationFlags.TrkNtuple                = True
#FatrasValidationFlags.Simulation               = True
#FatrasValidationFlags.Refitting                = True
#FatrasValidationFlags.Residuals                = True
#FatrasValidationFlags.OutputFileName           = True
#FatrasValidationFlags.ValidationStream         = True
#FatrasValidationFlags.MaterialEffects          = True
#FatrasValidationFlags.Bremsstrahlung           = True
#FatrasValidationFlags.CaloEnergyDeposition     = True
#FatrasValidationFlags.PairProduction           = True
#FatrasValidationFlags.ParticleDecay            = True
#FatrasValidationFlags.Simulation               = True
#FatrasValidationFlags.Refitting                = True
#FatrasValidationFlags.Reconstruction           = True
#FatrasValidationFlags.TrkNtuple                = True
#FatrasValidationFlags.Residuals                = True
#FatrasValidationFlags.TrackPlotter             = True
#FatrasValidationFlags.McNtuple                 = True
#FatrasValidationFlags.PixelClusterSimulation    = True
#FatrasValidationFlags.SctClusterSimulation      = True
#FatrasValidationFlags.TrtDriftCircleSimulation  = True
#FatrasValidationFlags.MuonSimHitNtuple         = True
#FatrasValidationFlags.MuonSimHitValidation     = False
#FatrasValidationFlags.MuonMonitoringNtuple     = True
#FatrasValidationFlags.CheckGenEventRecord      = True

# ID based validation
InDetFlags.doTrkD3PD         = True
#InDetFlags.doPixelTrkNtuple = True
#InDetFlags.doVtxNtuple      = True
#InDetFlags.doConvVtxNtuple  = True
#InDetFlags.doV0VtxNtuple    = True
#InDetFlags.doPhysValMon   = True
InDetFlags.doSGDeletion      = False

from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails     = 2
TrackD3PDFlags.storeTrackPredictionAtBLayer                     = False
TrackD3PDFlags.storeTrackSummary                                = True
TrackD3PDFlags.storeHitTruthMatching                            = True  
TrackD3PDFlags.storeDetailedTruth                               = True

#--------------------------------------------------------------
# GenericGeometry interface
#--------------------------------------------------------------
# FatrasFlags.InputGeometry.set_Value( "FatrasDetDescrExample/AtlasGeometry.py")

#--------------------------------------------------------------
# Final setup
#--------------------------------------------------------------
# the master jobOptions to 
include("FatrasExample/Fatras_jobOptions.py")    
# screen output saving
ServiceMgr.MessageSvc.Format      = "% F%50W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = 3

#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------
# set the path variables consistently
from InDetSLHC_Example.SLHC_Setup import SLHC_Setup
SLHC_Setup = SLHC_Setup()

#ServiceMgr.StoreGateSvc.Dump = True
#ServiceMgr.DetectorStore.Dump = True
#ServiceMgr.EventSelector.SkipEvents = 50
