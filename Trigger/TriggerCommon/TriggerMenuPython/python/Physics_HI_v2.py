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
        'J5', 'J5_win6', 'J10', 'J15', 'J18',  'J20', 'J30', 'J50', #'J75', 'J175',  
        'JE100', 'JE200', 'JE300', 'JE500',
        'JB0', 'JB5', 'JB10', 'JB15',
        'JF0', 'JF5', 'JF10', 'JF15',
        'TE0', 'TE10', 'TE20', 'TE35', 'TE50', 'TE65', 'TE90', 'TE100', 
        'MBTS_A', 'MBTS_C',
        'BPTX0'    ,'BPTX1',
        'LUCID_A','LUCID_C',
        'BCM_AtoC' , 'BCM_CtoA', 'BCM_Wide', 'BCM_Comb',
        'ZDC_A', 'ZDC_C', 'ZDC_AND',
        # Direct NIMs
        'NIMDIR5', 'NIMDIR6', 'NIMDIR7', 'NIMDIR8',
        'NIMDIR14', 'NIMDIR25', 'NIMDIR26',
        #LHCf
        'NIMDIR36',
        #CALREQ
        'NIM28', 'NIM29', 'NIM30',
        #ZB
        'ZB_J10',
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
        'HA3', 'HA6', 'HA6I', 'HA8', 'HA11', 'HA11I', 'HA20', 'HA30',
        'XE10', 'XE15', 'XE20', 'XE25', 'XE30', 'XE35', 'XE40', 'XE50',
        'XS25', 'XS30', 'XS35', 'XS40', 'XS45', 'XS50', 'XS60', 'XS65',
        #### Forced for ALFA
        'NIM_B7R1L','NIM_B7R1U','NIM_B7L1L','NIM_B7L1U',
        'NIM_A7R1L','NIM_A7R1U','NIM_A7L1L','NIM_A7L1U',
        'NIM_B7R1L_OD','NIM_B7R1U_OD','NIM_B7L1L_OD','NIM_B7L1U_OD',
        'NIM_A7R1L_OD','NIM_A7R1U_OD','NIM_A7L1L_OD','NIM_A7L1U_OD', 
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
                       'L1_TAU3',
                       #muons
                       'L1_MU0',
                       'L1_MU11',
                       'L1_MU15',
                       'L1_MU20',
                       'L1_MU4',
                       'L1_MU6',
                       'L1_2MU0',
                       'L1_2MU4',
                       'L1_2MU11',
                       'L1_2MU20',
                       'L1_2MU6',
                       #jets
                       'L1_J5',
		       'L1_J5_win6',
		       'L1_2J5',
                       'L1_J10',
		       'L1_2J10',
                       'L1_J15',
		       'L1_J18',
                       'L1_J20',                       
                       'L1_J30',
                       'L1_J50',
                       #'L1_J75',
                       #'L1_J175',
		       'L1_J5_TE90',
		       'L1_2J5_TE90',
                       #jet energy
                       #'L1_JE200',
                       #'L1_JE300',
                       #'L1_JE500',
                       # forward jets
                       'L1_FJ0',
                       'L1_FJ5',
                       'L1_FJ10' ,
                       'L1_FJ10_UNPAIRED_NONISO',
                       'L1_FJ15',
                       'L1_FJ0_A',
                       #'L1_FJ1_A',
                       #'L1_FJ2_A' ,
                       #'L1_FJ3_A',
                       'L1_FJ0_C',
                       #'L1_FJ1_C',
                       #'L1_FJ2_C' ,
                       #'L1_FJ3_C',
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
                       'L1_MBTS_1_1_OVERLAY',
                       #randoms
                       'L1_RD0_FILLED',
                       'L1_RD0_FIRSTEMPTY',
                       'L1_RD0_UNPAIRED_ISO',
                       'L1_RD0_UNPAIRED_NONISO',
                       'L1_RD0_EMPTY',
                       'L1_RD1_FILLED',
                       'L1_RD1_EMPTY',
                       #MET
                       #total energy
                       'L1_TE0',
                       'L1_TE10',
                       'L1_TE20',
                       'L1_TE35',
                       'L1_TE50',
                       'L1_TE65',
                       'L1_TE90',
                       'L1_TE100',
                       #'L1_TE5_NZ',
                       #Min Bias
                       'L1_ZDC',
                       'L1_ZDC_A',
                       'L1_ZDC_C',
                       'L1_ZDC_AND',
                       'L1_ZDC_A_C',
                       'L1_ZDC_A_VTE20',
                       'L1_ZDC_C_VTE20',
                       'L1_BCM_HT_BGRP0','L1_BCM_Wide_BGRP0','L1_BCM_AC_CA_BGRP0',
                       'L1_ZDC_MBTS_1',
                       'L1_ZDC_MBTS_2',
                       'L1_ZDC_MBTS_1_1',
                       'L1_ZDC_MBTS_2_2',
                       #NIMDIR stuff
                       'L1_NIM_S8C2B21', 'L1_NIM_S8C2B22', 'L1_NIM_S8C2B23',  
                       # votoed by TE
                       #UPC triggers
                       'L1_MU0_MV_VTE50',
 	               'L1_MU0_VTE20',
                       'L1_MU0_VTE50',
                       'L1_MU0_TE50',
                       'L1_MU4_MV_VTE50',
                       'L1_MU4_VTE50',
                       'L1_MU4_TE50',
                       'L1_EM3_MV_VTE50',
                       'L1_EM3_VTE20',
                       'L1_EM3_VTE50',
                       # photon+jet
                       'L1_EM3_2J5',
                       ## VDM
                       'L1_MBTS_2_BGRP7','L1_LUCID_BGRP7','L1_BGRP7',
                       'L1_ZDC_A_BGRP7', 'L1_ZDC_C_BGRP7',
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
                       'L1_J5_UNPAIRED_ISO','L1_J5_UNPAIRED_NONISO','L1_J5_EMPTY','L1_J5_FIRSTEMPTY',
                       'L1_J10_UNPAIRED_ISO','L1_J10_UNPAIRED_NONISO','L1_J10_EMPTY','L1_J10_FIRSTEMPTY',
                       'L1_J30_EMPTY', 'L1_J30_UNPAIRED', 'L1_J30_FIRSTEMPTY',
                       #FJ  UNPAIRED-EMPTY-ETC
                       'L1_FJ0_UNPAIRED_ISO', 'L1_FJ0_EMPTY',
                       'L1_FJ5_UNPAIRED_ISO', 'L1_FJ5_EMPTY',
                       #ZDC
                       'L1_ZDC_EMPTY',
                       'L1_ZDC_UNPAIRED_ISO',
                       'L1_ZDC_UNPAIRED_NONISO',
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
                       'L1_LUCID_UNPAIRED_ISO', 'L1_LUCID_UNPAIRED_NONISO', 'L1_LUCID_COMM_UNPAIRED_ISO',
                       'L1_LUCID_A_C_EMPTY',
                       'L1_LUCID_A_C_UNPAIRED_ISO',
                       'L1_LUCID_A_C_UNPAIRED_NONISO',
                       #ZB
                       'L1_ZB',
                       ## BGRP and BPTX
                       'L1_BPTX0_BGRP0', 'L1_BPTX1_BGRP0',
                       #BCM
                       'L1_BCM_Wide_EMPTY','L1_BCM_Wide_UNPAIRED_ISO','L1_BCM_Wide_UNPAIRED_NONISO',
                       'L1_BCM_AC_CA_UNPAIRED_ISO', 'L1_BCM_AC_CA_UNPAIRED_NONISO',
                       ##Calibration requests
                       'L1_CALREQ0','L1_CALREQ1', 'L1_CALREQ2',
                       #Other monitoring
                       'L1_TRT', 'L1_L1A_Mon', 'L1_BGRP1',
                       #LHCf
                       'L1_LHCF',
                       ]

    Lvl1Flags.CtpIdMap =  {
        'L1_EM3' : 0,
        'L1_EM5' : 1,
        'L1_EM7' : 2,
        'L1_EM10' : 3,
        'L1_EM12' : 4,
        'L1_EM14' : 5,
        'L1_2MU4_EMPTY' : 6,
        'L1_RD0_UNPAIRED_NONISO' : 7,
        'L1_BCM_AC_CA_UNPAIRED_NONISO': 8,
        'L1_FJ10_UNPAIRED_NONISO' :  9,
        'L1_2EM3' : 10,
        'L1_2EM5' : 11,
        'L1_MU0_VTE20' : 12,
        'L1_NIM_S8C2B21'            : 13,#DIRNIM
        'L1_NIM_S8C2B22'            : 14,#DIRNIM
        'L1_MBTS_4_4' : 15,
        'L1_RD1_EMPTY': 16,
        'L1_RD0_FILLED' : 17,
        'L1_RD0_FIRSTEMPTY' : 18,
        'L1_RD0_UNPAIRED_ISO' : 19,
        'L1_MBTS_4_4_UNPAIRED_ISO': 20,
        #'L1_2EM3_NL' : 21,
        #'L1_2MU0_NL' : 22,
        #'L1_2J15_NL': 23,
        #'L1_EM3_NL' : 24,
        #'L1_MU0_NL' : 25,
        #'L1_J15_NL': 26,
        'L1_ZDC_A_VTE20' : 27,
        'L1_EM3_VTE20': 28,
        #'L1_2J15': 29,
        #'L1_2J15_NZ' : 30,
        #'L1_2MU0_NZ' : 31,
        #'L1_MU0_MV' : 32,
        #'L1_2MU0_MV' : 33,
        'L1_MU0_MV_VTE50' : 34,
        'L1_MU0_VTE50' : 35,
        'L1_ZDC_C_VTE20' : 36,
        'L1_ZDC_A_C_UNPAIRED_NONISO' : 37,
        'L1_MU4_VTE50' : 38,
        'L1_MU4_MV_VTE50' : 39,
        'L1_MBTS_1_1_OVERLAY' : 40,
        'L1_MU0_TE50' : 41,
        'L1_ZDC_A_C_TE50' : 42,
        'L1_MU4_TE50' : 43,
        'L1_EM3_VTE50' : 44,
        'L1_EM3_MV_VTE50' : 45,
        #'L1_EM3_TE50' : 46,
        'L1_EM3_2J5' : 46,
        'L1_BGRP7' : 47,
        'L1_LUCID_BGRP7' : 48,
        'L1_MBTS_2_BGRP7' : 49,
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
        'L1_TAU3' : 64,
        'L1_MBTSC3' : 65,
        'L1_EM3_FIRSTEMPTY' : 66,
        'L1_MU4_FIRSTEMPTY' : 67,
        'L1_MU6_FIRSTEMPTY' : 68,
        'L1_2MU0_EMPTY' : 69,
        'L1_MU0_FIRSTEMPTY' : 70,
        'L1_2MU6' : 71,
        'L1_2MU11' : 72,
        'L1_2MU20' : 73,
        #'L1_3MU6' : 74,
        'L1_MU11_EMPTY' : 75,
        'L1_MBTSC2' : 76,
        'L1_LUCID_UNPAIRED_NONISO' : 77,
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
        'L1_J5_win6' : 100,
	#'L1_J75' : 100,
        'L1_J50' : 102,
	'L1_J18' : 104,
        #'L1_J175' : 104,
        'L1_J5' : 105,
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
        'L1_FJ0' : 123,
        'L1_2MU6_UNPAIRED_ISO' : 124,
        'L1_2MU6_UNPAIRED_NONISO' : 125,
        'L1_BCM_Wide_UNPAIRED_NONISO' : 126,
        'L1_EM3_UNPAIRED_ISO' : 127,
        'L1_EM3_UNPAIRED_NONISO' : 128,
        'L1_J10_FIRSTEMPTY' : 129,
        'L1_J30_FIRSTEMPTY' : 130,
        'L1_MBTSA2' : 131,
        'L1_TE0' : 132,
        'L1_TE10' : 133,
        'L1_TE20' : 134,
        'L1_TE50' : 135,
        'L1_TE65' : 136,
        'L1_MBTSA3' : 137,
        'L1_TE35' : 138,
        'L1_2J5' : 139,
        'L1_2J10' : 140,
        'L1_TE100' : 141,
        'L1_TE90' : 142,
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
        'L1_FJ0_EMPTY' : 175,
        'L1_MBTSA14' : 176,
        'L1_EM3_EMPTY' : 177,
        'L1_MBTSA15' : 178,
        'L1_EM5_EMPTY' : 179,
        'L1_FJ0_UNPAIRED_ISO' : 180,
        'L1_FJ5_UNPAIRED_ISO' : 181,
        'L1_ZDC_UNPAIRED_ISO' : 182,
        'L1_ZDC_UNPAIRED_NONISO' : 183,
        'L1_J5_EMPTY' : 184,
        'L1_J5_FIRSTEMPTY' : 185,
        'L1_J5_UNPAIRED_ISO' : 186,
        'L1_J5_UNPAIRED_NONISO' : 187,
        'L1_ZDC_A_BGRP7' : 188,
        'L1_ZDC_AND' : 189,
        'L1_ZDC_A' : 190,
        'L1_ZDC_C' : 191,
        'L1_ZDC_A_C' : 192,
        'L1_ZDC' : 193,
        'L1_ZDC_C_BGRP7' : 194,
        #'L1_ZDC_C_EMPTY' : 195,
        'L1_ZDC_A_C_EMPTY' : 196,
        'L1_ZDC_EMPTY' : 197,
        'L1_FJ5' : 198,
        'L1_FJ10' : 199,
        'L1_MBTS_1_UNPAIRED_ISO':    200,
        'L1_MBTS_1_UNPAIRED_NONISO':    201,        
        'L1_MBTS_1_1_UNPAIRED_ISO':  202,
        'L1_FJ15':  203,        
        'L1_MBTS_2_UNPAIRED_ISO':    204,
        'L1_MBTS_2_UNPAIRED_NONISO':    205,
        'L1_LUCID_A_C_UNPAIRED_NONISO':  206,
        'L1_LUCID_A_C_UNPAIRED_ISO':  208,
        'L1_LUCID_A_UNPAIRED':    209,
        'L1_LUCID_C_UNPAIRED':    210,         
        'L1_LUCID_A_C_UNPAIRED':  211,
        'L1_LUCID_A' : 212,
        'L1_LUCID_C' : 213,
        'L1_LUCID_A_C' : 214,
        'L1_LUCID' : 215,
        #'L1_FJ2_A' : 216,
        #'L1_FJ3_A' : 217,
        'L1_FJ5_EMPTY' : 218,
        'L1_FJ0_C' : 219,
        #'L1_FJ1_C' : 220,
        #'L1_FJ2_C' : 221,
        'L1_MBTS_2' : 222,
        'L1_MBTS_2_2' : 223,
        'L1_MBTS_3_3' : 224,
        'L1_BCM_HT_BGRP0' : 225,
        'L1_MBTS_1' : 226,
        #'L1_FJ3_C' : 227,
        'L1_MBTS_1_1' : 228,
        'L1_MBTS_2_2_EMPTY' : 229,
        'L1_MBTS_3_3_EMPTY' : 230,
        'L1_MBTS_2_2_UNPAIRED_ISO' : 231,
        'L1_MBTS_3_3_UNPAIRED_ISO' : 232,
        'L1_J5_TE90' : 233,
        'L1_2J5_TE90' : 234,
        #'L1_MBLZ' : 235,
        'L1_LUCID_A_EMPTY' : 236,
        'L1_LUCID_C_EMPTY' : 237,
        'L1_LUCID_A_C_EMPTY' : 238,
        'L1_FJ0_A' : 239,
        #'L1_FJ1_A' : 240,
        'L1_BPTX0_BGRP0' : 241,
        'L1_BPTX1_BGRP0' : 242,
        #'L1_BPTX0_BPTX1_BGRP0':243,
        #'' : 244,
        'L1_LUCID_EMPTY' : 245,
        #'L1_BCM_MUL_5_EMPTY' : 246,
        'L1_LHCF' : 247,
        #'L1_BGRP0' : 248,
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
        'mbMbts_2',
        'mbMbts_2_unpaired_iso',
        'mbMbts_2_eff',
        'mbMbts_2_eff_unpaired_iso',
        #
        'mbMbts_1_1',
        'mbMbts_1_1_unpaired_iso',
        'mbMbts_1_1_eff_HI',
        'mbMbts_1_1_eff_unpaired_iso_HI',
        'mbMbts_1_1_overlay',
        'zerobias_Overlay_L2mbMbts',
        #'mbRmMbts_1_NoCut_Time',
        #'mbMbts_2_Monitoring',
        #L1_ZDC_A_C stuff
        'mbZdc_a_c_NoAlg',
        'mbZdc_a_c_eff_HI',
        'mbZdc_a_c_eff_unpaired_iso_HI',
        'mbZdc_a_c_cosmic_NoAlg',
        #'mbZdc_eff_unpaired_HI',
        'mbLucid_eff',
        'mbLucid_eff_unpaired_iso',
        'mbSpBg_unpaired_iso_HI',
        'mbSpTrk',
        'mbSpTrk_empty',
	'mbSpTrk_unpaired_iso',
        'mbSp_HI_L1BGRP1',
        'mbRd1_eff_HI',
        'rd1_empty_NoAlg',
	'rd0_empty_NoAlg',
        'zerobias_NoAlg',
        'zerobias_Overlay_NoAlg', 
        'mbRd0_eff_unpaired_iso_HI',
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
        'L1TE0_NoAlg', 'L1TE10_NoAlg', 'L1TE20_NoAlg',
	'L1TE35_NoAlg', 'L1TE50_NoAlg', 'L1TE65_NoAlg', 'L1TE90_NoAlg',
        'L1TE100_NoAlg', 
        #pPb
        'L1MBTS_2_NoAlg', 'L1MBTS_1_1_NoAlg',
        'L1ZDC_NoAlg', 'L1ZDC_A_NoAlg', 'L1ZDC_C_NoAlg',
	#LHCf
	'L1LHCF_NoAlg',
        # for overlay
        'L1MBTS_1_1_overlay_NoAlg',
        #pA triggers
        'mbSpTrk_L1ZDC',
        'mbSpTrk_L1ZDC_A',
        'mbSpTrk_L1ZDC_C',
	'mbSpTrk_L1MBTS_1',
	'mbSpTrk_L1MBTS_2',
        'mbSpTrk_L1MBTS_1_1',
        'mbSpTrk_L1TE0',
        'mbSpTrk_L1FJ0',
        'mbSpTrk_L1FJ0_A',
        'mbSpTrk_L1FJ0_C',
	'mbSpTrk_L1LHCF',
	# high-multiplicity triggers for pp at 2.76TeV
	'mbSpTrkVtxMh',
 	'mbSpTrkVtxMh_eff',
        #pPb high-multiplicity triggers
        'mbSpTrkVtxMh_hip_trk100_L1TE10', 'mbSpTrkVtxMh_hip_trk110_L1TE10', 'mbSpTrkVtxMh_hip_trk120_L1TE10',
        'mbSpTrkVtxMh_hip_trk130_L1TE10',
        'mbSpTrkVtxMh_hip_trk140_L1TE20', 'mbSpTrkVtxMh_hip_trk150_L1TE20', 'mbSpTrkVtxMh_hip_trk155_L1TE20',
        'mbSpTrkVtxMh_hip_trk160_L1TE20',
        'mbSpTrkVtxMh_hip_trk170_L1TE35', 'mbSpTrkVtxMh_hip_trk175_L1TE35', 'mbSpTrkVtxMh_hip_trk180_L1TE35',
        'mbSpTrkVtxMh_hip_trk190_L1TE35',
        'mbSpTrkVtxMh_hip_trk140_L1TE50', 'mbSpTrkVtxMh_hip_trk150_L1TE50', 'mbSpTrkVtxMh_hip_trk155_L1TE50',
        'mbSpTrkVtxMh_hip_trk160_L1TE50',
        'mbSpTrkVtxMh_hip_trk170_L1TE50', 'mbSpTrkVtxMh_hip_trk175_L1TE50', 'mbSpTrkVtxMh_hip_trk180_L1TE50',
        'mbSpTrkVtxMh_hip_trk190_L1TE50',
        'mbSpTrkVtxMh_hip_trk180_L1TE65', 'mbSpTrkVtxMh_hip_trk190_L1TE65', 'mbSpTrkVtxMh_hip_trk200_L1TE65', 
        'mbSpTrkVtxMh_hip_trk210_L1TE65',
        'mbSpTrkVtxMh_hip_trk175_L1MBTS',
        'mbSpTrkVtxMh_hip_trk220_L1TE65', 'mbSpTrkVtxMh_hip_trk225_L1TE65', 'mbSpTrkVtxMh_hip_trk230_L1TE65',
        'mbSpTrkVtxMh_hip_trk240_L1TE65', 'mbSpTrkVtxMh_hip_trk245_L1TE65',
	#TE90
	'mbSpTrkVtxMh_hip_trk200_L1TE90', 'mbSpTrkVtxMh_hip_trk210_L1TE90',
        'mbSpTrkVtxMh_hip_trk220_L1TE90', 'mbSpTrkVtxMh_hip_trk230_L1TE90',
        'mbSpTrkVtxMh_hip_trk240_L1TE90', 'mbSpTrkVtxMh_hip_trk260_L1TE90', 
        #HMT with pile up suppression
        'mbSpTrkVtxMh_hip_trk140_L1TE20_pileupSup',
        'mbSpTrkVtxMh_hip_trk160_L1TE50_pileupSup',
        'mbSpTrkVtxMh_hip_trk180_L1TE50_pileupSup',
        'mbSpTrkVtxMh_hip_trk200_L1TE65_pileupSup',
        'mbSpTrkVtxMh_hip_trk240_L1TE90_pileupSup',
        'mbSpTrkVtxMh_hip_trk260_L1TE90_pileupSup',
        #HMT for pp at 2.76TeV
        'mbSpTrkVtxMh_pp_trk30_L1TE0_pileupSup', 'mbSpTrkVtxMh_pp_trk30_L1TE10_pileupSup', 'mbSpTrkVtxMh_pp_trk30_L1TE20_pileupSup',
        'mbSpTrkVtxMh_pp_trk50_L1TE10',
        'mbSpTrkVtxMh_pp_trk50_L1MBTS', 'mbSpTrkVtxMh_pp_trk50_L1TE0',
        'mbSpTrkVtxMh_pp_trk60_L1TE10',
        'mbSpTrkVtxMh_pp_trk60_L1MBTS', 'mbSpTrkVtxMh_pp_trk60_L1TE0',
        'mbSpTrkVtxMh_pp_trk70_L1TE10', 'mbSpTrkVtxMh_pp_trk70_L1TE0',
        'mbSpTrkVtxMh_pp_trk70_L1TE20',
        'mbSpTrkVtxMh_pp_trk80_L1TE10', 'mbSpTrkVtxMh_pp_trk80_L1TE0',
        'mbSpTrkVtxMh_pp_trk80_L1TE20',
        'mbSpTrkVtxMh_pp_trk90_L1TE10', 'mbSpTrkVtxMh_pp_trk90_L1TE0',
        'mbSpTrkVtxMh_pp_trk100_L1TE10', 'mbSpTrkVtxMh_pp_trk110_L1TE10', 'mbSpTrkVtxMh_pp_trk120_L1TE10', 'mbSpTrkVtxMh_pp_trk130_L1TE10',
        'mbSpTrkVtxMh_pp_trk90_L1TE20',
        'mbSpTrkVtxMh_pp_trk100_L1TE20', 'mbSpTrkVtxMh_pp_trk100_L1TE0',
        'mbSpTrkVtxMh_pp_trk100_L1TE35',
        'mbSpTrkVtxMh_pp_trk110_L1TE20',
        'mbSpTrkVtxMh_pp_trk110_L1TE35',
        'mbSpTrkVtxMh_pp_trk120_L1TE20',
        'mbSpTrkVtxMh_pp_trk120_L1TE35',
        'mbSpTrkVtxMh_pp_trk130_L1TE20',
        'mbSpTrkVtxMh_pp_trk130_L1TE35',
        'mbSpTrkVtxMh_pp_trk50_L1TE10_pileupSup',
        'mbSpTrkVtxMh_pp_trk50_L1MBTS_pileupSup', 'mbSpTrkVtxMh_pp_trk50_L1TE0_pileupSup',
        'mbSpTrkVtxMh_pp_trk60_L1TE10_pileupSup',
        'mbSpTrkVtxMh_pp_trk60_L1MBTS_pileupSup', 'mbSpTrkVtxMh_pp_trk60_L1TE0_pileupSup',
        'mbSpTrkVtxMh_pp_trk70_L1TE10_pileupSup', 'mbSpTrkVtxMh_pp_trk70_L1TE0_pileupSup',
        'mbSpTrkVtxMh_pp_trk70_L1TE20_pileupSup',
        'mbSpTrkVtxMh_pp_trk80_L1TE10_pileupSup', 'mbSpTrkVtxMh_pp_trk80_L1TE0_pileupSup',
        'mbSpTrkVtxMh_pp_trk80_L1TE20_pileupSup',
        'mbSpTrkVtxMh_pp_trk90_L1TE10_pileupSup', 'mbSpTrkVtxMh_pp_trk90_L1TE0_pileupSup',
        'mbSpTrkVtxMh_pp_trk100_L1TE10_pileupSup', 'mbSpTrkVtxMh_pp_trk110_L1TE10_pileupSup', 'mbSpTrkVtxMh_pp_trk120_L1TE10_pileupSup',
        'mbSpTrkVtxMh_pp_trk130_L1TE10_pileupSup',
        'mbSpTrkVtxMh_pp_trk90_L1TE20_pileupSup',
        'mbSpTrkVtxMh_pp_trk100_L1TE20_pileupSup', 'mbSpTrkVtxMh_pp_trk100_L1TE0_pileupSup',
        'mbSpTrkVtxMh_pp_trk100_L1TE35_pileupSup',
        'mbSpTrkVtxMh_pp_trk110_L1TE20_pileupSup',
        'mbSpTrkVtxMh_pp_trk110_L1TE35_pileupSup',
        'mbSpTrkVtxMh_pp_trk120_L1TE20_pileupSup',
        'mbSpTrkVtxMh_pp_trk120_L1TE35_pileupSup',
        'mbSpTrkVtxMh_pp_trk130_L1TE20_pileupSup',
        'mbSpTrkVtxMh_pp_trk130_L1TE35_pileupSup',
        # SCT noise suppression
        'mbSpTrk_noiseSup',
        'mbSpTrk_noiseSup_unpaired_iso',
        'rd0_filled_NoAlg',
        'mbSpBg_noiseSup_unpaired_iso',
	#ZDC at L2
        'mbZdc_LG_eff',
        'mbZdc_HG_eff',
        'mbZdc_HG_eff_L1MBTS2',
        'mbZdc_HG_sideA',
        'mbZdc_HG_sideC',
        'mbZdc_LG_sideA',
        'mbZdc_LG_sideC',
        'mbZdc_LG_sideA_L1MBTS2',
        'mbZdc_LG_sideC_L1MBTS2',
        ]
    
    # HLT Muons
    TriggerFlags.MuonSlice.signatures = [
        #MSonly
        'mu4_MSonly', 'mu4T_MSonly',
        'mu6_MSonly', 'mu6T_MSonly',
        'mu10_MSonly', 'mu10T_MSonly',
        'mu13_MSonly', 'mu13T_MSonly',
        '2mu6_MSonly', '2mu6T_MSonly',
        '2mu4_MSonly', '2mu4T_MSonly',
        #InDet monitoring
        'mu4_IDTrkNoCut',
        'mu4T_IDTrkNoCut',
        'mu6_IDTrkNoCut',
	'mu6T_IDTrkNoCut',
	'mu8_IDTrkNoCut',
        'mu10_IDTrkNoCut',
        # barrel only
        'mu4_MSonly_barrel',
        'mu4T_MSonly_barrel',
        #EFFS
        '2mu2_MSonly_EFFS_L1MU0',
        '2mu4_MSonly_EFFS_L1MU0',
	'2mu2_EFFS_L1MU0', 'mu4_mu2_EFFS_L1MU0',
        '2mu2_EFFS', 'mu4_mu2_EFFS',
        #pPb and pp triggers
	'mu2',
        'mu4', 'mu4T',
        'mu6', 'mu6T',
	'mu8',
#        'mu8_newl2muonSA',
        'mu10',
	'2mu4',
        #pPb low lumi
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
        'j10_u0uchad_LArNoiseBurst',
        'j10_u0uchad_firstempty_LArNoiseBurst',
        'j10_u0uchad_empty_LArNoiseBurst',        
        # 0.2 hi jets seeded of L1_J10
        'L1J10_NoAlg',
        # 0.4 hi jets
        'j10_a4hi_EFFS_L1J5',
        'j10_a4hi_EFFS_L1EM3',
        'j10_a4hi_EFFS_L1TAU3',
#         'j7_a4hi_EFFS_L1MBTS', 'j10_a4hi_EFFS_L1MBTS', 'j15_a4hi_EFFS_L1MBTS',
#         'j20_a4hi_EFFS_L1MBTS', 'j30_a4hi_EFFS_L1MBTS', 'j40_a4hi_EFFS_L1MBTS',
#         'j50_a4hi_EFFS_L1MBTS',
        'j10_a4hi_EFFS_L2mbMbts', 'j15_a4hi_EFFS_L2mbMbts',
        'j20_a4hi_EFFS_L2mbMbts', 'j30_a4hi_EFFS_L2mbMbts', 'j40_a4hi_EFFS_L2mbMbts',
        'j50_a4hi_EFFS_L2mbMbts',
        # MultiJets
        '3j15_a4tchad_L2FS_2L1J5',
        '3j15_a4tchad_L2FS',
        '2j15_a4tchad_L2FS',
        # Random seded jets
#        'j10_a4hi_EFFS_L2mbMbts_L1RD0',
        'j10_a4tchad_EFFS_L2mbMbts_L1RD0',
        'j15_a4tchad_EFFS_L2mbMbts_L1RD0',
        'j20_a4tchad_EFFS_L2mbMbts_L1RD0',
        'j30_a4tchad_EFFS_L2mbMbts_L1RD0',
 #       'j10_a4hi_EFFS_L2te_L1RD0',
        'j10_a4tchad_EFFS_L2te_L1RD0',
        'j15_a4tchad_EFFS_L2te_L1RD0',
        'j20_a4tchad_EFFS_L2te_L1RD0',
        'j30_a4tchad_EFFS_L2te_L1RD0',
 #       'j10_a4hi_EFFS_L1RD0',
        'j10_a4tchad_EFFS_L1RD0',
        'j15_a4tchad_EFFS_L1RD0',
        'j20_a4tchad_EFFS_L1RD0',
        'j30_a4tchad_EFFS_L1RD0',
        
        

        
        # high-mult jets
        'j10_a4hi_EFFS_L1TE90', 'j10_a4hi_EFFS_L1TE100',
        '2j10_a4hi_EFFS_2L1J5_L1TE90', '2j10_a4hi_EFFS_L1TE90', '2j10_a4hi_EFFS_L1TE100',
        '2j10_a4tchad_eta50_deta40_L2mbMbts_L1RD0',
        '2j10_a4tchad_deta40_L2mbMbts_L1RD0',
        '2j10_a4tchad_eta50_deta40_L2te_L1RD0',
        '2j10_a4tchad_deta40_L2te_L1RD0',
        '2j10_a4tchad_eta50_deta40_L1RD0',
        '2j10_a4tchad_deta40_L1RD0',
        #
        'j20_a4hi_EFFS_L1J10',
        'j30_a4hi_EFFS_L1J10',
        'j40_a4hi_EFFS_L1J10',
        'j50_a4hi_EFFS_L1J10',
        'j75_a4hi_EFFS_L1J10',
        'j60_a4hi_EFFS_L1J15',
        'j75_a4hi_EFFS_L1J15',
        'j80_a4hi_EFFS_L1J15', 
        'j90_a4hi_EFFS_L1J15',
        'j100_a4hi_EFFS_L1J20',
	## pp at 2.76TeV
#        'j7_a4tchad_EFFS_L1MBTS', 'j10_a4tchad_EFFS_L1MBTS', 'j15_a4tchad_EFFS_L1MBTS',
#        'j20_a4tchad_EFFS_L1MBTS', 'j30_a4tchad_EFFS_L1MBTS', 'j40_a4tchad_EFFS_L1MBTS', 'j50_a4tchad_EFFS_L1MBTS',
        'j7_a4tchad_EFFS_L2mbMbts', 'j10_a4tchad_EFFS_L2mbMbts', 'j15_a4tchad_EFFS_L2mbMbts',
        'j20_a4tchad_EFFS_L2mbMbts', 'j30_a4tchad_EFFS_L2mbMbts', 'j40_a4tchad_EFFS_L2mbMbts', 'j50_a4tchad_EFFS_L2mbMbts',        
	#pp jets for pPb
        'j50_a4tchad_EFFS_L1J10', 'j60_a4tchad_EFFS_L1J10', 'j75_a4tchad_EFFS_L1J10',
        #
        'j50_a4tchad_EFFS_L1J15', 'j60_a4tchad_EFFS_L1J15', 'j75_a4tchad_EFFS_L1J15',
        'j80_a4tchad_EFFS_L1J15', 'j90_a4tchad_EFFS_L1J15', 'j100_a4tchad_EFFS_L1J20',
	#calibrated jets
	'j7_a4tchad_EFFS_L1J5', 'j10_a4tchad_EFFS_L1J5',
        'j20_a4tchad_EFFS_L1J5', 'j30_a4tchad_EFFS_L1J5', 'j40_a4tchad_EFFS_L1J5',
        # deta dor dijets
        '2j10_a4hi_eta50_deta40', '2j15_a4hi_eta50_deta40',
	# had jets
#	'2j7_a4tchad_deta40_L1MBTS', '2j10_a4tchad_deta40_L1MBTS', '2j15_a4tchad_deta40_L1MBTS',
#        '2j7_a4tchad_eta50_deta40_L1MBTS', '2j10_a4tchad_eta50_deta40_L1MBTS', '2j15_a4tchad_eta50_deta40_L1MBTS',
#        '2j7_a4hi_deta40_L1MBTS', '2j10_a4hi_deta40_L1MBTS', '2j15_a4hi_deta40_L1MBTS',
#        '2j7_a4hi_eta50_deta40_L1MBTS', '2j10_a4hi_eta50_deta40_L1MBTS', '2j15_a4hi_eta50_deta40_L1MBTS',
	'2j10_a4tchad_deta40_L2mbMbts', '2j15_a4tchad_deta40_L2mbMbts',
        '2j10_a4tchad_eta50_deta40_L2mbMbts', '2j15_a4tchad_eta50_deta40_L2mbMbts',
        '2j10_a4hi_deta40_L2mbMbts', '2j15_a4hi_deta40_L2mbMbts',
        '2j10_a4hi_eta50_deta40_L2mbMbts', '2j15_a4hi_eta50_deta40_L2mbMbts',

        '2j10_a4tchad_EFFS_L2mbMbts_L1RD0',
        '2j15_a4tchad_EFFS_L2mbMbts_L1RD0',
        '2j10_a4tchad_EFFS_L2te_L1RD0',
        '2j15_a4tchad_EFFS_L2te_L1RD0',
        '2j10_a4tchad_EFFS_L1RD0',
        '2j15_a4tchad_EFFS_L1RD0',
        '3j15_a4tchad_EFFS_L1RD0',
        '3j15_a4tchad_EFFS_L2te_L1RD0',
        '3j15_a4tchad_EFFS_L2mbMbts_L1RD0',
        
	#di-jets        
        '2j7_a4hi_EFFS_2L1J5', '2j10_a4hi_EFFS_2L1J5', '2j15_a4hi_EFFS_2L1J5',
        #had jets
#	'2j7_a4tchad_EFFS_L1MBTS', '2j10_a4tchad_EFFS_L1MBTS', '2j15_a4tchad_EFFS_L1MBTS', '2j20_a4tchad_EFFS_L1MBTS',
#        '2j7_a4hi_EFFS_L1MBTS', '2j10_a4hi_EFFS_L1MBTS', '2j15_a4hi_EFFS_L1MBTS', '2j20_a4hi_EFFS_L1MBTS',
	'2j7_a4tchad_EFFS_L2mbMbts', '2j10_a4tchad_EFFS_L2mbMbts', '2j15_a4tchad_EFFS_L2mbMbts', '2j20_a4tchad_EFFS_L2mbMbts',
        '2j7_a4hi_EFFS_L2mbMbts', '2j10_a4hi_EFFS_L2mbMbts', '2j15_a4hi_EFFS_L2mbMbts', '2j20_a4hi_EFFS_L2mbMbts',

        #forward jets
#	'fj7_a4tchad_EFFS_L1MBTS', 'fj7_a4hi_EFFS_L1MBTS',
        'fj7_a4tchad_EFFS_L1FJ0', 'fj10_a4tchad_EFFS_L1FJ0', 'fj15_a4tchad_EFFS_L1FJ0',
        'fj7_a4hi_EFFS_L1FJ0', 'fj10_a4hi_EFFS_L1FJ0', 'fj15_a4hi_EFFS_L1FJ0',
	'fj10_a4hi_EFFS_L2mbMbts', 'fj15_a4hi_EFFS_L2mbMbts',
        'fj10_a4tchad_EFFS_L2mbMbts', 'fj15_a4tchad_EFFS_L2mbMbts',

        'fj10_a4tchad_EFFS_L2mbMbts_L1RD0',
        'fj15_a4tchad_EFFS_L2mbMbts_L1RD0',
        'fj10_a4tchad_EFFS_L2te_L1RD0',
        'fj15_a4tchad_EFFS_L2te_L1RD0',
        'fj10_a4tchad_EFFS_L1RD0',
        'fj15_a4tchad_EFFS_L1RD0',
        # streamers
        #'L1J75_NoAlg',
        #'L1J175_NoAlg',
        #cosmic
        'L1J5_NoAlg',
        'L1TAU3_NoAlg',
        'L1J5_empty_NoAlg',
        'L1J5_firstempty_NoAlg',
        'L1J10_empty_NoAlg',  
        'L1J10_firstempty_NoAlg',  
        'L1J30_empty_NoAlg',  
        'L1J30_firstempty_NoAlg',
        'L1FJ0_empty_NoAlg',
        'L1FJ5_empty_NoAlg',
        'j30_u0uchad_empty_LArNoiseBurst',
        ]
    
    # HLT MET
    TriggerFlags.METSlice.signatures = [
	#'xe_NoCut_allL1_noMu_Mon',
        ]

    
    # HLT HI (whatever which can't be attained by the regular slices)
    TriggerFlags.HeavyIonSlice.signatures = [
	#p+Pb upc
	'L1MU0_VTE20_hip_loose2_upc',
        'L1EM3_VTE20_hip_loose2_upc',
        'L1EM3_VTE50_hip_loose2_upc',
        'L1ZDC_A_VTE20_hip_loose2_upc',
        'L1ZDC_C_VTE20_hip_loose2_upc',
        #peripheral events
        'fte75_veto',
        'fte75_veto_mbMbts_2_2',
        #centrality triggers
        'centrality100_60',
        'centrality80_60',
        #met for p+Pb
	'xe20_FEB_L1TE65',
        'xe20_FEB_L1TE90',
        'xe20_FEB_L1EM5',
        'xe20_FEB_L1J10', 
        'xe20_FEB_L1MU0',
        'xe25_FEB_L1TE65',
        'xe25_FEB_L1TE90',
        'xe30_FEB_L1TE65',
        'xe30_FEB_L1TE90',
        'xe30_FEB_L1EM5', 
        'xe30_FEB_L1J10', 
        'xe30_FEB_L1MU0', 
        'xe40_FEB_L1TE65',
        'xe40_FEB_L1TE90',
        'xe40_FEB_L1EM5',
        'xe40_FEB_L1J10',
        'xe40_FEB_L1MU0',
        'zerobias_Overlay_L2te',
        ]
    
    # calibration
    TriggerFlags.CalibSlice.signatures = [
        'g5_larcalib','g10_larcalib',
        'j25_larcalib', 'j50_larcalib', #'j95_larcalib', 'j200_larcalib',
        'fj30_larcalib',
        #'fj25_larcalib', 
        # LVL1-based chains
        'em3_empty_larcalib', 'em5_empty_larcalib', 'tau8_empty_larcalib',
        'j5_empty_larcalib', 'j10_empty_larcalib',
        'fj0_empty_larcalib', 'fj5_empty_larcalib',
        #'fj10_empty_larcalib',
        ]
    TriggerFlags.MonitorSlice.signatures = [
        'cost_monitor',
        'InDetMon_FS',
        #HLT Monitoring
        'ROBRequestMon',
        'ROBRequestMonUnseeded',
        'HLTTimeBurner',
        #TRT Monitoring
        'IDMon_filled',
        'IDMon_empty',
        'IDMon_unpaired_iso',
        ]

    TriggerFlags.BeamSpotSlice.signatures = [
        'beamspot_vertex_FSTracks_L2Star_B',
        'beamspot_vertex_activeTE_L2Star_B_peb',
        'beamspot_vertex_allTE_L2Star_B_peb',
        ]

    TriggerFlags.EgammaSlice.signatures = [
        #single photons
        'g5_loose', 'g7_loose', 'g10_loose',
        'g5_medium',
        'g7_medium', #config needs to be fixed
        'g10_medium',
	'g12_loose', 'g12_medium',
        'g15_loose', 'g15_medium',
        'g20_loose', 'g20_medium',
	#tight photons
        'g5_tight', 'g7_tight', 'g9_tight', 'g10_tight', 'g12_tight', 'g15_tight',
	#photon+jet at L1
	'g5_loose_2L1J5', 'g5_medium_2L1J5',
        #single electrons
        'e5_loose1', 'e7_loose1', 'e10_loose1',
        'e5_medium1', 'e7_medium1', 'e10_medium1',
        'e5_tight1', 'e7_tight1', 'e9_tight1', 'e14_tight1',
	'e15_loose0', 'e15_loose1', 'e15_medium1',
        #ID monitoring
	'e5_medium1_IDTrkNoCut', 'e15_loose1_IDTrkNoCut',
	#di-electrons
	'2e5_loose1', '2e5_medium1', '2e5_tight1',
        'e9_tight1_e5_etcut',
        # ID monitoring
	'e5_tight1_e5_NoCut',
        #di-photons
        '2g5_loose', '2g5_medium',
        '2g7_loose', '2g7_medium',
        '2g10_loose', '2g10_medium',
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
        'mu4_MSonly_j15_a4tchad_EFFS_matched',
	'mu4_j10_a4tchad_EFFS_matched', 'mu4_j10_a4hi_EFFS_matched',
	'mu4_j15_a4tchad_EFFS_matched', 'mu4_j15_a4hi_EFFS_matched',
        'mu4_j20_a4tchad_EFFS_matched', 'mu4_j20_a4hi_EFFS_matched',
        'mu4_j30_a4tchad_EFFS_matched', 'mu4_j30_a4hi_EFFS_matched',
        'mu4_j40_a4tchad_EFFS_matched', 'mu4_j40_a4hi_EFFS_matched',
        ]

    TriggerFlags.BphysicsSlice.signatures = [
	#ID monitoring
	'2mu4_Jpsimumu_IDTrkNoCut',
	]

    TriggerFlags.CombinedSlice.signatures = [
        'g7_medium_2j10_a4tchad_EFFS_L1EM3', 'g10_medium_2j10_a4tchad_EFFS_L1EM5',
        'g7_medium_2j10_a4hi_EFFS_L1EM3', 'g10_medium_2j10_a4hi_EFFS_L1EM5',
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
        'PreS_L1J5_empty',
        'PreS_L1J10_empty',
        'PreS_L1J30_empty',
        'PreS_L1J10_firstempty',
        'PreS_L1J30_firstempty',
        #VdM scan
        'VdM_BGRP7',
        'VdM_LUCID_BGRP7',
        'VdM_MBTS_2_BGRP7',
        'VdM_MBTS_2_UNPAIRED_ISO',
        'VdM_RD0_UNPAIRED_ISO',
        'VdM_ZDC_A_BGRP7',
        'VdM_ZDC_C_BGRP7',
        'VdM_mbMbts_1_1_MBTS2_BRGP7',
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
        "Background_RD0_UNPAIRED_ISO",
        "Background_RD0_UNPAIRED_NONISO",
        "Background_RD0_EMPTY",
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
        'e5_medium1'               : [    1,    1,    0,    0,   1],
        'e5_loose1'               : [    1,    1,    0,    0,   1],
        'e7_loose1'               : [    1,    1,    0,    0,   1],
        'mu6'                     : [    1,    1,    0,    0,   1],
	'mbMbts_1_1'               : [    1,    1,    0,    0,   100],
        'g5_loose'               : [    1,    1,    0,    0,   1],
        'g7_loose'               : [    1,    1,    0,    0,   1],
        'g10_loose'               : [    1,    1,    0,    0,   1],
        'g12_loose'               : [    1,    1,    0,    0,   1],
        'g15_loose'               : [    1,    1,    0,    0,   1],
        'g5_medium'               : [    1,    1,    0,    0,   1],
        'g7_medium'               : [    1,    1,    0,    0,   1],
        'g10_medium'               : [    1,    1,    0,    0,   1],
        'g12_medium'               : [    1,    1,    0,    0,   1],
        'g15_medium'               : [    1,    1,    0,    0,   1],
        'g5_tight'               : [    1,    1,    0,    0,   1],
        'g7_tight'               : [    1,    1,    0,    0,   1],
        'g10_tight'               : [    1,    1,    0,    0,   1],
        'g12_tight'               : [    1,    1,    0,    0,   1],
        'g15_tight'               : [    1,    1,    0,    0,   1],
        #upc
        'L1ZDC_A_VTE20_hip_loose2_upc'      : [    1,    1,    0,    0,   100],
        'L1ZDC_C_VTE20_hip_loose2_upc'      : [    1,    1,    0,    0,   100],
	#HMT
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
        #'mbSpTrk_L1ZDC'                                   : [    -1,    -1,    0,    0,  1],
	#'mbSpTrk_L1MBTS_1'                                : [    -1,    -1,    0,    0,  1],
	#'mbSpTrk_L1MBTS_2'                                : [    -1,    -1,    0,    0,  1],
        #'mbSpTrk_L1MBTS_1_1'                              : [    -1,    -1,    0,    0,  1],
        #'mbZdc_NoAlg'                                     : [    -1,    -1,    0,    0,  1],
        #'mu4_MSonly_EFFS_L1ZDC_OR'                        : [    -1,    -1,    0,    0,  0],
        #'mu4_MSonly_EFFS_L1MBTS'                          : [    -1,    -1,    0,    0,  0],
        #'2mu2_MSonly_EFFS_L1ZDC_OR'                       : [    -1,    -1,    0,    0,  0],
        #'2mu2_MSonly_EFFS_L1MBTS'                         : [    -1,    -1,    0,    0,  0],
        #'j15_a2hi_EFFS_L1MBTS'                            : [    -1,    -1,    0,    0,  0],
        #'j20_a2hi_EFFS_L1MBTS'                            : [    -1,    -1,    0,    0,  0],
        #'j15_a4hi_EFFS_L1MBTS'                            : [    -1,    -1,    0,    0,  0],
        #'j20_a4hi_EFFS_L1MBTS'                            : [    -1,    -1,    0,    0,  0],
        #'j15_a4hi_EFFS_L1TE10'                            : [    -1,    -1,    0,    0,  0],
        #'j20_a4hi_EFFS_L1TE10'                            : [    -1,    -1,    0,    0,  0],
        }


StreamConfig = {
    'HardProbes':  [ 'egamma',
                   'comb__ej', 'comb__emet', 'comb__ejmet', 'comb__eg',
                   'comb__gmet', 
                   'comb__emu', 'comb__gmu',
                   'muons',
                   'comb__muj', 'comb__mumet', 'comb__mujmet',
                   'comb__emu', 'comb__gmu',
                   'bphysics',
                   'jets', 'tauEtmiss',
                   'comb__jmet',
                   'met',
                   'L1Calo', 'EnhancedBias', 'MuonswBeam', 'JetTauEtmiss'],

    'MinBias':      [  'minbias',
                       'MinBias',
                       'bulk',
                       'RNDM',  ],
    
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
