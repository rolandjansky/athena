# ------------------------------------------------------------
#
# ----------- Loading the Tracking Services
#
# ------------------------------------------------------------

from InDetRecExample.TrackingCommon import createAndAddCondAlg
from InDetRecExample.TrackingCommon import getRIO_OnTrackErrorScalingCondAlg

use_broad_cluster_any = InDetFlags.useBroadClusterErrors() and (not InDetFlags.doDBMstandalone())
use_broad_cluster_pix = InDetFlags.useBroadPixClusterErrors() and (not InDetFlags.doDBMstandalone())
use_broad_cluster_sct = InDetFlags.useBroadSCTClusterErrors() and (not InDetFlags.doDBMstandalone())
if use_broad_cluster_pix is None :
    use_broad_cluster_pix = use_broad_cluster_any
if use_broad_cluster_sct is None :
    use_broad_cluster_sct = use_broad_cluster_any

# detector specific settings will override the global setting:
use_broad_cluster_any = use_broad_cluster_pix or use_broad_cluster_sct

# load common NN tools for clustering and ROT creation
if InDetFlags.doPixelClusterSplitting() and not InDetFlags.doSLHC():

    #
    # --- Neutral Network version ?
    #
    if InDetFlags.pixelClusterSplittingType() == 'NeuralNet':

        # --- temp: read calib file
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if not hasattr(svcMgr, 'THistSvc'):
            from GaudiSvc.GaudiSvcConf import THistSvc
            svcMgr += THistSvc()

        dataPathList = os.environ[ 'DATAPATH' ].split(os.pathsep)
        dataPathList.insert(0, os.curdir)


        # --- neutral network tools
        from TrkNeuralNetworkUtils.TrkNeuralNetworkUtilsConf import Trk__NeuralNetworkToHistoTool
        NeuralNetworkToHistoTool=Trk__NeuralNetworkToHistoTool(name = "NeuralNetworkToHistoTool")

        ToolSvc += NeuralNetworkToHistoTool
        if (InDetFlags.doPrintConfigurables()):
            printfunc (NeuralNetworkToHistoTool)

        # --- new NN factor

        # --- put in a temporary hack here for 19.1.0, to select the necessary settings when running on run 1 data/MC
        # --- since a correction is needed to fix biases when running on new run 2 compatible calibation
        # --- a better solution is needed...

        if not hasattr(ToolSvc, "PixelLorentzAngleTool"):
            from SiLorentzAngleTool.PixelLorentzAngleToolSetup import PixelLorentzAngleToolSetup
            pixelLorentzAngleToolSetup = PixelLorentzAngleToolSetup()

        from SiClusterizationTool.SiClusterizationToolConf import InDet__NnClusterizationFactory

        from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as geoFlags
        do_runI = geoFlags.Run() not in ["RUN2", "RUN3"]
        from InDetRecExample.TrackingCommon import getPixelClusterNnCondAlg,getPixelClusterNnWithTrackCondAlg
        createAndAddCondAlg( getPixelClusterNnCondAlg,         'PixelClusterNnCondAlg',          GetInputsInfo = do_runI)
        createAndAddCondAlg( getPixelClusterNnWithTrackCondAlg,'PixelClusterNnWithTrackCondAlg', GetInputsInfo = do_runI)
        if do_runI :
            NnClusterizationFactory = InDet__NnClusterizationFactory( name                               = "NnClusterizationFactory",
                                                                      PixelLorentzAngleTool              = ToolSvc.PixelLorentzAngleTool,
                                                                      doRunI                             = True,
                                                                      useToT                             = False,
                                                                      useRecenteringNNWithoutTracks      = True,
                                                                      useRecenteringNNWithTracks         = False,
                                                                      correctLorShiftBarrelWithoutTracks = 0,
                                                                      correctLorShiftBarrelWithTracks    = 0.030,
                                                                      NnCollectionReadKey                = 'PixelClusterNN',
                                                                      NnCollectionWithTrackReadKey       = 'PixelClusterNNWithTrack')

        else:
            NnClusterizationFactory = InDet__NnClusterizationFactory( name                         = "NnClusterizationFactory",
                                                                      PixelLorentzAngleTool        = ToolSvc.PixelLorentzAngleTool,
                                                                      useToT                       = InDetFlags.doNNToTCalibration(),
                                                                      NnCollectionReadKey          = 'PixelClusterNN',
                                                                      NnCollectionWithTrackReadKey = 'PixelClusterNNWithTrack')

        ToolSvc += NnClusterizationFactory

        if (InDetFlags.doPrintConfigurables()):
            printfunc (NnClusterizationFactory)
elif InDetFlags.doPixelClusterSplitting():
    if not hasattr(ToolSvc, "PixelLorentzAngleTool"):
        from SiLorentzAngleTool.PixelLorentzAngleToolSetup import PixelLorentzAngleToolSetup
        pixelLorentzAngleToolSetup = PixelLorentzAngleToolSetup()

    from SiClusterizationTool.SiClusterizationToolConf import InDet__TruthClusterizationFactory
    NnClusterizationFactory = InDet__TruthClusterizationFactory( name                 = "TruthClusterizationFactory",
                                                                 PixelLorentzAngleTool= ToolSvc.PixelLorentzAngleTool)
    ToolSvc += NnClusterizationFactory
    if (InDetFlags.doPrintConfigurables()):
        printfunc (NnClusterizationFactory)


# --- load cabling (if needed)
include("InDetRecExample/InDetRecCabling.py")

# --- load event cnv tool
# --- this hack is needed by every ID job that uses pool i/o
from TrkEventCnvTools import TrkEventCnvToolsConfig

# ------------------------------------------------------------
#
# ----------- Loading of general Tracking Tools
#
# ------------------------------------------------------------

#
# ----------- control loading of ROT_creator
#

if InDetFlags.loadRotCreator():
    import InDetRecExample.TrackingCommon as TrackingCommon
    #
    # --- configure default ROT creator
    #
    if DetFlags.haveRIO.pixel_on():
        #
        # load Pixel ROT creator, we overwrite the defaults for the
        # tool to always make conservative pixel cluster errors
        # SiLorentzAngleTool
        if InDetFlags.doDBM():
            PixelClusterOnTrackToolDBM = TrackingCommon.getInDetPixelClusterOnTrackToolDBM()
        PixelClusterOnTrackTool        = TrackingCommon.getInDetPixelClusterOnTrackTool()
        PixelClusterOnTrackToolPattern = TrackingCommon.getInDetPixelClusterOnTrackToolPattern()

        if InDetFlags.doDigitalROTCreation():
            PixelClusterOnTrackToolDigital = TrackingCommon.getInDetPixelClusterOnTrackToolDigital()
        else :
            PixelClusterOnTrackToolDigital = TrackingCommon.getInDetPixelClusterOnTrackTool(name                     = "InDetPixelClusterOnTrackToolNoSplitClusterMap",
                                                                                            SplitClusterAmbiguityMap = "")
    else:
        PixelClusterOnTrackTool = None
        PixelClusterOnTrackToolDigital = None
        PixelClusterOnTrackToolPattern = None

    if DetFlags.haveRIO.SCT_on():
        SCT_ClusterOnTrackTool = TrackingCommon.getInDetSCT_ClusterOnTrackTool()
    else:
        SCT_ClusterOnTrackTool = None

    #
    # default ROT creator, not smart !
    #
    InDetRotCreator = TrackingCommon.getInDetRotCreator()
    InDetRotCreatorPattern = TrackingCommon.getInDetRotCreatorPattern()

    if InDetFlags.doDBM():
        InDetRotCreatorDBM=TrackingCommon.getInDetRotCreatorDBM()

    if PixelClusterOnTrackToolDigital is not None :
        InDetRotCreatorDigital = TrackingCommon.getInDetRotCreatorDigital()
    else:
        InDetRotCreatorDigital=InDetRotCreatorPattern

    #
    # --- configure broad cluster ROT creator
    #
    if DetFlags.haveRIO.pixel_on():
        #
        # tool to always make conservative pixel cluster errors
        if not InDetFlags.doDBMstandalone():
            BroadPixelClusterOnTrackTool = TrackingCommon.getInDetBroadPixelClusterOnTrackTool()
        else:
            BroadPixelClusterOnTrackTool = None
    else:
        BroadPixelClusterOnTrackTool = None

    if DetFlags.haveRIO.SCT_on():
        #
        # tool to always make conservative sct cluster errors
        #
        BroadSCT_ClusterOnTrackTool = TrackingCommon.getInDetBroadSCT_ClusterOnTrackTool()
    else:
        BroadSCT_ClusterOnTrackTool= None

    if DetFlags.haveRIO.TRT_on():
        #
        # tool to always make conservative trt drift circle errors
        #
        BroadTRT_DriftCircleOnTrackTool = TrackingCommon.getInDetBroadTRT_DriftCircleOnTrackTool()
    else:
        BroadTRT_DriftCircleOnTrackTool = None

    BroadInDetRotCreator = TrackingCommon.getInDetBroadRotCreator()

    #
    # --- load error scaling
    #
    createAndAddCondAlg(getRIO_OnTrackErrorScalingCondAlg,'RIO_OnTrackErrorScalingCondAlg')
    #
    # --- smart ROT creator in case we do the TRT LR in the refit
    #
    ScaleHitUncertainty = 2.5

    if InDetFlags.redoTRT_LR():

        if DetFlags.haveRIO.TRT_on():
            # --- this is the cut for making a TRT hit a tube hit (biases the distribution)
            TRT_RefitRotCreator = TrackingCommon.getInDetTRT_DriftCircleOnTrackUniversalTool()
        else:
            TRT_RefitRotCreator = None

        InDetRefitRotCreator = TrackingCommon.getInDetRefitRotCreator()

    else:
        InDetRefitRotCreator = InDetRotCreator

#
# ----------- control loading of the kalman updator
#
if InDetFlags.loadUpdator() :

    InDetUpdator = TrackingCommon.getInDetUpdator()
    ToolSvc += InDetUpdator
    if (InDetFlags.doPrintConfigurables()):
      printfunc (     InDetUpdator)

#
# ----------- control laoding extrapolation
#
if InDetFlags.loadExtrapolator():
    #
    # if (InDetFlags.doPrintConfigurables()):
    #     printfunc (     InDetMultipleScatteringUpdator)

    InDetPropagator      = TrackingCommon.getInDetPropagator()
    InDetNavigator       = TrackingCommon.getInDetNavigator()
    InDetMaterialUpdator = TrackingCommon.getInDetMaterialEffectsUpdator()
    InDetExtrapolator    = TrackingCommon.getInDetExtrapolator()

#
# ----------- control loading of fitters
#
if InDetFlags.loadFitter():

    if InDetFlags.doBremRecovery() and InDetFlags.trackFitterType() == 'GlobalChi2Fitter' :
        # @TODO  create where it is needed
        from TrkExTools.TrkExToolsConf import Trk__EnergyLossUpdator
        InDetEnergyLossUpdator = Trk__EnergyLossUpdator(name="AtlasEnergyLossUpdator",
                                                        UseBetheBlochForElectrons = False)
        ToolSvc               += InDetEnergyLossUpdator

    from AthenaCommon import CfgGetter
    InDetTrackFitter    = CfgGetter.getPublicTool('InDetTrackFitter')

    if InDetFlags.doLowPt() or InDetFlags.doVeryLowPt() or (InDetFlags.doTrackSegmentsPixel() and InDetFlags.doMinBias()):
        InDetTrackFitterLowPt = CfgGetter.getPublicTool('InDetTrackFitterLowPt')
    if DetFlags.TRT_on():
        InDetTrackFitterTRT =   CfgGetter.getPublicTool('InDetTrackFitterTRT')
    if InDetFlags.doDBM():
        InDetTrackFitterDBM =   CfgGetter.getPublicTool('InDetTrackFitterDBM')

        # @TODO should aldready be in ToolSvc
        # if not InDetFlags.doDBMstandalone():
            # ToolSvc += InDetTrackFitterTRT
            # if (InDetFlags.doPrintConfigurables()):
            #     printfunc (InDetTrackFitterTRT)
        # if InDetFlags.doLowPt() or (InDetFlags.doTrackSegmentsPixel() and InDetFlags.doMinBias()):
            # ToolSvc+=InDetTrackFitterLowPt
            # if (InDetFlags.doPrintConfigurables()):
            #    printfunc (InDetTrackFitterLowPt)

#
# ----------- load association tool from Inner Detector to handle pixel ganged ambiguities
#
if InDetFlags.loadAssoTool():

    from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
    InDetPrdAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels(name                           = "InDetPrdAssociationTool",
                                                                          PixelClusterAmbiguitiesMapName = InDetKeys.GangedPixelMap(),
                                                                          addTRToutliers                 = True)
    # InDetPrdAssociationTool.OutputLevel = VERBOSE
    ToolSvc += InDetPrdAssociationTool
    if (InDetFlags.doPrintConfigurables()):
      printfunc (     InDetPrdAssociationTool)

    from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
    InDetPrdAssociationTool_setup = InDet__InDetPRD_AssociationToolGangedPixels(name                           = "InDetPrdAssociationTool",
                                                                                PixelClusterAmbiguitiesMapName = InDetKeys.GangedPixelMap(),
                                                                                SetupCorrect                   = True,
                                                                                addTRToutliers                 = True)
    # InDetPrdAssociationTool.OutputLevel = VERBOSE
    ToolSvc += InDetPrdAssociationTool

#
# ----------- control loading of SummaryTool
#

if InDetFlags.loadSummaryTool():

    from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
    AtlasTrackSummaryTool = AtlasTrackSummaryTool()
    ToolSvc += AtlasTrackSummaryTool

    InDetPixelConditionsSummaryTool = TrackingCommon.getInDetPixelConditionsSummaryTool()

    if (InDetFlags.doPrintConfigurables()):
        printfunc (InDetPixelConditionsSummaryTool)

    InDetTestPixelLayerTool = TrackingCommon.getInDetTestPixelLayerTool()
    InDetHoleSearchTool     = TrackingCommon.getInDetHoleSearchTool()
    #
    # Load BLayer tool
    #
    InDetRecTestBLayerTool = None
    if DetFlags.haveRIO.pixel_on() :
        InDetRecTestBLayerTool = TrackingCommon.getInDetRecTestBLayerTool()

    #
    # Configurable version of TrkTrackSummaryTool: no TRT_PID tool needed here (no shared hits)
    #
    InDetTrackSummaryTool           = TrackingCommon.getInDetTrackSummaryTool()
    InDetTrackSummaryToolSharedHits = TrackingCommon.getInDetTrackSummaryToolSharedHits()

# ------------------------------------------------------------
#
# ----------- Loading of pattern tools
#
# ------------------------------------------------------------


if InDetFlags.doPattern():
    #
    # Igors propagator needed by Igors tools
    #
    from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
    InDetPatternPropagator = Propagator(name = 'InDetPatternPropagator')
    ToolSvc += InDetPatternPropagator
    if (InDetFlags.doPrintConfigurables()):
      printfunc (     InDetPatternPropagator)
    #
    # fast Kalman updator tool
    #
    from TrkMeasurementUpdator_xk.TrkMeasurementUpdator_xkConf import Trk__KalmanUpdator_xk
    InDetPatternUpdator = Trk__KalmanUpdator_xk(name = 'InDetPatternUpdator')
    ToolSvc += InDetPatternUpdator
    if (InDetFlags.doPrintConfigurables()):
      printfunc (     InDetPatternUpdator)

    # ------------------------------------------------------------
    #
    # ----------- Loading of tools for TRT extensions
    #
    # ------------------------------------------------------------

    #
    # TRT detector elements road builder
    #
    if not InDetFlags.doDBMstandalone():
        InDetTRTDetElementsRoadMaker =  TrackingCommon.getInDetTRT_RoadMaker()
        from InDetRecExample.TrackingCommon import getTRT_DetElementsRoadCondAlg
        createAndAddCondAlg(getTRT_DetElementsRoadCondAlg,'TRT_DetElementsRoadCondAlg_xk')

        if (InDetFlags.doPrintConfigurables()):
            printfunc (     InDetTRTDetElementsRoadMaker)

    #
    # TRT segment minimum number of drift circles tool
    #
    from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetTrtDriftCircleCutTool
    InDetTRTDriftCircleCut = TrackingCommon.getInDetTRTDriftCircleCutForPatternReco()

    if (InDetFlags.doPrintConfigurables()):
        printfunc (  InDetTRTDriftCircleCut)

    #
    # Local combinatorial track finding using space point seed and detector element road
    #
    from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiCombinatorialTrackFinder_xk
    InDetSiComTrackFinder = InDet__SiCombinatorialTrackFinder_xk(name                  = 'InDetSiComTrackFinder',
                                                                 PropagatorTool        = TrackingCommon.getInDetPatternPropagator(),
                                                                 UpdatorTool           = TrackingCommon.getInDetPatternUpdator(),
                                                                 RIOonTrackTool        = TrackingCommon.getInDetRotCreatorDigital(), # #NS HERE
                                                                 usePixel              = DetFlags.haveRIO.pixel_on(),
                                                                 useSCT                = DetFlags.haveRIO.SCT_on(),
                                                                 PixelClusterContainer = InDetKeys.PixelClusters(),
                                                                 SCT_ClusterContainer  = InDetKeys.SCT_Clusters())
    if DetFlags.haveRIO.pixel_on():
        # Condition algorithm for SiCombinatorialTrackFinder_xk
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        if not hasattr(condSeq, "InDetSiDetElementBoundaryLinksPixelCondAlg"):
            from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiDetElementBoundaryLinksCondAlg_xk
            condSeq += InDet__SiDetElementBoundaryLinksCondAlg_xk(name = "InDetSiDetElementBoundaryLinksPixelCondAlg",
                                                                  ReadKey = "PixelDetectorElementCollection",
                                                                  WriteKey = "PixelDetElementBoundaryLinks_xk")
    if DetFlags.haveRIO.SCT_on():
        # Condition algorithm for SiCombinatorialTrackFinder_xk
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        if not hasattr(condSeq, "InDetSiDetElementBoundaryLinksSCTCondAlg"):
            from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiDetElementBoundaryLinksCondAlg_xk
            condSeq += InDet__SiDetElementBoundaryLinksCondAlg_xk(name = "InDetSiDetElementBoundaryLinksSCTCondAlg",
                                                                  ReadKey = "SCT_DetectorElementCollection",
                                                                  WriteKey = "SCT_DetElementBoundaryLinks_xk")

    #if InDetFlags.doDBM():
    #    InDetSiComTrackFinderDBM = InDet__SiCombinatorialTrackFinder_xk(name                  = 'InDetSiComTrackFinderDBM',
    #                                                                    PropagatorTool        = InDetPatternPropagator,
    #                                                                    UpdatorTool           = InDetPatternUpdator,
    #                                                                    RIOonTrackTool        = InDetRotCreatorDBM,
    #                                                                    AssosiationTool       = InDetPrdAssociationTool,
    #                                                                    usePixel              = True,
    #                                                                    useSCT                = False,
    #                                                                    PixelClusterContainer = InDetKeys.PixelClusters(),
    #                                                                    SCT_ClusterContainer  = InDetKeys.SCT_Clusters(),
    #                                                                    MagneticFieldMode     = "NoField",
    #                                                                    TrackQualityCut       = 9.3
    #                                                                    )
    #if InDetFlags.doDBMstandalone():
    #    InDetSiComTrackFinder.MagneticFieldMode     =  "NoField"

    # if (DetFlags.haveRIO.SCT_on()):
    #   InDetSiComTrackFinder.SctSummaryTool = InDetSCT_ConditionsSummaryTool
    # else:
    #  InDetSiComTrackFinder.SctSummaryTool = None

    if (InDetFlags.doPrintConfigurables()):
      printfunc (InDetSiComTrackFinder)

# ------------------------------------------------------------
#
# ----------- Track extension to TRT tool for New Tracking
#
# ------------------------------------------------------------
InDetTRTExtensionTool = TrackingCommon.getInDetTRT_ExtensionTool(TrackingCuts = InDetNewTrackingCuts)

# ------------------------------------------------------------
#
# ----------- Loading of tools for Cosmics
#
# ------------------------------------------------------------

if InDetFlags.doPattern() and InDetFlags.doCosmics():

    from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetCosmicScoringTool
    InDetScoringToolCosmics = InDet__InDetCosmicScoringTool(name         = 'InDetCosmicScoringTool',
                                                            SummaryTool  = InDetTrackSummaryTool)
    ToolSvc += InDetScoringToolCosmics
    if (InDetFlags.doPrintConfigurables()):
        printfunc (     InDetScoringToolCosmics)

    from InDetAmbiTrackSelectionTool.InDetAmbiTrackSelectionToolConf import InDet__InDetAmbiTrackSelectionTool
    InDetAmbiTrackSelectionToolCosmics = InDet__InDetAmbiTrackSelectionTool(name                  = 'InDetAmbiTrackSelectionToolCosmics',
                                                                            AssociationTool       = TrackingCommon.getInDetPRDtoTrackMapToolGangedPixels(),
                                                                            minNotShared          = 3,
                                                                            minHits               = 0,
                                                                            maxShared             = 0,
                                                                            maxTracksPerSharedPRD = 10,
                                                                            Cosmics               = True,
                                                                            DriftCircleCutTool    = InDetTRTDriftCircleCut,
                                                                            UseParameterization   = False)

    ToolSvc += InDetAmbiTrackSelectionToolCosmics
    if (InDetFlags.doPrintConfigurables()):
        printfunc (     InDetAmbiTrackSelectionToolCosmics)


    from AthenaCommon import CfgGetter
    from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__SimpleAmbiguityProcessorTool
    InDetAmbiguityProcessorCosmics = Trk__SimpleAmbiguityProcessorTool(name             = 'InDetAmbiguityProcessorCosmics',
                                                                       ScoringTool      = InDetScoringToolCosmics,
                                                                       Fitter           = CfgGetter.getPublicTool('InDetTrackFitter'),
                                                                       AssociationTool  = TrackingCommon.getInDetPRDtoTrackMapToolGangedPixels(),
                                                                       TrackSummaryTool = TrackingCommon.getInDetTrackSummaryTool(),
                                                                       SelectionTool    = InDetAmbiTrackSelectionToolCosmics,
                                                                       SuppressTrackFit = True,
                                                                       ForceRefit       = False,
                                                                       RefitPrds        = False)

    ToolSvc += InDetAmbiguityProcessorCosmics
    if (InDetFlags.doPrintConfigurables()):
        printfunc (     InDetAmbiguityProcessorCosmics)


# ------------------------------------------------------------
#
# ----------- Loading of tools for truth comparison
#
# ------------------------------------------------------------

# id rec stat processing and trk+pixel ntuple creation need this tool if truth is on
if InDetFlags.doTruth() and (InDetFlags.doStatistics() or InDetFlags.doPhysValMon() or InDetFlags.doNtupleCreation()):
    #
    # --- load truth to track tool
    #
    if InDetFlags.doCosmics():
      from TrkTruthToTrack.TrkTruthToTrackConf import Trk__TruthTrackRecordToTrack
      InDetTruthToTrack  = Trk__TruthTrackRecordToTrack(name         = "InDetTruthToTrack",
                                                        # for Cosmics sim before Summer2009 activate this:      TrackRecordKey = "CaloEntryLayer",
                                                        Extrapolator = InDetExtrapolator)
    else:
      from TrkTruthToTrack.TrkTruthToTrackConf import Trk__TruthToTrack
      InDetTruthToTrack  = Trk__TruthToTrack(name         = "InDetTruthToTrack",
                                             Extrapolator = InDetExtrapolator)
    ToolSvc += InDetTruthToTrack
    if (InDetFlags.doPrintConfigurables()):
        printfunc (InDetTruthToTrack)

# ------------------------------------------------------------
#
# ----------- Loading of tools for Vertexing
#
# ------------------------------------------------------------

#
# ------ load track selector for vertexing, needs to be done here because is also needed for vertex ntuple creation
#
if InDetFlags.doVertexFinding() or InDetFlags.doVertexFindingForMonitoring() or InDetFlags.doSplitVertexFindingForMonitoring() or InDetFlags.doVtxNtuple():
    from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
    InDetTrackSelectorTool = InDet__InDetTrackSelectionTool(name = "InDetDetailedTrackSelectionTool",
                                                            CutLevel = InDetPrimaryVertexingCuts.TrackCutLevel(),
                                                            minPt = InDetPrimaryVertexingCuts.minPT(),
                                                            maxD0 = InDetPrimaryVertexingCuts.IPd0Max(),
                                                            maxZ0 = InDetPrimaryVertexingCuts.z0Max(),
                                                            maxZ0SinTheta = InDetPrimaryVertexingCuts.IPz0Max(),
                                                            maxSigmaD0 = InDetPrimaryVertexingCuts.sigIPd0Max(),
                                                            maxSigmaZ0SinTheta = InDetPrimaryVertexingCuts.sigIPz0Max(),
                                                            # maxChiSqperNdf = InDetPrimaryVertexingCuts.fitChi2OnNdfMax(), # Seems not to be implemented?
                                                            maxAbsEta = InDetPrimaryVertexingCuts.etaMax(),
                                                            minNInnermostLayerHits = InDetPrimaryVertexingCuts.nHitInnermostLayer(),
                                                            minNPixelHits = InDetPrimaryVertexingCuts.nHitPix(),
                                                            maxNPixelHoles = InDetPrimaryVertexingCuts.nHolesPix(),
                                                            minNSctHits = InDetPrimaryVertexingCuts.nHitSct(),
                                                            minNTrtHits = InDetPrimaryVertexingCuts.nHitTrt(),
                                                            minNSiHits = InDetPrimaryVertexingCuts.nHitSi(),
                                                            TrackSummaryTool = InDetTrackSummaryTool,
                                                            Extrapolator = InDetExtrapolator)

    ToolSvc += InDetTrackSelectorTool
    if (InDetFlags.doPrintConfigurables()):
        printfunc (InDetTrackSelectorTool)


if (InDetFlags.doVertexFinding() or InDetFlags.doVertexFindingForMonitoring()) or InDetFlags.doSplitVertexFindingForMonitoring() and InDetFlags.primaryVertexSetup() != 'DummyVxFinder':
  #
  # --- load linearized track factory
  #
  from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
  InDetLinFactory = Trk__FullLinearizedTrackFactory(name              = "InDetFullLinearizedTrackFactory",
                                                    Extrapolator      = InDetExtrapolator )
  ToolSvc += InDetLinFactory
  if (InDetFlags.doPrintConfigurables()):
    printfunc (InDetLinFactory)

  #
  # --- load configured Seed finder
  #
  if (InDetFlags.primaryVertexSetup() == 'GaussIterativeFinding' or
          InDetFlags.primaryVertexSetup() == 'GaussAdaptiveMultiFinding'):
    from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__GaussianTrackDensity
    GaussDensityEstimator = Trk__GaussianTrackDensity(name="GaussianDensity")
    ToolSvc += GaussDensityEstimator

    from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__TrackDensitySeedFinder
    InDetVtxSeedFinder = Trk__TrackDensitySeedFinder(name="GaussianDensitySeedFinder",
                                                     DensityEstimator=GaussDensityEstimator)

  elif (InDetFlags.primaryVertexSetup() == 'MedImgMultiFinding'):
    from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__LocalMax1DClusterFinder, Trk__VertexImageMaker
    InDetMedImgClusterFinder = Trk__LocalMax1DClusterFinder(name="InDetMedImgClusterFinder",
                                                            weightThreshold=1500.0,
                                                            mergeParameter=0.95,
                                                            clusterWindowXY=0.34,
                                                            refineZ=True,
                                                            gaussianWindow=True)
    ToolSvc += InDetMedImgClusterFinder
    InDetMedImgMaker = Trk__VertexImageMaker(name="InDetMedImgMaker",
                                             xbins=32,
                                             ybins=32,
                                             zbins=2048,
                                             xrange=2.0,
                                             yrange=2.0,
                                             zrange=200.0,
                                             cutoffFreqDenominator_xy=2,
                                             cutoffFreqDenominator_z=1,
                                             angularCutoffParameter=0.75)
    ToolSvc += InDetMedImgMaker
    from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__ImagingSeedFinder
    InDetVtxSeedFinder = Trk__ImagingSeedFinder(name="InDetMedImgSeedFinder",
                                                VertexCluster=InDetMedImgClusterFinder,
                                                VertexImageMaker=InDetMedImgMaker)

  elif (InDetFlags.doPrimaryVertex3DFinding()):
    from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__CrossDistancesSeedFinder
    InDetVtxSeedFinder = Trk__CrossDistancesSeedFinder(name="InDetCrossDistancesSeedFinder",
                                                       trackdistcutoff=1.,
                                                       trackdistexppower=2)
  
  else:
    from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__ZScanSeedFinder
    InDetVtxSeedFinder = Trk__ZScanSeedFinder(name="InDetZScanSeedFinder"
                                              # Mode1dFinder = # default, no setting needed
                                              )
  ToolSvc += InDetVtxSeedFinder
  if (InDetFlags.doPrintConfigurables()):
      printfunc(InDetVtxSeedFinder)

  #
  # --- load Impact Point Factory
  #
  from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__ImpactPoint3dEstimator
  InDetImpactPoint3dEstimator = Trk__ImpactPoint3dEstimator(name="InDetImpactPoint3dEstimator",
                                                            Extrapolator=InDetExtrapolator)
  ToolSvc += InDetImpactPoint3dEstimator
  if (InDetFlags.doPrintConfigurables()):
      printfunc(InDetImpactPoint3dEstimator)

  #
  # --- load Configured Annealing Maker
  #
  from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__DetAnnealingMaker
  if(InDetFlags.primaryVertexSetup() == 'GaussAdaptiveMultiFinding'):
    InDetAnnealingMaker = Trk__DetAnnealingMaker(name="InDetAnnealingMaker",
                                                 SetOfTemperatures=[8., 4., 2., 1.4142136, 1.2247449, 1.])  # 'standard' annealing temps raised to 0.5
  else:
      InDetAnnealingMaker = Trk__DetAnnealingMaker(name="InDetAnnealingMaker",
                                                   SetOfTemperatures=[64., 16., 4., 2., 1.5, 1.])  # not default
  ToolSvc += InDetAnnealingMaker
  if (InDetFlags.doPrintConfigurables()):
      printfunc(InDetAnnealingMaker)

  if (InDetFlags.primaryVertexSetup() == 'DefaultFastFinding' or
      InDetFlags.primaryVertexSetup() == 'DefaultFullFinding' or
      InDetFlags.primaryVertexSetup() == 'DefaultKalmanFinding'):

    from InDetMultipleVertexSeedFinderUtils.InDetMultipleVertexSeedFinderUtilsConf import InDet__InDetTrackZ0SortingTool
    InDetTrackZ0SortingTool =  InDet__InDetTrackZ0SortingTool(name = "InDetTrackZ0SortingTool")
    ToolSvc += InDetTrackZ0SortingTool
    if (InDetFlags.doPrintConfigurables()):
      printfunc (InDetTrackZ0SortingTool)

    if (not InDetFlags.useBeamConstraint()):
      import logging
      logger = logging.getLogger('InDet')
      logger.info('Using special 2D per-event seeding procedure for approximate 2D beam spot position')

      from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__Trk2dDistanceSeeder
      Trk2dDistanceSeeder = Trk__Trk2dDistanceSeeder(name                 = "Trk2dDistanceSeederFor2D",
                                                     SolveAmbiguityUsingZ = False)
      ToolSvc+=Trk2dDistanceSeeder
      if (InDetFlags.doPrintConfigurables()):
        printfunc (Trk2dDistanceSeeder)


      from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__Trk2DDistanceFinder
      Trk2DDistanceFinder = Trk__Trk2DDistanceFinder(name                = "Trk2DDistanceFinder",
                                                     Trk2dDistanceSeeder = Trk2dDistanceSeeder)

      ToolSvc+=Trk2DDistanceFinder
      if (InDetFlags.doPrintConfigurables()):
        printfunc (Trk2DDistanceFinder)

      from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__CrossDistancesSeedFinder
      InDet2DVtxSeedFinder = Trk__CrossDistancesSeedFinder(name                = "InDet2DCrossDistancesSeedFinder",
                                                           TrkDistanceFinder   = Trk2DDistanceFinder,
                                                           trackdistcutoff     = 1.,
                                                           trackdistexppower   = 2,
                                                           useweights          = True
                                                           # Mode1dFinder = # default, no setting needed
                                                           )
      ToolSvc+=InDet2DVtxSeedFinder
      if (InDetFlags.doPrintConfigurables()):
        printfunc (InDet2DVtxSeedFinder)

    def getDeprecatedInDetDetailedTrackSelectorTool() :
         from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
         sliding_window_track_selector = InDet__InDetDetailedTrackSelectorTool(name                                = "InDetDetailedTrackSelectorTool",
                                                                      pTMin                               = InDetPrimaryVertexingCuts.minPT(),
                                                                      IPd0Max                             = InDetPrimaryVertexingCuts.IPd0Max(),
                                                                      IPz0Max                             = InDetPrimaryVertexingCuts.IPz0Max(),
                                                                      z0Max                               = InDetPrimaryVertexingCuts.z0Max(),
                                                                      sigIPd0Max                          = InDetPrimaryVertexingCuts.sigIPd0Max(),
                                                                      sigIPz0Max                          = InDetPrimaryVertexingCuts.sigIPz0Max(),
                                                                      d0significanceMax                   = InDetPrimaryVertexingCuts.d0significanceMax(),
                                                                      z0significanceMax                   = InDetPrimaryVertexingCuts.z0significanceMax(),
                                                                      etaMax                              = InDetPrimaryVertexingCuts.etaMax(),
                                                                      useTrackSummaryInfo                 = InDetPrimaryVertexingCuts.useTrackSummaryInfo(),
                                                                      # nHitBLayer                          = InDetPrimaryVertexingCuts.nHitBLayer(),
                                                                      nHitPix                             = InDetPrimaryVertexingCuts.nHitPix(),
                                                                      nHolesPixel                         = InDetPrimaryVertexingCuts.nHolesPix(),
                                                                      # nHitBLayerPlusPix                   = InDetPrimaryVertexingCuts.nHitBLayerPlusPix(),
                                                                      nHitSct                             = InDetPrimaryVertexingCuts.nHitSct(),
                                                                      nHitSi                              = InDetPrimaryVertexingCuts.nHitSi(),
                                                                      nHitTrt                             = InDetPrimaryVertexingCuts.nHitTrt(),
                                                                      nHitTrtHighEFractionMax             = InDetPrimaryVertexingCuts.nHitTrtHighEFractionMax(),
                                                                      nHitTrtHighEFractionWithOutliersMax = InDetPrimaryVertexingCuts.nHitTrtHighEFractionWithOutliersMax(),
                                                                      useSharedHitInfo                    = InDetPrimaryVertexingCuts.useSharedHitInfo(),
                                                                      useTrackQualityInfo                 = InDetPrimaryVertexingCuts.useTrackQualityInfo(),
                                                                      fitChi2OnNdfMax                     = InDetPrimaryVertexingCuts.fitChi2OnNdfMax(),
                                                                      TrtMaxEtaAcceptance                 = InDetPrimaryVertexingCuts.TrtMaxEtaAcceptance(),
                                                                      # InDetTestBLayerTool                 = InDetRecTestBLayerTool,
                                                                      TrackSummaryTool                    = TrackingCommon.getInDetTrackSummaryTool(),
                                                                      Extrapolator                        = TrackingCommon.getInDetExtrapolator())
         ToolSvc += sliding_window_track_selector
         return sliding_window_track_selector

    def getInDetMultiSeedFinder() :
       if(InDetFlags.vertexSeedFinder() == 'DivisiveMultiSeedFinder'):
         if (not InDetFlags.useBeamConstraint()):
           from InDetMultipleVertexSeedFinder.InDetMultipleVertexSeedFinderConf import InDet__DivisiveMultiSeedFinder
           InDetMultiSeedFinder = InDet__DivisiveMultiSeedFinder(name               = "InDetDivisiveMultiSeedFinder",
                                                                 TrackSelector      = InDetTrackSelectorTool,
                                                                 SortingTool        = InDetTrackZ0SortingTool,
                                                                 IgnoreBeamSpot     = True,
                                                                 VertexSeedFinder   = InDet2DVtxSeedFinder,
                                                                 Extrapolator       = InDetExtrapolator,
                                                                 separationDistance = 5.)

         else:
           from InDetMultipleVertexSeedFinder.InDetMultipleVertexSeedFinderConf import InDet__DivisiveMultiSeedFinder
           InDetMultiSeedFinder = InDet__DivisiveMultiSeedFinder(name               = "InDetDivisiveMultiSeedFinder",
                                                                 TrackSelector      = InDetTrackSelectorTool,
                                                                 SortingTool        = InDetTrackZ0SortingTool,
                                                                 Extrapolator       = InDetExtrapolator,
                                                                 separationDistance = 5.)




       elif(InDetFlags.vertexSeedFinder() == 'HistogrammingMultiSeedFinder'):
         if (not InDetFlags.useBeamConstraint()):
           from InDetMultipleVertexSeedFinder.InDetMultipleVertexSeedFinderConf import InDet__HistogrammingMultiSeedFinder
           InDetMultiSeedFinder = InDet__HistogrammingMultiSeedFinder(name             = "InDetHistogrammingMultiSeedFinder",
                                                                      TrackSelector    = InDetTrackSelectorTool,
                                                                      IgnoreBeamSpot   = True,
                                                                      VertexSeedFinder = InDet2DVtxSeedFinder,
                                                                      Extrapolator     = InDetExtrapolator)
         else:
           from InDetMultipleVertexSeedFinder.InDetMultipleVertexSeedFinderConf import InDet__HistogrammingMultiSeedFinder
           InDetMultiSeedFinder = InDet__HistogrammingMultiSeedFinder(name          = "InDetHistogrammingMultiSeedFinder",
                                                                      TrackSelector = InDetTrackSelectorTool,
                                                                      Extrapolator  = InDetExtrapolator)


       elif(InDetFlags.vertexSeedFinder() == 'SlidingWindowMultiSeedFinder'):

         # now setup new stuff
         if (not InDetFlags.useBeamConstraint()):
           from InDetMultipleVertexSeedFinder.InDetMultipleVertexSeedFinderConf import InDet__SlidingWindowMultiSeedFinder
           InDetMultiSeedFinder = InDet__SlidingWindowMultiSeedFinder(name             = "InDetSlidingWindowMultiSeedFinder",
                                                                      clusterLength    = 8.*mm,
                                                                      TrackSelector    = getDeprecatedInDetDetailedTrackSelectorTool(),
                                                                      Extrapolator     = TrackingCommon.getInDetExtrapolator(),
                                                                      SortingTool      = InDetTrackZ0SortingTool,
                                                                      IgnoreBeamSpot   = True,
                                                                      VertexSeedFinder = InDet2DVtxSeedFinder
                                                                      # UseMaxInCluster = True
                                                                      )

         else:

           from InDetMultipleVertexSeedFinder.InDetMultipleVertexSeedFinderConf import InDet__SlidingWindowMultiSeedFinder
           InDetMultiSeedFinder = InDet__SlidingWindowMultiSeedFinder(name          = "InDetSlidingWindowMultiSeedFinder",
                                                                      clusterLength = 5.*mm,
                                                                      TrackSelector = getDeprecatedInDetDetailedTrackSelectorTool(),
                                                                      Extrapolator  = TrackingCommon.getInDetExtrapolator(),
                                                                      SortingTool   = InDetTrackZ0SortingTool,
                                                                      # UseMaxInCluster = True
                                                                      )

       ToolSvc += InDetMultiSeedFinder
       if (InDetFlags.doPrintConfigurables()):
         printfunc (InDetMultiSeedFinder)
       return InDetMultiSeedFinder

  # -----------------------------------------
  #
  # ------ load vertex fitter tool
  #
  # -----------------------------------------

  # load smoother in case of 'DefaultKalmanFinding', 'DefaultAdaptiveFinding' or 'IterativeFinding'
  if (InDetFlags.primaryVertexSetup() == 'DefaultKalmanFinding' or
      InDetFlags.primaryVertexSetup() == 'DefaultAdaptiveFinding' or
      InDetFlags.primaryVertexSetup() == 'IterativeFinding' or
      InDetFlags.primaryVertexSetup() == 'MedImgMultiFinding' or
      InDetFlags.primaryVertexSetup() == 'GaussIterativeFinding' ):
    from TrkVertexFitters.TrkVertexFittersConf import Trk__SequentialVertexSmoother
    InDetVertexSmoother = Trk__SequentialVertexSmoother(name = "InDetSequentialVertexSmoother")
    ToolSvc += InDetVertexSmoother
    if (InDetFlags.doPrintConfigurables()):
      printfunc (InDetVertexSmoother)

  if InDetFlags.primaryVertexSetup() == 'DefaultFastFinding':
    #
    # --- load fast Billoir fitter
    #
    from TrkVertexBilloirTools.TrkVertexBilloirToolsConf import Trk__FastVertexFitter
    InDetVxFitterTool = Trk__FastVertexFitter(name                   = "InDetFastVertexFitterTool",
                                              LinearizedTrackFactory = InDetLinFactory,
                                              Extrapolator           = InDetExtrapolator)

  elif InDetFlags.primaryVertexSetup() == 'DefaultFullFinding':
    #
    # --- load full billoir fitter
    #
    from TrkVertexBilloirTools.TrkVertexBilloirToolsConf import Trk__FullVertexFitter
    InDetVxFitterTool = Trk__FullVertexFitter(name                    = "InDetFullVertexFitterTool",
                                              LinearizedTrackFactory  = InDetLinFactory,
                                              Extrapolator            = InDetExtrapolator)

  elif InDetFlags.primaryVertexSetup() == 'DefaultKalmanFinding':
    #
    # --- case default finding with Kalman filter requested
    #

    from TrkVertexFitters.TrkVertexFittersConf import Trk__SequentialVertexFitter
    InDetVxFitterTool = Trk__SequentialVertexFitter(name                   = "InDetSequentialVxFitterTool",
                                                    LinearizedTrackFactory = InDetLinFactory,
                                                    VertexSmoother         = InDetVertexSmoother
                                                    # VertexUpdator   = # no setting required
                                                    )
  elif (InDetFlags.primaryVertexSetup() == 'MedImgMultiFinding') :

      from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__CrossDistancesSeedFinder
      InDet3DVtxSeedFinder = Trk__CrossDistancesSeedFinder(name                = "InDet3DCrossDistancesSeedFinder",
                                                           trackdistcutoff     = 1.,
                                                           trackdistexppower   = 2,
                                                           # Mode1dFinder = # default, no setting needed
                                                           )
      ToolSvc+=InDet3DVtxSeedFinder
      if (InDetFlags.doPrintConfigurables()):
        printfunc (InDet3DVtxSeedFinder)
    #
    # --- load configured adaptive vertex fitter (with simplified seed finder for start point)
    #
      from TrkVertexFitters.TrkVertexFittersConf import Trk__AdaptiveVertexFitter
      InDetVxFitterTool = Trk__AdaptiveVertexFitter(name                         = "InDetAdaptiveVxFitterTool",
                                                    SeedFinder                   = InDet3DVtxSeedFinder,
                                                    LinearizedTrackFactory       = InDetLinFactory,
                                                    ImpactPoint3dEstimator       = InDetImpactPoint3dEstimator,
                                                    AnnealingMaker               = InDetAnnealingMaker,
                                                    VertexSmoother               = InDetVertexSmoother)

  elif (InDetFlags.primaryVertexSetup() == 'DefaultAdaptiveFinding' or
        InDetFlags.primaryVertexSetup() == 'IterativeFinding' or
        InDetFlags.primaryVertexSetup() == 'GaussIterativeFinding' ) :
    #
    # --- load configured adaptive vertex fitter
    #
    from TrkVertexFitters.TrkVertexFittersConf import Trk__AdaptiveVertexFitter
    InDetVxFitterTool = Trk__AdaptiveVertexFitter(name                         = "InDetAdaptiveVxFitterTool",
                                                  SeedFinder                   = InDetVtxSeedFinder,
                                                  LinearizedTrackFactory       = InDetLinFactory,
                                                  ImpactPoint3dEstimator       = InDetImpactPoint3dEstimator,
                                                  AnnealingMaker               = InDetAnnealingMaker,
                                                  VertexSmoother               = InDetVertexSmoother)

  elif (InDetFlags.primaryVertexSetup() == 'AdaptiveMultiFinding' or
        InDetFlags.primaryVertexSetup() == 'GaussAdaptiveMultiFinding'):
    #
    # --- load adaptive multi vertex fitter
    #
    from TrkVertexFitters.TrkVertexFittersConf import Trk__AdaptiveMultiVertexFitter
    InDetVxFitterTool = Trk__AdaptiveMultiVertexFitter(name                         = "InDetAdaptiveMultiVertexFitter",
                                                       LinearizedTrackFactory       = InDetLinFactory,
                                                       ImpactPoint3dEstimator       = InDetImpactPoint3dEstimator,
                                                       AnnealingMaker               = InDetAnnealingMaker,
                                                       DoSmoothing                  = True) # false is default

  elif InDetFlags.primaryVertexSetup() == 'DefaultVKalVrtFinding':
    #
    # --- load vkal fitter
    #
    from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
    InDetVxFitterTool = Trk__TrkVKalVrtFitter(name = "InDetVKalVrtFitter")

  ToolSvc += InDetVxFitterTool
  if (InDetFlags.doPrintConfigurables()):
    printfunc (InDetVxFitterTool)

  # -----------------------------------------
  #
  # ----- load primary vertex finder tool
  #
  # -----------------------------------------

  if (InDetFlags.primaryVertexSetup() == 'AdaptiveMultiFinding' or
          InDetFlags.primaryVertexSetup() == 'GaussAdaptiveMultiFinding'):
    #
    # --- load adaptive multi primary vertex finder
    #
    if not InDetFlags.useActsPriVertexing():
      from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetAdaptiveMultiPriVxFinderTool
      InDetPriVxFinderTool = InDet__InDetAdaptiveMultiPriVxFinderTool(name              = "InDetAdaptiveMultiPriVxFinderTool",
                                                                      SeedFinder        = InDetVtxSeedFinder,
                                                                      VertexFitterTool  = InDetVxFitterTool,
                                                                      TrackSelector     = InDetTrackSelectorTool,
                                                                      useBeamConstraint = InDetFlags.useBeamConstraint(),
                                                                      selectiontype     = 0,
  								                                                    TracksMaxZinterval = 3,#mm 
                                                                      do3dSplitting     = InDetFlags.doPrimaryVertex3DFinding())
    else:
      from ActsGeometry.ActsTrackingGeometryTool import ActsTrackingGeometryTool
      from ActsPriVtxFinder.ActsPriVtxFinderConf import ActsAdaptiveMultiPriVtxFinderTool
      actsTrackingGeometryTool = getattr(ToolSvc,"ActsTrackingGeometryTool")
      actsExtrapolationTool = CfgMgr.ActsExtrapolationTool("ActsExtrapolationTool")
      actsExtrapolationTool.TrackingGeometryTool = actsTrackingGeometryTool
      InDetPriVxFinderTool = ActsAdaptiveMultiPriVtxFinderTool(name  = "ActsAdaptiveMultiPriVtxFinderTool",
                                                               TrackSelector     = InDetTrackSelectorTool,
                                                               useBeamConstraint = InDetFlags.useBeamConstraint(),
                                                               tracksMaxZinterval = 3,#mm 
                                                               do3dSplitting     = InDetFlags.doPrimaryVertex3DFinding(),
                                                               TrackingGeometryTool = actsTrackingGeometryTool,
                                                               ExtrapolationTool = actsExtrapolationTool)

  else:
    #
    # --- The default is to load the adaptive primary vertex finder
    #
    from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetIterativePriVxFinderTool
    InDetPriVxFinderTool = InDet__InDetIterativePriVxFinderTool(
        name="InDetIterativePriVxFinderTool",
        VertexFitterTool=InDetVxFitterTool,
        TrackSelector=InDetTrackSelectorTool,
        SeedFinder=InDetVtxSeedFinder,
        ImpactPoint3dEstimator=InDetImpactPoint3dEstimator,
        LinearizedTrackFactory=InDetLinFactory,
        useBeamConstraint=InDetFlags.useBeamConstraint(),
        significanceCutSeeding=12,
        maximumChi2cutForSeeding=49,
        maxVertices=200,
        doMaxTracksCut=InDetPrimaryVertexingCuts.doMaxTracksCut(),
        MaxTracks=InDetPrimaryVertexingCuts.MaxTracks()
    )

  ToolSvc += InDetPriVxFinderTool
  if (InDetFlags.doPrintConfigurables()):
    printfunc(InDetPriVxFinderTool)

  # -----------------------------------------
  #
  # ---- load primary vertex sorting tool and configure the right methodi
  #
  # -----------------------------------------

  #
  # --- set up hist svc
  #
  if (InDetFlags.primaryVertexSortingSetup() == 'SumPt2Sorting' or
      InDetFlags.primaryVertexSortingSetup() == 'VxProbSorting' or
      InDetFlags.primaryVertexSortingSetup() == 'NNSorting'):

    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    if not hasattr(svcMgr, 'THistSvc'):
      from GaudiSvc.GaudiSvcConf import THistSvc
      svcMgr += THistSvc()
      import os
      dataPathList = os.environ[ 'DATAPATH' ].split(os.pathsep)
      dataPathList.insert(0, os.curdir)
      from AthenaCommon.Utils.unixtools import FindFile
      VxProbFileName = "VxProbHisto.root"
      NNFileName     = "NNHisto.root"
      VxProbFile     = FindFile(VxProbFileName , dataPathList, os.R_OK )
      NNFile         = FindFile(NNFileName , dataPathList, os.R_OK )
      printfunc (VxProbFile)
      svcMgr.THistSvc.Input += ["VxProbHisto DATAFILE='"+VxProbFile+"' OPT='OLD'"]
      printfunc (NNFile)
      svcMgr.THistSvc.Input += ["NNHisto DATAFILE='"+NNFile+"' OPT='OLD'"]

    #
    # --- set up sorting
    #
    if InDetFlags.primaryVertexSortingSetup() == 'SumPt2Sorting':

      from TrkVertexWeightCalculators.TrkVertexWeightCalculatorsConf import Trk__SumPtVertexWeightCalculator
      VertexWeightCalculator = Trk__SumPtVertexWeightCalculator(name              = "InDetSumPtVertexWeightCalculator",
                                                                DoSumPt2Selection = True)

    elif InDetFlags.primaryVertexSortingSetup() == 'VxProbSorting':

      from TrkVertexWeightCalculators.TrkVertexWeightCalculatorsConf import Trk__VxProbVertexWeightCalculator
      VertexWeightCalculator = Trk__VxProbVertexWeightCalculator(name          = "InDetVxProbVertexWeightCalculator",
                                                                 HistogramPath = "/VxProbHisto/h_sTrkPdfminBias")

    elif InDetFlags.primaryVertexSortingSetup() == 'NNSorting':

      from TrkVertexWeightCalculators.TrkVertexWeightCalculatorsConf import Trk__NNVertexWeightCalculator
      VertexWeightCalculator = Trk__NNVertexWeightCalculator(name = "InDetNNVertexWeightCalculator",
                                                             HistoFilePath ="/NNHisto/")
    #
    ToolSvc += VertexWeightCalculator
    if InDetFlags.doPrintConfigurables():
      printfunc (VertexWeightCalculator)

    #
    # --- load sorting tool
    #
    from TrkVertexTools.TrkVertexToolsConf import Trk__VertexCollectionSortingTool
    VertexCollectionSortingTool = Trk__VertexCollectionSortingTool(name                   = "InDetVertexCollectionSortingTool",
                                                                   VertexWeightCalculator = VertexWeightCalculator)
    ToolSvc += VertexCollectionSortingTool
    if InDetFlags.doPrintConfigurables():
      printfunc (VertexCollectionSortingTool)

  elif InDetFlags.primaryVertexSortingSetup() == 'NoReSorting':

    VertexCollectionSortingTool = None

  else:
    import logging
    logger = logging.getLogger('InDet')
    logger.error('Sorting option '+InDetFlags.primaryVertexSortingSetup()+' not defined. ')


