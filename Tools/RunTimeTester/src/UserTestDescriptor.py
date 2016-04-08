# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from formatCollection import formatCollection

import logging
logger = logging.getLogger('rtt')
smallLogger = logging.getLogger('rtt.standalone')
# -------------------------------------------------------------------------

class UserTestDescriptor:
    def __init__(self, runInAtlasEnv, pos, moduleName, testName, argDict):
        self.moduleName    = moduleName
        self.testName      = testName
        self.argDict       = argDict
        self.position      = str(pos)
        self.runInAtlasEnv = str(runInAtlasEnv)

    def addParameter(self, item):
        self.argDict[str(item[0])] = item[1]

    def __str__(self):
        str  = 'TestDescriptor: module name %s test name %s \n' % (self.moduleName, self.testName)
        str += 'argDict: \n%s' % formatCollection(self.argDict)
        return str
