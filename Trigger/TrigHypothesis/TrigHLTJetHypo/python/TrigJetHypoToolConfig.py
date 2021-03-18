# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function

from AthenaConfiguration.ComponentFactory import CompFactory

from TrigHLTJetHypo.treeVisitors import (TreeParameterExpander,
                                         FilterConditionsMover,
                                         TreeChecker,
                                         IdenticalNodeCompressor)

from TrigHLTJetHypo.ConditionsToolSetterFastReduction import (
    ConditionsToolSetterFastReduction,
)

from TrigHLTJetHypo.FastReductionAlgToolFactory import (
    FastReductionAlgToolFactory,
    )

from  TrigHLTJetHypo.chainDict2jetLabel import chainDict2jetLabel
from  TrigHLTJetHypo.prefilterLabelFromChainDict import (
    prefilterLabelFromChainDict,)

from  TrigHLTJetHypo.ChainLabelParser import ChainLabelParser


from AthenaCommon.Logging import logging
logger = logging.getLogger(__name__)

algToolFactory = FastReductionAlgToolFactory()

debug = False  # SET TO FALSE  WHEN COMMITTING
if debug:
    from AthenaCommon.Constants import DEBUG
    logger.setLevel(DEBUG)


def  tree2tools(tree, toolSetter, checker):

    # expand strings of cuts to a cut dictionary
    visitor = TreeParameterExpander()
    tree.accept(visitor)
    logger.debug(visitor.report())

    # coelesce identical nodes
    visitor = IdenticalNodeCompressor()
    tree.accept_cf(visitor)
    logger.debug(visitor.report())

    # move the filter conditions into node.filter_conditions
    visitor = FilterConditionsMover()
    tree.accept(visitor)

    # tell the child nodes who their parent is.
    tree.set_ids(node_id=0, parent_id=0)

    # check tree invariants 
    if checker is not None:
        tree.accept(checker)
        logger.debug(checker.report())
        assert not checker.error()
    
    # create - possibly nested - tools, The tools are attached to the visitor.
    toolSetter.mod(tree)

    return tree  # used by debug tools


def  nodesFromLabel(label):
    """from a label eg simple([(260et,320eta490, leg000)])
    create a node. The node may have children, thus forming a tree."""

    parser = ChainLabelParser(label, debug=debug)
    return parser.parse()
   
def trigJetHypoToolHelperConfigurersFromLabel(
        chain_label,  # simple([(260et,320eta490, leg000)])
        chain_name,):  # HLT_j260_320eta490_L1J75_31ETA49

    # construct the FastReduction Trees
    node_forest =  nodesFromLabel(chain_label)

    # use a visitor to convert simple types to a HypoConfigurer Tool,
    # one for each
    # tree, The HypoConfigurer Tool will have ConditionConfigure AlgTools
    # each creating a single Condition.
    configurer_tools = []
    for tree in node_forest:
        toolSetter = ConditionsToolSetterFastReduction(algToolFactory)
        checker = TreeChecker()
        tree2tools(tree, toolSetter, checker)
        configurer_tools.append(toolSetter.config_tool)   

        logger.debug(toolSetter.report())

    return configurer_tools


def trigJetConfigurerToolsFromDict(chain_dict):

    try:
        chain_label = chainDict2jetLabel(chain_dict)
    except Exception as e:
        m = str(e)
        m += ' - TrigJetHypoToolConfig: Error obtaining jet label for %s' % (
            chain_dict['chainName'],)
        m += '  jet hypo scenario: %s' % (
            chain_dict['chainParts'][0]['hypoScenario'],)
        logger.debug(m)        
        raise e

    chain_name = chain_dict['chainName']
    return trigJetHypoToolHelperConfigurersFromLabel(chain_label, chain_name)

       
def  trigJetHypoToolHelperFromDict(chain_dict):
    """Produce  a jet trigger hypo tool helper from a chainDict
    Helper tools do the hypio work. They are used, for example
    by TrigJetHypoToolMT to  devide whether an event passes.
    A Helper Tool returned by this function may be the root of a Helper
    Tool tree structure."""

    
    msg = 'trigJetHypoToolFromDict chainDict %s' %  str(chain_dict)
    logger.debug(msg)
    algToolFactory = FastReductionAlgToolFactory()

    # Build the helper tool
    helper_tool = algToolFactory('helper')

    # build the Configuration Tools for the JetHypoHelperTool.
    # there is one Configurer per tree in the forest.
    configurer_tools = trigJetConfigurerToolsFromDict(chain_dict)

    helper_tool.HypoConfigurers = configurer_tools

    # find the prefilters. These are Conditions that will be used
    # remove jets from the input jet collection
    # before the helper is run (eg blank out eta-phi regions,...)
    #
    # 1/2021... one could imagine using FastReducer to act as
    # a prefilter predicate. For now, more prosaically
    # we build a Conditions tree consisting  of a root node with
    # a single child. The root node willbe an AcceptAll condition.
    # Vetoing on this will remove all jets. The usefull conditions
    # are in the child node. These will be extracted here.

    label = prefilterLabelFromChainDict(chain_dict)
    if label:
        forest = ChainLabelParser(label, debug=debug).parse()
        assert len(forest) == 1
        tree = forest[0]

        assert tree.size() == 2  # root and single child.
        
        tool_setter = ConditionsToolSetterFastReduction(algToolFactory)

        checker = None
        tree2tools(tree.children[0], tool_setter, checker)  # single child node

        helper_tool.prefiltConditionMakers = tool_setter.conditionMakersVec

    return helper_tool
    

def  trigJetHypoToolFromDict_(chain_dict, hypo_tool, debug=False):
    """Produce  a jet trigger hypo tool from a chainDict"""

    
    msg = 'trigJetHypoToolFromDict_ tool type %s  chainDict %s' % (
        hypo_tool.__class__.__name__, str(chain_dict))
    logger.debug(msg)

    # obtain  a Helper Tool (possibly a tree of tools) to
    # make the hypo decision.
    hypo_tool.helper_tool = trigJetHypoToolHelperFromDict(chain_dict)

    # controls whether debug visitor is sent to helper tool
    hypo_tool.visit_debug = debug

    hypo_tool.chain_name = chain_dict['chainName']

    logger.debug(str(hypo_tool))

    return hypo_tool


def  trigJetTLAHypoToolFromDict(chain_dict):
    tool = CompFactory.TrigJetTLAHypoToolMT(name=chain_dict['chainName'])
    return trigJetHypoToolFromDict_(chain_dict, tool)


def  trigJetHypoToolFromDict(chain_dict):
    tool = CompFactory.TrigJetHypoToolMT(name=chain_dict['chainName'])
    tool.endLabelIndex = len(chain_dict['chainParts'])
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
            logger.debug(chain_name.rjust(wid), str(tool))


class TestDebugFlagIsFalse(unittest.TestCase):
    def testValidConfigs(self):
        self.assertFalse(debug)



if __name__ == '__main__':
    unittest.main()

    # other local tests have been moved to testChainDictMaker.py
