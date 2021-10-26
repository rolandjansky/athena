# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def BTagHighLevelAugmenterAlgCfg(ConfigFlags, JetCollection, BTagCollection, Associator,  TrackCollection, doFlipTagger=False):

    name = (BTagCollection + 'augment').lower()
    flip_config = 'FLIP_SIGN' if doFlipTagger else 'STANDARD'

    btagAug = CompFactory.FlavorTagDiscriminants.BTagAugmenterTool(
        name=name,
        flipTagConfig=flip_config,
        trackAssociator=Associator,
    )

    decorAlg = CompFactory.FlavorTagDiscriminants.BTagDecoratorAlg(
        name=f'{name}_alg',
        container=BTagCollection,
        constituentContainer=TrackCollection,
        decorator=btagAug,
    )
    # -- create the augmentation algorithm
    acc = ComponentAccumulator()
    acc.addEventAlgo(decorAlg)

    return acc
