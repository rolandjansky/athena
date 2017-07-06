#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os,sys
from get_release_2017 import (fn2dt, _get_release)

__author__ = 'Renaud Bruneliere <Renaud.Bruneliere@cern.ch>'
__doc__    = 'Call compareTCTs.py from Walter Lampl available in Tools/PROCTools.'

## main
if __name__ == "__main__":


    try:  
       os.environ["AtlasBuildBranch"]
    except KeyError: 
       print "Please set the environment variable AtlasBuildBranch"
       sys.exit(1)

    try:  
       os.environ["AtlasBuildStamp"]
    except KeyError: 
       print "Please set the environment variable AtlasBuildStamp"
       sys.exit(1)

    release = os.environ["AtlasBuildBranch"]
    timeStamp = os.environ["AtlasBuildStamp"]
    nightly =  _get_release(fn2dt(timeStamp))

    if nightly == 'rel_0':
        prevnightly = 'rel_6'
    else:
        prevnightly = 'rel_%d' % (int(nightly.split('_')[1])-1)
        pass

    print ('tct_getESDTAGcomparison.py : compare %s,%s with %s,%s' % (release,prevnightly,release,nightly))
    cmd = 'compareTCTs.py --nightly=%s --rRef=%s --rVal=%s --sum=summaryESDTAGcomp.txt --diff-root=True' % (release,prevnightly,nightly)
    print "Actual command (useful for reproducing locally):\n\n%s\n" % cmd
    stat=os.system(cmd)
    if (stat==0):
        print "ErrorCode=0 (OK)"
    else:
        print "ErrorCode=",stat
        sys.exit(stat)
    pass
