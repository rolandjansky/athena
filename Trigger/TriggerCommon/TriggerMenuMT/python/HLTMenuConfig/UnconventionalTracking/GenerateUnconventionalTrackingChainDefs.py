# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.UnconventionalTracking.UnconventionalTrackingChainConfiguration import UnconventionalTrackingChainConfiguration

import pprint
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.UnconventionalTracking.generateChainConfigs' )
log.info("Importing %s",__name__)



def generateChainConfigs( chainDict ):
    log.debug('dictionary is: %s\n', pprint.pformat(chainDict))

    # Unconventional tracking chain is assembled always from the full dictionary (multiple legs are handled internally by the jet reco / hypo)
    theChainDef = UnconventionalTrackingChainConfiguration(chainDict)

    unconventionalTrackingChain = theChainDef.assembleChain()


    return unconventionalTrackingChain
