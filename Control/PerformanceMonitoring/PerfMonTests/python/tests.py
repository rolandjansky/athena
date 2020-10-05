# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##
## A little py-module holding methods to ease the scheduling of PerfMonTests
## @author: Sebastien Binet

from __future__ import print_function

import os
from AthenaCommon import ChapPy

from future import standard_library
standard_library.install_aliases()
import subprocess

###-----------------------------------------------------
## For compatibility with ATN tests
from TestTools.iobench import workDir

###-----------------------------------------------------
## Little helper to validate output of jobs
from TestTools.iobench import ScOutput

def testPerfMon( jobOptName      = "PerfMonTests/test_perfMonSvc_noopalg.py",
                 perfMonFileName = "perfmon.noopalg.root",
                 evtMax          = 50000 ):
    print ("")
    print ("#"*80)
    print ("## testing PerfMonSvc [%s]"         % jobOptName)
    print ("                  ==> [%s]... (%i)" % (perfMonFileName,evtMax))

    perfMonFileName = workDir( perfMonFileName )
    refPerfMonFileName = "ref." + os.path.basename( perfMonFileName )
    chkPerfMonFileName = "chk." + os.path.basename( perfMonFileName )
    outPerfMonFileName = "ana." + os.path.basename( perfMonFileName )

    refPerfMonFileName = refPerfMonFileName.replace(".root", ".pmon.gz")
    chkPerfMonFileName = chkPerfMonFileName.replace(".root", ".pmon.gz")

    ## create the reference file
    athena = ChapPy.Athena(
        jobOptions = [ ChapPy.JobOptions( jobOptName ),
                       ChapPy.JobOptionsCmd( "jobproperties.PerfMonFlags.OutputFile = '%s'" % refPerfMonFileName ) ],
        checkLeak = False,
        logFile = refPerfMonFileName + ".log"
        )
    athena.EvtMax = evtMax
    sc = athena.run()
    if sc != 0:
        print ("ERROR: could not create the 'ref' perfmon file !!")
        return ScOutput(sc, "ERROR")
    
    ## create the to-be-checked file
    athena = ChapPy.Athena(
        jobOptions = [ ChapPy.JobOptions( jobOptName ),
                       ChapPy.JobOptionsCmd( "jobproperties.PerfMonFlags.OutputFile = '%s'" % chkPerfMonFileName ) ],
        checkLeak = False,
        logFile = chkPerfMonFileName + ".log"
        )
    athena.EvtMax = evtMax
    sc = athena.run()
    if sc != 0:
        print ("ERROR: could not create the 'chk' perfmon file !!")
        return ScOutput(sc, "ERROR")

    #outPerfMonFileName = "ana." + os.path.basename( perfMonFileName )
    print (" :::running [perfmon.py]...")
    cmd = "perfmon.py %s %s -o %s" % \
          ( chkPerfMonFileName, refPerfMonFileName, outPerfMonFileName )
    sc,out = subprocess.getstatusoutput( cmd )
    if sc != 0:
        print ("## Problem while doing [perfmon] !!")
        print (out)
        out = "ERROR"
        return ScOutput(sc, out)

    out = "OK"
    print ("## [DONE]")
    return ScOutput(sc, out)
