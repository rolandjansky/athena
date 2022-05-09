# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def KalmanVertexOnJetAxisSmootherCfg(name, **options):
    acc = ComponentAccumulator()
    options['name'] = name
    acc.setPrivateTools(CompFactory.Trk.KalmanVertexOnJetAxisSmoother(**options))
    
    return acc
