# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
__log = logging.getLogger( 'HLTMenuJSON.py' )


def __generateJSON( chainDicts, sequences, menuName ):
    """ Generates JSON given the ChainProps and sequences
    """
    menuDict = { "name":menuName ,  "chains":[] }
    counter = 0    

    for chain in chainDicts:
        
        streamDicts = [ { "obeyLB": "yes", "prescale":"1", "name":sn, "type":"physics" } 
                        for sn in chain["stream"]]
        
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
            
    __log.info( "Menu name:" + menuName )
    with open( menuName+'.json', 'w' ) as fp:
        import json
        json.dump( menuDict, fp, indent=4, sort_keys=True )
        

def generateJSON( allStepsSequence ):    
    __log.info("Generating HLT JSON config in the rec-ex-common job")
    from TriggerJobOpts.TriggerFlags              import TriggerFlags
    from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT
    triggerConfigHLT = TriggerConfigHLT.currentTriggerConfig()
    return __generateJSON( triggerConfigHLT.allChainDicts, None, TriggerFlags.outputHLTconfigFile().replace( '.xml', '' ) )
    
def generateJSON_newJO( allStepsSequence ):
    __log.info("Generating HLT JSON config in the new JO")
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from TriggerMenuMT.HLTMenuConfig.Menu import DictFromChainName


    decoder = DictFromChainName.DictFromChainName()
    chainDicts = []
    #TODO, once we move to New JO, it is likely there will be a repository with all chains
    #we shoudl switch to use it then
    for name, cfgFlag in list( ConfigFlags._flagdict.iteritems() ):
        if 'Trigger.menu.' in name:            
            for chain in ConfigFlags._get( name ):
                chainDicts.append( decoder.getChainDict( chain ) )
                                    
    import os
    return __generateJSON( chainDicts, None, "HLTconfig_"+os.getenv( "AtlasVersion" ) + "." + ConfigFlags.Trigger.triggerMenuSetup )
