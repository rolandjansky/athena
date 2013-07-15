#!/usr/bin/env python
""" a simple test case for PerfMonSvc
    launch with:
    lxplus> chappy PerfMonTests/test_perfMonSvc.py
"""

import user
import os
import sys
import commands
from AthenaCommon import ChapPy

###-----------------------------------------------------
## For compatibility with ATN tests
from TestTools.iobench import workDir

def testPerfMon( jobOptName      = "PerfMonTests/test_perfMonSvc_noopalg.py",
                 perfMonFileName = "perfmon.noopalg.pmon.gz",
                 evtMax          = 50000 ):
    print ""
    print "#"*80
    print "## testing PerfMonSvc [%s]"         % jobOptName
    print "                  ==> [%s]... (%i)" % (perfMonFileName,evtMax)
    
    refPerfMonFileName = "ref." + os.path.basename( perfMonFileName )
    chkPerfMonFileName = "chk." + os.path.basename( perfMonFileName )
    outPerfMonFileName = "ana." + os.path.basename( perfMonFileName )

    ## create the reference file
    athena = ChapPy.Athena( jobOptions = [ ChapPy.JobOptions( jobOptName ) ],
                            checkLeak = False,
                            logFile = refPerfMonFileName+".log" )
    athena.EvtMax = evtMax
    athena.run()
    os.rename( "perfmon.pmon.gz", refPerfMonFileName )

    ## create the to-be-checked file
    athena = ChapPy.Athena( jobOptions = [ ChapPy.JobOptions( jobOptName ) ],
                            checkLeak = False,
                            logFile = chkPerfMonFileName+".log" )
    athena.EvtMax = evtMax
    athena.run()
    os.rename( "perfmon.pmon.gz", chkPerfMonFileName )

    print " :::running [perfmon.py]..."
    cmd = "perfmon.py -o %s %s %s --labels 'chk,ref'" % \
          ( outPerfMonFileName, chkPerfMonFileName, refPerfMonFileName )
    sc,out = commands.getstatusoutput( cmd )
    if sc != 0:
        print "## Problem while doing [perfmon] !!"
        print out
    else:
        print "## [DONE]"
        
    return sc

evtMax = 5000
jobs = [
    ['noopalg.py'              , 'perfmon.noopalg.pmon.gz',               evtMax],
    ['noopalg_write_datahdr.py', 'perfmon.noopalg.write_datahdr.pmon.gz', evtMax],
    ['noopalg_write_evtinfo.py', 'perfmon.noopalg.write_evtinfo.pmon.gz', evtMax],
    ['basicalg.py'             , 'perfmon.basicalg.pmon.gz',               1000],
    ['leakyalg.py'             , 'perfmon.leakyalg.pmon.gz',              evtMax],
    #['multiple_noopalgs.py'    , 'perfmon.noopalgs.pmon.gz', 1000],

    #['multiple_noopalgs_write_evtinfo.py',
    # 'perfmon.noopalgs.write_evtinfo.pmon.gz', 1000],

    ['baseline_bench.py'       , 'perfmon.baseline_bench.pmon.gz', evtMax],
    ['evtgen.py'               , 'perfmon.evtgen.pmon.gz',           1000],
    ['truthjets.py'            , 'perfmon.truthjets.pmon.gz',         500],
    
    ]

for jobOptName, perfMonFileName, evtMax in jobs:
    testPerfMon( "PerfMonTests/test_perfMonSvc_%s" % jobOptName,
                 perfMonFileName,
                 evtMax )

print ""
print "## Bye."
print "#"*80
