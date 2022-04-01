# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def ITkPatternPropagatorCfg(flags, name='ITkPatternPropagator', **kwargs):
    result = ComponentAccumulator()
    tool = CompFactory.Trk.RungeKuttaPropagator(name, **kwargs)
    result.addPublicTool( tool, primary=True )
    return result

def ITkPatternUpdatorCfg(flags, name='ITkPatternUpdator', **kwargs):
    result = ComponentAccumulator()
    tool = CompFactory.Trk.KalmanUpdator_xk(name, **kwargs)
    result.setPrivateTools( tool )
    return result

def ITkUpdatorCfg(flags, name = 'ITkUpdator', **kwargs):
    result = ComponentAccumulator()

    tool = None
    if flags.ITk.Tracking.kalmanUpdator == "fast" :
        tool = CompFactory.Trk.KalmanUpdator_xk(name, **kwargs)
    elif flags.ITk.Tracking.kalmanUpdator == "weight" :
        tool = CompFactory.Trk.KalmanWeightUpdator(name, **kwargs)
    elif flags.ITk.Tracking.kalmanUpdator == "smatrix" :
        tool = CompFactory.Trk.KalmanUpdatorSMatrix(name, **kwargs)
    elif flags.ITk.Tracking.kalmanUpdator == "amg" :
        tool = CompFactory.Trk.KalmanUpdatorAmg(name, **kwargs)
    else :
        tool = CompFactory.Trk.KalmanUpdator(name, **kwargs)

    result.setPrivateTools( tool )
    return result
