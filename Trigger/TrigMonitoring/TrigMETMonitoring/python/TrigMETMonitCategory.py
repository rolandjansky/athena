# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# l1 trigger to monitor: 
# the first one is the denominator for all efficiencies
met_triggers_l1 = [
        "L1_XE35", 
        "L1_XE60",
        "L1_XE70",
]

# hlt triggers to monitor
# 
met_triggers_hlt = [
        ##      "HLT_mu30",
        "HLT_xe80",    
        "HLT_xe80_tc_lcw",
        "HLT_xe80_tc_em",
        "HLT_xe80_pueta",
        "HLT_xe80_pufit",
        "HLT_xe80_mht",
] 

monitoring_met = met_triggers_l1 + met_triggers_hlt
