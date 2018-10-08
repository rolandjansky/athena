# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from SUSYCommonTriggerList import *

# start with common list for MET triggers
triggersMET = MET_2015 + MET_2016 + MET_2017 + MET_2018

triggersMET += [
# these were unprescaled in 2015, in addition to the primary HLT_xe70_mht from
# the common list
'HLT_xe70',
'HLT_xe70_tc_lcw',
# useful for cross-checks
'HLT_noalg_L1J400',
'HLT_noalg_L1J420',
'HLT_noalg_L1J450',
]

# our dedicated soft lepton + MET + jet triggers
# SUSY16.py makes use of this for skimming, so let's keep the name as 
# triggersSoftMuon despite e.g. the electron chains included here
triggersSoftMuon = [
# 2016
'HLT_2mu4_j85_xe50_mht',
'HLT_mu4_j125_xe90_mht',
# 2017
'HLT_2mu4_invm1_j20_xe40_pufit_2dphi10_L12MU4_J20_XE30_DPHI-J20s2XE30',
'HLT_2mu4_invm1_j20_xe60_pufit_2dphi10_L12MU4_J20_XE30_DPHI-J20s2XE30',
'HLT_2mu4_invm1_j20_xe60_pufit_2dphi10_L12MU4_J20_XE40_DPHI-J20s2XE30',
'HLT_2mu4_invm1_j20_xe60_pufit_2dphi10_L12MU4_J40_XE50',
'HLT_2mu4_invm1_j20_xe80_pufit_2dphi10_L12MU4_J40_XE50',
'HLT_mu4_j90_xe90_pufit_2dphi10_L1MU4_J50_XE50_DPHI-J20s2XE30',
'HLT_mu4_j90_xe90_pufit_2dphi10_L1MU4_XE60',
'HLT_mu4_j80_xe80_pufit_2dphi10_L1MU4_XE60',
'HLT_2e5_lhvloose_nod0_j40_xe70_pufit_2dphi10_L1J40_XE50_DPHI-J20s2XE50',
'HLT_2e5_lhvloose_nod0_j40_xe70_pufit_2dphi10_L1XE60',
'HLT_2e5_lhmedium_nod0_j40_xe80_pufit_2dphi10_L1XE60',
'HLT_2e5_lhloose_nod0_j40_xe70_pufit_2dphi10_L1J40_XE50_DPHI-J20s2XE50',
'HLT_e5_lhvloose_nod0_mu4_j30_xe40_pufit_2dphi10_L1MU4_J30_XE40_DPHI-J20s2XE30',
'HLT_e5_lhloose_nod0_j50_xe70_pufit_2dphi10_L1J40_XE50_DPHI-J20s2XE50',
'HLT_e5_lhloose_nod0_mu4_j30_xe40_pufit_2dphi10_L1MU4_J30_XE40_DPHI-J20s2XE30',
'HLT_e5_lhmedium_nod0_j50_xe80_pufit_2dphi10_L1J40_XE50_DPHI-J20s2XE50',
'HLT_e5_lhmedium_nod0_mu4_j30_xe65_pufit_2dphi10_L1MU4_XE60',
'HLT_e5_lhloose_nod0_j40_xe70_pufit_2dphi10_L1XE60',
'HLT_e5_lhmedium_nod0_j40_xe80_pufit_2dphi10_L1XE60',
'HLT_e5_lhmedium_nod0_j50_xe90_pufit_2dphi10_L1J40_XE50_DPHI-J20s2XE50',
# MC16
'HLT_mu4_j100_xe60_mht_L1MU4_J20_XE30_DPHI-J20s2XE30',
'HLT_mu4_xe60_mht_L1MU4_J20_XE30_DPHI-J20s2XE30',
'HLT_2mu4_xe40_mht_L12MU4_J20_XE30_DPHI-J20s2XE30',
'HLT_e5_lhmedium_nod0_mu4_xe40_mht_L1MU4_J20_XE30_DPHI-J20s2XE30',
'HLT_2e5_lhmedium_nod0_j50_xe80_mht_L1J40_XE50_DPHI-J20s2XE50',
]

triggersSoftMuonEmulation = [
'HLT_2mu4',
'HLT_mu4',
'HLT_e5_lhloose_nod0',
'HLT_e5_lhvloose_nod0',
'HLT_xe35',
'HLT_xe50',
'HLT_xe80',
'HLT_xe35_mht',
'HLT_xe50_mht',
'HLT_xe80_mht',
'HLT_xe90_mht_L1XE50',
'HLT_j60',
'HLT_j85',
'HLT_j100',
'HLT_j110',
'HLT_j125',
]

triggersJetPlusMet = [
'HLT_j80_xe80',
'HLT_j100_xe80',
'HLT_j80_xe80_1dphi10_L1J40_DPHI-J20s2XE50',
'HLT_j80_xe80_1dphi10_L1J40_DPHI-J20XE50',
'HLT_j80_xe80_dphi10_L1J40_DPHI-Js2XE50',
'HLT_j80_xe80_1dphi10_L1J40_DPHI-CJ20XE50',
'HLT_j100_xe80_dphi10_L1J40_DPHI-J20s2XE50',
'HLT_j100_xe80_dphi10_L1J40_DPHI-J20XE50',
'HLT_j100_xe80_dphi10_L1J40_DPHI-Js2XE50',
'HLT_j100_xe80_dphi10_L1J40_DPHI-CJ20XE50',
'HLT_j100_xe110_mht_1dphi10_L1J40_DPHI-J20s2XE50',
'HLT_j120_xe80',
'HLT_j150_xe80',
]

# start with common list; keep these for potential ISR photon triggered analysis
triggersPhoton = SinglePhoton_2015 + SinglePhoton_2016 + SinglePhoton_2017 + SinglePhoton_2018

triggersPhoton += [
# add gamma+met
'HLT_g40_tight_xe40noL1',
'HLT_g60_loose_xe60noL1',
'HLT_g70_loose_xe70noL1',
'HLT_g80_loose_xe80noL1',
]

# rely on common list for dilepton triggers
triggersDiLep = DiLepton_2015 + DiLepton_2016 + DiLepton_2017 + DiLepton_2018

# no common list for trilepton triggers, so define our own here
triggersTriLep = [
# trimuon for low-MET analysis
'HLT_3mu4',
'HLT_3mu6',
'HLT_3mu6_msonly',
'HLT_3mu8_msonly',
'HLT_mu24_2mu4noL1',
'HLT_mu22_2mu4noL1',
'HLT_mu20_2mu4noL1',
'HLT_mu18_2mu4noL1',
'HLT_mu22_mu8noL1_mu6noL1',
'HLT_mu6_2mu4',
# tri-electron
'HLT_e17_lhloose_2e9_lhloose',
'HLT_e17_lhloose_nod0_2e9_lhloose_nod0',
'HLT_e17_lhloose_nod0_2e10_lhloose_nod0',
'HLT_e17_lhloose_nod0_2e10_lhloose_nod0_L1EM15VH_3EM8VH',
'HLT_e24_lhvloose_nod0_2e12_lhvloose_nod0_L1EM20VH_3EM10VH',
'HLT_e24_lhloose_nod0_2e12_lhloose_nod0_L1EM20VH_3EM10VH',
# mixed trilepton
'HLT_e12_lhloose_2mu10', 
'HLT_e12_lhloose_nod0_2mu10', 
'HLT_e12_lhmedium_nod0_2mu10',
'HLT_2e12_lhloose_mu10', 
'HLT_2e12_lhloose_nod0_mu10',
'HLT_2e12_lhmedium_nod0_mu10',
]

triggersJets=[
# razor
'HLT_j30_xe10_razor100',
'HLT_j30_xe10_razor170',
'HLT_j30_xe10_razor185',
'HLT_j30_xe10_razor195',
'HLT_j30_xe60_razor100',
'HLT_j30_xe60_razor170',
'HLT_j30_xe60_razor185',
'HLT_j30_xe60_razor190',
'HLT_j30_xe60_razor220',
# VBF
'HLT_2j40_0eta490_invm250_L1XE55',
# gammajet
'HLT_g75_tight_3j50noL1_L1EM22VHI',
]

# mostly rely on the common list, but add some extra combined triggers below
triggersSingleLep = SingleLepton_2015 + SingleLepton_2016 + SingleLepton_2017 + SingleLepton_2018

triggersSingleLep += [
# e+jets
'HLT_e15_lhtight_iloose_3j20_L1EM13VH_3J20',
'HLT_e15_lhtight_ivarloose_3j20_L1EM13VH_3J20',
'HLT_e15_lhtight_nod0_iloose_3j20_L1EM13VH_3J20',
'HLT_e15_lhtight_nod0_ivarloose_3j20_L1EM13VH_3J20',
'HLT_e24_lhmedium_5j30_0eta240_L1EM13VH_3J20',
'HLT_e26_lhmedium_5j30_0eta240_L1EM13VH_3J20',
# mu+jets
'HLT_mu14_iloose_3j20_L1MU10_3J20',
'HLT_mu14_ivarloose_3j20_L1MU10_3J20',
'HLT_mu24_5j30_0eta240_L1MU10_3J20',
'HLT_mu26_5j30_0eta240_L1MU10_3J20',
# muon VBF
'HLT_mu6_2j40_0eta490_invm400_L1MU6_J30.0ETA49_2J20.0ETA49',
'HLT_mu6_2j40_0eta490_invm400',
]

triggersNavThin=triggersMET+triggersPhoton+triggersDiLep+triggersJets+triggersSingleLep+triggersSoftMuon+triggersJetPlusMet+triggersSoftMuonEmulation+triggersTriLep
