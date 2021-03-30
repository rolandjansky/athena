# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# l1 trigger to monitor: 
met_triggers_l1_shifter = [
    "L1_XE50",
    "L1_XE55",
]

met_triggers_l1_expert = [
    "L1_XE10",
    "L1_XE60",
]

# hlt triggers to monitor
met_triggers_hlt_shifter = [
    "HLT_xe70_mht",    
    "HLT_xe90_mht_L1XE50",    
    "HLT_xe100_mht_L1XE50",    
    "HLT_xe110_mht_L1XE50",
    "HLT_xe90_pufit_L1XE50",
    "HLT_xe100_pufit_L1XE50",
    "HLT_xe100_pufit_L1XE55",
    "HLT_xe110_pufit_L1XE50",
    "HLT_xe110_pufit_L1XE55",
    "HLT_xe110_pufit_xe70_L1XE50",
    "HLT_xe110_pufit_xe65_L1XE50",
] 

met_triggers_hlt_expert = [
    "HLT_xe30_cell_L1XE10",
    "HLT_xe90_pufit_L1XE50",
    "HLT_xe100_pufit_L1XE55",
]


monitoring_met_shifter = met_triggers_l1_shifter + met_triggers_hlt_shifter
monitoring_met_expert = met_triggers_l1_expert + met_triggers_hlt_expert

monitoring_met_shifter_pp = met_triggers_l1_shifter + met_triggers_hlt_shifter
monitoring_met_expert_pp = met_triggers_l1_expert + met_triggers_hlt_expert

monitoring_met_shifter_cosmic = met_triggers_l1_shifter + met_triggers_hlt_shifter
monitoring_met_expert_cosmic = met_triggers_l1_expert + met_triggers_hlt_expert

monitoring_met = monitoring_met_shifter
monitoring_met_pp = monitoring_met_shifter_pp
monitoring_met_cosmic = monitoring_met_shifter_cosmic

