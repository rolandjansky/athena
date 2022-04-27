# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
from InDetRecExample import TrackingCommon as TrackingCommon


def getGSFTrackFitter(name="EMGSFTrackFitter",
                      **kwargs):

    if "RefitOnMeasurementBase" not in kwargs:
        kwargs["RefitOnMeasurementBase"] = True

    kwargs["ToolForROTCreation"] = None
    if not kwargs["RefitOnMeasurementBase"]:
        kwargs["ToolForROTCreation"] = TrackingCommon.getInDetRotCreator(
            name='egRotCreator',
            private=True)

        TrackingCommon.createAndAddCondAlg(
            TrackingCommon.getRIO_OnTrackErrorScalingCondAlg,
            'RIO_OnTrackErrorScalingCondAlg')

    if "ToolForExtrapolation" not in kwargs:
        # setup Rk propagator
        from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import (
            Trk__RungeKuttaPropagator as Propagator)

        egTrkPropagator = Propagator(name='egTrkPropagator')
        egTrkPropagator.AccuracyParameter = 0.0001

        # setup Navigator
        egTrkNavigator = TrackingCommon.getInDetNavigator(
            name='egTrkNavigator',
            private=True)

        from TrkGaussianSumFilter.TrkGaussianSumFilterConf import (
            Trk__ElectronMaterialMixtureConvolution)

        GsfMaterialUpdator = Trk__ElectronMaterialMixtureConvolution(
            name='GsfMaterialUpdator',
            MaximumNumberOfComponents=12)

        from TrkGaussianSumFilter.TrkGaussianSumFilterConf import (
            Trk__GsfExtrapolator)
        kwargs["ToolForExtrapolation"] = Trk__GsfExtrapolator(
            name='GsfExtrapolator',
            Propagator=egTrkPropagator,
            Navigator=egTrkNavigator,
            GsfMaterialConvolution=GsfMaterialUpdator)

    # setup the GSF
    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import (
        Trk__GaussianSumFitter)
    kwargs.setdefault("ReintegrateOutliers", True)
    kwargs.setdefault("MakePerigee", True)
    kwargs.setdefault("DoHitSorting", True)

    GSFTrackFitter = Trk__GaussianSumFitter(name, **kwargs)
    # --- end of fitter loading
    return GSFTrackFitter
