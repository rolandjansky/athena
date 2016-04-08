# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.Lvl1Flags import Lvl1Flags
from TriggerMenuPython.TriggerPythonConfig import *

class MultiMuon:
    def __init__(self, sig_id, lvl1item, l2chains, l2counter, efchains, efcounter):
        self.sig_id = sig_id
        self.lvl1item = lvl1item
        self.l2chains = l2chains
        self.l2counter = l2counter
        self.efchains = efchains
        self.efcounter = efcounter

        mlog = logging.getLogger( 'MultiMuon.py:MultiMuon' )

        ## only these signatures are supported here
        self.__supported = ['2mu4', '2mu4T', '2mu4_MSonly', '2mu4T_MSonly', '3mu4', '3mu4T', '4mu4', '2mu6', '2mu8',
                            '2mu6_NL', 'mu4_mu6',
                            '2mu10', '2mu15',
                            '2mu10_l2muonSA',
                            '3mu6' ,
                            '3mu6_MSonly', '3mu6_MSonly_EMPTY', '3mu6_MSonly_UNPAIRED_ISO',
                            '4mu4T',
                            '2mu4_noOvlpRm', '2mu10_noOvlpRm', '3mu6_MSonly_noOvlpRm',
                            '3mu6_MSonly_nonfilled', 
                            '2mu10_loose', '2mu10_loose_noOvlpRm', '2mu10_loose_empty', '2mu10_empty', '2mu10_loose1', '2mu10_L2StarA',  '2mu10_L2StarB', '2mu10_L2StarC',
                            '2mu13', '2mu13_l2muonSA','2mu13_muFast',
                            #HI
                            '2mu6_MSonly', '2mu6T_MSonly',
                            '2mu6ia1_MSonly', '2mu6ia2_MSonly', '2mu10ia1_MSonly',  '2mu10ia2_MSonly',  '2mu10ia3_MSonly', '2mu10ia4_MSonly',
                            'mu10_mu6ia1_MSonly', 'mu10_mu6ia2_MSonly', 'mu10_mu6ia3_MSonly', 'mu10_mu6ia4_MSonly',
                            '2mu6ia3_MSonly', '2mu6ia4_MSonly',
                            ]
        
        if sig_id not in self.__supported:
            mlog.error(sig_id+' not supprted '+str(self.__supported))
            raise Exception('ERROR: '+sig_id+' not supprted '+str(self.__supported) )

    def generateMenu(self, triggerPythonConfig):
        
        mlog = logging.getLogger( 'MultiMuon.py' )  ## get the logger
        l2parts = [ triggerPythonConfig.getHLTChain(i) for i in self.l2chains ] ## find chains objects given names fro L2
        efparts = [ triggerPythonConfig.getHLTChain(i) for i in self.efchains  ] ## same for EF
        if None in l2parts + efparts:  ## some action if simple chains are not found
            mlog.warning("Basic chains used to built combined chain: "+self.sig_id+" are not present.")
            mlog.warning("This were found:  "+str([c.chain_name for c in  l2parts + efparts if c != None ]))
            mlog.warning("This were needed: "+str( self.l2chains +  self.efchains))
            return 

        l2chain = HLTChain(chain_name='L2_'+self.sig_id, chain_counter=str(self.l2counter), lower_chain_name=self.lvl1item, level='L2', prescale='1', pass_through='0')
        l2chain.addStreamTag('muons')
        if TriggerFlags.doLVL2():
            l2chain.mergeAndAppendChains(l2parts)  


        ## identical for EF as for L2
        efchain = HLTChain(chain_name='EF_'+self.sig_id, chain_counter=str(self.efcounter), lower_chain_name='L2_'+self.sig_id, level='EF', prescale='1', pass_through='0')
        efchain.addStreamTag('muons')
        efchain.mergeAndAppendChains(efparts)  

        #add groups to the muon chains
        l2chain.addGroup(["RATE:MultiMuon",'BW:Muon'])
        efchain.addGroup(["RATE:MultiMuon",'BW:Muon'])
        
        triggerPythonConfig.registerHLTChain(self.sig_id, l2chain) 
        triggerPythonConfig.registerHLTChain(self.sig_id, efchain)
        
