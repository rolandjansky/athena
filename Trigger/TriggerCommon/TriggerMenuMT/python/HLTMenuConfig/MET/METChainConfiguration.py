# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.MET.METChainConfiguration")


from ..Menu.ChainConfigurationBase import ChainConfigurationBase
from .METMenuSequences import metMenuSequence
from .ConfigHelpers import extractMetRecoDict, metRecoDictToString
from ..Menu.MenuComponents import RecoFragmentsPool, ChainStep


#----------------------------------------------------------------
# Class to configure chain
#----------------------------------------------------------------
class MetChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)
        # Only some subset of keys in the METChainParts dictionary describe
        # reconstruction details - only these keys are passed down into the menu
        # sequence (the actual hypo tool is created later)
        self.recoDict = extractMetRecoDict(self.dict["chainParts"])
        
    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):                            
        log.debug("Assembling chain for " + self.chainName)
        
        # Right now we only ever have one step, however we may want to
        # reintroduce the automatic cell > 50 preselection
        # this function interpretst the reco dict extracted in __init__
        mainStep = self.getMetStep()
        return self.buildChain([mainStep])

    def getMetStep(self):
        """ Use the reco-dict to construct a single MET step """
        stepName = "Step1_met_{}".format(metRecoDictToString(self.recoDict) )
        seq = RecoFragmentsPool.retrieve(metMenuSequence, None, **self.recoDict)
        return ChainStep(stepName, [seq], multiplicity=[1])
