
# ------------------------------------------------------------
# 
# ----------- Loading the Tracking Services 
#
# ------------------------------------------------------------

from InDetRecExample.TrackingCommon import createAndAddCondAlg
from InDetRecExample.TrackingCommon import getRIO_OnTrackErrorScalingCondAlg,getEventInfoKey, getInDetTRT_DriftCircleOnTrackTool

use_broad_cluster_any = InDetFlags.useBroadClusterErrors() and (not InDetFlags.doDBMstandalone())
use_broad_cluster_pix = InDetFlags.useBroadPixClusterErrors() and (not InDetFlags.doDBMstandalone())
use_broad_cluster_sct = InDetFlags.useBroadSCTClusterErrors() and (not InDetFlags.doDBMstandalone())
if use_broad_cluster_pix == None :
    use_broad_cluster_pix = use_broad_cluster_any
if use_broad_cluster_sct == None :
    use_broad_cluster_sct = use_broad_cluster_any

# detector specific settings will override the global setting:
use_broad_cluster_any = use_broad_cluster_pix or use_broad_cluster_sct

#load common NN tools for clustering and ROT creation
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
            print NeuralNetworkToHistoTool

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
        from InDetRecExample.TrackingCommon import createAndAddCondAlg,getPixelClusterNnCondAlg,getPixelClusterNnWithTrackCondAlg
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

        # special setup for DVRetracking mode
        # if InDetFlags.doDVRetracking() :
           # COOL binding
        from IOVDbSvc.CondDB import conddb
        if InDetFlags.doTIDE_RescalePixelCovariances() :
            if not conddb.folderRequested('/PIXEL/PixelClustering/PixelCovCorr'):
                # COOL binding
                conddb.addFolder("PIXEL_OFL","/PIXEL/PixelClustering/PixelCovCorr")

        if (InDetFlags.doPrintConfigurables()):
            print NnClusterizationFactory
elif InDetFlags.doPixelClusterSplitting():
    if not hasattr(ToolSvc, "PixelLorentzAngleTool"):
        from SiLorentzAngleTool.PixelLorentzAngleToolSetup import PixelLorentzAngleToolSetup
        pixelLorentzAngleToolSetup = PixelLorentzAngleToolSetup()

    from SiClusterizationTool.SiClusterizationToolConf import InDet__TruthClusterizationFactory
    NnClusterizationFactory = InDet__TruthClusterizationFactory( name                 = "TruthClusterizationFactory",
                                                                 PixelLorentzAngleTool= ToolSvc.PixelLorentzAngleTool)
    ToolSvc += NnClusterizationFactory
    if (InDetFlags.doPrintConfigurables()):
        print NnClusterizationFactory


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

    if PixelClusterOnTrackToolDigital != None :
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
        from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__SCT_ClusterOnTrackTool
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
    
    if InDetFlags.kalmanUpdator() is "fast" :
        from TrkMeasurementUpdator_xk.TrkMeasurementUpdator_xkConf import Trk__KalmanUpdator_xk
        InDetUpdator = Trk__KalmanUpdator_xk(name = 'InDetUpdator')
    elif InDetFlags.kalmanUpdator() is "weight" :
        from TrkMeasurementUpdator.TrkMeasurementUpdatorConf import Trk__KalmanWeightUpdator 
        InDetUpdator = Trk__KalmanWeightUpdator(name='InDetUpdator')
    elif InDetFlags.kalmanUpdator() is "smatrix" :
        from TrkMeasurementUpdator.TrkMeasurementUpdatorConf import Trk__KalmanUpdatorSMatrix 
        InDetUpdator = Trk__KalmanUpdatorSMatrix(name='InDetUpdator')
    elif InDetFlags.kalmanUpdator() is "amg" :
        from TrkMeasurementUpdator.TrkMeasurementUpdatorConf import Trk__KalmanUpdatorAmg 
        InDetUpdator = Trk__KalmanUpdatorAmg(name = 'InDetUpdator')
    else :
        from TrkMeasurementUpdator.TrkMeasurementUpdatorConf import Trk__KalmanUpdator
        InDetUpdator = Trk__KalmanUpdator(name = 'InDetUpdator')
    ToolSvc += InDetUpdator
    if (InDetFlags.doPrintConfigurables()):
      print      InDetUpdator
    #
    # ---------- control loading of the gsf updator
    #
    if InDetFlags.trackFitterType() is 'GaussianSumFilter' :
        #
        # Load the Gsf Measurement Updator
        #
        from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfMeasurementUpdator
        InDetGsfMeasurementUpdator = Trk__GsfMeasurementUpdator( name    = 'InDetGsfMeasurementUpdator',
                                                            Updator = InDetUpdator )
        ToolSvc += InDetGsfMeasurementUpdator
        if (InDetFlags.doPrintConfigurables()):
          print      InDetGsfMeasurementUpdator
        
#
# ----------- control laoding extrapolation
#
if InDetFlags.loadExtrapolator():
    #
    # set up geometry
    #
    from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
    #
    # get propagator
    #
    if InDetFlags.propagatorType() is "STEP":
        from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator as Propagator
    else:
        from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
    #   
    InDetPropagator = Propagator(name = 'InDetPropagator')
    if InDetFlags.propagatorType() is "RungeKutta":
        InDetPropagator.AccuracyParameter = 0.0001
        InDetPropagator.MaxStraightLineStep = .004  # Fixes a failed fit 
    ToolSvc += InDetPropagator
    
    # set up the propagator for outside ID (A.S. needed as a fix for 14.5.0 )
    #from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator as StepPropagator
    #InDetStepPropagator = StepPropagator(name = 'InDetStepPropagator')
    #ToolSvc += InDetStepPropagator
    
    if (InDetFlags.doPrintConfigurables()):
      print      InDetPropagator
    #
    # Setup the Navigator (default, could be removed)
    #
    from TrkExTools.TrkExToolsConf import Trk__Navigator
    InDetNavigator = Trk__Navigator(name                = 'InDetNavigator',
                                    TrackingGeometrySvc = AtlasTrackingGeometrySvc)
    ToolSvc += InDetNavigator
    if (InDetFlags.doPrintConfigurables()):
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
#    if InDetFlags.doDBMstandalone():
#        InDetMaterialUpdator.EnergyLoss          = False
#        InDetMaterialUpdator.ForceMomentum       = False

    ToolSvc += InDetMaterialUpdator

    from TrkExTools.TrkExToolsConf import Trk__MultipleScatteringUpdator 
    InDetMultipleScatteringUpdator = Trk__MultipleScatteringUpdator(name = "InDetMultipleScatteringUpdator", 
 		                                                    UseTrkUtils = False) 
 		 
    ToolSvc += InDetMultipleScatteringUpdator 
 		     
    if (InDetFlags.doPrintConfigurables()): 
        print      InDetMultipleScatteringUpdator 


    
    if (InDetFlags.doPrintConfigurables()):
      print      InDetMaterialUpdator
      
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
    if (InDetFlags.doPrintConfigurables()):
      print      InDetExtrapolator  

#
# ----------- control loading of fitters
#
if InDetFlags.loadFitter():

    if InDetFlags.doBremRecovery() and InDetFlags.trackFitterType() is 'GlobalChi2Fitter' :
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
            #     print InDetTrackFitterTRT
        # if InDetFlags.doLowPt() or (InDetFlags.doTrackSegmentsPixel() and InDetFlags.doMinBias()):
            # ToolSvc+=InDetTrackFitterLowPt
            # if (InDetFlags.doPrintConfigurables()):
            #    print InDetTrackFitterLowPt

#
# ----------- load association tool from Inner Detector to handle pixel ganged ambiguities
#
if InDetFlags.loadAssoTool():

    from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
    InDetPrdAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels(name                           = "InDetPrdAssociationTool",
                                                                          PixelClusterAmbiguitiesMapName = InDetKeys.GangedPixelMap(),
                                                                          addTRToutliers                 = True)
    #InDetPrdAssociationTool.OutputLevel = VERBOSE
    ToolSvc += InDetPrdAssociationTool
    if (InDetFlags.doPrintConfigurables()):
      print      InDetPrdAssociationTool
    
#
# ----------- control loading of SummaryTool
#

if InDetFlags.loadSummaryTool():

    from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
    AtlasTrackSummaryTool = AtlasTrackSummaryTool()
    ToolSvc += AtlasTrackSummaryTool

    from PixelConditionsTools.PixelConditionsSummaryToolSetup import PixelConditionsSummaryToolSetup
    pixelConditionsSummaryToolSetup = PixelConditionsSummaryToolSetup()
    pixelConditionsSummaryToolSetup.setUseConditions(True)
    pixelConditionsSummaryToolSetup.setUseDCSState((globalflags.DataSource=='data') and InDetFlags.usePixelDCS())
    pixelConditionsSummaryToolSetup.setUseByteStream((globalflags.DataSource=='data'))
    pixelConditionsSummaryToolSetup.setUseTDAQ(athenaCommonFlags.isOnline())
    pixelConditionsSummaryToolSetup.setUseDeadMap((not athenaCommonFlags.isOnline()))
    pixelConditionsSummaryToolSetup.setup()
    InDetPixelConditionsSummaryTool = pixelConditionsSummaryToolSetup.getTool()

    if InDetFlags.usePixelDCS():
        InDetPixelConditionsSummaryTool.IsActiveStates = [ 'READY', 'ON', 'UNKNOWN', 'TRANSITION', 'UNDEFINED' ]
        InDetPixelConditionsSummaryTool.IsActiveStatus = [ 'OK', 'WARNING', 'ERROR', 'FATAL' ]

    if (InDetFlags.doPrintConfigurables()):
        print InDetPixelConditionsSummaryTool

    #
    # Loading Pixel test tool
    #
    from InDetTestPixelLayer.InDetTestPixelLayerConf import InDet__InDetTestPixelLayerTool
    InDetTestPixelLayerTool = InDet__InDetTestPixelLayerTool(name = "InDetTestPixelLayerTool",
                                                             PixelSummaryTool = InDetPixelConditionsSummaryTool,
                                                             CheckActiveAreas=InDetFlags.checkDeadElementsOnTrack(),
                                                             CheckDeadRegions=InDetFlags.checkDeadElementsOnTrack(),
                                                             CheckDisabledFEs=InDetFlags.checkDeadElementsOnTrack())
    ToolSvc += InDetTestPixelLayerTool
    if (InDetFlags.doPrintConfigurables()):
        print InDetTestPixelLayerTool

    #
    # Loading Configurable HoleSearchTool
    #
    from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
    InDetHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "InDetHoleSearchTool",
                                                          Extrapolator = InDetExtrapolator,
                                                          PixelSummaryTool = InDetPixelConditionsSummaryTool,
                                                          usePixel      = DetFlags.haveRIO.pixel_on(),
                                                          useSCT        = DetFlags.haveRIO.SCT_on(),                                                          
                                                          CountDeadModulesAfterLastHit = True,
                                                          PixelLayerTool = InDetTestPixelLayerTool)

    if (DetFlags.haveRIO.SCT_on()):
      InDetHoleSearchTool.SctSummaryTool = InDetSCT_ConditionsSummaryTool
    else:
      InDetHoleSearchTool.SctSummaryTool = None

    if InDetFlags.doCosmics:
        InDetHoleSearchTool.Cosmics = True

    ToolSvc += InDetHoleSearchTool
    if (InDetFlags.doPrintConfigurables()):
      print    InDetHoleSearchTool
          
    #
    # Load BLayer tool
    #
    InDetRecTestBLayerTool = None
    if DetFlags.haveRIO.pixel_on() :
        from InDetTestBLayer.InDetTestBLayerConf import InDet__InDetTestBLayerTool
        InDetRecTestBLayerTool = InDet__InDetTestBLayerTool(name            = "InDetRecTestBLayerTool",
                                                            PixelSummaryTool = InDetPixelConditionsSummaryTool,
                                                            Extrapolator    = InDetExtrapolator)
        ToolSvc += InDetRecTestBLayerTool
        if (InDetFlags.doPrintConfigurables()):
            print  InDetRecTestBLayerTool
    #
    # Configurable version of TRT_ElectronPidTools
    #
    InDetTRT_ElectronPidTool = None
    if DetFlags.haveRIO.TRT_on() and not InDetFlags.doSLHC() and not InDetFlags.doHighPileup() \
            and not InDetFlags.useExistingTracksAsInput(): # TRT_RDOs (used byt the TRT_LocalOccupancy tool) are not present in ESD

        from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbTool
        InDetTRTCalDbTool = TRT_CalDbTool(name = "TRT_CalDbTool")

        # Straw status DB Tool
        from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummaryTool
        InDetTRTStrawStatusSummaryTool = TRT_StrawStatusSummaryTool(name = "TRT_StrawStatusSummaryTool",
                                                                    isGEANT4=(globalflags.DataSource == 'geant4'))

        from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import InDet__TRT_LocalOccupancy
        InDetTRT_LocalOccupancy = InDet__TRT_LocalOccupancy(	  name 				="InDet_TRT_LocalOccupancy",
                                                                  isTrigger			= False,
                                                                  TRTCalDbTool = InDetTRTCalDbTool,
                                                                  TRTStrawStatusSummaryTool = InDetTRTStrawStatusSummaryTool )


        ToolSvc += InDetTRT_LocalOccupancy
        if (InDetFlags.doPrintConfigurables()):
         print InDetTRT_LocalOccupancy

        from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import InDet__TRT_ElectronPidToolRun2
        InDetTRT_ElectronPidTool = InDet__TRT_ElectronPidToolRun2(name   = "InDetTRT_ElectronPidTool",
                                                                  TRT_LocalOccupancyTool 	= InDetTRT_LocalOccupancy,
                                                                  TRTStrawSummaryTool     	= InDetTRTStrawStatusSummaryTool,
                                                                  isData = (globalflags.DataSource == 'data'))



        ToolSvc += InDetTRT_ElectronPidTool
        if (InDetFlags.doPrintConfigurables()):
         print InDetTRT_ElectronPidTool


    import InDetRecExample.TRTCommon
    InDetTRT_dEdxTool = InDetRecExample.TRTCommon.getInDetTRT_dEdxTool()

    if (InDetTRT_dEdxTool != None and InDetFlags.doPrintConfigurables()):
        print InDetTRT_dEdxTool


    #
    # Configurable version of PixelToTPIDTOol
    #
    InDetPixelToTPIDTool = None
    if DetFlags.haveRIO.pixel_on() and not InDetFlags.doSLHC():
        from PixelToTPIDTool.PixelToTPIDToolConf import InDet__PixelToTPIDTool
        InDetPixelToTPIDTool = InDet__PixelToTPIDTool(name = "InDetPixelToTPIDTool")
        if not athenaCommonFlags.isOnline():
            InDetPixelToTPIDTool.ReadFromCOOL = True
        else:
            if ( globalflags.DataSource == 'data' ):
                InDetPixelToTPIDTool.CalibrationFile="dtpar_signed_234.txt"
            else:
                InDetPixelToTPIDTool.CalibrationFile="mcpar_signed_234.txt"

        ToolSvc += InDetPixelToTPIDTool
        if (InDetFlags.doPrintConfigurables()):
            print  InDetPixelToTPIDTool
    
    #
    # Configrable version of loading the InDetTrackSummaryHelperTool
    #
    from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
    if DetFlags.haveRIO.TRT_on():
        InDetTrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name            = "InDetSummaryHelper",
                                                                         AssoTool        = InDetPrdAssociationTool,
                                                                         PixelToTPIDTool = None,         # we don't want to use those tools during pattern
                                                                         TestBLayerTool  = None,         # we don't want to use those tools during pattern
                                                                         #PixelToTPIDTool = InDetPixelToTPIDTool,
                                                                         #TestBLayerTool  = InDetRecTestBLayerTool,
                                                                         RunningTIDE_Ambi = InDetFlags.doTIDE_Ambi(),
                                                                         DoSharedHits    = False,
                                                                         HoleSearch      = InDetHoleSearchTool,
                                                                         usePixel        = DetFlags.haveRIO.pixel_on(),
                                                                         useSCT          = DetFlags.haveRIO.SCT_on(),
                                                                         useTRT          = DetFlags.haveRIO.TRT_on())
    else:
        InDetTrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name            = "InDetSummaryHelper",
                                                                         AssoTool        = InDetPrdAssociationTool,
                                                                         PixelToTPIDTool = None,         # we don't want to use those tools during pattern
                                                                         TestBLayerTool  = None,         # we don't want to use those tools during pattern
                                                                         #PixelToTPIDTool = InDetPixelToTPIDTool,
                                                                         #TestBLayerTool  = InDetRecTestBLayerTool,
                                                                         RunningTIDE_Ambi = InDetFlags.doTIDE_Ambi(),
                                                                         DoSharedHits    = False,
                                                                         HoleSearch      = InDetHoleSearchTool,
                                                                         usePixel        = DetFlags.haveRIO.pixel_on(),
                                                                         useSCT          = DetFlags.haveRIO.SCT_on(),
                                                                         useTRT          = DetFlags.haveRIO.TRT_on(),
                                                                         TRTStrawSummarySvc = ""
                                                                         )

    ToolSvc += InDetTrackSummaryHelperTool
    if (InDetFlags.doPrintConfigurables()):
      print      InDetTrackSummaryHelperTool                                                                   
    
    #
    # Configurable version of TrkTrackSummaryTool: no TRT_PID tool needed here (no shared hits)
    #
    from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
    InDetTrackSummaryTool = Trk__TrackSummaryTool(name = "InDetTrackSummaryTool",
                                                  InDetSummaryHelperTool = InDetTrackSummaryHelperTool,
                                                  doSharedHits           = False,
                                                  doHolesInDet           = True,
                                                  TRT_ElectronPidTool    = None,         # we don't want to use those tools during pattern
                                                  TRT_ToT_dEdxTool       = None,         # dito
                                                  PixelToTPIDTool        = None)         # we don't want to use those tools during pattern
                                                  #TRT_ElectronPidTool    = InDetTRT_ElectronPidTool,
                                                  #PixelToTPIDTool        = InDetPixelToTPIDTool)
    #InDetTrackSummaryTool.OutputLevel = VERBOSE
    ToolSvc += InDetTrackSummaryTool
    if (InDetFlags.doPrintConfigurables()):
      print      InDetTrackSummaryTool

    #
    # --- we may need another instance for shared hits computation
    #
    from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
    if DetFlags.haveRIO.TRT_on():
        InDetTrackSummaryHelperToolSharedHits = InDet__InDetTrackSummaryHelperTool(name            = "InDetSummaryHelperSharedHits",
                                                                                   AssoTool        = InDetPrdAssociationTool,
                                                                                   PixelToTPIDTool = InDetPixelToTPIDTool,
                                                                                   TestBLayerTool  = InDetRecTestBLayerTool,
                                                                                   DoSharedHits    = InDetFlags.doSharedHits(),
                                                                                   DoSharedHitsTRT = InDetFlags.doSharedHits(),
                                                                                   RunningTIDE_Ambi = InDetFlags.doTIDE_Ambi(),
                                                                                   HoleSearch      = InDetHoleSearchTool,
                                                                                   usePixel        = DetFlags.haveRIO.pixel_on(),
                                                                                   useSCT          = DetFlags.haveRIO.SCT_on(),
                                                                                   useTRT          = DetFlags.haveRIO.TRT_on())
    else:
        InDetTrackSummaryHelperToolSharedHits = InDet__InDetTrackSummaryHelperTool(name            = "InDetSummaryHelperSharedHits",
                                                                                   AssoTool        = InDetPrdAssociationTool,
                                                                                   PixelToTPIDTool = InDetPixelToTPIDTool,
                                                                                   TestBLayerTool  = InDetRecTestBLayerTool,
                                                                                   DoSharedHits    = InDetFlags.doSharedHits(),
                                                                                   RunningTIDE_Ambi = InDetFlags.doTIDE_Ambi(),
                                                                                   HoleSearch      = InDetHoleSearchTool,
                                                                                   usePixel        = DetFlags.haveRIO.pixel_on(),
                                                                                   useSCT          = DetFlags.haveRIO.SCT_on(),
                                                                                   useTRT          = DetFlags.haveRIO.TRT_on(),
                                                                                   TRTStrawSummarySvc = ""
                                                                                   )

    #InDetTrackSummaryHelperToolSharedHits.OutputLevel = VERBOSE
    ToolSvc += InDetTrackSummaryHelperToolSharedHits
    if (InDetFlags.doPrintConfigurables()):
        print  InDetTrackSummaryHelperToolSharedHits
    #
    # Configurable version of TrkTrackSummaryTool
    #
    from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
    InDetTrackSummaryToolSharedHits = Trk__TrackSummaryTool(name = "InDetTrackSummaryToolSharedHits",
                                                            InDetSummaryHelperTool = InDetTrackSummaryHelperToolSharedHits,
                                                            doSharedHits           = InDetFlags.doSharedHits(),
                                                            doHolesInDet           = True,
                                                            TRT_ElectronPidTool    = InDetTRT_ElectronPidTool,
                                                            TRT_ToT_dEdxTool       = InDetTRT_dEdxTool,
                                                            TRTdEdx_DivideByL      = True, # default is True
                                                            TRTdEdx_useHThits      = True, # default is True
                                                            TRTdEdx_corrected      = True, # default is True
                                                            minTRThitsForTRTdEdx   = 1,    # default is 1
                                                            PixelToTPIDTool        = InDetPixelToTPIDTool)

    #InDetTrackSummaryToolSharedHits.OutputLevel = VERBOSE
    ToolSvc += InDetTrackSummaryToolSharedHits
    if (InDetFlags.doPrintConfigurables()):
        print  InDetTrackSummaryToolSharedHits

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
      print      InDetPatternPropagator
    #
    # fast Kalman updator tool
    #
    from TrkMeasurementUpdator_xk.TrkMeasurementUpdator_xkConf import Trk__KalmanUpdator_xk
    InDetPatternUpdator = Trk__KalmanUpdator_xk(name = 'InDetPatternUpdator')
    ToolSvc += InDetPatternUpdator
    if (InDetFlags.doPrintConfigurables()):
      print      InDetPatternUpdator
    
    # ------------------------------------------------------------
    #
    # ----------- Loading of tools for TRT extensions
    #
    # ------------------------------------------------------------

    #
    # TRT detector elements road builder
    #
    if not InDetFlags.doDBMstandalone():
        from TRT_DetElementsRoadTool_xk.TRT_DetElementsRoadTool_xkConf import InDet__TRT_DetElementsRoadMaker_xk
        InDetTRTDetElementsRoadMaker =  InDet__TRT_DetElementsRoadMaker_xk(name                  = 'InDetTRT_RoadMaker',
                                                                           TRTManagerLocation    = InDetKeys.TRT_Manager(),
                                                                           RoadWidth             = 20.,
                                                                           PropagatorTool        = InDetPatternPropagator)
        ToolSvc += InDetTRTDetElementsRoadMaker
        if (InDetFlags.doPrintConfigurables()):
            print      InDetTRTDetElementsRoadMaker

    #
    # TRT segment minimum number of drift circles tool
    #
    from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetTrtDriftCircleCutTool
    InDetTRTDriftCircleCut = InDet__InDetTrtDriftCircleCutTool(name                   = 'InDetTRTDriftCircleCut',
                                                               MinOffsetDCs           = 5,
                                                               UseNewParameterization = True,  # Use Thomas's new parameterization by default
                                                               UseActiveFractionSvc   = DetFlags.haveRIO.TRT_on())
    
    ToolSvc += InDetTRTDriftCircleCut
    if (InDetFlags.doPrintConfigurables()):
        print   InDetTRTDriftCircleCut

    #
    # Local combinatorial track finding using space point seed and detector element road 
    #
    from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiCombinatorialTrackFinder_xk
    InDetSiComTrackFinder = InDet__SiCombinatorialTrackFinder_xk(name                  = 'InDetSiComTrackFinder',
                                                                 PropagatorTool        = InDetPatternPropagator,
                                                                 UpdatorTool           = InDetPatternUpdator,
                                                                 RIOonTrackTool        = InDetRotCreatorDigital,##NS HERE
                                                                 AssosiationTool       = InDetPrdAssociationTool,
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

    if InDetFlags.doDBM():
        InDetSiComTrackFinderDBM = InDet__SiCombinatorialTrackFinder_xk(name                  = 'InDetSiComTrackFinderDBM',
                                                                        PropagatorTool        = InDetPatternPropagator,
                                                                        UpdatorTool           = InDetPatternUpdator,
                                                                        RIOonTrackTool        = InDetRotCreatorDBM,
                                                                        AssosiationTool       = InDetPrdAssociationTool,
                                                                        usePixel              = True,
                                                                        useSCT                = False,
                                                                        PixelClusterContainer = InDetKeys.PixelClusters(),
                                                                        SCT_ClusterContainer  = InDetKeys.SCT_Clusters(),
                                                                        MagneticFieldMode     = "NoField",
                                                                        TrackQualityCut       = 9.3
                                                                        )
    if InDetFlags.doDBMstandalone():
        InDetSiComTrackFinder.MagneticFieldMode     =  "NoField"
    if (DetFlags.haveRIO.SCT_on()):
      InDetSiComTrackFinder.SctSummaryTool = InDetSCT_ConditionsSummaryTool
    else:
      InDetSiComTrackFinder.SctSummaryTool = None

    if (InDetFlags.doPrintConfigurables()):
      print InDetSiComTrackFinder
      if InDetFlags.doDBM():
          print InDetSiComTrackFinderDBM

# ------------------------------------------------------------
#
# ----------- Track extension to TRT tool for New Tracking
#
# ------------------------------------------------------------
#if InDetFlags.doPattern() and DetFlags.haveRIO.TRT_on() and InDetFlags.doTRTExtension():
if InDetFlags.doPattern() and DetFlags.haveRIO.TRT_on():
    # if new tracking is OFF then xk extension type has to be used!!
    if (InDetFlags.trtExtensionType() is 'xk') or (not InDetFlags.doNewTracking()) :

        #
        # TRT segment minimum number of drift circles tool just for Pattern Reco stage
        #
        from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetTrtDriftCircleCutTool
        InDetTRTDriftCircleCutForPatternReco = InDet__InDetTrtDriftCircleCutTool(name                   = 'InDetTRTDriftCircleCutForPatternReco',
                                                                                 MinOffsetDCs           = 5,
                                                                                 UseNewParameterization = InDetNewTrackingCuts.useNewParameterizationTRT(),  # Use new parameterization only for high lumi
                                                                                 UseActiveFractionSvc   = DetFlags.haveRIO.TRT_on())
            
        ToolSvc += InDetTRTDriftCircleCutForPatternReco
        if (InDetFlags.doPrintConfigurables()):
            print   InDetTRTDriftCircleCutForPatternReco
            
        #
        # load normal extension code
        #
        if InDetFlags.doCosmics():
            from TRT_TrackExtensionTool_xk.TRT_TrackExtensionTool_xkConf import InDet__TRT_TrackExtensionToolCosmics
            InDetTRTExtensionTool = InDet__TRT_TrackExtensionToolCosmics(name                  = 'InDetTRT_ExtensionToolCosmics',
                                                                         Propagator            = InDetPropagator,
                                                                         Extrapolator          = InDetExtrapolator,
                                                                         TRT_ClustersContainer = InDetKeys.TRT_DriftCircles(),
                                                                         SearchNeighbour       = False, #needs debugging!!!
                                                                         RoadWidth             = 10.) 
        else:
            from TRT_TrackExtensionTool_xk.TRT_TrackExtensionTool_xkConf import InDet__TRT_TrackExtensionTool_xk
            InDetTRTExtensionTool =  InDet__TRT_TrackExtensionTool_xk(name                  = 'InDetTRT_ExtensionTool',
                                                                      TRT_ClustersContainer = InDetKeys.TRT_DriftCircles(),
                                                                      TrtManagerLocation    = InDetKeys.TRT_Manager(),
                                                                      PropagatorTool        = InDetPatternPropagator,
                                                                      UpdatorTool           = InDetPatternUpdator, 
                                                                      DriftCircleCutTool    = InDetTRTDriftCircleCutForPatternReco,
                                                                      UseDriftRadius        = not InDetFlags.noTRTTiming(),
                                                                      RoadTool              = InDetTRTDetElementsRoadMaker,
                                                                      MinNumberDriftCircles = InDetNewTrackingCuts.minTRTonTrk(),
                                                                      ScaleHitUncertainty   = 2.,
                                                                      RoadWidth             = 20.,
                                                                      UseParameterization   = InDetNewTrackingCuts.useParameterizedTRTCuts() )
            # --- single beam running, open cuts
            if InDetFlags.doBeamHalo() or InDetFlags.doBeamGas():
                InDetTRTExtensionTool.maxImpactParameter  = 500

    elif InDetFlags.trtExtensionType() is 'DAF' :
        #
        # laod TRT Competing ROT tool
        #
        from TrkDeterministicAnnealingFilter.TrkDeterministicAnnealingFilterConf import Trk__DAF_SimpleWeightCalculator
        InDetWeightCalculator =  Trk__DAF_SimpleWeightCalculator( name = 'InDetWeightCalculator')
        ToolSvc += InDetWeightCalculator
        if (InDetFlags.doPrintConfigurables()):
            print InDetWeightCalculator
        #
        from InDetCompetingRIOsOnTrackTool.InDetCompetingRIOsOnTrackToolConf import InDet__CompetingTRT_DriftCirclesOnTrackTool
        InDetCompetingTRT_DC_Tool =  InDet__CompetingTRT_DriftCirclesOnTrackTool( name                                  = 'InDetCompetingTRT_DC_Tool',
                                                                                  Extrapolator                          = InDetExtrapolator,
                                                                                  ToolForWeightCalculation              = InDetWeightCalculator,
                                                                                  ToolForTRT_DriftCircleOnTrackCreation = InDetRotCreator.ToolTRT_DriftCircle)
        ToolSvc += InDetCompetingTRT_DC_Tool
        if (InDetFlags.doPrintConfigurables()):
            print InDetCompetingTRT_DC_Tool
  
        from TRT_TrackExtensionTool_DAF.TRT_TrackExtensionTool_DAFConf import InDet__TRT_TrackExtensionTool_DAF
        InDetTRTExtensionTool =  InDet__TRT_TrackExtensionTool_DAF(name                        = 'InDetTRT_ExtensionTool',
                                                                   TRT_DriftCircleContainer    = InDetKeys.TRT_DriftCircles(),
                                                                   CompetingDriftCircleTool    = InDetCompetingTRT_DC_Tool,
                                                                   PropagatorTool              = InDetPatternPropagator,
                                                                   RoadTool                    = InDetTRTDetElementsRoadMaker)
        
    # --- print out the final configuration
    ToolSvc += InDetTRTExtensionTool
    if (InDetFlags.doPrintConfigurables()):
        print InDetTRTExtensionTool

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
        print      InDetScoringToolCosmics
    
    from InDetAmbiTrackSelectionTool.InDetAmbiTrackSelectionToolConf import InDet__InDetAmbiTrackSelectionTool
    InDetAmbiTrackSelectionToolCosmics = InDet__InDetAmbiTrackSelectionTool(name                  = 'InDetAmbiTrackSelectionToolCosmics',
                                                                            AssociationTool       = InDetPrdAssociationTool,
                                                                            minNotShared          = 3,
                                                                            minHits               = 0,
                                                                            maxShared             = 0,
                                                                            maxTracksPerSharedPRD = 10,
                                                                            Cosmics               = True,
                                                                            DriftCircleCutTool    = InDetTRTDriftCircleCut,
                                                                            UseParameterization   = False)
    
    ToolSvc += InDetAmbiTrackSelectionToolCosmics
    if (InDetFlags.doPrintConfigurables()):
        print      InDetAmbiTrackSelectionToolCosmics


    from AthenaCommon import CfgGetter
    from TrkAmbiguityProcessor.TrkAmbiguityProcessorConf import Trk__SimpleAmbiguityProcessorTool
    InDetAmbiguityProcessorCosmics = Trk__SimpleAmbiguityProcessorTool(name             = 'InDetAmbiguityProcessorCosmics',
                                                                       ScoringTool      = InDetScoringToolCosmics,
                                                                       Fitter           = CfgGetter.getPublicTool('InDetTrackFitter'),
                                                                       SelectionTool    = InDetAmbiTrackSelectionToolCosmics,
                                                                       SuppressTrackFit = True,
                                                                       ForceRefit       = False,
                                                                       RefitPrds        = False)
    
    ToolSvc += InDetAmbiguityProcessorCosmics
    if (InDetFlags.doPrintConfigurables()):
        print      InDetAmbiguityProcessorCosmics
        
        
# ------------------------------------------------------------
#
# ----------- Loading of tools for truth comparison
#
# ------------------------------------------------------------

# id rec stat processing and trk+pixel ntuple creation need this tool if truth is on
if InDetFlags.doTruth() and (InDetFlags.doStatistics() or InDetFlags.doStandardPlots() or InDetFlags.doPhysValMon() or InDetFlags.doNtupleCreation()):
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
        print InDetTruthToTrack

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
        print InDetTrackSelectorTool


if (InDetFlags.doVertexFinding() or InDetFlags.doVertexFindingForMonitoring()) or InDetFlags.doSplitVertexFindingForMonitoring() and InDetFlags.primaryVertexSetup() != 'DummyVxFinder':
  #
  # --- load linearized track factory
  #
  from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
  InDetLinFactory = Trk__FullLinearizedTrackFactory(name              = "InDetFullLinearizedTrackFactory",
                                                    Extrapolator      = InDetExtrapolator )
  ToolSvc += InDetLinFactory
  if (InDetFlags.doPrintConfigurables()):
    print InDetLinFactory

    
  #
  # --- load other tools if needed
  #
  if (InDetFlags.primaryVertexSetup() == 'DefaultAdaptiveFinding' or
      InDetFlags.primaryVertexSetup() == 'IterativeFinding' or
      InDetFlags.primaryVertexSetup() == 'AdaptiveMultiFinding' or
      InDetFlags.primaryVertexSetup() == 'MedImgMultiFinding' or 
      InDetFlags.primaryVertexSetup() == 'GaussIterativeFinding' or
      InDetFlags.primaryVertexSetup() == 'GaussAdaptiveMultiFinding'):
    #
    # --- load configured Seed finder
    #
    if (InDetFlags.primaryVertexSetup() == 'GaussIterativeFinding' or
        InDetFlags.primaryVertexSetup() == 'GaussAdaptiveMultiFinding'):
      from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__GaussianTrackDensity
      GaussDensityEstimator = Trk__GaussianTrackDensity( name = "GaussianDensity" )
      ToolSvc += GaussDensityEstimator

      from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__TrackDensitySeedFinder
      InDetVtxSeedFinder = Trk__TrackDensitySeedFinder( name = "GaussianDensitySeedFinder",
                                                        DensityEstimator = GaussDensityEstimator )
    elif (InDetFlags.primaryVertexSetup() == 'MedImgMultiFinding'):
      from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__LocalMax1DClusterFinder, Trk__VertexImageMaker
      InDetMedImgClusterFinder = Trk__LocalMax1DClusterFinder( name            = "InDetMedImgClusterFinder",
                                                               weightThreshold = 1500.0,
                                                               mergeParameter  = 0.95,
                                                               clusterWindowXY = 0.34,
                                                               refineZ         = True,
                                                               gaussianWindow  = True)
      ToolSvc += InDetMedImgClusterFinder
      InDetMedImgMaker = Trk__VertexImageMaker( name                           = "InDetMedImgMaker",
                                                xbins                          = 32,
                                                ybins                          = 32,
                                                zbins                          = 2048,
                                                xrange                         = 2.0,
                                                yrange                         = 2.0,
                                                zrange                         = 200.0,
                                                cutoffFreqDenominator_xy       = 2,
                                                cutoffFreqDenominator_z        = 1,
                                                angularCutoffParameter         = 0.75)
      ToolSvc += InDetMedImgMaker
      from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__ImagingSeedFinder
      InDetVtxSeedFinder = Trk__ImagingSeedFinder( name                        = "InDetMedImgSeedFinder",
                                                      VertexCluster            = InDetMedImgClusterFinder,
                                                      VertexImageMaker         = InDetMedImgMaker )

    elif (InDetFlags.doPrimaryVertex3DFinding()):
      from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__CrossDistancesSeedFinder
      InDetVtxSeedFinder = Trk__CrossDistancesSeedFinder(name              = "InDetCrossDistancesSeedFinder",
                                                         trackdistcutoff   = 1.,
                                                         trackdistexppower = 2)
    else:
      from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__ZScanSeedFinder
      InDetVtxSeedFinder = Trk__ZScanSeedFinder(name = "InDetZScanSeedFinder"
                                                #Mode1dFinder = # default, no setting needed
                                                )
    ToolSvc += InDetVtxSeedFinder
    if (InDetFlags.doPrintConfigurables()):
      print InDetVtxSeedFinder
    
    #
    # --- load Impact Point Factory
    #
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__ImpactPoint3dEstimator
    InDetImpactPoint3dEstimator = Trk__ImpactPoint3dEstimator(name              = "InDetImpactPoint3dEstimator",
                                                              Extrapolator      = InDetExtrapolator)
    ToolSvc += InDetImpactPoint3dEstimator
    if (InDetFlags.doPrintConfigurables()):
      print InDetImpactPoint3dEstimator
            
    #
    # --- load Configured Annealing Maker
    #
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__DetAnnealingMaker
    InDetAnnealingMaker = Trk__DetAnnealingMaker(name = "InDetAnnealingMaker",
                                                 SetOfTemperatures = [64.,16.,4.,2.,1.5,1.]) # not default
    ToolSvc += InDetAnnealingMaker
    if (InDetFlags.doPrintConfigurables()):
      print InDetAnnealingMaker

  if (InDetFlags.primaryVertexSetup() == 'DefaultFastFinding' or
      InDetFlags.primaryVertexSetup() == 'DefaultFullFinding' or
      InDetFlags.primaryVertexSetup() == 'DefaultKalmanFinding'):

    from InDetMultipleVertexSeedFinderUtils.InDetMultipleVertexSeedFinderUtilsConf import InDet__InDetTrackZ0SortingTool
    InDetTrackZ0SortingTool =  InDet__InDetTrackZ0SortingTool(name = "InDetTrackZ0SortingTool")
    ToolSvc += InDetTrackZ0SortingTool
    if (InDetFlags.doPrintConfigurables()):
      print InDetTrackZ0SortingTool

    if (not InDetFlags.useBeamConstraint()):

      print 'Using special 2D per-event seeding procedure for approximate 2D beam spot position'

      from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__Trk2dDistanceSeeder
      Trk2dDistanceSeeder = Trk__Trk2dDistanceSeeder(name                 = "Trk2dDistanceSeederFor2D",
                                                     SolveAmbiguityUsingZ = False)
      ToolSvc+=Trk2dDistanceSeeder
      if (InDetFlags.doPrintConfigurables()):
        print Trk2dDistanceSeeder
      

      from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__Trk2DDistanceFinder
      Trk2DDistanceFinder = Trk__Trk2DDistanceFinder(name                = "Trk2DDistanceFinder",
                                                     Trk2dDistanceSeeder = Trk2dDistanceSeeder)
      
      ToolSvc+=Trk2DDistanceFinder
      if (InDetFlags.doPrintConfigurables()):
        print Trk2DDistanceFinder                                     
        
      from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__CrossDistancesSeedFinder
      InDet2DVtxSeedFinder = Trk__CrossDistancesSeedFinder(name                = "InDet2DCrossDistancesSeedFinder",
                                                           TrkDistanceFinder   = Trk2DDistanceFinder,
                                                           trackdistcutoff     = 1.,
                                                           trackdistexppower   = 2,
                                                           useweights          = True
                                                           #Mode1dFinder = # default, no setting needed
                                                           )
      ToolSvc+=InDet2DVtxSeedFinder
      if (InDetFlags.doPrintConfigurables()):
        print InDet2DVtxSeedFinder                                   
      

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
      #now setup new stuff
      if (not InDetFlags.useBeamConstraint()):
        from InDetMultipleVertexSeedFinder.InDetMultipleVertexSeedFinderConf import InDet__SlidingWindowMultiSeedFinder
        InDetMultiSeedFinder = InDet__SlidingWindowMultiSeedFinder(name             = "InDetSlidingWindowMultiSeedFinder",
                                                                   clusterLength    = 8.*mm,
                                                                   TrackSelector    = InDetTrackSelectorTool,
                                                                   Extrapolator     = InDetExtrapolator,
                                                                   SortingTool      = InDetTrackZ0SortingTool,
                                                                   IgnoreBeamSpot   = True,
                                                                   VertexSeedFinder = InDet2DVtxSeedFinder
                                                                   #UseMaxInCluster = True
                                                                   )
        
      else:

        from InDetMultipleVertexSeedFinder.InDetMultipleVertexSeedFinderConf import InDet__SlidingWindowMultiSeedFinder
        InDetMultiSeedFinder = InDet__SlidingWindowMultiSeedFinder(name          = "InDetSlidingWindowMultiSeedFinder",
                                                                   clusterLength = 5.*mm,
                                                                   TrackSelector = InDetTrackSelectorTool,
                                                                   Extrapolator  = InDetExtrapolator,
                                                                   SortingTool   = InDetTrackZ0SortingTool,
                                                                   #UseMaxInCluster = True
                                                                   )

    ToolSvc += InDetMultiSeedFinder
    if (InDetFlags.doPrintConfigurables()):
      print InDetMultiSeedFinder

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
      print InDetVertexSmoother

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
                                                    #VertexUpdator   = # no setting required 
                                                    )
  elif (InDetFlags.primaryVertexSetup() == 'MedImgMultiFinding') :

      from TrkVertexSeedFinderTools.TrkVertexSeedFinderToolsConf import Trk__CrossDistancesSeedFinder
      InDet3DVtxSeedFinder = Trk__CrossDistancesSeedFinder(name                = "InDet3DCrossDistancesSeedFinder",
                                                           trackdistcutoff     = 1.,
                                                           trackdistexppower   = 2,
                                                           #Mode1dFinder = # default, no setting needed
                                                           )
      ToolSvc+=InDet3DVtxSeedFinder
      if (InDetFlags.doPrintConfigurables()):
        print InDet3DVtxSeedFinder                                   
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
    print InDetVxFitterTool

  # -----------------------------------------
  #
  # ----- load primary vertex finder tool
  #
  # -----------------------------------------

  if (not (InDetFlags.primaryVertexSetup() == 'IterativeFinding') and
      not (InDetFlags.primaryVertexSetup() == 'AdaptiveMultiFinding') and
      not (InDetFlags.primaryVertexSetup() == 'DefaultVKalVrtFinding') and
      not (InDetFlags.primaryVertexSetup() == 'MedImgMultiFinding' ) and
      not (InDetFlags.primaryVertexSetup() == 'GaussIterativeFinding' ) and
      not (InDetFlags.primaryVertexSetup() == 'GaussAdaptiveMultiFinding' ) ):
    #
    # --- load primary vertex finder tool
    #
    from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetPriVxFinderTool
    InDetPriVxFinderTool = InDet__InDetPriVxFinderTool(name              = "InDetPriVxFinderTool",
                                                       PriVxSeedFinder   = InDetMultiSeedFinder,
                                                       TrackSelector     = InDetTrackSelectorTool,
                                                       VertexFitterTool  = InDetVxFitterTool,
                                                       maxChi2PerTrack   = InDetPrimaryVertexingCuts.MaxChi2PerTrack(),
                                                       chi2CutMethod     = InDetPrimaryVertexingCuts.chi2CutMethod(),
                                                       enableMultipleVertices = InDetPrimaryVertexingCuts.enableMultipleVertices(),
                                                       useBeamConstraint = InDetFlags.useBeamConstraint())

  elif (InDetFlags.primaryVertexSetup() == 'MedImgMultiFinding') :
    #
    # --- load Medical Imaging seeded multi vertex finder
    #
    from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetMultiPriVxFinderTool
    InDetPriVxFinderTool = InDet__InDetMultiPriVxFinderTool( name                        = "InDetMedImgMultiPriVxFinderTool",
                                                            VertexFitterTool             = InDetVxFitterTool,
                                                            TrackSelector                = InDetTrackSelectorTool,
                                                            SeedFinder                   = InDetVtxSeedFinder,
                                                            ImpactPoint3dEstimator       = InDetImpactPoint3dEstimator,
                                                            useBeamConstraint            = InDetFlags.useBeamConstraint(),
                                                            significanceCutSeeding       = 12,
                                                            maxVertices                  = 200)

  elif ( (InDetFlags.primaryVertexSetup() == 'IterativeFinding') or
         (InDetFlags.primaryVertexSetup() == 'GaussIterativeFinding' ) ):
    #
    # --- load adaptive primary vertex finder
    #
    from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetIterativePriVxFinderTool
    InDetPriVxFinderTool = InDet__InDetIterativePriVxFinderTool(name                     = "InDetIterativePriVxFinderTool",
                                                                VertexFitterTool         = InDetVxFitterTool,
                                                                TrackSelector            = InDetTrackSelectorTool,
                                                                SeedFinder               = InDetVtxSeedFinder,
                                                                ImpactPoint3dEstimator   = InDetImpactPoint3dEstimator,
                                                                LinearizedTrackFactory   = InDetLinFactory,
                                                                useBeamConstraint        = InDetFlags.useBeamConstraint(),
                                                                significanceCutSeeding   = 12,
                                                                maximumChi2cutForSeeding = 49,
                                                                maxVertices              = 200,
                                                                doMaxTracksCut           = InDetPrimaryVertexingCuts.doMaxTracksCut(),
                                                                MaxTracks                = InDetPrimaryVertexingCuts.MaxTracks()
                                                                )

  elif (InDetFlags.primaryVertexSetup() == 'AdaptiveMultiFinding' or
        InDetFlags.primaryVertexSetup() == 'GaussAdaptiveMultiFinding' ):
    #
    # --- load adaptive multi primary vertex finder
    #
    from InDetPriVxFinderTool.InDetPriVxFinderToolConf import InDet__InDetAdaptiveMultiPriVxFinderTool
    InDetPriVxFinderTool = InDet__InDetAdaptiveMultiPriVxFinderTool(name              = "InDetAdaptiveMultiPriVxFinderTool",
                                                                    SeedFinder        = InDetVtxSeedFinder,
                                                                    VertexFitterTool  = InDetVxFitterTool,
                                                                    TrackSelector     = InDetTrackSelectorTool,
                                                                    useBeamConstraint = InDetFlags.useBeamConstraint(),
                                                                    selectiontype     = 0,
                                                                    do3dSplitting     = InDetFlags.doPrimaryVertex3DFinding())
  
  elif InDetFlags.primaryVertexSetup() == 'DefaultVKalVrtFinding':
    #
    # --- load vkal vertex finder tool
    #  
    from InDetVKalPriVxFinderTool.InDetVKalPriVxFinderTool import InDet__InDetVKalPriVxFinderTool
    InDetPriVxFinderTool = InDet__InDetVKalPriVxFinderTool(name                   = "InDetVKalPriVxFinder",
                                                           TrackSummaryTool       = InDetTrackSummaryTool,
                                                           FitterTool             = InDetVxFitterTool,
                                                           BeamConstraint         = 0)
    if InDetFlags.useBeamConstraint():
      InDetPriVxFinderTool.BeamConstraint = 1

  ToolSvc += InDetPriVxFinderTool
  if (InDetFlags.doPrintConfigurables()):
    print InDetPriVxFinderTool

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
      print VxProbFile
      svcMgr.THistSvc.Input += ["VxProbHisto DATAFILE='"+VxProbFile+"' OPT='OLD'"]
      print NNFile
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
      print VertexWeightCalculator

    #
    # --- load sorting tool
    #
    from TrkVertexTools.TrkVertexToolsConf import Trk__VertexCollectionSortingTool
    VertexCollectionSortingTool = Trk__VertexCollectionSortingTool(name                   = "InDetVertexCollectionSortingTool",
                                                                   VertexWeightCalculator = VertexWeightCalculator)
    ToolSvc += VertexCollectionSortingTool
    if InDetFlags.doPrintConfigurables():
      print VertexCollectionSortingTool

  elif InDetFlags.primaryVertexSortingSetup() == 'NoReSorting':

    VertexCollectionSortingTool = None 

  else:

    print 'ERROR: Sorting option '+InDetFlags.primaryVertexSortingSetup()+' not defined. '
    
# ------------------------------------------------------------
#
# ----------- Loading of tool to monitor track candidates in ambi solving
#
# ------------------------------------------------------------
if InDetFlags.doTIDE_AmbiTrackMonitoring():
  from TrkValTools.TrkValToolsConf import Trk__TrkObserverTool
  TrackObserverTool = Trk__TrkObserverTool("TrackObserverTool")
  ToolSvc += TrackObserverTool
  if InDetFlags.doPrintConfigurables():
      print TrackObserverTool

