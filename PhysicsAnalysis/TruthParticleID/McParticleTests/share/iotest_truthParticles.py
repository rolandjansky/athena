#!/usr/bin/env python

import user
import os, time
import sys
from TestTools import iobench
from AthenaCommon import ChapPy

###-----------------------------------------------------
## For compatibility with ATN tests
from TestTools.iobench import workDir

###-----------------------------------------------------
## Little helper to validate output of jobs
from TestTools.iobench import doValidation
from TestTools.iobench import ScOutput
from TestTools.iobench import BenchSequence

EVTMAX = 100
NTIMES = 1

def timeofday():
    from time import time
    return int(time())

uuid = "%s_%s" % (os.getpid(),timeofday())
genEventFile = workDir("mc.event.%s.pool"%uuid)
mcAodFile    = workDir("mc.aod.%s.pool"%uuid)

print "#"*80
print "## testing TruthParticleContainer I/O..."
print "## Job uuid:",uuid
benchSequence = BenchSequence( "TruthParticleContainer I/O" )

###-----------------------------------------------------
if os.path.exists(genEventFile):
    print "## Re-using input data..."
else:
    print "## Preparing input data..."
    jobOptions = [
        ChapPy.JobOptionsCmd( "OUTPUT=\"%s\"" % genEventFile ),
        ChapPy.JobOptionsCmd( "DUMP=False" ),
        ChapPy.JobOptions( "McParticleTests/iotest_WriteGenEvent_jobOptions.py" ),
        ]
    athena = ChapPy.Athena( jobOptions = jobOptions,
                            checkLeak  = False )
    athena.EvtMax = EVTMAX
    athena.run()
    pass

###-----------------------------------------------------
print "\n\n"
print "#"*80
print "## Testing [writing]..."
jobOptions = [
    ChapPy.JobOptionsCmd( "INPUT=[\"%s\"]" % genEventFile ),
    ChapPy.JobOptionsCmd( "OUTPUT=\"%s\""  % mcAodFile ),
    ChapPy.JobOptionsCmd( "DUMP=False" ),
    ChapPy.JobOptions( "McParticleTests/iotest_WriteTruthParticles_jobOptions.py" ),
    ]

athena = ChapPy.Athena( jobOptions = jobOptions,
                        checkLeak  = False )
athena.EvtMax = EVTMAX

bench = iobench.AthBench( athena, nTimes = NTIMES,
                          logFile = workDir("write.%s.log" %
                                            os.path.basename(mcAodFile)) )
bench.run()

bench.ioStats( [ "McEventCollection#GEN_EVENT" ],   "r" )
bench.ioStats( [ "TruthParticleContainer#SpclMC" ], "w" )
bench.save( workDir("iobench-truthparticles-%ievts.write.log"%athena.EvtMax) )
bench.stats()

###-----------------------------------------------------
print "\n\n"
print "#"*80
print "## Testing [reading]..."
jobOptions = [
    ChapPy.JobOptionsCmd( "INPUT=[\"%s\"]" % mcAodFile ),
    ChapPy.JobOptionsCmd( "DUMP=False" ),
    ChapPy.JobOptions( "McParticleTests/iotest_ReadTruthParticles_jobOptions.py" ),
    ]
athena = ChapPy.Athena( jobOptions = jobOptions,
                        checkLeak  = False )
athena.EvtMax = EVTMAX

bench = iobench.AthBench( athena, nTimes = NTIMES,
                          logFile = workDir("read.%s.log" %
                                            os.path.basename(mcAodFile))  )
bench.run()

bench.ioStats( [ "McEventCollection#GEN_EVENT" ],   "r" )
bench.ioStats( [ "TruthParticleContainer#SpclMC" ], "r" )
bench.ioStats( [ "TruthParticleContainer#SpclMC" ], "w" )
bench.save( workDir("iobench-truthparticles-%ievts.read.log"%athena.EvtMax) )
bench.stats()

print ""
print "#"*80
benchSequence.printStatus()
print "## Bye."
print "#"*80
