# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.CalibCosmicMon.MonitorChainConfiguration")

from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase

#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO, 
# so let's make them functions already now
#----------------------------------------------------------------

#----------------------------------------------------------------
# Class to configure chain
#----------------------------------------------------------------
class MonitorChainConfiguration(ChainConfigurationBase):

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
            "costmonitor":[self.getNoalgStep()],
        }

        #key = self.chainPart['EFrecoAlg']
        key = 'costmonitor'
        steps=stepDictionary[key]
        for step in steps:
            chainSteps+=[step]
            
        myChain = self.buildChain(chainSteps)
        return myChain
   
    # --------------------
    # Configuration of costmonitor
    # --------------------
    def getNoalgStep(self):
        from TriggerMenuMT.HLTMenuConfig.CalibCosmicMon.StreamingChainConfiguration import StreamingSequenceCfg
        return self.getStep(1,"noalg", [StreamingSequenceCfg] )

