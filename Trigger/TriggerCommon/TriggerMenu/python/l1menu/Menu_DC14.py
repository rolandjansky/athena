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

    Lvl1Flags.CTPVersion = 3 # new CTP

    Lvl1Flags.BunchGroupPartitioning = [1, 11, 14] # partition 1: 1-10, partition 2: 11-13, partition 3: 14-15 (note that BGRP0 is used by all items)
    Lvl1Flags.BunchGroupNames = ['BCRVeto', 'Filled', 'Calib', 'Empty', 'UnpairedBeam1', 'UnpairedBeam2', 'FirstEmpty', 'InTrain']

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

        # 6 x MU
        'MU4', 'MU6', 'MU10', 'MU11', 'MU15', 'MU20',

        # 9 x EM
        'EM3', 'EM7V', 'EM8VH', 'EM15V', 'EM15VH', 'EM18VH','EM20V', 'EM24VHI', 'EM50',

        # 8 x TAU
        'HA6', 'HA8', 'HA12', 'HA15', 'HA20', 'HA40', 'HA60',

        # 8 x JET
        'J12', 'J15', 'J15.ETA24', 'J20', 'J25', 'J40', 'J75', 'J100', 

        # 4 x JE
        'JE140', 'JE200', 'JE350','JE500',

        ## 4 x JB
        'JB15.23ETA49', 'JB20.32ETA49', 'JB25.32ETA49', 'JB100.32ETA49',

        # 4 x JF
        'JF15.23ETA49', 'JF20.32ETA49', 'JF25.32ETA49', 'JF100.32ETA49',

        # 8 x TE
        'TE20', 'TE30', 'TE40', 'TE700', 'TE800', 'TE1000', 'TE1200', 'TE1400', # --> the last three are left in as a forced threshold.

        # 8 x XS
        'XS30', 'XS35', 'XS45', 'XS50', 'XS55', 'XS60', 'XS65',

        # 8 x XE
        'XE35', 'XE40', 'XE45', 'XE50', 'XE60', 'XE70', 'XE80', 'XE90',
        
        # 2 x MBTS
        'MBTS_A', 'MBTS_C',

        # 2  x BPTX
        'BPTX0','BPTX1',
        
        # 2 x LUCID
        'LUCID_A','LUCID_C',

        # 4 x BCM
        'BCM_AtoC', 'BCM_CtoA', 'BCM_Wide', 'BCM_Comb',

        # 3 x ZDC
        'ZDC_A', 'ZDC_C', 'ZDC_AND',

        # LUCID_COMM 
        'NIMDIR5',

        # TRT
        'NIMDIR25',

        # L1A_MON
        'NIMDIR26',

        # LHCF
        'NIMDIR36',
        
        # 3 x CALREQ
        'NIM28','NIM29','NIM30', 'NIM_MUB', 'NIM_MUE',

        # 1 x ZB
        #'ZB_EM12',
        'ZB_EM15V',

        # Forced for MBTSSI
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
        'NIM_ALFA_LOG', 'NIM_ALFA_LOG_OD',

        ]


    Lvl1Flags.items = [

        # MU
        'L1_MU6','L1_MU20',
        'L1_2MU4', 'L1_2MU6',  'L1_2MU10', 'L1_3MU6',

        #EM
        # single EM 
 	'L1_EM3', 'L1_EM7V', 'L1_EM8VH', 'L1_EM15V', 'L1_EM15VH', 'L1_EM18VH', 'L1_EM20V', 'L1_EM24VHI', 'L1_EM50',         

        # multi lepton
        "L1_2EM3", "L1_2EM15V", 
        "L1_2EM15VH",
        "L1_2EM8VH",
        "L1_EM7V_2EM3",
        "L1_EM15VH_3EM7V",
        'L1_EM15VH_MU10', 'L1_2EM8VH_MU10', 'L1_EM8VH_2MU6',

        # TAU
        'L1_TAU6', 'L1_TAU8', 'L1_TAU12', 'L1_TAU20', 'L1_TAU40', 'L1_TAU60',

        # JET
        'L1_J20', 'L1_J40', 'L1_J75', 'L1_J100', 

        # multi - jet
        'L1_3J15', 'L1_3J20', 'L1_4J20', 
        'L1_3J15.ETA24', 'L1_5J15.ETA24', 
   
        # forward jet
        'L1_J20.32ETA49',  'L1_J100.32ETA49',

        # combined tau - lepton
        "L1_EM15VH_TAU40_2TAU15", "L1_EM15VH_2TAU12_J25_2J15_3J12",

        "L1_MU10_TAU12_J25_2J12", "L1_MU10_TAU20",

        # combined tau - jet
        "L1_TAU20_2TAU12_J25_2J20_3J12",

        # combined tau - xe
        "L1_TAU20_2J20_XE45", "L1_TAU20_2TAU12_XE35",

        # combined MU - jet

        # combined muon - xe

        # combined jet - xe
        'L1_J40_XE50',
        
        # multi - lepton

        # combined em - xe

        # combined e - jet
        
        'L1_EM15VH_J15.23ETA49',

        # XS

        # XE
        'L1_XE35', 'L1_XE50', 'L1_XE70',
        
        # TE
        "L1_TE20","L1_TE30","L1_TE40",  "L1_TE700", "L1_TE800", 

        # RNDM
        'L1_RD0_FILLED', 'L1_RD0_FIRSTEMPTY', 'L1_RD0_UNPAIRED_ISO', 'L1_RD0_EMPTY', 'L1_RD1_FILLED', 'L1_RD1_EMPTY',

        # LHCF
        'L1_LHCF',

        # BCM
        'L1_BCM_HT_BGRP0', 'L1_BCM_Wide_BGRP0', 'L1_BCM_AC_CA_BGRP0', 'L1_BCM_Wide_EMPTY','L1_BCM_Wide_UNPAIRED_ISO','L1_BCM_Wide_UNPAIRED_NONISO', 'L1_BCM_AC_CA_UNPAIRED_ISO',
        # ZDC
        'L1_ZDC','L1_ZDC_A_C', 'L1_ZDC_EMPTY', 'L1_ZDC_A_C_EMPTY',

        # MBTS
        "L1_MBTS_1","L1_MBTS_2", "L1_MBTS_4_4", "L1_MBTS_2_UNPAIRED_ISO",

        #LUCID
        "L1_LUCID", "L1_LUCID_A_C", "L1_LUCID_A_C_EMPTY", "L1_LUCID_A_C_UNPAIRED_ISO", "L1_LUCID_A_C_UNPAIRED_NONISO", "L1_LUCID_COMM", "L1_LUCID_COMM_EMPTY", "L1_LUCID_COMM_UNPAIRED_ISO", "L1_LUCID_EMPTY", "L1_LUCID_UNPAIRED_ISO",
        
        # ZB
        'L1_ZB',
        
        # BGRP and BPTX
        'L1_BPTX0_BGRP0', 'L1_BPTX1_BGRP0',
        
        # Calibration requests
        'L1_CALREQ0', 'L1_CALREQ1', 'L1_CALREQ2',
        
        # Other monitoring
        'L1_TRT', 'L1_L1A_Mon',
        
        # ALFA
        'L1_ALFA_EMPTY','L1_ALFA_BGRP7', 'L1_ALFA_EMPTY_OD','L1_ALFA_BGRP7_OD',
        
        # VDM
        'L1_MBTS_2_BGRP7','L1_LUCID_BGRP7','L1_BGRP7',
        # VBF
 
        ]



    ### the CTP ID mapping still has to be defined, currently it is taken from the order of the items

    Lvl1Flags.CtpIdMap = {
        }
    
    Lvl1Flags.prescales = {}
    
