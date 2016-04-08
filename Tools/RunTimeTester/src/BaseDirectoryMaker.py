# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Tools        import mkdirIfNew, emptyDirMkDir

#===== RTT MODULES =====
from exc2string2              import exc2string2
#===== PYTHON MODULES ==
import sys, os

"""
A class to make Work, Results and Run directories.

User             calls: makeDirs
makeDirs         calls: makeBaseDirs - sets up non minder dependent parts of
                                work and results directory tree
"""

class BaseDirectoryMaker:
    def __init__(self, paths, logger):
        self.paths  = paths
        self.logger = logger
                

    def makeDir(self,parentDir):
        try:
            # create base directories for work directory and results
            # directory trees
            if os.path.exists(parentDir):
                self.logger.debug('Emptying base directory %s' % parentDir)
                emptyDirMkDir(parentDir)
            else:
                self.logger.debug('Creating base directory %s' % parentDir)
                mkdirIfNew(parentDir)

            # to permit signal in Tester to know if directories have been made
            self.status =  True
            msg = self.logger.debug('Base directory %s ready' % parentDir)
        except Exception, e:
            msg = 'Error handling base directory %s\nException%s\nTraceback\n%s' % (parentDir, str(e), exc2string2())
            self.logger.error(msg)            

    def makeDirs(self):
        resDir = self.paths.resultsDirs['packages']
        wrkDir = self.paths.workDirs['packages']

        self.makeDir(wrkDir)
        if resDir != wrkDir:
                self.makeDir(resDir)


class CERNSLC4BaseDirectoryMaker:
    'do nothing - all the base directories are pre-made'
    def __init__(self, paths, logger):
        self.status =  True
        self.logger = logger
        self.paths  = paths
        
    def makeDirs(self):
        resDir = self.paths.resultsDirs['packages']
        wrkDir = self.paths.workDirs['packages']
        
        emptyDirMkDir(wrkDir)
        if resDir != wrkDir:
            emptyDirMkDir(resDir)
            
        self.logger.debug('Emptied the directories %s and %s' % (self.paths.resultsDirs['packages'],
                                                                 self.paths.workDirs['packages']))
