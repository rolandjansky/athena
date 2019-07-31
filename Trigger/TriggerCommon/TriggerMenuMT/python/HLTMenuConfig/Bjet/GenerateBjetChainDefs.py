# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainDict
from TriggerMenuMT.HLTMenuConfig.Bjet.BjetDef import BjetChainConfiguration as BjetChainConfiguration


from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Bjet.GenerateBjetChainConfigs' )
log.info("Importing %s",__name__)



def generateChainConfigs( chainDict ):
    import pprint
    pprint.pprint( chainDict )

    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    for subChainDict in listOfChainDicts:
        
        Bjet = BjetChainConfiguration(subChainDict).assembleChain() 

        listOfChainDefs += [Bjet]
        log.debug('length of chaindefs %s', len(listOfChainDefs) )
        

    if len(listOfChainDefs)>1:
        log.warning("Implement case for multi-bjet chain!!") 
        theChainDef = listOfChainDefs[0] #needs to be implemented properly
    else:
        theChainDef = listOfChainDefs[0]

    log.debug("theChainDef: %s" , theChainDef)

    return theChainDef



    

    
