# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.CalibCosmicMon.EnhancedBiasChainConfiguration import EnhancedBiasChainConfiguration as EnhancedBiasChainConfiguration
from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainDict
from TriggerMenuMT.HLTMenuConfig.Menu.ChainMerging import mergeChainDefs

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.CalibCosmicMon.generateChainConfigs' )
log.info("Importing %s",__name__)


def generateChainConfigs( chainDict ):
    log.debug("Generate EnhancedBias...")
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    for subChainDict in listOfChainDicts:
        EnhancedBiasChain = EnhancedBiasChainConfiguration(subChainDict).assembleChain()
        listOfChainDefs += [EnhancedBiasChain]

    if len(listOfChainDefs)>1:
        theChainDef = mergeChainDefs(listOfChainDefs, chainDict)
    else:
        theChainDef = listOfChainDefs[0]

    log.debug("theChainDef %s" , theChainDef)

    return theChainDef
