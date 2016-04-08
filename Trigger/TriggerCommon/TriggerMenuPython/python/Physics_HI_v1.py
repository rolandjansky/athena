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
        'MU0', 'MU4', 'MU6', 'MU11', 'MU15', 'MU20', 
        'EM3', 'EM4', 'EM5', 'EM7', 'EM10', 'EM12', 'EM14', 'EM16',
        'J10', 'J15', 'J20', 'J30', 'J50', 'J75', 'J175', 'J250', 
        'JE100', 'JE200', 'JE300', 'JE500',
        'JB10', 'JB30', 'JB55', 'JB95',
        'JF10', 'JF30', 'JF55', 'JF95',
        'TE5', 'TE10','TE20', 'TE30', 'TE40', 'TE50', 'TE2000', 'TE4000', 
        'MBTS_A', 'MBTS_C',
        'BPTX0'    ,'BPTX1',
        'LUCID_A','LUCID_C',
        'BCM_AtoC' , 'BCM_CtoA', 'BCM_Wide', 'BCM_Comb',
        'ZDC_A', 'ZDC_C', 'ZDC_AND',
        # Direct NIMs
        'NIMDIR5', 'NIMDIR6', 'NIMDIR7', 'NIMDIR8',
        'NIMDIR14', 'NIMDIR25', 'NIMDIR26',
        #CALREQ
        'NIM28', 'NIM29', 'NIM30',
        #ZB
        'ZB_J75',
        ### The following are added in as forced thresholds (MBTSSI).
        'MBTS_A0'  , 'MBTS_A1', 'MBTS_A2', 'MBTS_A3', 
        'MBTS_A4'  , 'MBTS_A5', 'MBTS_A6', 'MBTS_A7', 
        'MBTS_A8'  , 'MBTS_A9', 'MBTS_A10', 'MBTS_A11', 
        'MBTS_A12' , 'MBTS_A13', 'MBTS_A14', 'MBTS_A15', 
        'MBTS_C0'  , 'MBTS_C1', 'MBTS_C2', 'MBTS_C3', 
        'MBTS_C4'  , 'MBTS_C5', 'MBTS_C6', 'MBTS_C7', 
        'MBTS_C8'  , 'MBTS_C9', 'MBTS_C10', 'MBTS_C11', 
        'MBTS_C12' , 'MBTS_C13', 'MBTS_C14', 'MBTS_C15',
        #forced thresholds
        'HA5', 'HA6', 'HA6I', 'HA8', 'HA11', 'HA11I', 'HA20', 'HA30',
        'XE10', 'XE15', 'XE20', 'XE25', 'XE30', 'XE35', 'XE40', 'XE50',
        'XS25', 'XS30', 'XS35', 'XS40', 'XS45', 'XS50', 'XS60', 'XS65',
        ]

    Lvl1Flags.items = [
                       'L1_EM3',
                       'L1_EM4',
                       'L1_EM5',
                       'L1_EM7',
                       'L1_EM10',
                       'L1_EM12',
                       'L1_EM14',
                       'L1_EM16',
                       'L1_2EM3',
                       #'L1_2EM4',
                       'L1_2EM5',
                       # tau beam items
                       #'L1_TAU8',
                       #muons
                       'L1_MU0',
                       'L1_MU11',
                       'L1_MU15',
                       'L1_MU20',
                       'L1_MU4',
                       'L1_MU6',
                       'L1_2MU0',
                       'L1_2MU4',
                       'L1_2MU0_MU6',
                       'L1_2MU11',
                       'L1_2MU20',
                       'L1_2MU6',
                       'L1_3MU6',
                       #jets
                       'L1_J10',
                       'L1_J15',
                       'L1_J20',                       
                       'L1_J30',
                       'L1_J50',
                       'L1_J75',
                       'L1_J175',
                       'L1_J250',
                       #jet energy
                       'L1_2J15',
                       #'L1_JE200',
                       #'L1_JE300',
                       #'L1_JE500',
                       # forward jets
                       'L1_FJ10',
                       #'L1_FJ30',
                       #'L1_FJ55' ,
                       #'L1_FJ95',
                       #MinBias
                       'L1_LUCID_A',
                       'L1_LUCID_C',
                       'L1_LUCID_A_C',
                       'L1_LUCID',
                       'L1_LUCID_COMM',
                       'L1_MBTS_2',
                       'L1_MBTS_2_EMPTY',
                       'L1_MBTS_2_UNPAIRED_ISO',
                       'L1_MBTS_2_UNPAIRED_NONISO',
                       'L1_MBTS_1',
                       'L1_MBTS_1_EMPTY',
                       'L1_MBTS_1_UNPAIRED_ISO',
                       'L1_MBTS_1_UNPAIRED_NONISO',
                       'L1_MBTS_1_1',
                       'L1_MBTS_2_2',
                       'L1_MBTS_3_3',
                       'L1_MBTS_4_4',
                       'L1_MBTS_1_1_VTE50',
                       'L1_MBTS_2_2_VTE50',
                       #randoms
                       'L1_RD0_FILLED',
                       'L1_RD0_FIRSTEMPTY',
                       'L1_RD0_UNPAIRED_ISO',
                       'L1_RD0_EMPTY',
                       'L1_RD1_FILLED',
                       'L1_RD1_EMPTY',
                       #MET
                       #total energy
                       'L1_TE5',
                       'L1_TE10',
                       'L1_TE20',
                       'L1_TE30',
                       'L1_TE40',
                       'L1_TE50',
                       'L1_TE2000',
                       'L1_TE4000',
                       'L1_TE5_NZ',
                       #Min Bias
                       'L1_ZDC',
                       'L1_ZDC_A',
                       'L1_ZDC_C',
                       'L1_ZDC_AND',
                       'L1_ZDC_AND_VTE50',
                       'L1_ZDC_A_C',
                       'L1_ZDC_A_C_OVERLAY',
                       'L1_ZDC_A_C_VTE50',
                       'L1_ZDC_A_C_TE50',
                       'L1_BCM_Wide',
                       'L1_BCM_HT_BGRP0','L1_BCM_Wide_BGRP0','L1_BCM_AC_CA_BGRP0',
                       'L1_ZDC_MBTS_1',
                       'L1_ZDC_MBTS_2',
                       'L1_ZDC_MBTS_1_1',
                       'L1_ZDC_MBTS_2_2',
                       #NIMDIR stuff
                       'L1_NIM_S8C2B21', 'L1_NIM_S8C2B22', 'L1_NIM_S8C2B23',  
                       # votoed by TE
                       #UPC triggers
                       'L1_MU0_NZ',
                       'L1_EM3_NZ',
                       'L1_J15_NZ',
                       'L1_2MU0_NZ',
                       'L1_2EM3_NZ',
                       'L1_2J15_NZ',
                       'L1_MU0_NL',
                       'L1_EM3_NL',
                       'L1_J15_NL',
                       'L1_2MU0_NL',
                       'L1_2EM3_NL',
                       'L1_2J15_NL',
                       'L1_MU0_MV',
                       'L1_2MU0_MV',
                       'L1_MU0_MV_VTE50',
                       'L1_MU0_VTE50',
                       'L1_MU0_TE50',
                       'L1_MU4_MV_VTE50',
                       'L1_MU4_VTE50',
                       'L1_MU4_TE50',
                       'L1_EM3_MV_VTE50',
                       'L1_EM3_VTE50',
                       'L1_EM3_TE50',
                       ## VDM
                       'L1_ZDC_A_C_BGRP7','L1_LUCID_BGRP7','L1_BGRP7',
                       #MBTS 32 inputs
                       'L1_MBTSA0', 'L1_MBTSA1', 'L1_MBTSA2', 'L1_MBTSA3', 'L1_MBTSA4',
                       'L1_MBTSA5', 'L1_MBTSA6', 'L1_MBTSA7', 'L1_MBTSA8', 'L1_MBTSA9',
                       'L1_MBTSA10', 'L1_MBTSA11', 'L1_MBTSA12', 'L1_MBTSA13', 'L1_MBTSA14',
                       'L1_MBTSA15',
                       'L1_MBTSC0', 'L1_MBTSC1', 'L1_MBTSC2', 'L1_MBTSC3', 'L1_MBTSC4',
                       'L1_MBTSC5', 'L1_MBTSC6', 'L1_MBTSC7', 'L1_MBTSC8', 'L1_MBTSC9',
                       'L1_MBTSC10', 'L1_MBTSC11', 'L1_MBTSC12', 'L1_MBTSC13', 'L1_MBTSC14',
                       'L1_MBTSC15',
                       #background
                       #EM UNPAIRED-EMPTY-ETC
                       'L1_EM3_UNPAIRED_ISO','L1_EM3_UNPAIRED_NONISO','L1_EM3_EMPTY','L1_EM3_FIRSTEMPTY',
                       'L1_EM5_EMPTY',
                        #MU UNPAIRED-EMPTY-ETC
                       'L1_MU0_UNPAIRED_ISO','L1_MU0_UNPAIRED_NONISO','L1_MU0_EMPTY','L1_MU0_FIRSTEMPTY',
                       'L1_MU4_UNPAIRED_ISO', 'L1_MU4_UNPAIRED_NONISO', 'L1_MU4_EMPTY',
                       'L1_MU4_FIRSTEMPTY',
                       'L1_MU6_FIRSTEMPTY','L1_MU11_EMPTY',
                       'L1_2MU0_EMPTY', 'L1_2MU4_EMPTY',
                       'L1_2MU6_UNPAIRED_ISO','L1_2MU6_UNPAIRED_NONISO','L1_2MU6_EMPTY','L1_2MU6_FIRSTEMPTY',
                        #TAU UNPAIRED-EMPTY-ETC
                       'L1_TAU8_UNPAIRED_ISO','L1_TAU8_UNPAIRED_NONISO','L1_TAU8_EMPTY','L1_TAU8_FIRSTEMPTY',
                       #J  UNPAIRED-EMPTY-ETC
                       'L1_J10_UNPAIRED_ISO','L1_J10_UNPAIRED_NONISO','L1_J10_EMPTY','L1_J10_FIRSTEMPTY',
                       'L1_J30_EMPTY', 'L1_J30_UNPAIRED', 'L1_J30_FIRSTEMPTY',
                       #FJ  UNPAIRED-EMPTY-ETC
                       #'L1_FJ10_UNPAIRED_ISO', 'L1_FJ10_FIRSTEMPTY',
                       'L1_FJ10_EMPTY',
                       #ZDC
                       'L1_ZDC_EMPTY',
                       'L1_ZDC_UNPAIRED_ISO',
                       'L1_ZDC_UNPAIRED_NONISO',
                       #L1_ZDC_AND
                       'L1_ZDC_AND_EMPTY', 
                       'L1_ZDC_AND_UNPAIRED_ISO',
                       'L1_ZDC_AND_UNPAIRED_NONISO', 
                       #
                       'L1_ZDC_A_C_EMPTY',
                       'L1_ZDC_A_C_UNPAIRED_ISO',
                       'L1_ZDC_A_C_UNPAIRED_NONISO',
                       #MBTS
                       'L1_MBTS_1_1_EMPTY',
                       'L1_MBTS_1_1_UNPAIRED_ISO',
                       'L1_MBTS_2_2_EMPTY',
                       'L1_MBTS_3_3_EMPTY',
                       'L1_MBTS_2_2_UNPAIRED_ISO',
                       'L1_MBTS_3_3_UNPAIRED_ISO',
                       'L1_MBTS_4_4_UNPAIRED_ISO',
                       #LUCID
                       'L1_LUCID_EMPTY',
                       'L1_LUCID_UNPAIRED_ISO','L1_LUCID_COMM_UNPAIRED_ISO',
                       'L1_LUCID_A_C_EMPTY',
                       'L1_LUCID_A_C_UNPAIRED_ISO',
                       'L1_LUCID_A_C_UNPAIRED_NONISO',
                       #ZB
                       'L1_ZB',
		       # lumi measurements
		       'L1_MLZ_A', 'L1_MLZ_C', 'L1_MBLZ',
                       ## BGRP and BPTX
                       'L1_BPTX0_BGRP0', 'L1_BPTX1_BGRP0',
                       'L1_BGRP0',
                       'L1_BGRP1',
                       #BCM
                       'L1_BCM_Wide_EMPTY','L1_BCM_Wide_UNPAIRED_ISO','L1_BCM_Wide_UNPAIRED_NONISO',
                       'L1_BCM_AC_CA_UNPAIRED_ISO',
                       ##Calibration requests
                       'L1_CALREQ0','L1_CALREQ1', 'L1_CALREQ2',
                       #Other monitoring
                       'L1_TRT', 'L1_L1A_Mon',
                       ]

    Lvl1Flags.CtpIdMap =  {
        'L1_EM3' : 0,
        'L1_EM5' : 1,
        'L1_EM7' : 2,
        'L1_EM10' : 3,
        'L1_EM12' : 4,
        'L1_EM14' : 5,
        'L1_EM3_NZ' : 6,
        'L1_MU0_NZ' : 7,
        'L1_J15_NZ': 8,
        'L1_2EM3_NZ' :  9,
        'L1_2EM3' : 10,
        'L1_2EM5' : 11,
        'L1_MBTS_1_1_VTE50' : 12,
        'L1_NIM_S8C2B21'            : 13,#DIRNIM
        'L1_NIM_S8C2B22'            : 14,#DIRNIM
        'L1_MBTS_4_4' : 15,
        'L1_RD1_EMPTY': 16,
        'L1_RD0_FILLED' : 17,
        'L1_RD0_FIRSTEMPTY' : 18,
        'L1_RD0_UNPAIRED_ISO' : 19,
        'L1_MBTS_4_4_UNPAIRED_ISO': 20,
        'L1_2EM3_NL' : 21,
        'L1_2MU0_NL' : 22,
        'L1_2J15_NL': 23,
        'L1_EM3_NL' : 24,
        'L1_MU0_NL' : 25,
        'L1_J15_NL': 26,
        'L1_ZDC_AND_VTE50' : 27,
        'L1_TE5_NZ': 28,
        'L1_2J15': 29,
        'L1_2J15_NZ' : 30,
        'L1_2MU0_NZ' : 31,
        'L1_MU0_MV' : 32,
        'L1_2MU0_MV' : 33,
        'L1_MU0_MV_VTE50' : 34,
        'L1_MU0_VTE50' : 35,
        'L1_ZDC_A_C_VTE50' : 36,
        'L1_ZDC_A_C_UNPAIRED_NONISO' : 37,
        'L1_MU4_VTE50' : 38,
        'L1_MU4_MV_VTE50' : 39,
        'L1_ZDC_A_C_OVERLAY' : 40,
        'L1_MU0_TE50' : 41,
        'L1_ZDC_A_C_TE50' : 42,
        'L1_MU4_TE50' : 43,
        'L1_EM3_VTE50' : 44,
        'L1_EM3_MV_VTE50' : 45,
        'L1_EM3_TE50' : 46,
        'L1_BGRP7' : 47,
        'L1_LUCID_BGRP7' : 48,
        'L1_ZDC_A_C_BGRP7' : 49,
        'L1_MBTSC4' : 50,
        'L1_MBTSC5' : 51,
        'L1_MBTSC6' : 52,
        'L1_MBTSC7' : 53,
        'L1_MBTSC8' : 54,
        'L1_MBTSC9' : 55,
        'L1_MBTSC10' : 56,
        'L1_MBTSC11' : 57,
        'L1_MBTSC12' : 58,
        'L1_MBTSC13' : 59,
        'L1_MBTSC14' : 60,
        'L1_MBTSC15' : 61,
        'L1_RD0_EMPTY' : 62,
        'L1_RD1_FILLED' : 63,            
        'L1_TAU8' : 64,
        'L1_MBTSC3' : 65,
        'L1_EM3_FIRSTEMPTY' : 66,
        'L1_MU4_FIRSTEMPTY' : 67,
        'L1_MU6_FIRSTEMPTY' : 68,
        'L1_2MU0_EMPTY' : 69,
        'L1_MU0_FIRSTEMPTY' : 70,
        'L1_2MU6' : 71,
        'L1_2MU11' : 72,
        'L1_2MU20' : 73,
        'L1_3MU6' : 74,
        'L1_MU11_EMPTY' : 75,
        'L1_MBTSC2' : 76,
        'L1_2MU0_MU6' : 77,
        'L1_TRT' : 78,
        'L1_L1A_Mon' : 79,
        'L1_BCM_Wide_BGRP0' : 80,
        'L1_BCM_AC_CA_BGRP0' : 81,
        'L1_MBTSC1' : 82,
        'L1_J10_UNPAIRED' : 83,
        'L1_EM4' : 84,
        'L1_EM16' : 85,
        'L1_MBTSC0' : 86,
        'L1_J30_UNPAIRED' : 87,
        'L1_MU15' : 88,
        'L1_MU0' : 89,
        'L1_MU6' : 90,
        'L1_MU11' : 91,
        'L1_MU20' : 92,
        'L1_MU4' : 93,
        'L1_2MU0' : 94,
        'L1_2MU4' : 95,
        'L1_J20' : 96,
        'L1_J10' : 97,
        'L1_J15' : 98,
        'L1_J30' : 99,
        'L1_J75' : 100,
        'L1_J50' : 102,
        'L1_J175' : 104,
        'L1_J250' : 105,
        'L1_BCM_AC_CA_UNPAIRED_ISO' : 106,
        'L1_BCM_Wide_EMPTY' : 107,
        'L1_BCM_Wide_UNPAIRED_ISO' : 108,
        'L1_L1_BCM_Wide_UNPAIRED_NONISO' : 109,
        'L1_J10_UNPAIRED_ISO' : 110,
        'L1_J10_UNPAIRED_NONISO' : 111,
        'L1_LUCID_COMM_UNPAIRED_ISO' : 112,
        'L1_LUCID_UNPAIRED_ISO' : 113,
        'L1_TAU8_FIRSTEMPTY' : 114,
        'L1_TAU8_UNPAIRED_ISO' : 115,
        'L1_TAU8_UNPAIRED_NONISO' : 116,
        'L1_ZDC_A_C_UNPAIRED_ISO' : 117,
        'L1_ZDC_UNPAIRED_ISO' : 118,
        'L1_ZDC_UNPAIRED_NONISO' : 119,
        'L1_MBTSA0' : 120,
        'L1_MBTSA1' : 122,
        'L1_FJ10' : 123,
        'L1_2MU6_UNPAIRED_ISO' : 124,
        'L1_2MU6_UNPAIRED_NONISO' : 125,
        'L1_BCM_Wide_UNPAIRED_NONISO' : 126,
        'L1_EM3_UNPAIRED_ISO' : 127,
        'L1_EM3_UNPAIRED_NONISO' : 128,
        'L1_J10_FIRSTEMPTY' : 129,
        'L1_J30_FIRSTEMPTY' : 130,
        'L1_MBTSA2' : 131,
        'L1_TE5' : 132,
        'L1_TE10' : 133,
        'L1_TE20' : 134,
        'L1_TE30' : 135,
        'L1_TE40' : 136,
        'L1_MBTSA3' : 137,
        'L1_TE50' : 138,
        'L1_ZDC_AND_UNPAIRED_ISO' : 139,
        'L1_ZDC_AND_UNPAIRED_NONISO' : 140,
        'L1_TE4000' : 141,
        'L1_TE2000' : 142,
        'L1_2MU6_EMPTY' : 143,
        'L1_2MU6_FIRSTEMPTY' : 144,
        'L1_ZDC_MBTS_1' : 145,
        'L1_ZDC_MBTS_2' : 146,
        'L1_ZDC_MBTS_1_1' : 147,
        'L1_ZDC_MBTS_2_2' : 148,
        'L1_MBTS_1_EMPTY' : 149,
        'L1_MBTS_1_1_EMPTY' : 150,
        'L1_MBTS_2_EMPTY' : 151,
        'L1_TAU8_EMPTY' : 152,
        'L1_MBTSA4' : 153,
        'L1_MBTSA5' : 154,
        'L1_MBTSA6' : 155,
        'L1_MBTSA7' : 156,
        'L1_NIM_S8C2B23'            : 157,#DIRNIM
        #'L1_NIM_S8C2B24'            : 158,#DIRNIM
        'L1_MU0_UNPAIRED_NONISO' : 159,
        'L1_MU0_UNPAIRED_ISO' : 160,
        'L1_MBTSA8' : 161,
        'L1_MU0_EMPTY' : 162,
        'L1_MBTSA9' : 163,
        'L1_MBTSA10' : 164,
        'L1_MU4_UNPAIRED_NONISO': 165,
        'L1_MU4_EMPTY' : 166,
        'L1_MU4_UNPAIRED_ISO': 167,
        'L1_MBTSA11' : 168,
        'L1_J10_EMPTY' : 171,
        'L1_J30_EMPTY' : 172,
        'L1_MBTSA12' : 173,
        'L1_MBTSA13' : 174,
        'L1_FJ10_EMPTY' : 175,
        'L1_MBTSA14' : 176,
        'L1_EM3_EMPTY' : 177,
        'L1_MBTSA15' : 178,
        'L1_EM5_EMPTY' : 179,
        'L1_MBTS_2_2_VTE50' : 180,
        'L1_ZDC_AND_EMPTY' : 181,
        'L1_ZDC_UNPAIRED_ISO' : 182,
        'L1_ZDC_UNPAIRED_NONISO' : 183,
        #'L1_EM14_EMPTY' : 184,
        #'L1_2EM3_EMPTY' : 185,
        #'L1_2EM4_EMPTY' : 186,
        #'L1_2EM5_EMPTY' : 187,
        #'L1_2EM5I_EMPTY' : 188,
        'L1_ZDC_AND' : 189,
        'L1_ZDC_A' : 190,
        'L1_ZDC_C' : 191,
        'L1_ZDC_A_C' : 192,
        'L1_ZDC' : 193,
        #'L1_ZDC_A_EMPTY' : 194,
        #'L1_ZDC_C_EMPTY' : 195,
        'L1_ZDC_A_C_EMPTY' : 196,
        'L1_ZDC_EMPTY' : 197,
        #'L1_TAU6_XE10' : 198,
        #'L1_TAU6_XE10_EMPTY' : 199,
        'L1_MBTS_1_UNPAIRED_ISO':    200,
        'L1_MBTS_1_UNPAIRED_NONISO':    201,        
        'L1_MBTS_1_1_UNPAIRED_ISO':  202,
        #'L1_MBTS_1_1_UNPAIRED2':  203,        
        'L1_MBTS_2_UNPAIRED_ISO':    204,
        'L1_MBTS_2_UNPAIRED_NONISO':    205,
        #'L1_MBTS_1_UNPAIRED':     206,
        'L1_LUCID_A_C_UNPAIRED_NONISO':  206,
        #'L1_MBTS_2_UNPAIRED':     208,        
        'L1_LUCID_A_C_UNPAIRED_ISO':  208,
        'L1_LUCID_A_UNPAIRED':    209,
        'L1_LUCID_C_UNPAIRED':    210,         
        'L1_LUCID_A_C_UNPAIRED':  211,
        'L1_LUCID_A' : 212,
        'L1_LUCID_C' : 213,
        'L1_LUCID_A_C' : 214,
        'L1_LUCID' : 215,
        #'L1_BCM_AtoC' : 216,
        #'L1_BCM_CtoA' : 217,
        'L1_BCM_Wide' : 218,
        #'L1_BCM_MUL_1' : 219,
        #'L1_BCM_MUL_2' : 220,
        #'L1_BCM_MUL_3' : 221,
        'L1_MBTS_2' : 222,
        'L1_MBTS_2_2' : 223,
        'L1_MBTS_3_3' : 224,
        'L1_BCM_HT_BGRP0' : 225,
        'L1_MBTS_1' : 226,
        #'L1_MBTS_2' : 227,
        'L1_MBTS_1_1' : 228,
        'L1_MBTS_2_2_EMPTY' : 229,
        'L1_MBTS_3_3_EMPTY' : 230,
        'L1_MBTS_2_2_UNPAIRED_ISO' : 231,
        'L1_MBTS_3_3_UNPAIRED_ISO' : 232,
        'L1_MLZ_A' : 233,
        'L1_MLZ_C' : 234,
        'L1_MBLZ' : 235,
        'L1_LUCID_A_EMPTY' : 236,
        'L1_LUCID_C_EMPTY' : 237,
        'L1_LUCID_A_C_EMPTY' : 238,
        #'L1_TAU20_EMPTY' : 239,
        #'L1_TAU30_EMPTY' : 240,
        'L1_BPTX0_BGRP0' : 241,
        'L1_BPTX1_BGRP0' : 242,
        #'L1_BPTX0_BPTX1_BGRP0':243,
        #'' : 244,
        'L1_LUCID_EMPTY' : 245,
        #'L1_BCM_MUL_5_EMPTY' : 246,
        #'L1_BCM_MUL_6_EMPTY' : 247,
        'L1_BGRP0' : 248,
        'L1_LUCID_COMM' : 249,
        'L1_ZB' : 250,
        'L1_LUCID_COMM_EMPTY' : 251,
        'L1_BGRP1' : 252,
        'L1_CALREQ0': 253,
        'L1_CALREQ1' : 254,
        'L1_CALREQ2' : 255,
        }        

    # HLT MinBias
    TriggerFlags.MinBiasSlice.signatures = [
        'mbMbts_1_1',
        'mbMbts_1_1_unpaired_iso',
        'mbMbts_1_1_eff_HI',
        'mbMbts_1_1_eff_unpaired_iso_HI',
        #'mbRmMbts_1_NoCut_Time',
        #'mbMbts_2_Monitoring',
        #L1_ZDC_AND stuff
        'mbZdc_and_eff_HI',
        'mbZdc_and_eff_unpaired_iso_HI',
        'mbZdc_and_NoAlg',
        'mbZdc_and_cosmic_NoAlg',
        #L1_ZDC_A_C stuff
        'mbZdc_a_c_NoAlg',
        'mbZdc_a_c_eff_HI',
        'mbZdc_a_c_eff_unpaired_iso_HI',
        'mbZdc_a_c_cosmic_NoAlg',
        #'mbZdc_eff_unpaired_HI',
        'mbLucid_a_c_eff_HI',
        'mbLucid_a_c_eff_unpaired_iso_HI',
        'mbSpBg_unpaired_iso_HI',
        'mbSp_HI',
        'mbRd1_eff_HI',
        'rd1_empty_NoAlg',
	'rd0_empty_NoAlg', 
        'mbRd0_eff_unpaired_iso_HI',
        'mbSp_unpaired_iso_HI',
        # Backup for mbMbts_1_1
        'mbMbts_2_2', 
        'mbMbts_2_2_eff_HI',
        'mbMbts_2_2_unpaired_iso',
        'mbMbts_2_2_eff_unpaired_iso_HI',
        'mbMbts_4_4',
        'mbMbts_4_4_eff_HI',
        'mbMbts_4_4_unpaired_iso',
        'mbMbts_4_4_eff_unpaired_iso_HI',
        #streamer
        'mbZdc_a_c_L1TE50_NoAlg',
        'L1TE10_NoAlg',
	'L1TE20_NoAlg',
        'L1TE50_NoAlg',
        #streamer for VdM scan
        'L1ZDC_A_C_BGRP7_NoAlg',
        #pPb
        'L1MBTS_2_NoAlg',
        'mbZdc_NoAlg',
        # for overlay
        'mbZdc_a_c_overlay_NoAlg',
        #pA triggers
        'mbSpTrk_L1ZDC',
	'mbSpTrk_L1MBTS_1',
	'mbSpTrk_L1MBTS_2',
        'mbSpTrk_L1MBTS_1_1',
        # SCT noise suppression
        'mbSpTrk_noiseSup',
        'mbSpTrk_noiseSup_unpaired_iso',
        'mbSpBg_noiseSup_unpaired_iso', 
        ]
    
    # HLT Muons
    TriggerFlags.MuonSlice.signatures = [
        'mu4_MSonly', 'mu4T_MSonly',
        'mu4_MSonly_L1TE50', 'mu4_L1VTE50',
        'mu4T_MSonly_L1TE50',
        '2mu4_MSonly',
        '2mu4T_MSonly',
        'mu6_MSonly',
        'mu6T_MSonly',
        'mu10_MSonly', 'mu10T_MSonly',
        'mu13_MSonly', 'mu13T_MSonly',
        '2mu6_MSonly', '2mu6T_MSonly',
        #'mu4_MV', 'mu4_MV_MSonly', 
        #InDet monitoring
        'mu4_IDTrkNoCut',  'mu4T_IDTrkNoCut',
        #EFFS
        'mu4_MSonly_EFFS_L1ZDC', 'mu4_MSonly_EFFS_L1TE10',
        'mu4_MSonly_EFFS_L1TE20', 'mu4_MSonly_EFFS_L1TE50',
	'2mu4_MSonly_EFFS_L1ZDC', '2mu4_MSonly_EFFS_L1TE10',
        '2mu4_MSonly_EFFS_L1TE20', '2mu4_MSonly_EFFS_L1TE50',
        'mu10_MSonly_EFFS_L1ZDC', 'mu10_MSonly_EFFS_L1TE10',
        'mu10_MSonly_EFFS_L1TE20', 'mu10_MSonly_EFFS_L1TE50',
        'mu13_MSonly_EFFS_L1ZDC', 'mu13_MSonly_EFFS_L1TE10',
        'mu13_MSonly_EFFS_L1TE20', 'mu13_MSonly_EFFS_L1TE50',
        '2mu2_MSonly_EFFS_L1ZDC',  '2mu2_MSonly_EFFS_L1TE10',
        '2mu2_MSonly_EFFS_L1TE20', '2mu2_MSonly_EFFS_L1TE50',  
        'mu4_mu2_MSonly_EFFS_L1ZDC', 'mu4_mu2_MSonly_EFFS_L1TE10',
        'mu4_mu2_MSonly_EFFS_L1TE20', 'mu4_mu2_MSonly_EFFS_L1TE50',
        # barrel only
        'mu4_MSonly_barrel_EFFS_L1ZDC',
        'mu4_MSonly_barrel',
        'mu4T_MSonly_barrel',
        #
        '2mu4_MSonly_EFFS_L1MU0',
        #backup
        'mu4', 'mu4T',
        'mu4_loose',
        'mu6_MSonly_L1TE50', 'mu6_L1VTE50',
        'mu6_IDTrkNoCut',
        'mu4_barrel_L1VTE50', 'mu4_MSonly_barrel_L1TE50',
        #pPb
        'mu4_MSonly_EFFS_L1ZDC_OR', 'mu4_MSonly_EFFS_L1MBTS',
        '2mu2_MSonly_EFFS_L1ZDC_OR', '2mu2_MSonly_EFFS_L1MBTS',
        #streamers
        'L1MU0_NoAlg',
        'L1MU4_NoAlg',
        'L1MU11_NoAlg',
        'L1MU15_NoAlg',
        'L1MU20_NoAlg',
        # cosmic
        'mu4_cosmic', 
        'mu4_MSonly_cosmic',
        'mu4T_cosmic', 
        'mu4T_MSonly_cosmic',
        #calibration
        'mu6_l2cal',
        'mu4_l2cal_empty',
        'mu4T_l2cal',
        ]
    
    # HLT Jets 
    TriggerFlags.JetSlice.signatures = [
        # 0.2 hi jets
        'j15_a2hi_EFFS_L1ZDC',
        'j20_a2hi_EFFS_L1ZDC',
        'j25_a2hi_EFFS_L1ZDC',
        'j30_a2hi_EFFS_L1ZDC',
        'j40_a2hi_EFFS_L1ZDC',
        'j50_a2hi_EFFS_L1ZDC',
        'j75_a2hi_EFFS_L1ZDC',
        # 0.2 hi jets seeded of L1_J10
        'L1J10_NoAlg',
        'j15_a2hi_EFFS_L1J10',
        'j20_a2hi_EFFS_L1J10',
	'j25_a2hi_EFFS_L1J10',
	# 0.2 hi jets seeded of L1_TE10 and L1_TE20
        'j15_a2hi_EFFS_L1TE10',
        'j20_a2hi_EFFS_L1TE10',
        'j25_a2hi_EFFS_L1TE10',
	'j15_a2hi_EFFS_L1TE20',
        'j20_a2hi_EFFS_L1TE20',
        'j25_a2hi_EFFS_L1TE20',
        # 0.4 hi jets
        'j15_a4hi_EFFS_L1ZDC',
        'j20_a4hi_EFFS_L1ZDC',
        'j75_a4hi_EFFS_L1ZDC',
        'j100_a4hi_EFFS_L1ZDC',
        'j150_a4hi_EFFS_L1ZDC',
        'j75_a4hi_EFFS_L1J10',
        #pPb
        'j15_a2hi_EFFS_L1MBTS',
        'j20_a2hi_EFFS_L1MBTS',
        'j15_a4hi_EFFS_L1MBTS',
        'j20_a4hi_EFFS_L1MBTS',
        'j15_a4hi_EFFS_L1TE10',
        'j20_a4hi_EFFS_L1TE10',
        # streamers
        'L1J75_NoAlg',
        'L1J175_NoAlg',
        'L1J250_NoAlg',
        #cosmic
        # L15 pp jets
	'NoCut_a4JE_L15FS_NoEF',
        'NoCut_a4TT_L15FS_NoEF',
        'NoCut_a4JE_L15FS_NoEF_L1TE10',
        'NoCut_a4TT_L15FS_NoEF_L1TE10',	
        ]
    
    # HLT MET
    TriggerFlags.METSlice.signatures = [
	#'xe_NoCut_allL1_noMu_Mon',
        ]

    
    # HLT HI (whatever which can't be attained by the regular slices)
    TriggerFlags.HeavyIonSlice.signatures = [
        #UPC
        'L1MU0_MV_VTE50_upc',
        'L1MU0_VTE50_upc',
        'L1MU4_MV_VTE50_upc',
        'L1MU4_VTE50_upc',
        'L1EM3_MV_VTE50_upc',
        'L1EM3_VTE50_upc',
        'L1ZDC_VTE50_upc',
        'L1ZDC_VTE50_mv_loose2_upc',
        'L1ZDC_OR_mv_loose2_upc',
        'L1ZDC_VTE50_mv_medium2_upc',
        'L1ZDC_OR_mv_medium2_upc',
        #backup UPC
        'L1MU0_MV_VTE50_loose_upc',
        'L1MU0_VTE50_loose_upc',
        'L1MU4_MV_VTE50_loose_upc',
        'L1MU4_VTE50_loose_upc',
        'L1EM3_MV_VTE50_loose_upc',
        'L1EM3_VTE50_loose_upc',
        'L1ZDC_VTE50_loose_upc',
        #peripheral events
        'fte75_veto',
        'fte75_veto_mbMbts_2_2',
        #centrality triggers
        'centrality100_60',
        'centrality80_60',
        # min bias with tracking
        'mbZdc_a_c_L1VTE50_trk',
        'mbMbts_1_1_L1VTE50_trk',
        'mbMbts_2_2_L1VTE50_trk',
        ]
    
    # calibration
    TriggerFlags.CalibSlice.signatures = [
        'g5_larcalib','g10_larcalib',
        'j25_larcalib', 'j50_larcalib', 'j95_larcalib', 'j200_larcalib',
        'fj25_larcalib', 
        # LVL1-based chains
        'em3_empty_larcalib', 'em5_empty_larcalib', 'tau8_empty_larcalib',
        'j10_empty_larcalib', 'fj10_empty_larcalib',
        ]
    TriggerFlags.MonitorSlice.signatures = [
        'cost_monitor',
        'InDetMon_HI_FS',
        #HLT Monitoring
        'ROBRequestMon',
        'HLTTimeBurner',
        #TRT Monitoring
        'TRTMon_filled',
        'TRTMon_empty',
        ]

    TriggerFlags.BeamSpotSlice.signatures = [
         'beamspot_vertex_FSTracks_SiTrk',
         'beamspot_vertex_activeTE_SiTrk_peb',
         'beamspot_vertex_allTE_SiTrk_peb',
        ]

    TriggerFlags.EgammaSlice.signatures = [
        'g5_loose', 'g7_loose', 'g10_loose',
        '2g5_loose',
        '2g7_loose',
        '2g10_loose',
        # streamers
        'L1EM3_NoAlg',
        'L1EM10_NoAlg', 'L1EM12_NoAlg',
        'L1EM14_NoAlg', 'L1EM16_NoAlg',
        # cosmic
        'g5_NoCut_cosmic',
        ]

    TriggerFlags.TauSlice.signatures = [
        #cosmic
        'tauNoCut_cosmic',
        ]

    TriggerFlags.BjetSlice.signatures = [
        'mu4_MSonly_j15_a2hi_EFFS_matched',
        ]

    TriggerFlags.BphysicsSlice.signatures = []
    TriggerFlags.CombinedSlice.signatures = [
        'mu4_MSonly_j15_a2hi_EFFS_L1TE10',
        ]

    TriggerFlags.CosmicSlice.signatures = [
        ## CALIBRATIONS
        'TileCalib_laser',
        'PixelNoise',
        'PixelBeam',
        'SCTNoise',
        ## Monitoring
        'TileMon',
        # physics
        'CosmicsAllTeIDSCAN_AllPhysics_TrkHypo',
        'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo',
        # physics
        'CosmicsAllTeSiTrack_AllPhysics_TrkHypo',
        'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo',
        # physics
        'CosmicsAllTeTRTxK_TRTTrkHypo_Pixel',
        'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics',
        'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField',
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
        'VdM_BGRP7',
        'VdM_LUCID_BGRP7',
        'VdM_ZDC_A_C_BGRP7',
        'VdM_ZDC_A_C_VTE50',
        ]

    TriggerFlags.StreamingSlice.signatures = [
        "Standby",
        "SeededStreamerRNDM",
        "SeededStreamerL1Calo",
        "SeededStreamerMinBias",
        "SeededStreamerMBTS",
        "SeededStreamerBCM_LUCID_ZDC",
        "SeededStreamerMuonswBeam",
        "Background",
        ]
    TriggerFlags.GenericSlice.signatures = ['allL1_NoAlg']
    
    # hack for per L1-item streaming
    listOfL1Streamers=[]
    for l1item in Lvl1Flags.items.get_Value():
        l1streamername='L1ItemStreamer_'+l1item
        listOfL1Streamers.append(l1streamername)
        
    from TriggerMenuPython.CommonSliceHelper import  AllowedList
    TriggerFlags.PerL1ItemStreamingSlice.signatures.allowedValues=AllowedList(listOfL1Streamers)
    TriggerFlags.PerL1ItemStreamingSlice.signatures=listOfL1Streamers

    
class Prescales:
    #   Item name             | Prescale
    #----------------------------------------------------------
    L1Prescales = {}
    #   Signature name        |
    #      [ L2prescale, EFprescale, L2pass-through, EF-pass-through]
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
        # Need to have rerun=1:
        'xe30_allL1'               : [    0,    0,    0,    0,  1],  # = any event passed by L1 calo, muon, minbias, random triggers, followed by L2_xe30 in pass-through and EF_xe30 with rerun=1
        'xe30_allL1_allCells'      : [    -0,    -0,    0,    0,  1],  # (same as above, with noise suppression at EF)
        'xe30_allL1_FEB'           : [    -0,    -0,    0,    0,  1],  # (same as above, with FEB headers at EF)
        'xe_NoCut_allL1_noMu_Mon'  : [    0,    0,    0,    0,  1],
        'xe_NoCut_L1MBTS_1_1_noMu_Mon'  : [    0,    0,    0,    0,  1],
        'xe_NoCut_L1MBTS_2_VTE50_noMu_Mon'  : [    0,    0,    0,    0,  1],
        'allL1_NoAlg'              : [   -1,   -1],
        #cosmic chains
        'CosmicsAllTeTRTxK_TRTTrkHypo_Pixel'              : [    -1,    -1,    0,    0],
        'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics'         : [    -1,    -1,    0,    0],
        'CosmicsAllTeIDSCAN_AllPhysics_TrkHypo'           : [    -1,    -1,    0,    0],
        'CosmicsAllTeSiTrack_AllPhysics_TrkHypo'          : [    -1,    -1,    0,    0],
        'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo'     : [    -1,    -1,    0,    0],
        'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo'    : [    -1,    -1,    0,    0],
        'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField' : [    -1,    -1,    0,    0],
        'Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_Pixel'         : [    -1,    -1,    0,    0],
        'CosmicsAllTeEFID'                                : [    -1,    -1,    0,    0],
        'prompt'                                          : [    -1,    -1,    0,    0],
        'allL1_NoAlg'                                     : [    -1,    -1,    0,    0],
        "SeededStreamerRNDM"                              : [    -1,    -1,    0,    0],
        "SeededStreamerL1Calo"                            : [    -1,    -1,    0,    0],
        "SeededStreamerMinBias"                           : [    -1,    -1,    0,    0],
        "SeededStreamerMBTS"                              : [    -1,    -1,    0,    0],
        "SeededStreamerBCM_LUCID_ZDC"                     : [    -1,    -1,    0,    0],
        "SeededStreamerMuonswBeam"                        : [    -1,    -1,    0,    0],
        #L1.5 jets
	"NoCut_a4JE_L15FS_NoEF"                           : [    -1,    -1,    0,    0],
        "NoCut_a4TT_L15FS_NoEF"                           : [    -1,    -1,    0,    0],
        "NoCut_a4JE_L15FS_NoEF_L1TE10"                    : [    -1,    -1,    0,    0],
        "NoCut_a4TT_L15FS_NoEF_L1TE10"                    : [    -1,    -1,    0,    0],
        #HI stuff
    	"g5_loose_EFFS_L1ZDC"                             : [    -1,    -1,    0,    0],
    	"centrality100_60"				  : [    1,    1,    0,    0,  1],
	"centrality80_60"                                 : [    1,    1,    0,    0,  1],
        'mbZdc_a_c_L1TE50_NoAlg'                          : [    1,    1,    0,    0,  1],
        'mbZdc_a_c_L1VTE50_trk'                           : [    1,    1,    0,    0,  1],
        'L1TE50_NoAlg'                                    : [    1,    1,    0,    0,  1],
        'mbZdc_a_c_NoAlg'                                 : [    1,    1,    0,    0,  1],
        'L1ZDC_VTE50_mv_loose2_upc'                       : [    1,    1,    0,    0,  1],
        'L1ZDC_OR_mv_loose2_upc'                          : [    1,    1,    0,    0,  1],
        'L1ZDC_VTE50_mv_medium2_upc'                       : [    1,    1,    0,    0,  1],
        'L1ZDC_OR_mv_medium2_upc'                          : [    1,    1,    0,    0,  1],
        #pPb triggers
        'mbSpTrk_L1ZDC'                                   : [    -1,    -1,    0,    0,  1],
	'mbSpTrk_L1MBTS_1'                                : [    -1,    -1,    0,    0,  1],
	'mbSpTrk_L1MBTS_2'                                : [    -1,    -1,    0,    0,  1],
        'mbSpTrk_L1MBTS_1_1'                              : [    -1,    -1,    0,    0,  1],
        'mbZdc_NoAlg'                                     : [    -1,    -1,    0,    0,  1],
        'mu4_MSonly_EFFS_L1ZDC_OR'                        : [    -1,    -1,    0,    0,  0],
        'mu4_MSonly_EFFS_L1MBTS'                          : [    -1,    -1,    0,    0,  0],
        '2mu2_MSonly_EFFS_L1ZDC_OR'                       : [    -1,    -1,    0,    0,  0],
        '2mu2_MSonly_EFFS_L1MBTS'                         : [    -1,    -1,    0,    0,  0],
        'j15_a2hi_EFFS_L1MBTS'                            : [    -1,    -1,    0,    0,  0],
        'j20_a2hi_EFFS_L1MBTS'                            : [    -1,    -1,    0,    0,  0],
        'j15_a4hi_EFFS_L1MBTS'                            : [    -1,    -1,    0,    0,  0],
        'j20_a4hi_EFFS_L1MBTS'                            : [    -1,    -1,    0,    0,  0],
        'j15_a4hi_EFFS_L1TE10'                            : [    -1,    -1,    0,    0,  0],
        'j20_a4hi_EFFS_L1TE10'                            : [    -1,    -1,    0,    0,  0],
        }


StreamConfig = {
    'HardProbes':      [ 'egamma',
                   'comb__ej', 'comb__emet', 'comb__ejmet', 'comb__eg',
                   'comb__gmet', 
                   'comb__emu', 'comb__gmu',
                   'muons',
                   'comb__muj', 'comb__mumet', 'comb__mujmet',
                   'comb__emu', 'comb__gmu',
                   'jets', 'tauEtmiss',
                   'comb__jmet',
                   'L1Calo', 'EnhancedBias', 'RNDM', 'MuonswBeam', 'JetTauEtmiss'],

    'MinBias':      [  'minbias',
                       'MinBias',
                       'bulk',],
    
    'Background':  [ 'background' ],
    'calibration': [ 'calibration' ], 
    'express':     [ 'express' ],
    'Standby':     [ 'standby' ], 
    }

TriggerGroups = {
    'Primary'             : [
    ],
    'Supporting'          : [
    ],
    'Monitoring'          : [
    ],
    'Calibration'         : [
    ],
    'Backup'              : [
    ],
    'Commissioning'       : [
    ],
    'Test'                : [
    ],
    }

#Define higher groups
#Format is GROUPNAME then in any order SUBGROUPS, ADDITIONAL CHAINS, ALGOS
HigherGroups = {
    #
    #    'MyGroup1'     : [ 'ALG:TrigIDSCAN/*' ],
    #    'MyGroup2'     : [ 'Exclusive_Electrons', 'mb_BM_calib' ],
    #    'MyGroup3'     : [ 'MyGroup2', 'Exclusive_MinBias' ],
    #
    }
