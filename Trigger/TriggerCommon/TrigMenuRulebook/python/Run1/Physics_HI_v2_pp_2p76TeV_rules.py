# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

### This is a PS list for a menu for P1 
### consistent with XML files:
### /afs/cern.ch/atlas/software/builds/nightlies/17.1.X.Y.Z-VAL/AtlasCAFHLT/rel_1/Trigger/TriggerCommon/TriggerMenuXML/data/LVL1config_Physics_HI_v2_17.1.5.15.1.xml
### /afs/cern.ch/atlas/software/builds/nightlies/17.1.X.Y.Z-VAL/AtlasCAFHLT/rel_1/Trigger/TriggerCommon/TriggerMenuXML/data/HLTconfig_Physics_HI_v2_17.1.5.15.1.xml


from RuleClasses import TriggerRule 


# note that luminosities in this rulbook are in units of 1e24 !!!


tag = 'Physics_HI_v2_pp_2p76TeV' 


lhcf_in = False


rules = { 

    'L1_MBTS_1' :                { 100 : TriggerRule( PS=-1, comment='backup trigger',)}, 
    'L1_MBTS_1_EMPTY' :          { 100 : TriggerRule( PS=-1, comment='backup trigger',)}, 
    'L1_MBTS_1_UNPAIRED_ISO' :    { 100 : TriggerRule( PS=-1, comment='backup trigger',)},
    'L1_MBTS_1_1_UNPAIRED_ISO' : { 100 : TriggerRule( PS=-1, comment='backup trigger',)},
    'L1_MBTS_1_UNPAIRED_NONISO' :{ 100 : TriggerRule( PS=-1, comment='backup trigger',)},
    'L1_MBTS_2' :                { 100 : TriggerRule( PS=-1, comment='backup trigger',)},
    'L1_MBTS_2_2' :            { 100 : TriggerRule( rate=5., comment='backup trigger',)},
    'L1_MBTS_3_3' :            { 100 : TriggerRule( PS=-1, comment='backup trigger',)}, 

    'L1_MBTS_1_1' :              { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    'L1_MBTS_4_4' :            { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    #TE seeds
    'L1_TE0' :                { 100 : TriggerRule( rate=10000., comment='backup trigger',),
                                50000 : TriggerRule( rate=3000., comment='backup trigger',)},
    'L1_TE35' :                { 100 : TriggerRule( PS=1, comment='backup trigger',),
                                 40000 : TriggerRule( rate=5000., comment='backup trigger',)},
    #         'L1_RD0_FILLED' :    { 100 : TriggerRule( PS=80, comment='gives full rate with 8Hz',)},
    'L1_RD0_FILLED' :            { 100 : TriggerRule( rate=5000, scaling='bunches', comment='gives full rate with 8Hz',),
                                   100001 : TriggerRule( rate=2500, comment='gives full rate with 8Hz',)},
    #    'L1_RD0_FIRSTEMPTY' :    { 100 : TriggerRule( PS=1400*1300, comment='set to 1 Hz with full ring',)},
    'L1_RD0_FIRSTEMPTY' :        { 100 : TriggerRule( rate=1, scaling='bunches', comment='set to 1 Hz with full ring',)}, 
    #'L1_RD0_UNPAIRED_ISO' :    { 100 : TriggerRule( PS=616000, comment='set to 5 Hz with 25 unpaired bunches',)},
    'L1_RD0_UNPAIRED_ISO':       { 100 : TriggerRule( rate=5, scaling='bunches', comment='set to 5 Hz with 25 unpaired bunches - Background',)}, 
    #    'L1_RD0_EMPTY' :    { 100 : TriggerRule( PS=1400*40*1.1, comment='set to 10 Hz with 350 empty bunches',)},
    'L1_RD0_EMPTY' :            { 100 : TriggerRule( rate=10, scaling='bunches', comment='set to 10 Hz with 350 empty bunches - Background',)}, 
    #    'L1_RD1_FILLED' :    { 100 : TriggerRule( PS=40000, comment='',)},
    'L1_RD1_FILLED' :            { 100 : TriggerRule( rate=10, scaling='bunches', comment='',)}, 
    #    'L1_RD1_EMPTY' :    { 100 : TriggerRule( PS=2000*1400*1.1, comment='primary trigger',)},
    'L1_RD1_EMPTY' :             { 100 : TriggerRule( rate=10, scaling='bunches', comment='primary trigger',)},
    
    'L1_LHCF' :                  { 100 : TriggerRule(PS=(10 if lhcf_in else 1000) , comment='70Hz',),
                                   80000 : TriggerRule(PS=(23 if lhcf_in else 1000) , comment='30Hz',)}, #100010, 100091
    'L1_EM3_VTE50' :             { 100   : TriggerRule( PS=-1, comment='primary trigger',)},
    'L1_FJ0' :                   { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    'L1_ZB' :                    { 100 : TriggerRule( PS=100, comment='primary trigger',)},

    'L1_FJ0_EMPTY' :             { 100 : TriggerRule( PS=100, comment='primary trigger',)},
    'L1_J5_EMPTY' :              { 100 : TriggerRule( PS=10000, comment='primary trigger',)}, 
    'L1_MU0_EMPTY' :             { 100 : TriggerRule( PS=1000, comment='primary trigger - Background',)}, 
    'L1_MU0_FIRSTEMPTY' :        { 100 : TriggerRule( PS=1000, comment='primary trigger',)},
    'L1_MU0_UNPAIRED_ISO' :      { 100 : TriggerRule( PS=20, comment='primary trigger - Background',)}, 
    'L1_MU4_EMPTY' :            { 100 : TriggerRule( PS=500, comment='primary trigger',)}, 
    'L1_MU4_FIRSTEMPTY' :        { 100 : TriggerRule( PS=500, comment='primary trigger',)}, 
    'L1_MU6_FIRSTEMPTY' :        { 100 : TriggerRule( PS=500, comment='primary trigger',)}, 
    'L1_MU11_EMPTY' :            { 100 : TriggerRule( PS=100, comment='primary trigger',)}, 
    'L1_2MU0_EMPTY' :            { 100 : TriggerRule( PS=100, comment='primary trigger',)}, 
    'L1_2MU4_EMPTY' :            { 100 : TriggerRule( PS=100, comment='primary trigger',)}, 
    'L1_2MU6_UNPAIRED_ISO' :    { 100 : TriggerRule( PS=1, comment='primary trigger - Background',)}, 
    'L1_2MU6_UNPAIRED_NONISO' :  { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
    'L1_2MU6_EMPTY' :            { 100 : TriggerRule( PS=100, comment='primary trigger',)}, 
    'L1_2MU6_FIRSTEMPTY' :        { 100 : TriggerRule( PS=100, comment='primary trigger',)}, 
    'L1_TAU8_UNPAIRED_ISO' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L1_TAU8_UNPAIRED_NONISO' :  { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L1_EM3_UNPAIRED_ISO' :    { 100 : TriggerRule( PS=1, comment='primary trigger - Background',)},
    'L1_FJ0_UNPAIRED_ISO' :    { 100 : TriggerRule( PS=20, comment='primary trigger - Background',)},
    'L1_FJ5_UNPAIRED_ISO' :    { 100 : TriggerRule( PS=1, comment='primary trigger - Background',)},
    'L1_FJ10_UNPAIRED_ISO' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L1_FJ5_UNPAIRED_NONISO' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L1_FJ10_UNPAIRED_NONISO' :  { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L1_J10_UNPAIRED_ISO' :    { 100 : TriggerRule( PS=1, comment='primary trigger - Background',)},
    'L1_LUCID_A_C_UNPAIRED_ISO' :{ 100 : TriggerRule( PS=1, comment='primary trigger - Background',)},
    'L1_LUCID_A_C_EMPTY' :        { 100 : TriggerRule( PS=1, comment='primary trigger - Background',)},
    'L1_LUCID_UNPAIRED_ISO' :    { 100 : TriggerRule( PS=100, comment='primary trigger - Background',)},
    'L1_LUCID_EMPTY'    :      { 100 : TriggerRule( PS=1000, comment='primary trigger - Background',)},
    #
    'L1_BCM_Wide_EMPTY' :        { 100 : TriggerRule( PS=1, comment='primary trigger - Background',)},
    'L1_BCM_Wide_UNPAIRED_ISO' : { 100 : TriggerRule( PS=1, comment='primary trigger - Background',)},
    'L1_BCM_Wide_UNPAIRED_NONISO' : { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    'L1_BCM_AC_CA_BGRP0' :       { 100 : TriggerRule( PS=10, comment='primary trigger - Background',)},
    'L1_BCM_AC_CA_UNPAIRED_ISO' :{ 100 : TriggerRule( PS=1, comment='primary trigger - Background',)}, 
    'L1_BCM_AC_CA_UNPAIRED_NONISO' :{ 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    
    #L2         
    'L2_rd1_empty_NoAlg' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_j95_larcalib' :    { 100 : TriggerRule( rate=0.5, comment='primary trigger',)}, 
    'L2_j25_larcalib' :    { 100 : TriggerRule( rate=0.5, comment='primary trigger',)}, 
    'L2_j50_larcalib' :    { 100 : TriggerRule( rate=1., comment='primary trigger',),
                               10000 : TriggerRule( rate=1., comment='primary trigger',)}, 
    'L2_j200_larcalib' :    { 100 : TriggerRule( rate=1, comment='primary trigger',)},
    'L2_j25_c4ccem_L1J15_larcalib'  :    { 100 : TriggerRule( rate=1, comment='primary trigger',)},
    'L2_j45_c4ccem_larcalib'  :    { 100 : TriggerRule( rate=1, comment='primary trigger',)},
    'L1_FJ10'  :    { 100 : TriggerRule( rate=800, comment='HACK till we get predictions for larcalib',)},
    'L2_fj30_c4ccem_larcalib'  :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    
    
    
    #cost monitoring
    'L2_cost_monitor' :    { 100 : TriggerRule( propagate=False,PS=2000, comment='primary trigger',)}, 
    
    
    #backgrounds
    'L2_Background_RD0_EMPTY' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_Background_RD0_UNPAIRED_ISO' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_Background' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    
    #tile calib
    'L2_TileCalib_laser' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    
    
    'L2_mbRd1_eff' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    
    
    'L2_g5_NoCut_cosmic' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_tauNoCut_cosmic' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    
    #overlay
    'L2_L1MBTS_1_1_overlay_NoAlg' :    { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
    
    #muon calib
    'L2_mu6_l2cal' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_mu4_l2cal_empty' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_mu4T_l2cal' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    
    'L2_mbRd0_eff_unpaired_iso' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_rd0_empty_NoAlg' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    
    'L2_InDetMon_FS' :    { 100 : TriggerRule( rate=0.5, comment='primary trigger',)}, 
    
    
    'L2_vtxbeamspot_allTE_L2Star_B_peb' :    { 100 : TriggerRule( rate=30, comment='primary trigger',)}, 
    'L2_vtxbeamspot_FSTracks_L2Star_B' :    { 100 : TriggerRule( rate=20, comment='primary trigger',)},
    'L2_vtxbeamspot_activeTE_L2Star_B_peb' :    { 100 : TriggerRule( rate=3, comment='primary trigger',)},          
    
    #Noise
    'L2_PixelBeam' :         { 100 : TriggerRule( PS=2000, comment='luminosity calibration',)}, 
    'L2_PixelNoise' :         { 100 : TriggerRule( rate=10, comment='calibration for InDet',)}, 
    'L2_SCTNoise' :         { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L2_IDMon_unpaired_iso' : { 100 : TriggerRule( PS=1, comment='Monitoring - target 5 Hz',)}, 
    'L2_IDMon_filled' :     { 100 : TriggerRule( PS=1000, comment='Monitoring',)},
    'L2_IDMon_empty' :         { 100 : TriggerRule( PS=2, comment='primary trigger',)}, 
    
    #larcalib
    'L2_j5_empty_larcalib' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_tau8_empty_larcalib' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_j10_empty_larcalib' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_em5_empty_larcalib' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L2_em3_empty_larcalib' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L2_fj0_empty_larcalib' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    
    'L2_PreS_L1TAU8_empty' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_PreS_L1J30_empty' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_PreS_L1J10_empty' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_PreS_L1EM5_empty' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L2_PreS_L1EM3_empty' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_PreS_L1J5_empty' :        { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L2_PreS_L1J30_firstempty' : { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_PreS_L1J10_firstempty' : { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    
    'L2_L1J5_empty_NoAlg' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L2_L1FJ0_empty_NoAlg' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L2_L1J30_empty_NoAlg' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L2_L1J10_empty_NoAlg' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L2_L1J10_firstempty_NoAlg' :{ 100 : TriggerRule( PS=1, comment='primary trigger',)},
    
    #
    'L2_L1J5_NoAlg' :    { 100 : TriggerRule( rate=500., comment='primary trigger',)},
    'EF_mbMbts_1_1' :    { 100 : TriggerRule( PS=-1, rerun=100, comment='primary trigger',)}, #100010
    'EF_mbMbts_2' :    { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
    #'EF_mbMbts_1_1' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    
    #MinBias
    'EF_L1MBTS_2_NoAlg' :    { 100 : TriggerRule( PS=-1, ESRate=0.4, comment='primary trigger',)},
    'EF_L1MBTS_1_1_NoAlg' :  { 100 : TriggerRule( PS=-1, ESRate=0.4, comment='primary trigger',)},
    'EF_mbMbts_1_1_eff' :    { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    'EF_L1LHCF_NoAlg' :      { 100 : TriggerRule(PS=(7 if lhcf_in else -1), ESValue=100, comment='primary trigger',),
                               80000 : TriggerRule(PS=(3 if lhcf_in else -1), ESValue=100, comment='primary trigger',)}, #100091
    'EF_mbSpTrk_L1LHCF' :    { 100 : TriggerRule(PS=(1 if lhcf_in else -1), comment='',)}, #100091

    #ZDC
    'EF_mbZdc_LG_sideC' :    { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    'EF_mbZdc_HG_sideC' :    { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    'EF_mbZdc_LG_sideA' :    { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    'EF_mbZdc_HG_sideA' :    { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    'EF_L1ZDC_C_NoAlg' :     { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    'EF_L1ZDC_A_NoAlg' :     { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    'EF_L1ZDC_NoAlg' :       { 100 : TriggerRule( rate=1., comment='primary trigger',)},
    'EF_L1ItemStreamer_L1_ZDC_A_BGRP7' :       { 100 : TriggerRule( PS=500, comment='primary trigger',)},
    'EF_L1ItemStreamer_L1_ZDC_C_BGRP7' :       { 100 : TriggerRule( PS=500, comment='primary trigger',)},
    'EF_mbSpTrk_L1ZDC' :     { 100 : TriggerRule( rate=10., comment='primary trigger',)},
    
    #'EF_mbSpTrk' :            { 100 : TriggerRule( PS=16, ESValue=200,  comment='primary trigger',),
    #                          100001 : TriggerRule( PS=8, ESValue=200,  comment='primary trigger',)},
    'EF_mbSpTrk' :            { 100 : TriggerRule( PS=8, ESValue=200,  comment='primary trigger',),
                              100001 : TriggerRule( PS=4, ESValue=200,  comment='primary trigger',),
                              200001 : TriggerRule( PS=8, ESValue=200,  comment='primary trigger',)},                                
    'EF_mbLucid_eff' :        { 100 : TriggerRule( rate=5., comment='primary trigger',)},
    
    'EF_rd0_filled_NoAlg':   { 100: TriggerRule(rate=20. , comment='Unbiased triggers')}, 
    'EF_rd0_empty_NoAlg' :    { 100 : TriggerRule( rate=20. , ESValue=10, comment='primary trigger',)},
    
    #electrons
    'EF_e5_loose1' :     { 100 : TriggerRule( rate=20., rerun=True, comment='supporting trigger',)},
    'EF_e7_loose1' :     { 100 : TriggerRule( rate=30., rerun=True, comment='supporting trigger',)},
    'EF_e5_medium1' :    { 100 : TriggerRule( rate=10., rerun=True, comment='primary trigger',)},
    'EF_e7_medium1' :    { 100 : TriggerRule( PS=1, comment='primary trigger',),
                           20001 : TriggerRule( PS=-1, comment='primary trigger',)},
    'EF_e10_loose1' :    { 100 : TriggerRule( rate=20., comment='primary trigger',)}, 
    'EF_e10_medium1' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_e5_tight1' :    { 100 : TriggerRule( PS=1, comment='primary trigger',),
                           5000 : TriggerRule( PS=-1, comment='primary trigger',)},
    'EF_e7_tight1' :    { 100 : TriggerRule( PS=1, comment='primary trigger',),
                           20001 : TriggerRule( PS=-1, comment='primary trigger',)},
    
    'EF_2e5_loose1' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_2e5_medium1' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_e9_tight1' :    { 100 : TriggerRule( PS=1, comment='primary trigger',),
                          100001 : TriggerRule( rate=10., comment='primary trigger',) },
    'EF_e9_tight1_e5_etcut' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_e14_tight1' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_e15_loose0' :    { 100 : TriggerRule( PS=1, comment='supporting trigger',)},
    'EF_e15_loose1' :    { 100 : TriggerRule( PS=1, ESRate=0.4, comment='primary trigger',)},
    'EF_e15_medium1' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_e5_medium1_IDTrkNoCut' :    { 100 : TriggerRule( rate=1, ESRate=0.4, comment='ID mon trigger',)},
    'EF_e15_loose1_IDTrkNoCut' :    { 100 : TriggerRule( rate=1, comment='ID mon trigger',)},
    'EF_e5_tight1_e5_NoCut' :    { 100 : TriggerRule( rate=1, ESRate=0.4, comment='ID mon trigger',)},
    
    #photons
    'EF_g12_loose' :     { 100 : TriggerRule( PS=1, comment='primary trigger',),
                            20001 : TriggerRule( PS=-1, rerun=True, comment='primary trigger',)},
    'EF_g12_medium' :     { 100 : TriggerRule( PS=1, comment='primary trigger',),
                            20001 : TriggerRule( PS=-1, rerun=True, comment='primary trigger',)},
    'EF_g15_medium' :     { 100 : TriggerRule( PS=1, ESRate=0.5, rerun=True, comment='primary trigger',),
                            20001 : TriggerRule( rate=0.5, ESRate=0.5, rerun=True, comment='primary trigger',)},
    'EF_g10_medium' :     { 100 : TriggerRule( rate=2., rerun=True, comment='primary trigger',)}, #100010
    'EF_g7_tight' :       { 100 : TriggerRule( rate=2., rerun=True, comment='primary trigger',),
                            20001 : TriggerRule( rate=2., rerun=True, comment='primary trigger',)},
    'EF_g20_medium' :     { 100 : TriggerRule( PS=1, ESRate=0.4, comment='primary trigger',)},
    'EF_g20_loose' :      { 100 : TriggerRule( PS=1, comment='supporting trigger',)},
    'EF_2g7_loose' :     { 100 : TriggerRule( rate=20., comment='primary trigger',)},
    'EF_2g10_loose' :     { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_2g10_medium' :     { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    #tight photons only for low-lumi
    'EF_g5_loose' :       { 100 : TriggerRule( rate=3., comment='primary trigger',),
                            80001 : TriggerRule( PS=-1, comment='primary trigger',)},
    'EF_g7_loose' :       { 100 : TriggerRule( rate=20., rerun=True, comment='primary trigger',)}, #100010
    'EF_g10_loose' :      { 100 : TriggerRule( rate=20., rerun=True, comment='primary trigger',)}, #100010
    'EF_g15_loose' :      { 100 : TriggerRule( rate=20., ESRate=0.3, rerun=True, comment='primary trigger',)},
    'EF_g5_tight' :       { 100 : TriggerRule( rate=10., rerun=True, comment='primary trigger',),
                            2001 : TriggerRule( PS=-1, rerun=True, comment='primary trigger',)},
    'EF_g9_tight' :       { 100 : TriggerRule( rate=10., comment='primary trigger',),
                            2001 : TriggerRule( PS=-1, comment='primary trigger',)},
    'EF_g10_tight' :      { 100 : TriggerRule( rate=1., rerun=True, comment='primary trigger',)}, #100010
    'EF_g12_tight' :      { 100 : TriggerRule( PS=1, rerun=True, comment='primary trigger',),
                            20001 : TriggerRule( PS=-1, rerun=True, comment='primary trigger',)},
    'EF_g15_tight' :      { 100 : TriggerRule( PS=1, rerun=True, comment='primary trigger',),
                            20001 : TriggerRule( PS=-1, rerun=True, comment='primary trigger',)},
    
    #muons
    'EF_mu2' :    { 100 : TriggerRule( rate=12., rerun=True, comment='primary trigger',)},
    'EF_mu4' :    { 100 : TriggerRule( rate=25., rerun=True, comment='primary trigger',)},
    'EF_mu6' :    { 100 : TriggerRule( rate=30., rerun=True, comment='primary trigger',)},
    'EF_mu8' :    { 100 : TriggerRule( PS=1, ESRate=0.4, comment='primary trigger',),
                    100001 : TriggerRule( rate=30., ESRate=0.4, comment='primary trigger',), },
    'EF_mu10' :     { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_2mu4' :    { 100 : TriggerRule( PS=1, ESRate=0.4, comment='primary trigger',)},
    'EF_2mu4_MSonly' :    { 100 : TriggerRule( PS=1, comment='primary trigger',),
                           20001 : TriggerRule( PS=-1, comment='primary trigger',)},
    'EF_2mu2_MSonly_EFFS_L1MU0' :    { 100 : TriggerRule( PS=1, comment='primary trigger',),
                                       20001 : TriggerRule( PS=-1, comment='primary trigger',)},
    'EF_2mu2_EFFS_L1MU0' :    { 100 : TriggerRule( PS=1, comment='primary trigger',),
                                20001 : TriggerRule( PS=-1, comment='primary trigger',)},
    'EF_2mu2_EFFS' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_mu4_mu2_EFFS_L1MU0' :    { 100 : TriggerRule( PS=1, comment='primary trigger',),
                                   20001 : TriggerRule( PS=-1, comment='primary trigger',) },
    'EF_mu4_mu2_EFFS' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_L1MU0_NoAlg' :    { 100 : TriggerRule( rate=10., ESRate=0.5, comment='primary trigger',)},
    'EF_mu4_IDTrkNoCut' :    { 100 : TriggerRule( rate=1, ESRate=0.4, comment='primary trigger',)},
    'EF_mu8_IDTrkNoCut' :    { 100 : TriggerRule( rate=1, comment='primary trigger',)},
    'EF_2mu4_Jpsimumu_IDTrkNoCut' :  { 100 : TriggerRule( rate=3., ESRate=0.4, comment='primary trigger',)},
    
    #jets
    'EF_L1J5_NoAlg' :    { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
    #'EF_L1J10_NoAlg' :    { 100 : TriggerRule( PS=1, comment='primary trigger',),
    #                          10000 : TriggerRule( PS=-1, comment='primary trigger',)},
    
    #single jets
    'EF_j10_a4tchad_EFFS_L2mbMbts_L1RD0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',),
                                                   150001 : TriggerRule( PS=10, comment='primary trigger',)},
    'EF_j15_a4tchad_EFFS_L2mbMbts_L1RD0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_j20_a4tchad_EFFS_L2mbMbts_L1RD0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_j30_a4tchad_EFFS_L2mbMbts_L1RD0' : 	 {  100 : TriggerRule( PS=1, ESValue=10, comment='primary trigger',)},
    
    'EF_j10_a4tchad_EFFS_L2te_L1RD0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',),
                                           150001 : TriggerRule( PS=2, comment='primary trigger',) },
    'EF_j15_a4tchad_EFFS_L2te_L1RD0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_j20_a4tchad_EFFS_L2te_L1RD0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_j30_a4tchad_EFFS_L2te_L1RD0' : 	 {  100 : TriggerRule( PS=1, ESValue=10, comment='primary trigger',)},
   
    #L1_Jx seeded jets
    'EF_j40_a4tchad_EFFS_L1J5' :     { 100 : TriggerRule( rate=15., ESRate=0.4, comment='primary trigger',)},
    'EF_j50_a4tchad_EFFS_L1J10' :    { 100 : TriggerRule( rate=15., ESRate=0.4, comment='primary trigger',)},
    'EF_j60_a4tchad_EFFS_L1J15' :    { 100 : TriggerRule( rate=15., comment='primary trigger',)},
    'EF_j75_a4tchad_EFFS_L1J15' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_j80_a4tchad_EFFS_L1J15' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_j90_a4tchad_EFFS_L1J15' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_j100_a4tchad_EFFS_L1J20' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    #a4hi
    'EF_j75_a4hi_EFFS_L1J15' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_j80_a4hi_EFFS_L1J15' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_j90_a4hi_EFFS_L1J15' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_j100_a4hi_EFFS_L1J20' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    
    #forward
    'EF_fj10_a4tchad_EFFS_L2mbMbts_L1RD0' :  { 100 : TriggerRule( PS=1, ESValue=10, comment='primary trigger',)},
    'EF_fj15_a4tchad_EFFS_L2mbMbts_L1RD0' :  { 100 : TriggerRule( PS=1, ESValue=10, comment='primary trigger',)},

    'EF_fj10_a4tchad_EFFS_L2te_L1RD0' :  { 100 : TriggerRule( PS=1, ESValue=10, comment='primary trigger',)},
    'EF_fj15_a4tchad_EFFS_L2te_L1RD0' :  { 100 : TriggerRule( PS=1, ESValue=10, comment='primary trigger',)},
    
    #dijets
    'EF_2j10_a4tchad_EFFS_L2mbMbts_L1RD0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_2j15_a4tchad_EFFS_L2mbMbts_L1RD0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_2j20_a4tchad_EFFS_L2mbMbts_L1RD0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},

    'EF_2j10_a4tchad_EFFS_L2te_L1RD0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_2j15_a4tchad_EFFS_L2te_L1RD0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_2j20_a4tchad_EFFS_L2te_L1RD0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    
    'EF_3j15_a4tchad_L2FS' : 	     { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_3j15_a4tchad_L2EF_2L1J5' :   { 100 : TriggerRule( PS=1, comment='primary trigger',),
                                       70000 : TriggerRule( PS=10, comment='primary trigger',)},
    
    'EF_2j15_a4tchad_L2FS' : 	 { 100 : TriggerRule( rate=10., comment='primary trigger',)},
    'EF_3j15_a4tchad_EFFS_L2mbMbts_L1RD0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_3j15_a4tchad_EFFS_L2te_L1RD0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},

    #deta jets
    'EF_2j10_a4tchad_deta40_L2mbMbts_L1RD0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_2j10_a4tchad_eta50_deta40_L2mbMbts_L1RD0' : 	 { 100 : TriggerRule( PS=1, ESValue=10, comment='primary trigger',)},
    'EF_2j15_a4tchad_deta40_L2mbMbts_L1RD0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_2j15_a4tchad_eta50_deta40_L2mbMbts_L1RD0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},

    'EF_2j10_a4tchad_deta40_L2te_L1RD0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_2j10_a4tchad_eta50_deta40_L2te_L1RD0' : 	 { 100 : TriggerRule( PS=1, ESValue=10, comment='primary trigger',)},
    'EF_2j15_a4tchad_deta40_L2te_L1RD0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_2j15_a4tchad_eta50_deta40_L2te_L1RD0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
        
    #combined
    'EF_mu4_j15_a4tchad_EFFS_matched' :    { 100 : TriggerRule( rate=30., comment='primary trigger',)},
    'EF_mu4_j20_a4tchad_EFFS_matched' :    { 100 : TriggerRule( PS=1, comment='primary trigger',),
                                             50000 : TriggerRule( rate=30., comment='primary trigger',)},
    'EF_mu4_j30_a4tchad_EFFS_matched' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_mu4_j40_a4tchad_EFFS_matched' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    #'EF_g10_medium_2j10_a4tchad_EFFS_L1EM5' :    { 100 : TriggerRule( rate=5, comment='primary trigger',)},

    #HM jets
    'EF_j10_a4hi_EFFS_L1TE90' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_2j10_a4hi_EFFS_L1TE90' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_L1TE65_NoAlg' :    { 100 : TriggerRule( rate=7., comment='primary trigger',)},
    'EF_L1TE90_NoAlg' :    { 100 : TriggerRule( rate=20., comment='primary trigger',)},
    
    #missing Et
    'EF_xe20_FEB_L1EM5' :    { 100 : TriggerRule( PS=1, comment='supporting trigger',)},
    'EF_xe20_FEB_L1J10' :    { 100 : TriggerRule( PS=1, comment='supporting trigger',)},
    'EF_xe20_FEB_L1MU0' :    { 100 : TriggerRule( PS=1, comment='supporting trigger',)},
    'EF_xe20_FEB_L1TE65' :  { 100 : TriggerRule( PS=1, comment='supporting trigger',)},
    'EF_xe20_FEB_L1TE90' :  { 100 : TriggerRule( PS=1, comment='supporting trigger',)},
                              
    'EF_xe25_FEB_L1TE65' :  { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_xe25_FEB_L1TE90' :  { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    
    'EF_xe30_FEB_L1EM5' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_xe30_FEB_L1J10' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_xe30_FEB_L1MU0' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_xe30_FEB_L1TE65' :  { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_xe30_FEB_L1TE90' :  { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    
    'EF_xe40_FEB_L1EM5' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_xe40_FEB_L1J10' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_xe40_FEB_L1MU0' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_xe40_FEB_L1TE65' :  { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_xe40_FEB_L1TE90' :  { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    
    #upc
    'EF_L1MU0_VTE20_hip_loose2_upc' :        { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    'EF_L1EM3_VTE20_hip_loose2_upc' :        { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    'EF_L1EM3_VTE50_hip_loose2_upc' :        { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, #100010 (request to disable)
    'EF_L1ZDC_A_VTE20_hip_loose2_upc' :      { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    'EF_L1ZDC_C_VTE20_hip_loose2_upc' :      { 100 : TriggerRule( PS=-1, comment='primary trigger',)},    
                                             
    #HMT seeded of MBTS

    #TE0
    'EF_mbSpTrkVtxMh_pp_trk30_L1TE0_pileupSup' :    { 100 : TriggerRule( rate=7., comment='primary trigger',)},
    'EF_mbSpTrkVtxMh_pp_trk50_L1TE0_pileupSup' :    { 100 : TriggerRule( rate=200., ESRate=0.2, comment='primary trigger',)},
    'EF_mbSpTrkVtxMh_pp_trk60_L1TE0_pileupSup' :    { 100 : TriggerRule( rate=50., comment='primary trigger',)},
    'EF_mbSpTrkVtxMh_pp_trk70_L1TE0_pileupSup' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_mbSpTrkVtxMh_pp_trk80_L1TE0_pileupSup' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_mbSpTrkVtxMh_pp_trk90_L1TE0_pileupSup' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_mbSpTrkVtxMh_pp_trk100_L1TE0_pileupSup' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},

    #TE10
    'EF_mbSpTrkVtxMh_pp_trk30_L1TE10_pileupSup' :    { 100 : TriggerRule( rate=7., comment='primary trigger',)},
    'EF_mbSpTrkVtxMh_pp_trk50_L1TE10_pileupSup' :    { 100 : TriggerRule( rate=25., ESRate=0.2, comment='primary trigger',)},
    'EF_mbSpTrkVtxMh_pp_trk60_L1TE10_pileupSup' :    { 100 : TriggerRule( rate=25., comment='primary trigger',)},
    'EF_mbSpTrkVtxMh_pp_trk70_L1TE10_pileupSup' :    { 100 : TriggerRule( PS=1, comment='primary trigger',),
                                                       50000 : TriggerRule( rate=25., comment='primary trigger',)},
    'EF_mbSpTrkVtxMh_pp_trk80_L1TE10_pileupSup' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_mbSpTrkVtxMh_pp_trk90_L1TE10_pileupSup' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_mbSpTrkVtxMh_pp_trk100_L1TE10_pileupSup' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_mbSpTrkVtxMh_pp_trk110_L1TE10_pileupSup' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_mbSpTrkVtxMh_pp_trk120_L1TE10_pileupSup' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_mbSpTrkVtxMh_pp_trk130_L1TE10_pileupSup' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},

    #TE20
    'EF_mbSpTrkVtxMh_pp_trk30_L1TE20_pileupSup' :    { 100 : TriggerRule( rate=7., comment='primary trigger',)},
    'EF_mbSpTrkVtxMh_pp_trk80_L1TE20_pileupSup' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_mbSpTrkVtxMh_pp_trk90_L1TE20_pileupSup' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_mbSpTrkVtxMh_pp_trk100_L1TE20_pileupSup' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_mbSpTrkVtxMh_pp_trk110_L1TE20_pileupSup' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_mbSpTrkVtxMh_pp_trk120_L1TE20_pileupSup' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_mbSpTrkVtxMh_pp_trk130_L1TE20_pileupSup' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},

    'EF_L1TE0_NoAlg' :    { 100 : TriggerRule( rate=10., comment='primary trigger',)},
    'EF_L1TE10_NoAlg' :    { 100 : TriggerRule( rate=10., comment='primary trigger',)},
    'EF_L1TE20_NoAlg' :    { 100 : TriggerRule( rate=5., comment='primary trigger',)},

    #MinBias Overlay
    'EF_L1MBTS_1_1_overlay_NoAlg' :    { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    'EF_mbMbts_1_1_overlay' :    { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    'EF_zerobias_Overlay_NoAlg' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_zerobias_Overlay_L2mbMbts' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)},

    #EF
    'EF_PreS_L1J30_empty' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_rd1_empty_NoAlg' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_cost_monitor' :    { 100 : TriggerRule( propagate=False,PS=100, comment='cost monitoring',)}, 
    'EF_mbSpTrk_unpaired_iso' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_L1J30_empty_NoAlg' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_mbLucid_eff_unpaired_iso' :    { 100 : TriggerRule( rate=1, comment='primary trigger',)}, 
    'EF_PreS_L1TAU8_empty' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_mbSpBg_noiseSup_unpaired_iso' :    { 100 : TriggerRule( rate=0.5, comment='primary trigger',)}, 
    'EF_Background_RD0_EMPTY' :    { 100 : TriggerRule( PS=16*40000/61600, comment='primary trigger',)}, 
    'EF_L1J10_firstempty_NoAlg' :    { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
    'EF_mbMbts_1_1_eff_unpaired_iso' :    { 100 : TriggerRule( rate=1, comment='primary trigger',)}, 
    'EF_PreS_L1J30_firstempty' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_mbSpBg_unpaired_iso' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_TileCalib_laser' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_g10_loose_larcalib' :    { 100 : TriggerRule( rate=2, comment='primary trigger',)}, 
    'EF_Background_RD0_UNPAIRED_ISO' :    { 100 : TriggerRule( PS=5*500/770, comment='primary trigger',)}, 
    'EF_tauNoCut_cosmic' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_mbRd1_eff' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_g5_NoCut_cosmic' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_L1J10_empty_NoAlg' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_PreS_L1J10_empty' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_L1FJ0_empty_NoAlg' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    
    'EF_Background' :    { 100 : TriggerRule( PS=1, ESRate=2, comment='primary trigger',)}, 
    'EF_PreS_L1J10_firstempty' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    
    'EF_zerobias_NoAlg' :    { 100 : TriggerRule( PS=5, comment='primary trigger',)},
    'EF_PreS_L1J5_empty' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    
    
    'EF_L1J5_empty_NoAlg' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_L1J30_firstempty_NoAlg' :    { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
    'EF_mbRd0_eff_unpaired_iso' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_PreS_L1EM3_empty' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_PreS_L1EM5_empty' :    { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_InDetMon_FS' :    { 100 : TriggerRule( PS=1, ESRate=0.4, comment='primary trigger',)}, 
    'EF_g5_loose_larcalib' :   { 100 : TriggerRule( rate=1, comment='primary trigger',)}, 
    
    #LArNoiseBurst
    'EF_j10_u0uchad_LArNoiseBurst':              { 100: TriggerRule(PS=1, ESValue=10, comment='LArNoiseBurst'), },
    'EF_j10_u0uchad_empty_LArNoiseBurst':        { 100: TriggerRule(PS=1, ESValue=10, comment='LArNoiseBurst'), },
    'EF_j10_u0uchad_firstempty_LArNoiseBurst':   { 100: TriggerRule(PS=1, ESValue=10, comment='LArNoiseBurst'), },
    'EF_j30_u0uchad_empty_LArNoiseBurst':        { 100: TriggerRule(PS=1, comment='LArNoiseBurst'), },

    ## disabled in pp
    'EF_j15_Ecorr':              { 100: TriggerRule(PS=-1, comment='old trigger'), },
    'EF_j20_Ecorr':              { 100: TriggerRule(PS=-1, comment='old trigger'), },
    
    } 


