# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

########################################################################
#
# SliceDef file for muon chains/signatures
#
#########################################################################
from AthenaCommon import CfgGetter
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)

log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Muon.MuonDef")

from TriggerMenuMT.HLTMenuConfig.Menu.HltConfig import *


############################### Class/function to configure muon chains ##############################
class Chain_mu(L2EFChainDef):

  def __init__(self, chainDict):
    self.chainName       = chainDict['chainName']
    self.chainL1Item     = chainDict['L1item']
    self.chainPart       = chainDict['chainParts']
    self.chainPartL1Item = self.chainPart['L1item']
    self.mult            = int(self.chainPart['multiplicity'])
    self.chainPartName   = self.chainPart['chainPartName']
    self.chainPartNameNoMult = self.chainPartName[1:] if self.mult > 1 else self.chainPartName
    self.L2InputTE = self.chainPartL1Item or self.chainL1Item
    if self.L2InputTE:      # cut of L1_, _EMPTY,..., & multiplicity
      self.L2InputTE = self.L2InputTE.replace("L1_","")
      self.L2InputTE = self.L2InputTE.split("_")[0]
      self.L2InputTE = self.L2InputTE[1:] if self.L2InputTE[0].isdigit() else self.L2InputTE
    
    # --- call corresponding functions to set up chains ---
    if        not self.chainPart['extra'] \
          and not self.chainPart['FSinfo'] \
          and not self.chainPart['hypoInfo'] \
          and not self.chainPart['reccalibInfo'] :
      self.setup_muXX()      
    else:
      log.error('Chain %s could not be assembled' % (self.chainPartName))
      return False



############################### DEFINE GROUPS OF CHAIN CONFIGURATIONS HERE ##############################
  def setup_muXX(self):

    log.info('Function to setup muXX chains to be implemented')
    
#################################################################################################


