# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RuleClasses import TriggerRule
import Monitoring_pp_v4_rules

tag = 'ALFA_v4'

AlfaFinal=True

rules = {

  ######## vdM rules
  ### same for safe and final!!
  'L1_BGRP7'                          : {1000 : TriggerRule(PS=21), },  
  'L1_RD0_UNPAIRED_ISO'               : {1000 : TriggerRule(PS=41), },  
  'L1_MBTS_2_BGRP7'                   : {1000 : TriggerRule(PS=1), },  
  'L2_VdM_BGRP7'                      : {1000 : TriggerRule(PS=1), },  
  'L2_VdM_RD0_UNPAIRED_ISO'           : {1000 : TriggerRule(PS=1), },  
  'L2_VdM_MBTS_2_BGRP7'               : {1000 : TriggerRule(PS=1), },  
  'L2_VdM_MBTS_2_UNPAIRED_ISO'        : {1000 : TriggerRule(PS=1), },  

  ###### MinBias
  'EF_mbMbts_1_A_ALFA_C_NoAlg'                : {1000 : TriggerRule(PS = -1), },
  'EF_mbMbts_1_C_ALFA_A_NoAlg'                : {1000 : TriggerRule(PS = -1), },
  'EF_mbMbts_1_A_ALFA_C_UNPAIRED_ISO_NoAlg'   : {1000 : TriggerRule(PS = -1), },
  'EF_mbMbts_1_C_ALFA_A_UNPAIRED_ISO_NoAlg'   : {1000 : TriggerRule(PS = -1), },

  # 17.1.5.20
  'L1_MBTS_2_UNPAIRED_ISO'           : {1000 : TriggerRule(PS = 1), }, 
  'L1_MBTS_2'                        : {1000 : TriggerRule(PS = 10), }, 
  'L2_mbMbts_2_NoAlg'                : {1000 : TriggerRule(PS = 1), }, 
  'EF_mbMbts_2_NoAlg'                : {1000 : TriggerRule(PS = 1,ESValue=10), },
  'L2_mbMbts_2_unpaired_iso_NoAlg'   : {1000 : TriggerRule(PS = 1), }, 
  'EF_mbMbts_2_unpaired_iso_NoAlg' : {1000 : TriggerRule(PS = 1), },

  ###### ALFA items!!
  ## some different  in safe/final
  'L2_ALFA_Phys'                     : {1000 : TriggerRule(PS = 1), },  
  'EF_ALFA_Phys'                     : {1000 : TriggerRule(PS = 1), }, 

  'L2_ALFA_Phys_Any'                 : {1000 : TriggerRule(PS = 100), }, 
  'EF_ALFA_Phys_Any'                 : {1000 : TriggerRule(PS = 1), }, 

  'L2_ALFA_Calib'                    : {1000 : TriggerRule(PS = 1), }, 
  
  'L1_ALFA_A7L1L'                    : {1000 : TriggerRule(PS = -1), }, 
  'L1_ALFA_A7L1L_OD'                 : {1000 : TriggerRule(PS = -1), }, 
  'L1_ALFA_A7L1U'                    : {1000 : TriggerRule(PS = 1), }, 
  'L1_ALFA_A7L1U_OD'                 : {1000 : TriggerRule(PS = -1), }, 
  'L1_ALFA_A7L1_OD'                  : {1000 : TriggerRule(PS = 1), }, 
  'L1_ALFA_A7R1L'                    : {1000 : TriggerRule(PS = -1), }, 
  'L1_ALFA_A7R1L_OD'                 : {1000 : TriggerRule(PS = -1), }, 
  'L1_ALFA_A7R1U'                    : {1000 : TriggerRule(PS =  -1), }, 
  'L1_ALFA_A7R1U_OD'                 : {1000 : TriggerRule(PS = -1), }, 
  'L1_ALFA_A7R1_OD'                  : {1000 : TriggerRule(PS =  1), }, 
  'L1_ALFA_ANY'                      : {1000 : TriggerRule(PS = 4), }, 
  'L1_ALFA_ANY_EMPTY'                : {1000 : TriggerRule(PS = 4), }, 
  'L1_ALFA_ANY_FIRSTEMPTY'           : {1000 : TriggerRule(PS = 4), }, 
  'L1_ALFA_ANY_UNPAIRED_ISO'         : {1000 : TriggerRule(PS = 4), }, 
  'L1_ALFA_ANY_UNPAIRED_NONISO'      : {1000 : TriggerRule(PS = 4), }, 
  'L1_ALFA_B1_EMPTY'                 : {1000 : TriggerRule(PS = -1), }, 
  'L1_ALFA_B2_EMPTY'                 : {1000 : TriggerRule(PS = -1), }, 
  'L1_ALFA_B7L1L'                    : {1000 : TriggerRule(PS =  -1), }, 
  'L1_ALFA_B7L1L_OD'                 : {1000 : TriggerRule(PS = -1), }, 
  'L1_ALFA_B7L1U'                    : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_B7L1U_OD'                 : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_B7L1_OD'                  : {1000 : TriggerRule(PS = 1), },  
  'L1_ALFA_B7R1L'                    : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_B7R1L_OD'                 : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_B7R1U'                    : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_B7R1U_OD'                 : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_B7R1_OD'                  : {1000 : TriggerRule(PS = 1), },  
  'L1_ALFA_BGRP7'                    : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_BGRP7_OD'                 : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_ELAST1'                   : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_ELAST11'                  : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_ELAST12'                  : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_ELAST13'                  : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_ELAST14'                  : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_ELAST15'                  : {1000 : TriggerRule(PS = 1), },  
  'L1_ALFA_ELAST16'                  : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_ELAST17'                  : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_ELAST18'                  : {1000 : TriggerRule(PS = 1), },  
  'L1_ALFA_ELAST2'                   : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_EMPTY'                    : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_EMPTY_OD'                 : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_SDIFF1'                   : {1000 : TriggerRule(PS = 1), },  
  'L1_ALFA_SDIFF2'                   : {1000 : TriggerRule(PS = 1), },  
  'L1_ALFA_SDIFF3'                   : {1000 : TriggerRule(PS = 1), },  
  'L1_ALFA_SDIFF4'                   : {1000 : TriggerRule(PS = 1), },  
  'L1_ALFA_SDIFF5'                   : {1000 : TriggerRule(PS = 1000), },  
  'L1_ALFA_SDIFF6'                   : {1000 : TriggerRule(PS = 1000), },  
  'L1_ALFA_SDIFF7'                   : {1000 : TriggerRule(PS = 1000), },  
  'L1_ALFA_SDIFF8'                   : {1000 : TriggerRule(PS = 1000), },  
  'L1_ALFA_SHOW1'                    : {1000 : TriggerRule(PS =  -1), },  
  'L1_ALFA_SHOW2'                    : {1000 : TriggerRule(PS =  -1), },  
  'L1_ALFA_SHOW3'                    : {1000 : TriggerRule(PS =  -1), },  
  'L1_ALFA_SHOW4'                    : {1000 : TriggerRule(PS =  -1), },  
  'L1_ALFA_SHOWSYST1'                : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_SHOWSYST2'                : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_SHOWSYST3'                : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_SHOWSYST4'                : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_SHOWSYST5'                : {1000 : TriggerRule(PS =  1), },  
  'L1_ALFA_SYST1'                    : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_SYST10'                   : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_SYST11'                   : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_SYST12'                   : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_SYST17'                   : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_SYST18'                   : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_SYST2'                    : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_SYST3'                    : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_SYST4'                    : {1000 : TriggerRule(PS = -1), },  
  'L1_ALFA_SYST9'                    : {1000 : TriggerRule(PS = -1), },  
  'L1_BGRP1_ALFA_BGT'                : {1000 : TriggerRule(PS = 20), },  
  'L1_BGRP4_ALFA_BGT'                : {1000 : TriggerRule(PS = 20), },  
  'L1_BGRP7_ALFA_BGT'                : {1000 : TriggerRule(PS = -1), },  
  'L1_LUCID_ALFA'                    : {1000 : TriggerRule(PS = -1), },  
  'L1_LUCID_A_ALFA_C'                : {1000 : TriggerRule(PS = 50), },  
  'L1_LUCID_C_ALFA_A'                : {1000 : TriggerRule(PS = 50), },  
  'L1_MBTS_2_ALFA'                   : {1000 : TriggerRule(PS = -1), },  
  'L1_MBTS_2_A_ALFA_C'               : {1000 : TriggerRule(PS = 50), },  
  'L1_MBTS_2_C_ALFA_A'               : {1000 : TriggerRule(PS = 50), },  
  'L1_ZDC_ALFA'                      : {1000 : TriggerRule(PS =  -1), },  
  'L1_ZDC_A_ALFA_C'                  : {1000 : TriggerRule(PS = -1), },  
  'L1_ZDC_C_ALFA_A'                  : {1000 : TriggerRule(PS = -1), },  
  'L1_MBTS_1_A_ALFA_C'               : {1000 : TriggerRule(PS = -1), },  
  'L1_MBTS_1_C_ALFA_A'               : {1000 : TriggerRule(PS = -1), },  
  'L1_MBTS_2_ALFA_UNPAIRED_ISO'      : {1000 : TriggerRule(PS = -1 ), }, 
  'L1_LUCID_A_ALFA_C_UNPAIRED_ISO'   : {1000 : TriggerRule(PS = 1), },  
  'L1_LUCID_C_ALFA_A_UNPAIRED_ISO'   : {1000 : TriggerRule(PS = 1), },  
  'L1_MBTS_1_A_ALFA_C_UNPAIRED_ISO'  : {1000 : TriggerRule(PS = -1), },  
  'L1_MBTS_1_C_ALFA_A_UNPAIRED_ISO'  : {1000 : TriggerRule(PS = 1), },  
  'L1_MBTS_2_A_ALFA_C_UNPAIRED_ISO'  : {1000 : TriggerRule(PS = 1), },  	 
  'L1_MBTS_2_C_ALFA_A_UNPAIRED_ISO'  : {1000 : TriggerRule(PS = 1), }, 



 
  ##### Modifications for monitoring
  'L1_J10_FIRSTEMPTY' : {1000 : TriggerRule(PS=370), },
  'EF_fj15_a4tchad' : {1000 : TriggerRule(inputRate = 2000), },
  'EF_j15_a4tchad'  : {1000 : TriggerRule(inputRate = 2000), },
  'EF_fj25_a4tchad' : {1000 : TriggerRule(inputRate = 2000), },
  'EF_j25_a4tchad'  : {1000 : TriggerRule(inputRate = 2000), },
  'EF_rd0_filled_NoAlg' : {1000 : TriggerRule(inputRate = 10), },



  ######## background rules
  ### same for safe and final
  'L2_Background'                          : {1000 : TriggerRule(PS=1), },  
  'EF_Background'                          : {1000 : TriggerRule(PS=1), },  
  'L2_Background_RD0_UNPAIRED_ISO'         : {1000 : TriggerRule(PS=-1), },  
  'EF_Background_RD0_UNPAIRED_ISO'         : {1000 : TriggerRule(PS=-1), },  
  'L2_Background_RD0_EMPTY'                : {1000 : TriggerRule(PS=-1), },  
  'EF_Background_RD0_EMPTY'                : {1000 : TriggerRule(PS=-1), },  

  'L1_RD0_EMPTY'                           : {10000: TriggerRule(PS=1400*40*1.1*10,comment='set to 10 Hz with 3400 empty bunches ')},
  #was for 350 =>so factor of 10 more empty bunches => factor of 10 higher prescale

  #'L1_RD0_EMPTY'         : {1000 : TriggerRule(PS=-1), }, #??
  #'L2_rd0_empty_NoAlg'    : {1000 : TriggerRule(PS=-1), }, #??
  #'EF_rd0_empty_NoAlg'    : {1000 : TriggerRule(PS=-1), }, #??

  'L1_BCM_AC_CA_BGRP0'                : {1000 : TriggerRule(PS=10), },   
  'L1_BCM_AC_CA_UNPAIRED_ISO'         : {1000 : TriggerRule(PS=1), },   
  'L1_BCM_Wide_EMPTY'                 : {1000 : TriggerRule(PS=10), },   
  'L1_BCM_Wide_UNPAIRED_ISO'          : {1000 : TriggerRule(PS=1), },   
  'L1_BCM_Wide_UNPAIRED_NONISO'       : {1000 : TriggerRule(PS=1), },   
  'L1_EM3_UNPAIRED_ISO'               : {1000 : TriggerRule(PS=-1), },   
  'L1_FJ30_UNPAIRED_ISO'              : {1000 : TriggerRule(PS=-1), },   
  'L1_J10'                            : {1000 : TriggerRule(PS=1), },   
  'L1_J10_UNPAIRED_ISO'               : {1000 : TriggerRule(PS=1), },   
  'L1_J10_UNPAIRED_NONISO'            : {1000 : TriggerRule(PS=1), },   
  'L1_LUCID_A_C_EMPTY'                : {1000 : TriggerRule(PS=100000), },   
  'L1_LUCID_A_C_UNPAIRED_ISO'         : {1000 : TriggerRule(PS=1000), },   
  'L1_LUCID_A_C_UNPAIRED_NONISO'      : {1000 : TriggerRule(PS=-1), },   
  'L1_LUCID_COMM_EMPTY'               : {1000 : TriggerRule(PS=-1), },   
  'L1_LUCID_COMM_UNPAIRED_ISO'        : {1000 : TriggerRule(PS=-1), },   
  'L1_LUCID_EMPTY'                    : {1000 : TriggerRule(PS=1000000), },   
  'L1_LUCID_UNPAIRED_ISO'             : {1000 : TriggerRule(PS=10000), },   
  


  ####### Jet and Spacepoint (ALFA seeded)
  ## for final all PS set to 1!!
  'L1_EM3_ALFA_ANY'                          : {1000 : TriggerRule(PS=1), },    
  'L1_EM3_ALFA_ANY_UNPAIRED_ISO'             : {1000 : TriggerRule(PS=1), },    
  'L2_e5_tight1_ALFA_ANY'                    : {1000 : TriggerRule(PS=100), },    
  'EF_e5_tight1_ALFA_ANY'                    : {1000 : TriggerRule(PS=1), },    
  'L2_e5_tight1_ALFA_ANY_UNPAIREDISO'        : {1000 : TriggerRule(PS=1), },    
  'EF_e5_tight1_ALFA_ANY_UNPAIREDISO'        : {1000 : TriggerRule(PS=1), },    
  'L2_mbSpTrk_ALFA_any'                      : {1000 : TriggerRule(PS=1000), },    
  'EF_mbSpTrk_ALFA_any'                      : {1000 : TriggerRule(PS=1), },    
  'L2_alfa_any_NoAlg'                        : {1000 : TriggerRule(PS=1000), },    
  'EF_j10_a4tchadloose_L1ALFAANY'            : {1000 : TriggerRule(PS=1), },    
  'EF_j15_a4tchad_L1ALFAANY'                 : {1000 : TriggerRule(PS=1), },    
  'EF_j25_a4tchad_L1ALFAANY'                 : {1000 : TriggerRule(PS=1), },    
  'EF_j35_a4tchad_L1ALFAANY'                 : {1000 : TriggerRule(PS=1), },    
  'L2_alfa_sdiff_NoAlg'                      : {1000 : TriggerRule(PS=100), },    
  'EF_j10_a4tchadloose_L1ALFASDIFF'          : {1000 : TriggerRule(PS=1), },    
  'EF_j15_a4tchad_L1ALFASDIFF'               : {1000 : TriggerRule(PS=1), },    
  'EF_j25_a4tchad_L1ALFASDIFF'               : {1000 : TriggerRule(PS=1), },    
  'EF_j35_a4tchad_L1ALFASDIFF'               : {1000 : TriggerRule(PS=1), },    
  'L1_EM3_ALFA_MBTS_A'                       : {1000 : TriggerRule(PS=100), },    
  'L1_EM3_ALFA_MBTS_A_UNPAIRED_ISO'          : {1000 : TriggerRule(PS=1), },    
  'L1_EM3_ALFA_MBTS_C'                       : {1000 : TriggerRule(PS=100), },    
  'L1_EM3_ALFA_MBTS_C_UNPAIRED_ISO'          : {1000 : TriggerRule(PS=1), },    
  'L2_e5_tight1_ALFA_MBTS_A'                 : {1000 : TriggerRule(PS=100), },    
  'L2_e5_tight1_ALFA_MBTS_A_UNPAIREDISO'     : {1000 : TriggerRule(PS=1), },    
  'L2_e5_tight1_ALFA_MBTS_C'                 : {1000 : TriggerRule(PS=100), },    
  'L2_e5_tight1_ALFA_MBTS_C_UNPAIREDISO'     : {1000 : TriggerRule(PS=1), },    
  'EF_e5_tight1_ALFA_MBTS_A'                 : {1000 : TriggerRule(PS=1), },    
  'EF_e5_tight1_ALFA_MBTS_A_UNPAIREDISO'     : {1000 : TriggerRule(PS=1), },    
  'EF_e5_tight1_ALFA_MBTS_C'                 : {1000 : TriggerRule(PS=1), },    
  'EF_e5_tight1_ALFA_MBTS_C_UNPAIREDISO'     : {1000 : TriggerRule(PS=1), },



  ####triggers not used
  'L2_mbMbts_1_unpaired_iso_NoAlg'    : {1000 : TriggerRule(PS=-1), },
  'EF_mbMbts_1_unpaired_iso_NoAlg'    : {1000 : TriggerRule(PS=-1), },
  'L2_ROBRequestMon'    : {1000 : TriggerRule(PS=-1), },
  'EF_ROBRequestMon'    : {1000 : TriggerRule(PS=-1), },

  'EF_e24vh_medium1'    : {1000 : TriggerRule(PS=-1), },
  'EF_e24vh_tight1_e15_NoCut_Zee'    : {1000 : TriggerRule(PS=-1), },
  'EF_mbLucid_eff'    : {1000 : TriggerRule(PS=-1), },
  'EF_g12Tvh_loose'    : {1000 : TriggerRule(PS=-1), },
  'EF_g12Tvh_loose_larcalib'    : {1000 : TriggerRule(PS=-1), },
  'EF_mbZdc_eff'    : {1000 : TriggerRule(PS=-1), },
  'EF_j10_a4tchadloose_L1MBTS'    : {1000 : TriggerRule(PS=-1), },
  'EF_j15_a4tchad_L1MBTS'    : {1000 : TriggerRule(PS=-1), },
  'EF_j25_a4tchad_L1MBTS'    : {1000 : TriggerRule(PS=-1), },
  'EF_j35_a4tchad_L1MBTS'    : {1000 : TriggerRule(PS=-1), },
  'EF_mbMbts_1_NoAlg'    : {1000 : TriggerRule(PS=-1), },
  'EF_L1TRT_NoAlg'    : {1000 : TriggerRule(PS=-1), },
  'EF_Cosmics_MU11_TRTxK_TRTTrkHypo_AllID'    : {1000 : TriggerRule(PS=-1), },
  'EF_Cosmics_MU11_TRTxK_TRTTrkHypo_Pixel'    : {1000 : TriggerRule(PS=-1), },
  'EF_xe80'    : {1000 : TriggerRule(PS=-1), },
  'EF_mbRd1_eff'    : {1000 : TriggerRule(PS=-1), },
  'EF_b55_NoCut_j55_a4tchad'    : {1000 : TriggerRule(PS=-1), },
  'EF_mbSpTrk_unpaired_iso'    : {1000 : TriggerRule(PS=-1), },
  'EF_g40_loose'    : {1000 : TriggerRule(PS=-1), },
  'EF_g40_loose_larcalib'    : {1000 : TriggerRule(PS=-1), },
  'EF_g60_loose'    : {1000 : TriggerRule(PS=-1), },
  'EF_g60_loose_larcalib'    : {1000 : TriggerRule(PS=-1), },
  'EF_g80_loose'    : {1000 : TriggerRule(PS=-1), },
  'EF_g80_loose_larcalib'    : {1000 : TriggerRule(PS=-1), },
  'EF_zerobias_NoAlg'    : {1000 : TriggerRule(PS=-1), },
  'EF_xe45_LArNoiseBurst'    : {1000 : TriggerRule(PS=-1), },
  'EF_CosmicsAllTeEFID'    : {1000 : TriggerRule(PS=-1), },
  'EF_CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo'    : {1000 : TriggerRule(PS=-1), },
  'EF_CosmicsAllTeIDSCAN_AllPhysics_TrkHypo'    : {1000 : TriggerRule(PS=-1), },
  'EF_CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo'    : {1000 : TriggerRule(PS=-1), },
  'EF_CosmicsAllTeSiTrack_AllPhysics_TrkHypo'    : {1000 : TriggerRule(PS=-1), },
  'EF_CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics'    : {1000 : TriggerRule(PS=-1), },
  'EF_CosmicsAllTeTRTxK_TRTTrkHypo_Pixel'    : {1000 : TriggerRule(PS=-1), },
  'EF_ROBRequestMonUnseeded'    : {1000 : TriggerRule(PS=-1), },
  'L2_cost_monitor'    : {1000 : TriggerRule(PS=-1), },
  'EF_cost_monitor'    : {1000 : TriggerRule(PS=-1), },
  'EF_e5_tight1'    : {1000 : TriggerRule(PS=-1), },
  'EF_j80_u0uchad_LArNoiseBurstT'    : {1000 : TriggerRule(PS=-1), },
  'EF_L1Calo_NoAlg'    : {1000 : TriggerRule(PS=-1), },
  'EF_L1MinBias_NoAlg'    : {1000 : TriggerRule(PS=-1), },
  'EF_mbSpTrk_ALFA_SDIFF'    : {1000 : TriggerRule(PS=-1), },
  'EF_xe30'    : {1000 : TriggerRule(PS=-1), },
  'EF_j35_u0uchad_empty_LArNoiseBurst'    : {1000 : TriggerRule(PS=-1), },
  'EF_mb_BCM_Wide_eff'    : {1000 : TriggerRule(PS=-1), },
  'EF_mbSpTrkVtxMh'    : {1000 : TriggerRule(PS=-1), },
  'EF_mbSpTrkVtxMh_medium'    : {1000 : TriggerRule(PS=-1), },
  'EF_mbSpTrkVtxMh_tight'    : {1000 : TriggerRule(PS=-1), },
  'EF_fj55_a4tchad_L2FS'    : {1000 : TriggerRule(PS=-1), },
  'EF_mbSpTrk'    : {1000 : TriggerRule(PS=-1), },
  'EF_fj35_a4tchad'    : {1000 : TriggerRule(PS=-1), },
  'EF_j35_a4tchad'    : {1000 : TriggerRule(PS=-1), },
  'EF_j35_u0uchad_firstempty_LArNoiseBurst'    : {1000 : TriggerRule(PS=-1), },
  'EF_g20_loose'    : {1000 : TriggerRule(PS=-1), },
  'EF_g20_loose_larcalib'    : {1000 : TriggerRule(PS=-1), },
  #''    : {1000 : TriggerRule(PS=-1), },
  }


rules.update(Monitoring_pp_v4_rules.physics_rules)


if AlfaFinal:

    rules['L2_ALFA_Phys_Any']  =                  {  1000 : TriggerRule(PS = 10),}
        
    rules['L1_ALFA_ANY']                 =      {1000 : TriggerRule(PS = 1), } 
    rules['L1_ALFA_ANY_EMPTY']           =      {1000 : TriggerRule(PS = 1), } 
    rules['L1_ALFA_ANY_FIRSTEMPTY']      =      {1000 : TriggerRule(PS = 1), } 
    rules['L1_ALFA_ANY_UNPAIRED_ISO']    =      {1000 : TriggerRule(PS = 1), } 
    rules['L1_ALFA_ANY_UNPAIRED_NONISO'] =      {1000 : TriggerRule(PS = 1), } 
    
    rules['L1_ALFA_SDIFF5']              =      {1000 : TriggerRule(PS = 10), }  
    rules['L1_ALFA_SDIFF6']              =      {1000 : TriggerRule(PS = 10), }  
    rules['L1_ALFA_SDIFF7']              =      {1000 : TriggerRule(PS = 10), }  
    rules['L1_ALFA_SDIFF8']              =      {1000 : TriggerRule(PS = 10), }  
    
    rules['L1_BGRP1_ALFA_BGT']           =      {1000 : TriggerRule(PS = 10), } 
    rules['L1_BGRP4_ALFA_BGT']           =      {1000 : TriggerRule(PS = 10), }  
    rules['L1_LUCID_A_ALFA_C']           =      {1000 : TriggerRule(PS = 1), } 
    rules['L1_LUCID_C_ALFA_A']           =      {1000 : TriggerRule(PS = 1), }  
    rules['L1_MBTS_2_A_ALFA_C']          =      {1000 : TriggerRule(PS = 1), }  
    rules['L1_MBTS_2_C_ALFA_A']          =      {1000 : TriggerRule(PS = 1), }  
    
    
    rules['L2_e5_tight1_ALFA_ANY']       =              {1000 : TriggerRule(PS=1), }   
    rules['L2_mbSpTrk_ALFA_any']         =              {1000 : TriggerRule(PS=1), }    
    rules['L2_alfa_any_NoAlg']           =              {1000 : TriggerRule(PS=1), }    
    rules['L2_alfa_sdiff_NoAlg']         =              {1000 : TriggerRule(PS=1), }    
    rules['L1_EM3_ALFA_MBTS_A']          =              {1000 : TriggerRule(PS=1), }    
    rules['L1_EM3_ALFA_MBTS_C']          =              {1000 : TriggerRule(PS=1), }    
    rules['L2_e5_tight1_ALFA_MBTS_A']    =              {1000 : TriggerRule(PS=1), }    
    rules['L2_e5_tight1_ALFA_MBTS_C']    =              {1000 : TriggerRule(PS=1), }    
    
    rules['L1_MBTS_2']                   =              {1000 : TriggerRule(PS = 3), }, 
