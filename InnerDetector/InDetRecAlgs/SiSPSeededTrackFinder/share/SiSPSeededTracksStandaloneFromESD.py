# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

###############################################################
#
# Standalone job options file to create SiSPSeededTracks
#
#==============================================================

# Configuration flags
if not "doPixel" in dir():
    doPixel = True
if not "doSCT" in dir():
    doSCT = True
if not "doBeamSpot" in dir():
    doBeamSpot = True
if not "doPrint" in dir():
    doPrint = True
if not "doDump" in dir():
    doDump = False
if not "EvtMax" in dir():
    EvtMax = -1
if not "inputESDFiles" in dir():
    inputESDFiles = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"]

# Output track location
TracksLocation = "SiSPSeededTracks"
if doPixel and not doSCT:
    TracksLocation = "SiSPSeededPixelTracks"
if not doPixel and doSCT:
    TracksLocation = "SiSPSeededSCTTracks"

#--------------------------------------------------------------
# Standard includes
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixStandardJob

# Common fragments
import sys
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr
from InDetRecExample.InDetJobProperties import InDetFlags
from InDetRecExample.InDetKeys import InDetKeys

if not doBeamSpot:
    InDetFlags.useBeamConstraint.set_Value_and_Lock(False)
if not doPixel:
    InDetKeys.PixelClusters.set_Value_and_Lock("")
    InDetKeys.PixelSpacePoints.set_Value_and_Lock("")
    # InDetKeys.OverlapSpacePoints.set_Value_and_Lock("")
if not doSCT:
    InDetKeys.SCT_Clusters.set_Value_and_Lock("")
    InDetKeys.SCT_SpacePoints.set_Value_and_Lock("")
    # InDetKeys.OverlapSpacePoints.set_Value_and_Lock("")

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
globalflags.InputFormat="pool"
globalflags.DataSource="geant4"
if doPrint:
    print globalflags

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------
# --- switch on BField, bpipe, pixel and SCT
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
if doBeamSpot:
    DetFlags.bpipe_setOn()
if doPixel:
    DetFlags.pixel_setOn()
if doSCT:
    DetFlags.SCT_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.BField_setOn()

# ---- switch parts of ID off/on as follows
DetFlags.digitize.all_setOff()
DetFlags.geometry.all_setOff()
DetFlags.overlay.all_setOff()
DetFlags.pileup.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.simulate.all_setOff()
DetFlags.simulateLVL1.all_setOff()
DetFlags.writeBS.all_setOff()
DetFlags.writeRIOPool.all_setOff()

import AtlasGeoModel.SetGeometryVersion
import AtlasGeoModel.GeoModelInit
import MagFieldServices.SetupField

if doPrint:
    DetFlags.Print()

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
IOVDbSvc.GlobalTag="OFLCOND-MC16-SDR-20"
IOVDbSvc.OutputLevel = WARNING

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = inputESDFiles

import AthenaPoolCnvSvc.ReadAthenaPool
from PoolSvc.PoolSvcConf import PoolSvc
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr += PoolSvc()
PoolSvc = svcMgr.PoolSvc
EventSelector = svcMgr.EventSelector
EventSelector.InputCollections = inputESDFiles

# Set up topSequence and condSeq
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")

# Set up EventInfo
if not hasattr(topSequence, "xAODMaker::EventInfoCnvAlg") and not hasattr(condSeq, "xAODMaker::EventInfoCnvAlg"):
    from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
    topSequence += xAODMaker__EventInfoCnvAlg()

# Set up BeamSpot
if doBeamSpot:
    if not hasattr(condSeq, "BeamSpotCondAlg"):
        conddb.addFolderSplitOnline("INDET", "/Indet/Onl/Beampos", "/Indet/Beampos", className="AthenaAttributeList")
        from BeamSpotConditions.BeamSpotConditionsConf import BeamSpotCondAlg
        condSeq += BeamSpotCondAlg("BeamSpotCondAlg")

from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as geoFlags
do_runI = geoFlags.Run() not in ["RUN2", "RUN3"]
if do_runI:
    sys.exit("RUN1 is not supported. Bye.")

# Set up cabling
include("InDetRecExample/InDetRecCabling.py")

# Set up Pixel conditions
if doPixel:
    # Taken from InDetRecExample/share/InDetRecConditionsAccess.py
    if not hasattr(condSeq, "PixelConfigCondAlg"):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelConfigCondAlg
        condSeq += PixelConfigCondAlg(name="PixelConfigCondAlg")
        if not (conddb.folderRequested("/PIXEL/PixMapOverlay") or conddb.folderRequested("/PIXEL/Onl/PixMapOverlay")):
            conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/PixMapOverlay", "/PIXEL/PixMapOverlay", className="CondAttrListCollection")
    if not hasattr(condSeq, "PixelChargeCalibCondAlg"):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelChargeCalibCondAlg
        condSeq += PixelChargeCalibCondAlg(name="PixelChargeCalibCondAlg", ReadKey="/PIXEL/PixCalib")
        if not conddb.folderRequested("/PIXEL/PixCalib"):
            conddb.addFolderSplitOnline("PIXEL", "/PIXEL/Onl/PixCalib", "/PIXEL/PixCalib", className="CondAttrListCollection")
    if not hasattr(condSeq, "PixelOfflineCalibCondAlg"):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelOfflineCalibCondAlg
        condSeq += PixelOfflineCalibCondAlg(name="PixelOfflineCalibCondAlg", ReadKey="/PIXEL/PixReco",
                                            InputSource = 2)
        if not conddb.folderRequested("/PIXEL/PixReco"):
            conddb.addFolder("PIXEL_OFL", "/PIXEL/PixReco", className="DetCondCFloat")
    if not hasattr(ToolSvc, "PixelLorentzAngleTool"):
        from SiLorentzAngleTool.PixelLorentzAngleToolSetup import PixelLorentzAngleToolSetup
        pixelLorentzAngleToolSetup = PixelLorentzAngleToolSetup()
    # Takne from InDetRecExample/share/InDetRecLoadTools.py
    from InDetRecExample.TrackingCommon import createAndAddCondAlg,getPixelClusterNnCondAlg,getPixelClusterNnWithTrackCondAlg
    createAndAddCondAlg( getPixelClusterNnCondAlg,         "PixelClusterNnCondAlg",          GetInputsInfo = do_runI)
    createAndAddCondAlg( getPixelClusterNnWithTrackCondAlg,"PixelClusterNnWithTrackCondAlg", GetInputsInfo = do_runI)

# Set up SCT conditions
SCT_ConditionsSummaryTool = None
if doSCT:
    # Taken from InDetRecExample/share/InDetRecConditionsAccess.py
    from SCT_ConditionsTools.SCT_ConfigurationConditionsToolSetup import SCT_ConfigurationConditionsToolSetup
    sct_ConfigurationConditionsToolSetup = SCT_ConfigurationConditionsToolSetup()
    sct_ConfigurationConditionsToolSetup.setup()
    from SCT_ConditionsTools.SCT_ReadCalibDataToolSetup import SCT_ReadCalibDataToolSetup
    sct_ReadCalibDataToolSetup = SCT_ReadCalibDataToolSetup()
    sct_ReadCalibDataToolSetup.setup()
    from SCT_ConditionsTools.SCT_DCSConditionsToolSetup import SCT_DCSConditionsToolSetup
    sct_DCSConditionsToolSetup = SCT_DCSConditionsToolSetup()
    sct_DCSConditionsToolSetup.setup()
    from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
    from SCT_ConditionsTools.SCT_FlaggedConditionToolSetup import SCT_FlaggedConditionToolSetup
    sct_FlaggedConditionToolSetup = SCT_FlaggedConditionToolSetup()
    sct_FlaggedConditionToolSetup.setup()
    sct_ConditionsSummaryToolSetup = SCT_ConditionsSummaryToolSetup()
    sct_ConditionsSummaryToolSetup.setup()
    SCT_ConditionsSummaryTool = sct_ConditionsSummaryToolSetup.getTool()
    SCT_ConditionsSummaryTool.ConditionsTools=[sct_ConfigurationConditionsToolSetup.getTool().getFullName(),
                                               sct_FlaggedConditionToolSetup.getTool().getFullName(),
                                               sct_ReadCalibDataToolSetup.getTool().getFullName(),
                                               sct_DCSConditionsToolSetup.getTool().getFullName()]
    from SiLorentzAngleTool.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup
    sctLorentzAngleToolSetup = SCTLorentzAngleToolSetup()
    # Taken from InDetRecExample/share/InDetRecPreProcessingSilicon.py
    if not hasattr(condSeq, "InDetSiElementPropertiesTableCondAlg"):
        from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiElementPropertiesTableCondAlg
        condSeq += InDet__SiElementPropertiesTableCondAlg(name = "InDetSiElementPropertiesTableCondAlg")
    # Taken from InDetRecExample/share/InDetRecLoadTools.py
    if not hasattr(condSeq, "InDetSiDetElementBoundaryLinksCondAlg"):
        from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiDetElementBoundaryLinksCondAlg_xk
        condSeq += InDet__SiDetElementBoundaryLinksCondAlg_xk(name = "InDetSiDetElementBoundaryLinksCondAlg")

if doPixel or doSCT:
    # This is for both Pixel and SCT.
    # Takne from InDetRecExample/share/InDetRecLoadTools.py
    from InDetRecExample.TrackingCommon import createAndAddCondAlg, getRIO_OnTrackErrorScalingCondAlg
    createAndAddCondAlg(getRIO_OnTrackErrorScalingCondAlg,"RIO_OnTrackErrorScalingCondAlg")

# Set up Pixel neutral network tools
clusterSplitProbTool = None
clusterSplitterTool = None
if doPixel:
    # Taken from InDetRecExample/share/InDetRecLoadTools.py
    from TrkNeuralNetworkUtils.TrkNeuralNetworkUtilsConf import Trk__NeuralNetworkToHistoTool
    NeuralNetworkToHistoTool=Trk__NeuralNetworkToHistoTool(name = "NeuralNetworkToHistoTool")
    ToolSvc += NeuralNetworkToHistoTool
    from SiClusterizationTool.SiClusterizationToolConf import InDet__NnClusterizationFactory
    NnClusterizationFactory = InDet__NnClusterizationFactory(name                         = "NnClusterizationFactory",
                                                             PixelLorentzAngleTool        = ToolSvc.PixelLorentzAngleTool,
                                                             useToT                       = InDetFlags.doNNToTCalibration(),
                                                             NnCollectionReadKey          = "PixelClusterNN",
                                                             NnCollectionWithTrackReadKey = "PixelClusterNNWithTrack")
    ToolSvc += NnClusterizationFactory

# Set up InDet__SiTrackerSpacePointFinder (alg)
# Taken from InDetRecExample/share/InDetRecPreProcessingSilicon.py
from SiSpacePointTool.SiSpacePointToolConf import InDet__SiSpacePointMakerTool
InDetSiSpacePointMakerTool = InDet__SiSpacePointMakerTool(name = "InDetSiSpacePointMakerTool")
from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiTrackerSpacePointFinder
InDetSiTrackerSpacePointFinder = InDet__SiTrackerSpacePointFinder(name                   = "InDetSiTrackerSpacePointFinder",
                                                                  SiSpacePointMakerTool  = InDetSiSpacePointMakerTool,
                                                                  PixelsClustersName     = InDetKeys.PixelClusters(),
                                                                  SCT_ClustersName       = InDetKeys.SCT_Clusters(),
                                                                  SpacePointsPixelName   = InDetKeys.PixelSpacePoints(),
                                                                  SpacePointsSCTName     = InDetKeys.SCT_SpacePoints(),
                                                                  SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
                                                                  ProcessPixels          = DetFlags.haveRIO.pixel_on(),
                                                                  ProcessSCTs            = DetFlags.haveRIO.SCT_on(),
                                                                  ProcessOverlaps        = DetFlags.haveRIO.pixel_on() and DetFlags.haveRIO.SCT_on(),
                                                                  OverrideBeamSpot       = not doBeamSpot)
if numThreads >= 2:
    InDetSiTrackerSpacePointFinder.Cardinality = numThreads
topSequence += InDetSiTrackerSpacePointFinder

# Set up ConfiguredNewTrackingCuts
# Taken from InDetRecExample/share/InDetRec_jobOptions.py
from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
NewTrackingCuts = None
if doPixel and doSCT:
    NewTrackingCuts = ConfiguredNewTrackingCuts("Offline")
elif doPixel:
    NewTrackingCuts = ConfiguredNewTrackingCuts("Pixel")
elif doSCT:
    NewTrackingCuts = ConfiguredNewTrackingCuts("SCT")

# Set up InDet__InDetPRD_AssociationToolGangedPixels (public)
if (NewTrackingCuts.mode() == "LowPt" or
    NewTrackingCuts.mode() == "VeryLowPt" or
    NewTrackingCuts.mode() == "LargeD0" or
    NewTrackingCuts.mode() == "LowPtLargeD0" or
    NewTrackingCuts.mode() == "BeamGas" or
    NewTrackingCuts.mode() == "ForwardTracks" or
    NewTrackingCuts.mode() == "ForwardSLHCTracks" or
    NewTrackingCuts.mode() == "PixelPrdAssociation" or
    NewTrackingCuts.mode() == "VeryForwardSLHCTracks" or
    NewTrackingCuts.mode() == "SLHCConversionFinding"):
    usePrdAssociationTool = True
else:
    usePrdAssociationTool = False
InDetPrdAssociationTool = None
if usePrdAssociationTool:
    # Taken from InDetRecExample/share/InDetRecLoadTools.py
    from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
    InDetPrdAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels(name                           = "InDetPrdAssociationTool",
                                                                          PixelClusterAmbiguitiesMapName = InDetKeys.GangedPixelMap(),
                                                                          addTRToutliers                 = False) ###
    ToolSvc += InDetPrdAssociationTool

# Set up InDet__SiSpacePointsSeedMaker_ATLxk (private)
# Taken from InDetRecExample/share/ConfiguredNewTrackingSiPattern.py
from SiSpacePointsSeedTool_xk.SiSpacePointsSeedTool_xkConf import InDet__SiSpacePointsSeedMaker_ATLxk as SiSpacePointsSeedMaker
InDetSiSpacePointsSeedMaker = SiSpacePointsSeedMaker(name                   = "InDetSpSeedsMaker"+NewTrackingCuts.extension(),
                                                     pTmin                  = NewTrackingCuts.minPT(),
                                                     maxdImpact             = NewTrackingCuts.maxPrimaryImpact(),
                                                     maxZ                   = NewTrackingCuts.maxZImpact(),
                                                     minZ                   = -NewTrackingCuts.maxZImpact(),
                                                     usePixel               = NewTrackingCuts.usePixel(),
                                                     SpacePointsPixelName   = InDetKeys.PixelSpacePoints(),
                                                     useSCT                 = (NewTrackingCuts.useSCT() and NewTrackingCuts.useSCTSeeding()),
                                                     SpacePointsSCTName     = InDetKeys.SCT_SpacePoints(),
                                                     useOverlapSpCollection = NewTrackingCuts.usePixel() and (NewTrackingCuts.useSCT() and NewTrackingCuts.useSCTSeeding()), ###
                                                     SpacePointsOverlapName = InDetKeys.OverlapSpacePoints(),
                                                     radMax                 = NewTrackingCuts.radMax(),
                                                     RapidityCut            = NewTrackingCuts.maxEta(),
                                                     UseAssociationTool     = usePrdAssociationTool,
                                                     AssociationTool        = InDetPrdAssociationTool,
                                                     maxdImpactPPS = NewTrackingCuts.maxdImpactPPSSeeds(),
                                                     maxdImpactSSS = NewTrackingCuts.maxdImpactSSSSeeds())
if not doBeamSpot:
    InDetSiSpacePointsSeedMaker.BeamSpotKey = ""

# Set up InDet__SiZvertexMaker_xk (private)
# Taken from ConfiguredNewTrackingSiPattern.py
from SiZvertexTool_xk.SiZvertexTool_xkConf import InDet__SiZvertexMaker_xk
InDetZvertexMaker = InDet__SiZvertexMaker_xk(name          = "InDetZvertexMaker"+NewTrackingCuts.extension(),
                                             Zmax          = NewTrackingCuts.maxZImpact(),
                                             Zmin          = -NewTrackingCuts.maxZImpact(),
                                             minRatio      = 0.17,
                                             SeedMakerTool = InDetSiSpacePointsSeedMaker)

# Set up Trk__RungeKuttaPropagator (public)
# Taken from InDetRecExample/share/InDetRecLoadTools.py
from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
InDetPatternPropagator = Propagator(name = "InDetPatternPropagator")
ToolSvc += InDetPatternPropagator

# Set up InDet__SiDetElementsRoadMaker_xk (private)
# Taken from InDetRecExample/share/ConfiguredNewTrackingSiPattern.py
if not hasattr(condSeq, "InDet__SiDetElementsRoadCondAlg_xk"):
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags
    from SiDetElementsRoadTool_xk.SiDetElementsRoadTool_xkConf import InDet__SiDetElementsRoadCondAlg_xk
    condSeq += InDet__SiDetElementsRoadCondAlg_xk(name = "InDet__SiDetElementsRoadCondAlg_xk",
                                                  UseDynamicAlignFolders = InDetGeometryFlags.useDynamicAlignFolders(),
                                                  usePixel = doPixel,
                                                  useSCT = doSCT)
from SiDetElementsRoadTool_xk.SiDetElementsRoadTool_xkConf import InDet__SiDetElementsRoadMaker_xk
InDetSiDetElementsRoadMaker = InDet__SiDetElementsRoadMaker_xk(name               = "InDetSiRoadMaker"+NewTrackingCuts.extension(),
                                                               PropagatorTool     = InDetPatternPropagator,
                                                               usePixel           = NewTrackingCuts.usePixel(),
                                                               PixManagerLocation = InDetKeys.PixelManager(),
                                                               useSCT             = NewTrackingCuts.useSCT(), 
                                                               RoadWidth          = NewTrackingCuts.RoadWidth())

# Set up InDetPatternUpdator (public)
# Taken from InDetRecExample/share/InDetRecLoadTools.py
from TrkMeasurementUpdator_xk.TrkMeasurementUpdator_xkConf import Trk__KalmanUpdator_xk
InDetPatternUpdator = Trk__KalmanUpdator_xk(name = "InDetPatternUpdator")
ToolSvc += InDetPatternUpdator

# Set up InDet__PixelClusterOnTrackTool (public)
PixelClusterOnTrackTool = None
if doPixel:
    # Taken from InDetRecExample/share/InDetRecLoadTools.py
    from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__PixelClusterOnTrackTool
    PixelClusterOnTrackTool = InDet__PixelClusterOnTrackTool("InDetPixelClusterOnTrackTool",
                                                             LorentzAngleTool   = ToolSvc.PixelLorentzAngleTool,
                                                             DisableDistortions = (InDetFlags.doFatras() or InDetFlags.doDBMstandalone()),
                                                             applyNNcorrection = ( InDetFlags.doPixelClusterSplitting() and
                                                                                   InDetFlags.pixelClusterSplittingType() == "NeuralNet" and not InDetFlags.doSLHC()),
                                                             NNIBLcorrection = ( InDetFlags.doPixelClusterSplitting() and
                                                                                 InDetFlags.pixelClusterSplittingType() == "NeuralNet" and not InDetFlags.doSLHC()),
                                                             SplitClusterAmbiguityMap = InDetKeys.SplitClusterAmbiguityMap(),
                                                             RunningTIDE_Ambi = InDetFlags.doTIDE_Ambi())
    PixelClusterOnTrackTool.NnClusterizationFactory  = NnClusterizationFactory
    ToolSvc += PixelClusterOnTrackTool

# Set up InDet__SCT_ClusterOnTrackTool (private)
SCT_ClusterOnTrackTool = None
if doSCT:
    # Taken from InDetRecExample/share/InDetRecLoadTools.py
    from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__SCT_ClusterOnTrackTool
    SCT_ClusterOnTrackTool = InDet__SCT_ClusterOnTrackTool("InDetSCT_ClusterOnTrackTool",
                                                           CorrectionStrategy = 0,  # do correct position bias
                                                           ErrorStrategy      = 2,  # do use phi dependent errors
                                                           LorentzAngleTool   = sctLorentzAngleToolSetup.SCTLorentzAngleTool)

# Set up InDetRotCreator (public)
# Taken from InDetRecExample/share/InDetRecLoadTools.py
from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
InDetRotCreator = Trk__RIO_OnTrackCreator(name             = "InDetRotCreator",
                                          ToolPixelCluster = PixelClusterOnTrackTool,
                                          ToolSCT_Cluster  = SCT_ClusterOnTrackTool,
                                          ToolTRT_DriftCircle = None,
                                          Mode             = "indet")
ToolSvc += InDetRotCreator

# Set up SiCombinatorialTrackFinder_xk (private)
# Taken from InDetRecExample/share/InDetRecLoadTools.py
from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiCombinatorialTrackFinder_xk
InDetSiComTrackFinder = InDet__SiCombinatorialTrackFinder_xk(name                  = "InDetSiComTrackFinder",
                                                             PropagatorTool        = InDetPatternPropagator,
                                                             UpdatorTool           = InDetPatternUpdator,
                                                             RIOonTrackTool        = InDetRotCreator,
                                                             SctSummaryTool        = SCT_ConditionsSummaryTool,
                                                             AssosiationTool       = InDetPrdAssociationTool,
                                                             usePixel              = DetFlags.haveRIO.pixel_on(),
                                                             useSCT                = DetFlags.haveRIO.SCT_on(),
                                                             PixManagerLocation    = InDetKeys.PixelManager(),
                                                             PixelClusterContainer = InDetKeys.PixelClusters(),
                                                             SCT_ClusterContainer  = InDetKeys.SCT_Clusters())

# Set up SiTrackMaker_xk (private)
# Taken from InDetRecExample/share/ConfiguredNewTrackingSiPattern.py
# useBremMode = NewTrackingCuts.mode() == "Offline" or NewTrackingCuts.mode() == "SLHC"
useBremMode = False ###
InDetFlags.doCaloSeededBrem.set_Value_and_Lock(False) ###
InDetFlags.doHadCaloSeededSSS.set_Value_and_Lock(False) ###
InDetKeys.CaloClusterROIContainer.set_Value_and_Lock("") ###
InDetKeys.HadCaloClusterROIContainer.set_Value_and_Lock("") ###
from SiTrackMakerTool_xk.SiTrackMakerTool_xkConf import InDet__SiTrackMaker_xk as SiTrackMaker
InDetSiTrackMaker = SiTrackMaker(name                      = "InDetSiTrackMaker"+NewTrackingCuts.extension(),
                                 useSCT                    = NewTrackingCuts.useSCT(),
                                 usePixel                  = NewTrackingCuts.usePixel(),
                                 RoadTool                  = InDetSiDetElementsRoadMaker,
                                 CombinatorialTrackFinder  = InDetSiComTrackFinder,
                                 pTmin                     = NewTrackingCuts.minPT(),
                                 pTminBrem                 = NewTrackingCuts.minPTBrem(),
                                 pTminSSS                  = InDetFlags.pT_SSScut(),
                                 nClustersMin              = NewTrackingCuts.minClusters(),
                                 nHolesMax                 = NewTrackingCuts.nHolesMax(),
                                 nHolesGapMax              = NewTrackingCuts.nHolesGapMax(),
                                 SeedsFilterLevel          = NewTrackingCuts.seedFilterLevel(),
                                 Xi2max                    = NewTrackingCuts.Xi2max(),
                                 Xi2maxNoAdd               = NewTrackingCuts.Xi2maxNoAdd(),
                                 nWeightedClustersMin      = NewTrackingCuts.nWeightedClustersMin(),
                                 CosmicTrack               = InDetFlags.doCosmics(),
                                 Xi2maxMultiTracks         = NewTrackingCuts.Xi2max(), # was 3.
                                 useSSSseedsFilter         = InDetFlags.doSSSfilter(), 
                                 doMultiTracksProd         = True,
                                 useBremModel              = InDetFlags.doBremRecovery() and useBremMode, # only for NewTracking the brem is debugged !!!
                                 doCaloSeededBrem          = InDetFlags.doCaloSeededBrem(),
                                 doHadCaloSeedSSS          = InDetFlags.doHadCaloSeededSSS(),
                                 phiWidth                  = NewTrackingCuts.phiWidthBrem(),
                                 etaWidth                  = NewTrackingCuts.etaWidthBrem(),
                                 InputClusterContainerName = InDetKeys.CaloClusterROIContainer(), # "InDetCaloClusterROIs" 
                                 InputHadClusterContainerName = InDetKeys.HadCaloClusterROIContainer(), # "InDetCaloClusterROIs" 
                                 UseAssociationTool        = usePrdAssociationTool)
InDetSiTrackMaker.TrackPatternRecoInfo = "SiSPSeededFinder"
if not doBeamSpot:
    InDetSiTrackMaker.BeamSpotKey = ""

# Set up SiSPSeededTrackFinder (alg)
# InDetRecExample/share/ConfiguredNewTrackingSiPattern.py
from SiSPSeededTrackFinder.SiSPSeededTrackFinderConf import InDet__SiSPSeededTrackFinder
InDetSiSPSeededTrackFinder = InDet__SiSPSeededTrackFinder(name           = "InDetSiSpTrackFinder"+NewTrackingCuts.extension(),
                                                          TrackTool      = InDetSiTrackMaker,
                                                          TracksLocation = TracksLocation,
                                                          SeedsTool      = InDetSiSpacePointsSeedMaker,
                                                          useZvertexTool = InDetFlags.useZvertexTool() and NewTrackingCuts.mode() != "DBM",
                                                          ZvertexTool    = InDetZvertexMaker,
                                                          useNewStrategy = InDetFlags.useNewSiSPSeededTF() and NewTrackingCuts.mode() != "DBM",
                                                          useMBTSTimeDiff = InDetFlags.useMBTSTimeDiff(),
                                                          useZBoundFinding = NewTrackingCuts.doZBoundary() and NewTrackingCuts.mode() != "DBM")
if not doBeamSpot:
    InDetSiSPSeededTrackFinder.BeamSpotKey = ""
if not doPixel:
    InDetSiSPSeededTrackFinder.SpacePointsPixelName = ""
if not doSCT:
    InDetSiSPSeededTrackFinder.SpacePointsSCTName = ""

if doPrint:
    print InDetSiSPSeededTrackFinder
if numThreads >= 2:
    InDetSiSPSeededTrackFinder.Cardinality = numThreads
topSequence += InDetSiSPSeededTrackFinder

# Print algorithms
if doPrint:
    print topSequence

# Set the number of events to be processed
theApp.EvtMax = EvtMax

# Output file
if doDump:
    from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
    outStream = AthenaPoolOutputStream("OutStream", "SiSPSeededTracksStandaloneFromESD.pool.root")
    outStream.ItemList  = ["xAOD::EventInfo#EventInfo", "xAOD::EventAuxInfo#EventInfoAux."]
    outStream.ItemList += ["TrackCollection#"+TracksLocation]

#--------------------------------------------------------------
# Set output lvl (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"

if numThreads >= 2:
    from SCT_ConditionsAlgorithms.SCTCondAlgCardinality import sctCondAlgCardinality
    sctCondAlgCardinality.set(numThreads)
