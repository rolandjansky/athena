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

## PerfMon helper
from PerfMonTests.tests import testPerfMon

print "#"*80
print "## PerfMon test... [baseline-bench]"
print "#"*80
bench = BenchSequence( "PerfMonTest" )

bench += testPerfMon( "PerfMonTests/test_perfMonSvc_baseline_bench.py",
                      "perfmon.baseline.root",
                      evtMax = 1000 )

print ""
print "#"*80
bench.printStatus()
print "## Bye."
print "#"*80
