#!/bin/env python2.5
#
# ----------------------------------------------------------------
# Script : subproc.py
# Project: AtlRunQuery
# Purpose: Executable for python subprocesses
# Authors: Andreas Hoecker (CERN), Joerg Stelzer (DESY)
# Created: Nov 12, 2009
# ----------------------------------------------------------------
#

import subprocess
import time
import sys
import os.path

if __name__ == '__main__':

    subtype = 'caf'
    argstr  = ''
    if len(sys.argv)>1: subtype = sys.argv[1]
    if len(sys.argv)>2: argstr  = sys.argv[2]

    # start python subprocess to find files on CAF ---------
    # first check age of reference file, reload if older than 1d
    if subtype == 'caf':
        reffile = 'data/atlas.atlcal.last'
        if not os.path.isfile(reffile) or (time.time()-os.path.getmtime(reffile))/3600.>24.:
            subprocess.Popen('wget -o data/wget.log http://castor.web.cern.ch/castor/DiskPoolDump/atlas.atlcal.last -O %s' % (reffile), shell=True).wait()
                                    
        dsnamesAndAge = argstr.split(',')
        agrs = []
        for dsnameAndAge in dsnamesAndAge:
            # format is: '<dsname>@<age_in_sec>'
            dsname, sep, age = dsnameAndAge.partition('@')
            subprocess.Popen('python2.5 grepfile.py %s %s %s' % (dsname, age, reffile), shell=True)

