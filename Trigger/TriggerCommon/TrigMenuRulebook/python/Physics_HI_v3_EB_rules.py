# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'Physics_HI_v3_EB'

 
#=======================================

# 9 chains, 100 Hz total, aim to get 11 Hz each
# Set PS to be put in at 2e27 

stream_eb_rules = {
    'HLT_noalg_eb_L1ZDC_A_C_VTE50':       { 0.009 : TriggerRule(PS=16,   comment='EnhancedBias chain') , 
                                            0.01  : TriggerRule(PS=-1,   comment='EnhancedBias chain') , 
                                            },
    'HLT_noalg_eb_L1TE5_VTE200':          { 0.009 : TriggerRule(PS=200,  comment='EnhancedBias chain') ,
                                            0.01  : TriggerRule(PS=-1,  comment='EnhancedBias chain') ,
                                            },
    'HLT_noalg_eb_L1MBTS_1_1':            { 0.009 : TriggerRule(PS=200,  comment='EnhancedBias chain') ,
                                            0.01  : TriggerRule(PS=-1, comment='EnhancedBias chain') ,
                                            },
    'HLT_noalg_eb_L1ZDC_XOR_TE5_VTE200':  { 0.009 : TriggerRule(PS=6.1,  comment='EnhancedBias chain') ,
                                            0.01  : TriggerRule(PS=-1,  comment='EnhancedBias chain') ,
                                            },
    'HLT_noalg_eb_L1MU0_VTE50':           { 0.009 : TriggerRule(PS=12.6, comment='EnhancedBias chain') ,
                                            0.01  : TriggerRule(PS=-1, comment='EnhancedBias chain') ,
                                            },
    'HLT_noalg_eb_L1EM12':                { 0.009 : TriggerRule(PS=46.4, comment='EnhancedBias chain') , 
                                            0.01  : TriggerRule(PS=-1, comment='EnhancedBias chain') , 
                                            },
    'HLT_noalg_eb_L1MU4':                 { 0.009 : TriggerRule(PS=57.7, comment='EnhancedBias chain') ,
                                            0.01  : TriggerRule(PS=-1, comment='EnhancedBias chain') ,
                                            },
    'HLT_noalg_eb_L1TE50':                { 0.009 : TriggerRule(PS=617,  comment='EnhancedBias chain') ,
                                            0.01  : TriggerRule(PS=-1,  comment='EnhancedBias chain') ,
                                            },
    'HLT_noalg_eb_L1TE12000':             { 0.009 : TriggerRule(PS=46.4, comment='EnhancedBias chain') ,
                                            0.01  : TriggerRule(PS=-1, comment='EnhancedBias chain') ,
                                            },
    }

