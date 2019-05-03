# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainDict
from TriggerMenuMT.HLTMenuConfig.Egamma.PhotonDef import PhotonChainConfiguration as PhotonChainConfiguration


from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Egamma.generateChainConfigs' )
log.info("Importing %s",__name__)



def generateChainConfigs( chainDict ):
    import pprint
    pprint.pprint( chainDict )

    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    for subChainDict in listOfChainDicts:
        
        Photon = PhotonChainConfiguration(subChainDict).assembleChain() 

        listOfChainDefs += [Photon]
        log.debug('length of chaindefs %s', len(listOfChainDefs) )
        

    if len(listOfChainDefs)>1:
        log.warning("Implement case for multi-electron chain!!") 
        theChainDef = listOfChainDefs[0] #needs to be implemented properly
    else:
        theChainDef = listOfChainDefs[0]

    log.debug("theChainDef.name: %s" , theChainDef.name)
    log.debug("theChainDef.seed: %s" , theChainDef.seed)
    log.debug("theChainDef.ChainSteps: %s" , theChainDef.steps)

    return theChainDef



    

    
