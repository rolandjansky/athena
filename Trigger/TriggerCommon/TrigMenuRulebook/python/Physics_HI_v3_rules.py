# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'Physics_HI_v3'

forRateEstim=False
doHLTtransparent=False  #HLT commissioning on Nov23 rd

doVdMOthers=False  #True for a VdM scan on Dec 3rd
doEB=False         # True for EB data on Dec 11th

from runOptions import EnableL1Topo_HIMenuOnly as EnableL1Topo

#-----------------------------------------------------
#Importing Monitoring HI v3 rules
import Monitoring_HI_v3_rules
#-----------------------------------------------------

from runOptions import filled_bunches as NumBunches
HLT_minbias_highlumi_ps = 4 #for 50 bunches
HLT_minbias_lowlumi_ps = 1.5 #for 50 bunches
HLT_overlay_ps = 150 #for 50 bunches
ZDC_A_C_ps = NumBunches*0.5
ZDC_A_ps   = NumBunches*10
ZDC_XOR_ps = NumBunches*320

rules = {}

if doEB:
    import Physics_HI_v3_EB_rules
    
    RulesList=[Physics_HI_v3_EB_rules.stream_eb_rules]
    if forRateEstim or doHLTtransparent:
        RulesList=[]

    for Rules in RulesList:
        for newrule in Rules.keys():
            if rules.has_key(newrule):
                print 'FATAL     Physics_HI_v3_rules     Duplicated rule in EB rule. Cannot be added from monitoring rule for:',newrule
    rules.update(Rules)

    #decrease main min-bias rates to make room for EB data at 100Hz
    HLT_minbias_highlumi_ps = 6 #for 50 bunches
    HLT_minbias_lowlumi_ps = 2 #for 50 bunches

#################################
# triggers for VdM scan (Dec 3rd)
#################################
if doVdMOthers:

    vdm_rules = {}

    # --------------------------------------
    # PixelBeam
    Monitoring_HI_v3_rules.physics_rules['HLT_lumipeb_L1RD0_FILLED'] = {
                        10 : TriggerRule(inputRate=1000, comment='PixelBeam',  rerun=0),
    }
    Monitoring_HI_v3_rules.physics_rules['HLT_lumipeb_L1RD0_UNPAIRED_ISO'] = {
                        10 : TriggerRule(inputRate=60, comment='PixelBeam',  rerun=0),
    }
    Monitoring_HI_v3_rules.physics_rules['HLT_lumipeb_L1RD0_ABORTGAPNOTCALIB'] = {
                        10 : TriggerRule(inputRate=40, comment='PixelBeam',  rerun=0),
                        }
    
    #zdcpeb
    Monitoring_HI_v3_rules.physics_rules['L1_ZDC_A'] = {
        10 : TriggerRule(PS=1000, comment='zdcpeb at 150kHz',  rerun=0),
        }
    Monitoring_HI_v3_rules.physics_rules['L1_ZDC_C'] = {
        10 : TriggerRule(PS=1000, comment='zdcpeb at 150kHz',  rerun=0),
        }
    Monitoring_HI_v3_rules.physics_rules['L1_ZDC_A_C'] = {
        10 : TriggerRule(PS=100, comment='zdcpeb at 150kHz',  rerun=0),
        }
    Monitoring_HI_v3_rules.physics_rules['L1_ZDC_AND'] = {
        10 : TriggerRule(PS=-1, comment='zdcpeb disabled',  rerun=0),
        }

    rules.update(vdm_rules)


#################################
# streamers for HLT transparent (Nov 23rd)
#################################

if doHLTtransparent:
    print 'WARNING: HLT in transparent mode'
    eb_rules={}

    #physics
    L1_list=['L1_MBTS_1_1', 
             'L1_ZDC_AND', 'L1_ZDC_A_C', 'L1_ZDC_XOR_TE5_VTE200',
             'L1_TE5', 'L1_TE50']
    eb_rules.update(dict(map(None,L1_list,len(L1_list)*[{
                        400 : TriggerRule(PS=1, comment='L1'),
                        }])))

    L1_list=['L1_ZDC_XOR']
    eb_rules.update(dict(map(None,L1_list,len(L1_list)*[{
                        400 : TriggerRule(PS=ZDC_XOR_ps, comment='L1'),
                        }])))
    
    HLT_list=[ 'HLT_noalg_eb_L1MBTS_1_1', 'HLT_noalg_eb_L1ZDC_AND',
               'HLT_noalg_eb_L1ZDC_A_C', 'HLT_noalg_eb_L1ZDC_XOR',
               'HLT_noalg_eb_L1ZDC_XOR_TE5_VTE200']
    eb_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        400 : TriggerRule(PS=1., comment=''), 
                        }])))
    rules.update(eb_rules)

    HLT_list=[ 'HLT_noalg_mb_L1ZDC_AND']
    eb_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        400 : TriggerRule(PS=1., comment=''), 
                        }])))

    HLT_list=[ 'HLT_noalg_mb_L1ZDC_A_C', 
               'HLT_noalg_mb_L1TE5',
               'HLT_noalg_mb_L1TE50']
    eb_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        400 : TriggerRule(PS=1., comment='', ESValue=0), 
                        }])))

    HLT_list=[ 'HLT_noalg_mb_L1MBTS_1_1']
    eb_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                        400 : TriggerRule(PS=1., comment='', ESValue=2),
                        }])))

    rules.update(eb_rules)
    ########################################
    # Pixel Beam: 1000 Hz in 50ns run (500 Hz later in 25ns run)


    stream_PixelBeam_rules = { 
        'HLT_lumipeb_L1RD0_FILLED'           : {  1 : TriggerRule(PS=-1, comment='PixelBeam disabled in cosmic',  rerun=0),
                                                  2 : TriggerRule(inputRate=180, comment='PixelBeam',  rerun=0) },
        'HLT_lumipeb_L1RD0_UNPAIRED_ISO'     : {  1 : TriggerRule(PS=-1, comment='PixelBeam disabled in cosmic',  rerun=0),
                                                  2 : TriggerRule(inputRate=12,  comment='PixelBeam',  rerun=0) },
        'HLT_lumipeb_L1RD0_ABORTGAPNOTCALIB' : {  1 : TriggerRule(PS=-1, comment='PixelBeam disabled in cosmic',  rerun=0),
                                                  2 : TriggerRule(inputRate=8,  comment='PixelBeam',  rerun=0) },
        }
    rules.update(stream_PixelBeam_rules)
########################################
# Pixel Noise: 10 Hz
    
    stream_PixelNoise_Rules = {
        'HLT_lumipeb_L1RD0_EMPTY'            : {  1 : TriggerRule(inputRate=10, comment='PixelNoise', rerun=0) }, 
        }
    rules.update(stream_PixelNoise_Rules)

########################################
# SCT Noise: 10 Hz

    stream_SCTNoise_Rules = {
        'HLT_sct_noise'                      : {  1 : TriggerRule(inputRate=10, scaling="bunches", comment='SCTNoise', rerun=0) },
        }
    
    rules.update(stream_SCTNoise_Rules)

########################################
# CSC monitoring: 1% of all L1 accepts both cosmic and physics
    
    stream_CSCMonitoring_Rules = {
        'HLT_cscmon_L1All'                   : {  1 : TriggerRule(PS=100, comment='CSC', rerun=0) },
        }

    rules.update(stream_CSCMonitoring_Rules)

########################################
# Tile: 3 Hz
    
    stream_Tile_Rules = {
        'HLT_tilecalib_laser'                : { 1 : TriggerRule(PS=1, comment='Tile', rerun=0) },
        }
    rules.update(stream_Tile_Rules)
########################################

# ID Monitoring: 10 Hz
    
    stream_IDMonitoring_Rules = {
        'HLT_noalg_idmon_L1RD0_EMPTY'       : {  1 : TriggerRule(inputRate=10, scaling="bunches", comment='IDMonitoring', rerun=0),
                                                 2 : TriggerRule(inputRate=0.5, scaling="bunches", comment='IDMonitoring', rerun=0) },
        'HLT_noalg_idmon_L1RD0_FILLED'      : {  1 : TriggerRule(inputRate=5.0, scaling="bunches", comment='IDMonitoring', rerun=0) },
        'HLT_noalg_idmon_L1RD0_UNPAIRED_ISO': {  1 : TriggerRule(inputRate=5.0, scaling="bunches", comment='IDMonitoring', rerun=0) }, 
        'HLT_noalg_dcmmon_L1RD0_EMPTY'        : {  1 : TriggerRule(rate=10.0, comment='monitoring_random', rerun=0, ESValue=0) },
}
    rules.update(stream_IDMonitoring_Rules)

########################################
# Muon calibration

    stream_MuonCalibration_Rules = {

    # MU4: beam
    'HLT_mu0_muoncalib'                  : {  1 : TriggerRule(PS=-1, comment='disabled in cosmic', rerun=0),
                                              2 : TriggerRule(PS=1, comment='Muon_Calibration', rerun=0),
                                              },
    }    

    rules.update(stream_MuonCalibration_Rules)

    stream_CosmicCalo_Rules = {
    'HLT_noalg_cosmiccalo_L1RD1_EMPTY'   : { 1 : TriggerRule(inputRate=1.0, comment='CosmicCalo', rerun=0, ESValue=1),
                                             2 : TriggerRule(inputRate=1.0, comment='CosmicCalo', rerun=0, ESValue=0) },
  }

    rules.update(stream_CosmicCalo_Rules)

else:
    print 'Nominal PS sets are generated'

##############################
# min bias
#############################
minbias_rules={}

minbias_rules = {

    'L1_RD0_FILLED'                      : {   2 : TriggerRule(rate=5000, scaling='bunches', comment='monitoring+lumi') },
    'HLT_mb_sp'                          : {   0.003  : TriggerRule(inputRate=400, comment='Target = 20 Hz', ESValue=320)
                                               }, 

    #supporting
    'HLT_mb_mbts_L1MBTS_1_1'             : {   2 : TriggerRule(rate=2, comment='Target =2 Hz', ESRate=0.2)}, 
    'HLT_mb_idperf_ion_L1MBTS_1_1'       : {   2 : TriggerRule(PS=-1, comment='Target =2 Hz', ESRate=0.2)},
    'HLT_mb_perf_L1RD1_FILLED'           : {   0.001 : TriggerRule(rate=10, comment='Target =2 Hz'),
                                               0.002 : TriggerRule(rate=5, comment='Target =2 Hz'),
                                               0.003 : TriggerRule(rate=2, comment='Target =2 Hz'),
                                               },
    'HLT_mb_perf_L1RD1_EMPTY'            : {   0.001 : TriggerRule(rate=10, comment='Target =2 Hz'),
                                               0.002 : TriggerRule(rate=5, comment='Target =2 Hz'),
                                               0.003 : TriggerRule(rate=3, comment='Target =2 Hz')
                                               },

    #physics
    'HLT_mb_sptrk_ion_L1ZDC_A_C_VTE50'   : {
        0.00005 : TriggerRule(PS=1,    comment='Target 100 Hz', rerun=1, ESRate=0.5),
        #0.00006 : TriggerRule(PS=HLT_minbias_lowlumi_ps*0.5,    comment='Target 100 Hz', rerun=1, ESRate=0.5),
        #0.00007 : TriggerRule(PS=HLT_minbias_lowlumi_ps*0.6,    comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.00008 : TriggerRule(PS=HLT_minbias_lowlumi_ps*0.7,    comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.00009 : TriggerRule(PS=HLT_minbias_lowlumi_ps*0.8,    comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.0001  : TriggerRule(PS=HLT_minbias_lowlumi_ps*0.9,    comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.0002  : TriggerRule(PS=HLT_minbias_lowlumi_ps,    comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.0003  : TriggerRule(PS=HLT_minbias_lowlumi_ps*1.5,  comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.0004  : TriggerRule(PS=HLT_minbias_lowlumi_ps*2.2,  comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.0005  : TriggerRule(PS=HLT_minbias_lowlumi_ps*3.5,  comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.0006  : TriggerRule(PS=HLT_minbias_highlumi_ps*1.8,  comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.0007  : TriggerRule(PS=HLT_minbias_highlumi_ps*2,  comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.0008  : TriggerRule(PS=HLT_minbias_highlumi_ps*3,  comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.001   : TriggerRule(PS=HLT_minbias_highlumi_ps*4,  comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.0015  : TriggerRule(PS=HLT_minbias_highlumi_ps*6,  comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.002   : TriggerRule(PS=HLT_minbias_highlumi_ps*8,  comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.003   : TriggerRule(PS=HLT_minbias_highlumi_ps*12, comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.004   : TriggerRule(PS=HLT_minbias_highlumi_ps*16, comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.005   : TriggerRule(PS=HLT_minbias_highlumi_ps*20, comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.006   : TriggerRule(PS=HLT_minbias_highlumi_ps*24, comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.007   : TriggerRule(PS=HLT_minbias_highlumi_ps*28, comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.008   : TriggerRule(PS=HLT_minbias_highlumi_ps*32, comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.01   : TriggerRule(PS=HLT_minbias_highlumi_ps*100, comment='Target 100 Hz', rerun=1, ESRate=0.5),
        }, 
    'HLT_noalg_mb_L1TE50'                : {   
        0.00005  : TriggerRule(PS=1, comment='Target 100 Hz', ESRate=0.3),
        #0.00006  : TriggerRule(PS=HLT_minbias_lowlumi_ps*0.5, comment='Target 100 Hz', ESRate=0.3),
        #0.00007  : TriggerRule(PS=HLT_minbias_lowlumi_ps*0.6, comment='Target 100 Hz', ESRate=0.3),
        0.00008  : TriggerRule(PS=HLT_minbias_lowlumi_ps*0.7, comment='Target 100 Hz', ESRate=0.3),
        0.00009  : TriggerRule(PS=HLT_minbias_lowlumi_ps*0.8, comment='Target 100 Hz', ESRate=0.3),
        0.0001   : TriggerRule(PS=HLT_minbias_lowlumi_ps*0.9, comment='Target 100 Hz', ESRate=0.3),
        0.0002   : TriggerRule(PS=HLT_minbias_lowlumi_ps, comment='Target 100 Hz', ESRate=0.3),
        0.0003   : TriggerRule(PS=HLT_minbias_lowlumi_ps*1.5, comment='Target 100 Hz', ESRate=0.3),
        0.0004   : TriggerRule(PS=HLT_minbias_lowlumi_ps*2.2, comment='Target 100 Hz', ESRate=0.3),
        0.0005   : TriggerRule(PS=HLT_minbias_lowlumi_ps*3.5, comment='Target 100 Hz', ESRate=0.3),
        0.0006   : TriggerRule(PS=HLT_minbias_highlumi_ps*1.8, comment='Target 100 Hz', ESRate=0.3),
        0.0007   : TriggerRule(PS=HLT_minbias_highlumi_ps*2, comment='Target 100 Hz', ESRate=0.3),
        0.0008   : TriggerRule(PS=HLT_minbias_highlumi_ps*3,  comment='Target 100 Hz', ESRate=0.5),
        0.001    : TriggerRule(PS=HLT_minbias_highlumi_ps*4, comment='Target 100 Hz', ESRate=0.3),
        0.0015   : TriggerRule(PS=HLT_minbias_highlumi_ps*6, comment='Target 100 Hz', ESRate=0.3),
        0.002    : TriggerRule(PS=HLT_minbias_highlumi_ps*8, comment='Target 100 Hz', ESRate=0.3),
        0.003    : TriggerRule(PS=HLT_minbias_highlumi_ps*12, comment='Target 100 Hz', ESRate=0.3),
        0.004    : TriggerRule(PS=HLT_minbias_highlumi_ps*16, comment='Target 100 Hz', ESRate=0.3),
        0.005   : TriggerRule(PS=HLT_minbias_highlumi_ps*20, comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.006   : TriggerRule(PS=HLT_minbias_highlumi_ps*24, comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.007   : TriggerRule(PS=HLT_minbias_highlumi_ps*28, comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.008   : TriggerRule(PS=HLT_minbias_highlumi_ps*32, comment='Target 100 Hz', rerun=1, ESRate=0.5),
        0.01   : TriggerRule(PS=HLT_minbias_highlumi_ps*100, comment='Target 100 Hz', rerun=1, ESRate=0.5),
        }, 

    #supporting
    'HLT_noalg_mb_L1ZDC_A_C'        : {  0.001 : TriggerRule(PS=40., comment='Target 0.5 Hz', ESValue=1),}, 


    #overlay (first run)
    'HLT_noalg_L1ZDC_A_C_OVERLAY'        : {    0. : TriggerRule(PS=HLT_overlay_ps, comment='Target 40 Hz'),
                                                0.00025 : TriggerRule(PS=-1, comment='Target 40 Hz'),                                             }, 
    
    # overlay (nominal)
    'HLT_mb_sptrk_ion_L1ZDC_A_C_VTE50_OVERLAY' : {  0.002 : TriggerRule(PS=125*10, comment='Target 2 Hz'),
                                                    0.004 : TriggerRule(PS=125*20, comment='Target 2 Hz'),
                                                    0.006 : TriggerRule(PS=125*60, comment='Target 2 Hz'),
                                                    },
    'HLT_noalg_L1TE50_OVERLAY'                 : { 0.002 : TriggerRule(PS=25*10,  comment='Target 18 Hz'),
                                                   0.004 : TriggerRule(PS=25*20,  comment='Target 18 Hz'),
                                                   0.006 : TriggerRule(PS=25*60,  comment='Target 18 Hz'),
                                                   },
    'HLT_noalg_mb_L1TE1500.0ETA49_OVERLAY'     : {  0.002 : TriggerRule(PS=20*10,  comment='Target 10 Hz'),
                                                    0.004 : TriggerRule(PS=20*20,  comment='Target 10 Hz'),
                                                    0.006 : TriggerRule(PS=20*60,  comment='Target 10 Hz'),
                                                    },
    'HLT_noalg_mb_L1TE6500.0ETA49_OVERLAY'     : {  0.002 : TriggerRule(PS=10*10,  comment='Target 10 Hz'),
                                                    0.004 : TriggerRule(PS=10*20,  comment='Target 10 Hz'),
                                                    0.006 : TriggerRule(PS=10*60,  comment='Target 10 Hz'),
                                                    },
    }


if not doHLTtransparent:
    rules.update(minbias_rules)

##############################
# UCC
#############################
ucc_rules={}

ucc_rules = {

    'HLT_hi_th1_ucc_L1TE10000'          : {   0.002 : TriggerRule(PS=-1,  comment='disable for nominal lumi')},
    'HLT_hi_th2_ucc_L1TE10000'          : {   2 : TriggerRule(PS=-1,  comment='') },
    'HLT_hi_th3_ucc_L1TE10000'          : {   2 : TriggerRule(PS=1,  comment='') },

    'HLT_hi_th1_ucc_L1TE12000'          : {   0.002 : TriggerRule(PS=-1,  comment='disable for nominal lumi')},
    'HLT_hi_th2_ucc_L1TE12000'          : {   2 : TriggerRule(PS=-1,  comment='') },
    'HLT_hi_th3_ucc_L1TE12000'          : {   2 : TriggerRule(PS=1,  comment='') },

    'HLT_hi_th1_ucc_L1TE14000'          : {   0.002 : TriggerRule(PS=-1,  comment='disable for nominal lumi')},
    'HLT_hi_th2_ucc_L1TE14000'          : {   2 : TriggerRule(PS=-1,  comment='') },
    'HLT_hi_th3_ucc_L1TE14000'          : {   2 : TriggerRule(PS=1,  comment='') },

    #supporting
    'HLT_hi_th1_zdcpu_ucc_L1TE10000'        : {   2 : TriggerRule(PS=-1,  comment='', ESValue=50),},
    'HLT_hi_th3_zdcpu_ucc_L1TE10000'        : {   2 : TriggerRule(PS=1,  comment='', ESValue=50),},
    'HLT_hi_perfzdc_ucc_L1ZDC_A_C'          : {   2 : TriggerRule(PS=200,  comment='') },
}

if not doHLTtransparent:
    rules.update(ucc_rules)

##############################
# ES triggers
#############################
es_rules={}

es_rules = {
    #calibration
    'HLT_hi_v2_th16_L1TE50'        : {  2 : TriggerRule(PS=10,  comment='') },
    'HLT_hi_v3_th16_L1TE50'        : {  2 : TriggerRule(PS=10,  comment='') },

    #physics (unprescaled)
    'HLT_hi_v2_th5_L1TE50'         : {  0.002 : TriggerRule(PS=1,  comment='gives 22 Hz at 2e27') },

    'HLT_hi_v2_th10_L1TE50'         : { 0.003 : TriggerRule(PS=2, comment = '', rerun = 0.0),
                                        0.002 : TriggerRule(PS=1, comment = '46 Hz wiht PS 1', rerun = 0.0),
                                        0.001 : TriggerRule(PS=1, comment = '', rerun = 0.0) }, 

    'HLT_hi_v3_th10_L1TE50'        : {  0.003 : TriggerRule(PS=2,  comment=''),
                                        0.002 : TriggerRule(PS=1,  comment='gives 39 Hz at 2e27'),
                                        0.001 : TriggerRule(PS=1,  comment='') },

    'HLT_hi_v3_th13_veto2_L1TE50'         : { 0.0025 : TriggerRule(PS=8,  comment=''),
                                              0.002 : TriggerRule(PS=6,  comment='86 with PS = 1'),
                                               0.0015 : TriggerRule(PS=3,  comment=''),
                                               0.001 : TriggerRule(PS=2,  comment=''),
                                               0.0005 : TriggerRule(PS=1,  comment='') },

    'HLT_hi_v2_th13_veto3_L1TE50'         : {  0.0025 : TriggerRule(PS=6,  comment=''),
                                               0.002 : TriggerRule(PS=3,  comment='64 with PS = 1'),
                                               0.001 : TriggerRule(PS=2,  comment=''),
                                               0.0005 : TriggerRule(PS=1,  comment='') },

    'HLT_hi_v23_th14_L1TE50'       : {  2 : TriggerRule(PS=1,  comment='9.2 Hz') },
    'HLT_hi_v23_th15_L1TE50'       : {  2 : TriggerRule(PS=1,  comment='2 Hz') },

    #physics prescaled
    'HLT_hi_v2_th15_L1TE50'         : {  2 : TriggerRule(PS=10,  comment='110 Hz') },
    'HLT_hi_v2_th14_L1TE50'         : {  2 : TriggerRule(PS=0,  comment='360') },
    'HLT_hi_v2_th13_L1TE50'         : {  2 : TriggerRule(PS=0,  comment='720') },

    'HLT_hi_v3_th15_L1TE50'         : {  2 : TriggerRule(PS=0,  comment='97') },
    'HLT_hi_v3_th14_L1TE50'         : {  2 : TriggerRule(PS=0,  comment='360') },
    'HLT_hi_v3_th13_L1TE50'         : {  2 : TriggerRule(PS=0,  comment='720') },
    'HLT_hi_v3_th5_L1TE50'         : {  2 : TriggerRule(PS=0,  comment='1440') },
}

if not doHLTtransparent:
    rules.update(es_rules)


##############################
# UPC
#############################
upc_rules={}

upc_rules = {

    #primary
    'HLT_mb_sptrk_vetombts2in_L1MU0_VTE50'    : {   2 : TriggerRule(PS=1, comment='UPC primary',ESRate=0.3) },
    'HLT_mb_sptrk_vetombts2in_L1ZDC_A_C_VTE50': {   2 : TriggerRule(rate=5., comment='UPC primary') },
    'HLT_hi_gg_upc_L1TE5_VTE200'              : {   2 : TriggerRule(rate=10., comment='UPC primary') },
    'HLT_hi_loose_upc_L1ZDC_A_C_VTE50'        : {   0.001 : TriggerRule(rate=3., comment='UPC primary'),
                                                    },
    
    #photoproduction
    'HLT_j10_ion_mb_mbts_vetombts1side2in_L1ZDC_XOR_TE5_VTE200'    : {   2 : TriggerRule(PS=1, comment='') },
    'HLT_j15_ion_mb_mbts_vetombts1side2in_L1ZDC_XOR_TE5_VTE200'    : {   2 : TriggerRule(PS=1, comment='') },
    'HLT_j20_ion_mb_mbts_vetombts1side2in_L1ZDC_XOR_TE5_VTE200'    : {   2 : TriggerRule(PS=1, comment='') },

    # unprescaled as input rate is small 
    'HLT_j5_ion_L1ZDC_XOR_TE5_VTE200'     : {   2 : TriggerRule(PS=1, comment='') },
    'HLT_j10_ion_L1ZDC_XOR_TE5_VTE200'    : {   2 : TriggerRule(PS=1, comment='') },
    'HLT_j15_ion_L1ZDC_XOR_TE5_VTE200'    : {   2 : TriggerRule(PS=1, comment='') },
    'HLT_j20_ion_L1ZDC_XOR_TE5_VTE200'    : {   2 : TriggerRule(PS=1, comment='') },
    'HLT_mb_sptrk_ion_L1ZDC_XOR_TE5_VTE200' : { 0.002: TriggerRule(PS=100, comment=''), 
                                                0.001: TriggerRule(PS=50, comment='') },
    
    # need to prescale heavilly
    'HLT_j5_ion_L1ZDC_A_C_TE5_VTE200'     : {   0.002 : TriggerRule(PS=150, comment=''), 
                                                0.001 : TriggerRule(PS=75, comment='') },
    'HLT_j10_ion_L1ZDC_A_C_TE5_VTE200'    : {   0.002 : TriggerRule(PS=75, comment=''), 
                                                0.001 : TriggerRule(PS=37, comment='') },
    'HLT_j15_ion_L1ZDC_A_C_TE5_VTE200'    : {   0.002 : TriggerRule(PS=50, comment=''), 
                                                0.001 : TriggerRule(PS=25, comment='') },
    'HLT_j20_ion_L1ZDC_A_C_TE5_VTE200'    : {   0.002 : TriggerRule(PS=20, comment=''), 
                                                0.001 : TriggerRule(PS=10, comment='') },
    'HLT_mb_sptrk_ion_L1ZDC_A_C_TE5_VTE200' : { 0.002: TriggerRule(PS=3000, comment=''), 
                                                0.001 : TriggerRule(PS=1500, comment='') },

    'HLT_j5_ion_L1TE5_VTE200'     : {   0.002 : TriggerRule(PS=150, comment=''), 
                                        0.001 : TriggerRule(PS=75, comment='') },
    'HLT_j10_ion_L1TE5_VTE200'    : {   0.002 : TriggerRule(PS=75, comment=''), 
                                        0.001 : TriggerRule(PS=37, comment='') },
    'HLT_j15_ion_L1TE5_VTE200'    : {   0.002 : TriggerRule(PS=50, comment=''), 
                                        0.001 : TriggerRule(PS=25, comment='') },
    'HLT_j20_ion_L1TE5_VTE200'    : {   0.002 : TriggerRule(PS=20, comment=''), 
                                        0.001 : TriggerRule(PS=10, comment='') },


    #forward
    'HLT_j10_320eta490_ion_L1TE5_VTE200'    : {   2 : TriggerRule(PS=1, comment='') },
    'HLT_j10_320eta490_ion_L1ZDC_A_C_TE5_VTE200'    : {   2 : TriggerRule(PS=1, comment='') },
    'HLT_j10_320eta490_ion_L1ZDC_XOR_TE5_VTE200'    : {   2 : TriggerRule(PS=1, comment='') },

    #supporting
    'HLT_noalg_L1MU0_VTE50'              : {   2 : TriggerRule(rate=10, comment='') },
    'HLT_noalg_L1ZDC_A_C_VTE50'          : {   0. : TriggerRule(rate=10, comment=''),
                                               0.0001  : TriggerRule(rate=2, comment='') },
    'HLT_noalg_L1ZDC_XOR'                : {   2 : TriggerRule(PS=ZDC_XOR_ps, comment='') },
    'HLT_noalg_L1ZDC_XOR_TE5_VTE200'     : {   2 : TriggerRule(PS=5, comment='') },
    'HLT_mb_sptrk_ion_L1ZDC_VTE200'      : {   2 : TriggerRule(PS=NumBunches*10, comment='2 Hz', rerun=1)},
    
}

if not doHLTtransparent:
    rules.update(upc_rules)

##############################
# muons
#############################
muon_rules={}

# physics
L1_list=['L1_MU4']
muon_rules.update(dict(map(None,L1_list,len(L1_list)*[{
    0.01 : TriggerRule(PS=1, comment='L1'),
    0.02 : TriggerRule(PS=10, comment='L1'),
    }])))

L1_list=['L1_MU6', 'L1_MU11', 'L1_MU15', 
         'L1_MU20', 'L1_2MU4', 'L1_2MU6']
muon_rules.update(dict(map(None,L1_list,len(L1_list)*[{
    400 : TriggerRule(PS=1, comment='L1'),
    }])))


HLT_list=[ 'HLT_mu8',
           'HLT_mu20_msonly',
           'HLT_2mu4', 'HLT_2mu6', 
           'HLT_2mu4_msonly', 'HLT_2mu6_msonly']
muon_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=1, comment=''), 
                    }])))

HLT_list=[ 'HLT_mu4_mu4noL1']
muon_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    0.009 : TriggerRule(PS=1, comment='',ESRate=0.3), 
                    0.01  : TriggerRule(PS=100, comment='',ESRate=0.3), 
                    0.02  : TriggerRule(PS=1, comment='',ESRate=0.3), 
                    }])))

HLT_list=[ 'HLT_mu4_mu2noL1', 'HLT_mu6_mu4noL1']
muon_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    0.009 : TriggerRule(PS=1, comment=''), 
                    0.01  : TriggerRule(PS=100, comment=''), 
                    0.02  : TriggerRule(PS=1, comment=''), 
                    }])))

HLT_list=['HLT_mu4_mu4_idperf_bJpsimumu_noid', 'HLT_mu13_mu13_idperf_Zmumu',
          'HLT_2mu4_bDimu', 'HLT_2mu4_bDimu_novtx_noos']
muon_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=1, comment='offline monitoring', ESRate=0.3), 
                    }])))

HLT_list=[ 'HLT_mu14', 'HLT_mu15_msonly', 'HLT_mu10']
muon_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=1, comment='',ESRate=0.5), 
                    }])))

HLT_list=[ 'HLT_mu6']
muon_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    0.009 : TriggerRule(PS=1, comment='',rerun=1), 
                    0.01  : TriggerRule(PS=2, comment='',rerun=1), 
                    }])))

# supporting
HLT_list=[ 'HLT_mu4']
muon_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    0.0005  : TriggerRule(PS=1, comment='',rerun=1, ESRate=0.4), 
                    0.001   : TriggerRule(rate=40., comment='',rerun=1, ESRate=0.4),
                    0.01   : TriggerRule(rate=10., comment='',rerun=1, ESRate=0.4),
                    }])))

HLT_list=[ 'HLT_mu4noL1']
muon_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    0.009 : TriggerRule(rate=5., comment='',rerun=1), 
                    0.01  : TriggerRule(rate=5., comment='',rerun=0), 
                    }])))

HLT_list=[ 'HLT_mu2noL1']
muon_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=-1, comment=''), 
                    }])))

HLT_list=[ 'HLT_noalg_L1MU4']
muon_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(rate=1, comment='offline monitoring 0.3Hz', ESRate=0.3), 
                    }])))


HLT_list=[ 'HLT_noalg_L1MU6']
muon_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(rate=0.5, comment='offline monitoring 0.3Hz', ESRate=0.3), 
                    }])))

HLT_list=[ 'HLT_noalg_L1MU11']
muon_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(rate=0.3, comment='offline monitoring 0.3Hz', ESValue=1), 
                    }])))

#id monitoring
HLT_list=[ 'HLT_mu6_idperf' ]
muon_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(rate=10., comment=''), 
                    }])))

HLT_list=[ 'HLT_mu4_idperf', 'HLT_mu10_idperf']
muon_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(rate=2., comment='', ESRate=0.5), 
                    }])))

#HLT_list=[ 'HLT_noalg_L1MU4', 'HLT_noalg_L1MU6', 'HLT_noalg_L1MU11',]
#muon_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
#                    400 : TriggerRule(rate=0.3, comment='', ESValue=1), 
#                    }])))

if not doHLTtransparent:
    rules.update(muon_rules)

#print muon_rules

##############################
# egamma
#############################
egamma_rules={}

# physics
L1_list=['L1_EM10', 'L1_EM12', 'L1_EM14', 'L1_EM16']
egamma_rules.update(dict(map(None,L1_list,len(L1_list)*[{
    400 : TriggerRule(PS=1, comment='L1'),
    }])))
HLT_list=[ 'HLT_e15_loose_ion', 'HLT_e15_medium_ion']
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    #0. : TriggerRule(PS=1, comment='', ESRate=0.5),
                    0.0003 : TriggerRule(PS=-1, comment=''),
                    }])))

HLT_list=[ 'HLT_e15_etcut']
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    #0. : TriggerRule(rate=0.5, comment='',rerun=1),
                    0.0003 : TriggerRule(PS=-1, comment=''),
                    }])))

HLT_list=[ 'HLT_e15_etcut_ion']
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    #0. : TriggerRule(rate=2, comment='', ESValue=1,rerun=1),
                    0.0003 : TriggerRule(PS=-1, comment=''),
                    }])))

HLT_list=[ 'HLT_e15_loose', 'HLT_e15_medium' ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    #0. : TriggerRule(PS=1, comment=''),
                    0.0003 : TriggerRule(PS=-1, comment=''),
                    }])))

#backup electron triggers for L>0.001
HLT_list=[ 'HLT_e15_loose_ion_L1EM12', 'HLT_e15_medium_ion_L1EM12']
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    0.001 : TriggerRule(PS=1, comment='', ESValue=40),
                    }])))

HLT_list=[ 'HLT_e15_etcut_L1EM12']
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    0.001 : TriggerRule(PS=90, comment='', rerun=1),
                    0.002 : TriggerRule(PS=180, comment='', rerun=1),
                    }])))

HLT_list=[ 'HLT_e15_etcut_ion_L1EM12']
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    0.001 : TriggerRule(PS=40, comment='', ESValue=1, rerun=1),
                    0.002 : TriggerRule(PS=80, comment='', ESValue=1, rerun=1),
                    }])))

HLT_list=[ 'HLT_e15_loose_L1EM12', 'HLT_e15_medium_L1EM12' ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    0.001 : TriggerRule(PS=1, comment=''),
                    }])))
#end of backup

HLT_list=[ 'HLT_e18_loose_ion',  'HLT_e18_medium_ion',
           'HLT_e20_loose_ion', 
           'HLT_g20_medium_ion' ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=1, comment=''),
                    }])))

HLT_list=[ 'HLT_g15_loose_ion' ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(rate=10, comment='', ESRate=0.2,rerun=1),
                    }])))

HLT_list=[ 'HLT_g20_loose_ion' ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=1, comment='', ESRate=0.3),
                    }])))

HLT_list=[ 'HLT_g12_etcut' ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    100 : TriggerRule(PS=-1, comment = 'HardProbes')
                    }])))

HLT_list=[ 'HLT_g12_etcut_ion' ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    100 : TriggerRule(rate=4, comment = 'express', rerun = 1.0) 
                    }])))


HLT_list=[ 'HLT_g20_etcut_ion',  'HLT_g20_etcut' ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=-1, comment=''),
                    }])))

HLT_list=[ 'HLT_e20_loose' ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=1, comment=''),
                    }])))

HLT_list=[ 'HLT_g15_medium', 'HLT_g15_medium_ion', 'HLT_g20_medium', ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=-1, comment=''),
                    }])))

HLT_list=[ 'HLT_e12_etcut_ion' ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=-1, comment=''),
                    }])))

HLT_list=[ 'HLT_e12_etcut' ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=-1, comment=''),
                    }])))

HLT_list=[ 'HLT_g18_etcut_ion' ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(rate=2., comment='', rerun=1),
                    }])))

HLT_list=[ 'HLT_g15_loose' ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(rate=0.5, comment='', ESValue=2),
                    }])))

HLT_list=[ 'HLT_g20_loose' ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(rate=3, comment='', ESRate=0.1,rerun=1),
                    }])))

HLT_list=[ 'HLT_g18_etcut' ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=-1, comment=''),
                    }])))

HLT_list=[ 'HLT_noalg_L1EM12', 'HLT_noalg_L1EM14' ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(rate=0.2, comment='', ESValue=1),
                    }])))

HLT_list=[ 'HLT_g15_etcut_ion' ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(rate=3, comment='', rerun=1, ESRate=0.3),
                    }])))

HLT_list=[ 'HLT_g15_etcut' ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=-1, comment=''),
                    }])))


#ES
HLT_list=[ 'HLT_e15_loose_ion_idperf', 'HLT_e15_medium_ion_idperf' ]
egamma_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{ 
                    400 : TriggerRule(rate=1., comment='', ESRate=0.3),
                    }])))

if not doHLTtransparent:
    rules.update(egamma_rules)

##############################
# jet
#############################
jet_rules={}

# physics
L1_list=['L1_J75']
jet_rules.update(dict(map(None,L1_list,len(L1_list)*[{
    0.009 : TriggerRule(PS=10, comment='L1'),
    0.01  : TriggerRule(PS=20, comment='L1'),
    }])))
   
L1_list=['L1_TE50', 'L1_J10']
jet_rules.update(dict(map(None,L1_list,len(L1_list)*[{
    400 : TriggerRule(PS=1, comment='L1'),
    }])))

#beam spot and idcalib
L1_list=['L1_J10_VTE200']
jet_rules.update(dict(map(None,L1_list,len(L1_list)*[{
    0.009 : TriggerRule(PS=1, comment='L1'),
    0.01  : TriggerRule(PS=2, comment='L1'),
    }])))

HLT_list=['HLT_j75_ion_2j40_ion_L1TE50',  'HLT_j100_ion_2j40_ion_L1TE50',
          'HLT_j100_ion_2j50_ion_L1TE50']
jet_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=1, comment=''),
                    }])))

HLT_list=['HLT_j100_ion_L1TE50']
jet_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=1., comment='', ESValue=10),
                    }])))

HLT_list=['HLT_j150_ion_L1TE50']
jet_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=1., comment='', ESValue=5),
                    }])))

HLT_list=['HLT_j40_ion_L1TE20']
jet_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=-1, comment=''),
                    }])))

HLT_list=['HLT_j40_ion_L1TE50']
jet_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=-1, comment=''),
                    }])))

HLT_list=['HLT_j50_ion_L1TE20']
jet_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    0.0005 : TriggerRule(PS=13, comment=''),
                    0.001 : TriggerRule(PS=27, comment=''),
                    0.005 : TriggerRule(PS=70, comment=''),
                    0.006 : TriggerRule(PS=700, comment=''),
                    }])))

HLT_list=[ 'HLT_j60_ion_L1TE50']
jet_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    0.0005 : TriggerRule(PS=8, comment=''),
                    0.001 : TriggerRule(PS=13, comment=''),
                    0.005 : TriggerRule(PS=35, comment=''),
                    0.006 : TriggerRule(PS=350, comment=''),
                    }])))

HLT_list=[ 'HLT_j50_ion_L1TE50' ]
jet_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(rate=1.5, comment='', ESValue=10),
                    }])))

HLT_list=[ 'HLT_j50_ion_L1J10' ]
jet_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(rate=2, comment='', ESRate=0.3),
                    }])))

HLT_list=[ 'HLT_j75_ion_L1J10', 'HLT_j75_ion_L1TE50', 
           'HLT_j50_320eta490_ion_L1TE50' ]
jet_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=1., comment='', rerun=1),
                    }])))

HLT_list=[ 'HLT_j40_320eta490_ion_L1TE20']
jet_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(rate=4., comment='', rerun=1),
                    }])))

#muon+jet
HLT_list=[ 'HLT_mu4_j40_ion_dr05', 'HLT_mu4_j50_ion_dr05',
           'HLT_mu4_j60_ion_dr05']
jet_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=1., comment=''),
                    }])))

HLT_list=[ 'HLT_mu6_j30_ion_dr05']
jet_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(PS=1., comment='', ESRate=0.3),
                    }])))

HLT_list=[ 'HLT_mu4_j30_a3_ion_dr05']
jet_rules.update(dict(map(None,HLT_list,len(HLT_list)*[{
                    400 : TriggerRule(rate=10., comment='', rerun=1),
                    }])))


if not doHLTtransparent:
    rules.update(jet_rules)

###############################
# hip
##############################

hip_rules={
    'HLT_g0_hiptrt_L1EM14'           : { 1 : TriggerRule(PS=1, comment='HIP') }, 
    'HLT_g0_hiptrt_L1EM16'           : { 1 : TriggerRule(PS=1, comment='HIP') }, 
    }

if not doHLTtransparent:
    rules.update(hip_rules)

##############################
beamspot_rules = {
    # online measurement: unPS
    'HLT_beamspot_trkFS_trkfast'           : { 1 : TriggerRule(PS=1, comment='BeamSpot') }, 
    'HLT_beamspot_trkFS_trkfast_L1J10_VTE200'           : { 1 : TriggerRule(PS=1, comment='BeamSpot') },
    'HLT_beamspot_trkFS_trkfast_L1TE5_VTE200'           : { 0.001  : TriggerRule(PS=-1, comment='BeamSpot'), },
    # offline measurement
    'HLT_beamspot_activeTE_trkfast_peb'              : { 1      : TriggerRule(rate=200, comment='BeamSpot') },
    'HLT_beamspot_allTE_trkfast_pebTRT'              : { 1      : TriggerRule(rate=20, comment='BeamSpot') },
    'HLT_beamspot_activeTE_trkfast_peb_L1J10_VTE200' : { 1      : TriggerRule(rate=200, comment='BeamSpot') },
    'HLT_beamspot_allTE_trkfast_pebTRT_L1J10_VTE200' : { 1      : TriggerRule(rate=20, comment='BeamSpot') },
    'HLT_beamspot_activeTE_trkfast_peb_L1TE5_VTE200' : { 0.001  : TriggerRule(PS=-1, comment='BeamSpot') },
    'HLT_beamspot_allTE_trkfast_pebTRT_L1TE5_VTE200' : { 0.001  : TriggerRule(PS=-1, comment='BeamSpot') },
    }

# ----------------------------------------
zerobias_rules = {
    # cost file is patched with online measured rate
    # 'L1_ZB'                    : { 1 : TriggerRule(PS=60, comment='PS adjusted to 476b to give 400 Hz') },
    'L1_ZB'                    : { 1 : TriggerRule(rate=400, comment='400 Hz input') },
    'HLT_noalg_zb_L1ZB'        : { 1 : TriggerRule(PS=160, comment='5 Hz target') },
    }

#----------------
RulesList=[zerobias_rules, beamspot_rules]

if forRateEstim or doHLTtransparent:
    RulesList=[]

for Rules in RulesList:
    for newrule in Rules.keys():
        if rules.has_key(newrule):
            print 'FATAL     Physics_HI_v3_rules     Duplicated rule inside calibration rule. Cannot be added:',newrule
    rules.update(Rules)

# ----------------------------------------
RulesList=[Monitoring_HI_v3_rules.physics_rules]
if forRateEstim or doHLTtransparent:
    RulesList=[]

for Rules in RulesList:
    for newrule in Rules.keys():
        if rules.has_key(newrule):
            print 'FATAL     Physics_HI_v3_rules     Duplicated rule in monitoring rule. Cannot be added from monitoring rule for:',newrule
    rules.update(Rules)

#------------------------------
topo_Rules = {
    'L1_DR-TAU20ITAU12I-J25'	                   :{ 1: TriggerRule(PS= 8125  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_LAR-EM'	                                   :{ 1: TriggerRule(PS= 43.75 , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_MU6_MJJ-200'	                           :{ 1: TriggerRule(PS= 225   , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_MU6_MJJ-300'	                           :{ 1: TriggerRule(PS= 225   , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_MU6_MJJ-400'	                           :{ 1: TriggerRule(PS= 200   , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_MU6_MJJ-500'	                           :{ 1: TriggerRule(PS= 200   , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_J30_2J20_4J20.0ETA49_MJJ-400'                  :{ 1: TriggerRule(PS= 8250  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_J30_2J20_4J20.0ETA49_MJJ-700'                  :{ 1: TriggerRule(PS= 5000  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_J30_2J20_4J20.0ETA49_MJJ-800'                  :{ 1: TriggerRule(PS= 4875  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_J30_2J20_4J20.0ETA49_MJJ-900'                  :{ 1: TriggerRule(PS= 4000  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_3J20_4J20.0ETA49_MJJ-400'	                   :{ 1: TriggerRule(PS= 8250  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_3J20_4J20.0ETA49_MJJ-700'	                   :{ 1: TriggerRule(PS= 5000  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_3J20_4J20.0ETA49_MJJ-800'	                   :{ 1: TriggerRule(PS= 4500  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_3J20_4J20.0ETA49_MJJ-900'	                   :{ 1: TriggerRule(PS= 4125  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_XE35_MJJ-200'	                           :{ 1: TriggerRule(PS= 5750  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_BTAG-MU4J15'	                           :{ 1: TriggerRule(PS= 8500  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_BTAG-MU4J30'	                           :{ 1: TriggerRule(PS= 8500  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_BTAG-MU6J20'	                           :{ 1: TriggerRule(PS= 8500  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_BTAG-MU6J25'	                           :{ 1: TriggerRule(PS= 8500  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_BTAG-MU4J20'	                           :{ 1: TriggerRule(PS= 8500  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_3J15_BTAG-MU4J15'	                           :{ 1: TriggerRule(PS= 8500  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_3J15_BTAG-MU4J30'	                           :{ 1: TriggerRule(PS= 8500  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_3J15_BTAG-MU6J25'	                           :{ 1: TriggerRule(PS= 8500  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_3J20_BTAG-MU4J20'	                           :{ 1: TriggerRule(PS= 8500  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_J40_DPHI-J20XE50'	                           :{ 1: TriggerRule(PS= 8000  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_J40_DPHI-CJ20XE50'	                           :{ 1: TriggerRule(PS= 8000  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_HT190-J15s5.ETA21'	                           :{ 1: TriggerRule(PS= 112.5 , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_HT150-J20s5.ETA31'	                           :{ 1: TriggerRule(PS= 112.5 , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_JPSI-1M5'	                                   :{ 1: TriggerRule(PS= 1750  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_JPSI-1M5-EM7'	                           :{ 1: TriggerRule(PS= 1750  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_JPSI-1M5-EM12'	                           :{ 1: TriggerRule(PS= 412.5 , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_W-05DPHI-JXE-0'	                           :{ 1: TriggerRule(PS= 6500  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_W-10DPHI-JXE-0'	                           :{ 1: TriggerRule(PS= 3125  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_W-15DPHI-JXE-0'	                           :{ 1: TriggerRule(PS= 1187.5, comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_W-10DPHI-EMXE-0'	                           :{ 1: TriggerRule(PS= 1500  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_W-15DPHI-EMXE-0'	                           :{ 1: TriggerRule(PS= 1187.5, comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_W-05DPHI-EMXE-1'	                           :{ 1: TriggerRule(PS= 375   , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_EM10_W-MT25'	                           :{ 1: TriggerRule(PS= 1125  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_EM10_W-MT30'	                           :{ 1: TriggerRule(PS= 750   , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_EM15_W-MT35'	                           :{ 1: TriggerRule(PS= 112.5 , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS30' :{ 1: TriggerRule(PS= 25    , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20' :{ 1: TriggerRule(PS= 25    , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE'      :{ 1: TriggerRule(PS= 62.5  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_XS60' :{ 1: TriggerRule(PS= 25    , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_XS30' :{ 1: TriggerRule(PS= 25    , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE'	   :{ 1: TriggerRule(PS= 68.75 , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_DPHI-J20XE50'	                           :{ 1: TriggerRule(PS= 8500  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_DPHI-CJ20XE50'	                           :{ 1: TriggerRule(PS= 8500  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_MJJ-900'	                                   :{ 1: TriggerRule(PS= 6250  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_MJJ-800'	                                   :{ 1: TriggerRule(PS= 6375  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_MJJ-700'	                                   :{ 1: TriggerRule(PS= 7000  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_MJJ-400'	                                   :{ 1: TriggerRule(PS= 10375 , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_MJJ-300'	                                   :{ 1: TriggerRule(PS= 10750 , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_MJJ-200'	                                   :{ 1: TriggerRule(PS= 11250 , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_MJJ-100'	                                   :{ 1: TriggerRule(PS= 11875 , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_DETA-JJ'	                                   :{ 1: TriggerRule(PS= 375   , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_J4-MATCH'	                                   :{ 1: TriggerRule(PS= 7250  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_TAU12I-J25'	                                   :{ 1: TriggerRule(PS= 8250  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_EM15-TAU40'	                                   :{ 1: TriggerRule(PS= 537.5 , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_EM15-TAU12I'	                           :{ 1: TriggerRule(PS= 537.5 , comment='L1Topo commissioning, max rate 0.8 Hz')},	        
    'L1_LAR-J'	                                   :{ 1: TriggerRule(PS= 1.875 , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_J40_DPHI-Js2XE50'	                           :{ 1: TriggerRule(PS= 1.25  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_J40_DPHI-J20s2XE50'	                           :{ 1: TriggerRule(PS= 1.25  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_DPHI-Js2XE50'	                           :{ 1: TriggerRule(PS= 8500  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_DPHI-J20s2XE50'	                           :{ 1: TriggerRule(PS= 8500  , comment='L1Topo commissioning, max rate 0.8 Hz')},
    'L1_LLP-NOMATCH'	                           :{ 1: TriggerRule(PS= 3.75  , comment='L1Topo commissioning, max rate 0.8 Hz')},

    'HLT_noalg_L1Topo' : { 1 : TriggerRule(PS=1, comment='L1Topo for testing')},

#    'L1_LAR-EM':         { 1 : TriggerRule(PS=2*NumBunches*0.09, comment='LAREM for testing, request max rate 0.5 Hz')},
#    'L1_MJJ-100':        { 1 : TriggerRule(PS=2*NumBunches*182  , comment='L1Topo commissioning, max rate 0.5 Hz')}  ,#  18.4 kHz
#    'L1_MJJ-200':        { 1 : TriggerRule(PS=2*NumBunches*91   , comment='L1Topo commissioning, max rate 0.5 Hz')}   ,# 7.7 kHz
#    'L1_MJJ-300':        { 1 : TriggerRule(PS=2*NumBunches*60.6 , comment='L1Topo commissioning, max rate 0.5 Hz')}   ,# 4.2 kHz
#    'L1_MJJ-400':        { 1 : TriggerRule(PS=2*NumBunches*51.5 , comment='L1Topo commissioning, max rate 0.5 Hz')}   ,# 2.6 kHz
#    'L1_MJJ-700':        { 1 : TriggerRule(PS=2*NumBunches*15.2 , comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   974 Hz
#    'L1_MJJ-800':        { 1 : TriggerRule(PS=2*NumBunches*12.1 , comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   681 Hz
#    'L1_MJJ-900':        { 1 : TriggerRule(PS=2*NumBunches*12.1 , comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   535 Hz
#    'L1_JPSI-1M5':       { 1 : TriggerRule(PS=2*NumBunches*24    , comment='L1Topo commissioning, max rate 0.5 Hz')},#    924 kHz
#    'L1_JPSI-1M5-EM7':   { 1 : TriggerRule(PS=2*NumBunches*30    , comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   93 kHz
#    'L1_JPSI-1M5-EM12':  { 1 : TriggerRule(PS=2*NumBunches*22 , comment='L1Topo commissioning, max rate 0.5 Hz')}  ,#   34 kHz
#    'L1_W-10DPHI-EMXE-0':{ 1 : TriggerRule(PS=2*NumBunches*2.42 , comment='L1Topo commissioning, max rate 0.5 Hz')}  ,#  94 kHz
#    'L1_W-15DPHI-EMXE-0':{ 1 : TriggerRule(PS=2*NumBunches*1.51 , comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   121 kHz
#    'L1_EM10_W-MT25'	:{ 1 : TriggerRule(PS=2*NumBunches*1.82 , comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#6.20E+04
#    'L1_EM10_W-MT30'    :{ 1 : TriggerRule(PS=2*NumBunches*1.2  , comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#4.70E+04
#    'L1_EM15_W-MT35' 	:{ 1 : TriggerRule(PS=2*NumBunches*0.303, comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#18792	   
#    'L1_W-05RO-XEEMHT'	:{ 1 : TriggerRule(PS=2*NumBunches*3.03, comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#130568	
#    'L1_W-05DPHI-JXE-0'	:{ 1 : TriggerRule(PS=2*NumBunches*3600,   comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#66144	
#    'L1_W-10DPHI-JXE-0'	:{ 1 : TriggerRule(PS=2*NumBunches*3.64,   comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#43828	
#    'L1_W-15DPHI-JXE-0'	:{ 1 : TriggerRule(PS=2*NumBunches*1.818 , comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#29336	
#    'L1_W-05RO-XEHT-0'	:{ 1 : TriggerRule(PS=2*NumBunches*3.03,  comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#79477	
#    'L1_W-08RO-XEHT-0'	:{ 1 : TriggerRule(PS=2*NumBunches*0.5,    comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#40830	
#    'L1_W-90RO2-XEHT-0'	:{ 1 : TriggerRule(PS=2*NumBunches*3.03,  comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#61779	
#    'L1_W-250RO2-XEHT-0':{ 1 : TriggerRule(PS=2*NumBunches*1,      comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#16873	
#    'L1_DPHI-CJ20XE50'	:{ 1 : TriggerRule(PS=2*NumBunches*0.9091, comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#10.8	
#    'L1_DPHI-J20s2XE50'	:{ 1 : TriggerRule(PS=2*NumBunches*0.9091, comment="L1Topo commissioning, max rate 0.5 Hz")}  ,#6.5	
#    'L1_DPHI-Js2XE50':   { 1 : TriggerRule(PS=2*NumBunches*0.9091, comment='L1Topo commissioning, max rate 0.5 Hz')} ,#   94 Hz
      

}

#----------------
RulesList=[topo_Rules]

if not EnableL1Topo:
    RulesList=[]
    
for Rules in RulesList:
    for newrule in Rules.keys():
        if rules.has_key(newrule):
            print 'FATAL     Physics_HI_v3_rules     Duplicated rule inside calibration rule. Cannot be added:',newrule        
        if Rules[newrule][1]['PS']< 1:
            print "WARNING     Rules changed for: " , newrule, " PS from ",Rules[newrule][1]['PS'], " to 1"
            Rules[newrule][1]['PS']=1
    rules.update(Rules)
#------------------------------


EBrun=False

disable_rules = {
                                        'HLT_2e10_loose_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'express', rerun = 0.0) }, 
                                            'HLT_2g15_loose' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_2g15_loose_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_2j10_ion_L1ZDC_A_C_TE5_VTE200' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
                         'HLT_2j10_ion_L1ZDC_XOR_TE5_VTE200' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
'HLT_2j10_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_TE5_VTE200' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
     'HLT_2j10_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_VTE50' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
                         'HLT_2j15_ion_L1ZDC_A_C_TE5_VTE200' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
                         'HLT_2j15_ion_L1ZDC_XOR_TE5_VTE200' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
'HLT_2j15_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_TE5_VTE200' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
     'HLT_2j15_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_VTE50' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
                                        'HLT_2j20_ion_L1ZDC_A_C_TE5_VTE200' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
                         'HLT_2j20_ion_L1ZDC_XOR_TE5_VTE200' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
'HLT_2j20_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_TE5_VTE200' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
     'HLT_2j20_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_VTE50' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
                                        'HLT_2mu4_L1MU0' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_2mu4_msonly' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                     'HLT_2mu4_msonly_L1MU0' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_2mu6_L1MU0' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                           'HLT_2mu6_msonly' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                     'HLT_2mu6_msonly_L1MU0' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_e10_etcut' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_e12_etcut' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                           'HLT_e15_lhloose' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                       'HLT_e15_lhloose_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                          'HLT_e15_lhmedium' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                      'HLT_e15_lhmedium_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                             'HLT_e15_loose' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                            'HLT_e15_medium' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                       'HLT_e18_lhloose_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                      'HLT_e18_lhmedium_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                             'HLT_e20_etcut' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                         'HLT_e20_etcut_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                           'HLT_e20_lhloose' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                       'HLT_e20_lhloose_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_hi_gg_upc_L1All' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
                                        'HLT_hi_gg_upc_L1ZDC_A_C_VTE50' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
                                    'HLT_hi_loose_upc_L1All' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
                                        'HLT_hi_medium_upc_L1ZDC_A_C_VTE50' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
                                     'HLT_hi_perf_ucc_L1All' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                 'HLT_hi_perf_ucc_L1TE10000' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                 'HLT_hi_perf_ucc_L1ZDC_A_C' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                  'HLT_hi_perfzdc_ucc_L1All' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                            'HLT_hi_th1_zdcpu_ucc_L1TE12000' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                            'HLT_hi_th1_zdcpu_ucc_L1TE14000' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                            'HLT_hi_th2_zdcpu_ucc_L1TE10000' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                            'HLT_hi_th2_zdcpu_ucc_L1TE12000' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                            'HLT_hi_th2_zdcpu_ucc_L1TE14000' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                            'HLT_hi_th3_zdcpu_ucc_L1TE12000' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                            'HLT_hi_th3_zdcpu_ucc_L1TE14000' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                          'HLT_hi_tight_upc_L1ZDC_A_C_VTE50' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
                                        'HLT_id_cosmicid_L1MU11_EMPTY' : { 100 : TriggerRule(PS=-1.0, comment = 'express', rerun = 0.0) }, 
                                        'HLT_id_cosmicid_L1MU4_EMPTY' : { 100 : TriggerRule(PS=-1.0, comment = 'express', rerun = 0.0) }, 
                                 'HLT_idcalib_trk16_central' : { 100 : TriggerRule(PS=-1.0, comment = 'IDTracks', rerun = 0.0) }, 
                                     'HLT_idcalib_trk16_fwd' : { 100 : TriggerRule(PS=-1.0, comment = 'IDTracks', rerun = 0.0) }, 
                                 'HLT_idcalib_trk29_central' : { 100 : TriggerRule(PS=-1.0, comment = 'IDTracks', rerun = 0.0) }, 
                                     'HLT_idcalib_trk29_fwd' : { 100 : TriggerRule(PS=-1.0, comment = 'IDTracks', rerun = 0.0) }, 
                                  'HLT_idcalib_trk9_central' : { 100 : TriggerRule(PS=-1.0, comment = 'IDTracks', rerun = 0.0) }, 
                                      'HLT_idcalib_trk9_fwd' : { 100 : TriggerRule(PS=-1.0, comment = 'IDTracks', rerun = 0.0) }, 
                                        'HLT_j0_perf_L1RD0_FILLED' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                     'HLT_j0_perf_L1ZDC_A_C' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                     'HLT_j100_a3_ion_L1J10' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                    'HLT_j100_a3_ion_L1TE50' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                               'HLT_j100_ion_2j30_ion_L1J10' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                              'HLT_j100_ion_2j30_ion_L1TE50' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                               'HLT_j100_ion_2j40_ion_L1J10' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                               'HLT_j100_ion_2j50_ion_L1J10' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_j100_ion_L1J10' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
 'HLT_j10_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_TE5_VTE200' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
      'HLT_j10_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_VTE50' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
                                        'HLT_j150_a3_ion_L1J10' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                    'HLT_j150_a3_ion_L1TE50' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_j150_ion_L1J10' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_j15_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_TE5_VTE200' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
      'HLT_j15_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_VTE50' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
                                      'HLT_j20_a3_ion_L1TE20' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_j20_ion_L1TE20' : { 100 : TriggerRule(PS=-1.0, comment = 'express', rerun = 0.0) }, 
 'HLT_j20_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_TE5_VTE200' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
      'HLT_j20_ion_mb_mbts_vetombts1side2in_L1ZDC_A_C_VTE50' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
                                        'HLT_j25_320eta490_ion_L1TE20' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_j25_a3_320eta490_ion_L1TE20' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                           'HLT_j25_a3_320eta490_ion_L1TE50' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                     'HLT_j25_a3_ion_L1TE20' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_j25_ion_L1TE20' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                              'HLT_j30_320eta490_ion_L1TE20' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                              'HLT_j30_a3_320eta490_ion_L1TE20' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                           'HLT_j30_a3_320eta490_ion_L1TE50' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                     'HLT_j30_a3_ion_L1TE20' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                     'HLT_j30_a3_ion_L1TE50' : { 100 : TriggerRule(PS=-1.0, comment = 'express', rerun = 0.0) }, 
                                  'HLT_j30_a3_ion_L1ZDC_A_C' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_j30_ion_L1TE20' : { 100 : TriggerRule(PS=-1.0, comment = 'express', rerun = 0.0) }, 
                                        'HLT_j30_ion_L1ZDC_A_C' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_j40_a3_320eta490_ion_L1TE20' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                           'HLT_j40_a3_320eta490_ion_L1TE50' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                     'HLT_j40_a3_ion_L1TE20' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                     'HLT_j40_a3_ion_L1TE50' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                  'HLT_j40_a3_ion_L1ZDC_A_C' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_j40_ion_L1ZDC_A_C' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                              'HLT_j50_320eta490_ion_L1TE20' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_j50_a3_320eta490_ion_L1TE20' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                           'HLT_j50_a3_320eta490_ion_L1TE50' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                      'HLT_j50_a3_ion_L1J10' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                     'HLT_j50_a3_ion_L1TE20' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                     'HLT_j50_a3_ion_L1TE50' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                  'HLT_j50_a3_ion_L1ZDC_A_C' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_j50_ion_L1ZDC_A_C' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                      'HLT_j60_a3_ion_L1J10' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                     'HLT_j60_a3_ion_L1TE50' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                  'HLT_j60_a3_ion_L1ZDC_A_C' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                'HLT_j60_ion_2j30_ion_L1J10' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                               'HLT_j60_ion_2j30_ion_L1TE50' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                         'HLT_j60_ion_L1J10' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_j60_ion_L1ZDC_A_C' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                      'HLT_j75_a3_ion_L1J10' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                     'HLT_j75_a3_ion_L1TE50' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                  'HLT_j75_a3_ion_L1ZDC_A_C' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                'HLT_j75_ion_2j30_ion_L1J10' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                               'HLT_j75_ion_2j30_ion_L1TE50' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                'HLT_j75_ion_2j40_ion_L1J10' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_j75_ion_L1ZDC_A_C' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                               'HLT_larhec_L1EM7_FIRSTEMPTY' : { 100 : TriggerRule(PS=-1.0, comment = 'LArCellsEmpty', rerun = 0.0) }, 
                               'HLT_larhec_L1J12_FIRSTEMPTY' : { 100 : TriggerRule(PS=-1.0, comment = 'LArCellsEmpty', rerun = 0.0) }, 
                               'HLT_larhec_L1J30_FIRSTEMPTY' : { 100 : TriggerRule(PS=-1.0, comment = 'LArCellsEmpty', rerun = 0.0) }, 
                             'HLT_larhec_L1TAU12_FIRSTEMPTY' : { 100 : TriggerRule(PS=-1.0, comment = 'LArCellsEmpty', rerun = 0.0) }, 
                                   'HLT_lumipeb_vdm_L1MBTS_1_1' : { 100 : TriggerRule(PS=-1.0, comment = 'VdM', rerun = 0.0) }, 
                         'HLT_lumipeb_vdm_L1MBTS_1_1_BGRP11' : { 100 : TriggerRule(PS=-1.0, comment = 'VdM', rerun = 0.0) }, 
                          'HLT_lumipeb_vdm_L1MBTS_1_1_BGRP9' : { 100 : TriggerRule(PS=-1.0, comment = 'VdM', rerun = 0.0) }, 
                   'HLT_lumipeb_vdm_L1MBTS_1_1_UNPAIRED_ISO' : { 100 : TriggerRule(PS=-1.0, comment = 'VdM', rerun = 0.0) }, 
                                  'HLT_lumipeb_vdm_L1MBTS_2' : { 100 : TriggerRule(PS=-1.0, comment = 'VdM', rerun = 0.0) }, 
                           'HLT_lumipeb_vdm_L1MBTS_2_BGRP11' : { 100 : TriggerRule(PS=-1.0, comment = 'VdM', rerun = 0.0) }, 
                            'HLT_lumipeb_vdm_L1MBTS_2_BGRP9' : { 100 : TriggerRule(PS=-1.0, comment = 'VdM', rerun = 0.0) }, 
                     'HLT_lumipeb_vdm_L1MBTS_2_UNPAIRED_ISO' : { 100 : TriggerRule(PS=-1.0, comment = 'VdM', rerun = 0.0) }, 
                               'HLT_lumipeb_vdm_L1RD0_BGRP9' : { 100 : TriggerRule(PS=-1.0, comment = 'VdM', rerun = 0.0) }, 
                                 'HLT_lumipeb_vdm_L1ZDC_A_C' : { 100 : TriggerRule(PS=-1.0, comment = 'VdM', rerun = 0.0) }, 
                          'HLT_lumipeb_vdm_L1ZDC_A_C_BGRP11' : { 100 : TriggerRule(PS=-1.0, comment = 'VdM', rerun = 0.0) }, 
                           'HLT_lumipeb_vdm_L1ZDC_A_C_BGRP9' : { 100 : TriggerRule(PS=-1.0, comment = 'VdM', rerun = 0.0) }, 
                    'HLT_lumipeb_vdm_L1ZDC_A_C_UNPAIRED_ISO' : { 100 : TriggerRule(PS=-1.0, comment = 'VdM', rerun = 0.0) }, 
                                   'HLT_mb_hg_zdcperf_L1All' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                               'HLT_mb_hg_zdcperf_L1ZDC_A_C' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                'HLT_mb_idperf_ion_L1MBTS_1' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                   'HLT_mb_lg_zdcperf_L1All' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                               'HLT_mb_lg_zdcperf_L1ZDC_A_C' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                        'HLT_mb_mbts_L1MBTS_2' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                    'HLT_mb_mbts_L1MBTS_2_2' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                    'HLT_mb_mbts_L1MBTS_3_3' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                    'HLT_mb_mbts_L1MBTS_4_4' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
        'HLT_mb_sptrk_vetombts1side2in_L1ZDC_XOR_TE5_VTE200' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
        'HLT_mb_sptrk_vetombts1side2in_L1ZDC_XOR_TRT_VTE200' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
                                        'HLT_mu4_L1MU0' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_mu4_idperf_L1MU0' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_mu4_j20_a3_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                   'HLT_mu4_j20_a3_ion_dr05' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                           'HLT_mu4_j20_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                      'HLT_mu4_j20_ion_dr05' : { 100 : TriggerRule(PS=-1.0, comment = 'express', rerun = 0.0) }, 
                                        'HLT_mu4_j25_a3_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                   'HLT_mu4_j25_a3_ion_dr05' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                           'HLT_mu4_j25_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                      'HLT_mu4_j25_ion_dr05' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_mu4_j30_a3_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_mu4_j30_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                      'HLT_mu4_j30_ion_dr05' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_mu4_j40_a3_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                   'HLT_mu4_j40_a3_ion_dr05' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_mu4_j50_a3_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                   'HLT_mu4_j50_a3_ion_dr05' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                           'HLT_mu4_j50_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_mu4_j60_a3_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                   'HLT_mu4_j60_a3_ion_dr05' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                           'HLT_mu4_j60_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                       'HLT_mu6_L1MU0' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_mu6_idperf_L1MU0' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_mu6_j20_a3_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                   'HLT_mu6_j20_a3_ion_dr05' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_mu6_j20_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_mu6_j20_ion_dr05' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_mu6_j25_a3_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                   'HLT_mu6_j25_a3_ion_dr05' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                           'HLT_mu6_j25_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                      'HLT_mu6_j25_ion_dr05' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_mu6_j30_a3_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                   'HLT_mu6_j30_a3_ion_dr05' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                           'HLT_mu6_j30_ion' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_mu6noL1' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_noalg_L1Calo' : { 100 : TriggerRule(PS=-1.0, comment = 'L1Calo', rerun = 0.0) }, 
                                        'HLT_noalg_L1EM10' : { 100 : TriggerRule(PS=-1.0, comment = 'express', rerun = 0.0) }, 
                                        'HLT_noalg_L1EM16' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_noalg_L1EM18' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_noalg_L1EM20' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_noalg_L1EM7' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_noalg_L1EM8' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_noalg_L1J10' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_noalg_L1J10_VTE100' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_noalg_L1J10_VTE200' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_noalg_L1J15' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_noalg_L1J175' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_noalg_L1J20' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_noalg_L1J30' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_noalg_L1J35' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_noalg_L1J50' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_noalg_L1J75' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_noalg_L1MU15' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_noalg_L1MU20' : { 100 : TriggerRule(PS=-1.0, comment = 'HardProbes', rerun = 0.0) }, 
                                        'HLT_noalg_L1Standby' : { 100 : TriggerRule(PS=-1.0, comment = 'Standby', rerun = 0.0) }, 
                                        'HLT_noalg_L1ZDC_XOR_TRT_VTE200' : { 100 : TriggerRule(PS=-1.0, comment = 'UPC', rerun = 0.0) }, 
                                        'HLT_noalg_mb_L1LUCID' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                        'HLT_noalg_mb_L1LUCID_EMPTY' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                        'HLT_noalg_mb_L1LUCID_UNPAIRED_ISO' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                        'HLT_larnoiseburst_loose_rerun' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                     'HLT_noalg_mb_L1MBTS_2' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                   'HLT_noalg_mb_L1MBTS_2_2' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                  'HLT_noalg_mb_L1RD0_EMPTY' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                 'HLT_noalg_mb_L1RD0_FILLED' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                           'HLT_noalg_mb_L1RD0_UNPAIRED_ISO' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                      'HLT_noalg_mb_L1TE100' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                    'HLT_noalg_mb_L1TE10000' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                    'HLT_noalg_mb_L1TE12000' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                    'HLT_noalg_mb_L1TE14000' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                              'HLT_noalg_mb_L1TE1500.0ETA49' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                       'HLT_noalg_mb_L1TE20' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                      'HLT_noalg_mb_L1TE200' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                              'HLT_noalg_mb_L1TE3000.0ETA49' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                      'HLT_noalg_mb_L1TE3000.0ETA49_OVERLAY' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBiasOverlay', rerun = 0.0) }, 
                              'HLT_noalg_mb_L1TE3500.0ETA49' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                      'HLT_noalg_mb_L1TE3500.0ETA49_OVERLAY' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBiasOverlay', rerun = 0.0) }, 
                                        'HLT_noalg_mb_L1TE500.0ETA49' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                       'HLT_noalg_mb_L1TE500.0ETA49_OVERLAY' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBiasOverlay', rerun = 0.0) }, 
                              'HLT_noalg_mb_L1TE5000.0ETA49' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                      'HLT_noalg_mb_L1TE5000.0ETA49_OVERLAY' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBiasOverlay', rerun = 0.0) }, 
                              'HLT_noalg_mb_L1TE6500.0ETA49' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                              'HLT_noalg_mb_L1TE8000.0ETA49' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                      'HLT_noalg_mb_L1TE8000.0ETA49_OVERLAY' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBiasOverlay', rerun = 0.0) }, 
                              'HLT_noalg_mb_L1TE9000.0ETA49' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                      'HLT_noalg_mb_L1TE9000.0ETA49_OVERLAY' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBiasOverlay', rerun = 0.0) }, 
                                      'HLT_noalg_mb_L1ZDC_A' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                                        'HLT_noalg_mb_L1ZDC_C' : { 100 : TriggerRule(PS=-1.0, comment = 'MinBias', rerun = 0.0) }, 
                             'HLT_noalg_standby_L1RD0_EMPTY' : { 100 : TriggerRule(PS=-1.0, comment = 'Standby', rerun = 0.0) }, 
                            'HLT_noalg_standby_L1RD0_FILLED' : { 100 : TriggerRule(PS=-1.0, comment = 'Standby', rerun = 0.0) }, 
        }

rules.update(disable_rules)
