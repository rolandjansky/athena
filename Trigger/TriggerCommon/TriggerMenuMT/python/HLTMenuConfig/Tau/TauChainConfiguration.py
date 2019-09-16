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

from TriggerMenuMT.HLTMenuConfig.Tau.TauMenuSequences import tauCaloMenuSequence, tauCaloMVAMenuSequence, tauCoreTrackSequence, tauPrecisionSequence, tauTwoStepTrackSeqCore, tauTwoStepTrackSeqIso

#--------------------------------------------------------
# fragments generating config will be functions in new JO
#--------------------------------------------------------
def getTauCaloCfg(flags):
    return tauCaloMenuSequence("Tau")

def getTauCaloMVACfg(flags):
    return tauCaloMVAMenuSequence("Tau")

def getTauCoreTrackCfg(flags):
    return tauCoreTrackSequence()

def getTauFastTrackCfg(flags):
    return tauTwoStepTrackSeqCore()

def getTauIsoTrackCfg(flags):
    return tauTwoStepTrackSeqIso()

def getTauPrecisionCfg(flags):
    return tauPrecisionSequence()

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

        # --------------------
        # define here the names of the steps and obtain the chainStep configuration 
        # --------------------
        stepDictionary = {
            "ptonly":[self.getCaloSeq(), self.getTrackCore()],
            #"tracktwo":[self.getCaloSeq(), self.getFastTrack()],
            "tracktwo":[self.getCaloSeq(), self.getFastTrack(), self.getTrackIso()],
            #"tracktwo":[self.getCaloSeq(), self.getTrackCore()],
            "tracktwoMVA":[self.getCaloMVASeq(), self.getPrecision()],
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
    def getCaloMVASeq(self):
        stepName = 'Step1MVA_tau'
        log.debug("Configuring step " + stepName)
        tauSeq = RecoFragmentsPool.retrieve( getTauCaloMVACfg, None)
        return ChainStep(stepName, [tauSeq])
        
    # --------------------
    def getTrackCore(self):
        stepName = 'Step2TP_tau'
        log.debug("Configuring step " + stepName)
        tauSeq = RecoFragmentsPool.retrieve( getTauCoreTrackCfg, None)
        return ChainStep(stepName, [tauSeq])

    # --------------------
    def getFastTrack(self):
        stepName = 'Step2FT_tau'
        log.debug("Configuring step " + stepName)
        tauSeq = RecoFragmentsPool.retrieve( getTauFastTrackCfg, None)
        return ChainStep(stepName, [tauSeq])

    # --------------------                                                                                                       
    def getTrackIso(self):
        stepName = 'Step2FTIso_tau'
        log.debug("Configuring step " + stepName)
        tauSeq = RecoFragmentsPool.retrieve( getTauIsoTrackCfg, None)
        return ChainStep(stepName, [tauSeq])
        
    # --------------------
    def getPrecision(self):
        stepName = 'Step2PT_tau'
        log.debug("Configuring step " + stepName)
        tauSeq = RecoFragmentsPool.retrieve( getTauPrecisionCfg, None)
        return ChainStep(stepName, [tauSeq])



