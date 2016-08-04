# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Categories currently used by Egamma TO monitoring tool
primary_single_ele = ['HLT_e24_lhtight_nod0','HLT_e26_lhtight_nod0','HLT_e60_lhmedium_nod0','HLT_e120_lhloose_nod0','HLT_e140_lhloose_nod0']
primary_single_ele_iso =['HLT_e24_lhtight_nod0_ivarloose','HLT_e26_lhtight_nod0_ivarloose']
monitoring_Zee = ['HLT_e26_lhtight_nod0_e15_etcut_Zee']
monitoring_Jpsiee = ['HLT_e5_lhtight_nod0_e4_etcut','HLT_e5_lhtight_nod0_e4_etcut_Jpsiee']
primary_single_pho = ['HLT_g120_loose','g140_loose']

#Other categories
primary_double_ele = ['HLT_2e17_lhvloose_nod0']
monitoring_ele_idperf = ['HLT_e26_lhtight_idperf']
primary_double_pho = ['HLT_g35_loose_g25_loose','HLT_g35_medium_g25_medium','HLT_2g20_tight','HLT_2g22_tight']

#Old categories
primary_single_ele_cutbased = []
primary_single_ele_cutbased_iso = []
primary_double_ele_cutbased = []
monitoring_ele_idperf_cutbased = []
# Create a single list for other groups
monitoring_egamma = primary_single_ele + primary_double_ele + monitoring_ele_idperf + monitoring_Zee + monitoring_Jpsiee + primary_single_pho + primary_double_pho

#Additional trigger-only lists for monitoring tools that will evolve with lumi

# L1 trigger items to monitor from inclusive offline electrons
monitoring_L1Calo = ['L1_EM22VHI','L1_EM24VHI']
# Startup and high-pt electrons to monitor from inclusive electrons
monitoring_electron = ['HLT_e60_lhmedium_nod0','HLT_e140_lhloose_nod0']
# 2016 single electrons triggers to monitor from Zee TP, includes rerun
monitoringTP_electron = ['HLT_e17_lhvloose_nod0','HLT_e26_lhtight_nod0','HLT_e26_lhtight_nod0_ivarloose',
    'HLT_e28_lhtight_nod0_ivarloose','HLT_e28_lhtight_smooth_ivarloose','HLT_e28_lhtight_nod0_ringer_ivarloose',
    'HLT_e26_lhtight_cutd0detadphi_ivarloose']
monitoringTP_Jpsiee = ["HLT_e5_lhtight_nod0"]    
monitoring_photon = ['HLT_g25_medium','HLT_g140_loose']
#Add the Menu-Aware monitoring; maps a trigger to a category
monitoring_mam = {'primary_single_ele':'HLT_e26_lhtight_nod0',
    'primary_single_ele_iso':'HLT_e26_lhtight_nod0_ivarloose',
    'primary_double_pho':'HLT_g25_medium',
    'primary_single_pho':'HLT_g140_loose',
    'monitoring_Jpsiee' : 'HLT_e5_lhtight_nod0'}
    #'monitoring_Zee':'HLT_e26_lhtight_nod0_e15_etcut_Zee',

# Cosmic items
monitoring_electron_cosmic=['HLT_e0_perf_L1EM3_EMPTY']
monitoring_photon_cosmic=['HLT_g0_perf_L1EM3_EMPTY']
monitoring_mam_cosmic = {'primary_single_ele':'HLT_e0_perf_L1EM3_EMPTY',
    'primary_single_pho':'HLT_g0_perf_L1EM3_EMPTY'}

