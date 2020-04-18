# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# import the KalmanVertexOnJetAxisSmoother configurable
Trk__KalmanVertexOnJetAxisSmoother=CompFactory.Trk.KalmanVertexOnJetAxisSmoother

def KalmanVertexOnJetAxisSmootherCfg(name, **options):
    acc = ComponentAccumulator()
    options['name'] = name
    acc.setPrivateTools(Trk__KalmanVertexOnJetAxisSmoother(**options))
    
    return acc
