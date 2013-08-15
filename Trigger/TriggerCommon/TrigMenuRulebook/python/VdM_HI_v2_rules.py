# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

### This is a PS list for a menu for P1 
### consistent with XML files:
### /afs/cern.ch/atlas/software/builds/nightlies/17.1.X.Y.Z-VAL/AtlasCAFHLT/rel_1/Trigger/TriggerCommon/TriggerMenuXML/data/LVL1config_Physics_HI_v2_17.1.5.15.1.xml
### /afs/cern.ch/atlas/software/builds/nightlies/17.1.X.Y.Z-VAL/AtlasCAFHLT/rel_1/Trigger/TriggerCommon/TriggerMenuXML/data/HLTconfig_Physics_HI_v2_17.1.5.15.1.xml


from RuleClasses import TriggerRule 


# note that luminosities in this rulbook are in units of 1e24 !!!


tag = 'Physics_HI_v2' 

jet_scale_factor = 1

lhcf_in = False

rules = { 

#    'L1_MBTS_2' :                { 100 : TriggerRule( rate=30, comment='backup trigger',)}, 
#    'L1_MBTS_1_1' :              { 100 : TriggerRule( rate=4500., comment='primary trigger',),}, 
    'L1_MBTS_2_BGRP7' :                { 100 : TriggerRule( PS=1, comment='VdM',)},   ## TO CHANGE based on rate...
    'L1_MBTS_2_UNPAIRED_ISO' :         { 100 : TriggerRule( PS=5., comment='VdM',),}, ## TO CHANGE based on rate...

    'L1_RD0_FILLED' :            { 100 : TriggerRule( rate=5000, scaling='bunches', comment='gives full rate with 8Hz',)}, 
    'L1_RD0_FIRSTEMPTY' :        { 100 : TriggerRule( rate=1, scaling='bunches', comment='set to 1 Hz with full ring',)}, 
    'L1_RD0_UNPAIRED_ISO':       { 100 : TriggerRule( rate=5, scaling='bunches', comment='set to 5 Hz with 25 unpaired bunches - Background',)}, 
    'L1_RD0_EMPTY' :             { 100 : TriggerRule( rate=10, scaling='bunches', comment='set to 10 Hz with 350 empty bunches - Background',)}, 
    'L1_RD1_FILLED' :            { 100 : TriggerRule( rate=3, scaling='bunches', comment='',)}, 
    'L1_RD1_EMPTY' :             { 100 : TriggerRule( rate=1, scaling='bunches', comment='primary trigger',)},
    
    'L1_LHCF' :                  { 100 : TriggerRule(PS=(17 if lhcf_in else 1000) , comment='40Hz',), #100010
                                   80000 : TriggerRule(PS=(35 if lhcf_in else 1000) , comment='20Hz',)}, #100010


############################################## VdM    
####### start running the below in parallel and disable mbMbts_1_1 or MBTS_2 depending on mbMbts rejection.
# Physics:                                   
    'EF_mbMbts_1_1' :            { 100 : TriggerRule( rate=20., ESValue=20, comment='primary trigger',)}, #100010
    'EF_L1MBTS_2_NoAlg' :        { 100 : TriggerRule( rate=20., ESValue=20, comment='primary trigger',)}, 
    'EF_L1ItemStreamer_L1_MBTS_2_UNPAIRED_ISO' :  { 100 : TriggerRule( rate=5.,   comment='primary trigger',)}, #100010
    'EF_rd0_filled_NoAlg':                        { 100 : TriggerRule( rate=200., comment='Unbiased triggers')}, 

# VdM: (mbts chain has typo in the menu)
    'L2_VdM_mbMbts_1_1_MBST2_BGRP7': { 100 : TriggerRule( PS=1, comment='primary trigger',)}, #Rate has to be at least 8kHz -- prescale to be adjusted accordingly
    'L2_VdM_MBTS_2_BGRP7' :          { 100 : TriggerRule( PS=1, comment='primary trigger',)}, #Rate has to be at least 8kHz -- prescale to be adjusted accordingly
    'L2_VdM_MBTS_2_UNPAIRED_ISO' :   { 100 : TriggerRule( PS=1, comment='primary trigger',)}, #Rate has to be 200Hz -- prescale to be adjusted accordingly
    
############################################## VdM    


    'L1_FJ0_EMPTY' :             { 100 : TriggerRule( PS=100, comment='primary trigger',)},
    'L1_J5_EMPTY' :              { 100 : TriggerRule( PS=10000, comment='primary trigger',)}, 
    'L1_MU0_EMPTY' :             { 100 : TriggerRule( PS=1000, comment='primary trigger - Background',)}, 
    'L1_MU0_FIRSTEMPTY' :        { 100 : TriggerRule( PS=1000, comment='primary trigger',)},
    'L1_MU0_UNPAIRED_ISO' :      { 100 : TriggerRule( PS=20, comment='primary trigger - Background',)}, 
    'L1_MU4_EMPTY' :             { 100 : TriggerRule( PS=500, comment='primary trigger',)}, 
    'L1_MU4_FIRSTEMPTY' :        { 100 : TriggerRule( PS=500, comment='primary trigger',)}, 
    'L1_MU6_FIRSTEMPTY' :        { 100 : TriggerRule( PS=500, comment='primary trigger',)}, 
    'L1_MU11_EMPTY' :            { 100 : TriggerRule( PS=100, comment='primary trigger',)}, 
    'L1_2MU0_EMPTY' :            { 100 : TriggerRule( PS=100, comment='primary trigger',)}, 
    'L1_2MU4_EMPTY' :            { 100 : TriggerRule( PS=100, comment='primary trigger',)}, 
    'L1_2MU6_UNPAIRED_ISO' :     { 100 : TriggerRule( PS=1, comment='primary trigger - Background',)}, 
    'L1_2MU6_UNPAIRED_NONISO' :  { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
    'L1_2MU6_EMPTY' :            { 100 : TriggerRule( PS=100, comment='primary trigger',)}, 
    'L1_2MU6_FIRSTEMPTY' :       { 100 : TriggerRule( PS=100, comment='primary trigger',)}, 
    'L1_TAU8_UNPAIRED_ISO' :     { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L1_TAU8_UNPAIRED_NONISO' :  { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L1_EM3_UNPAIRED_ISO' :      { 100 : TriggerRule( PS=1, comment='primary trigger - Background',)},
    'L1_FJ0_UNPAIRED_ISO' :      { 100 : TriggerRule( PS=20, comment='primary trigger - Background',)},
    'L1_FJ5_UNPAIRED_ISO' :      { 100 : TriggerRule( PS=1, comment='primary trigger - Background',)},
    'L1_FJ10_UNPAIRED_ISO' :     { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L1_FJ5_UNPAIRED_NONISO' :   { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L1_FJ10_UNPAIRED_NONISO' :  { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L1_J10_UNPAIRED_ISO' :      { 100 : TriggerRule( PS=1, comment='primary trigger - Background',)},
    'L1_LUCID_A_C_UNPAIRED_ISO' :{ 100 : TriggerRule( PS=1, comment='primary trigger - Background',)},
    'L1_LUCID_A_C_EMPTY' :       { 100 : TriggerRule( PS=1, comment='primary trigger - Background',)},
    'L1_LUCID_UNPAIRED_ISO' :    { 100 : TriggerRule( PS=100, comment='primary trigger - Background',)},
    'L1_LUCID_EMPTY'    :        { 100 : TriggerRule( PS=1000, comment='primary trigger - Background',)},
    #
    'L1_BCM_Wide_EMPTY' :        { 100 : TriggerRule( PS=1, comment='primary trigger - Background',)},
    'L1_BCM_Wide_UNPAIRED_ISO' : { 100 : TriggerRule( PS=1, comment='primary trigger - Background',)},
    'L1_BCM_Wide_UNPAIRED_NONISO' : { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    'L1_BCM_AC_CA_BGRP0' :       { 100 : TriggerRule( PS=10, comment='primary trigger - Background',)},
    'L1_BCM_AC_CA_UNPAIRED_ISO' :{ 100 : TriggerRule( PS=1, comment='primary trigger - Background',)}, 
    'L1_BCM_AC_CA_UNPAIRED_NONISO' :{ 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    
    #L2         
    'L2_rd1_empty_NoAlg' :       { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 

    'L2_j95_larcalib' :          { 100 : TriggerRule( rate=0.5, comment='primary trigger',)}, 
    'L2_j25_larcalib' :          { 100 : TriggerRule( rate=0.5, comment='primary trigger',)}, 
    'L2_j50_larcalib' :          { 100 : TriggerRule( rate=1., comment='primary trigger',),
                                 10000 : TriggerRule( rate=1., comment='primary trigger',)}, 
    'L2_j200_larcalib' :         { 100 : TriggerRule( rate=1, comment='primary trigger',)},
    'L2_j25_c4ccem_L1J15_larcalib'  :    { 100 : TriggerRule( rate=1, comment='primary trigger',)},
    'L2_j45_c4ccem_larcalib'  :  { 100 : TriggerRule( rate=1, comment='primary trigger',)},
    'L1_FJ10'  :                 { 100 : TriggerRule( rate=800, comment='HACK till we get predictions for larcalib',)},
    'L2_fj30_c4ccem_larcalib'  : { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    
    #cost monitoring
    'L2_cost_monitor' :          { 100 : TriggerRule( propagate=False,PS=2000, comment='primary trigger',)}, 
    
    #backgrounds
    'L2_Background_RD0_EMPTY' :  { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_Background_RD0_UNPAIRED_ISO' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_Background' :            { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    
    #tile calib
    'L2_TileCalib_laser' :       { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    
    'L2_g5_NoCut_cosmic' :       { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_tauNoCut_cosmic' :       { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    
    #muon calib
    'L2_mu6_l2cal' :             { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_mu4_l2cal_empty' :       { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_mu4T_l2cal' :            { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    
    'L2_mbRd0_eff_unpaired_iso': { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_rd0_empty_NoAlg' :       { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    
    'L2_InDetMon_FS' :           { 100 : TriggerRule( rate=0.5, comment='primary trigger',)}, 
    
    'L2_vtxbeamspot_allTE_L2Star_B_peb' :  { 100 : TriggerRule( rate=30, comment='primary trigger',)}, 
    'L2_vtxbeamspot_FSTracks_L2Star_B' :   { 100 : TriggerRule( rate=20, comment='primary trigger',)},
    'L2_vtxbeamspot_activeTE_L2Star_B_peb' : { 100 : TriggerRule( rate=3, comment='primary trigger',)},          
    
    #Noise
    'L2_PixelBeam' :             { 100 : TriggerRule( PS=200, comment='luminosity calibration',)}, 
    'L2_PixelNoise' :            { 100 : TriggerRule( rate=10, comment='calibration for InDet',)}, 
    'L2_SCTNoise' :              { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L2_IDMon_unpaired_iso' :    { 100 : TriggerRule( PS=1, comment='Monitoring - target 5 Hz',)}, 
    'L2_IDMon_filled' :          { 100 : TriggerRule( PS=1000, comment='Monitoring',)},
    'L2_IDMon_empty' :           { 100 : TriggerRule( PS=2, comment='primary trigger',)}, 
    
    #larcalib
    'L2_j5_empty_larcalib' :     { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_tau8_empty_larcalib' :   { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_j10_empty_larcalib' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_em5_empty_larcalib' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L2_em3_empty_larcalib' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L2_fj0_empty_larcalib' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    
    'L2_PreS_L1TAU8_empty' :     { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_PreS_L1J30_empty' :      { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_PreS_L1J10_empty' :      { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_PreS_L1EM5_empty' :      { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L2_PreS_L1EM3_empty' :      { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_PreS_L1J5_empty' :       { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L2_PreS_L1J30_firstempty' : { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_PreS_L1J10_firstempty' : { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    
    'L2_L1J5_empty_NoAlg' :      { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L2_L1FJ0_empty_NoAlg' :     { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L2_L1J30_empty_NoAlg' :     { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L2_L1J10_empty_NoAlg' :     { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L2_L1J10_firstempty_NoAlg': { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    
    #
    'L2_L1J5_NoAlg' :            { 100 : TriggerRule( PS=-1, comment='primary trigger',),
                                 20001 : TriggerRule( rate=5., comment='primary trigger',)},
    #EF
    'EF_PreS_L1J30_empty' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_rd1_empty_NoAlg' :     { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_cost_monitor' :        { 100 : TriggerRule( propagate=False,PS=100, comment='cost monitoring',)}, 
    'EF_L1J30_empty_NoAlg' :   { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_L1FJ1_empty_NoAlg' :   { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_mbLucid_eff_unpaired_iso' :    { 100 : TriggerRule( rate=1, comment='primary trigger',)}, 
    'EF_PreS_L1TAU8_empty' :   { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_mbSpBg_noiseSup_unpaired_iso' :    { 100 : TriggerRule( rate=0.5, comment='primary trigger',)}, 
    'EF_Background_RD0_EMPTY': { 100 : TriggerRule( PS=16*40000/61600, comment='primary trigger',)}, 
    'EF_L1J10_firstempty_NoAlg':       { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
    'EF_mbMbts_1_1_eff_unpaired_iso':  { 100 : TriggerRule( rate=1, comment='primary trigger',)}, 
    'EF_PreS_L1J30_firstempty' :       { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_mbSpBg_unpaired_iso':  { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_TileCalib_laser' :     { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_g10_loose_larcalib' :  { 100 : TriggerRule( rate=2, comment='primary trigger',)}, 
    'EF_Background_RD0_UNPAIRED_ISO' : { 100 : TriggerRule( PS=5*500/770, comment='primary trigger',)}, 
    'EF_tauNoCut_cosmic' :     { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_mbRd1_eff' :           { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_g5_NoCut_cosmic' :     { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_L1J10_empty_NoAlg' :   { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_PreS_L1J10_empty' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_L1FJ0_empty_NoAlg' :   { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    
    'EF_Background' :          { 100 : TriggerRule( PS=1, ESRate=2, comment='primary trigger',)}, 
    'EF_PreS_L1J10_firstempty' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    
    'EF_zerobias_NoAlg' :      { 100 : TriggerRule( rate=1.5, comment='primary trigger',)}, 
    'EF_PreS_L1J5_empty' :     { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    
    
    'EF_L1J5_empty_NoAlg' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_L1J30_firstempty_NoAlg' :    { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
    'EF_mbRd0_eff_unpaired_iso' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_PreS_L1EM3_empty' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_PreS_L1EM5_empty' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_InDetMon_FS' :         { 100 : TriggerRule( PS=1, ESRate=0.4, comment='primary trigger',)}, 
    'EF_g5_loose_larcalib' :   { 100 : TriggerRule( rate=1, comment='primary trigger',)}, 
    
    #LArNoiseBurst
    'EF_j10_u0uchad_LArNoiseBurst':              { 100: TriggerRule(PS=1, ESValue=10, comment='LArNoiseBurst'), },
    'EF_j10_u0uchad_empty_LArNoiseBurst':        { 100: TriggerRule(PS=1, ESValue=10, comment='LArNoiseBurst'), },
    'EF_j10_u0uchad_firstempty_LArNoiseBurst':   { 100: TriggerRule(PS=1, ESValue=10, comment='LArNoiseBurst'), },
    'EF_j30_u0uchad_empty_LArNoiseBurst':        { 100: TriggerRule(PS=1, comment='LArNoiseBurst'), },
    
    } 

