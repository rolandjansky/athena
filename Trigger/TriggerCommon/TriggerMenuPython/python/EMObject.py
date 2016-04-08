# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from string import atoi
from AthenaCommon.Logging import logging
        
from TriggerMenuPython.TriggerPythonConfig import *
from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.Lvl1Flags import Lvl1Flags

class EMObject:
    def __init__(self, sig_id):
        mlog = logging.getLogger( 'EMObject.py:EMObject' )  ## take the logger please do not use print ... it is hard to find from where message is comming then
        
        self.sig_id = sig_id  ## remember the signature ID for the use in generate function
    
        self.__supported = [ 'em105_passHLT' ]
        if sig_id not in self.__supported:
            mlog.error(sig_id+' not supprted '+str(self.__supported))
            raise Exception('ERROR: '+sig_id+' not supprted '+str(self.__supported) )

        ## LVL1 Thresholds from which sequences are to be seeded
        self.l1_te = { 
                       'em105_passHLT' : 'EM100'
                       }
            
        ## L2 chains predefinitions

        if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():
            self.l2_chains = {
                               'em105_passHLT':  HLTChain( chain_name='L2_em105_passHLT',     chain_counter='50', lower_chain_name='L1_EM30', level='L2', prescale='1', pass_through='0')
                               }
            
            self.ef_chains = {
                               'em105_passHLT':   HLTChain( chain_name='EF_em105_passHLT',     chain_counter='50', lower_chain_name='L2_em105_passHLT', level='EF', prescale='1', pass_through='0')
                               }
            
        else:
            self.l2_chains = { 
                               'em105_passHLT':  HLTChain( chain_name='L2_em105_passHLT',     chain_counter='50', lower_chain_name='L1_EM85', level='L2', prescale='1', pass_through='0')
                               }
            
            self.ef_chains = { 
                               'em105_passHLT':   HLTChain( chain_name='EF_em105_passHLT',     chain_counter='50', lower_chain_name='L2_em105_passHLT', level='EF', prescale='1', pass_through='0')
                               }
        
    def generateMenu(self, triggerPythonConfig):
        from TriggerMenuPython.EgammaSliceFlags import EgammaSliceFlags ## slice flags are import so late to make sure they are never before used

        ## build L2
        l2chain = self.l2_chains[self.sig_id]
        l2chain.addStreamTag('egamma') ## assign streamtag to chain (could be as well done in __init__

        l2chain.addGroup(["RATE:Egamma"])
        l2chain.addGroup(["BW:Egamma"])
        
        ## varaible TE is temporary and keeps the current input/output name of the TriggerElement
        TE = self.l1_te[self.sig_id] ## asign it to lvl1 threshold 


        ## build EF
        ## note that TE name is preserved between levels
        efchain = self.ef_chains[self.sig_id]
        efchain.addStreamTag('egamma')

        efchain.addGroup(["RATE:Egamma"])
        efchain.addGroup(["BW:Egamma"])
        ## register the chain in the pool of available chains
        ## the decision whether the chain wil be in XML depends on flags in EgammaSliceFlags.signatures
        ## and does not need to be taken here, it is automatized in GenerateMenu.py
        triggerPythonConfig.registerHLTChain(self.sig_id, l2chain) 
        triggerPythonConfig.registerHLTChain(self.sig_id, efchain) 

EMObjects = [ 
              EMObject('em105_passHLT')
              ]
