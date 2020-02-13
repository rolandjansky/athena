#!/usr/bin/env python

from __future__ import print_function

import os
import sys
from AthenaCommon import ChapPy

###-----------------------------------------------------
## For compatibility with ATN tests
from TestTools.iobench import workDir

###-----------------------------------------------------
## Little helper to validate output of jobs
from TestTools.iobench import ScOutput
from TestTools.iobench import BenchSequence

## PerfMon helper
from PerfMonTests.tests import testPerfMon

print ("#"*80)
print ("## PerfMon test... [noopalg]")
print ("#"*80)
bench = BenchSequence( "PerfMonTest" )

bench += testPerfMon( "PerfMonTests/test_perfMonSvc_noopalg.py",
                      "perfmon.noopalg.root",
                      evtMax = 5000 )

print ("")
print ("#"*80)
bench.printStatus()
print ("## Bye.")
print ("#"*80)
