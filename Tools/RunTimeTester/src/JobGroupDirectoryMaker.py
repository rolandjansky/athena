# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Tools        import rmdirMkdir, changePerm, mkdirIfNew
import Tools2

#===== RTT MODULES =====
from JobGroupKits            import JobGroupKits
from Factory_ScriptWriter    import ScriptWriterFactory
from releaseToNum            import releaseToNum
from getInheritanceHeirarchy import getInheritanceHeirarchy
from formatCollection        import formatCollection
from exc2string2             import exc2string2

#===== PYTHON MODULES ==
import os, shutil, anydbm, string
from os.path              import join
import shelve
import xml.dom.ext
import cPickle

"""
A class to make Work, Results and Run directories.
User             calls: makeJobGroupDirs - extracts results and working
                                           directory names (minder dependent)
                                           

makeJobGroupDirs calls: createJobGroupDir - for each of [resDir, workDir]
                                            to actually create the directory

                 calls: setupRunDir
"""

# -------------------------------------------------------------------------

class JobGroupDirectoryMaker:
    def __init__(self, descSetUpRunDirFn, logger):
        self.logger = logger
        self.descSetUpRunDirFn = descSetUpRunDirFn
        
    def createJobGroupDir(self, pathToDirectory):
        """
        create run and results directory trees. This function is sometimes
        overridden by a null function in derived classes.
        """

        try:
            rmdirMkdir(pathToDirectory)
            self.logger.debug('making '+pathToDirectory)
        except Exception, e:
            errmsg = exc2string2()
            self.logger.error(errmsg)

    def makeJobGroupDirs(self, runPath, resPath):
        """
        makes  the run and results directories.
        fills the run directories with necessary files before laucnching jobs 
        """
        
        self.createJobGroupDir(runPath)
        self.logger.debug('Making run directory %s', runPath)

        self.logger.debug('Making results directory %s', resPath)
        self.createJobGroupDir(resPath)

    # --------------------------------------------------------------------------

    def writeTestRunnerScript(self, shareArea, dbPath, runPath):
        script = [
            'import sys',
            'sys.path.append("'+shareArea+'")',
            'import shelve, RTTTestRunner',
            'from exc2string2 import exc2string2',
            'import RttLibraryTools',
            'db = shelve.open("%s")' % dbPath,
            'keys = db.keys()',
            'print "Keys before order: %s" % str(keys)',
            'keys.sort()',
            'print "Keys after order: %s" % str(keys)',
            'for k in keys:',
            '   print "Accessing key: %s" % str(k)',
            '   t = db[k]',
            '   try:',
            '      t.run()',
            '   except:',
            '      m = exc2string2()',
            '      t.error=True',
            '      t.errmsg=m',
            '      print m',
            ' ',
            '   print t.testId,": ",t.result',
            'db.close()'
           ]
        script = string.join(script, '\n')
        fn = 'testRunner.py'
        fn = os.path.join(runPath, fn)
        of = open(fn, 'w')
        of.write(script)
        of.close()
            
            
# --------------------------------------------------------------------------

    def setupRunDir(self, jMinder):
        
        """
        Creates run directory (obtained from the descriptor)
        
        Uses JobGroupKits to establish what is needed to be copied to it.
        Uses ScriptWriter to write a run script to it.

        This version does not write a requirements file into the run
        directory.
        """
        self.logger.debug("DirectoryMakerProject: entering setupRunDir")
        self.logger.debug("setupRunDir processing: %s" %(jMinder.identifiedName))


        runPath = jMinder.runPath

        def writeScript(fn, script):
            ofn = os.path.join(runPath, fn)
            of = open(ofn, 'w')
            of.write(script)
            of.close()
            changePerm(ofn, 0777)

        dicts = jMinder.scripts.values()
        [writeScript(d['scriptName'], d['script']) for d in dicts]
    

        #######################################################################
        # handle aux files (= files needed in the run dir to run the job)
        #######################################################################


        # if the aux file is a sub directory of the Install directory, make
        # the corresponding directory in the work directory
        dict = {}
        for path in jMinder.auxFiles:
            base, fn = os.path.split(path)
            if base: dict[base] = fn

        dirs = dict.keys()
        dirs = [os.path.join(runPath, d) for d in dirs]
        dirs = [mkdirIfNew(d) for d in dirs]
        
        auxFileDirectory = jMinder.shareArea

        # explicitly remove the package XML file, we add it back later
        auxFiles = [a for a in jMinder.auxFiles if a.strip() != os.path.basename(jMinder.confFile).strip()]
                
        # copy the files from the install area (sub) dir to the run (sub) dir.
        # for f in jMinder.auxFiles:
        for f in auxFiles:

            src  = join(auxFileDirectory, f)
            dest = join(runPath, f)

            if not os.path.exists(src):
                self.logger.error("Non-existent aux file: %s , skipping" %  src )
                continue
            
            # shutil.copy(src, dest)
            os.symlink(src, dest)
            changePerm(dest, 0777)

        # now add the package XML file
        confFileName = os.path.basename(jMinder.confFile).strip()
        src = jMinder.confFile.strip()
        dst = join(runPath, confFileName)
        os.symlink(src, dst)
        changePerm(dst, 0777)
                    

        # write out a python script to run the RTT test wrappers
        self.writeTestRunnerScript(jMinder.shareArea, jMinder.testDBPath, jMinder.runPath)

        # Output the RTT argdict pickle file
        pickleDict = jMinder.rttArgDictPickle
        cPickle.dump(pickleDict['what'], open(pickleDict['where'], 'wb'))

        # Now perform tasks specific to the different descriptors
        self.descSetUpRunDirFn()

        # make a note of the files in the run directory
        # at this point - in case a rerun is needed
        of = open(os.path.join(runPath, 'listDir.txt'), 'w')
        [of.write(p+'\n') for p in os.listdir(runPath)]
        of.close()

        self.logger.debug("returning from setupRunDir()")

        


class JobGroupDirectoryMakerWatcher(JobGroupDirectoryMaker):
    def __init__(self, logger):
        JobGroupDirectoryMaker.__init__(self, logger)
        
    def setupRunDir(self, jMinder):
        JobGroupDirectoryMaker.setupRunDir(self, jMinder)
        workerRunPaths = [minder.runPath for minder in jMinder.confederation]
        histsFile = open(join(jMinder.runPath,'histLocation.txt'),'w')
        [histsFile.write(path+'\n') for path in workerRunPaths]
        histsFile.close()


