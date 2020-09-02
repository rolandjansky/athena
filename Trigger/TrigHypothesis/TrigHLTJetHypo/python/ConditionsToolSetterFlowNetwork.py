# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Instantiates TrigJetHypoToolConfig_flownetwork AlgTool 
from a hypo tree."""

from __future__ import print_function

from AthenaConfiguration.ComponentFactory import CompFactory
from TrigHLTJetHypo.node import Node

from collections import defaultdict

import copy


def is_leaf(node):
    return node.scenario in  ('simple', 'etaet', 'dijet', 'qjet')


def is_inner(node):
    return node.scenario in ('root', 'and', 'combgen', 'partgen')


class ConditionsToolSetterFlowNetwork(object):

    """Visitor to set instantiated AlgTools to a jet hypo tree"""
    
    def __init__(self, name):

        self.name = name
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
            'all': [CompFactory.TrigJetConditionConfig_acceptAll, 0],
            'compound': [CompFactory.TrigJetConditionConfig_compound, 0],
            'flownetwork': [CompFactory.TrigJetHypoToolConfig_flownetwork, 0],
            'helper': [CompFactory.TrigJetHypoToolHelperMT, 0],
            }

        # map conaining parent child ids for the node
        self.treeMap = {0: 0}

        # map containing the a list of Condition factory AlgTools for scenario
        self.conditionMakers = defaultdict(list)

    def _set_conditions(self, node):
        """attach Conditions to leaf nodes"""

        self._mod_leaf(node)
        
        for cn in node.children:
            self._set_conditions(cn)


    def _remove_combgen(self, node):                       
        """Combination nodes represent parent children relationships.
        The child may be a subtree. For now, the parent will be in the 
        child list at position 0, and the child subtree in position 1."""

        parent_children = {}
        ipos  = 0

        # identify the combgen nodes, and rotate them
        for cn in node.children:
            if cn.scenario == 'combgen':
                assert (len(cn.children) == 2)
                parent_children[ipos] = cn.children
            ipos += 1

        # rotate the first combgen child (parent) into the position of the
        # combgen node, and set its child node.
        for pos, p_c in parent_children.items():
            node.children[pos] = p_c[0]
            node.children[pos].children = [p_c[1]]

        for cn in node.children:
            self._remove_combgen(cn)

    def _remove_scenario(self, node, scenario):                       
        """Remove Partgen nodes by adding their children to their 
        parent's children."""

        def remove_scenario(node, scenario):
            for cn in node.children:
                if cn.scenario == scenario:
                    node.children.remove(cn)
                    node.children.extend(cn.children)
                    return True

            return False

        more = True
        while(more):
            more = remove_scenario(node, scenario)

        for cn in node.children:
            self._remove_scenario(cn, scenario)

  
    def _get_tool_instance(self, key, extra=''):
   
        klass = self.tool_factories[key][0]
        sn = self.tool_factories[key][1]
        
        name = '%s_%d_fn' % (key, sn)
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

    def _split_leaves(self, node):
        """Recursively replace leaf nodes with >1 Condition tools by nodes with
        one Condition tool."""

        def split_leaves(node):   
            for cn in node.children:
                if is_leaf(cn):
                    if len(cn.compound_condition_tools) > 1:
                        new_children =  []
                        new_node = copy.deepcopy(cn)
                        # set scenarrio to other than leaf results in 
                        # the assignement of  an acceptall condition
                        new_node.scenario = 'inserted'
                        new_node.compound_condition_tools = []
                        for ct in cn.compound_condition_tools:
                            new_children.append(copy.deepcopy(cn))
                            new_children[-1].compound_condition_tools = [ct]
                            new_children[-1].conf_attrs = []
                        new_node.children.extend(new_children)
                        node.children.remove(cn)
                        node.children.append(new_node)
                        return True # return after first modification

            return False


        more = True
        while(more):
            more = split_leaves(node)

        for cn in node.children:
            self._split_leaves(cn)


    def _find_shared(self, node, shared):
        """Determine which nodes are "shared" - shared nodes
        are nodes that see the input jet collection. There
        more than one set of shared nodes. These are generated
        if an "And" not is present in the hypo tree"""


        if node.scenario == 'root':
            for cn in node.children:
                self._find_shared(cn, shared)

        elif node.scenario == 'and':
            for cn in node.children:
                shared.append([])
                self._find_shared(cn, shared)

        elif node.scenario == 'partgen':
            for cn in node.children:
                self._find_shared(cn, shared)

        elif node.scenario == 'inserted':
            for cn in node.children:
                self._find_shared(cn, shared)

        elif is_leaf(node):
            if len(node.children) == 0:
                if len(shared) == 0:
                    shared.append([node])
                else:
                    shared[-1].append(node)

            else:
                for cn in node.children:
                    self._find_shared(cn, shared)

        else:
            raise RuntimeError('%s illegal node. scenario: %s' %
                               (self.__class__.__name__,
                               node.scenario))

        return shared



    def report(self):
        wid = max(len(k) for k in self.tool_factories.keys())
        rep = '\n%s: ' % self.__class__.__name__

        rep += '\n'.join(
            ['%s: %d' % (k.ljust(wid), v[1])
             for k, v in self.tool_factories.items()])

        return rep

    def _fill_tree_map(self, node, tmap):
        tmap[node.node_id] = node.parent_id

        for cn in node.children:
            self._fill_tree_map(cn, tmap)

            
    def _fill_conditions_map(self, node, cmap):
        if is_leaf(node):

            assert (len(node.compound_condition_tools) == 1)
            cmap[node.node_id] = node.compound_condition_tools[0]

        else:
            # must have a tool for Gaudi to instantiate in
            cmap[node.node_id] = self._get_tool_instance('all')
        
        for cn in node.children:
            self._fill_conditions_map(cn, cmap)


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

    def mod(self, node):
        """Entry point for this module. 
        Modifies a  (usually compound) hypo tree node to 
        reduce it to form from whuch the treevector, conditionsVector and
        sharedNodes list can be extracted. These will be used to initialise
        FlowNetworkBuilder.

        In particular: all leaf nodes will have a single ConmpoundCondition
        All other nodes will be assigned an AcceptAll condition.
        """

        # navigate the tree filling in node-parent and node- Condtion factory
        # relations

        print ('%s: starts' % self.__class__.__name__)


        print ('%s: start step 1' % self.__class__.__name__)

        # Alg step 1: add root node
        root = Node(scenario='root')
        root.children = [node]

        print ('%s: checking scenarios' % self.__class__.__name__)

        self._check_scenarios(root)
        
        print ('%s: setting conditions' % self.__class__.__name__)
        # add Condition builders to leaf nodes.
        self._set_conditions(root)
        
        print ('%s: removing combgen' % self.__class__.__name__)
        # Alg step 2: remove combgen nodes
        self._remove_combgen(root)

        print ('%s: split leaves' % self.__class__.__name__)
        # Alg step 3: split leaf nodes with multiple Conditions with a
        # single Condition
        self._split_leaves(root)
        
        ## print ('%s: remove partgen' % self.__class__.__name__)
        # Alg step 4: remove partgen nodes
        # single Condition
        ## self._remove_scenario(root, 'partgen')

        # Alg step 5: identify the leaf nodes that are to shared
        # ie that see the input jet collection. Then remove And nodes
        shared = []
        slist = self._find_shared(root, shared)
        self._remove_scenario(root, 'and')

        # remove top stub node
        assert len(root.children) == 1
        root = root.children[0]
        root.set_ids(node_id=0, parent_id = 0)

        # would like to pass a list of lists to the C++ tools
        # but this cannot be done using Gaudi::Properties.
        # use -1 to separate the list sections all entries of which
        # are >= 0.

        self.shared = []
        for ilist in slist:
            for n in ilist:
                self.shared.append(n.node_id)
            self.shared.append(-1)

        self.shared = self.shared[:-1] # remnove trailing -1
            
        tree_map = {}
        self._fill_tree_map(root, tree_map)
        self.treeVec = self._map_2_vec(tree_map)

        conditionsMap = {}
        self._fill_conditions_map(root, conditionsMap)
        self.conditionsVec = self._map_2_vec(conditionsMap)
               
                # make a config tool and provide it with condition makers
        config_tool = self._get_tool_instance('flownetwork')
        config_tool.conditionMakers = self.conditionsVec
        config_tool.treeVector = self.treeVec
        config_tool.sharedVector = self.shared

        nodestr = 'n%dp%d' % (node.node_id, node.parent_id)
        helper_tool = self._get_tool_instance('helper', extra=nodestr)
        helper_tool.HypoConfigurer = config_tool
        helper_tool.node_id = node.node_id
        helper_tool.parent_id = node.parent_id

        self.tool = helper_tool
