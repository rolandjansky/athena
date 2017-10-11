# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# jet triggers
jetTriggers = [
# central jets, nominal
'HLT_j0_perf_L1RD0_FILLED',
'HLT_j15',
'HLT_j25',
'HLT_j35',
'HLT_j45',
'HLT_j55',
'HLT_j60',
'HLT_j85',
'HLT_j100',
'HLT_j110',
'HLT_j150',
'HLT_j175',
'HLT_j200',
'HLT_j260',
'HLT_j300',
'HLT_j320',
'HLT_j340',
'HLT_j360', # lowest unprescaled, 2015
'HLT_j380', # lowest unprescaled, 2016
'HLT_j400',
'HLT_j420', # lowest unprescaled, 2017 (1.7e34 menu)
'HLT_j440',
'HLT_j450', # lowest unprescaled, 2017 (2.0e34 menu)
'HLT_j460',

# central jets, GSC (new 2017 nominal)
'HLT_j225_gsc400_boffperf_split',
'HLT_j225_gsc420_boffperf_split', # lowest unprescaled, 2017 (1.7e34 menu)
'HLT_j225_gsc440_boffperf_split', # lowest unprescaled, 2017 (2.0e34 menu)

# central jets, alternative calibration
'HLT_j35_jes',
'HLT_j35_lcw',
'HLT_j35_lcw_jes',
'HLT_j35_lcw_nojcalib',
'HLT_j35_nojcalib',
'HLT_j85_jes',
'HLT_j380_jes',
'HLT_j380_lcw',
'HLT_j380_lcw_jes',
'HLT_j380_lcw_nojcalib',
'HLT_j380_nojcalib',
'HLT_j400_jes',
'HLT_j400_lcw',
'HLT_j400_lcw_jes',
'HLT_j400_nojcalib',
'HLT_j400_sub',
'HLT_j420_jes',
'HLT_j420_lcw',
'HLT_j420_lcw_jes',
'HLT_j420_lcw_nojcalib',
'HLT_j420_nojcalib',

# forward jets, nominal
'HLT_j15_320eta490',
'HLT_j25_320eta490',
'HLT_j35_320eta490',
'HLT_j45_320eta490',
'HLT_j55_320eta490',
'HLT_j60_320eta490',
'HLT_j85_320eta490',
'HLT_j110_320eta490',
'HLT_j175_320eta490',
'HLT_j260_320eta490', # lowest unprescaled, 2015 - 2017 inclusive
'HLT_j360_320eta490',

# forward jets, alternative calibration
'HLT_j175_320eta490_jes',
'HLT_j175_320eta490_lcw',
'HLT_j175_320eta490_lcw_jes',
'HLT_j175_320eta490_lcw_nojcalib',
'HLT_j175_320eta490_nojcalib',
'HLT_j260_320eta490_jes',
'HLT_j260_320eta490_lcw',
'HLT_j260_320eta490_lcw_jes',
'HLT_j260_320eta490_lcw_nojcalib',
'HLT_j260_320eta490_nojcalib',
'HLT_j360_320eta490_jes',
'HLT_j360_320eta490_lcw',
'HLT_j360_320eta490_lcw_jes',
'HLT_j360_320eta490_lcw_nojcalib',
'HLT_j360_320eta490_nojcalib',




# large-R jets
'HLT_j260_a10_lcw_L1J75',
'HLT_j300_a10_lcw_L1J75',
'HLT_j360_a10_lcw_sub_L1J100', # lowest unprescaled, 2015
'HLT_j420_a10_lcw_L1J100', # lowest unprescaled, 2016
'HLT_j460_a10_lcw_subjes_L1J100', # lowest unprescaled, 2017 (1.7e34 menu)
'HLT_j480_a10_lcw_subjes_L1J100', # lowest unprescaled, 2017 (2.0e34 menu)

# large-R jets, alternative calibration
'HLT_j260_a10_lcw_nojcalib_L1J75',
'HLT_j260_a10_lcw_sub_L1J75',
'HLT_j260_a10_nojcalib_L1J75',
'HLT_j260_a10_sub_L1J75',
'HLT_j300_a10_sub_L1J75',
'HLT_j360_a10_lcw_L1J100',
'HLT_j400_a10_lcw_L1J100',
'HLT_j420_a10_lcw_sub_L1J100',
'HLT_j460_a10_lcw_nojcalib_L1J100',
'HLT_j460_a10_lcw_sub_L1J100',
'HLT_j460_a10_nojcalib_L1J100',
'HLT_j460_a10_sub_L1J100',

# large-R jets, trimmed (new 2017 nominal)
'HLT_j460_a10t_lcw_jes_L1J100', # lowest unprescaled, 2017 (1.7e34 menu)
'HLT_j480_a10t_lcw_jes_L1J100', # lowest unprescaled, 2017 (2.0e34 menu)

# large-R jets, trimmed with 1xmass cuts (substructure-enriched triggers)
'HLT_j390_a10t_lcw_jes_L1J100', # prescaled, for mass bootstrap
'HLT_j420_a10t_lcw_jes_L1J100', # prescaled, for mass bootstrap
'HLT_j440_a10t_lcw_jes_L1J100', # prescaled, for mass bootstrap
'HLT_j390_a10t_lcw_jes_40smcINF_L1J100',
'HLT_j420_a10t_lcw_jes_40smcINF_L1J100', # lowest unprescaled, 2017 (1.7e34 menu)
'HLT_j440_a10t_lcw_jes_40smcINF_L1J100', # lowest unprescaled, 2017 (2.0e34 menu)

# di-large-R jets, trimmed (substructure-enriched triggers)
'HLT_2j330_a10t_lcw_jes_L1J100', # prescaled, for mass bootstrap
'HLT_2j330_a10t_lcw_jes_40smcINF_L1J100', # lowest unprescaled, 2017 (2.0e34 menu)

# reclustered jets
'HLT_j260_a10r_L1J75',
'HLT_j300_a10r_L1J75',
'HLT_j360_a10r_L1J100', # lowest unprescaled, 2015 - TRIGGER HAD A BUG IN 2015
'HLT_j400_a10r_L1J100',
'HLT_j420_a10r_L1J100', # lowest unprescaled, 2016
'HLT_j460_a10r_L1J100', # lowest unprescaled, 2017 (1.7e34 menu)
'HLT_j480_a10r_L1J100', # lowest unprescaled, 2017 (2.0e34 menu)




# special support triggers
'HLT_j45_L1RD0_FILLED',
'HLT_j60_L1RD0_FILLED',
'HLT_j85_L1RD0_FILLED',
'HLT_j60_280eta320',
'HLT_j85_280eta320',
'HLT_j85_280eta320_jes',
'HLT_j85_280eta320_lcw',
'HLT_j85_280eta320_lcw_jes',
'HLT_j85_280eta320_lcw_nojcalib',
'HLT_j85_280eta320_nojcalib',
'HLT_j15_j15_320eta490',
'HLT_j25_j25_320eta490',
'HLT_j35_j35_320eta490',
'HLT_j45_j45_320eta490',
'HLT_j60_j60_320eta490',
'HLT_j85_j85_320eta490',

]

# single electron triggers

singleElTriggers = [
'HLT_e26_lhtight_smooth_iloose',
'HLT_e26_lhtight_smooth_ivarloose',
'HLT_e26_lhtight_iloose',
'HLT_e26_lhtight_ivarloose',
'HLT_e26_lhtight_nod0_iloose',
'HLT_e26_lhtight_nod0_ivarloose',
'HLT_e26_lhtight_cutd0dphideta_iloose',
'HLT_e28_lhtight_smooth_iloose',
'HLT_e28_lhtight_smooth_ivarloose',
'HLT_e28_lhtight_iloose',
'HLT_e28_lhtight_ivarloose',
'HLT_e28_lhtight_nod0_iloose',
'HLT_e28_lhtight_nod0_ivarloose',
'HLT_e28_lhtight_nod0_ringer_iloose',
'HLT_e28_lhtight_nod0_ringer_ivarloose',
'HLT_e60_lhmedium',
'HLT_e60_lhmedium_nod0',
'HLT_e60_lhmedium_L1EM24VHI ',
'HLT_e60_lhmedium_nod0_L1EM24VHI ',
'HLT_e60_lhmedium_cutd0dphideta',
'HLT_e140_lhloose',
'HLT_e140_lhloose_nod0 ',
'HLT_e140_lhloose_L1EM24VHI ',
'HLT_e140_lhloose_nod0_L1EM24VHI ',
'HLT_e300_etcut ',
'HLT_e26_lhtight_idperf',
'HLT_e24_lhtight_iloose',
'HLT_e24_lhtight_nod0_iloose',
'HLT_e24_lhtight_ivarloose',
'HLT_e24_lhtight_nod0_ivarloose',
'HLT_e24_lhtight_cutd0dphideta_iloose			',
'HLT_e17_lhloose_nod0_2e9_lhloose_nod0',
'HLT_e24_lhmedium_iloose',
'HLT_e24_lhmedium_ivarloose',
'HLT_e24_lhmedium_nod0_iloose',
'HLT_e24_lhmedium_nod0_ivarloose ',
'HLT_e24_lhmedium_iloose_L1EM20VH',
'HLT_e24_lhmedium_nod0_iloose_L1EM20VH',
'HLT_e24_lhmedium_ivarloose_L1EM20VH',
'HLT_e24_lhmedium_nod0_ivarloose_L1EM20VH',
'HLT_e24_lhmedium_L1EM20VH',
'HLT_e24_lhmedium_nod0_L1EM20VH',
'HLT_e24_medium_iloose_L1EM20VH ',
'HLT_e120_lhloose',
'HLT_e120_lhloose_nod0',
'HLT_e24_lhmedium_iloose_L1EM18VH',
'HLT_e24_lhmedium_nod0_iloose_L1EM18VH',
'HLT_e24_lhmedium_ivarloose_L1EM18VH',
'HLT_e24_lhmedium_nod0_ivarloose_L1EM18VH',
'HLT_e24_lhmedium_nod0_ringer_iloose',
'HLT_e24_lhmedium_nod0_ringer_ivarloose',
'HLT_e60_medium',
'HLT_e60_medium_L1EM24VHI',
]

# single muon triggers

singleMuTriggers = [
'HLT_mu26_imedium',
'HLT_mu26_ivarmedium',
'HLT_mu50HLT_mu60_0eta105_msonly',
'HLT_mu24_mu8noL1',
'HLT_mu24_2mu4noL1',
'HLT_mu22_2mu4noL1',
'HLT_mu20_2mu4noL1',
'HLT_mu22_mu8noL1',
'HLT_mu24_imedium',
'HLT_mu24_ivarmedium',
'HLT_mu20_mu8noL1',
'HLT_mu40',
'HLT_mu50',
'HLT_mu24_ivarloose',
'HLT_mu24_iloose',
'HLT_mu24_iloose_L1MU15',
'HLT_mu24_ivarloose_L1MU15',
'HLT_mu20_iloose_L1MU15',
'HLT_mu20_ivarloose_L1MU15',
]

# xe triggers

metTriggers = [
'HLT_xe80_tc_lcw_L1XE50',
'HLT_xe90_mht_L1XE50',
'HLT_xe90_mht_wEFMu_L1XE50',
'HLT_xe90_tc_lcw_L1XE50 ',
'HLT_xe90_tc_lcw_wEFMu_L1XE50 ',
'HLT_xe100',
'HLT_xe100_L1XE50 ',
'HLT_xe100_L1XE60',
'HLT_xe100_mht_L1XE50',
'HLT_xe100_mht_L1XE60',
'HLT_xe100_mht_wEFMu_L1XE50',
'HLT_xe100_tc_em_L1XE50',
'HLT_xe100_tc_em_wEFMu_L1XE50',
'HLT_xe100_tc_lcw_L1XE50 ',
'HLT_xe100_tc_lcw_L1XE60',
'HLT_xe100_tc_lcw_wEFMu_L1XE50 ',
'HLT_xe100_wEFMu_L1XE50 ',
'HLT_xe110_L1XE50 ',
'HLT_xe110_mht_L1XE50',
'HLT_xe110_pueta_L1XE50 ',
'HLT_xe110_pufit_L1XE50    ',
'HLT_xe110_tc_em_L1XE50',
'HLT_xe110_tc_em_wEFMu_L1XE50',
'HLT_xe110_wEFMu_L1XE50 ',
'HLT_xe120      ',
'HLT_xe120_mht  ',
'HLT_xe120_mht_wEFMu  ',
'HLT_xe120_pueta',
'HLT_xe120_pueta_L1XE50 ',
'HLT_xe120_pueta_wEFMu',
'HLT_xe120_pufit',
'HLT_xe120_pufit_L1XE50',
'HLT_xe120_pufit_wEFMu',
'HLT_xe120_tc_em',
'HLT_xe120_tc_em_wEFMu',
'HLT_xe120_tc_lcw',
'HLT_xe120_tc_lcw_wEFMu',
'HLT_xe120_wEFMu      ',
'HLT_xe130_mht_L1XE50',
]

#multi-muon triggers

multiMuTriggers = [
'HLT_2mu10',
'HLT_3mu4',
'HLT_2mu14',
'HLT_2mu14_nomucomb',
'HLT_3mu6',
'HLT_3mu6_msonly',
'HLT_mu18_mu8noL1',
'HLT_mu18_2mu4noL1',
]

# multi-el triggers

multiElTriggers = [
'HLT_e17_lhloose_2e9_lhloose',
'HLT_e17_lhmedium_2e9_lhmedium',
'HLT_e17_lhmedium_nod0_2e9_lhmedium_nod0',
'HLT_2e17_lhvloose',
'HLT_2e17_lhvloose_nod0',
'HLT_2e17_lhloose_cutd0dphideta',
'HLT_2e17_lhloose',
'HLT_2e15_lhvloose_L12EM13VH',
'HLT_2e15_lhvloose_nod0_L12EM13VH',
'HLT_2e12_lhvloose_L12EM10VH ',
'HLT_2e12_lhvloose_nod0_L12EM10VH',
]

# photon triggers
singlePhotonTriggers = [
# prescaled
'HLT_g10_loose',
'HLT_g15_loose',
'HLT_g15_loose_L1EM3',
'HLT_g15_loose_L1EM7',
'HLT_g20_loose',
'HLT_g20_loose_L1EM12',
'HLT_g20_loose_L1EM15',
'HLT_g25_loose',
'HLT_g25_loose_L1EM15',
'HLT_g35_loose',
'HLT_g35_loose_L1EM15',
'HLT_g40_loose_L1EM15',
'HLT_g45_loose_L1EM15',
'HLT_g50_loose',
'HLT_g50_loose_L1EM15',
'HLT_g60_loose',
'HLT_g60_loose_L1EM15VH',
'HLT_g70_loose',
'HLT_g80_loose',
'HLT_g100_loose',
'HLT_g120_loose',
# unprescaled
'HLT_g140_loose',
'HLT_g200_etcut',
'HLT_g250_etcut',
'HLT_g300_etcut',
]

# multi-photon triggers

multiPhotonTriggers = [
'HLT_2g60_loose_L12EM15VH',
'HLT_2g22_tight',
'HLT_3g20_loose',
'HLT_2g50_loose',
'HLT_2g20_tight',
'HLT_2g20_loose_g15_loose',
'HLT_3g15_loose',
'HLT_g35_medium_g25_medium',
'HLT_g35_loose_g25_loose',
'HLT_g35_loose_L1EM15_g25_loose_L1EM15',
]
