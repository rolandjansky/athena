# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from ..Menu.ChainConfigurationBase import ChainConfigurationBase

from .BjetMenuSequences import getBJetSequence

#----------------------------------------------------------------
# Class to configure chain
#----------------------------------------------------------------
class BjetChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict, jc_name):
        ChainConfigurationBase.__init__(self, chainDict)

        # TODO: figure out how to get the jet collection name
        self.jc_name = jc_name

    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChainImpl(self):                            
        log.debug("Assembling chain for %s", self.chainName)

        def bjetSequenceCfg(flags):
            return getBJetSequence(
                jc_name=self.jc_name
            )

        chainSteps = [self.getStep(2, "Step2_bjet", [bjetSequenceCfg])]

        myChain = self.buildChain(chainSteps)
        return myChain









