# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def defineMenu():
    """
    Defines the following LvlFlags:
    
    thresholds   .... list of all threshold names in the menu (includes forced thresholds)
    items        .... list of all L1 item names in the menu
    CtpIdMap     .... map of item names to CTP IDs
    """
    
    from ..LVL1.Lvl1Flags import Lvl1Flags

    Lvl1Flags.CTPVersion = 4 # new CTP

    Lvl1Flags.BunchGroupPartitioning = [1, 14, 15] # partition 1: 1-10, partition 2: 14, partition 3: 15 (note that BGRP0 is used by all items)
    Lvl1Flags.BunchGroupNames = ['BCRVeto', 'Paired', 'CalReq', 'Empty', 'IsolatedUnpaired', 'NonIsolatedUnpaired', 'EmptyAfterPaired', 'InTrain', 'AbortGapNotCalReq', 'VdM', 'ALFA', 'EmptyBeforePaired','EmptyAndPaired']
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

        # Note that the forced thresholds (FTHR) are not used in the menu but are needed for input monitoring
        # They can always be replaced with something that is needed for the menu


        #-------------------------
        # SLOT 7 / CON 0,1 (EM1,2)
        #-------------------------
        
 
        # 16 x EM
        'EM3', 'EM7', 'EM8VH',
        'EM10VH', 'EM12', 'EM15', 'EM15VH',
        'EM15VHI',
        'EM18VHI', 'EM20VH', 'EM20VHI', 'EM22VH', 'EM22VHI', 'EM24VHI', 'EM24VHIM', 'EM30VHI', 
        # 1 x ZB/
        'ZB_EM15',

        #--------------------------
        # SLOT 7 / CON 2,3 (TAU1,2)
        #--------------------------

        # 16 x TAU
        'HA5', 'HA8', 'HA12', 'HA12IL', 'HA12IM', 'HA15', 'HA20', 'HA20IL', 'HA20IM', 'HA25', 'HA25IM', 'HA30', 'HA40', 'HA60', 'HA90', 'HA100',
 

        #----------------------
        # SLOT 8 / CON 0 (JET1)
        #----------------------
        # 10 x 3-bit JET (can have multiplicity 4 or more)

        # 8 x JETs and central jets
        'J12', 'J12.0ETA23', 'J15', 'J15.0ETA25','J20', 'J25','J25.0ETA23','J30',
        # 2 x Central Jets
         'J12.0ETA28', #'J12.0ETA25',

        # VBF Jets
        'J20.0ETA49', 'J30.0ETA49',


        #----------------------
        # SLOT 8 / CON 1 (JET2)
        #----------------------
        # 15 x 2-bit JET (can have maximum multiplicity of 3) (SLOT 8, CON 1)

        # 3 x Central Jet
        'J35.0ETA23','J40.0ETA25', #'J20.28ETA31',

        # 6 Jets # replace J400 with J45.0ETA20 - ATR-19309
        'J40', 'J50', 'J75', 'J85', 'J100', 'J45.0ETA20',  'J400', #'J120',


        # 6 x FJ              
        'J15.31ETA49', 'J20.31ETA49', 'J30.31ETA49', 'J50.31ETA49', 'J75.31ETA49', 
 


        #---------------------
        # SLOT 8 / CON 2 (EN1) 
        #---------------------
        # 24 x 1-bit thresholds

        # 8 x TE
        'TE5', 'TE10', 'TE15', 'TE20', 'TE25', 'TE30', 'TE40', 'TE50',
    
        # 8 x XE (for standard XE)
        'XE10', 'XE20', 'XE25', 'XE30', 'XE35', 'XE40', 'XE45', 'XE50',
        'XE55', 'XE60', 'XE65', 'XE70', 'XE75', 'XE80', 'XE150', 'XE300',
        
        # 8 x XS
        'XS20', 'XS30', 'XS40', 'XS45', 'XS50', 'XS55', 'XS60', 'XS65', 

 
        #---------------------
        # SLOT 8 / CON 3 (EN2)
        #---------------------
        # 16 x 1-bit thresholds

        # 8 x weighted sum ET
        #'RXE35', 'RXE40', 'RXE45', 'RXE50', 'RXE55', 'RXE60', 'RXE70', 'RXE80', # FTHR

        # 8 x restricted eta range
        #'TE5.24ETA49', 'TE10.24ETA49', 'TE15.24ETA49', 'TE20.24ETA49', 'TE25.24ETA49', 'TE30.24ETA49', 'TE40.24ETA49', 'TE70.24ETA49',



        #------------------------
        # SLOT 9 / CON 0 (MUCTPi) 
        #------------------------
        
        # 6 x MU
        'MU4', 'MU6', 'MU10', 'MU11', 'MU20', 'MU21',

        
        #------------------------
        # SLOT 9 / CON 1 (CTPCal)
        #------------------------

        # 3 x 1-bit BCM
        'BCM_AtoC', 'BCM_CtoA', 'BCM_Wide',
        # 1 x 3-bit BCM
        'BCM_Comb',

        # 8 x DBM
        #'DBM0', 'DBM1', 'DBM2', 'DBM3', 'DBM4', 'DBM5', 'DBM6', 'DBM7', # FTHR

        # 2  x BPTX
       'BPTX0','BPTX1',

        # 6 x LUCID 
        'LUCID_A', 'LUCID_C',
        #'LUCID_Coinc_AC', # FTHR
        #'LUCID_COMM',
        #'LUCID_05', 'LUCID_06', # FHTR

        # 3 x ZDC
        'ZDC_A', 'ZDC_C',
        'ZDC_AND', # FTHR

        # 3 x CALREQ
        'CAL0','CAL1','CAL2',

 

        #-----------------------------
        # SLOT 9 / CON 2,3 (NIM1,NIM2)
        #-----------------------------

        # 2 x MBTS
        'MBTS_A', 'MBTS_C',

        # 32 x MBTSSI (all FTHR)
        # NOTE: THESE ARE OUT OF ORDER FOR A REASON! Do not change!
        # The order defines the mapping - see ATR-17870.
        'MBTS_A0', 'MBTS_A1', 'MBTS_A2', 'MBTS_A3', 'MBTS_A4', 'MBTS_A5', 'MBTS_A6', 'MBTS_A7',
        'MBTS_A8', 'MBTS_A10', 'MBTS_A12', 'MBTS_A14', 'MBTS_A9', 'MBTS_A11','MBTS_A13', 'MBTS_A15',
        'MBTS_C0', 'MBTS_C1', 'MBTS_C2', 'MBTS_C3', 'MBTS_C4', 'MBTS_C5', 'MBTS_C6', 'MBTS_C7',
        'MBTS_C8', 'MBTS_C10', 'MBTS_C12', 'MBTS_C14', 'MBTS_C11', 'MBTS_C13','MBTS_C9','MBTS_C15',

        # L1A for CTP monitoring itself
        'NIML1A',

        # LHCF
        'NIMLHCF',

        # TGC
        'NIMTGC',
        
        # RPC
        'NIMRPC',
        
        # TRT
        'NIMTRT',

        # AFP
        'AFP_NSC', 'AFP_NSA',
        #'AFP_FSA_SIT', 'AFP_FSA_TOF', 'AFP_FSA_LOG',
        #'AFP_FSC_SIT', 'AFP_FSC_LOG', 'AFP_FSC_TOF',

        
        #-------------------------------------------------------------------
        #L1 TOPO inputs 
        #-------------------------------------------------------------------

        #B-jets
        #'BTAG-MU4J15',
        #'BTAG-MU6J20',

        #HT
        'HT190-J15s5.ETA21',
        'HT150-J20s5.ETA31',

        

        #ZH 
        '10MINDPHI-J20s2-XE50', '10MINDPHI-J20s2-XE30', 

        #Jpsi T&P
         '1INVM5-EM7s1-EMs6', '1INVM5-EM12s1-EMs6',        

        #W T&P - all were commented out
 
        
        # central muon
        'MULT-CMU4ab',
        'MULT-CMU6ab',
        

        #B-physics
        '0DR03-EM7ab-CJ15ab',
 

        # L1 thresholds for L1Topo streamers
        #'2DR99_2MU4ab',
        '5DETA99-5DPHI99-MU6ab-MU4ab',
        '5DETA99-5DPHI99-2MU6ab',
        '0DR15-2MU6ab',
        '0DETA04-EM8abi-MU10ab',
        '0DPHI03-EM8abi-MU10ab',
        '0DETA04-EM15abi-MUab',
        '0DPHI03-EM15abi-MUab',
        #'800INVM9999-AJ30s6-AJ20s6',
        '700INVM9999-AJ30s6-AJ20s6',
        '300INVM9999-AJ30s6-AJ20s6',
        #'400INVM9999-AJ30s6-AJ20s6',
        '500INVM9999-J30s6-AJ20s6',
        #'100INVM9999-AJ30s6-AJ20s6',
        '0DETA20-J50s1-Js2',
        '27DPHI32-EMs1-EMs6',
        #'HT150-AJj15all.ETA49',
        #'0MATCH-4AJ20.ETA31-4AJj15.ETA31',
        '100RATIO-0MATCH-TAU30si2-EMall',
        'NOT-0MATCH-TAU30si1-EMall',
        #'1DISAMB-TAU12abi-J25ab',
        #'1DISAMB-EM15his2-TAU12abi-J25ab',
        '2DISAMB-TAU12abi-J25ab',
        '2DISAMB-J25ab-0DR28-EM15his2-TAU12abi',
        #'0DR28-MU10ab-TAU12abi',  # add back if needed
        #'1DISAMB-TAU20abi-TAU12abi-J25ab',
        '0DR25-TAU20abi-TAU12abi',
        '0DR28-TAU20abi-TAU12abi',
        '2DISAMB-J25ab-0DR25-TAU20abi-TAU12abi',
        '2DISAMB-J25ab-0DR28-TAU20abi-TAU12abi',
        'DISAMB-30INVM-EM20his2-TAU12ab',
        '400INVM9999-AJ30s6.ETA31-AJ20s6.31ETA49',

        '0INVM9-EM7ab-EMab',
        '400INVM9999-0DPHI20-J30s6-AJ20s6',
        '400INVM9999-0DPHI22-J30s6-AJ20s6',
        '400INVM9999-0DPHI24-J30s6-AJ20s6',
        '400INVM9999-0DPHI26-J30s6-AJ20s6',

        'LATE-MU10s1',
        'SC111-CJ15ab.ETA26',
        
        
        # ALFA (the replication is needed to build all the combinations in the CTPCore)
        'ALFA_B7R1L', 'ALFA_B7R1U', 'ALFA_A7R1L', 'ALFA_A7R1U', 'ALFA_A7L1L', 'ALFA_A7L1U', 'ALFA_B7L1L', 'ALFA_B7L1U',
        'ALFA2_B7R1L', 'ALFA2_B7R1U', 'ALFA2_A7R1L', 'ALFA2_A7R1U', 'ALFA2_A7L1L', 'ALFA2_A7L1U', 'ALFA2_B7L1L', 'ALFA2_B7L1U',
        'ALFA3_B7R1L', 'ALFA3_B7R1U', 'ALFA3_A7R1L', 'ALFA3_A7R1U', 'ALFA3_A7L1L', 'ALFA3_A7L1U', 'ALFA3_B7L1L', 'ALFA3_B7L1U',
        'ALFA4_B7R1L', 'ALFA4_B7R1U', 'ALFA4_A7R1L', 'ALFA4_A7R1U', 'ALFA4_A7L1L', 'ALFA4_A7L1U', 'ALFA4_B7L1L', 'ALFA4_B7L1U',
#ATR-13743      
        'ALFA_B7R1L_OD', 'ALFA_B7R1U_OD', 'ALFA_A7R1L_OD', 'ALFA_A7R1U_OD', 'ALFA_A7L1L_OD', 'ALFA_A7L1U_OD', 'ALFA_B7L1L_OD', 'ALFA_B7L1U_OD',

        #ATR-19355
        '0INVM10-3MU4ab',

        #ATR-18815 - Algorithm needs to be moved to master
#        '0INVM10-0DR15-EM8abi-MU10ab',
#        '0INVM10-0DR15-EM12abi-MU6ab',

        #ATR-18824
        '60INVM9999-25DPHI32-EM15abhi-FJj15s623ETA49',
        '60INVM9999-04DPHI32-EM15abhi-FJj15s623ETA49',
        #ATR-19302
        '0INVM70-27DPHI32-EM12his1-EM12his6',

        #ATR-19720
        #'8INVM15-0DR22-2MU6ab',
        #'2INVM9-2DR15-2MU6ab',
        #'2INVM9-0DR15-MU6ab-MU4ab',
        #'8INVM15-0DR22-MU6ab-MU4ab',
        #'2INVM9-0DR15-2MU4ab',

        #ATR-19510
        '5DETA99-5DPHI99-2MU4ab',
            
        #ATR-17320
        'CEP-CJ60s6',
        'CEP-CJ50s6',
       
        #ATR-20174
        '8INVM15-2CMU4ab'

        
    ]

    
    Lvl1Flags.items = [

        # single EM

        'L1_EM3','L1_EM7','L1_EM12', 'L1_EM8VH',
        'L1_EM10VH', 'L1_EM15',
        'L1_EM30VHI',
        'L1_EM15VHI',
        'L1_EM24VHIM','L1_EM15VH', 'L1_EM18VHI', 'L1_EM20VH', 'L1_EM20VHI', 'L1_EM22VHI', 'L1_EM22VH','L1_EM24VHI', 
        'L1_EM3_EMPTY', 'L1_EM7_EMPTY', 'L1_EM7_UNPAIRED_ISO', 'L1_EM7_FIRSTEMPTY',
        'L1_EM20VH_FIRSTEMPTY',


        # combined EM -jet - all commented

        # single MU
        "L1_MU4", "L1_MU6", "L1_MU10", "L1_MU11", "L1_MU20", "L1_MU21",
        
        "L1_MU4_EMPTY", "L1_MU6_EMPTY", "L1_MU4_FIRSTEMPTY", "L1_MU11_EMPTY",
        'L1_MU4_UNPAIRED_ISO',


        
        # multi lepton
        "L1_2EM3", "L1_2EM7",
        "L1_2EM15", "L1_2EM15VH", "L1_2EM15VHI",
        "L1_2EM20VH",
        "L1_EM20VH_3EM10VH", 
        "L1_EM7_MU10",
        "L1_2MU4", "L1_2MU6", "L1_2MU10", "L1_2MU20_OVERLAY",  "L1_MU11_2MU6", "L1_3MU4", "L1_MU6_2MU4", "L1_3MU6",  "L1_4MU4", "L1_MU6_3MU4", "L1_2MU6_3MU4", "L1_2MU11", "L1_MU11_2MU10",
        "L1_2EM8VH_MU10", "L1_EM15VH_MU10",  "L1_EM7_MU20", "L1_EM8VH_MU20",

        # single tau
        "L1_TAU5", "L1_TAU12", "L1_TAU12IM",   "L1_TAU20IM",
        "L1_TAU30","L1_TAU30_EMPTY","L1_TAU30_UNPAIRED_ISO", "L1_TAU40", "L1_TAU60", "L1_TAU90", "L1_TAU100", "L1_TAU8_EMPTY", 
        "L1_TAU8_UNPAIRED_ISO","L1_TAU8_FIRSTEMPTY",

        # multi tau
        "L1_TAU20IM_2TAU12IM",   "L1_TAU60_2TAU40", "L1_2TAU5", "L1_2TAU8",

        # combined tau - lepton
        "L1_EM15VHI_2TAU12IM",          
        "L1_EM15VHI_2TAU12IM_J25_3J12",   
        "L1_EM15VHI_2TAU12IM_4J12",
        "L1_EM15VHI_TAU40_2TAU15",
        "L1_MU10_TAU12IM",  
        "L1_MU10_TAU12IM_J25_2J12",
        "L1_MU10_TAU12IM_3J12",
        #'L1_DR-MU10TAU12I_TAU12I-J25',
        "L1_MU10_TAU20IM",
        "L1_MU11_TAU20IM",
        "L1_MU10_TAU20IM_J25_2J20",

        # combined tau - jet

        "L1_TAU20IM_2TAU12IM_J25_2J20_3J12",
        "L1_TAU20IM_2TAU12IM_4J12.0ETA23",
        #"L1_TAU20IM_2TAU12IM_4J12.0ETA25",
        "L1_TAU20IM_2TAU12IM_4J12.0ETA28",
        "L1_TAU25IM_2TAU20IM_2J25_3J20", 



        # combined tau - xe
        "L1_TAU20IM_2J20_XE45", 
        "L1_TAU20IM_2J20_XE50", 


        #"L1_EM15HI_2TAU12I_XE35", 
        "L1_EM15VHI_2TAU12IM_XE35", 
        'L1_EM20VHI_TAU20IM_2TAU20_J25_3J20',


        "L1_MU10_TAU12IM_XE35",  

        "L1_TAU20IM_2TAU12IM_XE35", 
        "L1_TAU40_2TAU12IM_XE40",  



        # combined em - jet
        'L1_EM18VHI_3J20',
        'L1_EM20VH_3J20',

        

        # combined mu - jet
        'L1_MU6_J20', 'L1_MU6_J30.0ETA49_2J20.0ETA49', 'L1_MU6_J40', 'L1_MU6_J75',
        'L1_MU10_2J20','L1_MU10_3J20', 'L1_MU10_2J15_J20',
        'L1_MU20_J40',
        'L1_MU20_XE30',
        'L1_MU20_J50',

        # single jet
        "L1_J15","L1_J20","L1_J25", "L1_J30", "L1_J40", "L1_J50" ,"L1_J75","L1_J85", "L1_J100",  "L1_J400", #"L1_J120",
        "L1_J20.31ETA49", "L1_J30.31ETA49", "L1_J50.31ETA49", "L1_J75.31ETA49", 
        'L1_J15.31ETA49',
        "L1_J12_EMPTY","L1_J12_FIRSTEMPTY", "L1_J12_UNPAIRED_ISO", "L1_J12_UNPAIRED_NONISO", "L1_J12_ABORTGAPNOTCALIB", 
        "L1_J15.31ETA49_UNPAIRED_ISO",
        "L1_J30_EMPTY", "L1_J30_FIRSTEMPTY", "L1_J30.31ETA49_EMPTY", "L1_J30.31ETA49_UNPAIRED_ISO",
        "L1_J30.31ETA49_UNPAIRED_NONISO",
        "L1_J50_UNPAIRED_ISO", "L1_J50_UNPAIRED_NONISO", "L1_J50_ABORTGAPNOTCALIB",         
        'L1_J100_FIRSTEMPTY',
 
        
        # multi jet
        "L1_J45.0ETA20_3J15.0ETA25",
        "L1_J50_2J40.0ETA25_3J15.0ETA25",
        "L1_3J50", "L1_4J15", "L1_4J20", #"L1_4J20.0ETA49",
        "L1_3J15.0ETA25_XE40",
        
        "L1_6J15",
        "L1_J85_3J30",
       # multi jet forward
        "L1_J25.0ETA23_2J15.31ETA49",
        "L1_J40.0ETA25_2J15.31ETA49",
        "L1_J40.0ETA25_2J25_J20.31ETA49",
        "L1_J30.0ETA49_2J20.0ETA49",
        
        # multi jet central
        "L1_3J25.0ETA23",
        "L1_3J35.0ETA23",
        "L1_4J15.0ETA25",
        "L1_5J15.0ETA25", 
        # combined jet
        "L1_2J15_XE55", "L1_J40_XE50",

        #'L1_2J40_XE45',
        "L1_2J50_XE40", "L1_J40_XE60",
        

        # XE
        "L1_XE35", "L1_XE40", "L1_XE45", "L1_XE50", 
        "L1_XE55", "L1_XE60", "L1_XE70", "L1_XE80",
        'L1_XE30', 'L1_XE300',
        "L1_XE10",
        
        #XS
        'L1_EM12_XS20', 'L1_EM15_XS30',
        
        # TE
#        'L1_TE5', ## disabled?

        #VTE + MBTS
        #"L1_MBTS_1_VTE2", "L1_MBTS_1_VTE3", "L1_MBTS_1_VTE4", "L1_MBTS_1_VTE5", "L1_MBTS_1_VTE10", "L1_MBTS_2_VTE2", "L1_MBTS_2_VTE3", "L1_MBTS_2_VTE4", "L1_MBTS_2_VTE5", "L1_MBTS_2_VTE10", 
        
        #TE + MU

        # MBTS
        "L1_MBTS_1_EMPTY",
        "L1_MBTS_2_EMPTY",
        "L1_MBTS_1_1_EMPTY",
        # beam splashes
        
        #Overlay
#        "L1_MBTS_1_OVERLAY","L1_TE20_OVERLAY",
        
        #BeamSpot
#        "L1_MBTS_1_VTE70",

        #BGRP9

        #BGRP11

        # RNDM
        'L1_RD0_FILLED', 'L1_RD0_UNPAIRED_ISO',  'L1_RD0_EMPTY',
        "L1_RD1_EMPTY","L1_RD2_EMPTY",
#        'L1_RD0_FIRSTEMPTY', 
        'L1_RD0_ABORTGAPNOTCALIB',
#        'L1_RD2_FILLED', # This is not needed?

        #LUCID
#        'L1_LUCID_A_C_EMPTY', 'L1_LUCID_A_C_UNPAIRED_ISO', 'L1_LUCID_A_C_UNPAIRED_NONISO',
        #'L1_LUCID_COMM_EMPTY', 'L1_LUCID_COMM_UNPAIRED_ISO', 

        

        # ZDC
         # FTHR

        # VDM
         #"L1_BGRP7",

        # TRT
        # "L1_TRT",
        "L1_TRT_FILLED",
        "L1_TRT_EMPTY",

        # TGC
        "L1_TGC_BURST",
#        "L1_TGC_BURST_EMPTY",

        # LHCF
    
        #CALREQ
        "L1_CALREQ2",

        #ZB
        "L1_ZB",

        # BPTX
        
        # BCM
         #'L1_BCM_AC_CA_UNPAIRED_ISO',
        'L1_BCM_AC_CA_BGRP0', 'L1_BCM_Wide_EMPTY','L1_BCM_Wide_UNPAIRED_ISO','L1_BCM_Wide_UNPAIRED_NONISO',
        'L1_BCM_AC_UNPAIRED_ISO','L1_BCM_CA_UNPAIRED_ISO',
        'L1_BCM_AC_UNPAIRED_NONISO','L1_BCM_CA_UNPAIRED_NONISO',
        'L1_BCM_AC_ABORTGAPNOTCALIB', 'L1_BCM_CA_ABORTGAPNOTCALIB','L1_BCM_Wide_ABORTGAPNOTCALIB',
        'L1_BCM_AC_CALIB', 'L1_BCM_CA_CALIB','L1_BCM_Wide_CALIB',

        #AFP
#         'L1_AFP_FSC', 
#        'L1_AFP_C_ANY', 'L1_AFP_C_ANY_UNPAIRED_ISO', 'L1_AFP_C_ANY_UNPAIRED_NONISO', 'L1_AFP_C_ANY_EMPTY', 'L1_AFP_C_ANY_FIRSTEMPTY', 'L1_AFP_C_AND',

#	'L1_EM7_AFP_A_AND_C','L1_EM12_AFP_A_AND_C',


          #--------------------------------
        # TOPO items
        #--------------------------------

        # B-jet
        #'L1_BTAG-MU4J15',
        #'L1_BTAG-MU6J20',

        # ZH - all removed
   

        #Higgsino
        'L1_2MU4_J40_XE50',
        'L1_MU4_J30_XE40_DPHI-J20s2XE30','L1_MU4_J50_XE50_DPHI-J20s2XE30',
        'L1_MU4_XE60',

        # HT
        'L1_HT190-J15s5.ETA21',

        "L1_HT150-J20s5.ETA31_MJJ-400-CF",
        
        # Jpsi T&P
        'L1_JPSI-1M5-EM7', 'L1_JPSI-1M5-EM12',    
#            
        
        # W T&P - all removed

        'L1_LFV-MU6',
        'L1_LFV-MU11',
        'L1_LFV-EM8I',
        'L1_LFV-EM15I',
        'L1_DPHI-J20s2XE50',
        'L1_J40_XE50_DPHI-J20s2XE50',
        'L1_2MU4_J20_XE30_DPHI-J20s2XE30',
        'L1_MJJ-700',
        'L1_MJJ-300',
        #'L1_MJJ-100',
        #'L1_MJJ-400',
        #'L1_MJJ-800',
        'L1_MJJ-500-NFF',
        'L1_J50_DETA20-J50J',
        'L1_DPHI-2EM3',
        #'L1_HT150-JJ15.ETA49',
        #'L1_HT150-JJ15.ETA49_MJJ-400',
        #'L1_J4-MATCH',
        'L1_LLP-RO',
        #'L1_LLP-NOMATCH',
        #'L1_EM15TAU12I-J25',
        'L1_DR-EM15TAU12I-J25',
        #'L1_TAU20ITAU12I-J25',
        'L1_DR-TAU20ITAU12I',    
        'L1_DR-TAU20ITAU12I-J25',
        #'L1_DR-MU10TAU12I',

        'L1_30M-EM20ITAU12',
        'L1_MJJ-400-CF',
    
        #'L1_LAR-EM',
        #'L1_LAR-J',
        #'L1_LAR-ZEE',
        #'L1_LAR-ZEE_LAR-EM',
        #'L1_BPH-2M9-2MU4_BPH-0DR15-2MU4',
        #'L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4',
        #'L1_BPH-8M15-2MU6_BPH-0DR22-2MU6',
        #'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4-BO',

        #ATR-19720 - no algorithm in master
#        'L1_BPH-8M15-0DR22-2MU6',
#        'L1_BPH-2M9-2DR15-2MU6',
#        'L1_BPH-2M9-0DR15-MU6MU4',
#        'L1_BPH-8M15-0DR22-MU6MU4-BO',
#        'L1_BPH-2M9-0DR15-2MU4',
#
#        'L1_BPH-0M9-EM7-EM5',        
#        'L1_BPH-0DR3-EM7J15',
#        'L1_BPH-0M9-EM7-EM5_MU6',
#        'L1_BPH-0DR3-EM7J15_MU6',
#        'L1_BPH-0M9-EM7-EM5_2MU4',
#        'L1_BPH-0DR3-EM7J15_2MU4',
#
        # combined L1Topo for VBF
        
        # INVM + DPHI 
        #'L1_MJJ-400-NFF-0DPHI20',
        'L1_MJJ-400-NFF-0DPHI22',
        'L1_MJJ-400-NFF-0DPHI24',
        'L1_MJJ-400-NFF-0DPHI26',
   
        ### ATR-15062
        'L1_EM18VHI_MJJ-300',

        'L1_LATE-MU10_XE50','L1_LATE-MU10_XE40', 'L1_LATE-MU10_J50',

        'L1_TAU60_DR-TAU20ITAU12I',

        'L1_SC111-CJ15',
#        'L1_SC85-CJ15',
        
        ##     # Partition 1
        ##     'L1_CALREQ0_P1', 'L1_CALREQ1_P1', 'L1_CALREQ2_P1',

        #ALFA
#        'L1_ALFA_SDIFF5','L1_ALFA_SDIFF6','L1_ALFA_SDIFF7','L1_ALFA_SDIFF8',
#        'L1_MBTS_1_A_ALFA_C','L1_MBTS_1_C_ALFA_A','L1_MBTS_1_A_ALFA_C_UNPAIRED_ISO','L1_MBTS_1_C_ALFA_A_UNPAIRED_ISO',
#        'L1_MBTS_2_A_ALFA_C','L1_MBTS_2_C_ALFA_A','L1_MBTS_2_A_ALFA_C_UNPAIRED_ISO','L1_MBTS_2_C_ALFA_A_UNPAIRED_ISO',
#        'L1_LUCID_A_ALFA_C','L1_LUCID_C_ALFA_A','L1_LUCID_A_ALFA_C_UNPAIRED_ISO','L1_LUCID_C_ALFA_A_UNPAIRED_ISO',
#
#        'L1_ALFA_ELASTIC_UNPAIRED_ISO', 'L1_ALFA_ANTI_ELASTIC_UNPAIRED_ISO',        
#
#        #'L1_ZDC_A_ALFA_C','L1_ZDC_C_ALFA_A','L1_ZDC_ALFA',
#        'L1_EM3_ALFA_ANY',
#        'L1_EM3_ALFA_ANY_UNPAIRED_ISO',
#
#        'L1_EM3_ALFA_EINE',
#        'L1_J12_ALFA_ANY','L1_J12_ALFA_ANY_UNPAIRED_ISO',
#        'L1_TE5_ALFA_ANY','L1_TE5_ALFA_ANY_UNPAIRED_ISO',
#         'L1_TE5_ALFA_EINE',
#        'L1_TRT_ALFA_ANY','L1_TRT_ALFA_ANY_UNPAIRED_ISO',
#        'L1_TRT_ALFA_EINE',
#        #'L1_LHCF_ALFA_ANY_A',
#        #'L1_LHCF_ALFA_ANY_C',
##        'L1_LHCF_ALFA_ANY_A_UNPAIRED_ISO','L1_LHCF_ALFA_ANY_C_UNPAIRED_ISO',
#        'L1_ALFA_BGT','L1_ALFA_BGT_UNPAIRED_ISO',
#        'L1_ALFA_ANY_A_EMPTY','L1_ALFA_ANY_C_EMPTY',
#
#         'L1_AFP_C_MBTS_A',
#         'L1_AFP_C_ZDC_C',
#         'L1_AFP_C_J12',
#         'L1_AFP_C_EM3',
#         'L1_AFP_C_TE5',
#         'L1_AFP_C_ALFA_C',
#         'L1_AFP_C_ALFA_A',
#         'L1_AFP_C_ANY_MBTS_A',
#         'L1_AFP_C_MU4',

        # Items for beta*=90m run
#        'L1_J12_ALFA_EINE',
#        'L1_2EM3_ALFA_EINE',
#        'L1_MU4_ALFA_ANY',
#        'L1_MU4_ALFA_ANY_UNPAIRED_ISO',
#        'L1_MU4_ALFA_ANY_PAIRED_UNPAIRED_ISO',
#        'L1_MU4_ALFA_EINE',


        #ATR-18815
        #'L1_LFV-EM8I-MU11',
        #'L1_LFV-EM12I-MU6',

        #ATR-19355
        'L1_BPH-0M10-3MU4',

        #ATR-18824
        'L1_ZAFB-04DPHI-EM15I',
        'L1_ZAFB-25DPHI-EM15I',
        #'L1_ZAFB-25DPHI-EM18I',

        #ATR-19302: 
#        'L1_DPHI-M70-2EM10I',
        'L1_DPHI-M70-2EM12I',

        #ATR-19510
        'L1_DY-BOX-2MU4',
        #'L1_DY-DR-2MU4',
        'L1_DY-BOX-2MU6',

                
        #ATR-17320
        'L1_CEP-CJ60',
        'L1_CEP-CJ50' ,

        #ATR-20174
        'L1_BPH-8M15-2MU4-BO'



        ]
    

    #CTP IDs are taken from this mapping. Every L1 item needs a unique ctpid.
    # Run this file as python <this file> to print out available IDs
    # 463-464 are reserved for L1_RD2_BGRP14 and L1_RD3_BGRP15 (from run 2)
    # 509-511 are reserved for CALREQ

    

    Lvl1Flags.CtpIdMap = {

        'L1_EM3' : 0,
        'L1_EM7' : 1,
        'L1_EM12' : 2,
        'L1_EM8VH' : 3,
        'L1_EM10VH' : 4,
        'L1_EM15' : 6,
        'L1_EM30VHI' : 25,
        'L1_EM15VHI' : 29,
        'L1_EM24VHIM' : 30,
        'L1_EM15VH' : 7,
        'L1_EM18VHI' : 8,
        'L1_EM20VH' : 9,
        'L1_EM20VHI' : 10,
        'L1_EM22VHI' : 11,
        'L1_EM22VH' : 295,
        'L1_EM24VHI' : 392,
        'L1_EM3_EMPTY' : 12,
        'L1_EM7_EMPTY' : 13,
        'L1_EM7_UNPAIRED_ISO' : 389,
        'L1_EM7_FIRSTEMPTY' : 371,
        'L1_EM20VH_FIRSTEMPTY' : 411,
        'L1_MU4' : 14,
        'L1_MU6' : 15,
        'L1_MU10' : 16,
        'L1_MU11' : 256,
        'L1_MU20' : 18,
        'L1_MU21' : 17,
        'L1_MU4_EMPTY' : 19,
        'L1_MU6_EMPTY' : 66,
        'L1_MU4_FIRSTEMPTY' : 20,
        'L1_MU11_EMPTY' : 21,
        'L1_MU4_UNPAIRED_ISO' : 22,
        'L1_2EM3' : 23,
        'L1_2EM7' : 24,
        'L1_2EM15' : 27,
        'L1_2EM15VH' : 28,
        'L1_2EM15VHI' : 31,
        'L1_2EM20VH' : 400,
        'L1_EM20VH_3EM10VH' : 431,
        'L1_EM7_MU10' : 65,
        'L1_2MU4' : 32,
        'L1_2MU6' : 33,
        'L1_2MU10' : 34,
        'L1_2MU20_OVERLAY' : 35,
        'L1_MU11_2MU6' : 37,
        'L1_3MU4' : 38,
        'L1_MU6_2MU4' : 39,
        'L1_3MU6' : 40,
        'L1_4MU4' : 46,
        'L1_MU6_3MU4' : 36,
        'L1_2MU6_3MU4' : 42,
        'L1_2MU11' : 48,
        'L1_MU11_2MU10' : 49,
        'L1_2EM8VH_MU10' : 43,
        'L1_EM15VH_MU10' : 44,
        'L1_EM7_MU20' : 483,
        'L1_EM8VH_MU20' : 484,
        'L1_TAU5' : 50,
        'L1_TAU12' : 45,
        'L1_TAU12IM' : 47,
        'L1_TAU20IM' : 51,
        'L1_TAU30' : 53,
        'L1_TAU30_EMPTY' : 341,
        'L1_TAU30_UNPAIRED_ISO' : 343,
        'L1_TAU40' : 54,
        'L1_TAU60' : 55,
        'L1_TAU90' : 56,
        'L1_TAU100' : 52,
        'L1_TAU8_EMPTY' : 57,
        'L1_TAU8_UNPAIRED_ISO' : 388,
        'L1_TAU8_FIRSTEMPTY' : 391,
        'L1_TAU20IM_2TAU12IM' : 58,
        'L1_TAU60_2TAU40' : 458,
        'L1_2TAU5' : 59,
        #'L1_2TAU8' : 166,
        'L1_EM15VHI_2TAU12IM' : 60,
        'L1_EM15VHI_2TAU12IM_J25_3J12' : 61,
        'L1_EM15VHI_2TAU12IM_4J12' : 5,
        'L1_EM15VHI_TAU40_2TAU15' : 62,
        'L1_MU10_TAU12IM' : 63,
        'L1_MU10_TAU12IM_J25_2J12' : 64,
        'L1_MU10_TAU12IM_3J12' : 482,
        'L1_MU10_TAU20IM' : 67,
        'L1_MU11_TAU20IM' : 430,
        'L1_MU10_TAU20IM_J25_2J20' : 377,
        'L1_TAU20IM_2TAU12IM_J25_2J20_3J12' : 70,
        'L1_TAU20IM_2TAU12IM_4J12.0ETA23' : 316,
        #'L1_TAU20IM_2TAU12IM_4J12.0ETA25' : 73,
        'L1_TAU20IM_2TAU12IM_4J12.0ETA28' : 77,
        'L1_TAU25IM_2TAU20IM_2J25_3J20' : 398,
        'L1_TAU20IM_2J20_XE45' : 74,
        'L1_TAU20IM_2J20_XE50' : 79,
        'L1_EM15VHI_2TAU12IM_XE35' : 78,
        'L1_EM20VHI_TAU20IM_2TAU20_J25_3J20' : 69,
        'L1_MU10_TAU12IM_XE35' : 81,
        'L1_TAU20IM_2TAU12IM_XE35' : 83,
        'L1_TAU40_2TAU12IM_XE40' : 429,
        'L1_EM18VHI_3J20' : 172,
        'L1_EM20VH_3J20' : 26,
        'L1_MU6_J20' : 88,
        'L1_MU6_J30.0ETA49_2J20.0ETA49' : 382,
        'L1_MU6_J40' : 89,
        'L1_MU6_J75' : 90,
        'L1_MU10_2J20' : 278,
        'L1_MU10_3J20' : 173,
        'L1_MU10_2J15_J20' : 255,
        'L1_MU20_J40' : 428,
        'L1_MU20_XE30' : 433,
        'L1_MU20_J50' : 82,
        'L1_J15' : 92,
        'L1_J20' : 93,
        'L1_J25' : 94,
        'L1_J30' : 95,
        'L1_J40' : 96,
        'L1_J50' : 97,
        'L1_J75' : 98,
        'L1_J85' : 99,
        'L1_J100' : 100,
        #'L1_J120' : 101,
        'L1_J400' : 102,
        #'L1_4J20.0ETA49' : 383,
        'L1_J20.31ETA49' : 103,
        'L1_J30.31ETA49' : 104,
        'L1_J50.31ETA49' : 105,
        'L1_J75.31ETA49' : 106,
        'L1_J15.31ETA49' : 109,
        'L1_J30.0ETA49_2J20.0ETA49' : 137,
        'L1_J12_EMPTY' : 111,
        'L1_J12_FIRSTEMPTY' : 112,
        'L1_J12_UNPAIRED_ISO' : 113,
        'L1_J12_UNPAIRED_NONISO' : 114,
        'L1_J12_ABORTGAPNOTCALIB' : 115,
        'L1_J15.31ETA49_UNPAIRED_ISO' : 75,
        'L1_J30_EMPTY' : 116,
        'L1_J30_FIRSTEMPTY' : 117,
        'L1_J30.31ETA49_EMPTY' : 118,
        'L1_J30.31ETA49_UNPAIRED_ISO' : 119,
        'L1_J30.31ETA49_UNPAIRED_NONISO' : 120,
        'L1_J50_UNPAIRED_ISO' : 121,
        'L1_J50_UNPAIRED_NONISO' : 122,
        'L1_J50_ABORTGAPNOTCALIB' : 123,
        'L1_J100_FIRSTEMPTY' : 414,
        'L1_J45.0ETA20_3J15.0ETA25' : 86,
        'L1_J50_2J40.0ETA25_3J15.0ETA25' : 87,
        'L1_3J50' : 130,
        'L1_4J15' : 131,
        'L1_4J20' : 132,
        'L1_3J15.0ETA25_XE40' : 184,
        'L1_6J15' : 135,
        'L1_J85_3J30' : 480,
        'L1_J25.0ETA23_2J15.31ETA49' : 335,
        'L1_J40.0ETA25_2J15.31ETA49' : 181,
        'L1_J40.0ETA25_2J25_J20.31ETA49' : 182,
        'L1_3J25.0ETA23' : 373,
        'L1_3J35.0ETA23' : 425,
        'L1_4J15.0ETA25' : 41,
        'L1_5J15.0ETA25' : 140,
        'L1_2J15_XE55' : 141,
        'L1_J40_XE50' : 142,
        'L1_2J50_XE40' : 175,
        'L1_J40_XE60' : 176,
        'L1_XE10': 68,
        'L1_XE35' : 144,
        'L1_XE40' : 145,
        'L1_XE45' : 146,
        'L1_XE50' : 147,
        'L1_XE55' : 148,
        'L1_XE60' : 149,
        'L1_XE70' : 150,
        'L1_XE80' : 151,
        'L1_XE30' : 85,
        'L1_XE300' : 187,
        'L1_EM12_XS20' : 154,
        'L1_EM15_XS30' : 155,
        'L1_RD0_FILLED' : 200,
        'L1_RD0_UNPAIRED_ISO' : 201,
        'L1_RD0_EMPTY' : 202,
        'L1_RD1_EMPTY' : 204,
        'L1_RD2_EMPTY' : 206,
        'L1_TRT_FILLED' : 218,
        'L1_TRT_EMPTY' : 219,
        'L1_RD0_ABORTGAPNOTCALIB' : 372,
        'L1_TGC_BURST' : 220,
        'L1_ZB' : 240,
        'L1_BCM_AC_CA_BGRP0' : 223,
        'L1_BCM_Wide_EMPTY' : 224,
        'L1_BCM_Wide_UNPAIRED_ISO' : 225,
        'L1_BCM_Wide_UNPAIRED_NONISO' : 161,
        'L1_BCM_AC_UNPAIRED_ISO' : 163,
        'L1_BCM_CA_UNPAIRED_ISO' : 229,
        'L1_BCM_AC_UNPAIRED_NONISO' : 230,
        'L1_BCM_CA_UNPAIRED_NONISO' : 231,
        'L1_BCM_AC_ABORTGAPNOTCALIB' : 232,
        'L1_BCM_CA_ABORTGAPNOTCALIB' : 233,
        'L1_BCM_Wide_ABORTGAPNOTCALIB' : 234,
        'L1_BCM_AC_CALIB' : 235,
        'L1_BCM_CA_CALIB' : 236,
        'L1_BCM_Wide_CALIB' : 237,
        #'L1_BTAG-MU4J15' : 238,
        #'L1_BTAG-MU6J20' : 243,
        'L1_2MU4_J40_XE50' : 449,
        'L1_MU4_J30_XE40_DPHI-J20s2XE30' : 452,
        'L1_MU4_J50_XE50_DPHI-J20s2XE30' : 453,
        'L1_MU4_XE60' : 454,
        'L1_HT190-J15s5.ETA21' : 259,
        'L1_HT150-J20s5.ETA31_MJJ-400-CF' : 91,
        'L1_JPSI-1M5-EM7' : 263,
        'L1_JPSI-1M5-EM12' : 264,
        'L1_LFV-MU6' : 332,
        'L1_LFV-MU11' : 364,
        'L1_LFV-EM8I' : 320,
        'L1_LFV-EM15I' : 321,
        'L1_DPHI-J20s2XE50' : 323,
        'L1_J40_XE50_DPHI-J20s2XE50' : 245,
        'L1_2MU4_J20_XE30_DPHI-J20s2XE30' : 271,
        'L1_MJJ-700' : 216,
        'L1_MJJ-300' : 331,
        #'L1_MJJ-100' : 333,
        #'L1_MJJ-400' : 329,
        #'L1_MJJ-800' : 327,
        'L1_MJJ-500-NFF' : 108,
        'L1_J50_DETA20-J50J' : 275,
        'L1_DPHI-2EM3' : 288,
        #'L1_HT150-JJ15.ETA49' : 334,
        #'L1_HT150-JJ15.ETA49_MJJ-400' : 416,
        #'L1_J4-MATCH' : 336,
        'L1_LLP-RO' : 338,
        #'L1_LLP-NOMATCH' : 339,
        #'L1_EM15TAU12I-J25' : 345,
        #'L1_DR-MU10TAU12I' : 340,
        #'L1_DR-MU10TAU12I_TAU12I-J25' : 72,
        'L1_DR-EM15TAU12I-J25' : 346,
        #'L1_TAU20ITAU12I-J25' : 347,
        'L1_DR-TAU20ITAU12I' : 348,
        'L1_DR-TAU20ITAU12I-J25' : 350,
        'L1_30M-EM20ITAU12' : 387,
        'L1_MJJ-400-CF' : 397,
        #'L1_LAR-EM' : 351,
        #'L1_LAR-J' : 352,
        #'L1_LAR-ZEE' : 71,
        #'L1_LAR-ZEE_LAR-EM' : 285,
        #'L1_BPH-2M9-2MU4_BPH-0DR15-2MU4' : 426,
        #'L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4' : 427,
        'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4-BO' : 488,
        #'L1_BPH-8M15-2MU6_BPH-0DR22-2MU6' : 301,
        'L1_BPH-8M15-0DR22-2MU6' : 107,
        'L1_BPH-2M9-2DR15-2MU6' : 110,
        'L1_BPH-2M9-0DR15-MU6MU4' : 125,
        'L1_BPH-8M15-0DR22-MU6MU4-BO' : 126,
        'L1_BPH-2M9-0DR15-2MU4' : 127,
        'L1_BPH-0M9-EM7-EM5' : 80,
        'L1_BPH-0DR3-EM7J15' : 84,
        'L1_BPH-0M9-EM7-EM5_MU6' : 124,
        'L1_BPH-0DR3-EM7J15_MU6' : 134,
        'L1_BPH-0M9-EM7-EM5_2MU4' : 153,
        'L1_BPH-0DR3-EM7J15_2MU4' : 156,
        #'L1_MJJ-400-NFF-0DPHI20' : 128,
        'L1_MJJ-400-NFF-0DPHI22' : 129,
        'L1_MJJ-400-NFF-0DPHI24' : 133,
        'L1_MJJ-400-NFF-0DPHI26' : 136,
        'L1_EM18VHI_MJJ-300' : 385,
        'L1_LATE-MU10_XE50' : 354,
        'L1_LATE-MU10_XE40' : 466,
        'L1_LATE-MU10_J50' : 355,
        'L1_TAU60_DR-TAU20ITAU12I' : 76,
        'L1_SC111-CJ15' : 356,
        #'L1_LFV-EM8I-MU11' : 138,
        #'L1_LFV-EM12I-MU6' : 139,
        'L1_BPH-0M10-3MU4' : 143,
        'L1_ZAFB-04DPHI-EM15I' : 152,
        'L1_ZAFB-25DPHI-EM15I' : 157,
        #'L1_ZAFB-25DPHI-EM18I' : 158,
        'L1_DPHI-M70-2EM12I' : 159,
        #'L1_DY-DR-2MU4' : 315,
        'L1_DY-BOX-2MU6' : 318,
        'L1_DY-BOX-2MU4' : 160,
        'L1_CEP-CJ60' : 162,
        #'L1_CEP-CJ50' : 164,
        'L1_MBTS_1_EMPTY' : 164,
        'L1_MBTS_2_EMPTY' : 166,
        'L1_MBTS_1_1_EMPTY' : 168,
        'L1_BPH-8M15-2MU4-BO' : 165,
        'L1_CALREQ2' : 511,

}


    Lvl1Flags.prescales = {}

