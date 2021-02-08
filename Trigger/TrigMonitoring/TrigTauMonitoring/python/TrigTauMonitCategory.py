# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags import TriggerFlags


# Lowest-energy single tau monitored at HLT
monitoring_singleTau = 'tau25_medium1_tracktwo'

# Trigger lists that are used by default, unless they are overwritten based on the running mode. 
# This allows for the trigger lists to be updated based on the running mode.
monitoring_tau = [
    'tau0_perf_ptonly_L1TAU12',
    'tau0_perf_ptonly_L1TAU100', #TES change. 'tau0_perf_ptonly_L1TAU60',
    'tau0_idperf_tracktwoMVA_L1eTAU12',
    'tau0_idperf_tracktwoMVA_L1eTAU5',
    #'tau25_idperf_track',
    'tau25_idperf_tracktwo',
    'tau25_perf_tracktwo',
    'tau25_medium1_tracktwo',
    'tau35_perf_tracktwo_tau25_perf_tracktwo',
    'tau35_medium1_tracktwo_tau25_medium1_tracktwo',
    'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM',
    'mu14_iloose_tau25_perf_tracktwo',
    'tau160_idperf_tracktwo_L1TAU100', # TES change. 'tau160_idperf_tracktwo',
    'tau160_perf_tracktwo_L1TAU100', # TES change. 'tau160_perf_tracktwo',
    'tau160_medium1_tracktwo_L1TAU100', # TES change. 'tau160_medium1_tracktwo',
    'tau1_cosmic_track_L1MU4_EMPTY',
    'tau1_cosmic_ptonly_L1MU4_EMPTY',
    'tau8_cosmic_ptonly',
    'tau8_cosmic_track',
    # new
    'tau25_idperf_tracktwoMVA_L1eTAU12',
    'tau25_mediumRNN_tracktwoMVA_L1eTAU12',
    'tau160_idperf_tracktwoMVA_L1eTAU100',
    'tau160_medium1_tracktwoEF_L1eTAU100',
    'tau35_medium1_tracktwoEF_tau25_medium1_tracktwoEF_L1TAU20IM_2TAU12IM_4J12.0ETA25',
]

# Trigger lists that are dependent on the running mode.
monitoring_tau_pp = [
    'tau0_perf_ptonly_L1TAU12',
    'tau0_perf_ptonly_L1TAU100',
    'tau0_idperf_tracktwoMVA_L1eTAU12',
    'tau0_idperf_tracktwoMVA_L1eTAU5',
    #'tau25_idperf_track',
    'tau25_idperf_tracktwo',
    'tau25_perf_tracktwo',
    'tau25_idperf_tracktwoMVABDT',
    'tau25_perf_tracktwoMVABDT',
    'tau25_mediumRNN_tracktwoMVABDT',
    'tau25_idperf_tracktwoRNN',
    'tau25_perf_tracktwoRNN',
    'tau25_mediumRNN_tracktwoRNN',
    'tau25_medium1_tracktwo',
    'tau25_idperf_tracktwoMVA',
    'tau25_perf_tracktwoMVA',
    'tau25_looseRNN_tracktwoMVA',
    'tau25_mediumRNN_tracktwoMVA',
    'tau25_idperf_tracktwoEF',
    'tau25_perf_tracktwoEF',
    'tau25_medium1_tracktwoEF',
    'tau25_idperf_tracktwoEFmvaTES',
    'tau25_perf_tracktwoEFmvaTES',
    'tau25_medium1NoPt_tracktwoEFmvaTES',
    'tau25_medium1_tracktwoEFmvaTES',
    'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM',  #keep this as reference for the L1Topo plots
    'tau35_medium1_tracktwo_xe70_L1XE45', # tau+xe
    'tau35_medium1_L1TAU20_tau25_medium1_L1TAU12_xe50', # 2tau+xe
    #'tau40_medium1_tracktwo_tau35_medium1_tracktwo', # tau+mu, high ET
    'tau80_medium1_tracktwo_L1TAU60_tau50_medium1_tracktwo_L1TAU12', # 2tau 
    'tau80_medium1_tracktwo_L1TAU60_tau60_medium1_tracktwo_L1TAU40', # 2tau 
    'tau160_idperf_tracktwo_L1TAU100', # TES change. 'tau160_idperf_tracktwo',
    'tau160_perf_tracktwo_L1TAU100', # TES change. 'tau160_perf_tracktwo',
    'tau160_medium1_tracktwo_L1TAU100', # 1tau
    #'mu14_ivarloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25', # tau+mu L1TOPO
    #'mu14_ivarloose_tau25_medium1_tracktwo_L1MU10_TAU20IM_J25_2J20', # tau+mu non-L1TOPO
    #'mu14_ivarloose_tau35_medium1_tracktwo', # tau+mu, high ET
    #'mu14_ivarloose_tau25_medium1_tracktwo_xe50', # tau+mu+xe
    #'e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25', # tau+e+J L1TOPO
    #'e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo', # tau+e+J non-L1TOPO
    #'e24_lhmedium_nod0_ivarloose_tau35_medium1_tracktwo', # tau+e, high ET
    #'e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_xe50' # tau+e+xe
    # new
    'tau25_idperf_tracktwoMVA_L1eTAU12',
    'tau25_mediumRNN_tracktwoMVA_L1eTAU12',
    'tau160_idperf_tracktwoMVA_L1eTAU100',
    'tau160_medium1_tracktwoEF_L1eTAU100',
    'tau35_medium1_tracktwoEF_tau25_medium1_tracktwoEF_L1TAU20IM_2TAU12IM_4J12.0ETA25',

]
# removed monitoring_tau_pp chains:
#    'tau0_perf_ptonly_L1TAU60',
#    'mu14_iloose_tau25_perf_tracktwo',   # need to move to the ivarloose chain
#    'tau160_idperf_track', # no longer used
# moved to tau_topo_chains
    #'tau35_medium1_tracktwo_tau25_medium1_tracktwo', # 2tau non-L1TOPO    -- moved to tau_topo_chains
    #'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I-J25', # 2tau L1TOPO+J    -- moved to tau_topo_chains
    #'tau35_medium1_tracktwo_tau25_medium1_tracktwo_03dR30_L1DR-TAU20ITAU12I-J25', # 2tau L1TOPO+J    -- moved to tau_topo_chains
    #'tau35_medium1_L1TAU20_tau25_medium1_L1TAU12_xe50', # 2tau+xe    -- moved to tau_topo_chains
    #'tau80_medium1_tracktwo_L1TAU60_tau50_medium1_tracktwo_L1TAU12', # 2tau    -- moved to tau_topo_chains
    #'tau80_medium1_tracktwo_L1TAU60_tau60_medium1_tracktwo_L1TAU40', # 2tau    -- moved to tau_topo_chains
    #'tau80_medium1_tracktwo_L1TAU60_tau35_medium1_tracktwo_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I', # 2tau    -- moved to tau_topo_chains L1TOPO

# Trigger lists that are used for MC validation.
monitoring_tau_validation = [
    'tau0_perf_ptonly_L1TAU12',
    'tau0_perf_ptonly_L1TAU100', # 'tau0_perf_ptonly_L1TAU60',
    'tau0_idperf_tracktwoMVA_L1eTAU12',
    'tau0_idperf_tracktwoMVA_L1eTAU5',
    #'tau25_idperf_track',
    'tau25_perf_ptonly',
    'tau25_loose1_ptonly',
    'tau25_medium1_ptonly',
    'tau25_tight1_ptonly',
    'tau25_idperf_tracktwo',
    'tau25_perf_tracktwo',
    'tau25_medium1_tracktwo',
    'tau25_loose1_tracktwo',
    'tau25_tight1_tracktwo',
    'tau25_medium1_tracktwo_L1TAU12',
    'tau25_medium1_tracktwo_L1TAU12IL',
    'tau25_medium1_tracktwo_L1TAU12IT',
    'tau25_idperf_tracktwoMVA',
    'tau25_perf_tracktwoMVA',
    'tau25_looseRNN_tracktwoMVA',
    'tau25_mediumRNN_tracktwoMVA',
    'tau25_idperf_tracktwoEF',
    'tau25_perf_tracktwoEF',
    'tau25_medium1_tracktwoEF',
    'tau25_idperf_tracktwoEFmvaTES',
    'tau25_perf_tracktwoEFmvaTES',
    'tau25_medium1NoPt_tracktwoEFmvaTES',
    'tau25_medium1_tracktwoEFmvaTES',
    'tau35_perf_tracktwo',
    'tau35_perf_ptonly',
    'tau35_medium1_tracktwo_xe70_L1XE45', # tau+xe
    'tau40_medium1_tracktwo_tau35_medium1_tracktwo', # tau+mu, high ET
    'tau50_medium1_tracktwo_L1TAU12',
    'tau80_medium1_tracktwo',
    'tau80_medium1_tracktwo_L1TAU60',
    'tau125_medium1_tracktwo',
    'tau125_perf_tracktwo',
    'tau125_perf_ptonly',
    'tau160_idperf_tracktwo_L1TAU100', # 'tau160_idperf_tracktwo',
    'tau160_perf_tracktwo_L1TAU100', # 'tau160_perf_tracktwo',
    'tau160_medium1_tracktwo',
    'tau160_medium1_tracktwo_L1TAU100', # 1tau
    'mu14_iloose_tau25_perf_tracktwo',
    'mu14_ivarloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25', # tau+mu L1TOPO
    'mu14_ivarloose_tau25_medium1_tracktwo_L1MU10_TAU20IM_J25_2J20', # tau+mu non-L1TOPO
    'mu14_ivarloose_tau35_medium1_tracktwo', # tau+mu, high ET
    'mu14_ivarloose_tau25_medium1_tracktwo_xe50', # tau+mu+x
    'mu14_ivarloose_tau25_medium1_tracktwo_03dR30',
    'mu14_ivarloose_tau25_medium1_tracktwo_03dR30_L1DR-MU10TAU12I_TAU12I-J25',
    'e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25', # tau+e+J L1TOPO
    'e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo', # tau+e+J non-L1TOPO
    'e24_lhmedium_nod0_ivarloose_tau35_medium1_tracktwo', # tau+e, high ET
    'e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_xe50', # tau+e+xe
    'e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_03dR30',
    'e17_lhmedium_nod0_tau25_medium1_tracktwo_03dR30_L1DR-EM15TAU12I-J25',
    # new
    'tau25_idperf_tracktwoMVA_L1eTAU12',
    'tau25_mediumRNN_tracktwoMVA_L1eTAU12',
    'tau160_idperf_tracktwoMVA_L1eTAU100',
    'tau160_medium1_tracktwoEF_L1eTAU100',
    'tau35_medium1_tracktwoEF_tau25_medium1_tracktwoEF_L1TAU20IM_2TAU12IM_4J12.0ETA25',

]
    #'tau35_medium1_tracktwo_tau25_medium1_tracktwo', # 2tau non-L1TOPO    -- moved to tau_topo_chains
    #'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I-J25', # 2tau L1TOPO+J -- moved to tau_topo_chains
    #'tau35_medium1_tracktwo_tau25_medium1_tracktwo_03dR30_L1DR-TAU20ITAU12I-J25', # 2tau L1TOPO+J -- moved to tau_topo_chains
    #'tau35_medium1_L1TAU20_tau25_medium1_L1TAU12_xe50', # 2tau+xe    -- moved to tau_topo_chains
    #'tau80_medium1_tracktwo_L1TAU60_tau50_medium1_tracktwo_L1TAU12', # 2tau    -- moved to tau_topo_chains
    #'tau80_medium1_tracktwo_L1TAU60_tau60_medium1_tracktwo_L1TAU40', # 2tau    -- moved to tau_topo_chains
    #'tau80_medium1_tracktwo_L1TAU60_tau35_medium1_tracktwo_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I', # 2tau L1TOPO    -- moved to tau_topo_chains

# Trigger lists that are used for cosmic runs.
monitoring_tau_cosmic = [
    'tau1_cosmic_track_L1MU4_EMPTY',
    'tau1_cosmic_ptonly_L1MU4_EMPTY',
    'tau8_cosmic_ptonly',
    'tau8_cosmic_track'
]

# Lowest-energy single tau monitored for cosmic runs.
monitoring_singleTau_cosmic = 'tau8_cosmic_track'
