# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'MC_pp_v5'

rules={}

########################################

#######
# muon

L1_muon_5e33=[
    'L1_MU15',
    'L1_MU20',
    'L1_2MU10',
    'L1_3MU6'
    ]

HLT_muon_5e33=[
    # 1mu
    'HLT_mu20_iloose_L1MU15',
    'HLT_mu24_iloose_L1MU15',
    'HLT_mu24_imedium',
    'HLT_mu26_imedium',
    # 1mu high pT
    'HLT_mu50',
    'HLT_mu60_0eta105_msonly',
    # FS
    'HLT_mu18_mu8noL1',
    'HLT_mu18_2mu4noL1',
    'HLT_mu20_mu8noL1',
    'HLT_mu20_2mu4noL1',
    'HLT_mu22_mu8noL1',
    'HLT_mu22_2mu4noL1',
    'HLT_mu24_mu8noL1',
    'HLT_mu24_2mu4noL1',
    # 2mu
    'HLT_2mu10',
    'HLT_2mu14',
    # 3mu
    'HLT_3mu6',
    'HLT_3mu6_msonly',
    # jpsi T&P
#     'HLT_mu18_2mu0noL1_JpsimumuFS',   # not in physics menu
#     'HLT_mu18_2mu4_JpsimumuL2',   # not in physics menu
    # Z T&P
    'HLT_mu13_mu13_idperf_Zmumu',
    ]

#######
# B-phys

L1_bphys_5e33=[
    'L1_2MU4',
    'L1_3MU4'
    ]

HLT_bphys_5e33=[
    #
    # 2mu
    'HLT_2mu4_bBmumu',
#     'HLT_2mu4_bBmumuxv2',   # not in physics menu
#     'HLT_2mu4_bBmumux_BcmumuDsloose',   # not in physics menu
#     'HLT_2mu4_bBmumux_BpmumuKp',   # not in physics menu
    'HLT_2mu4_bJpsimumu',
    'HLT_2mu4_bUpsimumu',
#
    'HLT_mu6_mu4_bBmumu',
#     'HLT_mu6_mu4_bBmumuxv2',   # not in physics menu
#     'HLT_mu6_mu4_bBmumux_BcmumuDsloose',   # not in physics menu
#     'HLT_mu6_mu4_bBmumux_BpmumuKp',   # not in physics menu
    'HLT_mu6_mu4_bJpsimumu',
    'HLT_mu6_mu4_bUpsimumu',
#
    'HLT_2mu6_bBmumu',
    'HLT_2mu6_bBmumuxv2',
#     'HLT_2mu6_bBmumux_BcmumuDsloose',   # not in physics menu
    'HLT_2mu6_bBmumux_BpmumuKp',
    'HLT_2mu6_bJpsimumu',
    'HLT_2mu6_bUpsimumu',
#
    'HLT_2mu10_bBmumu',
#     'HLT_2mu10_bBmumuxv2',   # not in physics menu
#     'HLT_2mu10_bBmumux_BcmumuDsloose',   # not in physics menu
#     'HLT_2mu10_bBmumux_BpmumuKp',   # not in physics menu
    'HLT_2mu10_bJpsimumu',
    'HLT_2mu10_bUpsimumu',
    #
    # 3mu
    'HLT_3mu4_bDimu',
    'HLT_3mu4_bJpsi',
    'HLT_3mu4_bTau',
    'HLT_3mu4_bUpsi',
    #
    'HLT_3mu6_bDimu',
    'HLT_3mu6_bJpsi',
    'HLT_3mu6_bTau',
    'HLT_3mu6_bUpsi',
    #
    # Trkloose
####    'HLT_mu18_bJpsi_Trkloose',  ### Trkloose does not work for the moment
    ]

#######
# MET

L1_met_5e33=[
    'L1_XE50',
    ]

HLT_met_5e33=[
#
    'HLT_xe70',
    'HLT_xe70_wEFMu',
    'HLT_xe70_mht',
    'HLT_xe70_mht_wEFMu',
    'HLT_xe70_pueta',
    'HLT_xe70_pueta_wEFMu',
    'HLT_xe70_pufit',
    'HLT_xe70_pufit_wEFMu',
    'HLT_xe70_tc_em',
    'HLT_xe70_tc_em_wEFMu',
    'HLT_xe70_tc_lcw',
    'HLT_xe70_tc_lcw_wEFMu',
#
    'HLT_xe80_L1XE50',
    'HLT_xe80_wEFMu_L1XE50',
    'HLT_xe80_mht_L1XE50',
    'HLT_xe80_mht_wEFMu_L1XE50',
    'HLT_xe80_pueta_L1XE50',
    'HLT_xe80_pueta_wEFMu_L1XE50',
    'HLT_xe80_pufit_L1XE50',
    'HLT_xe80_pufit_wEFMu_L1XE50',
    'HLT_xe80_tc_em_L1XE50',
    'HLT_xe80_tc_em_wEFMu_L1XE50',
    'HLT_xe80_tc_lcw_L1XE50',
    'HLT_xe80_tc_lcw_wEFMu_L1XE50',
#
    'HLT_xe80',
    'HLT_xe80_wEFMu',
    'HLT_xe80_mht',
    'HLT_xe80_mht_wEFMu',
    'HLT_xe80_pueta',
    'HLT_xe80_pueta_wEFMu',
    'HLT_xe80_pufit',
    'HLT_xe80_pufit_wEFMu',
    'HLT_xe80_tc_em',
    'HLT_xe80_tc_em_wEFMu',
    'HLT_xe80_tc_lcw',
    'HLT_xe80_tc_lcw_wEFMu',
#
    'HLT_xe80_L1XE70',
    'HLT_xe80_wEFMu_L1XE70',
    'HLT_xe80_mht_L1XE70',
    'HLT_xe80_mht_wEFMu_L1XE70',
    'HLT_xe80_pueta_L1XE70',
    'HLT_xe80_pueta_wEFMu_L1XE70',
    'HLT_xe80_pufit_L1XE70',
    'HLT_xe80_pufit_wEFMu_L1XE70',
    'HLT_xe80_tc_em_L1XE70',
    'HLT_xe80_tc_em_wEFMu_L1XE70',
    'HLT_xe80_tc_lcw_L1XE70',
    'HLT_xe80_tc_lcw_wEFMu_L1XE70',
#
#     'HLT_xe100',   # not in physics menu
#     'HLT_xe100_wEFMu',   # not in physics menu
#     'HLT_xe100_mht',   # not in physics menu
#     'HLT_xe100_mht_wEFMu',   # not in physics menu
#     'HLT_xe100_pueta',   # not in physics menu
#     'HLT_xe100_pueta_wEFMu',   # not in physics menu
#     'HLT_xe100_pufit',   # not in physics menu
#     'HLT_xe100_pufit_wEFMu',   # not in physics menu
#     'HLT_xe100_tc_em',   # not in physics menu
#     'HLT_xe100_tc_em_wEFMu',   # not in physics menu
#     'HLT_xe100_tc_lcw',   # not in physics menu
#     'HLT_xe100_tc_lcw_wEFMu',   # not in physics menu
    ]

#######
# jet

L1_jet_5e33=[
    'L1_J75',
    'L1_3J40',
    'L1_4J15',
    'L1_5J15.0ETA25',
    'L1_6J15'
    ]

HLT_jet_5e33=[
    #
    # 1jet
    'HLT_j360',
    #
    'HLT_j380',
    'HLT_j380_jes',
    'HLT_j380_lcw',
    'HLT_j380_lcw_jes',
    'HLT_j380_lcw_nojcalib',
    'HLT_j380_nojcalib',
    #
    'HLT_j400',
    'HLT_j400_jes',
    'HLT_j400_lcw',
    'HLT_j400_lcw_jes',
    'HLT_j400_nojcalib',
    'HLT_j400_sub',
    # 
    'HLT_j420',
    'HLT_j420_jes',
    'HLT_j420_lcw',
    'HLT_j420_lcw_jes',
    'HLT_j420_lcw_nojcalib',
    'HLT_j420_nojcalib',
    'HLT_j440',
    'HLT_j460',
    #
    # reclustered jet
    'HLT_j360_a10r_L1J100',
    'HLT_j460_a10r_L1J100',
    #
    # Fat jet
    'HLT_j360_a10_sub_L1J100',
    'HLT_j360_a10_lcw_sub_L1J100',
#
    'HLT_j460_a10_sub_L1J100',
    'HLT_j460_a10_nojcalib_L1J100',
    'HLT_j460_a10_lcw_sub_L1J100',
    'HLT_j460_a10_lcw_nojcalib_L1J100',
    #
    # Forward jets
    'HLT_j260_320eta490',
    'HLT_j260_320eta490_jes',
    'HLT_j260_320eta490_lcw',
    'HLT_j260_320eta490_lcw_jes',
    'HLT_j260_320eta490_lcw_nojcalib',
    'HLT_j260_320eta490_nojcalib',
    #
    'HLT_j360_320eta490',
    'HLT_j360_320eta490_jes',
    'HLT_j360_320eta490_lcw',
    'HLT_j360_320eta490_lcw_jes',
    'HLT_j360_320eta490_lcw_nojcalib',
#     'HLT_j360_320eta490_nojcalib',   # not in physics menu
    #
    # 3jet
    'HLT_3j175',
    #
    # 4jet
    'HLT_4j85',
    'HLT_4j85_jes',
    'HLT_4j85_lcw',
    'HLT_4j85_lcw_jes',
    'HLT_4j85_lcw_nojcalib',
    'HLT_4j85_nojcalib',
    #
    'HLT_4j100',
    #
    # 5jet 
    'HLT_5j60',
    'HLT_5j70',
    'HLT_5j85',
    'HLT_5j85_jes',
    'HLT_5j85_lcw',
    'HLT_5j85_lcw_jes',
    'HLT_5j85_lcw_nojcalib',
    'HLT_5j85_nojcalib',
    #
    # 6jet
#     'HLT_6j45',   # not in physics menu
    'HLT_6j50_0eta240_L14J20',
    'HLT_6j50_0eta240_L15J150ETA25',
    'HLT_6j55_0eta240_L14J20',
    'HLT_6j55_0eta240_L15J150ETA25',
    #
    # 7jet
    'HLT_7j45',
    'HLT_7j45_L14J20',
    'HLT_7j45_0eta240_L14J20',
    'HLT_7j45_0eta240_L15J150ETA25',
    #
    # HT
    'HLT_ht850_L1J100',
    'HLT_ht1000_L1J100',
    ]

#######
# bjet

L1_bjet_5e33=[
    'L1_J75_3J20',
    'L1_3J25.0ETA23',
    ]

HLT_bjet_5e33=[
    #
    # 2b + 1j
#     'HLT_2j70_bmedium_j70',   # not in physics menu
#     'HLT_2j70_bmedium_split_j70',   # not in physics menu
#     'HLT_2j65_btight_j65',   # not in physics menu
#     'HLT_2j65_btight_split_j65',   # not in physics menu
    #
#     'HLT_2j75_bmedium_j75',   # not in physics menu
#     'HLT_2j75_bmedium_split_j75',   # not in physics menu
#     'HLT_2j70_btight_j70',   # not in physics menu
#     'HLT_2j70_btight_split_j70',   # not in physics menu
    #
    # 2b + 2j
    # 3J25.0ETA23 seeded
#     'HLT_2j45_bmedium_2j45_L13J25.0ETA23',   # not in physics menu   # not in physics menu
#     'HLT_2j45_bmedium_split_2j45_L13J25.0ETA23',   # not in physics menu   # not in physics menu
#     'HLT_2j35_btight_2j35_L13J25.0ETA23',   # not in physics menu   # not in physics menu
#     'HLT_2j35_btight_split_2j35_L13J25.0ETA23',   # not in physics menu   # not in physics menu
#     'HLT_2j55_bmedium_2j55_L13J25.ETA23',   # not in physics menu
#     'HLT_2j55_bmedium_split_2j55_L13J25.ETA23',   # not in physics menu
#     'HLT_2j45_btight_2j45_L13J25.ETA23',   # not in physics menu
#     'HLT_2j45_btight_split_2j45_L13J25.ETA23',   # not in physics menu
    # 4J15.0ETA25 seeded
#     'HLT_2j45_bmedium_2j45_L14J15.0ETA25',   # not in physics menu
#     'HLT_2j45_bmedium_split_2j45_L14J15.0ETA25',   # not in physics menu
#     'HLT_2j35_btight_2j35_L14J15.0ETA25',   # not in physics menu
#     'HLT_2j35_btight_split_2j35_L14J15.0ETA25',   # not in physics menu
    #
#     'HLT_2j55_bmedium_2j55',   # not in physics menu
#     'HLT_2j55_bmedium_split_2j55',   # not in physics menu
#     'HLT_2j45_btight_2j45',   # not in physics menu
#     'HLT_2j45_btight_split_2j45',   # not in physics menu
    #
    # 1b + 3j
    # 3J25.0ETA23 seeded
#     'HLT_j70_bmedium_3j70_L13J25.0ETA23',   # not in physics menu   # not in physics menu
#     'HLT_j70_bmedium_split_3j70_L13J25.0ETA23',   # not in physics menu   # not in physics menu
#     'HLT_j65_btight_3j65_L13J25.0ETA23',   # not in physics menu   # not in physics menu
#     'HLT_j65_btight_split_3j65_L13J25.0ETA23',   # not in physics menu   # not in physics menu
#     'HLT_j75_bmedium_3j75_L13J25.ETA23',   # not in physics menu
#     'HLT_j75_bmedium_split_3j75_L13J25.ETA23',   # not in physics menu
#     'HLT_j70_btight_3j70_L13J25.ETA23',   # not in physics menu
#     'HLT_j70_btight_split_3j70_L13J25.ETA23',   # not in physics menu
    # 4J15.0ETA25 seeded
#     'HLT_j70_bmedium_3j70_L14J15.0ETA25',   # not in physics menu
#     'HLT_j70_bmedium_split_3j70_L14J15.0ETA25',   # not in physics menu
#     'HLT_j65_btight_3j65_L14J15.0ETA25',   # not in physics menu
#     'HLT_j65_btight_split_3j65_L14J15.0ETA25',   # not in physics menu
    #
#     'HLT_j75_bmedium_3j75',   # not in physics menu
#     'HLT_j75_bmedium_split_3j75',   # not in physics menu
#     'HLT_j70_btight_3j70',   # not in physics menu
#     'HLT_j70_btight_split_3j70',   # not in physics menu
    #
    # 2b + 1j
#     'HLT_j100_2j55_bmedium',   # not in physics menu
#     'HLT_j100_2j55_bmedium_split',   # not in physics menu
    #
    # 2b
#     'HLT_j150_bmedium_j50_bmedium',   # not in physics menu
#     'HLT_j150_bmedium_split_j50_bmedium_split',   # not in physics menu
    #
#     'HLT_j175_bmedium_j60_bmedium',   # not in physics menu
#     'HLT_j175_bmedium_split_j60_bmedium_split',   # not in physics menu
    #
    # 1b
#     'HLT_j225_bloose',   # not in physics menu
#     'HLT_j225_bloose_split',   # not in physics menu
    #
#     'HLT_j300_bloose',   # not in physics menu
#     'HLT_j300_bloose_split',   # not in physics menu
]

#######
# tau

L1_tau_5e33=[
    'L1_TAU20_2TAU12_XE35',
    'L1_TAU20IM_2TAU12IM',
    ]

HLT_tau_5e33=[
    #
    # single tau
    'HLT_tau80_medium1_tracktwo_L1TAU60',
    'HLT_tau125_medium1_tracktwo',
#     'HLT_tau125_medium1_calo',   # not in physics menu
#     'HLT_tau125_medium1_track',   # not in physics menu
    'HLT_tau160_medium1_tracktwo',
    #
    # high-pT di-tau from single Tau
    'HLT_tau80_medium1_tracktwo_L1TAU60_tau50_medium1_tracktwo_L1TAU12',
    'HLT_tau125_medium1_tracktwo_tau50_medium1_tracktwo_L1TAU12',
    #
    # tau + xe
    'HLT_tau35_medium1_tracktwo_xe70_L1XE45',
    #
    # tau + tau
    'HLT_tau35_loose1_tracktwo_tau25_loose1_tracktwo',
    'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM',
    'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo',
    'HLT_tau35_tight1_tracktwo_tau25_tight1_tracktwo',
    #
    # di-tau + xe
    'HLT_tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_xe50',
    'HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_xe50',
    #
    # tau + e
    'HLT_e17_medium_tau25_medium1_tracktwo',
    'HLT_e17_lhmedium_tau25_medium1_tracktwo',
#     'HLT_e17_lhmedium_nod0_tau25_medium1_tracktwo',   # not in physics menu
    #
    'HLT_e17_medium_iloose_tau25_medium1_tracktwo',
    'HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo',
    'HLT_e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo',
    #
    # tau + e (high et, w/o jet)
    'HLT_e17_medium_tau80_medium1_tracktwo',
    'HLT_e17_lhmedium_tau80_medium1_tracktwo',
    'HLT_e17_lhmedium_nod0_tau80_medium1_tracktwo',
    #
    # tau + e + xe
    'HLT_e17_medium_iloose_tau25_medium1_tracktwo_xe50',
    'HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo_xe50',
#     'HLT_e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_xe50',   # not in physics menu
    #
    # tau + mu (high et, w/o jet)
    'HLT_mu14_tau25_medium1_tracktwo',
    'HLT_mu14_tau35_medium1_tracktwo',
    'HLT_mu14_iloose_tau25_medium1_tracktwo',
    'HLT_mu14_iloose_tau35_medium1_tracktwo',
    #
    # tau + mu + xe
    'HLT_mu14_tau25_medium1_tracktwo_xe50',
    'HLT_mu14_iloose_tau25_medium1_tracktwo_xe50',
    ]

#######
# egamma

L1_egamma_5e33=[
    'L1_EM20VH',
    'L1_2EM10VH',
    ]

HLT_egamma_5e33=[
    #
    # 1e
    'HLT_e24_medium_iloose_L1EM20VH',
    'HLT_e24_lhmedium_iloose_L1EM20VH',
    'HLT_e24_lhmedium_nod0_iloose_L1EM20VH',
#     'HLT_e24_lhmedium_iloose_HLTCalo_L1EM20VH',   # not in physics menu
    'HLT_e24_lhmedium_cutd0dphideta_iloose_L1EM20VH',
    #
    'HLT_e24_tight_iloose',
    'HLT_e24_lhtight_iloose',
    'HLT_e24_lhtight_nod0_iloose',
    'HLT_e24_lhtight_cutd0dphideta_iloose',
#     'HLT_e24_lhtight_iloose_HLTCalo',   # not in physics menu
    #
    'HLT_e26_tight_iloose',
    'HLT_e26_lhtight_iloose',
    'HLT_e26_lhtight_nod0_iloose',
    'HLT_e26_lhtight_cutd0dphideta_iloose',
#     'HLT_e26_lhtight_iloose_HLTCalo', # kn: ??? naming to be changed ???   # not in physics menu
    #
    # 1e, high ET
    'HLT_e60_medium',
    'HLT_e60_lhmedium',
    'HLT_e60_lhmedium_nod0',
    'HLT_e60_lhmedium_cutd0dphideta',
#     'HLT_e60_lhmedium_HLTCalo',   # not in physics menu
    #
    'HLT_e120_loose',
    'HLT_e120_lhloose',
    #
#     'HLT_e140_loose',   # not in physics menu
#     'HLT_e140_lhloose',   # not in physics menu
#     'HLT_e140_lhloose_nod0',   # not in physics menu
#     'HLT_e140_lhloose_HLTCalo',   # not in physics menu
    #
    # 2e
    'HLT_2e12_loose_L12EM10VH',
    'HLT_2e12_lhloose_L12EM10VH',
    'HLT_2e12_lhloose_cutd0dphideta_L12EM10VH',
#     'HLT_2e12_lhloose_HLTCalo_L12EM10VH',   # not in physics menu
    'HLT_2e12_lhloose_nodphires_L12EM10VH',
    'HLT_2e12_lhloose_nodeta_L12EM10VH',
    'HLT_2e12_lhloose_nod0_L12EM10VH',
    #
#     'HLT_2e15_loose_L12EM13VH',   # not in physics menu
#     'HLT_2e15_lhloose_L12EM13VH',   # not in physics menu
#     'HLT_2e15_lhloose_nod0_L12EM13VH',   # not in physics menu
#     'HLT_2e15_lhloose_cutd0dphideta_L12EM13VH',   # not in physics menu
#     'HLT_2e15_lhloose_HLTCalo_L12EM13VH',   # not in physics menu
    #
    'HLT_2e17_loose',
    'HLT_2e17_lhloose',
#     'HLT_2e17_lhloose_nod0',   # not in physics menu
#     'HLT_2e17_lhloose_cutd0dphideta',   # not in physics menu
#     'HLT_2e17_lhloose_HLTCalo',   # not in physics menu
    #
    # 3e
#     'HLT_e17_medium_2e9_medium',   # not in physics menu
#     'HLT_e17_lhmedium_2e9_lhmedium',   # not in physics menu
#     'HLT_e17_lhmedium_nod0_2e9_lhmedium_nod0',   # not in physics menu
    #
#     'HLT_e17_medium_iloose_2e9_medium',   # not in physics menu
#     'HLT_e17_lhmedium_iloose_2e9_lhmedium',   # not in physics menu
#     'HLT_e17_lhmedium_nod0_iloose_2e9_lhmedium_nod0',   # not in physics menu
    #
    # 1g
    'HLT_g120_loose',
#     'HLT_g120_loose_HLTCalo',   # not in physics menu
    'HLT_g140_loose',
    #
    # 2g
    'HLT_g35_medium_g25_medium',
#     'HLT_g35_medium_HLTCalo_g25_medium_HLTCalo',   # not in physics menu
    #
#     'HLT_2g50_loose',   # not in physics menu
#     'HLT_2g60_loose_L12EM15VH',   # not in physics menu
    #
#     'HLT_2g20_tight',   # not in physics menu
#     'HLT_2g25_tight',   # not in physics menu
    #
    # 3g
#     'HLT_3g15_loose',   # not in physics menu
#     'HLT_2g20_loose_g15_loose',   # not in physics menu
#     'HLT_3g20_loose',   # not in physics menu
    #
    # Z T&P
    'HLT_e24_lhtight_L1EM20VH_e15_etcut_Zee',
    'HLT_e24_tight_L1EM20VH_e15_etcut_Zee',
    ]

#######
# combined

HLT_combined_5e33=[
    #
    # e + 2g
#     'HLT_e20_medium_2g10_medium',   # not in physics menu
#     'HLT_e20_lhmedium_2g10_medium',   # not in physics menu
#     'HLT_e20_lhmedium_nod0_2g10_medium',   # not in physics menu
    # kn: loose is preferred, keep unPS for now
#     'HLT_e20_medium_2g10_loose',   # not in physics menu
#     'HLT_e20_lhmedium_2g10_loose',   # not in physics menu
#     'HLT_e20_lhmedium_nod0_2g10_loose',   # not in physics menu
    #
    # e + g
#     'HLT_e20_medium_g35_loose',   # not in physics menu
#     'HLT_e20_lhmedium_g35_loose',   # not in physics menu
#     'HLT_e20_lhmedium_nod0_g35_loose',   # not in physics menu
    #
#     'HLT_e24_medium_L1EM15VH_g25_medium',   # not in physics menu
#     'HLT_e24_lhmedium_L1EM15VH_g25_medium',   # not in physics menu
#     'HLT_e24_lhmedium_nod0_L1EM15VH_g25_medium',   # not in physics menu
    #
    # e + mu
#     'HLT_e17_loose_mu14',   # not in physics menu
#     'HLT_e17_lhloose_mu14',   # not in physics menu
#     'HLT_e17_lhloose_nod0_mu14',   # not in physics menu
    #
#     'HLT_e7_medium_mu24',   # not in physics menu
#     'HLT_e7_lhmedium_mu24',   # not in physics menu
#     'HLT_e7_lhmedium_nod0_mu24',   # not in physics menu
    #
#     'HLT_e24_medium_L1EM20VHI_mu8noL1',   # not in physics menu
#     'HLT_e24_lhmedium_L1EM20VHI_mu8noL1',   # not in physics menu
#     'HLT_e24_lhmedium_nod0_L1EM20VHI_mu8noL1',   # not in physics menu
#     'HLT_e26_medium_L1EM22VHI_mu8noL1',   # not in physics menu
#     'HLT_e26_lhmedium_L1EM22VHI_mu8noL1',   # not in physics menu
#     'HLT_e26_lhmedium_nod0_L1EM22VHI_mu8noL1',   # not in physics menu
    #
    # e + 2mu
#     'HLT_e12_loose_2mu10',   # not in physics menu
#     'HLT_e12_lhloose_2mu10',   # not in physics menu
#     'HLT_e12_lhloose_nod0_2mu10',   # not in physics menu
    # backup
#     'HLT_e12_medium_2mu10',   # not in physics menu
#     'HLT_e12_lhmedium_2mu10',   # not in physics menu
#     'HLT_e12_lhmedium_nod0_2mu10',   # not in physics menu
    #
    # 2e + mu
#     'HLT_2e12_loose_mu10',   # not in physics menu
#     'HLT_2e12_lhloose_mu10',   # not in physics menu
#     'HLT_2e12_lhloose_nod0_mu10',   # not in physics menu
    # backup
#     'HLT_2e12_medium_mu10',   # not in physics menu
#     'HLT_2e12_lhmedium_nod0_mu10',   # not in physics menu
#     'HLT_2e12_lhmedium_mu10',   # not in physics menu
    #
    # mu + g
#     'HLT_g25_medium_mu24',   # not in physics menu
    #
    # mu + 2g
#     'HLT_2g10_medium_mu20',   # not in physics menu
    # kn: loose is preferred, keep unPS for now
#     'HLT_2g10_loose_mu20',   # not in physics menu
    #
    # e + xe
#     'HLT_e60_loose_xe60noL1',   # not in physics menu
#     'HLT_e60_lhloose_xe60noL1',   # not in physics menu
#     'HLT_e60_lhloose_nod0_xe60noL1',   # not in physics menu
#     'HLT_e70_loose_xe70noL1',   # not in physics menu
#     'HLT_e70_lhloose_xe70noL1',   # not in physics menu
#     'HLT_e70_lhloose_nod0_xe70noL1',   # not in physics menu
    #
    # g + xe (loose)
#     'HLT_g60_loose_xe60noL1',   # not in physics menu
#     'HLT_g70_loose_xe70noL1',   # not in physics menu
    #
    # g + xe (tight)
#     'HLT_g45_tight_L1EM22VHI_xe45noL1',   # not in physics menu
    #
    # jet + xe
    'HLT_j80_xe80',
#     'HLT_j100_xe80',   # not in physics menu
    ]

#######
# VBF

HLT_vbf_5e33=[
    'HLT_2j55_bloose_L14J20.0ETA49',
    ]

#######
# dedicated

HLT_dedicated_5e33=[
    # razor
#     'HLT_j30_xe10_razor170',   # not in physics menu
    # LLP
#     'HLT_j30_muvtx',   # not in physics menu
#     'HLT_j30_muvtx_noiso',   # not in physics menu
    #
#     'HLT_g15_loose_2mu10_msonly',   # not in physics menu
#     'HLT_g15_loose_2mu10_msonly_L1MU4_EMPTY',   # not in physics menu
#     'HLT_g15_loose_2mu10_msonly_L1MU4_UNPAIRED_ISO',   # not in physics menu
    #
#     'HLT_3mu6_msonly_L1MU4_EMPTY',   # not in physics menu
#     'HLT_3mu6_msonly_L1MU4_UNPAIRED_ISO',   # not in physics menu
    #
#     'HLT_j30_jes_PS_llp_L1TAU60',   # not in physics menu
#     'HLT_j30_jes_PS_llp_noiso_L1TAU60',   # not in physics menu
    #
    # tau->3mu (narrow scan)
    'HLT_mu11_2mu4noL1_nscan03_L1MU10_2MU6',
    #
    # Lepton-Jet
    'HLT_mu20_msonly_mu6noL1_msonly_nscan05',
#     'HLT_mu20_msonly_mu6noL1_msonly_nscan05_L1MU4_EMPTY',   # not in physics menu
#     'HLT_mu20_msonly_mu6noL1_msonly_nscan05_L1MU4_UNPAIRED_ISO',   # not in physics menu
    #
    # HIP
#     'HLT_g0_hiptrt_L1EM20VH',   # not in physics menu
]

########################################

#L1_5e33  = L1_muon_5e33  + L1_bphys_5e33  + L1_met_5e33 +  L1_jet_5e33  + L1_bjet_5e33  + L1_tau_5e33  + L1_egamma_5e33
#HLT_5e33 = HLT_muon_5e33 + HLT_bphys_5e33 + HLT_met_5e33 + HLT_jet_5e33 + HLT_bjet_5e33 + HLT_tau_5e33 + HLT_egamma_5e33 + HLT_combined_5e33 + HLT_dedicated_5e33

L1_5e33  = L1_met_5e33 +  L1_jet_5e33  + L1_bjet_5e33  + L1_tau_5e33  + L1_egamma_5e33
HLT_5e33 = HLT_met_5e33 + HLT_jet_5e33 + HLT_bjet_5e33 + HLT_tau_5e33 + HLT_egamma_5e33 + HLT_combined_5e33 + HLT_dedicated_5e33

L1_5e33_Rules=dict(map(None,L1_5e33,len(L1_5e33)*[{
    1000 : TriggerRule(PS=1, comment='L1'),
    }]))

HLT_5e33_Rules=dict(map(None,HLT_5e33,len(HLT_5e33)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    }]))

## rules=dict(rules,**L1_5e33_Rules)
rules=dict(rules,**HLT_5e33_Rules)

########################################
