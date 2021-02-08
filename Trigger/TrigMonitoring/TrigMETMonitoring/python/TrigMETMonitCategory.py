# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# l1 trigger to monitor: 
met_triggers_l1_shifter = [
    "L1_XE50",
    "L1_jXENC50",
    "L1_jXERHO50",
    "L1_gXENC50",
    "L1_gXEJWOJ50",
    "L1_gXERHO50",
    "L1_gXEPUFIT50",
]

met_triggers_l1_expert = [
    "L1_XE55",
    "L1_XE60",
    ]

# hlt triggers to monitor
met_triggers_hlt_shifter = [
    "HLT_xe110_pufit_xe65_L1XE50",
    "HLT_xe110_pufit_xe70_L1XE50",
    "HLT_xe110_pufit_xe65_L1XE55",
    "HLT_xe110_pufit_xe65_L1XE60",
    "HLT_xe110_pufit_xe65_L1gXERHO50",
    "HLT_xe110_pufit_xe65_L1gXEPUFIT50",
    "HLT_xe0noL1_l2fsperf_L1gXERHO50",
    "HLT_xe0noL1_l2fsperf_L1gXEPUFIT50",
    "HLT_xe0noL1_l2fsperf_pufit_L1gXERHO50",
    "HLT_xe0noL1_l2fsperf_pufit_L1gXEPUFIT50",
] 

met_triggers_hlt_expert = [
    "HLT_xe110_pufit_xe75_L1XE50",
    "HLT_xe110_pufit_xe70_L1XE55",
    "HLT_xe110_pufit_xe70_L1XE60",
    "HLT_xe100_trkmht_FTK_L1XE50",
    "HLT_xe110_trkmht_FTK_L1XE55",
    "HLT_xe110_trkmht_FTK_L1XE60",
    "HLT_xe110_trktc_lcw_L1XE50",
    "HLT_xe110_trktc_lcw_FTK_L1XE50",
]


# Trigger chins in early 2018
    #"HLT_xe100_pufit_xe75_L1XE60",
    #"HLT_xe120_pufit_L1XE50",
    #"HLT_xe120_pufit_L1XE55",
    #"HLT_xe120_pufit_L1XE60",


# Trigger chins in 2015-2017
    #"HLT_xe110_pufit_L1XE50",
    #"HLT_xe110_pufit_L1XE55",
    #"HLT_xe110_pufit_L1XE60",
    #"HLT_xe120_mht_xe80_L1XE60",
        #"HLT_mu30",
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


monitoring_met_shifter = met_triggers_l1_shifter + met_triggers_hlt_shifter
monitoring_met_expert = met_triggers_l1_expert + met_triggers_hlt_expert

monitoring_met_shifter_pp = met_triggers_l1_shifter + met_triggers_hlt_shifter
monitoring_met_expert_pp = met_triggers_l1_expert + met_triggers_hlt_expert

monitoring_met_shifter_cosmic = met_triggers_l1_shifter + met_triggers_hlt_shifter
monitoring_met_expert_cosmic = met_triggers_l1_expert + met_triggers_hlt_expert

monitoring_met = monitoring_met_shifter
monitoring_met_pp = monitoring_met_shifter_pp
monitoring_met_cosmic = monitoring_met_shifter_cosmic

