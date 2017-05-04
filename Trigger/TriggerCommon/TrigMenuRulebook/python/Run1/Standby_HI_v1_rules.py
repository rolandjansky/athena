# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'Standby_HI_v1'


rules = {

    #============================
    # L1 items
    #============================
    'L1_RD0_FILLED' : 	 { 0 : TriggerRule( PS=2500000, comment='',)}, 
    'L1_RD1_FILLED' : 	 { 0 : TriggerRule( PS=10000000, comment='',)}, 
    'L1_RD1_EMPTY' : 	 { 0 : TriggerRule( PS=5000000, comment='random in empty bunches',)}, 

    'L1_MBTS_2_2' : 	 { 0 : TriggerRule( PS=40, comment='',)}, 
    'L1_LUCID' : 	 { 0 : TriggerRule( PS=10, comment='',)}, 
    'L1_MBTS_4_4' : 	 { 0 : TriggerRule( PS=100000, comment='',)}, 

    'L1_ZDC' : 	 { 0 : TriggerRule( PS=40000000, comment='',)}, 

    'L1_ZB' : 	 { 0 : TriggerRule( PS=100, comment='',)}, 

    #'L1_LUCID' : 	 { 0 : TriggerRule( rate=75000000, comment='',)}, 

    'L1_BCM_Wide_BGRP0' : 	 { 0 : TriggerRule( PS=6000000, comment='',)}, 

    'L1_EM3' : 	 { 0 : TriggerRule( PS=40000, comment='',)}, 
    'L1_EM5' : 	 { 0 : TriggerRule( PS=5000, comment='',)}, 
    
    'L1_2EM3' :  { 0 : TriggerRule( PS=500, comment='',)}, 
    'L1_2EM5' :  { 0 : TriggerRule( PS=10, comment='',)}, 


    'L1_J10' : 	 { 0 : TriggerRule( PS=40000, comment='',)}, 
    'L1_J15' : 	 { 0 : TriggerRule( PS=35000, comment='',)}, 
    'L1_J30' : 	 { 0 : TriggerRule( PS=10000, comment='',)}, 
    'L1_J50' : 	 { 0 : TriggerRule( PS=3000, comment='',)}, 
    'L1_J75' : 	 { 0 : TriggerRule( PS=10, comment='',)}, 
    'L1_J250' :  { 0 : TriggerRule( PS=10, comment='',)}, 
    
    'L1_FJ10' :  { 0 : TriggerRule( PS=10000, comment='',)}, 
    
    #'L1_XE10' :  { 0 : TriggerRule( PS=60000, comment='',)}, 
    #'L1_XE20' :  { 0 : TriggerRule( PS=2000, comment='',)}, 
    #'L1_XE30' :  { 0 : TriggerRule( PS=250, comment='',)}, 
    #'L1_XE40' :  { 0 : TriggerRule( PS=10, comment='',)}, 
    #'L1_XE50' :  { 0 : TriggerRule( PS=10, comment='',)}, 

    #'L1_XS25' :  { 0 : TriggerRule( PS=500, comment='',)}, 
    #'L1_XS30' :  { 0 : TriggerRule( PS=200, comment='',)}, 
    #'L1_XS35' :  { 0 : TriggerRule( PS=100, comment='',)}, 
    #'L1_XS40' :  { 0 : TriggerRule( PS=10, comment='',)}, 
    #'L1_XS45' :  { 0 : TriggerRule( PS=10, comment='',)}, 
    #'L1_XS50' :  { 0 : TriggerRule( PS=10, comment='',)}, 

    #TE items?
    
    'L1_MU4' : 	 { 0 : TriggerRule( PS=1000, comment='',)}, 
    'L1_MU11' :  { 0 : TriggerRule( PS=100, comment='',)}, 


    'L1_EM3_EMPTY' : 	 { 0 : TriggerRule( PS=1, comment='',)}, 
    'L1_EM5_EMPTY' : 	 { 0 : TriggerRule( PS=1, comment='',)}, 
    'L1_TAU8_EMPTY' : 	 { 0 : TriggerRule( PS=1, comment='',)}, 
    'L1_J10_EMPTY' : 	 { 0 : TriggerRule( PS=1, comment='',)}, 
    'L1_FJ10_EMPTY' : 	 { 0 : TriggerRule( PS=1, comment='',)}, 
    'L1_J30_EMPTY' : 	 { 0 : TriggerRule( PS=1, comment='',)}, 

    'L1_CALREQ2' : 	 { 0 : TriggerRule( PS=1, comment='',)}, 


    #?'L1_TE5' : 	 { 0 : TriggerRule( PS=1, comment='',)}, 
    #?'L1_TE10' : 	 { 0 : TriggerRule( PS=1, comment='',)}, 
    #?'L1_TE20' : 	 { 0 : TriggerRule( PS=1, comment='',)}, 
    #?'L1_TE30' : 	 { 0 : TriggerRule( PS=1, comment='',)}, 
    #?'L1_TE40' : 	 { 0 : TriggerRule( PS=1, comment='',)}, 
    #?'L1_TE50' : 	 { 0 : TriggerRule( PS=1, comment='',)}, 
    #?'L1_TE2000' : 	 { 0 : TriggerRule( PS=1, comment='',)}, 
    #?'L1_TE4000' : 	 { 0 : TriggerRule( PS=1, comment='',)}, 


    #?'L1_EM4' : 	 { 0 : TriggerRule( PS=1, comment='',)}, 
    #?'L1_EM7' : 	 { 0 : TriggerRule( PS=1, comment='',)}, 
    #?'L1_EM10' :  { 0 : TriggerRule( PS=1, comment='',)}, 
    #?'L1_EM12' :  { 0 : TriggerRule( PS=1, comment='',)}, 
    #?'L1_EM14' :  { 0 : TriggerRule( PS=1, comment='',)}, 
    #?'L1_EM16' :  { 0 : TriggerRule( PS=1, comment='',)}, 
    #?'L1_J20' : 	 { 0 : TriggerRule( PS=1, comment='',)}, 
    #?'L1_J175' :  { 0 : TriggerRule( PS=1, comment='',)}, 
    #?'L1_XE15' :  { 0 : TriggerRule( PS=60000, comment='',)}, 
    #?'L1_XE25' :  { 0 : TriggerRule( PS=60000, comment='',)}, 
    #?'L1_XE35' :  { 0 : TriggerRule( PS=60000, comment='',)}, 
    #?'L1_XE60' :  { 0 : TriggerRule( PS=60000, comment='',)}, 
    #?'L1_XE65' :  { 0 : TriggerRule( PS=60000, comment='',)}, 
    #?'L1_MU15' :  { 0 : TriggerRule( PS=1, comment='',)}, 
    #?'L1_MU20' :  { 0 : TriggerRule( PS=1, comment='',)}, 
    #?'L1_MU6' : 	 { 0 : TriggerRule( PS=1, comment='',)},     
    #?'L1_2MU0' :  { 0 : TriggerRule( PS=1, comment='',)}, 
    #?'L1_2MU0_MU6' : 	 { 0 : TriggerRule( PS=1, comment='',)}, 
    #?'L1_2MU11' : { 0 : TriggerRule( PS=1, comment='',)}, 
    #?'L1_2MU20' : { 0 : TriggerRule( PS=1, comment='',)}, 
    #?'L1_2MU6' :  { 0 : TriggerRule( PS=1, comment='',)}, 
    #?'L1_3MU6' :  { 0 : TriggerRule( PS=1, comment='',)}, 
    
   

    #=========================

    
    #HLT
#    'EF_rd0_empty_NoAlg' : {
#    0: TriggerRule(PS=1, ESValue=-1, comment='streamer for L1_RD0_EMPTY'),
#    },
    
    'EF_rd1_empty_NoAlg' : { 0: TriggerRule(PS=1, comment='streamer for L1_RD1_EMPTY'),},

    'EF_PreS_L1EM3_empty' : {0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr, during cosmic'),},
    'EF_PreS_L1EM5_empty' : {    0: TriggerRule(PS=-1, comment='rejection of pre-sampler noise in LAr'),    },
    'EF_PreS_L1TAU8_empty' : {    0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr'),    },
    'EF_PreS_L1J10_empty' : {    0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr'),    },
    'EF_PreS_L1J30_empty' : {    0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr'),    },

    'EF_TileCalib_laser' : {    0: TriggerRule(PS=1, comment='Tile calibration'),    },           #seeds off L1_CALREQ2
    'EF_g5_NoCut_cosmic' : {    0: TriggerRule(PS=1, comment='cosmic chain for egamma'),    },    #seeds off L1_EM3_EMPTY
    'EF_tauNoCut_cosmic' : {    0: TriggerRule(PS=1, comment='cosmic chain for taus'),    },      #seeds off L1_TAU8_EMPTY

    'L2_em5_empty_larcalib' : {    0: TriggerRule(PS=1, comment='cosmic chain for LAr'),    },
    'L2_tau8_empty_larcalib' : {    0: TriggerRule(PS=1, comment='cosmic chain for LAr'),    },
    'L2_j10_empty_larcalib' : {    0: TriggerRule(PS=1, comment='cosmic chain for LAr'),    },
    'L2_fj10_empty_larcalib' : {    0: TriggerRule(PS=1, comment='cosmic chain for LAr'),    },
    #'EF_L1J10_empty_NoAlg' : {    0: TriggerRule(PS=1, comment='cosmic chain for LAr'),    },
    #'EF_L1J30_empty_NoAlg' : {    0: TriggerRule(PS=1, comment='cosmic chain for LAr'),    },
    #'EF_L1FJ10_empty_NoAlg' : {0: TriggerRule(PS=1, comment='cosmic chain for LAr'),    },

    'EF_Standby' : {0: TriggerRule(PS=2000, comment='streamer for LVL1'),    },

    'L2_TRTMon_empty':  { 0:TriggerRule(rate=1, comment='Monitoring'),},
    'L2_TRTMon_filled': { 0:TriggerRule(PS=1, comment='Monitoring'), },
    'L2_VdM_BGRP7' : { 500:TriggerRule(PS=-1, comment='Monitoring'),},
    'L2_VdM_MBTS_2_BGRP7' : { 500:TriggerRule(PS=-1, comment='Monitoring'),},

#    'L2_cost_monitor' : {
#    0: TriggerRule(PS=10000, comment='Cost monitoring'),
#    },
#    'EF_cost_monitor' : {
#    0: TriggerRule(PS=10000, comment='Cost monitoring'),
#    },
    }

if __name__ == "__main__":
    print "tag:   %s" % tag
    for item in sorted(rules.keys()):
        print "\n", item
        for point in rules[item]:
            print "    %20s %s" % (point, rules[item][point])
            
