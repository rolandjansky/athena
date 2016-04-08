# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from listFiles import listFiles

import sys
import os
import getopt
import string
import xml.sax
import pprint
from xml.sax.handler import ContentHandler

class KFHandler(ContentHandler):
    def __init__(self):
        self.kfiles = []
        self.enable = False
    def startElement(self, name, attr):
        # print 'handling tag', name
        if name == 'keepfile_newstyle':
            self.enable = True
    def endElement(self, name):
            self.enable = False
    def characters(self, ch):
        if not self.enable:return
        ch = string.strip(ch)
        if len(ch)>0: self.kfiles.append(ch)
        # if len(ch)>0: self.kfiles.append(os.path.basename(ch))
    
def optusage():
    print 'usage: python keepFileDiff -d [path to dir with RTTSummaryReference.xml]'
    
def getSuiteDir():
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'd:')
    except getopt.GetoptError:
        optuasge()
        sys.exit(2)

    for o,a in opts:
        if o == '-d': return a
        
    optusage()
    sys.exit(2)

def getKeepFiles(fn):
    lines = open(fn, 'r').readlines()
    lines = [string.strip(l) for l in lines if l.find('<keepfile') != -1]
    
    print 'getting keep files from file ', fn

    return lines

def doit():
    cwd = os.getcwd()
    sdir = os.path.join(cwd, getSuiteDir())
    if not os.path.exists(sdir):
        print 'Non -existant test suite directory ', sdir
        sys.exit(0)
    rfile = os.path.join(sdir, 'RTTSummaryReference.xml')
    if not os.path.exists(os.path.join(sdir,rfile)):
        print 'No reference file ', rfile
        sys.exit(0)

    parser = xml.sax.make_parser()
    handler = KFHandler()
    parser.setContentHandler(handler)
    parser.parse(rfile)
    rkfiles = handler.kfiles

    lfiles = listFiles(sdir, 'RTTSummary.xml')
    if len(lfiles) != 1:
        print 'error looking for RTTSummaryFile', lfiles
        sys.exit(0)
        
    handler = KFHandler()
    parser.setContentHandler(handler)
    parser.parse(lfiles[0])
    skfiles = handler.kfiles
    
    diff = [f for f in rkfiles if f not in skfiles]
    pp = pprint.PrettyPrinter()
    print
    print ' in Reference and not in Summary'
    pp.pprint(diff)

    diff = [f for f in skfiles if f not in rkfiles]
    print
    print ' in Summary and not in Reference'
    pp.pprint(diff)
    
doit()
