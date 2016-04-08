# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Taken from Lutz, programming python 2nd edition p 211.

The classes in the module wrap os.path.walk

The classes in this module are to be overridden to obtain desired behaviour
"""

import os, sys, string
listonly = 0

class FileVisitor:
    """
    visits all non-directory files below startDir;
    overide visitfile to provide a file handler;
    """

    def __init__(self,data=None, listonly=0):

        self.context  = data
        self.fcount   = 0
        self.dcount   = 0

        self.listonly = listonly

    def run(self, startDir=os.curdir):
        os.path.walk(startDir, self.visitor, None)

    def visitor(self, data, dirName, filesInDir):
        
        self.visitDir(dirName)
        for fname in filesInDir:
            fpath = os.path.join(dirName, fname)
            if not os.path.isdir(fpath):
                self.visitfile(fpath)
                
    def visitDir(self, dirPath):
        self.dcount += 1
        print dirPath,'...'

    def visitfile(self, filepath):
        self.fcount += 1
        print self.fcount, '=>', filepath

class SearchVisitor (FileVisitor):

    "search files at and below startDir for a string"

    skipexts = ['.gif', '.exe', '.pyc', '.o', '.a']

    def __init__(self, key, listonly=0):

        FileVisitor.__init__(self)
        self.scount = 0

    def visitfile(self, fname):
        FileVisitor.visitfile(self, fname)
        if not self.listonly:
            if os.path.splitext(fname)[1] in self.skipexts:
                print 'skipping', fname
            else:
                text = open(fname).read()
                if string.find(text, self.context) != -1:
                    self.visitmatch(fname, text)
                    self.scount += 1

    def visitmatch(self, fname, text):
        raw_input('%s has %s' % (fname, self.context))

# self-test logic

dolist   = 1
dosearch = 2
donext   = 4

def selftest(testmask):
    if testmask & dolist:
        visitor=FileVisitor()
        visitor.run('.')
        print 'Visited %d files and %d directories' % (visitor.fcount,
                                                       visitor.dcount)

if __name__ == '__main__':

    selftest(int(sys.argv[1]))
            
        
