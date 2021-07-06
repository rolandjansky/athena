# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from collections import defaultdict

class HypoToolAnalyser:
    """ "Look inside" a jet hypo tool, and post its contents, including all
    nested classes to two tables:
    - self.node_table : a look up table of node id (int)  to AlgTool
    - self.connected: a look up table of connections. Keys are node ids,
     values are the list of node ids of tool contained within the tool
    with node id given by the key.

    example (tool name replaces tool instance):
    
    node_table:

    0 HLT_j80_j60_L1J15
    1 helper_tool_0
    2 HelperToolConfigTool_0
    3 RepeatedConditionConfigTool_0
    4 all_0
    5 RepeatedConditionConfigTool_2
    6 eta_0
    7 et_0
    8 RepeatedConditionConfigTool_3
    9 eta_1
    10 et_1
    11 RepeatedConditionConfigTool_1
    12 RepeatedConditionConfigTool_4
    13 RepeatedConditionConfigTool_5


    connections:

    0 [1]
    1 [2]
    2 [3, 5, 8, 11, 12, 13]
    3 [4]
    5 [6, 7]
    8 [9, 10] 
"""
    def __init__(self, tool):
        self.connections = defaultdict(list)
        self.node_table = {}
        self.nid = 0
        self.cid = 1

        self._process(tool)
        
    def _process(self, tool):

        self.node_table[self.nid] = tool

    
        children = []
        for k, v in tool._properties.items():
            if v.__class__.__name__ in (
                    'TrigJetHypoToolHelperNoGrouper',
                    'TrigJetHypoToolConfig_conditionfilter',
                    'TrigJetHypoToolConfig_passthroughfilter',
            ):
                children.append(v)
            
            elif v.__class__.__name__== 'PrivateToolHandleArray':
                children.extend(v)

        pid = self.nid

        for c in children:
            self.nid += 1
            self.connections[pid].append(self.nid)
            self._process(c)

    def tables(self):
        return self.node_table, self.connections

    def __str__(self):

        s = [self.__class__.__name__, '\nnode_table:\n']
        for k, v in self.node_table.items():
            s.append('%3d: %s' % (k, v.name))

        s.append('\nconnections:\n')

        for k, v in self.connections.items():
            s.append('%3d: %s' % (k, str(v)))

        return ('\n'.join(s))
            
