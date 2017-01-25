# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr,CfgGetter

def setTool(prop,tool_name,kwargs) :
    if tool_name == None :
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
    from AthenaCommon.AppMgr import ToolSvc
    kwargs.setdefault('BroadPixelClusterOnTrackTool',ToolSvc.InDetBroadPixelClusterOnTrackTool)
    kwargs.setdefault('BroadSCT_ClusterOnTrackTool', ToolSvc.InDetBroadSCT_ClusterOnTrackTool)
    kwargs.setdefault('CommonRotCreator',            ToolSvc.InDetRefitRotCreator)
    return Trk__MeasRecalibSteeringTool(name,**kwargs)

def InDetKalmanTrackFitterBase(name='InDetKalmanTrackFitterBase',**kwargs) :
    from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanFitter
    from AthenaCommon.AppMgr import ToolSvc
    kwargs.setdefault('ExtrapolatorHandle', ToolSvc.InDetExtrapolator)
    kwargs.setdefault('RIO_OnTrackCreatorHandle', ToolSvc.InDetRefitRotCreator)
    kwargs.setdefault('MeasurementUpdatorHandle', ToolSvc.InDetUpdator)
    setTool('KalmanSmootherHandle', 'InDetBKS', kwargs )
    setTool('KalmanOutlierLogicHandle', 'InDetKOL',kwargs )
    kwargs.setdefault('DynamicNoiseAdjustorHandle', None)
    kwargs.setdefault('BrempointAnalyserHandle', None)
    kwargs.setdefault('AlignableSurfaceProviderHandle',None)
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
    setTool('BrempointAnalyserHandle', 'InDetDNASeparator', kwargs)
    setTool('InternalDAFHandle','KalmanInternalDAF',kwargs )
    return InDetKalmanTrackFitterBase(name,**kwargs)
