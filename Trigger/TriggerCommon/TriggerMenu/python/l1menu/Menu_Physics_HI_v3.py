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

    Lvl1Flags.BunchGroupPartitioning = [1, 13, 14] # partition 1: 1-12, partition 2: 13, partition 3: 14-15 (note that BGRP0 is used by all items)
    Lvl1Flags.BunchGroupNames = ['BCRVeto', 'Filled', 'Calib', 'Empty', 'UnpairedBeam1', 'UnpairedBeam2', 'FirstEmpty', 'InTrain']
    Lvl1Flags.BunchGroupNames += ['NotUsed'] * len(Lvl1Flags.BunchGroupNames())

    Lvl1Flags.MenuPartitioning = [0, 472, 492] # partition 1: ctpid 0-471, partition 2: ctpid 472-491, partition 3: ctpid 492-511

    Lvl1Flags.RemapThresholdsAsListed = True

    Lvl1Flags.thresholds = [
        # Note that the forced thresholds (FTHR) are not used in the menu but are needed for input monitoring
        # They can always be replaced with something that is needed for the menu


        #-------------------------
        # SLOT 7 / CON 0,1 (EM1,2)
        #-------------------------
        
        # 16 x EM
        'EM7', 'EM8', 'EM8I', 'EM10', 'EM10VH', 'EM12', 'EM13VH', 'EM14', 'EM15', 'EM15HI', 'EM16', 'EM18', 'EM20', 'EM20VH', 'EM20VHI', 'EM22',
        
        # 1 x ZB
        'ZB_J75', # Will double check if 'ZB_EM15' should be used for Run-2.

        #--------------------------
        # SLOT 7 / CON 2,3 (TAU1,2)
        #--------------------------

        # 16 x TAU
        'HA12', 'HA12IL', 'HA12IM', 'HA12IT', 'HA15', 'HA20', 'HA20IL',  'HA20IM', 'HA20IT', 'HA25', 'HA25IT', 'HA30', 'HA35', 'HA40', 'HA50', 'HA60', 

        #----------------------
        # SLOT 8 / CON 0 (JET1)
        #----------------------
        # 10 x 3-bit JET (can have multiplicity 4 or more)

        # 8 x JETs and central jets
        'J10', 'J12', 'J15', 'J20', 'J25', 'J25.0ETA23', 'J30', 'J35',
        # 2 x VBF
        'J20.0ETA49', 'J30.0ETA49',
        

        #----------------------
        # SLOT 8 / CON 1 (JET2)
        #----------------------
        # 15 x 2-bit JET (can have maximum multiplicity of 3) (SLOT 8, CON 1)

        # 3 x Central Jet
        'JJ15.23ETA49','J20.28ETA31','J40.0ETA25', 

        # 6 Jets
        'J40', 'J50', 'J75', 'J85', 'J100', 'J175',

        # 6 x FJ              
        'J10.31ETA49', 'J15.31ETA49', 'J20.31ETA49', 'J30.31ETA49', 'J75.31ETA49', 'J100.31ETA49',

        #---------------------
        # SLOT 8 / CON 2 (EN1)
        #---------------------
        # 24 x 1-bit thresholds
        
        # 8 x TE
        'TE5', 'TE20', 'TE50', 'TE100', 'TE200', 'TE10000', 'TE12000', 'TE14000',
        
        # 8 x XE
        'XE35', 'XE40', 'XE45', 'XE50', 'XE55', 'XE60', 'XE70', 'XE80',
        
        'XE35.0ETA24', 'XE40.0ETA24', 'XE45.0ETA24', 'XE50.0ETA24', 'XE55.0ETA24', 'XE60.0ETA24', 'XE70.0ETA24', 'XE80.0ETA24',

        # 8 x XS
        'XS20', 'XS30', 'XS40', 'XS45', 'XS50', 'XS55', 'XS60', 'XS65', 


        #---------------------
        # SLOT 8 / CON 3 (EN2)
        #---------------------
        # 16 x 1-bit thresholds

        # 8 x weighted sum ET
        #'RXE35', 'RXE40', 'RXE45', 'RXE50', 'RXE55', 'RXE60', 'RXE70', 'RXE80', # FTHR

        # 8 x restricted eta range in |eta|<4.9
        'TE500.0ETA49', 'TE1500.0ETA49', 'TE3000.0ETA49', 'TE3500.0ETA49',
        'TE5000.0ETA49', 'TE6500.0ETA49', 'TE8000.0ETA49', 'TE9000.0ETA49',    
        
        #------------------------       
        # SLOT 9 / CON 0 (MUCTPi)
        #------------------------
        
        # 6 x MU
        'MU0', 'MU4', 'MU6', 'MU11', 'MU15', 'MU20', 
        # MU10 is needed for monitoring 
        
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


        
	

        ]

    Lvl1Flags.items = [
                       'L1_EM7',
		       'L1_EM8',
                       'L1_EM10',
                       'L1_EM12',
                       'L1_EM14',
                       'L1_EM16',
                       'L1_EM18',
		       'L1_EM20',
	               'L1_EM22',
                       'L1_2EM10',
                       #'L1_2EM5',
		       'L1_2EM7',
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
		       'L1_J35',
                       'L1_J50',
                       'L1_J75',
                       'L1_J175',
                       #jet energy
                       'L1_2J15',
                       #'L1_JE200',
                       #'L1_JE300',
                       #'L1_JE500',
                       # forward jets
                       'L1_J10.31ETA49',
                       #'L1_FJ30',
                       #'L1_FJ55' ,
                       #'L1_FJ95',
                       #MinBias
                       'L1_LUCID_A',
                       'L1_LUCID_C',
                       'L1_LUCID_A_C',
                       'L1_LUCID',
                       #'L1_LUCID_COMM',
                       'L1_MBTS_2',
                       'L1_MBTS_2_BGRP9',
                       'L1_MBTS_2_BGRP11',
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
                       'L1_MBTS_1_1_BGRP11',
                       'L1_MBTS_1_1_VTE50',
                       'L1_MBTS_2_2_VTE50',
		       #TRT
		       'L1_TRT_FILLED',
		       'L1_TRT_EMPTY',
                       #randoms
                       'L1_RD0_BGRP9',
                       'L1_RD0_FILLED',
                       'L1_RD0_FIRSTEMPTY',
                       'L1_RD0_UNPAIRED_ISO',
                       'L1_RD0_EMPTY',
                       'L1_RD1_FILLED',
                       'L1_RD1_EMPTY',
                       #MET
                       #total energy
                       'L1_TE5',
                       'L1_TE20',
                       'L1_TE50',
                       'L1_TE100',
                       'L1_TE200', 
                       'L1_TE10000',
                       'L1_TE12000',
		       'L1_TE14000',
                       'L1_TE5_NZ',
		       'L1_TE5_VTE200',
		       # restricted TE
		       'L1_TE500.0ETA49',
                       'L1_TE1500.0ETA49',
                       'L1_TE3000.0ETA49',
                       'L1_TE3500.0ETA49',
                       'L1_TE5000.0ETA49',
                       'L1_TE6500.0ETA49',
                       'L1_TE8000.0ETA49',
                       'L1_TE9000.0ETA49',
		       # restricted TE for overlay
                       'L1_TE500.0ETA49_OVERLAY',
                       'L1_TE1500.0ETA49_OVERLAY',
                       'L1_TE3000.0ETA49_OVERLAY',
                       'L1_TE3500.0ETA49_OVERLAY',
                       'L1_TE5000.0ETA49_OVERLAY',
                       'L1_TE6500.0ETA49_OVERLAY',
                       'L1_TE8000.0ETA49_OVERLAY',
                       'L1_TE9000.0ETA49_OVERLAY',
                       #Min Bias
                       'L1_ZDC',
                       'L1_ZDC_A',
                       'L1_ZDC_C',
                       'L1_ZDC_AND',
                       'L1_ZDC_AND_VTE50',
                       'L1_ZDC_A_C',
                       'L1_ZDC_A_C_BGRP11',
                       'L1_ZDC_A_C_OVERLAY',
		       'L1_ZDC_A_C_VTE50_OVERLAY',
		       'L1_TE50_OVERLAY',
                       'L1_ZDC_A_C_VTE50',
                       'L1_ZDC_A_C_TE50',
                       'L1_BCM_Wide',
                       'L1_BCM_HT_BGRP0','L1_BCM_Wide_BGRP0','L1_BCM_AC_CA_BGRP0',
                       'L1_ZDC_MBTS_1',
                       'L1_ZDC_MBTS_2',
                       'L1_ZDC_MBTS_1_1',
                       'L1_ZDC_MBTS_2_2',
		       'L1_ZDC_VTE200',
		       #ZDC one side
		       'L1_ZDC_A_VZDC_C_VTE200',
		       'L1_ZDC_C_VZDC_A_VTE200',
	               'L1_ZDC_XOR',
		       'L1_ZDC_XOR_VTE200',
	               'L1_ZDC_XOR_TE5_VTE200',
                       'L1_ZDC_XOR_TRT_VTE200',
		       #coincidence
		       'L1_ZDC_A_C_VTE200',
	               'L1_ZDC_A_C_TE5_VTE200',
                       #NIMDIR stuff
                       # temporary commented out in HI_v3: 'L1_NIM_S8C2B21', 'L1_NIM_S8C2B22', 'L1_NIM_S8C2B23',  
                       # votoed by TE
                       #UPC triggers
                       'L1_MU0_NZ',
                       'L1_J15_NZ',
                       'L1_2MU0_NZ',
                       #'L1_2EM3_NZ',
                       'L1_2J15_NZ',
                       'L1_MU0_NL',
                       #'L1_EM3_NL',
                       'L1_J15_NL',
                       'L1_2MU0_NL',
                       #'L1_2EM3_NL',
                       'L1_2J15_NL',
                       'L1_MU0_MV',
                       'L1_2MU0_MV',
                       'L1_MU0_MV_VTE50',
                       'L1_MU0_VTE50',
                       'L1_MU0_TE50',
                       'L1_MU4_MV_VTE50',
                       'L1_MU4_VTE50',
                       'L1_MU4_TE50',
                       #'L1_EM3_MV_VTE50',
                       #'L1_EM3_VTE50',
                       'L1_J10_VTE100',
                       'L1_J10_VTE200',
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

                       'L1_EM7_UNPAIRED_ISO','L1_EM7_UNPAIRED_NONISO','L1_EM7_EMPTY','L1_EM7_FIRSTEMPTY',
                       #MU UNPAIRED-EMPTY-ETC
                       'L1_MU0_UNPAIRED_ISO','L1_MU0_UNPAIRED_NONISO','L1_MU0_EMPTY','L1_MU0_FIRSTEMPTY',
                       'L1_MU4_UNPAIRED_ISO', 'L1_MU4_UNPAIRED_NONISO', 'L1_MU4_EMPTY',
                       'L1_MU4_FIRSTEMPTY',
                       'L1_MU6_FIRSTEMPTY','L1_MU11_EMPTY',
                       'L1_2MU0_EMPTY', 'L1_2MU4_EMPTY',
                       'L1_2MU6_UNPAIRED_ISO','L1_2MU6_UNPAIRED_NONISO','L1_2MU6_EMPTY','L1_2MU6_FIRSTEMPTY',
                       #TAU UNPAIRED-EMPTY-ETC
                       'L1_TAU12_UNPAIRED_ISO','L1_TAU12_UNPAIRED_NONISO','L1_TAU12_EMPTY','L1_TAU12_FIRSTEMPTY',
                       #J  UNPAIRED-EMPTY-ETC
                       'L1_J10_UNPAIRED_ISO','L1_J10_UNPAIRED_NONISO','L1_J10_EMPTY','L1_J10_FIRSTEMPTY',
                       'L1_J30_EMPTY', 'L1_J30_UNPAIRED', 'L1_J30_FIRSTEMPTY',
                       #FJ  UNPAIRED-EMPTY-ETC
                       #'L1_FJ10_UNPAIRED_ISO', 'L1_FJ10_FIRSTEMPTY',
                       'L1_J10.31ETA49_EMPTY',
                       #ZDC
                       'L1_ZDC_EMPTY',
                       'L1_ZDC_UNPAIRED_ISO',
                       'L1_ZDC_UNPAIRED_NONISO',
                       #L1_ZDC_AND
                       'L1_ZDC_AND_EMPTY', 
                       'L1_ZDC_AND_UNPAIRED_ISO',
                       'L1_ZDC_AND_UNPAIRED_NONISO', 
                       #
                       'L1_ZDC_A_C_BGRP9',
                       'L1_ZDC_A_C_EMPTY',
                       'L1_ZDC_A_C_UNPAIRED_ISO',
                       'L1_ZDC_A_C_UNPAIRED_NONISO',
                       #MBTS
                       'L1_MBTS_1_1_BGRP9',
                       'L1_MBTS_1_1_EMPTY',
                       'L1_MBTS_2_2_EMPTY',
                       'L1_MBTS_3_3_EMPTY',
                       'L1_MBTS_1_1_UNPAIRED_ISO',
                       'L1_MBTS_2_2_UNPAIRED_ISO',
                       'L1_MBTS_3_3_UNPAIRED_ISO',
                       'L1_MBTS_4_4_UNPAIRED_ISO',
                       #LUCID
                       'L1_LUCID_EMPTY',
                       'L1_LUCID_UNPAIRED_ISO',#'L1_LUCID_COMM_UNPAIRED_ISO',
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

                       ######### Run-2 monitoring items taken from monitoring rules 
                       'L1_BCM_AC_ABORTGAPNOTCALIB',
                       'L1_BCM_AC_CALIB',
                       'L1_BCM_AC_UNPAIRED_ISO',
                       'L1_BCM_AC_UNPAIRED_NONISO',
                       'L1_BCM_CA_ABORTGAPNOTCALIB',
                       'L1_BCM_CA_CALIB',
                       'L1_BCM_CA_UNPAIRED_ISO',
                       'L1_BCM_CA_UNPAIRED_NONISO',
                       'L1_BCM_Wide_ABORTGAPNOTCALIB',
                       'L1_BCM_Wide_CALIB',
                       'L1_J12_ABORTGAPNOTCALIB',
                       'L1_J12_UNPAIRED_ISO',
                       'L1_J12_UNPAIRED_NONISO',
                       'L1_J12_BGRP12',
                       'L1_J30.31ETA49_UNPAIRED_ISO',
                       'L1_J30.31ETA49_UNPAIRED_NONISO',
                       'L1_J30.31ETA49_BGRP12',
                       'L1_J50_ABORTGAPNOTCALIB',
                       'L1_J50_UNPAIRED_ISO',
                       'L1_J50_UNPAIRED_NONISO',
                       'L1_CALREQ2',
                       'L1_EM10VH',
                       'L1_EM15',
                       'L1_EM15HI_2TAU12IM',
                       'L1_EM15HI_2TAU12IM_J25_3J12',
                       'L1_EM15HI_2TAU12IM_XE35',
                       'L1_EM15HI_TAU40_2TAU15',
                       #### NO-MU10 'L1_MU10_TAU12IM',
                       #### NO-MU10 'L1_MU10_TAU12IM_J25_2J12',
                       #### NO-MU10 'L1_MU10_TAU12IM_XE35',
                       #### NO-MU10 'L1_MU10_TAU12IM_XE40',
                       #### NO-MU10 'L1_MU10_TAU12_J25_2J12',
                       #### NO-MU10 'L1_MU10_TAU20',
                       #### NO-MU10 'L1_MU10_TAU20IM',
                       'L1_TAU12',
                       'L1_TAU12IL',
                       'L1_TAU12IM',
                       'L1_TAU12IT',
                       'L1_TAU20',
                       'L1_TAU20IL',
                       'L1_TAU20IL_2TAU12IL_J25_2J20_3J12',
                       'L1_TAU20IM',
                       'L1_TAU20IM_2J20_XE45',
                       'L1_TAU20IM_2J20_XE50',
                       'L1_TAU20IM_2TAU12IM',
                       'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',
                       'L1_TAU20IM_2TAU12IM_XE35',
                       'L1_TAU20IM_2TAU12IM_XE40',
                       'L1_TAU20IT',
                       'L1_TAU20_2J20_XE45',
                       'L1_TAU20_2TAU12',
                       'L1_TAU20_2TAU12_XE35',
                       'L1_TAU25IT_2TAU12IT_2J25_3J12',
                       'L1_TAU30',
                       'L1_TAU40',
                       'L1_TAU60',
                       #'L1_TAU8',
                       'L1_EM20VH_FIRSTEMPTY',
                       'L1_EM20VHI',
                       'L1_EM7_EMPTY',
                       'L1_EM7_FIRSTEMPTY',
                       'L1_J100',
                       'L1_J100.31ETA49',
                       'L1_J100.31ETA49_FIRSTEMPTY',
                       'L1_J100_FIRSTEMPTY',
                       'L1_J12',
                       'L1_J12_EMPTY',
                       'L1_J12_FIRSTEMPTY',
                       'L1_J30.31ETA49',
                       'L1_J30.31ETA49_EMPTY',
                       'L1_J40_XE50',
                       'L1_J75.31ETA49',
                       'L1_J75_XE40',
                       'L1_RD0_ABORTGAPNOTCALIB',
                       'L1_TGC_BURST',
                       'L1_XE35',
                       'L1_XE50',
		
		

                       
                       ]

    Lvl1Flags.CtpIdMap = {
	'L1_EM18' : 0,
        'L1_EM22' : 1,
        'L1_EM7' : 2,
        'L1_EM10' : 3,
        'L1_EM12' : 4,
        'L1_EM14' : 5,
        'L1_2MU4_EMPTY' : 6,
        'L1_RD0_UNPAIRED_NONISO' : 7,
        'L1_BCM_AC_CA_UNPAIRED_NONISO': 8,
        'L1_FJ10_UNPAIRED_NONISO' :  9,
        'L1_2EM10' : 10,
        #'L1_2EM5' : 11,
        'L1_MU0_VTE20' : 12,
        'L1_NIM_S8C2B21'            : 13,#DIRNIM
        'L1_NIM_S8C2B22'            : 14,#DIRNIM
        'L1_MBTS_4_4' : 15,
        'L1_RD1_EMPTY': 16,
        'L1_RD0_FILLED' : 17,
        'L1_RD0_FIRSTEMPTY' : 18,
        'L1_RD0_UNPAIRED_ISO' : 19,
        'L1_MBTS_4_4_UNPAIRED_ISO': 20,        

        'L1_ZDC_AND_VTE50' : 27,
        #'L1_EM3_VTE20': 28,
                
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
        #'L1_EM3_VTE50' : 44,
        #'L1_EM3_MV_VTE50' : 45,
        'L1_J10_VTE100' : 46,
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
        #'L1_TAU3' : 64,
        'L1_MBTSC3' : 65,
        'L1_MU4_FIRSTEMPTY' : 67,
        'L1_MU6_FIRSTEMPTY' : 68,
        'L1_2MU0_EMPTY' : 69,
        'L1_MU0_FIRSTEMPTY' : 70,
        'L1_2MU6' : 71,
        'L1_2MU11' : 72,
        'L1_2MU20' : 73,
        
        'L1_MU11_EMPTY' : 75,
        'L1_MBTSC2' : 76,
        'L1_LUCID_UNPAIRED_NONISO' : 77,
        
        'L1_BCM_Wide_BGRP0' : 80,
        'L1_BCM_AC_CA_BGRP0' : 81,
        'L1_MBTSC1' : 82,
        'L1_J10_UNPAIRED' : 83,
        'L1_EM20' : 84,
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
        'L1_J35' : 100,

        'L1_J50' : 102,
        'L1_J18' : 104,
        #'L1_J5' : 105,
        'L1_BCM_AC_CA_UNPAIRED_ISO' : 106,
        'L1_BCM_Wide_EMPTY' : 107,
        'L1_BCM_Wide_UNPAIRED_ISO' : 108,
        'L1_L1_BCM_Wide_UNPAIRED_NONISO' : 109,
        
        'L1_LUCID_UNPAIRED_ISO' : 113,
        #'L1_TAU8_FIRSTEMPTY' : 114,
        'L1_TAU8_UNPAIRED_ISO' : 115,
        'L1_TAU8_UNPAIRED_NONISO' : 116,
        'L1_ZDC_A_C_UNPAIRED_ISO' : 117,

        'L1_MBTSA0' : 120,
        'L1_MBTSA1' : 122,
        'L1_FJ0' : 123,
        'L1_2MU6_UNPAIRED_ISO' : 124,
        'L1_2MU6_UNPAIRED_NONISO' : 125,
        'L1_BCM_Wide_UNPAIRED_NONISO' : 126,
        'L1_EM7_UNPAIRED_ISO' : 127,
        'L1_EM7_UNPAIRED_NONISO' : 128,
        'L1_J30_FIRSTEMPTY' : 130,
        'L1_MBTSA2' : 131,
        'L1_TE5' : 132,
        'L1_TE14000' : 133,
        'L1_TE20' : 134,
        'L1_TE50' : 138,
        'L1_TE100' : 136,
        'L1_TE200' : 135,
        'L1_MBTSA3' : 137,
        
        'L1_2J5' : 139,
        'L1_2J10' : 140,
        'L1_TE12000' : 141,
	'L1_TE10000' : 142,
        'L1_2MU6_EMPTY' : 143,
        'L1_2MU6_FIRSTEMPTY' : 144,
        'L1_ZDC_MBTS_1' : 145,
        'L1_ZDC_MBTS_2' : 146,
        'L1_ZDC_MBTS_1_1' : 147,
        'L1_ZDC_MBTS_2_2' : 148,
        'L1_MBTS_1_EMPTY' : 149,
        'L1_MBTS_1_1_EMPTY' : 150,
        'L1_MBTS_2_EMPTY' : 151,
        #'L1_TAU8_EMPTY' : 152,
        'L1_MBTSA4' : 153,
        'L1_MBTSA5' : 154,
        'L1_MBTSA6' : 155,
        'L1_MBTSA7' : 156,
        'L1_NIM_S8C2B23'            : 157,#DIRNIM
        
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
        #'L1_J10_EMPTY' : 171,
        'L1_J30_EMPTY' : 172,
        'L1_MBTSA12' : 173,
        'L1_MBTSA13' : 174,
        'L1_FJ0_EMPTY' : 175,
        'L1_MBTSA14' : 176,
        #'L1_EM3_EMPTY' : 177,
        'L1_MBTSA15' : 178,
        'L1_FJ0_UNPAIRED_ISO' : 180,
        'L1_FJ5_UNPAIRED_ISO' : 181,
        'L1_ZDC_UNPAIRED_ISO' : 182,
        'L1_ZDC_UNPAIRED_NONISO' : 183,
        'L1_J10_EMPTY' : 184,
        'L1_J10_FIRSTEMPTY' : 185,
        'L1_J10_UNPAIRED_ISO' : 186,
        'L1_J10_UNPAIRED_NONISO' : 187,
        'L1_ZDC_A_BGRP7' : 188,
        'L1_ZDC_AND' : 189,
        'L1_ZDC_A' : 190,
        'L1_ZDC_C' : 191,
        'L1_ZDC_A_C' : 192,
        'L1_ZDC' : 193,
        'L1_ZDC_C_BGRP7' : 194,
        
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
        
        'L1_FJ5_EMPTY' : 218,
        'L1_FJ0_C' : 219,
        
        'L1_MBTS_2' : 222,
        'L1_MBTS_2_2' : 223,
        'L1_MBTS_3_3' : 224,
        'L1_BCM_HT_BGRP0' : 225,
        'L1_MBTS_1' : 226,
        
        'L1_MBTS_1_1' : 228,
        'L1_MBTS_2_2_EMPTY' : 229,
        'L1_MBTS_3_3_EMPTY' : 230,
        'L1_MBTS_2_2_UNPAIRED_ISO' : 231,
        'L1_MBTS_3_3_UNPAIRED_ISO' : 232,
        #'L1_J5_TE90' : 233,
        'L1_2J5_TE90' : 234,
	'L1_ZDC_A_C_BGRP11' : 235,
        'L1_LUCID_A_EMPTY' : 236,
        'L1_LUCID_C_EMPTY' : 237,
        'L1_LUCID_A_C_EMPTY' : 238,
        'L1_FJ0_A' : 239,
        'L1_MBTS_1_1_BGRP11' : 240,
        'L1_BPTX0_BGRP0' : 241,
        'L1_BPTX1_BGRP0' : 242,
        'L1_MBTS_2_BGRP9' : 243,
        'L1_MBTS_1_1_BGRP9' : 244,
        'L1_LUCID_EMPTY' : 245,
        'L1_RD0_BGRP9' : 246,
        'L1_LHCF' : 247,
        'L1_ZDC_A_C_BGRP9' : 248,
        'L1_MBTS_2_BGRP11' : 249,
        'L1_ZB' : 250,
        
        'L1_BGRP1' : 252,


        
	#new in Run 2
	'L1_ZDC_A_C_VTE50_OVERLAY' : 256,
	'L1_TE50_OVERLAY' : 257,
        'L1_J10_VTE200' : 258,
        'L1_BCM_AC_ABORTGAPNOTCALIB': 259,
        'L1_BCM_AC_CALIB': 260,
        'L1_BCM_AC_UNPAIRED_ISO': 261,
        'L1_BCM_AC_UNPAIRED_NONISO': 262,
        'L1_BCM_CA_ABORTGAPNOTCALIB': 263,
        'L1_BCM_CA_CALIB': 264,
        'L1_BCM_CA_UNPAIRED_ISO': 265,
        'L1_BCM_CA_UNPAIRED_NONISO': 266,
        'L1_BCM_Wide_ABORTGAPNOTCALIB': 267,
        'L1_BCM_Wide_CALIB': 268,
        'L1_J12_ABORTGAPNOTCALIB': 269,
        'L1_J12_UNPAIRED_ISO': 270,
        'L1_J12_UNPAIRED_NONISO': 271,
        'L1_J12_BGRP12': 493,
        'L1_J30.31ETA49_UNPAIRED_ISO': 272,
        'L1_J30.31ETA49_UNPAIRED_NONISO': 273,
        'L1_J30.31ETA49_BGRP12': 494,
        'L1_J50_ABORTGAPNOTCALIB': 274,
        'L1_J50_UNPAIRED_ISO': 275,
        'L1_J50_UNPAIRED_NONISO': 276,  
        #'L1_TAU8': 277,         
        'L1_TAU12': 278,
        'L1_TAU12IL': 279,
        'L1_TAU12IM': 280,
        'L1_TAU12IT': 281,
        'L1_TAU20': 282,
        'L1_TAU20IL': 283,
        'L1_TAU20IL_2TAU12IL_J25_2J20_3J12': 284,
        'L1_TAU20IM': 285,
        'L1_TAU20IM_2J20_XE45': 286,
        'L1_TAU20IM_2J20_XE50': 287,
        'L1_TAU20IM_2TAU12IM': 288,
        'L1_TAU20IM_2TAU12IM_J25_2J20_3J12': 289,
        'L1_TAU20IM_2TAU12IM_XE35': 290,
        'L1_TAU20IM_2TAU12IM_XE40': 291,
        'L1_TAU20IT': 292,
        'L1_TAU20_2J20_XE45': 293,
        'L1_TAU20_2TAU12': 294,
        'L1_TAU20_2TAU12_XE35': 295,
        'L1_TAU25IT_2TAU12IT_2J25_3J12': 296,
        'L1_TAU30': 297,
        'L1_TAU40': 298,
        'L1_TAU60': 299,
        'L1_EM20VH_FIRSTEMPTY': 300,
        'L1_EM20VHI': 301,
        'L1_EM7_EMPTY': 302,
        'L1_EM7_FIRSTEMPTY': 303,
        'L1_J100': 304,
        'L1_J100.31ETA49': 305,
        'L1_J100.31ETA49_FIRSTEMPTY': 306,
        'L1_J100_FIRSTEMPTY': 307,
        'L1_J12': 308,
        'L1_J12_EMPTY': 309,
        'L1_J12_FIRSTEMPTY': 310,
        'L1_J30.31ETA49': 311,
        'L1_J30.31ETA49_EMPTY': 312,   
        'L1_J40_XE50': 313,
        'L1_J75.31ETA49': 314,
        'L1_J75_XE40': 315,
        'L1_RD0_ABORTGAPNOTCALIB': 316,
        'L1_TGC_BURST': 317,
        'L1_XE35': 318,
        'L1_XE50': 319,
        'L1_EM10VH': 320, 
        'L1_EM15': 321, 
        'L1_EM15HI_2TAU12IM': 322, 
        'L1_EM15HI_2TAU12IM_J25_3J12': 323,
        'L1_EM15HI_2TAU12IM_XE35': 324, 
        'L1_EM15HI_TAU40_2TAU15': 325,
        
	# restricted TE
        'L1_TE500.0ETA49': 326,
        'L1_TE1500.0ETA49': 327,
        'L1_TE3000.0ETA49': 328,
        'L1_TE3500.0ETA49': 329,
        'L1_TE4500.0ETA49': 330,
        'L1_TE6500.0ETA49': 331,
        'L1_TE8000.0ETA49': 332,
        'L1_TE9000.0ETA49': 333,

	#'L1_J5': 334,
	'L1_TE5_VTE200': 335,

        # restricted TE for overlay
        'L1_TE500.0ETA49_OVERLAY': 336,
        'L1_TE1500.0ETA49_OVERLAY': 337,
        'L1_TE3000.0ETA49_OVERLAY': 338,
        'L1_TE3500.0ETA49_OVERLAY': 339,
        'L1_TE4500.0ETA49_OVERLAY': 340,
        'L1_TE6500.0ETA4_OVERLAY9': 341,
        'L1_TE8000.0ETA49_OVERLAY': 342,
        'L1_TE9000.0ETA49_OVERLAY': 343,

	'L1_EM8':      344,
	'L1_2EM7':      345,
        'L1_CALREQ2' : 511,
	


	'L1_TRT_FILLED' : 482,
	'L1_TRT_EMPTY'  : 483,
	'L1_ZDC_A_VZDC_C_VTE200' : 484,
	'L1_ZDC_C_VZDC_A_VTE200' : 485,
	'L1_ZDC_XOR' : 486,
	'L1_ZDC_XOR_VTE200' : 487,
	'L1_ZDC_XOR_TE5_VTE200' : 488,
	'L1_ZDC_XOR_TRT_VTE200' : 489,
	'L1_ZDC_VTE200' : 490,
	'L1_ZDC_A_C_VTE200' : 491,
	'L1_ZDC_A_C_TE5_VTE200' : 492,
	}

    Lvl1Flags.prescales = {}
    
