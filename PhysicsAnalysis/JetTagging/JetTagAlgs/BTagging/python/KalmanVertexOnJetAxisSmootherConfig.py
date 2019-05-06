# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

# import the KalmanVertexOnJetAxisSmoother configurable
from TrkJetVxFitter.TrkJetVxFitterConf import Trk__KalmanVertexOnJetAxisSmoother

def KalmanVertexOnJetAxisSmootherCfg(name, **options):
    acc = ComponentAccumulator()
    options['name'] = name
    acc.setPrivateTools(Trk__KalmanVertexOnJetAxisSmoother(**options))
    
    return acc
