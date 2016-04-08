# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RTTConfigParser           import * 
#from Paths                     import NoPackagePaths
from RunTimer                  import RunTimer
from formatCollection          import formatCollection
from Paths                     import NoPackagePaths
#from UserStuffRetrieverProject import UserStuffRetrieverProject
#from UserStuffRetrieverProject import *
#from UserStuffRetriever2       import UserStuffRetriever
from Factory_UserStuffRetriever import UserStuffRetrieverFactory
import sys, os
    
import logging
logger = logging.getLogger('rtt')

class USRBase:
    def __init__(self, rttConfFile,
                 subString = '_TestConfiguration'):
        self.rttConfFile = rttConfFile
        self.subString   = subString


class USRProjectMaker:
    def __init__(self, rttConfFile,
                 subString = '_TestConfiguration'):
       
        self.rttConfFile = rttConfFile
        self.subString   = subString
    
    def makeUSR(self):
        logger.info( 'Making UserStuffRetriever Project ')
        cfg = RTTConfiguration(self.rttConfFile)
        confDict = cfg.config
        #extend oringinal Config
        confDict['originalBranch'] = cfg.originalBranch
        confDict['findNightly']    = cfg.findNightly
        confDict['valid']          = cfg.valid
        confDict['isNightly']       = cfg.isNightly

        if confDict['release'] == 'nightly':
            lcr  ='/afs/cern.ch/atlas/software/builds/nightlies/dev/AtlasOffline/latest_copied_release'
            confDict['release'] = os.readlink(lcr)
            print 'nightly Tag is '+str(os.readlink(lcr))
        
        print formatCollection(confDict)
        #print cfg.__str__()
        
        paths = NoPackagePaths('RunTimeTester-00-00-96',
                                confDict, RunTimer(60*60*20 ,logger ),
                                'outLog.log', logger)

        #usr    = UserStuffRetrieverProject(paths,
        #usr     = UserStuffRetrieverRelease(paths,
        #                                   self.subString)
        #usr    = UserStuffRetrieverFactory(logger).create(paths, self.subString, confDict)
        usr    = UserStuffRetrieverFactory(logger).create(paths, self.subString)
        return usr
        

class USRMonolithicMaker:
    def __init__(self, rttConfFile,
                 subString = '_TestConfiguration'):

        self.rttConfFile = rttConfFile
        self.subString   = subString

    def makeUSR(self):
        logger.info('Making UserStuffRetriever Monolithic')
        confDict = rttConfiguration(self.rttConfFile)
            
        logger.debug( 'rttConfig :'+formatCollection(str(confDict))) 
        paths    = NoPackagePaths('RunTimeTester-00-00-89',
                           confDict, RunTimer(60*60*20 ,logger ),
                                'outLog.log')
        usr   = UserStuffRetriever(paths, self.subString, confDict )

        return usr
    

if __name__ == '__main__':

    from DSMConfigReader import DSMConfigReader

    cr  = DSMConfigReader('/home/en/RTT/dsmProject/dsmCfg.xml')
    cfg = cr.readDSMcfg()
    print str(cfg)
    sys.path.append(cfg['rttsrc'])
    usrMaker = USRProjectMaker(cfg['rttcfg'])
    #usrMaker = USRMonolithicMaker(cfg['rttcfg'])
    
    usr = usrMaker.makeUSR()

    print str(usr)
    
