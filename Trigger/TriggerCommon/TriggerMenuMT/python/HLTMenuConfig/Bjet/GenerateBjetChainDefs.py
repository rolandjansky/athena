# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from ..Menu.ChainDictTools import splitChainDict
from .BjetChainConfiguration import BjetChainConfiguration
from ..Jet.JetChainConfiguration import JetChainConfiguration
from ..Menu.ChainMerging import mergeChainDefs

import pprint
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)
log.info("Importing %s",__name__)

def generateChainConfigs( chainDict ):
    log.debug('dictionary is: %s\n', pprint.pformat(chainDict))


    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    jet = JetChainConfiguration(chainDict).assembleChain()

    for subChainDict in listOfChainDicts:

        # don't setup btagging for legs that are jet-only
        # happens for bjet + normal jet chains
        if subChainDict['chainParts'][0]['signature'] != 'Bjet':
            continue

        Bjet = BjetChainConfiguration(subChainDict, jet).assembleChain()

        listOfChainDefs += [Bjet]

    if len(listOfChainDefs)>1:
        theBjetChainDef = mergeChainDefs(listOfChainDefs, chainDict)
        jet.steps = jet.steps + theBjetChainDef.steps
    else:
        jet.steps = jet.steps + Bjet.steps

    theChainDef = jet

    return theChainDef
