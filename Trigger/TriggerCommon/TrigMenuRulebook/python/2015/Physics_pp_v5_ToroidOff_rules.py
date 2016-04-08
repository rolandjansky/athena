# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule
# ATR-9923
tag = 'Physics_pp_v5_ToroidOff'

rules={}
doAfterGlow=False
removePhysics=False


if removePhysics:
########################################
# first import Monitoring

    import Monitoring_pp_v5_rules

    
    rules.update(Monitoring_pp_v5_rules.physics_rules)


else:
########################################
# first import physics_pp_v5 as it is

    import Physics_pp_v5_rules
    
    rules.update(Physics_pp_v5_rules.rules)


#=======================================
# disable L1_MU except for MU20
    
L1_list=['L1_MU4', 'L1_MU6', 'L1_MU10', 'L1_MU15', 'L1_MU4_EMPTY', 'L1_MU4_FIRSTEMPTY', 'L1_MU11_EMPTY', 'L1_MU4_UNPAIRED_ISO', 'L1_2MU4', 'L1_2MU6', 'L1_2MU10', 'L1_2MU20_OVERLAY', 'L1_MU10_2MU6', 'L1_MU11_2MU6', 'L1_3MU4', 'L1_MU6_2MU4', 'L1_3MU6', 'L1_EM7_MU10', 'L1_EM15_MU4', 'L1_2EM8VH_MU10', 'L1_EM15VH_MU10', 'L1_MU10_TAU12IM', 'L1_MU10_TAU12IM_J25_2J12', 'L1_MU10_TAU12_J25_2J12', 'L1_MU10_TAU20', 'L1_MU10_TAU20IM', 'L1_MU10_TAU12IM_XE35', 'L1_MU10_TAU12IM_XE40', 'L1_MU4_J12', 'L1_MU6_J20', 'L1_MU6_J40', 'L1_MU6_J75', 'L1_MU6_J30.0ETA49_2J20.0ETA49', 'L1_MU4_3J15', 'L1_MU4_3J20', 'L1_MU4_J30', 'L1_MU4_J50', 'L1_EM8I_MU10', 'L1_MU6_3MU4', 'L1_2MU6_3MU4', 'L1_MU20_FIRSTEMPTY' ]
rules.update(dict(map(None,L1_list,len(L1_list)*[{
                    10 : TriggerRule(PS=-1, comment='disabled for toroid off run'),
                    }])))
    

#=======================================
# disable HLT chains seeded from MU20

HLT_list=['HLT_2g10_loose_mu20', 'HLT_2g10_medium_mu20', 'HLT_e7_lhmedium_mu24', 'HLT_e7_lhmedium_nod0_mu24', 'HLT_e7_medium_mu24', 'HLT_g25_medium_mu24', 'HLT_mu0_muoncalib', 'HLT_mu0_muoncalib_ds3', 'HLT_mu10_mucombTag_noEF_L1MU40', 'HLT_mu15_mucombTag_noEF_L1MU40', 'HLT_mu20', 'HLT_mu20_2mu0noL1_JpsimumuFS', 'HLT_mu20_2mu4_JpsimumuL2', 'HLT_mu20_2mu4noL1', 'HLT_mu20_idperf', 'HLT_mu20_l2msonly_mu6noL1_nscan03_l2msonly', 'HLT_mu20_msonly', 'HLT_mu20_msonly_mu6noL1_msonly_nscan05', 'HLT_mu20_mu6noL1_nscan03', 'HLT_mu20_mu8noL1', 'HLT_mu20_mucombTag_noEF_L1MU40', 'HLT_mu22', 'HLT_mu22_2mu4noL1', 'HLT_mu22_mu8noL1', 'HLT_mu24', 'HLT_mu24_2j20noL1', 'HLT_mu24_2mu4noL1', 'HLT_mu24_3j20noL1', 'HLT_mu24_4j15noL1', 'HLT_mu24_5j15noL1', 'HLT_mu24_L2Star_idperf', 'HLT_mu24_idperf', 'HLT_mu24_imedium', 'HLT_mu24_mu8noL1', 'HLT_mu25_mucombTag_noEF_L1MU40', 'HLT_mu26', 'HLT_mu26_2j20noL1', 'HLT_mu26_3j20noL1', 'HLT_mu26_4j15noL1', 'HLT_mu26_5j15noL1', 'HLT_mu26_imedium', 'HLT_mu26_imedium_2j35_bperf', 'HLT_mu40', 'HLT_mu50', 'HLT_mu60_0eta105_msonly', 'HLT_mu8_mucombTag_noEF_L1MU40', 'HLT_noalg_L1MU20', ]
rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    10 : TriggerRule(PS=-1, comment='disabled for toroid off run'), 
                    }])))


########################################
# then, override rule hereafter on purpose

#=======================================
# toroid-off main triggers and afterglow

NumOfBunches=72

PSforL1LowLumiStreamer = { 26 : 70,
                           72 : 200,
                          144 : 450 }

# MU20 rate: 17/47/94 kHz with 26/72/144b
# Phys rate: 1.5/4/8 kHz with 26/72/144b
# L1 limit (w/o IBL): 15/50/90 kHz with 26/72/144b
# L1 limit (w/  IBL): 12/16    kHz with 26/72b
#
# Afterglow (to be w/ IBL) at 26b, possibliy at 72b as well
# MU20 to be (w/o IBL) ~12/~45/~80 kHz
# MU20 to be (w/  IBL) ~9/~10 kHz

# L1_MU20 prescale
PSforL1Mu20 = { 26  : 2,
                72  : 1,
                144 : 1.15, }
if doAfterGlow:
    PSforL1Mu20.update({ 72  : 5})

# get 10 Hz for IDperf
PSforMu20Idperf = { 26  :  900,
                    72  : 4500,
                    144 : 8000, }
if doAfterGlow:
    PSforMu20Idperf.update({72  : 1000})

# get ~25 kHz for muon calibration
PSforMuoncalib = { 26  : 1,
                   72  : 2,
                   144 : 3, }
if doAfterGlow:
    PSforMuoncalib.update({72  : 1})

# ALFA prescale: 15 Hz per bunch -> adjust to 100 Hz
PSforAlfacalib = { 26  : 8,
                   72  : 22,
                   144 : 42, }

try:
    ps_low= PSforL1LowLumiStreamer[NumOfBunches]
    ps_mu20= PSforL1Mu20[NumOfBunches]
    ps_id= PSforMu20Idperf[NumOfBunches]
    ps_mucal= PSforMuoncalib[NumOfBunches]
    ps_alfa= PSforAlfacalib[NumOfBunches]
    print "Num of Bunches: ", NumOfBunches, ", doAfterGlow: ", doAfterGlow, ", PS for L1LowLumi: ",  ps_low, ", PS for L1_MU20: ", ps_mu20, ", PS for mu20_idperf: ", ps_id, ", PS for mu0_muoncalib: ", ps_mucal, ", PS for L1 items for alfacalib: ", ps_alfa
except:
    print "Please, make sure to choose correcto num. of bunches"

rules.update({
    # Disable PixelBeam to save L1 bandwidth if generated with lumi<1500
    'L1_RD0_FILLED'                      : { 10 : TriggerRule(rate=1000, comment='L1'),
                                           1500 : TriggerRule(rate=100, comment='L1') },
    'HLT_lumipeb_L1RD0_FILLED'           : { 10 : TriggerRule(PS=1, comment='PixelBeam enabled',  rerun=0),
                                           1500 : TriggerRule(PS=-1, comment='PixelBeam disabled',  rerun=0), },
    'HLT_lumipeb_L1RD0_UNPAIRED_ISO'     : { 10 : TriggerRule(PS=1, comment='PixelBeam enabled',  rerun=0),
                                           1500 : TriggerRule(PS=-1, comment='PixelBeam disabled',  rerun=0), },
    'HLT_lumipeb_L1RD0_ABORTGAPNOTCALIB' : { 10 : TriggerRule(PS=1, comment='PixelBeam enabled',  rerun=0),
                                           1500 : TriggerRule(PS=-1, comment='PixelBeam disabled',  rerun=0), },

    # L1LowLumi streamer for express stream
    'HLT_noalg_L1LowLumi'                : { 10 : TriggerRule(PS=PSforL1LowLumiStreamer[NumOfBunches], comment='Target 10Hz in express stream', ESValue=1) }, 

    # LAr
    "HLT_2e12_lhloose_L12EM10VH"        : { 10 : TriggerRule(PS=1, comment='LAr special request for toroid-off run', rerun=0) },
    "HLT_2e15_lhloose_L12EM13VH"        : { 10 : TriggerRule(PS=1, comment='LAr special request for toroid-off run', rerun=0) },

    # Toroid off main muon triggers
    "L1_MU20"                           : { 10 : TriggerRule(PS=PSforL1Mu20[NumOfBunches], comment='Toroid off run primary L1 muon', rerun=0) },
    "HLT_mu8_mucombTag_noEF_L1MU40"     : { 10 : TriggerRule(PS=1, comment='Toroid off run primary muon', rerun=0) },
    "HLT_mu10_mucombTag_noEF_L1MU40"    : { 10 : TriggerRule(PS=1, comment='Toroid off run primary muon', rerun=0) },
    "HLT_mu15_mucombTag_noEF_L1MU40"    : { 10 : TriggerRule(PS=1, comment='Toroid off run primary muon', rerun=0) },
    "HLT_mu20_mucombTag_noEF_L1MU40"    : { 10 : TriggerRule(PS=1, comment='Toroid off run primary muon', rerun=0) },
    "HLT_mu25_mucombTag_noEF_L1MU40"    : { 10 : TriggerRule(PS=1, comment='Toroid off run primary muon', rerun=0) },

    # Muon calibration stream 
    'HLT_mu0_muoncalib'                 : { 10 : TriggerRule(PS=PSforMuoncalib[NumOfBunches], comment='Target 20 kHz in muon calibration stream', rerun=0) },
    'HLT_mu0_muoncalib_ds3'             : { 10 : TriggerRule(PS=-1, comment='Disable DS version muon calibration stream (ATR-11998)', rerun=0) },

    # IDperf monitoring
    'HLT_mu20_idperf'                   : { 10 : TriggerRule(PS=PSforMu20Idperf[NumOfBunches], comment='Target 10 Hz in Main, 1 Hz in express stream', ESValue=10) },
   
    # ALFA calibration
    'L1_ALFA_B7L1U'                     : { 10 : TriggerRule(PS=PSforAlfacalib[NumOfBunches], comment='alfa calib (ATR-11756)') },
    'L1_ALFA_B7L1L'                     : { 10 : TriggerRule(PS=PSforAlfacalib[NumOfBunches], comment='alfa calib (ATR-11756)') },
    'L1_ALFA_A7L1U'                     : { 10 : TriggerRule(PS=PSforAlfacalib[NumOfBunches], comment='alfa calib (ATR-11756)') },
    'L1_ALFA_A7L1L'                     : { 10 : TriggerRule(PS=PSforAlfacalib[NumOfBunches], comment='alfa calib (ATR-11756)') },
    'L1_ALFA_A7R1U'                     : { 10 : TriggerRule(PS=PSforAlfacalib[NumOfBunches], comment='alfa calib (ATR-11756)') },
    'L1_ALFA_A7R1L'                     : { 10 : TriggerRule(PS=PSforAlfacalib[NumOfBunches], comment='alfa calib (ATR-11756)') },
    'L1_ALFA_B7R1U'                     : { 10 : TriggerRule(PS=PSforAlfacalib[NumOfBunches], comment='alfa calib (ATR-11756)') },
    'L1_ALFA_B7R1L'                     : { 10 : TriggerRule(PS=PSforAlfacalib[NumOfBunches], comment='alfa calib (ATR-11756)') },
    'HLT_alfacalib'                     : { 10 : TriggerRule(PS=1, comment='target 100 Hz, alfa calib (ATR-11756)') },

    # Disable after-glow trigger
    'HLT_lumipeb_vdm_L1RD0_BGRP9'       : { 10 : TriggerRule(PS=-1, comment='afterglow disabled', rerun=0) },
    'HLT_lumipeb_vdm_L1RD0_FILLED'      : { 10 : TriggerRule(PS=-1, comment='afterglow disabled', rerun=0) },
    'HLT_lumipeb_vdm_L1RD0_UNPAIRED_ISO': { 10 : TriggerRule(PS=-1, comment='afterglow disabled', rerun=0) },   
    }
)


if doAfterGlow:
    # Afterglow (ATR-11756)
    # 1000 Hz BGRP9, 500 Hz BGRP1, 100 Hz BGRP4  --> 1 hour (3 M of BGRP9)
    rules['L1_RD0_BGRP9']                       = { 10 : TriggerRule(rate=1000, scaling='bunches', comment='') }
    rules['L1_RD0_FILLED']                      = { 10 : TriggerRule(rate=500,  scaling='bunches', comment='') }
    rules['L1_RD0_UNPAIRED_ISO']                = { 10 : TriggerRule(rate=100,  scaling='bunches', comment='') }
    rules['HLT_lumipeb_vdm_L1RD0_BGRP9']        = { 10 : TriggerRule(PS=1, comment='afterglow enabled, target 1000 Hz', rerun=0) }
    rules['HLT_lumipeb_vdm_L1RD0_FILLED']       = { 10 : TriggerRule(PS=1, comment='afterglow enabled, target 500 Hz',  rerun=0) }
    rules['HLT_lumipeb_vdm_L1RD0_UNPAIRED_ISO'] = { 10 : TriggerRule(PS=1, comment='afterglow enabled, target 100 Hz',  rerun=0) }
