# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Egamma.ElectronDef")


from TrigUpgradeTest.electronMenuDefs import fastCaloSequence, electronSequence
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import Chain, ChainStep
from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT

class Chain_electron:

    def __init__(self, chainDict):

        self.chainSteps = []

        self.chainPart = chainDict['chainParts']
        self.chainName = chainDict['chainName']
        self.chainL1Item = chainDict['L1item']        
        self.chainPartL1Item = self.chainPart['L1item']
        self.mult = int(self.chainPart['multiplicity'])
        self.chainPartName = self.chainPart['chainPartName']
        self.chainPartNameNoMult = self.chainPartName[1:] if self.mult > 1 else self.chainPartName
        self.chainPartNameNoMult += "_"+self.chainL1Item


    def assembleChain(self):                            
        myStepNames = []
        chainSteps = []
        if 'etcut' in self.chainName:
            myStepNames += ["Step1_etcut"]
            myStepNames += ["Step2_etcut"]
            
            for step in myStepNames: 
                chainSteps += [self.getStep(step)]


        self.chainSteps = chainSteps
        myChain = Chain(name = 'HLT_'+self.chainName, 
                        Seed = self.chainL1Item, 
                        ChainSteps = self.chainSteps)
        return myChain


    def getFastCaloStep(self, stepName):
        fastCaloStep= fastCaloSequence()
        step1=ChainStep(stepName, [fastCaloStep])
        return step1

    def getElectronStep(self, stepName):
        electronStep= electronSequence()
        step2=ChainStep(stepName, [electronStep])
        return step2
        
    def getStep(self, name):
        myTriggerConfigHLT = TriggerConfigHLT.currentTriggerConfig()        
        if name in myTriggerConfigHLT.allChainSteps:
            return myTriggerConfigHLT.allChainSteps[name]            
        log.debug("ChainStep has not been configured so far, add to the list!")
        if name == "Step1_etcut":
            chainStep = self.getFastCaloStep(name)
        elif name == "Step2_etcut":
            chainStep = self.getElectronStep(name)
        else:
            raise RuntimeError("chainStepName unknown: " + name )
                        
        myTriggerConfigHLT.allChainSteps[name]= chainStep    
        return chainStep
            
            

        
                
