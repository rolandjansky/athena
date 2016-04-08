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

import shelve

from LoggerSetup import setupLoggers
import logging
os.environ['RTTLogDir'] = setupLoggers(os.environ['RTTLogDir'])
logger = logging.getLogger('rtt')

def handleError(e):
    print "%s  error - please fix:\nException:\n%s\nTraceback:\n%s" % (e.__class__.__name__, str(e), exc2string2())
    sys.exit(0)

class Runner:
    def __init__(self):
        self.RTTSrcDir                        = os.environ['RTTSrcDir']
        self.RTTLibDir                        = os.environ['RTTShareDir']
        self.topLevelRTTconfFileLocation      = os.environ['RTTCfgFile']
        self.logDir                           = os.environ['RTTLogDir']
        self.useDB                            = os.environ.has_key('RTTFactoryUseDB')

    def makeTester(self):
        db = {}
        if self.useDB:
            dbPath = 'RTTFactory.dbm'
            db =  shelve.open(dbPath, 'c')
            logger.info('Using an RTT object database for this run: %s' % dbPath)
        else:
            db = {}
            logger.info('NOT using an RTT object database for this run')
                           
        
        try:
            factory  = RTTFactory(self.RTTSrcDir, self.RTTLibDir, self.topLevelRTTconfFileLocation, db, self.logDir)
        except Exception, e:
            handleError(e)

        # logger = factory.logger
        
        adjustPathEnvVar(factory.siteSpecificsObj)
        
        sys.path.append(factory.noPackagePaths.RTTLibDir)
        
        installWebFiles(factory.noPackagePaths.RTTLibDir,
                        factory.noPackagePaths.resultsBasePath)
        installBaseDirectories(factory.noPackagePaths, logger)
        installStartTimeStamp(factory.noPackagePaths,  factory.startTime2, logger)
        
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
    t = r.makeTester()
    try:
        sys.argv[1]
        print 'Not running the instantiated Tester object as you gave a CL argument.'
    except:
        r.runTester(t)
    
