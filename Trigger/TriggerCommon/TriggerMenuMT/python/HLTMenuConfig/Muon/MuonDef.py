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

from TriggerMenuMT.HLTMenuConfig.Muon.MuonSequenceSetup import muFastSequence, muCombSequence, muEFMSSequence, muEFSASequence, muIsoSequence, muEFCBSequence, muEFSAFSSequence, muEFCBFSSequence, muEFIsoSequence



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

def muEFIsoSequenceCfg(flags):
    return muEFIsoSequence()


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

        stepDictionary = self.getStepDictionary()

        key = self.chainPart['extra']+self.chainPart['isoInfo']

        steps=stepDictionary[key]

        for step_level in steps:
            for step in step_level:
                chainSteps+=[step]
    
        myChain = self.buildChain(chainSteps)
        return myChain

    def getStepDictionary(self):


        # --------------------
        # define here the names of the steps and obtain the chainStep configuration
        # each value is a list [ L2, EF ] where L2 = [list of L2 steps] and EF = [ EF steps]
        # this way, Bphys (or others) can insert steps at the end of L2 and end of EF after
        # the muon steps are defined
        # note that bphys chains are by default noL2Comb, even though this is not in the name
        # --------------------

        stepDictionary = {
            "":[[self.getmuFast(), self.getmuComb()], [self.getmuEFSA(), self.getmuEFCB()]],
            "fast":[[self.getmuFast()]],
            "Comb":[[self.getmuFast(), self.getmuComb()]],
            "noL2Comb" : [[self.getmuFast()], [self.getmuEFSA(), self.getmuEFCB()]],
            "ivar":[[self.getmuFast(), self.getmuComb(), self.getmuIso()]],
            "noL1":[[],[self.getFSmuEFSA(), self.getFSmuEFCB()]],
            "msonly":[[self.getmuFast(), self.getmuMSEmpty()], [self.getmuEFMS()]],
            "ivarmedium":[[self.getmuFast(), self.getmuComb()], [self.getmuEFSA(), self.getmuEFCB(), self.getmuEFIso()]],
        }
        return stepDictionary

    # --------------------
    def getmuFast(self):
        stepName = 'Step1_mufast'
        log.debug("Configuring step " + stepName)
        muSeq = RecoFragmentsPool.retrieve( muFastSequenceCfg, None)
        return ChainStep(stepName, [muSeq], self.mult)
        
    # --------------------
    def getmuComb(self):
        stepName = 'Step2_muComb'
        log.debug("Configuring step " + stepName)
        muSeq = RecoFragmentsPool.retrieve( muCombSequenceCfg, None)
        return ChainStep(stepName, [muSeq], self.mult)

    # --------------------
    def getmuEFSA(self):
        stepName = 'Step3_muEFSA'
        log.debug("Configuring step " + stepName)
        muSeq = RecoFragmentsPool.retrieve( muEFSASequenceCfg, None)
        return ChainStep(stepName, [muSeq], self.mult)

    # --------------------
    def getmuEFMS(self):
        stepName = 'Step3_muEFMS'
        log.debug("Configuring step " + stepName)
        muSeq = RecoFragmentsPool.retrieve( muEFMSSequenceCfg, None)
        return ChainStep(stepName, [muSeq], self.mult)

    # --------------------
    def getmuIso(self):
        stepName = 'Step3_muIso'
        log.debug("Configuring step " + stepName)
        muSeq = RecoFragmentsPool.retrieve( muIsoSequenceCfg, None)
        return ChainStep(stepName, [muSeq], self.mult)

    # --------------------
    def getmuEFCB(self):
        stepName = 'Step4_muEFCB'
        log.debug("Configuring step " + stepName)
        muSeq = RecoFragmentsPool.retrieve( muEFCBSequenceCfg, None)
        return ChainStep(stepName, [muSeq], self.mult)

    # --------------------
    def getFSmuEFSA(self):
        stepName = 'Step1_FSmuEFSA'
        log.debug("Configuring step " + stepName)
        muSeq = RecoFragmentsPool.retrieve( FSmuEFSASequenceCfg, None)
        return ChainStep(stepName, [muSeq], self.mult)

    # --------------------
    def getFSmuEFCB(self):
        stepName = 'Step2_FSmuEFCB'
        log.debug("Configuring step " + stepName)
        muSeq = RecoFragmentsPool.retrieve( FSmuEFCBSequenceCfg, None)
        return ChainStep(stepName, [muSeq], self.mult)

    #---------------------
    def getmuEFIso(self):
        stepName = 'Step5_muEFIso'
        log.debug("Configuring step " + stepName)
        muSeq = RecoFragmentsPool.retrieve( muEFIsoSequenceCfg, None)
        return ChainStep(stepName, [muSeq], self.mult)

    #--------------------
    def getmuMSEmpty(self):
        stepName = 'Step_muMS_empty'
        log.debug("Configuring empty step")
        return ChainStep(stepName)


