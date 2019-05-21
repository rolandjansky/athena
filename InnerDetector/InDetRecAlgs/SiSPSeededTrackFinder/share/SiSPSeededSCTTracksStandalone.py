###############################################################
#
# Standalone job options file to create SiSPSeededSCTTracks
#
#==============================================================

doPrint = False

#--------------------------------------------------------------
# Standard includes
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixStandardJob

#--------------------------------------------------------------
# Thread-specific setup
#--------------------------------------------------------------
from AthenaCommon.ConcurrencyFlags import jobproperties
numThreads = jobproperties.ConcurrencyFlags.NumThreads()
if numThreads > 0:
    from AthenaCommon.AlgScheduler import AlgScheduler
    AlgScheduler.CheckDependencies( True )
    AlgScheduler.ShowControlFlow( True )
    AlgScheduler.ShowDataDependencies( True )

#--------------------------------------------------------------
# use auditors
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import AuditorSvc
ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
theApp.AuditAlgorithms=True

#--------------------------------------------------------------
# Load Geometry
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion="ATLAS-R2-2016-01-00-01"
globalflags.DetGeo="atlas"
globalflags.InputFormat="bytestream"
globalflags.DataSource="data"
if doPrint:
    print globalflags

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------
# --- switch on InnerDetector
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.SCT_setOn()
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.SCT_setOn()

# ---- switch parts of ID off/on as follows
#switch off tasks
DetFlags.pileup.all_setOff()
DetFlags.simulate.all_setOff()
DetFlags.makeRIO.all_setOff()
DetFlags.makeRIO.SCT_setOn()
DetFlags.writeBS.all_setOff()
DetFlags.readRDOBS.all_setOff()
DetFlags.readRDOBS.SCT_setOn()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.writeRIOPool.all_setOff()

if doPrint:
    DetFlags.Print()

import AtlasGeoModel.SetGeometryVersion
import AtlasGeoModel.GeoModelInit
import MagFieldServices.SetupField

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
conddb.dbdata="CONDBR2"
IOVDbSvc.GlobalTag="CONDBR2-BLKPA-2018-03"
IOVDbSvc.OutputLevel = WARNING

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# EventInfo setup
if not hasattr(topSequence, "xAODMaker::EventInfoCnvAlg"):
    from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
    topSequence +=xAODMaker__EventInfoCnvAlg()

# Set input byte stream file (from q431 test)
inputBSFiles = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data17_13TeV.00330470.physics_Main.daq.RAW._lb0310._SFO-1._0001.data"]
include("ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py")
ServiceMgr.ByteStreamInputSvc.FullFileName = inputBSFiles
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.FilesInput = inputBSFiles

# Set up byte stream converters (SCTRawDataProvider, SCTRawDataProviderTool, SCT_RodDecoder, SCTEventFlagWriter)
include("InDetRecExample/InDetReadBS_jobOptions.py")
topSequence.InDetSCTRawDataProvider.OutputLevel = INFO
topSequence.InDetSCTRawDataProvider.ProviderTool.Decoder.OutputLevel = INFO
topSequence.InDetSCTEventFlagWriter.OutputLevel = INFO
if numThreads >= 2:
    topSequence.InDetSCTRawDataProvider.Cardinality = numThreads
    topSequence.InDetSCTEventFlagWriter.Cardinality = numThreads

# Set up SCT clsutering
from SiLorentzAngleTool.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup
sctLorentzAngleToolSetup = SCTLorentzAngleToolSetup()
from SiClusterizationTool.SiClusterizationToolConf import InDet__ClusterMakerTool
InDetClusterMakerTool = InDet__ClusterMakerTool(name = "InDetClusterMakerTool",
                                                PixelCablingSvc = None,
                                                PixelModuleData = "",
                                                PixelChargeCalibCondData = "",
                                                PixelLorentzAngleTool = None,
                                                SCTLorentzAngleTool = sctLorentzAngleToolSetup.SCTLorentzAngleTool)

# SCT conditions setups
from SCT_ConditionsTools.SCT_ConfigurationConditionsToolSetup import SCT_ConfigurationConditionsToolSetup
sct_ConfigurationConditionsToolSetup = SCT_ConfigurationConditionsToolSetup()
sct_ConfigurationConditionsToolSetup.setup()

from SCT_ConditionsTools.SCT_ByteStreamErrorsToolSetup import SCT_ByteStreamErrorsToolSetup
sct_ByteStreamErrorsToolSetup = SCT_ByteStreamErrorsToolSetup()
sct_ByteStreamErrorsToolSetup.setConfigTool(sct_ConfigurationConditionsToolSetup.getTool())
sct_ByteStreamErrorsToolSetup.setup()

from SCT_ConditionsTools.SCT_ReadCalibDataToolSetup import SCT_ReadCalibDataToolSetup
sct_ReadCalibDataToolSetup = SCT_ReadCalibDataToolSetup()
sct_ReadCalibDataToolSetup.setup()

from SCT_ConditionsTools.SCT_TdaqEnabledToolSetup import SCT_TdaqEnabledToolSetup
sct_TdaqEnabledToolSetup = SCT_TdaqEnabledToolSetup()
sct_TdaqEnabledToolSetup.setFolder('/TDAQ/Resources/ATLAS/SCT/Robins')
sct_TdaqEnabledToolSetup.setup()

from SCT_ConditionsTools.SCT_DCSConditionsToolSetup import SCT_DCSConditionsToolSetup
sct_DCSConditionsToolSetup = SCT_DCSConditionsToolSetup()
sct_DCSConditionsToolSetup.setup()

from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
sct_ConditionsSummaryToolSetupWithoutFlagged = SCT_ConditionsSummaryToolSetup("InDetSCT_ConditionsSummaryToolWithoutFlagged")
sct_ConditionsSummaryToolSetupWithoutFlagged.setup()
sct_ConditionsSummaryToolSetupWithoutFlagged.ConditionsTools=[sct_ConfigurationConditionsToolSetup.getTool().getFullName(),
                                                              sct_ByteStreamErrorsToolSetup.getTool().getFullName(),
                                                              sct_ReadCalibDataToolSetup.getTool().getFullName(),
                                                              sct_TdaqEnabledToolSetup.getTool().getFullName(),
                                                              sct_DCSConditionsToolSetup.getTool().getFullName()]

from SiClusterizationTool.SiClusterizationToolConf import InDet__SCT_ClusteringTool
InDetSCT_ClusteringTool = InDet__SCT_ClusteringTool(name = "InDetSCT_ClusteringTool",
                                                    OutputLevel = INFO,
                                                    globalPosAlg = InDetClusterMakerTool,
                                                    timeBins = "01X",
                                                    conditionsTool = sct_ConditionsSummaryToolSetupWithoutFlagged.getTool())
from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__SCT_Clusterization
InDetSCT_Clusterization = InDet__SCT_Clusterization(name = "InDetSCT_Clusterization",
                                                    OutputLevel = INFO,
                                                    clusteringTool = InDetSCT_ClusteringTool,
                                                    conditionsTool = sct_ConditionsSummaryToolSetupWithoutFlagged.getTool(),
                                                    DataObjectName = "SCT_RDOs",
                                                    ClustersName = "SCT_Clusters",
                                                    maxFiredStrips = 384)
if numThreads >= 2:
    InDetSCT_Clusterization.Cardinality = numThreads

topSequence += InDetSCT_Clusterization

# Set up SCT_ConditionsSummaryTestAlg with SCT_FlaggedConditionTool
from SCT_ConditionsTools.SCT_FlaggedConditionToolSetup import SCT_FlaggedConditionToolSetup
sct_FlaggedConditionToolSetup = SCT_FlaggedConditionToolSetup()
sct_FlaggedConditionToolSetup.setup()
sct_ConditionsSummaryToolSetup = SCT_ConditionsSummaryToolSetup()
sct_ConditionsSummaryToolSetup.setup()
SCT_ConditionsSummaryTool = sct_ConditionsSummaryToolSetup.getTool()
SCT_ConditionsSummaryTool.ConditionsTools=[sct_ConfigurationConditionsToolSetup.getTool().getFullName(),
                                           sct_FlaggedConditionToolSetup.getTool().getFullName(),
                                           sct_ByteStreamErrorsToolSetup.getTool().getFullName(),
                                           sct_ReadCalibDataToolSetup.getTool().getFullName(),
                                           sct_TdaqEnabledToolSetup.getTool().getFullName(),
                                           sct_DCSConditionsToolSetup.getTool().getFullName()]

# Set up space point formation
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
if not hasattr(condSeq, "InDetSiElementPropertiesTableCondAlg"):
    from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiElementPropertiesTableCondAlg
    condSeq += InDet__SiElementPropertiesTableCondAlg(name = "InDetSiElementPropertiesTableCondAlg")
from SiSpacePointTool.SiSpacePointToolConf import InDet__SiSpacePointMakerTool
InDetSiSpacePointMakerTool = InDet__SiSpacePointMakerTool(name = "InDetSiSpacePointMakerTool")
from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiTrackerSpacePointFinder
InDetSiTrackerSpacePointFinder = InDet__SiTrackerSpacePointFinder(name                   = "InDetSiTrackerSpacePointFinder",
                                                                  SiSpacePointMakerTool  = InDetSiSpacePointMakerTool,
                                                                  PixelsClustersName     = "",
                                                                  SCT_ClustersName       = "SCT_Clusters",
                                                                  SpacePointsPixelName   = "",
                                                                  SpacePointsSCTName     = "SCT_SpacePoints",
                                                                  SpacePointsOverlapName = "OverlapSpacePoints",
                                                                  ProcessPixels          = False,
                                                                  ProcessSCTs            = True,
                                                                  ProcessOverlaps        = False,
                                                                  OverrideBeamSpot       = True)
if numThreads >= 2:
    InDetSiTrackerSpacePointFinder.Cardinality = numThreads
topSequence += InDetSiTrackerSpacePointFinder

# Setup SiSPSeededTrackFinder
if not hasattr(condSeq, "InDetSiDetElementBoundaryLinksCondAlg"):
    from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiDetElementBoundaryLinksCondAlg_xk
    condSeq += InDet__SiDetElementBoundaryLinksCondAlg_xk(name = "InDetSiDetElementBoundaryLinksCondAlg")

from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
NewTrackingCuts = ConfiguredNewTrackingCuts("SCT")

from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_ATLxk as SiSpacePointsSeedMaker
InDetSiSpacePointsSeedMaker = SiSpacePointsSeedMaker(name                   = "InDetSpSeedsMaker"+NewTrackingCuts.extension(),
                                                     pTmin                  = NewTrackingCuts.minPT(),
                                                     maxdImpact             = NewTrackingCuts.maxPrimaryImpact(),
                                                     maxZ                   = NewTrackingCuts.maxZImpact(),
                                                     minZ                   = -NewTrackingCuts.maxZImpact(),
                                                     usePixel               = False,
                                                     SpacePointsPixelName   = "",
                                                     useSCT                 = (NewTrackingCuts.useSCT() and NewTrackingCuts.useSCTSeeding()),
                                                     SpacePointsSCTName     = "SCT_SpacePoints",
                                                     useOverlapSpCollection = True,
                                                     SpacePointsOverlapName = "OverlapSpacePoints",
                                                     radMax                 = NewTrackingCuts.radMax(),
                                                     RapidityCut            = NewTrackingCuts.maxEta(),
                                                     UseAssociationTool     = False,
                                                     AssociationTool        = None,
                                                     BeamSpotKey            = "")

from SiZvertexTool_xk.SiZvertexTool_xkConf import InDet__SiZvertexMaker_xk
InDetZvertexMaker = InDet__SiZvertexMaker_xk(name          = 'InDetZvertexMaker'+NewTrackingCuts.extension(),
                                             Zmax          = NewTrackingCuts.maxZImpact(),
                                             Zmin          = -NewTrackingCuts.maxZImpact(),
                                             minRatio      = 0.17,
                                             SeedMakerTool = InDetSiSpacePointsSeedMaker)

from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
InDetPatternPropagator = Propagator(name = 'InDetPatternPropagator')
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += InDetPatternPropagator

if not hasattr(condSeq, "InDet__SiDetElementsRoadCondAlg_xk"):
    from SiDetElementsRoadTool_xk.SiDetElementsRoadTool_xkConf import InDet__SiDetElementsRoadCondAlg_xk
    useDynamicAlignFolders = False
    condSeq += InDet__SiDetElementsRoadCondAlg_xk(name = "InDet__SiDetElementsRoadCondAlg_xk",
                                                  UseDynamicAlignFolders = useDynamicAlignFolders,
                                                  usePixel = False)

from SiDetElementsRoadTool_xk.SiDetElementsRoadTool_xkConf import InDet__SiDetElementsRoadMaker_xk
InDetSiDetElementsRoadMaker = InDet__SiDetElementsRoadMaker_xk(name               = 'InDetSiRoadMaker'+NewTrackingCuts.extension(),
                                                               PropagatorTool     = InDetPatternPropagator,
                                                               usePixel           = False,
                                                               useSCT             = NewTrackingCuts.useSCT(), 
                                                               SCTManagerLocation = "SCT",
                                                               RoadWidth          = NewTrackingCuts.RoadWidth())

from TrkMeasurementUpdator_xk.TrkMeasurementUpdator_xkConf import Trk__KalmanUpdator_xk
InDetPatternUpdator = Trk__KalmanUpdator_xk(name = 'InDetPatternUpdator')
ToolSvc += InDetPatternUpdator

from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__SCT_ClusterOnTrackTool
SCT_ClusterOnTrackTool = InDet__SCT_ClusterOnTrackTool("InDetSCT_ClusterOnTrackTool",
                                                       CorrectionStrategy = 0,  # do correct position bias
                                                       ErrorStrategy      = 2,  # do use phi dependent errors
                                                       LorentzAngleTool   = sctLorentzAngleToolSetup.SCTLorentzAngleTool)
from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
InDetRotCreator = Trk__RIO_OnTrackCreator(name             = 'InDetRotCreator',
                                          ToolPixelCluster = None,
                                          ToolSCT_Cluster  = SCT_ClusterOnTrackTool,
                                          ToolTRT_DriftCircle = None,
                                          Mode             = 'indet')
ToolSvc += InDetRotCreator

from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiCombinatorialTrackFinder_xk
InDetSiComTrackFinder = InDet__SiCombinatorialTrackFinder_xk(name                  = 'InDetSiComTrackFinder',
                                                             PropagatorTool        = InDetPatternPropagator,
                                                             UpdatorTool           = InDetPatternUpdator,
                                                             RIOonTrackTool        = InDetRotCreator,
                                                             SctSummaryTool        = SCT_ConditionsSummaryTool,
                                                             AssosiationTool       = None,
                                                             usePixel              = False,
                                                             PixelClusterContainer = "",
                                                             useSCT                = True,
                                                             SCTManagerLocation    = "SCT",
                                                             SCT_ClusterContainer  = "SCT_Clusters")
ToolSvc += InDetSiComTrackFinder

from SiTrackMakerTool_xk.SiTrackMakerTool_xkConf import InDet__SiTrackMaker_xk as SiTrackMaker
InDetSiTrackMaker = SiTrackMaker(name                      = 'InDetSiTrackMaker'+NewTrackingCuts.extension(),
                                 useSCT                    = NewTrackingCuts.useSCT(),
                                 usePixel                  = False,
                                 RoadTool                  = InDetSiDetElementsRoadMaker,
                                 CombinatorialTrackFinder  = InDetSiComTrackFinder,
                                 pTmin                     = NewTrackingCuts.minPT(),
                                 pTminBrem                 = NewTrackingCuts.minPTBrem(),
                                 pTminSSS                  = -1.,
                                 nClustersMin              = NewTrackingCuts.minClusters(),
                                 nHolesMax                 = NewTrackingCuts.nHolesMax(),
                                 nHolesGapMax              = NewTrackingCuts.nHolesGapMax(),
                                 SeedsFilterLevel          = NewTrackingCuts.seedFilterLevel(),
                                 Xi2max                    = NewTrackingCuts.Xi2max(),
                                 Xi2maxNoAdd               = NewTrackingCuts.Xi2maxNoAdd(),
                                 nWeightedClustersMin      = NewTrackingCuts.nWeightedClustersMin(),
                                 CosmicTrack               = InDetFlags.doCosmics(),
                                 Xi2maxMultiTracks         = NewTrackingCuts.Xi2max(), # was 3.
                                 useSSSseedsFilter         = True, 
                                 doMultiTracksProd         = True,
                                 useBremModel              = False,
                                 doCaloSeededBrem          = False,
                                 doHadCaloSeedSSS          = False,
                                 phiWidth                  = NewTrackingCuts.phiWidthBrem(),
                                 etaWidth                  = NewTrackingCuts.etaWidthBrem(),
                                 InputClusterContainerName = "",
                                 InputHadClusterContainerName = "",
                                 UseAssociationTool        = False,
                                 TrackPatternRecoInfo = "SiSPSeededFinder",
                                 BeamSpotKey = "")

from SiSPSeededTrackFinder.SiSPSeededTrackFinderConf import InDet__SiSPSeededTrackFinder
InDetSiSPSeededTrackFinder = InDet__SiSPSeededTrackFinder(name           = 'InDetSiSpTrackFinder'+NewTrackingCuts.extension(),
                                                          TrackTool      = InDetSiTrackMaker,
                                                          TracksLocation = "SiSPSeededSCTTracks",
                                                          SeedsTool      = InDetSiSpacePointsSeedMaker,
                                                          SpacePointsPixelName = "",
                                                          useZvertexTool = True,
                                                          ZvertexTool    = InDetZvertexMaker,
                                                          useNewStrategy = False,
                                                          useMBTSTimeDiff = False,
                                                          useZBoundFinding = NewTrackingCuts.doZBoundary(),
                                                          BeamSpotKey = "")
if doPrint:
    print InDetSiSPSeededTrackFinder
if numThreads >= 2:
    InDetSiSPSeededTrackFinder.Cardinality = numThreads
topSequence += InDetSiSPSeededTrackFinder

# Print algorithms
if doPrint:
    print topSequence

# Set the number of events to be processed
theApp.EvtMax = -1

#--------------------------------------------------------------
# Set output lvl (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = INFO
# Gaudi/GaudiKernel/GaudiKernel/Message.h
ServiceMgr.MessageSvc.Format = "% F%50W%S% F%3W%s%7W%R%T %0W%M"

if numThreads >= 2:
    from SCT_ConditionsAlgorithms.SCTCondAlgCardinality import sctCondAlgCardinality
    sctCondAlgCardinality.set(numThreads)
