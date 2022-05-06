# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def EventInfoBeamSpotDecoratorAlgCfg(flags, name="EventInfoBeamSpotDecoratorAlg", **kwargs):

    result=ComponentAccumulator()
    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    result.merge(BeamSpotCondAlgCfg(flags))

    result.addEventAlgo(CompFactory.xAODMaker.EventInfoBeamSpotDecoratorAlg(name,**kwargs))

    return result
