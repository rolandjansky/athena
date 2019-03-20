# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

########################################################################
#
# SliceDef file for muon chains/signatures
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)

log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Muon.MuonDef")

from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase


############################################# 
###  Class/function to configure muon chains 
#############################################

class MuonChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)
        
    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):                            
        myStepNames = []
        chainSteps = []
        log.debug("Assembling chain for " + self.chainName)
        # --------------------
        # define here the names of the steps and obtain the chainStep configuration 
        # --------------------
        if        not self.chainPart['extra'] \
              and not self.chainPart['FSinfo'] :
          
          myStepNames += ["Step1_etcut"]
          myStepNames += ["Step2_etcut"]            
          for step in myStepNames:
            log.debug("Assembling steps for chain %s", self.chainName)
            #chainSteps += [self.getMuonSteps(step)]

        else:
            raise RuntimeError("Chain configuration unknown for chain: " + self.chainName )
            
        myChain = self.buildChain(chainSteps)
        return myChain
        







