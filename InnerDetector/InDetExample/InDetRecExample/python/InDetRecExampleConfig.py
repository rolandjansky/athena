# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr,CfgGetter
import AthenaCommon.SystemOfUnits as Units
from InDetRecExample.TrackingCommon import setDefaults,copyArgs

def setTool(prop,tool_name,kwargs) :
    if tool_name is None :
        kwargs.setdefault(prop, tool_name )
        return

    if prop not in kwargs :
        from AthenaCommon import CfgGetter
        if isinstance(tool_name, list) :
            tools=[]
            for a_tool_name in tool_name :
                tools.append( CfgGetter.getPublicTool(a_tool_name) )
            kwargs.setdefault(prop,tools)
        else:
            kwargs.setdefault(prop, CfgGetter.getPublicTool(tool_name) )

def stripArgs(kwargs, copy_list) :
    dict_copy = copyArgs(kwargs,copy_list)
    for an_arg in copy_list :
        kwargs.pop(an_arg,None)
    return dict_copy

def KalmanCompetingPixelClustersTool(name='KalmanCompetingPixelClustersTool',**kwargs):
    kwargs.setdefault('WeightCutValueBarrel',5.5)
    kwargs.setdefault('WeightCutValueEndCap',5.5)
    from InDetCompetingRIOsOnTrackTool.InDetCompetingRIOsOnTrackToolConf import InDet__CompetingPixelClustersOnTrackTool
    return InDet__CompetingPixelClustersOnTrackTool(name, **kwargs)

def KalmanCompetingSCT_ClustersTool(name='KalmanCompetingSCT_ClustersTool',**kwargs) :
    from InDetCompetingRIOsOnTrackTool.InDetCompetingRIOsOnTrackToolConf import InDet__CompetingSCT_ClustersOnTrackTool
    kwargs.setdefault('WeightCutValueBarrel',5.5)
    kwargs.setdefault('WeightCutValueEndCap',5.5)
    return InDet__CompetingSCT_ClustersOnTrackTool(name,**kwargs)

def KalmanCompetingRIOsTool(name='KalmanCompetingRIOsTool',**kwargs) :
    from TrkCompetingRIOsOnTrackTool.TrkCompetingRIOsOnTrackToolConf import Trk__CompetingRIOsOnTrackTool
    setTool('ToolForCompPixelClusters','KalmanCompetingPixelClustersTool', kwargs )
    setTool('ToolForCompSCT_Clusters', 'KalmanCompetingSCT_ClustersTool', kwargs )
    return Trk__CompetingRIOsOnTrackTool(name,**kwargs)

def InDetDNAdjustor(name='InDetDNAdjustor',**kwargs) :
    from TrkDynamicNoiseAdjustor.TrkDynamicNoiseAdjustorConf import Trk__InDetDynamicNoiseAdjustment
    return Trk__InDetDynamicNoiseAdjustment(name,**kwargs)

def InDetDNASeparator(name='InDetDNASeparator',**kwargs) :
    from InDetDNASeparator.InDetDNASeparatorConf import InDet__InDetDNASeparator
    return InDet__InDetDNASeparator(name,**kwargs)

def InDetAnnealFKFRef(name='InDetAnnealFKFRef',**kwargs) :
    from TrkKalmanFitter.TrkKalmanFitterConf import Trk__ForwardRefTrackKalmanFitter
    return Trk__ForwardRefTrackKalmanFitter(name,**kwargs)

def InDetFKFRef(name='InDetFKFRef',**kwargs) :
    kwargs.setdefault('StateChi2PerNDFPreCut',30.0)
    return InDetAnnealFKF(name,**kwargs)

def InDetAnnealFKF(name='InDetAnnealFKF',**kwargs) :
    from TrkKalmanFitter.TrkKalmanFitterConf import Trk__ForwardKalmanFitter
    return Trk__ForwardKalmanFitter(name,**kwargs)

def InDetFKF(name='InDetFKF',**kwargs) :
    kwargs.setdefault('StateChi2PerNDFPreCut',30.0)
    return InDetAnnealFKF(name,**kwargs)

def InDetBKS(name='InDetBKS',**kwargs) :
    from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanSmoother
    kwargs.setdefault('InitialCovarianceSeedFactor',200.)

    return Trk__KalmanSmoother(name,**kwargs)

def InDetAnnealBKS(name='InDetAnnealBKS',**kwargs) :
    from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanSmoother
    kwargs.setdefault('InitialCovarianceSeedFactor',200.)

    return Trk__KalmanSmoother(name,**kwargs)

def KalmanInternalDAF(name='KalmanInternalDAF',**kwargs) :
    from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanPiecewiseAnnealingFilter
    setTool('CompetingRIOsOnTrackCreator', 'KalmanCompetingRIOsTool',kwargs)
    setTool('BackwardSmoother', 'InDetAnnealBKS',kwargs)
    return Trk__KalmanPiecewiseAnnealingFilter(name,**kwargs)

def KalmanInternalDAFRef(name='KalmanInternalDAFRef',**kwargs) :
    setTool('ForwardFitter','InDetAnnealFKF',kwargs)
    setTool('BackwardSmoother','InDetBKS',kwargs)  # same tuning so far
    return KalmanInternalDAF(name,**kwargs)


def KOL_RecoveryID(name='KOL_RecoveryID',**kwargs) :
    from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanOutlierRecovery_InDet
    return Trk__KalmanOutlierRecovery_InDet(name,**kwargs)

def InDetKOL(name='InDetKOL',**kwargs) :
    from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanOutlierLogic
    kwargs.setdefault('TrackChi2PerNDFCut', 17.0)
    kwargs.setdefault('StateChi2PerNDFCut', 12.5)
    return Trk__KalmanOutlierLogic(name,**kwargs)

# def KalmanInternalDAFBase(name='KalmanInternalDAFBase',**kwargs) :
#     from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanPiecewiseAnnealingFilter
#    setTool('CompetingRIOsOnTrackCreator','KalmanCompetingRIOsTool',kwargs )
#    setTool('BackwardSmoother','InDetBKS',kwargs )
#    return Trk__KalmanPiecewiseAnnealingFilter(name,**kwargs)

def InDetMeasRecalibST(name='InDetMeasRecalibST',**kwargs) :
    from TrkKalmanFitter.TrkKalmanFitterConf import Trk__MeasRecalibSteeringTool

    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','RenounceInputHandles','nameSuffix'])

    from InDetRecExample import TrackingCommon as TrackingCommon
    if 'BroadPixelClusterOnTrackTool' not in kwargs :
        kwargs = setDefaults(kwargs,
                             BroadPixelClusterOnTrackTool = TrackingCommon.getInDetBroadPixelClusterOnTrackTool(**pix_cluster_on_track_args))
    if 'BroadSCT_ClusterOnTrackTool' not in kwargs :
        kwargs = setDefaults(kwargs,
                             BroadSCT_ClusterOnTrackTool  = TrackingCommon.getInDetBroadSCT_ClusterOnTrackTool())

    if 'CommonRotCreator' not in kwargs :
        kwargs=setDefaults(kwargs,
                           CommonRotCreator = TrackingCommon.getInDetRefitRotCreator(**pix_cluster_on_track_args))

    return Trk__MeasRecalibSteeringTool(name,**kwargs)

def InDetKalmanTrackFitterBase(name='InDetKalmanTrackFitterBase',**kwargs) :
    from InDetRecExample import TrackingCommon as TrackingCommon
    from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanFitter
    from AthenaCommon.AppMgr import ToolSvc

    nameSuffix=kwargs.pop('nameSuffix','')
    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','RenounceInputHandles'])
    if len(pix_cluster_on_track_args)>0 and len(nameSuffix)>0 :
        pix_cluster_on_track_args['nameSuffix']=nameSuffix

    kwargs.setdefault('ExtrapolatorHandle', TrackingCommon.getInDetExtrapolator())
    if 'RIO_OnTrackCreatorHandle' not in kwargs :
        from InDetRecExample import TrackingCommon as TrackingCommon
        kwargs=setDefaults(kwargs,
                            RIO_OnTrackCreatorHandle = TrackingCommon.getInDetRefitRotCreator(**pix_cluster_on_track_args))

    kwargs.setdefault('MeasurementUpdatorHandle', ToolSvc.InDetUpdator)
    setTool('KalmanSmootherHandle', 'InDetBKS', kwargs )
    setTool('KalmanOutlierLogicHandle', 'InDetKOL',kwargs )
    kwargs.setdefault('DynamicNoiseAdjustorHandle', None)
    kwargs.setdefault('BrempointAnalyserHandle', None)
    kwargs.setdefault('AlignableSurfaceProviderHandle',None)
    if len(pix_cluster_on_track_args)>0 :
        if 'RecalibratorHandle' not in kwargs :
            the_tool_name = 'InDetMeasRecalibST'
            kwargs.setdefault('RecalibratorHandle', CfgGetter.getPublicToolClone(the_tool_name+nameSuffix,the_tool_name, **pix_cluster_on_track_args))
    else :
        setTool('RecalibratorHandle', 'InDetMeasRecalibST', kwargs )
    # setTool('InternalDAFHandle','KalmanInternalDAF',kwargs )
    # from InDetRecExample.InDetJobProperties import InDetFlags
    # kwargs.setdefault('DoDNAForElectronsOnly', True if InDetFlags.doBremRecovery() and InDetFlags.trackFitterType() is 'KalmanFitter' else False)
    return Trk__KalmanFitter(name,**kwargs)

# def InDetKalmanDNATrackFitter(name='InDetKalmanDNATrackFitter',**kwargs) :
#        from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanFitter as ConfiguredKalmanFitter
#        setTool('ForwardKalmanFitterHandle','InDetFKF', kwargs )
#        setTool('DynamicNoiseAdjustorHandle','InDetDNAdjustor',kwargs)
#        setTool('BrempointAnalyserHandle','InDetDNASeparator',kwargs)
#        return InDetKalmanTrackFitterBase(name,**kwargs)

#def InDetKalmanTrackFitter(name='InDetKalmanTrackFitter',**kwargs) :
#    setTool('ForwardKalmanFitterHandle','InDetFKF', kwargs )
#    setTool('InternalDAFHandle','KalmanInternalDAF',kwargs )
#    return InDetKalmanTrackFitterBase(name,**kwargs)

def KalmanFitter(name='KalmanFitter',**kwargs) :
    setTool('ForwardKalmanFitterHandle','InDetFKF', kwargs )    
    from InDetRecExample.InDetJobProperties import InDetFlags
    setTool('DynamicNoiseAdjustorHandle', 'InDetDNAdjustor' if InDetFlags.doBremRecovery() else None,kwargs)
    setTool('BrempointAnalyserHandle', 'InDetDNASeparator' if InDetFlags.doBremRecovery() else None,kwargs)
    setTool('InternalDAFHandle','KalmanInternalDAF',kwargs )
    if InDetFlags.doBremRecovery() :
        kwargs.setdefault('DoDNAForElectronsOnly',True)
    return InDetKalmanTrackFitterBase(name,**kwargs)

def ReferenceKalmanFitter(name='ReferenceKalmanFitter',**kwargs) :
    setTool('ForwardKalmanFitterHandle','InDetFKFRef', kwargs )
    setTool('InternalDAFHandle','KalmanInternalDAFRef',kwargs )
    return InDetKalmanTrackFitterBase(name,**kwargs)

def KalmanDNAFitter(name='KalmanDNAFitter',**kwargs) :
    setTool('ForwardKalmanFitterHandle','InDetFKF', kwargs )
    setTool('DynamicNoiseAdjustorHandle', 'InDetDNAdjustor',kwargs )
    setTool('BrempointAnalyserHandle', 'InDetDNASeparator', kwargs )
    setTool('InternalDAFHandle','KalmanInternalDAF',kwargs )
    return InDetKalmanTrackFitterBase(name,**kwargs)

def DistributedKalmanFilter(name="DistributedKalmanFilter", **kwargs) :
    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','RenounceInputHandles','nameSuffix'])

    from InDetRecExample.TrackingCommon          import setDefaults
    if 'ExtrapolatorTool' not in kwargs :
        kwargs = setDefaults(kwargs, ExtrapolatorTool = TrackingCommon.getInDetExtrapolator())

    from InDetRecExample import TrackingCommon as TrackingCommon
    if 'ROTcreator' not in kwargs :
        kwargs=setDefaults(kwargs,
                           ROTcreator = TrackingCommon.getInDetRotCreator(**pix_cluster_on_track_args))

    # @TODO set sortingReferencePoint = ???
    from TrkDistributedKalmanFilter.TrkDistributedKalmanFilterConf import Trk__DistributedKalmanFilter
    return Trk__DistributedKalmanFilter(name = name, **kwargs)


def InDetGlobalChi2FitterBase(name='GlobalChi2FitterBase', **kwargs) :
    from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
    from InDetRecExample.TrackingCommon          import setDefaults
    from AthenaCommon.AppMgr                     import ToolSvc
    from InDetRecExample.InDetJobProperties      import InDetFlags
    import InDetRecExample.TrackingCommon as TrackingCommon

    kwargs=setDefaults(kwargs,
                       ExtrapolationTool      = TrackingCommon.getInDetExtrapolator(),
                       NavigatorTool          = TrackingCommon.getInDetNavigator(),
                       PropagatorTool         = TrackingCommon.getInDetPropagator(),
                       MultipleScatteringTool = TrackingCommon.getInDetMultipleScatteringUpdator(),
                       MeasurementUpdateTool  = ToolSvc.InDetUpdator,
                       TrackingGeometrySvc    = AtlasTrackingGeometrySvc,
                       MaterialUpdateTool     = TrackingCommon.getInDetMaterialEffectsUpdator(),
                       StraightLine           = not InDetFlags.solenoidOn(),
                       OutlierCut             = 4,
                       SignedDriftRadius      = True,
                       ReintegrateOutliers    = True,
                       RecalibrateSilicon     = True,
                       RecalibrateTRT         = True,
                       TRTTubeHitCut          = 1.75, # use tighter hit classification, old: TrackingCommon.default_ScaleHitUncertainty
                       MaxIterations          = 40,
                       Acceleration           = True,
                       RecalculateDerivatives = InDetFlags.doMinBias() or InDetFlags.doCosmics() or InDetFlags.doBeamHalo(),
                       TRTExtensionCuts       = True,
                       TrackChi2PerNDFCut     = 7)
    from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter
    return Trk__GlobalChi2Fitter(name, **kwargs)

def InDetGlobalChi2Fitter(name='InDetGlobalChi2Fitter', **kwargs) :
    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','RenounceInputHandles','nameSuffix'])

    from InDetRecExample import TrackingCommon as TrackingCommon
    if 'RotCreatorTool' not in kwargs :
        kwargs=setDefaults(kwargs,
                           RotCreatorTool        = TrackingCommon.getInDetRotCreator(**pix_cluster_on_track_args))

    from InDetRecExample.InDetJobProperties import InDetFlags
    use_broad_cluster_any = InDetFlags.useBroadClusterErrors() and (not InDetFlags.doDBMstandalone())
    if 'BroadRotCreatorTool' not in kwargs and  not InDetFlags.doRefit():
        kwargs=setDefaults(kwargs,
                           BroadRotCreatorTool   = TrackingCommon.getInDetBroadRotCreator(**pix_cluster_on_track_args))

    if InDetFlags.doDBMstandalone():
        kwargs=setDefaults(kwargs,
                           StraightLine        = True,
                           OutlierCut          = 5,
                           RecalibrateTRT      = False,
                           TRTExtensionCuts    = False,
                           TrackChi2PerNDFCut  = 20)

    if InDetFlags.doRefit() or use_broad_cluster_any is True:
        kwargs=setDefaults(kwargs,
                           RecalibrateSilicon  = False)
    if InDetFlags.doRefit():
        kwargs=setDefaults(kwargs,
                           BroadRotCreatorTool = None,
                           ReintegrateOutliers = False,
                           RecalibrateTRT      = False)
    if InDetFlags.doRobustReco():
        kwargs=setDefaults(kwargs,
                           # BroadRotCreatorTool = None
                           OutlierCut          = 10.0,
                           TrackChi2PerNDFCut  = 20)

    if InDetFlags.doRobustReco() or InDetFlags.doCosmics():
        kwargs=setDefaults(kwargs,
                           MaxOutliers         = 99)
    if InDetFlags.doCosmics() or InDetFlags.doBeamHalo():
        kwargs=setDefaults(kwargs,
                           Acceleration        = False)

    if InDetFlags.materialInteractions() and not InDetFlags.solenoidOn():
        kwargs=setDefaults(kwargs,
                           Momentum            = 1000.*Units.MeV)
    return InDetGlobalChi2FitterBase(name, **kwargs)

def InDetGlobalChi2FitterLowPt(name='InDetGlobalChi2FitterLowPt', **kwargs) :
    # @TODO TrackingGeometrySvc was not set but is set now
    #       RotCreatorTool and BroadRotCreatorTool not set
    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','RenounceInputHandles','nameSuffix'])

    from InDetRecExample import TrackingCommon as TrackingCommon
    if 'RotCreatorTool' not in kwargs :
        kwargs=setDefaults(kwargs,
                           RotCreatorTool        = TrackingCommon.getInDetRotCreator(**pix_cluster_on_track_args))

    from InDetRecExample.InDetJobProperties import InDetFlags
    use_broad_cluster_any = InDetFlags.useBroadClusterErrors() and (not InDetFlags.doDBMstandalone())
    if 'BroadRotCreatorTool' not in kwargs and  not InDetFlags.doRefit():
        kwargs=setDefaults(kwargs,
                           BroadRotCreatorTool   = TrackingCommon.getInDetBroadRotCreator(**pix_cluster_on_track_args))

    return  InDetGlobalChi2FitterBase(name, **setDefaults(kwargs,
                                                          OutlierCut             = 5.0,
                                                          Acceleration           = False, # default
                                                          RecalculateDerivatives = True,
                                                          TrackChi2PerNDFCut     = 10))


def InDetGlobalChi2FitterTRT(name='InDetGlobalChi2FitterTRT', **kwargs) :
    '''
    Global Chi2 Fitter for TRT segments with different settings
    '''
    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','RenounceInputHandles','nameSuffix'])

    if 'RotCreatorTool' not in kwargs :
        from InDetRecExample import TrackingCommon as TrackingCommon
        kwargs=setDefaults(kwargs,
                           RotCreatorTool = TrackingCommon.getInDetRefitRotCreator(**pix_cluster_on_track_args))

    from InDetRecExample.InDetJobProperties import InDetFlags
    return  InDetGlobalChi2FitterBase(name, **setDefaults(
        kwargs,
        MaterialUpdateTool     = '',       # default
        TrackingGeometrySvc    = '',       # default
        SignedDriftRadius      = True,     # default,
        RecalibrateSilicon     = False,    # default,
        RecalibrateTRT         = False,    # default,
        TRTTubeHitCut          = 2.5,      # default,
        MaxIterations          = 10,
        Acceleration           = False,    # default,
        RecalculateDerivatives = False,
        TRTExtensionCuts       = True,     # default,
        TrackChi2PerNDFCut     = 999999,
        Momentum               = 1000.*Units.MeV   if InDetFlags.materialInteractions() and not InDetFlags.solenoidOn() else  0,     # default,
        OutlierCut             = 5,        # default
        MaxOutliers            = 99                if InDetFlags.doRobustReco() or InDetFlags.doCosmics()               else  10,    # default,
        ReintegrateOutliers    = False             if InDetFlags.doRefit()                                              else  False  # default
       ))

def InDetGlobalChi2FitterDBM(name='InDetGlobalChi2FitterDBM', **kwargs) :
    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','RenounceInputHandles','nameSuffix'])
    if 'RotCreatorTool' not in kwargs :
        from InDetRecExample import TrackingCommon as TrackingCommon
        kwargs=setDefaults(kwargs, RotCreatorTool = TrackingCommon.getInDetRotCreatorDBM(**pix_cluster_on_track_args))

    return  InDetGlobalChi2FitterBase(name, **setDefaults(kwargs,
                                                          BroadRotCreatorTool   = None,
                                                          StraightLine          = True,
                                                          OutlierCut            = 5,
                                                          RecalibrateTRT        = False,
                                                          RecalculateDerivatives= False,
                                                          TRTExtensionCuts      = False,
                                                          TrackChi2PerNDFCut    = 20,
                                                          Momentum              = 1000.*Units.MeV))

def GaussianSumFitter(name='GaussianSumFitter', **kwargs) :
    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','RenounceInputHandles','nameSuffix'])

    from InDetRecExample import TrackingCommon as TrackingCommon
    if 'ToolForROTCreation' not in kwargs :
        kwargs=setDefaults(kwargs,
                           ToolForROTCreation           = TrackingCommon.getInDetRotCreator(**pix_cluster_on_track_args))

    if 'ToolForExtrapolation' not in kwargs :
        kwargs=setDefaults(kwargs, ToolForExtrapolation = TrackingCommon.getInDetGsfExtrapolator())

    if 'MeasurementUpdatorType' not in kwargs :
        kwargs=setDefaults(kwargs, MeasurementUpdatorType = TrackingCommon.getInDetGsfMeasurementUpdator())

    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GaussianSumFitter
    return Trk__GaussianSumFitter(name = name, **setDefaults(kwargs,
                                                                 ReintegrateOutliers     = False,
                                                                 MakePerigee             = True,
                                                                 RefitOnMeasurementBase  = True,
                                                                 DoHitSorting            = True))

def InDetTrackFitter(name='InDetTrackFitter', **kwargs) :
    from InDetRecExample.InDetJobProperties import InDetFlags
    return {
        'KalmanFitter'            : KalmanFitter,
        'KalmanDNAFitter'         : KalmanDNAFitter,
        'ReferenceKalmanFitter'   : ReferenceKalmanFitter,
        'DistributedKalmanFilter' : DistributedKalmanFilter,
        'GlobalChi2Fitter'        : InDetGlobalChi2Fitter,
        'GaussianSumFilter'       : GaussianSumFitter
    }[InDetFlags.trackFitterType()](name,**kwargs)

def InDetTrackFitterLowPt(name='InDetTrackFitter', **kwargs) :
    from InDetRecExample.InDetJobProperties import InDetFlags
    if InDetFlags.trackFitterType() != 'GlobalChi2Fitter' :
        return InDetTrackFitter(name,**kwargs)
    else :
        return InDetGlobalChi2FitterLowPt(name,**kwargs)

def InDetTrackFitterTRT(name='InDetTrackFitterTRT', **kwargs) :
    from InDetRecExample.InDetJobProperties import InDetFlags
    if InDetFlags.trackFitterType() != 'GlobalChi2Fitter' :
        return InDetTrackFitter(name,**kwargs)
    else :
        return InDetGlobalChi2FitterTRT(name,**kwargs)

def InDetTrackFitterDBM(name='InDetTrackFitterDBM', **kwargs) :
    return InDetGlobalChi2FitterDBM(name,**kwargs)
