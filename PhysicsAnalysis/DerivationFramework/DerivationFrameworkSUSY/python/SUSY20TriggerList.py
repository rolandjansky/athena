# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from SUSYCommonTriggerList import SingleLepton_2018,SingleLepton_2017,SingleLepton_2016,SingleLepton_2015
from SUSYCommonTriggerList import SinglePhoton_2018,SinglePhoton_2017,SinglePhoton_2016,SinglePhoton_2015

SingleLeptonTriggers = SingleLepton_2018 + SingleLepton_2017 + SingleLepton_2016 + SingleLepton_2015
triggersElectron = [t for t in SingleLeptonTriggers if 'HLT_e' in t]
triggersPhoton = SinglePhoton_2018 + SinglePhoton_2017 + SinglePhoton_2016 + SinglePhoton_2015


from SUSYCommonTriggerList import MET_2018,MET_2017,MET_2016,MET_2015

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
'HLT_mu4_bJpsi_Trkloose',
'HLT_mu6_bJpsi_Trkloose',
'HLT_mu6_bJpsi_lowpt_TrkPEB',
'HLT_mu10_bJpsi_Trkloose',
'HLT_mu18_bJpsi_Trkloose',
'HLT_mu4_bJpsi_TrkPEB',
'HLT_mu6_bJpsi_TrkPEB',
'HLT_mu10_bJpsi_TrkPEB',
'HLT_mu14_bJpsi_TrkPEB',
'HLT_mu20_bJpsi_TrkPEB'
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



triggersNavThin=triggersMET+triggersSoftMuon+triggersJetPlusMet+triggersSoftMuonEmulation+triggersElectron+triggersPhoton
