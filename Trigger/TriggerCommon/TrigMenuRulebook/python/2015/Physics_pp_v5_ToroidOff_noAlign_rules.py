# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule
# ATR-9923
tag = 'Physics_pp_v5_ToroidOff_noAlign'

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
    
L1_list=['L1_MU4', 'L1_MU6', 'L1_MU15','L1_MU10', 'L1_MU4_EMPTY', 'L1_MU4_FIRSTEMPTY', 'L1_MU11_EMPTY', 'L1_MU4_UNPAIRED_ISO', 'L1_2MU4', 'L1_2MU6', 'L1_MU6_2MU4', 'L1_2MU20_OVERLAY', 'L1_MU10_2MU6', 'L1_MU11_2MU6', 'L1_3MU4', 'L1_3MU6', 'L1_EM8I_MU10', 'L1_EM15I_MU4', 'L1_2EM8VH_MU10', 'L1_EM15VH_MU10', 'L1_MU10_TAU12IM', 'L1_MU10_TAU12IM_J25_2J12', 'L1_MU10_TAU12_J25_2J12', 'L1_MU10_TAU20', 'L1_MU10_TAU20IM', 'L1_MU10_TAU12IM_XE35', 'L1_MU10_TAU12IM_XE40', 'L1_MU4_J12', 'L1_MU6_J20', 'L1_MU6_J40', 'L1_MU6_J75', 'L1_MU6_J30.0ETA49_2J20.0ETA49', 'L1_MU4_3J15', 'L1_MU4_3J20', 'L1_MU4_J30', 'L1_MU4_J50', 'L1_EM8I_MU10', 'L1_MU6_3MU4', 'L1_2MU6_3MU4', 'L1_MU20_FIRSTEMPTY' ]
rules.update(dict(map(None,L1_list,len(L1_list)*[{
                    10 : TriggerRule(PS=10000, comment='prescaled for toroid off run'),
                    }])))

# main physics muon items. keep unprescaling slowly as rate goes down.
L1_list=['L1_MU20']
rules.update(dict(map(None,L1_list,len(L1_list)*[{
                    10 : TriggerRule(PS=300, comment='prescaled for toroid off run'),
                    }])))

L1_list=['L1_2MU10']
rules.update(dict(map(None,L1_list,len(L1_list)*[{
                    10 : TriggerRule(PS=50, comment='prescaled for toroid off run'),
                    }])))

HLT_list=['HLT_mu0_muoncalib','HLT_mu0_muoncalib_ds3']
rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    10 : TriggerRule(PS=1, comment='Muon calibration',rerun=0),
    }])))
    
# used in runs with toroid off and alignment, during 25ns.1, but not really needed in toroid off purpose according to rates seen online. Keep an eye if IBL limitation to rates persist and is strict.

#L1_list=['L1_XE35', 'L1_XE40']
#rules.update(dict(map(None,L1_list,len(L1_list)*[{
#                    10 : TriggerRule(PS=100, comment='prescaled for toroid off run'),
#                    }])))

#L1_list=['L1_XE55']
#rules.update(dict(map(None,L1_list,len(L1_list)*[{
#                    1000 : TriggerRule(PS=200, comment='prescaled for toroid off run'),
#                    }])))

#L1_list=['L1_XE35_BGRP7', 'L1_XE40_BGRP7']
#rules.update(dict(map(None,L1_list,len(L1_list)*[{
#                    1000 : TriggerRule(PS=10, comment='prescaled for toroid off run'),
#                    }])))

#L1_list=['L1_XE55_BGRP7']
#rules.update(dict(map(None,L1_list,len(L1_list)*[{
#                    1000 : TriggerRule(PS=-1, comment='prescaled for toroid off run'),
#                    }])))
