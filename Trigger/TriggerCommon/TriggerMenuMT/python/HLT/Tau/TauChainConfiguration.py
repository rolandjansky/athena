# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

########################################################################
#
# SliceDef file for muon chains/signatures
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from TriggerMenuMT.HLT.Menu.ChainConfigurationBase import ChainConfigurationBase

from TriggerMenuMT.HLT.Tau.TauMenuSequences import tauCaloMVAMenuSeq, tauFTFTauCoreSeq, tauFTFTauIsoSeq, tauFTFTauIsoBDTSeq, tauTrackTwoMVASeq, tauTrackTwoLLPSeq, tauPrecTrackIsoSeq

#--------------------------------------------------------
# fragments generating config will be functions in new JO
#--------------------------------------------------------
def getTauCaloMVACfg(flags, is_probe_leg=False):
    return tauCaloMVAMenuSeq("Tau", is_probe_leg=is_probe_leg)

def getFTFCoreCfg(flags, is_probe_leg=False):
    return tauFTFTauCoreSeq(is_probe_leg=is_probe_leg)

def getFTFIsoCfg(flags, is_probe_leg=False):
    return tauFTFTauIsoSeq(is_probe_leg=is_probe_leg)

def getFTFIsoBDTCfg(flags, is_probe_leg=False):
    return tauFTFTauIsoBDTSeq(is_probe_leg=is_probe_leg)

def getTrackTwoMVACfg(flags, is_probe_leg=False):
    return tauTrackTwoMVASeq(is_probe_leg=is_probe_leg)

def getTrackTwoLLPCfg(flags, is_probe_leg=False):
    return tauTrackTwoLLPSeq(is_probe_leg=is_probe_leg)

def getPrecTrackIsoCfg(flags, is_probe_leg=False):
    return tauPrecTrackIsoSeq(is_probe_leg=is_probe_leg)

############################################# 
###  Class/function to configure muon chains 
#############################################

class TauChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)
        
    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChainImpl(self):                            
        chainSteps = []
        log.debug("Assembling chain for %s", self.chainName)

        # --------------------
        # define here the names of the steps and obtain the chainStep configuration 
        # --------------------
        stepDictionary = {
            "ptonly"        :['getCaloMVASeq', 'getFTFEmpty', 'getTrkEmpty' , 'getPTEmpty'      , 'getIDEmpty'     ], 
            "tracktwoMVA"   :['getCaloMVASeq', 'getFTFCore' , 'getFTFIso'   , 'getPrecTrackIso' , 'getTrackTwoMVA' ],
            "tracktwoMVABDT":['getCaloMVASeq', 'getFTFCore' , 'getFTFIsoBDT', 'getPrecTrackIso' , 'getTrackTwoMVA' ],
            "tracktwoLLP"   :['getCaloMVASeq', 'getFTFCore' , 'getFTFIsoBDT', 'getPrecTrackIso' , 'getTrackTwoLLP' ],
        }

        # this should be extended by the signature expert to make full use of the dictionary!
        key = self.chainPart['preselection']
        steps=stepDictionary[key]
        for step in steps:
            is_probe_leg = self.chainPart['tnpInfo']=='probe'
            if 'Empty' in step:
                chainstep = getattr(self, step)()
            else:
                chainstep = getattr(self, step)(is_probe_leg=is_probe_leg)
            chainSteps+=[chainstep]
    
        myChain = self.buildChain(chainSteps)
        return myChain


    # --------------------
    def getCaloMVASeq(self, is_probe_leg=False):
        stepName = 'MVA_tau'
        return self.getStep(1,stepName, [getTauCaloMVACfg], is_probe_leg=is_probe_leg)
        
    # --------------------
    def getFTFCore(self, is_probe_leg=False):
        stepName = 'FTFCore_tau'
        return self.getStep(2,stepName, [getFTFCoreCfg], is_probe_leg=is_probe_leg)

    # --------------------                                                                                                                                   
    def getFTFEmpty(self):
        stepName = 'FTFEmpty_tau'
        return self.getEmptyStep(2,stepName)

    # --------------------                                                                                                      
    def getFTFIso(self, is_probe_leg=False):
        stepName = 'FTFIso_tau'
        return self.getStep(3,stepName, [getFTFIsoCfg], is_probe_leg=is_probe_leg)

    # --------------------                                                                                                                                                                         
    def getFTFIsoBDT(self, is_probe_leg=False):
        stepName = 'FTFIsoBDT_tau'
        return self.getStep(3,stepName, [getFTFIsoBDTCfg], is_probe_leg=is_probe_leg)

    # --------------------                                                                                                                                   
    def getTrkEmpty(self):
        stepName = 'TrkEmpty_tau'
        return self.getEmptyStep(3,stepName)

    # --------------------                                                                                                                                   
    def getPrecTrackIso(self, is_probe_leg=False):
        stepName = 'PrecTrkIso_tau'
        return self.getStep(4,stepName,[getPrecTrackIsoCfg],is_probe_leg=is_probe_leg)

    # --------------------                                                                                                                                   
    def getPTEmpty(self):
        stepName = 'PTEmpty_tau'
        return self.getEmptyStep(4,stepName)

    # --------------------                                                                                                      
    def getTrackTwoMVA(self, is_probe_leg=False):
        stepName = "TrkTwoMVA_tau"
        return self.getStep(5,stepName,[getTrackTwoMVACfg],is_probe_leg=is_probe_leg)

    # --------------------                                                                                                      
    def getTrackTwoLLP(self, is_probe_leg=False):
        stepName = "TrkTwoLLP_tau"
        return self.getStep(5,stepName,[getTrackTwoLLPCfg],is_probe_leg=is_probe_leg)

    # --------------------                                                                                                                                   
    def getIDEmpty(self):
        stepName = 'IDEmpty_tau'
        return self.getEmptyStep(5,stepName)
