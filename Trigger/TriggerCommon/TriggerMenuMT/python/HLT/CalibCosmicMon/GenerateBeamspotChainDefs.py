# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLT.Config.Utility.ChainDictTools import splitChainDict
from TriggerMenuMT.HLT.Config.Utility.ChainMerging import mergeChainDefs
from TriggerMenuMT.HLT.CalibCosmicMon.BeamspotChainConfiguration import BeamspotChainConfiguration
from TriggerMenuMT.HLT.Jet.JetChainConfiguration import JetChainConfiguration

import pprint
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)
log.info("Importing %s",__name__)



def generateChainConfigs( chainDict ):
    log.debug('dictionary is: %s\n', pprint.pformat(chainDict))

    listOfChainDicts = splitChainDict(chainDict)
    log.debug("Will generate Config for chain: %s", chainDict['chainName'])

    listOfChainDefs = []
        
    #streamers will never have more than one chainPart but this is still
    #needed to move to the correct format [{}]->{}
    for subChainDict in listOfChainDicts:
        
        if subChainDict['chainParts'][0]['beamspotChain'] != '':

            log.debug("Jet tracking based beamspot chain")            
            log.debug("chainDict %s", chainDict)

            jetConfig = JetChainConfiguration(chainDict)
            jetName = jetConfig.jetName
            log.debug("Jet name %s", jetConfig.jetName)
            jet =  jetConfig.assembleChain()
            log.debug('Input jet collection name is %s \n', jetName)

            Beamspot = BeamspotChainConfiguration(subChainDict, jetName).assembleChain()
            jet.append_step_to_jet(Beamspot.steps)

            listOfChainDefs += [ jet ]
            log.debug('length of chaindefs %s', len(listOfChainDefs) )

        else:
            log.debug("Traditional beamspot chain")
            Beamspot = BeamspotChainConfiguration(subChainDict).assembleChain()             
            
            listOfChainDefs += [Beamspot]
            log.debug('length of chaindefs %s', len(listOfChainDefs) )


    if len(listOfChainDefs)>1:
        log.warning("This is a chain with more than one chainPart, is this really intended?")
        theChainDef = mergeChainDefs(listOfChainDefs, chainDict)
    else:
        theChainDef = listOfChainDefs[0]

    log.debug("theChainDef %s" , theChainDef)

    return theChainDef
