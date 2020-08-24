# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""Instantiates TrigJetHypoToolConfig_flownetwork AlgTool 
from a hypo tree."""

from __future__ import print_function


from AthenaConfiguration.ComponentFactory import CompFactory

from collections import defaultdict

class ConditionsToolSetterTree(object):
    """Visitor to set instantiated AlgTools to a jet hypo tree"""
    
    def __init__(self, name):

        self.name           = name
        # for simple, use TrigJetConditionConfig_etaet. Needs to be
        # completed because simple can conain any single jet condition
        self.tool_factories = {
            'eta': [CompFactory.TrigJetConditionConfig_abs_eta, 0], 
            'peta': [CompFactory.TrigJetConditionConfig_signed_eta, 0],
            'neta': [CompFactory.TrigJetConditionConfig_signed_eta, 0],
            'et': [CompFactory.TrigJetConditionConfig_et, 0],
            'djmass': [CompFactory.TrigJetConditionConfig_dijet_mass, 0],
            'djdphi': [CompFactory.TrigJetConditionConfig_dijet_dphi, 0],
            'djdeta': [CompFactory.TrigJetConditionConfig_dijet_deta, 0],
            'qjmass': [CompFactory.TrigJetConditionConfig_qjet_mass, 0],
            'momwidth': [CompFactory.TrigJetConditionConfig_moment, 0],
            'smc': [CompFactory.TrigJetConditionConfig_smc, 0],
            'jvt': [CompFactory.TrigJetConditionConfig_jvt, 0],
            'compound': [CompFactory.TrigJetConditionConfig_compound, 0],
            'leaf': [CompFactory.TrigJetHypoToolConfig_leaf, 0],
            'helper': [CompFactory.TrigJetHypoToolHelperMT, 0],
            'combgen_helper': [CompFactory.CombinationsHelperTool, 0],
            'not': [CompFactory.NotHelperTool, 0],
            'and': [CompFactory.AndHelperTool, 0],
            'agree': [CompFactory.AgreeHelperTool, 0],
            'or': [CompFactory.OrHelperTool, 0],
            'combgen': [CompFactory.TrigJetHypoToolConfig_combgen, 0],
            'partgen': [CompFactory.TrigJetHypoToolConfig_partgen, 0],
            }

        self.mod_router = {
            'z': self._mod_z,
            'not': self.mod_logical_unary,
            'and': self.mod_logical_binary,
            'agree': self.mod_logical_binary,
            'or': self.mod_logical_binary,
            'combgen': self._mod_combgen,  #  shared with partgen
            'partgen': self._mod_combgen,  #  shared with combgen
            'simple': self._mod_leaf,
            'etaet': self._mod_leaf,
            'dijet': self._mod_leaf,
            'qjet': self._mod_leaf,
        }

        # map conaining parent child ids for the node
        self.treeMap = {0: 0}

        # map containing the a list of Condition factory AlgTools for scenario
        self.conditionMakers = defaultdict(list)

    def not_supported_yet(self, node):
        raise NotImplementedError(
            'ConditionsToolSetter: scenarion not yet supported: ' + node.scenario)


    def illegal(self, node):
        raise RuntimeError(
            'ConditionsToolSetter: illegal scenario: %s' + node.scenario)

    def _mod_z(self, node):
        """z is the root node: process its children"""

        for cn in node.children:
            self.mod_router[cn.scenario](cn)

    def mod_logical_binary(self, node):
        """Set the HypoConfigTool instance the 'and' and 'or' scenarios
        these take two predicates"""
        
        scen = node.scenario
        klass = self.tool_factories[scen][0]
        sn = self.tool_factories[scen][1]
        name = '%s_%d' % (scen, sn)
        self.tool_factories[scen][1] += 1

        tool = klass(name=name)

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

        tool.hypoTool = node.children[0].tool

        tool.node_id = node.node_id
        tool.parent_id = node.parent_id

        node.tool = tool

    def _mod_combgen(self, node):
        """Set the HypoConfigTool instance for the combgen or partgeen scenario
        these take different condifiguration tools."""

        config_tool = self._get_tool_instance(node.scenario)  

        compound_condition_tools = self._make_compound_condition_tools(node)
        config_tool.conditionMakers = compound_condition_tools
        # for combinational nodes, the connection between job groups and
        # child helper tools is done via a matcher. Provide the helper
        # tool children to the condig class which uses them to
        # construct the matcher.
        config_tool.children = [child.tool for child in node.children]

        nodestr = 'n%dp%d' % (node.node_id, node.parent_id)
        helper_tool = self._get_tool_instance('combgen_helper', extra=nodestr)

        helper_tool.HypoConfigurer = config_tool

        helper_tool.node_id = node.node_id
        helper_tool.parent_id = node.parent_id

        node.tool = helper_tool
  
    def _get_tool_instance(self, key, extra=''):
   
        klass = self.tool_factories[key][0]
        sn = self.tool_factories[key][1]
        
        name = '%s_%d' % (key, sn)
        if extra: name += '_' + extra
        tool = klass(name=name)            
        self.tool_factories[key][1] += 1
        return tool

    def _make_compound_condition_tools(self, node):

        #  compound_condition_tools:
        # elemental condition maker AlgToolshelper by the compound condition
        # AlgTool
        compound_condition_tools = []  
        for c in node.conf_attrs: # loop over conditions
            condition_tools = [] # elemental conditions for this compounnd ct.
            for k, v in c.items(): # loop over elemental conditions
                condition_tool = self._get_tool_instance(k)
                for lim, val in v.items():  # lim: min, max
                    setattr(condition_tool, lim, val)

                # SPECIAL CASE: Moment tool needs the name of the
                # moment as well as the min. max cuts:
                if (k.startswith ('mom')):
                    condition_tool.moment = k[len('mom'):]                    
                # END SPECIAL CASE

                condition_tools.append(condition_tool)

            # create compound condition from elemental condition
            compoundCondition_tool =self._get_tool_instance('compound')
            compoundCondition_tool.conditionMakers = condition_tools

            # add compound condition to list
            compound_condition_tools.append(compoundCondition_tool)

        return compound_condition_tools

    def _mod_leaf(self, node):
        """For a leaf node, fill in 
        1. map conaining parent child ids for the node
        2. map conainting the AlgTool used to instanitiate the node Conditions
        """

        self.treeMap[node.node_id] = node.parent_id

        #
        # parameters: (10et,0eta320)(20et)
        # conf_attrs: [2]: (is a list of length 2)
        # defaultdict(<type 'dict'>, {'et': {'max': 'inf', 'min': '10000.0'},
        #                             'eta': {'max': '3.2', 'min': '0.0'}})
        # defaultdict(<type 'dict'>, {'et': {'max': 'inf', 'min': '20000.0'}})


        # make a config tool and provide it with condition makers
        config_tool = self._get_tool_instance('leaf')
        compound_condition_tools = self._make_compound_condition_tools(node)
        config_tool.conditionMakers = compound_condition_tools

        nodestr = 'n%dp%d' % (node.node_id, node.parent_id)
        helper_tool = self._get_tool_instance('helper', extra=nodestr)

        helper_tool.HypoConfigurer = config_tool
        helper_tool.node_id = node.node_id
        helper_tool.parent_id = node.parent_id

        node.tool = helper_tool

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

        # navigate the tree filling in node-parent and node- Condtion factory
        # relations
        
        self.mod_router[node.scenario](node)

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
