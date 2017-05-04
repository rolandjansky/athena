# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'Week1_rules'

###Switches to enable HLT triggers
IntroduceHLT_Jets=True
IntroduceHLT_Egamma=True #g10_loose
IntroduceHLT_MinBias=True
IntroduceHLT_Monitoring=True
IntroduceHLT_5e33=True
Introduce_L1CaloStreamers =True

IntroduceHLT_TRTKr=False
AfterGlow=False
Introduce_EB=False

#################################################################################################
#
# Rules that were added in Week 1 on the fly for run adjustments
#
#################################################################################################

rules = {
    'HLT_noalg_mb_L1RD3_FILLED'    : { 10000 : TriggerRule(rate=100, comment='random coverage', rerun=0) },        
    'HLT_noalg_L1LowLumi'          : { 10000 : TriggerRule(PS=1, comment='Main', ESValue=24) }, 
    'L1_2MU20_OVERLAY'             : { 10000 : TriggerRule(PS=20, comment = 'TauOverlay', rerun = 0.0) },
    'HLT_noalg_to_L12MU20_OVERLAY' : { 10000 : TriggerRule(PS=1, comment = 'TauOverlay', rerun = 0.0) },

    'HLT_mu13_mu13_idperf_Zmumu'         : { 10000 : TriggerRule(PS=1, comment = 'trackingEfficiencies', rerun = 0.0) },
    'HLT_mu4_mu4_idperf_bJpsimumu_noid'  : { 10000 : TriggerRule(PS=1, comment = 'trackingEfficiencies', rerun = 0.0) },

    ##Needs to run at 10Hz, rate unkown at the moment, set PS=1
    'HLT_alfacalib_L1ALFA_ANY' : { 10000 : TriggerRule(PS=45, comment = 'ALFACalib', rerun = 0.0) },

    'L1_ALFA_B7L1_OD'      : { 10000 : TriggerRule(PS=1, comment = 'ALFACalib', rerun = 0.0) },
    'L1_ALFA_A7L1_OD'      : { 10000 : TriggerRule(PS=1, comment = 'ALFACalib', rerun = 0.0) },
    'L1_ALFA_A7R1_OD'      : { 10000 : TriggerRule(PS=1, comment = 'ALFACalib', rerun = 0.0) },
    'L1_ALFA_B7R1_OD'      : { 10000 : TriggerRule(PS=1, comment = 'ALFACalib', rerun = 0.0) },
    'HLT_alfacalib'        : { 10000 : TriggerRule(PS=1, comment = 'ALFACalib', rerun = 0.0) },
    
    #L1 items: #safety prescales for run start, PS for 8 filled bunches at 2.4e31
    'L1_MU4'         : { 10000 : TriggerRule(PS=8, comment='L1', rerun=0) },
    'L1_MU6'         : { 10000 : TriggerRule(PS=40, comment='L1', rerun=0) },
    'L1_MU10'        : { 10000 : TriggerRule(PS=32, comment='L1', rerun=0) },
    'L1_MU4_EMPTY'   : { 10000 : TriggerRule(PS=15, comment='L1', rerun=0) },

    'L1_TAU8'        : { 10000 : TriggerRule(PS=28, comment='L1', rerun=0) },
    'L1_TAU12'       : { 10000 : TriggerRule(PS=10, comment='L1', rerun=0) },
    'L1_TAU12IM'     : { 10000 : TriggerRule(PS=10, comment='L1', rerun=0) },
    'L1_TAU20'       : { 10000 : TriggerRule(PS=4, comment='L1', rerun=0) },
    'L1_TAU40'       : { 10000 : TriggerRule(PS=4, comment='L1', rerun=0) },
    'L1_TAU8_EMPTY'  : { 10000 : TriggerRule(PS=2, comment='L1', rerun=0) },

    'L1_EM3'               : { 10000 : TriggerRule(PS=50, comment='L1', rerun=0) },
    'L1_EM7'               : { 10000 : TriggerRule(PS=32, comment='L1', rerun=0) },
    'L1_EM12'              : { 10000 : TriggerRule(PS=32, comment='L1', rerun=0) },
    'L1_EM15'              : { 10000 : TriggerRule(PS=32, comment='L1', rerun=0) },
    'L1_EM18VH'            : { 10000 : TriggerRule(PS=4, comment='L1', rerun=0) },
    'L1_EM20VH'            : { 10000 : TriggerRule(PS=4, comment='L1', rerun=0) },
    'L1_EM20VHI'           : { 10000 : TriggerRule(PS=4, comment='L1', rerun=0) },
    'L1_EM3_EMPTY'         : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },
    'L1_EM7_EMPTY'         : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },

    'L1_J12'         : { 10000 : TriggerRule(PS=800, comment='L1', rerun=0) },
    'L1_J15'         : { 10000 : TriggerRule(PS=800, comment='L1', rerun=0) },
    'L1_J20'         : { 10000 : TriggerRule(PS=48, comment='L1', rerun=0) },
    'L1_J30'         : { 10000 : TriggerRule(PS=48, comment='L1', rerun=0) },
    'L1_J50'         : { 10000 : TriggerRule(PS=2, comment='L1', rerun=0) },
    'L1_J75'         : { 10000 : TriggerRule(PS=2, comment='L1', rerun=0) },
    'L1_J85'         : { 10000 : TriggerRule(PS=2, comment='L1', rerun=0) },
    'L1_J12_EMPTY'   : { 10000 : TriggerRule(PS=2, comment='L1', rerun=0) },
    'L1_J30_EMPTY'         : { 10000 : TriggerRule(PS=2, comment='L1', rerun=0) },

    'L1_J15.32ETA49'       : { 10000 : TriggerRule(PS=40, comment='L1', rerun=0) },
    'L1_J20.32ETA49'       : { 10000 : TriggerRule(PS=40, comment='L1', rerun=0) },
    'L1_J30.32ETA49'       : { 10000 : TriggerRule(PS=40, comment='L1', rerun=0) },
    'L1_J75.32ETA49'       : { 10000 : TriggerRule(PS=40, comment='L1', rerun=0) },

    'L1_J30.32ETA49_EMPTY'      : { 10000 : TriggerRule(PS=2, comment='L1', rerun=0) },    
    'L1_J30.0ETA49_2J20.0ETA49' : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },

    'L1_3J15'                   : { 10000 : TriggerRule(PS=6, comment='L1', rerun=0) },
    'L1_4J15'                   : { 10000 : TriggerRule(PS=1, comment='L1', rerun=0) },

    'L1_XE35'                   : { 10000 : TriggerRule(PS=4, comment='L1', rerun=0) },


}


#################################################################################################
#
# L1Calo streamers
#
#################################################################################################

l1calostreamer_rules ={
    # run unprescaled
    'HLT_noalg_L1J75'           : {   1 : TriggerRule(PS=1, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J75.32ETA49'   : {   1 : TriggerRule(PS=1, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J85'           : {   1 : TriggerRule(PS=1, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J100'          : {   1 : TriggerRule(PS=1, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J100.32ETA49'  : {   1 : TriggerRule(PS=1, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J120'          : {   1 : TriggerRule(PS=1, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J400'          : {   1 : TriggerRule(PS=1, comment='Streamer for L1Calo')},
    'HLT_noalg_L1EM22VHI'       : {   1 : TriggerRule(PS=1, comment='Streamer for L1Calo')},
    'HLT_noalg_L1EM20VHI'       : {   1 : TriggerRule(PS=1, comment='Streamer for L1Calo')},
    'HLT_noalg_L1EM20VH'        : {   1 : TriggerRule(PS=1, comment='Streamer for L1Calo')},
    'HLT_noalg_L1XE80'          : {   1 : TriggerRule(PS=1, comment='Streamer for L1Calo')},
    'HLT_noalg_L1XE70'          : {   1 : TriggerRule(PS=1, comment='Streamer for L1Calo')},
    'HLT_noalg_L1XE60'          : {   1 : TriggerRule(PS=1, comment='Streamer for L1Calo')},
    'HLT_noalg_L1XE55'          : {   1 : TriggerRule(PS=1, comment='Streamer for L1Calo')},
    'HLT_noalg_L1XE50'          : {   1 : TriggerRule(PS=1, comment='Streamer for L1Calo')},

    # run at 2Hz = 80Hz
    'HLT_noalg_L1J12'           : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J15'           : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J15.29ETA32'   : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J15.32ETA49'   : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J20'           : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J20.29ETA32'   : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J20.32ETA49'   : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J25'           : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J30'           : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J30.32ETA49'   : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J40'           : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1J50.32ETA49'   : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    
    'HLT_noalg_L1EM8VH'         : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1EM15VH'        : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1EM13VH'        : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1EM10VH'        : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1EM10'          : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    
    'HLT_noalg_L1TAU8'          : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1TAU60'         : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1TAU40'         : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    
    'HLT_noalg_L1XE45'          : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1XE40'          : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1TE50'          : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1TE40'          : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1TE30'          : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1TE20'          : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1TE10'          : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1TE10.0ETA25'   : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1TE20.0ETA25'   : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},   
    
    'HLT_noalg_L1XS60'          : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1XS50'          : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1XS40'          : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1XS30'          : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},
    'HLT_noalg_L1XS20'          : {   1 : TriggerRule(PS=22, comment='Streamer for L1Calo')},

    ##NOT IN MENU YET['HLT_noalg_L1TE30.0ETA25',  'L1_TE30.0ETA25',   [], [PhysicsStream], ["RATE:SeededStreamers", "BW:MET"], -1 ],  
}

if Introduce_L1CaloStreamers:
    rules.update(l1calostreamer_rules)

#################################################################################################
#
# L1 Low Lumi rule
#
#################################################################################################
L1_lowlumi=[
    #Item added for L1Calo count/GeV inconsistency, for L1Calo streamer
    #'L1_TE20',
    #'L1_TE50',
    #'L1_J12',
    #'L1_EM3',
    #'L1_EM7',
    #'L1_2EM3',
    #'L1_TAU8',
    #'L1_TAU12',
    #'L1_TAU20',
    #'L1_EM12',

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
    'L1_XE50']

L1_lowlumi_Rules=dict(map(None,L1_lowlumi,len(L1_lowlumi)*[{
                         1 : TriggerRule(PS=1, comment='L1'), 
                       }]))

rules.update(L1_lowlumi_Rules)
#rules=dict(rules,**L1_lowlumi_Rules)



#################################################################################################
#
#
# egamma rules
#
#
#################################################################################################
lumi_move_to_l1_em15=11
lumi_enable_hlt_egamma=51
lumi_move_to_l1_em18vh=51
lumi_move_to_l1_2em10vh=51
lumi_move_to_l1_em20vh=101

egamma_Streamers = {
    # streamers: single
    'HLT_noalg_L1EM18VH'                : {   1 : TriggerRule(PS=1, comment=''),
                         lumi_move_to_l1_em20vh : TriggerRule(PS=11, comment='Disabled') },
    'HLT_noalg_L1EM15'                  : {   1 : TriggerRule(PS=1, comment=''),
                         lumi_move_to_l1_em18vh : TriggerRule(PS=11, comment='Disabled') },
    'HLT_noalg_L1EM12'                  : {   1 : TriggerRule(PS=1, comment=''),
                           lumi_move_to_l1_em15 : TriggerRule(PS=11, comment='Disabled') },

    # streamers: di
    'HLT_noalg_L12EM7'                  : {   1 : TriggerRule(PS=1, comment=''),
                        lumi_move_to_l1_2em10vh : TriggerRule(PS=-1, comment='Disabled') },
    
    'HLT_noalg_L12EM15'                  : {   1 : TriggerRule(PS=1, comment=''),},
    'HLT_noalg_L1EM15VH_3EM7'            : {   1 : TriggerRule(PS=1, comment=''),},
}

rules.update(egamma_Streamers)

#-----------------------
# SM run 
#-----------------------
egamma_Rules_HLTs = {    
    #SM request, low threshold single photon
    #SM requests for 30 Hz (Anna suggest ok to run it up to 50); 6M in total
    'HLT_g10_loose'                     : {   1 : TriggerRule(rate=50, comment='Introduce HLT') },
}

if IntroduceHLT_Egamma:
    rules.update(egamma_Rules_HLTs)

#-----------------------
# Krypton TRT run
#-----------------------
egamma_Rules_TRTKr = {
    #3M requested for Kr study by TRT, 100-150 Hz for each e/my chains (ATR-10740)
    'HLT_e5_loose'                      : {   1 : TriggerRule(rate=100, comment='Introduce HLT') },
    }

if IntroduceHLT_TRTKr:
    rules.update(egamma_Rules_TRTKr)


#-----------------------
# HLT unprescaled + rerun chains
#-----------------------
HLT_egamma_unPS_rerun=[
    # Week-1 main is e17
    #'HLT_e17_loose_L1EM15',
    #'HLT_e17_lhloose_L1EM15',
    #'HLT_e17_lhloose_cutd0dphideta_L1EM15',
    #'HLT_e17_lhloose_nod0_L1EM15',
    #'HLT_e17_lhloose_nodeta_L1EM15',
    #'HLT_e17_lhloose_nodphires_L1EM15',
    #
    # below for primary triggers SF determination
    #'HLT_e26_tight_iloose',
    #'HLT_e26_lhtight_iloose',
    #'HLT_e26_lhtight_cutd0dphideta_iloose',
    #'HLT_e26_lhtight_nod0_iloose',
    #'HLT_e60_medium',
    #'HLT_e60_lhmedium',
    #'HLT_e60_lhmedium_cutd0dphideta',
    #'HLT_e60_lhmedium_nod0',
    #
    #'HLT_e24_tight_iloose',
    #'HLT_e24_lhtight_iloose',
    #'HLT_e24_lhtight_cutd0dphideta_iloose',
    #'HLT_e24_lhtight_nod0_iloose',
    #
    #'HLT_e24_medium_iloose_L1EM20VH',
    #'HLT_e24_lhmedium_iloose_L1EM20VH',
    #'HLT_e24_lhmedium_nod0_iloose_L1EM20VH',
    #'HLT_e24_lhmedium_cutd0dphideta_iloose_L1EM20VH',
    #
    #'HLT_e24_medium_iloose_L1EM18VH',
    #'HLT_e24_lhmedium_iloose_L1EM18VH',
    #'HLT_e24_lhmedium_cutd0dphideta_iloose_L1EM18VH',
    #'HLT_e24_lhmedium_nod0_iloose_L1EM18VH',
    #'HLT_e24_lhmedium_nodeta_iloose_L1EM18VH',
    #'HLT_e24_lhmedium_nodphires_iloose_L1EM18VH',
    #
    #'HLT_g120_loose',
    ]

HLT_egamma_unPS_rerun_Rules=dict(map(None,HLT_egamma_unPS_rerun,len(HLT_egamma_unPS_rerun)*[{
                         1 : TriggerRule(PS=-1, comment=''), 
    lumi_enable_hlt_egamma : TriggerRule(PS=1,  comment='Introduce HLT', rerun=1),
                       }]))

rules.update(HLT_egamma_unPS_rerun_Rules)
#egamma_Rules=dict(egamma_Rules,**HLT_egamma_unPS_rerun_Rules)

#-----------------------
# HLT unprescaled chains
#-----------------------
HLT_egamma_unPS=[
    #'HLT_2e12_loose_L12EM10VH',
    #'HLT_2e12_lhloose_L12EM10VH',
    #'HLT_2e12_lhloose_cutd0dphideta_L12EM10VH',
    #'HLT_2e12_lhloose_nod0_L12EM10VH',
    #'HLT_2e12_lhloose_nodeta_L12EM10VH',
    #'HLT_2e12_lhloose_nodphires_L12EM10VH',
    #'HLT_2g10_loose'
    ]

HLT_egamma_unPS_Rules=dict(map(None,HLT_egamma_unPS,len(HLT_egamma_unPS)*[{
                         1 : TriggerRule(PS=-1, comment=''), 
    lumi_enable_hlt_egamma : TriggerRule(PS=1,  comment='Introduce HLT'),
                       }]))

rules.update(HLT_egamma_unPS_Rules)

#-----------------------
# HLT rerun chains
#-----------------------
HLT_egamma_rerun=[
    #'HLT_e12_loose_L1EM10VH',
    #'HLT_e12_lhloose_L1EM10VH',
    #'HLT_e12_lhloose_cutd0dphideta_L1EM10VH',
    #'HLT_e12_lhloose_nod0_L1EM10VH',
    #'HLT_e12_lhloose_nodeta_L1EM10VH',
    #'HLT_e12_lhloose_nodphires_L1EM10VH',
    #
    #'HLT_e12_loose',
    #'HLT_e12_lhloose',
    #'HLT_e12_lhloose_nod0',
    #
    #'HLT_e15_loose_L1EM13VH',
    #'HLT_e15_lhloose_L1EM13VH',
    #'HLT_e15_lhloose_cutd0dphideta_L1EM13VH',
    #'HLT_e15_lhloose_nod0_L1EM13VH',
    #
    #'HLT_e17_loose',
    #'HLT_e17_lhloose',
    #'HLT_e17_lhloose_cutd0dphideta',
    #'HLT_e17_lhloose_nod0',
    #
    #'HLT_g25_loose',
    #'HLT_g25_medium',
    #'HLT_g35_loose',
    #'HLT_g35_medium',
]

HLT_egamma_rerun_Rules=dict(map(None,HLT_egamma_rerun,len(HLT_egamma_rerun)*[{
                         1 : TriggerRule(PS=-1, comment=''), 
    lumi_enable_hlt_egamma : TriggerRule(PS=0,  comment='Introduce HLT', rerun=1),
                       }]))

rules.update(HLT_egamma_rerun_Rules)


#################################################################################################
#
#
# muon and bphys rules
#
#
#################################################################################################

lumi_enable_hlt_mubphys=31
lumi_move_to_l1_mu15=31
lumi_move_to_l1_2mu6=31

mubphys_Streamers = {
    
    # streamers: single
    'HLT_noalg_L1MU10'      : {   1 : TriggerRule(PS=1, comment=''),
                                                                   lumi_move_to_l1_mu15 : TriggerRule(PS=-1, comment='Disabled') },
    # low pT
    # 20 Hz requested by muon trigger (ATR-10740), but Cost information not available, PS by hand at the moment
    'HLT_noalg_L1MU4'    : {   1 : TriggerRule(PS=70, comment='') }, #L1_MU4 ~ 1.3kHz
    'HLT_noalg_L1MU6'    : {   1 : TriggerRule(PS=70, comment='') },

    'HLT_noalg_L1MU4_3J15'                               : {   1 : TriggerRule(PS=1, comment='') }, 
    'HLT_noalg_L1MU4_J30'                                : {   1 : TriggerRule(PS=1, comment='') }, 
    'HLT_noalg_L1MU6_J20'                                : {   1 : TriggerRule(PS=2, comment='') }, 
    'HLT_noalg_L1MU4_J12'                                : {   1 : TriggerRule(PS=2, comment='') }, 
       
    # streamers: di
    'HLT_noalg_L12MU4'                                       : {   1 : TriggerRule(PS=1, comment='') },
}
    
rules.update(mubphys_Streamers)

#-----------------------
# Bphys
#-----------------------
mubphys_Rules = {
    # low pT single muon
    # 10 Hz requested by muon trigger (ATR-10740)
    # 10 Hz requested by muon trigger (ATR-10740)
    #'HLT_mu6'                                                : {   1 : TriggerRule(rate=10, comment='Introduce HLT') },

}

#-----------------------
# L1 unprescaled items
#-----------------------
#L1_mubphys_unPS=[
#    'L1_MU10',
#    'L1_MU15',
#    'L1_MU20',
#    'L1_2MU4',
#    'L1_2MU6',
#    'L1_MU6_2MU4',
#    'L1_2MU10',
#    'L1_3MU4',
#    'L1_3MU6' ]
#
#L1_mubphys_unPS_Rules=dict(map(None,L1_mubphys_unPS,len(L1_mubphys_unPS)*[{
#                         1 : TriggerRule(PS=1, comment=''), 
#                       }]))
#
#rules.update(L1_mubphys_unPS_Rules)

#-----------------------
# TRT Krypton rules
#-----------------------
mu_Rules_TRTKr = {
    # 3M requested for Kr study by TRT -> 15 Hz? (ATR-10740)
    'HLT_mu4'                                                : {   1 : TriggerRule(rate=15, comment='Introduce HLT', rerun=1) }, # rerun for 2mu4    
    }

if IntroduceHLT_TRTKr:
    rules.update(mu_Rules_TRTKr)


#-----------------------
# HLT unprescaled + rerun chains
#-----------------------
HLT_mu_unPS_rerun=[
    #'HLT_mu10',   # main for Week-1
    # below for primary triggers SF determination
    #'HLT_mu14',   # for 2mu14
    #'HLT_mu26_imedium',
    #'HLT_mu50',
    #'HLT_mu60_0eta105_msonly',
    #'HLT_mu24_imedium',
    #'HLT_mu24_iloose_L1MU15',
    #'HLT_mu20_iloose_L1MU15'
    ]

HLT_mu_unPS_rerun_Rules=dict(map(None,HLT_mu_unPS_rerun,len(HLT_mu_unPS_rerun)*[{
                          1 : TriggerRule(PS=-1, comment=''), 
    lumi_enable_hlt_mubphys : TriggerRule(PS=1,  comment='Introduce HLT', rerun=1),
                       }]))

rules.update(HLT_mu_unPS_rerun_Rules)

#-----------------------
# HLT unprescaled chains
#-----------------------
HLT_mubphys_unPS=[
    #'HLT_2mu4',
    #'HLT_2mu4_bDimu',
    #'HLT_2mu4_bDimu_novtx_noos'
    ]

HLT_mubphys_unPS_Rules=dict(map(None,HLT_mubphys_unPS,len(HLT_mubphys_unPS)*[{
                          1 : TriggerRule(PS=-1, comment=''), 
    lumi_enable_hlt_mubphys : TriggerRule(PS=1,  comment='Introduce HLT'),
                       }]))

rules.update(HLT_mubphys_unPS_Rules)


#################################################################################################
#
#
# jet & MET rules
#
#
#################################################################################################

lumi_enable_hlt_met=51
lumi_move_to_l1_j75=51
lumi_move_to_l1_xe50=51
lumi_disable_l1_2jet=11
lumi_disable_l1_intercalib=51

jetmet_Streamers = {
    # single-jet
    'HLT_noalg_L1J50'                                         : {   1 : TriggerRule(PS=1, comment=''),
                                                  lumi_move_to_l1_j75 : TriggerRule(PS=11, comment='Disabled') },
    # di-jet
    'HLT_noalg_L1J30.0ETA49_2J20.0ETA49'                      : {   1 : TriggerRule(PS=1, comment=''),
                                                 lumi_disable_l1_2jet : TriggerRule(PS=-1, comment='Disabled') },

    # eta-intercalib
    'HLT_noalg_L1J15_J15.32ETA49'                             : {   1 : TriggerRule(PS=1, comment=''),
                                           lumi_disable_l1_intercalib : TriggerRule(PS=-1, comment='Disabled') },

    'HLT_noalg_L1J20_J20.32ETA49'                             : {   1 : TriggerRule(PS=1, comment=''),
                                           lumi_disable_l1_intercalib : TriggerRule(PS=-1, comment='Disabled') },
    #Target Rate
    #'HLT_noalg_L1XE35'                                        : {   1 : TriggerRule(PS=1, comment='', ESRate=0.5),
                                                 #lumi_move_to_l1_xe50 : TriggerRule(rate=0.5, comment='', ESRate=0.5) },
    #From Operational Experience
    'HLT_noalg_L1XE35'                                        : {   1 : TriggerRule(PS=1, comment='', ESValue=120),
                                                 lumi_move_to_l1_xe50 : TriggerRule(PS=11, comment='', ESValue=240) },

    ########################################
    # HLT
    #'HLT_xe50'                                                : {   1 : TriggerRule(PS=-1, comment=''),     
    #                                              lumi_enable_hlt_met : TriggerRule(PS=1,  comment='Introduce HLT') },
}

rules.update(jetmet_Streamers)

#-----------------------
# L1 unprescaled items
#-----------------------
#L1_jetmet=[
#    'L1_J75',
#    'L1_J75.32ETA49',
#    'L1_J100',
#    'L1_J100.32ETA49',
#    'L1_J120',
#    'L1_J400',
#    'L1_3J25.0ETA23',
#    'L1_3J40',
#    'L1_3J50',
#    'L1_4J15',
#    'L1_4J15.0ETA25',
#    'L1_4J20',
#    'L1_5J15.0ETA25',
#    'L1_6J15',
#    'L1_XE55',
#    'L1_XE60',
#    'L1_XE70' ]
#
#L1_jetmet_Rules=dict(map(None,L1_jetmet,len(L1_jetmet)*[{
#                         1 : TriggerRule(PS=1, comment=''), 
#                       }]))
#jetmet_Rules=dict(jetmet_Rules,**L1_jetmet_Rules)


# JetETMiss rates assignment
# ATR-10740 (+ ATR-9321)

#-----------------------
# Jet calibration request
#-----------------------
if IntroduceHLT_Jets:
        rules.update({
            ##When enabling jets, start with 1kHz of L1 random, then eventually increase rate
            'L1_RD0_FILLED'   : {   1 : TriggerRule(rate=1000, scaling='bunches', comment='L1') },            
            })
                     
#-----------------------
# JetETMiss CP: 20 M
#-----------------------
HLT_jets_20M=[
    'HLT_j15', 
    'HLT_j15_320eta490', 
    'HLT_j60'       ,
    'HLT_j60_320eta490'
    ]
HLT_jets_20M_Rules=dict(map(None,HLT_jets_20M,len(HLT_jets_20M)*[{
                      1 : TriggerRule(rate=100,  comment='Introduce HLT'),
                }]))

if IntroduceHLT_Jets:
        rules.update(HLT_jets_20M_Rules)

#-----------------------
# JetETMiss CP: 10 M
#-----------------------
HLT_jets_10M=[
    'HLT_j25_320eta490',
    'HLT_j25'
    ]

HLT_jets_10M_Rules=dict(map(None,HLT_jets_10M,len(HLT_jets_10M)*[{
                     1 : TriggerRule(rate=50,  comment='Introduce HLT'),
                }]))

if IntroduceHLT_Jets:
        rules.update(HLT_jets_10M_Rules)

#-----------------------
# JetETMiss CP: 100 K
#-----------------------
HLT_jets_100K=[
    'HLT_j35',
    'HLT_j35_320eta490',
    
    'HLT_j45_L1RD0',
    'HLT_j45_320eta490',
    
    'HLT_j55_L1RD0',
    'HLT_j55', # J15
    'HLT_j55_320eta490',
    
    'HLT_j85_L1RD0',
    'HLT_j85', # J20
    'HLT_j85_320eta490',
    
    'HLT_j110',
    'HLT_j110_320eta490',
    ]

HLT_jets_100K_Rules=dict(map(None,HLT_jets_100K,len(HLT_jets_100K)*[{
                     1 : TriggerRule(rate=0.5,  comment='Introduce HLT'),
                }]))
if IntroduceHLT_Jets:
        rules.update(HLT_jets_100K_Rules)

#-------END JET CALIBRATION REQUEST---------------------------------------------------------------



#############################################################################################
#
#
# tau rules
#
#
#############################################################################################


lumi_enable_hlt_tau=201
lumi_move_to_l1_tau60=51
lumi_move_to_l1_2taui=51


tau_Streamers = {
    # streamers: single
    'HLT_noalg_L1TAU30'                                               : {   1 : TriggerRule(PS=1, comment=''),
                                                        lumi_move_to_l1_tau60 : TriggerRule(PS=11, comment='Disabled') },
  
    'HLT_noalg_L1TAU20'                                               : {   1 : TriggerRule(rate=20, comment='') },
    
    #20 Hz is the target rate, but rate not available in the Cost file, prescale by hand 
    #'HLT_noalg_L1TAU12'                                               : {   1 : TriggerRule(rate=20, comment='') },
    'HLT_noalg_L1TAU12'                                               : {   1 : TriggerRule(PS=6, comment='') },    

    # streamers: di
    # keep un-prscaled for the moment
    'HLT_noalg_L1TAU20IM_2TAU12IM'                                    : {   1 : TriggerRule(PS=1, comment='') },
    
    'HLT_noalg_L1TAU20_2TAU12'                                        : {   1 : TriggerRule(PS=1, comment=''),
                                                        lumi_move_to_l1_2taui : TriggerRule(rate=5, comment='Prescaled to 5 Hz') },
}

rules.update(tau_Streamers)



#################################################################################################
#
#
# minbias rules
#
#
#################################################################################################


minbias_Streamers = {

    ##These chains are enabled as they are needed by the Express stream (see ATR-11236)
    #Target rates
#removefrom Exp stream for 8bunches (priority to lowlumi streamer)   
    'HLT_noalg_mb_L1TE20'             : { 10000 : TriggerRule(rate=2, comment='MinBias', rerun=0, ESRate=0) }, 
    #'HLT_noalg_mb_L1TE20'             : { 10000 : TriggerRule(PS=4000, comment='MinBias', rerun=0, ESValue=1) },

    #PS for 2 bunches, based on operation experience with 1b
    #'HLT_noalg_mb_L1TE20.0ETA25'       : { 10000 : TriggerRule(rate=1, comment='MinBias', rerun=0, ESRate=1) },        
    #'HLT_noalg_mb_L1TE10'             : { 10000 : TriggerRule(rate=1, comment='MinBias', rerun=0, ESRate=1) },

    #PS for 2 bunches, based on operation experience with 1b
    #'HLT_noalg_mb_L1TE20.0ETA25'       : { 10000 : TriggerRule(PS=5000, comment='MinBias', rerun=0, ESValue=5000) },        
    #'HLT_noalg_mb_L1TE10'             : { 10000 : TriggerRule(PS=5000, comment='MinBias', rerun=0, ESValue=5000) },

    #Target rates
    #'HLT_noalg_mb_L1MBTS_2'           : { 10000 : TriggerRule(rate=0.5, comment='MinBias', rerun=0, ESRate=0.5) },        
    #'HLT_noalg_mb_L1MBTS_1_1'         : { 10000 : TriggerRule(rate=2 comment='MinBias', rerun=0, ESRate=2) },        
    
    #PS for 2 bunches, based on operation experience with 1b
    'HLT_noalg_mb_L1MBTS_2'           : { 10000 : TriggerRule(PS=44000, comment='MinBias', rerun=0, ESValue=1) },        
    'HLT_noalg_mb_L1MBTS_1_1'         : { 10000 : TriggerRule(PS=22000, comment='MinBias', rerun=0, ESValue=2) },        

    #Target rates
    #'HLT_mb_perf_L1MBTS_2'           : { 10000 : TriggerRule(PS=0.5, comment='MinBias', rerun=0, ESValue=0.5) },        

    #PS for 2 bunches, based on operation experience with 1b
    'HLT_mb_perf_L1MBTS_2'           : { 10000 : TriggerRule(PS=-1, comment='MinBias', rerun=0, ESValue=-1) },        
    #'HLT_mb_perf_L1MBTS_2'           : { 10000 : TriggerRule(PS=44000, comment='MinBias', rerun=0, ESValue=-1) },        

    'HLT_noalg_mb_L1RD0_FILLED'       : { 10000 : TriggerRule(rate=0.5, comment='MinBias', rerun=0, ESValue=1) },        
    #INVALID COMMENT FOR NOW rulebook seems to be unable to adjust this so setting a ps assume a rate of 20Hz from RD0_EMPTY
    #'HLT_noalg_mb_L1RD0_EMPTY'        : { 10000 : TriggerRule(PS=20, comment='MinBias', rerun=0, ESValue=1) }, #expecting rate of 1Hz       
    'HLT_noalg_mb_L1RD0_EMPTY'        : { 10000 : TriggerRule(rate=0.5, comment='MinBias', rerun=0, ESValue=1) },        


    'HLT_mb_sptrk'      : { 10000 : TriggerRule(PS=-1, comment='MinBias', rerun=0, ESValue=-1) },        
    #'HLT_mb_sptrk'      : { 10000 : TriggerRule(rate=0.5, comment='MinBias', rerun=0, ESValue=-1) },        
    
    }

rules.update(minbias_Streamers)

#-----------------------
# HLT unprescaled chains
#-----------------------
HLT_minbias_noPS=[
    'HLT_mb_sp2_hmtperf',               
    'HLT_mb_sptrk_pt4_L1RD3_FILLED',                               
    'HLT_mb_sptrk_pt6_L1RD3_FILLED',    
    'HLT_mb_sptrk_pt8_L1RD3_FILLED',    
    'HLT_mb_sptrk_L1RD3_FILLED',    
    'HLT_mb_sp500_hmtperf',
    'HLT_mb_sp1800_hmtperf',
    'HLT_mb_sp700_trk50_hmt_L1RD3_FILLED', 
    'HLT_te50_L1RD3_FILLED',
    'HLT_mb_sp300_trk10_sumet50_hmt_L1RD3_FILLED',
    'HLT_mb_sp600_trk10_sumet50_hmt_L1RD3_FILLED'
    ]

HLT_minbias_Rules=dict(map(None,HLT_minbias_noPS,len(HLT_minbias_noPS)*[{
                         1 : TriggerRule(PS=1, comment=''), 
                         }]))
                                                                   
if IntroduceHLT_MinBias:
    rules.update({
            'L1_RD3_FILLED'                            : {   1 : TriggerRule(rate=100, scaling='bunches', comment='L1') },
            })    
    rules.update(HLT_minbias_Rules)


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
    #'L1_4J15'                              : { 1 : TriggerRule(PS=1, comment='L1') },

    ########################################
    # HLT

    # online measurement: unPS (ATR-10984)
    'HLT_beamspot_trkFS_L2StarB'           : { 1 : TriggerRule(PS=1, comment='BeamSpot') },
    'HLT_beamspot_trkFS_trkfast'           : { 1 : TriggerRule(PS=1, comment='BeamSpot') },

    # offline measurement
    # activeTE: 200 Hz, allTE: 10 Hz (ATR-10984)
    #'HLT_beamspot_activeTE_L2StarB_pebTRT' : { 1 : TriggerRule(rate=200, comment='BeamSpot') },
    #'HLT_beamspot_activeTE_trkfast_pebTRT' : { 1 : TriggerRule(rate=200, comment='BeamSpot') },
    #'HLT_beamspot_allTE_L2StarB_pebTRT'    : { 1 : TriggerRule(rate=10,  comment='BeamSpot') },
    #'HLT_beamspot_allTE_trkfast_pebTRT'    : { 1 : TriggerRule(rate=10,  comment='BeamSpot') },

    'HLT_beamspot_activeTE_L2StarB_pebTRT' : { 1 : TriggerRule(PS=1, comment='BeamSpot') },
    'HLT_beamspot_activeTE_trkfast_pebTRT' : { 1 : TriggerRule(PS=1, comment='BeamSpot') },
    'HLT_beamspot_allTE_L2StarB_pebTRT'    : { 1 : TriggerRule(PS=1,  comment='BeamSpot') },
    'HLT_beamspot_allTE_trkfast_pebTRT'    : { 1 : TriggerRule(PS=1,  comment='BeamSpot') },

}

rules.update(beamspot_Rules)


#############################################################################################
#
#
# zero bias rules 
#
#
#############################################################################################

zerobias_Rules = {
    # PS values adhoc for now
    # aiming for ~30 Hz at 5e31
    'L1_ZB'                    : { 1 : TriggerRule(PS=56, comment='L1') }, # 8 bunches 5Hz

    # 5 Hz each (ATR-9878, ATR-9922)
    # assign 10 Hz for streamer for now
    'HLT_noalg_zb_L1ZB'        : { 1 : TriggerRule(PS=1, comment='') },
    #'HLT_j40_L1ZB'             : { 1 : TriggerRule(PS=1, comment='') },
}

rules.update(zerobias_Rules)



#############################################################################################
#
#
# HLT Chains for monitoring
#
#
#############################################################################################


HLTMonitoring_Rules = {
    'HLT_mu10'                                     : {   1 : TriggerRule(PS=1, comment='Introduce HLT') },
    'HLT_mu20_iloose_L1MU15'                       : {   1 : TriggerRule(PS=1, comment='Introduce HLT') },
    'HLT_j15_bperf'                                : {   1 : TriggerRule(PS=1, comment='Introduce HLT') },
    'HLT_tau25_perf_ptonly'                        : {   1 : TriggerRule(PS=1, comment='Introduce HLT') },
    'HLT_tau25_perf_tracktwo'                      : {   1 : TriggerRule(PS=1, comment='Introduce HLT') },
    'HLT_tau25_loose1_tracktwo'                    : {   1 : TriggerRule(PS=1, comment='Introduce HLT') },
    'HLT_tau25_medium1_tracktwo'                   : {   1 : TriggerRule(PS=1, comment='Introduce HLT') },
    'HLT_tau25_tight1_tracktwo'                    : {   1 : TriggerRule(PS=1, comment='Introduce HLT') },
    'HLT_tau25_perf_tracktwo_L1TAU12'              : {   1 : TriggerRule(PS=1, comment='Introduce HLT') },
    'HLT_mu4_idperf'                               : {   1 : TriggerRule(PS=1, comment='Introduce HLT') },
    'HLT_mu6_idperf'                               : {   1 : TriggerRule(PS=1, comment='Introduce HLT') },
    'HLT_mu6_L2Star_idperf'                        : {   1 : TriggerRule(PS=1, comment='Introduce HLT') },
    'HLT_e5_loose_idperf'                          : {   1 : TriggerRule(PS=1, comment='Introduce HLT') },
    'HLT_e5_loose_L2Star_idperf'                   : {   1 : TriggerRule(PS=1, comment='Introduce HLT') },
    'HLT_tau25_idperf_track'                       : {   1 : TriggerRule(PS=1, comment='Introduce HLT') },
    'HLT_tau25_idperf_tracktwo'                    : {   1 : TriggerRule(PS=1, comment='Introduce HLT') },
    'HLT_2mu4_bDimu_novtx_noos'                    : {   1 : TriggerRule(PS=1, comment='Introduce HLT') },
    'HLT_e17_lhloose_L1EM15'                       : {   1 : TriggerRule(PS=1, comment='Introduce HLT') },
    'HLT_2g10_loose'                               : {   1 : TriggerRule(PS=1, comment='Introduce HLT') },
    #'HLT_mb_perf_L1MBTS_2'                                        : { 10000 : TriggerRule(PS=50.0, comment = 'MinBias', rerun = 0.0, ESValue=200) },
    'HLT_j0_lcw_jes_L1J12'                                        : { 10000 : TriggerRule(PS=1, comment = 'Main', rerun = 0.0) }, 
    'HLT_j0_perf_bperf_L1MU10'                                    : { 10000 : TriggerRule(PS=1, comment = 'Main', rerun = 0.0) },
    'HLT_te20'                                                    : { 10000 : TriggerRule(PS=100, comment = 'Main', rerun = 0.0) }, 
    'HLT_xe35'                                                    : { 10000 : TriggerRule(PS=1.0, comment = 'Main', rerun = 0.0) },
    'HLT_2mu4_bDimu'                                              : { 10000 : TriggerRule(PS=1.0, comment = 'Main', rerun = 0.0) },
    'HLT_tau35_perf_tracktwo_L1TAU20_tau25_perf_tracktwo_L1TAU12' : { 10000 : TriggerRule(PS=1.0, comment = 'Main', rerun = 0.0) },
    'HLT_tau35_perf_tracktwo_tau25_perf_tracktwo'                 : { 10000 : TriggerRule(PS=1.0, comment = 'Main', rerun = 0.0) },
    
    #20.2.1.4
    'HLT_mu14_iloose_tau25_perf_tracktwo' : { 10000 : TriggerRule(PS=1, comment= "Introduce HLT" , ESValue=10,rerun=0) },


    #'HLT_noalg_mb_L1RD0_UNPAIRED_ISO' : { 10000 : TriggerRule(PS=1, comment='MinBias', rerun=0) },

    #'HLT_mu6_L2Star_idperf'                                       : { 10000 : TriggerRule(rate=3, comment='IDMonitoring', rerun=0) },
    #'HLT_mu6_idperf'                                              : { 10000 : TriggerRule(rate=3, comment='IDMonitoring', rerun=0) },
    #'HLT_mu4_idperf'                                              : { 10000 : TriggerRule(rate=3, comment='IDMonitoring', rerun=0) },
    #'HLT_e5_loose_idperf'                                         : { 10000 : TriggerRule(rate=3, comment='IDMonitoring', rerun=0) },
    #'HLT_tau25_idperf_track'                                      : { 10000 : TriggerRule(rate=3, comment='IDMonitoring', rerun=0) },
    #'HLT_tau25_idperf_tracktwo'                                   : { 10000 : TriggerRule(rate=3, comment='IDMonitoring', rerun=0) },


    }

if IntroduceHLT_Monitoring:
    rules.update(HLTMonitoring_Rules)


#############################################################################################
#
#
# monitoring rules (import from Monitoring_pp_v5_rules.py)
#
#
#############################################################################################


monitoring_Rules = {

}

rules.update(monitoring_Rules)

import Monitoring_pp_v5_rules

#check that rules are not duplicated                                                                                                                                   
duplication_list = [ rule  for rule in Monitoring_pp_v5_rules.physics_rules if rules.has_key(rule) ]
if duplication_list:
    print "FATAL: duplicated rules for chains: "
    print '\n'.join([ str(myelement)  for myelement in duplication_list ])
    sys.exit("FATAL rulebook failed, exiting")


rules.update(Monitoring_pp_v5_rules.physics_rules)



#############################################################################################
#
#
# AfterGlow Rules
#
#
#############################################################################################

afterGlow_Rules={
    'HLT_lumipeb_vdm_L1RD0_BGRP9'                 : {   1 : TriggerRule(rate=8620, comment=''),},
    'HLT_lumipeb_vdm_L1RD0_FILLED'                : {   1 : TriggerRule(rate=230, comment=''),},
    'HLT_lumipeb_vdm_L1RD0_UNPAIRED_ISO'          : {   1 : TriggerRule(rate=8620, comment=''),},    
    }

if AfterGlow:
    print "Update rules for the AfterGlow run"
    lumipeb_items=[item for item, rule in rules.items() if "lumipeb" in item and not "lumipeb_L1RD0_EMPTY" in item]    
    for item in lumipeb_items:
        del rules[item]        
    rules.update(afterGlow_Rules)
    



#############################################################################################
#
#
# EB rules (import from Physics_pp_v5_EB_rules.py)
#
#
#############################################################################################

if Introduce_EB:
    import Physics_pp_v5_EB_rules

    from copy import deepcopy    
    ebRules = deepcopy(Physics_pp_v5_EB_rules.rules)

    rules.update(ebRules)

    
#############################################################################################
#
#
# 5e33 rules
#
#
#############################################################################################

if IntroduceHLT_5e33:
    import Physics_pp_v5_5e33_rules

    #check that rules are not duplicated
    duplication_list = [ rule  for rule in Physics_pp_v5_Week1_5e33_rules.rules if rules.has_key(rule) ]
    if duplication_list:
        print "FATAL: duplicated rules for chains: "
        print '\n'.join([ str(myelement)  for myelement in duplication_list ])
        sys.exit("FATAL rulebook failed, exiting")

    from copy import deepcopy    
    hlt5e33Rules = deepcopy(Physics_pp_v5_Week1_5e33_rules.rules)

    rules.update(hlt5e33Rules)

    
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
