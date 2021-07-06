# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
#------------Just a copy of Physics_pp_v3 with empty HLT-----------------#
#------------------------------------------------------------------------#
def defineMenu():

    from TriggerMenuMT.LVL1MenuConfig.LVL1.Lvl1Flags import Lvl1Flags

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
    

    Lvl1Flags.CtpIdMap =  {}



    
