# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule
import Monitoring_pp_v4_rules

tag = 'VdM_pp_v4'


rules = {

    # L1
    
    'L1_BGRP7' : {
    0 : TriggerRule(PS=2, comment='Prescale set for 3 bunches and target of roughly 15kHz.'),
    },

    'L1_RD0_FILLED' : {
    0 : TriggerRule(rate=20000, comment='For SCT Fixed rate'),
    },

    # HLT
    
#    'L2_VdM_MBTS_2_BGRP7' : {
#    0:TriggerRule(PS=2, comment='Monitoring'),
#    },
#    'L2_VdM_MBTS_2_UNPAIRED_ISO' : {
#    0:TriggerRule(PS=5, comment='Monitoring'),
#    },

    'L2_VdM_BGRP7' : {
    0:TriggerRule(PS=1, comment='Monitoring'),
    },
    'L2_vtxbeamspot_FSTracks_L2Star_B' : {
    0:TriggerRule(PS=1, comment='Monitoring'),
    },
#    'L2_VdM_RD0_UNPAIRED_ISO' : {
#    0:TriggerRule(PS=5, comment='Monitoring'),
#    },

    

}


Monitoring_pp_v4_rules.physics_rules.update(rules)
rules = Monitoring_pp_v4_rules.physics_rules

if __name__ == "__main__":
    print "tag:   %s" % tag
    for item in sorted(rules.keys()):
        print "\n", item
        for point in rules[item]:
            print "    %20s %s" % (point, rules[item][point])
            
