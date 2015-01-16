# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#------------------------------------------------------------------------#
#------------------------------------------------------------------------#
def defineMenu():
    """
    Defines the following LvlFlags:
    
    thresholds   .... list of all threshold names in the menu (includes forced thresholds)
    items        .... list of all L1 item names in the menu
    CtpIdMap     .... map of item names to CTP IDs
    """
    
    from TriggerMenu.l1.Lvl1Flags import Lvl1Flags

    Lvl1Flags.BunchGroupPartitioning = [1] # just partition 1
    Lvl1Flags.BunchGroupNames = ['BCRVeto', 'Filled', 'Calib', 'Empty', 'UnpairedBeam1', 'UnpairedBeam2', 'FirstEmpty', 'InTrain']

    Lvl1Flags.MenuPartitioning = [0] # just partition 1

    Lvl1Flags.RemapThresholdsAsListed = True

    Lvl1Flags.thresholds = [
        #------------------------------------
        #------------------------------------
        #      !!!!!!!IMPORTANT!!!!!!!!!
        # Please make sure to use the right
        # order to get the mapping right!!!
        #------------------------------------
        #------------------------------------

        # 6 x MU
        'MU4', 'MU6', 'MU10', 'MU11', 'MU15', 'MU20',

        # 9 x EM
        'EM3', 'EM6', 'EM10VH', 'EM12', 'EM14VH', 'EM16V', 'EM16VH', 'EM18VH','EM30',

        # 7 x TAU
        'HA8', 'HA11', 'HA11I', 'HA15','HA15I', 'HA20', 'HA40',

        # 8 x JET
        'J10', 'J15', 'J15CF',  'J20', 'J30', 'J50', 'J75', 'J350',

        # 4 x JE
        'JE140', 'JE200', 'JE350','JE500',

        # 4 x JB
        'JB15','JB30', 'JB50',  'JB75',

        # 4 x JF
        'JF15','JF30', 'JF50',  'JF75', 

        # 8 x TE
        'TE20', 'TE300', 'TE500', 'TE700', 'TE800', 'TE1000', 'TE1200', 'TE1400',# --> the last three are left in as a forced threshold.

        # 8 x XS
        'XS25', 'XS30', 'XS35', 'XS45', 'XS50', 'XS55', 'XS60', 'XS65',

        # 8 x XE
        'XE20', 'XE25', 'XE30', 'XE35', 'XE40', 'XE50', 'XE60', 'XE70',

        # 2 x MBTS
        'MBTS_A', 'MBTS_C',

        # 2  x BPTX
        'BPTX0','BPTX1',

        # 2 x LUCID
        'LUCID_A','LUCID_C',

        # 4 x BCM
        'BCM_AtoC', 'BCM_CtoA', 'BCM_Wide', 'BCM_Comb',

        # 3 x ZDC
        'ZDC_A', 'ZDC_C','ZDC_AND',

        # LUCID_COMM 
        'NIMDIR5',

        # TRT
        'NIMDIR25',

        # L1A_MON
        'NIMDIR26',

        # LHCF
        'NIMDIR36',

        # 3 x CALREQ
        'NIM28','NIM29','NIM30','NIM_MUB', 'NIM_MUE',

        # 1 x ZB
        'ZB_EM12',

        ### Forced for MBTSSI
        'MBTS_A0', 'MBTS_A1', 'MBTS_A2', 'MBTS_A3', 'MBTS_A4'  , 'MBTS_A5', 'MBTS_A6', 'MBTS_A7', 
        'MBTS_A8', 'MBTS_A9', 'MBTS_A10', 'MBTS_A11', 'MBTS_A12' , 'MBTS_A13', 'MBTS_A14', 'MBTS_A15', 
        'MBTS_C0', 'MBTS_C1', 'MBTS_C2', 'MBTS_C3', 'MBTS_C4'  , 'MBTS_C5', 'MBTS_C6', 'MBTS_C7', 
        'MBTS_C8', 'MBTS_C9', 'MBTS_C10', 'MBTS_C11', 'MBTS_C12' , 'MBTS_C13', 'MBTS_C14', 'MBTS_C15',

        # Forced for ALFA
        'NIM_B7R1L','NIM_B7R1U','NIM_B7L1L','NIM_B7L1U',
        'NIM_A7R1L','NIM_A7R1U','NIM_A7L1L','NIM_A7L1U',
        'NIM_B7R1L_OD','NIM_B7R1U_OD','NIM_B7L1L_OD','NIM_B7L1U_OD',
        'NIM_A7R1L_OD','NIM_A7R1U_OD','NIM_A7L1L_OD','NIM_A7L1U_OD',

        # For L1_ALFA
        'NIM_ALFA_LOG', 'NIM_ALFA_LOG_OD'

        ]


    Lvl1Flags.items = [
        # EM
        'L1_EM3', 'L1_EM6', 'L1_EM10VH', 'L1_EM12', 'L1_EM14VH', 'L1_EM16V','L1_EM16VH', 'L1_EM18VH', 'L1_EM30', 'L1_EM30_BGRP7',
        # 2 x EM
        'L1_2EM3', 'L1_2EM6', 'L1_2EM12', 'L1_2EM10VH', 'L1_2EM12_EM16V',
        # 3 x EM
        'L1_3EM6', 'L1_2EM3_EM6','L1_2EM3_EM12', 'L1_2EM6_EM16VH', 'L1_3EM6_EM14VH',
        #EM UNPAIRED-EMPTY-ETC
        'L1_EM3_EMPTY', 'L1_EM6_EMPTY', 'L1_EM3_UNPAIRED_ISO','L1_EM3_UNPAIRED_NONISO', 'L1_EM3_FIRSTEMPTY',
        #MU
        'L1_MU4', 'L1_MU6', 'L1_MU10', 'L1_MU11', 'L1_MU15',  'L1_MU20',  'L1_MUB', 'L1_MUE',
        #Multi MU
        'L1_2MU4',  'L1_2MU6', 'L1_2MU10', 'L1_3MU4', 'L1_3MU6', 'L1_2MU4_MU6', 'L1_2MU4_BARRELONLY', 'L1_2MU4_MU6_BARREL','L1_2MU4_BARREL',
        'L1_2MU6_BARREL','L1_2MU6_BARRELONLY','L1_2MU6_MU10','L1_3MU4_MU6',
        #MU UNPAIRED-EMPTY-ETC
        'L1_MU4_UNPAIRED_ISO','L1_MU4_UNPAIRED_NONISO','L1_MU4_EMPTY','L1_MU4_FIRSTEMPTY', 'L1_MU6_FIRSTEMPTY','L1_MU10_FIRSTEMPTY', 'L1_MU20_FIRSTEMPTY',
        'L1_MU10_EMPTY', 'L1_MU11_EMPTY','L1_MU10_UNPAIRED_ISO', 'L1_2MU4_FIRSTEMPTY', 'L1_2MU4_EMPTY', 'L1_2MU6_UNPAIRED_ISO', 'L1_2MU6_UNPAIRED_NONISO',
        ####'L1_2MU6_EMPTY', 'L1_2MU6_FIRSTEMPTY', 'L1_MU4_J15_FIRSTEMPTY', MU4_J15_UNPAIRED_NONISO, 'L1_MU10_J10', 
        # MU combined UNPAIRED 
        'L1_MU4_J15_UNPAIRED_ISO','L1_MU4_J15_EMPTY',
        #TAU
        'L1_TAU8', 'L1_TAU11','L1_TAU15','L1_TAU15I', 'L1_TAU20','L1_TAU40', 'L1_TAU11I',
        #2TAU
        'L1_2TAU11', 'L1_2TAU11I', 'L1_2TAU15', 'L1_2TAU20', 'L1_2TAU11_TAU15', 'L1_2TAU11I_TAU15', 
        #TAU UNPAIRED-EMPTY-ETC
        'L1_TAU8_UNPAIRED_ISO','L1_TAU8_UNPAIRED_NONISO','L1_TAU8_EMPTY','L1_TAU8_FIRSTEMPTY',
        #J
        'L1_J10', 'L1_J15', 'L1_J20', 'L1_J30', 'L1_J50', 'L1_J75',  'L1_J350', 
        #J  UNPAIRED-EMPTY-ETC
        'L1_J10_UNPAIRED_ISO','L1_J10_UNPAIRED_NONISO', 'L1_J10_EMPTY', 'L1_J30_EMPTY',
        'L1_J30_UNPAIRED_ISO','L1_J30_UNPAIRED_NONISO', 'L1_J10_FIRSTEMPTY', 'L1_J30_FIRSTEMPTY',
        #multi J
        'L1_3J15','L1_3J20','L1_4J10', 'L1_4J20', 'L1_5J10', 'L1_5J15', 'L1_5J20',
        'L1_6J10', 'L1_4J15', 'L1_3J15_J50',
        #### 'L1_2J10_J50', 'L1_2J10_J75', L1_J75_FJ75, 'L1_2J75', L1_3J75, L1_4J30, 'L1_3J10_J50'
        #FJ
        'L1_FJ30', 'L1_FJ50', 'L1_FJ75' , #'L1_FJ100',
        #FJ  UNPAIRED-EMPTY-ETC
        'L1_FJ30_UNPAIRED_ISO','L1_FJ30_EMPTY','L1_FJ30_FIRSTEMPTY',
        #muli-FJ
        'L1_2FJ15', 'L1_2FJ30', 'L1_2FJ50', 'L1_J30_FJ30', 'L1_J50_FJ50',
        #JE
        #'L1_JE140','L1_JE200','L1_JE350','L1_JE500',
        #XE
        'L1_XE60', 'L1_XE20', 'L1_XE25', 'L1_XE30','L1_XE35', 'L1_XE40', 'L1_XE50',  'L1_XE70', 'L1_XE35_BGRP7','L1_XE40_BGRP7', 'L1_XE50_BGRP7',
        #TE
        'L1_TE20', 'L1_TE300', 'L1_TE500', 'L1_TE700', 'L1_TE800',
        # XS
        'L1_XS60', 'L1_XS65',   'L1_XS30', 'L1_XS55','L1_XS45','L1_XS50', 
        #Combined
        #TAU+X
        'L1_2TAU11_EM10VH', 'L1_2TAU11I_EM14VH', 'L1_2TAU11_TAU20_EM14VH', 'L1_2TAU11_TAU20_EM10VH',
        'L1_TAU8_MU10', 'L1_TAU11_MU10',
        'L1_TAU15_XE40', 'L1_TAU15_XE35', 'L1_TAU15I_XE35', 'L1_TAU15I_XE40', 'L1_TAU20_XE40', 'L1_TAU20_XE35',
         'L1_TAU15_XE25_3J15',
        'L1_TAU15_XS35', 
        #MU+X
        'L1_MU4_J10', 'L1_MU4_J15', 'L1_MU4_J30', 'L1_MU4_J50', 'L1_MU4_J75', 'L1_MU6_2J20', 'L1_MU10_J20', 'L1_MU6_J15',
        'L1_MU10_XE20',
        'L1_MU10_XE25', 'L1_2MU4_XE40', 'L1_2MU4_XE30',
        'L1_MU4_J20_XE20', 'L1_MU4_J20_XE35', 
        #J+X
        'L1_J30_XE35', 'L1_J30_XE40', 'L1_J50_XE35', 'L1_2J20_XE20', 'L1_2J30_XE20','L1_J50_XE40', 'L1_J30_XE50', 'L1_J50_XE30',
        #EM+X
        'L1_EM6_MU6', 'L1_EM10VH_MU6', 'L1_EM6_MU10', 'L1_EM3_MU6', 'L1_EM16VH_MU4', 'L1_EM6_2MU6', 'L1_2EM6_MU6', 'L1_2MU4_EM3', 'L1_2MU4_2EM3', 
        'L1_EM10VH_XE20', 'L1_EM10VH_XE30', 'L1_EM10VH_XE35', 'L1_EM12_XE20','L1_EM16V_XE20',
        'L1_EM12_4J10',
        'L1_EM16V_XS45', 'L1_EM6_XS45', 'L1_EM12_XS30', 'L1_EM12_XS45','L1_EM6_XS50','L1_EM6_XS60','L1_EM6_XS55',
        #RNDM
        'L1_RD0_FILLED', 'L1_RD0_FIRSTEMPTY','L1_RD0_UNPAIRED_ISO','L1_RD0_EMPTY', 'L1_RD1_FILLED', 'L1_RD1_EMPTY',
        #
        'L1_LHCF',
        #BCM
        'L1_BCM_HT_BGRP0','L1_BCM_Wide_BGRP0','L1_BCM_AC_CA_BGRP0', 'L1_BCM_Wide_EMPTY','L1_BCM_Wide_UNPAIRED_ISO','L1_BCM_Wide_UNPAIRED_NONISO', 'L1_BCM_AC_CA_UNPAIRED_ISO',
        #ZDC
        'L1_ZDC','L1_ZDC_A_C', 'L1_ZDC_EMPTY', 'L1_ZDC_A_C_EMPTY',
        #MBTS
        'L1_MBTS_1', 'L1_MBTS_2', 'L1_MBTS_4_4', 'L1_MBTS_2_UNPAIRED_ISO',
        #LUCID
        'L1_LUCID_A_C', 'L1_LUCID','L1_LUCID_COMM', 'L1_LUCID_COMM_EMPTY', 'L1_LUCID_EMPTY', 'L1_LUCID_UNPAIRED_ISO','L1_LUCID_COMM_UNPAIRED_ISO', 'L1_LUCID_A_C_EMPTY', 'L1_LUCID_A_C_UNPAIRED_ISO', 'L1_LUCID_A_C_UNPAIRED_NONISO',
        #ZB
        'L1_ZB',
        #BGRP and BPTX
        'L1_BPTX0_BGRP0', 'L1_BPTX1_BGRP0',
        #Calibration requests
        'L1_CALREQ0','L1_CALREQ1', 'L1_CALREQ2',
        #Other monitoring
        'L1_TRT', 'L1_L1A_Mon',
        #ALFA
        'L1_ALFA_EMPTY','L1_ALFA_BGRP7', 'L1_ALFA_EMPTY_OD','L1_ALFA_BGRP7_OD',
        # VDM
        'L1_MBTS_2_BGRP7','L1_LUCID_BGRP7','L1_BGRP7',

        'L1_MBTSA0' ,'L1_MBTSA1' ,'L1_MBTSA2' ,'L1_MBTSA3' ,'L1_MBTSA4' , 
        'L1_MBTSA5' ,'L1_MBTSA6' ,'L1_MBTSA7' ,'L1_MBTSA8' ,'L1_MBTSA9' ,
        'L1_MBTSA10','L1_MBTSA11','L1_MBTSA12','L1_MBTSA13','L1_MBTSA14','L1_MBTSA15',
        
        'L1_MBTSC0' ,'L1_MBTSC1' ,'L1_MBTSC2' ,'L1_MBTSC3' ,'L1_MBTSC4' ,
        'L1_MBTSC5' ,'L1_MBTSC6' ,'L1_MBTSC7' ,'L1_MBTSC8' ,'L1_MBTSC9' ,
        'L1_MBTSC10','L1_MBTSC11','L1_MBTSC12','L1_MBTSC13','L1_MBTSC14',
        'L1_MBTSC15',
        
        #VBF
        'L1_2TAU8_TAU15_FJ15', 'L1_2TAU8_TAU15_J15CF' , 'L1_2TAU8_TAU11I_EM10VH_FJ15' , 'L1_2TAU8_TAU11I_EM10VH_J15CF' , 
        'L1_EM14VH_FJ15', 'L1_EM14VH_J15CF' , 'L1_MU10_FJ15', 'L1_MU10_J15CF' ,  'L1_3J15_FJ15',  'L1_2J15_2FJ15',

        ]




    Lvl1Flags.CtpIdMap = {
        'L1_EM3'                       : 0,
        'L1_EM6'                       : 1,
        'L1_EM16V'                     : 2, 
        'L1_EM10VH'                    : 3,
        'L1_EM18VH'                    : 4,
        'L1_EM16VH'                    : 5,
        'L1_EM10VH_XE35'               : 6,
        'L1_TAU15I_XE35'               : 7,
        'L1_TAU15_XS35'                : 8,
        'L1_2EM3_EM6'                  : 9,
        'L1_2EM3'                      : 10,
        'L1_2EM6'                      : 11,
        'L1_MU4_J15_FIRSTEMPTY'        : 12,#DIRNIM    
        'L1_MU4_J15_EMPTY'             : 13,#DIRNIM   
        'L1_MU4_J15_UNPAIRED_ISO'      : 14,#DIRNIM  
        'L1_MBTS_4_4'                  : 15,
        'L1_RD0_FILLED'                : 17,
        'L1_RD0_FIRSTEMPTY'            : 18,
        'L1_RD0_UNPAIRED_ISO'          : 19,
        'L1_J50_XE35'                  : 20,
        'L1_3J15_J50'                  : 21,
        'L1_XS60'                      : 22,
        'L1_J10_UNPAIRED_ISO'          : 23,
        'L1_FJ30_UNPAIRED_ISO'         : 24,
        'L1_TAU8_UNPAIRED_ISO'         : 25,
        'L1_EM6_XS50'                  : 26,#DIRNIM
        'L1_EM6_XS60'                  : 27,#DIRNIM
        'L1_2MU6_UNPAIRED_ISO'         : 28,#DIRNIM 
        'L1_TAU11I'                    : 29,
        'L1_EM6_XS55'                  : 30,
        'L1_XE35_BGRP7'                : 31, #'L1_2MU40_XE40'
        'L1_MU10_FJ15'                 : 32, #L1_TAU11_XE20
        'L1_TAU15_XE25_3J15'           : 33,
        'L1_4J10'                      : 34,
        'L1_J50_XS25'                  : 35,
        'L1_2MU6_UNPAIRED_NONISO'      : 36,
        'L1_J30_XE35'                  : 37,
        'L1_EM14VH'                    : 38, 
        'L1_2TAU11_TAU20_EM10VH'       : 39,
        'L1_EM10VH_MU6'                : 40,
        'L1_2EM6_MU6'                  : 41,
        'L1_MBTS_1'                    : 42,
        'L1_2MU4_EM3'                  : 43,
        'L1_XS65'                      : 44,
        'L1_ALFA_EMPTY'                : 45,
        'L1_ALFA_BGRP7'                : 46,
        'L1_XE40_BGRP7'                : 47,
        'L1_MU10_UNPAIRED_ISO'         : 48,
        'L1_2TAU8_TAU11I_EM10VH_FJ15'  : 49, #L1_XS25
        'L1_XS30'                      : 50,
        'L1_MU4_J30'                   : 51,
        'L1_MU4_UNPAIRED_ISO'          : 52,
        'L1_EM3_UNPAIRED_ISO'          : 53,
        'L1_2MU4_XE30'                 : 54,
        'L1_MU4_J50'                   : 55,
        'L1_2MU4_2EM3'                 : 56,
        'L1_3J15_FJ15'                 : 57, #L1_2J15_J50
        'L1_EM3_FIRSTEMPTY'            : 58,
        'L1_TAU8_FIRSTEMPTY'           : 59,
        'L1_BGRP7'                     : 60,
        'L1_FJ30_FIRSTEMPTY'           : 61,
        'L1_RD0_EMPTY'                 : 62,
        'L1_RD1_FILLED'                : 63,            
        'L1_TAU15'                     : 64,
        'L1_TAU15I'                    : 65, ## was TAU8I
        'L1_TAU8'                      : 66,
        'L1_2TAU11I_EM14VH'            : 67, 
        'L1_2TAU11_TAU15'              : 68,
        'L1_TAU11'                     : 69,
        'L1_2TAU20'                    : 70,
        'L1_2MU6'                      : 71,
        'L1_2MU10'                     : 72,
        'L1_TAU30_NOTEM12'             : 73, 
        'L1_TAU20'                     : 74, 
        'L1_MU10_XE25'                 : 75, ## was 2TAU15_TAU20
        'L1_3MU4'                      : 76,
        'L1_2MU4_MU6'                  : 77,
        'L1_TRT'                       : 78,
        'L1_L1A_Mon'                   : 79,
        'L1_2EM12_EM16V'               : 80, # was TAU11_XS35
        'L1_2TAU8_TAU11I_EM10VH_J15CF' : 81, # was L1_XS35
        'L1_LUCID_COMM_EMPTY'          : 82,
        'L1_EM30_BGRP7'                : 83,
        'L1_MU4_J20_XE20'              : 84,
        'L1_EM30'                      : 85,
        'L1_6J10'                      : 86,
        'L1_MU4_J10'                   : 87, 
        'L1_MU15'                      : 88,
        'L1_MU4'                       : 89,
        'L1_MU6'                       : 90,
        'L1_MU10'                      : 91,
        'L1_MU20'                      : 92,
        'L1_MU11'                      : 93,
        'L1_2MU4'                      : 94,
        'L1_2TAU11'                    : 95,
        'L1_J20'                       : 96,
        'L1_J10'                       : 97,
        'L1_J15'                       : 98,
        'L1_J30'                       : 99,
        'L1_J75'                       : 100,
        'L1_TAU15_XE40'                : 101,
        'L1_J50'                       : 102,
        'L1_J350'                      : 103,
        'L1_4J20'                      : 104,
        'L1_MU10_EMPTY'                : 105,
        'L1_2MU4_EMPTY'                : 106,
        'L1_MU6_J15'                   : 107,
        'L1_LUCID_BGRP7'               : 108,
        'L1_MBTS_2_BGRP7'              : 109,
        'L1_ALFA_BGRP7_OD'             : 110,
        'L1_2FJ15'                     : 111, #L1_J100
        'L1_XS40'                      : 112,
        'L1_2TAU11_EM10VH'             : 113,
        'L1_TAU20_XE35'                : 114,   
        'L1_XE25'                      : 115,
        'L1_XE35'                      : 116,
        'L1_XE40'                      : 117,
        'L1_XE50'                      : 118,
        'L1_LHCF'                      : 119, #L1_FJ100
        'L1_FJ30'                      : 120,
        'L1_TAU8_MU10'                 : 121,
        'L1_J50_XE40'                  : 122,
        'L1_2FJ50'                     : 123,
        'L1_ALFA_EMPTY_OD'             : 124,
        'L1_2FJ30'                     : 125,
        'L1_FJ50'                      : 126,
        'L1_2TAU8_TAU15_J15CF'         : 127, #L1_3J10
        'L1_FJ75'                      : 128,
        'L1_XE70'                      : 129,
        'L1_J30_FJ30'                  : 130,
        'L1_XE60'                      : 131,
        'L1_2TAU11_TAU20_EM14VH'       : 132,
        'L1_XE20'                      : 133,
        'L1_XE50_BGRP7'                : 134,
        'L1_XE30'                      : 135,
        'L1_2TAU15'                    : 136,
        'L1_2TAU11I'                   : 137,
        'L1_TE20'                      : 138,
        'L1_2TAU11I_TAU15'             : 139,
        'L1_TE300'                     : 140,
        'L1_TE700'                     : 141,
        'L1_XS50'                      : 142,
        'L1_TAU15_XE35'                : 143, # was L1_EM10VH_XE20_2J10_J30
        'L1_EM14VH_J15CF'              : 144, # was L1_TAU15_XS25_3J10, and before was L1_EM10VH_XE25_2J10_J30
        'L1_BCM_AC_CA_UNPAIRED_ISO'    : 145,
        'L1_2MU6_MU10'                 : 146, #L1_JE350
        'L1_2MU6_BARREL'               : 147, #L1_JE140
        'L1_2MU6_BARRELONLY'           : 148, #L1_JE200
        'L1_EM12_XS45'                 : 149, 
        'L1_EM6_XS45'                  : 150, # EM12_XS50
        'L1_EM12_4J10'                 : 151, 
        'L1_TAU8_EMPTY'                : 152,
        'L1_XS45'                      : 153,
        'L1_EM6_2MU6'                  : 154,
        'L1_MU10_J10'                  : 155,
        'L1_J10_FIRSTEMPTY'            : 156,
        'L1_J30_FIRSTEMPTY'            : 157,
        'L1_5J20'                      : 158,
        'L1_J30_UNPAIRED_ISO'          : 159,
        'L1_MU4_FIRSTEMPTY'            : 160,
        'L1_MU20_FIRSTEMPTY'           : 161,
        'L1_MU4_EMPTY'                 : 162,
        'L1_2MU4_FIRSTEMPTY'           : 163,
        'L1_MU10_FIRSTEMPTY'           : 164,
        'L1_MU6_FIRSTEMPTY'            : 165,
        'L1_MU11_EMPTY'                : 166,
        'L1_TE800'                     : 167,
        'L1_J50_FJ50'                  : 168,
        'L1_J30_UNPAIRED_NONISO'       : 169,
        'L1_J50_XE30'                  : 170,
        'L1_J10_EMPTY'                 : 171,
        'L1_J30_EMPTY'                 : 172,
        'L1_MU10_J15CF'                : 173, #L1_TAU15_XE25_3J10
        'L1_BCM_Wide_UNPAIRED_ISO'     : 174,        
        'L1_FJ30_EMPTY'                : 175,
        'L1_EM6_EMPTY'                 : 176,
        'L1_EM3_EMPTY'                 : 177,
        'L1_2EM12'                     : 178,
        'L1_EM12'                      : 179,
        'L1_BCM_Wide_EMPTY'            : 180,
        'L1_5J15'                      : 181,
        'L1_EM6_MU6'                   : 182,
        'L1_EM12_XE20'                 : 183,
        'L1_3J20'                      : 184,
        'L1_2EM10VH'                   : 185,
        'L1_2TAU8_TAU15_FJ15'          : 186, #L1_3J50
        'L1_4J15'                      : 187,
        'L1_5J10'                      : 188,
        'L1_ZDC_A_C'                   : 189,
        'L1_2MU4_BARRELONLY'           : 190,
        'L1_2MU4_MU6_BARREL'           : 191,
        'L1_3MU4_MU6'                  : 192, #L1_JE500
        'L1_ZDC'                       : 193,
        'L1_TAU11_MU10'                : 194, # MC_pp_v2
        'L1_MU6_2J20'                  : 195, # MC_pp_v2
        'L1_ZDC_A_C_EMPTY'             : 196,
        'L1_ZDC_EMPTY'                 : 197,
        'L1_LCHF'                      : 198,
        'L1_RD1_EMPTY'                 : 199,
        'L1_3MU6'                      : 200,
        'L1_J30_XE50'                  : 201,
        'L1_2EM6_EM16VH'               : 202, 
        'L1_LUCID_A_C_UNPAIRED_ISO'    : 203,         
        'L1_LUCID_A_C_UNPAIRED_NONISO' : 204, 
        'L1_BCM_Wide_UNPAIRED_NONISO'  : 205, 
        'L1_MBTS_2_UNPAIRED_ISO'       : 206,
        'L1_2J30_XE20'                 : 207,
        'L1_EM10VH_XE30'               : 208,  
        'L1_EM14VH_FJ15'               : 209, #L1_TAU15_XE25_3J10_J30
        'L1_MU4_J20_XE35'              : 210,         
        'L1_MU10_XE20'                 : 211,
        'L1_J30_XE40'                  : 212,
        'L1_2J20_XE20'                 : 213,
        'L1_LUCID_A_C'                 : 214,
        'L1_LUCID'                     : 215,
        'L1_BCM_AC_CA_BGRP0'           : 216,
        'L1_BCM_Wide_BGRP0'            : 217,
        'L1_BCM_HT_BGRP0'              : 218,
        'L1_MU4_UNPAIRED_NONISO'       : 219,
        'L1_3J15'                      : 220,
        'L1_EM3_UNPAIRED_NONISO'       : 221,
        'L1_TAU8_UNPAIRED_NONISO'      : 222,
        'L1_J10_UNPAIRED_NONISO'       : 223,
        'L1_2EM3_EM12'                 : 224,
        'L1_EM10VH_XE20'               : 225,
        'L1_EM16V_XE20'                : 226, # L1_MBTS_1
        'L1_MBTS_2'                    : 227,
        'L1_EM16V_XS45'                : 228,
        'L1_TE500'                     : 229,
        'L1_EM16VH_MU4'                : 230,
        'L1_3EM6_EM14VH'               : 231,
        'L1_LUCID_UNPAIRED_ISO'        : 232,
        'L1_LUCID_COMM_UNPAIRED_ISO'   : 233,
        'L1_MU10_J20'                  : 234,
        'L1_2MU4_BARREL'               : 235,
        'L1_EM12_XS30'                 : 236,
        'L1_2J15_2FJ15'                : 237, #L1_EM12_3J10
        'L1_LUCID_A_C_EMPTY'           : 238,
        'L1_3EM6'                      : 239,
        'L1_ZB'                        : 240,
        'L1_BPTX0_BGRP0'               : 241,
        'L1_BPTX1_BGRP0'               : 242,
        'L1_EM6_MU10'                  : 243,        
        'L1_MU4_J15'                   : 244,
        'L1_LUCID_EMPTY'               : 245,
        'L1_MUB'                       : 246,
        'L1_TAU15I_XE40'               : 247,
        'L1_TAU20_XE40'                : 248,
        'L1_LUCID_COMM'                : 249,
        'L1_EM3_MU6'                   : 250,
        'L1_MUE'                       : 251,
        'L1_MU4_J75'                   : 252,
        'L1_CALREQ0'                   : 253,
        'L1_CALREQ1'                   : 254,
        'L1_CALREQ2'                   : 255
        }
    
    Lvl1Flags.prescales = {
        'L1_ZDC_A_C'          :       -1,
        'L1_MBTS_2'           :   184000, 
        'L1_MBTS_1'           :  1840000,
        }
    
