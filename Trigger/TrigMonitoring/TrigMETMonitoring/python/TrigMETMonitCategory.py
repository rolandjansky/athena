# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# l1 trigger to monitor: 
# the first one is the denominator for all efficiencies
met_triggers_l1 = [
        "L1_XE50", 
        #"L1_XS30", 
        "L1_XE60",
        #"L1_XE70",
]

# hlt triggers to monitor
# 
met_triggers_hlt = [
        ##      "HLT_mu30",
        "HLT_xe100_pufit_L1XE50",
        "HLT_xe110_mht_L1XE50",
        "HLT_xe110_pufit_L1XE60",
        "HLT_xe120_mht_xe80_L1XE60",
        #"HLT_xs30",    
        #"HLT_xe70_tc_lcw",    
        #"HLT_xe80_tc_lcw_L1XE50",    
        #"HLT_xe100_tc_lcw",    
        #"HLT_xe80_tc_lcw",
        #"HLT_xe80_tc_em",
        #"HLT_xe80_pueta",
        #"HLT_xe80_pufit",
        #"HLT_xe80_mht",
        #"HLT_xe35_xeNOcut",
        #"HLT_xe35_xeNOcut_tc_lcw",
        #"HLT_xe35_xeNOcut_tc_em",
        #"HLT_xe35_xeNOcut_pueta",
        #"HLT_xe35_xeNOcut_pufit",
        #"HLT_xe35_xeNOcut_mht",
] 

monitoring_met = met_triggers_l1 + met_triggers_hlt
monitoring_met_pp = met_triggers_l1 + met_triggers_hlt
monitoring_met_cosmic = met_triggers_l1 + met_triggers_hlt

