# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Met.MetDef")


from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep

from TriggerMenuMT.HLTMenuConfig.MET.metMenuDefs import metCellMenuSequence # still needs to be created/renamed metsecquencesetup

#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO, 
# so let's make them functions already now
#----------------------------------------------------------------

    
def MetCellSequenceCfg( flags ):    
    return metCellMenuSequence()



#----------------------------------------------------------------
# Class to configure chain
#----------------------------------------------------------------
class MetChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)
        
    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):                            
        MetStepNames = []
        chainSteps = []
        log.debug("Assembling chain for " + self.chainName)
        # --------------------
        # define here the names of the steps and obtain the chainStep configuration 
        # --------------------

        if 'cell' in self.chainPart['EFrecoAlg']: 

            MetStepNames +=  ["Step1_met_cell"]
            for step in MetStepNames:
                chainSteps += [self.getMetCellStep(step)]
        else:
            raise RuntimeError("Chain configuration unknown for chain: " + self.chainName )
            
        myChain = self.buildChain(chainSteps)
        return myChain
        
    # --------------------
    # Configuration of cell chain
    # --------------------
    def getMetCellStep(self, stepName):
        if stepName == "Step1_met_cell":
            log.debug("Configuring step " + stepName)
            metCellSeq = metCellMenuSequence()
            chainStep =ChainStep(stepName, [metCellSeq])
        else:            
            raise RuntimeError("chainStepName unknown: " + stepName )
                        
        log.debug("Returning chainStep from getMetCellStep function: " + stepName)
        return chainStep
            
            

        
                
