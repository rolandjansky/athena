#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
FileComparatorRunnerJet this module contains:
classes:
FileComparatorRunnerJet - runs fileComparator as a subprocess.
"""

######################################################
#                                                    #
# Do NOT bind any objects (self.xxx) which contain   #
# file objects (such as self.logger in this class    #
# otherwise cannit shelve the objects                #
# Instead, unload the necessary variables in         #
# __init__                                           #
#                                                    #
######################################################
import sys, os
from subprocess import (Popen,
                        PIPE)

from ExeRunnerBase import ExeRunnerBase
from TestResult import TestResult

sys.path.append(os.path.dirname(sys.modules[__name__].__file__))

class FileComparatorRunnerJet(ExeRunnerBase):

    def __init__(self, paramDict):

        self.testFile = paramDict['testFile']
        self.refFile = paramDict['refFile']
        testIdentifier = paramDict['testIdentifierObj']
        outputFile = paramDict['testOutputFile']
        descriptor = paramDict['JobDescriptor']
       
        release = descriptor.paths.branch
        self.refFile = '%s%s.txt' % (self.refFile, release)

        cmd = 'fileComparator.py %s %s' % (self.testFile, self.refFile)

        ExeRunnerBase.__init__(self,
                               cmd=cmd,
                               testIdentifier=testIdentifier,
                               outputFile=outputFile)

    def run(self):

        for f in (self.testFile, self.refFile):
            if not os.path.exists(f):
                msg = '%s.run missing file %s: ' % (self.__class__.__name__,
                                                    f)
                self.testResults.append(TestResult(cmd=self.cmd,
                                                   comment=msg))
                return

        ExeRunnerBase.run(self)
