# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

### This is an PS list for Physics_HI_v1 menu for P1 
### The HI menu works in units of e24 cm-2s-1
### consistent with XML files:
### /afs/cern.ch/atlas/software/builds/nightlies/16.1.X.Y.Z-VAL/AtlasCAFHLT/rel_2/Trigger/TriggerCommon/TriggerMenuXML/data/LVL1config_MC_HI_v1_16.1.3.6.1.xml
### /afs/cern.ch/atlas/software/builds/nightlies/16.1.X.Y.Z-VAL/AtlasCAFHLT/rel_2/Trigger/TriggerCommon/TriggerMenuXML/data/HLTconfig_MC_HI_v1_16.1.3.6.1.xml


from RuleClasses import TriggerRule 


tag = 'MC_HI_v1' 


rules = { 


	 'L1_EM3' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_EM4' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_EM5' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_EM7' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_EM10' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 #'L1_EM12' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_EM14' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_EM16' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_MU0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_MU11' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_MU15' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_MU20' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
         'L1_MU6' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_2MU0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_2MU0_MU6' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_2MU11' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_2MU20' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_2MU6' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_3MU6' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_J10' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_J15' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_J20' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_J30' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_J50' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_J75' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_J175' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_J250' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_FJ10' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',),\
                           40 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_LUCID_A' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_LUCID_C' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_LUCID_A_C' : 	 { 10 : TriggerRule( rate=1, comment='primary trigger',),\
                                   40 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_LUCID' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_LUCID_COMM' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_MBTS_1_1' : 	 { 10 : TriggerRule( rate=10, maxRate=10, comment='primary trigger',)}, 
 	 'L1_MBTS_2_2' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_MBTS_3_3' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_MBTS_4_4' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 

 	 'L1_RD0_FILLED' : 	 { 100 : TriggerRule( PS=200, comment='primary trigger',)}, 
 	 'L1_RD0_FIRSTEMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_RD0_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=60000, comment='primary trigger',)}, 
 	 'L1_RD0_EMPTY' : 	 { 100 : TriggerRule( PS=9000,scaling="bunches", comment='primary trigger',)}, 
 	 'L1_RD1_FILLED' : 	 { 100 : TriggerRule(PS=288000, scaling="bunches", comment='primary trigger',)}, 
         'L1_RD1_EMPTY' : 	 { 100 : TriggerRule(PS=3020000,scaling="bunches", comment='primary trigger',)}, 

 	 'L1_TE5' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_TE10' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_TE20' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_TE30' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_TE40' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_TE50' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_TE2000' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_TE4000' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_TE5_NZ' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 

 	 'L1_ZDC' : 	 { 10 : TriggerRule( rate=1000, comment='primary trigger',),
                           300 : TriggerRule( rate=100, comment='primary trigger',)}, 
 	 'L1_ZDC_A' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_ZDC_C' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_ZDC_AND' : 	 { 10 : TriggerRule( rate=10, comment='primary trigger',),\
                                   40 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_ZDC_AND_VTE50' : 	 { 10 : TriggerRule( PS=1, comment='primary trigger',),\
                                   40 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_ZDC_A_C' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',),
                                   500 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_BCM_Wide' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_BCM_HT_BGRP0' : 	 { 100 : TriggerRule( PS=10000, comment='primary trigger',)}, 
 	 'L1_BCM_Wide_BGRP0' : 	 { 100 : TriggerRule( PS=600000, comment='primary trigger',)}, 
 	 'L1_BCM_AC_CA_BGRP0' : 	 { 100 : TriggerRule( PS=10000, comment='primary trigger',)}, 
 	 'L1_NIM_S8C2B21' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_NIM_S8C2B22' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_NIM_S8C2B23' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 

 	 'L1_MU0_NZ' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_EM3_NZ' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_J15_NZ' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_MU0_NL' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_EM3_NL' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_J15_NL' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_2MU0_NL' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_2EM3_NL' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_2J15_NL' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_EM3_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_EM3_UNPAIRED_NONISO' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_EM3_EMPTY' : 	 { 100 : TriggerRule( PS=100, comment='primary trigger',)}, 
 	 'L1_EM3_FIRSTEMPTY' : 	 { 100 : TriggerRule( PS=-1,scaling="bunches", comment='primary trigger',)}, 
 	 'L1_EM5_EMPTY' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_MU0_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=5000, comment='primary trigger',)}, 
 	 'L1_MU0_UNPAIRED_NONISO' : 	 { 100 : TriggerRule( PS=1200, comment='primary trigger',)}, 
 	 'L1_MU0_EMPTY' : 	 { 100 : TriggerRule( PS=1300, comment='primary trigger',)}, 
 	 'L1_MU0_FIRSTEMPTY' : 	 { 100 : TriggerRule( PS=1300,scaling="bunches", comment='primary trigger',)}, 
         'L1_MU6_FIRSTEMPTY' : 	 { 100 : TriggerRule( PS=250,scaling="bunches", comment='primary trigger',)}, 
         'L1_2MU6_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_2MU6_UNPAIRED_NONISO' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_2MU6_EMPTY' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_2MU6_FIRSTEMPTY' : 	 { 100 : TriggerRule( PS=10,scaling="bunches", comment='primary trigger',)}, 
 	 'L1_TAU8_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_TAU8_UNPAIRED_NONISO' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_TAU8_EMPTY' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_TAU8_FIRSTEMPTY' : 	 { 100 : TriggerRule( PS=-1,scaling="bunches", comment='primary trigger',)}, 
 	 'L1_J10_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_J10_UNPAIRED_NONISO' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_J10_EMPTY' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_J10_FIRSTEMPTY' : 	 { 100 : TriggerRule( PS=10,scaling="bunches", comment='primary trigger',)}, 
 	 'L1_J30_EMPTY' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_J30_UNPAIRED' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_J30_FIRSTEMPTY' : 	 { 100 : TriggerRule( PS=10,scaling="bunches", comment='primary trigger',)}, 
 	 'L1_FJ10_EMPTY' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_ZDC_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_ZDC_AND_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_ZDC_A_C_EMPTY' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_ZDC_AND_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_ZDC_AND_UNPAIRED_NONISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_ZDC_UNPAIRED_ISO' : 	 { 10 : TriggerRule( PS=100000, comment='primary trigger',)}, 
 	 'L1_ZDC_A_C_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_MBTS_1_1_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_MBTS_1_1_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_MBTS_2_2_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_MBTS_3_3_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_MBTS_2_2_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_MBTS_3_3_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_MBTS_4_4_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_LUCID_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_LUCID_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=10000, comment='primary trigger',)}, 
 	 'L1_LUCID_COMM_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_LUCID_A_C_EMPTY' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_LUCID_A_C_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_LUCID_A_C_UNPAIRED_NONISO' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_ZB' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_MLZ_A' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_MLZ_C' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_MBLZ' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_BPTX0_BGRP0' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_BPTX1_BGRP0' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_BGRP0' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_BGRP1' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_BCM_Wide_EMPTY' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_BCM_Wide_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=15, comment='primary trigger',)}, 
 	 'L1_BCM_Wide_UNPAIRED_NONISO' : 	 { 100 : TriggerRule( PS=30, comment='primary trigger',)}, 
 	 'L1_BCM_AC_CA_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_CALREQ0' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_CALREQ1' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_CALREQ2' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L1_TRT' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L1_L1A_Mon' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 


 	 'L2_PreS_L1J30_empty' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
         'L2_PreS_L1TAU8_empty' : 	 { 100 : TriggerRule( PS=21, comment='primary trigger',)},
         'L2_PreS_L1J30_firstempty' : 	 { 100 : TriggerRule( PS=10, comment='primary trigger',)},
         'L2_PreS_L1J10_empty' : 	 { 100 : TriggerRule( PS=12, comment='primary trigger',)},
         'L2_PreS_L1EM3_empty' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'L2_PreS_L1EM5_empty' : 	 { 100 : TriggerRule( PS=25, comment='primary trigger',)},
         'L2_PreS_L1J10_firstempty' : 	 { 100 : TriggerRule( PS=50, comment='primary trigger',)},
         'EF_tauNoCut_cosmic' : 	 { 100 : TriggerRule( PS=17, comment='primary trigger',)}, 
 	 'EF_g5_NoCut_cosmic':		 {  10:  TriggerRule( PS=-1, comment='cosmic background trigger',)}, 

         'L2_fj10_empty_larcalib' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
         'L2_em3_empty_larcalib' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'L2_j10_empty_larcalib' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'L2_em5_empty_larcalib' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
         'L2_tau8_empty_larcalib' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
         
         #larcalib
         'L2_j50_larcalib' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
 	 'L2_j95_larcalib' : 	 { 100 : TriggerRule( rate=1.0, comment='primary trigger',)}, 
 	 'L2_j25_larcalib' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'L2_j200_larcalib' : 	 { 100 : TriggerRule( rate=1.0, comment='primary trigger',)}, 
         'L2_fj25_larcalib' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_g10_loose_larcalib' : 	 { 100 : TriggerRule( rate=1, comment='primary trigger',)},
         'EF_g5_loose_larcalib' : 	 { 100 : TriggerRule( rate=1, comment='primary trigger',)},

         'L2_fj20_Ecorr_larcalib' : 	 { 100 : TriggerRule( PS=-1, comment='test for larcalib trigger',)},
         'L2_j20_Ecorr_larcalib' : 	 { 100 : TriggerRule( PS=-1, comment='test for larcalib trigger',)},
         'L2_j15_Ecorr_larcalib' : 	 { 100 : TriggerRule( PS=-1, comment='test for larcalib trigger',)},

 	 'L2_cost_monitor' : 	 { 10 : TriggerRule( PS=100, propagate=False, comment='primary trigger',)}, 
         'EF_cost_monitor' : 	 { 10 : TriggerRule( PS=100, propagate=False, comment='primary trigger',)},
         
 	 'L2_SCTNoise' : 	 { 10 : TriggerRule( PS=9, comment='monitoring trigger',)}, 
  	 'L2_PixelNoise' : 	 { 10 : TriggerRule( PS=9, comment='monitoring trigger',)},
         'L2_PixelBeam' : 	 { 100 : TriggerRule( PS=-1, comment='backup trigger',)},
	 'L2_TRTMon_empty' : 	 { 100 : TriggerRule( PS=250, comment='monitoring',)},
         'L2_TRTMon_filled' : 	 { 100 : TriggerRule( PS=1400, comment='monitoring',)},

 	 #'L2_g5_hiptrt' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 
         'L2_TileCalib_laser' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
         
         'L2_mu6_cal' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
         'L2_mu4_cal_empty' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'L2_mu4T_cal' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},

         #'L2_vtxbeamspot_activeTE_SiTrk_peb' : 	 { 100 : TriggerRule( rate=3, comment='beam spot trigger',)}, 
         #'L2_vtxbeamspot_allTE_SiTrk_peb' : 	 { 100 : TriggerRule( rate=30, comment='beam spot trigger',)}, 
         #'L2_vtxbeamspot_FSTracks_SiTrk' : 	 { 100 : TriggerRule( inputRate=500, comment='beam spot trigger',)}, 
         'L2_vtxbeamspot_activeTE_SiTrk_peb' : 	 { 100 : TriggerRule( rate=3, propagate=False, comment='beam spot trigger',)}, 
         'L2_vtxbeamspot_allTE_SiTrk_peb' : 	 { 100 : TriggerRule( rate=30, propagate=False, comment='beam spot trigger',)}, 
         'L2_vtxbeamspot_FSTracks_SiTrk' : 	 { 100 : TriggerRule( inputRate=500, propagate=False, comment='beam spot trigger',)}, 
 
 	 
         #EF
 	 'EF_PreS_L1J30_empty' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'EF_SeededStreamerRNDM' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_mbMbts_4_4_eff' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'EF_rd1_empty_NoAlg' : 	      { 10 : TriggerRule( rate=1.0, comment='supporting trigger',)}, 
 	 'EF_mbMbts_4_4_unpaired_iso' :       { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_EM5' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_2EM5' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_TE5' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_TE50' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_J15' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_2J15' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_2J15_NZ' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_LUCID_A_C' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 
 	 'EF_xe_NoCut_allL1_noMu_Mon' : 	 { 100 : TriggerRule( PS=-1,rerun=True,comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_2MU6_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_2MU0_NL' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_2MU0_NZ' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_MU0_NZ' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_TE40' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_MU0_NL' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_BCM_Wide_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_2MU6_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 

 	 'EF_PreS_L1TAU8_empty' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 #'EF_g5_hiptrt' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'EF_Background' : 	 { 100 : TriggerRule( PS=1, ESValue=1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_TAU8_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_Standby' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_J30_UNPAIRED' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_ZDC_AND_VTE50' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_BCM_Wide_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_2MU6_UNPAIRED_NONISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_CosmicsAllTeTRTxK_TRTTrkHypo_Pixel' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_BCM_Wide_UNPAIRED_NONISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_LUCID_COMM_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_J250' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_ZDC_AND_UNPAIRED_NONISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_TE2000' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_J10_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_J10_UNPAIRED_NONISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_mbMbts_1_1_eff_unpaired_iso' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_MU20' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_PreS_L1J30_firstempty' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_ZDC_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_TileCalib_laser' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 

 	 'EF_L1ItemStreamer_L1_BPTX0_BGRP0' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_RD0_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_mbMbts_4_4' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'EF_SeededStreamerMBTS' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_J15_NZ' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_ZDC_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_SeededStreamerBCM_LUCID_ZDC' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_FJ10_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_MU0_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_MU0_UNPAIRED_NONISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_MBTS_1_1_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_NIM_S8C2B22' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_NIM_S8C2B21' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_mbMbts_4_4_eff_unpaired_iso' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 
 	 'EF_mbZdc_and_eff_unpaired_iso' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
         'EF_mbZdc_and_cosmic_NoAlg' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
         'EF_mbZdc_a_c_cosmic_NoAlg' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)},
         'EF_mbZdc_a_c_eff_unpaired_iso' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_LUCID' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_EM3_NL' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_EM3_NZ' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_TileMon' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_SeededStreamerL1Calo' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_MBTS_3_3_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_MBTS_1_1_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_PreS_L1J10_empty' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_ZDC_A_C_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_ZDC_A_C_UNPAIRED_NONISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_ZDC_A_C_VTE50' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_BCM_AC_CA_BGRP0' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_MBTS_2_2_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_BGRP0' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_MLZ_A' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_J10_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_RD1_FILLED' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_2J15_NL' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_TAU8_UNPAIRED_NONISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_MLZ_C' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_ZDC_A_C' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_mu4T_MSonly_cosmic' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_MBTS_2_2' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_2MU0' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_2MU4' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_2MU6' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_ZDC_A_C_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_TAU8_FIRSTEMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_CosmicsAllTeEFID' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_EM12' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_EM10' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_EM16' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_EM14' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_ZDC' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_PreS_L1J10_firstempty' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_J75' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_TRT' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_LUCID_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_EM3_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 
         'EF_allL1_NoAlg' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_SeededStreamerMinBias' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_J15_NL' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_FJ10' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_EM3_UNPAIRED_NONISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_2MU11' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_MU11' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_MU15' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_2MU0_MV' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_MU0_MV' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_2MU4_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_MU0_MV_VTE50' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_MU4_MV_VTE50' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_MU4_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_MU11_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_2MU0_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_J10_FIRSTEMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_EM3_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_LUCID_COMM' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_EM3_FIRSTEMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_LUCID_A_C_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_RD0_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_ZDC_AND' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_TE20' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_BCM_AC_CA_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_ZDC_C' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_ZDC_A' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_2MU0_MU6' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_BCM_Wide' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_BCM_HT_BGRP0' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_LUCID_A_C_UNPAIRED_NONISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_J50' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_J30_FIRSTEMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_BGRP1' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_MU6_FIRSTEMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_CosmicsAllTeIDSCAN_AllPhysics_TrkHypo' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_MBTS_4_4_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_TE10' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_CosmicsAllTeSiTrack_AllPhysics_TrkHypo' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_J20' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_BPTX1_BGRP0' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_ZB' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_MBLZ' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_TE4000' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_TE30' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_MU0' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_MU0_VTE50' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_MU4_VTE50' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_MU4_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_MU4_UNPAIRED_NONISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_MU4_FIRSTEMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_MU4' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_MU6' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_2MU6_FIRSTEMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_PreS_L1EM3_empty' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_mu4T_cosmic' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_RD1_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_RD0_FILLED' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_J30_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_2EM3_NL' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_2EM3_NZ' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_MBTS_2_2_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_TE5_NZ' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_PreS_L1EM5_empty' : 	 { 100 : TriggerRule( PS=1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_J30' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_MU0_FIRSTEMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_SeededStreamerMuonswBeam' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_EM5_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_RD0_FIRSTEMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_NIM_S8C2B23' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_MU0_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_LUCID_A_C_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_L1A_Mon' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_MBTS_1_1' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_MBTS_3_3' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_LUCID_C' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_LUCID_A' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_MBTS_4_4' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_BCM_Wide_BGRP0' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_J175' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         
         'EF_L1ItemStreamer_L1_EM3' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_EM4' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
         'EF_L1ItemStreamer_L1_EM7' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_2EM3' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_J10' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_ZDC_AND_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_LUCID_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_MBTSA0' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSA1' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSA2' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSA3' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSA4' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSA5' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSA6' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSA7' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSA8' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSA9' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSA10' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSA11' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSA12' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSA13' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSA14' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSA15' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSC0' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSC1' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSC2' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSC3' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSC4' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSC5' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSC6' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSC7' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSC8' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSC9' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSC10' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSC11' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSC12' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSC13' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSC14' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTSC15' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},

         'EF_L1ItemStreamer_L1_MBTS_1_1_VTE50' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         'EF_L1ItemStreamer_L1_MBTS_2_2_VTE50' : 	 { 100 : TriggerRule( PS=-1, comment='supporting trigger',)},
         
         'EF_InDetMon_FS' : 	 { 10 : TriggerRule( rate=0.7, ESRate=0.3, comment='supporting tracking trigger',)},
#         'EF_InDetMon_FS' : 	 { 10 : TriggerRule( rate=0.7, ESRate=0.3, propagate=False, comment='supporting tracking trigger',)},

         'EF_L1ItemStreamer_L1_ZDC_AND_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_MBTS_3_3_UNPAIRED_ISO' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)}, 
 	 'EF_L1ItemStreamer_L1_TAU8_EMPTY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
 
         'EF_ROBRequestMon' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         #16.1.3.14.1
         'EF_L1ItemStreamer_L1_MU0_TE50' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_MU4_TE50' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_ZDC_A_C_OVERLAY' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_ZDC_A_C_TE50' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_MBTS_2' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_e5_loose_L1VTE50' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_g5_loose_L1TE50' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},


         'EF_L1ItemStreamer_L1_EM3_TE50' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_EM3_VTE50' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_L1ItemStreamer_L1_EM3_MV_VTE50' : 	 { 100 : TriggerRule( PS=-1, comment='primary trigger',)},

         #
         #jets
         'EF_j30_a2hi_EFFS_L1ZDC': 		{  10: TriggerRule( PS=1, comment='physics jets',)},
	 'EF_j40_a2hi_EFFS_L1ZDC': 		{  10: TriggerRule( PS=1, comment='physics jets',)},
	 'EF_j50_a2hi_EFFS_L1ZDC': 		{  10: TriggerRule( PS=1, comment='physics jets',)},
	 'EF_j75_a2hi_EFFS_L1ZDC':	 	{  10: TriggerRule( PS=1, ESRate=0.3, comment='physics jets',)},

         'EF_j15_a2hi_EFFS_L1TE10' :     	{  40: TriggerRule( rate=2,  comment='physics jets',)},
         'EF_j20_a2hi_EFFS_L1TE10' : 	        {  10:  TriggerRule( PS=1,     comment='physics jets',),\
                                                   220: TriggerRule( PS=1,  comment='physics jets',)},
         'EF_j25_a2hi_EFFS_L1TE10' : 	        {  10:  TriggerRule( PS=1,     comment='physics jets',)},
         
         'EF_j15_a2hi_EFFS_L1TE20' : 	        {  10:  TriggerRule( PS=1,     comment='physics jets',),\
                                                   40: TriggerRule( PS=-1,  comment='physics jets',)},
         'EF_j20_a2hi_EFFS_L1TE20' : 	        {  10:  TriggerRule( PS=1,     comment='physics jets',),\
                                                   220: TriggerRule( PS=1,  comment='physics jets',)},
         'EF_j25_a2hi_EFFS_L1TE20' : 	        {  10:  TriggerRule( PS=1,     comment='physics jets',)},

         # backups
         'EF_j15_a2hi_EFFS_L1J10': 		{  10: TriggerRule( PS=-1, comment='bkp up to 3e26',),\
                                                   40: TriggerRule( PS=-1,  comment='physics jets',)},
         'EF_j20_a2hi_EFFS_L1J10': 		{  10: TriggerRule( PS=1, comment='bkp above 3e26: 5kHz EF input',)},
         'EF_j25_a2hi_EFFS_L1J10' : 	        {  10: TriggerRule( PS=1, comment='jet trigger',)},
         'EF_j15_a2hi_EFFS_L1ZDC': 		{  10: TriggerRule( PS=1, ESRate=0.3,comment='bkp up to 3e26',),\
                                                   40: TriggerRule( PS=-1,  comment='physics jets',)},
         'EF_j20_a2hi_EFFS_L1ZDC': 		{  10: TriggerRule( PS=1, ESRate=0.3, comment='bkp above 3e26: 5kHz EF input',)},
         'EF_j25_a2hi_EFFS_L1ZDC' : 	        {  10: TriggerRule( PS=1, comment='jet trigger',)},
         
         # R=0.4 triggers
	 'EF_j15_a4hi_EFFS_L1ZDC':	 	{  10:  TriggerRule( rate=20, comment='eff meas',),\
                                                   40:  TriggerRule( PS=-1,comment='eff meas',)},
         
                                  
         
	 'EF_j20_a4hi_EFFS_L1ZDC': 		{  10:  TriggerRule( rate=20, comment='eff meas',),\
                                                   100: TriggerRule( PS=-1,comment='eff meas',)},
                  
	 'EF_j75_a4hi_EFFS_L1ZDC':	 	{  10: TriggerRule( PS=1, comment='jets',),
                                                   500: TriggerRule( PS=-1, comment='jets',),},
         'EF_j75_a4hi_EFFS_L1J10':	 	{  10: TriggerRule( PS=1, comment='jets',)}, 
	 'EF_j100_a4hi_EFFS_L1ZDC':	 	{  10: TriggerRule( PS=1, comment='jets',),
                                                   500: TriggerRule( PS=-1, comment='jets',)},
	 'EF_j150_a4hi_EFFS_L1ZDC': 		{  10: TriggerRule( PS=1, comment='jets',),
                                                   500: TriggerRule( PS=-1, comment='jets',)},
         
         'EF_NoCut_a4JE_L15FS_NoEF': 		{  10: TriggerRule( PS=-1, comment='L1.5 pp jets',)},
         'EF_NoCut_a4TT_L15FS_NoEF': 		{  10: TriggerRule( PS=-1, comment='L1.5 pp jets',)},
#         'EF_NoCut_a4JE_L15FS_NoEF_L1TE10': 	{  10: TriggerRule( rate=1, comment='L1.5 pp jets',),
#                                                   },
#         'EF_NoCut_a4TT_L15FS_NoEF_L1TE10': 	{  10: TriggerRule( rate=1, comment='L1.5 pp jets',),
#                                                   },
         # No rate estimates for L1.5 jets
         'L2_NoCut_a4JE_L15FS_NoEF_L1TE10': 	{  10: TriggerRule( PS=50, comment='L1.5 pp jets',),
                                                   20: TriggerRule( PS=100, comment='L1.5 pp jets',),
                                                   30: TriggerRule( PS=150, comment='L1.5 pp jets',),
                                                   50: TriggerRule( PS=250, comment='L1.5 pp jets',),
                                                   70: TriggerRule( PS=350, comment='L1.5 pp jets',),
                                                   100: TriggerRule( PS=500, comment='L1.5 pp jets',),
                                                   150: TriggerRule( PS=750, comment='L1.5 pp jets',),
                                                   200: TriggerRule( PS=1000, comment='L1.5 pp jets',),
                                                   300: TriggerRule( PS=1500, comment='L1.5 pp jets',),
                                                   500: TriggerRule( PS=2500, comment='L1.5 pp jets',),
                                                   700: TriggerRule( PS=3500, comment='L1.5 pp jets',),
                                                   },
         'L2_NoCut_a4TT_L15FS_NoEF_L1TE10': 	{  10: TriggerRule( PS=50, comment='L1.5 pp jets',),
                                                   20: TriggerRule( PS=100, comment='L1.5 pp jets',),
                                                   30: TriggerRule( PS=150, comment='L1.5 pp jets',),
                                                   50: TriggerRule( PS=250, comment='L1.5 pp jets',),
                                                   70: TriggerRule( PS=350, comment='L1.5 pp jets',),
                                                   100: TriggerRule( PS=500, comment='L1.5 pp jets',),
                                                   150: TriggerRule( PS=750, comment='L1.5 pp jets',),
                                                   200: TriggerRule( PS=1000, comment='L1.5 pp jets',),
                                                   300: TriggerRule( PS=1500, comment='L1.5 pp jets',),
                                                   500: TriggerRule( PS=2500, comment='L1.5 pp jets',),
                                                   700: TriggerRule( PS=3500, comment='L1.5 pp jets',),
                                                   }, 
         'EF_NoCut_a4JE_L15FS_NoEF_L1TE10': 	{  10: TriggerRule( PS=1, comment='L1.5 pp jets',),
                                                   },
         'EF_NoCut_a4TT_L15FS_NoEF_L1TE10': 	{  10: TriggerRule( PS=1, comment='L1.5 pp jets',),
                                                   }, 


         # p+p like triggers
 	 'EF_j15_a4_EFFS_L1ZDC':	 	{  10: TriggerRule( PS=-1, comment='pp jets',), 20: TriggerRule( PS=-1, comment='pp jets',)},
	 'EF_j20_a4_EFFS_L1ZDC': 		{  10: TriggerRule( PS=-1, comment='pp jets',), 30: TriggerRule( PS=-1, comment='pp jets',)},
 	 'EF_j40_a4_EFFS_L1ZDC':	 	{  10: TriggerRule( PS=-1, comment='pp jets',), 50: TriggerRule( PS=-1, comment='pp jets',)}, 
	 'EF_j75_a4_EFFS_L1ZDC':	 	{  10: TriggerRule( PS=-1, comment='pp jets',),100: TriggerRule( PS=-1, comment='pp jets',)},
         # streamers
	 'EF_L1J75_NoAlg': 			{  10: TriggerRule( PS=-1, comment='L1 jets',)},
	 'EF_L1J175_NoAlg': 			{  10: TriggerRule( PS=-1, comment='L1 jets',)},
	 'EF_L1J250_NoAlg': 			{  10: TriggerRule( PS=1, comment='primary trigger',)},
         'EF_j15_Ecorr': 			{  10: TriggerRule( PS=-1, comment='auxiliary for larcalib trigger',)},
         'EF_j20_Ecorr': 			{  10: TriggerRule( PS=-1, comment='for larcalib trigger',)},
         'EF_fj20_Ecorr': 			{  10: TriggerRule( PS=-1, comment='for larcalib trigger',)},

         # photons
         # main 2-photon triggers
	 'EF_2g5_loose': 			{ 10: TriggerRule( PS=-1, comment='di-photon',)}, 
	 'EF_2g7_loose': 			{  10: TriggerRule( PS=-1, comment='di-photon',)}, 
	 'EF_2g10_loose':	 	 	{  10: TriggerRule( PS=-1, comment='di-photon',)}, 

         # single photon triggers
 	 'EF_g5_loose': 			{  10:  TriggerRule( PS=-1, comment='phot',)},
 	 'EF_g7_loose':	 			{  10:  TriggerRule( PS=-1,     comment='phot',),\
                                                   150: TriggerRule( PS=-1,  comment='eff 2g7_loose',),
                                                   220: TriggerRule( PS=-1, comment='eff 2g7_loose',)},
 	 'EF_g10_loose':	 		{  10:  TriggerRule( PS=-1, comment='phot',)},


 	 'EF_g5_loose_EFFS_L1ZDC': 		{  10:  TriggerRule( PS=-1,     comment='below 3.5e25',),\
                                                   35:  TriggerRule( PS=-1,    comment='CPU expensive',)}, 
	 'EF_g5_loose_L1TE50': 		        {  10:  TriggerRule( PS=-1,  comment='phot',),\
                                                   150: TriggerRule( PS=-1,  comment='phot',),\
                                                   350: TriggerRule( PS=-1,   comment='phot',)},

 	 'EF_g9_etcut': 			{  10:  TriggerRule( PS=-1,    comment='backup photon',)}, 

         'EF_L1EM3_NoAlg': 	 		{  10:  TriggerRule( PS=-1,  comment='phot',)},
	 'EF_L1EM10_NoAlg': 			{  10:  TriggerRule( PS=1,  comment='phot',),\
                                                   40:  TriggerRule( PS=-1, comment='phot',)},
	 'EF_L1EM12_NoAlg': 			{  10:  TriggerRule( rate=2, ESRate=0.25, comment='phot',)},
	 'EF_L1EM14_NoAlg': 			{  10:  TriggerRule( PS=1,     comment='phot',)},
	 'EF_L1EM16_NoAlg': 			{  10:  TriggerRule( PS=1,  ESRate=0.25,   comment='phot',)},

         'EF_L1J10_NoAlg': 			{ 100: TriggerRule( PS=-1, comment='eff measurement trigger',)},

         #Muons
         # di-muon triggers may go on a separate stream
	 'EF_2mu4_MSonly': 			{ 100: TriggerRule( PS=-1, comment='primary di-muon',)},
         'EF_2mu6_MSonly': 			{ 100: TriggerRule( PS=-1, comment='primary di-muon',)},

	 'EF_2mu4T_MSonly': 			{ 100: TriggerRule( PS=-1, comment='primary di-muon',)}, 
         'EF_2mu6T_MSonly': 			{ 100: TriggerRule( PS=-1, comment='primary di-muon',)},
         'EF_2mu4_MSonly_EFFS_L1ZDC' : 	        { 100: TriggerRule( PS=1, ESRate=0.13, comment='primary di-muon trigger',)},
         'EF_2mu4_MSonly_EFFS_L1TE10' : 	        { 100: TriggerRule( PS=1, comment='primary di-muon trigger',)},
         'EF_2mu4_MSonly_EFFS_L1TE20' : 	        { 100: TriggerRule( PS=1, comment='primary di-muon trigger',)},
         'EF_2mu4_MSonly_EFFS_L1TE50' : 	        { 100: TriggerRule( PS=1, comment='primary di-muon trigger',)},
         'EF_2mu2_MSonly_EFFS_L1ZDC' : 	        { 100: TriggerRule( PS=1, comment='primary di-muon trigger',)},
         'EF_2mu2_MSonly_EFFS_L1TE10' : 	        { 100: TriggerRule( PS=1, comment='primary di-muon trigger',)},
         'EF_2mu2_MSonly_EFFS_L1TE20' : 	        { 100: TriggerRule( PS=1, comment='primary di-muon trigger',)},
         'EF_2mu2_MSonly_EFFS_L1TE50' : 	        { 100: TriggerRule( PS=1, comment='primary di-muon trigger',)},
         'EF_mu4_mu2_MSonly_EFFS_L1ZDC' : 	        { 100: TriggerRule( PS=1, comment='primary di-muon trigger',)},
         'EF_mu4_mu2_MSonly_EFFS_L1TE10' : 	        { 100: TriggerRule( PS=1, comment='primary di-muon trigger',)},
         'EF_mu4_mu2_MSonly_EFFS_L1TE20' : 	        { 100: TriggerRule( PS=1, comment='primary di-muon trigger',)},
         'EF_mu4_mu2_MSonly_EFFS_L1TE50' : 	        { 100: TriggerRule( PS=1, comment='primary di-muon trigger',)},
         'EF_2mu4_MSonly_EFFS_L1MU0' : 	        { 100: TriggerRule( PS=-1, comment='muon trigger',)},
         'EF_mu4_MSonly_j15_a2hi_EFFS_L1TE10' : { 100: TriggerRule( PS=1, comment='heavy flavor trigger',)},
         'EF_mu4_MSonly_j15_a2hi_EFFS_matched' : { 100: TriggerRule( PS=1, ESRate=0.2, comment='heavy flavor trigger',)},

         'EF_mu10_MSonly_EFFS_L1ZDC' : 	        { 100: TriggerRule( PS=1, ESRate=0.1, comment='high-pt muon trigger',)},
         'EF_mu10_MSonly_EFFS_L1TE10' : 	{ 100: TriggerRule( PS=1, comment='high-pt muon trigger',)},
         'EF_mu10_MSonly_EFFS_L1TE20' : 	{ 100: TriggerRule( PS=1, comment='high-pt muon trigger',)},
         'EF_mu10_MSonly_EFFS_L1TE50' : 	{ 100: TriggerRule( PS=1, comment='high-pt muon trigger',)},
         'EF_mu13_MSonly_EFFS_L1ZDC' : 	        { 100: TriggerRule( PS=1, comment='high-pt muon trigger',)},
         'EF_mu13_MSonly_EFFS_L1TE10' : 	{ 100: TriggerRule( PS=1, comment='high-pt muon trigger',)},
         'EF_mu13_MSonly_EFFS_L1TE20' : 	{ 100: TriggerRule( PS=1, comment='high-pt muon trigger',)},
         'EF_mu13_MSonly_EFFS_L1TE50' : 	{ 100: TriggerRule( PS=1, comment='high-pt muon trigger',)},

 	 'EF_mu4': 				{ 100: TriggerRule( PS=-1, comment='comb muon',)},
         'EF_mu4_loose': 			{ 100: TriggerRule( PS=1, comment='comb muon',)},
	 'EF_mu6': 				{ 100: TriggerRule( PS=-1, comment='comb muon',)},
	 'EF_mu4T': 				{ 100: TriggerRule( PS=-1, comment='comb muon',)}, 
	 'EF_mu6T': 				{ 100: TriggerRule( PS=-1, comment='comb muon',)},

 	 'EF_mu4_MSonly':	 		{  10: TriggerRule( PS=-1, comment='use T version',)},
 	 'EF_mu6_MSonly':		 	{  10: TriggerRule( PS=-1, comment='use T version',)},
	 'EF_mu10_MSonly': 			{  10: TriggerRule( PS=-1, comment='use T version',),
                                                   40: TriggerRule( PS=-1, comment='use T version',)},
	 'EF_mu13_MSonly': 			{  10: TriggerRule( PS=-1, comment='use T version',),
                                                   40: TriggerRule( PS=-1, comment='use T version',)},

	 'EF_mu4T_MSonly': 			{  10:  TriggerRule( PS=-1,     comment='primary muon',),\
                                                   100: TriggerRule( PS=-1,  comment='muon',),\
                                                   150: TriggerRule( PS=-1, comment='muon',)},
	 'EF_mu6T_MSonly': 			{  10:  TriggerRule( PS=-1,     comment='primary muon',),\
                                                   40: TriggerRule( PS=-1, comment='muon',)},
	 'EF_mu10T_MSonly':		 	{  10:  TriggerRule( PS=-1,     comment='primary muon',),\
                                                   40: TriggerRule( PS=-1,    comment='muon',)},
	 'EF_mu13T_MSonly': 			{  10:  TriggerRule( PS=-1,     comment='primary muon',),\
                                                   40: TriggerRule( PS=-1,    comment='muon',)},

	 'EF_L1MU0_NoAlg': 			{ 100: TriggerRule( rate=0.1, maxRate=0.1, ESRate=0.1, comment='muon eff meas',)},
	 'EF_L1MU4_NoAlg': 			{ 100: TriggerRule( rate=0.2, maxRate=0.2, ESRate=0.13, comment='eff for muons',)},
	 'EF_L1MU11_NoAlg':	 		{ 100: TriggerRule( PS=1,     comment='muon',),},
#                                                  155: TriggerRule( PS=-1, comment='muon',),},# keep enabled, was running at 3Hz for 170bunches Tuesday early morning, stongy overlapping with mu10_MSOnly_L1ZDC

         
	 'EF_L1MU15_NoAlg': 			{ 100: TriggerRule( PS=1,     comment='muon',)},
	 'EF_L1MU20_NoAlg': 			{ 100: TriggerRule( PS=1,     comment='muon',)},

 	 'EF_mu4_MSonly_EFFS_L1ZDC': 		{  10: TriggerRule( PS=1,     comment='primary muon',),
                                                   40: TriggerRule( PS=-1, comment='muon',)},
         'EF_mu4_MSonly_EFFS_L1TE10': 		{  10: TriggerRule( PS=1,     comment='primary muon',),
                                                   40: TriggerRule( PS=-1, comment='muon',)},
         'EF_mu4_MSonly_EFFS_L1TE20': 		{  10: TriggerRule( PS=1,     comment='primary muon',),                                                  
                                                   40: TriggerRule( PS=-1, comment='muon',)},
         'EF_mu4_MSonly_EFFS_L1TE50': 		{  10: TriggerRule( PS=1,     comment='primary muon',),
                                                  40: TriggerRule( PS=-1, comment='muon',)},
 	 'EF_mu4_MSonly_EFFS_L1TE5_NZ': 	{ 100: TriggerRule( PS=-1,    comment='test UPC muon',)}, 
 	 'EF_mu4_MSonly_EFFS_L1MU0_NZ': 	{ 100: TriggerRule( PS=-1,    comment='test UPC muon',)}, 
	 'EF_mu4_MSonly_barrel': 		{ 100: TriggerRule( PS=-1,    comment='test muon',)},
         'EF_mu4_MSonly_barrel_L1TE50': 	{ 100: TriggerRule( PS=1,    comment='test muon',)},
         'EF_mu4_barrel_L1VTE50': 	{ 100: TriggerRule( PS=1,    comment='test muon',)},
	 'EF_mu4_MSonly_L1TE50':	 	{ 100: TriggerRule( PS=1,  ESRate=0.1,   comment='primary muon',)},
         'EF_mu6_MSonly_L1TE50':	 	{ 100: TriggerRule( PS=1,  comment='primary muon',)},
	 'EF_mu4_MSonly_barrel_EFFS_L1ZDC': 	{ 100: TriggerRule( PS=-1,    comment='test muon',)},
 	 'EF_mu4_IDTrkNoCut':		 	{ 100: TriggerRule( rate=0.5, ESRate=0.13, comment='eff determination muon',)},
	 'EF_mu4_L1VTE50':		 	{ 100: TriggerRule( PS=1,     comment='primary muon',)},
         'EF_mu6_L1VTE50':		 	{ 100: TriggerRule( PS=1,     comment='primary muon',)},
 	 'EF_mu4_MSonly_cosmic': 		{ 100: TriggerRule( PS=1,     comment='cosmic background muon',)},
 	 'EF_mu4_cosmic': 			{ 100: TriggerRule( PS=1,     comment='cosmic background muon',)},

	 'EF_mu4T_MSonly_barrel': 		{ 100: TriggerRule( PS=-1,    comment='test muon',)},
	 'EF_mu4T_MSonly_L1TE50':	 	{ 100: TriggerRule( PS=1,     comment='primary muon',)},
	 'EF_mu4T_cosmic': 			{ 100: TriggerRule( PS=-1,     comment='cosmic background muon',)}, 
	 'EF_mu4T_L1VTE50':		 	{ 100: TriggerRule( PS=-1,     comment='primary muon',)},
	 'EF_mu4T_IDTrkNoCut': 			{ 100: TriggerRule( rate=0.2, maxRate=0.2, ESRate=0.1, comment='eff determination muon',)}, 
	 'EF_mu4T_MSonly_cosmic': 		{ 100: TriggerRule( PS=-1,     comment='cosmic background muon',)}, 

	 'EF_mu6_IDTrkNoCut':		 	{ 100: TriggerRule( PS=-1,    comment='bkp muon',)},
	 'EF_mu6T_IDTrkNoCut': 			{ 100: TriggerRule( PS=-1,    comment='bkp muon',)},

         #UPC
	 'EF_L1ZDC_VTE50_upc': 			{  10: TriggerRule( PS=-1, comment='primary upc',)},
       	 'EF_L1MU0_VTE50_upc': 		        {  10: TriggerRule( PS=-1, comment='primary upc',)},
	 'EF_L1MU4_VTE50_upc': 			{  10: TriggerRule( PS=-1, comment='test upc',)},
	 'EF_L1MU0_MV_VTE50_upc':	 	{  10: TriggerRule( PS=-1, comment='bkp upc',)},
	 'EF_L1MU4_MV_VTE50_upc':	 	{  10: TriggerRule( PS=-1, comment='test upc',)},

         'EF_L1ZDC_VTE50_loose_upc': 	        {  10: TriggerRule( PS=-1,   comment='backup upc',)},
         'EF_L1ZDC_VTE50_mv_loose2_upc' : 	{  10 : TriggerRule( PS=-1, comment='main UPC trigger',)},
         'EF_L1ZDC_VTE50_mv_medium2_upc' : 	{  10 : TriggerRule( PS=1, comment='main UPC trigger',)},
#         'EF_L1ZDC_VTE50_mv_loose2_upc' : 	{  10 : TriggerRule( rate=8, propagate=False, comment='main UPC trigger',)},
         'EF_L1ZDC_OR_mv_loose2_upc' : 	        {  10 : TriggerRule( PS=-1, comment='UPC supporting trigger',)},
         # L1_ZDC rate not predicted reliably, rates/PS adjusted at L1
         'EF_L1ZDC_OR_mv_medium2_upc' : 	{  10 : TriggerRule( PS=1, maxRate=10, comment='UPC supporting trigger',),
                                                   300 : TriggerRule( PS=1, comment='UPC supporting trigger',)},
                                                   #10 : TriggerRule( rate=5, maxRate=10, comment='UPC supporting trigger',),
                                                   #300 : TriggerRule( rate=5, comment='UPC supporting trigger',)},

         'EF_L1MU0_VTE50_loose_upc': 	        {  10: TriggerRule( PS=1,  comment='primary upc',)},
         'EF_L1MU4_VTE50_loose_upc': 	        {  10: TriggerRule( PS=1,  comment='primary upc',)},
         'EF_L1EM3_VTE50_loose_upc': 	        {  10: TriggerRule( PS=1,  comment='primary upc',)},
         'EF_L1EM3_VTE50_upc': 	                {  10: TriggerRule( PS=-1, comment='tighter upc',)},

         'EF_L1MU0_MV_VTE50_loose_upc': 	{  10: TriggerRule( PS=1,    comment='upc',)},
         'EF_L1EM3_MV_VTE50_loose_upc': 	{  10: TriggerRule( PS=1,    comment='bkp upc',)},
         'EF_L1MU4_MV_VTE50_loose_upc': 	{  10: TriggerRule( PS=1,   comment='test upc',)},        
         'EF_L1EM3_MV_VTE50_upc': 	        {  10: TriggerRule( PS=-1,    comment='bkp upc',)},
         

         #pA triggers
         #this is new did not do anything about them yet
         'EF_L1ItemStreamer_L1_ZDC_UNPAIRED_NONISO': 	 { 100: TriggerRule( PS=-1, comment='pA primary trigger',)},
         'EF_L1ItemStreamer_L1_MBTS_1_EMPTY': 	         { 100: TriggerRule( PS=-1, comment='pA trigger',)},
         'EF_L1ItemStreamer_L1_MBTS_1_UNPAIRED_NONISO':  { 100: TriggerRule( PS=-1, comment='pA trigger',)},
         'EF_L1ItemStreamer_L1_MBTS_1_UNPAIRED_ISO': 	 { 100: TriggerRule( PS=-1, comment='pA trigger',)},
         'EF_L1ItemStreamer_L1_MBTS_2_UNPAIRED_NONISO':  { 100: TriggerRule( PS=-1, comment='pA trigger',)},
         'EF_L1ItemStreamer_L1_MBTS_2_UNPAIRED_ISO': 	 { 100: TriggerRule( PS=-1, comment='pA trigger',)},
         'EF_L1ItemStreamer_L1_MBTS_2_EMPTY': 	         { 100: TriggerRule( PS=-1, comment='pA trigger',)},
         'EF_L1ItemStreamer_L1_ZDC_MBTS_1': 	         { 100: TriggerRule( PS=-1, comment='pA trigger',)},
         'EF_L1ItemStreamer_L1_ZDC_MBTS_2':              { 100: TriggerRule( PS=-1, comment='pA trigger',)},
         'EF_L1ItemStreamer_L1_ZDC_MBTS_1_1':  	         { 100: TriggerRule( PS=-1, comment='pA trigger',)},
         'EF_L1ItemStreamer_L1_ZDC_MBTS_2_2':  	         { 100: TriggerRule( PS=-1, comment='pA trigger',)},
         'EF_L1ItemStreamer_L1_MBTS_1':        	         { 100: TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_mbSpTrk_L1ZDC': 	                         { 100: TriggerRule( PS=-1, ESRate=0.25, comment='pA primary trigger',)},
         'EF_mbSpTrk_L1MBTS_1': 	                 { 100: TriggerRule( PS=-1, comment='pA primary trigger',)},
         'EF_mbSpTrk_L1MBTS_2': 	                 { 100: TriggerRule( PS=-1, comment='pA primary trigger',)},
         'EF_mbSpTrk_L1MBTS_1_1': 	                 { 100: TriggerRule( PS=-1, comment='pA primary trigger',)},
         'EF_mbZdc_NoAlg': 	                         { 100: TriggerRule( PS=-1, comment='pA primary trigger',)},
         'EF_j15_a4hi_EFFS_L1TE10':	 	         {  10:  TriggerRule( PS=-1, comment='pPb triggers',)},
         'EF_j20_a4hi_EFFS_L1TE10':	 	         {  10:  TriggerRule( PS=-1, comment='pPb triggers',)},
         'EF_2mu2_MSonly_EFFS_L1MBTS':	 	         {  10:  TriggerRule( PS=-1, comment='pPb triggers',)},
         'EF_2mu2_MSonly_EFFS_L1ZDC_OR':	 	         {  10:  TriggerRule( PS=-1, comment='pPb triggers',)},
         'EF_mu4_MSonly_EFFS_L1MBTS':	 	         {  10:  TriggerRule( PS=-1, comment='pPb triggers',)},
         'EF_mu4_MSonly_EFFS_L1ZDC_OR':	 	         {  10:  TriggerRule( PS=-1, comment='pPb triggers',)},
         'EF_j15_a4hi_EFFS_L1MBTS':	 	         {  10:  TriggerRule( PS=-1, comment='pPb triggers',)},
         'EF_j15_a2hi_EFFS_L1MBTS':	 	         {  10:  TriggerRule( PS=-1, comment='pPb triggers',)},
         'EF_j20_a4hi_EFFS_L1MBTS':	 	         {  10:  TriggerRule( PS=-1, comment='pPb triggers',)},
         'EF_j20_a2hi_EFFS_L1MBTS':	 	         {  10:  TriggerRule( PS=-1, comment='pPb triggers',)},
         
         #VdM scan triggers
         #these are also new.
         'EF_L1ItemStreamer_L1_BGRP7': 	                 { 100: TriggerRule( PS=-1, comment='VdM scan trigger',)},
         'EF_L1ItemStreamer_L1_LUCID_BGRP7': 	         { 100: TriggerRule( PS=-1, comment='VdM scan trigger',)},
         'EF_L1ItemStreamer_L1_ZDC_A_C_BGRP7': 	         { 100: TriggerRule( PS=-1, comment='VdM scan trigger',)},
         'L2_VdM_BGRP7': 	                         { 100: TriggerRule( PS=-1, comment='VdM scan trigger',)},
         'L2_VdM_LUCID_BGRP7': 	                         { 100: TriggerRule( PS=-1, comment='VdM scan trigger',)},
         'L2_VdM_ZDC_A_C_BGRP7': 	                 { 100: TriggerRule( PS=-1, comment='VdM scan trigger',)},
     
         #MinBias
	 'EF_mbZdc_a_c_eff':		 	{  10: TriggerRule( rate=1, ESRate=0.25, comment='primary trigger',)}, 
	 'EF_mbZdc_a_c_NoAlg': 		 	{  10: TriggerRule( rate=5, ESRate=0.5, \
                                                                    comment='supporting trigger for eff measurements for early running',),\
                                                  40: TriggerRule( PS=-1, comment='primary trigger',)},
         'EF_mbZdc_a_c_overlay_NoAlg': 		{  10: TriggerRule( rate=5, comment='primary trigger',)},
         'EF_fte75_veto': 		 	{  10: TriggerRule( PS=-1, comment='centrality trigger',)},
         'EF_fte75_veto_mbMbts_2_2':  	 	{  10: TriggerRule( PS=-1, comment='centrality backup trigger',)},
         'EF_centrality100_60':        	 	{  10: TriggerRule( PS=0, rerun=True, comment='centrality trigger',)},
         'EF_centrality80_60':        	 	{  10: TriggerRule( PS=0, rerun=True, comment='centrality trigger',)},
         'EF_mbSpTrk': 		        	{  10: TriggerRule( PS=5, ESValue=3, comment='supporting trigger',)},

         
         'EF_mbSpTrk_unpaired_iso': 		{  10: TriggerRule( PS=1, comment='supporting trigger',)},
         'L2_mbRd1_eff' :                 	{ 10 : TriggerRule( PS=1, comment='supporting trigger',)},
         'EF_mbRd1_eff' :                 	{ 10 : TriggerRule( PS=1, comment='supporting trigger',)},
         'EF_mbRd0_eff_unpaired_iso' : 	        { 10 : TriggerRule( rate=1, comment='primary trigger',)},
         #'EF_rd0_empty_NoAlg' : 	        { 10 : TriggerRule( rate=1, ESRate=1, comment='primary trigger',)},
         'EF_rd0_empty_NoAlg' : 	        { 10 : TriggerRule( PS=85, ESRate=1, comment='primary trigger',)}, 


         'EF_L1TE10_NoAlg' :                    {  10: TriggerRule( PS=-1, comment='min bias trigger',)},
         'EF_L1TE20_NoAlg' : 	                {  10: TriggerRule( PS=-1, comment='min bias trigger',)},
         'EF_L1MBTS_2_NoAlg' : 	                {  10: TriggerRule( PS=-1, comment='min bias trigger',)},
         
         'EF_mbSpBg_unpaired_iso' : 	         { 10 : TriggerRule( rate=0.5, comment='primary trigger',)}, 
 	 'EF_mbZdc_and_NoAlg' :          	 { 10 : TriggerRule( rate=2, comment='backup trigger',),\
                                                   40 : TriggerRule( PS=-1, comment='backup trigger',)},
         'EF_mbZdc_and_eff' :           	 { 30 : TriggerRule( PS=-1, comment='backup trigger',)},
         'EF_mbZdc_and_Mbts_1_1' : 	         { 100 : TriggerRule( PS=-1, comment='backup trigger',)},
         'EF_mbZdc_and_Mbts_2_2' : 	         { 100 : TriggerRule( PS=-1, comment='backup trigger',)},
         	 
         'EF_mbLucid_a_c_eff' :            	 { 10 : TriggerRule( PS=1, comment='backup trigger',),\
                                                   40 : TriggerRule( PS=-1, comment='backup trigger',)},
         'EF_mbLucid_a_c_eff_unpaired_iso' : 	 { 100 : TriggerRule( PS=-1, comment='backup trigger',)}, 
         'EF_mbZdc_a_c_Mbts_1_1' :            	 { 100 : TriggerRule( PS=-1, comment='backup trigger',)},
         'EF_mbZdc_a_c_Mbts_2_2' :            	 { 100 : TriggerRule( PS=-1, comment='backup trigger',)},

         'EF_mbMbts_1_1_unpaired_iso' : 	 { 100 : TriggerRule( PS=-1, comment='backup min bias trigger',)},
         'EF_mbMbts_1_1_eff_unpaired_iso' : 	 { 100 : TriggerRule( PS=-1, comment='backup min bias trigger',)},
         'EF_mbMbts_1_1' :              	 { 10 : TriggerRule( rate=2, comment='backup min bias trigger',),\
                                                   40 : TriggerRule( PS=-1, comment='backup min bias trigger',)},
         'EF_mbMbts_1_1_eff' :              	 { 100 : TriggerRule( PS=-1, comment='backup min bias trigger',)},
         
         'EF_mbMbts_2_2_unpaired_iso' : 	 { 100 : TriggerRule( PS=-1, comment='backup min bias trigger',)},
         'EF_mbMbts_2_2_eff_unpaired_iso' : 	 { 100 : TriggerRule( PS=-1, comment='backup min bias trigger',)},
         'EF_mbMbts_2_2' :              	 { 10 : TriggerRule(  PS=-1, comment='backup min bias trigger',),\
                                                   40 : TriggerRule(  PS=-1, comment='backup min bias trigger',)},
         'EF_mbMbts_2_2_eff' :              	 { 100 : TriggerRule( PS=-1, comment='backup min bias trigger',)},

         'EF_mbMbts_3_3_unpaired_iso' : 	 { 100 : TriggerRule( PS=-1, comment='backup min bias trigger',)},
         'EF_mbMbts_3_3_eff_unpaired_iso' : 	 { 100 : TriggerRule( PS=-1, comment='backup min bias trigger',)},
         'EF_mbMbts_3_3' :              	 { 100 : TriggerRule( PS=-1, comment='backup min bias trigger',)},
         'EF_mbMbts_3_3_eff' :              	 { 100 : TriggerRule( PS=-1, comment='backup min bias trigger',)},
         
         'EF_mbMbts_4_4_unpaired_iso' : 	 { 100 : TriggerRule( PS=-1, comment='backup min bias trigger',)},
         'EF_mbMbts_4_4_eff_unpaired_iso' : 	 { 100 : TriggerRule( PS=-1, comment='backup min bias trigger',)},
         'EF_mbMbts_4_4' :              	 { 100 : TriggerRule( PS=-1, comment='backup min bias trigger',)},
         'EF_mbMbts_4_4_eff' :              	 { 100 : TriggerRule( PS=-1, comment='backup min bias trigger',)},
         
         
         'EF_mbMbts_1_1_L1VTE50_trk' : 	        {  10: TriggerRule( PS=-1, comment='min bias backup trigger',)},
         'EF_mbMbts_2_2_L1VTE50_trk' : 	        {  10: TriggerRule( PS=-1, comment='min bias backup trigger',)},

         'EF_mbZdc_a_c_L1TE50_NoAlg': 		{  10:  TriggerRule( PS=-1,  comment='backup trigger',)},
         
	 #'EF_mbZdc_a_c_L1TE50_NoAlg': 		{  10:  TriggerRule( PS=1,  comment='primary trigger',),\
         #                                          15:  TriggerRule( PS=1.5,  comment='primary trigger',),\
         #                                          20:  TriggerRule( PS=2,  comment='primary trigger',),\
         #                                          30:  TriggerRule( PS=3,  comment='primary trigger',),\
         #                                          40:  TriggerRule( PS=4,  comment='primary trigger',),\
         #                                          60:  TriggerRule( PS=6,  comment='primary trigger',),\
         #                                          80:  TriggerRule( PS=8,  comment='primary trigger',),\
         #                                          120: TriggerRule( PS=12, comment='primary trigger',),\
         #                                          160: TriggerRule( PS=16, comment='primary trigger',),\
         #                                          200: TriggerRule( PS=20, comment='primary trigger',),\
         #                                          250: TriggerRule( PS=25, comment='primary trigger',),\
         #                                          300: TriggerRule( PS=30, comment='primary trigger',),\
         #                                          350: TriggerRule( PS=35, comment='primary trigger',),\
         #                                          400: TriggerRule( PS=40, comment='primary trigger',)},
         
         'EF_L1TE50_NoAlg' : 	                {  10:  TriggerRule( PS=1,  ESRate=0.5, comment='primary trigger',),\
                                                   15:  TriggerRule( PS=1.5, ESRate=0.5, comment='primary trigger',),\
                                                   20:  TriggerRule( PS=2, ESRate=0.5,  comment='primary trigger',),\
                                                   30:  TriggerRule( PS=3, ESRate=0.5,  comment='primary trigger',),\
                                                   40:  TriggerRule( PS=4, ESRate=0.5,  comment='primary trigger',),\
                                                   60:  TriggerRule( PS=6, ESRate=0.5,  comment='primary trigger',),\
                                                   80:  TriggerRule( PS=8, ESRate=0.5,  comment='primary trigger',),\
                                                   120: TriggerRule( PS=12, ESRate=0.5,  comment='primary trigger',),\
                                                   160: TriggerRule( PS=16, ESRate=0.5, comment='primary trigger',),\
                                                   200: TriggerRule( PS=20, ESRate=0.5, comment='primary trigger',),\
                                                   250: TriggerRule( PS=25, ESRate=0.5, comment='primary trigger',),\
                                                   300: TriggerRule( PS=30, ESRate=0.5, comment='primary trigger',),\
                                                   350: TriggerRule( PS=35, ESRate=0.5, comment='primary trigger',),\
                                                   400: TriggerRule( PS=40, ESRate=0.5, comment='primary trigger',),\
                                                   450: TriggerRule( PS=45, ESRate=0.5, comment='primary trigger',),\
                                                   500: TriggerRule( PS=50, ESRate=0.5, comment='primary trigger',),\
                                                   550: TriggerRule( PS=55, ESRate=0.5, comment='primary trigger',),\
                                                   600: TriggerRule( PS=60, ESRate=0.5, comment='primary trigger',),\
                                                   650: TriggerRule( PS=65, ESRate=0.5, comment='primary trigger',),\
                                                   700: TriggerRule( PS=70, ESRate=0.5, comment='primary trigger',),\
                                                   750: TriggerRule( PS=75, ESRate=0.5, comment='primary trigger',),\
                                                   800: TriggerRule( PS=80, ESRate=0.5, comment='primary trigger',)},

#	 'EF_mbZdc_a_c_L1VTE50_trk':	 	{  10:  TriggerRule( PS=1, ESRate=0.5, rerun=True, propagate=False, comment='primary trigger',),\
#                                                   15:  TriggerRule( PS=1.5, ESRate=0.5, rerun=True, propagate=False, comment='primary trigger',),\
#                                                   20:  TriggerRule( PS=2, ESRate=0.5, rerun=True, propagate=False, comment='primary trigger',),\
#                                                   30:  TriggerRule( PS=3, ESRate=0.5, rerun=True, propagate=False, comment='primary trigger',),\
#                                                   40:  TriggerRule( PS=4,ESRate=0.5, rerun=True,  propagate=False, comment='primary trigger',),\
#                                                   60:  TriggerRule( PS=6, ESRate=0.5, rerun=True, propagate=False, comment='primary trigger',),\
#                                                   80:  TriggerRule( PS=8, ESRate=0.5, rerun=True, propagate=False, comment='primary trigger',),\
#                                                   120: TriggerRule( PS=12, ESRate=0.5, rerun=True, propagate=False, comment='primary trigger',),\
#                                                   160: TriggerRule( PS=16, ESRate=0.5, rerun=True, propagate=False, comment='primary trigger',),\
#                                                   200: TriggerRule( PS=20, ESRate=0.5, rerun=True, propagate=False, comment='primary trigger',),\
#                                                   250: TriggerRule( PS=25, ESRate=0.5, rerun=True, propagate=False, comment='primary trigger',),\
#                                                   300: TriggerRule( PS=30, ESRate=0.5, rerun=True, propagate=False, comment='primary trigger',),\
#                                                   350: TriggerRule( PS=35, ESRate=0.5, rerun=True, propagate=False, comment='primary trigger',),\
#                                                   400: TriggerRule( PS=40, ESRate=0.5, rerun=True, propagate=False, comment='primary trigger',)},

	 'EF_mbZdc_a_c_L1VTE50_trk':	 	{  10:  TriggerRule( PS=1, ESRate=0.5, rerun=True, comment='primary trigger',),\
                                                   15:  TriggerRule( PS=1.5, ESRate=0.5, rerun=True, comment='primary trigger',),\
                                                   20:  TriggerRule( PS=2, ESRate=0.5, rerun=True, comment='primary trigger',),\
                                                   30:  TriggerRule( PS=3, ESRate=0.5, rerun=True, comment='primary trigger',),\
                                                   40:  TriggerRule( PS=4,ESRate=0.5, rerun=True,  comment='primary trigger',),\
                                                   60:  TriggerRule( PS=6, ESRate=0.5, rerun=True, comment='primary trigger',),\
                                                   80:  TriggerRule( PS=8, ESRate=0.5, rerun=True, comment='primary trigger',),\
                                                   120: TriggerRule( PS=12, ESRate=0.5, rerun=True, comment='primary trigger',),\
                                                   160: TriggerRule( PS=16, ESRate=0.5, rerun=True, comment='primary trigger',),\
                                                   200: TriggerRule( PS=20, ESRate=0.5, rerun=True, comment='primary trigger',),\
                                                   250: TriggerRule( PS=25, ESRate=0.5, rerun=True, comment='primary trigger',),\
                                                   300: TriggerRule( PS=30, ESRate=0.5, rerun=True, comment='primary trigger',),\
                                                   350: TriggerRule( PS=35, ESRate=0.5, rerun=True, comment='primary trigger',),\
                                                   400: TriggerRule( PS=40, ESRate=0.5, rerun=True, comment='primary trigger',),\
                                                   450: TriggerRule( PS=45, ESRate=0.5, rerun=True, comment='primary trigger',),\
                                                   500: TriggerRule( PS=50, ESRate=0.5, rerun=True, comment='primary trigger',),\
                                                   550: TriggerRule( PS=55, ESRate=0.5, rerun=True, comment='primary trigger',),\
                                                   600: TriggerRule( PS=60, ESRate=0.5, rerun=True, comment='primary trigger',),\
                                                   650: TriggerRule( PS=65, ESRate=0.5, rerun=True, comment='primary trigger',),\
                                                   700: TriggerRule( PS=70, ESRate=0.5, rerun=True, comment='primary trigger',),\
                                                   750: TriggerRule( PS=75, ESRate=0.5, rerun=True, comment='primary trigger',),\
                                                   800: TriggerRule( PS=80, ESRate=0.5, rerun=True, comment='primary trigger',)},


         'EF_mbTe10_L2Zdc_eff' : 	 { 100 : TriggerRule( PS=-1, comment='test trigger',)},
         'EF_mbZdc_a_c_L2Zdc_eff' : 	 { 100 : TriggerRule( PS=-1, comment='test trigger',)},
         
         }

