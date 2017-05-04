# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

### This is an PS list for Physics_HI_v1 menu for P1 
### The HI menu works in units of e24 cm-2s-1
### consistent with XML files:
### /afs/cern.ch/atlas/software/builds/nightlies/16.1.X.Y.Z-VAL/AtlasCAFHLT/rel_2/Trigger/TriggerCommon/TriggerMenuXML/data/LVL1config_MC_HI_v1_16.1.3.6.1.xml
### /afs/cern.ch/atlas/software/builds/nightlies/16.1.X.Y.Z-VAL/AtlasCAFHLT/rel_2/Trigger/TriggerCommon/TriggerMenuXML/data/HLTconfig_MC_HI_v1_16.1.3.6.1.xml


from RuleClasses import TriggerRule 


tag = 'Physics_HI_pbp_v1' 


rules = { 

    #disabled until fixed!!!
    'EF_xe_NoCut_allL1_noMu_Mon' : 	 { 100 : TriggerRule( PS=-1,rerun=True,comment='primary trigger',)}, 
    'EF_centrality100_60':        	 {  10: TriggerRule( PS=-1, comment='centrality trigger',)},
    'EF_centrality80_60':        	 {  10: TriggerRule( PS=-1, comment='centrality trigger',)},
    #disabled until fixed!!!


    # L1 items:
    'L1_ZDC' : 	                 { 10 : TriggerRule( PS=1, comment='primary trigger',)},
    'L1_ZDC_UNPAIRED_ISO' : 	 { 10 : TriggerRule( PS=100000, comment='primary trigger',)}, 
    'L1_ZDC_A_C' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L1_ZDC_AND_VTE50' : 	 { 10 : TriggerRule( PS=1, comment='primary trigger',)},
#                                   120 : TriggerRule( PS=-1, comment='primary trigger',)}, 
    
    'L1_MBTS_1_1' : 	 { 10 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L1_MBTS_2_2' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, #backup

    'L1_BCM_AC_CA_BGRP0' : 	 { 100 : TriggerRule( PS=10000, comment='primary trigger',)}, 
    'L1_BCM_Wide_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=15, comment='primary trigger',)}, 
    'L1_BCM_Wide_UNPAIRED_NONISO' : 	 { 100 : TriggerRule( PS=30, comment='primary trigger',)}, 
    'L1_LUCID_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=10000, comment='primary trigger',)}, 
    'L1_MU0_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=5000, comment='primary trigger',)}, 


    'L1_RD0_FILLED' : 	         { 100 : TriggerRule( PS=200, comment='primary trigger',)},   # mbsptrk -> should be 3-4Hz
    'L1_RD0_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=60000, comment='primary trigger',)}, 
    'L1_RD0_EMPTY' : 	 { 100 : TriggerRule( PS=9000,scaling="bunches", comment='primary trigger',)}, 
    'L1_RD1_FILLED' : 	 { 100 : TriggerRule(PS=288000, scaling="bunches", comment='primary trigger',)}, 
    'L1_RD1_EMPTY' : 	 { 100 : TriggerRule(PS=3020000,scaling="bunches", comment='primary trigger',)}, 

    'L1_MU0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L1_2MU0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, # for mu4_MSonly
    'L1_EM3' : 	         { 100 : TriggerRule( PS=1, comment='primary trigger',)},  # for g5_loose
    'L1_EM16' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L1_TE10' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},  # for e.g.EF_j15_a4hi_EFFS_L1TE10
    'L1_TE50' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 

    'L1_MU0_EMPTY' : 	 { 100 : TriggerRule( PS=1300, comment='primary trigger',)}, # for cosmic muons
    #for cosmic calo
    'L1_TAU8_EMPTY' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L1_J10_EMPTY' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L1_J30_EMPTY' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L1_J10_FIRSTEMPTY' : 	 { 100 : TriggerRule( PS=10,scaling="bunches", comment='primary trigger',)}, 
    'L1_J30_FIRSTEMPTY' : 	 { 100 : TriggerRule( PS=10,scaling="bunches", comment='primary trigger',)}, 
    'L1_EM3_EMPTY' : 	 { 100 : TriggerRule( PS=100, comment='primary trigger',)}, 
    'L1_EM5_EMPTY' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 



    ## background stream
    #if yes, need to add more L1 items (like ZDC_A_C_EMPTY, BCM items, LUCID items
    'EF_Background' : 	 { 100 : TriggerRule( PS=1, ESValue=1, comment='primary trigger',)},
    'EF_mbSpBg_unpaired_iso' : 	         { 10 : TriggerRule( rate=0.5, comment='primary trigger',)}, #needs RD0_UNPAIRED_ISO
    'EF_mbSpTrk_unpaired_iso': 		{  10: TriggerRule( PS=1, comment='supporting trigger',)},


    #hard probes
    'EF_mu4': 				{ 100: TriggerRule( PS=1, comment='comb muon',)},
    'EF_mu4_MSonly':	 		{  10: TriggerRule( PS=1, comment='use T version',)},
    'EF_L1J250_NoAlg': 			{  10: TriggerRule( PS=1, comment='primary trigger',)},

    'EF_L1MU0_NoAlg': 			{ 100: TriggerRule( PS=1, ESRate=0.1, comment='muon eff meas',)},       # ~20Hz
    'EF_L1MU4_NoAlg': 			{ 100: TriggerRule( PS=1, ESRate=0.13, comment='eff for muons',)},      # ~20Hz
    'EF_L1MU11_NoAlg':	 		{ 100: TriggerRule( PS=1,     comment='muon',),},
    'EF_L1MU15_NoAlg': 			{ 100: TriggerRule( PS=1,     comment='muon',)},
    'EF_L1MU20_NoAlg': 			{ 100: TriggerRule( PS=1,     comment='muon',)},
    
    'EF_j20_a2hi_EFFS_L1TE10' : 	        {  10:  TriggerRule( PS=1,     comment='physics jets',),\
                                                   220: TriggerRule( PS=1,  comment='physics jets',)},
    'EF_j20_a2hi_EFFS_L1TE20' : 	        {  10:  TriggerRule( PS=1,     comment='physics jets',),\
                                                   220: TriggerRule( PS=1,  comment='physics jets',)},
    'EF_2mu4_MSonly_EFFS_L1ZDC' : 	        { 100: TriggerRule( PS=1, ESRate=0.13, comment='primary di-muon trigger',)},
    'EF_mu10_MSonly_EFFS_L1ZDC' : 	        { 100: TriggerRule( PS=1, ESRate=0.1, comment='high-pt muon trigger',)},
    'EF_mu13_MSonly_EFFS_L1ZDC' : 	{ 100: TriggerRule( PS=1, comment='high-pt muon trigger',)},
    'EF_mu4_mu2_MSonly_EFFS_L1ZDC' : 	        { 100: TriggerRule( PS=1, comment='primary di-muon trigger',)},

    'EF_j25_a2hi_EFFS_L1TE10' : 	        {  10:  TriggerRule( PS=1,     comment='physics jets',)},
    'EF_j25_a2hi_EFFS_L1TE20' : 	        {  10:  TriggerRule( PS=1,     comment='physics jets',)},
    'EF_j25_a2hi_EFFS_L1J10' : 	        {  10: TriggerRule( PS=1, comment='jet trigger',)},
    'EF_2mu2_MSonly_EFFS_L1ZDC' : 	        { 100: TriggerRule( PS=1, comment='primary di-muon trigger',)},
    'EF_2mu2_MSonly_EFFS_L1TE10' : 	        { 100: TriggerRule( PS=1, comment='primary di-muon trigger',)},
    'EF_mu10_MSonly_EFFS_L1TE10' : 	{ 100: TriggerRule( PS=1, comment='high-pt muon trigger',)},
    'EF_mu4_MSonly_j15_a2hi_EFFS_L1TE10' : { 100: TriggerRule( PS=1, comment='heavy flavor trigger',)},
    'EF_mu4_MSonly_j15_a2hi_EFFS_matched' : { 100: TriggerRule( PS=1, ESRate=0.2, comment='heavy flavor trigger',)},
    'EF_mu4_IDTrkNoCut':		 	{ 100: TriggerRule( rate=0.5, ESRate=0.13, comment='eff determination muon',)},
    'EF_mu4T_IDTrkNoCut': 			{ 100: TriggerRule( rate=0.2, maxRate=0.2, ESRate=0.1, comment='eff determination muon',)}, 

    'EF_g5_loose': 			{  10:  TriggerRule( PS=1, ESRate=0.25, comment='phot',)},   
    'EF_2g5_loose': 			{  10: TriggerRule( rate=5, maxRate=5, ESRate=0.25, comment='di-photon',)},
    
    'EF_mu4_L1VTE50':		 	{ 100: TriggerRule( PS=1,     comment='primary muon',)},
    'EF_L1EM16_NoAlg': 			{  10:  TriggerRule( PS=1,  ESRate=0.25,   comment='phot',)},
    'EF_L1EM14_NoAlg': 			{  10:  TriggerRule( PS=1,     comment='phot',)},
    'EF_L1EM12_NoAlg': 			{  10:  TriggerRule( PS=1, ESRate=0.25, comment='phot',),\
                                           310: TriggerRule( rate=0.25, ESRate=0.25, comment='phot',)},
    'EF_mu4_MSonly_L1TE50':	 	{ 100: TriggerRule( PS=1,  ESRate=0.1,   comment='primary muon',)},
    'EF_mu4T_MSonly_L1TE50':	 	{ 100: TriggerRule( PS=1,     comment='primary muon',)},

    'EF_j20_a2hi_EFFS_L1ZDC': 		{  10: TriggerRule( PS=1, ESRate=0.3, comment='bkp above 3e26: 5kHz EF input',)},
    'EF_j25_a2hi_EFFS_L1ZDC' : 	        {  10: TriggerRule( PS=1, comment='jet trigger',)},
    'EF_j30_a2hi_EFFS_L1ZDC': 		{  10: TriggerRule( PS=1, comment='physics jets',)},
    'EF_j40_a2hi_EFFS_L1ZDC': 		{  10: TriggerRule( PS=1, comment='physics jets',)},
    'EF_j50_a2hi_EFFS_L1ZDC': 		{  10: TriggerRule( PS=1, comment='physics jets',)},
    'EF_j75_a2hi_EFFS_L1ZDC':	 	{  10: TriggerRule( PS=1, ESRate=0.3, comment='physics jets',)},
    'EF_j75_a4hi_EFFS_L1ZDC':	 	{  10: TriggerRule( PS=1, comment='jets',)}, 
    'EF_j100_a4hi_EFFS_L1ZDC':	 	{  10: TriggerRule( PS=1, comment='jets',)},
    'EF_j150_a4hi_EFFS_L1ZDC': 		{  10: TriggerRule( PS=1, comment='jets',)},

    'EF_j15_a2hi_EFFS_L1TE10' :     	{  10:  TriggerRule( PS=1,     comment='physics jets',)},
    'EF_j15_a2hi_EFFS_L1ZDC': 		{  10: TriggerRule( PS=1, ESRate=0.3,comment='bkp up to 3e26',)},



    ## L2 and EFCostMonitoring
    'L2_cost_monitor' : 	 { 10 : TriggerRule( PS=100, propagate=False, comment='primary trigger',)}, 
    'EF_cost_monitor' : 	 { 10 : TriggerRule( PS=100, propagate=False, comment='primary trigger',)},


    ## LArCells 
    'EF_g10_loose_larcalib' : 	 { 100 : TriggerRule( rate=1, comment='primary trigger',)}, #L2: g10_loose
    'EF_g5_loose_larcalib' : 	 { 100 : TriggerRule( rate=1, comment='primary trigger',)}, #L2: g5_loose
    'L2_j50_larcalib' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    'L2_j95_larcalib' : 	 { 100 : TriggerRule( rate=1.0, comment='primary trigger',)}, 
    'L2_j25_larcalib' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    'L2_j200_larcalib' : 	 { 100 : TriggerRule( rate=1.0, comment='primary trigger',)}, 
    'L2_fj25_larcalib' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 

    
    ## LArCellsEmpty
    'L2_fj10_empty_larcalib' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L2_em3_empty_larcalib' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    'L2_j10_empty_larcalib' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_em5_empty_larcalib' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'L2_tau8_empty_larcalib' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},


    #minbias + triggers under comment #pA below
    'EF_mbSpTrk': 		        	{  10: TriggerRule( PS=1, ESValue=3, comment='supporting trigger',)},
    'EF_mbZdc_a_c_eff':		 	{  10: TriggerRule( rate=1, ESRate=0.25, comment='primary trigger',)}, 
    'EF_mbZdc_a_c_eff_unpaired_iso' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_L1ItemStreamer_L1_ZDC_UNPAIRED_ISO' : 	{ 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_mbZdc_a_c_cosmic_NoAlg' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_mbZdc_and_eff_unpaired_iso' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_mbZdc_and_cosmic_NoAlg' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_mbRd0_eff_unpaired_iso' : 	        { 10 : TriggerRule( rate=1, comment='primary trigger',)}, 
    'L2_mbRd1_eff' :                 	{ 10 : TriggerRule( PS=1, comment='supporting trigger',)},
    'EF_mbRd1_eff' :                 	{ 10 : TriggerRule( PS=1, comment='supporting trigger',)},
    'EF_InDetMon_FS' : 	 { 10 : TriggerRule( rate=0.7, ESRate=0.3, comment='supporting tracking trigger',)},
    'EF_L1MBTS_2_NoAlg' : 	                {  10: TriggerRule( PS=1, comment='min bias trigger',)},   # to be prescaled if high rate!!!
    'EF_L1TE10_NoAlg' :                    {  10: TriggerRule( PS=1, comment='min bias trigger',)},
    'EF_L1TE50_NoAlg' : 	                {  10:  TriggerRule( PS=1,  ESRate=0.5, comment='primary trigger',)},
    'EF_mbZdc_a_c_L1VTE50_trk':	 	{  10:  TriggerRule( PS=1, ESRate=0.5, rerun=True, comment='primary trigger',)},


    #MinBiasOverlay 
    'EF_mbZdc_a_c_overlay_NoAlg': 		{  10: TriggerRule( rate=5, comment='primary trigger',)},

    
    ## UPC triggers 
    'EF_L1ZDC_VTE50_upc': 			{  10: TriggerRule( PS=-1, comment='primary upc',)},
    'EF_L1MU0_VTE50_upc': 		        {  10: TriggerRule( PS=-1, comment='primary upc',)},
    'EF_L1MU4_VTE50_upc': 			{  10: TriggerRule( PS=-1, comment='test upc',)},
    'EF_L1MU0_MV_VTE50_upc':	 	{  10: TriggerRule( PS=-1, comment='bkp upc',)},
    'EF_L1MU4_MV_VTE50_upc':	 	{  10: TriggerRule( PS=-1, comment='test upc',)},
    
    'EF_L1ZDC_VTE50_loose_upc': 	        {  10: TriggerRule( PS=-1,   comment='backup upc',)},
    'EF_L1ZDC_VTE50_mv_loose2_upc' : 	{  10 : TriggerRule( PS=1, comment='main UPC trigger',)},
    'EF_L1ZDC_OR_mv_loose2_upc' : 	        {  10 : TriggerRule( rate=2, comment='UPC supporting trigger',)},
    
    'EF_L1MU0_VTE50_loose_upc': 	        {  10: TriggerRule( PS=1,  comment='primary upc',)},
    'EF_L1MU4_VTE50_loose_upc': 	        {  10: TriggerRule( PS=1,  comment='primary upc',)},
    'EF_L1EM3_VTE50_loose_upc': 	        {  10: TriggerRule( PS=1,  comment='primary upc',)},
    'EF_L1EM3_VTE50_upc': 	                {  10: TriggerRule( PS=-1, comment='tighter upc',)},
    
    'EF_L1MU0_MV_VTE50_loose_upc': 	{  10: TriggerRule( PS=1,    comment='upc',)},
    'EF_L1EM3_MV_VTE50_loose_upc': 	{  10: TriggerRule( PS=1,    comment='bkp upc',)},
    'EF_L1MU4_MV_VTE50_loose_upc': 	{  10: TriggerRule( PS=1,   comment='test upc',)},        
    'EF_L1EM3_MV_VTE50_upc': 	        {  10: TriggerRule( PS=-1,    comment='bkp upc',)},
         

    #Tile 
    'L2_TileCalib_laser' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_TileCalib_laser' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 

    #TRTMonitoring 
    'L2_TRTMon_empty' : 	 { 100 : TriggerRule( PS=250, comment='monitoring',)},  #L1 seed: L1_RD0_EMPTY
    'L2_TRTMon_filled' : 	 { 100 : TriggerRule( PS=700, comment='monitoring',)},  #L1 seed: L1_RD0_FILLED

    #SCTNoise 
    'L2_SCTNoise' : 	 { 10 : TriggerRule( PS=9, comment='monitoring trigger',)}, 

    #PixelNoise 
    'L2_PixelNoise' : 	 { 10 : TriggerRule( PS=9, comment='monitoring trigger',)},

    #Muon_Calibration 
    'L2_mu6_cal' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, #=> needs L1_MU6
    'L2_mu4_cal_empty' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
    'L2_mu4T_cal' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, #=> needs L1_MU4


    ## beamspot
    # L1 item is L1_ZDC_A_C_VTE50 same as L1_ZDC_AND_VTE50??
    'L2_vtxbeamspot_activeTE_SiTrk_peb' : 	 { 100 : TriggerRule( rate=3, comment='beam spot trigger',)}, 
    'L2_vtxbeamspot_allTE_SiTrk_peb' : 	 { 100 : TriggerRule( rate=30, comment='beam spot trigger',)}, 
    'L2_vtxbeamspot_FSTracks_SiTrk' : 	 { 100 : TriggerRule( inputRate=500, comment='beam spot trigger',)}, 


    ## cosmic calo
    # needs L1 items: RD1_EMPTY, TAU8_EMPTY, EM5_EMPTY, J10_EMPTY, J30_EMPTY, J10_FIRSTEMTPY, EM3_EMPTY, J30_FIRSTEMPTY
    'L2_PreS_L1J30_empty' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_PreS_L1TAU8_empty' : 	 { 100 : TriggerRule( PS=21, comment='primary trigger',)},
    'L2_PreS_L1J30_firstempty' : 	 { 100 : TriggerRule( PS=10, comment='primary trigger',)},
    'L2_PreS_L1J10_empty' : 	 { 100 : TriggerRule( PS=12, comment='primary trigger',)},
    'L2_PreS_L1EM3_empty' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'L2_PreS_L1EM5_empty' : 	 { 100 : TriggerRule( PS=25, comment='primary trigger',)},
    'L2_PreS_L1J10_firstempty' : 	 { 100 : TriggerRule( PS=50, comment='primary trigger',)},
    'EF_rd1_empty_NoAlg' : 	 { 10 : TriggerRule( rate=1.0, comment='supporting trigger',)}, 
    'EF_tauNoCut_cosmic' : 	 { 100 : TriggerRule( PS=17, comment='primary trigger',)}, 
    'EF_PreS_L1EM5_empty' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_PreS_L1TAU8_empty' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
    'EF_PreS_L1J10_empty' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_PreS_L1J30_empty' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_PreS_L1J10_firstempty' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_PreS_L1EM3_empty' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
    'EF_PreS_L1J30_firstempty' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 


    ## cosmic muons 
    # needs L1: MU0_EMPTY
    'EF_mu4_cosmic': 			{ 100: TriggerRule( PS=1,     comment='cosmic background muon',)},
    'EF_mu4_MSonly_cosmic': 		{ 100: TriggerRule( PS=1,     comment='cosmic background muon',)},



    #HLTPassthrough ??  => was disabled in previous run
    #IDCosmic ?? => was all disabled in previous run
    #PixelBeam => was disabled
    #DISCARD stream => was disabled


    ##not sure!!!! Request form HI group 
    #'EF_L1ItemStreamer_L1_ZDC' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},   
    
    #'EF_mbMbts_1_1' :              	 { 10 : TriggerRule( rate=2, comment='backup min bias trigger',),\
    #                                       40 : TriggerRule( PS=-1, comment='backup min bias trigger',)},



    #pA triggers
    #this is new did not do anything about them yet    
    'EF_L1ItemStreamer_L1_ZDC_UNPAIRED_NONISO':     { 100: TriggerRule( PS=1, comment='pA primary trigger',)},
    'EF_L1ItemStreamer_L1_MBTS_2_UNPAIRED_NONISO':  { 100: TriggerRule( PS=1, maxRate=1, comment='pA trigger',)}, #max rate should not be more than 1HZ
    'EF_L1ItemStreamer_L1_MBTS_2_UNPAIRED_ISO':     { 100: TriggerRule( PS=1, maxRate=1, comment='pA trigger',)}, #max rate should not be more than 1HZ
    'EF_L1ItemStreamer_L1_ZDC_MBTS_1': 	         { 100: TriggerRule( PS=1, comment='pA trigger',)},
    'EF_L1ItemStreamer_L1_ZDC_MBTS_2':           { 100: TriggerRule( PS=1, comment='pA trigger',)},
    'EF_L1ItemStreamer_L1_ZDC_MBTS_1_1':  	 { 100: TriggerRule( PS=1, comment='pA trigger',)},
    'EF_L1ItemStreamer_L1_ZDC_MBTS_2_2':  	 { 100: TriggerRule( PS=1, comment='pA trigger',)},
    'EF_mbSpTrk_L1ZDC': 	                 { 100: TriggerRule( PS=1, ESRate=0.25, comment='pA primary trigger',)},
    'EF_mbSpTrk_L1MBTS_1': 	                 { 100: TriggerRule( PS=1, comment='pA primary trigger',)}, #check rate: not higher than other mbsptrk
    'EF_mbSpTrk_L1MBTS_2': 	                 { 100: TriggerRule( PS=1, comment='pA primary trigger',)},
    'EF_mbSpTrk_L1MBTS_1_1': 	                 { 100: TriggerRule( PS=1, comment='pA primary trigger',)},
    'EF_mbZdc_NoAlg': 	                         { 100: TriggerRule( PS=1, ESValue= 100,comment='pA primary trigger',)},
    'EF_j15_a4hi_EFFS_L1TE10':	 	         {  10:  TriggerRule( PS=1, comment='pPb triggers',)},
    'EF_j20_a4hi_EFFS_L1TE10':	 	         {  10:  TriggerRule( PS=1, comment='pPb triggers',)},
    'EF_2mu2_MSonly_EFF_L1MBTS':	 	 {  10:  TriggerRule( PS=1, comment='pPb triggers',)},
    'EF_2mu2_MSonly_EFFS_L1ZDC_OR':	 	 {  10:  TriggerRule( PS=1, comment='pPb triggers',)},
    'EF_mu4_MSonly_EFFS_L1MBTS':	 	 {  10:  TriggerRule( PS=1, comment='pPb triggers',)},
    'EF_mu4_MSonly_EFFS_L1ZDC_OR':	 	 {  10:  TriggerRule( PS=1, comment='pPb triggers',)},
    'EF_j15_a4hi_EFFS_L1MBTS':	 	         {  10:  TriggerRule( PS=1, comment='pPb triggers',)},
    'EF_j15_a2hi_EFFS_L1MBTS':	 	         {  10:  TriggerRule( PS=1, comment='pPb triggers',)},
    'EF_j20_a4hi_EFFS_L1MBTS':	 	         {  10:  TriggerRule( PS=1, comment='pPb triggers',)},
    'EF_j20_a2hi_EFFS_L1MBTS':	 	         {  10:  TriggerRule( PS=1, comment='pPb triggers',)},


    'EF_j15_a2hi_EFFS_L1J10': 		{  10: TriggerRule( PS=1, comment='bkp up to 3e26',)},
    'EF_j20_a2hi_EFFS_L1J10': 		{  10: TriggerRule( PS=1, comment='bkp above 3e26: 5kHz EF input',)},
    'EF_j15_a2hi_EFFS_L1TE20' : 	{  10:  TriggerRule( PS=1,     comment='physics jets',)},



    'EF_L1ItemStreamer_L1_MBTS_1_EMPTY': 	    { 100: TriggerRule( PS=-1, comment='pA trigger',)},
    'EF_L1ItemStreamer_L1_MBTS_2_EMPTY': 	    { 100: TriggerRule( PS=-1, comment='pA trigger',)},
    'EF_L1ItemStreamer_L1_MBTS_1_UNPAIRED_NONISO':  { 100: TriggerRule( PS=-1, comment='pA trigger',)},
    'EF_L1ItemStreamer_L1_MBTS_1_UNPAIRED_ISO':     { 100: TriggerRule( PS=-1, comment='pA trigger',)},
    'EF_L1ItemStreamer_L1_MBTS_1':        	 { 100: TriggerRule( PS=-1, comment='primary trigger',)},




    
    }

