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
    
L1_list=['L1_MU4', 'L1_MU6', 'L1_MU10', 'L1_MU15', 'L1_MU4_EMPTY', 'L1_MU4_FIRSTEMPTY', 'L1_MU11_EMPTY', 'L1_MU4_UNPAIRED_ISO', 'L1_2MU4', 'L1_2MU6', 'L1_2MU10', 'L1_2MU20_OVERLAY', 'L1_MU10_2MU6', 'L1_MU11_2MU6', 'L1_3MU4', 'L1_MU6_2MU4', 'L1_3MU6', 'L1_EM7_MU10', 'L1_EM15_MU4', 'L1_2EM8VH_MU10', 'L1_EM15VH_MU10', 'L1_MU10_TAU12IM', 'L1_MU10_TAU12IM_J25_2J12', 'L1_MU10_TAU12_J25_2J12', 'L1_MU10_TAU20', 'L1_MU10_TAU20IM', 'L1_MU10_TAU12IM_XE35', 'L1_MU10_TAU12IM_XE40', 'L1_MU4_J12', 'L1_MU6_J20', 'L1_MU6_J40', 'L1_MU6_J75', 'L1_MU6_J30.0ETA49_2J20.0ETA49', 'L1_MU4_3J15', 'L1_MU4_3J20', 'L1_MU4_J30', 'L1_MU4_J50', 'L1_EM8I_MU10', ]
rules.update(dict(map(None,L1_list,len(L1_list)*[{
                    10 : TriggerRule(PS=-1, comment='disabled for toroid off run'),
                    }])))
    

#=======================================
# disable HLT chains seeded from MU20

HLT_list=['HLT_2g10_loose_mu20', 'HLT_2g10_medium_mu20', 'HLT_e7_lhmedium_mu24', 'HLT_e7_lhmedium_nod0_mu24', 'HLT_e7_medium_mu24', 'HLT_g25_medium_mu24', 'HLT_mu10_mucombTag_noEF_L1MU40', 'HLT_mu15_mucombTag_noEF_L1MU40', 'HLT_mu20', 'HLT_mu20_2mu0noL1_JpsimumuFS', 'HLT_mu20_2mu4_JpsimumuL2', 'HLT_mu20_2mu4noL1', 'HLT_mu20_idperf', 'HLT_mu20_msonly', 'HLT_mu20_msonly_mu6noL1_msonly_nscan05', 'HLT_mu20_mu6noL1_nscan03', 'HLT_mu20_mu8noL1', 'HLT_mu20_mucombTag_noEF_L1MU40', 'HLT_mu20_r1extr', 'HLT_mu22', 'HLT_mu22_2mu4noL1', 'HLT_mu22_mu8noL1', 'HLT_mu24', 'HLT_mu24_2j20noL1', 'HLT_mu24_2mu4noL1', 'HLT_mu24_3j20noL1', 'HLT_mu24_4j15noL1', 'HLT_mu24_5j15noL1', 'HLT_mu24_L2Star_idperf', 'HLT_mu24_idperf', 'HLT_mu24_imedium', 'HLT_mu24_mu8noL1', 'HLT_mu25_mucombTag_noEF_L1MU40', 'HLT_mu26', 'HLT_mu26_2j20noL1', 'HLT_mu26_3j20noL1', 'HLT_mu26_4j15noL1', 'HLT_mu26_5j15noL1', 'HLT_mu26_imedium', 'HLT_mu26_imedium_2j35_bperf', 'HLT_mu40', 'HLT_mu50', 'HLT_mu60_0eta105_msonly', 'HLT_noalg_L1MU20', ]
rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    10 : TriggerRule(PS=-1, comment='disabled for toroid off run'), 
                    }])))


########################################
# then, override rule hereafter on purpose

#=======================================
# toroid-off main triggers and afterglow

NumOfBunches=38
PSforL1LowLumiStreamer ={2 : 10,
                         38 : 100,
                         110 : 350}

try:
    ps= PSforL1LowLumiStreamer[NumOfBunches]
    print "Num of Bunches: ", NumOfBunches, ", PS for L1LowLumi: ", ps
except:
    print "Please, make sure to choose correcto num. of bunches"

rules.update({
    'HLT_lumipeb_L1RD0_FILLED'           : { 10 : TriggerRule(PS=-1, comment='PixelBeam',  rerun=0) },
    'HLT_lumipeb_L1RD0_UNPAIRED_ISO'     : { 10 : TriggerRule(PS=-1, comment='PixelBeam',  rerun=0) }, 
    'HLT_lumipeb_L1RD0_ABORTGAPNOTCALIB' : { 10 : TriggerRule(PS=-1, comment='PixelBeam',  rerun=0) }, 

    'HLT_noalg_L1LowLumi'                : { 10 : TriggerRule(PS=PSforL1LowLumiStreamer[NumOfBunches], comment='Target 10Hz in express stream', ESValue=1) }, 

    'L1_RD0_FILLED'  : { 10 : TriggerRule(rate=1000, comment='L1'),
                         3000 : TriggerRule(rate=100, comment='L1') },

    # LAr
    "HLT_2e12_lhloose_L12EM10VH"      : { 10 : TriggerRule(PS=1, comment='LAr special request for toroid offrun', rerun=0) },
    "HLT_2e15_lhloose_L12EM13VH"      : { 10 : TriggerRule(PS=1, comment='LAr special request for toroid offrun', rerun=0) },

    # torid off main muon triggers
    "HLT_mu10_mucombTag_noEF_L1MU40"  : { 10 : TriggerRule(PS=1, comment='Toroid off run primary muon', rerun=0) },
    "HLT_mu15_mucombTag_noEF_L1MU40"  : { 10 : TriggerRule(PS=1, comment='Toroid off run primary muon', rerun=0) },
    "HLT_mu20_mucombTag_noEF_L1MU40"  : { 10 : TriggerRule(PS=1, comment='Toroid off run primary muon', rerun=0) },
    "HLT_mu25_mucombTag_noEF_L1MU40"  : { 10 : TriggerRule(PS=1, comment='Toroid off run primary muon', rerun=0) },

    #disable after-glow trigger
    'HLT_lumipeb_vdm_L1RD0_BGRP9'         : { 10 : TriggerRule(PS=-1, comment='afterglow disabled', rerun=0) },
    'HLT_lumipeb_vdm_L1RD0_FILLED'        : { 10 : TriggerRule(PS=-1, comment='afterglow disabled', rerun=0) },
    'HLT_lumipeb_vdm_L1RD0_UNPAIRED_ISO'  : { 10 : TriggerRule(PS=-1, comment='afterglow disabled', rerun=0) },   

    }
)


if doAfterGlow:
    # Afterglow
    # 1000 Hz BGRP9, 500 Hz BGRP9 --> 1 hour (3 M of BGRP9)
    rules['L1_RD0_BGRP9']                  = { 10 : TriggerRule(rate=1000, scaling='bunches', comment='') }
    rules['HLT_lumipeb_vdm_L1RD0_BGRP9']   = { 10 : TriggerRule(PS=1, comment='afterglow enabled, target 1000 Hz', rerun=0) }
    rules['HLT_lumipeb_vdm_L1RD0_FILLED']  = { 10 : TriggerRule(rate=500, comment='afterglow enabled, target 500 Hz', rerun=0) }


