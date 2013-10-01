#!/usr/bin/env python

import user
import os
import sys
import commands
from AthenaCommon import ChapPy

###-----------------------------------------------------
## For compatibility with ATN tests
from TestTools.iobench import workDir

###-----------------------------------------------------
## Little helper to validate output of jobs
from TestTools.iobench import ScOutput
from TestTools.iobench import BenchSequence

print "#"*80
print "## StoreGate test... [producer/consumer-bench]"
print "#"*80
athena = ChapPy.Athena(
    jobOptions = [ ChapPy.JobOptions( "StoreGateTests/test_sgProducerConsumer_jobOptions.py" ) ]
    )
athena.EvtMax = 100
sc = athena.run()
if sc != 0: print "ERROR"
else:
    commands.getstatusoutput(
        "perfmon.py %s -o %s" % ( "perfmon.pmon.gz", "out.sg.perfmon.root" )
        )
    print "All tests SUCCESSFULLY completed"
    
print ""
print "#"*80
print "## Bye."
print "#"*80
