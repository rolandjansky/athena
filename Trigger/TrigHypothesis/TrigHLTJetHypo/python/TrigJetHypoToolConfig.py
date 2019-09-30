# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function

from TrigHLTJetHypo.TrigHLTJetHypoConf import (
    TrigJetHypoToolMT,)

from  TrigHLTJetHypo.treeVisitors import TreeParameterExpander
from  TrigHLTJetHypo.ConditionsToolSetterTree import ConditionsToolSetterTree

from  TrigHLTJetHypo.chainDict2jetLabel import chainDict2jetLabel

# from TrigHLTJetHypo.chainDict2jetLabel import make_simple_comb_label as make_simple_label # TIMING studies

from  TrigHLTJetHypo.ChainLabelParser import ChainLabelParser

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TrigJetHypoToolConfig' )

def  trigJetHypoToolHelperFromDict_(chain_label,
                                    chain_name,
                                    toolSetter=None):

    parser = ChainLabelParser(chain_label, debug=False)

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
    
    if not chain_name.startswith('HLT_'):
        chain_name = 'HLT_' + chain_name
        
    log.info('trigJetHypoToolFromDict chain_name %s', chain_name)

    # debug flag to be relayed to C++ objects
    # visitor = ToolSetter(chain_name)
    tool = None
    if toolSetter is None:
        toolSetter = ConditionsToolSetterTree(chain_name)
        tree.accept(modifier=toolSetter)
        tool = tree.tool
    else:
        if toolSetter.__class__.__name__ == 'FlowNetworkSetter':
            toolSetter.mod(tree)
            tool = toolSetter.tool
        else:
            toolSetter = ConditionsToolSetterTree(chain_name)
            print ('using tool setter', toolSetter.__class__.__name__)
            tree.accept(modifier=toolSetter)

    # tool = tree.children[0].tool
    tool = tree.tool
    log.info(visitor.report())

    return tool


def  trigJetHypoToolHelperFromDict(chain_dict):
    """Produce  a jet trigger hypo tool helper from a chainDict
    Helper tools do the hypio work. They are used, for example
    by TrigJetHypoToolMT to  devide whether an event passes.
    A Helper Tool returned by this function may be the root of a Helper
    Tool tree structure."""

    try:
        chain_label = chainDict2jetLabel(chain_dict)
    except Exception as e:
        m = str(e)
        m += ' - TrigJetHypoToolConfig: Error obtaining jet label for %s' % (
            chain_dict['chainName'],)
        m += '  jet hypo scenario: %s' % (
            chain_dict['chainParts'][0]['hypoScenario'],)
        
        log.error(m)
        
        raise e
    
    chain_name = chain_dict['chainName']
    return trigJetHypoToolHelperFromDict_(chain_label,
                                          chain_name)
 

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
        from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import (
            dictFromChainName,)
        chain_names = ('HLT_j0_vbenf_L1J20',)
        wid = max(len(c) for c in chain_names)
        for chain_name in chain_names:
            chain_dict = dictFromChainName(chain_name)
            tool = trigJetHypoToolFromDict(chain_dict)
            self.assertIsNotNone(tool) 
            log.info('%s %s', chain_name.rjust(wid), tool)


class TestDebugFlagIsFalse(unittest.TestCase):
    def testValidConfigs(self):
        from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import (
            dictFromChainName,)

        chain_name = 'HLT_j85_L1J20'
        chain_dict = dictFromChainName(chain_name)
        tool = trigJetHypoToolFromDict(chain_dict)
        self.assertIsNotNone(tool) 
        self.assertFalse(tool.visit_debug) 

        
def _tests():

    from TriggerMenuMT.HLTMenuConfig.Menu import DictFromChainName

    chainNameDecoder = DictFromChainName.DictFromChainName()
        
    chain_names = (
        'j80_0eta240_2j60_320eta490_L1J20',
        'j80_0eta240_2j60_320eta490_j0_dijetSEP80j1etSEP0j1eta240SEP80j2etSEP0j2eta240SEP700djmass_L1J20',
    )
    for cn in chain_names:
        chain_dict = chainNameDecoder.getChainDict(cn)
        print(chain_dict)
        tool = trigJetHypoToolFromDict(chain_dict)
        print('tool:\n', tool)
        

if __name__ == '__main__':
    unittest.main()
    
    # run _tests outide untit tests so as to see stdout
    # _tests()
