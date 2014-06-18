#!/bin/bash

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"exec" "/usr/bin/python" "-u" "-Wignore" "$0" "$@"

# using /usr/bin/python for pyGTK 

import re
import os
import sys
import code
import time
import atexit
import signal
import datetime
import tempfile
import commands

# tweak sys.path since threading cannot be imported with Athena 15 on SLC5/64
tmpOut = commands.getoutput('unset LD_LIBRARY_PATH; unset PYTHONPATH; /usr/bin/python -c "import sys;print sys.path"')
try:
    exec "tmpSysPath = %s" % tmpOut.split('\n')[-1]
    sys.path = tmpSysPath + sys.path
except:
    pass

import optparse
import readline
import threading

from pandatools import PdbUtils
from pandatools import Client
from pandatools import BookConfig
from pandatools import GlobalConfig
from pandatools import PLogger
from pandatools import PsubUtils
from pandatools import PandaToolsPkgInfo


options_parser = optparse.OptionParser()
options_parser.add_option("-o","--output",dest="ofname",default="timing.dat",
                          help="Output file name",metavar="FILE")
options_parser.add_option("-v","--verbose",dest="verbose",
                          default=0,action="count")
(prog_options, prog_args) = options_parser.parse_args()

outfile = open(prog_options.ofname,"w")

print "Scan of JobsetIDs=", prog_args

# history support
pconfDir = os.path.expanduser(os.environ['PANDA_CONFIG_ROOT'])
if not os.path.exists(pconfDir):
    os.makedirs(pconfDir)

# set grid source file
globalConf = GlobalConfig.getConfig()
if globalConf.grid_src != '' and not os.environ.has_key('PATHENA_GRID_SETUP_SH'):
    os.environ['PATHENA_GRID_SETUP_SH'] = globalConf.grid_src

# make tmp dir
tmpDir = tempfile.mkdtemp()

# set tmp dir in Client
Client.setGlobalTmpDir(tmpDir)

# look for PandaTools package
for path in sys.path:
    if path == '':
        path = '.'
    
    if os.path.exists(path) and os.path.isdir(path) and 'pandatools' in os.listdir(path) \
           and os.path.exists('%s/pandatools/__init__.py' % path):
        # make symlink for module name
        os.symlink('%s/pandatools' % path,'%s/taskbuffer' % tmpDir)
        break
sys.path = [tmpDir]+sys.path

# total time
TotalCPUTime = 0
TotalRealTime = 0
TotalJobs = 0

tmpLog = PLogger.getPandaLogger()
gridPassPhrase,vomsFQAN = PsubUtils.checkGridProxy("",False,prog_options.verbose)
bookConf = BookConfig.getConfig()

if prog_options.verbose>0 :
    verbose2 = prog_options.verbose-1
else :
    verbose2 = 0

for strid in prog_args :
    print "Retrieving information for Jobset:", strid
    status, pandaIDstatus = Client.getPandIDsWithJobID(int(strid))
    if prog_options.verbose :
        print status, pandaIDstatus

    if status != 0 :
        print "Skip..."
        continue
    
    pandaJob = None
    if pandaIDstatus != {}:
        tmpPandaIDs = pandaIDstatus.keys()
        tmpPandaIDs.sort()
        nIDs = len(tmpPandaIDs)
        nIDs_step = (nIDs+9)/10
        for i, tmpID in enumerate(tmpPandaIDs) :
            if prog_options.verbose : 
                if i%nIDs_step==0 : 
                    print i, '/', nIDs
            status,tmpPandaJob = Client.getFullJobStatus([tmpID],verbose=verbose2)
            CPUTime = getattr(tmpPandaJob[0],"cpuConsumptionTime")
            startTime = getattr(tmpPandaJob[0],"startTime")
            endTime = getattr(tmpPandaJob[0],"endTime")
            parentID = getattr(tmpPandaJob[0],"parentID")
            jobsetID = getattr(tmpPandaJob[0],"jobsetID")
            if prog_options.verbose :
                print status, getattr(tmpPandaJob[0],"jobStatus"), CPUTime, startTime, endTime
            if getattr(tmpPandaJob[0],"jobStatus") == "finished" and "runGen" in getattr(tmpPandaJob[0],"transformation") :
                TotalCPUTime += CPUTime
                deltaT = endTime-startTime
                TotalRealTime += deltaT.days*86400+deltaT.seconds+deltaT.microseconds*1e-6
                TotalJobs += 1
                outfile.write("%d \"%s\" \"%s\" %s %s\n" %(CPUTime,startTime,endTime,jobsetID,parentID))
		
outfile.close()
commands.getoutput('rm -rf %s' % tmpDir)

print "Number of jobs:", TotalJobs
print "Total CPU time:", TotalCPUTime, " Average:", float(TotalCPUTime)/float(TotalJobs)
print "Total Real time:", TotalRealTime, " Average:", float(TotalRealTime)/float(TotalJobs)
print "CPU efficiency: %5.2f" % (float(TotalCPUTime)/float(TotalRealTime))
