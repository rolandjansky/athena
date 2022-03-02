# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging

logging.getLogger().info("Importing %s", __name__)
log = logging.getLogger(__name__)


from ..Config.ChainConfigurationBase import ChainConfigurationBase
from .ConfigHelpers import recoKeys, AlgConfig
from ..Menu.SignatureDicts import METChainParts_Default


def extractMETRecoDict(chainDict, fillDefaults=True):
    """ Extract the keys relevant to reconstruction from a provided dictionary

    If fillDefaults is True then any missing keys will be taken from the
    METChainParts_Default dictionary.
    """
    if fillDefaults:
        return {k: chainDict.get(k, METChainParts_Default[k]) for k in recoKeys}
    else:
        return {k: chainDict[k] for k in recoKeys if k in chainDict}


# ----------------------------------------------------------------
# Class to configure chain
# ----------------------------------------------------------------
class METChainConfiguration(ChainConfigurationBase):
    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self, chainDict)
        # Only some subset of keys in the METChainParts dictionary describe
        # reconstruction details - only these keys are passed down into the menu
        # sequence (the actual hypo tool is created later)
        self.recoDict = extractMETRecoDict(self.chainPart)

    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChainImpl(self):
        log.debug("Assembling chain for %s", self.chainName)
        conf = AlgConfig.fromRecoDict(**self.recoDict)
        return self.buildChain(conf.make_steps(self.dict))
