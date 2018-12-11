# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Egamma.ElectronDef import generateChain as generateElectronChain
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain
from TriggerMenuMT.HLTMenuConfig.Menu.MenuUtils import splitChainDict
#from TriggerMenuMT.HLTMenuConfig.Egamma.ElectronDef import Chain_electron as Chain_electron


from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Egamma.generateChainConfigs' )
log.info("Importing %s",__name__)



def generateChainConfigs( chainDict ):
    import pprint
    pprint.pprint( chainDict )

    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    for subChainDict in listOfChainDicts:
        log.debug("IN ELECTRON GENERATION CODE")
        
        Electron = generateElectronChain( None, subChainDict )
        #Electron = Chain_electron(subChainDict).assembleChain() 


        listOfChainDefs += [Electron]
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



    

    
