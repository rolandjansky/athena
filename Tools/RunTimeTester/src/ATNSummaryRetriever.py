# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import urllib, urlparse, xml.dom.minidom
from Evaluate import Evaluate

class ATNSummaryRetriever:
    def __init__(self, cmtConfig, originalBranch, release, logger):

        pathFrags = {
            ('i686-slc3-gcc323-opt',  'bugfix'): 'nicos_web_areaBF32BS3ProdOpt',
            ('i686-slc4-gcc34-opt',  'bugfix'): 'nicos_web_areaBF32BS4ProdOpt',
            ('i686-slc3-gcc323-opt',  'dev'):    'nicos_web_areaDev32BS3ProdOpt',
            ('i686-slc4-gcc34-debug', 'dev'):    'nicos_web_areaDev32BS4ProdDbg',
            ('i686-slc4-gcc34-opt',   'dev'):    'nicos_web_areaDev32BS4ProdOpt',
            ('i686-slc4-gcc34-dbg',   'val'):    'VALWebArea/nicos_web_areaVAL32BS4ProdDbg',
            ('i686-slc4-gcc34-opt',   'val'):    'VALWebArea/nicos_web_areaVAL32BS4ProdOpt',
            ('x86_64-slc4-gcc34-opt', 'dev'):    'nicos_web_areaExp64BS4ProdOpt',
            ('x86_64-slc4-gcc34-debug','dev'):   'nicos_web_areaExp64BS4ProdDbg'
            }

        cfg = (cmtConfig, originalBranch)
        try:
            pathFrag =  pathFrags[cfg]
        except:
            self.dom = None
            logger.warning('Cannot handle config:  %s %s' % cfg)
            return

        
        
        self.logger = logger
        nameLookUp = {
            'rel_0':  'ATNComboSummary_0.xml',
            'rel_1':  'ATNComboSummary_1.xml',
            'rel_2':  'ATNComboSummary_2.xml',
            'rel_3':  'ATNComboSummary_3.xml',
            'rel_4':  'ATNComboSummary_4.xml',
            'rel_5':  'ATNComboSummary_5.xml',
            'rel_6':  'ATNComboSummary_6.xml'}

        

        servername = 'atlas-computing.web.cern.ch'
        try:
            localFile  = nameLookUp[release]
        except:
            self.logger.error('Unknown release: %s' % str(release))
            self.dom = None
            return

        addOn = 'projects'
        if originalBranch == 'val': addOn = ''
        filename   = '/atlas-computing/links/distDirectory/nightlies/%s/%s/%s' % (addOn, pathFrag, localFile)

        
        remoteAddr = 'http://%s%s' % (servername, filename)
        try:
            urllib.urlretrieve(remoteAddr, localFile)
            self.logger.info('Retrieved ATN summary: %s' %  remoteAddr)
        except:
            self.logger.warning('Could not retrieve ATN summary: %s' %  remoteAddr)
            self.dom = None

        try:
            self.dom = xml.dom.minidom.parse(localFile)
        except:
            self.logger.warning('Could not parse ATN summary: %s' %  localFile)
            self.dom = None

        if self.dom: self.massageDom()

    def massageDom(self):

        de = self.dom.documentElement
        tests = Evaluate('test', de)

        nonAthenaTests = [t for t in tests if Evaluate('testType/text()', t) != 'athena']
            
        [de.removeChild(t) for t  in nonAthenaTests]
        [n.unlink() for n in nonAthenaTests]
        
    def getDom(self): return self.dom
