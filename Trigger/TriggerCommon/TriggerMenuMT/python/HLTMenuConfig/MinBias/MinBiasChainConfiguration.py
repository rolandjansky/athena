# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger( __name__ )


from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLTMenuConfig.MinBias.MinBiasMenuSequences import MinBiasSPSequence, MinBiasTrkSequence

#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO,
# so let's make them functions already now
#----------------------------------------------------------------
def MinBiasSPSequenceCfg(flags):
    return MinBiasSPSequence()

def MinBiasTrkSequenceCfg(flags):
    return MinBiasTrkSequence()


class MinBiasChainConfig(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)

    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):
        log.debug("Assembling chain for %s", self.chainName)
        # mb_sptrk chains
        if "mb_sptrk" in self.chainName or "hmt" in self.chainName:
            SpStep = self.getMinBiasSpStep()
            TrkStep = self.getMinBiasTrkStep()
            return self.buildChain([SpStep,TrkStep])
        # performance chain for the aboce
        if "mb_sp_" in self.chainName:
            SpStep = self.getMinBiasSpStep()
            return self.buildChain([SpStep])


    
    def getMinBiasSpStep(self):
        return self.getStep(1,'SPCount',[MinBiasSPSequenceCfg])

    def getMinBiasTrkStep(self):
        return self.getStep(2,'TrkCount',[MinBiasTrkSequenceCfg])
