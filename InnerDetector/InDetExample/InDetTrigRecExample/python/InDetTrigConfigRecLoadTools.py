# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# ------------------------------------------------------------
#
# ----------- Loading of all the Tools needed to configure
#
# ------------------------------------------------------------
#
# common
from AthenaCommon.AppMgr import ToolSvc
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
InDetTrigCutValues = EFIDTrackingCuts

from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.Logging import logging 
log = logging.getLogger("InDetTrigConfigRecLoadTools.py")

from InDetTrigRecExample.InDetTrigConditionsAccess import PixelConditionsSetup


#
# common ClusterMakerTool
#
from SiClusterizationTool.SiClusterizationToolConf import InDet__ClusterMakerTool
InDetTrigClusterMakerTool = \
    InDet__ClusterMakerTool( name = "InDetTrigClusterMakerTool",
                             UsePixelCalibCondDB = False,  #simpler setup for EFID
                             #UsePixelCalibCondDB = True,  #simpler setup for EFID
                             PixelOfflineCalibSvc = PixelConditionsSetup.instanceName('PixelOfflineCalibSvc'),
                             #pixLorentzAnleSvc = "InDetTrigPixLorentzAngleSvc",
                             #UseLorentzAngleCorrections = False
                             )
if (InDetTrigFlags.doPrintConfigurables()):
  print InDetTrigClusterMakerTool
ToolSvc += InDetTrigClusterMakerTool


from SiSpacePointTool.SiSpacePointToolConf import InDet__SiSpacePointMakerTool
InDetTrigSiSpacePointMakerTool = InDet__SiSpacePointMakerTool(name="InDetTrigSiSpacePointMakerTool")
if (InDetTrigFlags.doPrintConfigurables()):
  print InDetTrigSiSpacePointMakerTool
ToolSvc += InDetTrigSiSpacePointMakerTool      
  
from SiTrigSpacePointFormation.SiTrigSpacePointFormationConf import InDet__SCT_TrigSpacePointTool
SCT_TrigSpacePointTool = InDet__SCT_TrigSpacePointTool(name="InDetTrigSCTSpacePointTool",
                                                       ProcessOverlaps = InDetTrigFlags.doOverlapSP(),
                                                       SiSpacePointMakerToolName="InDet::SiSpacePointMakerTool/InDetTrigSiSpacePointMakerTool",
                                                       SpacePointsOverlapName = 'SPTrigOverlap'
                                                       )

if (InDetTrigFlags.doPrintConfigurables()):
  print SCT_TrigSpacePointTool 
ToolSvc +=  SCT_TrigSpacePointTool


#
# ----------- control loading of ROT_creator
#

if InDetTrigFlags.loadRotCreator():
  #4 clusterOnTrack Tools
  #
  from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__SCT_ClusterOnTrackTool
  SCT_ClusterOnTrackTool = InDet__SCT_ClusterOnTrackTool ("SCT_ClusterOnTrackTool",
                                                          CorrectionStrategy = 0,  # do correct position bias
                                                          ErrorStrategy      = 2)  # do use phi dependent errors
  ToolSvc += SCT_ClusterOnTrackTool
  if (InDetTrigFlags.doPrintConfigurables()):
    print SCT_ClusterOnTrackTool

  # tool to always make conservative pixel cluster errors
  from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__PixelClusterOnTrackTool
  from InDetTrigRecExample.InDetTrigConditionsAccess import PixelConditionsSetup

  InDetTrigPixelClusterOnTrackTool = \
      InDet__PixelClusterOnTrackTool("InDetTrigPixelClusterOnTrackTool",
                                     PixelOfflineCalibSvc=PixelConditionsSetup.instanceName('PixelOfflineCalibSvc'),
                                     ErrorStrategy = 2)
  ToolSvc += InDetTrigPixelClusterOnTrackTool
  if (InDetTrigFlags.doPrintConfigurables()):
    print InDetTrigPixelClusterOnTrackTool

  # tool to always make conservative sct cluster errors
  from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__SCT_ClusterOnTrackTool
  InDetTrigBroadSCT_ClusterOnTrackTool = \
      InDet__SCT_ClusterOnTrackTool ("InDetTrigBroadSCT_ClusterOnTrackTool",
                                     CorrectionStrategy = 0,  # do correct position bias
                                     ErrorStrategy      = 0)  # do use broad errors
  ToolSvc += InDetTrigBroadSCT_ClusterOnTrackTool
  if (InDetTrigFlags.doPrintConfigurables()):
    print InDetTrigBroadSCT_ClusterOnTrackTool

  #--
  from InDetTrigRecExample.InDetTrigConditionsAccess import PixelConditionsSetup
  InDetTrigBroadPixelClusterOnTrackTool = \
      InDet__PixelClusterOnTrackTool("InDetTrigBroadPixelClusterOnTrackTool",
                                     PixelOfflineCalibSvc=PixelConditionsSetup.instanceName('PixelOfflineCalibSvc'),
                                     ErrorStrategy = 0)
  ToolSvc += InDetTrigBroadPixelClusterOnTrackTool
  if (InDetTrigFlags.doPrintConfigurables()):
    print InDetTrigBroadPixelClusterOnTrackTool

  # load RIO_OnTrackCreator for Inner Detector
  #

  from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
  InDetTrigRotCreator = Trk__RIO_OnTrackCreator(name = 'InDetTrigRotCreator',
                                                ToolPixelCluster= InDetTrigPixelClusterOnTrackTool,
                                                ToolSCT_Cluster = SCT_ClusterOnTrackTool,
                                                Mode = 'indet')
  ToolSvc += InDetTrigRotCreator

  if InDetTrigFlags.useBroadClusterErrors():
    InDetTrigRotCreator.ToolPixelCluster = InDetTrigBroadPixelClusterOnTrackTool
    InDetTrigRotCreator.ToolSCT_Cluster  = InDetTrigBroadSCT_ClusterOnTrackTool

  if (InDetTrigFlags.doPrintConfigurables()):
    print InDetTrigRotCreator

  #--
  from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
  InDetTrigBroadInDetRotCreator = \
      Trk__RIO_OnTrackCreator(name            = 'InDetTrigBroadInDetRotCreator',
                              ToolPixelCluster= InDetTrigBroadPixelClusterOnTrackTool,
                              ToolSCT_Cluster = InDetTrigBroadSCT_ClusterOnTrackTool,
                              Mode            = 'indet')
  ToolSvc += InDetTrigBroadInDetRotCreator
  if (InDetTrigFlags.doPrintConfigurables()):
    print InDetTrigBroadInDetRotCreator



  # load error scaling
  from IOVDbSvc.CondDB import conddb
  if not (conddb.folderRequested( "/Indet/TrkErrorScaling" ) or \
            conddb.folderRequested( "/Indet/Onl/TrkErrorScaling" )):
    conddb.addFolderSplitOnline("INDET", "/Indet/Onl/TrkErrorScaling", "/Indet/TrkErrorScaling" )

  #
  # smart ROT creator in case we do the TRT LR in the refit
  #
  if InDetTrigFlags.redoTRT_LR():

    from TRT_DriftCircleOnTrackTool.TRT_DriftCircleOnTrackToolConf import \
        InDet__TRT_DriftCircleOnTrackUniversalTool
    InDetTrigTRT_RefitRotCreator = \
        InDet__TRT_DriftCircleOnTrackUniversalTool(name  = 'InDetTrigTRT_RefitRotCreator',
                                                   ScaleHitUncertainty = 2.5) # fix from Thijs
#    if InDetTrigFlags.doCommissioning():    #introduced for cosmics do not use for collisions
#      InDetTrigTRT_RefitRotCreator.ScaleHitUncertainty = 5.
    ToolSvc += InDetTrigTRT_RefitRotCreator
      
    if (InDetTrigFlags.doPrintConfigurables()):
      print      InDetTrigTRT_RefitRotCreator
        
    from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
    InDetTrigRefitRotCreator = Trk__RIO_OnTrackCreator(name              = 'InDetTrigRefitRotCreator',
                                                       ToolPixelCluster= InDetTrigPixelClusterOnTrackTool,
                                                       ToolSCT_Cluster     = SCT_ClusterOnTrackTool,
                                                       ToolTRT_DriftCircle = InDetTrigTRT_RefitRotCreator,
                                                       Mode                = 'indet')
    if InDetTrigFlags.useBroadClusterErrors():
      InDetTrigRefitRotCreator.ToolPixelCluster = InDetTrigBroadPixelClusterOnTrackTool
      InDetTrigRefitRotCreator.ToolSCT_Cluster  = InDetTrigBroadSCT_ClusterOnTrackTool

    ToolSvc += InDetTrigRefitRotCreator
    if (InDetTrigFlags.doPrintConfigurables()):
      print      InDetTrigRefitRotCreator
         
  else:
    InDetTrigRefitRotCreator = InDetTrigRotCreator

#
# ----------- control loading of the kalman updator
#

if InDetTrigFlags.loadUpdator():
   
  if InDetTrigFlags.kalmanUpdator() is "fast" :
    # fast Kalman updator tool
    from TrkMeasurementUpdator_xk.TrkMeasurementUpdator_xkConf import Trk__KalmanUpdator_xk
    InDetTrigUpdator = Trk__KalmanUpdator_xk(name = 'InDetTrigUpdator')
  elif InDetTrigFlags.kalmanUpdator() is "weight" :
    from TrkMeasurementUpdator.TrkMeasurementUpdatorConf import Trk__KalmanWeightUpdator as ConfiguredWeightUpdator
    InDetTrigUpdator = ConfiguredWeightUpdator(name='InDetTrigUpdator')
  else :
    from TrkMeasurementUpdator.TrkMeasurementUpdatorConf import Trk__KalmanUpdator as ConfiguredKalmanUpdator
    InDetTrigUpdator = ConfiguredKalmanUpdator('InDetTrigUpdator')

  ToolSvc += InDetTrigUpdator
  if (InDetTrigFlags.doPrintConfigurables()):
    print      InDetTrigUpdator

  # ---------- control loading of the gsf updator
  if InDetTrigFlags.trackFitterType() is 'GaussianSumFilter' :
    # Load the Gsf Measurement Updator

    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfMeasurementUpdator
    InDetTrigGsfMeasurementUpdator = Trk__GsfMeasurementUpdator( name    = 'InDetTrigGsfMeasurementUpdator',
                                                                 Updator = InDetTrigUpdator )
    ToolSvc += InDetTrigGsfMeasurementUpdator
    if (InDetTrigFlags.doPrintConfigurables()):
      print      InDetTrigGsfMeasurementUpdator

#
# ----------- control loading extrapolation
#
if InDetTrigFlags.loadExtrapolator():

  # set up geometry
  from AthenaCommon.Include import include
  from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
  
  #
  # get propagator
  #
  from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator
  InDetTrigStepPropagator = Trk__STEP_Propagator(name = 'InDetTrigStepPropagator')
  ToolSvc += InDetTrigStepPropagator
  
  from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator
  InDetTrigRKPropagator = Trk__RungeKuttaPropagator(name = 'InDetTrigRKPropagator')
  ToolSvc += InDetTrigRKPropagator
  
  if InDetTrigFlags.propagatorType() is "STEP":
    InDetTrigPropagator = InDetTrigStepPropagator
  else:
    InDetTrigPropagator = InDetTrigRKPropagator
    
  if (InDetTrigFlags.doPrintConfigurables()):
    print      InDetTrigPropagator      

  #
  # Setup the Navigator (default, could be removed)
  #
  from AthenaCommon.AppMgr import ServiceMgr as svcMgr
  AtlasTrackingGeometrySvc  = svcMgr.AtlasTrackingGeometrySvc 

  from TrkExTools.TrkExToolsConf import Trk__Navigator
  InDetTrigNavigator = Trk__Navigator(name = 'InDetTrigNavigator',
                                      TrackingGeometrySvc = AtlasTrackingGeometrySvc)

  ToolSvc += InDetTrigNavigator
  if (InDetTrigFlags.doPrintConfigurables()):
    print      InDetTrigNavigator
  
  #
  # Setup the MaterialEffectsUpdator
  #
  from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
  InDetTrigMaterialUpdator = Trk__MaterialEffectsUpdator(name = "InDetTrigMaterialEffectsUpdator")

  ToolSvc += InDetTrigMaterialUpdator
  if (InDetTrigFlags.doPrintConfigurables()):
    print      InDetTrigMaterialUpdator


  #
  # Set up extrapolator
  #

  InDetTrigSubPropagators = []
  InDetTrigSubUpdators = []
  
  # -------------------- set it depending on the geometry --------------
  # default for ID is (Rk,Mat)
  InDetTrigSubPropagators += [ InDetTrigPropagator.name() ]
  InDetTrigSubUpdators    += [ InDetTrigMaterialUpdator.name() ]
  
  # default for Calo is (Rk,MatLandau)
  InDetTrigSubPropagators += [ InDetTrigPropagator.name() ]
  InDetTrigSubUpdators    += [ InDetTrigMaterialUpdator.name() ]
  
  # default for MS is (STEP,Mat)
  InDetTrigSubPropagators += [ InDetTrigStepPropagator.name() ]
  InDetTrigSubUpdators    += [ InDetTrigMaterialUpdator.name() ]

  from TrkExTools.TrkExToolsConf import Trk__Extrapolator
  InDetTrigExtrapolator = Trk__Extrapolator(name        = 'InDetTrigExtrapolator',
                                            Propagators             = [ InDetTrigRKPropagator, InDetTrigStepPropagator],
                                            MaterialEffectsUpdators = [ InDetTrigMaterialUpdator ],
                                            Navigator               = InDetTrigNavigator,
                                            SubPropagators          = InDetTrigSubPropagators,
                                            SubMEUpdators           = InDetTrigSubUpdators,
                                            #DoCaloDynamic          = False
                                            )

  ToolSvc += InDetTrigExtrapolator
  if (InDetTrigFlags.doPrintConfigurables()):
    print      InDetTrigExtrapolator                                          


#
# ----------- control loading of fitters
#
if InDetTrigFlags.loadFitter():
  if InDetTrigFlags.trackFitterType() is 'KalmanFitter' or InDetTrigFlags.trackFitterType() is 'KalmanDNAFitter':

    from InDetCompetingRIOsOnTrackTool.InDetCompetingRIOsOnTrackToolConf \
         import InDet__CompetingPixelClustersOnTrackTool as IDCPCOTT
    InDetTrigCompetingPixelTool = IDCPCOTT(name='TrigKalmanCompetingPixelClustersTool',
                                           ToolForPixelClusterOnTrackCreation=InDetTrigBroadPixelClusterOnTrackTool,
                                           WeightCutValueBarrel = 5.5,
                                           WeightCutValueEndCap = 5.5)
    ToolSvc+=InDetTrigCompetingPixelTool
    if (InDetTrigFlags.doPrintConfigurables()):
      print InDetTrigCompetingPixelTool

    from InDetCompetingRIOsOnTrackTool.InDetCompetingRIOsOnTrackToolConf \
         import InDet__CompetingSCT_ClustersOnTrackTool as IDCSCOTT
    InDetTrigCompetingSctTool = IDCSCOTT(name='TrigKalmanCompetingSCT_ClustersTool',
                                             WeightCutValueBarrel = 5.5,
                                             WeightCutValueEndCap = 5.5)
    ToolSvc+=InDetTrigCompetingSctTool
    if (InDetTrigFlags.doPrintConfigurables()):
      print InDetTrigCompetingSctTool
    
    from TrkCompetingRIOsOnTrackTool.TrkCompetingRIOsOnTrackToolConf \
         import Trk__CompetingRIOsOnTrackTool as CompRotTool
    InDetTrigKalmanCompetingROT_Tool = CompRotTool(name='TrigKalmanCompetingRIOsTool',
                                                       ToolForCompPixelClusters = InDetTrigCompetingPixelTool,
                                                       ToolForCompSCT_Clusters = InDetTrigCompetingSctTool)
    ToolSvc += InDetTrigKalmanCompetingROT_Tool
    if (InDetTrigFlags.doPrintConfigurables()):
      print InDetTrigKalmanCompetingROT_Tool

    from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanPiecewiseAnnealingFilter as KPAF
    InDetTrigKalmanInternalDAF = KPAF(name = 'KalmanInternalDAF',
                                          CompetingRIOsOnTrackCreator = InDetTrigKalmanCompetingROT_Tool)
    ToolSvc += InDetTrigKalmanInternalDAF
    if (InDetTrigFlags.doPrintConfigurables()):
      print InDetTrigKalmanInternalDAF

    if InDetTrigFlags.trackFitterType() is 'KalmanDNAFitter':
      from TrkDynamicNoiseAdjustor.TrkDynamicNoiseAdjustorConf import Trk__InDetDynamicNoiseAdjustment
      InDetTrigDNAdjustor = Trk__InDetDynamicNoiseAdjustment(name       = 'InDetTrigDNAdjustor')
      ToolSvc += InDetTrigDNAdjustor
      if (InDetTrigFlags.doPrintConfigurables()):
        print      InDetTrigDNAdjustor
            
      from InDetDNASeparator.InDetDNASeparatorConf import InDet__InDetDNASeparator
      InDetTrigDNASeparator = InDet__InDetDNASeparator(name="InDetTrigDNASeparator")
            
      ToolSvc += InDetTrigDNASeparator
      if (InDetTrigFlags.doPrintConfigurables()):
        print      InDetTrigDNASeparator
            
    else:
      InDetTrigDNAdjustor   = None
      InDetTrigDNASeparator = None
            
    # Load Kalman Filter tools
    from TrkKalmanFitter.TrkKalmanFitterConf import Trk__ForwardKalmanFitter as PublicFKF
    InDetTrigFKF = PublicFKF(name                  = 'InDetTrigFKF',
                                 StateChi2PerNDFPreCut = 30.0)
    ToolSvc += InDetTrigFKF
    if (InDetTrigFlags.doPrintConfigurables()):
      print      InDetTrigFKF

    from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanSmoother as PublicBKS
    InDetTrigBKS = PublicBKS(name                        = 'InDetTrigBKS',
                                 InitialCovarianceSeedFactor = 200.)
    ToolSvc += InDetTrigBKS
    if (InDetTrigFlags.doPrintConfigurables()):
      print      InDetTrigBKS

    from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanOutlierLogic as PublicKOL
    InDetTrigKOL = PublicKOL(name                    = 'InDetTrigKOL',
                                 TrackChi2PerNDFCut      = 17.0,
                                 StateChi2PerNDFCut      = 12.5)
    ToolSvc += InDetTrigKOL
    if (InDetTrigFlags.doPrintConfigurables()):
      print      InDetTrigKOL

    from TrkKalmanFitter.TrkKalmanFitterConf import Trk__MeasRecalibSteeringTool
    InDetTrigMeasRecalibST = Trk__MeasRecalibSteeringTool(name='InDetTrigMeasRecalibST',
                                                          CommonRotCreator=InDetTrigRefitRotCreator,
                                                          BroadPixelClusterOnTrackTool=InDetTrigBroadPixelClusterOnTrackTool,
                                                          BroadSCT_ClusterOnTrackTool=InDetTrigBroadSCT_ClusterOnTrackTool,
                                                          #TRT_DriftCircleOnTrackTool=,
                                                          #TRT_StrawTubeOnTrackTool=,
                                                          )

    ToolSvc += InDetTrigMeasRecalibST
    if (InDetTrigFlags.doPrintConfigurables()):
      print      InDetTrigMeasRecalibST

    from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanFitter as ConfiguredKalmanFitter
    InDetTrigTrackFitter = ConfiguredKalmanFitter(name                           = 'InDetTrigTrackFitter',
                                                      ExtrapolatorHandle             = InDetTrigExtrapolator,
                                                      RIO_OnTrackCreatorHandle       = InDetTrigRefitRotCreator,
                                                      MeasurementUpdatorHandle       = InDetTrigUpdator,
                                                      ForwardKalmanFitterHandle      = InDetTrigFKF,
                                                      KalmanSmootherHandle           = InDetTrigBKS,
                                                      KalmanOutlierLogicHandle       = InDetTrigKOL,
                                                      DynamicNoiseAdjustorHandle     = InDetTrigDNAdjustor,
                                                      BrempointAnalyserHandle        = InDetTrigDNASeparator,
                                                      AlignableSurfaceProviderHandle = None,
                                                      RecalibratorHandle             = InDetTrigMeasRecalibST,
                                                      InternalDAFHandle              = InDetTrigKalmanInternalDAF)

    
  elif InDetTrigFlags.trackFitterType() is 'DistributedKalmanFilter' :
   
    from TrkDistributedKalmanFilter.TrkDistributedKalmanFilterConf import Trk__DistributedKalmanFilter
    InDetTrigTrackFitter = Trk__DistributedKalmanFilter(name             = 'InDetTrigTrackFitter',
                                                        ExtrapolatorTool = InDetTrigExtrapolator,
                                                        ROTcreator       = InDetTrigRotCreator
                                                        #sortingReferencePoint = ???
                                                        )
   
  elif InDetTrigFlags.trackFitterType() is 'GlobalChi2Fitter' :

    from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter
    InDetTrigTrackFitter = Trk__GlobalChi2Fitter(name                  = 'InDetTrigTrackFitter',
                                                 ExtrapolationTool     = InDetTrigExtrapolator,
                                                 NavigatorTool         = InDetTrigNavigator,
                                                 PropagatorTool        = InDetTrigPropagator,		
                                                 RotCreatorTool        = InDetTrigRefitRotCreator,
                                                 BroadRotCreatorTool   = InDetTrigBroadInDetRotCreator,
                                                 MeasurementUpdateTool = InDetTrigUpdator,
                                                 TrackingGeometrySvc   = AtlasTrackingGeometrySvc,
                                                 MaterialUpdateTool    = InDetTrigMaterialUpdator,
                                                 StraightLine          = not InDetTrigFlags.solenoidOn(),
                                                 OutlierCut            = 4,
                                                 SignedDriftRadius     = True,
                                                 RecalibrateSilicon    = True,
                                                 RecalibrateTRT        = True,
                                                 ReintegrateOutliers   = True,
                                                 TrackChi2PerNDFCut    = 9,
                                                 TRTExtensionCuts      = True, 
                                                 MaxIterations         = 40,
                                                 Acceleration          = True,
                                                 #Momentum=1000.,
                                                 Momentum=0.,
                                                 )
    if InDetTrigFlags.useBroadClusterErrors():
      InDetTrigTrackFitter.RecalibrateSilicon = False

    if InDetTrigFlags.doRefit():
      InDetTrigTrackFitter.BroadRotCreatorTool = None
      InDetTrigTrackFitter.RecalibrateSilicon = False
      InDetTrigTrackFitter.RecalibrateTRT     = False
      InDetTrigTrackFitter.ReintegrateOutliers= False


    if InDetTrigFlags.doRobustReco():
      InDetTrigTrackFitter.OutlierCut         = 10.0
      InDetTrigTrackFitter.TrackChi2PerNDFCut = 20
      InDetTrigTrackFitter.MaxOutliers        = 99
      #only switch off for cosmics InDetTrigTrackFitter.Acceleration       = False
      

    InDetTrigTrackFitterLowPt = Trk__GlobalChi2Fitter(name                  = 'InDetTrigTrackFitterLowPt',
                                                      ExtrapolationTool     = InDetTrigExtrapolator,
                                                      NavigatorTool         = InDetTrigNavigator,
                                                      PropagatorTool        = InDetTrigPropagator,		
                                                      RotCreatorTool        = InDetTrigRefitRotCreator,
                                                      BroadRotCreatorTool   = InDetTrigBroadInDetRotCreator,
                                                      MeasurementUpdateTool = InDetTrigUpdator,
                                                      StraightLine          = not InDetTrigFlags.solenoidOn(),
                                                      OutlierCut            = 5.0,
                                                      SignedDriftRadius     = True,
                                                      RecalibrateSilicon    = True,
                                                      RecalibrateTRT        = True,
                                                      ReintegrateOutliers   = True,
                                                      TrackChi2PerNDFCut    = 10,
                                                      TRTExtensionCuts      = True, 
                                                      MaxIterations         = 40,
                                                      Momentum=0.
                                                      )
    ToolSvc += InDetTrigTrackFitterLowPt



    #for cosmics
    InDetTrigTrackFitterCosmics = \
        Trk__GlobalChi2Fitter(name = 'InDetTrigTrackFitterCosmics',
                              ExtrapolationTool     = InDetTrigExtrapolator,
                              NavigatorTool         = InDetTrigNavigator,
                              PropagatorTool        = InDetTrigPropagator,		
                              RotCreatorTool        = InDetTrigRefitRotCreator,
                              BroadRotCreatorTool   = InDetTrigBroadInDetRotCreator,
                              MeasurementUpdateTool = InDetTrigUpdator,
                              StraightLine          = not InDetTrigFlags.solenoidOn(),
                              OutlierCut            = 10.0,
                              SignedDriftRadius     = True,
                              RecalibrateTRT        = True,
                              ReintegrateOutliers   = True,
                              TrackChi2PerNDFCut    = 20,
                              TRTExtensionCuts      = False, 
                              MaxIterations         = 40,
                              MaxOutliers           = 99,
                              RecalculateDerivatives = True,
                              Momentum=1000,
                              )
    InDetTrigTrackFitterCosmics.Acceleration       = False
    ToolSvc += InDetTrigTrackFitterCosmics

    #TRT
    InDetTrigTrackFitterTRT = Trk__GlobalChi2Fitter(name                  = 'InDetTrigTrackFitterTRT',
                                                    ExtrapolationTool     = InDetTrigExtrapolator,
                                                    NavigatorTool         = InDetTrigNavigator,
                                                    PropagatorTool        = InDetTrigPropagator,
                                                    RotCreatorTool        = InDetTrigRefitRotCreator,
                                                    MeasurementUpdateTool = InDetTrigUpdator,
                                                    StraightLine          = not InDetTrigFlags.solenoidOn(),
                                                    ReintegrateOutliers   = False,
                                                    MaxIterations         = 30,
                                                    #MaxOutliers           = 99,
                                                    RecalculateDerivatives= True,
                                                    TrackChi2PerNDFCut    = 999999,
                                                    Momentum=0.,
                                                    )
    if InDetTrigFlags.doRobustReco():
      InDetTrigTrackFitterTRT.MaxOutliers=99
            
  elif InDetTrigFlags.trackFitterType() is 'GaussianSumFilter' :
    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfMaterialMixtureConvolution
    InDetTrigGsfMaterialUpdator = Trk__GsfMaterialMixtureConvolution (name = 'InDetTrigGsfMaterialUpdator')
    ToolSvc += InDetTrigGsfMaterialUpdator
    if (InDetTrigFlags.doPrintConfigurables()):
      print      InDetTrigGsfMaterialUpdator
    #
    # component Reduction
    #
    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__CloseComponentsMultiStateMerger
    InDetTrigGsfComponentReduction = \
        Trk__CloseComponentsMultiStateMerger (name = 'InDetTrigGsfComponentReduction',
                                              MaximumNumberOfComponents = 12)
    ToolSvc += InDetTrigGsfComponentReduction
    if (InDetTrigFlags.doPrintConfigurables()):
      print      InDetTrigGsfComponentReduction
    #
    # declare the extrapolator
    #
    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfExtrapolator
    InDetTrigGsfExtrapolator = Trk__GsfExtrapolator(name = 'InDetTrigGsfExtrapolator',
                                                    Propagators                   = [ InDetTrigPropagator ],
                                                    SearchLevelClosestParameters  = 10,
                                                    StickyConfiguration           = True,
                                                    Navigator                     = InDetTrigNavigator,
                                                    GsfMaterialConvolution        = InDetTrigGsfMaterialUpdator,
                                                    ComponentMerger               = InDetTrigGsfComponentReduction,
                                                    SurfaceBasedMaterialEffects   = False )
    ToolSvc += InDetTrigGsfExtrapolator
    if (InDetTrigFlags.doPrintConfigurables()):
      print      InDetTrigGsfExtrapolator
    # load alternative track fitter
    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GaussianSumFitter
    InDetTrigTrackFitter = Trk__GaussianSumFitter(name                    = 'InDetTrigTrackFitter',
                                                  ToolForExtrapolation    = InDetTrigGsfExtrapolator,
                                                  MeasurementUpdatorType  = InDetTrigGsfMeasurementUpdator,
                                                  ToolForROTCreation      = InDetTrigRotCreator,
                                                  ReintegrateOutliers     = False,
                                                  MakePerigee             = True,
                                                  RefitOnMeasurementBase  = True,
                                                  DoHitSorting            = True)
    
    
   # --- end of fitter loading
  ToolSvc += InDetTrigTrackFitter
  if (InDetTrigFlags.doPrintConfigurables()):
    print      InDetTrigTrackFitter
    
  if InDetTrigFlags.trackFitterType() is not 'GlobalChi2Fitter' :
    InDetTrigTrackFitterTRT=InDetTrigTrackFitter
    InDetTrigTrackFitterLowPt=InDetTrigTrackFitter
    ToolSvc += InDetTrigTrackFitterLowPt
    InDetTrigTrackFitterCosmics=InDetTrigTrackFitter
    
  ToolSvc += InDetTrigTrackFitterTRT
  if (InDetTrigFlags.doPrintConfigurables()):
    print      InDetTrigTrackFitterTRT



if DetFlags.haveRIO.pixel_on():
  from PixelConditionsServices.PixelConditionsServicesConf import PixelConditionsSummarySvc
  from InDetTrigRecExample.InDetTrigConditionsAccess import PixelConditionsSetup
  InDetTrigPixelConditionsSummarySvc = PixelConditionsSummarySvc(PixelConditionsSetup.instanceName('PixelConditionsSummarySvc'))
else:
  InDetTrigPixelConditionsSummarySvc = None

if DetFlags.haveRIO.SCT_on():
  from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConditionsSummarySvc
  from InDetTrigRecExample.InDetTrigConditionsAccess import SCT_ConditionsSetup
  InDetTrigSCTConditionsSummarySvc = SCT_ConditionsSummarySvc(SCT_ConditionsSetup.instanceName('InDetSCT_ConditionsSummarySvc'))
else:
  InDetTrigSCTConditionsSummarySvc = None

#
# ------load association tool from Inner Detector to handle pixel ganged ambiguities
#
if InDetTrigFlags.loadAssoTool():
  from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
  InDetTrigPrdAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels(name = "InDetTrigPrdAssociationTool",
                                                                             PixelClusterAmbiguitiesMapName = "TrigPixelClusterAmbiguitiesMap")
   
  ToolSvc += InDetTrigPrdAssociationTool
  if (InDetTrigFlags.doPrintConfigurables()):
    print      InDetTrigPrdAssociationTool

#
# ----------- control loading of Summary Tool
#
if InDetTrigFlags.loadSummaryTool():


  # Load Pixel Layer tool
  from InDetTestPixelLayer.InDetTestPixelLayerConf import InDet__InDetTestPixelLayerTool
  InDetTrigTestPixelLayerTool = InDet__InDetTestPixelLayerTool(name            = "InDetTrigTestPixelLayerTool",
                                                               PixelSummarySvc = InDetTrigPixelConditionsSummarySvc,
                                                               Extrapolator    = InDetTrigExtrapolator,
                                                               CheckActiveAreas= True,
                                                               CheckDeadRegions= True)
  ToolSvc += InDetTrigTestPixelLayerTool
  if (InDetTrigFlags.doPrintConfigurables()):
    print  InDetTrigTestPixelLayerTool


   #
   # Loading Configurable HoleSearchTool
   #
  from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool

  InDetTrigHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "InDetTrigHoleSearchTool",
                                                            Extrapolator = InDetTrigExtrapolator,
                                                            usePixel      = DetFlags.haveRIO.pixel_on(),
                                                            useSCT        = DetFlags.haveRIO.SCT_on(),
                                                            PixelSummarySvc = InDetTrigPixelConditionsSummarySvc,
                                                            SctSummarySvc = InDetTrigSCTConditionsSummarySvc,
                                                            PixelLayerTool=InDetTrigTestPixelLayerTool,
                                                            )
                                                            #Commissioning = InDetTrigFlags.doCommissioning()) #renamed
  InDetTrigHoleSearchTool.CountDeadModulesAfterLastHit = True  

  ToolSvc += InDetTrigHoleSearchTool
  if (InDetTrigFlags.doPrintConfigurables()):
    print      InDetTrigHoleSearchTool

  # Load BLayer tool
  from InDetTestBLayer.InDetTestBLayerConf import InDet__InDetTestBLayerTool
  InDetTrigTestBLayerTool = InDet__InDetTestBLayerTool(name            = "InDetTrigTestBLayerTool",
                                                       PixelSummarySvc = InDetTrigPixelConditionsSummarySvc,
                                                       Extrapolator    = InDetTrigExtrapolator,
                                                       CheckActiveAreas= True)
  ToolSvc += InDetTrigTestBLayerTool
  if (InDetTrigFlags.doPrintConfigurables()):
    print  InDetTrigTestBLayerTool

  #prevent loading of the pixel dE/dx tool  
  InDetTrigPixelToTPIDTool = None
#   try:
#     from PixelToTPIDTool.PixelToTPIDToolConf import InDet__PixelToTPIDTool
#     InDetTrigPixelToTPIDTool = InDet__PixelToTPIDTool(name = "InDetTrigPixelToTPIDTool")
#     ToolSvc += InDetTrigPixelToTPIDTool
    
#   except:
#     log.info("InDetTrigPixelToTPIDTool requested but not available")

  if (InDetTrigFlags.doPrintConfigurables()):
    print     'InDetTrigPixelToTPIDTool ', InDetTrigPixelToTPIDTool

  #
  # Configrable version of loading the InDetTrackSummaryHelperTool
  #
  from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
  from InDetTrigRecExample.InDetTrigConditionsAccess import TRT_ConditionsSetup
  InDetTrigTrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name          = "InDetTrigSummaryHelper",
                                                                       HoleSearch    = InDetTrigHoleSearchTool,
                                                                       AssoTool      = InDetTrigPrdAssociationTool,
                                                                       TestBLayerTool = None,
                                                                       PixelToTPIDTool= InDetTrigPixelToTPIDTool,
                                                                       DoSharedHits  = False,
                                                                       TRTStrawSummarySvc=TRT_ConditionsSetup.instanceName('InDetTRTStrawStatusSummarySvc'),
                                                                       usePixel      = DetFlags.haveRIO.pixel_on(),
                                                                       useSCT        = DetFlags.haveRIO.SCT_on(),
                                                                       useTRT        = DetFlags.haveRIO.TRT_on())
    
  ToolSvc += InDetTrigTrackSummaryHelperTool
  if (InDetTrigFlags.doPrintConfigurables()):
    print      InDetTrigTrackSummaryHelperTool

  #
  # Configurable version of TRT_ElectronPidTools
  #
  from IOVDbSvc.CondDB import conddb
  if not (conddb.folderRequested("/TRT/Calib/PID") or \
            conddb.folderRequested("/TRT/Onl/Calib/PID")):
     conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/PID","/TRT/Calib/PID")
  if not (conddb.folderRequested("/TRT/Calib/PID_RToT") or \
            conddb.folderRequested("/TRT/Onl/Calib/PID_RToT")):
     conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/PID_RToT","/TRT/Calib/PID_RToT")
  if not (conddb.folderRequested("/TRT/Calib/PID_vector") or \
            conddb.folderRequested("/TRT/Onl/Calib/PID_vector")):
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/PID_vector","/TRT/Calib/PID_vector")
  if not (conddb.folderRequested("/TRT/Calib/ToT/ToTVectors") or \
            conddb.folderRequested("/TRT/Onl/Calib/ToT/ToTVectors")):
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/ToT/ToTVectors","/TRT/Calib/ToT/ToTVectors")
  if not (conddb.folderRequested("/TRT/Calib/ToT/ToTValue") or \
            conddb.folderRequested("/TRT/Onl/Calib/ToT/ToTValue")):
    conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/ToT/ToTValue","/TRT/Calib/ToT/ToTValue")
 
 #    from AthenaCommon.GlobalFlags import globalflags
  #    if globalflags.DataSource() == 'data':
  #      conddb.addOverride("/TRT/Onl/Calib/PID_vector"  ,"TRTOnlCalibPID_vector-ES1-UPD1-00-00-01")
     #else:
     #  conddb.addOverride("/TRT/Onl/Calib/PID_vector","TRTCalibPID_vector_MC_OnSetMC_CorrMC_noZR_00-01")

  from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import InDet__TRT_ElectronPidToolRun2,InDet__TRT_LocalOccupancy
  from AthenaCommon.GlobalFlags import globalflags
  from InDetTrigRecExample.InDetTrigConditionsAccess import TRT_ConditionsSetup

  InDetTrigTRT_LocalOccupancy = InDet__TRT_LocalOccupancy(name ="InDetTrigTRT_LocalOccupancy",
                                                          isTrigger=True,
                                                          TRT_RDOContainerName="TRT_RDOs_EF",
                                                          TRTStrawSummarySvc=TRT_ConditionsSetup.instanceName('InDetTRTStrawStatusSummarySvc'),
                                                          )
  ToolSvc += InDetTrigTRT_LocalOccupancy
  
  InDetTrigTRT_ElectronPidTool = InDet__TRT_ElectronPidToolRun2(name   = "InDetTrigTRT_ElectronPidTool",
                                                                TRT_LocalOccupancyTool = InDetTrigTRT_LocalOccupancy,
                                                                TRTStrawSummarySvc=TRT_ConditionsSetup.instanceName('InDetTRTStrawStatusSummarySvc'),
                                                                OccupancyUsedInPID = True,
                                                                isData = (globalflags.DataSource == 'data'))

  ToolSvc += InDetTrigTRT_ElectronPidTool
  if (InDetTrigFlags.doPrintConfigurables()):
    print      InDetTrigTRT_ElectronPidTool

  #
  # Configurable version of TrkTrackSummaryTool
  #
  from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
  InDetTrigTrackSummaryTool = Trk__TrackSummaryTool(name = "InDetTrigTrackSummaryTool",
                                                    InDetSummaryHelperTool = InDetTrigTrackSummaryHelperTool,
                                                    InDetHoleSearchTool    = InDetTrigHoleSearchTool,
                                                    doSharedHits           = False,
                                                    #this may be temporary #61512 (and used within egamma later)
                                                    #TRT_ElectronPidTool    = InDetTrigTRT_ElectronPidTool, 
                                                    TRT_ElectronPidTool    = None, 
                                                    )
  ToolSvc += InDetTrigTrackSummaryTool
  if (InDetTrigFlags.doPrintConfigurables()):
     print      InDetTrigTrackSummaryTool



  if InDetTrigFlags.doSharedHits():
    #
    # Configrable version of loading the InDetTrackSummaryHelperTool
    #
    from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
    from InDetTrigRecExample.InDetTrigConditionsAccess import TRT_ConditionsSetup
    InDetTrigTrackSummaryHelperToolSharedHits = InDet__InDetTrackSummaryHelperTool(name         = "InDetTrigSummaryHelperSharedHits",
                                                                                   AssoTool     = InDetTrigPrdAssociationTool,
                                                                                   DoSharedHits = InDetTrigFlags.doSharedHits(),
                                                                                   HoleSearch   = InDetTrigHoleSearchTool,
                                                                                   TestBLayerTool=InDetTrigTestBLayerTool,
                                                                                   PixelToTPIDTool=InDetTrigPixelToTPIDTool,
                                                                                   TRTStrawSummarySvc = TRT_ConditionsSetup.instanceName('InDetTRTStrawStatusSummarySvc'),
                                                                                   )

    ToolSvc += InDetTrigTrackSummaryHelperToolSharedHits
    if (InDetTrigFlags.doPrintConfigurables()):
      print      InDetTrigTrackSummaryHelperToolSharedHits
    #
    # Configurable version of TrkTrackSummaryTool
    #
    from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
    InDetTrigTrackSummaryToolSharedHits = Trk__TrackSummaryTool(name = "InDetTrigTrackSummaryToolSharedHits",
                                                                InDetSummaryHelperTool = InDetTrigTrackSummaryHelperToolSharedHits,
                                                                doSharedHits           = InDetTrigFlags.doSharedHits(),
                                                                InDetHoleSearchTool    = InDetTrigHoleSearchTool,
                                                                TRT_ElectronPidTool    = None)

    ToolSvc += InDetTrigTrackSummaryToolSharedHits
    if (InDetTrigFlags.doPrintConfigurables()):
      print      InDetTrigTrackSummaryToolSharedHits


  else:   
    InDetTrigTrackSummaryToolSharedHits        = InDetTrigTrackSummaryTool   

#
# ----------- control loading of tools which are needed by new tracking and backtracking
#

# Igor's propagator and updator for the pattern
#
from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
InDetTrigPatternPropagator = Propagator(name = 'InDetTrigPatternPropagator')

ToolSvc += InDetTrigPatternPropagator
if (InDetTrigFlags.doPrintConfigurables()):
   print      InDetTrigPatternPropagator

# fast Kalman updator tool
#
from TrkMeasurementUpdator_xk.TrkMeasurementUpdator_xkConf import Trk__KalmanUpdator_xk
InDetTrigPatternUpdator = Trk__KalmanUpdator_xk(name = 'InDetTrigPatternUpdator')

ToolSvc += InDetTrigPatternUpdator
if (InDetTrigFlags.doPrintConfigurables()):
   print      InDetTrigPatternUpdator


#
# TRT segment minimum number of drift circles tool
#

from InDetTrigRecExample.InDetTrigConditionsAccess import TRT_ConditionsSetup
InDetTrigTRT_ActiveFractionSvc = TRT_ConditionsSetup.instanceName("InDetTRTActiveFractionSvc")

from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetTrtDriftCircleCutTool
InDetTrigTRTDriftCircleCut = InDet__InDetTrtDriftCircleCutTool(
  name             = 'InDetTrigTRTDriftCircleCut',
  TrtConditionsSvc = InDetTrigTRT_ActiveFractionSvc,
  MinOffsetDCs     = 5,
  UseNewParameterization = True,
  UseActiveFractionSvc   = True #DetFlags.haveRIO.TRT_on()  # Use Thomas's new parameterization by default
  )

ToolSvc += InDetTrigTRTDriftCircleCut
if (InDetTrigFlags.doPrintConfigurables()):
  print   InDetTrigTRTDriftCircleCut

InDetTrigTRTDriftCircleCutForPatt = InDet__InDetTrtDriftCircleCutTool(
  name             = 'InDetTrigTRTDriftCircleCutForPatt',
  TrtConditionsSvc = InDetTrigTRT_ActiveFractionSvc,
  MinOffsetDCs     = 5,
  UseNewParameterization = InDetTrigCutValues.useNewParameterizationTRT(),
  UseActiveFractionSvc   = True #DetFlags.haveRIO.TRT_on()  # Use Thomas's new parameterization by default
  )

ToolSvc += InDetTrigTRTDriftCircleCut
if (InDetTrigFlags.doPrintConfigurables()):
  print   InDetTrigTRTDriftCircleCut




# SCT and Pixel detector elements road builder
#
from SiDetElementsRoadTool_xk.SiDetElementsRoadTool_xkConf import InDet__SiDetElementsRoadMaker_xk

InDetTrigSiDetElementsRoadMaker = \
                                InDet__SiDetElementsRoadMaker_xk(name = 'InDetTrigSiDetElementsRoadMaker',
                                                                 PropagatorTool = InDetTrigPatternPropagator,
                                                                 usePixel     = DetFlags.haveRIO.pixel_on(), 
                                                                 useSCT       = DetFlags.haveRIO.SCT_on(),
                                                                 RoadWidth    = InDetTrigCutValues.RoadWidth()
                                                                 )
ToolSvc += InDetTrigSiDetElementsRoadMaker


# Local combinatorial track finding using space point seed and detector element road
#
from SiCombinatorialTrackFinderTool_xk.SiCombinatorialTrackFinderTool_xkConf import InDet__SiCombinatorialTrackFinder_xk
InDetTrigSiComTrackFinder = \
                          InDet__SiCombinatorialTrackFinder_xk(name = 'InDetTrigSiComTrackFinder',
                                                               PropagatorTool	= InDetTrigPatternPropagator,
                                                               UpdatorTool	= InDetTrigPatternUpdator,
                                                               RIOonTrackTool   = InDetTrigRotCreator,
                                                               AssosiationTool  = InDetTrigPrdAssociationTool,
                                                               usePixel         = DetFlags.haveRIO.pixel_on(),
                                                               useSCT           = DetFlags.haveRIO.SCT_on(),   
                                                               PixelClusterContainer = 'PixelTrigClusters',
                                                               SCT_ClusterContainer = 'SCT_TrigClusters',
                                                               PixelSummarySvc = InDetTrigPixelConditionsSummarySvc,
                                                               SctSummarySvc = InDetTrigSCTConditionsSummarySvc
                                                               )															
ToolSvc += InDetTrigSiComTrackFinder

from InDetAmbiTrackSelectionTool.InDetAmbiTrackSelectionToolConf import InDet__InDetAmbiTrackSelectionTool
InDetTrigAmbiTrackSelectionTool = \
    InDet__InDetAmbiTrackSelectionTool(name               = 'InDetTrigAmbiTrackSelectionTool',
                                       AssociationTool    = InDetTrigPrdAssociationTool,
                                       DriftCircleCutTool = InDetTrigTRTDriftCircleCut,
                                       minHits         = InDetTrigCutValues.minClusters(),
                                       minNotShared    = InDetTrigCutValues.minSiNotShared(),
                                       maxShared       = InDetTrigCutValues.maxShared(),
                                       minTRTHits      = 0,  # used for Si only tracking !!!
                                       Cosmics         = False,  #there is a different instance
                                       UseParameterization = False,
                                       # sharedProbCut   = 0.10,
                                       # doPixelSplitting = InDetTrigFlags.doPixelClusterSplitting()
                                       )
 
 
ToolSvc += InDetTrigAmbiTrackSelectionTool
if (InDetTrigFlags.doPrintConfigurables()):
  print InDetTrigAmbiTrackSelectionTool



#
# ------ load new track selector (common for all vertexing algorithms, except for the moment VKalVrt
#
from InDetTrigRecExample.ConfiguredVertexingTrigCuts import EFIDVertexingCuts
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool

InDetTrigTrackSelectorTool = \
    InDet__InDetTrackSelectionTool(name = "InDetTrigDetailedTrackSelectorTool",
                                   CutLevel                   =  EFIDVertexingCuts.TrackCutLevel(),
                                   minPt                      =  EFIDVertexingCuts.minPT(),
                                   maxD0			=  EFIDVertexingCuts.IPd0Max(),
                                   maxZ0			=  EFIDVertexingCuts.z0Max(),
                                   maxZ0SinTheta              =  EFIDVertexingCuts.IPz0Max(),
                                   maxSigmaD0 = EFIDVertexingCuts.sigIPd0Max(),
                                   maxSigmaZ0SinTheta = EFIDVertexingCuts.sigIPz0Max(),
                                   # maxChiSqperNdf = EFIDVertexingCuts.fitChi2OnNdfMax(), # Seems not to be implemented?
                                   maxAbsEta = EFIDVertexingCuts.etaMax(),
                                   minNInnermostLayerHits = EFIDVertexingCuts.nHitInnermostLayer(),
                                   minNPixelHits = EFIDVertexingCuts.nHitPix(),
                                   maxNPixelHoles = EFIDVertexingCuts.nHolesPix(),
                                   minNSctHits = EFIDVertexingCuts.nHitSct(),
                                   minNTrtHits = EFIDVertexingCuts.nHitTrt(),
                                   minNSiHits = EFIDVertexingCuts.nHitSi(),
                                   TrackSummaryTool =  InDetTrigTrackSummaryTool,
                                   Extrapolator     = InDetTrigExtrapolator,
                                   #TrtDCCutTool     = InDetTrigTRTDriftCircleCut,
                                   )



ToolSvc += InDetTrigTrackSelectorTool
if (InDetTrigFlags.doPrintConfigurables()):
  print      InDetTrigTrackSelectorTool

