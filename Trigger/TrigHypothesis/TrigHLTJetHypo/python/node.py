# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
Node - represents a tree structure. scenario and parameters which are strings 
filled in while parsing a jet hyp[o label. A visitor is used to convert 
parameters to entites used to initialise jet hypo config alg tools. 
These processed quatnities are stored in the conf_attr dictionary.
The tree will be retraversed by a setter visitor which willuse the condig_Attrs
to instantiate a condiguration AlgTool, which, by convention, will have a
name TrigJetHypoToolConfig_XXX.
"""

from __future__ import print_function

from .constants import logicals


    
class Node(object):
    
    def __init__(self, scenario):

        self.node_id = None
        self.parent_id = None
        
        self.scenario = scenario
        self.parameters = ''
        self.children = []
        self.conf_attrs = []  # list of dictionaries
        self.tool = None
        self.compound_condition_tools = []
        # self.tree_top kludge carensure top level tools get chain name
        # as Tool name
        self.tree_top = False

    def set_ids(self, node_id, parent_id):
        "Set ids of nodes in a tree"

        self.node_id = node_id
        self.parent_id = parent_id

        c_node_id = node_id + 1
        for c in self.children:
            c_node_id = c.set_ids(c_node_id, self.node_id)
        return c_node_id
    
    def add_child(self, c):
        self.children.append(c)
        
    def accept(self, modifier):
        "call children before self"

        for c in self.children:
            c.accept(modifier)
        modifier.mod(self)

    def accept_cf(self, modifier):
        "call children before self"
        
        for c in self.children:
            c.accept_cf(modifier)
        modifier.mod(self)


    def buildTree(self, treeVisitor):
        if self.children:
            if self.scenario in logicals:
                treeVisitor.add(self.scenario + '(')
            else: 
                treeVisitor.add(self.tool.name() + '(')
                
            for c in self.children:
                c.buildTree(treeVisitor)
            treeVisitor.add(') ')

        else:
            treeVisitor.add(self.tool.name() + ' ')

    def str_list(self, indent):
        s = [indent + 'Node. scenario: %s' % self.scenario,
             indent + 'node id: %s' % self.node_id,
             indent + 'parent node id: %s' % self.parent_id,
             indent + 'is tree top? %s' % self.tree_top,
             indent + 'parameters: %s' % str(self.parameters),
             indent + 'conf_attrs [%d]:' % len(self.conf_attrs)]
        for ca in self.conf_attrs:
            s.append(indent + str(ca))
        
            # this attribute added by flow network setter tool
            s.append(indent + 'compound_condition_tools [%d]' % len(
                self.compound_condition_tools))

        s.append(indent + 'AlgTool: %s' % str(self.tool))
        s.append(indent + 'No of children: %d\n' % len(self.children))

        return s
        
    def dump_(self, n_in):

        indent = n_in * ' '
        s = self.str_list(indent)

        for c in self.children:
            s.extend(c.dump_(n_in+5))
            
        return s

    def dump(self):

        return '\n'.join(self.dump_(0))


    def __str__(self):

        return '\n'.join(self.str_list(''))

