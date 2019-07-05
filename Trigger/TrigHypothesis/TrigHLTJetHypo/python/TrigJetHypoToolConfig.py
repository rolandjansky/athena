# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigJetHypoToolMT

from  TrigHLTJetHypo.ToolSetter import ToolSetter
from  TrigHLTJetHypo.treeVisitors import TreeParameterExpander
from  TrigHLTJetHypo.chainDict2jetLabel import (
    make_simple_label,
    make_simple_partition_label,
    make_vbenf_label)

# from TrigHLTJetHypo.chainDict2jetLabel import make_simple_comb_label as make_simple_label # TIMING studies

from  TrigHLTJetHypo.ChainLabelParser import ChainLabelParser

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TrigJetHypoToolConfig' )

def  trigJetHypoToolHelperFromDict(chain_dict):
    """Produce  a jet trigger hypo tool helper from a chainDict
    Helper tools do the hypio work. They are used, for example
    by TrigJetHypoToolMT to  devide whether an event passes.
    A Helper Tool returned by this function may be the root of a Helper
    Tool tree structure."""

    chain_label = ''

    hypo_scenario_0 = chain_dict['chainParts'][0]['hypoScenario']
    if 'vbenf' in hypo_scenario_0:
        assert len(chain_dict['chainParts']) == 1
        chain_label = make_vbenf_label(hypo_scenario_0)
    elif 'simple_partition' == hypo_scenario_0:
        chain_label = make_simple_partition_label(chain_dict)
    elif 'simple' == hypo_scenario_0:
        chain_label = make_simple_label(chain_dict)
    else:
        m = 'trigJetHypoToolFromDict chain_name %s unknown hypoScenario %s' % (
            chain_dict['chainName'], hypo_scenario_0)

        log.info(m)
        
                                                  
                                                  
    parser = ChainLabelParser(chain_label, debug=True)
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
    visitor = ToolSetter(chain_name)
    tree.accept(visitor)
    log.info(visitor.report())

    return tree.tool


def  trigJetHypoToolFromDict(chain_dict):
    """Produce  a jet trigger hypo tool from a chainDict"""

    chain_name = chain_dict['chainName']
    tool = TrigJetHypoToolMT(name=chain_name)

    # obtain  a Helper Tool (possibly a tree of tools) to
    # make the hypo decision.
    tool.helper_tool = trigJetHypoToolHelperFromDict(chain_dict)

    # controls whether debug visitor is sent to helper tool
    debug = False
    tool.visit_debug = debug
    
    log.info('%s', tool)
    return tool


import unittest
class TestStringMethods(unittest.TestCase):
    def testValidConfigs(self):
        from TriggerMenuMT.HLTMenuConfig.Menu import DictFromChainName

        chainNameDecoder = DictFromChainName.DictFromChainName()
        # chain_names = ('HLT_j85', 'HLT_j35_0eta320')
        chain_names = ('HLT_j0_vbenf',)
        wid = max(len(c) for c in chain_names)
        for chain_name in chain_names:
            chain_dict = chainNameDecoder.getChainDict(chain_name)
            print chain_dict
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


if __name__ == '__main__':
    unittest.main()
