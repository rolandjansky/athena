# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from listFiles import listFiles
from parseHelpers import getText
from parseHelpers import getSingleNodeText

import unittest
import xml.dom.minidom
import difflib
import pprint
import string
import os

def addCR(l): return l+'\n'
def toLines(cList):
    line = ''
    for c in cList: line += c
    
    lines = line.split('\n')
    lines = [l.strip() for l in lines]
    lines = [l for l in lines if l != '']
    lines = [addCR(l)  for l in lines]

    return lines

def pruneDom(tNames, dom):
        for t in tNames:
            nodes = dom.getElementsByTagName(t)
            for n in nodes:
                parentNode = n.parentNode
                # print 'removing child %s from parent %s' % (n.nodeName, parentNode.nodeName)
                parentNode.removeChild(n)
                n.unlink()
        return dom
    
def fnsFromNodes(nodes):
    # return a list of file names from a sequence of nodes
    # constaining paths, with duplicates removed
    names = {}
    for n in nodes:
        fn = string.strip(getText(n))
        if len(fn) == 0: continue 
        names[os.path.basename(fn)] = 1

    return names.keys()
    

class RTTTestCase(unittest.TestCase):
    
    def setUp(self):
        if 'dom' in self.__dict__.keys(): return
        infileName = listFiles('.', 'RTTSummary.xml')[0]
        infile = open(infileName, 'r')

        refFile = open('RTTSummaryReference.xml')

        self.dom     = xml.dom.minidom.parseString(infile.read())
        self.refDom  = xml.dom.minidom.parseString(refFile.read())

        self.logfile = open('TestCase.log','w')

        
    def rttTestNTags(self, node,  tag, n):
        self.assert_(len(node.getElementsByTagName(tag)) == n)

    def rttTestCheckTagContent(self, tag, content):
        text =  getSingleNodeText(self.dom, tag)
        self.assert_(text.find(content)>-1)

    def rttTestNoErrorReport(self):
        enodes = self.dom.getElementsByTagName('errors')
        if len(enodes)>0:
            for node in enodes:
                self.assert_(len(getText(node)) == 0)

    def rttKeepfileDiff(self):
        tag = 'keepfile_newstyle'
        skfiles = fnsFromNodes(self.dom.getElementsByTagName(tag))
        rkfiles = fnsFromNodes(self.refDom.getElementsByTagName(tag))

        mismatch = [s for s in skfiles if s not in rkfiles]
        if(mismatch): self.logfile.write('\nkeepfile in summary and not in ref\n%s\n' % str(mismatch))
        self.assert_(len(mismatch)==0)

        mismatch = [s for s in rkfiles if s not in skfiles]
        if(mismatch): self.logfile.write('\nkeepfile in ref and not in summary\n%s\n' % str( mismatch))
        self.assert_(len(mismatch)==0)

    def variableContentTags(self):
        return ['statusText',
                'latestUpdateTime1',
                'latestUpdateTime2',
                'endTime1',
                'endTime2',
                'startTime1',
                'startTime2',
                'nicosDate1',
                'nicosDate2',
                'stateHistory',
                'wallTime',
                'mem',
                'vmem',
                'cpuTime',
                'resBaseDir',
                'resReleaseDir',
                'resultsPath',
                'cpuTime2000',
                'keepfile_newstyle',
                'baseKeepFileDir',
                'RTTLibVersion',
                'RTTSrcVersion'
                ]

    def rttTestXMLComp(self):

        tagsToPrune = self.variableContentTags()

        prunedDom    = pruneDom(tagsToPrune, self.dom)
        prunedRefDom = pruneDom(tagsToPrune, self.refDom)

        newxml = toLines(prunedDom.toxml())
        refxml = toLines(prunedRefDom.toxml())

        d = difflib.Differ()
        difflist = list(d.compare(newxml, refxml) )
        difflist = [d for d in difflist if not d[0] == ' ']
        pprint.pprint(difflist, self.logfile)

        self.assert_(not difflist)
