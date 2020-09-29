# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Jet.JetDef")

from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep, RecoFragmentsPool

import copy

def jetChainParts(chainParts):
    jChainParts = []
    for p in chainParts:
        if p['trigType'] == 'j':
            jChainParts.append(p)
    return jChainParts

#----------------------------------------------------------------
# Class to configure chain
#----------------------------------------------------------------
class JetChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        # we deliberately don't call base class constructore, since this assumes a single chain part
        # which is not the case for jets

        self.dict = copy.deepcopy(chainDict)
        
        self.chainName = self.dict['chainName']
        self.chainL1Item = self.dict['L1item']

        self.chainPart = self.dict['chainParts']
        self.L1Threshold = ''
        self.mult = 1 # from the framework point of view I think the multiplicity is 1, internally the jet hypo has to figure out what to actually do

        # these properties are in the base class, but I don't think we need them for jets
        #self.chainPartName = ''
        #self.chainPartNameNoMult = ''
        #self.chainPartNameNoMultwL1 = ''

        # expect that the L1 seed is the same for all jet parts, otherwise we have a problem
        jChainParts = jetChainParts(self.chainPart)
        for p in jChainParts:
            l1th = p['L1threshold']
            if self.L1Threshold != '' and self.L1Threshold != l1th:
                log.error('Cannot configure a jet chain with different L1 thresholds')
                exit(1)
            self.L1Threshold = l1th

        from TriggerMenuMT.HLTMenuConfig.Jet.JetRecoConfiguration import extractRecoDict
        self.recoDict = extractRecoDict(jChainParts)

    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):                            
        log.debug("Assembling chain " + self.chainName)

        # --------------------
        # define here the names of the steps and obtain the chainStep configuration 
        # --------------------
        # Only one step for now, but we might consider adding steps for
        # reclustering and trimming workflows

        step1_jetCollectionName, step1= self.getJetChainStep()
        steps=[step1]

        chainSteps = []
        for step in steps:
            chainSteps+=[step]
         
         
        if "JetDS" in self.chainName:
           TLAStep = self.getJetTLAChainStep(step1_jetCollectionName)
           chainSteps+= [TLAStep]
        
        myChain = self.buildChain(chainSteps)

        return myChain
        

    # --------------------
    # Configuration of steps
    # --------------------
    def getJetChainStep(self):
        from TriggerMenuMT.HLTMenuConfig.Jet.JetMenuSequences import jetMenuSequence
        from TriggerMenuMT.HLTMenuConfig.Jet.JetRecoSequences import jetRecoDictToString

        jetDefStr = jetRecoDictToString(self.recoDict)

        stepName = "Step1_jet_"+jetDefStr
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        jetSeq1 = RecoFragmentsPool.retrieve( jetMenuSequence, ConfigFlags, **self.recoDict ) # the None will be used for flags in future
        step1_jetCollectionName = jetSeq1.hypo.Alg.Jets 
        chainStep1 = ChainStep(stepName, [jetSeq1], multiplicity=[1], chainDicts=[self.dict])
        
        return step1_jetCollectionName, chainStep1

    def getJetTLAChainStep(self, jetCollectionName):
        #maybe not needed
        from TriggerMenuMT.HLTMenuConfig.Jet.JetTLAConfiguration import jetTLAMenuSequence

        stepName = "Step2_jetTLA_"+jetCollectionName
        jetSeq2 = RecoFragmentsPool.retrieve( jetTLAMenuSequence, jetCollectionName ) # the None will be used for flags in future
        chainStep2 = ChainStep(stepName, [jetSeq2], multiplicity=[1], chainDicts=[self.dict])

        return chainStep2

