# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from ..Menu.ChainDictTools import splitChainDict
from .BjetChainConfiguration import BjetChainConfiguration
from ..Jet.JetChainConfiguration import JetChainConfiguration
from ..Jet import JetRecoSequences
from ..Jet.JetRecoSequences import JetRecoConfiguration
from ..Menu.ChainMerging import mergeChainDefs

import pprint
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)
log.info("Importing %s",__name__)

def generateChainConfigs( chainDict ):

    log.info('bjet full dictionary is: %s\n', pprint.pformat(chainDict))

    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs = []

    for subChainDict in listOfChainDicts:

        jet = JetChainConfiguration(chainDict).assembleChain()

        # don't setup btagging for legs that are jet-only
        # happens for bjet + normal jet chains
        if subChainDict['chainParts'][0]['signature'] != 'Bjet':
            listOfChainDefs += [jet]
        else:
            log.info('bjet sub dictionary is: %s\n', pprint.pformat(subChainDict))
            jetRecoDict = JetRecoConfiguration.extractRecoDict(subChainDict["chainParts"])
            jetNamePrefix = JetRecoSequences.getJetNamePrefix()
            jetDef = JetRecoConfiguration.defineJets(jetRecoDict,pfoPrefix=jetNamePrefix+jetRecoDict["trkopt"],prefix=jetNamePrefix)
            jet_name = jetDef.fullname() 
            Bjet = BjetChainConfiguration(subChainDict, jet_name).assembleChain() 
            jet.steps = jet.steps + Bjet.steps
            listOfChainDefs += [ jet ] 

    if len(listOfChainDefs)>1:
        theBjetChainDef = mergeChainDefs(listOfChainDefs, chainDict) 
    else:
        theBjetChainDef = listOfChainDefs[0]

    return theBjetChainDef
