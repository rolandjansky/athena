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
from TestTools.iobench import doValidation
from TestTools.iobench import ScOutput
from TestTools.iobench import BenchSequence

print "#"*80
print "## testing McEventCollection (read/back) persistency..."
bench = BenchSequence( "McEventCollection (read/back) persistency" )

###-----------------------------------------------------
dataFilesDir = "/afs/cern.ch/atlas/offline/data/testfile"
dataRefDir   = "/afs/cern.ch/atlas/offline/test"

asciiFiles = {
    'rel_1004'  : { 'ref' : os.path.join(dataRefDir,'mc.event.rel_1004.ref'),
                    'chk' : workDir('mc.event.rel_1004.txt'),
                    'in'  : os.path.join(dataFilesDir,
                                         "q02initialprod.0001.H_2e2mu.q02dig_1004.etacut.0001_extract.pool.root") },
    'rel_11041' : { 'ref' : os.path.join(dataRefDir,'mc.event.rel_11041.ref'),
                    'chk' : workDir('mc.event.rel_11041.txt'),
                    'in'  : os.path.join(dataFilesDir,
                                         "mc11.004100.T1_McAtNLO_top.digit_test.RDO.v11000401._00001.pool.root") },
    'rel_1204'  : { 'ref' : os.path.join(dataRefDir,'mc.event.rel_1204.ref'),
                    'chk' : workDir('mc.event.rel_1204.txt'),
                    'in'  : os.path.join(dataFilesDir,
                                         "calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00016_extract_10evt.pool.root") },
    }

###-----------------------------------------------------
templateJobO = """
INPUT=['%(InputFile)s'];
DUMP = True;
include( 'McParticleTests/rbtest_ReadGenEvent_jobOptions.py' );
outStream.OutputFile = '%(OutputFile)s';
topSequence.GenEventAsciiWriter.McWriter.McEvents = 'TruthEvent';
topSequence.GenEventAsciiWriter.McWriter.Output = '%(AsciiChk)s';
"""

if 0:
    ###-----------------------------------------------------
    print "\n"
    print "#"*80
    print "## Testing [reading 12.0.4 data]..."
    jobOptions = [
        ChapPy.JobOptionsCmd( templateJobO % {
            'InputFile'  : asciiFiles['rel_1204']['in'],
            'OutputFile' : workDir("reaccessed.mc.events_rel12.pool"),
            'AsciiChk'   : asciiFiles['rel_1204']['chk'],
            } )
        ]
    athena = ChapPy.Athena( jobOptions = jobOptions,
                            logFile    = workDir("read.genevent.rel_1204.log"),
                            checkLeak  = False )
    athena.EvtMax = 10
    athena.run()

    ###-----------------------------------------------------
    print "\n"
    print "#"*80
    bench += doValidation(asciiFiles,'rel_1204')
    pass

print ""
print "#"*80
bench.printStatus()
print "## Bye."
print "#"*80

# ###-----------------------------------------------------
# print "\n"
# print "#"*80
# print "## Testing [reading 10.0.4 data]..."
# jobOptions = [
#     ChapPy.JobOptionsCmd( templateJobO % {
#     'InputFile'  : asciiFiles['rel_1004']['in'],
#     'OutputFile' : workDir("reaccessed.mc.events_rel10.pool"),
#     'AsciiChk'   : asciiFiles['rel_1004']['chk'],
#     } )
#     ]
# athena = ChapPy.Athena( jobOptions = jobOptions,
#                         logFile    = workDir("read.genevent.rel_1004.log"),
#                         checkLeak  = False )
# athena.EvtMax = 10
# athena.run()

# ###-----------------------------------------------------
# print "\n"
# print "#"*80
# bench += doValidation(asciiFiles,'rel_1004')

# ###-----------------------------------------------------
# print "\n"
# print "#"*80
# print "## Testing [reading 11.0.41 data]..."
# jobOptions = [
#     ChapPy.JobOptionsCmd( templateJobO % {
#     'InputFile'  : asciiFiles['rel_11041']['in'],
#     'OutputFile' : workDir("reaccessed.mc.events_rel11.pool"),
#     'AsciiChk'   : asciiFiles['rel_11041']['chk'],
#     } )
#     ]
# athena = ChapPy.Athena( jobOptions = jobOptions,
#                         logFile    = workDir("read.genevent.rel_11041.log"),
#                         checkLeak  = False )
# athena.EvtMax = 10
# athena.run()

# ###-----------------------------------------------------
# print "\n"
# print "#"*80
# bench += doValidation(asciiFiles,'rel_11041')

