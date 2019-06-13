# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

###########################################################################
# SliceDef file for Bphysics chains
###########################################################################

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Bphysics.generateChainConfigs' )
logging.getLogger().info("Importing %s",__name__)

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainDict
from TriggerMenuMT.HLTMenuConfig.Bphysics.BphysicsDef import BphysicsChainConfiguration as BphysicsChainConfiguration

def generateChainConfigs(chainDict):

    if not chainDict["topo"]:
         log.error( "No topo given -> not a bphysics chain...")
             
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs=[]

    for subChainDict in listOfChainDicts:
        
        my_chain = BphysicsChainConfiguration(subChainDict).assembleBphysChain() 

        listOfChainDefs += [my_chain]
        log.debug('length of chaindefs %s', len(listOfChainDefs) )
        

    if len(listOfChainDefs)>1:
        log.warning("Implement case for multi-bphys object chain!!")
        theChainDef = listOfChainDefs[0] #needs to be implemented properly
    else:
        theChainDef = listOfChainDefs[0]

    log.debug("theChainDef.name: %s" , theChainDef.name)
    log.debug("theChainDef.seed: %s" , theChainDef.seed)
    log.debug("theChainDef.ChainSteps: %s" , theChainDef.steps)

    return theChainDef

