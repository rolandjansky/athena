# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

"""Instantiates TrigJetHypoToolConfig_fastreduction AlgTool 
from a hypo tree."""

from __future__ import print_function

from collections import defaultdict

from AthenaCommon.Logging import logging
log = logging.getLogger( 'ConditionsToolSetterFastReduction' )

def is_leaf(node):
    return node.scenario in  ('simple', 'etaet', 'dijet', 'qjet', 'agg')


def is_inner(node):
    # return node.scenario in ('root', 'and', 'combgen', 'partgen' , 'inserted')
    return node.scenario in ('root', 'all', 'inserted')


class ConditionsToolSetterFastReduction(object):

    """Visitor to set instantiated AlgTools to a jet hypo tree"""


    def __init__(self, algToolFactory):

        self.algToolFactory = algToolFactory
 
        # map conaining parent child ids for the node
        self.treeMap = {0: 0}

        # map containing the a list of Condition factory AlgTools for scenario
        self.conditionMakers = defaultdict(list)

    def _set_conditions(self, node):
        """attach Conditions to leaf nodes"""

        self._mod_leaf(node)
        
        for cn in node.children:
            self._set_conditions(cn)


    def _make_el_condition_tools(self, conf_dict):
        """conf_dict: a dict containing names of elemental conditions 
        and min, max valies. These will be used to instantiate
        conditon building AlgTools, one for eac conditon 

        for 2j80_2j60, the dictionaries are:
        {'et': {'min': '80000.0', 'max': 'inf'}, 
        'eta': {'min': '0.0', 'max': '3.2'}}

        and 


        {'et': {'min': '60000.0', 'max': 'inf'}, 
        'eta': {'min': '0.0', 'max': '3.2'}})
        
        """

        condition_tools = [] # a list of AlgTools that build elemental Conds.
        
        for k, v in conf_dict.items(): # loop over elemental conditions
            # k in the condition name, v contains its min, max values.

            # create the AlgTool that will build the elemental condition
            condition_tool = self.algToolFactory(k) 
            for lim, val in v.items():  # lim: min, max
                setattr(condition_tool, lim, val)

            # SPECIAL CASE: Moment tool needs the name of the
            # moment as well as the min. max cuts:
            if (k.startswith ('mom')):
                moment = k[len('mom'):]
                if moment in self.JetMoments:
                    condition_tool.moment = self.JetMoments[moment]
                else: raise RuntimeError('%s not supported' % (moment))

            # END SPECIAL CASE

            condition_tools.append(condition_tool)

        return condition_tools


    def _make_filter_condition_tool(self, node):

        """Condtion filters use a list of CompoundCondition containing
        single jet elemental conditions  select a subset of the reco
        jets to send to the a Condition"""
        
        el_condition_tools = []

        for fc in node.filter_dicts:
            
            assert len(fc) == 1  # 1 elemental condition
            el_condition_tools.extend(self._make_el_condition_tools(fc))

        if not el_condition_tools:
            el_condition_tools.append(self.algToolFactory('all'))

        condition_tool = self.algToolFactory('repeated')

        condition_tool.conditionMakers = el_condition_tools
        condition_tool.multiplicity = 1
        
        return condition_tool

    
    def _make_compound_condition_tools(self, node):
        """For each element of  node.conf_attrs, construct a 
        ConditionContainer. Example for chain HLT_2j80_3j60_L1J15:

        First leaf node has 
        conf_attrs [1]:
        (defaultdict(<class 'dict'>, {
        'et': {'min': '80000.0', 'max': 'inf'}, 
        'eta': {'min': '0.0', 'max': '3.2'}}), 2)

        Second leaf node has 
        conf_attrs [1]:
        (defaultdict(<class 'dict'>, {'et': {'min': '60000.0', 'max': 'inf'}, 
        'eta': {'min': '0.0', 'max': '3.2'}}), 3)
        """

        #  compound_condition_tools:
        # elemental condition maker AlgToolshelper by the compound condition
        # AlgTool
        outer_condition_tools = []

        # loop  over elements of node.conf_attrs. The elements are (dict, int)
        # int is multiplicity, dict holds Condition parameters.

        assert len(node.conf_attrs) == 1
        mult = node.multiplicity
        for i in range(len(node.conf_attrs)):
            c = node.conf_attrs[i]
            cpi = ''
 
            if node.chainpartinds:
                cpi = node.chainpartinds[i]

            el_condition_tools = self._make_el_condition_tools(c)
 
            # create condition from elemental conditions
            condition_tool =self.algToolFactory('repeated')

            if cpi:

                # convert label from string to int for more efficient
                # processing in C++ land.
                condition_tool.chainPartInd = int(cpi[len('leg'):])
            
            condition_tool.conditionMakers = el_condition_tools
            condition_tool.multiplicity = mult
            # add condition container to list
            outer_condition_tools.append(condition_tool)
            
        return outer_condition_tools

    
    def _mod_leaf(self, node):
        """ Add Condition tools to For a leaf node."""

        if not is_leaf(node):
            return

        # parameters: (10et,0eta320)(20et)
        # conf_attrs: [2]: (is a list of length 2)
        # defaultdict(<type 'dict'>, {'et': {'max': 'inf', 'min': '10000.0'},
        #                             'eta': {'max': '3.2', 'min': '0.0'}})
        # defaultdict(<type 'dict'>, {'et': {'max': 'inf', 'min': '20000.0'}})


        # make a config tool and provide it with condition makers


        node.compound_condition_tools = self._make_compound_condition_tools(
            node)

        node.filter_condition_tool = self._make_filter_condition_tool(
            node)
            
    def report(self):
        return str(self.algToolFactory)
   
    def _fill_tree_map(self, node, tmap):
        tmap[node.node_id] = node.parent_id
        for cn in node.children:
            self._fill_tree_map(cn, tmap)

            
    def _fill_conditions_map(self, node, cmap, fmap):
        if is_leaf(node):

            assert (len(node.compound_condition_tools) == 1)
            cmap[node.node_id] = node.compound_condition_tools[0]

            fmap[node.node_id] = node.filter_condition_tool

        else:

            cmap[node.node_id] = self.algToolFactory('repeated')
            cmap[node.node_id].conditionMakers = [self.algToolFactory('all')]
            cmap[node.node_id].multiplicity = 1

            fmap[node.node_id] = self.algToolFactory('repeated')
            fmap[node.node_id].conditionMakers = [self.algToolFactory('all')]
            fmap[node.node_id].multiplicity = 1

        
        for cn in node.children:
            self._fill_conditions_map(cn, cmap, fmap)


    def _map_2_vec(self, amap):

        vec = [0 for i in range(len(amap))]
        for nid, value in amap.items():
            vec[nid] = value
        return vec

    def _check_scenarios(self, node):
        if not(is_inner(node) or is_leaf(node)):
            raise RuntimeError(
                'ConditionsToolSetter: illegal scenario: %s' % node.scenario)

        for cn in node.children:
            self._check_scenarios(cn)

    def mod(self, tree):
        """Entry point for this module. 
        Modifies a  (usually compound) hypo tree node to 
        reduce it to form from whuch the treevector, and conditionsVector
        These will be used to initialise FastReductionMatcher.

        In particular: all leaf nodes will have a single ConmpoundCondition
        All other nodes will be assigned an AcceptAll condition.
        """

        # navigate the tree filling in node-parent and node- Condtion factory
        # relations

        self._check_scenarios(tree)
        
        # add Condition builders to leaf nodes.
        self._set_conditions(tree)
  

        tree_map = {}  # tree of node indices
        self._fill_tree_map(tree, tree_map)

        treeVec = self._map_2_vec(tree_map)

        conditionsMap = {}
        filterConditionsMap = {}

        self._fill_conditions_map(tree, conditionsMap, filterConditionsMap)

        # conditionVec is an attribute as it will be used directly
        # to make prefilter tools, so hold onto it here
        self.conditionMakersVec = self._map_2_vec(conditionsMap)
        filterConditionsVec = self._map_2_vec(filterConditionsMap)
               
        # make a config tool and provide it with condition makers
        config_tool = self.algToolFactory('fastreduction')


        config_tool.conditionMakers = self.conditionMakersVec
        config_tool.filtConditionsMakers = filterConditionsVec
        config_tool.treeVector = treeVec
        self.config_tool = config_tool
        
        print (self.config_tool)
