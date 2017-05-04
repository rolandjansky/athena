# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from RuleClasses import TriggerRule

tag = 'Physics_pp_v5_EB'

# Approx time in hours (default: 1)
ebTime = 1.

# Approx start lumi in multiples of 10**33
ebLumi = 2.1

# Bunch groups to take enhanced bias data in
ebPhysics = True
ebEmpty = True
ebFirstEmpty = True
ebUnpairedIso = True
ebUnpairedNoniso = False
ebAbortgapnotcalib = True

rules={} 
#=======================================

import Physics_pp_v5_rules

rules.update(Physics_pp_v5_rules.rules)

if (ebPhysics):
  rules.update({
    'HLT_noalg_eb_L1RD3_FILLED':                { 1 : TriggerRule(rate=60/ebTime,        comment='EnhancedBias 60 Hz chain ') , },
    'HLT_eb_low_L1RD2_FILLED':                  { 1 : TriggerRule(PS=3.00*ebTime,        comment='EnhancedBias 60 Hz chain ') , },
    'HLT_eb_high_L1RD2_FILLED':                 { 1 : TriggerRule(PS=2.00*ebTime,        comment='EnhancedBias 60 Hz chain ') , },
    'HLT_noalg_eb_L1PhysicsLow_noPS':           { 1 : TriggerRule(PS=42.1*ebTime*ebLumi, comment='EnhancedBias 120 Hz chain '), }, #L dependent
    'HLT_noalg_eb_L1PhysicsHigh_noPS':          { 1 : TriggerRule(PS=2.10*ebTime*ebLumi, comment='EnhancedBias 10 Hz chain ') , }, #L dependent

    'L1_RD3_FILLED':                            { 1: TriggerRule(rate=60/ebTime,     comment='set to 60 Hz'),},
    'L1_RD2_FILLED':                            { 1: TriggerRule(rate=8000/ebTime,   comment='set to 8000 Hz'),},
 
    # Unprescaled L1 items
    'L1_2MU6':                                  { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_3MU4':                                  { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_EM15HI_2TAU12IM_J25_3J12':              { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_EM15HI_2TAU12IM_XE35':                  { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_EM15HI_TAU40_2TAU15':                   { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_EM15VH_MU10':                           { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_EM15VH_3EM7':                           { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_EM22VHI':                               { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_2EM8VH_MU10':                           { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_MU10_TAU12IM_J25_2J12':                 { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_MU10_TAU12IM_XE35':                     { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_MU10_TAU20':                            { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_MU4_3J15':                              { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_J75':                                   { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_3J40':                                  { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_4J15':                                  { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_XE50':                                  { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_2J15_XE55':                             { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_TAU60':                                 { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_TAU20_2TAU12_XE35':                     { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_TAU20IM_2TAU12IM_J25_2J20_3J12':        { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_TAU20IM_2TAU12IM_XE35':                 { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_TAU20IM_2J20_XE45':                     { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_MU20':                                  { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_XE80':                                  { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_J100.31ETA49':                          { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_3MU6':                                  { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
    'L1_J400':                                  { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
  })

if (ebEmpty):
  rules.update({
    'HLT_noalg_eb_L1RD3_EMPTY':                 { 1 : TriggerRule(rate=1,   comment='EnhancedBias BG 1 Hz chain '), },
    'HLT_noalg_eb_L1EMPTY_noPS':                { 1 : TriggerRule(PS=10.00, comment='EnhancedBias BG few Hz chain '), },
    'L1_RD3_EMPTY':                             { 1 : TriggerRule(rate=10,  comment='set to 10 Hz, want 1 Hz but limit to how low L1 can go'),},
    'L1_J12_EMPTY':                             { 1 : TriggerRule(PS=1.00,  comment='EnhancedBias BG unprescaled L1 seed item '), },
  })

if (ebFirstEmpty):
  rules.update({
    'HLT_noalg_eb_L1FIRSTEMPTY_noPS' :          { 1 : TriggerRule(PS=12.00, comment='EnhancedBias BG few Hz chain '), },
    'L1_J12_FIRSTEMPTY':                        { 1 : TriggerRule(PS=1.00,    comment='EnhancedBias BG unprescaled L1 seed item '), }, # 7
  })

if (ebUnpairedIso):
  rules.update({
    'HLT_noalg_eb_L1UNPAIRED_ISO_noPS':         { 1 : TriggerRule(PS=200.00, comment='EnhancedBias BG few Hz chain '), }, # 45
    'L1_J12_UNPAIRED_ISO':                      { 1 : TriggerRule(PS=1.00, comment='EnhancedBias BG unprescaled L1 seed item '), }, # 0.95
    'L1_J30.31ETA49_UNPAIRED_ISO':              { 1 : TriggerRule(PS=1.00, comment='EnhancedBias BG unprescaled L1 seed item '), }, # 0
    'L1_BCM_Wide_UNPAIRED_ISO':                 { 1 : TriggerRule(PS=1.00, comment='EnhancedBias BG unprescaled L1 seed item '), }, # 4.6 
    'L1_BCM_CA_UNPAIRED_ISO':                   { 1 : TriggerRule(PS=1.00, comment='EnhancedBias BG unprescaled L1 seed item '), }, # 21
    'L1_BCM_AC_UNPAIRED_ISO':                   { 1 : TriggerRule(PS=1.00, comment='EnhancedBias BG unprescaled L1 seed item '), }, # 18
  })

if (ebUnpairedNoniso):
  rules.update({
    'HLT_noalg_eb_L1UNPAIRED_NONISO_noPS':      { 1 : TriggerRule(PS=200.00, comment='EnhancedBias BG few Hz chain '), }, #472
    'L1_J12_UNPAIRED_NONISO':                   { 1 : TriggerRule(PS=1.00,   comment='EnhancedBias BG unprescaled L1 seed item '), }, # 3
    'L1_BCM_AC_UNPAIRED_NONISO':                { 1 : TriggerRule(PS=1.00,   comment='EnhancedBias BG unprescaled L1 seed item '), }, # 129
    'L1_BCM_CA_UNPAIRED_NONISO':                { 1 : TriggerRule(PS=1.00,   comment='EnhancedBias BG unprescaled L1 seed item '), }, # 83
    'L1_BCM_Wide_UNPAIRED_NONISO':              { 1 : TriggerRule(PS=1.00,   comment='EnhancedBias BG unprescaled L1 seed item '), }, # 257
  })

if (ebAbortgapnotcalib):
  rules.update({
    'HLT_noalg_eb_L1ABORTGAPNOTCALIB_noPS':     { 1 : TriggerRule(PS=1.00, comment='EnhancedBias few Hz chain '), },
    'L1_J12_ABORTGAPNOTCALIB':                  { 1 : TriggerRule(PS=1.00, comment='EnhancedBias unprescaled L1 seed item '), },
  })

