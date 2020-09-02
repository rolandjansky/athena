# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainDict
from TriggerMenuMT.HLTMenuConfig.Bjet.BjetDef import BjetChainConfiguration as BjetChainConfiguration
from TriggerMenuMT.HLTMenuConfig.Jet.JetChainConfiguration import JetChainConfiguration

import pprint
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Bjet.GenerateBjetChainConfigs' )
log.info("Importing %s",__name__)



def generateChainConfigs( chainDict ):
    log.debug('dictionary is: %s\n', pprint.pformat(chainDict))

    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    jet = JetChainConfiguration(chainDict).assembleChain()

    for subChainDict in listOfChainDicts:

        Bjet = BjetChainConfiguration(subChainDict).assembleChain() 
        jet.steps = jet.steps + Bjet.steps

        listOfChainDefs += [jet]
        log.debug('length of chaindefs %s', len(listOfChainDefs) )
        

    if len(listOfChainDefs)>1:
        log.warning("Implement case for multi-bjet chain!!") 
        theChainDef = listOfChainDefs[0] #needs to be implemented properly
    else:
        theChainDef = listOfChainDefs[0]

    log.debug("theChainDef: %s" , theChainDef)

    return theChainDef



    

    
