# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.MET.METChainConfiguration")


from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase

from TriggerMenuMT.HLTMenuConfig.MET.METMenuSequences import metCellMenuSequence, metClusterPufitMenuSequence, metJetMenuSequence

#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO, 
# so let's make them functions already now
#----------------------------------------------------------------

    
def MetCellSequenceCfg( flags ):    
    return metCellMenuSequence()

def MetClusterPufitSequenceCfg( flags ):    
    return metClusterPufitMenuSequence()

def MetJetSequenceCfg( flags ):    
   return metJetMenuSequence()



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
        chainSteps = []
        log.debug("Assembling chain for " + self.chainName)
        # --------------------
        # define here the names of the steps and obtain the chainStep configuration 
        # --------------------
        stepDictionary = {
            "cell":[self.getMetCellStep()],
            "tcpufit":[self.getMetClusterPufitStep()],
            "mht":[self.getMetJetStep()],
        }
        

        key = self.chainPart['EFrecoAlg']
        steps=stepDictionary[key]
        for step in steps:
            chainSteps+=[step]
            
        myChain = self.buildChain(chainSteps)
        return myChain


   
    # --------------------
    # Configuration of cell chain
    # --------------------
    def getMetCellStep(self):
        return self.getStep(1,"met_cell", [MetCellSequenceCfg] )

            
    # --------------------
    # Configuration of Jet chain
    # --------------------
    def getMetJetStep(self):
       return self.getStep(1, "met_jets", [MetJetSequenceCfg])
            
    # --------------------
    # Configuration of pufit chain
    # --------------------
    def getMetClusterPufitStep(self):
        return self.getStep(1,"met_clusterpufit", [MetClusterPufitSequenceCfg] )
     
            

        
                
