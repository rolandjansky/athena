# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule
# ATR-9923
tag = 'Physics_pp_v5_wL1Calo'

rules={}

#=======================================
# first import physics_pp_v5 as it is

import Physics_pp_v5_rules

rules.update(Physics_pp_v5_rules.rules)


rules.update({
        'HLT_noalg_L1TAU12'        : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1TAU20'        : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1J75'          : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1J75.31ETA49'  : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1J85'          : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1J100'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1J100.31ETA49' : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1J120'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1J400'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1EM20VH'       : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1EM20VHI'      : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1EM22VHI'      : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1XE50'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1XE55'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1XE60'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1XE70'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1XE80'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1J12'          : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1J15'          : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1J15.28ETA31'  : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1J15.31ETA49'  : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1J20'          : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1J20.28ETA31'  : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1J20.31ETA49'  : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1J25'          : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1J30'          : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1J30.31ETA49'  : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1J40'          : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1J50'          : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1J50.31ETA49'  : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1EM8VH'        : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1EM10'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1EM12'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1EM15'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1EM15VH'       : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1EM13VH'       : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1EM10VH'       : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1TAU8'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1TAU40'        : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1TAU60'        : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1XE40'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1XE45'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1TE10'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1TE20'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1TE30'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1TE40'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1TE50'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1TE10.0ETA24'  : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1TE20.0ETA24'  : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1XS20'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1XS30'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1XS40'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1XS50'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
        'HLT_noalg_L1XS60'         : { 1000 : TriggerRule(rate=1.0, comment='L1Calo data',  rerun=0) },
    }
)


