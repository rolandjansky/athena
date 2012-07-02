#!/usr/bin/env python

import user
import os
import sys
from AthenaCommon import ChapPy
from TestTools import iobench

###-----------------------------------------------------
## For compatibility with ATN tests
from TestTools.iobench import workDir

###-----------------------------------------------------
## Little helper to validate output of jobs
from TestTools.iobench import doValidation
from TestTools.iobench import ScOutput
from TestTools.iobench import BenchSequence

def timeofday():
    from time import time
    return int(time())

uuid = "%s_%s" % (os.getpid(),timeofday())

print "#"*80
print "## testing McEventCollection I/O..."
print "## Job uuid:",uuid
benchSequence = BenchSequence( "McEventCollection I/O" )

EVTMAX = 100
NTIMES = 1

###-----------------------------------------------------
print "## Testing [writing]..."
jobOptions = [
    ChapPy.JobOptionsCmd("OUTPUT=\"%s\"" % workDir("mc.io.event.%s.pool"%uuid)),
    ChapPy.JobOptionsCmd("DUMP=False"),
    ChapPy.JobOptions( "McParticleTests/iotest_WriteGenEvent_jobOptions.py" ),
    ]

athena = ChapPy.Athena( jobOptions = jobOptions,
                        #logFile = "/dev/stdout",
                        checkLeak  = False )
athena.EvtMax = EVTMAX

bench = iobench.AthBench( athena,
                          nTimes = NTIMES,
                          logFile = workDir("write.%s.log" %
                                            os.path.basename(workDir("mc.io.event.%s.pool"%uuid))) )

## print "## bench:"
## print bench.athena
bench.run()

#bench.ioStats( [ "McEventCollection#GEN_EVENT" ], "w" )
bench.save( "iobench-genevent-%ievts.write.%slog" % (athena.EvtMax,uuid) )
bench.stats()

###-----------------------------------------------------

print "\n\n"
print "#"*80
print "## Testing [reading]..."
jobOptions = [
    ChapPy.JobOptionsCmd("INPUT=[\"%s\"]"%workDir("mc.io.event.%s.pool"%uuid)),
    ChapPy.JobOptionsCmd("DUMP=False"),
    ChapPy.JobOptions( "McParticleTests/iotest_ReadGenEvent_jobOptions.py" ),
    ]
athena = ChapPy.Athena( jobOptions = jobOptions,
                        checkLeak  = False )
athena.EvtMax = EVTMAX

bench = iobench.AthBench( athena,
                          nTimes = NTIMES,
                          logFile = workDir("read.%s.log" %
                                            os.path.basename(workDir("mc.io.event.%s.pool"%uuid))) )
#print "## bench:"
#print bench.athena
bench.run()

#bench.ioStats( [ "McEventCollection#GEN_EVENT" ], "r" )
#bench.ioStats( [ "McEventCollection#GEN_EVENT" ], "w" )
bench.save( "iobench-genevent-%ievts.read.%s.log" % (athena.EvtMax,uuid) )
bench.stats()

print ""
print "#"*80
benchSequence.printStatus()
print "## Bye."
print "#"*80
