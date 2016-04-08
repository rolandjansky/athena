# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import string,logging,os
from xml.dom.minidom import *
from getText import getText

logger = logging.getLogger('rtt')

class DSMConfigReader:
    def __init__(self, dsmcfg):
        self.dsmcfg = dsmcfg
        self.rttcfg = ''

    def readDSMcfg(self):
        logger.debug('Reading dsm configurations')
        cfgDict = {}
        dom     = parse(self.dsmcfg)
        expectedTags = ['mode','datasetCatalog','rttsrc',
                        'reportDir','rttcfg','datasetDir',
                        'stageArea','rttLibDir', 'logDir']
        for t in expectedTags:
            tag = dom.getElementsByTagName(t)
            cfgDict[t]=string.strip(getText(tag[0].childNodes))

        cfgDict = self.addRTTcfg(cfgDict['rttcfg'], cfgDict)
        return cfgDict

    def addRTTcfg(self, rttcfg, cfgDict):
        dict = cfgDict
        dom  = parse(rttcfg)
        tags = ['release', 'releaseType', 'site'] #tags of interest from rtt cfg
        for t in tags:
            tag     = dom.getElementsByTagName(t)
            dict[t] = string.strip(getText(tag[0].childNodes))

        if dict['release'] == 'nightly':
            lcr  ='/afs/cern.ch/atlas/software/builds/nightlies/dev/AtlasProduction/latest_copied_release'
            dict['release'] = os.readlink(lcr)
            logger.debug ('nightly Tag is '+str(os.readlink(lcr)))

        return dict

if __name__ == '__main__':
    cr = DSMConfigReader('/home/en/RTT/dsmProject/dsmCfg.xml')
    cfg = cr.readDSMcfg()

    print cfg


       
        
