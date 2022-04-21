# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from TriggerMenuMT.HLT.Config.ChainConfigurationBase import ChainConfigurationBase



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
        


        for step in steps:
            chainstep = getattr(self, step)()
            chainSteps+=[chainstep]

        myChain = self.buildChain(chainSteps)

        return myChain

    def getStepDictionary(self):

        stepDictionary = {
            "isohpttrack" : ['getIsoHPtTrackEmpty', 'getRoITrkEmpty', 'getFTFTrackReco', 'getIsoHPtTrackTrigger'],
            "fslrt"       : ['getFSLRTEmpty',       'getRoITrkEmpty', 'getFSLRTTrigger'],
            "dedx"        : ['getdEdxEmpty',        'getRoITrkEmpty', 'getFTFTrackReco', 'getdEdxTrigger'],
            "hitdv"       : ['getJetReco',          'getRoITrkEmpty', 'getFTFTrackReco', 'getHitDVTrigger'],
            "distrk"      : ['getDisTrkEmpty',      'getRoITrkEmpty', 'getFTFTrackReco', 'getDisTrkTrigger'],
            "dispj"       : ['getJetReco',          'getRoITrkEmpty', 'getFTFTrackReco', 'getDJ1', 'getDJ2', 'getDJ3']
        }

        return stepDictionary


    # --------------------
    # Step definitions in alignment order
    # Step 1
    def getJetReco(self):
        return self.getStep(1,'JetRecoOnlyCfg',[JetRecoOnlyCfg])
    # Empty for alignment
    def getIsoHPtTrackEmpty(self):
        return  self.getEmptyStep(1,"EmptyUncTrk")
    def getFSLRTEmpty(self):
        return self.getEmptyStep(1, 'FSLRTEmptyStep')
    def getDisTrkEmpty(self):
        return self.getEmptyStep(1, 'DisTrkEmptyStep')
    def getdEdxEmpty(self):
        return self.getEmptyStep(1, 'dEdxEmptyStep')

    # Step 2
    def getFSLRTTrigger(self):
        return self.getStep(2,'FSLRTTrigger',[FSLRTTriggerCfg])
    # Empty for alignment with jets
    def getRoITrkEmpty(self):
        return self.getEmptyStep(2, 'RoITrkEmptyStep')

    # Step 3 -- all FTF tracking here
    def getFTFTrackReco(self):
        return self.getStep(3,'FTFRecoOnlyCfg',[FTFRecoOnlyCfg])

    # Step 4+ -- everything post FTF tracking
    def getIsoHPtTrackTrigger(self):
        return self.getStep(4,'IsoHPtTrackTriggerCfg',[IsoHPtTrackTriggerCfg])
    def getdEdxTrigger(self):
        return self.getStep(4,'dEdxTriggerCfg',[dEdxTriggerCfg])
    def getHitDVTrigger(self):
        return self.getStep(4,'HitDVTriggerCfg',[HitDVTriggerCfg])
    def getDisTrkTrigger(self):
        return self.getStep(4,'DisTrkTriggerCfg',[DisTrkTriggerCfg])

    def getDJ1(self):
        return self.getStep(4,'DJTrigger1',[DJStep1])
    def getDJ2(self):
        return self.getStep(5,'DJTrigger2',[DJStep2])
    def getDJ3(self):
        return self.getStep(6,'DJTrigger3',[DJStep3])


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

def DJStep1(flags):
    from TriggerMenuMT.HLT.UnconventionalTracking.DJTriggerConfiguration import DJPromptStep
    return DJPromptStep()

def DJStep2(flags):
    from TriggerMenuMT.HLT.UnconventionalTracking.DJTriggerConfiguration import DJDispStep
    return DJDispStep()

def DJStep3(flags):
    from TriggerMenuMT.HLT.UnconventionalTracking.DJTriggerConfiguration import DJEDStep
    return DJEDStep()