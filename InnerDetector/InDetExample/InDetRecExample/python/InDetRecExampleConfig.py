# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import AthenaCommon.SystemOfUnits as Units
from InDetRecExample.TrackingCommon import setDefaults,copyArgs,createAndAddCondAlg

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

def DistributedKalmanFilter(name="DistributedKalmanFilter", **kwargs) :
    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])
    from InDetRecExample                         import TrackingCommon
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


def InDetGlobalChi2FitterBase(name='GlobalChi2FitterBase', **kwargs):

    from InDetRecExample.TrackingCommon import setDefaults
    from AthenaCommon.AppMgr import ToolSvc
    from InDetRecExample.InDetJobProperties import InDetFlags
    import InDetRecExample.TrackingCommon as TrackingCommon

    if 'TrackingGeometryReadKey' not in kwargs:
        cond_alg = createAndAddCondAlg(TrackingCommon.getTrackingGeometryCondAlg,
                                       "AtlasTrackingGeometryCondAlg",
                                       name="AtlasTrackingGeometryCondAlg")
        kwargs['TrackingGeometryReadKey'] = cond_alg.TrackingGeometryWriteKey

    kwargs = setDefaults(kwargs,
                         ExtrapolationTool=TrackingCommon.getInDetExtrapolator(),
                         NavigatorTool=TrackingCommon.getInDetNavigator(),
                         PropagatorTool=TrackingCommon.getInDetPropagator(),
                         MultipleScatteringTool=TrackingCommon.getInDetMultipleScatteringUpdator(),
                         MeasurementUpdateTool=ToolSvc.InDetUpdator,
                         MaterialUpdateTool=TrackingCommon.getInDetMaterialEffectsUpdator(),
                         StraightLine=not InDetFlags.solenoidOn(),
                         OutlierCut=4,
                         SignedDriftRadius=True,
                         ReintegrateOutliers=True,
                         RecalibrateSilicon=True,
                         RecalibrateTRT=True,
                         # use tighter hit classification, old: 
                         # TrackingCommon.default_ScaleHitUncertainty
                         TRTTubeHitCut=1.75,
                         MaxIterations=40,
                         Acceleration=True,
                         RecalculateDerivatives=InDetFlags.doMinBias(
                         ) or InDetFlags.doCosmics() or InDetFlags.doBeamHalo(),
                         TRTExtensionCuts=True,
                         TrackChi2PerNDFCut=7)

    from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter
    return Trk__GlobalChi2Fitter(name, **kwargs)

def InDetGlobalChi2Fitter(name='InDetGlobalChi2Fitter', **kwargs) :
    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])
    if 'ClusterSplitProbabilityName' in pix_cluster_on_track_args:
      kwargs=setDefaults(kwargs,
                       ClusterSplitProbabilityName = pix_cluster_on_track_args['ClusterSplitProbabilityName'] )
    from InDetRecExample import TrackingCommon as TrackingCommon

    # PHF cut during fit iterations to save CPU time
    kwargs=setDefaults(kwargs,
                       MinPHFCut                 = TrackingCommon.getInDetNewTrackingCuts().minTRTPrecFrac()) 

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

def InDetGlobalChi2FitterBT(name='InDetGlobalChi2FitterBT', **kwargs):
    '''
    Global Chi2 Fitter for backtracking
    '''
    kwargs=setDefaults(kwargs,
                       MinPHFCut = 0.)
    return InDetGlobalChi2Fitter(name, **kwargs)


def InDetGlobalChi2FitterLowPt(name='InDetGlobalChi2FitterLowPt', **kwargs) :
    # @TODO TrackingGeometrySvc was not set but is set now
    #       RotCreatorTool and BroadRotCreatorTool not set
    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])

    from InDetRecExample import TrackingCommon as TrackingCommon
    if 'RotCreatorTool' not in kwargs :
        kwargs=setDefaults(kwargs,
                           RotCreatorTool        = TrackingCommon.getInDetRotCreator(**pix_cluster_on_track_args))

    from InDetRecExample.InDetJobProperties import InDetFlags
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
    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])

    if 'RotCreatorTool' not in kwargs :
        from InDetRecExample import TrackingCommon as TrackingCommon
        kwargs=setDefaults(kwargs,
                           RotCreatorTool = TrackingCommon.getInDetRefitRotCreator(**pix_cluster_on_track_args))

    from InDetRecExample.InDetJobProperties import InDetFlags
    return  InDetGlobalChi2FitterBase(name, **setDefaults(
        kwargs,
        MaterialUpdateTool     = '',       # default
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
    pix_cluster_on_track_args = stripArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])
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


def GaussianSumFitter(name='GaussianSumFitter', **kwargs):
    import egammaRec.EMCommonRefitter
    kwargs.setdefault("RefitOnMeasurementBase", True)
    return egammaRec.EMCommonRefitter.getGSFTrackFitter(name, **kwargs)

def InDetTrackFitter(name='InDetTrackFitter', **kwargs) :
    from InDetRecExample.InDetJobProperties import InDetFlags
    return {
        'DistributedKalmanFilter' : DistributedKalmanFilter,
        'GlobalChi2Fitter'        : InDetGlobalChi2Fitter,
        'GaussianSumFilter'       : GaussianSumFitter
    }[InDetFlags.trackFitterType()](name,**kwargs)

def InDetTrackFitterBT(name='InDetTrackFitterBT', **kwargs) :
    from InDetRecExample.InDetJobProperties import InDetFlags
    if InDetFlags.trackFitterType() != 'GlobalChi2Fitter' :
        return InDetTrackFitter(name,**kwargs)
    else :
        return InDetGlobalChi2FitterBT(name,**kwargs)

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
