# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.MET.METChainConfiguration")


from ..Menu.ChainConfigurationBase import ChainConfigurationBase
from .ConfigHelpers import extractMETRecoDict, metRecoDictToString, AlgConfig
from ..Menu.MenuComponents import ChainStep


#----------------------------------------------------------------
# Class to configure chain
#----------------------------------------------------------------
class METChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)
        # Only some subset of keys in the METChainParts dictionary describe
        # reconstruction details - only these keys are passed down into the menu
        # sequence (the actual hypo tool is created later)
        self.recoDict = extractMETRecoDict(self.chainPart)
        
    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):                            
        log.debug("Assembling chain for " + self.chainName)
        
        # Right now we only ever have one step, however we may want to
        # reintroduce the automatic cell > 50 preselection
        # this function interpretst the reco dict extracted in __init__
        mainStep = self.getMETStep()
        return self.buildChain([mainStep])

    def getMETStep(self):
        """ Use the reco-dict to construct a single MET step """
        stepName = "Step1_met_{}".format(metRecoDictToString(self.recoDict) )
        conf = AlgConfig.fromRecoDict(**self.recoDict)
        seq = conf.menuSequence
        return ChainStep(stepName, [seq], multiplicity=[1], chainDicts=[self.dict])
