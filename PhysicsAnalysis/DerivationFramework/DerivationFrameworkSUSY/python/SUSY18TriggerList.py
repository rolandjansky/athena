# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
# @class SUSY18Triggers
#
class SUSY18Triggers(object):
    """
    Class to manipulate trigger menus
    """
    def __init__(self):
        # muons
        self.__single_muon_triggers_2015 = [ #SMT
            "HLT_mu20_iloose_L1MU15",
            "HLT_mu40"
            ]
        self.__single_muon_triggers_2016_A = [ #SMT
            "HLT_mu24_ivarmedium",
            "HLT_mu40"
            ]
        self.__single_muon_triggers_2016_B_D3 = [ #SMT
            "HLT_mu24_ivarmedium",
            "HLT_mu50"
            ]
        self.__single_muon_triggers_2016_D4_End = [ #SMT
            "HLT_mu26_ivarmedium",
            "HLT_mu50"
            ]
        self.__single_muon_triggers_2017 = [ #SMT
            "HLT_mu26_ivarmedium",
            "HLT_mu50"
            ]
        #electrons
        self.__single_electron_triggers_2015 = [ #SET
            "HLT_e24_lhmedium_L1EM20VH",
            "HLT_e60_lhmedium",
            "HLT_e120_lhloose"
            ]
        self.__single_electron_triggers_2016 = [ #SET
            "HLT_e26_lhtight_nod0_ivarloose",
            "HLT_e60_lhmedium_nod0",
            "HLT_e140_lhloose_nod0"
            ]
        self.__single_electron_triggers_2017 = [ #SET
            "HLT_e26_lhtight_nod0_ivarloose",
            "HLT_e60_lhmedium_nod0",
            "HLT_e140_lhloose_nod0",
            "HLT_e300_etcut"
            ]
        # electron-tau
        self.__electron_tau_triggers_2015_2016 = [ #ETT
            "HLT_e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo", #with J25
            "HLT_e17_lhmedium_nod0_tau80_medium1_tracktwo"
            ]
        self.__electron_tau_triggers_2017_B1B4 = [ #ETT
            "HLT_e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo", #with J25
            "HLT_e17_lhmedium_nod0_tau80_medium1_tracktwo"
            ]
        self.__electron_tau_triggers_2017_B5 = [ #ETT
            "HLT_e24_lhmedium_nod0_ivarloose_tau35_medium1_tracktwo"
            ]
        #muon tau
        self.__muon_tau_triggers_2015_2016 = [ #MTT
            "HLT_mu14_tau25_medium1_tracktwo", #with J25
            "HLT_mu14_ivarloose_tau25_medium1_tracktwo",
            "HLT_mu14_ivarloose_tau35_medium1_tracktwo"
            ]
        self.__muon_tau_triggers_2017_B1B4 = [ #MTT
            "HLT_mu14_ivarloose_tau25_medium1_tracktwo",
            "HLT_mu14_ivarloose_tau35_medium1_tracktwo"
            ]
        self.__muon_tau_triggers_2017_B5 = [ #MTT
            "HLT_mu14_ivarloose_tau35_medium1_tracktwo_L1MU10_TAU20IM_J25_2J20",
            "HLT_mu14_ivarloose_tau35_medium1_tracktwo"
            ]

        #tau+electron+MET
        self.__electron_tau_met_triggers_2016_A = [ #ETMT
            "HLT_e17_lhmedium_tau25_medium1_tracktwo_xe50 "
            ]
        self.__electron_tau_met_triggers_2016_B_End = [ #ETMT
            "HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50"
            ]
        self.__electron_tau_met_triggers_2017 = [ #ETMT
            "HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50"
            ]

        #tau+muon+MET
        self.__muon_tau_met_triggers_2016_A = [ #MTMT
            "HLT_mu14_iloose_tau25_medium1_tracktwo_xe50"
            ]
        self.__muon_tau_met_triggers_2016_B_End = [ #MTMT
            "HLT_mu14_tau25_medium1_tracktwo_xe50"
            ]
        self.__muon_tau_met_triggers_2017 = [ #MTMT
            "HLT_mu14_tau25_medium1_tracktwo_xe50"
            ]

        # lepton tau triggers
        self.__muon_tau_triggers          = (self.__muon_tau_triggers_2015_2016 + \
                                                 self.__muon_tau_triggers_2017_B1B4 + \
                                                 self.__muon_tau_triggers_2017_B5)
        self.__electron_tau_triggers      = (self.__electron_tau_triggers_2015_2016 + \
                                                 self.__electron_tau_triggers_2017_B1B4 + \
                                                 self.__electron_tau_triggers_2017_B5)
        self.__lepton_tau_triggers        = self.__muon_tau_triggers + self.__electron_tau_triggers #LTT
        self.__MTT_logical_or             = ' || '.join( self.__muon_tau_triggers )
        self.__ETT_logical_or             = ' || '.join( self.__electron_tau_triggers )
        # met lepton tau triggers
        self.__muon_tau_met_triggers      = (self.__muon_tau_met_triggers_2016_A + \
                                                 self.__muon_tau_met_triggers_2016_B_End + \
                                                 self.__muon_tau_met_triggers_2017)
        self.__electron_tau_met_triggers  = (self.__electron_tau_met_triggers_2016_A + \
                                                 self.__electron_tau_met_triggers_2016_B_End + \
                                                 self.__electron_tau_met_triggers_2017)
        self.__lepton_tau_met_triggers    = self.__muon_tau_met_triggers + self.__electron_tau_met_triggers #LTMT
        self.__MTMT_logical_or            = ' || '.join( self.__muon_tau_met_triggers )
        self.__ETMT_logical_or            = ' || '.join( self.__electron_tau_met_triggers )
        # single lepton triggers
        self.__SMT_logical_or_2015        = ' || '.join( self.__single_muon_triggers_2015 )
        self.__SMT_logical_or_2016_A      = ' || '.join( self.__single_muon_triggers_2016_A )
        self.__SMT_logical_or_2016_B_D3   = ' || '.join( self.__single_muon_triggers_2016_B_D3 )
        self.__SMT_logical_or_2016_D4_End = ' || '.join( self.__single_muon_triggers_2016_D4_End )
        self.__SMT_logical_or_2017        = ' || '.join( self.__single_muon_triggers_2017 )
        self.__SET_logical_or_2015        = ' || '.join( self.__single_electron_triggers_2015 )
        self.__SET_logical_or_2016        = ' || '.join( self.__single_electron_triggers_2016 )
        self.__SET_logical_or_2017        = ' || '.join( self.__single_electron_triggers_2017 )
        #unique menu
        self.__lepton_triggers            = (self.__single_muon_triggers_2015 + \
                                                 self.__single_muon_triggers_2016_A + \
                                                 self.__single_muon_triggers_2016_B_D3 + \
                                                 self.__single_muon_triggers_2016_D4_End + \
                                                 self.__single_muon_triggers_2017 + \
                                                 self.__single_electron_triggers_2015 + \
                                                 self.__single_electron_triggers_2016 + \
                                                 self.__single_electron_triggers_2017 )

        self.__unique_triggers            = set( self.__lepton_triggers + \
                                                     self.__lepton_tau_triggers + \
                                                     self.__lepton_tau_met_triggers )

        self.__all_triggers_bitwise_or    = '|'.join(self.__unique_triggers)

    @property
    def SMT_OR_2015(self):
        return self.__SMT_logical_or_2015

    @property
    def SMT_OR_2016_A(self):
        return self.__SMT_logical_or_2016_A

    @property
    def SMT_OR_2016_B_D3(self):
        return self.__SMT_logical_or_2016_B_D3

    @property
    def SMT_OR_2016_D4_End(self):
        return self.__SMT_logical_or_2016_D4_End

    @property
    def SMT_OR_2017(self):
        return self.__SMT_logical_or_2017

    @property
    def SET_OR_2015(self):
        return self.__SET_logical_or_2015

    @property
    def SET_OR_2016(self):
        return self.__SET_logical_or_2016

    @property
    def SET_OR_2017(self):
        return self.__SET_logical_or_2017

    @property
    def MTT_OR(self):
        return self.__MTT_logical_or

    @property
    def ETT_OR(self):
        return self.__ETT_logical_or

    @property
    def MTMT_OR(self):
        return self.__MTMT_logical_or

    @property
    def ETMT_OR(self):
        return self.__ETMT_logical_or

    @property
    def AllTrig_BitwiseOR(self):
        return self.__all_triggers_bitwise_or




