# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
#------------Just a copy of Physics_pp_v3 with empty HLT-----------------#
#------------------------------------------------------------------------#
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
        'MU4'      , 'MU6',   'MU10', 'MU11', 'MU15', 'MU20', 
        'EM3'      , 'EM6', 'EM10VH', 'EM12', 'EM14VH', 'EM16V', 'EM16VH', 'EM18VH','EM30',
        'HA8'      , 'HA11', 'HA11I', 'HA15','HA15I', 'HA20', 'HA40',
        'J10'      , 'J15',   'J20', 'J30', 'J50', 'J75', 'J100', 'J350', 
        'JE140'    , 'JE200', 'JE350','JE500',
        'JB30'     , 'JB50',  'JB75', 'JB100',
        'JF30'     , 'JF50',  'JF75', 'JF100',
        'TE20'     , 'TE300', 'TE500', 'TE700', 'TE800', 'TE1000', 'TE1200', 'TE1400',# --> the last three are left in as a forced threshold.
        'XS25'     , 'XS30', 'XS35', 'XS40', 'XS45', 'XS50', 'XS60', 'XS65',
        'XE20', 'XE25', 'XE30', 'XE35', 'XE40', 'XE50', 'XE60', 'XE70',
        'MBTS_A'   , 'MBTS_C',
        'BPTX0'    ,'BPTX1',
        'LUCID_A'  ,'LUCID_C',
        'BCM_AtoC' , 'BCM_CtoA', 'BCM_Wide', 'BCM_Comb',
        'ZDC_A'    , 'ZDC_C','ZDC_AND',
        'NIMDIR5', 'NIMDIR25', 'NIMDIR26',
        #CALREQ
        'NIM28','NIM29','NIM30','NIM_MUB', 'NIM_MUE',
        #ZB
        'ZB_EM12',
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
        'NIM_B7R1L_OD','NIM_B7R1U_OD','NIM_B7L1L_OD','NIM_B7L1U_OD',
        'NIM_A7R1L_OD','NIM_A7R1U_OD','NIM_A7L1L_OD','NIM_A7L1U_OD',
        ### For L1_ALFA
        'NIM_ALFA_LOG', 'NIM_ALFA_LOG_OD'
        ]
    Lvl1Flags.items = [
        #EM thresholds
        'L1_EM3', 'L1_EM6', 'L1_EM12', 'L1_EM10VH',
        'L1_EM14VH','L1_EM16VH', 'L1_EM18VH', 'L1_EM30',
        'L1_EM30_BGRP7',
        #multi EM thresholds
        'L1_2EM3', 'L1_2EM6', #'L1_2EM8',
        'L1_2EM10VH', 'L1_2EM12',
        'L1_2EM3_EM6', 'L1_2EM6_EM16VH', #'L1_2EM12_EM16V',
        'L1_3EM6','L1_3EM6_EM14VH',
        #Jet thresholds
        'L1_J10','L1_J15', 'L1_J30','L1_J50','L1_J75','L1_J100','L1_J350',
        #multi jets         
        'L1_3J10','L1_3J15', 'L1_3J50','L1_3J75','L1_4J10',
        'L1_4J15','L1_4J20','L1_4J30','L1_5J15','L1_6J10',
        #forwards jets
        'L1_FJ30','L1_FJ75','L1_2FJ30','L1_2FJ50',
        #XS
        'L1_XS30', 'L1_XS45', 'L1_XS50','L1_XS60','L1_XS65',
        #XE
        'L1_XE20','L1_XE30','L1_XE40','L1_XE50','L1_XE60','L1_XE70',
        #JE
        'L1_JE140', 'L1_JE350','L1_JE200','L1_JE500',
        #TE
        'L1_TE20','L1_TE300','L1_TE500','L1_TE700','L1_TE800',
        #EMPTY, FIRSTEMPTY, UNPAIRED etc
        'L1_TAU8_EMPTY', 
        'L1_MU11_EMPTY',
        'L1_J10_EMPTY','L1_J10_FIRSTEMPTY','L1_J10_UNPAIRED_ISO','L1_J10_UNPAIRED_NONISO',
        'L1_J30_EMPTY','L1_J30_FIRSTEMPTY','L1_J30_UNPAIRED_ISO','L1_J30_UNPAIRED_NONISO',
        'L1_FJ30_EMPTY','L1_FJ30_FIRSTEMPTY','L1_FJ30_UNPAIRED_ISO',
        'L1_EM3_EMPTY','L1_EM3_FIRSTEMPTY','L1_EM3_UNPAIRED_ISO','L1_EM3_UNPAIRED_NONISO',
        'L1_EM6_EMPTY'            ,
        #ALFA 
        'L1_ALFA_EMPTY','L1_ALFA_EMPTY_OD',        
        'L1_ALFA_BGRP7', 'L1_ALFA_BGRP7_OD',
        #"old" ALFA ITEMS
        'L1_ALFA_ELAST1' ,'L1_ALFA_ELAST2' ,
        'L1_ALFA_ELAST11','L1_ALFA_ELAST12','L1_ALFA_ELAST13','L1_ALFA_ELAST14',
        'L1_ALFA_ELAST15','L1_ALFA_ELAST16','L1_ALFA_ELAST17','L1_ALFA_ELAST18',
        'L1_ALFA_SDIFF1' ,'L1_ALFA_SDIFF2' ,'L1_ALFA_SDIFF3' ,'L1_ALFA_SDIFF4' ,
        'L1_ALFA_SDIFF5' ,'L1_ALFA_SDIFF6' ,'L1_ALFA_SDIFF7' ,'L1_ALFA_SDIFF8' ,
        'L1_ALFA_SHOW1','L1_ALFA_SHOW2','L1_ALFA_SHOW3','L1_ALFA_SHOW4',
        'L1_ALFA_SYST1','L1_ALFA_SYST2','L1_ALFA_SYST3','L1_ALFA_SYST4','L1_ALFA_SYST9',
        'L1_ALFA_SYST10','L1_ALFA_SYST11','L1_ALFA_SYST12','L1_ALFA_SYST17','L1_ALFA_SYST18',
        'L1_ALFA_SHOWSYST1','L1_ALFA_SHOWSYST2','L1_ALFA_SHOWSYST3','L1_ALFA_SHOWSYST4','L1_ALFA_SHOWSYST5',
        'L1_ALFA_ANY','L1_ALFA_ANY_EMPTY','L1_ALFA_ANY_FIRSTEMPTY',
        'L1_ALFA_ANY_UNPAIRED_ISO','L1_ALFA_ANY_UNPAIRED_NONISO',    
        #new ALFA itmes
        'L1_ALFA_A7L1U_OD', 'L1_ALFA_A7L1L_OD','L1_ALFA_A7R1U_OD','L1_ALFA_A7R1L_OD', 
        'L1_ALFA_B7L1U_OD', 'L1_ALFA_B7L1L_OD','L1_ALFA_B7R1U_OD', 'L1_ALFA_B7R1L_OD',
        'L1_ALFA_A7L1_OD' ,'L1_ALFA_A7R1_OD' ,'L1_ALFA_B7L1_OD' ,'L1_ALFA_B7R1_OD' ,
        
        'L1_ALFA_B7L1U', 'L1_ALFA_B7L1L','L1_ALFA_A7L1U','L1_ALFA_A7L1L',             
        'L1_ALFA_A7R1U','L1_ALFA_A7R1L','L1_ALFA_B7R1U','L1_ALFA_B7R1L',                 
        'L1_ALFA_B1_EMPTY','L1_ALFA_B2_EMPTY',     
        
        'L1_MBTS_2_A_ALFA_C','L1_MBTS_2_C_ALFA_A','L1_MBTS_2_ALFA',             
        'L1_MBTS_1_A_ALFA_C','L1_MBTS_1_C_ALFA_A',#'L1_MBTS_1_ALFA',             
        'L1_MBTS_2_ALFA_UNPAIRED_ISO',
        'L1_LUCID_A_ALFA_C','L1_LUCID_C_ALFA_A','L1_LUCID_ALFA',  
        'L1_ZDC_A_ALFA_C','L1_ZDC_C_ALFA_A','L1_ZDC_ALFA',
        #new combined alfa item
        'L1_EM3_ALFA_ANY',
        'L1_EM3_ALFA_MBTS_C',
        'L1_EM3_ALFA_MBTS_A',
        'L1_EM3_ALFA_ANY_UNPAIRED_ISO',
        'L1_EM3_ALFA_MBTS_C_UNPAIRED_ISO',
        'L1_EM3_ALFA_MBTS_A_UNPAIRED_ISO',        
        #BGRP, L1A_Mon
        'L1_BPTX0_BGRP0',  #BPTX, BGRP
        'L1_BPTX1_BGRP0',
        'L1_L1A_Mon' ,
        'L1_BGRP1_ALFA_BGT','L1_BGRP4_ALFA_BGT','L1_BGRP7_ALFA_BGT',
        'L1_BGRP7',
        #ZDC
        'L1_ZDC','L1_ZDC_EMPTY','L1_ZDC_A_C','L1_ZDC_A_C_EMPTY' ,
        #RD0, RD1
        'L1_RD0_EMPTY', 'L1_RD0_FILLED','L1_RD0_FIRSTEMPTY','L1_RD0_UNPAIRED_ISO',       
        'L1_RD1_EMPTY' ,'L1_RD1_FILLED',
        #TRT, ZB
        'L1_TRT',  
        'L1_ZB',        
        #LUCID
        'L1_LUCID','L1_LUCID_EMPTY','L1_LUCID_UNPAIRED_ISO',
        'L1_LUCID_COMM','L1_LUCID_COMM_EMPTY','L1_LUCID_COMM_UNPAIRED_ISO',
        'L1_LUCID_BGRP7',
        'L1_LUCID_A_C','L1_LUCID_A_C_EMPTY','L1_LUCID_A_C_UNPAIRED_ISO','L1_LUCID_A_C_UNPAIRED_NONISO', 
        #MBTS
        'L1_MBTS_1', 'L1_MBTS_2' ,'L1_MBTS_4_4',
        'L1_MBTS_2_UNPAIRED_ISO' ,
        'L1_MBTS_1_UNPAIRED_ISO',
        'L1_MBTS_2_BGRP7'        ,
        #BCM
        'L1_BCM_Wide_EMPTY','L1_BCM_Wide_UNPAIRED_ISO','L1_BCM_Wide_UNPAIRED_NONISO',
        'L1_BCM_Wide_BGRP0','L1_BCM_HT_BGRP0', 'L1_BCM_AC_CA_BGRP0',
        'L1_BCM_AC_CA_UNPAIRED_ISO' ,                       
        'L1_CALREQ0','L1_CALREQ1','L1_CALREQ2',
        #UNPAIRED_ISO ADDITIONS:
        'L1_LUCID_A_ALFA_C_UNPAIRED_ISO','L1_LUCID_C_ALFA_A_UNPAIRED_ISO',
        'L1_MBTS_2_C_ALFA_A_UNPAIRED_ISO','L1_MBTS_2_A_ALFA_C_UNPAIRED_ISO',
        'L1_MBTS_1_C_ALFA_A_UNPAIRED_ISO','L1_MBTS_1_A_ALFA_C_UNPAIRED_ISO',

        ]
    

    Lvl1Flags.CtpIdMap =  {
        'L1_EM3'                    : 0,      #EM items
        'L1_EM6'                    : 1,        
        'L1_EM12'                   : 2,
        'L1_EM10VH'                 : 3,
        'L1_EM14VH'                 : 4, 
        'L1_EM16VH'                 : 5,
        'L1_EM18VH'                 : 6,
        'L1_EM30'                   : 7, 
        'L1_2EM3'                   : 8,      #multi EM items
        'L1_2EM6'                   : 9,
        'L1_2EM8'                   : 10,
        'L1_2EM10VH'                : 11,
        'L1_2EM12'                  : 12,
        'L1_2EM3_EM6'               : 13,
        'L1_2EM6_EM16VH'            : 14, 
        'L1_2EM12_EM16V'            : 15, 
        'L1_3EM6'                   : 16,
        'L1_3EM6_EM14VH'            : 17,
        
        'L1_J10'                    : 18,     # Jet items
        'L1_J15'                    : 19,
        'L1_J30'                    : 20,
        'L1_J50'                    : 21,
        'L1_J75'                    : 22,        
        'L1_J100'                   : 23,
        'L1_J350'                   : 24,
        
        'L1_3J10'                   : 25,     # multi jet items
        'L1_3J15'                   : 26,
        'L1_3J50'                   : 27,
        'L1_3J75'                   : 28,
        'L1_4J10'                   : 29,
        'L1_4J15'                   : 30,
        'L1_4J20'                   : 31,
        'L1_4J30'                   : 32,
        'L1_5J15'                   : 33,
        'L1_6J10'                   : 34,

        'L1_FJ30'                   : 35,     # forward jet items
        'L1_FJ75'                   : 36,     
        'L1_2FJ30'                  : 37,     # multi forward jet items
        'L1_2FJ50'                  : 38,     

        'L1_XS30'                   : 39,     # xs items
        'L1_XS45'                   : 40,
        'L1_XS50'                   : 41,
        'L1_XS60'                   : 42,
        'L1_XS65'                   : 43,

        'L1_XE20'                   : 44,     # xe items
        'L1_XE30'                   : 45,
        'L1_XE40'                   : 46,
        'L1_XE50'                   : 47,        
        'L1_XE60'                   : 48,        
        'L1_XE70'                   : 49,        

        'L1_JE140'                  : 50,     # JE items
        'L1_JE350'                  : 51,
        'L1_JE200'                  : 52,
        'L1_JE500'                  : 53,

        'L1_TE20'                   : 54,     # TE items
        'L1_TE300'                  : 55,
        'L1_TE500'                  : 56,
        'L1_TE700'                  : 57,
        'L1_TE800'                  : 58,

        'L1_TAU8_EMPTY'             : 59,   #EMPTY, FIRSTEMPTY ETC items
        'L1_MU11_EMPTY'             : 60,
        'L1_J10_EMPTY'              : 61,
        'L1_J10_FIRSTEMPTY'         : 62,
        'L1_J10_UNPAIRED_ISO'       : 63,
        'L1_J10_UNPAIRED_NONISO'    : 64,
        'L1_J30_EMPTY'              : 65,
        'L1_J30_FIRSTEMPTY'         : 66,
        'L1_J30_UNPAIRED_ISO'       : 67,
        'L1_J30_UNPAIRED_NONISO'    : 68,
        'L1_FJ30_EMPTY'             : 69,
        'L1_FJ30_FIRSTEMPTY'        : 70,
        'L1_FJ30_UNPAIRED_ISO'      : 71,
        #''   : 72,
        'L1_EM3_EMPTY'              : 73,
        'L1_EM3_FIRSTEMPTY'         : 74,
        'L1_EM3_UNPAIRED_ISO'       : 75,
        'L1_EM3_UNPAIRED_NONISO'    : 76,
        'L1_EM6_EMPTY'              : 77,
        'L1_EM30_BGRP7'             : 78,
                       
        'L1_ALFA_EMPTY'             : 100,   # ALFA items
        'L1_ALFA_EMPTY_OD'          : 101,        
        'L1_ALFA_BGRP7'             : 102,
        'L1_ALFA_BGRP7_OD'          : 103,

        'L1_ALFA_ELAST1'            : 104,
        'L1_ALFA_ELAST2'            : 105,
        'L1_ALFA_ELAST11'           : 106,
        'L1_ALFA_ELAST12'              : 107,
        'L1_ALFA_ELAST13'              : 108,
        'L1_ALFA_ELAST14'              : 109,
        'L1_ALFA_ELAST15'              : 110,
        'L1_ALFA_ELAST16'              : 111,
        'L1_ALFA_ELAST17'              : 112,
        'L1_ALFA_ELAST18'              : 113,
        'L1_ALFA_SDIFF1'              : 114,
        'L1_ALFA_SDIFF2'              : 115,
        'L1_ALFA_SDIFF3'              : 116,
        'L1_ALFA_SDIFF4'              : 117,
        'L1_ALFA_SDIFF5'              : 118,
        'L1_ALFA_SDIFF6'              : 119,
        'L1_ALFA_SDIFF7'              : 120,
        'L1_ALFA_SDIFF8'              : 121,
        'L1_ALFA_SHOW1'              : 122,
        'L1_ALFA_SHOW2'              : 123,
        'L1_ALFA_SHOW3'              : 124,
        'L1_ALFA_SHOW4'              : 125,
        'L1_ALFA_SYST1'              : 126,
        'L1_ALFA_SYST2'              : 127,
        'L1_ALFA_SYST3'              : 128,
        'L1_ALFA_SYST4'              : 129,
        'L1_ALFA_SYST9'              : 130,
        'L1_ALFA_SYST10'             : 131,
        'L1_ALFA_SYST11'             : 132,
        'L1_ALFA_SYST12'             : 133,
        'L1_ALFA_SYST17'             : 134,
        'L1_ALFA_SYST18'             : 135,
        'L1_ALFA_SHOWSYST1'          : 137,
        'L1_ALFA_SHOWSYST2'          : 138,
        'L1_ALFA_SHOWSYST3'          : 139,
        'L1_ALFA_SHOWSYST4'          : 140,
        'L1_ALFA_SHOWSYST5'          : 141,
        'L1_ALFA_ANY'                : 142,
        'L1_ALFA_ANY_EMPTY'          : 143,
        'L1_ALFA_ANY_FIRSTEMPTY'     : 144,
        'L1_ALFA_ANY_UNPAIRED_ISO'   : 145,
        'L1_ALFA_ANY_UNPAIRED_NONISO': 146,

        
        'L1_ALFA_A7L1U_OD'           : 147,   #ALFA OD
        'L1_ALFA_A7L1L_OD'           : 148,
        'L1_ALFA_A7R1U_OD'           : 149,
        'L1_ALFA_A7R1L_OD'           : 150, 
        'L1_ALFA_B7L1U_OD'           : 151, 
        'L1_ALFA_B7L1L_OD'           : 152,
        'L1_ALFA_B7R1U_OD'           : 153, 
        'L1_ALFA_B7R1L_OD'           : 154,
        'L1_ALFA_A7L1_OD'            : 155,
        'L1_ALFA_A7R1_OD'            : 156,
        'L1_ALFA_B7L1_OD'            : 157,
        'L1_ALFA_B7R1_OD'            : 158,

        'L1_ALFA_B7L1U'              : 159,  #Other new ALFA items
        'L1_ALFA_B7L1L'              : 160,
        'L1_ALFA_A7L1U'              : 161,
        'L1_ALFA_A7L1L'              : 162,		
        'L1_ALFA_A7R1U'              : 163,
        'L1_ALFA_A7R1L'              : 164,
        'L1_ALFA_B7R1U'              : 165,
        'L1_ALFA_B7R1L'              : 166,		    
        'L1_ALFA_B1_EMPTY'           : 167,
        'L1_ALFA_B2_EMPTY'           : 168,	
        'L1_MBTS_2_A_ALFA_C'         : 169,
        'L1_MBTS_2_C_ALFA_A'         : 170,
        'L1_MBTS_2_ALFA'             : 171,		
        'L1_LUCID_A_ALFA_C'          : 172,
        'L1_LUCID_C_ALFA_A'          : 173,
        'L1_LUCID_ALFA'              : 174,  
        'L1_ZDC_A_ALFA_C'            : 175,
        'L1_ZDC_C_ALFA_A'            : 176,
        'L1_ZDC_ALFA'                : 177,
        'L1_MBTS_1_A_ALFA_C'         : 178,
        'L1_MBTS_1_C_ALFA_A'         : 179,
        'L1_MBTS_2_ALFA_UNPAIRED_ISO': 180,
        'L1_LUCID_A_ALFA_C_UNPAIRED_ISO'  : 181,
        'L1_LUCID_C_ALFA_A_UNPAIRED_ISO'  : 182,
        'L1_MBTS_2_C_ALFA_A_UNPAIRED_ISO' : 183,
        'L1_MBTS_2_A_ALFA_C_UNPAIRED_ISO' : 184,
        'L1_MBTS_1_C_ALFA_A_UNPAIRED_ISO' : 185,
        'L1_MBTS_1_A_ALFA_C_UNPAIRED_ISO' : 186,
        'L1_EM3_ALFA_ANY'            :187,
        'L1_EM3_ALFA_MBTS_A'         :188,
        'L1_EM3_ALFA_MBTS_C'         :189,
        'L1_EM3_ALFA_ANY_UNPAIRED_ISO': 190,
        'L1_EM3_ALFA_MBTS_A_UNPAIRED_ISO': 191,
        'L1_EM3_ALFA_MBTS_B_UNPAIRED_ISO': 192,

        
        'L1_MBTS_1_UNPAIRED_ISO'     : 210,
        'L1_BGRP7'                   : 211,  
        'L1_BPTX0_BGRP0'             : 212,  #BPTX, BGRP
        'L1_BPTX1_BGRP0'             : 213,
        'L1_L1A_Mon'                 : 214,
        'L1_BGRP1_ALFA_BGT'          : 215,
        'L1_BGRP4_ALFA_BGT'          : 216,
        'L1_BGRP7_ALFA_BGT'          : 217,

        'L1_ZDC'                     : 218,    #ZDC
        'L1_ZDC_EMPTY'               : 219,
        'L1_ZDC_A_C'                 : 220,
        'L1_ZDC_A_C_EMPTY'           : 221,

        'L1_RD0_EMPTY'               : 222,   #RD0
        'L1_RD0_FILLED'              : 223,
        'L1_RD0_FIRSTEMPTY'          : 224,
        'L1_RD0_UNPAIRED_ISO'        : 225,

        'L1_RD1_EMPTY'               : 226,   #RD1
        'L1_RD1_FILLED'              : 227,

        'L1_TRT'                     : 228,    #TRT, ZB
        'L1_ZB'                      : 229,

        'L1_LUCID'                   : 230,    #LUCID
        'L1_LUCID_EMPTY'             : 231,
        'L1_LUCID_UNPAIRED_ISO'      : 232,
        'L1_LUCID_COMM'              : 233,
        'L1_LUCID_COMM_EMPTY'        : 234,
        'L1_LUCID_COMM_UNPAIRED_ISO' : 235,
        'L1_LUCID_BGRP7'             : 136,
        'L1_LUCID_A_C'               : 237,        
        'L1_LUCID_A_C_EMPTY'         : 238,
        'L1_LUCID_A_C_UNPAIRED_ISO'    : 239,         
        'L1_LUCID_A_C_UNPAIRED_NONISO' : 240, 

        'L1_MBTS_1'                  : 241,   # MBTS
        'L1_MBTS_2'                  : 242,        
        'L1_MBTS_4_4'                : 243,
        'L1_MBTS_2_UNPAIRED_ISO'     : 244,        
        'L1_MBTS_2_BGRP7'            : 245,
        

        'L1_BCM_Wide_EMPTY'          : 246,    # BCM
        'L1_BCM_Wide_UNPAIRED_ISO'   : 247,        
        'L1_BCM_Wide_UNPAIRED_NONISO': 248,
        'L1_BCM_Wide_BGRP0'          : 249,
        'L1_BCM_HT_BGRP0'            : 250,
        'L1_BCM_AC_CA_BGRP0'         : 251,
        'L1_BCM_AC_CA_UNPAIRED_ISO'  : 252,                

        'L1_CALREQ0'                 : 253,  #CALREQ
        'L1_CALREQ1'                 : 254,
        'L1_CALREQ2'                 : 255
        }



    TriggerFlags.MuonSlice.signatures = [
        ]
    
    TriggerFlags.EgammaSlice.signatures = [
         #cosmic
        'g5_NoCut_cosmic', #EM3
        'g10_NoCut_cosmic',#EM6
        # Needed to configure larcalib
        #'g11_etcut', #EM5 not in v4
        'g20_loose', #EM10VH
        'g40_loose', #EM30
        'g60_loose', #EM30
        'g80_loose', #EM30
        #express
        'e24vh_tight1_e15_NoCut_Zee',
        'e24vh_medium1',
        #
        'e5_tight1',
        'e5_tight1_ALFA_ANY',
        'e5_tight1_ALFA_MBTS_C',
        'e5_tight1_ALFA_MBTS_A',
        'e5_tight1_ALFA_ANY_UNPAIREDISO',
        'e5_tight1_ALFA_MBTS_C_UNPAIREDISO',
        'e5_tight1_ALFA_MBTS_A_UNPAIREDISO',

        'g12Tvh_loose',
        ]
    
    TriggerFlags.TauSlice.signatures = [
        #cosmic
        'tauNoCut_cosmic', #TAU8_EMPTY
        ]
    
    TriggerFlags.JetSlice.signatures = [
        'j15_a4tchad',   # L1_RD0_FILLED
        'j25_a4tchad',   # L1_RD0_FILLED
        'j35_a4tchad',   # L1_RD0_FILLED
        'fj15_a4tchad',  # L1_RD0_FILLED
        'fj25_a4tchad',  # L1_RD0_FILLED
        'fj35_a4tchad',  # L1_RD0_FILLED
        #cosmic
        'L1J10_empty_NoAlg',  #L1_J10_EMPTY
        'L1J30_empty_NoAlg',  #L1_J30_EMPTY
        'L1FJ30_empty_NoAlg', #L1_FJ10_EMPTY #changed from L1FJ10_empty_NoAlg
        #express stream
        'j35_u0uchad_empty_LArNoiseBurst',
        'j35_u0uchad_firstempty_LArNoiseBurst',
        'j55_u0uchad_firstempty_LArNoiseBurst',
        'j80_u0uchad_LArNoiseBurstT',
        #'j35_a4tchad', #needs prescaling
        #'j180_a4tchad', #needs prescaling
        'j165_u0uchad_LArNoiseBurst',
        'fj55_a4tchad_L2FS',
        'j55_u0uchad_empty_LArNoiseBurst',
        'L1J10_firstempty_NoAlg',  
        'L1J30_empty_NoAlg',
        'j10_a4tchadloose_L1ALFAANY',
        'j15_a4tchad_L1ALFAANY',
        'j25_a4tchad_L1ALFAANY', 
        'j35_a4tchad_L1ALFAANY', 

        'j10_a4tchadloose_L1ALFASDIFF',
        'j15_a4tchad_L1ALFASDIFF',
        'j25_a4tchad_L1ALFASDIFF', 
        'j35_a4tchad_L1ALFASDIFF', 

        'j10_a4tchadloose_L1MBTS',
        'j15_a4tchad_L1MBTS',
        'j25_a4tchad_L1MBTS', 
        'j35_a4tchad_L1MBTS',

        'fj10_a4tchadloose_L1MBTS',
        'fj15_a4tchad_L1MBTS',
        'fj25_a4tchad_L1MBTS', 

        'fj10_a4tchadloose_L1ALFAANY',
        'fj15_a4tchad_L1ALFAANY',
        'fj25_a4tchad_L1ALFAANY',

        'fj10_a4tchadloose_L1ALFASDIFF',
        'fj15_a4tchad_L1ALFASDIFF',
        'fj25_a4tchad_L1ALFASDIFF', 
        ]
    
    TriggerFlags.BjetSlice.signatures  = [
        'b55_NoCut_j55_a4tchad',
        ]
    
    TriggerFlags.BphysicsSlice.signatures  = [

        ]

    TriggerFlags.METSlice.signatures = [
        'xe45_LArNoiseBurst',
        'xe30',
        'xe80',
        ]

    TriggerFlags.MinBiasSlice.signatures = [
        'mbSpTrkVtxMh',          #L1_TE20
        'mbSpTrkVtxMh_eff',        #L1_TE20
        'mbSpTrkVtxMh_medium',        #L1_TE20
        'mbSpTrkVtxMh_medium_eff',        #L1_TE20
        'mbSpTrkVtxMh_tight',        #L1_TE20
        'mbSpTrkVtxMh_tight_eff',        #L1_TE20
        'mbSpTrk', #L1_RD0_FILLED
        'mbSpTrk_unpaired_iso',#L1_RD0_UNPAIRED_ISO  

        'mbSpTrk_ALFA_any',
        'mbSpTrk_ALFA_SDIFF',
        
        'mbLucid_eff',            #L1_LUCID
        'mbZdc_eff', #L1_ZDC
        'mb_BCM_Wide_eff',#L1_BCM_Wide_BGRP0
        'mbRd1_eff', #L1_RD1_FILLED
        'zerobias_NoAlg',  #L1_ZB
        'mbTE20_eff',      #L1_TE20
        'L1TRT_NoAlg',       #L1_TRT

        'rd0_filled_NoAlg',        #L1_RD0_FILLED
        'rd0_empty_NoAlg', #L1_RD0_EMPTY
        'rd1_empty_NoAlg', #L1_RD1_EMPTY

        'mbMbts_1_NoAlg',
        'mbMbts_2_NoAlg',
        'mbMbts_2_unpaired_iso_NoAlg',
        'mbMbts_1_unpaired_iso_NoAlg',
        'mbMbts_1_A_ALFA_C_NoAlg',
        'mbMbts_1_C_ALFA_A_NoAlg',
        'mbMbts_1_C_ALFA_A_UNPAIRED_ISO_NoAlg',
        'mbMbts_1_A_ALFA_C_UNPAIRED_ISO_NoAlg',
        
        ]
    
    TriggerFlags.CombinedSlice.signatures = [
 
        ]
    
    TriggerFlags.CalibSlice.signatures = [
	# LAr calib chains
        'g12_larcalib', #EM10VH
        'g20_larcalib',  #EM12
        'g40_larcalib',  #EM30
        'g60_larcalib',  #EM30
        'g80_larcalib',  #EM30

        'fj75_c4cchad_larcalib', #FJ75
        'fj140_c4cchad_larcalib',#FJ30
        'j40_c4cchad_larcalib', #J10
        'j165_c4cchad_larcalib',#J75
        
        #'j25_larcalib',
        #'j50_larcalib',
        #'j95_larcalib',
        #'fj25_larcalib',
        #'fj50_larcalib',
        #
	'em3_empty_larcalib', #EM3
        'em6_empty_larcalib', #EM6
        'tau8_empty_larcalib', #TAU8_EMPTY
	'j10_empty_larcalib', #J10_EMPTY
        'fj30_empty_larcalib', #FJ30_EMPTY
        # ALFA Chains
        'ALFA_Calib',
        'ALFA_Phys',
        'ALFA_Phys_Any',
        ]
    
    TriggerFlags.CosmicSlice.signatures = [
        ## CALIBRATIONS
        'TileCalib_laser', #L1_CALREQ2
        'PixelNoise', #RD0_EMPTY
        'PixelBeam',  #RD0_FILLED
        'SCTNoise',   #RD0_EMPTY
        ## ID ALIGNMENT
        'Cosmics_MU11_TRTxK_TRTTrkHypo_Pixel', #MU11_EMPTY
        'Cosmics_MU11_TRTxK_TRTTrkHypo_AllID', #MU11_EMPTY
        ## Monitoring
        #'TileMon', # savannah : 79351
        # physics
        'CosmicsAllTeIDSCAN_AllPhysics_TrkHypo',#NO L1 SEED
        'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo',#NO L1 SEED
        # physics
        'CosmicsAllTeSiTrack_AllPhysics_TrkHypo',#NO L1 SEED
        'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo',#NO L1 SEED
        # physics
        'CosmicsAllTeTRTxK_TRTTrkHypo_Pixel',#NO L1 SEED
        'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics',#NO L1 SEED
        #
        'CosmicsAllTeEFID', #NO L1 SEED
        #
        'PreS_L1EM3_empty',
        'PreS_L1EM6_empty',
        'PreS_L1TAU8_empty',
        'PreS_L1J10_empty',
        'PreS_L1J30_empty',  
        'PreS_L1J10_firstempty',
        'PreS_L1J30_firstempty',
        #VdM scan
        'VdM_RD0_UNPAIRED_ISO',
        'VdM_MBTS_2_UNPAIRED_ISO',
        'VdM_BGRP7',
        'VdM_MBTS_2_BGRP7',
        ]


    #Keep some special chains here rather than in MC menu:
    TriggerFlags.GenericSlice.signatures = [
 
        ]
    
    TriggerFlags.MonitorSlice.signatures = [
        'cost_monitor',
        'ROBRequestMon',
        'ROBRequestMonUnseeded',
        'HLTTimeBurner',
        #TRT Monitoring
        'IDMon_filled',
        'IDMon_empty',
        'IDMon_unpaired_iso',
        'InDetMon_FS', #express stream
        ]
    
    TriggerFlags.BeamSpotSlice.signatures = [

        ]
    
    TriggerFlags.StreamingSlice.signatures = [
        'L1Calo_NoAlg',
        'L1MinBias_NoAlg',
        'Background',
        'Background_RD0_UNPAIRED_ISO',
        'Background_RD0_EMPTY',
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
        'tauNoCut_cosmic'                              : [    1,    1,    1,    1,  0],
        'CosmicsAllTeTRTxK_TRTTrkHypo_Pixel'           : [    -1,    -1,    0,    0],
        'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics'      : [    -1,    -1,    0,    0],
        'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo' : [    -1,    -1,    0,    0],
        'CosmicsAllTeSiTrack_AllPhysics_TrkHypo'       : [    -1,    -1,    0,    0],
        'CosmicsAllTeIDSCAN_AllPhysics_TrkHypo'        : [    -1,    -1,    0,    0],
        'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo'  : [    -1,    -1,    0,    0],
        'Cosmics_MU11_TRTxK_TRTTrkHypo_Pixel'          : [    -1,    -1,    0,    0],
        'Cosmics_MU11_TRTxK_TRTTrkHypo_AllID'          : [    -1,    -1,    0,    0],
        'CosmicsAllTeEFID'                             : [    -1,    -1,    0,    0],
        'Standby'                                      : [    -1,    -1,    0,    0],

        'hadCalib_trk9_L1RD0'                          : [    1, 1000,    0,    0,  0],
        'hadCalib_trk18_L1RD0'                         : [    1, 1000,    0,    0,  0],
        'mbSpTrkVtxMh_eff'                             : [    1, 1000,    0,    0,  0],
        'mbSpTrkVtxMh_eff'                             : [    1, 1000,    0,    0,  0],
        'mbSpTrkVtxMh'                                 : [    1, 1000,    0,    0,  0],
        'mbSpTrkVtxMh_tight_eff'                       : [    1, 1000,    0,    0,  0],
        'mbSpTrkVtxMh_tight'                           : [    1, 1000,    0,    0,  0],
        'mbLucid_eff'                                  : [    1, 1000,    0,    0,  0],
        'mbTE20_eff'                                   : [    1, 1000,    0,    0,  0],
        'mbSpTrkVtxMh_medium_eff'                      : [    1, 1000,    0,    0,  0],
        'mbSpTrkVtxMh_medium'                          : [    1, 1000,    0,    0,  0],
        'mb_BCM_Wide_eff'                              : [    1, 1000,    0,    0,  0],
        'mbRd1_eff'                                    : [    1, 1000,    0,    0,  0],
        'mbZdc_eff'                                    : [    1, 1000,    0,    0,  0],
        'mbSpTrk'                                      : [    1, 1000,    0,    0,  0],
        'rd0_filled_NoAlg'                             : [    1, 1000,    0,    0,  0], 
        'mbMbts_1_NoAlg'                               : [    1, 1000,    0,    0,  0],
        'mbMbts_2_NoAlg'                               : [    1, 1000,    0,    0,  0],
        'mbMbts_1_eff'                                 : [    1, 1000,    0,    0,  0],
        'mbSpTrk'                                      : [    1, 1000,    0,    0,  0],
        'mbSpTrk_noiseSup'                             : [    1, 1000,    0,    0,  0],
        'rd0_empty_NoAlg'                              : [    1, 1000,    0,    0,  0],
        'mbSpTrk_empty'                                : [    1, 1000,    0,    0,  0],
        'mbLucid_eff_empty'                            : [    1, 1000,    0,    0,  0],
        'mbSpTrk_unpaired_iso'                         : [    1, 1000,    0,    0,  0],
        'rd0_unpaired_iso_NoAlg'                       : [    1, 1000,    0,    0,  0],
        'mbLucid_eff_unpaired_iso'                     : [    1, 1000,    0,    0,  0],
        'mbMbts_2_unpaired_iso_NoAlg'                  : [    1, 1000,    0,    0,  0],

        'ROBRequestMon'                                : [   -1,   -1],
        'HLTTimeBurner'                                : [   -1,   -1],
        'InDetMon_FS'                                  : [    1000,    1000,    0,    0],
        
        'eb_physics'                                   : [   -1,   -1],
        'eb_random'                                    : [   -1,   -1],
        'eb_physics_unpaired_iso'                      : [   -1,   -1],
        'eb_random_unpaired_iso'                       : [   -1,   -1],
        'eb_physics_firstempty'                        : [   -1,   -1],
        'eb_random_firstempty'                         : [   -1,   -1],
        'eb_physics_empty'                             : [   -1,   -1],
        'eb_random_empty'                              : [   -1,   -1],
        
        'xe_NoCut_allL1'                               : [     0,     0,    0,    0,    1],
        }


    #Not sure this is necessary, in Cosmics_v3.py, L1Prescales is empty
    L1Prescales_cosmics = {
        'L1_EM2'                      : -1,
        'L1_EM3'                      : -1,
        'L1_EM5'                      : -1,
        'L1_EM10'                     : -1,
        'L1_EM10I'                    : -1,
        'L1_EM14'                     : -1,
        'L1_EM14I'                    : -1,
        'L1_EM85'                     : -1,
        'L1_2EM2'                     : -1,
        'L1_2EM3'                     : -1,
        'L1_2EM5'                     : -1,
        'L1_2EM14'                    : -1,
        'L1_2EM10'                    : -1,
        'L1_EM2_UNPAIRED_ISO'         : -1,
        'L1_EM2_UNPAIRED_NONISO'      : -1,
        'L1_EM5_EMPTY'                : -1,
        'L1_EM3_FIRSTEMPTY'           : -1,
        'L1_MU0'                      : -1,
        'L1_MU0_COMM'                 : -1,
        'L1_MU6'                      : -1,
        'L1_MU10'                     : -1,
        'L1_MU15'                     : -1,
        'L1_MU20'                     : -1,
        'L1_2MU0'                     : -1,
        'L1_2MU6'                     : -1,
        'L1_2MU10'                    : -1,
        'L1_2MU20'                    : -1,
        'L1_MU0_UNPAIRED_ISO'         : -1,
        'L1_MU0_UNPAIRED_NONISO'      : -1,
        'L1_MU0_EMPTY'                : -1,
        'L1_MU0_FIRSTEMPTY'           : -1,
        'L1_MU0_COMM_UNPAIRED_ISO'    : -1,
        'L1_MU0_COMM_UNPAIRED_NONISO' : -1,
        'L1_MU0_COMM_FIRSTEMPTY'      : -1,
        'L1_MU6_FIRSTEMPTY'           : -1,
        'L1_MU10_FIRSTEMPTY'          : -1,
        'L1_2MU0_FIRSTEMPTY'          : -1,
        #'L1_TAU5'                     : -1,
        'L1_TAU6'                     : -1,
        'L1_TAU6I'                    : -1,
        'L1_TAU40'                    : -1,
        'L1_TAU11'                    : -1,
        'L1_TAU11I'                   : -1,
        #'L1_TAU20'                    : -1,
        'L1_TAU20'                    : -1,
        #'L1_2TAU5'                    : -1,
        'L1_2TAU6'                    : -1,
        'L1_2TAU11'                   : -1,
        #'L1_TAU5_UNPAIRED_ISO'        : -1,
        #'L1_TAU5_UNPAIRED_NONISO'     : -1,
        #'L1_TAU5_EMPTY'               : -1,
        #'L1_TAU5_FIRSTEMPTY'          : -1,
        'L1_J5'                       : -1,
        'L1_J10'                      : -1,
        'L1_J15'                      : -1,
        'L1_J30'                      : -1,
        'L1_J55'                      : -1,
        'L1_J75'                      : -1,
        'L1_J95'                      : -1,
        'L1_J115'                     : -1,
        'L1_J5_UNPAIRED_ISO'          : -1,
        'L1_J5_UNPAIRED_NONISO'       : -1,
        'L1_J5_EMPTY'                 : -1,
        'L1_J5_FIRSTEMPTY'            : -1,
        'L1_J10_EMPTY'                : -1,
        'L1_J30_EMPTY'                : -1,
        'L1_J10_UNPAIRED'             : -1,
        'L1_J30_UNPAIRED'             : -1,
        'L1_J10_FIRSTEMPTY'           : -1,
        'L1_J30_FIRSTEMPTY'           : -1,
        'L1_4J5'                      : -1,
        'L1_2J5'                      : -1,
        'L1_2J10'                     : -1,
        'L1_2J15'                     : -1,
        'L1_2J30'                     : -1,
        'L1_2J55'                     : -1,
        'L1_2J95'                     : -1,
        'L1_3J10'                     : -1,
        'L1_3J15'                     : -1,
        'L1_4J10'                     : -1,
        'L1_4J15'                     : -1,
        'L1_5J10'                     : -1,
        'L1_6J10'                     : -1,
        'L1_3J30'                     : -1,
        'L1_4J30'                     : -1,
        'L1_3J5_J30'                  : -1,
        'L1_4J5_J15'                  : -1,
        'L1_4J5_J30'                  : -1,
        'L1_2J10_J30'                 : -1,
        'L1_2J10_J55'                 : -1,
        'L1_2J15_J75'                 : -1,
        'L1_3J10_2J30'                : -1,
        'L1_4J10_3J15'                : -1,
        'L1_FJ10'                     : -1,
        'L1_FJ30'                     : -1,
#        'L1_FJ55'                     : -1,
#        'L1_FJ95'                     : -1,
        'L1_FJ10_UNPAIRED_ISO'        : -1,
        'L1_FJ10_EMPTY'               : -1,
        'L1_FJ10_FIRSTEMPTY'          : -1,
        'L1_2FJ10'                    : -1,
        'L1_2FJ30'                    : -1,
#        'L1_2FJ55'                    : -1,
        'L1_J10_FJ10'                 : -1,
        'L1_JE350'                     : -1,
        'L1_JE500'                    : -1,
        'L1_JE140'                    : -1,
        'L1_JE200'                    : -1,
        'L1_XE10'                     : -1,
        'L1_XE15'                     : -1,
        'L1_XE20'                     : -1,
        'L1_XE25'                     : -1,
        'L1_XE30'                     : -1,
        'L1_XE35'                     : -1,
        'L1_XE40'                     : -1,
        'L1_XE50'                     : -1,
        'L1_TE20'                     : -1,
        'L1_TE50'                     : -1,
        'L1_TE100'                    : -1,
        'L1_TE180'                    : -1,
        #'L1_2TAU5_EM5'                : -1,
        'L1_2TAU6_EM5'                : -1,
        'L1_2TAU6_EM10'               : -1,
        #'L1_TAU5_XE10'                : -1,
        #'L1_TAU5_MU6'                 : -1,
        'L1_TAU6_MU10'                : -1,
        'L1_TAU6_MU15'                : -1,
        'L1_TAU6_XE10'                : -1,
        #'L1_TAU5_4J5_3J15'            : -1,
        #'L1_TAU5_3J5_2J15'            : -1,
        'L1_TAU6_4J5_3J15'            : -1,
        'L1_TAU6_3J5_2J15'            : -1,
        'L1_MU0_J5'                   : -1,
        'L1_MU0_J10'                  : -1,
        'L1_MU0_J15'                  : -1,
        'L1_MU0_J30'                  : -1,
        'L1_MU0_J55'                  : -1,
        'L1_MU0_EM3'                  : -1,
        'L1_MU6_EM3'                  : -1,
        'L1_2MU6_EM5'                 : -1,
        'L1_MU6_J5'                   : -1,
        'L1_MU10_J10'                 : -1,
        'L1_2MU0_MU6'                 : -1,
        'L1_J15_XE15_MU15'            : -1,
        'L1_4J10_MU15'                : -1,
        'L1_J30_XE10'                 : -1,
        'L1_J30_XE15'                 : -1,
        'L1_J30_XE25'                 : -1,
        'L1_2J15_XE10'                : -1,
        'L1_2J15_XE15'                : -1,
        'L1_2J15_XE25'                : -1,
        'L1_J15_XE15_EM10'            : -1,
        'L1_4J10_EM10'                : -1,
        'L1_EM5_MU6'                  : -1,
        'L1_EM5_MU10'                 : -1,
        'L1_EM14_XE10'                : -1,
        'L1_EM14_XE15'                : -1,
        'L1_J10_MV'                   : -1,
        'L1_J15_MV'                   : -1,
        'L1_MU0_MV'                   : -1,
        'L1_EM3_MV'                   : -1,
        #'L1_TAU5_MV'                  : -1,
        'L1_RD0_FILLED'               : -1,
        'L1_RD0_FIRSTEMPTY'           : -1,
        'L1_RD0_UNPAIRED_ISO'         : -1,
        'L1_RD0_EMPTY'                : -1,
        'L1_RD1_FILLED'               : -1,
        'L1_RD1_EMPTY'                : -1,
        'L1_BCM_HT_BGRP0'             : -1,
        'L1_BCM_Wide_BGRP0'           : -1,
        'L1_BCM_AC_CA_BGRP0'          : -1,
        'L1_ZDC'                      : -1,
        'L1_ZDC_A'                    : -1,
        'L1_ZDC_C'                    : -1,
        'L1_ZDC_A_C'                  : -1,
        'L1_ZDC_UNPAIRED'             : -1,
        'L1_ZDC_EMPTY'                : -1,
        'L1_ZDC_A_C_EMPTY'            : -1,
        'L1_ZDC_AND_EMPTY'            : -1,
        'L1_MBTS_1'                   : -1,
        'L1_MBTS_2'                   : -1,
        'L1_MBTS_4_4'                 : -1,
        'L1_MBTS_1_EMPTY'             : -1,
        'L1_MBTS_2_EMPTY'             : -1,
        'L1_MBTS_1_1_EMPTY'           : -1,
        'L1_MBTS_1_UNPAIRED_ISO'      : -1,
        'L1_MBTS_2_UNPAIRED_ISO'      : -1,
        'L1_MBTS_1_1_UNPAIRED_ISO'    : -1,
        'L1_MBTS_4_4_UNPAIRED_ISO'    : -1,
        'L1_MBTS_2_UNPAIRED_NONISO'   : -1,
        'L1_LUCID_A'                  : -1,
        'L1_LUCID_C'                  : -1,
        'L1_LUCID_A_C'                : -1,
        'L1_LUCID'                    : -1,
        'L1_LUCID_COMM'               : -1,
        'L1_LUCID_A_EMPTY'            : -1,
        'L1_LUCID_C_EMPTY'            : -1,
        'L1_LUCID_A_C_EMPTY'          : -1,
        'L1_LUCID_EMPTY'              : -1,
        'L1_LUCID_COMM_EMPTY'         : -1,
        'L1_LUCID_A_UNPAIRED'         : -1,
        'L1_LUCID_C_UNPAIRED'         : -1,
        'L1_LUCID_A_C_UNPAIRED'       : -1,
        'L1_ZB'                       : -1,
        'L1_BPTX0_BGRP0'              : -1,
        'L1_BPTX1_BGRP0'              : -1,
        'L1_BGRP1'                    : -1,
        'L1_CALREQ0'                  : -1,
        'L1_CALREQ1'                  : -1,
        'L1_CALREQ2'                  : -1,
        'L1_NIM_S9C0A20'              : -1,
        'L1_NIM_S9C1C19'              : -1,
        'L1_NIM_S9C1C20'              : -1,
        'L1_NIM_S8C2B21'              : -1,
        'L1_NIM_S8C2B22'              : -1,
        'L1_NIM_S8C2B23'              : -1,
        'L1_NIM_S8C2B24'              : -1,
        'L1_NIM_S8C2B25'              : -1,
        'L1_NIM_S8C2B26'              : -1,
        'L1_NIM_S8C2B27'              : -1,
        'L1_NIM_S9C0B21'              : -1,
        'L1_NIM_S9C0B22'              : -1,
        'L1_NIM_S9C0B23'              : -1,
        'L1_NIM_S9C0B24'              : -1,
        'L1_TRT'                      : -1,
        'L1_L1A_Mon'                  : -1,
        'L1_MU0_COMM_EMPTY'           : 1,        
        }
    #Ideally, would do something like:
    #for Flag in Lvl1Flags.items:
    #    L1Prescales_cosmic[Flag] = -1
        
    HLTPrescales_cosmics = {
        }
    

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
                     'comb_taumuj','comb__mue', 
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
    #
    #    'MyGroup1'     : [ "ALG:TrigIDSCAN/*" ],
    #    'MyGroup2'     : [ "Exclusive_Electrons", "mb_BM_calib" ],
    #    'MyGroup3'     : [ "MyGroup2", "Exclusive_MinBias" ],
    #

#     'RATE:FS': ['mu4_Upsimumu_FS',
#                 'mu4_Upsimumu_SiTrk_FS',
#                 'L2IdScan_FS',
#                 'L2SiTrack_FS',
#                 'EFInsideOut_FS',
#                 'EFTRTOnly_FS',
#                 'InDetMon_FS',
#                 'beamspot_vertex_FSTracks_SiTrk']

    }

