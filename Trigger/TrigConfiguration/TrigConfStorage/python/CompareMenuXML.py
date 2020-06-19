#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import sys

from xml.dom import minidom
from xml.dom import expatbuilder
from past.builtins import cmp

class CompareMenuXML(object):
    def __init__(self, **args):
        # collected printout
        self.textoutput = ""

        self.uniqID = {}

        # elements that need to print information about the parent when printed
        self.needParentInfo = []

        # elements for which the information is stored as #PCDATA content
        self.checkText = []

        # elements that don't need to be checked
        self.exclFromCmpList = []

        # attributes that don't need to be checked 
        self.ignoreAttr = {}

        # verbosity level
        self.verboseLevel = 0
        if 'verbose' in args:
            self.verboseLevel = args['verbose']

        self.exlusionset = 'default'
        if 'excl' in args:
            self.exlusionset = args['excl']


    def write(self,line):
        print (line, end='')
        self.textoutput += line + "<BR>"


        
    def parseFile(self,file):
        builder = expatbuilder.ExpatBuilder()
        fp = open(file, 'rb')
        buff = fp.read().replace('&','&amp;')
        return builder.parseString(buff)



    def diff(self, doc1, doc2):
        self.uniqID.update({'document' : 'single'})
        self.addContextToNode(doc1, "")
        self.addContextToNode(doc2, "")
        return self.comp(doc1, doc2)

    def addContextToNode(self,node,parentContext):
        node.context = parentContext + "." + node.nodeName
        #print >> self, "Node %s with context %s" % (node.nodeName,node.context)
        node.compfield = self.getCompFieldFromContext(node)
        for c in node.childNodes:
            self.addContextToNode(c, node.context)


    def comp(self, node1, node2):
        if self.isOneMissing(node1, node2): return False

        equal = True
        if self.verboseLevel>0:
            if node1.compfield == 'single':
                print ("compare single %s with %s in context %s" % (node1.nodeName,node2.nodeName, node1.context), file=self)
            else:
                val1 = node1.attributes[node1.compfield].value
                val2 = node1.attributes[node2.compfield].value
                print ("compare %s (%s) with %s (%s) in context %s" % (node1.nodeName, val1, node2.nodeName, val2, node1.context), file=self)
        # first compare the node itself
        if node1.nodeType==minidom.Node.TEXT_NODE:
            # CDATA
            equal &= self.compCData(node1,node2)
        else:
            # or ELEMENT with attributes
            equal &= self.compAttr(node1,node2)

        # then compare the children
        if node1.nodeName in self.checkText:
            childrenType = minidom.Node.TEXT_NODE
        else:
            childrenType = minidom.Node.ELEMENT_NODE
            
        pairedElems = self.orderChildren(node1.childNodes, node2.childNodes, childrenType)
        for (l,r) in pairedElems:
            equal &= self.comp( l, r )
        return equal


    def getCompFieldFromContext(self, node):
        if node.nodeName == "#text" or node.nodeName == "#comment":
            return "text"
        compField = None
        lookfor = node.context
        for k,f in self.uniqID.items():
            if lookfor.endswith(k):
                compField = f
                break

        if not compField:
            print ("Don't know how to compare a node of type %s in context %s (was looking for a rule about %s), will abort" % ( node.nodeName, node.context, lookfor ), file=self)
            sys.exit(0)

        return compField



    def cmpNode(self, node1, node2, reqUniq=False):
        res = cmp(node1.nodeName,node2.nodeName)
        if res!=0: return res

        if node1.nodeType==minidom.Node.TEXT_NODE:
            return cmp(node1.data,node2.data)

        if node1.compfield=='single': return 0
        val1 = node1.attributes[node1.compfield].value
        val2 = node2.attributes[node2.compfield].value
        if val1<val2: return -1
        if val1>val2: return 1
        if reqUniq:
            raise RuntimeError ("Two equal nodes %s found with %s = %s and  %s = %s" % (node1.nodeName, node1.compfield, val1, node2.compfield, val2 ))
        else:
            return 0


    
    def isOneMissing(self, node1, node2):
        if not (node1 and node2):
            if not node1:
                eNode = node2
                eMenu = 2
            else:
                eNode = node1
                eMenu = 1

            parNode = eNode
            while parNode.nodeName in self.needParentInfo:
                parNode = parNode.parentNode

            if eNode.nodeName not in self.uniqID:
                print ("Don't know how to compare two items of type %s , will abort" % eNode.nodeName, file=self)
                sys.exit(0)

            if self.uniqID[eNode.nodeName] == 'single':
                print ("DIFF MISS: %s" % (eNode.nodeName), end='', file=self)
            else:
                print ("DIFF MISS: %s with %s %s" % (eNode.nodeName, self.uniqID[eNode.nodeName], eNode.attributes[self.uniqID[eNode.nodeName]].value), end='', file=self)
                
            if parNode != eNode:
                print ("[from %s %s]" % (parNode.nodeName, parNode.attributes[self.uniqID[parNode.nodeName]].value), end='', file=self)
            print ("appears only in menu %i" % (eMenu), file=self)
            return True
        return False



    def compCData(self, node1, node2):
        data1 = node1.data.strip()
        data2 = node2.data.strip()
        equal = data1==data2
        if not equal:
            par1 = node1.parentNode
            print ("DIFF DATA: %s with %s %s: different (%s != %s)" % (par1.nodeName, self.uniqID[par1.nodeName], par1.attributes[self.uniqID[par1.nodeName]].value, data1, data2), file=self)
        return equal



    def compAttr(self, node1, node2):
        if node1.nodeName!=node2.nodeName:
            raise RuntimeError ("Comparing attributes of two different element classes %s and  %s" % (node1.nodeName, node2.nodeName))
        if self.verboseLevel>2:
            if node1.attributes: print ("Attribute 1",node1.attributes.items())
            if node2.attributes: print ("Attribute 2",node2.attributes.items())
        nodeName = node1.nodeName
        equal = True
        allAttr = []
        ignorelist = []
        if node1.nodeName in self.ignoreAttr: ignorelist = self.ignoreAttr[nodeName]
        if node1.attributes: allAttr += [x for x in node1.attributes.keys() if x not in ignorelist]
        if node2.attributes: allAttr += [x for x in node2.attributes.keys() if not (x in allAttr or x in ignorelist)]
        if self.verboseLevel>1:
            print ("Attributes of %s:" % node1.nodeName, allAttr, file=self)
        for a in allAttr:
            if (node1.attributes is None) or (a not in node1.attributes.keys()):
                print ("DIFF ATTR: %s: attribute '%s' exists only in document file 2 (%s)" % (nodeName, str(a), node2.attributes[a].value), file=self)
                equal=False
                continue
            if (node2.attributes is None) or (a not in node2.attributes.keys()):
                print ("DIFF ATTR: %s: attribute '%s' exists only in document file 1 (%s)" % (nodeName, str(a), node1.attributes[a].value), file=self)
                equal=False
                continue
            if self.verboseLevel>2:
                print ("Attribute %s : %s  vs. %s" % (a, node1.attributes[a].value, node2.attributes[a].value))
            if node1.attributes[a].value != node2.attributes[a].value:
                if node1.compfield == 'single':
                    print ("DIFF ATTR: %s: different '%s' (%s != %s)" % (nodeName,str(a), node1.attributes[a].value, node2.attributes[a].value), file=self)
                else:
                    print ("DIFF ATTR: %s %s: different '%s' (%s != %s)" % (nodeName, node1.attributes[node1.compfield].value, str(a), node1.attributes[a].value, node2.attributes[a].value), file=self)
                equal=False
                
        return equal



    def orderChildren(self, children1, children2, childrenType):
        """takes two list of childrens, sorts them, and builds matching pairs. If a child appears
        in only one of the lists a pair (x,None) or (None,x) will be created. A list of all pairs
        is returned"""
        red1 = [e for e in children1 if e.nodeType==childrenType and e.nodeName not in self.exclFromCmpList]
        red1.sort(self.cmpNode)
        red2 = [e for e in children2 if e.nodeType==childrenType and e.nodeName not in self.exclFromCmpList]
        red2.sort(self.cmpNode)
        c1 = c2 = 0
        merged = []
        while c1<len(red1) and c2<len(red2):
            cmpnodes = self.cmpNode(red1[c1],red2[c2])
            if cmpnodes==0 or childrenType==minidom.Node.TEXT_NODE:
                merged += [(red1[c1],red2[c2])]
                c1 += 1
                c2 += 1
            elif cmpnodes<0:
                merged += [(red1[c1],None)]
                c1 += 1
            elif cmpnodes>0:
                merged += [(None,red2[c2])]
                c2 += 1
        while c1<len(red1):
            merged += [(red1[c1],None)]
            c1 += 1
        while c2<len(red2):
            merged += [(None,red2[c2])]
            c2 += 1
        return merged



