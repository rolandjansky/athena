# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from listFiles import listFiles
import os.path, sys

def stripstr(s):
    s.strip()
    s = s[:-1]
    return s
class Checker:
    def __init__(self, f, strings):
        self.strings = strings
        self.f = f
        self.dict = {}
        self.fdict = {}
        for s in self.strings:
            self.dict[s]=[]

        self.ncalls = 0
        self.checkForStrings()
        
        
    def checkForStrings(self):
        self.ncalls += 1
        lines = open(self.f,'r').readlines()
        for s in self.strings:
            for l in lines:
                # print "\n looking for:\n%s\n in line\n %s" %(s,l)
                if l.find(s)>= 0:
                    self.dict[s].append(l)

    def __str__(self):
        m = '\n File: %s' % self.f

        nostrings = True
        for s in self.strings:
            if self.dict[s]:
                nostrings = False

        if nostrings:
            m = 'File: %s: no strings found' % self.f
            return m
        

        m = '\n File: %s\n' % self.f
            
        
        
        for s in self.strings:
            if self.dict[s]:
                m += '\n   string: %s\n' %  s
                for l in self.dict[s]:
                    m += '      %s' % l

        return m

    def dump(self):
        print self

config = open(sys.argv[1], 'r')
lines  = config.readlines()
config.close()

toRemove = [l for l in lines if l.startswith('#')]
[lines.remove(l) for l in toRemove]
lines = [stripstr(l) for l in lines]
toRemove = [s for s in lines if len(s)==0]
[lines.remove(l) for l in toRemove]
root = lines[0]
globpar = lines[1]
strings = lines[2:]


print 'looking for files below', root,'matching',globpar
logfiles = listFiles(root, globpar)
print 'Found %d files' % len(logfiles)


checkers = [Checker(f, strings).dump() for f in logfiles]

