#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os

__author__ = 'Renaud Bruneliere <Renaud.Bruneliere@cern.ch>'
__doc__    = 'Call compareTCTs.py from Walter Lampl available in Tools/PROCTools.'

## main
if __name__ == "__main__":

    release = os.environ["AtlasArea"]
    releaseArr = release.split("/")
    release = "%s %s %s" % (releaseArr[7], releaseArr[8], releaseArr[9])
    if releaseArr[9] == 'rel_0':
        prevrelease = 'rel_6'
    else:
        prevrelease = 'rel_%d' % (int(releaseArr[9].split('_')[1])-1)
        pass

    print ('tct_getESDTAGcomparison.py : compare %s,%s with %s,%s' % (releaseArr[7],prevrelease,releaseArr[7],releaseArr[9]))
    os.system('compareTCTs.py --nightly=%s --rRef=%s --rVal=%s --sum=summaryESDTAGcomp.txt' % (releaseArr[7],prevrelease,releaseArr[9]))
    print "ErrorCode=0 (OK)"
    pass
