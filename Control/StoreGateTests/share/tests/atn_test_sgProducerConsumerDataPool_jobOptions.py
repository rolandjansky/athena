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
print "## StoreGate test... [producer/consumer-DataPool-bench]"
print "#"*80
print ":::   No DataPool ",
athena = ChapPy.Athena(
    jobOptions = [
    ChapPy.JobOptionsCmd( "SGDATAPOOL=False" ),
    ChapPy.JobOptions("StoreGateTests/test_sgProducerConsumer_jobOptions.py"),
    ChapPy.JobOptionsCmd("jp.PerfMonFlags.OutputFile = 'nodatapool.pmon.gz'"),
    ]
    )
athena.EvtMax = 100
sc = athena.run()
if sc != 0:
    print "ERROR"
    sys.exit(sc)

print "::: With DataPool ",
athena = ChapPy.Athena(
    jobOptions = [
    ChapPy.JobOptionsCmd( "SGDATAPOOL=True" ),
    ChapPy.JobOptions("StoreGateTests/test_sgProducerConsumer_jobOptions.py"),
    ChapPy.JobOptionsCmd("jp.PerfMonFlags.OutputFile = 'withdatapool.pmon.gz'"),
    ]
    )
athena.EvtMax = 100
sc = athena.run()
if sc != 0:
    print "ERROR"
    sys.exit(sc)

sc,out = commands.getstatusoutput(
    "perfmon.py %s %s -o %s -l dp,nodp -s \"m.name!=''\"" % (
             "withdatapool.pmon.gz",
             "nodatapool.pmon.gz",
             "datapool.root"
             )
    )
print "All tests SUCCESSFULLY completed"
    
print ""
print "#"*80
print "## Bye."
print "#"*80
