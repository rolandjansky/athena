#!/usr/bin/env python

import user
import sys
import os
import commands
from AthenaCommon import ChapPy

###-----------------------------------------------------
EVTMAX = 5

###-----------------------------------------------------
## For compatibility with ATN tests
def workDir( fileName ):
    if os.environ.has_key('ATN_WORK_AREA'):
        workArea = os.environ['ATN_WORK_AREA']
    else:
        workArea = "/tmp"
        pass
    if not os.path.exists(workArea):
        os.makedirs(workArea)
    return os.path.join( workArea, fileName )

###-----------------------------------------------------
def doValidation( validationName, refFileName, chkFileName, chkFilter ):
    print "## Validation of [%s]" % validationName
    print "## ref:    %s"   % refFileName
    print "## chk:    %s"   % chkFileName
    print "## filter: [%s]" % chkFilter
    sc, out = commands.getstatusoutput( "cat %s | %s | diff -u %s -" % \
                                        ( chkFileName, chkFilter,
                                          refFileName ) )
    if sc == 0 and len(out) == 0: print "==> Validation [OK]"
    else:                         print "==> Validation [ERROR]\n",\
                                        "*"*80,out,"*"*80
    return sc, out

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

###-----------------------------------------------------
print "#"*80
print "## testing slimming exercize..."
print "## installing reference files..."
installRefFiles( [ "AthExThinning_makeData.ref",
                   "pyWriteSlimmedData.ref",
                   "ReadSlimmedData.ref",
                   "ReadNonSlimmedData.ref",
                   ] )

###-----------------------------------------------------
print "\n"
print "#"*80
print "## Preparing input data..."
jobOptions = [
    ChapPy.JobOptionsCmd( "OUTPUT=\"%s\"" % workDir("my.pydata.toslim.pool") ),
    ChapPy.JobOptions( "AthExThinning/AthExThinning_makeData.py" ),
    ]

athena = ChapPy.Athena( jobOptions = jobOptions,
                        logFile    = workDir( "my.pydata.toslim.pool.log" ),
                        checkLeak  = False )
athena.EvtMax = EVTMAX
athena.run()

###-----------------------------------------------------
print "\n"
print "#"*80
sc,out = doValidation( "Input Data",
                       workDir("AthExThinning_makeData.ref"),
                       workDir("my.pydata.toslim.pool.log"),
                       "grep '^CreateData' | grep INFO " )
if sc != 0:
    raise SystemExit("ERROR")

###-----------------------------------------------------
print "\n"
print "#"*80
print "## Testing [writing slimmed data]..."
jobOptions = [
    ChapPy.JobOptionsCmd( "INPUT=[\"%s\"]" % workDir("my.pydata.toslim.pool") ),
    ChapPy.JobOptionsCmd( "OUTPUT=\"%s\""  % workDir("pyslimmed.data.pool") ),
    ChapPy.JobOptionsCmd( "ALGMODE=\"py\"" ),
    ChapPy.JobOptions( "AthExThinning/WriteSlimmedData_jobOptions.py" ),
    ]

athena = ChapPy.Athena( jobOptions = jobOptions,
                        logFile    = workDir("pyslimmed.data.pool.log"),
                        checkLeak  = False )
athena.EvtMax = -1
athena.run()

###-----------------------------------------------------
print "\n"
print "#"*80
sc,out = doValidation( "PyWriteSlimmedData",
                       workDir("pyWriteSlimmedData.ref"),
                       workDir("pyslimmed.data.pool.log"),
                       "grep \"^Py:Slimmer \"" )
if sc != 0:
    raise SystemExit("ERROR")

###-----------------------------------------------------
print "\n"
print "#"*80
print "## Testing [reading slimmed data]..."
jobOptions = [
    ChapPy.JobOptionsCmd( "INPUT=[\"%s\"]" % workDir("pyslimmed.data.pool") ),
    ChapPy.JobOptionsCmd( "OUTPUT=\"%s\""  % workDir("reaccessed.pyslimmed.data.pool") ),
    ChapPy.JobOptions( "AthExThinning/ReadSlimmedData_jobOptions.py" ),
    ]

athena = ChapPy.Athena( jobOptions = jobOptions,
                        logFile    = workDir("reaccessed.pyslimmed.data.pool.log"),
                        checkLeak  = False )
athena.EvtMax = -1
athena.run()

###-----------------------------------------------------
print "\n"
print "#"*80
sc,out = doValidation( "ReadSlimmedData",
                       workDir("ReadSlimmedData.ref"),
                       workDir("reaccessed.pyslimmed.data.pool.log"),
                       "grep \"^Py:ReadFatObject \"" )
if sc != 0:
    raise SystemExit("ERROR")


###-----------------------------------------------------
print "\n"
print "#"*80
print "## Testing [reading non-slimmed data]..."
jobOptions = [
    ChapPy.JobOptionsCmd( "INPUT=[\"%s\"]" % workDir("non.pyslimmed.data.pool") ),
    ChapPy.JobOptionsCmd( "OUTPUT=\"%s\""  % workDir("reaccessed.non.pyslimmed.data.pool") ),
    ChapPy.JobOptions( "AthExThinning/ReadNonSlimmedData_jobOptions.py" ),
    ]

athena = ChapPy.Athena( jobOptions = jobOptions,
                        logFile    = workDir("reaccessed.non.pyslimmed.data.pool.log"),
                        checkLeak  = False )
athena.EvtMax = -1
athena.run()

###-----------------------------------------------------
print "\n"
print "#"*80
sc,out = doValidation( "ReadNonSlimmedData",
                       workDir("ReadNonSlimmedData.ref"),
                       workDir("reaccessed.non.pyslimmed.data.pool.log"),
                       "grep \"^Py:ReadFatObject \"" )
if sc != 0:
    raise SystemExit("ERROR")

print ""
print "## [All tests completed]"
print "## Bye."
print "#"*80
