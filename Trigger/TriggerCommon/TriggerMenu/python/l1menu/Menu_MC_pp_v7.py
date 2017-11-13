# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Run this file in order to print out the empty slots

def print_available():
    from TriggerMenu.l1.Lvl1Flags import Lvl1Flags
    defineMenu()
    available = []
    for i in range(512):
      if i==463 or  i==464: continue #reserved for L1_RD2_BGRP14 and L1_RD3_BGRP15
      if i>=509 and i<=511: continue #reserved for CALREQ
      if not i in Lvl1Flags.CtpIdMap().values(): available.append(str(i))
    available.sort()
    print "There are %d available CTP IDs:"%len(available),",".join(available)
    print "IDs >= 472 go in partition 2, IDs >= 492 go in partition 3"

def defineMenu():
    """
    Defines the following LvlFlags:
    
    thresholds   .... list of all threshold names in the menu (includes forced thresholds)
    items        .... list of all L1 item names in the menu
    CtpIdMap     .... map of item names to CTP IDs
    """
    
    from TriggerMenu.l1.Lvl1Flags import Lvl1Flags

    Lvl1Flags.CTPVersion = 4 # new CTP

    Lvl1Flags.BunchGroupPartitioning = [1, 15, 15] # partition 1: 1-10, partition 2: empty (was 14), partition 3: 15 (note that BGRP0 is used by all items)
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
        'EM8I',
        'EM10VH', 'EM12', 'EM15', 'EM15VH',
        'EM15VHI',
        'EM18VHI',  'EM20VH', 'EM20VHI', 'EM22VHI', 'EM22VHIM', 'EM24VHI', 'EM24VHIM', 

        # 1 x ZB
        'ZB_EM15',

        #--------------------------
        # SLOT 7 / CON 2,3 (TAU1,2)
        #--------------------------

        # 16 x TAU
        'HA8', 'HA12', 'HA12IL', 'HA12IM', 'HA12IT', 'HA15', 'HA20', 'HA20IL',  'HA20IM', 'HA20IT', 'HA25', 'HA25IM', 'HA30', 'HA40', 'HA60', 'HA100', 

        #----------------------
        # SLOT 8 / CON 0 (JET1)
        #----------------------
        # 10 x 3-bit JET (can have multiplicity 4 or more)

        # 8 x JETs and central jets
        'J12', 'J15', 'J15.0ETA25','J20', 'J25','J25.0ETA23','J30', 'J40',
        
        # 2 x VBF
        'J20.0ETA49', 'J30.0ETA49',


        #----------------------
        # SLOT 8 / CON 1 (JET2)
        #----------------------
        # 15 x 2-bit JET (can have maximum multiplicity of 3) (SLOT 8, CON 1)

        # 3 x Central Jet
        'J35.0ETA23','J40.0ETA25', 'J20.28ETA31',

        # 6 Jets
        'J50', 'J75', 'J85', 'J100', 'J120', 'J400',

        # 6 x FJ              
        'J15.31ETA49', 'J20.31ETA49', 'J30.31ETA49', 'J50.31ETA49', 'J75.31ETA49', 'J100.31ETA49', 


        #---------------------
        # SLOT 8 / CON 2 (EN1)
        #---------------------
        # 24 x 1-bit thresholds
        
        # 8 x TE
        #'TE3', 'TE5', 'TE10', 'TE15', 'TE20', 'TE25', 'TE30', 'TE40',
        #'TE5', 'TE30', 'TE40', 'TE45', 'TE50', 'TE55', 'TE60', 'TE65',
        #'TE5', 'TE10', 'TE20', 'TE30', 'TE40', 'TE50', 'TE60', 'TE70',
        'TE5', 'TE10', 'TE15', 'TE20', 'TE25', 'TE30', 'TE40', 'TE50',
        #'TE2', 'TE3', 'TE4', 'TE5', 'TE10', 'TE20', 'TE30', 'TE70',

        # 8 x XE (for standard XE)
        #'XE35', 'XE40', 'XE45', 'XE50', 'XE55', 'XE60', 'XE70', 'XE80',
        # 8 x XE (for restricted ETA XE: check ATR-13234 for any further change)
        #'XE10', 'XE20', 'XE25', 'XE30', 'XE65', 'XE75', 'XE150', 'XE300',

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
        #'TE3.0ETA24', 'TE5.0ETA24','TE10.0ETA24', 'TE15.0ETA24', 'TE20.0ETA24', 'TE25.0ETA24', 'TE30.0ETA24', 'TE40.0ETA24',
        #'TE5.0ETA24', 'TE30.0ETA24','TE40.0ETA24', 'TE45.0ETA24', 'TE50.0ETA24', 'TE55.0ETA24', 'TE60.0ETA24', 'TE65.0ETA24',
        'TE5.0ETA24', 'TE10.0ETA24', 'TE15.0ETA24', 'TE20.0ETA24', 'TE25.0ETA24', 'TE30.0ETA24', 'TE40.0ETA24', 'TE70.0ETA24',

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

        # 24 x MBTSSI (all FTHR)
        'MBTS_A0', 'MBTS_A1', 'MBTS_A2', 'MBTS_A3', 'MBTS_A4'  , 'MBTS_A5', 'MBTS_A6', 'MBTS_A7', 
        'MBTS_A8', 'MBTS_A10', 'MBTS_A12' , 'MBTS_A14',
        'MBTS_C0', 'MBTS_C1', 'MBTS_C2', 'MBTS_C3', 'MBTS_C4'  , 'MBTS_C5', 'MBTS_C6', 'MBTS_C7', 
        'MBTS_C8', 'MBTS_C10', 'MBTS_C12' , 'MBTS_C14',

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
        'AFP_FSA_SIT', 'AFP_FSA_TOF', 'AFP_FSA_LOG',
        'AFP_FSC_SIT', 'AFP_FSC_LOG', 'AFP_FSC_TOF',

        #-------------------------------------------------------------------
        #L1 TOPO inputs 
        #-------------------------------------------------------------------
        
        #HT
        'HT190-AJ15all.ETA21',
        'HT190-J15s5.ETA21',
        'HT150-AJ20all.ETA31',
        'HT150-J20s5.ETA31',

        

        #ZH 
        '10MINDPHI-AJ20s2-XE50', '10MINDPHI-J20s2-XE50', '10MINDPHI-J20s2-XE30', '10MINDPHI-J20ab-XE50', '10MINDPHI-CJ20ab-XE50', 
        #Jpsi T&P
        '1INVM5-EMs1-EMs6', '1INVM5-EM7s1-EMs6', '1INVM5-EM12s1-EMs6',        

        #W T&P
        '05MINDPHI-AJj10s6-XE0',
        '10MINDPHI-AJj10s6-XE0',
        '15MINDPHI-AJj10s6-XE0',
        '05MINDPHI-EM12s6-XE0',
        '15MINDPHI-EM12s6-XE0',
        '05MINDPHI-EM15s6-XE0',
        '15MINDPHI-EM15s6-XE0',
        '05RATIO-XE0-HT0-AJj15all.ETA49',
        '90RATIO2-XE0-HT0-AJj15all.ETA49',
        '250RATIO2-XE0-HT0-AJj15all.ETA49',
        'HT20-AJj15all.ETA49',
        'NOT-02MATCH-EM10s1-AJj15all.ETA49',
        '25MT-EM12s6-XE0',
        '35MT-EM12s6-XE0',
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
        'MULT-CMU4ab',
        'MULT-CMU6ab',
        
        #B-jet
        '0DR04-MU4ab-CJ15ab', '0DR04-MU4ab-CJ20ab', '0DR04-MU4ab-CJ30ab', '0DR04-MU6ab-CJ20ab', '0DR04-MU6ab-CJ25ab',
        #B-physics
#SX        '2DR15-2MU4ab', 
#SX        '2INVM999-2MU4ab', 
#SX        '4INVM8-2MU4ab',
        '2DR15-2MU6ab', 
#SX        '2INVM999-2MU6ab', 
#SX        '4INVM8-2MU6ab',

        # L1 thresholds for L1Topo streamers
#SX        '2INVM999-CMU4ab-MU4ab',
#SX        '2INVM999-2CMU4ab',
#SX        '2INVM999-MU6ab-MU4ab',
#SX        '2INVM999-ONEBARREL-MU6ab-MU4ab',
#SX        '2INVM999-CMU6ab-CMU4ab',
#SX        '4INVM8-CMU4ab-MU4ab',
#SX        '4INVM8-2CMU4ab',
#SX        '4INVM8-MU6ab-MU4ab',
#SX        '4INVM8-ONEBARREL-MU6ab-MU4ab',
#SX        '4INVM8-CMU6ab-CMU4ab',        
        '2DR99-2MU4ab',
        '5DETA99-5DPHI99-MU6ab-MU4ab',
        '5DETA99-5DPHI99-2MU6ab',
        '0DR10-MU10ab-MU6ab',
        '0DR15-2MU6ab',
#        '0DETA04-0DPHI03-EM8abi-MU10ab',
        '0DETA04-EM8abi-MU10ab',
        '0DPHI03-EM8abi-MU10ab',
#        '0DETA04-0DPHI03-EM15abi-MUab',
        '0DETA04-EM15abi-MUab',
        '0DPHI03-EM15abi-MUab',
        '10MINDPHI-AJ20s2-XE50',
        '10MINDPHI-J20s2-XE50',
        '10MINDPHI-J20ab-XE50',
        '10MINDPHI-CJ20ab-XE50',
        '900INVM9999-AJ30s6-AJ20s6',
        '800INVM9999-AJ30s6-AJ20s6',
        '700INVM9999-AJ30s6-AJ20s6',
        '500INVM9999-AJ30s6-AJ20s6',
        '400INVM9999-AJ30s6-AJ20s6',
        #'350INVM9999-AJ30s6-AJ20s6',
        '300INVM9999-AJ30s6-AJ20s6',
        '200INVM9999-AJ30s6-AJ20s6',
        '100INVM9999-AJ30s6-AJ20s6',
        #'350INVM9999-J30s6-J20s6',
        #'300INVM9999-J30s6-J20s6',
        #'250INVM9999-J30s6-J20s6',
        #'200INVM9999-J30s6-J20s6',
        'HT150-AJj15all.ETA49',
        '0MATCH-4AJ20.ETA31-4AJj15.ETA31',
        '100RATIO-0MATCH-TAU30si2-EMall',
        'NOT-0MATCH-TAU30si2-EMall',
        '0DR28-MU10ab-TAU12abi',
        '1DISAMB-TAU12abi-J25ab',
        '1DISAMB-EM15his2-TAU12abi-J25ab',
        'DISAMB-0DR28-EM15his2-TAU12abi',
        '1DISAMB-J25ab-0DR28-EM15his2-TAU12abi',
        '1DISAMB-TAU20abi-TAU12abi-J25ab',
        '0DR25-TAU20abi-TAU12abi',
        '0DR28-TAU20abi-TAU12abi',
        '0DETA20-0DPHI20-TAU20abi-TAU12abi',
        '1DISAMB-J25ab-0DR25-TAU20abi-TAU12abi',
        '1DISAMB-J25ab-0DR28-TAU20abi-TAU12abi',
        'DISAMB-30INVM-EM20his2-TAU12ab',
        '400INVM9999-AJ30s6.ETA31-AJ20s6.31ETA49',
        'LAR-EM50s1',
        'LAR-J100s1',
#SX        '2DR15-CMU4ab-MU4ab',
#SX        '2DR15-2CMU4ab',
#SX        '2DR15-MU6ab-MU4ab',
#SX        '2DR15-ONEBARREL-MU6ab-MU4ab',
#SX        '2DR15-CMU6ab-CMU4ab',

        '2INVM9-2MU6ab',
        '7INVM15-2MU4ab', 
        '2INVM8-ONEBARREL-MU6ab-MU4ab', 
        '0DR24-2CMU4ab', 
        '0DR22-2MU6ab',
        '0DR34-2MU4ab', 
        '0DR24-2MU4ab', 
        '0DR24-CMU4ab-MU4ab',  
        '2INVM8-CMU4ab-MU4ab',
        '0DR15-2MU4ab',  
        '0DR15-MU6ab-MU4ab',  
        '0DR22-MU6ab-MU4ab', 
        '8INVM15-MU6ab-MU4ab',  
        '8INVM15-2MU6ab', 
        '2INVM8-2MU4ab',  # ATR-15197 '2INVM9-2MU4ab',
        '2INVM8-MU6ab-MU4ab', # ATR-15197 '2INVM9-MU6ab-MU4ab',
        '2INVM9-2MU4ab',
        '2INVM9-MU6ab-MU4ab',
        'KF-XE40-AJall',
        'KF-XE50-AJall',
        'KF-XE55-AJall',
        'KF-XE60-AJall',
        'KF-XE65-AJall',
        'KF-XE75-AJall',

        'LATE-MU10s1',
        'SC111-CJ15ab.ETA26',
        'SC85-CJ15ab.ETA26',
        
        # ALFA (the replication is needed to build all the combinations in the CTPCore)
        'ALFA_B7R1L', 'ALFA_B7R1U', 'ALFA_A7R1L', 'ALFA_A7R1U', 'ALFA_A7L1L', 'ALFA_A7L1U', 'ALFA_B7L1L', 'ALFA_B7L1U',
        'ALFA2_B7R1L', 'ALFA2_B7R1U', 'ALFA2_A7R1L', 'ALFA2_A7R1U', 'ALFA2_A7L1L', 'ALFA2_A7L1U', 'ALFA2_B7L1L', 'ALFA2_B7L1U',
        'ALFA3_B7R1L', 'ALFA3_B7R1U', 'ALFA3_A7R1L', 'ALFA3_A7R1U', 'ALFA3_A7L1L', 'ALFA3_A7L1U', 'ALFA3_B7L1L', 'ALFA3_B7L1U',
        'ALFA4_B7R1L', 'ALFA4_B7R1U', 'ALFA4_A7R1L', 'ALFA4_A7R1U', 'ALFA4_A7L1L', 'ALFA4_A7L1U', 'ALFA4_B7L1L', 'ALFA4_B7L1U',
#ATR-13743      
#        'ALFA_B7R1L_OD', 'ALFA_B7R1U_OD', 'ALFA_A7R1L_OD', 'ALFA_A7R1U_OD', 'ALFA_A7L1L_OD', 'ALFA_A7L1U_OD', 'ALFA_B7L1L_OD', 'ALFA_B7L1U_OD',


        ]

    
    Lvl1Flags.items = [

        
        # single EM

        'L1_EM3','L1_EM7','L1_EM12', 'L1_EM8VH',
#        'L1_EM8I',
        'L1_EM10VH', 'L1_EM15',
        'L1_EM15I','L1_EM15VHI',
        'L1_EM24VHIM','L1_EM15VH', 'L1_EM18VHI', 'L1_EM20VH', 'L1_EM20VHI', 'L1_EM22VHI', 'L1_EM22VHIM','L1_EM24VHI', # see Jira ATR-10334'L1_EM22VHLIL', #'L1_EM50V',
        'L1_EM3_EMPTY', 'L1_EM7_EMPTY', 'L1_EM7_UNPAIRED_ISO', 'L1_EM7_FIRSTEMPTY','L1_EM7_UNPAIRED_NONISO',
        'L1_EM20VH_FIRSTEMPTY',
        'L1_EM22VHI_FIRSTEMPTY',
        #'L1_EM15_BGRP7',

        # see savannah https://savannah.cern.ch/bugs/?103935

        # combined EM -jet
        #'L1_JJ15.23ETA49', # 4x4 window

        # single MU
        "L1_MU4", "L1_MU6", "L1_MU10", "L1_MU11", "L1_MU20", "L1_MU21",

        "L1_MU4_EMPTY", "L1_MU6_EMPTY", "L1_MU4_FIRSTEMPTY", "L1_MU11_EMPTY",
        'L1_MU4_UNPAIRED_ISO','L1_MU4_UNPAIRED_NONISO',
         'L1_MU20_FIRSTEMPTY','L1_MU21_FIRSTEMPTY',

        'L1_2EM3_VTE50', 
        
        # multi lepton
        "L1_2EM3", "L1_2EM7", "L1_2EM10VH", "L1_2EM12", "L1_2EM15", "L1_2EM15VH", "L1_2EM15VHI", "L1_EM7_2EM3", "L1_EM12_2EM3", "L1_EM15VH_3EM7",
	"L1_2EM20VH", "L1_EM15VH_3EM10VH", "L1_EM20VH_3EM10VH", "L1_EM20VH_2EM10VH_3EM8VH",
        "L1_EM7_MU10",
        "L1_2MU4", "L1_2MU6", "L1_2MU10", "L1_2MU20_OVERLAY", "L1_MU10_2MU6", "L1_MU11_2MU6", "L1_3MU4", "L1_MU6_2MU4", "L1_3MU6",  "L1_4MU4", "L1_MU6_3MU4", "L1_2MU6_3MU4",

        "L1_EM8I_MU10", "L1_EM15I_MU4", "L1_2EM8VH_MU10", "L1_EM15VH_MU10", "L1_EM7_MU11", "L1_EM7_MU20", "L1_EM8VH_MU20",

        # single tau
        "L1_TAU12", "L1_TAU12IM",  "L1_TAU20", "L1_TAU20IM",
        "L1_TAU30","L1_TAU30_EMPTY","L1_TAU30_UNPAIRED_ISO", "L1_TAU40", "L1_TAU60", "L1_TAU100", "L1_TAU8", "L1_TAU8_EMPTY", 
        "L1_TAU8_UNPAIRED_ISO","L1_TAU8_FIRSTEMPTY","L1_TAU8_UNPAIRED_NONISO",

        # multi tau
        "L1_TAU20IM_2TAU12IM", "L1_TAU20_2TAU12", "L1_TAU40_2TAU20IM", "L1_TAU60_2TAU40",

        # combined tau - lepton
        "L1_EM15VHI_2TAU12IM",          
        "L1_EM15VHI_2TAU12IM_J25_3J12",   
	"L1_EM15VHI_2TAU12IM_4J12",
        "L1_EM15VHI_TAU40_2TAU15",
        "L1_MU10_TAU12IM",  
        "L1_MU10_TAU12IM_J25_2J12",
        "L1_MU10_TAU12IM_3J12", 
#        "L1_MU10_TAU12_J25_2J12", 
#        "L1_MU10_TAU20",
        "L1_MU10_TAU20IM",
        "L1_MU11_TAU20IM",
        "L1_MU10_TAU20IM_J25_2J20",	

        # combined tau - jet
#        "L1_TAU25IT_2TAU12IT_2J25_3J12", 

        "L1_TAU20IM_2TAU12IM_J25_2J20_3J12",
        "L1_TAU20IM_2TAU12IM_4J12",
        "L1_TAU25IM_2TAU20IM_2J25_3J20", 
        'L1_DR-MU10TAU12I_TAU12I-J25', 
        "L1_MU10_TAU12I-J25",
        "L1_2TAU20IM_3J20",
        "L1_2TAU20IM_J25_3J20",
        "L1_TAU25IM_2TAU12IM_J25_3J12",



        # combined tau - xe
        "L1_TAU20IM_2J20_XE45", 
#        "L1_TAU20_2J20_XE45", 
#        "L1_TAU20IM_2J20_XE50",   

        #"L1_EM15HI_2TAU12I_XE35", 
        "L1_EM15VHI_2TAU12IM_XE35", 
        "L1_EM15VHI_TAU20IM_2TAU15_J25_2J20_3J15",	 
        'L1_EM20VHI_TAU20IM_2TAU20_J25_3J20',


        "L1_MU10_TAU12IM_XE35",  
#        "L1_MU10_TAU12IM_XE40",  

        "L1_TAU20IM_2TAU12IM_XE35", 
        "L1_TAU40_2TAU12IM_XE40",  



        # combined em - jet
        #'L1_EM15VH_J15.23ETA49',
#        'L1_EM15VH_JJ15.23ETA49',
        'L1_EM18VHI_3J20',
        'L1_EM20VH_3J20',
        #'L1_EM18VH_3J20',

        # combined mu - jet
        'L1_MU6_J20', 'L1_MU6_J30.0ETA49_2J20.0ETA49', 'L1_MU6_J40', 'L1_MU6_J75',
#ATR-13743        'L1_MU4_J30',
        #'L1_MU4_J50',
        'L1_MU10_2J20','L1_MU10_3J20', 'L1_MU10_2J15_J20',
        'L1_MU20_J40',
        'L1_MU20_XE30',

        # single jet
        "L1_J12","L1_J15","L1_J20","L1_J25", "L1_J30", "L1_J40", "L1_J50" ,"L1_J75","L1_J85", "L1_J100", "L1_J120", "L1_J400",
        "L1_J20.31ETA49", "L1_J30.31ETA49", "L1_J50.31ETA49", "L1_J75.31ETA49", "L1_J100.31ETA49",
        #'L1_J15.23ETA49',
         'L1_J15.31ETA49',
        "L1_J20.28ETA31",
        #'L1_J40.0ETA25', 
        "L1_J12_EMPTY","L1_J12_FIRSTEMPTY", "L1_J12_UNPAIRED_ISO", "L1_J12_UNPAIRED_NONISO", "L1_J12_ABORTGAPNOTCALIB", 
        "L1_J12_UNPAIREDB1", "L1_J12_UNPAIREDB2", 
        "L1_J15.31ETA49_UNPAIRED_ISO",
        "L1_J30_EMPTY", "L1_J30_FIRSTEMPTY", "L1_J30.31ETA49_EMPTY", "L1_J30.31ETA49_UNPAIRED_ISO",
        "L1_J30.31ETA49_UNPAIRED_NONISO",
        "L1_J50_UNPAIRED_ISO", "L1_J50_UNPAIRED_NONISO", "L1_J50_ABORTGAPNOTCALIB",         
        "L1_J12_BGRP12", "L1_J30.31ETA49_BGRP12",
        'L1_J100_FIRSTEMPTY',
        'L1_J100.31ETA49_FIRSTEMPTY',
        
        # multi jet

        "L1_J20_J20.31ETA49",
        "L1_3J15", "L1_3J40",  "L1_3J50", "L1_4J15", "L1_4J20",
        "L1_4J20.0ETA49",
        "L1_3J15.0ETA25_XE40",
        
        #"L1_3J75",
        #"L1_4J30",
        "L1_6J15",
        "L1_J75_3J20",
        "L1_J85_3J30",
        "L1_J30.0ETA49_2J20.0ETA49",

        "L1_J15.0ETA25_2J15.31ETA49",
        "L1_J25.0ETA23_2J15.31ETA49",
        "L1_J40.0ETA25_2J15.31ETA49",
        "L1_J40.0ETA25_2J25_J20.31ETA49",
        "L1_J40.0ETA25_2J30_J20.31ETA49",
        
        # multi jet central
        "L1_3J25.0ETA23",
        "L1_3J35.0ETA23",
        "L1_4J15.0ETA25",
        "L1_5J15.0ETA25", 
        
        # combined jet
        "L1_2J15_XE55", "L1_J40_XE50",

        #'L1_2J40_XE45',
        "L1_2J50_XE40", "L1_J40_XE60","L1_J40.0ETA25_XE50",

        # XE
        "L1_XE35", "L1_XE40", "L1_XE45", "L1_XE50", 
        "L1_XE55", "L1_XE60", "L1_XE70", "L1_XE80",
        'L1_XE10', 'L1_XE30', 'L1_XE300',
        
        #XS
        'L1_EM12_XS20', 'L1_EM15_XS30',
        
        # TE
        #"L1_TE3",        "L1_TE5",        "L1_TE10",        "L1_TE15",        "L1_TE20",        "L1_TE25",        "L1_TE30",        "L1_TE40",
        #"L1_TE5",        "L1_TE30",        "L1_TE40",        "L1_TE45",        "L1_TE50",        "L1_TE55",        "L1_TE60",        "L1_TE65",
        #"L1_TE5", "L1_TE10", "L1_TE20", "L1_TE30", "L1_TE40", "L1_TE50", "L1_TE60", "L1_TE70",
        "L1_TE5", "L1_TE10", "L1_TE15", "L1_TE20", "L1_TE25", "L1_TE30", "L1_TE40", "L1_TE50",
        #"L1_TE2",        "L1_TE3",   "L1_TE4",  "L1_TE5",        "L1_TE10",        "L1_TE20",        "L1_TE30",      "L1_TE70",

        #"L1_TE3.0ETA24", "L1_TE5.0ETA24", "L1_TE10.0ETA24", "L1_TE15.0ETA24", "L1_TE20.0ETA24", "L1_TE25.0ETA24", "L1_TE30.0ETA24", "L1_TE40.0ETA24", 
        #"L1_TE5.0ETA24", "L1_TE30.0ETA24", "L1_TE40.0ETA24", "L1_TE45.0ETA24", "L1_TE50.0ETA24", "L1_TE55.0ETA24", "L1_TE60.0ETA24", "L1_TE65.0ETA24", 
        #"L1_TE5.0ETA24", "L1_TE10.0ETA24", "L1_TE15.0ETA24", "L1_TE20.0ETA24", "L1_TE25.0ETA24", "L1_TE30.0ETA24", "L1_TE40.0ETA24","L1_TE70.0ETA24",
        #"L1_VTE2",        "L1_VTE3",   "L1_VTE4",  "L1_VTE5",        "L1_VTE10",

        #VTE + MBTS
        #"L1_MBTS_1_VTE2", "L1_MBTS_1_VTE3", "L1_MBTS_1_VTE4", "L1_MBTS_1_VTE5", "L1_MBTS_1_VTE10", "L1_MBTS_2_VTE2", "L1_MBTS_2_VTE3", "L1_MBTS_2_VTE4", "L1_MBTS_2_VTE5", "L1_MBTS_2_VTE10", 
        
        #TE + MU
        "L1_MU4_TE10", "L1_MU4_TE20", "L1_MU4_TE30", "L1_MU4_TE40",# "L1_MU4_TE50",

        # MBTS
        "L1_MBTS_1", "L1_MBTS_2", "L1_MBTS_1_1",
        "L1_MBTS_1_EMPTY", "L1_MBTS_1_UNPAIRED_ISO", 
        "L1_MBTS_2_EMPTY", "L1_MBTS_2_UNPAIRED_ISO",
        "L1_MBTS_1_1_EMPTY", "L1_MBTS_1_1_UNPAIRED_ISO","L1_MBTS_4_A_UNPAIRED_ISO","L1_MBTS_4_C_UNPAIRED_ISO",
         'L1_MBTSA0' ,'L1_MBTSA1' ,'L1_MBTSA2' ,'L1_MBTSA3' ,'L1_MBTSA4' ,
         'L1_MBTSA5' ,'L1_MBTSA6' ,'L1_MBTSA7' ,'L1_MBTSA8' ,
         'L1_MBTSA10','L1_MBTSA12','L1_MBTSA14',
         'L1_MBTSC0' ,'L1_MBTSC1' ,'L1_MBTSC2' ,'L1_MBTSC3' ,'L1_MBTSC4' ,
         'L1_MBTSC5' ,'L1_MBTSC6' ,'L1_MBTSC7' ,'L1_MBTSC8' ,
         'L1_MBTSC10','L1_MBTSC12','L1_MBTSC14',
        # beam splashes
        "L1_MBTS_4_A", "L1_MBTS_4_C",
        
        #Overlay
#        "L1_MBTS_1_OVERLAY","L1_TE20_OVERLAY",
        
        #BeamSpot
#        "L1_MBTS_1_VTE70",

        #BGRP9
        'L1_MBTS_1_BGRP9','L1_MBTS_2_BGRP9',

        #BGRP11
        'L1_MBTS_1_BGRP11','L1_MBTS_2_BGRP11',

        # RNDM
        'L1_RD0_FILLED', 'L1_RD0_UNPAIRED_ISO','L1_RD0_EMPTY',
        "L1_RD1_FILLED","L1_RD1_EMPTY","L1_RD1_BGRP10",
        "L1_RD2_FILLED","L1_RD2_EMPTY","L1_RD2_BGRP12",
        "L1_RD3_FILLED","L1_RD3_EMPTY",
        'L1_RD0_FIRSTEMPTY', 
        'L1_RD0_BGRP9', 'L1_RD0_BGRP10', 'L1_RD0_BGRP11', 'L1_RD0_ABORTGAPNOTCALIB',

        #LUCID
        'L1_LUCID', 'L1_LUCID_EMPTY', 'L1_LUCID_UNPAIRED_ISO', 
        'L1_LUCID_A_C_EMPTY', 'L1_LUCID_A_C_UNPAIRED_ISO', 'L1_LUCID_A_C_UNPAIRED_NONISO',
        #'L1_LUCID_COMM_EMPTY', 'L1_LUCID_COMM_UNPAIRED_ISO', 

        'L1_LUCID_BGRP9','L1_LUCID_BGRP11',
        

        # ZDC
        'L1_ZDC_A', 'L1_ZDC_C', 'L1_ZDC_A_C',
        'L1_ZDC_AND', # FTHR

        # VDM
        'L1_BGRP9', #"L1_BGRP7",

        # TRT
        # "L1_TRT",
        "L1_TRT_FILLED",
        "L1_TRT_EMPTY",

        # TGC
        "L1_TGC_BURST",
#        "L1_TGC_BURST_EMPTY",

        # LHCF
        "L1_LHCF", "L1_LHCF_UNPAIRED_ISO", "L1_LHCF_EMPTY",
    
        #CALREQ
        "L1_CALREQ2",

        #ZB
        "L1_ZB",

        # BPTX
        "L1_BPTX0_BGRP0", "L1_BPTX1_BGRP0",
        
        # BCM
        'L1_BCM_Wide_BGRP0','L1_BCM_AC_CA_BGRP0', 'L1_BCM_Wide_EMPTY','L1_BCM_Wide_UNPAIRED_ISO','L1_BCM_Wide_UNPAIRED_NONISO', 'L1_BCM_AC_CA_UNPAIRED_ISO',
        'L1_BCM_AC_UNPAIRED_ISO','L1_BCM_CA_UNPAIRED_ISO',
        'L1_BCM_AC_UNPAIREDB1','L1_BCM_CA_UNPAIREDB2',
        'L1_BCM_AC_UNPAIRED_NONISO','L1_BCM_CA_UNPAIRED_NONISO',
        'L1_BCM_AC_ABORTGAPNOTCALIB', 'L1_BCM_CA_ABORTGAPNOTCALIB','L1_BCM_Wide_ABORTGAPNOTCALIB',
        'L1_BCM_AC_CALIB', 'L1_BCM_CA_CALIB','L1_BCM_Wide_CALIB',

        #AFP
#        'L1_AFP_NSC', 'L1_AFP_FSC', 
#        'L1_AFP_C_ANY', 'L1_AFP_C_ANY_UNPAIRED_ISO', 'L1_AFP_C_ANY_UNPAIRED_NONISO', 'L1_AFP_C_ANY_EMPTY', 'L1_AFP_C_ANY_FIRSTEMPTY', 'L1_AFP_C_AND',

        'L1_AFP_A_BGRP0','L1_AFP_A_SPECTOF_BGRP0','L1_AFP_C_BGRP0','L1_AFP_C_SPECTOF_BGRP0',
        'L1_AFP_NSA_BGRP0', 'L1_AFP_NSC_BGRP0', 'L1_AFP_FSA_SIT_BGRP0', 'L1_AFP_FSC_SIT_BGRP0',
        'L1_AFP_A_OR_C_UNPAIRED_ISO', 'L1_AFP_A_OR_C_UNPAIRED_NONISO', 
        'L1_AFP_A_OR_C_EMPTY', 'L1_AFP_A_OR_C_FIRSTEMPTY',
        'L1_AFP_A_AND_C_SPECTOF',
        'L1_AFP_A_AND_C','L1_AFP_A_OR_C', 'L1_AFP_A', 'L1_AFP_C',
        'L1_AFP_A_AND_C_MBTS_2','L1_AFP_A_OR_C_MBTS_2',
        'L1_AFP_A_AND_C_J12','L1_AFP_A_OR_C_J12',
        'L1_EM3_AFP_A_AND_C','L1_EM3_AFP_A_OR_C',
        'L1_MU4_AFP_A_AND_C','L1_MU4_AFP_A_OR_C',
        'L1_AFP_A_AND_C_TE5','L1_AFP_A_OR_C_TE5',
        'L1_AFP_A_AND_C_SPECTOF_J50',
        'L1_AFP_A_AND_C_SPECTOF_J75',
        'L1_AFP_A_AND_C_SPECTOF_J100',


        #--------------------------------
        # TOPO items
        #--------------------------------

        # B-jet
        'L1_BTAG-MU4J15', 'L1_BTAG-MU6J20',
        #'L1_BTAG-MU4J20',
        'L1_3J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J30', 'L1_3J15_BTAG-MU6J25', 'L1_3J20_BTAG-MU4J20',
        #'L1_3J20_BTAG-MU4J30', 'L1_3J20_BTAG-MU6J25',

        # ZH
        'L1_DPHI-AJ20s2XE50', 'L1_J40_DPHI-J20s2XE50', 'L1_J40_DPHI-J20XE50', 'L1_J40_DPHI-CJ20XE50', #'L1_J40_DPHI-J20s2XE30',
#SX        'L1_BPH-DR-2MU4-B_BPH-2M-2MU4-B',
#SX        'L1_BPH-DR-2MU4-B_BPH-4M8-2MU4-B',
#SX        'L1_BPH-DR-2MU4-BO_BPH-2M-2MU4-BO',
#SX        'L1_BPH-DR-2MU4-BO_BPH-4M8-2MU4-BO',

        #Higgsino
        'L1_2MU4_J40_XE50',
        'L1_2MU4_J20_XE40_DPHI-J20s2XE30','L1_2MU4_J20_XE50_DPHI-J20s2XE30',
        'L1_MU4_J30_XE40_DPHI-J20s2XE30','L1_MU4_J50_XE50_DPHI-J20s2XE30',
        'L1_MU4_XE60',

        # HT
        'L1_HT190-J15.ETA21',
        'L1_HT190-J15s5.ETA21',
        'L1_HT150-J20.ETA31',
        'L1_HT150-J20s5.ETA31',   

        "L1_HT150-J20s5.ETA31_MJJ-400",
        "L1_HT150-J20s5.ETA31_MJJ-400-CF",
        
        # Jpsi T&P
        'L1_JPSI-1M5', 'L1_JPSI-1M5-EM7', 'L1_JPSI-1M5-EM12',    
        'L1_KF-XE50', 'L1_KF-XE55', 'L1_KF-XE60', 'L1_KF-XE65', 'L1_KF-XE75',
        
        # W T&P
#        'L1_W-05DPHI-JXE-0',
#        'L1_W-10DPHI-JXE-0',
#        'L1_W-15DPHI-JXE-0',
#        'L1_W-05DPHI-EMXE-1',
        #'L1_W-15DPHI-EMXE-1',
#        'L1_W-05DPHI-EM15XE-1',
        #'L1_W-15DPHI-EM15XE-1',
        'L1_W-05RO-XEHT-0',
        'L1_W-90RO2-XEHT-0',
        'L1_W-250RO2-XEHT-0',
        'L1_W-HT20-JJ15.ETA49',
        'L1_W-NOMATCH',
        #'L1_W-NOMATCH_W-05RO-XEEMHT',
        'L1_EM12_W-MT25',
#        'L1_EM12_W-MT35',
        'L1_EM15_W-MT35',
        'L1_EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE',
        'L1_EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE',
        # Old
        'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0',
        'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20',
        'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE',
        'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE_XS30',
        'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE',
        #'L1_W-10DPHI-JXE-0',
        #'L1_W-20DPHI-JXE-1',
        #'L1_W-20DPHI-JXE-0',
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
        #'L1_EM3_W-MT40',

        #Bphysics items
#SX        'L1_BPH-DR-2MU4', 
#SX        'L1_BPH-2M-2MU4', 
#SX        'L1_BPH-4M8-2MU4',
#SX        'L1_BPH-DR-2MU4_BPH-2M-2MU4', 'L1_BPH-DR-2MU4_BPH-4M8-2MU4',

#SX        'L1_BPH-DR-2MU6', 
#SX        'L1_BPH-2M-2MU6', 'L1_BPH-4M8-2MU6',
#SX        'L1_BPH-DR-2MU6_BPH-2M-2MU6', 'L1_BPH-DR-2MU6_BPH-4M8-2MU6',

        # temporarily commented out until CTP Simulation is fixed (menu generation and TMC understand multiplicity topo)
#SX uncommenting
        'L1_2MU4-BO',
        'L1_2MU6-BO',
        'L1_MU6MU4-BO', 

        # L1 items for L1Topo streamers
#SX        'L1_BPH-2M-2MU4-B',
#SX        'L1_BPH-2M-2MU4-BO',
#SX        'L1_BPH-2M-MU6MU4',
#SX        'L1_BPH-2M-MU6MU4-B',
#SX        'L1_BPH-2M-MU6MU4-BO',    
#SX        'L1_BPH-4M8-2MU4-B',
#SX        'L1_BPH-4M8-2MU4-BO',
#SX        'L1_BPH-4M8-MU6MU4',
#SX        'L1_BPH-4M8-MU6MU4-B',        
#SX        'L1_BPH-4M8-MU6MU4-BO',
        'L1_DY-DR-2MU4',
        #'L1_DY-BOX-MU6MU4',
        'L1_DY-BOX-2MU6',
        'L1_LFV-MU',
        'L1_LFV-MU6',
        'L1_LFV-EM8I',
        'L1_LFV-EM15I',
        'L1_DPHI-J20s2XE50',
        'L1_J40_XE50_DPHI-J20s2XE50',
        'L1_DPHI-J20XE50',
        'L1_DPHI-CJ20XE50',
        'L1_2MU4_J20_XE30_DPHI-J20s2XE30',
        'L1_MJJ-800',
        'L1_MJJ-400',
        #'L1_MJJ-350',
#        'L1_MJJ-300',
#        'L1_MJJ-200',
        'L1_MJJ-100',
        'L1_HT150-JJ15.ETA49',
        'L1_HT150-JJ15.ETA49_MJJ-400',
        'L1_J4-MATCH',
        'L1_LLP-RO',
        'L1_LLP-NOMATCH',
        'L1_DR-MU10TAU12I',    
#        'L1_TAU12I-J25',    
        'L1_EM15TAU12I-J25',
        'L1_DR-EM15TAU12I',    
        'L1_DR-EM15TAU12I-J25',    
        'L1_TAU20ITAU12I-J25',
        'L1_DR-TAU20ITAU12I',    
        'L1_DR-TAU20ITAU12I-J25',

        'L1_DR25-TAU20ITAU12I',
        'L1_DR25-TAU20ITAU12I-J25',
        'L1_30M-EM20ITAU12',
        'L1_MJJ-400-CF',
    
        'L1_LAR-EM',
        'L1_LAR-J',
#SX        'L1_BPH-DR-2MU4-B',
#SX        'L1_BPH-DR-2MU4-BO',
#SX        'L1_BPH-DR-MU6MU4',
#SX        'L1_BPH-DR-MU6MU4-B',
#SX        'L1_BPH-DR-MU6MU4-BO',

#new bhysics
        #'L1_BPH-1M19-2MU4_BPH-0DR34-2MU4',
        'L1_BPH-2M9-2MU4_BPH-0DR15-2MU4',
        'L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4',
        'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4',
        'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',
        'L1_BPH-8M15-2MU6_BPH-0DR22-2MU6',
        'L1_2MU4-B',
        'L1_2MU6-B' ,
        #'L1_BPH-1M19-2MU4-B_BPH-0DR34-2MU4',
        #'L1_BPH-1M19-2MU4-BO_BPH-0DR34-2MU4' ,
        #'L1_BPH-2M8-2MU4-BO_BPH-0DR15-2MU4' ,
        'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4-B',
        'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4-BO',
        'L1_MU6_2MU4-B',

        'L1_BPH-8M15-MU6MU4',
        'L1_BPH-8M15-2MU6',
        
        # combined L1Topo for VBF
        'L1_MU6_MJJ-200',
        'L1_3J20_4J20.0ETA49_MJJ-400',
        'L1_3J20_4J20.0ETA49_MJJ-700',
        'L1_3J20_4J20.0ETA49_MJJ-800',
        'L1_3J20_4J20.0ETA49_MJJ-900',
        
   
        ### ATR-15062
        'L1_EM18VHI_MJJ-300',

        'L1_LATE-MU10_XE50','L1_LATE-MU10_XE40', 'L1_LATE-MU10_J50',

        'L1_TAU60_DR-TAU20ITAU12I',

        'L1_SC111-CJ15',
        'L1_SC85-CJ15',
        
        ##     # Partition 1
        ##     'L1_CALREQ0_P1', 'L1_CALREQ1_P1', 'L1_CALREQ2_P1',

        #ALFA
#ALFA_B7R1L_OD        
        'L1_ALFA_ELAST1',
        'L1_ALFA_ELAST2',
        'L1_ALFA_ELAST11','L1_ALFA_ELAST12',
        'L1_ALFA_ELAST13','L1_ALFA_ELAST14','L1_ALFA_ELAST15','L1_ALFA_ELAST15_Calib',
        'L1_ALFA_ELAST16','L1_ALFA_ELAST17','L1_ALFA_ELAST18','L1_ALFA_ELAST18_Calib',
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
#        'L1_TE5_ALFA_ANY','L1_TE5_ALFA_ANY_UNPAIRED_ISO','L1_TE5_ALFA_EINE',
#        'L1_TRT_ALFA_ANY','L1_TRT_ALFA_ANY_UNPAIRED_ISO',
#        'L1_TRT_ALFA_EINE',
#        #'L1_LHCF_ALFA_ANY_A',
#        #'L1_LHCF_ALFA_ANY_C',
##        'L1_LHCF_ALFA_ANY_A_UNPAIRED_ISO','L1_LHCF_ALFA_ANY_C_UNPAIRED_ISO',
#        'L1_ALFA_BGT','L1_ALFA_BGT_UNPAIRED_ISO',
        'L1_ALFA_BGT_BGRP1','L1_ALFA_BGT_BGRP4','L1_ALFA_BGT_BGRP10',
        'L1_ALFA_SHOWSYST5',
        'L1_ALFA_SYST9' ,'L1_ALFA_SYST10','L1_ALFA_SYST11','L1_ALFA_SYST12',
        'L1_ALFA_SYST17','L1_ALFA_SYST18',
        'L1_ALFA_ANY',
        'L1_ALFA_ANY_EMPTY','L1_ALFA_B1_EMPTY','L1_ALFA_B2_EMPTY','L1_ALFA_ANY_FIRSTEMPTY','L1_ALFA_ANY_UNPAIRED_ISO','L1_ALFA_ANY_UNPAIRED_NONISO',
        'L1_ALFA_ANY_BGRP10',
        'L1_ALFA_ANY_ABORTGAPNOTCALIB','L1_ALFA_ANY_CALIB',
        'L1_ALFA_B7L1U','L1_ALFA_B7L1L','L1_ALFA_A7L1U','L1_ALFA_A7L1L','L1_ALFA_A7R1U','L1_ALFA_A7R1L','L1_ALFA_B7R1U','L1_ALFA_B7R1L',
#        'L1_ALFA_ANY_A_EMPTY','L1_ALFA_ANY_C_EMPTY',
#        'L1_ALFA_B7L1U_OD','L1_ALFA_B7L1L_OD','L1_ALFA_A7L1U_OD','L1_ALFA_A7L1L_OD','L1_ALFA_A7R1U_OD','L1_ALFA_A7R1L_OD','L1_ALFA_B7R1U_OD','L1_ALFA_B7R1L_OD',
#        'L1_ALFA_B7L1_OD','L1_ALFA_A7L1_OD','L1_ALFA_B7R1_OD','L1_ALFA_A7R1_OD',
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


        #item with FTK trigger type, for FTK commissioning
        'L1_MU6_FTK',
        'L1_MU20_FTK',
        'L1_MU21_FTK',
        'L1_TAU12IM_FTK', 
        'L1_TAU20IM_FTK',
        'L1_MU20_TAU12IM_FTK', 
        'L1_TAU20IM_2TAU12IM_J25_2J20_3J12_FTK',
        'L1_DR-TAU20ITAU12I-J25_FTK',
        'L1_4J20_FTK',
        'L1_4J15.0ETA25_FTK',
        'L1_J15_FTK',
        'L1_J40_FTK',
        'L1_J100_FTK'
                
        ]
    

#CTP IDs are taken from this mapping. Every L1 item needs a unique ctpid.
# Run this file as python python/l1menu/Menu_MC_pp_v7.py to print out available IDs
# 463-464 are reserved for L1_RD2_BGRP14 and L1_RD3_BGRP15 (in MC_Physics_pp_v7)
# 509-511 are reserved for CALREQ

    Lvl1Flags.CtpIdMap = {

        'L1_EM3' : 0,
        'L1_EM7' : 1,
        'L1_EM12' : 2,
        'L1_EM8VH' : 3,
        'L1_EM10VH' : 4,
        #'L1_EM13VH' : 5,
        'L1_EM15' : 6,
        'L1_EM15VH' : 7,
        'L1_EM18VHI' : 8,
        'L1_EM20VH' : 9,
        'L1_EM20VHI' : 10,
        'L1_EM22VHI' : 11,
        'L1_EM22VHIM' : 295,
        'L1_EM3_EMPTY' : 12,
        'L1_EM7_EMPTY' : 13,
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
        'L1_2EM10VH' : 25,
        #'L1_2EM13VH' : 26,
        'L1_2EM12' : 409,
        'L1_2EM15' : 27,
        'L1_2EM15VH' : 28,
        'L1_EM7_2EM3' : 29,
        'L1_EM12_2EM3' : 30,
        'L1_EM15VH_3EM7' : 31,
	#'L1_2EM18VH':398,
	'L1_2EM20VH':400,
	'L1_EM15VH_3EM10VH':401,
	#'L1_EM18VH_3EM8VH':406,
	#'L1_EM18VH_2EM10VH_3EM7':408,
        'L1_EM20VH_3EM10VH':431, 
        'L1_EM20VH_2EM10VH_3EM8VH':432,
        'L1_2MU4' : 32,
        'L1_2MU6' : 33,
        'L1_2MU10' : 34,
        'L1_2MU20_OVERLAY' : 35,
        'L1_MU10_2MU6' : 36,
        'L1_MU11_2MU6' : 37,
        'L1_3MU4' : 38,
        'L1_MU6_2MU4' : 39,
        'L1_3MU6' : 40,
        'L1_4MU4' : 46,
        'L1_4J15.0ETA25' : 41,
        'L1_EM15I_MU4' : 42,
        'L1_2EM8VH_MU10' : 43,
        'L1_EM15VH_MU10' : 44,
        'L1_EM7_MU20' : 483,
        'L1_EM8VH_MU20' : 484,
        'L1_EM7_MU11' : 485,
        'L1_TAU12' : 45,
        'L1_TAU12IM' : 47,
        'L1_TAU20' : 49,
        'L1_TAU20IM' : 51,
        'L1_TAU30' : 53,
        'L1_TAU40' : 54,
        'L1_TAU60' : 55,
        'L1_TAU100' : 52,
        'L1_TAU8' : 56,
        'L1_TAU8_EMPTY' : 57,
        'L1_TAU20IM_2TAU12IM' : 58,
        'L1_TAU20_2TAU12' : 59,
        'L1_EM15VHI_2TAU12IM' : 60,
        'L1_EM15VHI_2TAU12IM_J25_3J12' : 61,
        'L1_EM15VHI_2TAU12IM_4J12' : 5,
        'L1_EM15VHI_TAU20IM_2TAU15_J25_2J20_3J15' : 378,
        'L1_EM20VHI_TAU20IM_2TAU20_J25_3J20' : 69,
        'L1_EM15VHI_TAU40_2TAU15' : 62,
        'L1_MU10_TAU12IM' : 63,
        'L1_MU10_TAU12IM_J25_2J12' : 64,
        'L1_MU10_TAU12IM_3J12' : 482,
        'L1_EM7_MU10':65,
        'L1_TAU30_EMPTY':341,
        'L1_MU10_TAU20IM' : 67,
        'L1_MU11_TAU20IM' : 430,
        'L1_MU10_TAU20IM_J25_2J20' : 377,

        'L1_2TAU20IM_3J20' : 486, 
        'L1_2TAU20IM_J25_3J20' : 479, 
        'L1_TAU25IM_2TAU12IM_J25_3J12' : 481,

        'L1_TAU20IM_2TAU12IM_J25_2J20_3J12' : 70,
        'L1_TAU25IM_2TAU20IM_2J25_3J20' : 398,
        'L1_TAU20IM_2TAU12IM_4J12' : 408,
        'L1_DR-MU10TAU12I_TAU12I-J25' : 72,
        'L1_MU10_TAU12I-J25' : 73,
        'L1_TAU20IM_2J20_XE45' : 74,
        'L1_TAU30_UNPAIRED_ISO' :343,
        'L1_TAU60_DR-TAU20ITAU12I' : 76,
        'L1_EM15VHI_2TAU12IM_XE35' : 78,
        'L1_MU10_TAU12IM_XE35' : 81,
#        'L1_MU10_TAU12IM_XE40' : 82,
        'L1_TAU20IM_2TAU12IM_XE35' : 83,
        'L1_TAU40_2TAU12IM_XE40' : 429,
#        'L1_EM15VH_JJ15.23ETA49' : 86,
        'L1_MU6_J20' : 88,
        'L1_MU6_J40' : 89,
        'L1_MU6_J75' : 90,
        'L1_MU20_J40'  : 428,
        'L1_MU20_XE30' : 433,
        'L1_J12' : 91,
        'L1_J15' : 92,
        'L1_J20' : 93,
        'L1_J25' : 94,
        'L1_J30' : 95,
        
        'L1_J40' : 96,
        'L1_J50' : 97,
        'L1_J75' : 98,
        'L1_J85' : 99,
        'L1_J100' : 100,
        'L1_J120' : 101,
        'L1_J400' : 102,
        'L1_J20.31ETA49' : 103,
        'L1_J30.31ETA49' : 104,
        'L1_J50.31ETA49' : 105,
        'L1_J75.31ETA49' : 106,
        'L1_J100.31ETA49' : 107,
#        'L1_J40.0ETA25' : 108,
        
        'L1_J15.31ETA49' : 109,
        'L1_J20.28ETA31' : 110,
        'L1_J12_EMPTY' : 111,
        'L1_J12_FIRSTEMPTY' : 112,
        'L1_J12_UNPAIRED_ISO' : 113,
        'L1_J12_UNPAIRED_NONISO' : 114,
        'L1_J12_UNPAIREDB1' : 443,
        'L1_J12_UNPAIREDB2' : 476,
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

        'L1_J20_J20.31ETA49' : 125,
        'L1_3J15' : 126,
        'L1_3J40' : 128,
        'L1_3J50' : 130,
        'L1_4J15' : 131,
        'L1_4J20' : 132,
        'L1_3J15.0ETA25_XE40' : 184,
        
#        'L1_3J75' : 133,
#        'L1_4J30' : 134,
        'L1_6J15' : 135,
        'L1_J75_3J20' : 136,
        'L1_J85_3J30' : 480,
        'L1_J30.0ETA49_2J20.0ETA49' : 137,

       
        'L1_5J15.0ETA25' : 140,
        'L1_2J15_XE55' : 141,
        'L1_J40_XE50' : 142,
        #'L1_J75_XE40' : 143,

        'L1_XE10': 68,
        #'L1_XE25': 82,
        'L1_XE30': 85,
        'L1_XE300': 187,
        
        'L1_XE35' : 144,
        'L1_XE40' : 145,
        'L1_XE45' : 146,
        'L1_XE50' : 147,
        'L1_XE55' : 148,
        'L1_XE60' : 149,
        'L1_XE70' : 150,
        'L1_XE80' : 151,
        'L1_EM12_XS20' : 154,
        'L1_EM15_XS30' : 155,

        'L1_TE5' : 157,
        'L1_TE10' : 158,
        'L1_TE20' : 159,
        'L1_TE30' : 138,
        'L1_TE40' : 82,
        'L1_TE50' : 143,
        'L1_TE15' : 160,
        'L1_TE25' : 152,


#        'L1_MBTS_1_VTE2' : 160,
        'L1_BCM_Wide_UNPAIRED_NONISO' : 161,
        'L1_BCM_AC_CA_UNPAIRED_ISO' : 162,
        'L1_BCM_AC_UNPAIRED_ISO' : 163,
        'L1_BCM_AC_UNPAIREDB1' : 447,
        'L1_MBTS_1_EMPTY' : 164,
        'L1_MBTS_1_UNPAIRED_ISO' : 165,
        'L1_MBTS_2_EMPTY' : 166,
        'L1_MBTS_2_UNPAIRED_ISO' : 167,
        'L1_MBTS_1_1_EMPTY' : 168,
        'L1_MBTS_1_1_UNPAIRED_ISO' : 169,

        'L1_DPHI-AJ20s2XE50' : 171,
        'L1_EM18VHI_3J20' : 172,
        #'L1_EM18VH_3J20' : 295,
        'L1_EM20VH_3J20' : 26,
        'L1_MU10_3J20' : 173,

        'L1_2J50_XE40' : 175,
        'L1_J40_XE60' : 176,
        'L1_J40.0ETA25_XE50' : 177,
        'L1_MU10_2J20' : 278,
	'L1_TAU40_2TAU20IM' : 254,
        'L1_TAU60_2TAU40' : 458,
        'L1_MU10_2J15_J20' : 255,
# freeing some CTPIDs  (used for the Xe pilot run)
         'L1_MBTSA0' : 134,
         'L1_MBTSA1' : 445,
         'L1_MBTSA2' : 153,
         'L1_MBTSA3' : 156,
         'L1_MBTSA4' : 178,
         'L1_MBTSA5' : 239,
         'L1_MBTSA6' : 244,
         'L1_MBTSA7' : 265,
         'L1_MBTSA8' : 274,
         'L1_MBTSA10' : 284,
         'L1_MBTSA12' : 287,
         'L1_MBTSA14' : 289,
         'L1_MBTSC0' : 296,
         'L1_MBTSC1' : 297,
         'L1_MBTSC2' : 298,
         'L1_MBTSC3' : 306,
         'L1_MBTSC4' : 307,
         'L1_MBTSC5' : 310,
         'L1_MBTSC6' : 311,
         'L1_MBTSC7' : 331,
         'L1_MBTSC8' : 359,
         'L1_MBTSC10' : 360,
         'L1_MBTSC12' : 361,
         'L1_MBTSC14' : 435,

        
        'L1_BPH-8M15-MU6MU4' : 179,
        'L1_BPH-8M15-2MU6' : 180,
        'L1_J15.0ETA25_2J15.31ETA49': 305 , 
        'L1_J25.0ETA23_2J15.31ETA49': 335 , 
        'L1_J40.0ETA25_2J15.31ETA49' : 181,
        'L1_J40.0ETA25_2J25_J20.31ETA49' : 182,

        "L1_J40.0ETA25_2J30_J20.31ETA49": 465, 

        "L1_HT150-J20s5.ETA31_MJJ-400": 272 , ## noid(ea)
        "L1_HT150-J20s5.ETA31_MJJ-400-CF": 273,
        
        ## the ctp ids are a mess.. no 7 consequetive ids are free.. but these here are:
#        'L1_AFP_C_MBTS_A':183,
#        'L1_AFP_C_ZDC_C':189,
#        'L1_AFP_C_J12':190,
#        'L1_AFP_C_EM3':191,
#        'L1_AFP_C_TE5':192,
#        'L1_AFP_C_ALFA_C':193,
#        'L1_AFP_C_ALFA_A':170,
#        'L1_AFP_C_ANY_MBTS_A':174,
#        'L1_AFP_C_MU4':379,   
 
        'L1_AFP_A_BGRP0' : 170,
        'L1_AFP_A_SPECTOF_BGRP0': 174,
        'L1_AFP_C_BGRP0': 183,
        'L1_AFP_C_SPECTOF_BGRP0': 189, 
        'L1_AFP_NSA_BGRP0' : 459,
        'L1_AFP_NSC_BGRP0': 460,
        'L1_AFP_FSA_SIT_BGRP0' : 461,
        'L1_AFP_FSC_SIT_BGRP0' : 462,
        'L1_AFP_A_OR_C_UNPAIRED_ISO': 190, 
        'L1_AFP_A_OR_C_UNPAIRED_NONISO': 191, 
        'L1_AFP_A_OR_C_EMPTY': 192, 
        'L1_AFP_A_OR_C_FIRSTEMPTY': 193,
        'L1_AFP_A_AND_C_SPECTOF': 456,
        'L1_AFP_A_AND_C': 448,
        'L1_AFP_A_OR_C': 276, 
        'L1_AFP_A': 283, 
        'L1_AFP_C': 308,
        'L1_AFP_A_AND_C_MBTS_2': 309,
        'L1_AFP_A_OR_C_MBTS_2': 317,
        'L1_AFP_A_AND_C_J12': 379,
        'L1_AFP_A_OR_C_J12': 436,
        'L1_EM3_AFP_A_AND_C': 437,
        'L1_EM3_AFP_A_OR_C': 438,
        'L1_MU4_AFP_A_AND_C': 439,
        'L1_MU4_AFP_A_OR_C': 440,
        'L1_AFP_A_AND_C_TE5': 441,
        'L1_AFP_A_OR_C_TE5': 442,
        'L1_AFP_A_AND_C_SPECTOF_J50': 444,
        'L1_AFP_A_AND_C_SPECTOF_J75': 446,
        'L1_AFP_A_AND_C_SPECTOF_J100': 457,
       
#        'L1_EM8I' : 183,
#        'L1_EM15I': 184,
        'L1_MBTS_4_A_UNPAIRED_ISO' : 185,
        'L1_MBTS_4_C_UNPAIRED_ISO' : 186,        
        'L1_MBTS_4_A' : 194,
        'L1_MBTS_4_C' : 195,
        'L1_MBTS_1_BGRP9' : 196,
        'L1_MBTS_2_BGRP9' : 197,
        'L1_MBTS_1_BGRP11' : 198,
        'L1_MBTS_2_BGRP11' : 199,
        'L1_RD0_FILLED' : 200,
        'L1_RD0_UNPAIRED_ISO' : 201,
        'L1_RD0_EMPTY' : 202,
        'L1_RD1_FILLED' : 203,
        'L1_RD1_EMPTY' : 204,
        'L1_RD1_BGRP10' : 188,
        'L1_RD2_FILLED' : 205,
        'L1_RD2_EMPTY' : 206,
        'L1_RD3_FILLED' : 207,
        'L1_RD3_EMPTY' : 208,
        'L1_RD0_FIRSTEMPTY' : 209,
        'L1_RD0_BGRP9' : 210,
        'L1_RD0_BGRP10' : 473,
        'L1_RD0_BGRP11' : 211,
        'L1_LUCID' : 212,
        'L1_LUCID_EMPTY' : 213,
        'L1_LUCID_UNPAIRED_ISO' : 214,
        'L1_LUCID_A_C_EMPTY' : 215,
        'L1_LUCID_A_C_UNPAIRED_ISO' : 216,
        'L1_LUCID_A_C_UNPAIRED_NONISO' : 217,
        'L1_TRT_FILLED' : 218,
        'L1_TRT_EMPTY' : 219,
        'L1_TGC_BURST' : 220,
#        'L1_TGC_BURST_EMPTY' : 184,
        'L1_LHCF' : 221,
        'L1_BCM_Wide_BGRP0' : 222,
        'L1_BCM_AC_CA_BGRP0' : 223,
        'L1_BCM_Wide_EMPTY' : 224,
        'L1_BCM_Wide_UNPAIRED_ISO' : 225,
        'L1_MBTS_1' : 226,
        'L1_MBTS_2' : 227,
        'L1_MBTS_1_1' : 228,
        
      
        'L1_BCM_CA_UNPAIRED_ISO' : 229,
        'L1_BCM_CA_UNPAIREDB2' : 475,
        'L1_BCM_AC_UNPAIRED_NONISO' : 230,
        'L1_BCM_CA_UNPAIRED_NONISO' : 231,
        'L1_BCM_AC_ABORTGAPNOTCALIB' : 232,
        'L1_BCM_CA_ABORTGAPNOTCALIB' : 233,
        'L1_BCM_Wide_ABORTGAPNOTCALIB' : 234,
        'L1_BCM_AC_CALIB' : 235,
        'L1_BCM_CA_CALIB' : 236,
        'L1_BCM_Wide_CALIB' : 237,
        'L1_BTAG-MU4J15' : 238,
        'L1_ZB' : 240,
        'L1_BPTX0_BGRP0' : 241,
        'L1_BPTX1_BGRP0' : 242,
        'L1_BTAG-MU6J20' : 243,
#        'L1_BTAG-MU4J20' : 245,
        'L1_3J15_BTAG-MU4J15' : 246,
        'L1_3J15_BTAG-MU4J30' : 247,
        'L1_3J15_BTAG-MU6J25' : 248,
        'L1_3J20_BTAG-MU4J20' : 249,
#        'L1_J40_DPHI-Js2XE50' : 250,
        'L1_J40_DPHI-J20s2XE50' : 251,
        #'L1_J40_DPHI-J20s2XE30' : 257,
        'L1_J40_XE50_DPHI-J20s2XE50': 245,
        'L1_J40_DPHI-J20XE50' : 252,
        'L1_J40_DPHI-CJ20XE50' : 253,
        'L1_2MU4_J20_XE30_DPHI-J20s2XE30' : 271,

        'L1_2MU4_J40_XE50' : 449,
        'L1_2MU4_J20_XE40_DPHI-J20s2XE30' : 450,
        'L1_2MU4_J20_XE50_DPHI-J20s2XE30' : 451,
        'L1_MU4_J30_XE40_DPHI-J20s2XE30' : 452,
        'L1_MU4_J50_XE50_DPHI-J20s2XE30' : 453,
        'L1_MU4_XE60' : 454,

#SX
#        'L1_BPH-DR-2MU4-B_BPH-2M-2MU4-B' : 254,
#        'L1_BPH-DR-2MU4-B_BPH-4M8-2MU4-B' : 255,
#        'L1_BPH-DR-2MU4-BO_BPH-2M-2MU4-BO' : 256,
#        'L1_BPH-DR-2MU4-BO_BPH-4M8-2MU4-BO' : 257,

        
        'L1_HT190-J15.ETA21' : 258,
        'L1_HT190-J15s5.ETA21' : 259,
        'L1_HT150-J20.ETA31' : 260,
        'L1_HT150-J20s5.ETA31' : 261,
        'L1_JPSI-1M5' : 262,
        'L1_JPSI-1M5-EM7' : 263,
        'L1_JPSI-1M5-EM12' : 264,
        'L1_KF-XE50' : 266,
        'L1_KF-XE55' : 267,
        'L1_KF-XE60' : 268,
        'L1_KF-XE65' : 269,
        'L1_KF-XE75' : 270,
#        'L1_W-05DPHI-JXE-0' : 271,
#        'L1_W-10DPHI-JXE-0' : 272,
#        'L1_W-15DPHI-JXE-0' : 273,
        #'L1_W-15DPHI-EMXE-1' : 271,
        #'L1_W-15DPHI-EM15XE-1' : 272,
#        'L1_W-05DPHI-EMXE-1' : 276,
        'L1_W-05RO-XEHT-0' : 277,
        'L1_W-90RO2-XEHT-0' : 279,
        'L1_W-250RO2-XEHT-0' : 280,
        'L1_W-HT20-JJ15.ETA49' : 281,
        'L1_W-NOMATCH' : 282,
        #'L1_W-NOMATCH_W-05RO-XEEMHT' : 283,

        'L1_EM12_W-MT25' : 50,
        'L1_EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE' : 139,
        'L1_EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE' : 250,

        'L1_EM15_W-MT35' : 286,
        'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0' : 293,
        'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20' : 294,

        'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE' : 322,
        'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE_XS30' : 344,
        'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE' : 384,
#SX
#        'L1_BPH-DR-2MU4' : 295,
#        'L1_BPH-2M-2MU4' : 296,
#        'L1_BPH-4M8-2MU4' : 297,
#        'L1_BPH-DR-2MU4_BPH-2M-2MU4' : 298,
#        'L1_BPH-DR-2MU4_BPH-4M8-2MU4' : 299,
#        'L1_BPH-DR-2MU6' : 300,
#        'L1_BPH-2M-2MU6' : 301,
#        'L1_BPH-4M8-2MU6' : 302,
#        'L1_BPH-DR-2MU6_BPH-2M-2MU6' : 303,
#        'L1_BPH-DR-2MU6_BPH-4M8-2MU6' : 304,
#        'L1_BPH-2M-2MU4-B' : 305,
#        'L1_BPH-2M-2MU4-BO' : 306,
#        'L1_BPH-2M-MU6MU4' : 307,
#        'L1_BPH-2M-MU6MU4-B' : 308,
#        'L1_BPH-2M-MU6MU4-BO' : 309,
#        'L1_BPH-4M8-2MU4-B' : 310,
#        'L1_BPH-4M8-2MU4-BO' : 311,
#        'L1_BPH-4M8-MU6MU4' : 312,
#        'L1_BPH-4M8-MU6MU4-B' : 313,
#        'L1_BPH-4M8-MU6MU4-BO' : 314,

#SX adding
        #'L1_BPH-1M19-2MU4_BPH-0DR34-2MU4' : 295,
        'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4'  : 299,
        'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6' : 300,
        'L1_BPH-8M15-2MU6_BPH-0DR22-2MU6'  : 301,
        'L1_MU6MU4-BO' : 302,
        'L1_2MU4-B' : 303,
        'L1_2MU6-B'  : 304,
        #'L1_BPH-1M19-2MU4-B_BPH-0DR34-2MU4'  : 305,
        #'L1_BPH-1M19-2MU4-BO_BPH-0DR34-2MU4'   : 308,
        #'L1_BPH-2M8-2MU4-BO_BPH-0DR15-2MU4'  : 309,
        'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4-B' : 487,
        'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4-BO' : 488,

        'L1_2MU4-BO' : 312,
        'L1_2MU6-BO' : 313,
        'L1_MU6_2MU4-B'  : 314,

        ### ATR-14350 - these should be replaced in 2017 with items 296 and 298
        'L1_BPH-2M9-2MU4_BPH-0DR15-2MU4' : 426,
        'L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4' : 427,


        'L1_DY-DR-2MU4' : 315,
        #'L1_DY-BOX-MU6MU4' : 317,
        'L1_DY-BOX-2MU6' : 318,
        'L1_LFV-MU' : 319,
        'L1_LFV-MU6' : 332,
        'L1_LFV-EM8I' : 320,
        'L1_LFV-EM15I' : 321,
        'L1_DPHI-J20s2XE50' : 323,
        'L1_DPHI-J20XE50' : 324,
        'L1_DPHI-CJ20XE50' : 325,
        'L1_MJJ-800' : 327,
        'L1_MJJ-400' : 329,
        #'L1_MJJ-350' : 330,
#        'L1_MJJ-300' : 331,
#        'L1_MJJ-200' : 332,
        'L1_MJJ-100' : 333,
        'L1_HT150-JJ15.ETA49' : 334,
        'L1_J4-MATCH' : 336,
        'L1_LLP-RO' : 338,
        'L1_LLP-NOMATCH' : 339,
        'L1_DR-MU10TAU12I' : 340,
#        'L1_TAU12I-J25' : 341,
        'L1_EM15TAU12I-J25' : 345,
        'L1_DR-EM15TAU12I-J25' : 346,
        'L1_TAU20ITAU12I-J25' : 347,
        'L1_DR-TAU20ITAU12I' : 348,
        'L1_DR-TAU20ITAU12I-J25' : 350,

        'L1_DR25-TAU20ITAU12I' : 337,

        'L1_DR25-TAU20ITAU12I-J25' : 386,
        'L1_30M-EM20ITAU12' : 387,
        'L1_MJJ-400-CF': 397,

        'L1_LAR-EM' : 351,
        'L1_LAR-J' : 352,
        'L1_DR-EM15TAU12I' : 353,        
        'L1_LATE-MU10_XE50': 354,
        'L1_LATE-MU10_XE40': 466,
        'L1_LATE-MU10_J50' : 355,
        'L1_SC111-CJ15' : 356,
        'L1_SC85-CJ15' : 357,
#SX
#        'L1_BPH-DR-2MU4-B' : 353,
#        'L1_BPH-DR-2MU4-BO' : 354,
#        'L1_BPH-DR-MU6MU4' : 355,
#        'L1_BPH-DR-MU6MU4-B' : 356,
#        'L1_BPH-DR-MU6MU4-BO' : 357,
        'L1_MU6_MJJ-200' : 358,
        'L1_3J20_4J20.0ETA49_MJJ-400' : 366,
        'L1_3J20_4J20.0ETA49_MJJ-700' : 367,
        'L1_3J20_4J20.0ETA49_MJJ-800' : 368,
        'L1_3J20_4J20.0ETA49_MJJ-900' : 369,
        'L1_EM7_FIRSTEMPTY': 371, 
        'L1_RD0_ABORTGAPNOTCALIB':372,
        'L1_3J25.0ETA23' : 373,
        'L1_3J35.0ETA23' : 425,
#        'L1_TE4' : 374,
##         'L1_VTE3' : 375,
##         'L1_VTE4' : 48,
##         'L1_VTE5' : 376,
##         'L1_VTE10' : 409,
        #'L1_XS40' : 377,
        #'L1_XS50' : 378,
        #'L1_XS60' : 379,
        'L1_J12_BGRP12' : 380,
        'L1_J30.31ETA49_BGRP12' : 381,
        'L1_MU6_J30.0ETA49_2J20.0ETA49' : 382,
        'L1_4J20.0ETA49' : 383,
        'L1_HT150-JJ15.ETA49_MJJ-400' : 416,

        ### ATR-15062
        'L1_EM18VHI_MJJ-300' : 385,
        
#ATR-13743        'L1_MU4_J30' : 386,
#        'L1_MU4_J50' : 387,

        
        'L1_TAU8_UNPAIRED_ISO': 388,
        'L1_EM7_UNPAIRED_ISO': 389,
        'L1_TAU8_UNPAIRED_NONISO': 467,
        'L1_EM7_UNPAIRED_NONISO': 468,
        'L1_MU4_UNPAIRED_NONISO' : 469,
        'L1_RD2_BGRP12'   : 390,
        'L1_TAU8_FIRSTEMPTY': 391,
        'L1_EM24VHI'           : 392,
        'L1_LHCF_UNPAIRED_ISO' : 393,
        'L1_LHCF_EMPTY' : 394,
        'L1_EM8I_MU10'  : 399,
        "L1_EM24VHIM"    :402,
        "L1_EM15VHI"    :423,
        "L1_2EM15VHI"    :342,
        "L1_EM15I"    :424,
        "L1_MU6_3MU4"  :403,
        "L1_2MU6_3MU4" :404,
        "L1_BGRP9"     :405,
        #'L1_TE50'       :406,
        #'L1_TE2'       :407,
        #'L1_TE60'      :408,
        #'L1_TE50.0ETA24':409,
#        'L1_VTE2':410, 
        'L1_EM20VH_FIRSTEMPTY':411, 
        'L1_EM22VHI_FIRSTEMPTY':412, 
        'L1_MU20_FIRSTEMPTY':413, 
        'L1_MU21_FIRSTEMPTY':406, 
        'L1_J100_FIRSTEMPTY':414, 
        'L1_J100.31ETA49_FIRSTEMPTY':415, 
        #'L1_TE60.0ETA24':416, 
#        'L1_MBTS_1_VTE4':417, 
#        'L1_MBTS_1_VTE3':418,
        
##         'L1_MBTS_1_VTE5':77, 
##         'L1_MBTS_1_VTE10':79,
##         'L1_MBTS_2_VTE2':80, 
##         'L1_MBTS_2_VTE3':108,
##         'L1_MBTS_2_VTE4':124, 
##         'L1_MBTS_2_VTE5':127,
##         'L1_MBTS_2_VTE10':129,  

        'L1_2EM3_VTE50' : 474,
        
#        'L1_MBTS_1_VTE70' : 477,

        # 3 x ZDC
        'L1_ZDC_A':419,
        'L1_ZDC_C':420,
        'L1_ZDC_AND':421, 
        'L1_ZDC_A_C':422, 
        'L1_MU4_TE10':470,
        'L1_MU4_TE20':410,
        'L1_MU4_TE30':124,
        'L1_MU4_TE40':108,
        #'L1_MU4_TE50':124,

        'L1_LUCID_BGRP9' : 471,
        'L1_LUCID_BGRP11' : 472,
        
        #Overlay
#        'L1_MBTS_1_OVERLAY' : 445,
#        'L1_TE20_OVERLAY' : 455,
        
#ATR-13743        
        'L1_ALFA_ELAST1'  : 48,
        'L1_ALFA_ELAST2'  : 71,
        'L1_ALFA_ELAST11' : 77,
        'L1_ALFA_ELAST12' : 79,
        'L1_ALFA_ELAST13' : 80,
        'L1_ALFA_ELAST14' : 84,
        'L1_ALFA_ELAST15' : 87,
        'L1_ALFA_ELAST15_Calib' : 127,
        'L1_ALFA_ELAST16' : 129,
        'L1_ALFA_ELAST17' : 133,
        'L1_ALFA_ELAST18' : 257,
        'L1_ALFA_ELAST18_Calib' : 275,
#        'L1_ALFA_SDIFF5' : 436,
#        'L1_ALFA_SDIFF6' : 437,
#        'L1_ALFA_SDIFF7' : 438,
#        'L1_ALFA_SDIFF8' : 439,
#        'L1_MBTS_1_A_ALFA_C' : 440,
#        'L1_MBTS_1_C_ALFA_A' : 441,
#        'L1_MBTS_1_A_ALFA_C_UNPAIRED_ISO' : 442,
#        'L1_MBTS_1_C_ALFA_A_UNPAIRED_ISO' : 443,
#        'L1_MBTS_2_A_ALFA_C' : 444,
#        'L1_MBTS_2_C_ALFA_A' : 445,
#        'L1_MBTS_2_A_ALFA_C_UNPAIRED_ISO' : 446,
#        'L1_MBTS_2_C_ALFA_A_UNPAIRED_ISO' : 447,
#        'L1_LUCID_A_ALFA_C' : 448,
#        'L1_LUCID_C_ALFA_A' : 449,
#        'L1_LUCID_A_ALFA_C_UNPAIRED_ISO' : 450,
#        'L1_LUCID_C_ALFA_A_UNPAIRED_ISO' : 451,
#        'L1_EM3_ALFA_ANY' : 452,
#        'L1_EM3_ALFA_ANY_UNPAIRED_ISO' : 453,
#        'L1_EM3_ALFA_EINE' :   454,
#        'L1_ALFA_ELASTIC_UNPAIRED_ISO' : 455,
#        'L1_ALFA_ANTI_ELASTIC_UNPAIRED_ISO' : 456,
#        'L1_ALFA_ANY_A_EMPTY' : 457,
#        'L1_ALFA_ANY_C_EMPTY' : 458,
#        'L1_J12_ALFA_ANY' : 459,
#        'L1_J12_ALFA_ANY_UNPAIRED_ISO' : 460,
#        'L1_TE5_ALFA_ANY' : 461,
#        'L1_TE5_ALFA_ANY_UNPAIRED_ISO' : 462,
#        'L1_TE5_ALFA_EINE' : 463,
#        'L1_TRT_ALFA_ANY' : 464,
#        'L1_TRT_ALFA_ANY_UNPAIRED_ISO' : 465,
#        'L1_TRT_ALFA_EINE' :   466,
##        'L1_LHCF_ALFA_ANY_A' : 467,
##        'L1_LHCF_ALFA_ANY_C' : 468,
##        'L1_LHCF_ALFA_ANY_A_UNPAIRED_ISO' : 469,
##        'L1_LHCF_ALFA_ANY_C_UNPAIRED_ISO' : 470,
#        
#        'L1_ALFA_BGT' : 471,
#        'L1_ALFA_BGT_UNPAIRED_ISO' : 472,
        'L1_ALFA_BGT_BGRP1' : 395,
        'L1_ALFA_BGT_BGRP4' : 396,
        'L1_ALFA_BGT_BGRP10' : 497,
        'L1_ALFA_SHOWSYST5' : 285,
        'L1_ALFA_SYST9'  : 288,
        'L1_ALFA_SYST10' : 290,
        'L1_ALFA_SYST11' : 291,
        'L1_ALFA_SYST12' : 292,
        'L1_ALFA_SYST17' : 316,
        'L1_ALFA_SYST18' : 326,
        'L1_ALFA_ANY' : 328,
        'L1_ALFA_ANY_EMPTY' : 330,
        'L1_ALFA_B1_EMPTY' : 376,
        'L1_ALFA_B2_EMPTY' : 407,
        'L1_ALFA_ANY_FIRSTEMPTY' : 349,
        'L1_ALFA_ANY_UNPAIRED_ISO' : 362,
        'L1_ALFA_ANY_UNPAIRED_NONISO' : 363,
        'L1_ALFA_ANY_BGRP10' : 370,
        'L1_ALFA_ANY_ABORTGAPNOTCALIB' : 374,
        'L1_ALFA_ANY_CALIB' : 375,
        'L1_ALFA_B7L1U' : 489,
        'L1_ALFA_B7L1L' : 490,
        'L1_ALFA_A7L1U' : 491,
        'L1_ALFA_A7L1L' : 492,
        'L1_ALFA_A7R1U' : 493,
        'L1_ALFA_A7R1L' : 494,
        'L1_ALFA_B7R1U' : 495,
        'L1_ALFA_B7R1L' : 496,

        
        'L1_MU6_FTK'                           : 499,
        'L1_MU20_FTK'                          : 500,
#        'L1_MU21_FTK'                          : 497,
        'L1_TAU12IM_FTK'                       : 501,
        'L1_TAU20IM_FTK'                       : 86,
        'L1_TAU20IM_2TAU12IM_J25_2J20_3J12_FTK': 502,
        'L1_DR-TAU20ITAU12I-J25_FTK'           : 503,
        'L1_4J20_FTK'                          : 504,
        'L1_4J15.0ETA25_FTK'                   : 505,
        'L1_J15_FTK'                           : 506,
        'L1_J40_FTK'                           : 507,
        'L1_J100_FTK'                          : 508,           
        'L1_MU20_TAU12IM_FTK'                  : 498,
#        'L1_ALFA_B7L1U_OD' : ,
#        'L1_ALFA_B7L1L_OD' : ,
#        'L1_ALFA_A7L1U_OD' : ,
#        'L1_ALFA_A7L1L_OD' : ,
#        'L1_ALFA_A7R1U_OD' : 409,
#        'L1_ALFA_A7R1L_OD' : 410,
#        'L1_ALFA_B7R1U_OD' : 417,
#        'L1_ALFA_B7R1L_OD' : 418,
#        'L1_ALFA_B7L1_OD'  : 434,
#        'L1_ALFA_A7L1_OD'  : 455,
#        'L1_ALFA_B7R1_OD'  : 477,
#        'L1_ALFA_A7R1_OD'  : 478,
        'L1_CALREQ2' : 511,  # never use 509-511 for anything else than CALREQ triggers
}


    Lvl1Flags.prescales = {}

if __name__ == "__main__": print_available()
