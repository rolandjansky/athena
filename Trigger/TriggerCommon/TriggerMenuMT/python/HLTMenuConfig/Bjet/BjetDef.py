# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Bjet.BjetDef")


from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase

from TriggerMenuMT.HLTMenuConfig.Bjet.BjetSequenceSetup import getBJetSequence

#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO, 
# so let's make them functions already now
#----------------------------------------------------------------
def bjetSequenceCfg( flags ):
    return getBJetSequence()


#----------------------------------------------------------------
# Class to configure chain
#----------------------------------------------------------------
class BjetChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)

    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):                            
        log.debug("Assembling chain for " + self.chainName)

        # --------------------
        # define here the names of the steps and obtain the chainStep configuration 
        # --------------------
        stepDictionary = self.getStepDictionary()
        
        ## This needs to be configured by the Bjet Developer!!
        key = self.chainPart['extra'] 
        steps = stepDictionary[key]

        chainSteps = []
        for step in steps:
            chainstep = getattr(self, step)()
            chainSteps += [chainstep]
    
        myChain = self.buildChain(chainSteps)
        return myChain


    def getStepDictionary(self):
        # --------------------
        # define here the names of the steps and obtain the chainStep configuration 
        # --------------------
        stepDictionary = {
            "": ["getBjetSequence"]
        }
        
        return stepDictionary

    # --------------------
    # Configuration of steps
    # --------------------
    def getBjetSequence(self):
        stepName = "Step2_bjet"
        log.debug("Configuring step " + stepName)
        
        return self.getStep(2, stepName, [bjetSequenceCfg])        









