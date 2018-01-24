# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# add the common trigger list
from DerivationFrameworkSUSY.SUSYCommonTriggerList import *

SUSY7ElectronTriggers = [
	# 2016+17
	# 'HLT_e24_lhtight_nod0_ivarloose',
	'HLT_e24_lhmedium_nod0_L1EM20VH' # missing from Keisuke
	# 'HLT_e60_lhmedium_nod0',
	# 'HLT_e60_medium',
	# 'HLT_e140_lhloose_nod0',
	# 'HLT_e300_etcut',
	# 'HLT_e26_lhtight_nod0_ivarloose',
	# 'HLT_e26_lhtight_nod0_ivarloose_L1EM22VHIM',
	# 'HLT_e28_lhtight_nod0_ivarloose',
	# 'HLT_e28_lhtight_nod0_ivarloose_L1EM24VHIM',
	# 'HLT_e60_lhmedium_nod0_L1EM24VHI', 
	# 'HLT_e140_lhloose_nod0_L1EM24VHI',
	# # 2015
	# 'HLT_e120_lhloose',
	# 'HLT_e60_lhmedium',
	# 'HLT_e24_lhmedium_L1EM20VH'

	# Remove/Outdated
	# 'HLT_e26_lhtight_smooth_iloose',
	# 'HLT_e26_lhtight_smooth_ivarloose',
	# 'HLT_e26_lhtight_iloose',
	# 'HLT_e26_lhtight_ivarloose',
	# 'HLT_e26_lhtight_nod0_iloose',
	# 'HLT_e26_lhtight_cutd0dphideta_iloose',
	# 'HLT_e28_lhtight_smooth_iloose',
	# 'HLT_e28_lhtight_smooth_ivarloose',
	# 'HLT_e28_lhtight_iloose',
	# 'HLT_e28_lhtight_ivarloose',
	# 'HLT_e28_lhtight_nod0_iloose',
	# 'HLT_e28_lhtight_nod0_ringer_iloose',
	# 'HLT_e28_lhtight_nod0_ringer_ivarloose',
	# 'HLT_e60_lhmedium_L1EM24VHI', 
	# 'HLT_e140_lhloose',
	# 'HLT_e140_lhloose_L1EM24VHI', 
	# 'HLT_e60_lhmedium_cutd0dphideta',
	# 'HLT_e26_lhtight_idperf',
	# 'HLT_e24_lhtight_iloose',
	# 'HLT_e24_lhtight_nod0_iloose',
	# 'HLT_e24_lhtight_ivarloose',
	# 'HLT_e24_lhtight_cutd0dphideta_iloose',
	# 'HLT_e17_lhloose_nod0_2e9_lhloose_nod0',
	# 'HLT_e24_lhmedium_iloose',
	# 'HLT_e24_lhmedium_ivarloose',
	# 'HLT_e24_lhmedium_nod0_iloose',
	# 'HLT_e24_lhmedium_nod0_ivarloose', 
	# 'HLT_e24_lhmedium_iloose_L1EM20VH',
	# 'HLT_e24_lhmedium_nod0_iloose_L1EM20VH',
	# 'HLT_e24_lhmedium_ivarloose_L1EM20VH',
	# 'HLT_e24_lhmedium_nod0_ivarloose_L1EM20VH',
	# 'HLT_e24_lhmedium_nod0_L1EM20VH',
	# 'HLT_e120_lhloose_nod0',
	# 'HLT_e24_lhmedium_iloose_L1EM18VH',
	# 'HLT_e24_lhmedium_nod0_iloose_L1EM18VH',
	# 'HLT_e24_lhmedium_ivarloose_L1EM18VH',
	# 'HLT_e24_lhmedium_nod0_ivarloose_L1EM18VH',
	# 'HLT_e24_lhmedium_nod0_ringer_iloose',
	# 'HLT_e24_lhmedium_nod0_ringer_ivarloose'
]

SUSY7MuonTriggers = [
	# # 2016+17
	# 'HLT_mu24_iloose',
	# 'HLT_mu24_iloose_L1MU15',
	# 'HLT_mu24_ivarloose',
	# 'HLT_mu24_ivarloose_L1MU15',
	# 'HLT_mu50',
	# 'HLT_mu60',
	# 'HLT_mu24_ivarmedium',
	# 'HLT_mu24_imedium',
	# 'HLT_mu26_imedium',
	# 'HLT_mu26_ivarmedium',
	# # 2015
	# 'HLT_mu20_iloose_L1MU15',
	# 'HLT_mu40',
	# 'HLT_mu60_0eta105_msonly'

	# Remove/Outdated
	# 'HLT_mu20_ivarloose_L1MU15'
]

SUSY7SingleLeptonTriggers = SingleLepton_2017 + SingleLepton_2016 + SingleLepton_2015 + SUSY7ElectronTriggers + SUSY7MuonTriggers

SUSY7DiLeptonTriggers = DiLepton_2017 + DiLepton_2016 + DiLepton_2015 + [
	# # 2017
	# 'HLT_2e17_lhvloose_nod0_L12EM15VHI',
	# 'HLT_2e24_lhvloose_nod0',
	'HLT_2e22_lhvloose_nod0', # missing from Keisuke
	# 'HLT_mu22_mu8noL1_calotag_0eta010',
	# 'HLT_mu26_mu10noL1',
	# 'HLT_mu24_mu8noL1',
	# 'HLT_mu24_mu8noL1_calotag_0eta010',
	'HLT_2mu15_L1MU10' # missing from Keisuke
	# 'HLT_e26_lhmedium_nod0_mu8noL1',
	# # 2016
	# 'HLT_2e15_lhvloose_nod0_L12EM13VH',
	# 'HLT_2e17_lhvloose_nod0',
	# 'HLT_2mu10_nomucomb',
	# 'HLT_mu20_mu8noL1',
	# 'HLT_mu20_nomucomb_mu6noL1_nscan03',
	# 'HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb',
	# 'HLT_2mu14',
	# 'HLT_2mu14_nomucomb',
	# 'HLT_mu22_mu8noL1',
	# 'HLT_e17_lhloose_nod0_mu14',
	# 'HLT_e24_lhmedium_nod0_L1EM20VHI_mu8noL1',
	# 'HLT_e7_lhmedium_nod0_mu24',
	# 'HLT_e26_lhmedium_nod0_L1EM22VHI_mu8noL1',
	# # 2015
	# 'HLT_2e12_lhloose_L12EM10VH',
	# 'HLT_2mu10',
	# 'HLT_mu18_mu8noL1',
	# 'HLT_e17_lhloose_mu14',
	# 'HLT_e24_lhmedium_L1EM20VHI_mu8noL1',
	# 'HLT_e7_lhmedium_mu24'

	# Remove/Outdated
	# 'HLT_2e15_lhloose_L12EM13VH',
	# 'HLT_2e17_lhloose',
	# 'HLT_2e17_lhloose_cutd0dphideta',
	# 'HLT_2e17_lhvloose',
	# 'HLT_2e15_lhvloose_L12EM13VH',
	# 'HLT_2e12_lhvloose_L12EM10VH',
	# 'HLT_2e12_lhvloose_nod0_L12EM10VH',
	# 'HLT_e26_lhmedium_L1EM22VHI_mu8noL1'
]

# Remove (?)
# SUSY7TriLeptonTriggers = [
# 	'HLT_e17_lhloose_2e9_lhloose',
# 	'HLT_e17_lhmedium_2e9_lhmedium',
# 	'HLT_e17_lhmedium_nod0_2e9_lhmedium_nod0',
#         'HLT_e17_lhloose_nod0_2e10_lhloose_nod0_L1EM15VH_3EM8VH',
# 	'HLT_mu24_2mu4noL1',
# 	'HLT_mu22_2mu4noL1',
# 	'HLT_mu20_2mu4noL1',
# 	'HLT_mu18_2mu4noL1',
# 	'HLT_3mu4',
# 	'HLT_3mu6',
# 	'HLT_3mu6_msonly'
# ]

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

	# Remove/Outdated
	# 'HLT_mu6_2j40_0eta490_invm400',
	# 'HLT_e15_lhtight_ivarloose_3j20_L1EM13VH_3J20',
	# 'HLT_e15_lhtight_nod0_iloose_3j20_L1EM13VH_3J20',
	# 'HLT_e15_lhtight_nod0_ivarloose_3j20_L1EM13VH_3J20',
	# 'HLT_mu14_iloose_3j20_L1MU10_3J20',
]

SUSY7LeptonTriggers = SUSY7SingleLeptonTriggers + SUSY7DiLeptonTriggers + SUSY7LeptonJetTriggers

SUSY7PhotonTriggers = SinglePhoton_2017 + SinglePhoton_2016 + SinglePhoton_2015 +[
	# # 2015
 #    'HLT_g120_loose',
 #    'HLT_g200_etcut',
 #    # 2016
 #    'HLT_g140_loose',
 #    'HLT_g300_etcut',
 #    # 2017
 #    'HLT_g140_tight',
 #    'HLT_g200_loose',
 #    'HLT_g300_etcut_L1EM24VHI'

	# # Remove/Outdated
 #    # 'HLT_g20_loose',
 #    # 'HLT_g20_loose_L1EM12',
 #    # 'HLT_g20_loose_L1EM15',
 #    # 'HLT_g25_loose',
 #    # 'HLT_g25_loose_L1EM15',
 #    # 'HLT_g30_loose',
 #    # 'HLT_g30_loose_L1EM15',
 #    # 'HLT_g35_loose',
 #    # 'HLT_g35_loose_L1EM15',
 #    # 'HLT_g40_loose_L1EM15',
 #    # 'HLT_g45_loose_L1EM15',
 #    # 'HLT_g50_loose',
 #    # 'HLT_g50_loose_L1EM15',
 #    # 'HLT_g60_loose',
 #    # 'HLT_g60_loose_L1EM15VH',
 #    # 'HLT_g70_loose',
 #    # 'HLT_g80_loose',
 #    # 'HLT_g100_loose',
 #    # 'HLT_g140_loose_L1EM24VHI',
 #    # 'HLT_g120_loose',
 #    # 'HLT_g140_loose',
]

SUSY7XETriggers = MET_2017 + MET_2016 + MET_2015 + [
    # # 2017
    # 'HLT_xe90_pufit_L1XE50',
    # 'HLT_xe110_pufit_L1XE55',
    # 'HLT_xe100_pufit_L1XE55',
    # 'HLT_xe100_pufit_L1XE50',
    # 'HLT_xe110_pufit_L1XE50',  
    # 'HLT_xe120_pufit_L1XE50',
    # 'HLT_xe110_pufit_L1XE60',
    # 'HLT_xe120_pufit_L1XE60',
    'HLT_noalg_L1J400', # missing from Keisuke
	# # 2016
 #    'HLT_xe90_mht_L1XE50',
 #    'HLT_xe80_tc_lcw_L1XE50',
 #    'HLT_xe90_mht_wEFMu_L1XE50',
 #    'HLT_xe110_mht_L1XE50',
    'HLT_xe100_mht_L1XE50', # missing from Keisuke
	# # 2015
	# 'HLT_xe70_mht',
	'HLT_xe70_tc_lcw', # missing from Keisuke
	'HLT_xe70' # missing from Keisuke

    # Remove/Outdated
    # 'HLT_xe90_tc_lcw_L1XE50',
	# 'HLT_xe100_tc_lcw_L1XE50',
	# 'HLT_xe100_tc_lcw_L1XE60',
    # 'HLT_xe100_L1XE50',
    # 'HLT_xe100_L1XE60',
    # 'HLT_xe100_mht_L1XE60',
    # 'HLT_xe110_pueta_L1XE50', 
    # 'HLT_xe110_L1XE50', 
    # 'HLT_xe110_tc_em_L1XE50',
    # 'HLT_xe110_mht_L1XE50_AND_xe70_L1XE50',
    # 'HLT_xe110_mht_L1XE50_AND_xe70_L1XE55'
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

    # Remove/Outdated
    # 'HLT_j80_boffperf_split_L12J50_XE40',
    # 'HLT_j80_boffperf_split_xe70_L1J400ETA25_XE50',
    # 'HLT_j100_boffperf_split_L1XE60',
    # 'HLT_j80_bmv2c2060_split_L12J50_XE40',
    # 'HLT_j80_bmv2c2077_split_xe70_L1J400ETA25_XE50',
    # 'HLT_j100_bmv2c2070_split_L1XE60',
    # 'HLT_j100_xe110_mht_1dphi10_L1J40_DPHI-J20s2XE50'   
]

#do we need multiphoton triggers?
#    HLT_g35_loose_g25_loose
#    HLT_g35_loose_L1EM15_g25_loose_L1EM15
#    HLT_g20_loose_2g15_loose_L12EM13VH

# 1 b-jet, 2 b-jets, b+VHF (maybe we remove 1 b-jet?)
SUSY7BjetTriggers = [
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
	
	# Remove/Outdated
	# 'HLT_2j55_boffperf_split_ht300_L14J20',
	# 'HLT_2j45_boffperf_split_2j45',
	# 'HLT_j75_boffperf_split_3j75',
	# 'HLT_j225_bmv2c2085_split',
	# 'HLT_j80_boffperf_split_L12J50_XE40',
	# 'HLT_2j35_bmv2c2070_split_2j35_L14J15.0ETA25',
	# 'HLT_j100_2j55_boffperf_split',
	# 'HLT_2j55_bmv2c2050_split_ht300_L14J15',
	# 'HLT_2j45_bmv2c2070_split_3j45_L14J15.0ETA25',
	# 'HLT_j75_bmv2c2060_split_3j75_L14J15.0ETA25',
	# 'HLT_j225_bmv2c2050_split',
	# 'HLT_j80_bmv2c2060_split_xe60_L12J50_XE40',
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
	'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM_4J12'
]

#all triggers
# SUSY7AllTriggers = SUSY7LeptonTriggers + SUSY7PhotonTriggers + SUSY7XETriggers + SUSY7CombinedXETriggers + SUSY7BjetTriggers + SUSY7MultijetTriggers + SUSY7TauTriggers
SUSY7AllTriggers = SUSY7TauTriggers

#just those we need the navigation thinning for
SUSY7ThinTriggers = SUSY7SingleLeptonTriggers + SUSY7DiLeptonTriggers + SUSY7PhotonTriggers
