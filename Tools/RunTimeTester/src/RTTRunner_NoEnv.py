# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import os
import os.path

from RTTFactory                  import RTTFactory
from RTTFactory2                 import RTTFactory2
from installWebFiles             import installWebFiles
from installBaseDirectories      import installBaseDirectories
from installStartTimeStamp       import installStartTimeStamp
from adjustPathEnvVar            import adjustPathEnvVar
from exc2string2                 import exc2string2
from makeLocalLog                import makeLocalLog
from Tools                       import now
from Tools                       import mkdirIfNew

import shelve

def handleError(e):
    print "%s  error - please fix:\nException:\n%s\nTraceback:\n%s" % (e.__class__.__name__, str(e), exc2string2())
    sys.exit(0)

class Runner:
    def __init__(self, RTTSrcDir, RTTShareDir, RTTCfgFile, RTTLogDir, useDB, name):

        #self.RTTSrcDir                        = os.environ['RTTSrcDir']
        #self.RTTLibDir                        = os.environ['RTTShareDir']
        #self.topLevelRTTconfFileLocation      = os.environ['RTTCfgFile']
        #self.logDir                           = os.environ['RTTLogDir']
        #self.useDB                            = os.environ.has_key('RTTFactoryUseDB')

        self.RTTSrcDir                        = RTTSrcDir
        self.RTTLibDir                        = RTTShareDir
        self.topLevelRTTconfFileLocation      = RTTCfgFile
        self.logDir                           = os.path.normpath(os.path.join(RTTLogDir, 'logfiles_'+now()))
        self.useDB                            = useDB
        self.name                             = name

        mkdirIfNew(self.logDir)
        self.logger = makeLocalLog(self.logDir,
                                   'RTTRunnerError',
                                   'RTTRunnerWarning',
                                   'RTTRunnerInfo',
                                   'RTTRunnerDebug',
                                   name+'_log'
                                   )


    def makeTester(self):
        db = {}
        if self.useDB:
            dbPath = 'RTTFactory.dbm', 'c'
            db =  shelve.open(dbPath, 'c')
            self.logger.info('Using an RTT object database for this run: %s' % dbPath)
        else:
            db = {}
            self.logger.info('NOT using an RTT object database for this run')
                           
        
        try:
            factory  = RTTFactory(self.RTTSrcDir, self.RTTLibDir, self.topLevelRTTconfFileLocation, db, self.logDir)
        except Exception, e:
            handleError(e)

        # logger = factory.logger
        
        adjustPathEnvVar(factory.siteSpecificsObj)
        
        sys.path.append(factory.noPackagePaths.RTTLibDir)
        
        installWebFiles(factory.noPackagePaths.RTTLibDir,
                        factory.noPackagePaths.resultsBasePath)
        installBaseDirectories(factory.noPackagePaths, self.logger)
        installStartTimeStamp(factory.noPackagePaths,  factory.startTime2, self.logger)
        
        try:
            factory2 = RTTFactory2(factory, self.logDir, db)
        except Exception, e:
            handleError(e)
        
        factory2.closeDownKeyMaker.write() # install file to allow operator to close down the RTT
        
        tester = factory2.tester

        if self.useDB: db.close()
        return tester


    def runTester(self, tester): tester.execute()

if __name__ == '__main__':
    r = Runner()
    r.runTester(r.makeTester())
    
