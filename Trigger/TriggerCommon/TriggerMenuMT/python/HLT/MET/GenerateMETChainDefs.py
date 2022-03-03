# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLT.Config.Utility.ChainDictTools import splitChainDict
from TriggerMenuMT.HLT.Config.Utility.ChainMerging import mergeChainDefs
from .METChainConfiguration import METChainConfiguration

import pprint
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)
log.info("Importing %s",__name__)



def generateChainConfigs( chainDict ):
    log.debug('dictionary is: %s\n', pprint.pformat(chainDict))

    
    listOfChainDicts = splitChainDict(chainDict)
    log.debug("Implement case for met chain with %d legs ",len(listOfChainDicts))
        
    listOfChainDefs = []

    for subChainDict in listOfChainDicts:
        
        MET = METChainConfiguration(subChainDict).assembleChain() 

        listOfChainDefs += [MET]
        log.debug('length of chaindefs %s', len(listOfChainDefs) )
        

    if len(listOfChainDefs)>1:
        log.debug("Implement case for mulit-leg met chain")
        theChainDef = mergeChainDefs(listOfChainDefs, chainDict)
    else:
        theChainDef = listOfChainDefs[0]

    log.debug("theChainDef %s" , theChainDef)

    return theChainDef
