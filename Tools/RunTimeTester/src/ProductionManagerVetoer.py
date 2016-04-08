# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


"""
module contains class ProductionManagerVetoer
"""

from xml.dom.minidom     import parse
from Evaluate            import Evaluate
from validateXMLFile     import Validator

import re
import os

class ProductionManagerVetoer:
    """
    Used to determine if a job should be vetoed
    according to identifiers provided in an external
    file. Identiferes include the job hash identifier,
    and the job CPC values. All of these are
    attributes of the job descriptor.
    """

    def __init__(self, pathToVetoFile, logger):
        "read the file with the veto information"

        self.idsToVeto  = []
        self.cpcsToVeto = {}

        if not pathToVetoFile:
            logger.debug('No Job veto file provided')
            return

        if not os.path.exists(pathToVetoFile):
            msg = 'non existent Job veto file: %s', pathToVetoFile
            logger.warning(msg)
            return

        self.validator = Validator(pathToVetoFile)
        isValid, excInst = self.validator.validate()
        if not isValid:
            msg = 'File %s failed validation, no jobs vetoed' % str(pathToVetoFile)
            logger.warning(msg)
            return

        documentElement = self.validator._dom.documentElement
        self.setCPCs(documentElement)
        self.setIDs(documentElement)

    def setCPCs(self, element):
        def loadTag(tagName, parentEl):
            els = Evaluate(tagName, parentEl)
            if not els: return [re.compile('[a-zA-Z0-9]*')]            
            return [re.compile(Evaluate('text()', el)) for el in els]

        cpcEl = Evaluate('CPC', element)        
        if not cpcEl: return
        
        self.cpcsToVeto = {}        
        for tag in ['displayClass', 'displayProcess', 'displayComponent']:
            self.cpcsToVeto[tag] = loadTag(tag, cpcEl[0])

    def setIDs(self, element):
        idsEl = Evaluate('ids', element)
        if not idsEl: return
        ids = Evaluate('id', idsEl[0])
        for id in ids:
            try:
                self.idsToVeto.append(int(Evaluate('text()', id)))
            except:
                continue

    def idVetoed(self, jDesc):
        if not self.idsToVeto: return False

        if int(jDesc.hashString) in self.idsToVeto:
            jDesc.logger.debug('Job %s: has hash ID %s which matches a vetoed ID. Is thus vetoed!' % (jDesc.identifiedName, jDesc.hashString))
            return True

        m = 'Job %s: does not match any vetoed IDs, so is not vetoed' % jDesc.identifiedName
        jDesc.logger.debug(m)
        # jDesc.logger.debug('%s %s' % (str(self.idsToVeto), jDesc.hashString))
        return False

    def cpcVetoed(self, jDesc):
        def isMatched(vetoFileValues, userFileValues):
            for uf in userFileValues:
                if True not in [re.match(vf, uf)!=None for vf in vetoFileValues]: return False
            return True

        # not all descriptors (ie watchers) have CPCs. Do not make
        # CPC tests for such descriptors. If the attrbute displayClass
        # is present, so must be displayProcess, displayComponent
        if not hasattr(jDesc, 'displayClass') or not self.cpcsToVeto:
            jDesc.logger.debug('Returning False (%s)' % self.cpcsToVeto)
            return False

        matches = isMatched(self.cpcsToVeto['displayClass'], jDesc.displayClass)
        matches = matches and isMatched(self.cpcsToVeto['displayProcess'],   jDesc.displayProcess)
        matches = matches and isMatched(self.cpcsToVeto['displayComponent'], jDesc.displayComponent)
        m = 'Job %s: does not match all CPC veto flags, is thus not vetoed' % jDesc.identifiedName
        if matches: m = 'Job %s: matches all CPC veto flags. Is thus vetoed!' % jDesc.identifiedName

        jDesc.logger.debug(m)
        return matches
        
    def toBeVetoed(self, jDesc):
        return self.idVetoed(jDesc) or self.cpcVetoed(jDesc)
        

if __name__ == "__main__":

    from PseudoLogger import PseudoLogger
    class Bag:
        pass
    
    paths = Bag()
    pathToVetoFile = '/afs/cern.ch/atlas/project/RTT/data/brinick/veto.xml'
    logger = PseudoLogger()
    jv = ProductionManagerVetoer(pathToVetoFile, logger)

    desc = Bag()
    desc.hashString       = '-689235331'
    desc.displayClass     = ['displayProcess2']
    desc.displayProcess   = ['Core']
    desc.displayComponent = ['Athena-Core']
    desc.logger = PseudoLogger()
    desc.identifiedName = 'job1'

    # print 'ids:'
    # print 'desc: ', desc.hashString
    # print jv.idsToVeto
    print
    # print 'CPCs'
    # print 'desc: ', desc.displayClass, desc.displayProcess, desc.displayComponent
    # print jv.cpcsToVeto
    print 
    # print 'match result', jv.toBeVetoed(desc)







    
    
