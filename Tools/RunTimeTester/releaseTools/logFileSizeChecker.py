# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from listFiles import listFiles
import os

class Checker:
    def __init__(self):
        self.dict    = {}
        self.sumsize = 0
        self.ncalls  = 0
        
    def checkSize(self,f):
        self.ncalls += 1
        size = int((os.stat(f)).st_size)
        self.dict[os.path.basename(f)] = size
        self.sumsize += size

root = '/afs/cern.ch/atlas/project/RTT/Work/rel_4/dev/build/i686-slc4-gcc34-opt'

print 'looking for files in %s' % root
logfiles = listFiles(root, '*_log')
print 'Found %d log files' % len(logfiles)

checker = Checker()

[checker.checkSize(f) for f in logfiles]

print 'Root directory = ', root
print '\nlog file sizes:'

for s in checker.dict.items():
    print '%10s   %d' % s

print 'log files below:', root
print 'No of files:',len(checker.dict.keys())
print 'Total size (kbytes):',checker.sumsize/1024.


