# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigJetHypoToolMT

from  TrigHLTJetHypo.ToolSetter import ToolSetter
from  TrigHLTJetHypo.treeVisitors import TreeParameterExpander

from  TrigHLTJetHypo.chainDict2jetLabel import (make_simple_label,
                                                make_vbenf_label,
                                                make_multijetInvmLegacy_label,
)

from  TrigHLTJetHypo.ChainLabelParser import ChainLabelParser

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TrigJetHypoToolConfig' )

def  trigJetHypoToolFromDict(chain_dict):
    """Produce  a jet trigger hypo tool from a chainDict"""

    chain_label = ''

    hypo_scenario_0 = chain_dict['chainParts'][0]['hypoScenario']
    if 'vbenf' in hypo_scenario_0:
        assert len(chain_dict['chainParts']) == 1
        chain_label = make_vbenf_label(hypo_scenario_0)
    if hypo_scenario_0.startswith('multijetInvmLegacy'):
        assert len(chain_dict['chainParts']) == 1
        chain_label = make_multijetInvmLegacy_label(hypo_scenario_0)
    else:
        chain_label = make_simple_label(chain_dict)
    parser = ChainLabelParser(chain_label)
    tree = parser.parse()
    
    #expand strings of cuts to a cut dictionary
    visitor = TreeParameterExpander()
    tree.accept(visitor)
    log.info(visitor.report())

    # tell the child nodes who their parent is.
    tree.set_ids(node_id=0, parent_id=0)

    # create - possibly nested - tools

    # chain name in run 2 dicts were missing the 'HLT_' prefix
    # but it seems it is necessary to run the hypos in AthenaMT ?...?
    
    chain_name = chain_dict['chainName']
    if not chain_name.startswith('HLT_'):
        chain_name = 'HLT_' + chain_name

    log.info('trigJetHypoToolFromDict chain_name %s', chain_name)

    # debug flag to be relayed to C++ objects
    debug = False
    visitor = ToolSetter(chain_name)
    tree.accept(visitor)
    log.info(visitor.report())

    log.info('Dumping jet config for %s', chain_name)
    tool = TrigJetHypoToolMT(name=chain_name)
    tool.helper_tool = tree.tool

    # controls whether debuf visitor is sent to helper tool
    tool.visit_debug = debug
    
    log.info('%s', tool)
    return tool


import unittest
class TestStringMethods(unittest.TestCase):
    def testValidConfigs(self):
        from TriggerMenuMT.HLTMenuConfig.Menu import DictFromChainName

        chainNameDecoder = DictFromChainName.DictFromChainName()
        # chain_names = ('HLT_j85', 'HLT_j35_0eta320')
        chain_names = ('HLT_j0hs_vbenf',)
        wid = max(len(c) for c in chain_names)
        for chain_name in chain_names:
            chain_dict = chainNameDecoder.getChainDict(chain_name)
            tool = trigJetHypoToolFromDict(chain_dict)
            self.assertIsNotNone(tool) 
            log.info('%s %s', chain_name.rjust(wid), tool)


class TestDebugFlagIsFalse(unittest.TestCase):
    def testValidConfigs(self):
        from TriggerMenuMT.HLTMenuConfig.Menu import DictFromChainName

        chainNameDecoder = DictFromChainName.DictFromChainName()
        chain_name = 'HLT_j85'
        chain_dict = chainNameDecoder.getChainDict(chain_name)
        tool = trigJetHypoToolFromDict(chain_dict)
        self.assertIsNotNone(tool) 
        self.assertFalse(tool.visit_debug) 


def _test0():
    from TriggerMenuMT.HLTMenuConfig.Menu import DictFromChainName

    chainNameDecoder = DictFromChainName.DictFromChainName()
    chain_name = 'HLT_j0'
    chain_dict = chainNameDecoder.getChainDict(chain_name)
    chain_dict['chainParts'][0]['hypoScenario'] = 'multijetInvmLegacySEPmult4SEP35etSEP0eta490SEP1000mass'
    for d in chain_dict['chainParts']:
        print d

    tool = trigJetHypoToolFromDict(chain_dict)
    print 'tool:\n', tool
    self.assertIsNotNone(tool) 
    self.assertFalse(tool.visit_debug) 
if __name__ == '__main__':
    # unittest.main()
    _test0()
