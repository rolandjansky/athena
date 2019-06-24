# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainDict
from TriggerMenuMT.HLTMenuConfig.Jet.JetChainConfiguration import JetChainConfiguration


from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Jet.generateChainConfigs' )
log.info("Importing %s",__name__)



def generateChainConfigs( chainDict ):
    import pprint
    pprint.pprint( chainDict )

    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    for subChainDict in listOfChainDicts:
        
        Jet = JetChainConfiguration(subChainDict).assembleChain() 

        listOfChainDefs += [Jet]
        log.debug('length of chaindefs %s', len(listOfChainDefs) )
        
    # We should never use multiple reco definitions, as this
    # cannot be handled by the hypos.
    # FIXME: Check that all jet reco configs are identical
    if len(listOfChainDefs)>1:
        # Add reco consistency checking between all 
        log.warning("Multiple jet chainParts detected; reco consistency checks not yet implemented")
        theChainDef = listOfChainDefs[0] #needs to be implemented properly
    else:
        theChainDef = listOfChainDefs[0]

    log.debug("theChainDef.name: %s" , theChainDef.name)
    log.debug("theChainDef.seed: %s" , theChainDef.seed)
    log.debug("theChainDef.ChainSteps: %s" , theChainDef.steps)

    return theChainDef



    

    
