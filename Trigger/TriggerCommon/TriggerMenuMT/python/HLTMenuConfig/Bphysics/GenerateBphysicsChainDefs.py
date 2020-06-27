# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

###########################################################################
# SliceDef file for Bphysics chains
###########################################################################

from AthenaCommon.Logging import logging
log = logging.getLogger('TriggerMenuMT.HLTMenuConfig.Bphysics.generateChainConfigs')
logging.getLogger().info('Importing %s', __name__)

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainDict
from TriggerMenuMT.HLTMenuConfig.Menu.ChainMerging import mergeChainDefs
from TriggerMenuMT.HLTMenuConfig.Bphysics.BphysicsDef import BphysicsChainConfiguration as BphysicsChainConfiguration

def generateChainConfigs(chainDict):

    if not chainDict['topo']:
         log.error('No topo given -> not a bphysics chain...')

    listOfChainDicts = splitChainDict(chainDict)

    listOfChainDefs=[]
    for subChainDict in listOfChainDicts:
        subChain = BphysicsChainConfiguration(subChainDict).assembleBphysChain()
        listOfChainDefs += [subChain]

    log.debug('length of chaindefs %s', len(listOfChainDefs))

    if len(listOfChainDefs) > 1:
        chainDef = mergeChainDefs(listOfChainDefs, chainDict)
    else:
        chainDef = listOfChainDefs[0]

    log.debug('ChainDef %s', chainDef)
    return chainDef
