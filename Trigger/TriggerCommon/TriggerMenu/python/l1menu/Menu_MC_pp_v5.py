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

        # 6 x MU
        'MU4', 'MU6', 'MU10', 'MU11', 'MU15', 'MU20',

        # 16 x EM
        'EM3', 'EM7', 'EM8VH', 'EM8I', 'EM10', 'EM10VH', 'EM12', 'EM13VH', 'EM15', 'EM15VH', 'EM15I', 'EM15HI', 'EM18VH', 'EM20VH', 'EM20VHI', 'EM22VHI', 


        # 9 x TAU
        'HA8', 'HA10IM','HA12', 'HA12IL', 'HA12IM', 'HA12IT','HA15', 'HA20', 'HA20IL',  'HA20IM', 'HA20IT','HA25','HA25IT', 'HA30', 'HA40', 'HA60', 

        # JETs and central jets (first 10 can have multiplicity 4 or more)
        'J5', 'J12', 'J15', 'J15.0ETA25','J20', 'J25','J25.0ETA23','J30', 
        #2 x VBF
        'J20.0ETA49', 'J30.0ETA49',

        'JJ15.23ETA49','J15.29ETA32', 'J20.29ETA32',
        
        # those 15 can have maximum multiplicity of 3
        'J40','J50','J75', 'J85', 'J100', 'J120',

        # 6 x FJ              
        'J15.32ETA49', 'J20.32ETA49', 'J30.32ETA49', 'J50.32ETA49', 'J75.32ETA49', 'J100.32ETA49', 


        ##ONLY TWO ETA RANGES FOR TE and XE thresholds: 0-7 full eta and 8-15 restricted,         
        # 8 x TE
        'TE5','TE20', 'TE30', 'TE40', 'TE50', #'TE60', 'TE2000', 'TE4000' the last three are left in as a forced threshold.
        
        #ETA-RESTRICTED
        'TE30.0ETA25',
        

        # 8 x XS
        'XS20', 'XS30', 'XS40', 'XS45', 'XS50', 'XS55', 'XS60', 'XS65', 
        # 'XS25', 'XS30', 'XS35', 'XS45', 'XS50', 'XS55', 'XS60', 'XS65',

        # 8 x XE
        'XE35', 'XE40', 'XE45', 'XE50', 'XE55', 'XE60', 'XE70', 'XE80',

        # 2 x MBTS
        'MBTS_A', 'MBTS_C',
        #'MBTS_A0','MBTS_A1','MBTS_A2','MBTS_A3', 'MBTS_A4', 'MBTS_A5', 'MBTS_A6', 'MBTS_A7','MBTS_C0', 'MBTS_C1', 'MBTS_C2', 'MBTS_C3', 'MBTS_C4', 'MBTS_C5', 
        #'MBTS_C6','MBTS_C7, 

        # 2  x BPTX
        #'BPTX0','BPTX1',

        # 2 x LUCID
        'LUCID_A','LUCID_C',# 'LUCID_COMM', more than 2 lucids can currently not be loaded

        # 4 x BCM
        'BCM_AtoC', 'BCM_CtoA', 'BCM_Wide', 'BCM_Comb',

        # 3 x ZDC
        'ZDC_A', 'ZDC_C',#'ZDC_AND',

        # TRT
        'NIMTRT',

        # TGC
        'NIMTGC',
        
        # L1A for CTP monitoring itself
        #'NIML1A',

        # LHCF
        'NIMLHCF',

        # 3 x CALREQ
        'CAL0','CAL1','CAL2',

        # 2 x ZB
        'ZB_EM15', # 'ZB_J20',


        # Forced for MBTSSI
        'MBTS_A0', 'MBTS_A1', 'MBTS_A2', 'MBTS_A3', 'MBTS_A4'  , 'MBTS_A5', 'MBTS_A6', 'MBTS_A7', 
        'MBTS_A8',  'MBTS_A10', 'MBTS_A12' , 'MBTS_A14',
        'MBTS_C0', 'MBTS_C1', 'MBTS_C2', 'MBTS_C3', 'MBTS_C4'  , 'MBTS_C5', 'MBTS_C6', 'MBTS_C7', 
        'MBTS_C8',  'MBTS_C10', 'MBTS_C12' , 'MBTS_C14',


        #-------------------------------------------------------------------
        #L1 TOPO inputs 
        #-------------------------------------------------------------------
        # M8
        ### '4INVM9999-AJ0s6-AJ0s6',
        ### '3INVM9999-AJ0s6-AJ0s6',
        ### '2INVM9999-AJ0s6-AJ0s6',
        ### '1INVM9999-AJ0s6-AJ0s6',

        ### 'HT200-AJ20s5.ETA49',
        ### 'HT190-AJ20s5.ETA49',
        ### 'HT1-AJ0all.ETA49',
        ### '1INVM5-EM6s2-EMall',
        ### '1INVM5-EMs2-EMs',
        ### '1INVM5-EM6s2-EMs',
        ### '1INVM5-EM12s2-EMs',
        
        #HT
        'HT190-AJ15all.ETA20',
        'HT190-J15s5.ETA20',
        'HT150-AJ20all.ETA30',
        'HT150-J20s5.ETA30',

        #ZH 
        '10MINDPHI-Js2-XE50', '10MINDPHI-J20s2-XE50', '10MINDPHI-J20ab-XE50', '10MINDPHI-CJ20ab-XE50',        
        #Jpsi T&P
        '1INVM5-EMs2-EMall', '1INVM5-EM7s2-EMall', '1INVM5-EM12s2-EMall',        

        #W T&P
        '05MINDPHI-AJj20s6-XE0',
        '10MINDPHI-AJj20s6-XE0',
        '15MINDPHI-AJj20s6-XE0',
        '10MINDPHI-EM10s6-XE0',
        '15MINDPHI-EM10s6-XE0',
        '05MINDPHI-EM15s6-XE0',
        '05RATIO-XE0-HT0-AJj15all.ETA49',
        '08RATIO-XE0-HT0-AJj15all.ETA49',
        '90RATIO2-XE0-HT0-AJj15all.ETA49',
        '250RATIO2-XE0-HT0-AJj15all.ETA49',
        'HT20-AJj15all.ETA49',
        'NOT-02MATCH-EM10s1-AJj15all.ETA49',
        '05RATIO-XE0-SUM0-EM10s1-HT0-AJj15all.ETA49',
        '25MT-EM10s6-XE0',
        '30MT-EM10s6-XE0',
        '35MT-EM15s6-XE0',
        #'10MINDPHI-AJj15s2-XE0',
        #'20MINDPHI-AJjs6-XE0',
        #'20MINDPHI-AJj15s2-XE0',
        #'10MINDPHI-EM6s1-XE0',
        #'20MINDPHI-EM9s6-XE0',
        #'20MINDPHI-EM6s1-XE0',
        #'05RATIO-XE0-HT0-AJj15all.ETA49',
        #'08RATIO-XE0-HT0-AJj0all.ETA49',
        #'40RATIO2-XE0-HT0-AJj15all.ETA49',
        #'90RATIO2-XE0-HT0-AJj0all.ETA49',
        #'HT20-AJj0all.ETA49',
        #'NOT-02MATCH-EM9s1-AJj15all.ETA49',
        #'05RATIO-XE0-SUM0-EM9s1-HT0-AJj15all.ETA49',
        #'20MT-EM6s6-XE0',
        #'30MT-EM6s6-XE0',
        #'40MT-EM6s6-XE0',
        
        # central muon
        #'MULT-CMU4ab',
        #'MULT-CMU6ab',
        
        #B-jet
        '0DR04-MU4ab-CJ15ab', '0DR04-MU4ab-CJ30ab', '0DR04-MU6ab-CJ25ab', '0DR04-MU4ab-CJ17ab', '0DR04-MU4ab-CJ20ab',
        #B-physics
        '2DR15-2MU4ab', '2INVM999-2MU4ab', '4INVM8-2MU4ab',
        '2DR15-2MU6ab', '2INVM999-2MU6ab', '4INVM8-2MU6ab',

        # L1 thresholds for L1Topo streamers
        '2INVM999-CMU4ab-MU4ab',
        '2INVM999-2CMU4ab',
        '2INVM999-MU6ab-MU4ab',
        '2INVM999-ONEBARREL-MU6ab-MU4ab',
        '2INVM999-CMU6ab-CMU4ab',
        '4INVM8-CMU4ab-MU4ab',
        '4INVM8-2CMU4ab',
        '4INVM8-MU6ab-MU4ab',
        '4INVM8-ONEBARREL-MU6ab-MU4ab',
        '4INVM8-CMU6ab-CMU4ab',        
        '2DR99-2MU4ab',
        '5DETA99-5DPHI99-2MU4ab',
        '5DETA99-5DPHI99-MU6ab-MU4ab',
        '5DETA99-5DPHI99-2MU6ab',
        '0DR10-MU10ab-MU6ab',
        '0DETA04-0DPHI03-EM8abi-MU10ab',
        '0DETA04-0DPHI03-EM15abi-MUab',
        '10MINDPHI-Js2-XE50',
        '10MINDPHI-J20s2-XE50',
        '10MINDPHI-J20ab-XE50',
        '10MINDPHI-CJ20ab-XE50',
        '900INVM9999-AJ30s6-AJ20s6',
        '800INVM9999-AJ30s6-AJ20s6',
        '700INVM9999-AJ30s6-AJ20s6',
        '500INVM9999-AJ30s6-AJ20s6',
        '400INVM9999-AJ30s6-AJ20s6',
        '350INVM9999-AJ30s6-AJ20s6',
        '300INVM9999-AJ30s6-AJ20s6',
        '200INVM9999-AJ30s6-AJ20s6',
        '100INVM9999-AJ30s6-AJ20s6',
        '350INVM9999-J30s6-J20s6',
        '300INVM9999-J30s6-J20s6',
        '250INVM9999-J30s6-J20s6',
        '200INVM9999-J30s6-J20s6',
        'HT150-AJj15all.ETA49',
        '0DETA10-Js1-Js2',
        '0MATCH-4AJ20.ETA32-4AJj15',
        '210RATIO-0MATCH-TAU30si2-EMall',
        'NOT-0MATCH-TAU30si2-EMall',
        '0DR28-MU10ab-TAU12abi',
        '1DISAMB-TAU12abi-J25ab',
        'DISAMB-EM15abhi-TAU40ab',
        '1DISAMB-TAU20ab-J20ab',
        'DISAMB-EM15abhi-TAU12abi',
        '1DISAMB-EM15his2-TAU12abi-J25ab',
        '1DISAMB-J25ab-0DR28-EM15his2-TAU12abi',
        '1DISAMB-TAU20abi-TAU12abi-J25ab',
        '0DR28-TAU20abi-TAU12abi',
        '0DETA20-0DPHI20-TAU20abi-TAU12abi',
        '1DISAMB-J25ab-0DR28-TAU20abi-TAU12abi',
        'LAR-EM50s1',
        'LAR-J100s1',
        '2DR15-CMU4ab-MU4ab',
        '2DR15-2CMU4ab',
        '2DR15-MU6ab-MU4ab',
        '2DR15-ONEBARREL-MU6ab-MU4ab',
        '2DR15-CMU6ab-CMU4ab',
        'KF-XE35-AJall',
        'KF-XE45-AJall',
        'KF-XE55-AJall',
        'KF-XE60-AJall',
        'KF-XE65-AJall',
        'KF-XE75-AJall',
        ]

    
    Lvl1Flags.items = [

        # single EM

        'L1_EM3','L1_EM7', 'L1_EM12', 'L1_EM8VH', 'L1_EM10','L1_EM10VH', 'L1_EM13VH', 'L1_EM8I', 'L1_EM15', 'L1_EM15I', 'L1_EM15HI','L1_EM15VH', 'L1_EM18VH', 'L1_EM20VH', 'L1_EM20VHI', 'L1_EM22VHI',# see Jira ATR-10334'L1_EM22VHLIL', #'L1_EM50V',
        'L1_EM3_EMPTY', 'L1_EM7_EMPTY', 'L1_EM3_UNPAIRED_ISO',
        'L1_EM15_BGRP7',

        # see savannah https://savannah.cern.ch/bugs/?103935

        # combined EM -jet
        #'L1_JJ15.23ETA49', # 4x4 window

        # single MU
        "L1_MU4", "L1_MU6", "L1_MU10", "L1_MU11", "L1_MU15", "L1_MU20",
        "L1_MU4_EMPTY", "L1_MU4_FIRSTEMPTY", "L1_MU11_EMPTY",
        'L1_MU4_UNPAIRED_ISO',

        # multi lepton
        "L1_2EM3", "L1_2EM7", "L1_2EM8VH", "L1_2EM10VH", "L1_2EM13VH", "L1_2EM15", "L1_2EM15VH", "L1_EM7_2EM3", "L1_EM12_2EM3", "L1_EM15VH_3EM7",
        "L1_2MU4", "L1_2MU6", "L1_2MU10", "L1_MU10_2MU6", "L1_MU11_2MU6", "L1_MU10_2MU4", "L1_3MU4", "L1_MU6_2MU4", "L1_3MU6", 
        "L1_EM7_MU10", "L1_EM15_MU4", "L1_2EM8VH_MU10", "L1_EM15VH_MU10", 


        # single tau
        "L1_TAU12", "L1_TAU12IL",  "L1_TAU12IM",  "L1_TAU12IT", "L1_TAU20", "L1_TAU20IL", "L1_TAU20IM", "L1_TAU20IT", 
        "L1_TAU30", "L1_TAU40", "L1_TAU60", "L1_TAU8", "L1_TAU8_EMPTY",


        # multi tau
        "L1_TAU20IM_2TAU12IM", "L1_TAU20_2TAU12", 

        # combined tau - lepton
        "L1_EM15HI_2TAU12",  
        "L1_EM15HI_2TAU12IM",          
        "L1_EM15HI_2TAU12IM_J25_3J12",   
        "L1_EM15HI_2TAU12_J25_3J12",   
        "L1_EM15HI_TAU40_2TAU15",
        'L1_J25_3J12_EM15-TAU12I',  
        "L1_MU10_TAU12",  
        "L1_MU10_TAU12IM",  
        "L1_MU10_TAU12IM_J25_2J12", 
        "L1_MU10_TAU12IL_J25_2J12", 
        "L1_MU10_TAU12_J25_2J12", 
        'L1_J25_2J12_DR-MU10TAU12I',  
        "L1_MU10_TAU20",
        "L1_MU10_TAU20IM",

        # combined tau - jet
        "L1_TAU25IT_2TAU12IT_2J25_3J12", 

        "L1_TAU20IL_2TAU12IL_J25_2J20_3J12", 
        "L1_TAU20IM_2TAU12IM_J25_2J20_3J12", 
        "L1_TAU20_2TAU12_J25_2J20_3J12",
        'L1_J25_2J20_3J12_BOX-TAU20ITAU12I',  
        'L1_J25_2J20_3J12_DR-TAU20ITAU12I',  
        'L1_DR-MU10TAU12I_TAU12I-J25', 
        "L1_MU10_TAU12I-J25",


        # combined tau - xe
        "L1_TAU20IM_2J20_XE45", 
        "L1_TAU20_2J20_XE45", 
        "L1_TAU25_2J20_XE45",   
        "L1_TAU20IM_2J20_XE50",   
        'L1_XE45_TAU20-J20',    

        #"L1_EM15HI_2TAU12I_XE35", 
        "L1_EM15HI_2TAU12IM_XE35",  
        "L1_EM15HI_2TAU12IL_XE35",  
        "L1_EM15HI_2TAU12_XE35",  
        'L1_XE35_EM15-TAU12I', 
        'L1_XE40_EM15-TAU12I', 


        "L1_MU10_TAU12_XE35",    
        "L1_MU10_TAU12IM_XE35",  
        "L1_MU10_TAU12IL_XE35",  
        "L1_MU10_TAU12IT_XE35",  
        "L1_MU10_TAU12IM_XE40",  

        "L1_TAU20IM_2TAU12IM_XE35", 
        "L1_TAU20IL_2TAU12IL_XE35", 
        "L1_TAU20IT_2TAU12IT_XE35", 
        "L1_TAU20_2TAU12_XE35",
        "L1_TAU20IM_2TAU12IM_XE40",  



        # combined em - jet
        #'L1_EM15VH_J15.23ETA49',
        'L1_EM15VH_JJ15.23ETA49',

        # combined mu - jet
        'L1_MU4_J12', 'L1_MU6_J20', 'L1_MU6_J40', 'L1_MU6_J75',
        'L1_MU4_3J20',

        # single jet
        "L1_J12","L1_J15","L1_J20","L1_J25", "L1_J30", "L1_J40", "L1_J50","L1_J75","L1_J85", "L1_J100", "L1_J120",#"L1_J400",
        "L1_J20.32ETA49", "L1_J30.32ETA49", "L1_J50.32ETA49", "L1_J75.32ETA49", "L1_J100.32ETA49",
        #'L1_J15.23ETA49',
        'L1_J15.29ETA32', 'L1_J15.32ETA49',
        "L1_J20.29ETA32", 
        "L1_J12_EMPTY","L1_J12_FIRSTEMPTY", "L1_J12_UNPAIRED_ISO", "L1_J12_UNPAIRED_NONISO", "L1_J12_ABORTGAPNOTCALIB", 
        "L1_J30_EMPTY", "L1_J30_FIRSTEMPTY", "L1_J30.32ETA49_EMPTY", "L1_J30.32ETA49_FIRSTEMPTY","L1_J30.32ETA49_UNPAIRED_ISO",
        "L1_J30.32ETA49_UNPAIRED_NONISO",
        "L1_J50_UNPAIRED_ISO", "L1_J50_UNPAIRED_NONISO", "L1_J50_ABORTGAPNOTCALIB",         

        # multi jet
        "L1_3J15", "L1_3J20","L1_3J40", 'L1_3J15.0ETA25', "L1_3J50", "L1_4J15", "L1_4J20", "L1_3J75", "L1_4J30", "L1_6J15",
        "L1_J75_3J20", "L1_J30.0ETA49_2J20.0ETA49",
        
        # multi jet central
        "L1_3J25.0ETA23", "L1_4J15.0ETA25", "L1_5J15.0ETA25", 
        
        # combined jet
        "L1_2J15_XE55", "L1_J40_XE50", "L1_J75_XE40",
        
        # XE
        "L1_XE35", "L1_XE40", "L1_XE45", "L1_XE50", "L1_XE55", "L1_XE60", "L1_XE70", "L1_XE80", 
        "L1_XE35_BGRP7", "L1_XE40_BGRP7", "L1_XE45_BGRP7", "L1_XE50_BGRP7", "L1_XE55_BGRP7", "L1_XE60_BGRP7", "L1_XE70_BGRP7", "L1_XE80_BGRP7", 
        
        #XS
        'L1_XS20', 'L1_XS30', 'L1_XS40', 'L1_XS45', 'L1_XS50', 'L1_XS55', 'L1_XS60', 'L1_XS65', 
        
        # TE
        "L1_TE20", "L1_TE30", "L1_TE40", "L1_TE50", "L1_TE30.0ETA25",  


        # MBTS
        "L1_MBTS_1", "L1_MBTS_2", "L1_MBTS_1_1",
        "L1_MBTS_1_EMPTY", "L1_MBTS_1_UNPAIRED_ISO", 
        "L1_MBTS_2_EMPTY", "L1_MBTS_2_UNPAIRED_ISO",
        "L1_MBTS_1_1_EMPTY", "L1_MBTS_1_1_UNPAIRED_ISO",
        'L1_MBTSA0' ,'L1_MBTSA1' ,'L1_MBTSA2' ,'L1_MBTSA3' ,'L1_MBTSA4' ,
        'L1_MBTSA5' ,'L1_MBTSA6' ,'L1_MBTSA7' ,'L1_MBTSA8' ,
        'L1_MBTSA10','L1_MBTSA12','L1_MBTSA14',
        'L1_MBTSC0' ,'L1_MBTSC1' ,'L1_MBTSC2' ,'L1_MBTSC3' ,'L1_MBTSC4' ,
        'L1_MBTSC5' ,'L1_MBTSC6' ,'L1_MBTSC7' ,'L1_MBTSC8' ,
        'L1_MBTSC10','L1_MBTSC12','L1_MBTSC14',
        # beam splashes
        "L1_MBTS_4_A", "L1_MBTS_4_C",

        # RNDM
        'L1_RD0_FILLED', 'L1_RD0_UNPAIRED_ISO','L1_RD0_EMPTY',
        "L1_RD1_FILLED","L1_RD1_EMPTY",
        "L1_RD2_FILLED","L1_RD2_EMPTY",
        "L1_RD3_FILLED","L1_RD3_EMPTY",
        'L1_RD0_FIRSTEMPTY', 

        #LUCID
        'L1_LUCID', 'L1_LUCID_EMPTY', 'L1_LUCID_UNPAIRED_ISO', 
        'L1_LUCID_A_C_EMPTY', 'L1_LUCID_A_C_UNPAIRED_ISO', 'L1_LUCID_A_C_UNPAIRED_NONISO',
        #'L1_LUCID_COMM_EMPTY', 'L1_LUCID_COMM_UNPAIRED_ISO', 

        # VDM
        "L1_BGRP7",

        # TRT
        "L1_TRT",
        "L1_TRT_FILLED",
        "L1_TRT_EMPTY",

        # TGC
        "L1_TGC_BURST",

        # LHCF
        "L1_LHCF",
        "L1_LHCF_MBTS_1",
        "L1_LHCF_MBTS_2",
        
        #CALREQ
        "L1_CALREQ2",

        #ZB
        "L1_ZB",
        # "L1_ZB_J20",

        # BCM
        'L1_BCM_Wide_BGRP0','L1_BCM_AC_CA_BGRP0', 'L1_BCM_Wide_EMPTY','L1_BCM_Wide_UNPAIRED_ISO','L1_BCM_Wide_UNPAIRED_NONISO', 'L1_BCM_AC_CA_UNPAIRED_ISO',
        'L1_BCM_AC_UNPAIRED_ISO','L1_BCM_CA_UNPAIRED_ISO',
        'L1_BCM_AC_UNPAIRED_NONISO','L1_BCM_CA_UNPAIRED_NONISO',
        'L1_BCM_AC_ABORTGAPNOTCALIB', 'L1_BCM_CA_ABORTGAPNOTCALIB','L1_BCM_Wide_ABORTGAPNOTCALIB',
        'L1_BCM_AC_CALIB', 'L1_BCM_CA_CALIB','L1_BCM_Wide_CALIB',


        #--------------------------------
        # TOPO items
        #--------------------------------
        # M8 
        ### 'L1_MJJ-4',        
        ### 'L1_MJJ-3',
        ### 'L1_MJJ-2',
        ### 'L1_MJJ-1',

        ### 'L1_HT200-J20s5.ETA49',
        ### 'L1_HT190-J20s5.ETA49',
        ### 'L1_HT1-J0.ETA49',
        ### 'L1_JPSI-1M5-EM6',
        ### 'L1_JPSI-1M5-EMs',
        ### 'L1_JPSI-1M5-EM6s',
        ### 'L1_JPSI-1M5-EM12s',

        # B-jet
        'L1_BTAG-MU4J15', 'L1_BTAG-MU4J30', 'L1_BTAG-MU6J25', 'L1_BTAG-MU4J17', 'L1_BTAG-MU4J20',

        # ZH
        'L1_J40_DPHI-Js2XE50', 'L1_J40_DPHI-J20s2XE50', 'L1_J40_DPHI-J20XE50', 'L1_J40_DPHI-CJ20XE50',
        'L1_BPH-DR-2MU4-B_BPH-2M-2MU4-B','L1_BPH-DR-2MU4-B_BPH-4M8-2MU4-B','L1_BPH-DR-2MU4-BO_BPH-2M-2MU4-BO','L1_BPH-DR-2MU4-BO_BPH-4M8-2MU4-BO',
        # HT
        'L1_HT190-J15.ETA20',
        'L1_HT190-J15s5.ETA20',
        'L1_HT150-J20.ETA30',
        'L1_HT150-J20s5.ETA30',   

        # Jpsi T&P
        'L1_JPSI-1M5', 'L1_JPSI-1M5-EM7', 'L1_JPSI-1M5-EM12',    
        'L1_KF-XE35', 'L1_KF-XE45', 'L1_KF-XE55', 'L1_KF-XE60', 'L1_KF-XE65', 'L1_KF-XE75',
        
        # W T&P
        'L1_W-05DPHI-JXE-0',
        'L1_W-10DPHI-JXE-0',
        'L1_W-15DPHI-JXE-0',
        'L1_W-10DPHI-EMXE-0',
        'L1_W-15DPHI-EMXE-0',
        'L1_W-05DPHI-EMXE-1',
        'L1_W-05RO-XEHT-0',
        'L1_W-08RO-XEHT-0',
        'L1_W-90RO2-XEHT-0',
        'L1_W-250RO2-XEHT-0',
        'L1_W-HT20-JJ15.ETA49',
        'L1_W-NOMATCH',
        'L1_W-NOMATCH_W-05RO-XEEMHT',
        'L1_EM10_W-MT25',
        'L1_EM10_W-MT30',
        'L1_EM15_W-MT35',
        'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0',
        'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS30',
        'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_W-250RO2-XEHT-0',
        'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-15DPHI-EMXE_XS60',
        #'L1_W-10DPHI-JXE-0',
        #'L1_W-20DPHI-JXE-1',
        #'L1_W-20DPHI-JXE-0',
        #'L1_W-10DPHI-EMXE-0',
        #'L1_W-20DPHI-EMXE-1',
        #'L1_W-20DPHI-EMXE-0',
        #'L1_W-05RO-XEHT-0',
        #'L1_W-08RO-XEHT-1',
        #'L1_W-40RO2-XEHT-0',
        #'L1_W-90RO2-XEHT-1',
        #'L1_W-HT20-JJ0.ETA49',
        #'L1_W-NOMATCH',
        #'L1_W-NOMATCH_W-05RO-XEEMHT',
        #'L1_EM3_W-MT20',
        #'L1_EM3_W-MT30',
        #'L1_EM3_W-MT40',

        #Bphysics items
        'L1_BPH-DR-2MU4', 
        'L1_BPH-2M-2MU4', 'L1_BPH-4M8-2MU4',
        'L1_BPH-DR-2MU4_BPH-2M-2MU4', 'L1_BPH-DR-2MU4_BPH-4M8-2MU4',

        'L1_BPH-DR-2MU6', 
        'L1_BPH-2M-2MU6', 'L1_BPH-4M8-2MU6',
        'L1_BPH-DR-2MU6_BPH-2M-2MU6', 'L1_BPH-DR-2MU6_BPH-4M8-2MU6',

        # temporarily commented out until CTP Simulation is fixed (menu generation and TMC understand multiplicity topo)
        #'L1_2CMU4',
        #'L1_2CMU6',
        #'L1_CMU6_2CMU4', 

        # L1 items for L1Topo streamers
        'L1_BPH-2M-2MU4-B',
        'L1_BPH-2M-2MU4-BO',
        'L1_BPH-2M-MU6MU4',
        'L1_BPH-2M-MU6MU4-B',
        'L1_BPH-2M-MU6MU4-BO',    
        'L1_BPH-4M8-2MU4-B',
        'L1_BPH-4M8-2MU4-BO',
        'L1_BPH-4M8-MU6MU4',
        'L1_BPH-4M8-MU6MU4-B',        
        'L1_BPH-4M8-MU6MU4-BO',
        'L1_DY-DR-2MU4',
        'L1_DY-BOX-2MU4',
        'L1_DY-BOX-MU6MU4',
        'L1_DY-BOX-2MU6',
        'L1_LFV-MU',
        'L1_LFV-EM8I',
        'L1_LFV-EM15I',
        'L1_DPHI-Js2XE50',
        'L1_DPHI-J20s2XE50',
        'L1_DPHI-J20XE50',
        'L1_DPHI-CJ20XE50',
        'L1_MJJ-900',
        'L1_MJJ-800',
        'L1_MJJ-700',
        'L1_MJJ-400',
        'L1_MJJ-350',
        'L1_MJJ-300',
        'L1_MJJ-200',
        'L1_MJJ-100',
        'L1_HT150-JJ15.ETA49',
        'L1_DETA-JJ',
        'L1_J4-MATCH',
        'L1_W-05RO-XEEMHT',
        'L1_LLP-RO',
        'L1_LLP-NOMATCH',
        'L1_DR-MU10TAU12I',    
        'L1_TAU12I-J25',    
        'L1_EM15-TAU40',    
        'L1_TAU20-J20',
        'L1_EM15-TAU12I',
        'L1_EM15TAU12I-J25',
        'L1_DR-EM15TAU12I-J25',    
        'L1_TAU20ITAU12I-J25',
        'L1_DR-TAU20ITAU12I',    
        'L1_BOX-TAU20ITAU12I',
        'L1_DR-TAU20ITAU12I-J25',    
        'L1_LAR-EM',
        'L1_LAR-J',
        'L1_BPH-DR-2MU4-B',
        'L1_BPH-DR-2MU4-BO',
        'L1_BPH-DR-MU6MU4',
        'L1_BPH-DR-MU6MU4-B',
        'L1_BPH-DR-MU6MU4-BO',
        
        # combined L1Topo for VBF
        'L1_MU6_MJJ-200',
        'L1_MU6_MJJ-300',
        'L1_MU6_MJJ-400',
        'L1_MU6_MJJ-500',
        'L1_J30_2J20_4J20.0ETA49_MJJ-400',
        'L1_J30_2J20_4J20.0ETA49_MJJ-700',
        'L1_J30_2J20_4J20.0ETA49_MJJ-800',
        'L1_J30_2J20_4J20.0ETA49_MJJ-900',
        'L1_3J20_4J20.0ETA49_MJJ-400',
        'L1_3J20_4J20.0ETA49_MJJ-700',
        'L1_3J20_4J20.0ETA49_MJJ-800',
        'L1_3J20_4J20.0ETA49_MJJ-900',
        
        'L1_XE35_MJJ-200',
        
        
        ##     # Partition 1
        ##     'L1_CALREQ0_P1', 'L1_CALREQ1_P1', 'L1_CALREQ2_P1',

        ]
    
    # the CTP ID mapping still has to be defined, currently it is taken from the order of the items
    
    Lvl1Flags.CtpIdMap = {}
    
    Lvl1Flags.prescales = {}

