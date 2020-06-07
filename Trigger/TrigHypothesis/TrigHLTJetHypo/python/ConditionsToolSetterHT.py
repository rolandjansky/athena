# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""Instantiates TrigJetHypoToolConfig_fastreduction AlgTool 
from a hypo tree."""

from __future__ import print_function

from TrigHLTJetHypo.TrigHLTJetHypoConf import (
    TrigJetConditionConfig_ht,
    TrigJetHypoToolConfig_ht,
    TrigJetHypoToolHelperMT,
)


class ConditionsToolSetterHT(object):

    """Visitor to set instantiated AlgTools to a jet hypo tree"""
    
    def __init__(self, name):

        self.name = name
        # for simple, use TrigJetConditionConfig_etaet. Needs to be
        # completed because simple can conain any single jet condition
        self.tool_factories = {
            'htcondition': [TrigJetConditionConfig_ht, 0],
            'htconfig': [TrigJetHypoToolConfig_ht, 0],
            'helper': [TrigJetHypoToolHelperMT, 0],
        }


    def _get_tool_instance(self, key, extra=''):
   
        klass = self.tool_factories[key][0]
        sn = self.tool_factories[key][1]
        
        name = '%s_%d_fn' % (key, sn)
        if extra: name += '_' + extra
        tool = klass(name=name)            
        self.tool_factories[key][1] += 1
        return tool
    
    def report(self):
        wid = max(len(k) for k in self.tool_factories.keys())
        rep = '\n%s: ' % self.__class__.__name__

        rep += '\n'.join(
            ['%s: %d' % (k.ljust(wid), v[1])
             for k, v in self.tool_factories.items()])

        return rep

    def mod(self, node):
        """Entry point for this module. HT specific.
        Set up 
        TrigJetConditionConfig_ht,
        TrigJetHypoToolConfig_h
        TrigJetHypoToolHelperMT
        """

        # navigate the tree filling in node-parent and node- Condtion factory
        # relations


        # root = Node(scenario='root')
        # root.children = [node]

        # self._check_scenarios(root)

        # root is an alias for node - as in ConditionTooSetterFastReduction
        assert node.scenario == 'ht'

        print (node)
        conditionMaker = self._get_tool_instance('htcondition')
        config_tool = self._get_tool_instance('htconfig')
        cut_windows = {}
        [cut_windows.update(d) for d in node.conf_attrs]
        print (cut_windows)
        conditionMaker.htmin = cut_windows['ht']['min']
        conditionMaker.etmin = cut_windows['et']['min']
        conditionMaker.etamin = cut_windows['eta']['min']
        conditionMaker.etamax = cut_windows['eta']['max']
        print (conditionMaker)
            
        config_tool.conditionMakers = [conditionMaker]
        helper_tool = self._get_tool_instance('helper')
        helper_tool.HypoConfigurer = config_tool
 
        self.tool = helper_tool
