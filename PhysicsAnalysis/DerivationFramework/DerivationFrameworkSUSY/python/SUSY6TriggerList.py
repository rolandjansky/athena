# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from DerivationFrameworkSUSY.SUSYCommonTriggerList import MET_2018,MET_2017,MET_2016,MET_2015
from DerivationFrameworkSUSY.SUSYCommonTriggerList import SingleLepton_2018,SingleLepton_2017,SingleLepton_2016,SingleLepton_2015
from DerivationFrameworkSUSY.SUSYCommonTriggerList import DiLepton_2018,DiLepton_2017,DiLepton_2016,DiLepton_2015

SUSY6METTriggers = MET_2018 + MET_2017 + MET_2016 + MET_2015

SUSY6JetMETTriggers = [
  'HLT_j80_xe80',
  'HLT_j100_xe80',
  'HLT_j80_xe80_dphi10_L1J40_DPHI-J20s2XE50',
  'HLT_j80_xe80_dphi10_L1J40_DPHI-J20XE50',
  'HLT_j80_xe80_dphi10_L1J40_DPHI-Js2XE50',
  'HLT_j80_xe80_dphi10_L1J40_DPHI-CJ20XE50',
  'HLT_j100_xe80_dphi10_L1J40_DPHI-J20s2XE50',
  'HLT_j100_xe80_dphi10_L1J40_DPHI-J20XE50',
  'HLT_j100_xe80_dphi10_L1J40_DPHI-Js2XE50',
  'HLT_j100_xe80_dphi10_L1J40_DPHI-CJ20XE50',
  'HLT_j100_xe110_mht_1dphi10_L1J40_DPHI-J20s2XE50',
  'HLT_j120_xe80',
  'HLT_j150_xe80',
  'HLT_noalg_L1J400'
]

SUSY6SingleLeptonTriggers = SingleLepton_2018 + SingleLepton_2017 + SingleLepton_2016 + SingleLepton_2015
SUSY6SingleEleTriggers = [t for t in SUSY6SingleLeptonTriggers if 'HLT_e' in t]
SUSY6SingleMuTriggers = [t for t in SUSY6SingleLeptonTriggers if 'HLT_mu' in t]

SUSY6DiLeptonTriggers =  DiLepton_2018 + DiLepton_2017 + DiLepton_2016 + DiLepton_2015
SUSY6DiMuonTriggers = [t for t in SUSY6DiLeptonTriggers if 'HLT_2mu' in t]

SUSY6LateMuonTriggers = [
  'HLT_mu10_mgonly_L1LATEMU10_J50',
  'HLT_mu10_mgonly_L1LATEMU10_XE50',
  'HLT_mu10_mgonly_L1LATE-MU10_XE40'
]
