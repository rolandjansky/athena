# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
RetryTester - see if a failed job warants a retry - an attempt to
overcome networl connection problems
"""
import os
import re
from RTTpath                 import RTTpath

# -------------------------------------------------------------------------
from MethodTracer        import MetaTracer
# uncomment to monitor all method calls (shows args + return values)
# __metaclass__ = MetaTracer


def match(patterns, lines):
    line = None
    for p in patterns:
        cp = re.compile(p)
        line = None

        for l in lines:
            if re.search(cp,l):
                line = l
                break
                
        if line: break

    return line

def allMatches(patterns, lines):
    matchedLines = []
    for p in patterns:
        cp = re.compile(p)
        matchedLines.extend([l for l in lines if re.search(cp,l)])
    return matchedLines

def getFromEnvLog(runPath,envVar,pathSep=':'):
    envLogName = os.path.join(runPath, 'env.log')
    if os.path.exists(envLogName):
        ifile = open(envLogName, 'r')
        content = ifile.readlines()
        ifile.close()
    else:
        return []

    envVar = envVar.strip() + '='
    libstr = ''
    for line in content:
        if line.startswith(envVar):
            libstr = line[len(envVar):]
            break
    if not libstr: return []
        
    return libstr.split(pathSep)

class RetryTester:
    #def __init__(self, log, elog, logger, sharePath, runPath):
    def __init__(self, log, logger, sharePath, runPath):
        self.logName   = log # full abs path to log file (which doesn't exist yet)
        # self.elogName  = elog
        self.log       = []
        # self.elog      = []
        self.logger    = logger
        self.sharePath = sharePath
        self.runPath   = runPath
        
    def shouldRetry(self):
        if not self.log:            
            if os.path.exists(self.logName):
                ifile = open(self.logName,'r')
                self.log = ifile.readlines()
                ifile.close()

        # if not self.elog:
        #  if os.path.exists(self.elogName):
        #     self.elog = open(self.elogName,'r').readlines()
        
        if self.commandNotFound():        return True
        if self.missingJobOptionsExist(): return True
        if self.missingSharedLibrary():   return True
        
        return False
            

    # -------------------------------------------------------------------------
    # -------------------------------------------------------------------------
    
    def missingJobOptionsExist(self):
        """
        Looks for missing job options files in the log files. If Athena
        complains of such a file, check to see if it really does not exist
        """

        patterns = ['^AthenaCommon.Include.IncludeErr',
                    '^IOError: [Errno 2] No such file or directory:',
                    '^IncludeError: include file']
        # line = match(patterns, self.elog)
        line = match(patterns, self.log)
        if not line: return False
        
        tokens = line.split()
        jos = match(['.py$'], tokens)

        if not jos: return False
        
        # Note that this makes the assumption (for now, always borne out)
        # that the 'missing' job options file is in the same project as the
        # one trying to include it, and that these are both in the same project
        # as the package under test.

        if os.path.isabs(jos) and not os.path.exists(jos): return False
        
        joSearchPaths = getFromEnvLog(self.runPath,'JOBOPTSEARCHPATH',',')
        if not joSearchPaths:
            self.logger.warning('No search paths available in which to search for missing job options!')
            self.logger.warning('(This is either because RTT could not find env.log file, or JOBOPTSEARCHPATH missing/empty in that file.)')
            return False                                 

        for p in joSearchPaths:
            p = p.strip()
            fullPath = RTTpath(p,jos)
            if os.path.exists(str(fullPath)):
                m  = 'Apparent connection error: Found the following job options with'
                m += ' Athena include but which exist: %s' % str(jos)
                self.logger.info(m)        
                return True
        return False


    # -------------------------------------------------------------------------

    def commandNotFound(self):
        """
        Checks for 'not found' commands such as athena, cmt, etc.
        """

        patterns = [
            '^#CMT> Warning: package CMT * /afs/cern.ch/sw/contrib not found',
            'athena.py: command not found',
            'cmt: command not found',
            'pool_insertFileToCatalog: command not found'
            ]
        
        # lineFound = match(patterns, self.elog)
        lineFound = match(patterns, self.log)
        if lineFound:
            m = 'Apparent connection error:\n%s' % lineFound
            self.logger.info(m)
            return True
        return False

    # -------------------------------------------------------------------------
    
    def missingSharedLibrary(self):
        """
        Looks for missing shared library files in the log files. If Athena
        complains of such a file, check to see if it really does not exist
        """
        patterns = ['^System::doLoad: ERROR:']
        # lines = allMatches(patterns, self.elog)
        lines = allMatches(patterns, self.log)

        if not lines: return False

        soFiles = []
        for l in lines:
            #example line:
            #System::doLoad: ERROR:libMuonAlgs.so: cannot open shared object file
            #: No such file or directory'
            #
            tokens = l.split()
            soFiles.extend([t[:-1] for t in tokens if t.endswith('.so:')])

        # print soFiles
        if not soFiles: return False

        # make a list of libary paths
        libDirs = getFromEnvLog(self.runPath,'LD_LIBRARY_PATH')
        if not libDirs:
            self.logger.warning('No lib paths in which to search for missing shared library!')
            self.logger.warning('(This is either because RTT could not find env.log file, or LD_LIBRARY_PATH missing/empty in that file.)')
            return False
        
        fullLibPaths = []
        for lib in soFiles:
            possiblePaths  = [os.path.join(libDir, lib) for libDir in libDirs]
            foundLocations = [l for l in possiblePaths if os.path.exists(l)]

            # looked everywhere on LD_LIBRARY_PAH, but did not find the
            # library. It is genuinely not present so do not retry
            if foundLocations: 

                m  = 'Apparent afs connection error: '
                m += 'Found the following so files reported'
                m += ' missing but in fact present %s' % str(foundLocations)
                self.logger.info(m)

        
                return True

        return False



