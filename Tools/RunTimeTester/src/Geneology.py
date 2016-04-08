# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Base class dealing with geanelogy of tree structures
"""
# class Logger:
#     def __init__(self):
#         pass
#     def critical(self, m):
#         print str(m)
#     def error(self, m):
#         print str(m)
# 
# logger = Logger();
# 
#
from copy import copy


from formatCollection import formatCollection
from Tools            import unique
from RTTSException    import RTTCodingError

# -------------------------------------------------------------------------
import logging
logger = logging.getLogger('rtt')
# -------------------------------------------------------------------------

class Tree:
    def __init__(self, nodes, topLabel=None):

        logger.info('no. of nodes on entering Tree constructer = %d',len(nodes))
        self.nodeDict       = {}
        self.removedNodes   = []
        self.topLabel       = 'Top'
        self.topParentLabel = 'TopParent'

        self.sanity(nodes)
        
        for n in nodes: self.nodeDict[str(n.myLabel())]=n
        self.nodeDict[self.topParentLabel] = None
        self.lineage(nodes)

    def nodeString(self, nodes):
        def concoctString(node):
            return 'parent: '+n.parentLabel()+' group: '+ n.myLabel()+'\n'
        temp = [concoctString(n) for n in nodes]
        return reduce(lambda x,y:x+y, temp, ' ')
    
    def sanity(self, nodes):
        m = 'Geneology sanity check: nodes have:' + self.nodeString(nodes)
        logger.debug(m)
        self.areNodes(nodes)
        self.removeDuplicates(nodes)
        self.parentsPresent(nodes)
        self.hasTop(nodes)

    def areNodes(self, nodes):
        nonNodes = [n for n in nodes if not self.isNode(n)]
        for n in nonNodes:
            logger.error('removing non Node %s from Tree' % str(n))
            nodes.remove(n)

    def removeDuplicates(self, nodes):
        multiplicity = {}
        [multiplicity.setdefault(n.myLabel(), []).append(n) for n in nodes]

        for label in multiplicity.keys():
            nDup = len(multiplicity[label])
            if nDup >1:
                msg  = 'removing %d nodes with '
                msg += 'duplicate label %s' % (nDup, label)
                logger.error(msg)
                [nodes.remove(n) for n in multiplicity[label]]


    def parentsPresent(self, nodes):
        """
        remove a node if its parent is not present.
        if >= 1 node is removed, re-call the method.
        """
        
        labels = [n.myLabel() for n in nodes]
        parentMissing = [n for n in nodes if n.parentLabel() not in labels]
        tops = [n for n in parentMissing if n.myLabel() == self.topLabel]
        [parentMissing.remove(t) for t in tops]
        if parentMissing:
            [nodes.remove(n) for n in parentMissing]
            self.removedNodes.extend(parentMissing)
            msg  = 'No parents for the following nodes; the nodes have been removed\n'
            msg += self.nodeString(parentMissing)
            logger.debug(msg)
            self.parentsPresent(nodes)
            
    def hasTop(self, nodes):

        tops = [n for n in nodes if n.myLabel() == self.topLabel]

        assert (len(tops)<2)

        if len(tops) == 0:
            msg  = 'No top node in Tree, raising exception\n'
            msg += 'top node label %s \n' % self.topLabel
            msg += formatCollection([n.myLabel() for n in nodes])
            logger.error(msg)
            raise RTTCodingError(msg)

        top = tops[0]
        top.parent = self.topParentLabel

    def lineage(self, nodes):

        for n in nodes:

            # do not remove the top node
            if n.myLabel() == self.topLabel: continue
            
            aLabels =[a.myLabel() for a in self.ancestors(n)]
            if self.topLabel not in aLabels:
                msg  = 'removing node %s ' % n.myLabel()
                msg += 'which cannot be traced back to the top node'
                msg += 'ancestors are:\n%s\n ' % (formatCollection(aLabels))
                msg += 'top label is: %s' % self.topLabel
                logger.error(msg)
                nodes.remove(n)

                

    def nNodes(self):
        return len(self.nodeDict.keys())

    def nodeWasRemoved(self,nodeName):
        for rn in self.removedNodes:
            if nodeName==rn.myLabel():
                return True
        return False
        
    def getNodeNames(self):
        return self.nodeDict.keys()

    def getNode(self, nodeName):

        node = self.nodeDict.get(nodeName, None)
        if node == None  and nodeName != self.topParentLabel:
            if self.nodeWasRemoved(nodeName):
                msg = 'Unknown node: %s was removed because it had no known parent node.' % nodeName
            else:
                msg =  'Unknown node:'
                msg +=' %s\n possible nodes %s:' % (nodeName,
                                                    str(self.nodeDict.keys()))
            logger.error(msg)
        return node

    def isNode(self, node):
        try:
            parent = node.parentLabel()
            me     = node.myLabel()
        except:
            logger.error(str(node)+' is not a node object')
            return False
        return True
    
    def ancestors(self, nnode):
        """
        returns a list of Nodes which are parents of the Node node.
        Stops at the Top node if this an ancestor
        """
        
        ancestors          = []
        node               = copy(nnode)
        label              = node.myLabel()

        #if passed node is the top node, return it.
        if label == self.topLabel: return [node]
        
        while label != self.topParentLabel:

            ancestors.append(node)
            node = self.nodeDict[node.parentLabel()]
            label = node.myLabel()
            if label == self.topLabel:
                ancestors.append(node)
                break


        # ancestors.remove(self.nodeDict[self.topLabel])
        return ancestors

    def getAllNodes(self):
        nodes = []

        keys = self.nodeDict.keys()
        if self.topParentLabel in keys: keys.remove(self.topParentLabel)

        [nodes.append(self.nodeDict[k]) for k in keys]

        return nodes 


class KitTree(Tree):
    def __init__(self, ourKits, theirKits=[]):

        self.removeOverlaps(ourKits, theirKits)
        self.removeDuplicates(ourKits)
        self.removeDuplicates(theirKits)

        kits = copy(ourKits)
        kits.extend(theirKits)
        


        topParent   = str(None)
        Tree.__init__(self, kits, topParent)
        self.propagate()

    def propagate(self):
        """
        propagate parent kit to child kit down the tree for each generation
        """
        generations = {}
        nodes = self.getAllNodes()

        for n in nodes:
            nGen = len(self.ancestors(n))
            generations.setdefault(nGen, []).append(n)

        nGen = range(1, max(generations.keys())+1)
        for gen in nGen:
            thisGeneration = generations[gen]
            for node in thisGeneration:
                parentNode = self.nodeDict[node.parentLabel()]
                #add the parent is is not the top parent
                if parentNode: node.add(parentNode)

    def getKit(self, label):
        return self.getNode(label)
    
    def removeOverlaps(self, on, tn):
        onLabels = [n.myLabel() for n in on]
        overlap  = [n for n in tn if n.myLabel() in onLabels]
        if len(overlap)>1:
            msg  = 'the following nodes overlap with class A nodes'
            msg += ' and have been removed'
            logger.error(msg)
            msg = ''
            for n in overlap: msg+=n.myLabel()+' '
            logger.error(msg)
            [tn.remove(n) for n in overlap]
        
    def removeDuplicates(self, nodes):
        """
        Identify kits with the same lable. Remove all such kits.
        """
        dict = {}
        [dict.setdefault(n.myLabel(),[]).append(n)  for n in nodes]

        for label in dict.keys():
            list = dict[label]
            if len(list)>1:
                testKit = list[0]
                for kit in list:
                    if not testKit.equivalent(kit):
                        msg  = 'Node with label %s has non-equivalent '% label
                        msg += 'duplicates:  removing all such nodes'
                        logger.error(msg)
                        [nodes.remove(n) for n in list]
                        break
        
    def getKitNames(self):
        return self.getNodeNames()
                       
            
    def nKits(self): return self.nNodes()
    def getAllKits(self): return self.getAllNodes()
class TreeMember:
    def __init__(self, name, parent):
        self.name       = name
        self.parent     = parent
        self.parentNode = None
    def parentLabel(self):
        return str(self.parent)
    def myLabel(self):
        return str(self.name)
    def setParent(self, node): self.parentNode = node
    def __str__(self):
        msg  = 'Tree Member: parent = '+self.parentLabel()
        msg += ' me = '+self.myLabel()
        return msg  

class A(TreeMember):
    def __init__(self, name, parent):
        TreeMember.__init__(self, name, parent)
            


def fill(table):
    nodes = []
    for item in table.items():
        nodes.append(A(item[0], item[1]))

    return nodes

def TreeTest(logger):
    table= {
        'Top': 'TopParent',
        'Athena': 'Top',
        }


    nodes = fill(table)    
    tree = Tree(nodes)
    
    for n in [m.myLabel() for m in nodes]:
        logger.debug('node '+n)
        if not tree.getNode(n):
            logger.debug ('  error retrieving node '+str(n))
            

    if tree.nNodes() != 2:
        m = 'Expected 2 nodes, found %d' % tree.nNodes()
        return False


    table = {
        'c': 'Athena',
        'd': 'Top',
        'e': 'd'
        }


    nodes = fill(table)
    tree = Tree(nodes)

    if tree.nNodes() != 2:
        m = 'Expected 2 nodes, found %d' % tree.nNodes()
        return False



    table= {
        'Top': 'TopParent',
        'Athena': 'Top',
        'omega':  'chi'
        }

    nodes = fill(table)
    tree = Tree(nodes)

    if tree.nNodes() != 2:
        m = 'Expected 2 nodes, found %d' % tree.nNodes()
        return False


    return True

def KitTest(logger, kits):
    tree = KitTree(kits)

    kits = tree.getAllKits()
    for k in kits:
        if k.myLabel() == 'Top': print k

    cur = 0
    for k in kits:
        cur += 1
        logger.debug( '-------------%d-------------', cur)
        logger.debug(str(k))

if __name__ == "__main__":
    pass
    
