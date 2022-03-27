# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from .JetChainConfiguration import JetChainConfiguration
from TriggerMenuMT.HLT.Config.Utility.ChainMerging import mergeChainDefs

import pprint
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)
log.info("Importing %s",__name__)



def generateChainConfigs( chainDict ):
    log.debug('full jet dictionary is: %s\n', pprint.pformat(chainDict))

    # Jet chain is assembled always from the full dictionary (multiple legs are handled internally by the jet reco / hypo)
    theChainDef = JetChainConfiguration(chainDict)

    listOfChainDefs = []

    for leg in range(len(chainDict['chainParts'])):

        listOfChainDefs += [theChainDef.assembleChain()]

    if len(listOfChainDefs) > 1:
        jetChain = mergeChainDefs(listOfChainDefs, chainDict) 
    else:
        jetChain = listOfChainDefs[0]
    return jetChain



    

    
