#!/usr/bin/env python

from __future__ import print_function

import os
import sys
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
from TestTools.iobench import BenchSequence

print ("#"*80)
print ("## StoreGate test... [producer/consumer-bench]")
print ("#"*80)
athena = ChapPy.Athena(
    jobOptions = [ ChapPy.JobOptions( "StoreGateTests/test_sgProducerConsumer_jobOptions.py" ) ]
    )
athena.EvtMax = 100
sc = athena.run()
if sc != 0: print ("ERROR")
else:
    subprocess.getstatusoutput(
        "perfmon.py %s -o %s" % ( "perfmon.pmon.gz", "out.sg.perfmon.root" )
        )
    print ("All tests SUCCESSFULLY completed")
    
print ("")
print ("#"*80)
print ("## Bye.")
print ("#"*80)
