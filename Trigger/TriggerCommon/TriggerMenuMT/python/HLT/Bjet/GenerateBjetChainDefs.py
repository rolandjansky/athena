# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLT.Config.Utility.ChainDictTools import splitChainDict
from .BjetChainConfiguration import BjetChainConfiguration
from ..Jet.JetChainConfiguration import JetChainConfiguration
from TriggerMenuMT.HLT.Config.Utility.ChainMerging import mergeChainDefs

import pprint
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)
log.info("Importing %s",__name__)

def generateChainConfigs( chainDict ):

    log.debug('bjet full dictionary is: %s\n', pprint.pformat(chainDict))

    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    for subChainDict in listOfChainDicts:

        jet_cfg = JetChainConfiguration(chainDict)
        jet_name = jet_cfg.jetName
        jet = jet_cfg.assembleChain()

        # don't setup btagging for legs that are jet-only
        # happens for bjet + normal jet chains
        if subChainDict['chainParts'][0]['signature'] != 'Bjet':
            listOfChainDefs += [jet]
        else:
            log.debug('input jet collection name is: %s\n', jet_name)
            Bjet = BjetChainConfiguration(subChainDict, jet_name).assembleChain() 
            jet.append_bjet_steps(Bjet.steps)
            listOfChainDefs += [ jet ] 

    if len(listOfChainDefs)>1:
        theBjetChainDef = mergeChainDefs(listOfChainDefs, chainDict) 
    else:
        theBjetChainDef = listOfChainDefs[0]

    return theBjetChainDef
