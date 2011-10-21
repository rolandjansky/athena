# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @purpose gather some helpers...
import unittest
import tempfile, os, commands, atexit, shutil

### helper methods ------------------------------------------------------------
def safe_remove(fname):
    if os.path.exists(fname) and \
       (os.path.isfile(fname) or os.path.islink(fname)):
        atexit.register(os.unlink, fname)

## For compatibility with ATN tests
from TestTools.iobench import workDir
def installRefFiles( fileNames, quiet=True ):
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
            if not quiet:
                print " -%s" % workDir(refFile)
            continue
        try:
            shutil.move(refFile, workDir(refFile))
            if not quiet:
                print " -%s" % workDir(refFile)
        except OSError,err:
            print "## ERROR: could not install [%s] into [%s]" %\
                  ( refFile, workDir(refFile) )
            print "## reason:\n",err
            continue
    return

def _make_jobo(job):
    jobo = tempfile.NamedTemporaryFile(suffix='.py')
    jobo.writelines([l.strip()+os.linesep for l in job.splitlines()])
    jobo.flush()
    return jobo

def _run_jobo(self, job):
    jobo = _make_jobo(job)
    cmd = ' '.join([self.app, jobo.name])
    sc,out = commands.getstatusoutput(cmd)
    self.failUnless(sc==0, 'problem running jobo:\n%s'%out)
    jobo.close()
    return out

### capture the usual setUp/tearDown
class PyAthenaTestCase (unittest.TestCase):
    def setUp (self):
        # base class setUp
        super(PyAthenaTestCase,self).setUp()
        sc, self.app = commands.getstatusoutput('which athena.py')
        self.failUnless(sc==0, 'could not find \'athena.py\' !')
    _run_jobo = _run_jobo
    
