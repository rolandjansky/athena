from constants import logicals
    
class Node(object):
    
    def __init__(self, scenario):
        self.scenario = scenario
        self.parameters = ''
        self.children = []
        self.conf_attrs = {}
        self.tool = None

        # self.tree_top kludge carensure top level tools get chain name
        # as Tool name
        self.tree_top = False

    def add_child(self, c):
        self.children.append(c)
        
    def accept(self, modifier):
        "call self before children"

        modifier.mod(self)
        for c in self.children:
            c.accept(modifier)

    def accept_cf(self, modifier):
        "call children before self"
        
        for c in self.children:
            c.accept_cf(modifier)
        modifier.mod(self)

    def makeTool(self):
        if self.scenario not in logicals:
            self.tool = DummyTool(self.toolName)
            
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
            

    
        return s
        
    def dump(self, n_in=0):
        indent = ' '*n_in
        s = '\n'
        s =  indent + 'Node. scenario: %s \n' % self.scenario
        s +=  indent + 'is tree top? %s \n' % self.tree_top
        s += indent + 'parameters: %s\n' % str(self.parameters)
        s += indent + 'conf_attrs: %s\n' % str(self.conf_attrs)
        s += indent + 'AlgTool: %s\n' % str(self.tool)
        # s += indent + 'AlgTool dict: %s\n' % str(self.tool.__dict__)
        s += indent + 'No of children: %d\n\n' % len(self.children)
        for c in self.children:
            s += c.dump(n_in+5)

        return s
        
