# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def defineMenu():
    """
    Defines the following LvlFlags:
    
    thresholds   .... list of all threshold names in the menu (includes forced thresholds)
    items        .... list of all L1 item names in the menu
    CtpIdMap     .... map of item names to CTP IDs
    """
    
    from TriggerMenu.l1.Lvl1Flags import Lvl1Flags

    Lvl1Flags.CTPVersion = 4 # new CTP

    Lvl1Flags.BunchGroupPartitioning = [1, 11, 14] # partition 1: 1-10, partition 2: 11-13, partition 3: 14-15 (note that BGRP0 is used by all items)
    Lvl1Flags.BunchGroupNames = ['BCRVeto', 'Filled', 'Calib', 'Empty', 'UnpairedBeam1', 'UnpairedBeam2', 'FirstEmpty', 'InTrain']
    Lvl1Flags.BunchGroupNames += ['NotUsed'] * len(Lvl1Flags.BunchGroupNames())

    Lvl1Flags.MenuPartitioning = [0, 472, 492] # partition 1: ctpid 0-471, partition 2: ctpid 472-491, partition 3: ctpid 492-511

    Lvl1Flags.RemapThresholdsAsListed = True

    Lvl1Flags.thresholds = [
        #------------------------------------
        #------------------------------------
        #      !!!!!!!IMPORTANT!!!!!!!!!
        # Please make sure to use the right
        # order to get the mapping right!!!
        #------------------------------------
        #------------------------------------
        'MU0', 'MU4', 'MU6', 'MU11', 'MU15', 'MU20', 
        'EM3', 'EM4', 'EM5', 'EM7', 'EM10', 'EM12', 'EM14', 'EM16',
        'J10', 'J15', 'J20', 'J30', 'J50', 'J75', 'J175', 'J250', 
        'JE100', 'JE200', 'JE300', 'JE500',
        # 'JB10', 'JB30', 'JB55', 'JB95', # skipped as JB|JF thresholds are replaced by FJ.
        # 'JF10', 'JF30', 'JF55', 'JF95',
        'FJ10', # added instead of JB | JF
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
        'MBTS_A8'  , ##'MBTS_A9', 
        'MBTS_A10', ##'MBTS_A11', 
        'MBTS_A12' , ##'MBTS_A13', 
        'MBTS_A14', ##'MBTS_A15', 
        'MBTS_C0'  , 'MBTS_C1', 'MBTS_C2', 'MBTS_C3', 
        'MBTS_C4'  , 'MBTS_C5', 'MBTS_C6', 'MBTS_C7', 
        'MBTS_C8'  , ##'MBTS_C9', 
        'MBTS_C10', ##'MBTS_C11', 
        'MBTS_C12' , ##'MBTS_C13', 
        'MBTS_C14', ##'MBTS_C15',
        #forced thresholds
        #'HA5', 'HA6', 'HA6I',
        'HA8',
        #'HA11', 'HA11I', 'HA20', 'HA30',
        ## 'XE10', 'XE15', 'XE20', 'XE25', 'XE30', 'XE35', 'XE40', 'XE50',
        ## 'XS25', 'XS30', 'XS35', 'XS40', 'XS45', 'XS50', 'XS60', 'XS65',
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
                       # temporary commented out in HI_v3: 'L1_NIM_S8C2B21', 'L1_NIM_S8C2B22', 'L1_NIM_S8C2B23',  
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
                       'L1_MBTSA5', 'L1_MBTSA6', 'L1_MBTSA7', 'L1_MBTSA8', ##'L1_MBTSA9', 11, 13, 15 not in run2 anymore
                       'L1_MBTSA10', ##'L1_MBTSA11', 
                       'L1_MBTSA12', ##'L1_MBTSA13', 
                       'L1_MBTSA14',##'L1_MBTSA15',
                       'L1_MBTSC0', 'L1_MBTSC1', 'L1_MBTSC2', 'L1_MBTSC3', 'L1_MBTSC4',
                       'L1_MBTSC5', 'L1_MBTSC6', 'L1_MBTSC7', 'L1_MBTSC8', ##'L1_MBTSC9',
                       'L1_MBTSC10', ##'L1_MBTSC11', 
                       'L1_MBTSC12', ##'L1_MBTSC13', 
                       'L1_MBTSC14',
                       ##'L1_MBTSC15',
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
                       'L1_MBTS_2_2_EMPTY',
                       'L1_MBTS_3_3_EMPTY',
                       'L1_MBTS_1_1_UNPAIRED_ISO',
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
                       # BGRP and BPTX
                       'L1_BPTX0_BGRP0', 'L1_BPTX1_BGRP0',
                       'L1_BGRP0',
                       'L1_BGRP1',
                       #BCM
                       'L1_BCM_Wide_EMPTY','L1_BCM_Wide_UNPAIRED_ISO','L1_BCM_Wide_UNPAIRED_NONISO',
                       'L1_BCM_AC_CA_UNPAIRED_ISO',
                       ##Calibration requests
                       ## temporary skipped 'L1_CALREQ0','L1_CALREQ1', 'L1_CALREQ2',
                       #Other monitoring
                       ## temporary skipped 'L1_TRT', 'L1_L1A_Mon',
                       ]

    Lvl1Flags.CtpIdMap = {}
    
    Lvl1Flags.prescales = {}
    
