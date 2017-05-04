# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule

tag = 'Monitoring_pp_v4'

physics_rules = {

    #######################################
    # LAr calib

    'L2_j40_c4cchad_larcalib' :      {100: TriggerRule(rate=1,comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz
    'L2_j165_c4cchad_larcalib' :     {100: TriggerRule(rate=1,comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz
    'L2_fj75_c4cchad_larcalib':      {100: TriggerRule(rate=1,comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz
    'L2_fj140_c4cchad_larcalib':     {100: TriggerRule(rate=1,comment='LAr PEB calibration stream')}, ##Should get ~ 1Hz

    #######################################
    # CosmicCalo

    'L1_RD1_EMPTY':         {500: TriggerRule(rate=0.2, scaling="bunches", comment="beam background, large empty calo events") },
    #BP: to use rates once available
#    'L1_RD1_EMPTY':          {500: TriggerRule(PS=2000*1400*1.1, comment="Large empty calo events ~1 Hz") },
    'L1_EM6_EMPTY'  : { 5000: TriggerRule(PS=1, scaling="bunches", comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L1_FJ30_EMPTY' : { 5000: TriggerRule(PS=1, scaling="bunches", comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L1_J10_EMPTY'  : { 5000: TriggerRule(PS=1, scaling="bunches", comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L1_J30_EMPTY'  : { 5000: TriggerRule(PS=1, scaling="bunches", comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L1_TAU8_EMPTY' : { 5000: TriggerRule(PS=1, scaling="bunches", comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_L1FJ30_empty_NoAlg' : { 5000: TriggerRule(PS=1, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_L1J10_empty_NoAlg'  : { 5000: TriggerRule(inputRate=5, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_L1J30_empty_NoAlg'  : { 5000: TriggerRule(PS=1, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_PreS_L1EM6_empty'   : { 5000: TriggerRule(inputRate=30, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_PreS_L1J10_empty'   : { 5000: TriggerRule(inputRate=5, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_PreS_L1J30_empty'   : { 5000: TriggerRule(PS=1, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_PreS_L1TAU8_empty'  : { 5000: TriggerRule(inputRate=15, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_em3_empty_larcalib' : {    0: TriggerRule(PS=-1, comment='cosmic chain for LAr'),    },
    'L2_em6_empty_larcalib' : { 5000: TriggerRule(PS=1, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_fj30_empty_larcalib': { 5000: TriggerRule(PS=1, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_g10_NoCut_cosmic'   : { 5000: TriggerRule(inputRate=30, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_j10_empty_larcalib' : { 5000: TriggerRule(PS=1, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_tau8_empty_larcalib': { 5000: TriggerRule(inputRate=15, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },
    'L2_tauNoCut_cosmic'    : { 5000: TriggerRule(inputRate=15, comment='LArNoise-Menu-2012 : stable-beam conditions, 1331b scaled by 1.0') },

    'EF_j30_u0uchad_empty_LArNoiseBurst' : {    0: TriggerRule(PS=1, comment='LArNoiseBurst'),    },
    'EF_j165_u0uchad_LArNoiseBurst' :      {    0: TriggerRule(PS=1,ESValue=1, comment='LArNoiseBurst'),    },
    'EF_j55_u0uchad_firstempty_LArNoiseBurst': {    0: TriggerRule(PS=1,ESValue=1, comment='LArNoiseBurst'),    },
    'EF_j55_u0uchad_empty_LArNoiseBurst': {    0: TriggerRule(PS=1,ESValue=1, comment='LArNoiseBurst'),    },
    'EF_xe55_LArNoiseBurst':               {    0: TriggerRule(PS=1,ESValue=1, comment='LArNoiseBurst'),    },
    'EF_xe45_LArNoiseBurst':               {    0: TriggerRule(PS=1,ESValue=1, comment='Backup LArNoiseBurst'),    },

    'EF_rd1_empty_NoAlg' :  {    0: TriggerRule(PS=1, comment='streamer for L1_RD1_EMPTY'),    },
    'EF_xe10000_L1RD1'   :  {    0: TriggerRule(PS=-1, comment='test trigger, prescaled off'), },
    'EF_PreS_L1EM3_empty' : {    0: TriggerRule(PS=-1, comment='rejection of pre-sampler noise in LAr, disabled in stable beam'),    },
    'EF_PreS_L1EM6_empty' : {    0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr'),    },
    'EF_PreS_L1TAU8_empty' : {   0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr'),    },#PS=21 (#94047)
    'EF_PreS_L1J10_empty' : {    0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr'),    },
    'EF_PreS_L1J30_empty' : {    0: TriggerRule(PS=1, comment='rejection of pre-sampler noise in LAr'),    },
    'EF_PreS_L1J30_firstempty' : {100: TriggerRule(inputRate=1, ESValue=0, comment='rejection of pre-sampler noise in LAr, physics chain') },
    'EF_g5_NoCut_cosmic' : {     0: TriggerRule(PS=-1, comment='cosmic chain for egamma, disabled in stable beam'),    },
    'EF_g10_NoCut_cosmic' : {    0: TriggerRule(PS=1, comment='cosmic chain for egamma'),    }, #PS=120 changed due to #94047
    'EF_tauNoCut_cosmic' :  {    0: TriggerRule(PS=1, comment='cosmic chain for taus'),    },#PS=7 changed due to #94047


    #new in 17.1.5.12
    'EF_j100_u0uchad_LArNoiseBurstT' : { 0: TriggerRule(PS=1, comment='LArNoiseBurst'), },
    'EF_j130_u0uchad_LArNoiseBurstT' : { 0: TriggerRule(PS=1, comment='LArNoiseBurst'), },
    'EF_j165_u0uchad_LArNoiseBurstT' : { 0: TriggerRule(PS=1, comment='LArNoiseBurst'), },
    'EF_j55_u0uchad_firstempty_LArNoiseBurstT' : { 0: TriggerRule(PS=1, comment='LArNoiseBurst'), },

    'EF_L1J10_empty_NoAlg' : {    0: TriggerRule(PS=1, comment='cosmic chain for LAr'),    },#PS=10 (#94047)
    'EF_L1J30_empty_NoAlg' : {    0: TriggerRule(PS=1, comment='cosmic chain for LAr'),    },
    'EF_L1FJ30_empty_NoAlg' : {    0: TriggerRule(PS=1, comment='cosmic chain for LAr'),    },
    'EF_PreS_L1J30_firstempty' : {100: TriggerRule(inputRate=1, ESValue=0, comment='rejection of pre-sampler noise in LAr, physics chain') },
    'EF_L1J30_firstempty_NoAlg'   :{
                 #does not seem to work 100:  TriggerRule(rate=1.0,ESRate=1.0, comment='Baseline primary; long-lived particles'),
                 500:  TriggerRule(inputRate=1,ESValue=10, comment='Baseline primary; long-lived particles')
                 },#temporary hack
    'EF_L1J10_firstempty_NoAlg'   :{
                 #does not seem to work 100:  TriggerRule(rate=1.0,ESRate=1.0, comment='Calorimeter noise monitoring'),
                 500: TriggerRule(inputRate=1,ESValue=10,scaling="bunches", comment="beam background")
                 }, #temporary hack

    #######################################
    # online ID monitoring
    #BP: these should be IDMon chains and include unpaired_iso
    'L2_IDMon_empty'        :  {    0: TriggerRule(inputRate=5, comment='Monitoring - target 5 Hz'),    },
    'L2_IDMon_filled'       :  {    0: TriggerRule(inputRate=5, comment='Monitoring'),    },
    'L2_IDMon_unpaired_iso' :  {    0: TriggerRule(inputRate=5, comment='Monitoring - target 5 Hz'),    },


    #######################################
    # offline ID monitoring
    'L2_PixelNoise' : {100: TriggerRule(inputRate=10, comment='calibration for InDet')    },
    'L2_SCTNoise'   : {100: TriggerRule(inputRate=10, comment='calibration for InDet')    },
    'EF_InDetMon_FS': {100: TriggerRule(inputRate=0.4, ESValue=1,comment="Trigger monitoring")},

    #######################################
    # Offline luminosity measurement
    'L2_PixelBeam' : {100: TriggerRule(inputRate=100, comment='luminosity calibration.')    },

    #######################################
    # Tile
    'EF_TileCalib_laser' : {100: TriggerRule(PS=1, comment='Tile calibration')    },

    #######################################
    # ID Calib - copied over from 2011

    'L2_trk9_Central_Tau_IDCalib' :   {100: TriggerRule(PS=12, comment='IDTracks')    },
    'L2_trk9_Fwd_Tau_IDCalib'  :      {100: TriggerRule(PS=12, comment='IDTracks')    },
    'L2_trk16_Central_Tau_IDCalib' :  {100: TriggerRule(PS=1, comment='IDTracks')    },
    'L2_trk16_Fwd_Tau_IDCalib' :      {100: TriggerRule(PS=1, comment='IDTracks')    },
    'L2_trk29_Central_Tau_IDCalib' :  {100: TriggerRule(PS=1, comment='IDTracks')    },
    'L2_trk29_Fwd_Tau_IDCalib' :      {100: TriggerRule(PS=1, comment='IDTracks')    },

}

