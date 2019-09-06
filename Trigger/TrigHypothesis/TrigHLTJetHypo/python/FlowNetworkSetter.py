"""Instantiates TrigJetHypoToolConfig_flownetwork AlgTool 
from a hypo tree."""

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from TrigHLTJetHypo.TrigHLTJetHypoConf import (
    TrigJetConditionConfig_etaet,
    TrigJetConditionConfig_dijet,
    TrigJetHypoToolConfig_flownetwork,
    TrigJetHypoToolHelperMT,
)

from collections import defaultdict

class FlowNetworkSetter(object):
    """Visitor to set instantiated AlgTools to a jet hypo tree"""
    
    def __init__(self, name):

        self.name = name
        # for simple, use TrigJetConditionConfig_etaet. Needs to be
        # completed because simple can conain any single jet condition
        self.tool_factories = {
            'simple': [TrigJetConditionConfig_etaet, 0], 
            'etaet': [TrigJetConditionConfig_etaet, 0],
            'dijet': [TrigJetConditionConfig_dijet, 0],
            }

        self.nodeToParent=defaultdict(list)
        self.nodeToConditionTool=defaultdict(list)

        self.mod_router = {
            'not': self.not_supported_yet,
            'and': self.not_supported_yet,
            'agree': self.not_supported_yet,
            'or': self.not_supported_yet,
            'simple': self._mod_leaf,
            'etaet': self._mod_leaf,
            'simplepartition': self.not_supported_yet,
            'combgen': self.not_supported_yet,  #  shared with partgen
            'partgen': self.not_supported_yet,  #  shared with combgen
            'dijet': self._mod_leaf,
        }

    def not_supported_yet(self, node):
        raise NotImplementedError(
            'FlowNetworkSetter: scenarion not yet supported: ' + node.scenario)

    def mod_logical_binary(self, node):
        """Set the HypoConfigTool instance the 'and' and 'or' scenarios
        these take two predicates"""
        
        scen = node.scenario
        klass = self.tool_factories[scen][0]
        sn = self.tool_factories[scen][1]
        name = '%s_%d' % (scen, sn)
        self.tool_factories[scen][1] += 1

        tool = klass(name=name)

        # print 'ToolSetter, setting lhs ', node.children[0].tool
        tool.lhs = node.children[0].tool
        tool.rhs = node.children[1].tool

        tool.node_id = node.node_id
        tool.parent_id = node.parent_id

        node.tool = tool


    def mod_logical_unary(self, node):
        """Set the HypoConfigTool instance for the 'not' scenario
        this takes a single predicate"""
        
        scen = node.scenario
        klass = self.tool_factories[scen][0]
        sn = self.tool_factories[scen][1]
        name = '%s_%d' % (scen, sn)
        self.tool_factories[scen][1] += 1

        tool = klass(name=name)

        # print 'ToolSetter, setting lhs ', node.children[0].tool
        tool.hypoTool = node.children[0].tool

        tool.node_id = node.node_id
        tool.parent_id = node.parent_id

        node.tool = tool


    def mod_combgen(self, node):
        """Set the HypoConfigTool instance for the 'not' scenario
        this takes a single predicate"""
        
        scen = node.scenario
        klass = self.tool_factories[scen][0]
        sn = self.tool_factories[scen][1]
        name = '%s_%d' % (scen, sn)
        self.tool_factories[scen][1] += 1

        config_tool = klass(name=name+'_config')
        config_tool.children = [child.tool for child in node.children]
        [setattr(config_tool, k, v) for k, v in node.conf_attrs.items()]

        helper_tool = CombinationsHelperTool(name=name+'_helper')
        helper_tool.HypoConfigurer = config_tool

        helper_tool.node_id = node.node_id
        helper_tool.parent_id = node.parent_id

        node.tool = helper_tool

    def mod_simple(self, node):
        """Set the HypoConfigTool instance in a hypo tree node"""

        scen = node.scenario
        klass = self.tool_factories[scen][0]
        sn = self.tool_factories[scen][1]
        name = '%s_%d' % (scen, sn)
        
        self.tool_factories[scen][1] += 1

        config_tool = klass(name=name+'_config')
        [setattr(config_tool, k, v) for k, v in node.conf_attrs.items()]
        
        helper_tool = TrigJetHypoToolHelperMT(name=name+'_helper')
        helper_tool.HypoConfigurer = config_tool
        helper_tool.node_id = node.node_id
        helper_tool.parent_id = node.parent_id

        node.tool = helper_tool
 
    def _mod_leaf(self, node, treeMap, conditionMakers):
        """Set the HypoConfigTool instance in a hypo tree node"""

        scen = node.scenario
        klass = self.tool_factories[scen][0]
        sn = self.tool_factories[scen][1]
        name = '%s_%d' % (scen, sn)
        
        self.tool_factories[scen][1] += 1

        config_tool = klass(name=name)
        [setattr(config_tool, k, v) for k, v in node.conf_attrs.items()]
        node.tool = config_tool
        treeMap[node.node_id] = node.parent_id
        print node.dump()
        print 'treeMap from _mode_leaf_', treeMap
        conditionMakers[node.node_id] = config_tool
        


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

        # tree information to be filled in.
        treeMap = {}
        conditionMakersDict = {}

        # navicate the tree filling in node-parent and node- Condtion factory
        # relatrions
        self.mod_router[node.scenario](node,
                                       treeMap=treeMap,
                                       conditionMakers=conditionMakersDict)


        # convert the maps to  lists
        treeVector = [0 for i in range(len(treeMap))]
        for i,p in treeMap.items():
            treeVector[i] = p
        
        conditionMakersVector =  [0 for i in range(len(conditionMakersDict))]
        for i,p in conditionMakersDict.items():
            conditionMakersVector[i] = p

        # instantiate and initilise the TrigJetHypoToolConfig_flownetwork obj.
        # The helper tool will ask the config tool for various compnents.


        name = '%s_%d' % (node.scenario, node.node_id)
        print name

        config_tool = TrigJetHypoToolConfig_flownetwork(name + '_config')

        config_tool.treeVector = treeVector
        config_tool.conditionMakers = conditionMakersVector

        # instantantiate and initialise the helper tool
        helper_tool =  TrigJetHypoToolHelperMT(name+'_helper')
        helper_tool.HypoConfigurer = config_tool
        helper_tool.node_id = node.node_id
        helper_tool.parent_id = node.parent_id
        helper_tool.debug=True

        # overwrite the top node Condition Factory tool
        # with the TrigJetHypoToolHelperMT intance
        node.tool = helper_tool 

