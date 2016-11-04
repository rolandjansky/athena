# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Categories currently used by offline Egamma TO monitoring tool
#Important to keep lists updated
primary_single_ele = ['HLT_e22_lhloose_nod0','HLT_e7_etcut','HLT_e15_etcut','HLT_e15_lhloose_nod0','HLT_e15_loose','HLT_e17_lhloose_nod0','HLT_e17_loose']
primary_single_ele_iso =['HLT_e24_lhtight_nod0_ivarloose','HLT_e26_lhtight_nod0_ivarloose']
monitoring_Zee = ['HLT_e26_lhtight_nod0_e15_etcut_Zee']
monitoring_Jpsiee = ['HLT_e5_lhtight_nod0_e4_etcut','HLT_e5_lhtight_nod0_e4_etcut_Jpsiee']
primary_single_pho = ['HLT_g13_etcut','HLT_g18_etcut','HLT_g15_loose','HLT_g20_loose','HLT_g35_loose','HLT_g60_loose']

#Other categories
primary_double_ele = ['HLT_2e7_etcut','HLT_2e10_etcut','HLT_e7_lhloose_nod0','HLT_2e7_loose']
monitoring_ele_idperf = ['HLT_e15_lhloose_idperf']
primary_double_pho = ['HLT_2g10_loose']

#Old categories
primary_single_ele_cutbased = []
primary_single_ele_cutbased_iso = []
primary_double_ele_cutbased = []
monitoring_ele_idperf_cutbased = []
# Create a single list for other groups
monitoring_egamma = primary_single_ele + primary_double_ele + monitoring_ele_idperf + monitoring_Zee + monitoring_Jpsiee + primary_single_pho + primary_double_pho

# The following is our trigger T0 monitoring configuration
# Additional trigger-only lists for monitoring tools that will evolve with lumi
#
# Maintain lists for different running modes, e.g
# pp 'collisions'
# Heavy Ion 'heavyioncollisions'
# cosmics 'cosmics
# MC 'monteCarlo'

# Define the set of histograms to be monitored by shifters
plots_efficiency = ["eff_et","eff_eta","eff_mu"]
plots_distribution = ["et","eta","Reta","Rphi","Rhad","f1","f3","eratio","deta2","eprobHT","npixhits","nscthits","ptvarcone20"]
plots_resolution = ["res_et","res_Rphi","res_Reta","res_Rhad","res_ptvarcone20","res_deta2"]

# Define triggers for tagging 
monitoring_tags = ['HLT_e22_lhloose_nod0','HLT_e22_lhloose']

monitoring_jpsitags = ['HLT_e5_lhtight_nod0_e4_etcut','HLT_e9_lhtight_nod0_e4_etcut',
    'HLT_e14_lhtight_nod0_e4_etcut',
    'HLT_e5_lhtight_nod0_e4_etcut_Jpsiee',
    'HLT_e9_lhtight_nod0_e4_etcut_Jpsiee']
                        
# pp 'collisions' monitoring configuration, default
# L1 trigger items to monitor from inclusive offline electrons
monitoring_L1Calo = ['L1_EM22VHI','L1_EM24VHI']
# Startup and high-pt electrons to monitor from inclusive electrons
monitoring_electron = ['HLT_e60_lhmedium_nod0','HLT_e140_lhloose_nod0']
# 2016 single electrons triggers to monitor from Zee TP, includes rerun
monitoringTP_electron = ['HLT_e17_lhvloose_nod0','HLT_e26_lhtight_nod0_ivarloose',
    'HLT_e28_lhtight_nod0_ivarloose','HLT_e28_lhtight_smooth_ivarloose','HLT_e28_lhtight_nod0_ringer_ivarloose',
    'HLT_e26_lhtight_cutd0detadphi_ivarloose']
monitoringTP_Jpsiee = ["HLT_e5_lhtight_nod0"]    
monitoring_photon = ['HLT_g25_medium','HLT_g140_loose']
#Add the Menu-Aware monitoring; maps a trigger to a category
monitoring_mam = {'primary_single_ele_iso':'HLT_e26_lhtight_nod0_ivarloose',
    'primary_double_pho':'HLT_g25_medium',
    'primary_single_pho':'HLT_g140_loose',
    'monitoring_Jpsiee' : 'HLT_e5_lhtight_nod0'}
    #'monitoring_Zee':'HLT_e26_lhtight_nod0_e15_etcut_Zee',

# Cosmic items
monitoring_electron_cosmic=['HLT_e0_perf_L1EM3_EMPTY']
monitoring_photon_cosmic=['HLT_g0_perf_L1EM3_EMPTY']
monitoring_mam_cosmic = {'primary_single_ele':'HLT_e0_perf_L1EM3_EMPTY',
    'primary_single_pho':'HLT_g0_perf_L1EM3_EMPTY'}

# Validation Items for MC
validation_electron=monitoring_electron
validationTP_electron = ['HLT_e0_perf_L1EM15']
validationTP_electron.extend(monitoringTP_electron)
validation_photon = ['HLT_g0_perf_L1EM15','HLT_g35_medium','HLT_g25_loose','HLT_g35_loose','HLT_g45_tight','HLT_g120_loose']
validation_photon.extend(monitoring_photon)
validation_mam=monitoring_mam

# Heavy Ion items
monitoring_electron_hi=['HLT_e15_etcut','HLT_e15_lhloose','HLT_e17_lhloose']
monitoringTP_electron_hi=['HLT_e15_lhloose_idperf','HLT_e15_loose','HLT_e15_lhloose_nod0','HLT_e17_lhloose_nod0','HLT_e22_lhloose_nod0']
monitoring_photon_hi=['HLT_g13_etcut','HLT_g18_etcut','HLT_g15_loose','HLT_g20_loose','HLT_g35_loose','HLT_g60_loose']
monitoring_mam_hi = {'primary_single_ele_cutbased':'HLT_e15_loose',
        'primary_single_ele':'HLT_e15_lhloose_nod0',
        'primary_single_pho':'HLT_g15_loose'}

