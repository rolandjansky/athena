# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'VdM_pp_v5'

rules = {
    
    "HLT_lumipeb_vdm_L1MBTS_2_BGRP9" : { 1 : TriggerRule(PS=1, comment = 'VdM', rerun = 0.0) },
    
    #(5 bunches of Empty before paired from BGRP9, handpicked). Unprescaled Expect low rate of a few tenth of Hz.
    'HLT_lumipeb_vdm_L1MBTS_2_BGRP11' : { 1 : TriggerRule(PS=1, comment = 'VdM', rerun = 0.0) },
    
    # Expect low rate of a few tenth of Hz.
    'HLT_lumipeb_vdm_L1MBTS_2_UNPAIRED_ISO' : { 1 : TriggerRule(PS=1, comment = 'VdM', rerun = 0.0) },

    }


import Monitoring_pp_v5_rules
                                                                                                                                                                     
duplication_list = [ rule  for rule in Monitoring_pp_v5_rules.physics_rules if rules.has_key(rule) ]
if duplication_list:
    print "FATAL: duplicated rules for chains: "
    print '\n'.join([ str(myelement)  for myelement in duplication_list ])
    sys.exit("FATAL rulebook failed, exiting")

rules.update(Monitoring_pp_v5_rules.physics_rules)


