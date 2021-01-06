# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""Instantiates TrigJetHypoToolConfig_fastreduction AlgTool 
from a hypo tree."""

from __future__ import print_function

from TrigHLTJetHypo.node import Node

import copy

from AthenaCommon.Logging import logging
log = logging.getLogger( 'ConditionsToolSetterVisitor' )

def is_leaf(node):
    return node.scenario in  ('simple', 'etaet', 'dijet', 'qjet')


def is_inner(node):
    return node.scenario in ('root', 'all', 'inserted')


class NodeSplitterVisitor(object):

    """Visitor to set instantiated AlgTools to a jet hypo tree"""


    def mod(self, node):

        new_children = []
        for c in node.children:
            if c.scenario == 'simple':
                assert ((len(c.chainpartinds) ==
                        len(c.conf_attrs))) or not c.chainpartinds
                for cpi, c_a in zip(c.chainpartinds, c.conf_attrs):
                    n_c = copy.deepcopy(c)
                    n_c.conf_attrs = [c_a]
                    n_c.chainpartinds = [cpi]
                    new_children.append(n_c)
            else:
                new_children.append(c)
                

        node.children = new_children

if __name__ == '__main__':
    from chainDict2jetLabel import chainDict2jetLabel 
    from TrigHLTJetHypo.treeVisitors import TreeParameterExpander
    from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp
    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName
    from TriggerMenuMT.HLTMenuConfig.Menu.Physics_pp_run3_v1 import (
        SingleJetGroup,
        MultiJetGroup)
    from  TrigHLTJetHypo.ChainLabelParser import ChainLabelParser


    def testChainDictMaker():
        chain_props = [
            ChainProp(name='HLT_j260_320eta490_L1J75_31ETA49',
                      groups=SingleJetGroup),
            
            ChainProp(name='HLT_j80_j60_L1J15',
                      l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup),
            
            ChainProp(name='HLT_2j80_3j60_L1J15',
                    l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup),


            ChainProp(name='HLT_j0_HTSEP1000htSEP100etSEP0eta320_L1J15',
                      l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),


            ChainProp(name='HLT_j80_0eta240_2j60_320eta490_j0_dijetSEP80j1etSEP0j1eta240SEP80j2etSEP0j2eta240SEP700djmass_L1J20',
                      l1SeedThresholds=['FSNOSEED']*3,
                      groups=MultiJetGroup),
            
            ChainProp(name='HLT_10j40_L1J15',
                      l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),
            

            # ChainProp(name='HLT_j70_j50 _0eta490_invm1000j50_dphi20_deta40_L1J20',
            #          l1SeedThresholds=['FSNOSEED']*2,
            #          groups=MultiJetGroup),
            
        ]

        result = []
        for cp in chain_props:
            chain_dict = dictFromChainName(cp)
            result.append((cp.name, chain_dict))

        return result


    dicts = testChainDictMaker()
    for d in dicts:
        print('')
        # print (d)

    print ('\n chain_labels:\n')

    for d in dicts:
        print (d[0])
        l =  (chainDict2jetLabel(d[1]))
        print (l)
        print ()


        parser = ChainLabelParser(l)
        rootless_tree = parser.parse()

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

        visitor = NodeSplitterVisitor()
        tree.accept(visitor)
        tree.set_ids(0, 0)

        print ('\n - final dump ' + d[0])
        print ('label ', l, '\n')
        print (tree.dump())
        print ('\n - end dump ')
