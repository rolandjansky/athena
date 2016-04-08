# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RTTConfigParser            import * 
from formatCollection           import formatCollection
from Paths                      import NoPackagePaths
from Factory_UserStuffRetriever import UserStuffRetrieverFactory
from ParametersForCMT           import ParametersForCMT
from makeRTTDOM                 import makeRTTDOM
import sys, os
    
import logging
logger = logging.getLogger('rtt')

class USRBase:
    def __init__(self, rttConfFile,
                 subString = '_TestConfiguration', logger=''):
        self.rttConfFile = rttConfFile
        self.subString   = subString
        self.logger      = logger


class USRProjectMaker:
    def __init__(self, rttConfFile,logger, copierConfig,
                 subString = '_TestConfiguration'):
        
        self.copierConfig     = copierConfig
        self.logger           = logger
        self.rttConfFile      = rttConfFile
        self.subString        = subString
        self.summaryDocument  = makeRTTDOM('Summary')
    
    def makeUSR(self):
        logger.info( 'Making UserStuffRetriever Project ')
        cfg = RTTConfiguration(self.rttConfFile)
        self.confDict = cfg.config

        #extend original Config
        self.confDict['originalBranch'] = cfg.originalBranch
        self.confDict['findNightly']    = cfg.findNightly
        self.confDict['valid']          = cfg.valid
        self.confDict['isNightly']      = cfg.isNightly
        
        self.RTTSrcDir        = self.copierConfig.get('rttsrc')
        self.RTTLibDir        = self.copierConfig.get('rttLibDir')
        self.logFileDir       = self.copierConfig.get('logDir')
        
        if self.confDict['release'] == 'nightly':           
            lcr  = '/afs/cern.ch/atlas/software/builds/nightlies/'
            lcr += self.confDict['originalBranch']
            lcr += '/AtlasProduction/latest_copied_release'
            
            self.confDict['release'] = os.readlink(lcr)
            self.logger.info('nightly Tag is '+str(os.readlink(lcr)))
        
        self.logger.debug(formatCollection(self.confDict))
        
        self.parametersForCMT = ParametersForCMT(self.confDict, self.logger)

        self.paths = NoPackagePaths(self.RTTSrcDir,
                                    self.RTTLibDir,
                                    self.confDict,
                                    self.logFileDir,
                                    self.summaryDocument.createElement,
                                    self.summaryDocument.createTextNode,
                                    self.parametersForCMT,
                                    self.logger)
       
        usr    = UserStuffRetrieverFactory(logger).create(self.paths, self.subString)
        return usr
        
if __name__ == '__main__':

    from DSMConfigReader import DSMConfigReader

    cr  = DSMConfigReader('/home/en/RTT/dsmProject/dsmCfg.xml')
    cfg = cr.readDSMcfg()
    print str(cfg)
    sys.path.append(cfg['rttsrc'])
    usrMaker = USRProjectMaker(cfg['rttcfg'])
    
    usr = usrMaker.makeUSR()

    print str(usr)
    
