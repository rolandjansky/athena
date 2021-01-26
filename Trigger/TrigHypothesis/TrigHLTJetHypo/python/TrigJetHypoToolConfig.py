# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function

from AthenaConfiguration.ComponentFactory import CompFactory

from TrigHLTJetHypo.treeVisitors import TreeParameterExpander
from TrigHLTJetHypo.ConditionsToolSetterFastReduction import (
    ConditionsToolSetterFastReduction,
)

from TrigHLTJetHypo.FastReductionAlgToolFactory import (
    FastReductionAlgToolFactory,
    )

from  TrigHLTJetHypo.chainDict2jetLabel import chainDict2jetLabel

from  TrigHLTJetHypo.ChainLabelParser import ChainLabelParser
from TrigHLTJetHypo.node import Node
from TrigHLTJetHypo.NodeSplitterVisitor import NodeSplitterVisitor


from AthenaCommon.Logging import logging
log = logging.getLogger( 'TrigJetHypoToolConfig' )

algToolFactory = FastReductionAlgToolFactory()

def  tree2tools(rootless_tree, toolSetter):

    
    # add a root node so that split simple nodes cann connect.
    tree = Node('root')
    tree.children = [rootless_tree]
    tree.node_id = 0
    tree.parent_id = 0
    rootless_tree.tree_top = False
    tree.tree_top = True

    #expand strings of cuts to a cut dictionary
    visitor = TreeParameterExpander()
    tree.accept(visitor)
    log.debug(visitor.report())

    visitor = NodeSplitterVisitor()
    tree.accept(visitor)

    # tell the child nodes who their parent is.
    tree.set_ids(node_id=0, parent_id=0)

    # create - possibly nested - tools

    # chain name in run 2 dicts were missing the 'HLT_' prefix
    # but it seems it is necessary to run the hypos in AthenaMT ?...?

    toolSetter.mod(tree)

    return tree  # used by debug tools


def  nodeForestFromChainLabel(
        chain_label, # simple([(260et,320eta490, leg000)])
        chain_name,): # HLT_j260_320eta490_L1J75_31ETA49
    
    parser = ChainLabelParser(chain_label, debug=False)

    return parser.parse()
   
def trigJetHypoToolHelperFromDict_(
        chain_label,  # simple([(260et,320eta490, leg000)])
        chain_name,):  # HLT_j260_320eta490_L1J75_31ETA49

    
    node_forest =  nodeForestFromChainLabel(chain_label,
                                            chain_name)

    helper_tool = algToolFactory('helper')

    for tree in node_forest:
        toolSetter = ConditionsToolSetterFastReduction(algToolFactory)
        tree2tools(tree, toolSetter)
        helper_tool.HypoConfigurers.append(toolSetter.config_tool)   

        log.debug(toolSetter.report())
    
    return helper_tool

def  trigJetHypoToolHelperFromDict(chain_dict):
    """Produce  a jet trigger hypo tool helper from a chainDict
    Helper tools do the hypio work. They are used, for example
    by TrigJetHypoToolMT to  devide whether an event passes.
    A Helper Tool returned by this function may be the root of a Helper
    Tool tree structure."""

    
    log.debug('trigJetHypoToolFromDict chainDict %s', str(chain_dict))

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


def  trigJetHypoToolFromDict_(chain_dict, hypo_tool, debug=False):
    """Produce  a jet trigger hypo tool from a chainDict"""

    log.debug('trigJetHypoToolFromDict_ tool type ',
              hypo_tool.__class__.__name__,
              ' chainDict ',
              str(chain_dict))

    # obtain  a Helper Tool (possibly a tree of tools) to
    # make the hypo decision.
    hypo_tool.helper_tool = trigJetHypoToolHelperFromDict(chain_dict)

    # controls whether debug visitor is sent to helper tool
    hypo_tool.visit_debug = debug

    hypo_tool.chain_name = chain_dict['chainName']

    log.debug('%s', hypo_tool)

    return hypo_tool


def  trigJetTLAHypoToolFromDict(chain_dict):
    tool = CompFactory.TrigJetTLAHypoToolMT(name=chain_dict['chainName'])
    return trigJetHypoToolFromDict_(chain_dict, tool)


def  trigJetHypoToolFromDict(chain_dict):
    debug = False  # SET TO FALSE  WHEN COMMITTING
    tool = CompFactory.TrigJetHypoToolMT(name=chain_dict['chainName'])
    return trigJetHypoToolFromDict_(chain_dict, tool, debug)


import unittest
class TestStringMethods(unittest.TestCase):
    def testValidConfigs(self):
        from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import (
            dictFromChainName,)

        chain_names = (
            'HLT_j0_fbdjnosharedSEP10etSEP20etSEP34massSEP50fbet_L1J20',)
        
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



if __name__ == '__main__':
    unittest.main()

    # other local tests have been moved to testChainDictMaker.py
