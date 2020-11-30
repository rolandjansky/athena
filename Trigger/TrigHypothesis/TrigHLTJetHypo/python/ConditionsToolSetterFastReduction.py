# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""Instantiates TrigJetHypoToolConfig_fastreduction AlgTool 
from a hypo tree."""

from __future__ import print_function


from AthenaConfiguration.ComponentFactory import CompFactory

from collections import defaultdict

from AthenaCommon.Logging import logging
log = logging.getLogger( 'ConditionsToolSetterFastReduction' )

def is_leaf(node):
    return node.scenario in  ('simple', 'etaet', 'dijet', 'qjet')


def is_inner(node):
    # return node.scenario in ('root', 'and', 'combgen', 'partgen' , 'inserted')
    return node.scenario in ('root', 'all', 'inserted')


class ConditionsToolSetterFastReduction(object):

    """Visitor to set instantiated AlgTools to a jet hypo tree"""

    JetMoments = {
        'emfrac'  : 'EMFrac',
        'hecfrac' : 'HECFrac',
    }

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
            'smc': [CompFactory.TrigJetConditionConfig_smc, 0],
            'jvt': [CompFactory.TrigJetConditionConfig_jvt, 0],
            'all': [CompFactory.TrigJetConditionConfig_acceptAll, 0],
            'capacitychecked':
            [CompFactory.TrigJetConditionConfig_capacitychecked, 0],
            'fastreduction': [CompFactory.TrigJetHypoToolConfig_fastreduction, 0],
            'helper': [CompFactory.TrigJetHypoToolHelperMT, 0],
            }
        for var in self.JetMoments:
            self.tool_factories['mom'+var] = [CompFactory.TrigJetConditionConfig_moment, 0]

        # map conaining parent child ids for the node
        self.treeMap = {0: 0}

        # map containing the a list of Condition factory AlgTools for scenario
        self.conditionMakers = defaultdict(list)

    def _set_conditions(self, node):
        """attach Conditions to leaf nodes"""

        self._mod_leaf(node)
        
        for cn in node.children:
            self._set_conditions(cn)


 
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
        outer_condition_tools = []

        # loop  over elements of node.conf_attrs. The elements are (dict, int)
        # int is multiplicity, dict holds Condition parameters.

        for i in range(len(node.conf_attrs)):
            c, mult = node.conf_attrs[i]
            cpi = ''
            if node.chainpartinds:
                cpi = node.chainpartinds[i][0]
                assert mult == node.chainpartinds[i][1]
                    
                
            condition_tools = [] # elemental conditions for this compounnd ct.
            for k, v in c.items(): # loop over elemental conditions
                condition_tool = self._get_tool_instance(k)
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

            # create capacitychecked condition from elemental condition
            condition_tool =self._get_tool_instance('capacitychecked')
            condition_tool.chainLegLabel = cpi
            condition_tool.conditionMakers = condition_tools
            condition_tool.multiplicity = mult
            # add capacitychecked condition to list
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
            cmap[node.node_id] = self._get_tool_instance('capacitychecked')
            cmap[node.node_id].conditionMakers = [self._get_tool_instance('all')]
            cmap[node.node_id].multiplicity = 1
        
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
  

        tree_map = {}
        self._fill_tree_map(tree, tree_map)

        for k, v in tree_map.items():
            log.debug("Tree map debug ", str(k), str(v))
            
        treeVec = self._map_2_vec(tree_map)

        conditionsMap = {}
        self._fill_conditions_map(tree, conditionsMap)
        conditionsVec = self._map_2_vec(conditionsMap)
               
        # make a config tool and provide it with condition makers
        config_tool = self._get_tool_instance('fastreduction')
        config_tool.conditionMakers = conditionsVec
        config_tool.treeVector = treeVec

        nodestr = 'n%dp%d' % (tree.node_id, tree.parent_id)
        helper_tool = self._get_tool_instance('helper', extra=nodestr)
        helper_tool.HypoConfigurer = config_tool
        helper_tool.node_id = tree.node_id
        helper_tool.parent_id = tree.parent_id

        self.tool = helper_tool
