# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainDict
from TriggerMenuMT.HLTMenuConfig.MET.METChainConfiguration import MetChainConfiguration as MetChainConfiguration
from TriggerMenuMT.HLTMenuConfig.Menu.ChainMerging import mergeChainDefs


from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.MET.generateChainConfigs' )
log.info("Importing %s",__name__)



def generateChainConfigs( chainDict ):
    import pprint
    pprint.pprint( chainDict )

    
    listOfChainDicts = splitChainDict(chainDict)
    log.debug("Implement case for met chain with %d legs ",len(listOfChainDicts))
        
    listOfChainDefs = []

    for subChainDict in listOfChainDicts:
        
        Met = MetChainConfiguration(subChainDict).assembleChain() 

        listOfChainDefs += [Met]
        log.debug('length of chaindefs %s', len(listOfChainDefs) )
        

    if len(listOfChainDefs)>1:
        log.debug("Implement case for mulit-leg met chain")
        theChainDef = mergeChainDefs(listOfChainDefs, chainDict)
    else:
        theChainDef = listOfChainDefs[0]

    log.debug("theChainDef %s" , theChainDef)

    return theChainDef



    

    
