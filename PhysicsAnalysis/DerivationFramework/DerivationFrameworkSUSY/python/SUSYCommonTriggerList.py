# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################################################################ 
#
# the trigger list below is created as of Oct23, 2017 based on Twiki pages:
# https://twiki.cern.ch/twiki/bin/viewauth/Atlas/LowestUnprescaled
# https://twiki.cern.ch/twiki/bin/view/Atlas/L34TriggerMenu2017
# 2018 menu is based on the trigger API: https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerAPI
#
################################################################ 

# 2018 menu (covering 1.5e34 to 2.3e34 menus)
BJetMET_2018 = [
    'HLT_j85_gsc100_bmv2c1050_split_xe85_pufit_xe50_L1XE55',
    'HLT_2j35_gsc45_bmv2c1070_split_xe85_pufit_xe50_L12J15_XE55',
    'HLT_3j35_bmv2c1077_split_xe70_pufit_xe50_L13J15.0ETA25_XE40',
]

MET_2018 = [
    'HLT_xe100_pufit_xe75_L1XE60',
    'HLT_xe110_pufit_L1XE55',
    'HLT_xe110_pufit_L1XE60',
    'HLT_xe110_pufit_L1XE70',
    'HLT_xe110_pufit_wEFMu_L1XE55',
    'HLT_xe110_pufit_xe65_L1XE50',
    'HLT_xe110_pufit_xe65_L1XE55',
    'HLT_xe110_pufit_xe65_L1XE60',
    'HLT_xe110_pufit_xe70_L1XE50',
    'HLT_xe120_mht_xe80_L1XE55',
    'HLT_xe120_mht_xe80_L1XE60',
    'HLT_xe120_pufit_L1XE50',
    'HLT_xe120_pufit_L1XE55',
    'HLT_xe120_pufit_L1XE60',
    'HLT_xe120_pufit_L1XE70',
    'HLT_xe120_pufit_wEFMu_L1XE55',
    'HLT_xe120_pufit_wEFMu_L1XE60',
]


SingleLepton_2018 = [
    'HLT_e140_lhloose_nod0',
    'HLT_e140_lhloose_nod0_L1EM24VHI',
    'HLT_e140_lhloose_nod0_L1EM24VHIM',
    'HLT_e26_lhtight_nod0_ivarloose',
    'HLT_e26_lhtight_nod0',
    'HLT_e28_lhtight_nod0_ivarloose',
    'HLT_e28_lhtight_nod0_ivarloose_L1EM24VHIM',
    'HLT_e28_lhtight_nod0_noringer_ivarloose',
    'HLT_e28_lhtight_nod0_noringer_ivarloose_L1EM24VHIM',
    'HLT_e300_etcut',
    'HLT_e300_etcut_L1EM24VHIM',
    'HLT_e32_lhtight_nod0_ivarloose',
    'HLT_e60_lhmedium_nod0',
    'HLT_e60_lhmedium_nod0_L1EM24VHI',
    'HLT_e60_lhmedium_nod0_L1EM24VHIM',
    'HLT_e80_lhmedium_nod0_L1EM24VHI',
    'HLT_e80_lhmedium_nod0_L1EM24VHIM',
    'HLT_mu26_ivarmedium',
    'HLT_mu28_ivarmedium',
    'HLT_mu50',
    'HLT_mu60',
    'HLT_mu60_0eta105_msonly',
    'HLT_mu60_msonly_3layersEC',
    'HLT_mu80',
    'HLT_mu80_msonly_3layersEC',
]


MultiBJet_2018 = [
    'HLT_2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15.0ETA25',
    'HLT_2j35_bmv2c1060_split_2j35_L14J15.0ETA25',
    'HLT_2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15.0ETA25',
    'HLT_j150_gsc175_bmv2c1060_split_j45_gsc60_bmv2c1060_split',
    'HLT_j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30',
    'HLT_2j35_bmv2c1060_split_3j35',
    'HLT_2j35_gsc45_bmv2c1060_split_3j35_gsc45_boffperf_split',
    'HLT_3j50_gsc65_bmv2c1077_split_L13J35.0ETA23',
    'HLT_3j35_bmv2c1070_split_j35_L14J15.0ETA25',
    'HLT_4j35_bmv2c1077_split_L14J15.0ETA25',
    'HLT_2j35_bmv2c1070_split_2j35_bmv2c1085_split_L14J15.0ETA25',
    'HLT_j175_gsc225_bmv2c1040_split',
    'HLT_j225_gsc275_bmv2c1060_split',
    'HLT_j225_gsc300_bmv2c1070_split',
    'HLT_j225_gsc360_bmv2c1077_split',
    'HLT_j55_gsc75_bmv2c1040_split_3j55_gsc75_boffperf_split',
    'HLT_j60_gsc85_bmv2c1050_split_3j60_gsc85_boffperf_split',
]


SinglePhoton_2018 = [
    'HLT_g140_loose',
    'HLT_g140_tight',
    'HLT_g140_tight_L1EM24VHIM',
    'HLT_g200_loose',
    'HLT_g200_loose_L1EM24VHIM',
    'HLT_g300_etcut_L1EM24VHI',
    'HLT_g300_etcut_L1EM24VHIM',
    'HLT_g300_etcut',
]


DiLepton_2018 = [
    'HLT_e28_lhmedium_nod0_mu8noL1',
    'HLT_e28_lhmedium_nod0_L1EM24VHIM_mu8noL1',
    'HLT_e17_lhloose_nod0_mu14',
    'HLT_e7_lhmedium_nod0_mu24',
    'HLT_2e17_lhvloose_nod0_L12EM15VHI',
    'HLT_2e19_lhvloose_nod0',
    'HLT_2e24_lhvloose_nod0',
    'HLT_e26_lhmedium_nod0_mu8noL1',
    'HLT_e28_lhtight_nod0_L1EM24VHIM_e15_etcut_L1EM7_Zee',
    'HLT_e28_lhtight_nod0_e15_etcut_L1EM7_Zee',
    'HLT_2mu14',
    'HLT_2mu15_L12MU10',
    'HLT_mu22_mu8noL1',
    'HLT_mu20_ivarmedium_mu8noL1',   
    'HLT_mu24_mu12noL1',
    'HLT_mu24_mu10noL1',
    'HLT_mu24_mu8noL1',
    'HLT_mu26_mu10noL1',
    'HLT_mu26_mu8noL1',
    'HLT_mu28_mu8noL1',
]

# 2017 menu (covering 1.5e34 to 2.0e34 menus)
BJetMET_2017 = [
    'HLT_j60_gsc100_bmv2c1050_split_xe80_mht_L1XE60',
    'HLT_2j25_gsc45_bmv2c1070_split_xe80_mht_L12J15_XE55',
    'HLT_3j15_gsc35_bmv2c1077_split_xe60_mht_L13J15.0ETA25_XE40',
]

MET_2017 = [
    'HLT_xe90_pufit_L1XE50',
    'HLT_xe100_pufit_L1XE50',
    'HLT_xe100_pufit_L1XE55',
    'HLT_xe110_pufit_L1XE50',
    'HLT_xe110_pufit_L1XE55',
    'HLT_xe110_pufit_L1XE60',
    'HLT_xe120_pufit_L1XE50',
    'HLT_xe120_pufit_L1XE55',
    'HLT_xe120_pufit_L1XE60',
]

SingleLepton_2017 = [
    'HLT_e26_lhtight_nod0_ivarloose',
    'HLT_e26_lhtight_nod0_ivarloose_L1EM22VHIM',
    'HLT_e28_lhtight_nod0_ivarloose',
    'HLT_e28_lhtight_nod0_ivarloose_L1EM24VHIM',
    'HLT_e60_lhmedium_nod0',
    'HLT_e60_lhmedium_nod0_L1EM24VHI',
    'HLT_e140_lhloose_nod0',
    'HLT_e140_lhloose_nod0_L1EM24VHI',
    'HLT_e300_etcut',
    'HLT_e300_etcut_L1EM24VHI',
    'HLT_mu26_ivarmedium',
    'HLT_mu50',
    'HLT_mu60',
    'HLT_mu60_0eta105_msonly',
]

DiLepton_2017 = [
    'HLT_2e17_lhvloose_nod0_L12EM15VHI',
    'HLT_2e24_lhvloose_nod0',
    'HLT_e17_lhloose_nod0_mu14',
    'HLT_e26_lhmedium_nod0_mu8noL1',
    'HLT_e7_lhmedium_nod0_mu24',
    'HLT_2mu14',
    'HLT_mu22_mu8noL1',
    'HLT_mu22_mu8noL1_calotag_0eta010',
    'HLT_mu24_mu8noL1',
    'HLT_mu24_mu8noL1_calotag_0eta010',
    'HLT_mu26_mu10noL1',
]

SinglePhoton_2017 = [
    'HLT_g140_loose',
    'HLT_g140_tight',
    'HLT_g200_loose',
    'HLT_g300_etcut',
    'HLT_g300_etcut_L1EM24VHI',
]

MultiBJet_2017 = [
    'HLT_2j25_gsc45_bmv2c1050_split_2j25_gsc45_boffperf_split_L14J15.0ETA25',
    'HLT_2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J15.0ETA25',
    'HLT_2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L14J15.0ETA25',
    'HLT_j150_gsc175_bmv2c1070_split_j45_gsc60_bmv2c1070_split ',
    'HLT_j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1077_split_L1J85_3J30',
    'HLT_2j15_gsc35_bmv2c1060_split_3j15_gsc35_boffperf_split',
    'HLT_2j25_gsc45_bmv2c1070_split_3j25_gsc45_boffperf_split',
    'HLT_3j15_gsc35_bmv2c1070_split_j15_gsc35_boffperf_split_L14J15.0ETA25',
    'HLT_4j15_gsc35_bmv2c1077_split_L14J15.0ETA25',
    'HLT_2j15_gsc35_bmv2c1070_split_2j15_gsc35_bmv2c1085_split_L14J15.0ETA25',
    'HLT_3j50_gsc65_bmv2c1077_split_L13J35.0ETA23',
]


# 2016 menu
MET_2016 = [
    'HLT_xe80_tc_lcw_L1XE50',
    'HLT_xe90_mht_L1XE50',
    'HLT_xe90_tc_lcw_wEFMu_L1XE50',
    'HLT_xe90_mht_wEFMu_L1XE50',
    'HLT_xe100_L1XE50',
    'HLT_xe100_tc_em_L1XE50',
    'HLT_xe110_pueta_L1XE50',
    'HLT_xe110_mht_L1XE50',
    'HLT_xe120_tc_lcw_L1XE50',
    'HLT_xe120_pueta',
    'HLT_xe120_pufit',
]

SingleLepton_2016 = [
    'HLT_e24_lhtight_nod0_ivarloose',
    'HLT_e26_lhtight_nod0_ivarloose',
    'HLT_e60_lhmedium_nod0',
    'HLT_e60_medium',
    'HLT_e140_lhloose_nod0',
    'HLT_e300_etcut',
    'HLT_mu24_iloose',
    'HLT_mu24_iloose_L1MU15',
    'HLT_mu24_ivarloose',
    'HLT_mu24_ivarloose_L1MU15',
    'HLT_mu24_ivarmedium',
    'HLT_mu24_imedium',
    'HLT_mu26_ivarmedium',
    'HLT_mu26_imedium',
    'HLT_mu40',
    'HLT_mu50',
]

DiLepton_2016 = [
    'HLT_2e15_lhvloose_nod0_L12EM13VH',
    'HLT_2e17_lhvloose_nod0',
    'HLT_e17_lhloose_nod0_mu14',
    'HLT_e7_lhmedium_nod0_mu24',
    'HLT_e24_lhmedium_nod0_L1EM20VHI_mu8noL1',
    'HLT_e26_lhmedium_nod0_L1EM22VHI_mu8noL1',
    'HLT_2mu10',
    'HLT_2mu10_nomucomb',
    'HLT_2mu14',
    'HLT_2mu14_nomucomb',
    'HLT_mu20_mu8noL1',
    'HLT_mu20_nomucomb_mu6noL1_nscan03',
    'HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb',
    'HLT_mu22_mu8noL1',
]

SinglePhoton_2016 = [
    'HLT_g140_loose',
    'HLT_g300_etcut',
]

JetTrigger_2016 = [
    "HLT_noalg_L1J400"
]


MultiBJet_2016 = [
    'HLT_2j35_bmv2c2050_split_2j35_L14J15.0ETA25',
    'HLT_2j35_bmv2c2060_split_2j35_L14J15.0ETA25',
    'HLT_2j45_bmv2c2077_split_3j45_L14J15.0ETA25',
    'HLT_2j55_bmv2c2060_split_ht300_L14J15',
    'HLT_2j70_bmv2c2060_split_j70',
    'HLT_2j75_bmv2c2070_split_j75',
    'HLT_j100_2j55_bmv2c2060_split',
    'HLT_j150_bmv2c2060_split_j50_bmv2c2060_split',
    'HLT_j175_bmv2c2060_split_j50_bmv2c2050_split',
]

# 2015 menu
MET_2015 = [
    'HLT_xe70_mht',
]

SingleLepton_2015 = [
    'HLT_e24_lhmedium_L1EM20VH',
    'HLT_e60_lhmedium',
    'HLT_e120_lhloose',
    'HLT_mu20_iloose_L1MU15',
    'HLT_mu40',
    'HLT_mu60_0eta105_msonly',
]

DiLepton_2015 = [
    'HLT_2e12_lhloose_L12EM10VH',
    'HLT_e17_lhloose_mu14',
    'HLT_e24_lhmedium_L1EM20VHI_mu8noL1',
    'HLT_e7_lhmedium_mu24',
    'HLT_2mu10',
    'HLT_mu18_mu8noL1',
]

SinglePhoton_2015 = [
    'HLT_g120_loose',
    'HLT_g200_etcut',
]

MultiBJet_2015 = [
    'HLT_2j35_btight_2j35_L13J25.0ETA23',
    'HLT_2j45_bmedium_2j45_L13J25.0ETA23',
    'HLT_j100_2j55_bmedium',
    'HLT_j150_bmedium_j50_bmedium',
    'HLT_2j65_btight_j65',
    'HLT_2j70_bmedium_j70',
]
