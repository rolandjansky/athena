#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Run beam spot determination on a set of ESD files using JobRunner
#
# Written by Juerg Beringer in July 2008.

import sys
import glob

if len(sys.argv)!=3:
    print "Usage: checkJobs runnr name"
    exit(1)
runnr = sys.argv[1]
name = sys.argv[2]
prefix = runnr+'/'+name+'/'+'*/*-status.'
prefixexit = runnr+'/'+name+'/'+'*/*-exit.'

submitted = len(glob.glob(prefix+'SUBMITTED'))
running = len(glob.glob(prefix+'RUNNING'))
completed = len(glob.glob(prefix+'COMPLETED'))

print '%4i  jobs SUBMITTED' % submitted
print '%4i  jobs RUNNING' % running
print '%4i  jobs COMPLETED' % completed
print '%4i  jobs with exit status 0' % len(glob.glob(prefixexit+'0'))
