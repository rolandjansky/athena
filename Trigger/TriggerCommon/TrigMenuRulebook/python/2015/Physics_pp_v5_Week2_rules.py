# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule


tag = 'Week2_rules'
doALFA=True
ScaleMu = 3 # the default is for 0.01
doHI50Hz=False

rules = {
    # low lumi main L1 streamer
    'HLT_noalg_L1LowLumi'    : {   1 : TriggerRule(PS=1, comment='Main', ESValue=1) },
}

L1_lowlumi=[
    'L1_2EM10VH',
    'L1_2EM8VH_MU10',
    'L1_2J15_XE55',
    'L1_2MU6',
    'L1_3J25.0ETA23',
    'L1_3J40',
    'L1_3MU4',
    'L1_4J15',
    'L1_EM15HI_2TAU12IM',
    'L1_EM15HI_2TAU12IM_J25_3J12',
    'L1_EM15HI_2TAU12IM_XE35',
    'L1_EM15HI_TAU40_2TAU15',
    'L1_EM15VH_MU10',
    'L1_EM15_MU4',
    'L1_EM20VH',
    'L1_EM7_MU10',
    'L1_J75',
    'L1_J75.32ETA49',
    'L1_MU10_TAU12IM',
    'L1_MU10_TAU12IM_J25_2J12',
    'L1_MU10_TAU12IM_XE35',
    'L1_MU10_TAU12_J25_2J12',
    'L1_MU10_TAU20',
    'L1_MU15',
    'L1_MU4_3J20',
    'L1_MU6_J40',
    'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',
    'L1_TAU20IM_2TAU12IM_XE35',
    'L1_TAU20_2J20_XE45',
    'L1_TAU20_2TAU12_XE35',
    'L1_TAU60',
    'L1_XE50' ]

L1_lowlumi_Rules=dict(map(None,L1_lowlumi,len(L1_lowlumi)*[{
                         1 : TriggerRule(PS=1, comment='L1'), 
                       }]))

rules=dict(rules,**L1_lowlumi_Rules)


L1Calo_Rules={    
    'L1_EM12'                     : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_EM13VH'                   : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_EM15'                     : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_EM15VH'                   : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_EM18VH'                   : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_J25'                      : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_J30'                      : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_J30.32ETA49'              : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_J40'                      : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_J50'                      : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_J50.32ETA49'              : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_TAU20'                    : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_TAU20IL'                  : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_TAU20IM'                  : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_TAU20IT'                  : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_TAU30'                    : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_TAU40'                    : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_XE35'                     : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_XE40'                     : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_XE45'                     : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_XS30'                     : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_XS40'                     : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_XS50'                     : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_XS60'                     : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },

    'HLT_noalg_L1EM12'                     : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1EM13VH'                   : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1EM15'                     : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1EM15VH'                   : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1EM18VH'                   : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1J25'                      : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1J30'                      : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1J30.32ETA49'              : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1J40'                      : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1J50'                      : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1J50.32ETA49'              : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1TAU20'                    : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1TAU20IL'                  : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1TAU20IM'                  : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1TAU20IT'                  : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1TAU30'                    : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1TAU40'                    : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    #'HLT_noalg_L1XE35'                     : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1XE40'                     : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1XE45'                     : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1XS30'                     : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1XS40'                     : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1XS50'                     : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1XS60'                     : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },

    'L1_EM10'                              : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    'L1_EM10VH'                            : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    'L1_EM3'                               : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    'L1_EM7'                               : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    'L1_EM8VH'                             : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    'L1_J12'                               : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    'L1_J15'                               : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    'L1_J15.29ETA32'                       : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    'L1_J15.32ETA49'                       : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    'L1_J20'                               : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    'L1_J20.29ETA32'                       : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    'L1_J20.32ETA49'                       : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    'L1_TAU12'                             : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    'L1_TAU12IL'                           : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    'L1_TAU12IM'                           : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    'L1_TAU12IT'                           : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    'L1_TAU8'                              : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    #'L1_TE10'                              : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    #'L1_TE10.0ETA25'                       : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    #'L1_TE20'                              : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    #'L1_TE20.0ETA25'                       : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    'L1_TE30'                              : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    #'L1_TE30.0ETA25'                       : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    'L1_TE40'                              : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    'L1_TE50'                              : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },
    'L1_XS20'                              : { 10000 : TriggerRule(PS=1000*ScaleMu, comment='L1', rerun=0) },

    'HLT_noalg_L1EM10'                              : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1EM10VH'                            : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1EM3'                               : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1EM7'                               : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1EM8VH'                             : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1J12'                               : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1J15'                               : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1J15.29ETA32'                       : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1J15.32ETA49'                       : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1J20'                               : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1J20.29ETA32'                       : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1J20.32ETA49'                       : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1TAU12'                             : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1TAU12IL'                           : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1TAU12IM'                           : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1TAU12IT'                           : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1TAU8'                              : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1TE10'                              : { 10000 : TriggerRule(PS=200, comment='L1Calo streamer') },
    'HLT_noalg_L1TE10.0ETA25'                       : { 10000 : TriggerRule(PS=200, comment='L1Calo streamer') },
    #'HLT_noalg_L1TE20'                              : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    #'HLT_noalg_L1TE20.0ETA25'                       : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1TE30'                              : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    #'HLT_noalg_L1TE30.0ETA25'                       : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1TE40'                              : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1TE50'                              : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    'HLT_noalg_L1XS20'                              : { 10000 : TriggerRule(PS=1, comment='L1Calo streamer') },
    
}

rules=dict(rules,**L1Calo_Rules)

#################################################################################################
#
# minbias rules
#
# NOTE:
# For L1 MBTS, fixed to PS=1. 
# For HLT mbts, tentatively set to PS=1 and comment target rate such that PS values can be adjusted later.
# Might want do the same for TE and LUCID
#
#################################################################################################


minbias_Rules = {

    ########################################
    # Level-1

    # unprescale MBTS fixed unPS
    'L1_MBTS_1'                                               : {   1 : TriggerRule(PS=1, comment='Fix L1PS=1') },
    'L1_MBTS_2'                                               : {   1 : TriggerRule(PS=1, comment='Fix L1PS=1') },
    'L1_MBTS_1_1'                                             : {   1 : TriggerRule(PS=1, comment='Fix L1PS=1') },
    'L1_MBTS_2_EMPTY'                                         : {   1 : TriggerRule(PS=1, comment='Fix L1PS=1') },
    'L1_MBTS_1_UNPAIRED_ISO'                                  : {   1 : TriggerRule(PS=1, comment='Fix L1PS=1') }, 
    'L1_MBTS_2_UNPAIRED_ISO'                                  : {   1 : TriggerRule(PS=-1, comment='Fix L1PS=1') },
    
    # random
    # mb_sptrk 20 Hz with mu=0.01 -> random=2 kHz  
    'L1_RD0_FILLED'                                           : {   1 : TriggerRule(rate=2000, scaling='bunches', comment='L1') },
    'L1_RD0_UNPAIRED_ISO'                                     : {   1 : TriggerRule(rate=100,  scaling='bunches', comment='L1') },
    'L1_RD0_EMPTY'                                            : {   1 : TriggerRule(rate=100,  scaling='bunches', comment='L1') },

    # TE: set 5 kHz for the moment
    'L1_TE10'                                                 : {   1 : TriggerRule(PS=1, comment='L1') }, 
    'L1_TE10.0ETA25'                                          : {   1 : TriggerRule(PS=1, comment='L1') }, 
    'L1_TE20'                                                 : {   1 : TriggerRule(PS=1, comment='L1') },
    'L1_TE20.0ETA25'                                          : {   1 : TriggerRule(PS=1, comment='L1') }, 
   
    ########################################
    # Streamers

    # XE streamer
    'HLT_noalg_L1XE35'                                        : {   1 : TriggerRule(PS=1*ScaleMu, comment='Target 0.5', ESValue=1) }, 
    
    # MBTS streamer 
    # main 750Hz (with PS=5) 
    'HLT_noalg_mb_L1MBTS_1'                                   : {   1 : TriggerRule(PS=10, comment='L1PS=1. Target 1000Hz)') }, #PS=1 gives 1.32kHz with mu0.01 and 12 bunches  # updated
    'HLT_noalg_mb_L1RD0_FILLED'                               : {   1 : TriggerRule(rate=100, comment='L1PS=1. Target 100Hz', ESValue=200) },

    'HLT_noalg_mb_L1MBTS_1_EMPTY'                             : {   1 : TriggerRule(PS=2000*ScaleMu, comment='For covern background. no beam rate O(10kHz). Target 5Hz') },
    'HLT_noalg_mb_L1MBTS_1_UNPAIRED_ISO'                      : {   1 : TriggerRule(PS=1*ScaleMu, comment='Primary') }, # no beam rate 4 Hz per unpaired BCID. A factor of 100 less than the paired rate. 

    'HLT_noalg_mb_L1MBTS_1_1'                                 : {   1 : TriggerRule(PS=2000, comment='L1PS=1. Target 2Hz', ESValue=1) }, # updated
    'HLT_noalg_mb_L1MBTS_2'                                   : {   1 : TriggerRule(PS=8000*ScaleMu, comment='L1PS=1. Target 0.5Hz', ESValue=1) },
    
    'HLT_noalg_mb_L1MBTS_2_EMPTY'                             : {   1 : TriggerRule(PS=1*ScaleMu, comment='Proxy for MBTS_1_EMPTY. L1PS=1. Target 5Hz') },# lower PS. no beam rate 2Hz
    'HLT_noalg_mb_L1MBTS_2_UNPAIRED_ISO'                      : {   1 : TriggerRule(PS=-1, comment='OK, given PS on noalg_mb_L1MBTS_1_UNPAIRED_ISO is small.') },

    # backups: disabled    
    'HLT_noalg_mb_L1MBTS_1_1_EMPTY'                           : {   1 : TriggerRule(PS=-1, comment='') },
    'HLT_noalg_mb_L1MBTS_1_1_UNPAIRED_ISO'                    : {   1 : TriggerRule(PS=-1, comment='') },

    # supporting: random streamers
    'HLT_noalg_mb_L1RD0_EMPTY'                                : {   1 : TriggerRule(rate=1, comment='') },
    'HLT_noalg_mb_L1RD0_UNPAIRED_ISO'                         : {   1 : TriggerRule(rate=1, comment='') },

    # supporting: LUCID streamer
    'HLT_noalg_mb_L1LUCID'                                    : {   1 : TriggerRule(PS=120*ScaleMu, comment='Target 10Hz') },
    # kn: rate to be sorted out
    'HLT_noalg_mb_L1LUCID_EMPTY'                              : {   1 : TriggerRule(PS=1*ScaleMu, comment='Target 1Hz') }, 
    'HLT_noalg_mb_L1LUCID_UNPAIRED_ISO'                       : {   1 : TriggerRule(PS=1*ScaleMu, comment='Target 1Hz') }, 

    ########################################
    # HLT

    # -----------------------------
    # supporting: sptrk seeded from random
    'HLT_mb_sptrk'                                            : {   1 : TriggerRule(PS=1, comment='Target 20Hz in main, 0.5 Hz in ES', ESValue=40) }, 
    # kn: rate to be sorted out
    'HLT_mb_sptrk_L1RD0_UNPAIRED_ISO'                         : {   1 : TriggerRule(rate=1, comment='') },
    'HLT_mb_sptrk_L1RD0_EMPTY'                                : {   1 : TriggerRule(rate=1, comment='') },

    # supporting:
    'HLT_mb_perf_L1MBTS_2'                                    : {   1 : TriggerRule(PS=5000*ScaleMu, comment='', ESValue=1) },
    'HLT_mb_perf_L1RD1_FILLED'                                : {   1 : TriggerRule(rate=10, comment='') },

    # inclusive te
    'HLT_te50_L1MBTS_1_1'                                     : {   1 : TriggerRule(PS=-1, comment='Iwona not need it') },
    }

rules=dict(rules,**minbias_Rules)



if doHI50Hz:
    print "HI50Hz"
    
    minbias_hmt50Hz_Rules = {
    # HMT - MBTS seeded
    # backup: high thresholds
    ###'HLT_mb_sp900_trk60_hmt_L1MBTS_1_1'                       : {   1 : TriggerRule(PS=1, comment='') }, # SWITCH
    ###'HLT_mb_sp900_trk65_hmt_L1MBTS_1_1'                       : {   1 : TriggerRule(PS=1, comment='') }, # SWITCH
    ###'HLT_mb_sp1000_trk70_hmt_L1MBTS_1_1'                      : {   1 : TriggerRule(PS=1, comment='') }, # SWITCH 
    ###'HLT_mb_sp1200_trk75_hmt_L1MBTS_1_1'                      : {   1 : TriggerRule(PS=1, comment='') }, # SWITCH 
    ###'HLT_mb_sp1400_trk80_hmt_L1MBTS_1_1'                      : {   1 : TriggerRule(PS=1, comment='') }, # SWITCH 
    # TE-based HMT - MBTS seeded 
    # backup: high thresholds
    ###'HLT_mb_sp300_trk10_sumet60_hmt_L1MBTS_1_1'               : {   1 : TriggerRule(PS=1, comment='') }, # SWITCH 
    ###'HLT_mb_sp600_trk10_sumet60_hmt_L1MBTS_1_1'               : {   1 : TriggerRule(PS=1, comment='') }, # SWITCH 
    ###'HLT_mb_sp300_trk10_sumet70_hmt_L1MBTS_1_1'               : {   1 : TriggerRule(PS=1, comment='') }, #SWITCH 
    ###'HLT_mb_sp600_trk10_sumet70_hmt_L1MBTS_1_1'               : {   1 : TriggerRule(PS=1, comment='') }, #SWITCH 
    ###'HLT_mb_sp300_trk10_sumet80_hmt_L1MBTS_1_1'               : {   1 : TriggerRule(PS=1, comment='') }, #SWITCH 
    ###'HLT_mb_sp600_trk10_sumet80_hmt_L1MBTS_1_1'               : {   1 : TriggerRule(PS=1, comment='') }, #SWITCH
    ####'HLT_mb_sp300_trk10_sumet90_hmt_L1MBTS_1_1'               : {   1 : TriggerRule(PS=1, comment='') }, #SWITCH not found in SMK
    ####'HLT_mb_sp600_trk10_sumet90_hmt_L1MBTS_1_1'               : {   1 : TriggerRule(PS=1, comment='') }, #SWITCH not found in SMK
    
    ###'HLT_mb_sp1400_trk90_hmt_L1TE10'                          : {   1 : TriggerRule(PS=1, comment='') }, #SWITCH
    ###'HLT_mb_sp1400_trk90_hmt_L1TE10.0ETA25'                   : {   1 : TriggerRule(PS=1, comment='') }, #SWITCH
    ###'HLT_mb_sp1400_trk100_hmt_L1TE20'                         : {   1 : TriggerRule(PS=1, comment='') }, #SWITCH
    ###'HLT_mb_sp1400_trk100_hmt_L1TE20.0ETA25'                  : {   1 : TriggerRule(PS=1, comment='') }, #SWITCH

    ###'HLT_noalg_mb_L1TE20'                                     : {   1 : TriggerRule(PS=100*ScaleMu, comment='Target 2Hz', ESValue=1) }, #SWITCH
    
    # -----------------------------
    # supporting (for intermediate-mu run)
    ###'HLT_mb_sp1400_hmtperf_L1TE10'                            : {   1 : TriggerRule(PS=1, comment='') }, #SWITCH 
    ###'HLT_mb_sp1400_hmtperf_L1TE20'                            : {   1 : TriggerRule(PS=1, comment='') }, #SWITCH 
    ###'HLT_mb_sp1400_hmtperf_L1TE10.0ETA25'                     : {   1 : TriggerRule(PS=1, comment='') }, #SWITCH 
    ###'HLT_mb_sp1400_hmtperf_L1TE20.0ETA25'                     : {   1 : TriggerRule(PS=1, comment='') }, #SWITCH 
    ###'HLT_mb_sp2500_hmtperf_L1TE20'                            : {   1 : TriggerRule(PS=1, comment='') }, #SWITCH 
    ####'HLT_mb_sp1800_hmtperf_L1TE30.0ETA25'                     : {   1 : TriggerRule(PS=1, comment='') }, #SWITCH # Need new SMK
    ###'HLT_mb_sp2_hmtperf_L1MBTS_1_1'                           : {   1 : TriggerRule(PS=1000*ScaleMu, comment='Target 5Hz. Need to tune PS.') }, #SWITCH
        
    # -----------------------------
    # inclusive track trigger
    ###'HLT_mb_sptrk_pt4_L1MBTS_1'                               : {   1 : TriggerRule(PS=1*ScaleMu, comment='Target 250 Hz') }, 
    ###'HLT_mb_sptrk_pt6_L1MBTS_1'                               : {   1 : TriggerRule(PS=1*ScaleMu, comment='') },
    ###'HLT_mb_sptrk_pt8_L1MBTS_1'                               : {   1 : TriggerRule(PS=1*ScaleMu, comment='') },


        
        }
    rules=dict(rules,**minbias_hmt50Hz_Rules)
    
else:

    minbias_hmt300HzMu003_Rules = {
        'HLT_mb_sp700_trk50_hmt_L1MBTS_1_1' : { 1 : TriggerRule(PS=1, comment='Updated from PS=10')}, 
        'HLT_mb_sp700_trk55_hmt_L1MBTS_1_1' : { 1 : TriggerRule(PS=1, comment='Updated from PS=7')}, 
        'HLT_mb_sp900_trk60_hmt_L1MBTS_1_1' : { 1 : TriggerRule(PS=1, comment='')}, 
        'HLT_mb_sp900_trk65_hmt_L1MBTS_1_1' : { 1 : TriggerRule(PS=1, comment='')},
        'HLT_mb_sp1000_trk70_hmt_L1MBTS_1_1' : { 1 : TriggerRule(PS=1, comment='') },
        'HLT_mb_sp1200_trk75_hmt_L1MBTS_1_1' : { 1 : TriggerRule(PS=1, comment='')}, 
        'HLT_mb_sp1400_trk80_hmt_L1MBTS_1_1' : { 1 : TriggerRule(PS=1, comment='')}, 
        'HLT_mb_sp1400_trk90_hmt_L1TE10' : { 1 : TriggerRule(PS=1, comment='')}, 
        'HLT_mb_sp1400_trk90_hmt_L1TE10.0ETA25' : { 1 : TriggerRule(PS=1, comment='')}, 
        'HLT_mb_sp1400_trk100_hmt_L1TE20' : { 1 : TriggerRule(PS=1, comment='')}, 
        'HLT_mb_sp1400_trk100_hmt_L1TE20.0ETA25' : { 1 : TriggerRule(PS=1, comment='')}, 
        'HLT_mb_sp300_trk10_sumet80_hmt_L1MBTS_1_1' : { 1 : TriggerRule(PS=30, comment='Target 10Hz')}, 
        'HLT_mb_sp600_trk10_sumet80_hmt_L1MBTS_1_1' : { 1 : TriggerRule(PS=-1, comment='Originally PS=10. Requested by Iwona')}, 
        'HLT_mb_sp1400_hmtperf_L1TE10' : { 1 : TriggerRule(PS=10, comment='')}, 
        'HLT_mb_sp1400_hmtperf_L1TE10.0ETA25' : { 1 : TriggerRule(PS=10, comment='')}, 
        'HLT_mb_sp1400_hmtperf_L1TE20' : { 1 : TriggerRule(PS=1, comment='')}, 
        'HLT_mb_sp1400_hmtperf_L1TE20.0ETA25' : { 1 : TriggerRule(PS=1, comment='')}, 
        'HLT_mb_sp2500_hmtperf_L1TE20' : { 1 : TriggerRule(PS=1, comment='')}, 
        #'HLT_mb_sp1800_hmtperf_L1TE30.0ETA25' : { 1 : TriggerRule(, comment='')}, 
        'HLT_noalg_mb_L1TE10' : { 1 : TriggerRule(PS=40, comment='Rate=5Hz')}, 
        'HLT_noalg_mb_L1TE20' : { 1 : TriggerRule(PS=10, ESValue=10, comment='Rate=5Hz')}, 
        'HLT_noalg_mb_L1TE30' : { 1 : TriggerRule(PS=1, comment='Watch. Rate=5Hz')}, 
        'HLT_noalg_mb_L1TE10.0ETA25' : { 1 : TriggerRule(PS=1, comment='Rate=5Hz')}, 
        'HLT_noalg_mb_L1TE20.0ETA25' : { 1 : TriggerRule(PS=1, comment='Rate=5Hz')}, 
        #'HLT_noalg_mb_L1TE30.0ETA25' : { 1 : TriggerRule(rate=5Hz, comment='')}, 
        # inclusive track trigger
        'HLT_mb_sptrk_pt4_L1MBTS_1'                               : {   1 : TriggerRule(PS=10, comment='Target 250 Hz') }, 
        'HLT_mb_sptrk_pt6_L1MBTS_1'                               : {   1 : TriggerRule(PS=1, comment='') },
        'HLT_mb_sptrk_pt8_L1MBTS_1'                               : {   1 : TriggerRule(PS=1, comment='') },
        }
    
    minbias_hmt300Hz_Rules = {    
        ### BBB 'HLT_mb_sp700_trk50_hmt_L1MBTS_1_1'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp700_trk55_hmt_L1MBTS_1_1'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp900_trk60_hmt_L1MBTS_1_1'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp900_trk65_hmt_L1MBTS_1_1'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp1000_trk70_hmt_L1MBTS_1_1'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp1200_trk75_hmt_L1MBTS_1_1'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp1400_trk80_hmt_L1MBTS_1_1'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp1400_trk90_hmt_L1TE10'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp1400_trk90_hmt_L1TE10.0ETA25'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp1400_trk100_hmt_L1TE20'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp1400_trk100_hmt_L1TE20.0ETA25'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp300_trk10_sumet50_hmt_L1MBTS_1_1'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp600_trk10_sumet50_hmt_L1MBTS_1_1'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp300_trk10_sumet60_hmt_L1MBTS_1_1'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp600_trk10_sumet60_hmt_L1MBTS_1_1'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp300_trk10_sumet70_hmt_L1MBTS_1_1'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp600_trk10_sumet70_hmt_L1MBTS_1_1'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp300_trk10_sumet80_hmt_L1MBTS_1_1'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp600_trk10_sumet80_hmt_L1MBTS_1_1'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp1400_hmtperf_L1TE10'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp1400_hmtperf_L1TE10.0ETA25'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp1400_hmtperf_L1TE20'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp1400_hmtperf_L1TE20.0ETA25'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp2500_hmtperf_L1TE20'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        #### BBB 'HLT_mb_sp1800_hmtperf_L1TE30.0ETA25'          : { 10000 : TriggerRule(PS=1, comment='Main') },
        ### BBB 'HLT_mb_sp2_hmtperf_L1MBTS_1_1'          : { 10000 : TriggerRule(PS=1000, comment='Main. Target 5Hz') },
        ### BBB 'HLT_noalg_mb_L1TE10'          : { 10000 : TriggerRule(PS=100, comment='Main. Target 3Hz') },
        ### BBB 'HLT_noalg_mb_L1TE20'          : { 10000 : TriggerRule(PS=50, comment='Main. Target 3Hz',ESValue=6) },
        ### BBB 'HLT_noalg_mb_L1TE30'          : { 10000 : TriggerRule(PS=10, comment='Main. Target 3Hz') },
        ### BBB 'HLT_noalg_mb_L1TE10.0ETA25'          : { 10000 : TriggerRule(PS=50, comment='Main. Target 3Hz') },
        ### BBB 'HLT_noalg_mb_L1TE20.0ETA25'          : { 10000 : TriggerRule(PS=25, comment='Main. Target 3Hz') },
        #### BBB 'HLT_noalg_mb_L1TE30.0ETA25'          : { 10000 : TriggerRule(PS=5, comment='Main. Target 3Hz') },
        # inclusive track trigger
        ### BBB 'HLT_mb_sptrk_pt4_L1MBTS_1'                               : {   1 : TriggerRule(PS=1*ScaleMu, comment='Target 250 Hz') }, 
        ### BBB 'HLT_mb_sptrk_pt6_L1MBTS_1'                               : {   1 : TriggerRule(PS=1*ScaleMu, comment='') },
        ### BBB 'HLT_mb_sptrk_pt8_L1MBTS_1'                               : {   1 : TriggerRule(PS=1*ScaleMu, comment='') },
        }
    
    ### BBB rules=dict(rules,**minbias_hmt300Hz_Rules)
    rules=dict(rules, **minbias_hmt300HzMu003_Rules)
    
#################################################################################################
#
#
# LHCF rules
#
#
#################################################################################################

lhcf_Rules = {

    ########################################
    # Level-1
    'L1_LHCF'                                                 : {   1 : TriggerRule(PS=1, comment='') }, # Needs to adjust PS value to keep L1_LHCF rate < 500 Hz
    'L1_LHCF_UNPAIRED_ISO'                                    : {   1 : TriggerRule(PS=1, comment='') }, 
    'L1_LHCF_EMPTY'                                           : {   1 : TriggerRule(PS=1, comment='') },
    
    ########################################
    # HLT

    # with full readout: for first 10 M events
    'HLT_noalg_L1LHCF'                                        : {   1 : TriggerRule(PS=1, comment='') },
    'HLT_noalg_L1LHCF_UNPAIRED_ISO'                           : {   1 : TriggerRule(PS=1, comment='') },
    'HLT_noalg_L1LHCF_EMPTY'                                  : {   1 : TriggerRule(PS=1, comment='') },
    
    # with partial event readout: for the rest
    'HLT_lhcfpeb'                                             : {   1 : TriggerRule(PS=1, comment='') },
    'HLT_lhcfpeb_L1LHCF_UNPAIRED_ISO'                         : {   1 : TriggerRule(PS=1, comment='') },
    'HLT_lhcfpeb_L1LHCF_EMPTY'                                : {   1 : TriggerRule(PS=1, comment='') },
}


rules=dict(rules,**lhcf_Rules)


#################################################################################################
#
#
# alfa rules
#
#
#################################################################################################


alfa_phys_Rules = {

    ########################################
    # Level-1 ALFA should be limited to 5kHz
    # p+pi0 (ALFA diff)
    'L1_LHCF_ALFA_ANY_A'                                      : {   1 : TriggerRule(PS=1, comment='') },
    'L1_LHCF_ALFA_ANY_C'                                      : {   1 : TriggerRule(PS=1, comment='') },
    'L1_LHCF_ALFA_ANY_A_UNPAIRED_ISO'                         : {   1 : TriggerRule(PS=1, comment='') },
    'L1_LHCF_ALFA_ANY_C_UNPAIRED_ISO'                         : {   1 : TriggerRule(PS=1, comment='') },

    # central diff (ALFA_CEP)
    'L1_ALFA_ANTI_ELASTIC_UNPAIRED_ISO'                       : {   1 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_ELAST15'                                         : {   1 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_ELAST18'                                         : {   1 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_ELASTIC_UNPAIRED_ISO'                            : {   1 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_SYST17'                                          : {   1 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_SYST18'                                          : {   1 : TriggerRule(PS=1, comment='') },
    
    ########################################
    # HLT
    # ALFA streamers
    'HLT_noalg_L1ALFA_Phys'                                   : {   1 : TriggerRule(PS=-1, comment='') },
    'HLT_noalg_L1ALFA_SYS'                                    : {   1 : TriggerRule(PS=-1, comment='') },
    'HLT_noalg_L1ALFA_PhysAny'                                : {   1 : TriggerRule(PS=-10000, comment='') }, 
    
    # p+pi0 (ALFA diff) : PEB streamers (IDFwd)
    'HLT_lhcfpeb_L1LHCF_ALFA_ANY_A'                           : {   1 : TriggerRule(PS=1, comment='') },
    'HLT_lhcfpeb_L1LHCF_ALFA_ANY_C'                           : {   1 : TriggerRule(PS=1, comment='') },
    'HLT_lhcfpeb_L1LHCF_ALFA_ANY_A_UNPAIRED_ISO'              : {   1 : TriggerRule(PS=1, comment='') },
    'HLT_lhcfpeb_L1LHCF_ALFA_ANY_C_UNPAIRED_ISO'              : {   1 : TriggerRule(PS=1, comment='') },

    # central diff (full r/o)  
    'HLT_mb_sptrk_vetombts2in_L1ALFA_CEP'                     : {   1 : TriggerRule(PS=1, comment='') },

    # trigger/detectorcommissioning : PEB (IDFwd)
    'HLT_mb_sptrk_vetombts2in_peb_L1ALFA_ANY'                 : {   1 : TriggerRule(PS=1, comment='') },
    'HLT_mb_sptrk_vetombts2in_peb_L1ALFA_ANY_UNPAIRED_ISO'    : {   1 : TriggerRule(PS=1, comment='') },
}


if doALFA:
    rules.update(alfa_phys_Rules)
#rules=dict(rules,**alfa_phys_Rules)



alfa_calib_Rules = {

    ########################################
    # Level-1

    #
    'L1_ALFA_ANY'                                             : {   1 : TriggerRule(PS=1, comment='') }, 

    # "ALFA_Calib"
    'L1_ALFA_A7L1L'                                           : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_A7L1L_OD'                                        : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_A7L1U'                                           : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_A7L1U_OD'                                        : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_A7L1_OD'                                         : {   1 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_A7R1L'                                           : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_A7R1L_OD'                                        : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_A7R1U'                                           : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_A7R1U_OD'                                        : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_A7R1_OD'                                         : {   1 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_ANY_ABORTGAPNOTCALIB'                            : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_ANY_A_EMPTY'                                     : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_ANY_BGRP10'                                      : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_ANY_CALIB'                                       : {   1 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_ANY_C_EMPTY'                                     : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_ANY_EMPTY'                                       : {   1 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_ANY_FIRSTEMPTY'                                  : {   1 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_ANY_UNPAIRED_ISO'                                : {   1 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_ANY_UNPAIRED_NONISO'                             : {   1 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_B7L1L'                                           : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_B7L1L_OD'                                        : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_B7L1U'                                           : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_B7L1U_OD'                                        : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_B7L1_OD'                                         : {   1 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_B7R1L'                                           : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_B7R1L_OD'                                        : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_B7R1U'                                           : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_B7R1U_OD'                                        : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_B7R1_OD'                                         : {   1 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_ELAST15_Calib'                                   : {   1 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_ELAST18_Calib'                                   : {   1 : TriggerRule(PS=1, comment='') },
    'L1_ALFA_SHOWSYST5'                                       : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_BGRP10_ALFA_BGT'                                      : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_BGRP1_ALFA_BGT'                                       : {   1 : TriggerRule(PS=1000, comment='') },
    'L1_BGRP4_ALFA_BGT'                                       : {   1 : TriggerRule(PS=-1, comment='') },

    # "ALFA_SYS"  
    'L1_ALFA_SYST9'                                           : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_SYST10'                                          : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_SYST11'                                          : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_SYST12'                                          : {   1 : TriggerRule(PS=-1, comment='') },
    # SYST17/18 used in ALFA_CEP
    #'L1_ALFA_SYST17'                                         : {   1 : TriggerRule(PS=1, comment='') },
    #'L1_ALFA_SYST18'                                         : {   1 : TriggerRule(PS=1, comment='') },

    # "ALFA_ELAS"
    'L1_ALFA_ELAST1'                                          : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_ELAST2'                                          : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_ELAST11'                                         : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_ELAST12'                                         : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_ELAST13'                                         : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_ELAST14'                                         : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_ELAST16'                                         : {   1 : TriggerRule(PS=-1, comment='') },
    'L1_ALFA_ELAST17'                                         : {   1 : TriggerRule(PS=-1, comment='') },
    # ELAST15/18 used in ALFA_CEP
    #'L1_ALFA_ELAST15'                                        : {   1 : TriggerRule(PS=1, comment='') },
    #'L1_ALFA_ELAST18'                                        : {   1 : TriggerRule(PS=1, comment='') },


    ########################################
    # PEB streamers
    'HLT_alfacalib'                                           : {   1 : TriggerRule(PS=1, comment='') },
    'HLT_alfacalib_L1ALFA_ANY'                                : {   1 : TriggerRule(PS=1, comment='') },
    'HLT_alfacalib_L1ALFA_ELAST15'                            : {   1 : TriggerRule(PS=1, comment='') },
    'HLT_alfacalib_L1ALFA_ELAST18'                            : {   1 : TriggerRule(PS=1, comment='') },
    'HLT_alfacalib_L1ALFA_A7L1_OD'                            : {   1 : TriggerRule(PS=1, comment='') },
    'HLT_alfacalib_L1ALFA_B7L1_OD'                            : {   1 : TriggerRule(PS=1, comment='') },
    'HLT_alfacalib_L1ALFA_A7R1_OD'                            : {   1 : TriggerRule(PS=1, comment='') },
    'HLT_alfacalib_L1ALFA_B7R1_OD'                            : {   1 : TriggerRule(PS=1, comment='') },
    'HLT_alfacalib_L1ALFA_SYS'                                : {   1 : TriggerRule(PS=1, comment='') },
    'HLT_alfacalib_L1ALFA_ELAS'                               : {   1 : TriggerRule(PS=1, comment='') },

    'HLT_alfacalib_L1LHCF_ALFA_ANY_A'                         : {   1 : TriggerRule(PS=1, comment='') },
    'HLT_alfacalib_L1LHCF_ALFA_ANY_C'                         : {   1 : TriggerRule(PS=1, comment='') },
    'HLT_alfacalib_L1LHCF_ALFA_ANY_A_UNPAIRED_ISO'            : {   1 : TriggerRule(PS=1, comment='') },
    'HLT_alfacalib_L1LHCF_ALFA_ANY_C_UNPAIRED_ISO'            : {   1 : TriggerRule(PS=1, comment='') },
}



if doALFA:
    rules.update(alfa_calib_Rules)
#rules=dict(rules,**alfa_calib_Rules)


#############################################################################################
#
#
# beamspot rules
#
#
#############################################################################################


beamspot_Rules = {
    
    ########################################
    # Level-1
    'L1_4J15'                              : { 1 : TriggerRule(PS=1, comment='L1') },

    ########################################
    # HLT

    # online measurement: unPS (ATR-10984)
    'HLT_beamspot_trkFS_L2StarB'           : { 1 : TriggerRule(PS=1, comment='') },
    'HLT_beamspot_trkFS_trkfast'           : { 1 : TriggerRule(PS=1, comment='') },

    # offline measurement
    # activeTE: 200 Hz, allTE: 10 Hz (ATR-10984)
    'HLT_beamspot_activeTE_L2StarB_pebTRT' : { 1 : TriggerRule(PS=1, comment='') }, # target rate: 200 Hz
    'HLT_beamspot_activeTE_trkfast_pebTRT' : { 1 : TriggerRule(PS=1, comment='') }, # target rate: 200 Hz
    'HLT_beamspot_allTE_L2StarB_pebTRT'    : { 1 : TriggerRule(PS=1,  comment='') }, # target rate: 10 Hz
    'HLT_beamspot_allTE_trkfast_pebTRT'    : { 1 : TriggerRule(PS=1,  comment='') }, # target rate: 10 Hz
}

rules=dict(rules,**beamspot_Rules)


#############################################################################################
#
#
# monitoring rules (import from Monitoring_pp_v5_rules.py)
#
#
#############################################################################################


import Monitoring_pp_v5_rules
rules.update(Monitoring_pp_v5_rules.physics_rules)
### RD triggers are also used for PixelBeam/Noise etc (Monitoring rules). They also have some rate constraints. Need input how to adjust.

#check that rules are not duplicated

duplication_list = [ rule  for rule in Monitoring_pp_v5_rules.physics_rules if rules.has_key(rule) ]
'''
for rule in rules:
    for rulemon in Monitoring_pp_v5_rules.physics_rules:
        if rule==rulemon:
            print "week2", rules[rule]
            #print "monitoring", Monitoring_pp_v5_rules.physics_rules[rulemon]
'''

import sys
if duplication_list:
    print "FATAL: duplicated rules for chains: "
    print '\n'.join([ str(myelement)  for myelement in duplication_list ])
    #sys.exit("FATAL rulebook failed, exiting")

#############################################################################################
#
#
# disable rules
#
#
#############################################################################################
  

disable_Rules = {
    }

rules=dict(rules,**disable_Rules)
