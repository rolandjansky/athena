# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from ..Menu.ChainDictTools import splitChainDict
from ..Menu.ChainMerging import mergeChainDefs
from .TestChainConfiguration import TestChainConfiguration

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)
log.info("Importing %s",__name__)



def generateChainConfigs( chainDict ):
    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    for subChainDict in listOfChainDicts:
        log.debug('Assembling subChainsDict %s for chain %s', len(listOfChainDefs), subChainDict['chainName'] )
        Test = TestChainConfiguration(subChainDict).assembleChain() 

        listOfChainDefs += [Test]
        

    if len(listOfChainDefs)>1:
        theChainDef = mergeChainDefs(listOfChainDefs, chainDict)
    else:
        theChainDef = listOfChainDefs[0]

    return theChainDef



    

    
