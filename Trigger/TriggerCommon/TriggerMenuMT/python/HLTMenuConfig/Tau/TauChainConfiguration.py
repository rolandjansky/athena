# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

########################################################################
#
# SliceDef file for muon chains/signatures
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Tau.TauChainConfiguration")

from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase

from TriggerMenuMT.HLTMenuConfig.Tau.TauMenuSequences import tauCaloMenuSequence, tauCaloMVAMenuSequence, tauTwoStepTrackSeqCore, tauTwoStepTrackSeqIso, tauIdTrackSeq, tauTrackSeq, tauTrackTwoSeq, tauTrackTwoEFSeq, tauTwoStepTrackSeqCoreMVA

#--------------------------------------------------------
# fragments generating config will be functions in new JO
#--------------------------------------------------------
def getTauCaloCfg(flags):
    return tauCaloMenuSequence("Tau")

def getTauCaloMVACfg(flags):
    return tauCaloMVAMenuSequence("Tau")

def getTauTrackCfg(flags):
    return tauTrackSeq()

def getTauTrackTwoCfg(flags):
    return tauTrackTwoSeq()

def getTauIdTrackCfg(flags):
    return tauIdTrackSeq()

def getTauFastTrackMVACfg(flags):
    return tauTwoStepTrackSeqCoreMVA()

def getTauFastTrackCfg(flags):
    return tauTwoStepTrackSeqCore()

def getTauIsoTrackCfg(flags):
    return tauTwoStepTrackSeqIso()

def getTauTrackTwoEFCfg(flags):
    return tauTrackTwoEFSeq()

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
            "ptonly":['getCaloSeq', 'getIdTrack'], #This should use calo only sequence
            "track":['getCaloSeq', 'getTrack'], #This should use calo only sequence
            "tracktwo":['getCaloSeq', 'getFastTrack', 'getTrackTwo'], #This should use calo only sequence
            "tracktwoEF":['getCaloSeq','getFastTrack','getTrack2EF'], #This should use calo only sequence
            "tracktwoMVA":['getCaloMVASeq','getFastTrackMVA', 'getTrackIso'], #This should use calo mva sequence
        }

        # this should be extended by the signature expert to make full use of the dictionary!
        key = self.chainPart['preselection']
        steps=stepDictionary[key]
        for step in steps:
            chainstep = getattr(self, step)()
            chainSteps+=[chainstep]
    
        myChain = self.buildChain(chainSteps)
        return myChain


    # --------------------
    def getCaloSeq(self):
        stepName = 'tau'
        return self.getStep(1,stepName, [getTauCaloCfg])

    # --------------------
    def getCaloMVASeq(self):
        stepName = 'MVA_tau'
        return self.getStep(1,stepName, [getTauCaloMVACfg])

    # --------------------                                                                                                                                    
    def getTrack(self):
        stepName = 'Track_tau'
        return self.getStep(2,stepName, [getTauTrackCfg])

    # --------------------                                                                                                                        
    def getTrackTwo(self):
        stepName = 'TrackTwo_tau'
        return self.getStep(3,stepName, [getTauTrackTwoCfg])

    # --------------------                                                                                                                    
    def getIdTrack(self):
        stepName = 'FTId_tau'
        return self.getStep(2,stepName, [getTauIdTrackCfg])
        
    # --------------------
    def getFastTrack(self):
        stepName = 'FT_tau'
        return self.getStep(2,stepName, [getTauFastTrackCfg])

    # --------------------                                                                                                                                                       
    def getFastTrackMVA(self):
        stepName = 'FT_tauMVA'
        return self.getStep(2,stepName, [getTauFastTrackMVACfg])

    # --------------------                                                                                                       
    def getTrackIso(self):
        stepName = 'FTIso_tau'
        return self.getStep(3,stepName, [getTauIsoTrackCfg])

    # --------------------                                                                                                                                                       
    def getTrack2EF(self):
        stepName = 'FTEF_tau'
        return self.getStep(3, stepName, [getTauTrackTwoEFCfg])
