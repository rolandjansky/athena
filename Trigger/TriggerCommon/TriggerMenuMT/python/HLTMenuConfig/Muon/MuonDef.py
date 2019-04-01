# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

########################################################################
#
# SliceDef file for muon chains/signatures
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Muon.MuonDef")

from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase, RecoFragmentsPool
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep

from TriggerMenuMT.HLTMenuConfig.Muon.MuonSequenceSetup import muFastSequence, muCombSequence, muEFMSSequence, muEFSASequence, muIsoSequence, muEFCBSequence, muEFSAFSSequence, muEFCBFSSequence, inDetSetup


#--------------------------------------------------------
# fragments generating config will be functions in new JO
#--------------------------------------------------------
def muFastSequenceCfg(flags):
    return muFastSequence()

def muCombSequenceCfg(flags):
    return muCombSequence()

def muEFMSSequenceCfg(flags):
    return muEFMSSequence()

def muEFSASequenceCfg(flags):
    return muEFSASequence()

def muIsoSequenceCfg(flags):
    return muIsoSequence()

def muEFCBSequenceCfg(flags):
    return muEFCBSequence()

def FSmuEFSASequenceCfg(flags):
    return muEFSAFSSequence()

def FSmuEFCBSequenceCfg(flags):
    return muEFCBFSSequence()


############################################# 
###  Class/function to configure muon chains 
#############################################

class MuonChainConfiguration(ChainConfigurationBase):

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

        #--- First: 'nominal chain of type': mu
        if        not self.chainPart['extra'] \
              and not self.chainPart['FSinfo'] :
            chainSteps += [self.getmuFast()]
            chainSteps += [self.getmuComb()]

            #--- with isolation ---
            if 'ivar' in self.chainPart['isoInfo']:
                chainSteps += [self.getmuIso()]
            else:
                chainSteps += [self.getmuEFSA()]
                chainSteps += [self.getmuEFCB()]
                
        #--- combined ----
        elif 'Comb' in self.chainPart['extra']:
            chainSteps += [self.getmuFast()]
            chainSteps += [self.getmuComb()]

        #--- noL1 seed ---
        elif 'noL1' in self.chainPart['extra']:
            chainSteps += [self.getFSmuEFSA()]
            chainSteps += [self.getFSmuEFCB()]
            
        #--- fast setup ---
        elif 'fast' in self.chainPart['extra']:
            chainSteps += [self.getmuFast()]

        else:
            raise RuntimeError("Chain configuration unknown for chain: " + self.chainName )
    
        myChain = self.buildChain(chainSteps)
        return myChain


    # --------------------
    def getmuFast(self):
        stepName = 'Step1_mufast'
        log.debug("Configuring step " + stepName)
        muSeq = RecoFragmentsPool.retrieve( muFastSequenceCfg, None)
        return ChainStep(stepName, [muSeq])
        
    # --------------------
    def getmuComb(self):
        stepName = 'Step1_muComb'
        log.debug("Configuring step " + stepName)
        muSeq = RecoFragmentsPool.retrieve( muCombSequenceCfg, None)
        return ChainStep(stepName, [muSeq])

    # --------------------
    def getmuEFSA(self):
        stepName = 'Step1_muEFSA'
        log.debug("Configuring step " + stepName)
        muSeq = RecoFragmentsPool.retrieve( muEFSASequenceCfg, None)
        return ChainStep(stepName, [muSeq])

    # --------------------
    #def getmuEFMS(self):
    #    stepName = 'Step1_muEFMS'
    #    log.debug("Configuring step " + stepName)
    #    muSeq = RecoFragmentsPool.retrieve( muEFMSSequenceCfg, None)
    #    return ChainStep(stepName, [muSeq])

    # --------------------
    def getmuIso(self):
        stepName = 'Step1_muIso'
        log.debug("Configuring step " + stepName)
        muSeq = RecoFragmentsPool.retrieve( muIsoSequenceCfg, None)
        return ChainStep(stepName, [muSeq])

    # --------------------
    def getmuEFCB(self):
        stepName = 'Step1_muEFCB'
        log.debug("Configuring step " + stepName)
        muSeq = RecoFragmentsPool.retrieve( muEFCBSequenceCfg, None)
        return ChainStep(stepName, [muSeq])

    # --------------------
    def getFSmuEFSA(self):
        stepName = 'Step1_FSmuEFSA'
        log.debug("Configuring step " + stepName)
        muSeq = RecoFragmentsPool.retrieve( FSmuEFSASequenceCfg, None)
        return ChainStep(stepName, [muSeq])

    # --------------------
    def getFSmuEFCB(self):
        stepName = 'Step1_FSmuEFCB'
        log.debug("Configuring step " + stepName)
        muSeq = RecoFragmentsPool.retrieve( FSmuEFCBSequenceCfg, None)
        return ChainStep(stepName, [muSeq])



