# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import json
from collections import OrderedDict as odict
from TrigConfigSvc.TrigConfigSvcCfg import getHLTPrescalesSetFileName
from AthenaCommon.Logging import logging
__log = logging.getLogger( __name__ )

def __generateJSON( chainDicts, chainConfigs, menuName, fileName):
    """ Generates JSON given the ChainProps and sequences
    """
    # Prescale dictionary that is used to create the JSON content
    prescaleDict = odict([ ("filetype", "hltprescale"), ("name", menuName), ("prescales", odict()) ])

    # Sort chains by name not counter for easier post processing
    for chain in sorted(chainDicts, key=lambda d: d['chainName']):
        # Enabled flag is what determines disabling (here is based on prescale list from MenuPrescaleConfig)
        chainEnabled = True
        if (int(chain["prescale"]) <= 0):
            chainEnabled = False

        # Now have all information to write the chain to the prescale dictionary
        chainName = chain["chainName"]
        prescaleDict["prescales"][chainName] = odict([
            ("name", chainName),
            ("hash", chain["chainNameHash"]),
            ("prescale", chain["prescale"]),
            ("enabled", chainEnabled)
        ])
        # Add express stream information
        if "express" in chain["stream"]:
            prescaleDict["prescales"][chainName]['prescale_express'] = chain["prescale"]
            prescaleDict["prescales"][chainName]['enabled_express'] = chainEnabled

    # Prescale dictionary now completed, write to JSON
    __log.info( "Writing HLT Prescale JSON to %s", fileName )
    with open( fileName, 'w' ) as fp:
        json.dump( prescaleDict, fp, indent=4, sort_keys=False )


def generateJSON():
    __log.info("Generating HLT Prescale JSON in the rec-ex-common job")
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT

    return __generateJSON( TriggerConfigHLT.dictsList(),
                           TriggerConfigHLT.configsList(),
                           ConfigFlags.Trigger.triggerMenuSetup,
                           getHLTPrescalesSetFileName(ConfigFlags))

def generateJSON_newJO( chainDicts, chainConfigs ):
    __log.info("Generating HLT Prescale JSON in the new JO")
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    return __generateJSON( chainDicts,
                           chainConfigs,
                           ConfigFlags.Trigger.triggerMenuSetup,
                           getHLTPrescalesSetFileName(ConfigFlags) )