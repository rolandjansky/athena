# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerMenuPython.TriggerPythonConfig import *
from AthenaCommon.Logging import logging
from TrigL2MissingET.TrigL2MissingETConfig import L2MissingET_Fex

from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Fex
from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypo_te150
from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypo_te250
from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypo_te360
from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypo_te650
from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypo_xe15
from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypo_xe20
from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypo_xe25
from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypo_xe30
from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypo_xe40
from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypo_xe50
from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypo_xe70
from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypo_xe80
from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypo_te150
from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypo_te250
from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypo_te360
from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypo_te650
from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypo_xe15
from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypo_xe20
from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypo_xe25
from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypo_xe30
from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypo_xe40
from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypo_xe50
from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypo_xe70
from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypo_xe80

from AthenaCommon.Logging import logging

class MissingET:
    def __init__(self, sig_id):
        mlog = logging.getLogger( 'MissingET.py' )

        self.sig_id = sig_id
        self.final_te = sig_id

        self.__supported = ["te150", "te250", "te360", "te650", "xe15", "xe20", "xe25", "xe30", "xe40", "xe50", "xe70", "xe80"]
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supported"+str(self.__supported) )

        self.l2metfex =  L2MissingET_Fex()
        self.l2methypo = {"te150" : L2MetHypo_te150(),
                          "te250" : L2MetHypo_te250(),
                          "te360" : L2MetHypo_te360(),
                          "te650" : L2MetHypo_te650(),
                          "xe15" : L2MetHypo_xe15(),
                          "xe20" : L2MetHypo_xe20(),
                          "xe25" : L2MetHypo_xe25(),
                          "xe30" : L2MetHypo_xe30(),
                          "xe40" : L2MetHypo_xe40(),
                          "xe50" : L2MetHypo_xe50(),
                          "xe70" : L2MetHypo_xe70(),
                          "xe80" : L2MetHypo_xe80() }

        self.efmetfex =  EFMissingET_Fex()
        self.efmethypo = {"te150" : EFMetHypo_te150(),
                          "te250" : EFMetHypo_te250(),
                          "te360" : EFMetHypo_te360(),
                          "te650" : EFMetHypo_te650(),
                          "xe15" : EFMetHypo_xe15(),
                          "xe20" : EFMetHypo_xe20(),
                          "xe25" : EFMetHypo_xe25(),
                          "xe30" : EFMetHypo_xe30(),
                          "xe40" : EFMetHypo_xe40(),
                          "xe50" : EFMetHypo_xe50(),
                          "xe70" : EFMetHypo_xe70(),
                          "xe80" : EFMetHypo_xe80() }

        self.l2_chain_counters = {"te150" : "1088",
                                  "te250" : "1089",
                                  "te360" : "1090",
                                  "te650" : "1091",
                                  "xe15" : "1092",
                                  "xe20" : "1093",
                                  "xe25" : "1094",
                                  "xe30" : "1095",
                                  "xe40" : "1096",
                                  "xe50" : "1097",
                                  "xe70" : "1098",
                                  "xe80" : "1099"}

        self.ef_chain_counters = {"te150" : "1088",
                                  "te250" : "1089",
                                  "te360" : "1090",
                                  "te650" : "1091",
                                  "xe15" : "1092",
                                  "xe20" : "1093",
                                  "xe25" : "1094",
                                  "xe30" : "1095",
                                  "xe40" : "1096",
                                  "xe50" : "1097",
                                  "xe70" : "1098",
                                  "xe80" : "1099"}

        self.l1_item = { "te150" : "L1_TE150",
                       "te250" : "L1_TE250",
                       "te360" : "L1_TE360",
                       "te650" : "L1_TE650",
                       "xe15" : "L1_XE15",
                       "xe20" : "L1_XE20",
                       "xe25" : "L1_XE25",
                       "xe30" : "L1_XE30",
                       "xe40" : "L1_XE40",
                       "xe50" : "L1_XE50",
                       "xe70" : "L1_XE70",
                       "xe80" : "L1_XE80"}

        self.l1_te= { "te150" : "TE150",
                       "te250" : "TE250",
                       "te360" : "TE360",
                       "te650" : "TE650",
                       "xe15" : "XE15",
                       "xe20" : "XE20",
                       "xe25" : "XE25",
                       "xe30" : "XE30",
                       "xe40" : "XE40",
                       "xe50" : "XE50",
                       "xe70" : "XE70",
                       "xe80" : "XE80"}

        self.l1_temu= { "te150" : "L2_mu6l",
                        "te250" : "L2_mu6l",
                        "te360" : "L2_mu6l",
                        "te650" : "L2_mu6l",
                        "xe15" : "L2_mu6l",
                        "xe20" : "L2_mu6l",
                        "xe25" : "L2_mu6l",
                        "xe30" : "L2_mu6l",
                        "xe40" : "L2_mu6l",
                        "xe50" : "L2_mu6l",
                        "xe70" : "L2_mu6l",
                        "xe80" : "L2_mu6l"}

        self.final_te = { "te150" : "te150",
                          "te250" : "te250",
                          "te360" : "te360",
                          "te650" : "te650",
                          "xe15" : "xe15",
                          "xe20" : "xe20",
                          "xe25" : "xe25",
                          "xe30" : "xe30",
                          "xe40" : "xe40",
                          "xe50" : "xe50",
                          "xe70" : "xe70",
                          "xe80" : "xe80"}

        self.mult = { "te150" : 1,
                      "te250" : 1,
                      "te360" : 1,
                      "te650" : 1,
                      "xe15" : 1,
                      "xe20" : 1,
                      "xe25" : 1,
                      "xe30" : 1,
                      "xe40" : 1,
                      "xe50" : 1,
                      "xe70" : 1,
                      "xe80" : 1}

    def generateMenu(self, triggerPythonConfig):
#        from TriggerMenuPython.METSliceFlags import METSliceFlags
        from __main__ import TriggerFlags
        from string import atoi
        # build L2
        l2chain = HLTChain (chain_name="L2_"+self.sig_id,
                            chain_counter=self.l2_chain_counters[self.sig_id],
                            lower_chain_name=self.l1_item[self.sig_id],
                            level="L2", prescale="1", pass_through="0")
        l2chain.addStreamTag("missinget")
        l2chain.addTriggerTypeBit(atoi(self.l2_chain_counters[self.sig_id] ))

        TE = self.l1_te[self.sig_id]
        sequence = []
        # use only calorimeter
        if TriggerFlags.METSlice.doL2Calo():

            sequence = [self.l2metfex, self.l2methypo[self.sig_id] ]


        if TriggerFlags.MuonSlice.doL2Muon():

            TE = [ self.l1_te[self.sig_id], self.l1_temu[self.sig_id]]


        if len(sequence) is not 0:

            TE = triggerPythonConfig.addSequence(TE, sequence,"L2_"+self.final_te[self.sig_id])
            l2chain.addHLTSignature([TE]*self.mult[self.sig_id])

        efchain = HLTChain( chain_name="EF_"+self.sig_id,
                            chain_counter=self.ef_chain_counters[self.sig_id],
                            lower_chain_name="L2_"+self.sig_id,
                            level="EF", prescale="1", pass_through="0")
        efchain.addStreamTag("missinget")
        efchain.addTriggerTypeBit(atoi(self.ef_chain_counters[self.sig_id] ))

        sequence = []
        if TriggerFlags.METSlice.doEFCalo():
            sequence += self.efmetfex
            sequence += self.efmethypo[self.sig_id]
#        if len(sequence) is not 0:
            TE = triggerPythonConfig.addSequence(TE,
                                                 [self.efmetfex,
                                                  self.efmethypo[self.sig_id] ],
                                                 "EF_"+self.final_te[self.sig_id])
            efchain.addHLTSignature([TE]*self.mult[self.sig_id])

        triggerPythonConfig.addHLTChain(l2chain)
        triggerPythonConfig.addHLTChain(efchain)

MissingETs = [MissingET('te150'),
              MissingET('te250'),
              MissingET('te360'),
              MissingET('te650'),
              MissingET('xe15'),
              MissingET('xe20'),
              MissingET('xe25'),
              MissingET('xe30'),
              MissingET('xe40'),
              MissingET('xe50'),
              MissingET('xe70'),
              MissingET('xe80')]
