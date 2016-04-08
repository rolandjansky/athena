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

print "#"*80
print "## testing McEventCollection (read/write) persistency..."
print "#"*80
bench = BenchSequence( "McEventCollection (read/write) ASCII persistency" )

###-----------------------------------------------------
outFiles = {
    'gen_pythia' : { 'ref' : workDir("mc.event.pythia.pool"),
                     'chk' : workDir("reaccessed.mc.event.pythia.pool"), },
    'gen_herwig' : { 'ref' : workDir("mc.event.herwig.pool"),
                     'chk' : workDir("reaccessed.mc.event.herwig.pool"), },
    }

evtMax = 50

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
        'OutputFile' : workDir("mc.event.%s.pool" % genName),
        'Generator'  : genName.upper(),
        'TupleFile'  : outFiles['gen_%s' % genName]['ref']\
                       .replace(".pool",".tuple.root"),
        'PmonFile'   : workDir("write.genevent.%s.pmon.gz" % genName),
        } )
        ]
        
    athena = ChapPy.Athena(jobOptions = jobOptions,
                           logFile = workDir("write.genevent.%s.log"%genName),
                           checkLeak  = False )
    athena.EvtMax = evtMax
    sc = athena.run()
    if sc != 0:
        return ScOutput(sc, "ERROR")
    else:
        inFile  = workDir(    "write.genevent.%s.pmon.gz"   % genName)
        outFile = workDir("out.write.genevent.%s.pmon.root" % genName)
        print commands.getoutput( "perfmon.py %s -o %s" % ( inFile, outFile ) )
                                 
    ###-----------------------------------------------------
    print ""
    print "#"*80
    print "## Testing [ASCII-writing-%s (1)]..." % genName
    templateJobO = """
INPUT=%(InputFile)s;
OUTPUT='%(OutputFile)s';
DUMPTUPLE=True;
GENERATOR='%(Generator)s';
include( 'McParticleAlgs/GenEventAsciiWriter_jobOptions.py' );
"""
    jobOptions = [
        ChapPy.JobOptionsCmd( templateJobO % {
        'InputFile'  : [workDir("mc.event.%s.pool" % genName),],
        'OutputFile' : workDir("mc.event.%s.1.ascii" % genName),
        'Generator'  : genName.upper(),
        } )
        ]
        
    athena = ChapPy.Athena(jobOptions = jobOptions,
                           logFile = workDir("write.genevent.ascii.1.%s.log"%genName),
                           checkLeak  = False )
    athena.EvtMax = evtMax
    sc = athena.run()
    if sc != 0:
        return ScOutput(sc, "ERROR")
                                 
    ###-----------------------------------------------------
    print ""
    print "#"*80
    print "## Testing [ASCII-writing-%s (2)]..." % genName
    templateJobO = """
INPUT=%(InputFile)s;
OUTPUT='%(OutputFile)s';
DUMPTUPLE=True;
GENERATOR='%(Generator)s';
include( 'McParticleAlgs/GenEventAsciiWriter_jobOptions.py' );
"""
    jobOptions = [
        ChapPy.JobOptionsCmd( templateJobO % {
        'InputFile'  : [workDir("mc.event.%s.pool" % genName),],
        'OutputFile' : workDir("mc.event.%s.2.ascii" % genName),
        'Generator'  : genName.upper(),
        } )
        ]
        
    athena = ChapPy.Athena(jobOptions = jobOptions,
                           logFile = workDir("write.genevent.ascii.2.%s.log"%genName),
                           checkLeak  = False )
    athena.EvtMax = evtMax
    sc = athena.run()
    if sc != 0:
        return ScOutput(sc, "ERROR")
                                 
    ###-----------------------------------------------------
    print "\n"
    print "#"*80
    print "## Testing [ASCII-reading-%s]..." % genName
    templateJobO = """
INPUT=%(InputFiles)s;
include( 'McAsciiEventSelector/Example_McAsciiReader_jobOptions.py' );
"""
    jobOptions = [
        ChapPy.JobOptionsCmd( templateJobO % {
        'InputFiles' : [ workDir("mc.event.%s.1.ascii" % genName),
                         workDir("mc.event.%s.2.ascii" % genName),],
        'TupleFile' : outFiles['gen_%s' % genName]['chk']\
                      .replace(".pool",".tuple.root"),
        'PmonFile'  : workDir("read.genevent.%s.pmon.gz" % genName),
        } )
    ]
    athena = ChapPy.Athena( jobOptions = jobOptions,
                            logFile  = workDir("read.genevent.%s.log"%genName),
                            checkLeak  = False )
    athena.EvtMax = evtMax
    sc = athena.run()
    if sc != 0:
        return ScOutput(sc, "ERROR")

    ###-----------------------------------------------------
    print "\n"
    print "#"*80
    return ScOutput(0, "OK")

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
