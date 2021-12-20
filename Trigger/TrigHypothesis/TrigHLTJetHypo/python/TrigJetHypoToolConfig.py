# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import NoHypoToolCreated
from TrigHLTJetHypo.hypoConfigBuilder import hypotool_from_chaindict

from AthenaCommon.Logging import logging
logger = logging.getLogger(__name__)


debug = False  # SET TO FALSE  WHEN COMMITTING
if debug:
    from AthenaCommon.Constants import DEBUG
    logger.setLevel(DEBUG)


def  trigJetHypoToolFromDict(chain_dict):
    
    from DecisionHandling.TrigCompositeUtils import isLegId, getLegIndexInt
    chain_name = chain_dict['chainName']
    jet_signature_identifiers = ['Jet', 'Bjet']

    if isLegId(chain_name):
        # For multi-leg chains which include jet legs we have a -- SPECIAL BEHAVIOUR --
        # We instantiate a HypoTool only for the *first* jet leg, whichever leg that happens to be in the chain
        # This single HypoTool gets configured to perform the selection for _all_ of the jet legs, and to report
        # the per-jet passing status for all of these legs.
        #
        # Here we determine if this is the 2nd+ jet leg of a multi-leg chain which has jet legs, and return no tool if it is

        # Can we fetch this from elsewhere?

        leg_id = getLegIndexInt(chain_name)
        # CHECK: If we have called trigJetHypoToolFromDict, then the chain_dict['signatures'] list must contain at minimum one entry from the jet_signature_identifiers list. 
        if not any(signature in chain_dict['signatures'] for signature in jet_signature_identifiers):
            raise Exception("[trigJetHypoToolFromDict] No {} in {} for chain {}. Please update this list of jet signatures.".format(tuple(jet_signature_identifiers),tuple(chain_dict['signatures']),chain_name))

        # CHECK: All Jet and Bjet legs (i.e. signatures from jet_signature_identifiers) must be contiguous
        # (this check is probable best put somewhere else?)
        status = 0
        for entry in chain_dict['signatures']:
            if status == 0 and entry in jet_signature_identifiers:
                status = 1
            elif status == 1 and entry not in jet_signature_identifiers:
                status = 2
            elif status == 2 and entry in jet_signature_identifiers:
                raise Exception("[trigJetHypoToolFromDict] All {} legs should be contiguous in the signatures list, modify the ordering of the chain {}. Signatures:{}.".format(tuple(jet_signature_identifiers),chain_name, tuple(chain_dict['signatures'])))
        
        # CHECK: The leg_id must correspond to a Signature from jet_signature_identifiers. At the time of implementation, this is not guaranteed and can be affected by alignment.
        # If this check fails for any chain, then we need to look again at how the legXXX ordering maps to the chain_dict['signatures'] ordering.
        if not any(signature in chain_dict['signatures'][leg_id] for signature in jet_signature_identifiers):
            raise Exception("[trigJetHypoToolFromDict] For this code to work for chain {}, the signature at index {} must be one of {}. But the signature list is: {}".format(chain_name,leg_id,tuple(jet_signature_identifiers),tuple(chain_dict['signatures'])))

        # Locate the first index within chain_dict['signatures'] which contains an signature listed in jet_signature_identifiers
        first_leg_index = 999
        for signature in jet_signature_identifiers:
            if signature in chain_dict['signatures']:
                first_leg_index = min(first_leg_index, chain_dict['signatures'].index(signature))

        if leg_id > first_leg_index:
            logger.debug("Not returning a HypoTool for %s as this is not the first leg "
                         "with any of %s (leg signatures are %s)",
                         chain_name, tuple(jet_signature_identifiers), tuple(chain_dict['signatures']))
            raise NoHypoToolCreated("No HypoTool created for %s" % chain_name)

    logger.debug("Returning a HypoTool for %s as this is the first leg with any of %s (leg signatures are %s)",
                 chain_name, tuple(jet_signature_identifiers), tuple(chain_dict['signatures']))

    hypo_tool =  hypotool_from_chaindict(chain_dict)
    hypo_tool.visit_debug = debug

    return hypo_tool

    
def  trigJetTLAHypoToolFromDict(chain_dict):
    return  CompFactory.TrigJetTLAHypoTool(chain_dict['chainName'])

def  trigJetEJsHypoToolFromDict(chain_dict):
    chain_name = chain_dict['chainName']

    if 'emerging' in chain_name:
        trackless = int(0)
        ptf = float(chain_name.split('PTF')[1].split('dR')[0].replace('p', '.'))
        dr  = float(chain_name.split('dR')[1].split('_')[0].replace('p', '.'))
    elif 'trackless' in chain_name:
        trackless = int(1)
        ptf = 0.0
        dr = float(chain_name.split('dR')[1].split('_')[0].replace('p', '.'))
    else:
        raise Exception("misconfiguration of emerging jet chain")

    hypo = CompFactory.TrigJetEJsHypoTool(chain_name)
    hypo.PTF       = ptf
    hypo.dR        = dr
    hypo.Trackless = trackless

    return  hypo


import unittest
class TestStringMethods(unittest.TestCase):
    def testValidConfigs(self):
        from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import (
            dictFromChainName,)

        chain_names = (
            'HLT_j0_FBDJNOSHARED10etXX20etXX34massXX50fbet_L1J20',)
        
        wid = max(len(c) for c in chain_names)
        for chain_name in chain_names:
            chain_dict = dictFromChainName(chain_name)
            tool = trigJetHypoToolFromDict(chain_dict)
            self.assertIsNotNone(tool)
            logger.debug(chain_name.rjust(wid), str(tool))


class TestDebugFlagIsFalse(unittest.TestCase):
    def testValidConfigs(self):
        self.assertFalse(debug)



if __name__ == '__main__':
    unittest.main()

    # other local tests have been moved to testChainDictMaker.py
