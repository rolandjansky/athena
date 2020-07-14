#--------------------------------------------------------------------
# Perfmon configuration
#--------------------------------------------------------------------
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
jobproperties.PerfMonFlags.doMallocMonitoring = True
jobproperties.PerfMonFlags.doPostProcessing = True

from ISF_FatrasServices.ISF_FatrasJobProperties import ISF_FatrasFlags
ISF_FatrasFlags.OutputLevelGeneral = INFO
#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.SkipEvents=0
athenaCommonFlags.EvtMax=10
athenaCommonFlags.PoolHitsInput=["Hits.pool.root"]
athenaCommonFlags.PoolRDOOutput="DigitizationOutput.pool.root"

# the magnetic field
from MagFieldServices import SetupField
from IOVDbSvc.CondDB import conddb
conddb.addOverride('/GLOBAL/BField/Map','BFieldMap-FullAsym-09-solTil')

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.MaterialSource           = 'Input'

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.IOVDbGlobalTag='OFLCOND-SIM-00-00-00'

#--------------------------------------------------------------
# Global flags. Like eg the DD version:
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion='ATLAS-PX-ITK-00-00-00'

#--------------------------------------------------------------------
# DetFlags. Use to turn on/off individual subdetector or LVL1 trigger
#--------------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.detdescr.Muon_setOff()
DetFlags.LVL1_setOff()
DetFlags.Truth_setOn()

include("FastSiDigitization/SiSmearing_jobOptions.py")

DetFlags.pixel_setOff()
DetFlags.detdescr.pixel_setOn()

DetFlags.SCT_setOff() # setOn to use the standard digi
DetFlags.detdescr.SCT_setOn()

DetFlags.TRT_setOff() # setOn to use the standard digi

include("Digitization/Digitization.py")

from InDetRecExample.InDetJobProperties import InDetFlags

# --- Set output level threshold (2=DEBUG, 3=VERBOSE, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = VERBOSE
# --- produce an atlantis data file
doJiveXML       = False
# --- run the Virtual Point 1 event visualisation
doVP1           = True
# --- controls what is written out. ESD includes AOD, so it's normally enough
doWriteESD      = True
doWriteAOD      = True
# --- do auditors ?
doAuditors      = True

DetFlags.makeRIO.pixel_setOff()
DetFlags.makeRIO.SCT_setOff()

DetFlags.readRIOPool.pixel_setOn()
DetFlags.readRIOPool.SCT_setOn()

DetFlags.haveRIO.pixel_on()
DetFlags.haveRIO.SCT_on()

# --- uncomment to change the default of one of the following options:
InDetFlags.doTruth       = True
InDetFlags.doNewTracking           = True
# --- Turn off track slimming
#InDetFlags.doSlimming = False

# --- possibility to change the trackfitter
InDetFlags.trackFitterType = 'KalmanFitter'
#InDetFlags.trackFitterType = 'GlobalChi2Fitter'

# --- activate monitorings
InDetFlags.doMonitoringGlobal    = False
InDetFlags.doMonitoringPrimaryVertexingEnhanced = False
InDetFlags.doMonitoringPixel     = False
InDetFlags.doMonitoringSCT       = False
InDetFlags.doMonitoringTRT       = False
InDetFlags.doMonitoringAlignment = False
InDetFlags.doPrintConfigurables  = True 
InDetFlags.doPseudoTracking         = True 
InDetFlags.doxAOD = True

from InDetRecExample.InDetKeys import InDetKeys
InDetKeys.trkValidationNtupleName = 'TrkValidation.root'
InDetKeys.trkD3PDFileName = 'InDetTrackD3PD.root'

StoreGateSvc = Service("StoreGateSvc")
StoreGateSvc.Dump = True

from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTool
ToolSvc += PixelRecoDbTool()
ToolSvc.PixelRecoDbTool.InputSource = 1

from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.enableSuppression = False
ServiceMgr.MessageSvc.Format       = "% F%50W%S%7W%R%T %0W%M"

OutputNameString = 'CheckSmearing'

if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
 
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.SystemOfUnits import *
import math

# load the tracking geometry service
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

topSequence = AlgSequence()

topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.CheckSmear=True
topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.OutputLevel=OutputLevel
#topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.pitch_X=10.0*micrometer
#topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.pitch_X=0.0*micrometer
#topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.pitch_Y=55.0*micrometer
#topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.pitch_Y=0.0*micrometer
topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.TrackingGeometrySvc = AtlasTrackingGeometrySvc
topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.UseCustomGeometry = True
topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.DetectorElementMapName="Pixel_IdHashDetElementMap"
topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.MergeClusters = False
topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.PlanarClusterContainerName="Pixel_PlanarClusters"
topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.PRD_TruthPlanarContainerName="PRD_MultiTruthPlanarPixel"
#topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.Nsigma = 1.0

topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.CheckSmear=True
topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.SmearPixel = False
topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.OutputLevel=OutputLevel
#topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.pitch_X= 70.0*micrometer
#topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.pitch_X= 0.0*micrometer
topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.TrackingGeometrySvc = AtlasTrackingGeometrySvc
topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.UseCustomGeometry = True
topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.DetectorElementMapName="SCT_IdHashDetElementMap"
topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.MergeClusters = False
topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.PlanarClusterContainerName="SCT_PlanarClusters"
topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.PRD_TruthPlanarContainerName="PRD_MultiTruthPlanarSCT"

AlgSequence("Streams").StreamRDO.ItemList += ["iFatras::PlanarClusterContainer#Pixel_PlanarClusters"]
##AlgSequence("Streams").StreamRDO.ItemList += ["InDet::SiClusterContainer#PixelClusters"]
AlgSequence("Streams").StreamRDO.ItemList += ["iFatras::PlanarClusterContainer#SCT_PlanarClusters"]
##AlgSequence("Streams").StreamRDO.ItemList += ["InDet::SiClusterContainer#SCT_Clusters"]

AlgSequence("Streams").StreamRDO.ItemList += ["PRD_MultiTruthCollection#PRD_MultiTruthPlanarPixel"]
AlgSequence("Streams").StreamRDO.ItemList += ["PRD_MultiTruthCollection#PRD_MultiTruthPlanarSCT"]
##AlgSequence("Streams").StreamRDO.ItemList += ["PRD_MultiTruthCollection#PRD_MultiTruthPixel"]
##AlgSequence("Streams").StreamRDO.ItemList += ["PRD_MultiTruthCollection#PRD_MultiTruthSCT"]

#from SiSpacePointTool.SiSpacePointToolConf import InDet__SiSpacePointMakerTool
#InDetSiSpacePointMakerTool = InDet__SiSpacePointMakerTool(name = "InDetSiSpacePointMakerTool")

#InDetSiSpacePointMakerTool.OutputLevel = VERBOSE
#ToolSvc += InDetSiSpacePointMakerTool
#print InDetSiSpacePointMakerTool

#from ISF_FatrasRecoTools.ISF_FatrasRecoToolsConf import iFatras__ISF_SiSpacePointMakerTool
#ISFSiSpacePointMakerTool = iFatras__ISF_SiSpacePointMakerTool(name = "ISF_SiSpacePointMakerTool")

#ISFSiSpacePointMakerTool.OutputLevel = VERBOSE
#ToolSvc += ISFSiSpacePointMakerTool
#print ISFSiSpacePointMakerTool
   
##
## SiTrackerSpacePointFinder algorithm
##
#from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiTrackerSpacePointFinder
#InDetSiTrackerSpacePointFinder = InDet__SiTrackerSpacePointFinder(name                   = "InDetSiTrackerSpacePointFinder",
#                                                                  SiSpacePointMakerTool  = InDetSiSpacePointMakerTool,
#                                                                  ISF_SiSpacePointMakerTool = ISFSiSpacePointMakerTool,
#                                                                  PixelsClustersName     = InDetKeys.PixelClusters(),                                                                                                         SCT_ClustersName       = InDetKeys.SCT_Clusters(),
#                                                                  SpacePointsPixelName   = InDetKeys.PixelSpacePoints(),
#                                                                  SpacePointsSCTName     = InDetKeys.SCT_SpacePoints(),
#                                                                  SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
#                                                                  ProcessPixels          = True,
#                                                                  ProcessSCTs            = True,
#                                                                  ProcessOverlaps        = DetFlags.haveRIO.SCT_on())

#InDetSiTrackerSpacePointFinder.OutputLevel = VERBOSE

#topSequence += InDetSiTrackerSpacePointFinder
#print InDetSiTrackerSpacePointFinder

#AlgSequence("Streams").StreamRDO.ItemList += ["SpacePointContainer#PixelSpacePoints"]
#AlgSequence("Streams").StreamRDO.ItemList += ["SpacePointContainer#SCT_SpacePoints"]

from ISF_FatrasRecoTools.ISF_FatrasRecoToolsConf import iFatras__ISF_PRDGeneralTruthTrajectorySorterID
mysort=iFatras__ISF_PRDGeneralTruthTrajectorySorterID(name="ISFGeneralTruthTrajectorySorter")
mysort.OutputLevel = OutputLevel
ToolSvc+=mysort
print mysort

from ISF_FatrasRecoTools.ISF_FatrasRecoToolsConf import iFatras__PlanarClusterProvider
PlanarClusterProvider=iFatras__PlanarClusterProvider(name = "PlanarClusterProvider",
                                                     PlanarClusterContainers=["Pixel_PlanarClusters", "SCT_PlanarClusters"])
ToolSvc+=PlanarClusterProvider
PlanarClusterProvider.OutputLevel = OutputLevel
print PlanarClusterProvider

from ISF_FatrasRecoTools.ISF_FatrasRecoToolsConf import iFatras__PlanarClusterOnTrackTool
PixelPlanarClusterOnTrackTool = iFatras__PlanarClusterOnTrackTool("PixelPlanarClusterOnTrackTool")
PixelPlanarClusterOnTrackTool.OutputLevel = OutputLevel
ToolSvc += PixelPlanarClusterOnTrackTool
print PixelPlanarClusterOnTrackTool

SCT_PlanarClusterOnTrackTool = iFatras__PlanarClusterOnTrackTool("SCT_PlanarClusterOnTrackTool")
SCT_PlanarClusterOnTrackTool.OutputLevel = OutputLevel
ToolSvc += SCT_PlanarClusterOnTrackTool
print SCT_PlanarClusterOnTrackTool

from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
InDetRotCreator = Trk__RIO_OnTrackCreator(name                = 'InDetRotCreator',
                                          ToolPixelCluster    = PixelPlanarClusterOnTrackTool,
                                          ToolSCT_Cluster     = SCT_PlanarClusterOnTrackTool,
                                          ToolMuonCluster     = None,
                                          ToolMuonDriftCircle = None,
                                          ToolMuonMMCluster   = None,       
                                          ToolTRT_DriftCircle = None,    
                                          Mode                = 'indet')
InDetRotCreator.OutputLevel = OutputLevel
ToolSvc += InDetRotCreator

from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
BroadInDetRotCreator = Trk__RIO_OnTrackCreator(name                = 'BroadInDetRotCreator',
                                               ToolPixelCluster    = PixelPlanarClusterOnTrackTool,
                                               ToolSCT_Cluster     = SCT_PlanarClusterOnTrackTool,
                                               ToolMuonCluster     = None,
                                               ToolMuonDriftCircle = None,
                                               ToolMuonMMCluster   = None,       
                                               ToolTRT_DriftCircle = None,    
                                               Mode                = 'indet')
BroadInDetRotCreator.OutputLevel = OutputLevel
ToolSvc += BroadInDetRotCreator

print InDetRotCreator
print BroadInDetRotCreator

ScaleHitUncertainty  = 2.5
InDetRefitRotCreator = InDetRotCreator

from TrkMeasurementUpdator.TrkMeasurementUpdatorConf import Trk__KalmanUpdator
InDetUpdator = Trk__KalmanUpdator(name = 'InDetUpdator')
InDetUpdator.OutputLevel = OutputLevel
ToolSvc += InDetUpdator
print InDetUpdator

#
# get propagator
#

from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator

InDetPropagator = Propagator(name = 'InDetPropagator')

ToolSvc += InDetPropagator
print      InDetPropagator

#
# Setup the Navigator (default, could be removed)
#

from TrkExTools.TrkExToolsConf import Trk__Navigator
InDetNavigator = Trk__Navigator(name                = 'InDetNavigator',
                                TrackingGeometrySvc = AtlasTrackingGeometrySvc)
ToolSvc += InDetNavigator
print      InDetNavigator

#
# Setup the MaterialEffectsUpdator
#

from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
InDetMaterialUpdator = Trk__MaterialEffectsUpdator(name = "InDetMaterialEffectsUpdator")
if not InDetFlags.solenoidOn():
    InDetMaterialUpdator.EnergyLoss          = False
    InDetMaterialUpdator.ForceMomentum       = True
    InDetMaterialUpdator.ForcedMomentumValue = 1000*MeV

ToolSvc += InDetMaterialUpdator
print InDetMaterialUpdator

# CONFIGURE PROPAGATORS/UPDATORS ACCORDING TO GEOMETRY SIGNATURE
      
InDetSubPropagators = []
InDetSubUpdators    = []

# -------------------- set it depending on the geometry ----------------------------------------------------
# default for ID is (Rk,Mat)
InDetSubPropagators += [ InDetPropagator.name() ]
InDetSubUpdators    += [ InDetMaterialUpdator.name() ]

# default for Calo is (Rk,MatLandau)
InDetSubPropagators += [ InDetPropagator.name() ]
InDetSubUpdators    += [ InDetMaterialUpdator.name() ]
        
# default for MS is (STEP,Mat)
#InDetSubPropagators += [ InDetStepPropagator.name() ]
InDetSubUpdators    += [ InDetMaterialUpdator.name() ]
# ----------------------------------------------------------------------------------------------------------            

#
# set up extrapolator
#
from TrkExTools.TrkExToolsConf import Trk__Extrapolator
InDetExtrapolator = Trk__Extrapolator(name                    = 'InDetExtrapolator',
                                      Propagators             = [ InDetPropagator ], # [ InDetPropagator, InDetStepPropagator ],
                                      MaterialEffectsUpdators = [ InDetMaterialUpdator ],
                                      Navigator               = InDetNavigator,
                                      SubPropagators          = InDetSubPropagators,
                                      SubMEUpdators           = InDetSubUpdators)
ToolSvc += InDetExtrapolator
print      InDetExtrapolator  

from TrkExEngine.AtlasExtrapolationEngine import AtlasExtrapolationEngine
ATLAS_FatrasExEngine = AtlasExtrapolationEngine(name='Extrapolation', nameprefix='Atlas', ToolOutputLevel=OutputLevel)
ToolSvc += ATLAS_FatrasExEngine
print ATLAS_FatrasExEngine
ATLAS_FatrasExEngine.OutputLevel = OutputLevel

from TrkKalmanFitter.TrkKalmanFitterConf import Trk__ForwardKalmanFitter as PublicFKF
InDetFKF = PublicFKF(name                   = 'InDetFKF',
                     StateChi2PerNDFPreCut  = 30.0,
                     ExtrapolationEngine    = ATLAS_FatrasExEngine,
                     UseExtrapolationEngine = True)   
InDetFKF.OutputLevel = OutputLevel
ToolSvc += InDetFKF
print InDetFKF

from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanSmoother as PublicBKS
InDetBKS = PublicBKS(name                        = 'InDetBKS',
                     InitialCovarianceSeedFactor = 200.,
                     ExtrapolationEngine         = ATLAS_FatrasExEngine,
                     UseExtrapolationEngine      = True)
InDetBKS.OutputLevel = OutputLevel
ToolSvc += InDetBKS
print InDetBKS

from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanOutlierLogic as PublicKOL
InDetKOL = PublicKOL(name                    = 'InDetKOL',
                     TrackChi2PerNDFCut      = 17.0,
                     StateChi2PerNDFCut      = 12.5)
InDetKOL.OutputLevel = OutputLevel
ToolSvc += InDetKOL
print InDetKOL

InDetDNAdjustor   = None
InDetDNASeparator = None

from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanFitter as ConfiguredKalmanFitter
InDetTrackFitter = ConfiguredKalmanFitter(name                           = 'InDetTrackFitter',
                                          ExtrapolatorHandle             = InDetExtrapolator,
                                          RIO_OnTrackCreatorHandle       = InDetRotCreator,
                                          MeasurementUpdatorHandle       = InDetUpdator,
                                          ForwardKalmanFitterHandle      = InDetFKF,
                                          KalmanSmootherHandle           = InDetBKS,
                                          KalmanOutlierLogicHandle       = InDetKOL,
                                          DynamicNoiseAdjustorHandle     = InDetDNAdjustor,
                                          BrempointAnalyserHandle        = InDetDNASeparator,
                                          AlignableSurfaceProviderHandle = None,
                                          RecalibratorHandle             = None,                                          
                                          InternalDAFHandle              = None)
InDetTrackFitter.OutputLevel = OutputLevel #VERBOSE
ToolSvc += InDetTrackFitter
print InDetTrackFitter

from TrkTruthTrackTools.TrkTruthTrackToolsConf import Trk__TruthTrackBuilder
InDetTruthTrackBuilder = Trk__TruthTrackBuilder(name                = 'InDetTruthTrackBuilder',
                                                TrackFitter         = InDetTrackFitter,
                                                ExtrapolationTool   = InDetExtrapolator,
                                                RotCreatorTool      = InDetRotCreator,
                                                BroadRotCreatorTool = BroadInDetRotCreator,
                                                MinDegreesOfFreedom = 1)
InDetTruthTrackBuilder.OutputLevel = OutputLevel #VERBOSE
ToolSvc += InDetTruthTrackBuilder
print InDetTruthTrackBuilder


PRD_PlanarMultiTruthColl = []
PRD_PlanarMultiTruthColl += ["PRD_MultiTruthPlanarPixel","PRD_MultiTruthPlanarSCT"]

from ISF_FatrasRecoTools.ISF_FatrasRecoToolsConf import iFatras__PRD_PlanarTruthTrajectoryBuilder
# change input collection key if taking only pileup
ISF_PRD_TruthTrajectoryBuilder=iFatras__PRD_PlanarTruthTrajectoryBuilder(name = 'ISF_PRD_TruthTrajectoryBuilder',
                                                                         PRD_PlanarMultiTruthCollections = PRD_PlanarMultiTruthColl,
                                                                         InDetPRD_PlanarClusterProvider  = PlanarClusterProvider,
                                                                         MinimumPt                       = 400,
                                                                         PRD_TruthTrajectoryManipulators = [mysort])

ISF_PRD_TruthTrajectoryBuilder.OutputLevel = OutputLevel #VERBOSE
ToolSvc+=ISF_PRD_TruthTrajectoryBuilder
print ISF_PRD_TruthTrajectoryBuilder

def getIFS_PRDtoTrackMapTool(**kwargs) :
    tool_name = 'ISF_PRDtoTrackMapTool'
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,tool_name) :
        return getattr(ToolSvc,tool_name)

    from ISF_FatrasRecoTools.ISF_FatrasRecoToolsConf import iFatras__ISF_PRDtoTrackMapTool
    tool=iFatras__ISF_PRDtoTrackMapTool( name=tool_name, **kwargs)
    ToolSvc += tool
    return tool


from ISF_FatrasRecoTools.ISF_FatrasRecoToolsConf import iFatras__ISF_TrackSummaryHelperTool
ISF_TrackSummaryHelperTool = iFatras__ISF_TrackSummaryHelperTool(name         = "ISF_TrackSummaryHelperTool",
                                                                  AssoTool     = "",
                                                                  DoSharedHits = True)
ISF_TrackSummaryHelperTool.OutputLevel = OutputLevel #VERBOSE
ToolSvc += ISF_TrackSummaryHelperTool
print ISF_TrackSummaryHelperTool

# @TODO does th  ISF_TrackSummaryHelperTool do hole search and if so is that intedned ?
from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
InDetTrackSummaryTool = Trk__TrackSummaryTool(name = "InDetTrackSummaryTool",
                                              doSharedHits            = True,
                                              InDetSummaryHelperTool  = ISF_TrackSummaryHelperTool,
                                              AddDetailedInDetSummary = False,
                                              doHolesInDet            = False,
                                              doHolesMuon             = False,
                                              MuonSummaryHelperTool   = "",
                                              AddDetailedMuonSummary  = False,
                                              TRT_ElectronPidTool     = "",
                                              PixelToTPIDTool         = "",
                                              PixelExists             = True)
InDetTrackSummaryTool.OutputLevel = OutputLevel #VERBOSE
ToolSvc += InDetTrackSummaryTool
print InDetTrackSummaryTool

# --- the truth track creation algorithm
from InDetRecExample.TrackingCommon import getInDetPRDtoTrackMapToolGangedPixels
from TrkTruthTrackAlgs.TrkTruthTrackAlgsConf import Trk__TruthTrackCreation
InDetTruthTrackCreation = Trk__TruthTrackCreation(name = 'InDetTruthTrackCreation',
                                                  PRD_TruthTrajectoryBuilder = ISF_PRD_TruthTrajectoryBuilder,
                                                  TruthTrackBuilder          = InDetTruthTrackBuilder,
                                                  OutputTrackCollection      = InDetKeys.PseudoTracks(),
                                                  AssociationTool            = getIFS_PRDtoTrackMapTool(),
                                                  TrackSummaryTool           = InDetTrackSummaryTool)
InDetTruthTrackCreation.OutputLevel = OutputLevel #VERBOSE
topSequence += InDetTruthTrackCreation
print InDetTruthTrackCreation

if TrkDetFlags.ISF_FatrasCustomGeometry():
    InDetKeys.PixelClustersTruth = "PRD_MultiTruthPlanarPixel"
    InDetKeys.SCT_ClustersTruth = "PRD_MultiTruthPlanarSCT"
InDetFlags.doPhysValMon = True
InDetRecTestBLayerTool=None

InDetTrackSummaryToolSharedHits=InDetTrackSummaryTool

InDetKeys.UnslimmedTracks          = InDetKeys.PseudoTracks()
InDetKeys.UnslimmedTracksTruth     = InDetKeys.PseudoTracksTruth()

InputTrackCollection = InDetKeys.PseudoTracks()

include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
InDetTracksTruth = ConfiguredInDetTrackTruth(InDetKeys.PseudoTracks(),
                                             InDetKeys.PseudoDetailedTracksTruth(),
                                             InDetKeys.PseudoTracksTruth())                                             
InDetTracksTruth.OutputLevel = OutputLevel #VERBOSE

from TrkTruthToTrack.TrkTruthToTrackConf import Trk__TruthToTrack
InDetTruthToTrack  = Trk__TruthToTrack(name         = "InDetTruthToTrack",
                                       Extrapolator = InDetExtrapolator)
                                       
ToolSvc += InDetTruthToTrack
InDetTruthToTrack.OutputLevel = OutputLevel #VERBOSE

# track collection keys for statistics
if not 'TrackCollectionKeys' in dir():
    TrackCollectionKeys        = []
if not 'TrackCollectionTruthKeys' in dir():
    TrackCollectionTruthKeys   = []

TrackCollectionKeys      += [ InDetKeys.PseudoTracks() ]
TrackCollectionTruthKeys += [ InDetKeys.PseudoTracksTruth() ]

from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
xAODTruthCnvAlg = xAODMaker__xAODTruthCnvAlg( name                   = "xAODTruthCnvAlg",
                                              AODContainerName       = "TruthEvent",
                                              WriteInTimePileUpTruth = False,
                                              WriteAllPileUpTruth    = False)
xAODTruthCnvAlg.OutputLevel = OutputLevel #VERBOSE  
topSequence += xAODTruthCnvAlg

print xAODTruthCnvAlg

# register internally used track summary tool
from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
AtlasTrackSummaryTool = AtlasTrackSummaryTool()
ToolSvc += AtlasTrackSummaryTool

include("InDetRecExample/InDetxAODCreator.py") 

public_InDetxAODParticleCreatorTool = getInDetxAODParticleCreatorTool()
public_InDetxAODParticleCreatorTool.TrackSummaryTool = InDetTrackSummaryTool
public_InDetxAODParticleCreatorTool.OutputLevel = OutputLevel #VERBOSE
print public_InDetxAODParticleCreatorTool

# adjust track particle creators
add_truth_links=[False,True]
idx=0
for alg_name in ['InDetTrackParticles',InDetKeys.xAODPseudoTrackParticleContainer()] :
  if hasattr(topSequence,alg_name) :
    alg=getattr(topSequence,alg_name)
    alg.OutputLevel = OutputLevel # VERBOSE
    if hasattr(alg,"TrackParticleCreator") :
        alg.TrackParticleCreator.TrackSummaryTool = InDetTrackSummaryTool
        alg.TrackParticleCreator.OutputLevel = OutputLevel # VERBOSE
        if add_truth_links[idx] :
            alg.AddTruthLink = InDetFlags.doTruth()
    else :
        print >>sys.stderr,'WARNING the %s algorithm does not contain the expected TrackParticleCreator property.' % alg_name
    print alg
  else :
    import sys
    print >>sys.stderr,'WARNING top sequence does not contain the expected %s algorithm' % alg_name
  idx += 1

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolStream( "StreamAOD", "xAOD.pool.root" )
#xaodStream.AddItem( "#*" )
xaodStream.AddItem("xAOD::AuxContainerBase#InDetPseudoTrackParticlesAux.")
xaodStream.AddItem("xAOD::IParticleContainer#InDetPseudoTrackParticles")
xaodStream.AddItem("xAOD::TrackParticleContainer#InDetPseudoTrackParticles")
xaodStream.AddItem("xAOD::TrackParticleAuxContainer#InDetPseudoTrackParticlesAux.")
xaodStream.AddItem( "xAOD::TruthEventContainer#TruthEvents" )
xaodStream.AddItem( "xAOD::TruthEventAuxContainer#TruthEventsAux." )
xaodStream.AddItem( "xAOD::TruthParticleContainer#TruthParticles" ) 
xaodStream.AddItem( "xAOD::TruthParticleAuxContainer#TruthParticlesAux." )

xaodStream.Print()
xaodStream.OutputLevel = OutputLevel #VERBOSE

if doVP1:
    from VP1Algs.VP1AlgsConf import VP1Alg 
    topSequence += VP1Alg()
