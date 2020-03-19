# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.CalibCosmicMon.BeamspotChainConfiguration")

from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TrigStreamerHypo.TrigStreamerHypoConfigMT import StreamerHypoToolMTgenerator
from TrigStreamerHypo.TrigStreamerHypoConf import TrigStreamerHypoAlgMT
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence
from AthenaCommon.CFElements import seqAND
from DecisionHandling.DecisionHandlingConf import InputMakerForRoI

#----------------------------------------------------------------
class CosmicChainConfiguration(ChainConfigurationBase):

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
            "sct_noise":[self.getNoalgStep()],
        }

        key = self.chainPart['chainPartName']
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

