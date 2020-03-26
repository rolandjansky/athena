# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Jet.JetChainConfiguration import JetChainConfiguration


from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Jet.generateChainConfigs' )
log.info("Importing %s",__name__)



def generateChainConfigs( chainDict ):
    import pprint
    pprint.pprint( chainDict )

    # Jet chain is assembled always from the full dictionary (multiple legs are handled internally by the jet reco / hypo)
    theChainDef = JetChainConfiguration(chainDict)

    jetChain = theChainDef.assembleChain()


    return jetChain



    

    
