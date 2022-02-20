# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from ..Config.ChainConfigurationBase import ChainConfigurationBase

from .BjetMenuSequences import getBJetSequence

#----------------------------------------------------------------
# Class to configure chain
#----------------------------------------------------------------
class BjetChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict, jc_name):
        ChainConfigurationBase.__init__(self, chainDict)

        self.jc_name = jc_name

    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChainImpl(self):                            
        log.debug("Assembling chain for %s", self.chainName)

        stepName = f"Step2_{self.jc_name}_bjet"
        chainSteps = [self.getStep(2, stepName, [getBJetSequence], jc_name=self.jc_name)]

        myChain = self.buildChain(chainSteps)
        return myChain









