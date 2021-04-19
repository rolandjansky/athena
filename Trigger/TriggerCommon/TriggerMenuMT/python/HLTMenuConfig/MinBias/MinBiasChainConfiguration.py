# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger( __name__ )

from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import EmptyMenuSequence
from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLTMenuConfig.MinBias.MinBiasMenuSequences import MinBiasSPSequence, MinBiasTrkSequence, MinBiasMbtsSequence, MinBiasZVertexFinderSequence
from TriggerMenuMT.HLTMenuConfig.MinBias.ALFAMenuSequences import ALFAPerfSequence

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

def ALFAPerfSequenceCfg(flags):
    return ALFAPerfSequence()

def MinBiasZVertexFinderCfg(flags):
    return MinBiasZVertexFinderSequence()

class MinBiasChainConfig(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)

    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):
        log.debug("Assembling chain for %s", self.chainName)
        steps = []
        if "mbts" == self.chainPart['recoAlg'][0]:
            steps.append(self.getMinBiasMbtsStep())

        if self.chainPart['recoAlg'][0] in ['sp', 'sptrk', 'hmt']:
            steps.append(self.getMinBiasSpStep())

        if self.chainPart['recoAlg'][0] in ['sptrk', 'hmt']:
            if self.chainPart['pileupInfo']:
                steps.append(self.getMinBiasZFindStep())
            else:
                steps.append(self.getMinBiasEmptyZFindStep())

            steps.append(self.getMinBiasTrkStep())

        if "alfaperf" == self.chainPart["recoAlg"][0]:
            steps.append(self.getALFAPerfStep())

        return self.buildChain(steps)

    def getMinBiasMbtsStep(self):
        return self.getStep(1, 'Mbts',[MinBiasMbtsSequenceCfg])
    
    def getMinBiasSpStep(self):
        return self.getStep(2,'SPCount',[MinBiasSPSequenceCfg])

    def getMinBiasZFindStep(self):
        return self.getStep(3,'ZFind',[MinBiasZVertexFinderCfg])

    def getMinBiasEmptyZFindStep(self):
        return self.getStep(3,'EmptyZFind',[lambda flags: EmptyMenuSequence("EmptyZFind")])

    def getMinBiasTrkStep(self):
        return self.getStep(4,'TrkCount',[MinBiasTrkSequenceCfg])

    def getALFAPerfStep(self):
        return self.getStep(1,'ALFAPerf',[ALFAPerfSequenceCfg])

