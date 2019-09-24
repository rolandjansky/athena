# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Instantiates TrigJetHypoToolConfig_flownetwork AlgTool 
from a hypo tree."""

from __future__ import print_function

from TrigHLTJetHypo.TrigHLTJetHypoConf import (
    TrigJetConditionConfig_eta,
    TrigJetConditionConfig_et,
    TrigJetConditionConfig_dijet_mass,
    TrigJetHypoToolConfig_flownetwork,
    TrigJetHypoToolHelperMT,
    )

from collections import defaultdict

class ConditionsToolSetter(object):
    """Visitor to set instantiated AlgTools to a jet hypo tree"""
    
    def __init__(self, name):

        self.name = name
        # for simple, use TrigJetConditionConfig_etaet. Needs to be
        # completed because simple can conain any single jet condition
        self.tool_factories = {
            'eta': [TrigJetConditionConfig_eta, 0], 
            'et': [TrigJetConditionConfig_et, 0],
            'djmass': [TrigJetConditionConfig_dijet_mass, 0],
            }

        self.nodeToParent=defaultdict(list)
        self.nodeToConditionTool=defaultdict(list)

        self.mod_router = {
            'z': self._mod_z,
            'agree': self.not_supported_yet,
            'simple': self._mod_leaf,
            'etaet': self._mod_leaf,
            'dijet': self._mod_leaf,
        }

        # map conaining parent child ids for the node
        self.treeMap = {0: 0}

        # map containing the a list of Condition factory AlgTools for scenario
        self.conditionMakers = defaultdict(list)

    def not_supported_yet(self, node):
        raise NotImplementedError(
            'ConditionsToolSetter: scenarion not yet supported:',
            node.scenario)

    
    def illegal(self, node):
        raise RuntimeError(
            'ConditionsToolSetter: illegal scenario: %s' + node.scenario)

    def _mod_z(self, node):
        """z is the root node: process its children"""

        print ('_mod_z: treating ', len(node.children), 'children')
        for cn in node.children:
            print ('treating child ', node.node_id)
            self.mod_router[cn.scenario](cn)

    def _mod_leaf(self, node):
        """For a leaf node, fill in 
        1. map conaining parent child ids for the node
        2. map conainting the AlgTool used to instanitiate the node Conditions
        """
        print ('ConditionsToolSetter processing node with scenario',
               node.scenario)

        self.treeMap[node.node_id] = node.parent_id
        print ('_mod_leaf', node.node_id, node.parent_id, len(node.children))

        scen = node.scenario

        for k, v in node.conf_attrs.items():
      
            klass = self.tool_factories[scen][0]
            sn = self.tool_factories[scen][1]

            name = '%s_%d' % (scen, sn)
            config_tool = klass(name=name)
            setattr(config_tool, k, v)
            self.conditionMakers[node.node_id].append(config_tool)
            self.tool_factories[scen][1] += 1

 
        for cn in node.children:
             self.mod_router[cn.scenario](cn)

    def report(self):
        wid = max(len(k) for k in self.tool_factories.keys())
        rep = '\n%s: ' % self.__class__.__name__

        rep += '\n'.join(
            ['%s: %d' % (k.ljust(wid), v[1])
             for k, v in self.tool_factories.items()])

        return rep


    def mod(self, node):
        """Entry mode for this module. Uses a (usaully compound) hypo tree node.
        - The Tools set in the tree nodes are TrigJetConditionConfig_XXX
          objects. These are factories for Condition objects
        - The tree use used to 
          = find the tree vector (node-parent relationsships)
          = find the node - Condition factory relationships
          = instantiate and return a TrigJetHypoToolConfig_flownetwork instance.
        """

 
        # navigate the tree fillin]g in node-parent and node- Condtion factory
        # relations
        print ('ConditionsToolSetter - node.scenario', node.scenario)
        self.mod_router[node.scenario](node)

        
        print ('ConditionsToolSetter result:')
        print (node)

        config_tool = TrigJetHypoToolConfig_flownetwork()

        print ('treeMap: ', self.treeMap)
        treeVec = [0 for i in range(len(self.treeMap))]
        for n, p in self.treeMap.items():
            treeVec[n] = p

        config_tool.treeVector = treeVec
           
        conditionMakers = [None for i in range(len(self.treeMap))]
        for n, p in self.conditionMakers.items():
            conditionMakers[n] = p

        
        print (conditionMakers)

        assert None not in conditionMakers 

        config_tool.conditionMakers = conditionMakers
            
        self.tool = TrigJetHypoToolHelperMT(
            name='TrigJetHypoToolConfig_flownetwor_helper')
    
        self.tool.HypoConfigurer = config_tool


    def __str__(self):
        s =  '%s: treeMap: %s, conditionMakers [%d]: ' % (
            self.__class__.__name__,
            str(self.treeMap),
            len(self.conditionMakers))

        if self.conditionMakers:
            s += '\n'
            for cm in self.conditionMakers.values():
                s +=  cm.__class__.__name__ + '\n'

        return s 
