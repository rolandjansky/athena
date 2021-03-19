# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

########################################################################
#
# SliceDef file for muon chains/signatures
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Tau.TauChainConfiguration")

from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase

from TriggerMenuMT.HLTMenuConfig.Tau.TauMenuSequences import tauCaloMenuSeq, tauCaloMVAMenuSeq, tauFTFTauSeq, tauFTFTauCoreSeq, tauFTFTauIsoSeq, tauFTFTauIsoBDTSeq, tauTrackPrecSeq, tauTrackTwoPrecSeq, tauTrackTwoEFSeq, tauTrackTwoMVASeq, tauPreSelSeq, tauPreSelTTSeq, tauPrecTrackSeq, tauPrecTrackIsoSeq

#--------------------------------------------------------
# fragments generating config will be functions in new JO
#--------------------------------------------------------
def getTauCaloCfg(flags):
    return tauCaloMenuSeq("Tau")

def getTauCaloMVACfg(flags):
    return tauCaloMVAMenuSeq("Tau")

def getFTFTauCfg(flags):
    return tauFTFTauSeq()

def getFTFCoreCfg(flags):
    return tauFTFTauCoreSeq()

def getFTFIsoCfg(flags):
    return tauFTFTauIsoSeq()

def getFTFIsoBDTCfg(flags):
    return tauFTFTauIsoBDTSeq()

def getTrackPrecCfg(flags):
    return tauTrackPrecSeq()

def getTrackTwoPrecCfg(flags):
    return tauTrackTwoPrecSeq()

def getTrackTwoEFCfg(flags):
    return tauTrackTwoEFSeq()

def getTrackTwoMVACfg(flags):
    return tauTrackTwoMVASeq()

def getPreSelCfg(flags):
    return tauPreSelSeq()

def getPreSelTTCfg(flags):
    return tauPreSelTTSeq()

def getPrecTrackCfg(flags):
    return tauPrecTrackSeq()

def getPrecTrackIsoCfg(flags):
    return tauPrecTrackIsoSeq()

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
        log.debug("Assembling chain for %s", self.chainName)

        # --------------------
        # define here the names of the steps and obtain the chainStep configuration 
        # --------------------
        stepDictionary = {
            "ptonly"        :['getCaloSeq'   , 'getFTFEmpty', 'getTrkEmpty' , 'getTauEmpty'  , 'getPTEmpty'      , 'getIDEmpty'      ], 
            "track"         :['getCaloSeq'   , 'getFTFTau'  , 'getTrkEmpty' , 'getPreSel'    , 'getPrecTrack'    , 'getTrackPrec'   ], 
            "tracktwo"      :['getCaloSeq'   , 'getFTFCore' , 'getFTFIso'   , 'getPreSelTT'  , 'getPrecTrackIso' , 'getTrackTwoPrec'],
            "tracktwoEF"    :['getCaloSeq'   , 'getFTFCore' , 'getFTFIso'   , 'getTauEmpty'  , 'getPrecTrackIso' , 'getTrackTwoEF'  ],
            "tracktwoMVA"   :['getCaloMVASeq', 'getFTFCore' , 'getFTFIso'   , 'getTauEmpty'  , 'getPrecTrackIso' , 'getTrackTwoMVA' ],
            "tracktwoMVABDT":['getCaloMVASeq', 'getFTFCore' , 'getFTFIsoBDT', 'getTauEmpty'  , 'getPrecTrackIso' , 'getTrackTwoMVA' ],
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
    def getFTFTau(self):
        stepName = 'FTFTau_tau'
        return self.getStep(2,stepName, [getFTFTauCfg])
        
    # --------------------
    def getFTFCore(self):
        stepName = 'FTFCore_tau'
        return self.getStep(2,stepName, [getFTFCoreCfg])

    # --------------------                                                                                                                                   
    def getFTFEmpty(self):
        stepName = 'FTFEmpty_tau'
        return self.getEmptyStep(2,stepName)

    # --------------------                                                                                                      
    def getFTFIso(self):
        stepName = 'FTFIso_tau'
        return self.getStep(3,stepName, [getFTFIsoCfg])

    # --------------------                                                                                                                                                                         
    def getFTFIsoBDT(self):
        stepName = 'FTFIsoBDT_tau'
        return self.getStep(3,stepName, [getFTFIsoBDTCfg])

    # --------------------                                                                                                                                   
    def getTrkEmpty(self):
        stepName = 'TrkEmpty_tau'
        return self.getEmptyStep(3,stepName)

    # --------------------                                                                                                                                   
    def getPreSel(self):
        stepName = 'PreSel_tau'
        return self.getStep(4,stepName, [getPreSelCfg])

    # --------------------                                                                                                                                   
    def getPreSelTT(self):
        stepName = 'PreSelTT_tau'
        return self.getStep(4,stepName, [getPreSelTTCfg])

    # --------------------                                                                                                                                   
    def getTauEmpty(self):
        stepName = 'TauEmpty_tau'
        return self.getEmptyStep(4,stepName)

    # --------------------                                                                                                                                   
    def getPrecTrack(self):
        stepName = 'PrecTrk_tau'
        return self.getStep(5,stepName,[getPrecTrackCfg])

    # --------------------                                                                                                                                   
    def getPrecTrackIso(self):
        stepName = 'PrecTrkIso_tau'
        return self.getStep(5,stepName,[getPrecTrackIsoCfg])

    # --------------------                                                                                                                                   
    def getPTEmpty(self):
        stepName = 'PTEmpty_tau'
        return self.getEmptyStep(5,stepName)

    # --------------------                                                                                                       
    def getTrackPrec(self):
        stepName = 'TrkPrec_tau'
        return self.getStep(6,stepName, [getTrackPrecCfg])

    # --------------------                                                                                                     
    def getTrackTwoPrec(self):
        stepName = 'TrkTwo_tau'
        return self.getStep(6,stepName, [getTrackTwoPrecCfg])

    # --------------------                                                                                                     
    def getTrackTwoEF(self):
        stepName = 'TrkTwoEF_tau'
        return self.getStep(6, stepName, [getTrackTwoEFCfg])

    # --------------------                                                                                                      
    def getTrackTwoMVA(self):
        stepName = "TrkTwoMVA_tau"
        return self.getStep(6,stepName,[getTrackTwoMVACfg])

    # --------------------                                                                                                                                   
    def getIDEmpty(self):
        stepName = 'IDEmpty_tau'
        return self.getEmptyStep(6,stepName)
