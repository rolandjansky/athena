# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'Standby_pp_v4'


rules = {
    'L1_RD0_FILLED' : {
    0 : TriggerRule(rate=5000, scaling="bunches", comment=''),
    },
    'L1_ZB' : {
    0 : TriggerRule(PS=-1, comment=''),
    },
    ########### Background triggers ( missing
    'L1_RD0_UNPAIRED_ISO' : {0 : TriggerRule(rate=5, comment=''),
    },
    'L1_LUCID_A_C_EMPTY' : {
    0 : TriggerRule(PS=5000, comment=''),
    },
    'L1_LUCID_A_C_UNPAIRED_ISO' : {
    0 : TriggerRule(PS=900, comment=''),
    },
    'L1_LUCID_A_C_UNPAIRED_NONISO' : {
    0 : TriggerRule(PS=1500, comment=''),
    },
    'L1_LUCID_EMPTY' : {
    0 : TriggerRule(PS=400000, comment=''),
    },
    'L1_LUCID_UNPAIRED_ISO' : {
    0 : TriggerRule(PS=18000, comment=''),
    },
    'L1_LUCID_COMM_UNPAIRED_ISO' : {
    0 : TriggerRule(PS=900, comment=''),
    },
    'L1_LUCID_COMM_EMPTY' : {
    0 : TriggerRule(PS=900, comment=''),
    },
    'L1_MU4_UNPAIRED_ISO' : {
    0 : TriggerRule(PS=900, comment=''),
    },
    'L1_BCM_Wide_EMPTY' : {
    0 : TriggerRule(PS=-1, comment=''),
    },
    'L1_BCM_Wide_UNPAIRED_ISO' : {
    0 : TriggerRule(PS=-1, comment=''),
    },
    'L1_BCM_Wide_UNPAIRED_NONISO' : {
    0 : TriggerRule(PS=-1, comment=''),
    },
    'L1_BCM_AC_CA_BGRP0' : {
    0 : TriggerRule(PS=-1, comment=''),
    },
    'L1_BCM_AC_CA_UNPAIRED_ISO' : {
    0 : TriggerRule(PS=-1, comment=''),
    },
    'L1_EM3_UNPAIRED_ISO' : {
    0 : TriggerRule(PS=22, comment=''),
    },
    'L1_FJ30_UNPAIRED_ISO' : {
    0 : TriggerRule(PS=22, comment=''),
    },
    'L1_J10_UNPAIRED_ISO' : {
    0 : TriggerRule(PS=22, comment=''),
    },
    'L1_J10_UNPAIRED_NONISO' : {
    0 : TriggerRule(PS=22, comment=''),
    },
    'EF_Background' : {
    0 : TriggerRule(PS=1, comment=''),
    },
    'EF_Background_RD0_UNPAIRED_ISO' : {
    0 : TriggerRule(rate=5, comment=''),
    },
    'EF_Background_RD0_EMPTY' : {
    0 : TriggerRule(rate=5, comment=''), 
    },        
    ##################################################
    'L1_EM6' : {
    0 : TriggerRule(rate=5, comment=''),
    },
    'L1_EM30' : {
    0 : TriggerRule(rate=10, comment=''),
    },
    'L1_TAU8' : {
    0 : TriggerRule(rate=5, comment=''),
    },
    'L1_TAU40' : {
    0 : TriggerRule(rate=10, comment=''),
    },
    'L1_J15' : {
    0 : TriggerRule(rate=5, comment=''),
    },
    'L1_J30' : {
    0 : TriggerRule(rate=5, comment=''),
    },
    'L1_J75' : {
    0 : TriggerRule(rate=10, comment=''),
    },
    'L1_J350' : {
    0 : TriggerRule(rate=10, comment=''),
    },
    'L1_FJ30' : {
    0 : TriggerRule(rate=5, comment=''),
    },
    'L1_FJ75' : {
    0 : TriggerRule(rate=10, comment=''),
    },
    'L1_3J15' : {
    0 : TriggerRule(rate=10, comment=''),
    },
    'L1_J30_FJ30' : {
    0 : TriggerRule(rate=5, comment=''),
    },
    'L1_TAU15I_XE35' : { 
    0 : TriggerRule(rate=5, comment=''),
    },
    'L1_MU4' : {
    0 : TriggerRule(rate=1, comment=''),
    },
    'L1_RD0_EMPTY' : {
    0 : TriggerRule(rate=5, comment=''),
    },
    'L1_RD1_EMPTY' : {
    0 : TriggerRule(rate=0.2, scaling="bunches", comment='random in empty bunches'),
    },
    'L1_EM3_EMPTY' : {
    0 : TriggerRule(PS=3, comment=''),
    },
    'L1_EM6_EMPTY' : {
    0 : TriggerRule(PS=1, comment=''),
    },
    'L1_TAU8_EMPTY' : {
    0 : TriggerRule(PS=1, comment=''),
    },
    'L1_J10_EMPTY' : {
    0 : TriggerRule(PS=1, comment=''),
    },
    'L1_J30_EMPTY' : {
    0 : TriggerRule(PS=1, comment=''),
    },
    'L1_FJ30_EMPTY' : {
    0 : TriggerRule(PS=1, comment=''),
    },
    'L1_CALREQ2' : {
    0 : TriggerRule(PS=1, comment=''),
    },

    ## enable 4J10 for unbiased run

#     'L1_4J10' : {
#     0 : TriggerRule(PS=50, comment=''),
#     },
#     'L2_L1Calo_NoAlg' : {
#     0 : TriggerRule(PS=1, comment=''),
#     },
#     'EF_L1Calo_NoAlg' : {
#     0 : TriggerRule(PS=1, comment=''),
#     },
    

    #HLT
    'L2_IDMon_filled' : {
    0 : TriggerRule(inputRate=5, comment=''),
    },
    'L2_IDMon_empty' : {
    0 : TriggerRule(PS=1, comment='inputRate=5, PS=1'),
    },
    'L2_IDMon_unpaired_iso': {
    0: TriggerRule(PS=1, comment='Monitoring - seeded by RD0_unpaired, target 5 Hz'),
    },
    'L2_rd0_empty_NoAlg' : {
    0 : TriggerRule(PS=1, comment='only enabled at L2'),
    },
    'L2_rd0_filled_NoAlg' : {
    0 : TriggerRule(PS=1, comment='enabled at L2'),
    },
    'L2_Background_RD0_EMPTY' : {
    0 : TriggerRule(PS=1, comment='inputRate=5, PS=1'),
    },
    'L2_Standby' : {
    0 : TriggerRule(PS=500, comment='feedeing from L1_RD0_FILLED at 5k Hz'),
    },
    'EF_rd1_empty_NoAlg' : {
    0: TriggerRule(PS=1, comment='streamer for L1_RD1_EMPTY'),
    },
    'EF_PreS_L1EM3_empty' : {
    0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr, during cosmic'),
    },
    'EF_PreS_L1EM6_empty' : {
    0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr'),
    },
    'EF_PreS_L1TAU8_empty' : {
    0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr'),
    },
    'EF_PreS_L1J10_empty' : {
    0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr'),
    },
    'EF_PreS_L1J30_empty' : {
    0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr'),
    },
    'EF_TileCalib_laser' : {
    0: TriggerRule(PS=1, comment='Tile calibration'),
    },
    'EF_g5_NoCut_cosmic' : {
    0: TriggerRule(PS=1, comment='cosmic chain for egamma'),
    },
    'EF_g10_NoCut_cosmic' : {
    0: TriggerRule(PS=1, comment='cosmic chain for egamma'),
    },
    'EF_tauNoCut_cosmic' : {
    0: TriggerRule(PS=1, comment='cosmic chain for taus'),
    },
    'EF_j30_u0uchad_empty_LArNoiseBurst' : {
    0: TriggerRule(PS=1, comment='LArNoiseBurst'),
    },
    'L2_em6_empty_larcalib' : {
    0: TriggerRule(PS=1, comment='cosmic chain for LAr'),
    },
    'L2_tau8_empty_larcalib' : {
    0: TriggerRule(PS=1, comment='cosmic chain for LAr'),
    },
    'L2_j10_empty_larcalib' : {
    0: TriggerRule(PS=1, comment='cosmic chain for LAr'),
    },
    'L2_fj30_empty_larcalib' : {
    0: TriggerRule(PS=1, comment='cosmic chain for LAr'),
    },
    'EF_L1J10_empty_NoAlg' : {
    0: TriggerRule(PS=1, comment='cosmic chain for LAr'),
    },
    'EF_L1J30_empty_NoAlg' : {
    0: TriggerRule(PS=1, comment='cosmic chain for LAr'),
    },
    'EF_L1FJ30_empty_NoAlg' : {
    0: TriggerRule(PS=1, comment='cosmic chain for LAr'),
    },
    'EF_Standby' : {
    0: TriggerRule(PS=1, comment='streamer for LVL1'),
    },
#    'L2_VdM_BGRP7' : {
#    500:TriggerRule(PS=-1, comment='Monitoring'),
#    },
#    'L2_VdM_MBTS_2_BGRP7' : {
#    500:TriggerRule(PS=-1, comment='Monitoring'),
#    },
#    'L2_cost_monitor' : {
#    0: TriggerRule(PS=10000, comment='Cost monitoring'),
#    },
#    'EF_cost_monitor' : {
#    0: TriggerRule(PS=10000, comment='Cost monitoring'),
#    },
    }

# l1_disables = ["L1_2EM10VH", "L1_2EM12", "L1_2EM12_EM16V", "L1_2EM3", "L1_2EM3_EM12", "L1_2EM3_EM6", "L1_2EM6", "L1_2EM6_EM16VH", "L1_2FJ30", "L1_2FJ50", "L1_2MU4_2EM3", "L1_2MU4_EM3", "L1_2MU4_XE30", "L1_2MU4_XE40", "L1_2TAU11", "L1_2TAU11I", "L1_2TAU11I_EM14VH", "L1_2TAU11I_TAU15", "L1_2TAU11_EM10VH", "L1_2TAU11_TAU15", "L1_2TAU11_TAU20_EM10VH", "L1_2TAU11_TAU20_EM14VH", "L1_2TAU15", "L1_2TAU20", "L1_3EM6", "L1_3EM6_EM14VH", "L1_3J10", "L1_3J15", "L1_3J20", "L1_3J50", "L1_4J15", "L1_4J20", "L1_5J10", "L1_5J15", "L1_5J20", "L1_6J10", "L1_EM10VH", "L1_EM10VH_MU6", "L1_EM10VH_XE20", "L1_EM10VH_XE30", "L1_EM10VH_XE35", "L1_EM12", "L1_EM14VH", "L1_EM16V", "L1_EM16VH", "L1_EM16VH_MU4", "L1_EM18VH", "L1_EM3", "L1_EM30", "L1_EM30_BGRP7", "L1_EM3_FIRSTEMPTY", "L1_EM3_MU6", "L1_EM3_UNPAIRED_ISO", "L1_EM3_UNPAIRED_NONISO", "L1_EM6", "L1_FJ100", "L1_FJ30", "L1_FJ30_FIRSTEMPTY", "L1_FJ30_UNPAIRED_ISO", "L1_FJ50", "L1_FJ75", "L1_J10", "L1_J100", "L1_J10_FIRSTEMPTY", "L1_J10_UNPAIRED_ISO", "L1_J10_UNPAIRED_NONISO", "L1_J15", "L1_J20", "L1_J30", "L1_J30_FIRSTEMPTY", "L1_J30_UNPAIRED_ISO", "L1_J30_UNPAIRED_NONISO", "L1_J350", "L1_J50", "L1_J75", "L1_JE140", "L1_JE200", "L1_JE350", "L1_JE500", "L1_MU4_J10", "L1_MU4_J15", "L1_MU4_J50", "L1_MU6_2J20", "L1_MU6_J15", "L1_TAU11", "L1_TAU11I", "L1_TAU15", "L1_TAU15I", "L1_TAU15_XE25_3J10", "L1_TAU15_XE25_3J10_J30", "L1_TAU15_XE25_3J15", "L1_TAU15_XE35", "L1_TAU15_XE40", "L1_TAU15_XS25_3J10", "L1_TAU15_XS35", "L1_TAU20", "L1_TAU40", "L1_TAU8", "L1_TAU8_FIRSTEMPTY", "L1_TAU8_UNPAIRED_ISO", "L1_TAU8_UNPAIRED_NONISO", "L1_TE20", "L1_TE300", "L1_TE500", "L1_TE700", "L1_TE800", "L1_XE20", "L1_XE25", "L1_XE30", "L1_XE35", "L1_XE40", "L1_XE40_BGRP7", "L1_XE50", "L1_XE50_BGRP7", "L1_XE60", "L1_XE70", "L1_XS25", "L1_XS30", "L1_XS35", "L1_XS40", "L1_XS45", "L1_XS50", "L1_XS60", "L1_XS65"]

# for l1_name in l1_disables:
#     rules[l1_name] = {0 : TriggerRule(PS=-1) }

if __name__ == "__main__":
    print "tag:   %s" % tag
    for item in sorted(rules.keys()):
        print "\n", item
        for point in rules[item]:
            print "    %20s %s" % (point, rules[item][point])
            
