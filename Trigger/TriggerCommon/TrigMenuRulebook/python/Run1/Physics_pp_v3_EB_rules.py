# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'Physics_pp_v3'

rules = {
    
    #######################################################################
    # muon calibration
    'L2_mu10_cal':      { 500: TriggerRule(PS=1, comment='muon calibration;does not accept events in normal streams',),
                          2000: TriggerRule(PS=-1.0),},
    'L2_mu10_cal_medium':{ 3000: TriggerRule(PS=1, comment='muon calibration;does not accept events in normal streams',),},
    'L2_mu4_cal_empty':      { 500: TriggerRule(PS=-1, comment='muon calibration;does not accept events in normal streams',),},
    
    
    #--- Primary Photons (PSed)
    #'EF_g20_loose':              {1000: TriggerRule(rate=2.0, rerun=True,ESRate=0.4, comment='Baseline prescaled primary; Direct photons, support for 2g20')},
    #'EF_g40_loose':              {1000: TriggerRule(rate=2.0, rerun=True,comment='Baseline prescaled primary; Direct photons')},
    #'EF_g60_loose':              {1000: TriggerRule(rate=2.0,comment='Baseline prescaled Primary; Direct photons')},
    #'EF_g80_loose':            {3000: TriggerRule(PS=1.0,comment='Baseline primary; Direct photons')},

    #--- jet chains 
    #'EF_fj30_a4tc_EFFS':{500:TriggerRule(comment='QCD jet physics and jet/calorimeter monitoring',rate=1.0,PTRate=0.2,ESRate=0.4,maxRate=1.0)},
    #'EF_j55_a4tc_EFFS':{500:TriggerRule(comment='QCD jet physics',rate=0.5,maxRate=0.5)},
    #'EF_fj55_a4tc_EFFS':{1000:TriggerRule(comment='QCD jet physics',rate=0.5,maxRate=0.5)},
    #'EF_j100_a4tc_EFFS':{500:TriggerRule(comment='QCD jet physics',rate=0.5,PTRate=0.1,maxRate=0.5)},


    
 
    #######################################################################
    # Jets
    
    'L2_rd0_filled_NoAlg':{  #2:TriggerRule(comment='input to low pt jets',rate=600,maxRate=600.),
    #610: TriggerRule(comment='input to low pt jets',rate=300.,maxRate=300.),
    1000: TriggerRule(comment='RateControl',rate=10.,maxRate=10.),
    },


    ############################################################
    # minbias
    
    #'EF_mbLucid_eff':      {100: TriggerRule(rate=0.1,comment="TBC"),},# 500: TriggerRule(PS=-1.0,)},
    #'EF_mbZdc_eff':        {100: TriggerRule(rate=0.1,comment="standard Minbias analysis")},
    
    'L1_RD0_FILLED':       {100: TriggerRule(rate=2000, scaling="bunches")},  
    'L1_RD1_FILLED':       {100: TriggerRule(rate=100, scaling="bunches")},
    
    'EF_rd0_filled_NoAlg':  {100: TriggerRule(rate=11,ESRate=10.0,comment="standard Minbias analysis"), 
                             1000: TriggerRule(rate=11,ESRate=10.0,comment="standard Minbias analysis")},
    
    'L1_BCM_Wide_BGRP0': {100: TriggerRule(PS=600000)}, 
    #'EF_mb_BCM_Wide_eff': {100: TriggerRule(PS=1500)}, 
  
    'L1_RD0_FIRSTEMPTY': {100: TriggerRule(rate=1)},
    'L1_RD0_UNPAIRED_ISO': {100: TriggerRule(rate=1)},
    #'L1_RD0_UNPAIRED_ISO': {100: TriggerRule(PS=60000)},
    #'EF_mbRd0_eff_unpaired_iso': {100: TriggerRule(PS=10)},

  
    ############################################################
    # Cosmic, detector, etc
  
    #L1 items
    'L1_RD0_EMPTY' : { 100 : TriggerRule(rate=1,scaling="bunches",comment='cosmic'), },
    
    #'L1_TRT' : {100: TriggerRule(PS=-1, comment='primary for cosmic with TRT Fast Or') },
    #HLT
    'L2_PixelBeam' : {100: TriggerRule(PS=-1, comment='luminosity calibration. Disabled 12Sep2011.')    },
    'L2_PixelNoise' : {100: TriggerRule(rate=10, comment='calibration for InDet')    },
    'L2_SCTNoise' : {100: TriggerRule(rate=10, comment='calibration for InDet')    },
    'EF_rd0_empty_NoAlg' : {100: TriggerRule(rate=1,ESRate=0.5, comment='streamer for L1_RD0_EMPTY')    },
    #'EF_L1TRT_NoAlg' : {100: TriggerRule(PS=-1, comment='streamer for L1_TRT')    },
    
    #  'EF_PreS_L1EM3_empty' : {100: TriggerRule(PS=-1, comment='rejection of pre-sampler noise in LAr, during cosmic') },
    #  'EF_PreS_L1EM5_empty' : {100: TriggerRule(PS=10, comment='rejection of pre-sampler noise in LAr') },
    #  'EF_PreS_L1J10_empty' : {100: TriggerRule(PS=10, comment='rejection of pre-sampler noise in LAr') },
    #  'EF_PreS_L1J30_empty' : {100: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr, backup for L1J10') },
    #  'EF_PreS_L1TAU8_empty' : {100: TriggerRule(PS=90, comment='rejection of pre-sampler noise in LAr') },
    #  'EF_PreS_L1J10_firstempty' : {100: TriggerRule(PS=50, ESValue=10, comment='rejection of pre-sampler noise in LAr') },
    'EF_PreS_L1J30_firstempty' : {100: TriggerRule(PS=10, ESValue=0, comment='rejection of pre-sampler noise in LAr, physics chain') },
    'EF_mu4_cosmic' : {100: TriggerRule(PS=-1, comment='cosmic chain for muons')    },
    'EF_mu4_MSonly_cosmic' : {100: TriggerRule(PS=-1,  comment='cosmic chain for muons') },
    'EF_mu4_L1MU11_cosmic' : {100: TriggerRule(PS=1, comment='cosmic chain for muons')    }, #to match online
    #rate=0.1, comment='cosmic chain for muons')    },
    #'EF_mu4_L1MU11_MSonly_cosmic' : {100: TriggerRule(rate=0.1, comment='cosmic chain for muons')    },
    'EF_mu4_L1MU11_MSonly_cosmic' : {100: TriggerRule(PS=-1, comment='cosmic chain for muons')    },
    
    'EF_mu11_empty_NoAlg': { 100 : TriggerRule(PS=1000000, comment='cosmic'), },
    
    'EF_TileCalib_laser' : {100: TriggerRule(PS=1, comment='Tile calibration')    },


    ### LAr Menu (savannah #82252)
    #  -------------------------- 
    #In physics_CosmicCalo:
    'L1_EM3_EMPTY'  : { 1000: TriggerRule(PS=5000, comment='Efficiently disabled during collisions') } ,
    
    'L1_EM5_EMPTY' : { 1300: TriggerRule(PS=10, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0 - FIX') } ,
    'L1_TAU8_EMPTY' : { 1300: TriggerRule(PS=10, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') } ,
    'L1_J10_EMPTY' : { 1300: TriggerRule(PS=10, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') } ,
    'L1_FJ10_EMPTY' : { 1300: TriggerRule(PS=10, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') } ,
    'L1_J30_EMPTY' : { 1300: TriggerRule(PS=10, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') } ,
    
    
    'L2_g10_NoCut_cosmic' : { 1300: TriggerRule(PS=120, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') },
    'L2_PreS_L1EM5_empty' : { 1300: TriggerRule(PS=25, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') } ,
    'L2_tauNoCut_cosmic' : { 1300: TriggerRule(PS=17, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') } ,
    'L2_PreS_L1TAU8_empty' : { 1300: TriggerRule(PS=21, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') },
    #'L2_L1J10_empty_NoAlg' : { 1300: TriggerRule(PS=10, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') } ,
    'L2_PreS_L1J10_empty' : { 1300: TriggerRule(PS=12, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') },
#    'L2_L1FJ10_empty_NoAlg' : { 1300: TriggerRule(PS=1, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') }, 
    'L2_L1J30_empty_NoAlg' : { 1300: TriggerRule(PS=14, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') } ,
    'L2_PreS_L1J30_empty' : { 1300: TriggerRule(PS=14, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') },
    
    #-------------------------- 
    #In calibration_LArCellsEmpty:
    #'L2_em3_empty_larcalib'  : { 1000: TriggerRule(PS=-1, comment='Disabled during collisions') } ,
    #'L2_em5_empty_larcalib' : { 1300: TriggerRule(PS=1, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') }, 
    #'L2_tau8_empty_larcalib' : { 1300: TriggerRule(PS=1, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') }, 
    #'L2_j10_empty_larcalib' : { 1300: TriggerRule(PS=1, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') } ,
    #'L2_fj10_empty_larcalib' : { 1300: TriggerRule(PS=1, comment='LArNoise-Menu : stable-beam conditions, 1318b scaled by 1.0') },
    
    #-------------------------- 
    #Disabled during stable beams: 
    ##  'L2_g5_NoCut_cosmic' : { 1000: TriggerRule(PS=-1, comment='Disabled during collisions') } ,
    #  'EF_g5_NoCut_cosmic'  : { 1000: TriggerRule(PS=-1, comment='Disabled during collisions') } ,
    ##  'L2_PreS_L1EM3_empty' : {1000: TriggerRule(PS=-1, comment='Disabled during collisions') } ,
    #  'EF_PreS_L1EM3_empty' : { 1000: TriggerRule(PS=-1, comment='Disabled during collisions') } ,
    #-------------------------- 
    
    #All EF-equivalent should therefore have a PS=1, since they are protected by L2: 
    'EF_g10_NoCut_cosmic' : { 1000: TriggerRule(PS=1, comment='cosmic chain for egamma') } ,
    'EF_tauNoCut_cosmic'  : { 1000: TriggerRule(PS=1, comment='cosmic chain for taus') } ,
    'EF_PreS_L1EM5_empty' : { 1000: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr') } ,
    'EF_PreS_L1J10_empty' : { 1000: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr') } ,
    'EF_PreS_L1J30_empty' : { 1000: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr, backup for L1_J10') } ,
    'EF_PreS_L1TAU8_empty'  : { 1000: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr') } ,
    'EF_L1FJ10_empty_NoAlg' : { 1000: TriggerRule(PS=1, comment='LArNoise-Menu : stable-beam conditions, 700b scaled by 0.8') } ,
    'EF_L1J10_empty_NoAlg'  : { 1000: TriggerRule(PS=1, comment='LArNoise-Menu : stable-beam conditions, 700b scaled by 0.8') } ,
    
    #######################################
    # LAr calib
    
    'L2_em3_empty_larcalib' :  {100: TriggerRule(PS=-1, comment='cosmic chain for LAr, enabled during cosmic run only')    },
    'L2_em5_empty_larcalib' :  {100: TriggerRule(PS=20, comment='cosmic chain for LAr')    },
    'L2_tau8_empty_larcalib' : {100: TriggerRule(PS=16, comment='cosmic chain for LAr')    },
    'L2_j10_empty_larcalib' :  {100: TriggerRule(PS=1, comment='cosmic chain for LAr')    },
    'L2_fj10_empty_larcalib' : {100: TriggerRule(PS=1, comment='cosmic chain for LAr')    },
    'L2_L1J10_empty_NoAlg' :   {100: TriggerRule(PS=10, comment='cosmic chain for LAr')    },
    'L2_L1FJ10_empty_NoAlg' :  {100: TriggerRule(PS=1, comment='cosmic chain for LAr')    },
    
    #DISABLE depend_on items by hand
    #'EF_g11_etcut_larcalib':{100: TriggerRule(fraction=1,depends_on='EF_g11_etcut',comment='LAr PEB calibration stream')},
    #'EF_g20_loose_larcalib':{100: TriggerRule(fraction=1,depends_on='EF_g20_loose',comment='LAr PEB calibration stream')},
    #'EF_g40_loose_larcalib':{100: TriggerRule(fraction=1,depends_on='EF_g40_loose',comment='LAr PEB calibration stream')},
    #'EF_g60_loose_larcalib':{100: TriggerRule(fraction=1,depends_on='EF_g60_loose',comment='LAr PEB calibration stream')},
    #'EF_g80_loose_larcalib':{100: TriggerRule(fraction=1,depends_on='EF_g80_loose',comment='LAr PEB calibration stream')},
    
    #'L2_j25_larcalib'      :  {100: TriggerRule(fraction=1,depends_on='EF_fj30_a4tc_EFFS',comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz
    #'L2_j50_larcalib'      :  {100: TriggerRule(fraction=1,depends_on='EF_j55_a4tc_EFFS',comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz
    #'L2_j95_larcalib'      :  {100: TriggerRule(fraction=1,depends_on='EF_j100_a4tc_EFFS',comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz
    
    #'L2_fj25_larcalib'      :  {100: TriggerRule(fraction=1,depends_on='EF_fj30_a4tc_EFFS',comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz
    #'L2_fj50_larcalib'      :  {100: TriggerRule(fraction=1,depends_on='EF_fj55_a4tc_EFFS',comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz
    
    #######################################
    
    
    'EF_ROBRequestMon': { 100 : TriggerRule(PS=-1, comment='test trigger'), },
    'EF_rd1_empty_NoAlg': { 100 : TriggerRule(rate=1.0, comment='random trigger'), }, 
    #'EF_mbRd1_eff': { 100 : TriggerRule(rate=0.5, comment='random triggers'), },     
    
    'EF_L1Muon_NoAlg': {    100 : TriggerRule(PS=-1, comment='Streamers'), },
    'EF_L1Calo_NoAlg': {    100 : TriggerRule(PS=-1, comment='Streamers'), }, 
    'EF_L1MinBias_NoAlg': { 100 : TriggerRule(PS=-1, comment='Streamers'), },
    
    'L2_TRTMon_empty':  {500:TriggerRule(comment='Monitoring - rate=1',rate=1.0)},
    'L2_TRTMon_filled': {500:TriggerRule(comment='Monitoring',rate=1)},
    
        
    ##############################################################
    #  EB chains
    'EF_eb_random':                  { 100 : TriggerRule(PS=1, comment='cosmic'), },
    'EF_eb_physics':                 { 100 : TriggerRule(PS=4, comment='EnhancedBias chains'), },
    'EF_eb_physics_unpaired_iso':    { 100 : TriggerRule(PS=2, comment='EnhancedBias chains'), },
    'EF_eb_physics_firstempty':      { 100 : TriggerRule(PS=300, comment='EnhancedBias chains'), },
    'EF_eb_physics_empty':           { 100 : TriggerRule(PS=150, comment='EnhancedBias chains'), },

    'EF_eb_random_unpaired_iso':     { 100 : TriggerRule(PS=1, comment='EnhancedBias chains'), },
    'EF_eb_random_firstempty':       { 100 : TriggerRule(PS=1, comment='EnhancedBias chains'), },
    'EF_eb_random_empty':            { 100 : TriggerRule(PS=1, comment='cosmic'), },

    'EF_high_eb_physics':            { 100 : TriggerRule(PS=1, comment='EnhancedBias chains'), },
    'EF_eb_physics_noL1PS':          { 100 : TriggerRule(PS=1, comment='EnhancedBias chains'), },  
    'EF_eb_physics_unpaired_noniso': { 100 : TriggerRule(PS=1, comment='EnhancedBias chains'), },
    
    ##############################################################
    # Streamers
    
    #'EF_Standby': { 100 : TriggerRule(PS=-1, comment='Standby streamer'),},
    'L2_Background': { 100 : TriggerRule(PS=1, comment='cosmic'), },
    'EF_Background': { 100 : TriggerRule(PS=1, comment='cosmic'), },
    ##'EF_SeededStreamerL1CaloXS': { 100 : TriggerRule(PS=1, comment='Commissioning'), },
    #'EF_SeededStreamerL1CaloXS': { 100 : TriggerRule(PS=-1, comment='Commissioning'), },
    
    #'L2_cost_monitor':  { 100: TriggerRule(PS=2000, propagate=False, comment="Trigger monitoring"), },
    #'EF_cost_monitor':  { 100: TriggerRule(PS=100, propagate=False, comment="Trigger monitoring"), },
    

    ##############################################################
    # Set scaling for non-lumi scaling items:
    'L1_J30_UNPAIRED_NONISO':     {500: TriggerRule(PS=1,scaling="bunches", comment="sgluino searches, beam background") },
    'L1_J30_UNPAIRED_ISO':     {500: TriggerRule(PS=1,scaling="bunches", comment="sgluino searches,beam background") },
    'L1_J30_FIRSTEMPTY':   {500: TriggerRule(PS=1,comment="beam background") },
    'L1_J10_FIRSTEMPTY':   {500: TriggerRule(PS=100,comment="beam background, high rate with pileup") },
    #'L1_MU4_FIRSTEMPTY':   {500: TriggerRule(PS=1300,scaling="bunches", comment="beam background") },
    'L1_MU6_FIRSTEMPTY':   {500: TriggerRule(PS=250,scaling="bunches", comment="beam background") },
    'L1_2MU6_FIRSTEMPTY':   {500: TriggerRule(PS=4,scaling="bunches", comment="beam background") },
    'L1_2MU4_FIRSTEMPTY':   {500: TriggerRule(PS=50,scaling="bunches", comment="beam background") },
    
    
    'L1_MU10_FIRSTEMPTY':  {500: TriggerRule(PS=17,scaling="bunches", comment="beam background") },
    'L1_EM3_FIRSTEMPTY':   {500: TriggerRule(PS=-1,scaling="bunches", comment="beam background, not used so diabled") },
    'L1_TAU8_FIRSTEMPTY':   {500: TriggerRule(PS=40,scaling="bunches", comment="beam background") },
    #  'L1_FJ10_FIRSTEMPTY':   {500: TriggerRule(PS=-1,scaling="bunches", comment="beam background") },
    'L1_RD1_EMPTY':        {500: TriggerRule(rate=1.0,scaling="bunches", comment="beam background") },  
    
    #????
    'L1_MU4_J15_FIRSTEMPTY':   { 100: TriggerRule(PS=1) },
    'L1_MU4_UNPAIRED_NONISO':  { 100: TriggerRule(PS=5000,comment="0.2Hz for background"), },
    'L1_MU4_UNPAIRED_ISO': { 100 : TriggerRule(PS=1200, comment='0.3Hz for background'), },
    #????
    
    
    'L1_2FJ30':   { 100: TriggerRule(PS=1) },
    'L1_2J20_XE20':   { 100: TriggerRule(PS=1) },
    'L1_2MU4':   { 100: TriggerRule(PS=1) },
    'L1_EM30':   { 100: TriggerRule(PS=1) },
    'L1_2TAU11':   { 100: TriggerRule(PS=1) },
    'L1_2TAU8_EM10VH':   { 100: TriggerRule(PS=1) },
    'L1_EM10VH_XE30':   { 100: TriggerRule(PS=1) },
    'L1_EM16VH':   { 100: TriggerRule(PS=1) },
    'L1_EM5_MU6':   { 100: TriggerRule(PS=1) },
    'L1_FJ75':   { 100: TriggerRule(PS=1) },
    'L1_J50_XE35':   { 100: TriggerRule(PS=1) },
    'L1_J75':   { 100: TriggerRule(PS=1) },
    'L1_MU11':   { 100: TriggerRule(PS=1) },
    'L1_MU4_J20_XE20':   { 100: TriggerRule(PS=1) },
    'L1_TAU15_XE10_3J10':   { 100: TriggerRule(PS=1) },
    'L1_TAU30':   { 100: TriggerRule(PS=1) },
    'L1_TAU8_MU10':   { 100: TriggerRule(PS=1) },
    'L1_XE60':   { 100: TriggerRule(PS=1) },


    
    #PJB these are for background stream, pscales apply for 326 and 620 bunch configs - double for 480 bunches
    #The following are already increased for 768b. (UNPAIRED ISO increased by 3 - the # bunches is increased by more than that)
    'L1_EM3_UNPAIRED_ISO':  { 100 : TriggerRule(PS=75), },# was 25
    'L1_TAU8_UNPAIRED_ISO': { 100 : TriggerRule(PS=15), },# was 5
    'L1_J10_UNPAIRED_ISO':  { 100 : TriggerRule(PS=15), },# was 5
    'L1_J10_UNPAIRED_NONISO':  { 100 : TriggerRule(PS=5), },

    'L1_BCM_AC_CA_BGRP0':        { 100: TriggerRule(PS=10000) },
    'L1_BCM_AC_CA_UNPAIRED_ISO': { 100: TriggerRule(PS=100) },
    'L1_BCM_Wide_EMPTY':         { 100: TriggerRule(PS=1) },
    'L1_BCM_Wide_UNPAIRED_ISO':  { 100: TriggerRule(PS=15) },# was 5
    'L1_BCM_Wide_UNPAIRED_NONISO':  { 100: TriggerRule(PS=30) },

    'L1_LUCID_A_C_EMPTY':        { 100: TriggerRule(PS=5000) },
    'L1_LUCID_A_C_UNPAIRED_ISO': { 100: TriggerRule(PS=3000) },# was 3000
    'L1_LUCID_A_C_UNPAIRED_NONISO': { 100: TriggerRule(PS=5000) },
    'L1_LUCID_EMPTY':        { 100: TriggerRule(PS=400000) },
    'L1_LUCID_UNPAIRED_ISO': { 100: TriggerRule(PS=60000) },# was 20000

    'L1_ZDC_A_C_EMPTY':      { 100: TriggerRule(PS=1) },
    'L1_ZDC_EMPTY':          { 100: TriggerRule(PS=4000) },
    'L1_ZDC_UNPAIRED_ISO':   { 100: TriggerRule(PS=1000) },
    'L1_ZDC_A_C_UNPAIRED_ISO': { 100: TriggerRule(PS=1) },

    'L1_2MU6_UNPAIRED_ISO':  { 500 : TriggerRule(PS=1), },
    'L1_2MU6_UNPAIRED_NONISO': { 500 : TriggerRule(PS=1), },
    'L1_EM3_UNPAIRED_NONISO':  { 500 : TriggerRule(PS=40), },
    'L1_TAU8_UNPAIRED_NONISO': { 500 : TriggerRule(PS=4), },
    'L1_FJ10_UNPAIRED_ISO': { 500 : TriggerRule(PS=1), },
    
    
    ##############################################################   

  'L2_L1MU10_firstempty_NoAlg':       { 100: TriggerRule(PS=1.0,comment='L1 passthrough',),},
  'EF_L1MU10_firstempty_NoAlg':       { 100: TriggerRule(PS=1.0,comment='L1 passthrough',),},

    ##############################################################   

    'EF_zerobias_NoAlg': { 100 : TriggerRule(PS=1, comment='zeroBias, prescaled primary'), },
    'L1_ZB': { 100 : TriggerRule(PS=8, comment='Data for MC overlay studies'), }, # OI agreed on 1Hz in average during the fill. Needs to be updated once lumi > 2e33
    
    
  #'EF_j30_eta13_a4tc_EFFS_EFxe30_noMu_empty': { 100 : TriggerRule(PS=1, comment=' background trigger, can be prescaled'), },
  #'EF_j50_eta13_a4tc_EFFS_EFxe50_noMu_empty': { 100 : TriggerRule(PS=1, comment=' background trigger, can be prescaled'), },
  #'EF_j50_eta25_a4tc_EFFS_EFxe50_noMu_empty': { 100 : TriggerRule(PS=1, comment=' background trigger, can be prescaled'), },


}

