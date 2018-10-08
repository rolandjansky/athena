# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# add the common trigger list
from DerivationFrameworkSUSY.SUSYCommonTriggerList import *

SUSY7SingleLeptonTriggers = SingleLepton_2018 + SingleLepton_2017 + SingleLepton_2016 + SingleLepton_2015

SUSY7DiLeptonTriggers = DiLepton_2018 + DiLepton_2017 + DiLepton_2016 + DiLepton_2015 + [
	# 2017
	'HLT_2e22_lhvloose_nod0',
	'HLT_2mu15_L1MU10'
]

SUSY7LeptonJetTriggers = [
	# 2015
	'HLT_mu6_2j40_0eta490_invm400_L1MU6_J30.0ETA49_2J20.0ETA49',
	'HLT_mu6_2j40_0eta490_invm600_L1MU6_J30.0ETA49_2J20.0ETA49',
	# 2016
	'HLT_e15_lhtight_iloose_3j20_L1EM13VH_3J20',
	'HLT_mu14_ivarloose_3j20_L1MU10_3J20',
	'HLT_mu6_ivarloose_2j40_0eta490_invm1000_L1MU6_J30.0ETA49_2J20.0ETA49',
	# 2017
	'HLT_e20_lhtight_nod0_ivarloose_3j20_L1EM18VHI_3J20',
	'HLT_mu20_ivarmedium_L1MU10_2J15_J20',
	'HLT_mu20_ivarmedium_L1MU10_2J20'
]

SUSY7LeptonTriggers = SUSY7SingleLeptonTriggers + SUSY7DiLeptonTriggers + SUSY7LeptonJetTriggers

SUSY7PhotonTriggers = SinglePhoton_2018 + SinglePhoton_2017 + SinglePhoton_2016 + SinglePhoton_2015

SUSY7XETriggers = MET_2018 + MET_2017 + MET_2016 + MET_2015 + [
    # 2017
    'HLT_noalg_L1J400',
	# 2016
    'HLT_xe100_mht_L1XE50',
	# 2015
	'HLT_xe70_tc_lcw',
	'HLT_xe70'
]

# (q/b-jet || g) && MET
SUSY7CombinedXETriggers = [
    # 2015
    'HLT_g40_tight_xe40noL1',
    'HLT_j80_xe80',
    # 2016
    'HLT_j80_bmv2c2060_split_xe60_L12J50_XE40',
    'HLT_g45_tight_L1EM22VHI_xe45noL1',
    'HLT_g60_loose_xe60noL1',
    'HLT_g70_loose_xe70noL1',
    # 2017
    'HLT_g80_loose_xe80noL1',
    'HLT_j60_gsc100_bmv2c1050_split_xe80_mht',
    'HLT_j60_gsc100_bmv2c1050_split_xe80_mht_L1XE60',
    'HLT_2j25_gsc45_bmv2c1070_split_xe80_mht_L12J15_XE55',
    'HLT_3j15_gsc35_bmv2c1077_split_xe60_mht_L13J15.0ETA25_XE40',
    'HLT_g80_loose_icalovloose_L1EM24VHI_xe80noL1'  
]

# 1 b-jet, 2 b-jets, b+VHF (maybe we remove 1 b-jet?)
SUSY7BjetTriggers = MultiBJet_2018 + [
	# 2015
	'HLT_j225_bloose',
	'HLT_j65_btight_3j65_L13J25.0ETA23',
	'HLT_j70_bmedium_3j70_L13J25.0ETA23',
	'HLT_2j35_btight_2j35_L13J25.0ETA23',
	'HLT_2j45_bmedium_2j45_L13J25.0ETA23',
	'HLT_j100_2j55_bmedium',
	'HLT_j150_bmedium_j50_bmedium',
	'HLT_2j65_btight_j65',
	'HLT_2j70_bmedium_j70',
	'HLT_j175_bmedium',
	'HLT_2j55_bloose_L14J20.0ETA49',
	'HLT_2j55_bmedium_2j55',
	# 2016
	'HLT_j175_bmv2c2040_split',
	'HLT_j65_bmv2c2070_split_3j65_L14J15',
	'HLT_j55_bmv2c2060_split_ht500_L14J15',
	'HLT_j175_bmv2c2040_split',
	'HLT_j225_bmv2c2060_split',
	'HLT_j275_bmv2c2070_split',
	'HLT_j300_bmv2c2077_split',
	'HLT_j360_bmv2c2085_split',
	'HLT_j75_bmv2c2070_split_3j75_L14J15.0ETA25',
	'HLT_j150_bmv2c2077_split_j50_bmv2c2077_split',
	'HLT_2j35_bmv2c2070_split_2j35_L14J15',
	'HLT_2j65_bmv2c2070_split_j65',
	'HLT_2j55_bmv2c2070_split_ht300_L14J15',
	'HLT_j100_2j55_bmv2c2077_split',
	'HLT_j150_bmv2c2060_split_j50_bmv2c2060_split',
	'HLT_2j35_bmv2c2060_split_2j35_L14J15.0ETA25',
	'HLT_2j35_bmv2c2050_split_2j35_L14J15',
	'HLT_2j70_bmv2c2070_split_j70',
	'HLT_2j55_bmv2c2060_split_ht300_L14J15',
	'HLT_j100_2j55_bmv2c2060_split'
	'HLT_2j45_bmv2c2077_split_3j45',
	'HLT_2j45_bmv2c2077_split_3j45_L14J15.0ETA25',
	'HLT_2j70_bmv2c2060_split_j70',
	'HLT_2j75_bmv2c2070_split_j75',
	'HLT_j175_bmv2c2060_split_j50_bmv2c2050_split',
	'HLT_j175_bmv2c2060_split_j50_bmv2c2050_split',
	# 2017
	'HLT_j175_gsc225_bmv2c1040_split',
	'HLT_j225_gsc275_bmv2c1070_split',
	'HLT_j225_gsc300_bmv2c1077_split',
	'HLT_j225_gsc360_bmv2c1085_split',
	'HLT_j50_gsc65_bmv2c1040_split_3j50_gsc65_boffperf_split',
	'HLT_j60_gsc85_bmv2c1050_split_3j60_gsc85_boffperf_split',
	'HLT_j225_gsc275_bmv2c1060_split',
	'HLT_j225_gsc300_bmv2c1070_split',
	'HLT_j225_gsc360_bmv2c1077_split',
	'HLT_j55_gsc75_bmv2c1040_split_3j55_gsc75_boffperf_split',
	'HLT_j150_gsc175_bmv2c1070_split_j45_gsc60_bmv2c1070_split',
	'HLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1077_split_L1J85_3J30',
	'HLT_2j15_gsc35_bmv2c1050_split_2j15_gsc35_boffperf_split_L14J15.0ETA25',
	'HLT_2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L14J15.0ETA25',
	'HLT_2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L14J15.0ETA25',
	'HLT_2j15_gsc35_bmv2c1060_split_3j15_gsc35_boffperf_split',
	'HLT_2j25_gsc45_bmv2c1070_split_3j25_gsc45_boffperf_split',
	'HLT_3j15_gsc35_bmv2c1070_split_j15_gsc35_boffperf_split_L14J15.0ETA25',
	'HLT_4j15_gsc35_bmv2c1077_split_L14J15.0ETA25',
	'HLT_2j15_gsc35_bmv2c1070_split_2j15_gsc35_bmv2c1085_split_L14J15.0ETA25',
	'HLT_2j15_gsc35_bmv2c1070_split_2j15_gsc35_bmv2c1085',
	'HLT_3j50_gsc65_bmv2c1077_split_L13J35.0ETA23',
	'HLT_j150_gsc175_bmv2c1060_split_j45_gsc60_bmv2c1060_split',
	'HLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split_L1J85_3J30',
	'HLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15.0ETA25',
	'HLT_2j15_gsc35_bmv2c1050_split_3j15_gsc35_boffperf_split',
	'HLT_2j25_gsc45_bmv2c1060_split_3j25_gsc45_boffperf_split',
	'HLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15.0ETA25',
	'HLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15.0ETA25',
	'HLT_j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490',
	'HLT_j35_gsc55_bmv2c1070_split_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49',
	'HLT_j35_gsc55_bmv2c1070_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49',
	'HLT_ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF_AND_2j25_gsc45_bmv2c1070_split',
	'HLT_j175_gsc225_bmv2c2040_split',
	'HLT_j225_gsc275_bmv2c2060_split',
	'HLT_j200_gsc300_bmv2c2070_split',
	'HLT_j225_gsc360_bmv2c2077_split',
	'HLT_j150_bmv2c2050_split_j50_bmv2c2050_split',
	'HLT_j175_bmv2c2050_split_j50_bmv2c2050_split',
	'HLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c2060_split',
	'HLT_3j50_gsc65_bmv2c2077_split_L13J35.0ETA23',
	'HLT_2j15_gsc35_bmv2c2040_split_2j15_gsc35_boffperf_split',
	'HLT_j200_gsc300_bmv2c1077_split',
	'HLT_j35_gsc55_bmv2c1050_split_ht700_L1HT190-J15s5.ETA21',
	'HLT_2j35_gsc55_bmv2c1060_split_ht300_L1HT190-J15s5.ETA21',
	'HLT_2j35_gsc55_bmv2c1050_split_ht300_L1HT190-J15s5.ETA21',
	'HLT_j80_0eta240_j60_j45_320eta490_AND_2j25_gsc45_bmv2c1070_split'
]

# multijet + Ht
SUSY7MultijetTriggers = [
	# 2015
	'HLT_3j175',
	'HLT_4j85',
	'HLT_5j60',
	'HLT_6j45',
	'HLT_ht700_L1J75',
	'HLT_ht850_L1J100',
	# 2016
	'HLT_5j55',
	'HLT_7j45',
	'HLT_10j40_L14J20',
	'HLT_4j100',
	'HLT_5j70_L14J15',
	'HLT_6j60',
	'HLT_6j45_0eta240',
	'HLT_5j65_0eta240_L14J15',
	'HLT_7j45_L14J15',
	'HLT_10j40_L14J15',
	'HLT_3j200',
	'HLT_ht1000',
	'HLT_ht1000_L1J100',
	# 2017
	'HLT_3j225',
	'HLT_4j110',
	'HLT_4j120',
	'HLT_4j60_gsc115_boffperf_split',
	'HLT_4j60_gsc110_boffperf_split',
	'HLT_5j90',
	'HLT_5j90_L14J150ETA25',
	'HLT_5j60_gsc90_boffperf_split',
	'HLT_5j60_gsc90_boffperf_split_L14J150ETA25',
	'HLT_6j70',
	'HLT_6j50_gsc70_boffperf_split',
	'HLT_6j50_gsc70_boffperf_split_L14J150ETA25',
	'HLT_7j45_L14J150ETA25',
	'HLT_7j45_L14J20',
	'HLT_7j25_gsc45_boffperf_split_L14J20',
	'HLT_7j25_gsc45_boffperf_split_L14J150ETA25',
	'HLT_10j40_L14J150ETA25',
	'HLT_10j40_L16J15',
	'HLT_5j85',
	'HLT_5j85_L14J15',
	'HLT_5j50_gsc65_boffperf_split_0eta240_L14J150ETA25',
	'HLT_5j50_gsc70_boffperf_split',
	'HLT_5j50_gsc70_boffperf_split_L14J150ETA25',
	'HLT_5j60_gsc85_boffperf_split',
	'HLT_5j60_gsc85_boffperf_split_L14J150ETA25',
	'HLT_5j65_gsc90_boffperf_split_L14J150ETA25',
	'HLT_5j65_0eta240_L14J150ETA25',
	'HLT_6j85',
	'HLT_6j55_0eta240_L14J150ETA25',
	'HLT_6j60_L14J15',
	'HLT_6j60_gsc85_boffperf_split_L14J150ETA25',
	'HLT_6j70_L14J15',
	'HLT_6j35_gsc55_boffperf_split_0eta240_L14J150ETA25',
	'HLT_6j45_0eta240_L14J20',
	'HLT_6j45_gsc60_boffperf_split',
	'HLT_6j45_gsc60_boffperf_split_L14J150ETA25'
]

# 2 taus (+MET/Jets)
SUSY7TauTriggers = [
	# 2015
	'HLT_tau35_loose1_tracktwo_tau25_loose1_tracktwo_L1TAU20IM_2TAU12IM',
	'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo',
	'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM',
	'HLT_tau35_loose1_tracktwo_tau25_loose1_tracktwo',
	# 2016
	'HLT_tau80_medium1_TAU60_tau50_medium1_L1TAU12',
	'HLT_tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_xe50',
	'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_xe50',
	# 2017
	'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_03dR30_L1DR-TAU20ITAU12I-J25',
	'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I-J25',
	'HLT_tau40_medium1_tracktwo_tau35_medium1_tracktwo',
	'HLT_tau80_medium1_tracktwo_L1TAU60_tau50_medium1_tracktwo_L1TAU12',
	'HLT_tau80_medium1_tracktwo_L1TAU60_tau35_medium1_tracktwo_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I',
	'HLT_tau80_medium1_tracktwo_L1TAU60_tau60_medium1_tracktwo_L1TAU40',
	'HLT_tau60_medium1_tracktwo_tau25_medium1_tracktwo_xe50',
	'HLT_tau80_medium1_L1TAU60_tau50_medium1_L1TAU12',
	'HLT_tau125_medium1_tracktwo_tau50_medium1_tracktwo_L1TAU12',
	'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM_4J12',
	# 2018
	'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_03dR30',
	'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM_4J12.0ETA23',
	'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_tautsf_L1DR-TAU20ITAU12I-J25',
	'HLT_tau35_tight1_tracktwo_tau25_tight1_tracktwo_03dR30_L1DR-TAU20ITAU12I-J25',
	'HLT_tau35_tight1_tracktwo_tau25_tight1_tracktwo_L1DR-TAU20ITAU12I-J25',
	'HLT_tau35_medium1_tracktwoEF_tau25_medium1_tracktwoEF',
	'HLT_tau40_medium1_tracktwoEF_tau35_medium1_tracktwoEF',
	'HLT_tau35_medium1_tracktwoEF_tau25_medium1_tracktwoEF_L1TAU20IM_2TAU12IM_4J12.0ETA23',
	'HLT_tau35_medium1_tracktwoEF_tau25_medium1_tracktwoEF_L1DR-TAU20ITAU12I-J25',
	'HLT_tau35_medium1_tracktwoEF_tau25_medium1_tracktwoEF_03dR30_L1DR-TAU20ITAU12I-J25',
	'HLT_tau60_medium1_tracktwoEF_tau25_medium1_tracktwoEF_xe50',
	'HLT_tau35_medium1_tracktwoMVA_tau25_medium1_tracktwoMVA',
	'HLT_tau40_medium1_tracktwoMVA_tau35_medium1_tracktwoMVA',
	'HLT_tau35_medium1_tracktwoMVA_tau25_medium1_tracktwoMVA_L1TAU20IM_2TAU12IM_4J12.0ETA23',
	'HLT_tau35_medium1_tracktwoMVA_tau25_medium1_tracktwoMVA_L1DR-TAU20ITAU12I-J25',
	'HLT_tau35_medium1_tracktwoMVA_tau25_medium1_tracktwoMVA_03dR30_L1DR-TAU20ITAU12I-J25',
	'HLT_tau60_medium1_tracktwoMVA_tau25_medium1_tracktwoMVA_xe50',
	'HLT_tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA',
	'HLT_tau40_mediumRNN_tracktwoMVA_tau35_mediumRNN_tracktwoMVA',
	'HLT_tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_L1TAU20IM_2TAU12IM_4J12.0ETA23',
	'HLT_tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_L1DR-TAU20ITAU12I-J25',
	'HLT_tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_03dR30_L1DR-TAU20ITAU12I-J25',
	'HLT_tau60_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_xe50'
]

#all triggers
SUSY7AllTriggers = SUSY7LeptonTriggers + SUSY7PhotonTriggers + SUSY7XETriggers + SUSY7CombinedXETriggers + SUSY7BjetTriggers + SUSY7MultijetTriggers + SUSY7TauTriggers

#just those we need the navigation thinning for
SUSY7ThinTriggers = SUSY7SingleLeptonTriggers + SUSY7DiLeptonTriggers + SUSY7PhotonTriggers + SUSY7TauTriggers
