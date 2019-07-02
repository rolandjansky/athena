# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ToolSvc, ServiceMgr
from egammaRec import egammaRecFlags as egRecFlags
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.DetFlags import DetFlags
from InDetRecExample.InDetJobProperties import InDetFlags
print "EMCommonRefitter.py"

egammaRecFlags = egRecFlags.jobproperties.egammaRecFlags

# This is more of legacy, in principle we want the one from std tracking
# Seems trigger somehow end up using it ....


def CreateEgammaRotCreator():
    global ToolSvc
    # Setup e/gamma offline RotCreator if one is not present

    if InDetFlags.doPixelClusterSplitting() and InDetFlags.pixelClusterSplittingType() == 'NeuralNet':
        # --- temp: read calib file
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if not hasattr(svcMgr, 'THistSvc'):
            from GaudiSvc.GaudiSvcConf import THistSvc
            svcMgr += THistSvc()

        # --- new NN factor
        # COOL binding
        # --- Select the necessary settings when running on run 1 data/MC
        # --- since a correction is needed to fix biases when running on new run 2 compatible calibation

        if not hasattr(ToolSvc, "PixelLorentzAngleTool"):
            from SiLorentzAngleTool.PixelLorentzAngleToolSetup import PixelLorentzAngleToolSetup
            pixelLorentzAngleToolSetup = PixelLorentzAngleToolSetup()

        from SiClusterizationTool.SiClusterizationToolConf import InDet__NnClusterizationFactory

        from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as geoFlags
        if (not geoFlags.Run() in ["RUN2", "RUN3"]):
            egNnClusterizationFactory = InDet__NnClusterizationFactory(name="egNnClusterizationFactory",
                                                                       PixelLorentzAngleTool=ToolSvc.PixelLorentzAngleTool,
                                                                       doRunI=True,
                                                                       useToT=False,
                                                                       useRecenteringNNWithoutTracks=True,
                                                                       useRecenteringNNWithTracks=False,
                                                                       correctLorShiftBarrelWithoutTracks=0,
                                                                       correctLorShiftBarrelWithTracks=0.030,
                                                                       NnCollectionReadKey='PixelClusterNN',
                                                                       NnCollectionWithTrackReadKey='PixelClusterNNWithTrack')

        else:
            egNnClusterizationFactory = InDet__NnClusterizationFactory(name="egNnClusterizationFactory",
                                                                       PixelLorentzAngleTool=ToolSvc.PixelLorentzAngleTool,
                                                                       useToT=InDetFlags.doNNToTCalibration(),
                                                                       NnCollectionReadKey='PixelClusterNN',
                                                                       NnCollectionWithTrackReadKey='PixelClusterNNWithTrack')

        ToolSvc += egNnClusterizationFactory

    # End of do cluster splitting
    # ----------- control loading of ROT_creator
    if DetFlags.haveRIO.pixel_on():
        # load Pixel ROT creator, we overwrite the defaults for the
        # tool to always make conservative pixel cluster errors
        if not hasattr(ToolSvc, "PixelLorentzAngleTool"):
            from SiLorentzAngleTool.PixelLorentzAngleToolSetup import PixelLorentzAngleToolSetup
            pixelLorentzAngleToolSetup = PixelLorentzAngleToolSetup()

        from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__PixelClusterOnTrackTool
        egPixelClusterOnTrackTool = InDet__PixelClusterOnTrackTool("egPixelClusterOnTrackTool",
                                                                   LorentzAngleTool=ToolSvc.PixelLorentzAngleTool,
                                                                   DisableDistortions=InDetFlags.doFatras(),
                                                                   applyNNcorrection=(InDetFlags.doPixelClusterSplitting()
                                                                                      and InDetFlags.pixelClusterSplittingType() == 'NeuralNet'))
        if InDetFlags.doPixelClusterSplitting() and InDetFlags.pixelClusterSplittingType() == 'NeuralNet':
            egPixelClusterOnTrackTool.NnClusterizationFactory = egNnClusterizationFactory

        ToolSvc += egPixelClusterOnTrackTool

    else:
        egPixelClusterOnTrackTool = None

    if DetFlags.haveRIO.SCT_on():
        # SiLorentzAngleTool
        from SiLorentzAngleTool.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup
        sctLorentzAngleToolSetup = SCTLorentzAngleToolSetup()
        from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__SCT_ClusterOnTrackTool
        egSCT_ClusterOnTrackTool = InDet__SCT_ClusterOnTrackTool("egSCT_ClusterOnTrackTool",
                                                                 CorrectionStrategy=0,  # do correct position bias
                                                                 ErrorStrategy=2,  # do use phi dependent errors
                                                                 LorentzAngleTool=sctLorentzAngleToolSetup.SCTLorentzAngleTool)
        ToolSvc += egSCT_ClusterOnTrackTool
    else:
        egSCT_ClusterOnTrackTool = None

    # default ROT creator, not smart !
    from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
    egRotCreator = Trk__RIO_OnTrackCreator(name='egRotCreator',
                                           ToolPixelCluster=egPixelClusterOnTrackTool,
                                           ToolSCT_Cluster=egSCT_ClusterOnTrackTool,
                                           Mode='indet')
    ToolSvc += egRotCreator
    # load error scaling
    # load error scaling
    from InDetRecExample.TrackingCommon import createAndAddCondAlg, getRIO_OnTrackErrorScalingCondAlg
    createAndAddCondAlg(getRIO_OnTrackErrorScalingCondAlg,
                        'RIO_OnTrackErrorScalingCondAlg')

    return egRotCreator

##############################################################################


def getGSFTrackFitter():
    global ToolSvc
    # The following perhaps we need to factor inside the classes using them
    # i.e BremCollection Builder or so
    # Deal with the Rot creator

    if hasattr(ToolSvc, 'InDetRotCreator'):
        egRotCreator = ToolSvc.InDetRotCreator
    else:
        egRotCreator = CreateEgammaRotCreator()

    # Extrapolator to be used for tracking inside egamma i.e for GSF and  Conversions
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

    # get Rk propagator
    from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
    egTrkPropagator = Propagator(name='egTrkPropagator')
    egTrkPropagator.AccuracyParameter = 0.0001

    # Setup the Navigator (default)
    from TrkExTools.TrkExToolsConf import Trk__Navigator
    egTrkNavigator = Trk__Navigator(name='egTrkNavigator')

    # Set up the GSF
    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfMaterialMixtureConvolution
    GsfMaterialUpdator = Trk__GsfMaterialMixtureConvolution(name='GsfMaterialUpdator')

    # component Reduction
    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__QuickCloseComponentsMultiStateMerger
    GsfComponentReduction = Trk__QuickCloseComponentsMultiStateMerger(name='GsfComponentReduction',
                                                                      MaximumNumberOfComponents=12)

    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfExtrapolator
    GsfExtrapolator = Trk__GsfExtrapolator(name='GsfExtrapolator',
                                           Propagators=[egTrkPropagator],
                                           SearchLevelClosestParameters=10,
                                           StickyConfiguration=True,
                                           Navigator=egTrkNavigator,
                                           GsfMaterialConvolution=GsfMaterialUpdator,
                                           ComponentMerger=GsfComponentReduction,
                                           SurfaceBasedMaterialEffects=False)
    ToolSvc += GsfExtrapolator

    from TrkMeasurementUpdator.TrkMeasurementUpdatorConf import Trk__KalmanUpdatorAmg as ConfiguredKalmanUpdator
    egTrkUpdator = ConfiguredKalmanUpdator('egTrkUpdator')

    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfMeasurementUpdator
    GsfMeasurementUpdator = Trk__GsfMeasurementUpdator(name='GsfMeasurementUpdator',
                                                       Updator=egTrkUpdator)

    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GaussianSumFitter
    GSFTrackFitter = Trk__GaussianSumFitter(name='GSFTrackFitter',
                                            ToolForExtrapolation=GsfExtrapolator,
                                            MeasurementUpdatorType=GsfMeasurementUpdator,
                                            ReintegrateOutliers=True,
                                            MakePerigee=True,
                                            RefitOnMeasurementBase=True,
                                            DoHitSorting=True,
                                            ToolForROTCreation=egRotCreator)
    # --- end of fitter loading
    return GSFTrackFitter
