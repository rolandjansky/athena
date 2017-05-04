# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'StandbyLoose'

rules={}

########################################
# first import physics_pp_v5 as it is

import Standby_pp_v5_rules

rules.update(Standby_pp_v5_rules.rules)


#=======================================
# L1 LowLumi streamer

L1_list=[
    'L1_2EM10VH',
    'L1_2EM8VH_MU10',
    'L1_2J15_XE55',
    'L1_2MU6',
    'L1_3J25.0ETA23',
    'L1_3J40',
    'L1_3MU4',
    'L1_4J15',
    'L1_EM15HI_2TAU12IM',
    'L1_EM15HI_2TAU12IM_J25_3J12',
    'L1_EM15HI_2TAU12IM_XE35',
    'L1_EM15HI_TAU40_2TAU15',
    'L1_EM15VH_MU10',
    'L1_EM15_MU4',
    'L1_EM20VH',
    'L1_EM7_MU10',
    'L1_J75',
    'L1_J75.31ETA49',
    'L1_MU10_TAU12IM',
    'L1_MU10_TAU12IM_J25_2J12',
    'L1_MU10_TAU12IM_XE35',
    'L1_MU10_TAU12_J25_2J12',
    'L1_MU10_TAU20',
    'L1_MU15',
    'L1_MU4_3J20',
    'L1_MU6_J40',
    'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',
    'L1_TAU20IM_2TAU12IM_XE35',
    'L1_TAU20_2J20_XE45',
    'L1_TAU20_2TAU12_XE35',
    'L1_TAU60',
    'L1_XE50',]
rules.update(dict(map(None,L1_list,len(L1_list)*[{
    10 : TriggerRule(PS=1, comment='enable L1LowLumi with unPS'), 
    }])))

HLT_list=['HLT_noalg_L1LowLumi', ]
rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    10 : TriggerRule(PS=1, comment='enable L1LowLumi with unPS'),
    }])))

#=======================================
# L1Calo streamer

rules.update({
    'L1_TAU8'                   : { 10 : TriggerRule(PS=28,  comment='L1', rerun=0) },
    'L1_TAU12'                  : { 10 : TriggerRule(PS=10,  comment='L1', rerun=0) },
    'L1_TAU12IM'                : { 10 : TriggerRule(PS=10,  comment='L1', rerun=0) },
    'L1_TAU20'                  : { 10 : TriggerRule(PS=4,   comment='L1', rerun=0) },
    'L1_TAU40'                  : { 10 : TriggerRule(PS=4,   comment='L1', rerun=0) },
#
    'L1_EM3'                    : { 10 : TriggerRule(PS=50,  comment='L1', rerun=0) },
    'L1_EM7'                    : { 10 : TriggerRule(PS=32,  comment='L1', rerun=0) },
    'L1_EM12'                   : { 10 : TriggerRule(PS=32,  comment='L1', rerun=0) },
    'L1_EM15'                   : { 10 : TriggerRule(PS=32,  comment='L1', rerun=0) },
    'L1_EM18VH'                 : { 10 : TriggerRule(PS=4,   comment='L1', rerun=0) },
#
    'L1_J12'                    : { 10 : TriggerRule(PS=800, comment='L1', rerun=0) },
    'L1_J15'                    : { 10 : TriggerRule(PS=800, comment='L1', rerun=0) },
    'L1_J20'                    : { 10 : TriggerRule(PS=48,  comment='L1', rerun=0) },
    'L1_J25'                    : { 10 : TriggerRule(PS=48,  comment='L1', rerun=0) },
    'L1_J30'                    : { 10 : TriggerRule(PS=48,  comment='L1', rerun=0) },
    'L1_J40'                    : { 10 : TriggerRule(PS=20,  comment='L1', rerun=0) },
    'L1_J50'                    : { 10 : TriggerRule(PS=2,   comment='L1', rerun=0) },
#
    'L1_J15.28ETA31'            : { 10 : TriggerRule(PS=10,  comment='L1', rerun=0) },
    'L1_J20.28ETA31'            : { 10 : TriggerRule(PS=10,  comment='L1', rerun=0) },
#
    'L1_J15.31ETA49'            : { 10 : TriggerRule(PS=40,  comment='L1', rerun=0) },
    'L1_J20.31ETA49'            : { 10 : TriggerRule(PS=40,  comment='L1', rerun=0) },
    'L1_J30.31ETA49'            : { 10 : TriggerRule(PS=40,  comment='L1', rerun=0) },
    'L1_J50.31ETA49'            : { 10 : TriggerRule(PS=20,  comment='L1', rerun=0) },
#
    'L1_J30.0ETA49_2J20.0ETA49' : { 10 : TriggerRule(PS=1,   comment='L1', rerun=0) },
#
    'L1_3J15'                   : { 10 : TriggerRule(PS=6,   comment='L1', rerun=0) },
#
    'L1_XE35'                   : { 10 : TriggerRule(PS=4,   comment='L1', rerun=0) },
#
# XS quite uncertain
    'L1_XS20'                   : { 10 : TriggerRule(PS=50,   comment='L1', rerun=0) }, # similar rate to EM3
    'L1_XS30'                   : { 10 : TriggerRule(PS=30,   comment='L1', rerun=0) }, # similar rate to EM7
    'L1_XS40'                   : { 10 : TriggerRule(PS=15,   comment='L1', rerun=0) },
    'L1_XS50'                   : { 10 : TriggerRule(PS=4,    comment='L1', rerun=0) }, # similar rate to EM18VH
    'L1_XS60'                   : { 10 : TriggerRule(PS=2,    comment='L1', rerun=0) },
})

HLT_list=['HLT_noalg_L1Calo', ]
rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    10 : TriggerRule(PS=10, comment='enable L1Calo streamer'),  # PS value to be adjusted at P1
    }])))

#=======================================
# LArNoiseBurst

L1_list=['L1_XE70','L1_XE80','L1_J50','L1_J100','L1_J75_XE40','L1_J40_XE50',]
rules.update(dict(map(None,L1_list,len(L1_list)*[{
    10 : TriggerRule(PS=1, comment='enable LarNoiseBurst with unPS'),
    }])))

HLT_list=['HLT_larnoiseburst_L1XE50',
          'HLT_larnoiseburst_L1J50',
          'HLT_larnoiseburst_L1J75',
          'HLT_larnoiseburst_L1J75_XE40',
          'HLT_larnoiseburst_L1J40_XE50',
          'HLT_larnoiseburst_L1J100',
          'HLT_larnoiseburst_L1XE80',]
rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
    10 : TriggerRule(PS=1, comment='enable LarNoiseBurst with unPS'),
    }])))

#=======================================
# Relax NCB L1 items prescales

rules.update({
    # relax by a factor of 50
    'L1_BCM_AC_CA_BGRP0'           : { 5000 : TriggerRule(PS=20, comment='L1', rerun=0) },
    'L1_BCM_Wide_UNPAIRED_NONISO'  : { 5000 : TriggerRule(PS=20, comment='L1', rerun=0) }, 
    'L1_BCM_AC_UNPAIRED_ISO'       : { 5000 : TriggerRule(PS=1,  comment='L1', rerun=0) },
    'L1_BCM_CA_UNPAIRED_ISO'       : { 5000 : TriggerRule(PS=1,  comment='L1', rerun=0) },
    'L1_BCM_AC_UNPAIRED_NONISO'    : { 5000 : TriggerRule(PS=20, comment='L1', rerun=0) },
    'L1_BCM_CA_UNPAIRED_NONISO'    : { 5000 : TriggerRule(PS=20, comment='L1', rerun=0) },
    'L1_J12_UNPAIRED_NONISO'       : { 5000 : TriggerRule(PS=2,  comment='L1', rerun=0) },
})
