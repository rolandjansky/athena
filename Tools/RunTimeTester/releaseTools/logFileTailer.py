# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from listFiles import listFiles
import os.path

class LogFileTailer:
    def __init__(self, nlines, tailFile):
        self.nlines = nlines
        self.tailFile = open(tailFile, 'w')
        self.ncalls = 0
        
    def tail(self,f):
        self.ncalls += 1
        lines = open(f,'r').readlines()
        start = 0
        if len(lines)>self.nlines: start = -self.nlines
        self.tailFile.write('\n\n========== %s ========\n' % f)
        [self.tailFile.write(l) for l in lines[start:]]

    def finish(self): self.tailFile.close()
        
# root = '/afs/cern.ch/atlas/project/RTT/Work/rel_1/dev/build/i686-slc3-gcc323-opt'
def getFiles1():
    return 


nlines = 50
print 'looking for files'
logfiles = listFiles(root, '*_log')
print 'Found %d log files' % len(logfiles)

ofn = '/afs/cern.ch/user/r/rtt/Scratch/logFileTails.txt'
checker = LogFileTailer(nlines, ofn) 

#logfiles = listFiles(root, '*_log')
logfiles = listFiles(root, '*')
[checker.tail(f) for f in logfiles]

checker.finish()


/afs/cern.ch/atlas/project/RTT/prod/Peter/logs/logfiles_8feb_18:28
