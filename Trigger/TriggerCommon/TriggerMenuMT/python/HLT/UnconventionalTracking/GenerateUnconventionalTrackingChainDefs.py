# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from .UnconventionalTrackingChainConfiguration import UnconventionalTrackingChainConfiguration
from TriggerMenuMT.HLT.Config.Utility.ChainDictTools import splitChainDict
from TriggerMenuMT.HLT.Config.Utility.ChainMerging import mergeChainDefs

import pprint
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)
log.info("Importing %s",__name__)



def generateChainConfigs( chainDict ):
    log.debug('dictionary is: %s\n', pprint.pformat(chainDict))

    listOfChainDicts = splitChainDict(chainDict)

    listOfChainDefs=[]
    for subChainDict in listOfChainDicts:
        subChain = UnconventionalTrackingChainConfiguration(subChainDict).assembleChain()
        listOfChainDefs += [subChain]

    log.debug('length of chaindefs %s', len(listOfChainDefs))

    if len(listOfChainDefs) > 1:
        chainDef = mergeChainDefs(listOfChainDefs, chainDict)
    else:
        chainDef = listOfChainDefs[0]

    log.debug('ChainDef %s', chainDef)
    return chainDef
