# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging

from TriggerMenuPython.TriggerPythonConfig import *
from TriggerJobOpts.TriggerFlags import TriggerFlags

from TrigmuFast.TrigmuFastConfig import *
from TrigMuonHypo.TrigMuonHypoConfig import *


class Stau:
    def __init__(self, sig_id, l1item, l1threshold, lvl2Conf, counterId):
        mlog = logging.getLogger( 'Stau.py:Stau' )

        self.sig_id = sig_id
        self.l1threshold = l1threshold

        self.__supported = [ "stau" ] ## only this signatures are supprted here
        if sig_id not in self.__supported:
            mlog.error(sig_id+' not supprted '+str(self.__supported))
            raise Exception('ERROR: '+sig_id+' not supprted '+str(self.__supported) )



        self.muFast = TrigmuFastConfig(lvl2Conf)        
        self.stauHypo = StauHypoConfig(lvl2Conf)
        

        # defining the L2 chain for signatures
        self.l2chain = HLTChain( chain_name="L2_"+sig_id,
                                 chain_counter=counterId,
                                 lower_chain_name=l1item,
                                 level="L2",
                                 prescale='1',
                                 pass_through='0')
        self.l2chain.addStreamTag('muons', prescale='1')
        self.l2chain.addGroup(['RATE:SingleMuon'])

        # defining the EF chain for signatures
        self.efchain = HLTChain( chain_name="EF_"+sig_id,
                                 chain_counter=counterId,
                                 lower_chain_name="L2_"+sig_id,
                                 level="EF",
                                 prescale='1',
                                 pass_through='1')
        self.efchain.addStreamTag('muons', prescale='1')
        self.efchain.addGroup(['RATE:SingleMuon'])

        
    def generateMenu(self, triggerPythonConfig):
        from  TriggerMenuPython.MuonSliceFlags import MuonSliceFlags ## slice flags are import so late to make sure they are never before used

        TE = self.l1threshold
        ##########################################
        # Starting configuring the L2 signatures #
        ##########################################
 
        # muFast
        ##########
        sequence = []
        if MuonSliceFlags.doL2Muon() and TriggerFlags.doLVL2():

            sequence += [ self.muFast, self.stauHypo ]
            TE = triggerPythonConfig.addSequence(TE, sequence , 'L2_'+self.sig_id )
            self.l2chain.addHLTSignature(TE)    


        # print "registering chains", self.sig_id
        triggerPythonConfig.registerHLTChain(self.sig_id, self.l2chain)
        triggerPythonConfig.registerHLTChain(self.sig_id, self.efchain)

#             name       item       thr    config  counter

Staus = [ Stau('stau',    'L1_MU10',  'MU10',  'Muon',  '436' ) ]
