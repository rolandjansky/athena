# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetTruthAlgs package

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def InDetPRDTruthTrajectorySorterCfg(name='InDetTruthTrajectorySorter', **kwargs):
    result = ComponentAccumulator()

    result.setPrivateTools(CompFactory.InDet.PRD_TruthTrajectorySorterID(name, **kwargs))
    return result


def InDetPRD_ProviderCfg(name='InDetPRD_Provider', **kwargs):
    result = ComponentAccumulator()

    kwargs.setdefault('PixelClusterContainer', 'PixelClusters')
    kwargs.setdefault('SCT_ClusterContainer', 'SCT_Clusters')
    kwargs.setdefault('TRT_DriftCircleContainer', 'TRT_DriftCircles')

    result.setPrivateTools(CompFactory.InDet.InDetPRD_Provider(name, **kwargs))
    return result

def InDetPRD_TruthTrajectoryManipulatorIDCfg(name='InDetTruthTrajectoryManipulator'):
    result = ComponentAccumulator()

    result.setPrivateTools(CompFactory.InDet.PRD_TruthTrajectoryManipulatorID(name))
    return result

def InDetTruthTrackBuilderCfg(flags, name='InDetTruthTrackBuilder', **kwargs):
    result = ComponentAccumulator()

    from TrkConfig.CommonTrackFitterConfig import InDetTrackFitterCfg
    InDetTrackFitter = result.popToolsAndMerge(InDetTrackFitterCfg(flags))
    kwargs.setdefault('TrackFitter', InDetTrackFitter)

    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    InDetExtrapolator = result.getPrimaryAndMerge(InDetExtrapolatorCfg(flags))    
    kwargs.setdefault('ExtrapolationTool', InDetExtrapolator)

    from TrkConfig.TrkRIO_OnTrackCreatorConfig import InDetRotCreatorCfg
    InDetRotCreator = result.popToolsAndMerge(InDetRotCreatorCfg(flags))
    kwargs.setdefault('RotCreatorTool', InDetRotCreator)

    from TrkConfig.TrkRIO_OnTrackCreatorConfig import InDetBroadRotCreatorCfg
    InDetBroadRotCreator = result.popToolsAndMerge(InDetBroadRotCreatorCfg(flags))
    kwargs.setdefault('BroadRotCreatorTool', InDetBroadRotCreator)

    kwargs.setdefault('MinDegreesOfFreedom', 1)
    kwargs.setdefault('MatEffects', flags.InDet.Tracking.materialInteractionsType) #Does this need a default type of 0?
    kwargs.setdefault('MinSiHits', flags.InDet.Tracking.ActivePass.minClusters)

    result.setPrivateTools(CompFactory.Trk.TruthTrackBuilder(name, **kwargs))
    return result

def InDetPRD_TruthTrajectoryBuilderCfg(flags, name='InDetPRD_TruthTrajectoryBuilder', **kwargs):
    result = ComponentAccumulator()

    truthClusters = ['PRD_MultiTruthPixel', 'PRD_MultiTruthSCT', 'PRD_MultiTruthTRT']
    kwargs.setdefault('PRD_MultiTruthCollections', truthClusters)
    InDetPRD_Provider = result.popToolsAndMerge(InDetPRD_ProviderCfg())
    kwargs.setdefault('InDetPRD_Provider', InDetPRD_Provider)
    kwargs.setdefault('MinimumPt', flags.InDet.Tracking.ActivePass.minPT)

    InDetTruthTrajectorySorter = result.popToolsAndMerge(InDetPRDTruthTrajectorySorterCfg())
    manipulators = [ InDetTruthTrajectorySorter ]

    if not flags.InDet.Tracking.doIdealPseudoTracking:
        InDetTruthTrajectoryManipulator = result.popToolsAndMerge(InDetPRD_TruthTrajectoryManipulatorIDCfg())
        manipulators.append(InDetTruthTrajectoryManipulator)
    
    kwargs.setdefault('PRD_TruthTrajectoryManipulators', manipulators)

    result.setPrivateTools(CompFactory.Trk.PRD_TruthTrajectoryBuilder(name, **kwargs))
    return result

def InDetPRD_TruthTrajectorySelectorCfg(name='InDetTruthTrajectorySelector', **kwargs):
    result = ComponentAccumulator()

    result.setPrivateTools(CompFactory.InDet.PRD_TruthTrajectorySelectorID(name))
    return result  
