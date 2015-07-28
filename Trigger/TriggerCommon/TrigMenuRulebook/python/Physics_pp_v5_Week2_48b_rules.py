# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag='Physics_pp_v5'
doEB=False
doafterGlow=True
ScaleMoreBW=True

rules={}

########################################

#######
# muon

L1_muon=[
    'L1_MU10', 
    #
    'L1_MU15',
    'L1_MU20',
    'L1_2MU10',
    'L1_3MU6',
    ]

L1_muon_downstream=[
    # all included
    ]

HLT_muon=[
    # Week-1 specials
    'HLT_mu14',
    'HLT_2mu4',
    #
    # 1mu
    'HLT_mu18',
    'HLT_mu20',
    'HLT_mu20_L1MU15',
    'HLT_mu20_iloose_L1MU15',
    'HLT_mu20_msonly',
    'HLT_mu20_r1extr',
    'HLT_mu22',
    'HLT_mu24',
    'HLT_mu24_iloose_L1MU15',
    'HLT_mu24_imedium',
    'HLT_mu26',
    'HLT_mu24_L1MU15',
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
    'HLT_2mu6',
    'HLT_2mu10',
    'HLT_2mu14',
    # 3mu
    'HLT_3mu6',
    'HLT_3mu6_msonly',
    # Z T&P
    'HLT_mu13_mu13_idperf_Zmumu',
    ]

#######
# B-phys

L1_bphys=[
    'L1_2MU4',
    'L1_3MU4'
    ]

HLT_bphys=[
    #
    # 2mu
    'HLT_2mu4_bBmumu',
    'HLT_2mu4_bJpsimumu',
    'HLT_2mu4_bUpsimumu',
    #
    'HLT_2mu4_bDimu',
    'HLT_2mu4_bDimu_novtx_noos',
    'HLT_mu4_iloose_mu4_11invm60_noos',
    'HLT_mu4_iloose_mu4_7invm9_noos',
    'HLT_mu6_iloose_mu6_11invm60_noos',
    'HLT_mu6_iloose_mu6_11invm24_noos',
    'HLT_mu6_iloose_mu6_24invm60_noos',
    #
    'HLT_mu6_mu4_bBmumu',
    'HLT_mu6_mu4_bJpsimumu',
    'HLT_mu6_mu4_bUpsimumu',
    #
    'HLT_2mu6_bBmumu',
    'HLT_2mu6_bBmumuxv2',
    'HLT_2mu6_bBmumux_BpmumuKp',
    'HLT_2mu6_bJpsimumu',
    'HLT_2mu6_bUpsimumu',
    #
    'HLT_2mu10_bBmumu',
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

L1_met=[
    'L1_XE50',
    'L1_XE60',
    ]

HLT_met=[
    #
    'HLT_xe35_tc_lcw',
    'HLT_xe35_tc_em',
    'HLT_xe35_pueta',
    'HLT_xe35_pufit',
    'HLT_xe35_mht',
    'HLT_xe35_L2FS',
    'HLT_xe35_wEFMu',
    'HLT_xe35_tc_lcw_wEFMu',
    'HLT_xe35_tc_em_wEFMu',
    'HLT_xe35_mht_wEFMu',
    'HLT_xe35_pueta_wEFMu',
    'HLT_xe35_pufit_wEFMu',
    #
    'HLT_xe50',
    'HLT_xe50_mht',
    'HLT_xe50_pueta',
    'HLT_xe50_pufit',
    'HLT_xe50_tc_em',
    'HLT_xe50_tc_lcw',
    #
    'HLT_xe60',
    'HLT_xe60_tc_lcw',
    'HLT_xe60_tc_em',
    'HLT_xe60_mht',
    'HLT_xe60_pueta',
    'HLT_xe60_pufit',
    'HLT_xe60_wEFMu',
    'HLT_xe60_tc_lcw_wEFMu',
    'HLT_xe60_tc_em_wEFMu',
    'HLT_xe60_mht_wEFMu',
    'HLT_xe60_pueta_wEFMu',
    'HLT_xe60_pufit_wEFMu',
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
    ]

#######
# jet

L1_jet=[
    'L1_J75',
    'L1_J85',
    'L1_J100',
    'L1_3J40',
    'L1_3J50',
    'L1_4J15',
    'L1_4J20',
    'L1_5J15.0ETA25',
    'L1_6J15',
    'L1_J50.32ETA49',
    ]

HLT_jet=[
    #
    # 1jet
    'HLT_j260',
    'HLT_j300',
    'HLT_j300_lcw_nojcalib',
    'HLT_j320',
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
    'HLT_j175_320eta490',
    'HLT_j175_320eta490_jes',
    'HLT_j175_320eta490_lcw',
    'HLT_j175_320eta490_lcw_jes',
    'HLT_j175_320eta490_lcw_nojcalib',
    'HLT_j175_320eta490_nojcalib',
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
    'HLT_6j45_0eta240',
    'HLT_6j45_0eta240_L14J20',
    'HLT_6j45_0eta240_L15J150ETA25',
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
    'HLT_ht700_L1J100',
    'HLT_ht850_L1J100',
    'HLT_ht1000_L1J100',
    ]

#######
# bjet

L1_bjet=[
    ]

HLT_bjet=[
]

#######
# tau
L1_tau=[
    #'L1_TAU30',
    'L1_TAU60',
    #
    'L1_TAU20_2TAU12',
    'L1_TAU20IM_2TAU12IM',
    'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',
    #
    'L1_MU10_TAU12IM_J25_2J12',
    #
    'L1_MU10_TAU20',
    'L1_MU10_TAU20IM',
    #
    'L1_MU10_TAU12IM_XE35',
    #
    'L1_EM15HI_2TAU12IM_J25_3J12',
    #
    'L1_EM15HI_TAU40_2TAU15',
    #
    'L1_EM15HI_2TAU12IM_XE35',
    #
    'L1_TAU20_2J20_XE45',
    'L1_TAU20IM_2J20_XE45',
    #
    'L1_TAU20_2TAU12_XE35',
    'L1_TAU20IM_2TAU12IM_XE35',
    ]

L1_tau_downstream=[
    'L1_TAU40',
]

HLT_tau=[
    #
    # single tau
    'HLT_tau80_medium1_tracktwo',
    'HLT_tau80_medium1_tracktwo_L1TAU60',
    'HLT_tau125_medium1_tracktwo',
    'HLT_tau160_medium1_tracktwo',
    #'HLT_tau35_medium1_tracktwo_L1TAU20',
    #'HLT_tau25_medium1_tracktwo_L1TAU12',
    #
    # high-pT di-tau from single Tau
    'HLT_tau80_medium1_tracktwo_L1TAU60_tau50_medium1_tracktwo_L1TAU12',
    'HLT_tau125_medium1_tracktwo_tau50_medium1_tracktwo_L1TAU12',
    #
    # tau + xe
    'HLT_tau35_medium1_tracktwo_L1TAU20_xe70_L1XE45',
    'HLT_tau35_medium1_tracktwo_xe70_L1XE45',
    #
    # tau + tau
    'HLT_tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12',
    'HLT_tau35_loose1_tracktwo_tau25_loose1_tracktwo',
    'HLT_tau35_loose1_tracktwo_tau25_loose1_tracktwo_L1TAU20IM_2TAU12IM',
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
    'HLT_e17_medium_tau25_medium1_tracktwo_xe50',
    'HLT_e17_lhmedium_tau25_medium1_tracktwo_xe50',
    #
    # tau + mu (high et, w/o jet)
    'HLT_mu14_tau35_medium1_tracktwo_L1TAU20',
    'HLT_mu14_tau25_medium1_tracktwo',
    'HLT_mu14_tau35_medium1_tracktwo',
    'HLT_mu14_iloose_tau25_medium1_tracktwo',
    'HLT_mu14_iloose_tau35_medium1_tracktwo',
    #
    # tau + mu + xe
    'HLT_mu14_tau25_medium1_tracktwo_xe50',
    'HLT_mu14_iloose_tau25_medium1_tracktwo_xe50',
    #
    # Z T&P
    'HLT_mu14_iloose_tau25_perf_tracktwo',
    ]

#######
# egamma

L1_egamma=[
    'L1_EM15',
    'L1_EM18VH',
    'L1_EM20VH',
    'L1_2EM7',
    'L1_2EM10VH',
    ]

L1_egamma_downstream=[
    'L1_EM15VH',
    'L1_EM15VH_3EM7',
    'L1_EM20VHI',
    'L1_EM22VHI',
    'L1_2EM15',
    'L1_2EM13VH',
    'L1_2EM15VH',
]

HLT_egamma=[
    #
    # Week-1 specials
    'HLT_e17_loose_L1EM15',
    'HLT_e17_lhloose_L1EM15',
    'HLT_e17_lhloose_cutd0dphideta_L1EM15',
    'HLT_e17_lhloose_nod0_L1EM15',
    'HLT_e17_lhloose_nodeta_L1EM15',
    'HLT_e17_lhloose_nodphires_L1EM15',
    'HLT_e17_loose',
    'HLT_e17_lhloose',
    'HLT_e17_lhloose_cutd0dphideta',
    'HLT_e17_lhloose_nod0',    
    #
    'HLT_2g10_loose',
    #
    # 1e
    'HLT_e24_medium_iloose_L1EM18VH',
    'HLT_e24_lhmedium_iloose_L1EM18VH',
    'HLT_e24_lhmedium_nod0_iloose_L1EM18VH',
    'HLT_e24_lhmedium_nodphires_iloose_L1EM18VH',
    'HLT_e24_lhmedium_cutd0dphideta_iloose_L1EM18VH',
    'HLT_e24_lhmedium_nodeta_iloose_L1EM18VH',
    'HLT_e24_medium_L1EM18VH',
    #
    'HLT_e24_medium_iloose_L1EM20VH',
    'HLT_e24_lhmedium_iloose_L1EM20VH',
    'HLT_e24_lhmedium_nod0_iloose_L1EM20VH',
    'HLT_e24_lhmedium_cutd0dphideta_iloose_L1EM20VH',
    'HLT_e24_tight_L1EM20VH',
    'HLT_e24_lhmedium_L1EM18VH',
    'HLT_e24_lhtight_L1EM20VH',
    #
    'HLT_e24_tight_iloose',
    'HLT_e24_lhtight_iloose',
    'HLT_e24_lhtight_nod0_iloose',
    'HLT_e24_lhtight_cutd0dphideta_iloose',
    #
    'HLT_e26_tight_iloose',
    'HLT_e26_lhtight_iloose',
    'HLT_e26_lhtight_nod0_iloose',
    'HLT_e26_lhtight_cutd0dphideta_iloose',
    #
    # 1e, high ET
    'HLT_e60_medium',
    'HLT_e60_lhmedium',
    'HLT_e60_lhmedium_nod0',
    'HLT_e60_lhmedium_cutd0dphideta',
    #
    'HLT_e120_loose',
    'HLT_e120_lhloose',
    'HLT_e120_lhloose_nod0',
    #
    # 2e
    'HLT_2e12_loose_L12EM10VH',
    'HLT_2e12_lhloose_L12EM10VH',
    'HLT_2e12_lhloose_cutd0dphideta_L12EM10VH',
    'HLT_2e12_lhloose_nodphires_L12EM10VH',
    'HLT_2e12_lhloose_nodeta_L12EM10VH',
    'HLT_2e12_lhloose_nod0_L12EM10VH',
    #
    'HLT_2e17_loose',
    'HLT_2e17_lhloose',
    #
    # 1g
    'HLT_g25_loose',
    'HLT_g25_medium',
    'HLT_g35_loose',
    'HLT_g35_medium',
    'HLT_g120_loose',
    'HLT_g140_loose',
    #
    # 2g
    'HLT_g35_loose_g25_loose',
    'HLT_g35_medium_g25_medium',
    #
    # Z T&P
    'HLT_e24_lhtight_L1EM20VH_e15_etcut_Zee',
    'HLT_e24_tight_L1EM20VH_e15_etcut_Zee',
    'HLT_e26_lhtight_e15_etcut_Zee',
    'HLT_e26_tight_e15_etcut_Zee',
    ]

#######
# combined

L1_combined=[
    'L1_J40_XE50',
    ]

L1_combined_downstream=[
    'L1_MU10_TAU12IM',
    ]

HLT_combined=[
    # jet + xe
    'HLT_j80_xe80',
    ]

#######
# dedicated

L1_dedicated=[
    'L1_MU10_2MU6',
    'L1_EM7_MU10',
    'L1_EM15_MU4'
    ]

HLT_dedicated=[
    #
    # tau->3mu (narrow scan)
    'HLT_mu11_2mu4noL1_nscan03_L1MU10_2MU6',
    #
    # Lepton-Jet
    'HLT_mu20_msonly_mu6noL1_msonly_nscan05',
    #
    # LFV tau
    'HLT_g10_etcut_L1EM7_mu10_taumass',
    'HLT_g20_etcut_L1EM15_mu4_taumass',
]

#######
# PS'ed triggers

Rules_Prescaled={
    'L1_MU4'                                       : { 1 : TriggerRule(rate=200, comment='') },
    'L1_2EM3'                                      : { 1 : TriggerRule(rate=80, comment='') },
    'L1_EM3'                                       : { 1 : TriggerRule(rate=1, comment='') },
    'L1_EM7'                                       : { 1 : TriggerRule(rate=500, comment='') },
    'L1_XE35'                                      : { 1 : TriggerRule(PS=25, comment='For 2Hz output. Can PS') }, 
    'L1_XE40'                                      : { 1 : TriggerRule(PS=50, comment='') },
    'L1_TAU8'                                      : { 1 : TriggerRule(rate=100, comment='') },
    'L1_TAU12IL'                                   : { 1 : TriggerRule(rate=100, comment='') },
    'L1_TAU12IT'                                   : { 1 : TriggerRule(rate=100, comment='') },
    'L1_TAU12IM'                                   : { 1 : TriggerRule(rate=300, comment='') },
    'L1_TAU20IL'                                   : { 1 : TriggerRule(rate=100, comment='') },
    'L1_TAU20IT'                                   : { 1 : TriggerRule(rate=100, comment='') },
    'L1_TAU20IM'                                   : { 1 : TriggerRule(rate=100, comment='') },
    
    'HLT_noalg_L1EM3'                              : { 1 : TriggerRule(PS=1, comment='L1PS') },
    'HLT_noalg_L1EM7'                              : { 1 : TriggerRule(PS=5000, comment='L1EM7=10kHz raw') },
    'HLT_noalg_L1TAU12IL'                          : { 1 : TriggerRule(PS=300, comment='L1PS') },
    'HLT_noalg_L1TAU12IT'                          : { 1 : TriggerRule(PS=300, comment='L1PS') },
    'HLT_noalg_L1TAU12IM'                          : { 1 : TriggerRule(PS=300, comment='L1PS') },
    'HLT_noalg_L1TAU30'                            : { 1 : TriggerRule(rate=2, comment='HLTPS') },
    'HLT_noalg_L1XE35'                             : { 1 : TriggerRule(rate=2, comment='HLTPS') },
    
    'HLT_e5_tight_e4_etcut'                        : { 1 : TriggerRule(PS=1, comment='Controlled by L1_2EM3 rate') },
    'HLT_e5_lhtight_e4_etcut'                      : { 1 : TriggerRule(PS=1, comment='Controlled by L1_2EM3 rate') },
    
    # SM
    'HLT_g10_loose'   : {   1000 : TriggerRule(rate=50, comment='SM request') },

    # bjet
    'HLT_j15_bperf'   : {   1000 : TriggerRule(rate=10, comment='') },  # change from j110_bperf
    'HLT_j55_bperf'   : {   1000 : TriggerRule(rate=10, comment='') },  # L1_J20, HLT rej: 40%

    # monitoring chains taken from Week1 rules. Others are alredy in primary.
    'HLT_tau25_perf_ptonly'                        : { 1 : TriggerRule(rate=5, comment='') },
    'HLT_tau25_perf_tracktwo'                      : { 1 : TriggerRule(rate=5, comment='') },
    'HLT_tau25_loose1_tracktwo'                    : { 1 : TriggerRule(rate=5, comment='') },
    'HLT_tau25_medium1_tracktwo'                   : { 1 : TriggerRule(rate=5, comment='') },
    'HLT_tau25_tight1_tracktwo'                    : { 1 : TriggerRule(rate=5, comment='') },
    'HLT_tau25_perf_tracktwo_L1TAU12'              : { 1 : TriggerRule(rate=5, comment='') },
    'HLT_tau35_perf_ptonly_tau25_perf_ptonly_L1TAU20IM_2TAU12IM' : { 1 : TriggerRule(rate=5, comment='') },
    'HLT_tau35_medium1_ptonly_L1TAU20'             : { 1 : TriggerRule(PS=1, comment='') },
    
    'HLT_mu4_idperf'                               : { 1 : TriggerRule(rate=5, comment='') },
    'HLT_mu6_idperf'                               : { 1 : TriggerRule(rate=5, comment='', ESValue=-1) },
    'HLT_mu6_L2Star_idperf'                        : { 1 : TriggerRule(rate=5, comment='') },
    'HLT_e5_loose_idperf'                          : { 1 : TriggerRule(rate=5, comment='') },
    'HLT_e5_loose_L2Star_idperf'                   : { 1 : TriggerRule(rate=5, comment='') },
    'HLT_tau25_idperf_track'                       : { 1 : TriggerRule(rate=5, comment='') },
    'HLT_tau25_idperf_tracktwo'                    : { 1 : TriggerRule(rate=5, comment='') },
    'HLT_tau0_perf_ptonly_L1TAU12'                 : { 1 : TriggerRule(PS=1, comment='') },
    'HLT_tau25_loose1_ptonly'                      : { 1 : TriggerRule(PS=1, comment='') },
    'HLT_tau25_medium1_ptonly'                     : { 1 : TriggerRule(PS=1, comment='') },
    'HLT_j0_lcw_jes_L1J12'                         : { 1 : TriggerRule(rate=5, comment='', rerun = 0.0) },
    'HLT_j0_perf_bperf_L1MU10'                     : { 1 : TriggerRule(PS=-1, comment='', rerun = 0.0) },
    'HLT_te20'                                     : { 1 : TriggerRule(PS=1, comment='L1_TE rate controlled at L1. Rate=5Hz', rerun = 0.0) },
    'HLT_xe35'                                     : { 1 : TriggerRule(rate=5, comment='', rerun = 0.0) },

    'HLT_tau35_perf_tracktwo_L1TAU20_tau25_perf_tracktwo_L1TAU12' : { 1 : TriggerRule(rate=5, comment='', rerun = 0.0) },
    'HLT_tau35_perf_tracktwo_tau25_perf_tracktwo'                 : { 1 : TriggerRule(rate=5, comment='', rerun = 0.0) },
    'HLT_tau35_tight1_tracktwo_tau25_tight1_tracktwo_L1TAU20IM_2TAU12IM' : { 1 : TriggerRule(rate=5, comment='', rerun = 0.0) },
}


#################################################################################################
#
# L1 Low Lumi rule
#
#################################################################################################
L1_lowlumi=[
    #'L1_2EM10VH',
    'L1_3J25.0ETA23',
    #'L1_3J40',
    #'L1_3MU4',
    #'L1_4J15',
    'L1_EM15HI_2TAU12IM',
    #'L1_EM15HI_2TAU12IM_J25_3J12',
    #'L1_EM15HI_2TAU12IM_XE35',  
    #'L1_EM15HI_TAU40_2TAU15',
    #'L1_EM15_MU4',
    #'L1_EM20VH',
    #'L1_EM7_MU10',
    #'L1_J75',
    #'L1_J75.32ETA49', # unPSed in jetCP
    #'L1_MU10_TAU12IM',
    #'L1_MU10_TAU12IM_J25_2J12',
    #'L1_MU10_TAU12IM_XE35',
    'L1_MU10_TAU12_J25_2J12',
    #'L1_MU10_TAU20',
    #'L1_MU15',
    'L1_MU4_3J20',
    'L1_MU6_J40',
    #'L1_TAU20IM_2TAU12IM_J25_2J20_3J12', 
    #'L1_TAU20IM_2TAU12IM_XE35',
    #'L1_TAU20_2J20_XE45',
    #'L1_TAU20_2TAU12_XE35',
    #'L1_TAU60',
    #'L1_XE50',
    ]

if not doEB:
    L1_lowlumi+=[
        'L1_2EM8VH_MU10', # EB
        'L1_2J15_XE55', # EB
        'L1_2MU6', # EB
        'L1_EM15VH_MU10', # EB
        ]

L1_lowlumi_Rules=dict(map(None,L1_lowlumi,len(L1_lowlumi)*[{
                         1 : TriggerRule(PS=1, comment='L1'), 
                       }]))


#################################################################################################
#
# L1Calo streamers
#
#################################################################################################

L1CaloSeed = []

ScaleLumi=12 # wrt last weekend

L1Streamer_Rules = {
    'HLT_noalg_L1LowLumi'       : {   1 : TriggerRule(PS=3, comment='L1PS=1', ESValue=40) },

    'L1_TAU12'                  : {   1 : TriggerRule(rate=300, comment='For TAU Streamer')},
    'L1_TAU20'                  : {   1 : TriggerRule(rate=100, comment='For TAU streamer')},
    'HLT_noalg_L1TAU12'         : {   1 : TriggerRule(PS=108, comment='TAU Streamer Not available in cost file. Target 20Hz. Controlled at L1 was 5')},  
    'HLT_noalg_L1TAU20'         : {   1 : TriggerRule(PS=5, comment='TAU Streamer. Not available in cost file. Target 20Hz. Controlled at L1 and HLT for 20Hz')},  
    'HLT_noalg_L1MU4'           : {   1 : TriggerRule(rate=20, comment='MU Streamer')},  
    'HLT_noalg_L1MU6'           : {   1 : TriggerRule(rate=20, comment='MU Streamer')},
    
    'HLT_noalg_L1J75'           : {   1 : TriggerRule(PS=1*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J75.32ETA49'   : {   1 : TriggerRule(PS=1*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J85'           : {   1 : TriggerRule(PS=1*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J100'          : {   1 : TriggerRule(PS=1*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J100.32ETA49'  : {   1 : TriggerRule(PS=1*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J120'          : {   1 : TriggerRule(PS=1*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J400'          : {   1 : TriggerRule(PS=1*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1EM20VH'        : {   1 : TriggerRule(PS=1*ScaleLumi*5, comment='Streamer for L1Calo')},
    'HLT_noalg_L1EM20VHI'       : {   1 : TriggerRule(PS=1*ScaleLumi*5, comment='Streamer for L1Calo')},
    'HLT_noalg_L1EM22VHI'       : {   1 : TriggerRule(PS=1*ScaleLumi*5, comment='Streamer for L1Calo')},
    'HLT_noalg_L1XE50'          : {   1 : TriggerRule(PS=1*ScaleLumi*40, comment='Streamer for L1Calo')},
    'HLT_noalg_L1XE55'          : {   1 : TriggerRule(PS=1*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1XE60'          : {   1 : TriggerRule(PS=1*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1XE70'          : {   1 : TriggerRule(PS=1*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1XE80'          : {   1 : TriggerRule(PS=1*ScaleLumi, comment='Streamer for L1Calo')},

    'HLT_noalg_L1J12'           : {   1 : TriggerRule(PS=22*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J15'           : {   1 : TriggerRule(PS=22*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J15.29ETA32'   : {   1 : TriggerRule(PS=22*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J15.32ETA49'   : {   1 : TriggerRule(PS=22*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J20'           : {   1 : TriggerRule(PS=22*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J20.29ETA32'   : {   1 : TriggerRule(PS=22*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J20.32ETA49'   : {   1 : TriggerRule(PS=22*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J25'           : {   1 : TriggerRule(PS=22*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J30'           : {   1 : TriggerRule(PS=22*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J30.32ETA49'   : {   1 : TriggerRule(PS=22*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J40'           : {   1 : TriggerRule(PS=22*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J50'           : {   1 : TriggerRule(rate=5, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J50.32ETA49'   : {   1 : TriggerRule(PS=22*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1EM8VH'         : {   1 : TriggerRule(PS=22*ScaleLumi*30, comment='Streamer for L1Calo')},
    'HLT_noalg_L1EM10'          : {   1 : TriggerRule(PS=22*ScaleLumi*3, comment='Streamer for L1Calo')},
    'HLT_noalg_L1EM12'          : {   1 : TriggerRule(PS=22*ScaleLumi*3, comment='Streamer for L1Calo')},
    'HLT_noalg_L1EM15'          : {   1 : TriggerRule(PS=22*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1EM15VH'        : {   1 : TriggerRule(PS=22*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1EM13VH'        : {   1 : TriggerRule(PS=22*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1EM10VH'        : {   1 : TriggerRule(PS=22*ScaleLumi*3, comment='Streamer for L1Calo')},
    'HLT_noalg_L1TAU8'          : {   1 : TriggerRule(PS=22*ScaleLumi*100, comment='Streamer for L1Calo')},
    'HLT_noalg_L1TAU40'         : {   1 : TriggerRule(PS=22*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1TAU60'         : {   1 : TriggerRule(PS=22*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1XE40'          : {   1 : TriggerRule(PS=22*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1XE45'          : {   1 : TriggerRule(PS=22*ScaleLumi, comment='Streamer for L1Calo')},   

    'L1_TE10'                   : {   1 : TriggerRule(rate=2, comment='Streamer for L1Calo')},
    'L1_TE20'                   : {   1 : TriggerRule(rate=2, comment='Streamer for L1Calo')},
    'L1_TE30'                   : {   1 : TriggerRule(rate=2, comment='Streamer for L1Calo')},
    'L1_TE40'                   : {   1 : TriggerRule(rate=2, comment='Streamer for L1Calo')},
    'L1_TE50'                   : {   1 : TriggerRule(rate=2, comment='Streamer for L1Calo')},
    'L1_TE10.0ETA25'            : {   1 : TriggerRule(rate=2, comment='Streamer for L1Calo')},
    'L1_TE20.0ETA25'            : {   1 : TriggerRule(rate=2, comment='Streamer for L1Calo')},
    'HLT_noalg_L1TE10'          : {   1 : TriggerRule(PS=1, comment='Streamer for L1Calo')},
    'HLT_noalg_L1TE20'          : {   1 : TriggerRule(PS=1, comment='Streamer for L1Calo')},
    'HLT_noalg_L1TE30'          : {   1 : TriggerRule(PS=1, comment='Streamer for L1Calo')},
    'HLT_noalg_L1TE40'          : {   1 : TriggerRule(PS=1, comment='Streamer for L1Calo')},
    'HLT_noalg_L1TE50'          : {   1 : TriggerRule(PS=1, comment='Streamer for L1Calo')},
    'HLT_noalg_L1TE10.0ETA25'   : {   1 : TriggerRule(PS=1, comment='Streamer for L1Calo')},
    'HLT_noalg_L1TE20.0ETA25'   : {   1 : TriggerRule(PS=1, comment='Streamer for L1Calo')}, 
        
    'HLT_noalg_L1XS20'          : {   1 : TriggerRule(PS=22*ScaleLumi*50, comment='Streamer for L1Calo')},
    'HLT_noalg_L1XS30'          : {   1 : TriggerRule(PS=22*ScaleLumi*50, comment='Streamer for L1Calo')},
    'HLT_noalg_L1XS40'          : {   1 : TriggerRule(PS=22*ScaleLumi*50, comment='Streamer for L1Calo')},
    'HLT_noalg_L1XS50'          : {   1 : TriggerRule(PS=22*ScaleLumi, comment='Streamer for L1Calo')},
    'HLT_noalg_L1XS60'          : {   1 : TriggerRule(PS=22*ScaleLumi, comment='Streamer for L1Calo')},
    }


########################################

L1_jetCP={
    'L1_RD0_FILLED'   : {   1000 : TriggerRule(rate=500+(3500 if ScaleMoreBW else 0), scaling='bunches', comment='Higher priority than FJ. Halved rate for super safety margin. Originally 6kHz') },
    'L1_J12'          : {   1000 : TriggerRule(rate=10, comment='For HLT_conej40_larpebj') },  
    'L1_J15'          : {   1000 : TriggerRule(rate=(500 if not ScaleMoreBW else 700), comment='For j60') },  
    'L1_J15.32ETA49'  : {   1000 : TriggerRule(PS=(2 if ScaleMoreBW else 100), comment='PS for safety margin') }, 
    'L1_J20.32ETA49'  : {   1000 : TriggerRule(PS=(2 if ScaleMoreBW else 50 ), comment='') }, 
    'L1_J30.32ETA49'  : {   1000 : TriggerRule(PS=(2 if ScaleMoreBW else 20 ), comment='') }, 
    }

HLT_jetCP={
    'HLT_j15'                             : {   1000 : TriggerRule(PS=160, comment='RD seeded #Target 10Hz') }, 
    'HLT_j25'                             : {   1000 : TriggerRule(PS=40, comment='RD seeded # 10Hz') },
    'HLT_j35'                             : {   1000 : TriggerRule(PS=10, comment='RD seeded # 10Hz') },
    'HLT_j45_L1RD0'                       : {   1000 : TriggerRule(PS=6, comment='RD seeded # 10Hz') },
    'HLT_j15_320eta490'                   : {   1000 : TriggerRule(PS=5, comment='75Hz when increse RD0 from 200 Hz to 6 kHz') },
    'HLT_j25_320eta490'                   : {   1000 : TriggerRule(PS=1, comment='') },
    'HLT_j35_320eta490'                   : {   1000 : TriggerRule(PS=1, comment='') },
    'HLT_j45_320eta490'                   : {   1000 : TriggerRule(rate=10, comment='') },
    'HLT_j55_320eta490'                   : {   1000 : TriggerRule(rate=10, comment='') },
    'HLT_j55'                             : {   1000 : TriggerRule(rate=10, comment='J15') },
    'HLT_j60'                             : {   1000 : TriggerRule(PS=1, comment='') },
    'HLT_j60_320eta490'                   : {   1000 : TriggerRule(PS=1, comment='') },
    'HLT_j85_320eta490'                   : {   1000 : TriggerRule(PS=1, comment='') },
    'HLT_j110_320eta490'                  : {   1000 : TriggerRule(PS=1, comment='') },
    'HLT_j45_j45_320eta490'               : {   1000 : TriggerRule(PS=1, comment='') },
    'HLT_j55_j55_320eta490_L1RD0_FILLED'  : {   1000 : TriggerRule(PS=1, comment='') },
    }


L1  = L1_muon  + L1_bphys  + L1_met +  L1_jet  + L1_bjet  + L1_tau  + L1_egamma  + L1_combined  + L1_dedicated + L1_muon_downstream + L1_egamma_downstream + L1_tau_downstream + L1_combined_downstream

HLT = HLT_muon + HLT_bphys + HLT_met + HLT_jet + HLT_bjet + HLT_tau + HLT_egamma + HLT_combined + HLT_dedicated

L1_Rules=dict(map(None,L1,len(L1)*[{
    1000 : TriggerRule(PS=1, comment='L1'),
    }]))

HLT_Rules=dict(map(None,HLT,len(HLT)*[{
    1000 : TriggerRule(PS=1, comment=''), 
    }]))


########################################

# monitoring 
import Monitoring_pp_v5_rules

zerobias_Rules = {
    'L1_ZB'                    : { 1 : TriggerRule(PS=ScaleLumi*10, comment='L1') }, 
    'HLT_noalg_zb_L1ZB'        : { 1 : TriggerRule(PS=1, comment='') },
    'HLT_j40_L1ZB'             : { 1 : TriggerRule(PS=1, comment='') },
    }

tauoverlay_Rules = { 
    'L1_2MU20_OVERLAY'             : { 1 : TriggerRule(PS=1, comment = 'TauOverlay', rerun = 0.0) },
    'HLT_noalg_to_L12MU20_OVERLAY' : { 1 : TriggerRule(PS=1, comment = 'TauOverlay', rerun = 0.0) },
    }

beamspot_Rules = {
    # online measurement: unPS (ATR-10984)
    'HLT_beamspot_trkFS_L2StarB'           : { 1 : TriggerRule(PS=1, comment='BeamSpot') },
    'HLT_beamspot_trkFS_trkfast'           : { 1 : TriggerRule(PS=1, comment='BeamSpot') },
    # offline measurement
    'HLT_beamspot_activeTE_L2StarB_pebTRT' : { 1 : TriggerRule(PS=1, comment='BeamSpot') },
    'HLT_beamspot_activeTE_trkfast_pebTRT' : { 1 : TriggerRule(PS=1, comment='BeamSpot') },
    'HLT_beamspot_allTE_L2StarB_pebTRT'    : { 1 : TriggerRule(PS=1, comment='BeamSpot') },
    'HLT_beamspot_allTE_trkfast_pebTRT'    : { 1 : TriggerRule(PS=1, comment='BeamSpot') },
    }

afterGlow_Rules={
    'HLT_lumipeb_vdm_L1RD0_BGRP9'          : { 1 : TriggerRule(rate=100, comment='') },
    'L1_RD0_BGRP9'                         : { 1 : TriggerRule(PS=5000, comment='') },
    }



########################################
# EB: Tim will adjust PSs for eb streamers 

import Physics_pp_v5_EB_rules


########################################

HLT_Disabled=[
    'HLT_2mu6_bDimu_novtx_noos', 'HLT_2mu6_bDimu', 'HLT_2mu6_bDimu_noinvm_novtx_ss', 'HLT_2mu4_bDimu_noinvm_novtx_ss', 'HLT_noalg_L12MU4', 'HLT_mu4_mu4_idperf_bJpsimumu_noid', 'HLT_noalg_mb_L1MBTS_1_1_UNPAIRED_ISO', 'HLT_mb_mbts_L1MBTS_1_1_UNPAIRED_ISO', 'HLT_noalg_L1MU4_J30', 'HLT_mb_sp2_hmtperf', 'HLT_mb_sp300_trk10_sumet50_hmt_L1RD3_FILLED', 'HLT_mb_sptrk_noisesup_L1RD3_FILLED', 'HLT_mb_sp1800_hmtperf', 'HLT_noalg_mb_L1RD3_FILLED', 'HLT_mb_sp600_trk10_sumet50_hmt_L1RD3_FILLED', 'HLT_te50_L1RD3_FILLED', 'HLT_mb_sptrk_pt8_L1RD3_FILLED', 'HLT_mb_sp500_hmtperf', 'HLT_mb_sp700_trk50_hmt_L1RD3_FILLED', 'HLT_mb_sptrk_pt6_L1RD3_FILLED', 'HLT_mb_sptrk_L1RD3_FILLED', 'HLT_mb_sptrk_pt4_L1RD3_FILLED', 'HLT_noalg_mb_L1MBTS_1_1_EMPTY', 'HLT_mb_mbts_L1MBTS_1_1_EMPTY', 'HLT_e12_lhloose_nodeta_L1EM10VH', 'HLT_e12_lhloose_nod0_L1EM10VH', 'HLT_e12_lhloose_L1EM10VH', 'HLT_e12_lhloose_nodphires_L1EM10VH', 'HLT_e12_lhloose_cutd0dphideta_L1EM10VH', 'HLT_e12_loose_L1EM10VH', 'HLT_noalg_L1MU4_3J15', 'HLT_tau8_cosmic_track', 'HLT_tau8_cosmic_ptonly', 'HLT_noalg_L1DR-MU10TAU12I', 'HLT_noalg_L1TAU20_2TAU12', 'HLT_noalg_L1HT1-J0.ETA49', 'HLT_noalg_L1J20_J20.32ETA49', 'HLT_j85_j85_320eta490', 'HLT_j60_j60_320eta490', 'HLT_alfacalib_L1ALFA_ELAST15', 'HLT_alfacalib_L1ALFA_ELAST18', 'HLT_noalg_L1DETA-JJ', 'HLT_noalg_L1LHCF_UNPAIRED_ISO', 'HLT_lhcfpeb_L1LHCF_UNPAIRED_ISO', 'HLT_noalg_L1BPH-4M8-MU6MU4', 'HLT_noalg_bkg_L1J12_BGRP0', 'HLT_mb_mbts_L1MBTS_1_EMPTY', 'HLT_noalg_mb_L1MBTS_1_EMPTY', 'HLT_noalg_L1MJJ-250-0', 'HLT_noalg_dcmmon_L1RD2_BGRP11', 'HLT_lumipeb_vdm_L1MBTS_2_UNPAIRED_ISO', 'HLT_mb_mbts_L1MBTS_2_UNPAIRED_ISO', 'HLT_noalg_mb_L1MBTS_2_UNPAIRED_ISO', 'HLT_g0_perf_L1EM3_EMPTY', 'HLT_e0_perf_L1EM3_EMPTY', 'HLT_alfacalib_L1ALFA_ANY', 'HLT_noalg_L1ALFA_ANY', 'HLT_mb_sptrk_vetombts2in_peb_L1ALFA_ANY', 'HLT_noalg_bkg_L1J30.32ETA49', 'HLT_noalg_L1J30.0ETA49_2J20.0ETA49', 'HLT_noalg_L1TAU20IM_2TAU12IM', 'HLT_mb_sp1400_hmtperf_L1TE10.0ETA25', 'HLT_mb_sp1400_trk90_hmt_L1TE10.0ETA25', 'HLT_noalg_mb_L1TE10.0ETA25', 'HLT_mb_sp600_trk10_sumet50_hmt_L1TE10.0ETA25', 'HLT_noalg_L1MBTS_2_BGRP11', 'HLT_lumipeb_vdm_L1MBTS_2_BGRP11', 'HLT_alfacalib_L1ALFA_A7L1_OD', 'HLT_noalg_dcmmon_L1RD2_EMPTY', 'HLT_noalg_mb_L1RD2_EMPTY', 'HLT_mu6_mu4_bDimu_noinvm_novtx_ss', 'HLT_mu6_mu4_bDimu', 'HLT_mu6_mu4_bDimu_novtx_noos', 'HLT_noalg_L1Standby', 'HLT_noalg_L1Calo', 'HLT_noalg_L1Calo_EMPTY', 'HLT_alfacalib_L1ALFA_ELAS', 'HLT_alfacalib', 'HLT_noalg_L1ALFA_PhysAny', 'HLT_noalg_L1ALFA_SYS', 'HLT_alfacalib_L1ALFA_SYS', 'HLT_noalg_L1ALFA_Phys', 'HLT_mb_sptrk_vetombts2in_L1ALFA_CEP', 'HLT_lumipeb_vdm_L1MBTS_1_BGRP9', 'HLT_noalg_L1MBTS_1_BGRP9', 'HLT_noalg_mb_L1RD2_FILLED', 'HLT_tau35_medium1_tracktwo_L1TAU20', 'HLT_tau25_medium1_tracktwo_L1TAU12IL', 'HLT_tau25_medium1_tracktwo_L1TAU12', 'HLT_noalg_bkg_L1J30.32ETA49_BGRP0', 'HLT_2mu10_bDimu_noinvm_novtx_ss', 'HLT_2mu10_bDimu', 'HLT_2mu10_bDimu_novtx_noos', 'HLT_ibllumi_L1RD0_ABORTGAPNOTCALIB', 'HLT_tau25_medium1_tracktwo_L1TAU12IT', 'HLT_j175_lcw', 'HLT_j200', 'HLT_j175_lcw_jes', 'HLT_j175_nojcalib', 'HLT_j175', 'HLT_j175_jes', 'HLT_j175_lcw_nojcalib', 'HLT_j200_jes_PS', 'HLT_3j175_jes_PS', 'HLT_noalg_L1MU15', 'HLT_mu18_bJpsi_Trkloose', 'HLT_lhcfpeb_L1LHCF_EMPTY', 'HLT_noalg_L1LHCF_EMPTY', 'HLT_alfacalib_L1ALFA_B7R1_OD', 'HLT_4j45_test2', 'HLT_4j45_test1', 'HLT_4j45', 'HLT_noalg_L1LFV-MU', 'HLT_mb_sp300_trk10_sumet50_hmt_L1TE10', 'HLT_noalg_mb_L1TE10', 'HLT_mb_sp1400_trk90_hmt_L1TE10', 'HLT_mb_sp1400_hmtperf_L1TE10', 'HLT_tau25_perf_ptonly_L1TAU12', 'HLT_tau50_medium1_tracktwo_L1TAU12', 'HLT_noalg_mb_L1LUCID', 'HLT_mb_perf_L1LUCID', 'HLT_noalg_L1JPSI-1M5-EMs', 'HLT_tau160_idperf_track', 'HLT_tau125_perf_tracktwo', 'HLT_tau125_perf_ptonly', 'HLT_tau160_idperf_tracktwo', 'HLT_tau0_perf_ptonly_L1TAU60', 'HLT_noalg_L1BPH-DR-MU6MU4', 'HLT_mb_mbts_L1MBTS_2_EMPTY', 'HLT_noalg_mb_L1MBTS_2_EMPTY', 'HLT_noalg_bkg_L1J12', 'HLT_xs30_tc_lcw', 'HLT_xs30', 'HLT_noalg_mb_L1RD0_UNPAIRED_ISO', 'HLT_ibllumi_L1RD0_UNPAIRED_ISO', 'HLT_lumipeb_vdm_L1RD0_UNPAIRED_ISO', 'HLT_mb_sptrk_noisesup_L1RD0_UNPAIRED_ISO', 'HLT_mb_sptrk_L1RD0_UNPAIRED_ISO', 'HLT_alfacalib_L1LHCF_ALFA_ANY_A_UNPAIRED_ISO', 'HLT_lhcfpeb_L1LHCF_ALFA_ANY_A_UNPAIRED_ISO', 'HLT_noalg_mb_L1TE50', 'HLT_noalg_L1JPSI-1M5-EM6s', 'HLT_mb_sp600_trk10_sumet60_hmt_L1TE20.0ETA25', 'HLT_mb_sp1400_trk100_hmt_L1TE20.0ETA25', 'HLT_noalg_mb_L1TE20.0ETA25', 'HLT_mb_sp1400_hmtperf_L1TE20.0ETA25', 'HLT_e5_lhtight_idperf', 'HLT_e5_lhloose_idperf', 'HLT_e5_lhloose', 'HLT_e5_loose', 'HLT_e5_tight_idperf', 'HLT_noalg_L1DY-BOX-2MU4', 'HLT_noalg_L1DY-BOX-2MU6', 'HLT_g10_etcut', 'HLT_e10_lhtight_idperf', 'HLT_e10_tight_idperf', 'HLT_noalg_L1MJJ-350-0', 'HLT_noalg_L1RD0_BGRP9', 'HLT_j0_perf_bperf_L1J12_EMPTY', 'HLT_ht0_L1J12_EMPTY', 'HLT_j0_L1J12_EMPTY', 'HLT_alfacalib_L1LHCF_ALFA_ANY_C_UNPAIRED_ISO', 'HLT_lhcfpeb_L1LHCF_ALFA_ANY_C_UNPAIRED_ISO', 'HLT_noalg_cosmiccalo_L1J30_FIRSTEMPTY', 'HLT_alfacalib_L1ALFA_B7L1_OD', 'HLT_mb_sptrk_vetombts2in_peb_L1ALFA_ANY_UNPAIRED_ISO', 'HLT_noalg_L1EM15VH_3EM7', 'HLT_beamspot_trkFS_L2StarB_L1TRT_FILLED', 'HLT_noalg_idcosmic_L1TRT_FILLED', 'HLT_beamspot_allTE_L2StarB_peb_L1TRT_FILLED', 'HLT_beamspot_activeTE_L2StarB_pebTRT_L1TRT_FILLED', 'HLT_beamspot_allTE_trkfast_peb_L1TRT_FILLED', 'HLT_beamspot_allTE_trkfast_pebTRT_L1TRT_FILLED', 'HLT_beamspot_allTE_L2StarB_pebTRT_L1TRT_FILLED', 'HLT_beamspot_activeTE_trkfast_pebTRT_L1TRT_FILLED', 'HLT_beamspot_activeTE_trkfast_peb_L1TRT_FILLED', 'HLT_beamspot_trkFS_trkfast_L1TRT_FILLED', 'HLT_beamspot_activeTE_L2StarB_peb_L1TRT_FILLED', 'HLT_j85_test2', 'HLT_j85_test1', 'HLT_j85_lcw_nojcalib', 'HLT_j85_lcw', 'HLT_j85_0eta240_test1_j25_240eta490_test1', 'HLT_j85_jes_test1', 'HLT_j85_jes_test2', 'HLT_j85_0eta240_test2_j25_240eta490_test2', 'HLT_j85_lcw_jes', 'HLT_j85_test1_2j45_test1', 'HLT_j85_2j45', 'HLT_j85', 'HLT_j85_nojcalib', 'HLT_j85_test2_2j45_test2', 'HLT_j85_jes', 'HLT_j100', 'HLT_tau35_medium1_ptonly', 'HLT_tau35_perf_ptonly', 'HLT_tau35_perf_tracktwo', 'HLT_tau35_medium1_tracktwo', 'HLT_tau35_medium1_tracktwo_L1TAU20IL', 'HLT_noalg_L1EM18VH', 'HLT_e12_loose', 'HLT_e12_lhloose_nod0', 'HLT_e12_lhloose', 'HLT_tau35_medium1_tracktwo_L1TAU20IT', 'HLT_noalg_L1MJJ-300-0', 'HLT_noalg_L1J15_J15.32ETA49', 'HLT_j55_j55_320eta490', 'HLT_mu4_cosmic_L1MU11_EMPTY', 'HLT_noalg_cosmicmuons_L1MU11_EMPTY', 'HLT_mu4_msonly_cosmic_L1MU11_EMPTY', 'HLT_noalg_L1MU4_J12', 'HLT_noalg_L1BPH-DR-2MU4', 'HLT_noalg_L1BPH-DR-2MU6', 'HLT_j60_280eta320', 'HLT_noalg_L1CALREQ2', 'HLT_noalg_L1HT200-J20s5.ETA49', 'HLT_noalg_L1TGC_BURST', 'HLT_noalg_L1MU6_J20', 'HLT_j85_280eta320_lcw', 'HLT_j85_280eta320_lcw_jes', 'HLT_j85_280eta320_jes', 'HLT_j85_280eta320', 'HLT_noalg_mb_L1RD3_EMPTY', 'HLT_noalg_L1LHCF', 'HLT_lhcfpeb', 'HLT_alfacalib_L1LHCF_ALFA_ANY_C', 'HLT_lhcfpeb_L1LHCF_ALFA_ANY_C', 'HLT_noalg_mb_L1TE40', 'HLT_larnoiseburst_L1All', 'HLT_noalg_L1All', 'HLT_larnoiseburst_loose_rerun', 'HLT_j0_perf_ds1_L1All', 'HLT_id_cosmicid', 'HLT_larnoiseburst_loose_L1All', 'HLT_id_cosmicid_trtxk', 'HLT_id_cosmicid_trtxk_central', 'HLT_xe35_l2fsperf_wEFMuFEB_wEFMu', 'HLT_beamspot_trkFS_trkfast_L1TRT_EMPTY', 'HLT_beamspot_trkFS_L2StarB_L1TRT_EMPTY', 'HLT_beamspot_allTE_trkfast_pebTRT_L1TRT_EMPTY', 'HLT_beamspot_activeTE_trkfast_pebTRT_L1TRT_EMPTY', 'HLT_beamspot_activeTE_L2StarB_pebTRT_L1TRT_EMPTY', 'HLT_beamspot_activeTE_trkfast_peb_L1TRT_EMPTY', 'HLT_beamspot_allTE_L2StarB_pebTRT_L1TRT_EMPTY', 'HLT_beamspot_allTE_trkfast_peb_L1TRT_EMPTY', 'HLT_beamspot_allTE_L2StarB_peb_L1TRT_EMPTY', 'HLT_noalg_idcosmic_L1TRT_EMPTY', 'HLT_beamspot_activeTE_L2StarB_peb_L1TRT_EMPTY', 'HLT_noalg_mb_L1TE30', 'HLT_mu10', 'HLT_noalg_L1MU10', 'HLT_mu10_r1extr', 'HLT_mu10_idperf', 'HLT_mu10_bJpsi_Trkloose', 'HLT_mu10_msonly', 'HLT_noalg_L1MJJ-200-0', 'HLT_lumipeb_vdm_L1MBTS_1_UNPAIRED_ISO', 'HLT_noalg_mb_L1MBTS_1_UNPAIRED_ISO', 'HLT_mb_mbts_L1MBTS_1_UNPAIRED_ISO', 'HLT_noalg_L1HT190-J20s5.ETA49', 'HLT_noalg_L12EM3', 'HLT_e5_lhtight_e4_etcut_Jpsiee', 'HLT_e5_lhtight_nod0_e4_etcut', 'HLT_e5_tight_e4_etcut_Jpsiee', 'HLT_noalg_L12EM7', 'HLT_j35_lcw', 'HLT_noalg_mb_L1RD0_FILLED', 'HLT_4j25', 'HLT_mb_sptrk_noisesup', 'HLT_lumipeb_vdm_L1RD0_FILLED', 'HLT_6j25', 'HLT_mb_sptrk', 'HLT_j35_lcw_nojcalib', 'HLT_j25_j25_320eta490', 'HLT_j85_L1RD0', 'HLT_j55_L1RD0', 'HLT_mb_sptrk_costr', 'HLT_j60_L1RD0', 'HLT_ibllumi_L1RD0_FILLED', 'HLT_l1calocalib', 'HLT_j35_jes', 'HLT_j35_j35_320eta490', 'HLT_7j25', 'HLT_5j25', 'HLT_j35_nojcalib', 'HLT_j35_lcw_jes', 'HLT_j15_j15_320eta490', 'HLT_mu11_2mu4noL1_nscan03_L1MU11_2MU6', 'HLT_g20_etcut_L1EM12', 'HLT_e0_perf_L1EM15', 'HLT_g20_loose_L1EM15', 'HLT_g0_perf_L1EM15', 'HLT_mb_sptrk_L1RD0_EMPTY', 'HLT_j0_perf_bperf_L1RD0_EMPTY', 'HLT_noalg_mb_L1RD0_EMPTY', 'HLT_mb_sptrk_noisesup_L1RD0_EMPTY', 'HLT_mb_sptrk_costr_L1RD0_EMPTY', 'HLT_alfacalib_L1ALFA_A7R1_OD', 'HLT_mb_perf_L1LUCID_UNPAIRED_ISO', 'HLT_noalg_mb_L1LUCID_UNPAIRED_ISO', 'HLT_noalg_L1MJJ-4', 'HLT_noalg_L1MJJ-2', 'HLT_noalg_L1MJJ-3', 'HLT_noalg_L1MJJ-1', 'HLT_j110_bperf', 'HLT_j110', 'HLT_e24_lhmedium_idperf_L1EM20VH', 'HLT_e24_medium_idperf_L1EM20VH', 'HLT_noalg_L1BPH-4M8-2MU4', 'HLT_noalg_L1BPH-4M8-2MU6', 'HLT_noalg_mb_L1MBTS_2', 'HLT_mb_sptrk_pt4_L1MBTS_2', 'HLT_mb_mbts_L1MBTS_2', 'HLT_lumipeb_vdm_L1MBTS_2', 'HLT_mb_sptrk_pt6_L1MBTS_2', 'HLT_mb_perf_L1MBTS_2', 'HLT_mb_sptrk_pt8_L1MBTS_2', 'HLT_mb_sptrk_pt4_L1MBTS_1', 'HLT_lumipeb_vdm_L1MBTS_1', 'HLT_noalg_mb_L1MBTS_1', 'HLT_mb_mbts_L1MBTS_1', 'HLT_mb_sptrk_pt6_L1MBTS_1', 'HLT_mb_sptrk_pt8_L1MBTS_1', 'HLT_noalg_L12EM15', 'HLT_mb_perf_L1RD1_FILLED', 'HLT_noalg_mb_L1RD1_FILLED', 'HLT_noalg_L1MBTS_1_BGRP11', 'HLT_lumipeb_vdm_L1MBTS_1_BGRP11', 'HLT_e15_lhloose_L1EM13VH', 'HLT_e15_lhloose_cutd0dphideta_L1EM13VH', 'HLT_e15_loose_L1EM13VH', 'HLT_e15_lhloose_nod0_L1EM13VH', 'HLT_noalg_L1JPSI-1M5-EM12s', 'HLT_mu25_mucombTag_noEF_L1MU40', 'HLT_mu24_idperf', 'HLT_mu15_mucombTag_noEF_L1MU40', 'HLT_noalg_L1MU20', 'HLT_mu20_iloose_mu8noL1', 'HLT_mu20_iloose_2mu4noL1', 'HLT_mu20_imedium_mu8noL1', 'HLT_mu20_idperf', 'HLT_mu20_mucombTag_noEF_L1MU40', 'HLT_mu24_L2Star_idperf', 'HLT_noalg_mb_L1LUCID_EMPTY', 'HLT_mb_perf_L1LUCID_EMPTY', 'HLT_mu4_r1extr', 'HLT_mu4', 'HLT_mu0_perf', 'HLT_mu4_bJpsi_Trkloose', 'HLT_mu4_msonly', 'HLT_mu6_msonly', 'HLT_mu6', 'HLT_mu6_bJpsi_Trkloose', 'HLT_mb_sp300_trk10_sumet80_hmt_L1MBTS_1_1', 'HLT_mb_sp600_trk10_sumet70_hmt_L1MBTS_1_1', 'HLT_mb_sptrk_pt8_L1MBTS_1_1', 'HLT_mb_sp400_trk40_hmt_L1MBTS_1_1', 'HLT_mb_mbts_L1MBTS_1_1', 'HLT_mb_sp600_trk10_sumet50_hmt_L1MBTS_1_1', 'HLT_mb_sptrk_pt6_L1MBTS_1_1', 'HLT_mb_sp600_trk10_sumet60_hmt_L1MBTS_1_1', 'HLT_mb_sp1400_trk80_hmt_L1MBTS_1_1', 'HLT_noalg_mb_L1MBTS_1_1', 'HLT_mb_sp600_trk10_sumet40_hmt_L1MBTS_1_1', 'HLT_mb_sp900_trk60_hmt_L1MBTS_1_1', 'HLT_mb_sp300_trk10_sumet40_hmt_L1MBTS_1_1', 'HLT_te50_L1MBTS_1_1', 'HLT_mb_sp600_trk45_hmt_L1MBTS_1_1', 'HLT_mb_sp600_trk10_sumet80_hmt_L1MBTS_1_1', 'HLT_mb_sp900_trk65_hmt_L1MBTS_1_1', 'HLT_mb_sp300_trk10_sumet50_hmt_L1MBTS_1_1', 'HLT_mb_sp700_trk55_hmt_L1MBTS_1_1', 'HLT_mb_sp1200_trk75_hmt_L1MBTS_1_1', 'HLT_mb_sp2_hmtperf_L1MBTS_1_1', 'HLT_mb_sp700_trk50_hmt_L1MBTS_1_1', 'HLT_mb_sp1000_trk70_hmt_L1MBTS_1_1', 'HLT_mb_sptrk_pt4_L1MBTS_1_1', 'HLT_mb_sp300_trk10_sumet70_hmt_L1MBTS_1_1', 'HLT_mb_sp300_trk10_sumet60_hmt_L1MBTS_1_1', 'HLT_lumipeb_vdm_L1MBTS_2_BGRP9', 'HLT_noalg_L1MBTS_2_BGRP9', 'HLT_noalg_L1BPH-2M-MU6MU4', 'HLT_noalg_L1DR-TAU20ITAU12I', 'HLT_noalg_cosmiccalo_L1J12_FIRSTEMPTY', 'HLT_beamspot_allTE_trkfast_peb', 'HLT_beamspot_allTE_L2StarB_peb', 'HLT_5j45', 'HLT_5j55', 'HLT_beamspot_activeTE_trkfast_peb', 'HLT_beamspot_activeTE_L2StarB_peb', 'HLT_tau1_cosmic_ptonly_L1MU4_EMPTY', 'HLT_noalg_cosmicmuons_L1MU4_EMPTY', 'HLT_mu4_msonly_cosmic_L1MU4_EMPTY', 'HLT_tau1_cosmic_track_L1MU4_EMPTY', 'HLT_mu4_cosmic_L1MU4_EMPTY', 'HLT_j150', 'HLT_alfacalib_L1LHCF_ALFA_ANY_A', 'HLT_lhcfpeb_L1LHCF_ALFA_ANY_A', 'HLT_noalg_L1DY-BOX-MU6MU4', 'HLT_noalg_L1BPH-2M-2MU6', 'HLT_noalg_L1BPH-2M-2MU4', 'HLT_noalg_L1RD0_BGRP11', 'HLT_lumipeb_vdm_L1RD0_BGRP11', 'HLT_noalg_L1DY-DR-2MU4', 'HLT_mb_sp300_trk10_sumet60_hmt_L1TE20', 'HLT_mb_sp1400_trk100_hmt_L1TE20', 'HLT_mb_sp2500_hmtperf_L1TE20', 'HLT_te20_tc_lcw', 'HLT_mb_sp1400_hmtperf_L1TE20', 'HLT_noalg_mb_L1TE20',
    ]

Disabled_Rules=dict(map(None,HLT_Disabled,len(HLT_Disabled)*[{
                1000 : TriggerRule(PS=-1, comment=''), 
                }]))


########################################
RulesList=[L1_Rules, L1_jetCP, L1Streamer_Rules, L1_lowlumi_Rules, HLT_Rules, HLT_jetCP, Rules_Prescaled, Monitoring_pp_v5_rules.physics_rules, Disabled_Rules, zerobias_Rules, beamspot_Rules, tauoverlay_Rules]
if doEB:
    RulesList += [Physics_pp_v5_EB_rules.rules ]
if doafterGlow:
    RulesList += [afterGlow_Rules]
    #print afterGlow_Rules.keys()
for Rules in RulesList:
    for newrule in Rules.keys():
        if rules.has_key(newrule):
            print "FATAL: duplicated rule cannot be added.", newrule

    rules.update(Rules)

           

