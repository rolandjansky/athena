# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep, RecoFragmentsPool
from DecisionHandling.DecisionHandlingConfig import ComboHypoCfg

#----------------------------------------------------------------
# Base class to configure chain
#----------------------------------------------------------------
class ChainConfigurationBase(object):

    def __init__(self, chainDict):

        # Consider using translation from  dict["chainName"] to dict.chainName (less typing),
        # though need to be able to access list of dictionaries as value of chainPart as well
        # dict = type("dict", (object,), dict)

        self.dict = {}
        self.dict.update(chainDict)

        self.chainName = self.dict['chainName']
        self.chainL1Item = self.dict['L1item']

        # check dictionary contains only one chain part
        if len( self.dict['chainParts'] ) != 1:
            raise RuntimeError( "Passed chain dictionary with %i chainParts to ChainConfigurationBase, but this constructor only supports chains with a single part" % len( self.dict['chainParts'] ) )

        self.chainPart = self.dict['chainParts'][0]
        self.L1Threshold = self.chainPart['L1threshold'] # now threshold is always there
        self.mult = int(self.chainPart['multiplicity'])
        self.chainPartName = self.chainPart['chainPartName']
        self.chainPartNameNoMult = ''
        self.chainPartNameNoMultwL1 = ''

        self.chainPartNameNoMult = self.chainPartName[1:] if self.mult > 1 else self.chainPartName
        self.chainPartNameNoMultwL1 += "_"+self.chainL1Item

    # this is the cache, hoping to be able to get rid of it in future
    def checkRecoFragmentPool(mySequenceCfg):
        mySequence = RecoFragmentsPool.retrieve(mySequenceCfg, None) # the None will be used for flags in future
        return mySequence

    def getStep(self, stepID, stepPartName, sequenceCfgArray, comboHypoCfg=ComboHypoCfg, comboTools=[]):
        stepName = 'Step%d'%stepID + '_%d'%self.mult + stepPartName
        log.debug("Configuring step " + stepName)
        seqArray = []
        for sequenceCfg in sequenceCfgArray:
            seqArray.append( RecoFragmentsPool.retrieve( sequenceCfg, None))
        return ChainStep(stepName, seqArray, [self.mult], [self.dict], comboHypoCfg=comboHypoCfg, comboToolConfs=comboTools)

    def getEmptyStep(self, stepID, stepPartName):
        stepName = 'Step%d'%stepID + '_%d'%self.mult + stepPartName
        log.debug("Configuring empty step " + stepName)        
        return ChainStep(stepName, Sequences=[], multiplicity=[] ,chainDicts=[self.dict])
 
    def buildChain(self, chainSteps):
        myChain = Chain(name = self.chainName,
                        ChainSteps = chainSteps,
                        L1Thresholds = [self.L1Threshold] )

        return myChain


