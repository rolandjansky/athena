# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
from InDetRecExample import TrackingCommon as TrackingCommon

print("EMCommonRefitter.py")


def getGSFTrackFitter():

    # setup Rot and error scaling
    egRotCreator = TrackingCommon.getInDetRotCreator(
        name='egRotCreator',
        private=True)

    TrackingCommon.createAndAddCondAlg(
        TrackingCommon.getRIO_OnTrackErrorScalingCondAlg,
        'RIO_OnTrackErrorScalingCondAlg')

    # setup Rk propagator
    from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import (
        Trk__RungeKuttaPropagator as Propagator)

    egTrkPropagator = Propagator(name='egTrkPropagator')
    egTrkPropagator.AccuracyParameter = 0.0001

    # setup Navigator
    egTrkNavigator = TrackingCommon.getInDetNavigator(
        name='egTrkNavigator',
        private=True)

    # setup the GSF
    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import (
        Trk__GsfMaterialMixtureConvolution)

    GsfMaterialUpdator = Trk__GsfMaterialMixtureConvolution(
        name='GsfMaterialUpdator',
        MaximumNumberOfComponents=12)

    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import (
        Trk__GsfExtrapolator)

    GsfExtrapolator = Trk__GsfExtrapolator(
        name='GsfExtrapolator',
        Propagators=[egTrkPropagator],
        SearchLevelClosestParameters=10,
        StickyConfiguration=True,
        Navigator=egTrkNavigator,
        GsfMaterialConvolution=GsfMaterialUpdator,
        SurfaceBasedMaterialEffects=False)

    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import (
        Trk__GaussianSumFitter)

    GSFTrackFitter = Trk__GaussianSumFitter(
        name='GSFTrackFitter',
        ToolForExtrapolation=GsfExtrapolator,
        ReintegrateOutliers=True,
        MakePerigee=True,
        RefitOnMeasurementBase=True,
        DoHitSorting=True,
        ToolForROTCreation=egRotCreator)
    # --- end of fitter loading
    return GSFTrackFitter
