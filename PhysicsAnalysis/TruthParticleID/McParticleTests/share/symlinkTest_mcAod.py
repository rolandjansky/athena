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
from TestTools.iobench import doPostCheck
from TestTools.iobench import ScOutput
from TestTools.iobench import BenchSequence
    
###-----------------------------------------------------
def installRefFiles( fileNames ):
    for refFile in fileNames:
        for fileName in [ refFile, workDir(refFile) ]:
            if os.path.exists(fileName):
                os.remove(fileName)
        sc,out = commands.getstatusoutput( "get_files %s" % refFile )
        if sc != 0:
            print "## ERROR: could not retrieve [%s]" % refFile
            print "## reason:\n",out
            continue
        if os.path.exists(refFile) and \
           os.path.exists(workDir(refFile)) and \
           os.path.samefile( refFile, workDir(refFile) ):
            print " -%s" % workDir(refFile)
            continue
        sc,out = commands.getstatusoutput( "mv %s %s" % ( refFile,
                                                          workDir(refFile) ) )
        if sc != 0:
            print "## ERROR: could not install [%s] into [%s]" %\
                  ( refFile, workDir(refFile) )
            print "## reason:\n",out
            continue
        else:
            print " -%s" % workDir(refFile)
    return

print "#"*80
print "## testing symlinks for TruthParticleContainer..."
bench = BenchSequence( "Symlinks for TruthParticleContainer" )

print "## installing reference files..."
installRefFiles( [
    "mc.aod.symlinks.ref",
    "mc.aod.pysymlinks.ref",
    ] )

evtMax = 5

###-----------------------------------------------------
print "## Testing [writing]..."
jobOptions = [
    ChapPy.JobOptionsCmd( "OUTPUT=\"%s\"" % workDir("mc.aod.symlinks.pool") ),
    ChapPy.JobOptions( "McParticleTests/symlinkTest_mcAod_jobOptions.py" ),
    ]

athena = ChapPy.Athena( jobOptions = jobOptions,
                        logFile = workDir("mc.aod.symlinks.pool.log"),
                        checkLeak  = False )
athena.EvtMax = evtMax
athena.run()


###-----------------------------------------------------
print "\n"
print "#"*80
bench += doPostCheck( "McAod symlinks",
                      workDir("mc.aod.symlinks.ref"),
                      workDir("mc.aod.symlinks.pool.log"),
                      "grep \"^McAodSymLinkTests\"" )


###-----------------------------------------------------
print ""
print "#"*80
print "## testing py-symlinks for TruthParticleContainer..."
print "## Testing [writing]..."
jobOptions = [
    ChapPy.JobOptionsCmd( "OUTPUT=\"%s\"" % workDir("mc.aod.pysymlinks.pool") ),
    ChapPy.JobOptionsCmd( "ALGMODE='py'" ),
    ChapPy.JobOptions( "McParticleTests/symlinkTest_mcAod_jobOptions.py" ),
    ]

athena = ChapPy.Athena( jobOptions = jobOptions,
                        logFile = workDir("mc.aod.pysymlinks.pool.log"),
                        checkLeak  = False )
athena.EvtMax = evtMax
athena.run()


###-----------------------------------------------------
print "\n"
print "#"*80
bench += doPostCheck( "McAod py-symlinks",
                      workDir("mc.aod.pysymlinks.ref"),
                      workDir("mc.aod.pysymlinks.pool.log"),
                      "grep \"^Py:PyMcAodSymLinkTests\"" )
print ""
print "#"*80
bench.printStatus()
print "## Bye."
print "#"*80
