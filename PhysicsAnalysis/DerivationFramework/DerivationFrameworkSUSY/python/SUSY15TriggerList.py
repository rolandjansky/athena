# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from DerivationFrameworkSUSY.SUSYCommonTriggerList import MET_2018,MET_2017,MET_2016,MET_2015,JetTrigger_2016

triggers_met = MET_2018 + MET_2017 + MET_2016 + MET_2015 + JetTrigger_2016 # JetTrigger_2016 = HLT_noalg_L1J400 --> used as a backup L1 MET

triggers_jet = [
                "HLT_j340", # single jet
                "HLT_j380", 
                "HLT_j400", 
                "HLT_j450", 
                "HLT_3j175", # 3 jets
                "HLT_3j200",
                "HLT_3j225",
                "HLT_2j275_j140",
                "HLT_2j250_j120",
                "HLT_2j220_j120", 
                "HLT_4j85", # 4 jets
                "HLT_4j100", 
                "HLT_4j110",
                "HLT_4j120",
                "HLT_4j130",
                "HLT_5j55", # 5 jets                
                "HLT_5j85", 
                "HLT_5j85_lcw",
                "HLT_5j75_0eta250",
                "HLT_5j65_0eta240_L14J150ETA25",
                "HLT_5j60",
                "HLT_5j70",
                "HLT_5j70_L14J15",
                "HLT_5j85_L14J15",
                "HLT_5j90",
                "HLT_5j100",
                "HLT_6j45_0eta240", # 6 jets
                "HLT_6j55_0eta240_L14J150ETA25",
                "HLT_6j45",
                "HLT_6j60",
                "HLT_6j70",
                "HLT_6j60_L14J15",
                "HLT_6j70_L14J15",
                "HLT_6j85",
                "HLT_7j45", # 7 jets
                "HLT_7j45_L14J15", 
                "HLT_7j50",
                "HLT_4j60_gsc100_boffperf_split", #gsc
                "HLT_4j60_gsc110_boffperf_split",
                "HLT_4j60_gsc115_boffperf_split",
                "HLT_4j70_gsc120_boffperf_split",
                "HLT_4j70_gsc130_boffperf_split",
                "HLT_5j50_gsc70_boffperf_split",
                "HLT_5j55_gsc75_boffperf_split",
                "HLT_5j60_gsc85_boffperf_split",
                "HLT_5j60_gsc90_boffperf_split",
                "HLT_5j60_gsc100_boffperf_split",
                "HLT_6j45_gsc60_boffperf_split",
                "HLT_6j50_gsc65_boffperf_split", 
                "HLT_6j50_gsc70_boffperf_split",
                "HLT_6j60_gsc85_boffperf_split",
                "HLT_7j25_gsc45_boffperf_split_L14J20",
                "HLT_7j25_gsc50_boffperf_split_L14J20"
                
]

triggers_lep = [
                "HLT_mu20_iloose_L1MU15",
                "HLT_mu24_iloose",
                "HLT_mu24_ivarloose",
                "HLT_mu24_imedium",
                "HLT_mu24_ivarmedium",
                "HLT_mu26_imedium",
                "HLT_mu26_ivarmedium",
                "HLT_mu40",
                "HLT_mu50",
                "HLT_mu60_0eta105_msonly",
                "HLT_mu80_msonly_3layersEC",
                "HLT_e24_lhmedium_L1EM20VH",
                "HLT_e24_lhtight_nod0_ivarloose",
                "HLT_e26_lhtight_nod0",
                "HLT_e26_lhtight_nod0_ivarloose",
                "HLT_e60_lhmedium_nod0",
                "HLT_e60_lhmedium",
                "HLT_e60_medium",
                "HLT_e120_lhloose",
                "HLT_e140_lhloose_nod0",
                "HLT_e300_etcut"

]

triggers_photon = [
                "HLT_g140_loose",
                "HLT_2g50_loose",
                "HLT_g200_loose",
                "HLT_g200_loose_L1EM24VHIM",
                "HLT_2g50_loose_L12EM20VH",
                "HLT_2g60_loose_L12EM20VH",
]
