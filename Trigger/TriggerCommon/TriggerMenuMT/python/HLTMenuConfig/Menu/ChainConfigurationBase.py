# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase' )

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, RecoFragmentsPool

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

        self.chainPart = self.dict['chainParts']
        self.chainPartL1Item = self.chainPart['L1item']
        self.mult = int(self.chainPart['multiplicity'])
        self.chainPartName = self.chainPart['chainPartName']
        self.chainPartNameNoMult = ''
        self.chainPartNameNoMultwL1 = ''

        self.chainPartNameNoMult = self.chainPartName[1:] if self.mult > 1 else self.chainPartName
        self.chainPartNameNoMultwL1 += "_"+self.chainL1Item
        
        self.L1Threshold = self.chainPartL1Item or self.chainL1Item
        if ('L1_' in self.L1Threshold):
            self.L1Threshold = self.L1Threshold.replace('L1_', '')
        self.L1Threshold = self.L1Threshold.split('_')[0]
        self.L1Threshold = self.L1Threshold[1:] if self.L1Threshold[0].isdigit() else self.L1Threshold

        self.L1ItemToPass = 'L1_'+self.L1Threshold

    # this is the cache, hoping to be able to get rid of it in future
    def checkRecoFragmentPool(mySequenceCfg):
        mySequence = RecoFragmentsPool.retrieve(mySequenceCfg, None) # the None will be used for flags in future
        return mySequence

    def buildChain(self, chainSteps):   
        myChain = Chain(name = self.chainName, 
                        Seed = self.L1ItemToPass, 
                        ChainSteps = chainSteps)
        
        return myChain

        
