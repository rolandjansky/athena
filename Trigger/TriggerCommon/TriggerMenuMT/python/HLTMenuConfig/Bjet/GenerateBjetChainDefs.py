# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainDict
from TriggerMenuMT.HLTMenuConfig.Bjet.BjetDef import BjetChainConfiguration as BjetChainConfiguration
from TriggerMenuMT.HLTMenuConfig.Jet.JetChainConfiguration import JetChainConfiguration

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Bjet.GenerateBjetChainConfigs' )
log.info("Importing %s",__name__)



def generateChainConfigs( chainDict ):
    import pprint
    pprint.pprint( chainDict )

    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    for subChainDict in listOfChainDicts:

        subChainDict['chainParts']['signature'] = 'Jet'

        jet = JetChainConfiguration(subChainDict).assembleChain()
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



    

    
