# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger( __name__ )


from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from DecisionHandling.DecisionHandlingConf import InputMakerForRoI
#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO,
# so let's make them functions already now
#----------------------------------------------------------------

def MinBiasSequenceCfg( flags ):
    return MinBiasMenuSequence()

def MinBiasMenuSequence():

    inputMakerAlg = InputMakerForRoI("IM_minbiasInputMaker", mergeOutputs=False)
    inputMakerAlg.RoIs="minbiasInputRoIs"

class MinBiasChainConfig(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)

                # ----------------------
                # Assemble the chain depending on information from chainName
                # ----------------------
    def assembleChain(self):
        return self.buildChain([]) # empty list of steps and the chain requires none
