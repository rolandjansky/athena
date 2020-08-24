# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

Analysis__BTagHighLevelAugmenterAlg=CompFactory.Analysis.BTagHighLevelAugmenterAlg

def BTagHighLevelAugmenterAlgCfg(ConfigFlags, sequenceName, JetCollection, BTagCollection, Associator, doFlipTagger=False, **options):
    """Adds a SecVtxTool instance and registers it.

    input: name:               The algorithm's name.
           BTaggingCollectionName       The name of the btagging collections.
           options:            Python dictionary of options to be passed to the algorithm.
    output: The tool."""

    acc = ComponentAccumulator(sequenceName)

    options = {}
    options['JetCollectionName'] = JetCollection.replace('Track', 'PV0Track') + 'Jets'
    options['BTaggingCollectionName'] = BTagCollection
    options['JetLinkName'] = options['BTaggingCollectionName'] + '.jetLink'
    options['BTagTrackToJetAssociatorName'] = Associator
    options['name'] = (BTagCollection + '_Augment').lower()
    if doFlipTagger: options['FlipTagConfig'] = 'FLIP_SIGN'

    # -- create the augmentation algorithm
    acc.addEventAlgo(Analysis__BTagHighLevelAugmenterAlg(**options), sequenceName)

    return acc
