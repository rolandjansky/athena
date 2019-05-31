# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

########################################################################
#
# SliceDef file for muon chains/signatures
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Tau.TauChainConfiguration")

from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase, RecoFragmentsPool
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep

from TriggerMenuMT.HLTMenuConfig.Tau.TauMenuSequences import tauCaloMenuSequence, tauCoreTrackSequence

from TrigUpgradeTest.InDetSetup import inDetSetup

#--------------------------------------------------------
# fragments generating config will be functions in new JO
#--------------------------------------------------------
def getTauCaloCfg(flags):
    return tauCaloMenuSequence("Tau")

def getTauCoreTrackCfg(flags):
    return tauCoreTrackSequence()


############################################# 
###  Class/function to configure muon chains 
#############################################

class TauChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)
        
    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):                            
        chainSteps = []
        log.debug("Assembling chain for " + self.chainName)

        # Calling inDetSetup here 
        inDetSetup()

        # --------------------
        # define here the names of the steps and obtain the chainStep configuration 
        # --------------------
        stepDictionary = {
            "ptonly":[self.getCaloSeq(), self.getTrackCore()],
            "tracktwo":[self.getCaloSeq(), self.getTrackCore()],
        }

        # this should be extended by the signature expert to make full use of the dictionary!
        key = self.chainPart['preselection']
        steps=stepDictionary[key]
        for step in steps:
            chainSteps+=[step]
    
        myChain = self.buildChain(chainSteps)
        return myChain


    # --------------------
    def getCaloSeq(self):
        stepName = 'Step1_tau'
        log.debug("Configuring step " + stepName)
        tauSeq = RecoFragmentsPool.retrieve( getTauCaloCfg, None)
        return ChainStep(stepName, [tauSeq])
        
    # --------------------
    def getTrackCore(self):
        stepName = 'Step2_tau'
        log.debug("Configuring step " + stepName)
        tauSeq = RecoFragmentsPool.retrieve( getTauCoreTrackCfg, None)
        return ChainStep(stepName, [tauSeq])
        
    # --------------------



