# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# old trigger list will be kept for reference and comparison purposes
xeTrig    = '(HLT_xe50_pueta || HLT_xe100|| HLT_xe70_pueta || HLT_xe100_pueta || HLT_xe70 || HLT_xe50 || HLT_xe35 || HLT_xe35_pueta )'

jetXeTrig = '(HLT_j80_xe80 || HLT_j100_xe80)'

tauTrigData = '(HLT_tau125_medium1_tracktwo || HLT_tau125_perf_tracktwo || HLT_tau125_perf_ptonly || HLT_tau25_loose1_tracktwo || HLT_tau25_medium1_tracktwo || HLT_tau25_tight1_tracktwo || HLT_tau25_loose1_ptonly || HLT_tau25_medium1_ptonly || HLT_tau25_medium1_tracktwo_L1TAU12 || HLT_tau25_medium1_tracktwo_L1TAU12IL || HLT_tau25_medium1_tracktwo_L1TAU12IT || HLT_tau35_medium1_tracktwo || HLT_tau35_medium1_ptonly || HLT_tau35_medium1_ptonly_L1TAU20 || HLT_tau35_medium1_tracktwo_L1TAU20 || HLT_tau35_medium1_tracktwo_L1TAU20IL || HLT_tau35_medium1_tracktwo_L1TAU20IT || HLT_tau80_medium1_tracktwo || HLT_tau80_medium1_tracktwo_L1TAU60 || HLT_tau160_medium1_tracktwo || HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo || HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM || HLT_tau35_loose1_tracktwo_tau25_loose1_tracktwo || HLT_tau35_tight1_tracktwo_tau25_tight1_tracktwo || HLT_tau35_loose1_tracktwo_tau25_loose1_tracktwo_L1TAU20IM_2TAU12IM || HLT_tau35_tight1_tracktwo_tau25_tight1_tracktwo_L1TAU20IM_2TAU12IM || HLT_tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12 || HLT_tau35_medium1_tracktwo_xe70_L1XE45 || HLT_tau35_medium1_tracktwo_L1TAU20_xe70_L1XE45 || HLT_tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_xe50 || HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_xe50)'

tauMuTrigData = '(HLT_mu14_iloose_tau25_perf_tracktwo || HLT_mu14_iloose_tau25_medium1_tracktwo || HLT_mu14_iloose_tau35_medium1_tracktwo || HLT_mu14_tau35_medium1_tracktwo || HLT_mu14_tau25_medium1_tracktwo || HLT_mu14_tau35_medium1_tracktwo_L1TAU20 || HLT_mu14_tau25_medium1_tracktwo_xe50 || HLT_mu14_iloose_tau25_medium1_tracktwo_xe50)'

tauETrigData = '(HLT_e17_medium_tau25_medium1_tracktwo || HLT_e17_lhmedium_tau25_medium1_tracktwo || HLT_e17_medium_tau80_medium1_tracktwo || HLT_e17_lhmedium_tau80_medium1_tracktwo || HLT_e17_medium_tau25_medium1_tracktwo_xe50 || HLT_e17_lhmedium_tau25_medium1_tracktwo_xe50 || HLT_e17_medium_iloose_tau25_medium1_tracktwo_xe50 || HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo_xe50)'

tauTrigMC = '(HLT_tau125_medium1_calo || HLT_tau125_medium1_track || HLT_tau125_r1medium1 || HLT_tau125_r1perf || HLT_tau25_medium1_calo || HLT_tau25_medium1_trackcalo || HLT_tau25_medium1_tracktwocalo || HLT_tau25_tight1_ptonly || HLT_tau25_medium1_mvonly || HLT_tau25_medium1_track || HLT_tau35_loose1_tracktwo || HLT_tau35_loose1_ptonly || HLT_tau35_medium1_track || HLT_tau35_medium1_calo || HLT_tau35_tight1_tracktwo || HLT_tau35_tight1_ptonly || HLT_tau80_medium1_calo || HLT_tau80_medium1_track)'
# the following are not in the PeriodA menu, so we will not bother activating them
# || HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I-J25 || HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I || HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20ITAU12I-J25 || HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1BOX-TAU20ITAU12I )'

# the following are not in the PeriodA menu, so we will not bother activating them
tauETrigMC = '(0)' # (HLT_e17_medium_tau80_medium1_tracktwo_L1EM15-TAU40 || HLT_e17_medium_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I || HLT_e17_lhmedium_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I || HLT_e17_lhmedium_tau80_medium1_tracktwo_L1EM15-TAU40 || HLT_e17_medium_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I || e17_lhmedium_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I || HLT_e17_medium_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I || e17_lhmedium_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I || HLT_e17_medium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25 || HLT_e17_lhmedium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25 || HLT_e17_medium_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25 || HLT_e17_lhmedium_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25 || HLT_e17_medium_tau25_medium1_tracktwo_L1EM15TAU12I-J25 || HLT_e17_lhmedium_tau25_medium1_tracktwo_L1EM15TAU12I-J25 || HLT_e17_medium_tau25_medium1_tracktwo_L1EM15-TAU12I || HLT_e17_lhmedium_tau25_medium1_tracktwo_L1EM15-TAU12I)'

# the following are not in the PeriodA menu, so we will not bother activating them
tauMuTrigMC = '(0)' # (HLT_mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12-J25 || HLT_mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I || HLT_mu14_tau25_medium1_tracktwo_L1MU10_TAU12I-J25 )'

# dont exist anymore:
# HLT_e18_loose1_tau80_medium1_ptonly || HLT_e18_loose1_tau25_medium1_ptonly || HLT_e18_lhloose_tau80_medium1_calo || HLT_mu14_tau35_medium1_ptonly || HLT_mu14_tau25_medium1_ptonly || HLT_mu14_tau35_medium1_calo || HLT_tau25_medium1_trackonly ||  HLT_tau35_medium1_calo_tau25_medium1_calo || HLT_tau29_r1medium1 || HLT_tau35_medium1_calo_xe70_L1XE45 || HLT_tau25_medium1_mvonly_L1TAU6 || HLT_tau35_medium1_ptonly_xe70_L1XE45   || HLT_tau35_medium1_calo_tau25_medium1_calo_xe50 || HLT_tau35_medium1_ptonly_tau25_medium1_ptonly || HLT_tau35_medium1_ptonly_tau25_medium1_ptonly_xe50 || HLT_tau25_medium1_caloonly
tauTrig   = '(0)'

allTriggers = [
  xeTrig, jetXeTrig, tauTrig,
  tauETrigData, tauMuTrigData, tauTrigData,
  tauETrigMC  , tauMuTrigMC  , tauTrigMC,
]

triggerSelection = ' || '.join(allTriggers)

#================================
# new trigger regular expressions
#================================

triggerRegEx = [
  'HLT.*_tau[0-9][0-9].*_medium.*', 'HLT.*_tau[0-9][0-9].*_tight.*', 'HLT.*_tau[0-9][0-9].*_loose.*', 'HLT.*_j.*_xe.*', 'HLT_xe[0-9][0-9].*',
]
triggerRegExThinning = triggerRegEx + [
  'HLT_e.*',   # need single-electron triggers?
  'HLT_mu.*'
]
