# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from TriggerMenuMT.HLT.Menu.ChainConfigurationBase import ChainConfigurationBase



def unconventionalTrackingChainParts(chainParts):
    unconvtrkChainParts = []
    for p in chainParts:
        if p['trigType'] == 'unconvtrk':
            unconvtrkChainParts.append(p)
    return unconvtrkChainParts


#----------------------------------------------------------------
# Class to configure chain
#----------------------------------------------------------------
class UnconventionalTrackingChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)

    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChainImpl(self):
        log.debug("Assembling chain %s", self.chainName)

        chainSteps = []

        stepDictionary = self.getStepDictionary()


        key = self.chainPart['extra']
        steps=stepDictionary[key]
        


        for step_level in steps:
            for step in step_level:
                chainstep = getattr(self, step)()
                chainSteps+=[chainstep]

        myChain = self.buildChain(chainSteps)

        return myChain

    def getStepDictionary(self):

        stepDictionary = {
            "isohpttrack" : [['getIsoHPtTrackEmtpy'],['getFTFTrackReco'],['getIsoHPtTrackTrigger']],
            "fslrt": [['getFSLRTEmpty'], ['getFSLRTTrigger']],
            "dedx" : [['getdEdxEmpty'], ['getFTFTrackReco'],['getdEdxTrigger']],
            "hitdv": [['getJetReco'],['getFTFTrackReco'],['getHitDVTrigger']],
            "distrk" : [['getDisTrkEmpty'],['getFTFTrackReco'],['getDisTrkTrigger']]
        }

        return stepDictionary


        # --------------------
    def getIsoHPtTrackTrigger(self):
        return self.getStep(7,'IsoHPtTrackTriggerCfg',[IsoHPtTrackTriggerCfg])

    def getFTFTrackReco(self):
        return self.getStep(6,'FTFRecoOnlyCfg',[FTFRecoOnlyCfg])

    def getIsoHPtTrackEmtpy(self):
        return  self.getEmptyStep(1,"EmptyUncTrk")

    def getFSLRTTrigger(self):
        return self.getStep(2,'FSLRTTrigger',[FSLRTTriggerCfg])

    def getFSLRTEmpty(self):
        return self.getEmptyStep(1, 'FSLRTEmptyStep')

    def getdEdxTrigger(self):
        return self.getStep(7,'dEdxTriggerCfg',[dEdxTriggerCfg])

    def getdEdxEmpty(self):
        return self.getEmptyStep(1, 'dEdxEmptyStep')

    def getHitDVTrigger(self):
        return self.getStep(7,'HitDVTriggerCfg',[HitDVTriggerCfg])

    def getHitDVEmpty(self):
        return self.getEmptyStep(1, 'HitDVEmptyStep')

    def getJetReco(self):
        return self.getStep(1,'JetRecoOnlyCfg',[JetRecoOnlyCfg])

    def getDisTrkTrigger(self):
        return self.getStep(7,'DisTrkTriggerCfg',[DisTrkTriggerCfg])

    def getDisTrkEmpty(self):
        return self.getEmptyStep(1, 'DisTrkEmptyStep')


def IsoHPtTrackTriggerCfg(flags):
    from TriggerMenuMT.HLT.UnconventionalTracking.IsoHighPtTrackTriggerConfiguration import IsoHPtTrackTriggerHypoSequence
    return IsoHPtTrackTriggerHypoSequence()

def FTFRecoOnlyCfg(flags):
    from TriggerMenuMT.HLT.UnconventionalTracking.IsoHighPtTrackTriggerConfiguration import FTFRecoOnlySequence
    return FTFRecoOnlySequence()

def FSLRTTriggerCfg(flags):
    from TriggerMenuMT.HLT.UnconventionalTracking.FullScanLRTTrackingConfiguration import FullScanLRTTriggerMenuSequence
    return FullScanLRTTriggerMenuSequence()

def dEdxTriggerCfg(flags):
    from TriggerMenuMT.HLT.UnconventionalTracking.dEdxTriggerConfiguration import dEdxTriggerHypoSequence
    return dEdxTriggerHypoSequence()

def HitDVTriggerCfg(flags):
    from TriggerMenuMT.HLT.UnconventionalTracking.HitDVConfiguration import HitDVHypoSequence
    return HitDVHypoSequence()

def JetRecoOnlyCfg(flags):
    from TriggerMenuMT.HLT.UnconventionalTracking.HitDVConfiguration import UTTJetRecoSequence
    return UTTJetRecoSequence()

def DisTrkTriggerCfg(flags):
    from TriggerMenuMT.HLT.UnconventionalTracking.DisTrkTriggerConfiguration import DisTrkTriggerHypoSequence
    return DisTrkTriggerHypoSequence()
