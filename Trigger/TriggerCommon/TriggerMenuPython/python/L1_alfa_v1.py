# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def setupMenu():

    from TriggerMenuPython.Lvl1 import Lvl1
    from TriggerMenuPython.Lvl1Flags import Lvl1Flags
    from TriggerMenuPython.TriggerPythonConfig import TriggerPythonConfig
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    from AthenaCommon.Logging import logging

    import TriggerMenuPython.MinBiasSliceFlags

    TriggerFlags.Slices_all_setOff()
    Lvl1Flags.thresholds = [
        #------------------------------------
        #------------------------------------
        #      !!!!!!!IMPORTANT!!!!!!!!!
        # Please make sure to use the right
        # order to get the mapping right!!!
        #------------------------------------
        #------------------------------------
        'MU0'      , 'MU6',   'MU10', 'MU11', 'MU15', 'MU20', 
        'EM3'      , 'EM5',   'EM7',  'EM10', 'EM12', 'EM14', 'EM16', 'EM30', 
        'HA6',       'HA8',   'HA11', 'HA11I','HA15', 'HA20', 'HA30','HA50',
        'J10'      , 'J15',   'J20', 'J30', 'J50', 'J75', 'J175', 'J250', 
        'JE60'     , 'JE100', 'JE140','JE200',
        'JB10'     , 'JB30',  'JB50', 'JB75',
        'JF10'     , 'JF30',  'JF50', 'JF75',
        'TE20'     , 'TE180', 'TE300',  'TE400', 'TE500', 'TE600', 'TE700', 'TE1000', # --> the last is left in as a forced threshold.
        'XS15'     , 'XS20',   'XS25',  'XS30',  'XS35','XS40','XS45','XS50',
        'XE10'     ,  'XE20', 'XE25', 'XE30', 'XE35', 'XE40', 'XE50', 'XE60',
        'MBTS_A'   , 'MBTS_C',
        'BPTX0'    ,'BPTX1',
        'LUCID_A'  ,'LUCID_C',
        'BCM_AtoC' , 'BCM_CtoA', 'BCM_Wide', 'BCM_Comb',
        'ZDC_A'    , 'ZDC_C','ZDC_AND',
        'NIMDIR5', 'NIMDIR25', 'NIMDIR26',
        #CALREQ
        'NIM28'    ,'NIM29','NIM30',
        #ZB
        'ZB_EM14'  ,
        ### The following are added in as forced thresholds (MBTSSI).
        'MBTS_A0'  , 'MBTS_A1', 'MBTS_A2', 'MBTS_A3', 
        'MBTS_A4'  , 'MBTS_A5', 'MBTS_A6', 'MBTS_A7', 
        'MBTS_A8'  , 'MBTS_A9', 'MBTS_A10', 'MBTS_A11', 
        'MBTS_A12' , 'MBTS_A13', 'MBTS_A14', 'MBTS_A15', 
        'MBTS_C0'  , 'MBTS_C1', 'MBTS_C2', 'MBTS_C3', 
        'MBTS_C4'  , 'MBTS_C5', 'MBTS_C6', 'MBTS_C7', 
        'MBTS_C8'  , 'MBTS_C9', 'MBTS_C10', 'MBTS_C11', 
        'MBTS_C12' , 'MBTS_C13', 'MBTS_C14', 'MBTS_C15',
        #### Forced for ALFA
        'NIM_B7R1L','NIM_B7R1U','NIM_B7L1L','NIM_B7L1U',
        'NIM_A7R1L','NIM_A7R1U','NIM_A7L1L','NIM_A7L1U',
        ### For L1_ALFA
        'NIM_ALFA_LOG',
        ]
    Lvl1Flags.items = [
        #EM
        'L1_EM3', 'L1_EM5', 'L1_EM7', 'L1_EM10', 'L1_EM12', 'L1_EM14', 'L1_EM16', 'L1_EM30',
        #2EM
        'L1_2EM3', 'L1_2EM5', 'L1_2EM7', 'L1_2EM14', 
        'L1_2EM10', 'L1_3EM5',
        #3EM
        'L1_EM3_UNPAIRED_ISO','L1_EM3_UNPAIRED_NONISO','L1_EM5_EMPTY',
        'L1_EM3_FIRSTEMPTY','L1_EM3_EMPTY',
        #MU
        'L1_MU0', 'L1_MU6', 'L1_MU10', 'L1_MU11', 'L1_MU15',  'L1_MU20', 
        #Multi MU
        'L1_2MU0',  'L1_2MU6', 'L1_2MU10',
        'L1_3MU0',
        #'L1_2MU20',
        'L1_3MU6',
        #MU UNPAIRED-EMPTY-ETC
        'L1_MU0_UNPAIRED_ISO','L1_MU0_UNPAIRED_NONISO','L1_MU0_EMPTY','L1_MU0_FIRSTEMPTY',
        'L1_2MU6_UNPAIRED_ISO','L1_2MU6_UNPAIRED_NONISO','L1_2MU6_EMPTY','L1_2MU6_FIRSTEMPTY',
        'L1_MU6_FIRSTEMPTY','L1_MU10_FIRSTEMPTY','L1_2MU0_FIRSTEMPTY',
        'L1_MU10_EMPTY','L1_2MU0_EMPTY',
        'L1_MU11_EMPTY','L1_MU10_UNPAIRED_ISO',
        'L1_MU20_FIRSTEMPTY',
        # MU combined UNPAIRED etc
        'L1_MU0_J15_UNPAIRED_ISO','L1_MU0_J15_UNPAIRED_NONISO','L1_MU0_J15_EMPTY','L1_MU0_J15_FIRSTEMPTY',
        #TAU
        'L1_TAU15', 'L1_TAU6', 'L1_TAU8', 'L1_TAU50',
        'L1_TAU11', 'L1_TAU20', 'L1_TAU30',
        #2TAU
        'L1_2TAU6',
        'L1_2TAU11',
        'L1_2TAU15',
        #TAU UNPAIRED-EMPTY-ETC
        'L1_TAU8_UNPAIRED_ISO','L1_TAU8_UNPAIRED_NONISO','L1_TAU8_EMPTY','L1_TAU8_FIRSTEMPTY',
        #J
        'L1_J10', 'L1_J15', 'L1_J20',
        'L1_J30', 'L1_J50', 'L1_J75',
        'L1_J175', 'L1_J250',
        #J  UNPAIRED-EMPTY-ETC
        'L1_J10_UNPAIRED_ISO','L1_J10_UNPAIRED_NONISO',
        'L1_J10_EMPTY', 'L1_J30_EMPTY',
        'L1_J30_UNPAIRED_ISO','L1_J30_UNPAIRED_NONISO',
        'L1_J10_FIRSTEMPTY', 'L1_J30_FIRSTEMPTY',
        #multi-J (based on Michael's presentation, Jan 12 plus requests)
        'L1_3J10','L1_3J15','L1_3J20','L1_3J50','L1_3J75',
        'L1_4J10', 'L1_4J20', 'L1_4J30',
        'L1_5J10', 'L1_5J20',
        'L1_6J10',
        'L1_4J15',
        #FJ
        'L1_FJ10', 'L1_FJ30', 'L1_FJ50' , 'L1_FJ75',
        #FJ  UNPAIRED-EMPTY-ETC
        'L1_FJ10_UNPAIRED_ISO','L1_FJ10_EMPTY','L1_FJ10_FIRSTEMPTY',
        #muli-FJ
        'L1_2FJ10', 'L1_2FJ30', 'L1_2FJ50',
        #JE
        'L1_JE100',
        'L1_JE140', 'L1_JE200',
        #XE
        #'L1_XE10', 'L1_XE60', 'L1_XE20', 'L1_XE25', 'L1_XE30','L1_XE35',
        #'L1_XE40', 'L1_XE50',
        #TE
        'L1_TE20', 'L1_TE180', 'L1_TE300','L1_TE400','L1_TE500',
        #RNDM
        'L1_RD0_FILLED', 'L1_RD0_FIRSTEMPTY','L1_RD0_UNPAIRED_ISO','L1_RD0_EMPTY',
        'L1_RD1_FILLED', 'L1_RD1_EMPTY',
        #BCM
        'L1_BCM_HT_BGRP0','L1_BCM_Wide_BGRP0','L1_BCM_AC_CA_BGRP0',#'L1_BCM_Wide_NoCol',
        'L1_BCM_Wide_EMPTY','L1_BCM_Wide_UNPAIRED_ISO','L1_BCM_Wide_UNPAIRED_NONISO',
        'L1_BCM_AC_CA_UNPAIRED_ISO',
        #ZDC
        'L1_ZDC','L1_ZDC_A','L1_ZDC_C','L1_ZDC_A_C','L1_ZDC_AND',
        'L1_ZDC_EMPTY',
        'L1_ZDC_AND_EMPTY',
        'L1_ZDC_UNPAIRED_ISO',
        'L1_ZDC_A_C_EMPTY',
        #'L1_ZDC_A_C_NoCol',
        'L1_ZDC_A_C_UNPAIRED_ISO',
        #MBTS
        'L1_MBTS_1','L1_MBTS_1_1','L1_MBTS_2',
        'L1_MBTS_4_4',
        'L1_MBTS_1_1_UNPAIRED_ISO','L1_MBTS_2_UNPAIRED_ISO',
        'L1_MBTS_4_4_EMPTY',
        #LUCID
        'L1_LUCID_A_C', 'L1_LUCID','L1_LUCID_COMM','L1_LUCID_COMM_EMPTY',
        'L1_LUCID_EMPTY',
        'L1_LUCID_UNPAIRED_ISO','L1_LUCID_COMM_UNPAIRED_ISO',
        'L1_LUCID_A_C_EMPTY',
        'L1_LUCID_A_C_UNPAIRED_ISO',
        'L1_LUCID_A_C_UNPAIRED_NONISO',
#        'L1_LUCID_A_C_NoCol',
        #ZB
        'L1_ZB',
        ## BGRP and BPTX
        'L1_BPTX0_BGRP0', 'L1_BPTX1_BGRP0',
        #'L1_BGRP0',
        #'L1_BGRP1',
        ##Calibration requests
        'L1_CALREQ0','L1_CALREQ1', 'L1_CALREQ2',
        #NIMs
        'L1_NIM_S9C1C19', 'L1_NIM_S9C1C20',
        #Other monitoring
        'L1_TRT', 'L1_L1A_Mon',
        # lumi measurements
        'L1_MBLZ',
        # XS
        #'L1_XS15', 'L1_XS20',  'L1_XS25',  'L1_XS30',  'L1_XS35','L1_XS40','L1_XS45','L1_XS50',
        ## ALFA
        'L1_ALFA_EMPTY','L1_ALFA_BGRP7',
        'L1_ALFA_ELAST1','L1_ALFA_ELAST2','L1_ALFA_ELAST3','L1_ALFA_ELAST4','L1_ALFA_ELAST5', 'L1_ALFA_ELAST6','L1_ALFA_ELAST7','L1_ALFA_ELAST8','L1_ALFA_ELAST9',
        'L1_ALFA_ELAST10', 'L1_ALFA_ELAST11', 'L1_ALFA_ELAST12', 'L1_ALFA_ELAST13', 'L1_ALFA_ELAST14', 'L1_ALFA_ELAST15','L1_ALFA_ELAST16','L1_ALFA_ELAST17','L1_ALFA_ELAST18',
        'L1_ALFA_SDIFF1','L1_ALFA_SDIFF2','L1_ALFA_SDIFF3','L1_ALFA_SDIFF4','L1_ALFA_SDIFF5','L1_ALFA_SDIFF6','L1_ALFA_SDIFF7','L1_ALFA_SDIFF8',
        'L1_ALFA_SHOW1','L1_ALFA_SHOW2','L1_ALFA_SHOW3','L1_ALFA_SHOW4','L1_ALFA_SHOW5','L1_ALFA_SHOW6',
        'L1_ALFA_SYST1','L1_ALFA_SYST2','L1_ALFA_SYST3','L1_ALFA_SYST4',
        'L1_ALFA_SHOWSYST1','L1_ALFA_SHOWSYST2','L1_ALFA_SHOWSYST3','L1_ALFA_SHOWSYST4','L1_ALFA_SHOWSYST5',
        'L1_ALFA_SYST5','L1_ALFA_SYST6','L1_ALFA_SYST7','L1_ALFA_SYST8','L1_ALFA_SYST9','L1_ALFA_SYST10','L1_ALFA_SYST11','L1_ALFA_SYST12','L1_ALFA_SYST13','L1_ALFA_SYST14','L1_ALFA_SYST15',
        'L1_ALFA_SYST16','L1_ALFA_SYST17','L1_ALFA_SYST18','L1_ALFA_SYST19','L1_ALFA_SYST20',
        'L1_ALFA_ANY','L1_ALFA_ANY_EMPTY','L1_ALFA_ANY_FIRSTEMPTY','L1_ALFA_ANY_UNPAIRED_ISO','L1_ALFA_ANY_UNPAIRED_NONISO',
        ## VDM
        'L1_MBTS_2_BGRP7','L1_LUCID_BGRP7','L1_BGRP7',
        ]


    Lvl1Flags.CtpIdMap =  {
        'L1_EM3'                    : 0,
        'L1_EM5'                    : 1,
        'L1_EM7'                    : 2,
        'L1_EM10'                   : 3,
        'L1_EM16'                   : 4,
        'L1_EM14'                   : 5,
        'L1_ALFA_ELAST1'              : 6,
        'L1_MU0_J15_UNPAIRED_NONISO': 7,
        'L1_ALFA_ELAST2'             : 8,
        'L1_2EM7'                   : 9,
        'L1_2EM3'                   : 10,
        'L1_2EM5'                   : 11,
        'L1_MU0_J15_FIRSTEMPTY'     : 12,#DIRNIM    
        'L1_MU0_J15_EMPTY'          : 13,#DIRNIM   
        'L1_MU0_J15_UNPAIRED_ISO'   : 14,#DIRNIM  
        'L1_MBTS_4_4'               : 15,
        'L1_4J30'                   : 16,
        'L1_RD0_FILLED'             : 17,
        'L1_RD0_FIRSTEMPTY'         : 18,
        'L1_RD0_UNPAIRED_ISO'       : 19,
        'L1_ALFA_ELAST3'               : 20,
        'L1_MBTS_4_4_EMPTY'         : 21,
        'L1_XS15'                   : 22,
        'L1_J10_UNPAIRED_ISO'       : 23,
        'L1_FJ10_UNPAIRED_ISO'      : 24,
        'L1_TAU8_UNPAIRED_ISO'      : 25,
        'L1_2MU6_FIRSTEMPTY'        : 26,#DIRNIM
        'L1_2MU6_EMPTY'             : 27,#DIRNIM
        'L1_2MU6_UNPAIRED_ISO'      : 28,#DIRNIM 
        'L1_TE600'                  : 29,#DIRNIM 
        'L1_ALFA_ELAST4'               : 30,
        'L1_ALFA_ELAST5'               : 31,
        'L1_ALFA_ELAST6'               : 32,
        'L1_ALFA_ELAST7'               : 33,
        'L1_4J10'                   : 34,
        'L1_ALFA_ELAST8'               : 35,
        'L1_2MU6_UNPAIRED_NONISO'   : 36,
        'L1_ALFA_ELAST9'               : 37,
        'L1_ALFA_ELAST10'               : 38,
        'L1_ALFA_ELAST11'               : 39,
        'L1_ALFA_ELAST12'               : 40,
        'L1_ALFA_ELAST13'               : 41,
        'L1_ALFA_ELAST14'               : 42,
        'L1_ALFA_ELAST15'               : 43,
        'L1_XS20'                   : 44,
        'L1_ALFA_EMPTY'             : 45,
        'L1_ALFA_BGRP7'             : 46,
        'L1_ALFA_ELAST16'              : 47,
        'L1_MU10_UNPAIRED_ISO'      : 48,
        'L1_XS25'                   : 49,
        'L1_XS30'                   : 50,
        'L1_ALFA_ELAST17'                : 51,
        'L1_MU0_UNPAIRED_ISO'       : 52,
        'L1_EM3_UNPAIRED_ISO'       : 53,
        'L1_ALFA_ELAST18'                : 54,
        'L1_ALFA_SDIFF1'                : 55,
        'L1_ALFA_SDIFF2'                : 56,
        'L1_ALFA_SDIFF3'                : 57,
        'L1_EM3_FIRSTEMPTY'         : 58,
        'L1_TAU8_FIRSTEMPTY'        : 59,
        'L1_BGRP7'                  : 60,
        'L1_FJ10_FIRSTEMPTY'        : 61,
        'L1_RD0_EMPTY'              : 62,
        'L1_RD1_FILLED'             : 63,            
        'L1_TAU15'                  : 64,
        'L1_TAU6'                   : 65,
        'L1_TAU8'                   : 66,
        'L1_TAU50'                  : 67,
        'L1_ALFA_SDIFF4'           : 68,
        'L1_TAU11'                  : 69,
        'L1_ALFA_SDIFF5'            : 70,
        'L1_2MU6'                   : 71,
        'L1_2MU10'                  : 72,
        'L1_TAU20'                  : 73,
        'L1_TAU30'                  : 74,
        'L1_ALFA_SDIFF6'           : 75,
        'L1_3MU0'                   : 76,
        'L1_ALFA_SDIFF7'               : 77,
        'L1_TRT'                    : 78,
        'L1_L1A_Mon'                : 79,
        'L1_ALFA_SDIFF8'             : 80,
        'L1_XS35'                   : 81,
        'L1_NIM_S9C1C19'            : 82,
        'L1_NIM_S9C1C20'            : 83,
        'L1_ALFA_SHOW1'           : 84,
        'L1_EM30'                   : 85,
        'L1_6J10'                   : 86,
        'L1_2TAU8'                  : 87,
        'L1_MU15'                   : 88,
        'L1_MU0'                    : 89,
        'L1_MU6'                    : 90,
        'L1_MU10'                   : 91,
        'L1_MU20'                   : 92,
        'L1_MU11'                   : 93,
        'L1_2MU0'                   : 94,
        'L1_2TAU11'                 : 95,
        'L1_J20'                    : 96,
        'L1_J10'                    : 97,
        'L1_J15'                    : 98,
        'L1_J30'                    : 99,
        'L1_J75'                    : 100,
        'L1_ALFA_SHOW2'        : 101,
        'L1_J50'                    : 102,
        'L1_J175'                   : 103,
        'L1_4J20'                   : 104,
        'L1_MU10_EMPTY'             : 105,
        'L1_2MU0_EMPTY'             : 106,
        'L1_ALFA_SHOW3'                : 107,
        'L1_LUCID_BGRP7'            : 108,
        'L1_MBTS_2_BGRP7'           : 109,
        'L1_2TAU6'                  : 110,
        'L1_J250'                   : 111,
        'L1_XS40'                   : 112,
        'L1_ALFA_SHOW4'            : 113,
        'L1_ALFA_SHOW5'               : 114,   
        'L1_XE25'                   : 115,
        'L1_XE35'                   : 116,
        'L1_XE40'                   : 117,
        'L1_XE50'                   : 118,
        'L1_FJ10'                   : 119,
        'L1_FJ30'                   : 120,
        'L1_ALFA_SHOW6'              : 121,
        'L1_ALFA_SYST1'             : 122,
        'L1_2FJ50'                  : 123,
        'L1_2FJ10'                  : 124,
        'L1_2FJ30'                  : 125,
        'L1_FJ50'                   : 126,
        'L1_3J10'                   : 127,
        'L1_FJ75'                   : 128,
        'L1_XE10'                   : 129,
        'L1_ALFA_SYST2'               : 130,
        'L1_XE60'                   : 131,
        'L1_ALFA_SYST3'        : 132,
        'L1_XE20'                   : 133,
        'L1_3J75'                   : 134,
        'L1_XE30'                   : 135,
        'L1_2TAU15'                 : 136,
        'L1_JE60'                   : 137,
        'L1_TE20'                   : 138,
        'L1_TE800'                  : 139,
        'L1_TE300'                  : 140,
        'L1_TE180'                  : 141,
        'L1_XS50'                   : 142,
        'L1_ALFA_SYST4'                : 143,
        'L1_ALFA_SHOWSYST1'              : 144,
        'L1_BCM_AC_CA_UNPAIRED_ISO' : 145,
        'L1_JE100'                  : 146,
        'L1_JE140'                  : 147,
        'L1_JE200'                  : 148,
        'L1_ALFA_SHOWSYST2'              : 149, 
        'L1_ALFA_SHOWSYST3'              : 150, 
        'L1_TAU8_EMPTY'             : 152,
        'L1_XS45'                   : 153,
        'L1_ALFA_SHOWSYST4'               : 154,
        'L1_ALFA_SHOWSYST5'               : 155,
        'L1_J10_FIRSTEMPTY'         : 156,
        'L1_J30_FIRSTEMPTY'         : 157,
        'L1_5J20'                   : 158,
        'L1_J30_UNPAIRED_ISO'       : 159,
        'L1_MU0_FIRSTEMPTY'         : 160,
        'L1_MU20_FIRSTEMPTY'        : 161,
        'L1_MU0_EMPTY'              : 162,
        'L1_2MU0_FIRSTEMPTY'        : 163,
        'L1_MU10_FIRSTEMPTY'        : 164,
        'L1_MU6_FIRSTEMPTY'         : 165,
        'L1_MU11_EMPTY'             : 166,
        'L1_TE400'                  : 167,
        'L1_ALFA_SYST5'               : 168,
        'L1_J30_UNPAIRED_NONISO'    : 169,
        'L1_ALFA_SYST6'               : 170,
        'L1_J10_EMPTY'              : 171,
        'L1_J30_EMPTY'              : 172,
        'L1_XS650'                  : 173,
        'L1_BCM_Wide_UNPAIRED_ISO'  : 174,        
        'L1_FJ10_EMPTY'             : 175,
        'L1_EM5_EMPTY'              : 176,
        'L1_EM3_EMPTY'              : 177,
        'L1_2EM14'                  : 178,
        'L1_EM12'                   : 179,
        'L1_BCM_Wide_EMPTY'         : 180,
        'L1_ZDC_AND_EMPTY'          : 181,
        'L1_ALFA_SYST7'                : 182,
        'L1_ALFA_SYST8'              : 183,
        'L1_3J20'                   : 184,
        'L1_2EM10'                  : 185,
        'L1_3J50'                   : 186,
        'L1_4J15'                   : 187,
        'L1_5J10'                   : 188,
        'L1_ZDC_AND'                : 189,
        'L1_ZDC_A'                  : 190,
        'L1_ZDC_C'                  : 191,
        'L1_ZDC_A_C'                : 192,
        'L1_ZDC'                    : 193,
        'L1_ALFA_SYST9'             : 194, 
        'L1_ZDC_A_C_UNPAIRED_ISO'   : 195, # MC_pp_v2
        'L1_ZDC_A_C_EMPTY'          : 196,
        'L1_ZDC_EMPTY'              : 197,
        'L1_ALFA_SYST10'              : 198,
        'L1_RD1_EMPTY'              : 199,
        'L1_3MU6'                   : 200,
        'L1_ALFA_SYST11'               : 201,
        'L1_ALFA_SYST12'              : 202,  
        'L1_LUCID_A_C_UNPAIRED_ISO'    : 203,         
        'L1_LUCID_A_C_UNPAIRED_NONISO' : 204, 
        'L1_BCM_Wide_UNPAIRED_NONISO' : 205, 
        'L1_MBTS_2_UNPAIRED_ISO'    : 206,
        'L1_ALFA_SYST13'              : 207,
        'L1_ALFA_SYST14'              : 208,  
        'L1_ALFA_SYST15'                : 209,
        'L1_ALFA_SYST16'                 : 210,         
        'L1_ALFA_SYST17'              : 211,
        'L1_ALFA_SYST18'               : 212,
        'L1_ALFA_SYST19'              : 213,
        'L1_LUCID_A_C'              : 214,
        'L1_LUCID'                  : 215,
        'L1_BCM_AC_CA_BGRP0'        : 216,
        'L1_BCM_Wide_BGRP0'         : 217,
        'L1_BCM_HT_BGRP0'           : 218,
        'L1_MU0_UNPAIRED_NONISO'    : 219,
        'L1_3J15'                   : 220,
        'L1_EM3_UNPAIRED_NONISO'    : 221,
        'L1_TAU8_UNPAIRED_NONISO'   : 222,
        'L1_J10_UNPAIRED_NONISO'    : 223,
        'L1_ALFA_SYST20'              : 224,
        'L1_ALFA_ANY'              : 225,
        'L1_MBTS_1'                 : 226,
        'L1_MBTS_2'                 : 227,
        'L1_MBTS_1_1'               : 228,
        'L1_TE500'                  : 229,
        'L1_TE700'                  : 230, 
        'L1_ALFA_ANY_EMPTY'              : 231,
        'L1_LUCID_UNPAIRED_ISO'     : 232,
        'L1_LUCID_COMM_UNPAIRED_ISO': 233,
        'L1_ZDC_UNPAIRED_ISO'       : 234,
        'L1_MBLZ'                   : 235,
        'L1_MBTS_1_1_UNPAIRED_ISO'  : 237,
        'L1_LUCID_A_C_EMPTY'        : 238,
        'L1_3EM5'                   : 239,
        'L1_ZB'                     : 240,
        'L1_BPTX0_BGRP0'            : 241,
        'L1_BPTX1_BGRP0'            : 242,
        'L1_ALFA_ANY_FIRSTEMPTY'    : 243,        
        'L1_ALFA_ANY_UNPAIRED_ISO'  : 244,
        'L1_LUCID_EMPTY'            : 245,
        'L1_ALFA_ANY_UNPAIRED_NONISO'    : 247,
        'L1_LUCID_COMM'             : 249,
        'L1_LUCID_COMM_EMPTY'       : 251,
        'L1_MBTS_1'                : 252,
        'L1_CALREQ0'                : 253,
        'L1_CALREQ1'                : 254,
        'L1_CALREQ2'                : 255
        }

    TriggerFlags.MuonSlice.signatures = [

        ]
    
    TriggerFlags.EgammaSlice.signatures = [
         #cosmic
        'g5_NoCut_cosmic',
        'g10_NoCut_cosmic',
        # Needed to configure larcalib
        'g11_etcut', 
        'g20_loose',
        'g40_loose', 
        'g60_loose',
        'g80_loose',        
        ]
    
    TriggerFlags.TauSlice.signatures = [
        #cosmic
        'tauNoCut_cosmic',
        ]
    
    TriggerFlags.JetSlice.signatures = [
        'j10_a4tc_EFFS',
        'j15_a4tc_EFFS',
        'j20_a4tc_EFFS',
        'fj10_a4tc_EFFS',
        'fj15_a4tc_EFFS',
        'fj20_a4tc_EFFS',
        #cosmic
        'L1J10_empty_NoAlg',
        'L1J30_empty_NoAlg',
        'L1FJ10_empty_NoAlg',
        ]
    
    TriggerFlags.BjetSlice.signatures  = [
 
        ]
    
    TriggerFlags.BphysicsSlice.signatures  = [

        ]

    TriggerFlags.METSlice.signatures = [

        ]

    TriggerFlags.MinBiasSlice.signatures = [
        'mbSpTrkVtxMh',
        'mbSpTrkVtxMh_eff',
        'mbSpTrkVtxMh_medium',
        'mbSpTrkVtxMh_medium_eff',
        'mbSpTrkVtxMh_tight',
        'mbSpTrkVtxMh_tight_eff',
        'rd0_filled_NoAlg',
        'mbRmMbts_1_NoCut_Time',
        'mbLucid_eff',
#        'mbMbts_1_eff',
        'mbMbts_2_eff_unpaired_iso',
        'mbZdc_eff',
        'mbSpTrk',
        'mb_BCM_Wide_eff',
        'mbSpBg_unpaired_iso',
        'mbRd1_eff',
        'mbRd0_eff_unpaired_iso',
        'mbRd0_eff_empty',
        'mbSpTrk_unpaired_iso',
        'mbMbts_2_Monitoring',
        'mbRd0_Monitoring',
        'L1TRT_NoAlg',
        'rd0_empty_NoAlg',
        'rd1_empty_NoAlg',
        'zerobias_NoAlg',
        'mbTE20_eff',
        'VdM_Monitoring',
        ]
    
    TriggerFlags.CombinedSlice.signatures = [
 
        ]
    
    TriggerFlags.CalibSlice.signatures = [
	# LAr calib chains
        'g11_larcalib',
        'g20_larcalib',
        'g40_larcalib',
        'g60_larcalib',
        'g80_larcalib',
        'j25_larcalib',
        'j50_larcalib',
        'j95_larcalib',
        'fj25_larcalib',
        'fj50_larcalib',
        #
	'em3_empty_larcalib',
        'em5_empty_larcalib',
        'tau8_empty_larcalib',
	'j10_empty_larcalib',
        'fj10_empty_larcalib',
        # ALFA Chains
        'ALFA_Calib','ALFA_Phys',
        ]
    
    TriggerFlags.CosmicSlice.signatures = [
        ## CALIBRATIONS
        'TileCalib_laser',
        'PixelNoise',
        'PixelBeam',
        'SCTNoise',
        ## ID ALIGNMENT
        'Cosmics_MU11_TRTxK_TRTTrkHypo_Pixel',
        'Cosmics_MU11_TRTxK_TRTTrkHypo_AllID',
        ## Monitoring
        #'TileMon', # savannah : 79351
        # physics
        'CosmicsAllTeIDSCAN_AllPhysics_TrkHypo',
        'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo',
        # physics
        'CosmicsAllTeSiTrack_AllPhysics_TrkHypo',
        'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo',
        # physics
        'CosmicsAllTeTRTxK_TRTTrkHypo_Pixel',
        'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics',
        #
        'CosmicsAllTeEFID',
        #
        'PreS_L1EM3_empty',
        'PreS_L1EM5_empty',
        'PreS_L1TAU8_empty',
        'PreS_L1J10_empty',
        'PreS_L1J30_empty',
        'PreS_L1J10_firstempty',
        'PreS_L1J30_firstempty',
        #VdM scan
        'VdM_RD0_UNPAIRED_ISO',
        'VdM_MBTS_2_UNPAIRED_ISO',
#        'VdM_MBTS_1_1_UNPAIRED_ISO',
        'VdM_BGRP7',
        'VdM_MBTS_2_BGRP7',
        ]


    #Keep some special chains here rather than in MC menu:
    TriggerFlags.GenericSlice.signatures = [
 
        ]
    
    TriggerFlags.MonitorSlice.signatures = [

        ]
    
    TriggerFlags.BeamSpotSlice.signatures = [

        ]
    
    TriggerFlags.StreamingSlice.signatures = [
        'L1Calo_NoAlg',
        'L1Muon_NoAlg',
        'L1MinBias_NoAlg',
        ]
    
    TriggerFlags.HeavyIonSlice.signatures = []
    TriggerFlags.PerL1ItemStreamingSlice.signatures = []
    
class Prescales:
    #   Item name             | Prescale
    #----------------------------------------------------------
    L1Prescales = {
        }
    #   Signature name        |
    #      [ L2prescale, EFprescale, L2pass-through, EF-pass-through, rerun]
    #   - Prescale values should be a positive integer (default=1)
    #   - If the current pass_through value is non-zero,
    #     the value given here will be used as pass_through rate
    #     Assuming that pass through chains are configured so
    #     in the slice files and won't change. Also prescale
    #     and pass_through will not be used together.
    #   - If only the first two values (L2 and EF prescales) are specified,
    #     the default value of pass-through (=0) will be used
    #   - If L2 pass-through > 1, those events selected by L2 pass-through
    #     will automatically accepted at EF if EF pass-through=0 is set
    #----------------------------------------------------------
    HLTPrescales = {
        'tauNoCut_cosmic'      : [    1,    1,    1,    1,  0],
        }


    #Not sure this is necessary, in Cosmics_v3.py, L1Prescales is empty
    L1Prescales_cosmics = { 
        }
    #Ideally, would do something like:
    #for Flag in Lvl1Flags.items:
    #    L1Prescales_cosmic[Flag] = -1
        
    HLTPrescales_cosmics = {}
    

StreamConfig = {
    'Egamma':      [ 'egamma',
                     'comb__ej', 'comb__emet', 'comb__ejmet', 'comb__eg',
                     'comb__gmet', 
                     'comb__emu', 'comb__gmu',
                     'comb_tauej', 
                     ], 
    'MinBias':     [ 'minbias' ], 
    'Background':  [ 'background' ], 
    'Muons':       [ 'muons', 'bphysics',
                     'comb__muj', 'comb__mumet', 'comb__mujmet',
                     #'comb__emu', 'comb__gmu',
                     'comb_taumuj', 
                     ], 
    'JetTauEtmiss':   [ 'jets', 'tauEtmiss',
                        'comb__tauj', 'comb__taumet', 'comb__jmet',
                        'comb__taujmet', 'comb__taub', 'comb__taue', 
                        'comb__taue', 'comb__taumu', 'comb__fjj', 
                        'comb__tauej', 'comb__taumuj', ],
    'Standby': [ 'standby' ], 
    'calibration': [ 'calibration' ], 
    'express':     [ 'express' ],
    }

TriggerGroups = {
    }

#Define higher groups
#Format is GROUPNAME then in any order SUBGROUPS, ADDITIONAL CHAINS, ALGOS
HigherGroups = {
    }
