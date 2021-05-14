# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function

from AthenaConfiguration.ComponentFactory import CompFactory

from TrigHLTJetHypo.hypoConfigBuilder import hypotool_from_chaindict

from AthenaCommon.Logging import logging
logger = logging.getLogger(__name__)


debug = False  # SET TO FALSE  WHEN COMMITTING
if debug:
    from AthenaCommon.Constants import DEBUG
    logger.setLevel(DEBUG)


def  trigJetHypoToolFromDict(chain_dict):
    
    hypo_tool =  hypotool_from_chaindict(chain_dict)
    hypo_tool.visit_debug = debug

    return hypo_tool

    
def  trigJetTLAHypoToolFromDict(chain_dict):
    return  CompFactory.TrigJetTLAHypoTool(chain_dict['chainName'])


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
