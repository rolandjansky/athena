# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import json
from AthenaCommon.Logging import logging
__log = logging.getLogger( 'HLTMenuJSON.py' )


def __generateJSON( chainDicts, sequences, menuName, fileName ):
    """ Generates JSON given the ChainProps and sequences
    """
    menuDict = {"name": menuName, "chains": []}
    counter = 0

    from TriggerMenuMT.HLTMenuConfig.Menu import StreamInfo
    for chain in chainDicts:
        streamDicts = []
        streamTags = StreamInfo.getStreamTags(chain["stream"])
        for streamTag in streamTags:
            streamDicts.append({"name": streamTag[0],
                                "type": streamTag[1],
                                "obeyLB": streamTag[2],
                                "forceFullEventBuilding": streamTag[3]})

        l1Thresholds  = []
        [ l1Thresholds.append(p['L1threshold']) for p in chain['chainParts'] ]

        chainDict = { "counter": counter,
                      "name": chain["chainName"],
                      "l1item": chain["L1item"],
                      "l1thresholds": l1Thresholds,
                      "groups": chain["groups"],
                      "streams": streamDicts }
        menuDict["chains"].append( chainDict )
        counter += 1

    __log.info( "Writing trigger menu to %s", fileName )
    with open( fileName, 'w' ) as fp:
        json.dump( menuDict, fp, indent=4, sort_keys=True )

def generateJSON( allStepsSequence ):
    __log.info("Generating HLT JSON config in the rec-ex-common job")
    from TriggerJobOpts.TriggerFlags              import TriggerFlags
    from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT
    triggerConfigHLT = TriggerConfigHLT.currentTriggerConfig()

    return __generateJSON( triggerConfigHLT.allChainDicts, None, TriggerFlags.triggerMenuSetup(), TriggerFlags.outputHLTmenuJsonFile() )
    

def generateJSON_newJO( allStepsSequence ):
    __log.info("Generating HLT JSON config in the new JO")
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from TriggerMenuMT.HLTMenuConfig.Menu import DictFromChainName

    decoder = DictFromChainName.DictFromChainName()
    chainDicts = []
    #TODO, once we move to New JO, it is likely there will be a repository with all chains
    #we should switch to use it then
    for name, cfgFlag in list( ConfigFlags._flagdict.iteritems() ):
        if 'Trigger.menu.' in name:
            for chain in ConfigFlags._get( name ):
                chainDicts.append( decoder.getChainDict( chain ) )
                      
    return __generateJSON( chainDicts, None, ConfigFlags.Trigger.triggerMenuSetup, ConfigFlags.Trigger.HLTMenuJsonFile )
