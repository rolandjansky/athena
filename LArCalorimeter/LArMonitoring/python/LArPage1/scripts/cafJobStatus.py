#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, sys  
from optparse import OptionParser,  Option, OptionValueError
import subprocess as sp

usage = 'usage: %prog [options] arg'
parser = OptionParser()
parser.add_option('-b','--bulk',action='store_true',dest='doBulk',default=False,help='Activate option to look at status of cleaning job for the **Bulk**. [default = False]')

(options, args) = parser.parse_args()

type = "runCleaning"
if options.doBulk : type = "runCleaningBulk"

#cmd  = "export CAFMGRPATH=/afs/cern.ch/user/l/larcomm/w0/digitMon/CAFManager; "
cmd += "export CAFMGRPATH=/afs/cern.ch/user/l/larmon/public/LArPage1/scripts/CAFManager"
cmd += "export PATH=$PATH:$CAFMGRPATH; "
cmd += "cafMgr_dump --jobs --type " + type
#print "Executing", cmd

#os.system(cmd)
cout_popen = sp.Popen(cmd,shell=True,stdout=sp.PIPE,stderr=sp.PIPE)
output_popen = cout_popen.communicate()[0]
print output_popen
