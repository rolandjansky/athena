# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def BTagMuonAugmenterAlgCfg(ConfigFlags, BTagCollection, Associator,  MuonCollection):

    name = (BTagCollection + 'augment' + MuonCollection).lower()

    aug = CompFactory.FlavorTagDiscriminants.BTagMuonAugmenterTool(
        name=name,
        MuonAssociationName=Associator,
    )

    decorAlg = CompFactory.FlavorTagDiscriminants.BTagDecoratorAlg(
        name=f'{name}_alg',
        container=BTagCollection,
        constituentContainer=MuonCollection,
        decorator=aug,
    )
    # -- create the augmentation algorithm
    acc = ComponentAccumulator()
    acc.addEventAlgo(decorAlg)

    return acc
