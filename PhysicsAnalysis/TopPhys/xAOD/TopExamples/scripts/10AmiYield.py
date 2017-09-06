#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import TopExamples.ami
import TopExamples.logger as logger
import sys

if len(sys.argv) != 2:
    print 'Use like this:'
    print 'python %s %s' % (sys.argv[0], 'mc15_13TeV.410000.*.AOD.e3698_s2608_s2183_r6630_r6264')
    print sys.exit(1)

nameYields = TopExamples.ami.askAmi(sys.argv[1])

if len(nameYields) > 0:
    print logger.OKBLUE + '%80s %10s' % ('Dataset name', 'Yield') + logger.ENDC
    for k in sorted(nameYields.keys()):
        print '%80s %10d' % (k, nameYields[k])
else:
    print 'No datasets found'