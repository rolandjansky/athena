# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of TrkMeasurementUpdator_xk and TrkMeasurementUpdator packages
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def KalmanUpdator_xkCfg(flags, name='KalmanUpdator_xk', **kwargs):
    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.Trk.KalmanUpdator_xk(name, **kwargs))
    return result


def KalmanUpdatorCfg(flags, name='KalmanUpdator', **kwargs):
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.Trk.KalmanUpdator(name, **kwargs))
    return acc


def InDetUpdatorCfg(flags, name='InDetUpdator', **kwargs):
    if flags.Detector.GeometryITk:
        name = name.replace("InDet", "ITk")
        return ITkUpdatorCfg(flags, name, **kwargs)

    acc = ComponentAccumulator()

    tool = None
    if flags.InDet.Tracking.kalmanUpdator == "fast":
        tool = CompFactory.Trk.KalmanUpdator_xk(name, **kwargs)
    elif flags.InDet.Tracking.kalmanUpdator == "weight":
        tool = CompFactory.Trk.KalmanWeightUpdator(name, **kwargs)
    elif flags.InDet.Tracking.kalmanUpdator == "smatrix":
        tool = CompFactory.Trk.KalmanUpdatorSMatrix(name, **kwargs)
    elif flags.InDet.Tracking.kalmanUpdator == "amg":
        tool = CompFactory.Trk.KalmanUpdatorAmg(name, **kwargs)
    else:
        tool = CompFactory.Trk.KalmanUpdator(name, **kwargs)

    acc.setPrivateTools(tool)
    return acc

# Needed as long as flags.InDet.Tracking vs flags.ITk.Tracking are used
# To remove ultimately after Tracking flags are unified


def ITkUpdatorCfg(flags, name='ITkUpdator', **kwargs):
    result = ComponentAccumulator()

    tool = None
    if flags.ITk.Tracking.kalmanUpdator == "fast":
        tool = CompFactory.Trk.KalmanUpdator_xk(name, **kwargs)
    elif flags.ITk.Tracking.kalmanUpdator == "weight":
        tool = CompFactory.Trk.KalmanWeightUpdator(name, **kwargs)
    elif flags.ITk.Tracking.kalmanUpdator == "smatrix":
        tool = CompFactory.Trk.KalmanUpdatorSMatrix(name, **kwargs)
    elif flags.ITk.Tracking.kalmanUpdator == "amg":
        tool = CompFactory.Trk.KalmanUpdatorAmg(name, **kwargs)
    else:
        tool = CompFactory.Trk.KalmanUpdator(name, **kwargs)

    result.setPrivateTools(tool)
    return result
