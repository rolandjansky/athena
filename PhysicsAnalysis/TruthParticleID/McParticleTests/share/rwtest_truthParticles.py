#!/usr/bin/env python

import user
import commands
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
from McParticleTests.tests import doMcAodTupleValidation

def timeofday():
    from time import time
    return int(time())

uuid = "%s_%s" % (os.getpid(),timeofday())

print "#"*80
print "## testing TruthParticleContainer (read/write) persistency..."
print "## Job uuid:",uuid
bench = BenchSequence( "TruthParticleContainer (read/write) persistency" )

###-----------------------------------------------------
dataFilesDir = "/afs/cern.ch/atlas/offline/data/testfile"
dataRefDir   = "/afs/cern.ch/atlas/offline/test"

asciiFiles = {
    'w'    : { 'ref' : os.path.join(dataRefDir,"mc.aod.etisols.rel_13.ref"),
               'chk' : workDir("mc.aod.txt"), },
    'r'    : { 'ref' : os.path.join(dataRefDir,
                                    "reaccessed.mc.aod.etisols.rel_13.ref"),
               'chk' : workDir("reaccessed.mc.aod.txt"), },
    }

outFiles = {
    'ref' : workDir( "mc.aod.%s.pool"%uuid ),
    'chk' : workDir( "reaccessed.mc.aod.%s.pool"%uuid )
    }

evtMax = 100

def doReadWriteTest( genName = "TruthParticles", evtMax = 100 ):
    ###-----------------------------------------------------
    print "\n"
    print "#"*80
    print "## Preparing input data... [%s]" % genName
    templateJobO = """
OUTPUT='%(OutputFile)s';
DUMP=True;
include( 'McParticleTests/iotest_WriteGenEvent_jobOptions.py' );
"""
    jobOptions = [
        ChapPy.JobOptionsCmd( templateJobO % {
        'OutputFile' : workDir("mc.event.%s.pool"%uuid),
        } )
    ]
    athena = ChapPy.Athena( jobOptions = jobOptions,
                            logFile    = workDir("mc.event.%s.pool.log"%uuid),
                            checkLeak  = False )
    athena.EvtMax = evtMax
    sc = athena.run()
    if sc != 0:
        return ScOutput(sc, "ERROR")

    ###-----------------------------------------------------
    print "\n"
    print "#"*80
    print "## Testing [writing-%s]..." % genName
    templateJobO = """
INPUT=['%(InputFile)s'];
OUTPUT= '%(OutputFile)s';
DUMPTUPLE=True;
TUPLEFILENAME='%(TupleFile)s';
include( 'McParticleTests/iotest_WriteTruthParticles_jobOptions.py' );
jobproperties.PerfMonFlags.OutputFile = '%(PmonFile)s';
"""
    jobOptions = [
        ChapPy.JobOptionsCmd( templateJobO % {
        'InputFile'  : workDir("mc.event.%s.pool"%uuid),
        'OutputFile' : outFiles['ref'],
        'TupleFile'  : outFiles['ref'].replace(".pool", ".tuple.root"),
        'PmonFile'   : workDir("write.mcaod.%s.%s.perfmon.pmon.gz"%(genName,
                                                                    uuid)),
        } )
    ]    

    athena = ChapPy.Athena( jobOptions = jobOptions,
                            logFile    = outFiles['ref']+".log",
                            checkLeak  = False )
    athena.EvtMax = evtMax
    sc = athena.run()
    if sc != 0:
        return ScOutput(sc, "ERROR")
    else:
        inFile  = workDir(    "write.mcaod.%s.%s.perfmon.pmon.gz"%(genName,
                                                                   uuid))
        outFile = workDir("out.write.mcaod.%s.%s.perfmon.root"    %(genName,
                                                                    uuid))
        commands.getoutput( "perfmon.py %s -o %s" % ( inFile, outFile ) )

    ###-----------------------------------------------------
    print "\n"
    print "#"*80
    print "## Testing [reading-%s]..." % genName
    templateJobO = """
INPUT=['%(InputFile)s'];
OUTPUT= '%(OutputFile)s';
DUMPTUPLE=True;
TUPLEFILENAME='%(TupleFile)s';
include( 'McParticleTests/iotest_ReadTruthParticles_jobOptions.py' );
jobproperties.PerfMonFlags.OutputFile = '%(PmonFile)s';
"""
    jobOptions = [
        ChapPy.JobOptionsCmd( templateJobO % {
          'InputFile'  : outFiles['ref'],
          'OutputFile' : outFiles['chk'],
          'TupleFile'  : outFiles['chk'].replace(".pool", ".tuple.root"),
          'PmonFile'   : workDir("read.mcaod.%s.%s.perfmon.pmon.gz" %(genName,
                                                                      uuid)),
          } )
    ]    
    athena = ChapPy.Athena( jobOptions = jobOptions,
                            logFile    = outFiles['chk']+".log",
                            checkLeak  = False )
    athena.EvtMax = evtMax
    sc = athena.run()
    if sc != 0:
        return ScOutput(sc, "ERROR")
    else:
        inFile  = workDir(    "read.mcaod.%s.%s.perfmon.pmong.gz"%(genName,
                                                                   uuid))
        outFile = workDir("out.read.mcaod.%s.%s.perfmon.root"    %(genName,
                                                                   uuid))
        commands.getoutput( "perfmon.py %s -o %s" % ( inFile, outFile ) )

    ###-----------------------------------------------------
    print "\n"
    print "#"*80
    return doMcAodTupleValidation(
        outFiles['ref'].replace(".pool", ".tuple.root"),
        outFiles['chk'].replace(".pool", ".tuple.root")
        )

bench += doReadWriteTest( evtMax = evtMax )
print ""
print "#"*80
bench.printStatus()
print "## Bye."
print "#"*80
