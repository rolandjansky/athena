# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger( __name__ )


from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLTMenuConfig.MinBias.MinBiasMenuSequences import MinBiasSPSequence, MinBiasTrkSequence, MinBiasMbtsSequence 

#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO,
# so let's make them functions already now
#----------------------------------------------------------------
def MinBiasSPSequenceCfg(flags):
    return MinBiasSPSequence()

def MinBiasTrkSequenceCfg(flags):
    return MinBiasTrkSequence()


def MinBiasMbtsSequenceCfg(flags):
    return MinBiasMbtsSequence()


class MinBiasChainConfig(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)

    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):
        log.debug("Assembling chain for %s", self.chainName)
        steps = []
        if "_mbts" in self.chainName:
            steps.append(self.getMinBiasMbtsStep())

        if "_sp" in self.chainName:
            steps.append(self.getMinBiasSpStep())
        
        # mb_sptrk chains
        if "_sptrk" in self.chainName or "hmt" in self.chainName:
            steps.append(self.getMinBiasTrkStep())

        return self.buildChain(steps)

    def getMinBiasMbtsStep(self):
        return self.getStep(1, 'Mbts',[MinBiasMbtsSequenceCfg])
    
    def getMinBiasSpStep(self):
        return self.getStep(2,'SPCount',[MinBiasSPSequenceCfg])

    def getMinBiasTrkStep(self):
        return self.getStep(3,'TrkCount',[MinBiasTrkSequenceCfg])
