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
from McParticleTests.tests import doTupleValidation

def timeofday():
    from time import time
    return int(time())

uuid = "%s_%s" % (os.getpid(),timeofday())

print "#"*80
print "## testing McEventCollection (read/write) persistency..."
print "## Job uuid:",uuid
print "#"*80
bench = BenchSequence( "McEventCollection (read/write) persistency" )

###-----------------------------------------------------
dataFilesDir = "/afs/cern.ch/atlas/offline/data/testfile"
dataRefDir   = "/afs/cern.ch/atlas/offline/test"

outFiles = {
    'gen_pythia' : { 'ref' : workDir("mc.event.pythia.%s.pool"%uuid),
                     'chk' : workDir("reaccessed.mc.event.pythia.%s.pool"%uuid), },
    'gen_herwig' : { 'ref' : workDir("mc.event.herwig.%s.pool"%uuid),
                     'chk' : workDir("reaccessed.mc.event.herwig.%s.pool"%uuid), },
    }

evtMax = 100

def doReadWriteTest( genName = "pythia", evtMax = 100 ):
    """A simple wrapper around the read/write tests..."""
    genName = genName.lower()
    ###-----------------------------------------------------
    print ""
    print "#"*80
    print "## Testing [writing-%s]..." % genName
    templateJobO = """
OUTPUT='%(OutputFile)s';
DUMPTUPLE=True;
GENERATOR='%(Generator)s';
TUPLEFILENAME='%(TupleFile)s';
include( 'McParticleTests/iotest_WriteGenEvent_jobOptions.py' );
jobproperties.PerfMonFlags.OutputFile = '%(PmonFile)s';
"""
    jobOptions = [
        ChapPy.JobOptionsCmd( templateJobO % {
        'OutputFile' : workDir("mc.event.%s.%s.pool" % (genName,uuid)),
        'Generator'  : genName.upper(),
        'TupleFile'  : outFiles['gen_%s' % genName]['ref']\
                       .replace(".pool",".tuple.root"),
        'PmonFile'   : workDir("write.genevent.%s.%s.pmon.gz"%(genName,uuid)),
        } )
        ]
        
    athena = ChapPy.Athena(jobOptions = jobOptions,
                           logFile = workDir("write.genevent.%s.%s.log"%(genName,uuid)),
                           checkLeak  = False )
    athena.EvtMax = evtMax
    sc = athena.run()
    if sc != 0:
        return ScOutput(sc, "ERROR")
    else:
        inFile  = workDir(    "write.genevent.%s.%s.pmon.gz"   %(genName,uuid))
        outFile = workDir("out.write.genevent.%s.%s.pmon.root" %(genName,uuid))
        print commands.getoutput( "perfmon.py %s -o %s" % ( inFile, outFile ) )
                                 
    ###-----------------------------------------------------
    print "\n"
    print "#"*80
    print "## Testing [reading-%s]..." % genName
    templateJobO = """
INPUT=['%(InputFile)s'];
DUMPTUPLE=True;
TUPLEFILENAME='%(TupleFile)s';
include( 'McParticleTests/iotest_ReadGenEvent_jobOptions.py' );
jobproperties.PerfMonFlags.OutputFile = '%(PmonFile)s';
"""
    jobOptions = [
        ChapPy.JobOptionsCmd( templateJobO % {
        'InputFile' : workDir("mc.event.%s.%s.pool" % (genName,uuid)),
        'TupleFile' : outFiles['gen_%s' % genName]['chk']\
                      .replace(".pool",".tuple.root"),
        'PmonFile'  : workDir("read.genevent.%s.%s.pmon.gz" % (genName,uuid)),
        } )
    ]
    athena = ChapPy.Athena( jobOptions = jobOptions,
                            logFile  = workDir("read.genevent.%s.%s.log"%(genName,uuid)),
                            checkLeak  = False )
    athena.EvtMax = evtMax
    sc = athena.run()
    if sc != 0:
        return ScOutput(sc, "ERROR")
    else:
        inFile  = workDir(    "read.genevent.%s.%s.pmon.gz"   % (genName,uuid))
        outFile = workDir("out.read.genevent.%s.%s.pmon.root" % (genName,uuid))
        print commands.getoutput( "perfmon.py %s -o %s" % ( inFile, outFile ) )

    ###-----------------------------------------------------
    print "\n"
    print "#"*80
    return doTupleValidation(
        outFiles['gen_%s' % genName]['ref'].replace(".pool", ".tuple.root"),
        outFiles['gen_%s' % genName]['chk'].replace(".pool", ".tuple.root") )

###-----------------------------------------------------
### perform all the tests
for genName in [
    "pythia",
    "herwig",
    ]:
    try:
        bench += doReadWriteTest( genName, evtMax )
    except KeyboardInterrupt:
        print "\n*** user hit Ctrl-C ! ***"
        print "*** skipping test [%s] ***" % genName
        continue
    pass

print ""
print "#"*80
bench.printStatus()
print "## Bye."
print "#"*80
