from TrigHLTJetHypo.TrigHLTJetHypoConf import (TrigJetHypoToolConfig_simple,
                                               TrigJetHypoToolConfig_dijet,
                                               TrigJetNotToolMT,
                                               TrigJetAndToolMT,
                                               TrigJetOrToolMT,
                                               TrigJetHypoToolMT)


class ToolSetter(object):
    """Visitor to set instantiated AlgTools to a jet hypo tree"""
    
    def __init__(self, name, dump_jets=True):

        self.chain_name = name
        self.dump_jets = dump_jets
        
        self.tool_factories = {
            'simple': [TrigJetHypoToolConfig_simple, 0],
            'not': [TrigJetNotToolMT, 0],
            'and': [TrigJetAndToolMT, 0],
            'or': [TrigJetOrToolMT, 0],
            'dijet': [TrigJetHypoToolConfig_dijet, 0],
            }

        self.mod_router = {
            'not': self.mod_logical_unary,
            'and': self.mod_logical_binary,
            'or': self.mod_logical_binary,
            # 'not': self.mod_logical_unary,
        }

    def mod_logical_binary(self, node):
        """Set the HypoConfigTool instance the 'and' and 'or' scenarios
        these take two predicates"""
        
        scen = node.scenario
        klass = self.tool_factories[scen][0]
        sn = self.tool_factories[scen][1]
        name = '%s_%d' % (scen, sn)
        self.tool_factories[scen][1] += 1

        # kludgy. The name of the tool seen by the trigger must be
        # the trigger name, so have to figure  out if this is the top
        # level node (actually first daughter, as the start node is the top)
        # note that the name can only be set once so have to know
        # if we are the top of the tree while traversing it. kludgy...
        # also - will break when using a forest...

        print 'Toolsetter, node.tree_top', node.tree_top
        if node.tree_top:
            tool = klass(name=self.chain_name)
        else:
            tool = klass(name=name)

        print 'ToolSetter, setting lhs ', node.children[0].tool
        tool.lhs = node.children[0].tool
        tool.rhs = node.children[1].tool
        node.tool = tool


    def mod_logical_unary(self, node):
        """Set the HypoConfigTool instance for the 'not' scenario
        this takes a single predicate"""
        
        scen = node.scenario
        klass = self.tool_factories[scen][0]
        sn = self.tool_factories[scen][1]
        name = '%s_%d' % (scen, sn)
        self.tool_factories[scen][1] += 1

        # kludgy. The name of the tool seen by the trigger must be
        # the trigger name, so have to figure  out if this is the top
        # level node (actually first daughter, as the start node is the top)
        # note that the name can only be set once so have to know
        # if we are the top of the tree while traversing it. kludgy...
        # also - will break when using a forest...

        print 'Toolsetter, node.tree_top', node.tree_top
        if node.tree_top:
            tool = klass(name=self.chain_name)
        else:
            tool = klass(name=name)

        print 'ToolSetter, setting lhs ', node.children[0].tool
        tool.hypoTool = node.children[0].tool
        node.tool = tool
                               

                               
    def mod_simple(self, node):
        """Set the HypoConfigTool instance in a hypo tree node"""

        scen = node.scenario
        klass = self.tool_factories[scen][0]
        sn = self.tool_factories[scen][1]
        name = '%s_%d' % (scen, sn)
        
        self.tool_factories[scen][1] += 1

        config_tool = klass(name=name+'_config')
        [setattr(config_tool, k, v) for k, v in node.conf_attrs.items()]
        
        # kludgy. The name of the tool seen by the trigger must be
        # the trigger name, so have to figure  out if this is the top
        # level node (actually first daughter, as the start node is the top)
        # note that the name can only be set once so have to know
        # if we are the top of the tree while traversing it. kludgy...
        # also - will break when using a forest...
        print 'Toolsetter, node.tree_top', node.tree_top
        if node.tree_top:
            tool = TrigJetHypoToolMT(name=self.chain_name)
        else:
            tool = TrigJetHypoToolMT(name=name)
            
        tool.HypoConfigurer = config_tool
        tool.dumpJets  = self.dump_jets
        node.tool = tool
                               
    def mod(self, node):
        """Set the HypoConfigTool instance according to the scenario.
        Note: node.accept must perform depth first navigation to ensure
        child tools are set."""
        
        self.mod_router.get(node.scenario, self.mod_simple)(node)

    def report(self):
        wid = max(len(k) for k in self.tool_factories.keys())
        rep = '\n%s: ' % self.__class__.__name__

        rep += '\n'.join(
            ['%s: %d' % (k.ljust(wid), v[1])
             for k, v in self.tool_factories.items()])

        return rep
