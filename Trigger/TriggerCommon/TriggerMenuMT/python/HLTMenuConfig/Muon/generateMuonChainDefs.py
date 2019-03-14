# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

###########################################################################
# SliceDef file for Muon chains
###########################################################################

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Muon.generateChainConfigs' )
logging.getLogger().info("Importing %s",__name__)

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainDict
from TriggerMenuMT.HLTMenuConfig.Muon.MuonDef import MuonChainConfiguration as MuonChainConfiguration



def generateChainDefs(chainDict):
    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs=[]

    for subChainDict in listOfChainDicts:
        
        Muon = MuonChainConfiguration(subChainDict).assembleChain() 

        listOfChainDefs += [Muon]
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


