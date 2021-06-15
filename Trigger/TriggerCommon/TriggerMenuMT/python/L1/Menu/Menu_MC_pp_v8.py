# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Run this file in order to print out the empty slots

from TriggerMenuMT.L1.Base.L1MenuFlags import L1MenuFlags

def print_available():
    import logging
    defineMenu()
    available = list(set(range(509)) - set(L1MenuFlags.CtpIdMap.value.values()) - set([508]))
    freeItems = 512 - len(L1MenuFlags.items.value) # correct for ZB and CALREQ items
    floatingItems = sorted(list(set(L1MenuFlags.items.value) - set(L1MenuFlags.CtpIdMap.value.keys()))) # these items get their CTPID assigned automatically
    unusedItemsWithCTPID = set(L1MenuFlags.CtpIdMap.value.keys()) - set(L1MenuFlags.items.value) # this should be empty, otherwise remove the items from the CtpIdMap
    available.sort()
    logging.info("There are %d available CTP IDs: %s", len(available), ",".join(map(str,available)))
    logging.info("IDs >= 472 go in partition 2, IDs >= 492 go in partition 3")
    logging.info("There are %d free items", freeItems)
    logging.info("There are %d floating items: %s", len(floatingItems), ",".join(map(str,floatingItems)))
    logging.info("There are %d unused items with CTP ID: %s", len(unusedItemsWithCTPID), ",".join(map(str,unusedItemsWithCTPID)))

def defineMenu():

    L1MenuFlags.CTPVersion = 4 # new CTP

    L1MenuFlags.BunchGroupPartitioning = [1, 15, 15] # partition 1: 1-10, partition 2: empty (was 14), partition 3: 15 (note that BGRP0 is used by all items)
    L1MenuFlags.BunchGroupNames = ['BCRVeto', 'Paired', 'CalReq', 'Empty', 
                                   'IsolatedUnpaired', 'NonIsolatedUnpaired', 'EmptyAfterPaired', 'InTrain', 
                                   'AbortGapNotCalReq', 'VdM', 'ALFA', 'EmptyBeforePaired',
                                   'EmptyAndPaired']

    L1MenuFlags.MenuPartitioning = [0, 472, 492] # partition 1: ctpid 0-471, partition 2: ctpid 472-491, partition 3: ctpid 492-511



    L1MenuFlags.items = [

        ##
        # single EM
        ##
        'L1_EM3','L1_EM7','L1_EM12', 'L1_EM8VH', 'L1_EM10VH', 'L1_EM15', 'L1_EM15VH', 'L1_EM15VHI', 'L1_EM18VHI',
        'L1_EM20VH', 'L1_EM20VHI', 'L1_EM22VH', 'L1_EM22VHI', 'L1_EM24VHI',
        'L1_EM3_EMPTY', 'L1_EM7_EMPTY', 'L1_EM7_UNPAIRED_ISO', 'L1_EM7_FIRSTEMPTY',
        'L1_EM20VH_FIRSTEMPTY',
        # new calo
        'L1_eEM3', 'L1_eEM8', 'L1_eEM10', 'L1_eEM15', 'L1_eEM20',
        'L1_eEM22', 'L1_eEM22VHI', 'L1_eEM20VH', 'L1_eEM20VHI', 'L1_2eEM8',
        'L1_2eEM10',

        ## 
        # single MU
        ##
        'L1_MU4', 'L1_MU6', 'L1_MU10', 'L1_MU11', 'L1_MU20', 'L1_MU21',
        'L1_MU4_EMPTY', 'L1_MU6_EMPTY', 'L1_MU4_FIRSTEMPTY', 'L1_MU11_EMPTY',
        'L1_MU4_UNPAIRED_ISO',

        ##
        # combined lepton (e and mu)
        ##
        'L1_2EM3', 'L1_2EM7', 'L1_2EM15', 'L1_2EM15VH', 'L1_2EM15VHI',
        'L1_2EM20VH', 'L1_EM20VH_3EM10VH',
        'L1_EM7_MU10',
        'L1_2MU4', 'L1_2MU6', 'L1_2MU10', 'L1_2MU20_OVERLAY', 'L1_MU11_2MU6',
        'L1_3MU4', 'L1_MU6_2MU4', 'L1_3MU6',  'L1_4MU4', 'L1_MU6_3MU4', 'L1_2MU6_3MU4', 'L1_2MU11', 'L1_MU11_2MU10',
        'L1_2EM8VH_MU10', 'L1_EM15VH_MU10',  'L1_EM7_MU20', 'L1_EM20VH_MU20',
        # new calo
        'L1_2eEM15VHI', 'L1_2eEM20VH', 'L1_2eEM8VH_MU10',
        'L1_eEM20VH_3eEM10VH',
        'L1_eEM7_MU20',

        # single tau
        'L1_TAU8', 'L1_TAU12IM',   'L1_TAU20IM',  'L1_TAU40', 'L1_TAU60', 'L1_TAU100',
        'L1_TAU8_EMPTY', 'L1_TAU8_FIRSTEMPTY', 'L1_TAU8_UNPAIRED_ISO', 'L1_TAU40_EMPTY', 'L1_TAU40_UNPAIRED_ISO',
        # new calo
        'L1_eTAU12', 'L1_eTAU100',

        # multi tau
        'L1_2TAU8', 'L1_TAU20IM_2TAU12IM', 'L1_TAU60_2TAU40',
        # new calo
        'L1_eTAU60_2eTAU40',

        # combined tau - lepton
        'L1_EM15VHI_2TAU12IM',
        'L1_MU10_TAU12IM',  
        'L1_MU10_TAU12IM_J25_2J12',
        'L1_MU10_TAU12IM_3J12',
        'L1_EM15VHI_2TAU12IM_J25_3J12',
        'L1_MU10_TAU20IM',
        'L1_MU11_TAU20IM',
        'L1_MU10_TAU20IM_J25_2J20',
        # new calo
        'L1_MU10_eTAU20IM',

        # combined tau - jet
        'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',
        'L1_TAU20IM_2TAU12IM_4J12p0ETA25',
        'L1_TAU25IM_2TAU20IM_2J25_3J20',
        # new calo
        'L1_eTAU20IM_2jJ20_gXERHO45',
        'L1_eTAU20IM_2eTAU12IM_4jJ12p0ETA25',
        'L1_eTAU25IM_2eTAU20IM_2jJ25_3jJ20',

        # combined tau - xe
        'L1_TAU20IM_2J20_XE45',
        'L1_TAU20IM_2J20_XE50',
        'L1_EM15VHI_2TAU12IM_XE35',
        'L1_EM15VHI_2TAU12IM_4J12',
        'L1_MU10_TAU12IM_XE35',  
        'L1_TAU20IM_2TAU12IM_XE35', 
        'L1_TAU40_2TAU12IM_XE40',  
        # new calo
        'L1_eTAU40_2eTAU12IM_gXERHO40',

        # combined em - jet
        'L1_EM18VHI_3J20',
        'L1_EM20VH_3J20',
        'L1_eEM18VHI_3jJ20',

        # combined em / mu - tau - jet / XE
        'L1_eEM15VHI_2eTAU12IM_4jJ12',
        'L1_eEM15VHI_2eTAU12IM_jJ25_3jJ12',
        'L1_eEM15VHI_2eTAU12IM_gXERHO35',
        'L1_eEM20VHI_eTAU20IM_2eTAU20_jJ25_3jJ20',
        'L1_MU10_eTAU12IM_3jJ12',
        'L1_MU10_eTAU12IM_jJ25_2jJ12',
        'L1_MU10_eTAU12IM_gXERHO35',
        
        # combined mu - jet
        'L1_MU6_J20', 'L1_MU6_J40', 'L1_MU6_J75',
        #ATR-13743 
        'L1_MU10_2J20','L1_MU10_3J20', 'L1_MU10_2J15_J20',
        'L1_MU20_J40',
        'L1_MU20_XE30',
        'L1_MU10_2jJ15_jJ20',
        'L1_MU10_2jJ20',
        'L1_MU20_jJ40',
        'L1_MU20_gXERHO30',
        'L1_MU20_J50',

        # single jet
        'L1_J15','L1_J20','L1_J25', 'L1_J30', 'L1_J40', 'L1_J50' ,'L1_J75','L1_J85', 'L1_J100', 'L1_J400',
        'L1_J400_LAR',
        'L1_J20p31ETA49', 'L1_J30p31ETA49', 'L1_J50p31ETA49', 'L1_J75p31ETA49', 'L1_J15p31ETA49',
        'L1_J12_EMPTY','L1_J12_FIRSTEMPTY', 'L1_J12_UNPAIRED_ISO', 'L1_J12_UNPAIRED_NONISO', 'L1_J12_ABORTGAPNOTCALIB',
        'L1_J15p31ETA49_UNPAIRED_ISO',
        'L1_J30_EMPTY', 'L1_J30_FIRSTEMPTY', 'L1_J30p31ETA49_EMPTY', 'L1_J30p31ETA49_UNPAIRED_ISO', 'L1_J30p31ETA49_UNPAIRED_NONISO',
        'L1_J50_UNPAIRED_ISO', 'L1_J50_UNPAIRED_NONISO', 'L1_J50_ABORTGAPNOTCALIB',         
        'L1_J100_FIRSTEMPTY',
        'L1_jJ100',
       
        # multi jet
        'L1_J45p0ETA21_3J15p0ETA25',
        'L1_J50_2J40p0ETA25_3J15p0ETA25',
        'L1_3J50', 'L1_4J15', 'L1_4J20',
        'L1_3J15p0ETA25_XE40',
        'L1_6J15',
        'L1_J85_3J30',
        'L1_3jJ50',
        'L1_4jJ15',
        'L1_jJ25p0ETA23_2jJ15p31ETA49',
        'L1_jJ40p0ETA25_2jJ25_jJ20p31ETA49',
        'L1_jJ75p31ETA49',
        'L1_jJ85_3jJ30',

        # multi jet forward
        'L1_J25p0ETA23_2J15p31ETA49',
        'L1_J40p0ETA25_2J15p31ETA49',
        'L1_J40p0ETA25_2J25_J20p31ETA49',
        
        # multi jet central
        'L1_3J25p0ETA23',
        'L1_3J35p0ETA23',
        'L1_4J15p0ETA25',
        'L1_5J15p0ETA25', 
        'L1_3jJ15p0ETA25_gXERHO40',
        'L1_3jJ35p0ETA23',
        'L1_4jJ15p0ETA25',

        # combined jet
        'L1_2J15_XE55', 'L1_J40_XE50',
        'L1_2J50_XE40', 'L1_J40_XE60',
        # new calo
        'L1_2jJ50_gXERHO40',
        
        # XE
        'L1_XE35', 'L1_XE40', 'L1_XE45', 'L1_XE50', 
        'L1_XE55', 'L1_XE60', 'L1_XE30', 'L1_XE300',
        # new calo
        'L1_gXERHO50',
        'L1_gXEPUFIT50',
        'L1_gXERHO20',
        'L1_gXEPUFIT20',
        'L1_gXE50',
        'L1_jXE50',
                
        # RNDM
        'L1_RD0_FILLED', 'L1_RD0_UNPAIRED_ISO',  'L1_RD0_EMPTY', 'L1_RD0_ABORTGAPNOTCALIB',
        'L1_RD0_FIRSTEMPTY', 'L1_RD0_BGRP11',
        'L1_RD1_EMPTY',
        'L1_RD2_EMPTY',
        'L1_RD2_FILLED',
        'L1_RD3_EMPTY',
        'L1_RD3_FILLED',

        #LUCID

        # ZDC 
        # TODO: request for pilot run (ATR-22904), to be removed for pp
        'L1_ZDC_A', 'L1_ZDC_C', 'L1_ZDC_AND',

        # VDM

        # TRT
        'L1_TRT_FILLED', 'L1_TRT_EMPTY',

        # TGC
        'L1_TGC_BURST',

        # LHCF
    
        #CALREQ
        'L1_CALREQ1',
        'L1_CALREQ2',


        # BPTX
        
        # BCM
        'L1_BCM_AC_CA_BGRP0', 'L1_BCM_Wide_EMPTY','L1_BCM_Wide_UNPAIRED_ISO','L1_BCM_Wide_UNPAIRED_NONISO',
        'L1_BCM_AC_UNPAIRED_ISO','L1_BCM_CA_UNPAIRED_ISO',
        'L1_BCM_AC_UNPAIRED_NONISO','L1_BCM_CA_UNPAIRED_NONISO',
        'L1_BCM_AC_ABORTGAPNOTCALIB', 'L1_BCM_CA_ABORTGAPNOTCALIB','L1_BCM_Wide_ABORTGAPNOTCALIB',
        'L1_BCM_AC_CALIB', 'L1_BCM_CA_CALIB','L1_BCM_Wide_CALIB',

        # AFP
        # high-priority (all mu)
        'L1_AFP_A_AND_C_TOF_T0T1',
        'L1_AFP_FSA_BGRP0', 'L1_AFP_FSC_BGRP0',
        # med-priority (all mu)
        'L1_AFP_FSA_TOF_T0_BGRP0', 'L1_AFP_FSA_TOF_T1_BGRP0','L1_AFP_FSC_TOF_T0_BGRP0', 'L1_AFP_FSC_TOF_T1_BGRP0',
        # low-priority (all mu)
        'L1_AFP_FSA_TOF_T2_BGRP0', 'L1_AFP_FSA_TOF_T3_BGRP0','L1_AFP_FSC_TOF_T2_BGRP0', 'L1_AFP_FSC_TOF_T3_BGRP0',
        'L1_AFP_A_OR_C_UNPAIRED_ISO', 'L1_AFP_A_OR_C_UNPAIRED_NONISO', 'L1_AFP_A_OR_C_EMPTY', 'L1_AFP_A_OR_C_FIRSTEMPTY',
        'L1_AFP_A_AND_C_TOF_J50', 'L1_AFP_A_AND_C_TOF_T0T1_J50','L1_AFP_A_AND_C_TOF_J75', 'L1_AFP_A_AND_C_TOF_T0T1_J75', 
        # high-priority (low mu)
        'L1_AFP_NSA_BGRP0', 'L1_AFP_NSC_BGRP0', 
        'L1_AFP_A','L1_AFP_C', 'L1_AFP_A_AND_C',
        #'L1_AFP_A_OR_C_J5','L1_AFP_A_AND_C_J5', # J5 not available in legacy menu. Need to update to jJ threshold for low-mu
        'L1_AFP_A_OR_C_J12','L1_AFP_A_AND_C_J12',
        'L1_MU6_AFP_A_OR_C','L1_MU6_AFP_A_AND_C','L1_EM7_AFP_A_OR_C','L1_EM7_AFP_A_AND_C',
        # med-priority (low mu)
        'L1_AFP_A_OR_C','L1_AFP_A_OR_C_MBTS_2', 'L1_AFP_A_AND_C_MBTS_2',

        # MBTS
        'L1_MBTS_1_EMPTY', 'L1_MBTS_1_1_EMPTY', 'L1_MBTS_2_EMPTY', 
        'L1_MBTS_1', 'L1_MBTS_1_1', 'L1_MBTS_2',

        # extra MBTS 
        # TODO: to be removed for high-mu pp        
        'L1_MBTSA0', 'L1_MBTSA1', 'L1_MBTSA2', 'L1_MBTSA3', 'L1_MBTSA4', 'L1_MBTSA5', 'L1_MBTSA6', 'L1_MBTSA7', 'L1_MBTSA8', 'L1_MBTSA9', 'L1_MBTSA10', 'L1_MBTSA11', 'L1_MBTSA12', 'L1_MBTSA13', 'L1_MBTSA14', 'L1_MBTSA15', 'L1_MBTSC0', 'L1_MBTSC1', 'L1_MBTSC2', 'L1_MBTSC3', 'L1_MBTSC4', 'L1_MBTSC5', 'L1_MBTSC6', 'L1_MBTSC7', 'L1_MBTSC8', 'L1_MBTSC9', 'L1_MBTSC10', 'L1_MBTSC11', 'L1_MBTSC12', 'L1_MBTSC13', 'L1_MBTSC14', 'L1_MBTSC15', 

         # ZB
         # TODO: to be removed for high-mu pp
        'L1_ZB',

        #ATR-21371
        # TODO: to be removed for high-mu pp
        'L1_ALFA_ANY',
        'L1_ALFA_ELAST15', 'L1_ALFA_ELAST18',
        'L1_ALFA_B7L1U','L1_ALFA_B7L1L','L1_ALFA_A7L1U','L1_ALFA_A7L1L','L1_ALFA_A7R1U','L1_ALFA_A7R1L','L1_ALFA_B7R1U','L1_ALFA_B7R1L',
        'L1_ALFA_SYST9', 'L1_ALFA_SYST10', 'L1_ALFA_SYST11', 'L1_ALFA_SYST12', 'L1_ALFA_SYST17', 'L1_ALFA_SYST18',



        #--------------------------------
        # TOPO items
        #--------------------------------

        # legacy L1Topo
        'L1_HT190-J15s5pETA21', 
        'L1_BPH-0M9-EM7-EM5_2MU4',
        'L1_BPH-0M9-EM7-EM5_MU6',
        'L1_BPH-0DR3-EM7J15_2MU4',
        'L1_BPH-0DR3-EM7J15_MU6',
        'L1_JPSI-1M5-EM7',
        'L1_JPSI-1M5-EM12',
        'L1_MJJ-500-NFF',
        'L1_MJJ-700',
        'L1_EM18VHI_MJJ-300',
        'L1_HT150-J20s5pETA31_MJJ-400-CF',
        'L1_LLP-RO',
        'L1_LLP-NOMATCH',
        'L1_DPHI-2EM3',
        'L1_SC111-CJ15',
        'L1_J50_DETA20-J50J',
        'L1_DR-TAU20ITAU12I',
        'L1_DR-TAU20ITAU12I-J25',
        'L1_TAU60_DR-TAU20ITAU12I', 
        'L1_BPH-0M9-EM7-EM5', 
        'L1_BPH-0DR3-EM7J15',
        'L1_LAR-ZEE',

        # phase1 L1Topo
        'L1_LAR-ZEE-eEM',
        'L1_JPSI-1M5-eEM7',
        'L1_JPSI-1M5-eEM12',
        'L1_BPH-0M9-eEM7-eEM5', 'L1_BPH-0M9-eEM7-eEM5_MU6', 'L1_BPH-0M9-eEM7-eEM5_2MU4',
        'L1_BPH-0DR3-eEM7J15', 'L1_BPH-0DR3-eEM7J15_MU6', 'L1_BPH-0DR3-eEM7J15_2MU4',
        'L1_LLP-RO-eEM', 'L1_LLP-NOMATCH-eEM',
        'L1_DPHI-2eEM3', 
        'L1_jHT150-jJ20s5pETA31_jMJJ-400-CF',
        'L1_DY-BOX-2MU6',
        'L1_LFV-eEM8I-MU11', 'L1_LFV-eEM12I-MU6',
        'L1_LFV-MU6',
        'L1_LFV-MU11',
        'L1_LFV-eEM8I', 'L1_LFV-eEM15I', 
        'L1_jMJJ-700-NFF',
        'L1_jMJJ-300-NFF',
        'L1_jMJJ-500-NFF',
        'L1_jMJJ-400-CF',
        'L1_eEM18VHI_jMJJ-300-NFF',

        'L1_10DR-MU20-MU6', #ATR-19376
    
        #ATR-19720
        'L1_BPH-2M9-0DR15-2MU4',
        'L1_BPH-8M15-2MU4-BO',
        'L1_BPH-2M9-0DR15-MU6MU4',
        'L1_BPH-8M15-0DR22-MU6MU4-BO',
        'L1_BPH-2M9-2DR15-2MU6',
        'L1_BPH-8M15-0DR22-2MU6',
        
        # INVM + DPHI 
        'L1_jMJJ-400-NFF-0DPHI22',
        'L1_jMJJ-400-NFF-0DPHI24',
        'L1_jMJJ-400-NFF-0DPHI26',

        'L1_LATE-MU10_XE50','L1_LATE-MU10_XE40', 'L1_LATE-MU10_J50',

        #ATR-19355
        'L1_BPH-0M10-3MU4',

        #ATR-18824
        'L1_ZAFB-04DPHI-eEM15I',
        'L1_ZAFB-25DPHI-eEM15I',
        #ATR-22109
        'L1_ZAFB-25DPHI-eEM18I',

        'L1_DPHI-M70-2eEM10I', 'L1_DPHI-M70-2eEM12I', #ATR-19302
        'L1_DPHI-M70-2eEM12', # ATR-21637 (no shower shape cuts)

        #ATR-19510
        'L1_DY-BOX-2MU4',
                
        #ATR-17320
        'L1_CEP-CJ60',
        'L1_CEP-CJ50' ,
        'L1_AFP_A_AND_C_TOF_CEP-CJ60','L1_AFP_A_AND_C_TOF_T0T1_CEP-CJ60',

        ]


#CTP IDs are taken from this mapping. Every L1 item needs a unique ctpid.
# Run this file as python python/l1menu/Menu_MC_pp_v7.py to print out available IDs
# 463-464 are reserved for L1_RD2_BGRP14 and L1_RD3_BGRP15 (in MC_Physics_pp_v7)
# 509-511 are reserved for CALREQ
    
    L1MenuFlags.CtpIdMap = {

        'L1_RD0_FILLED' : 200,
        'L1_RD0_UNPAIRED_ISO' : 201,
        'L1_RD0_EMPTY' : 202,
        'L1_RD0_FIRSTEMPTY' : 209,
        'L1_RD0_ABORTGAPNOTCALIB':372,
        'L1_RD0_UNPAIRED_NONISO' : 178,
        'L1_RD0_BGRP9' : 210,
        'L1_RD0_BGRP10' : 473,
        'L1_RD0_BGRP11' : 211,
        'L1_RD1_EMPTY' : 204,
        'L1_RD1_FILLED' : 203,
        'L1_RD1_BGRP10' : 188,
        'L1_RD2_FILLED' : 205,
        'L1_RD2_EMPTY' : 206,
        'L1_RD2_BGRP12' : 390,
        'L1_RD2_BGRP14' : 463,
        'L1_RD3_FILLED' : 207,
        'L1_RD3_EMPTY' : 208,
        'L1_RD3_BGRP15' : 464,

        'L1_TGC_BURST' : 220,
        'L1_ZB' : 240,

        'L1_LUCID' : 212,
        'L1_LUCID_EMPTY' : 213,
        'L1_LUCID_UNPAIRED_ISO' : 214,
        'L1_LUCID_BGRP9' : 471,
        'L1_LUCID_BGRP11' : 472,

        'L1_TRT_FILLED' : 218,
        'L1_TRT_EMPTY' : 219,

        'L1_BCM_AC_CA_BGRP0' : 223,
        'L1_BCM_AC_UNPAIRED_ISO' : 163,
        'L1_BCM_CA_UNPAIRED_ISO' : 229,
        'L1_BCM_AC_UNPAIRED_NONISO' : 230,
        'L1_BCM_CA_UNPAIRED_NONISO' : 231,
        'L1_BCM_AC_ABORTGAPNOTCALIB' : 232,
        'L1_BCM_CA_ABORTGAPNOTCALIB' : 233,
        'L1_BCM_AC_CALIB' : 235,
        'L1_BCM_CA_CALIB' : 236,
        'L1_BCM_Wide_ABORTGAPNOTCALIB' : 234,  
        'L1_BCM_Wide_CALIB' : 237,
        'L1_BCM_Wide_UNPAIRED_NONISO' : 161,
        'L1_BCM_Wide_EMPTY' : 224,
        'L1_BCM_Wide_UNPAIRED_ISO' : 225,

        'L1_MBTS_1_EMPTY' : 164,
        'L1_MBTS_1_1_EMPTY' : 168,
        'L1_MBTS_2_EMPTY' : 166,

        'L1_MBTSA0' : 401,
        'L1_MBTSA1' : 402,
        'L1_MBTSA2' : 403,
        'L1_MBTSA3' : 404,
        'L1_MBTSA4' : 405,
        'L1_MBTSA5' : 406,
        'L1_MBTSA6' : 407,
        'L1_MBTSA7' : 408,
        'L1_MBTSA8' : 409,
        'L1_MBTSA9' : 410,
        'L1_MBTSA10' : 443,
        'L1_MBTSA11' : 412,
        'L1_MBTSA12' : 413,
        'L1_MBTSA13' : 415,
        'L1_MBTSA14' : 416,
        'L1_MBTSA15' : 444,
        'L1_MBTSC0' : 418,
        'L1_MBTSC1' : 419,
        'L1_MBTSC2' : 420,
        'L1_MBTSC3' : 421,
        'L1_MBTSC4' : 422,
        'L1_MBTSC5' : 423,
        'L1_MBTSC6' : 424,
        'L1_MBTSC7' : 434,
        'L1_MBTSC8' : 435,
        'L1_MBTSC9' : 436,
        'L1_MBTSC10' : 437,
        'L1_MBTSC11' : 438,
        'L1_MBTSC12' : 439,
        'L1_MBTSC13' : 440,
        'L1_MBTSC14' : 441,
        'L1_MBTSC15' : 442,

        'L1_ALFA_A7L1L': 492,
        'L1_ALFA_A7L1U': 491,
        'L1_ALFA_A7R1L': 494,
        'L1_ALFA_A7R1U': 493,
        'L1_ALFA_ANY'  : 490,
        'L1_ALFA_B7L1L': 497,
        'L1_ALFA_B7L1U': 489,
        'L1_ALFA_B7R1L': 496,
        'L1_ALFA_B7R1U': 495,

        # legacy non-physics items
        'L1_EM3_EMPTY' : 12,
        'L1_EM7_EMPTY' : 13,
        'L1_EM7_FIRSTEMPTY': 371,
        'L1_EM7_UNPAIRED_ISO': 389,
        'L1_EM20VH_FIRSTEMPTY': 411,
        'L1_TAU8_EMPTY' : 57,
        'L1_TAU8_FIRSTEMPTY' : 391,
        'L1_TAU8_UNPAIRED_ISO' : 388,
        #'L1_TAU30_EMPTY':341,
        #'L1_TAU30_UNPAIRED_ISO' : 343,
        'L1_J12_EMPTY' : 111,
        'L1_J12_FIRSTEMPTY' : 112,
        'L1_J12_UNPAIRED_ISO' : 113,
        'L1_J12_UNPAIRED_NONISO' : 114,
        'L1_J12_ABORTGAPNOTCALIB' : 115,
        'L1_J15p31ETA49_UNPAIRED_ISO' : 75,
        'L1_J30_EMPTY' : 116,
        'L1_J30_FIRSTEMPTY' : 117,
        'L1_J30p31ETA49_EMPTY' : 118,
        'L1_J30p31ETA49_UNPAIRED_ISO' : 119,
        'L1_J30p31ETA49_UNPAIRED_NONISO' : 120,
        'L1_J50_UNPAIRED_ISO' : 121,
        'L1_J50_UNPAIRED_NONISO' : 122,
        'L1_J50_ABORTGAPNOTCALIB' : 123,
        'L1_J100_FIRSTEMPTY': 414,

        # muon non-physics items
        'L1_MU4_EMPTY' : 19,
        'L1_MU4_FIRSTEMPTY' : 20,
        'L1_MU4_UNPAIRED_ISO' : 22,
        'L1_MU6_EMPTY' : 66,
        'L1_MU11_EMPTY' : 21,
        'L1_2MU20_OVERLAY' : 35,

        # physics items

        # muons
        'L1_MU4' : 14,
        'L1_MU6' : 15,
        'L1_MU10' : 16,
        'L1_MU11' : 256,
        'L1_MU20' : 18,
        'L1_MU21' : 17,
        'L1_2MU4' : 32,
        'L1_2MU6' : 33,
        'L1_2MU10' : 34,
        'L1_2MU11' : 48,
        'L1_MU11_2MU6' : 37,
        'L1_MU11_2MU10' : 49,    
        'L1_3MU4' : 38,
        'L1_MU6_2MU4' : 39,
        'L1_3MU6' : 40,
        'L1_4MU4' : 46,
        'L1_MU6_3MU4'  : 36,
        'L1_2MU6_3MU4' : 42,

        # Legacy L1Calo threholds
        # EM
        'L1_EM3' : 0,
        'L1_EM7' : 1,
        'L1_EM12' : 2,
        'L1_EM8VH' : 3,
        'L1_EM10VH' : 4,
        'L1_EM15VHI_2TAU12IM_4J12' : 5,
        'L1_EM15' : 6,
        'L1_EM15VH' : 7,
        'L1_EM18VHI' : 8,
        'L1_EM20VH' : 9,
        'L1_EM20VHI' : 10,
        'L1_EM22VHI' : 11,
        'L1_EM22VH' : 295,
        'L1_EM24VHI' : 392,
        'L1_2EM3' : 23,
        'L1_2EM7' : 24,
        #'L1_EM30VHI' : 25,
        'L1_2EM15' : 27,
        'L1_2EM15VH' : 28,
        'L1_EM15VHI' : 29,
        #'L1_EM24VHIM' : 30,
        'L1_2EM15VHI' : 31,
        'L1_2EM20VH':400,
        'L1_EM20VH_3EM10VH':431, 
        'L1_4J15p0ETA25' : 41,
        'L1_2EM8VH_MU10' : 43,
        'L1_EM15VH_MU10' : 44,
        'L1_EM7_MU20' : 483,
        'L1_EM20VH_MU20' : 484,

        # TAU
        #'L1_TAU5' : 50,
        #'L1_2TAU5' : 59,
        'L1_2TAU8': 459,
        #'L1_TAU12' : 45,
        'L1_TAU12IM' : 47,
        'L1_TAU20IM' : 51,
        'L1_TAU100' : 52,
        #'L1_TAU30' : 53,
        'L1_TAU40' : 54,
        'L1_TAU60' : 55,
        #'L1_TAU90' : 56,
        'L1_TAU20IM_2TAU12IM' : 58,
        'L1_TAU60_2TAU40': 458, 

        'L1_EM15VHI_2TAU12IM' : 60,
        'L1_EM15VHI_2TAU12IM_XE35' : 78,
        'L1_EM15VHI_2TAU12IM_J25_3J12' : 61,
        #'L1_EM15VHI_TAU40_2TAU15' : 62,
        #'L1_EM20VHI_TAU20IM_2TAU20_J25_3J20' : 69,
        'L1_MU10_TAU12IM' : 63,
        'L1_MU10_TAU12IM_J25_2J12' : 64,
        'L1_MU10_TAU12IM_3J12' : 482,
        'L1_EM7_MU10' : 65,
        'L1_MU10_TAU20IM' : 67,
        'L1_MU11_TAU20IM' : 430,
        'L1_MU10_TAU20IM_J25_2J20' : 377,


        'L1_TAU20IM_2TAU12IM_J25_2J20_3J12' : 70,
        'L1_TAU20IM_2TAU12IM_4J12p0ETA25' : 316,
        'L1_TAU20IM_2J20_XE45' : 74,
        #'L1_TAU20IM_2TAU12IM_4J12p0ETA28' : 77,
        'L1_TAU20IM_2J20_XE50' : 79,
        'L1_TAU25IM_2TAU20IM_2J25_3J20' : 398,
        'L1_MU10_TAU12IM_XE35' : 81,
        'L1_TAU20IM_2TAU12IM_XE35' : 83,
        'L1_TAU40_2TAU12IM_XE40' : 429,
        'L1_MU6_J20' : 88,
        'L1_MU6_J40' : 89,
        'L1_MU6_J75' : 90,
        'L1_MU20_J40'  : 428,
        'L1_MU20_XE30' : 433,
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
        'L1_J20p31ETA49' : 103,
        'L1_J30p31ETA49' : 104,
        'L1_J50p31ETA49' : 105,
        'L1_J75p31ETA49' : 106,
        'L1_J15p31ETA49' : 109,
        'L1_3J40' : 128,
        'L1_3J50' : 130,
        'L1_4J15' : 131,
        'L1_4J20' : 132,
        'L1_J45p0ETA21_3J15p0ETA25' : 86,
        'L1_J50_2J40p0ETA25_3J15p0ETA25' : 87,
        'L1_6J15' : 135,
        'L1_J85_3J30' : 480,
        #'L1_J30p0ETA49_2J20p0ETA49' : 137,

        'L1_5J15p0ETA25' : 140,
        'L1_2J15_XE55' : 141,
        'L1_J25p0ETA23_2J15p31ETA49': 335 ,
        'L1_J40p0ETA25_2J15p31ETA49' : 181,
        'L1_J40p0ETA25_2J25_J20p31ETA49' : 182,
        'L1_3J25p0ETA23' : 373,
        'L1_3J35p0ETA23' : 425,
        #'L1_4J20p0ETA49' : 383,

        #'L1_XE10': 68,
        'L1_XE30': 85,
        'L1_XE300': 187,
        'L1_XE35' : 144,
        'L1_XE40' : 145,
        'L1_XE45' : 146,
        'L1_XE50' : 147,
        'L1_XE55' : 148,
        'L1_XE60' : 149,
        #'L1_XE70' : 150,
        #'L1_XE80' : 151,
        #'L1_EM12_XS20' : 154,
        #'L1_EM15_XS30' : 155,
        'L1_TE10' : 158,
        #'L1_TE30' : 138,

        'L1_MU10_3J20' : 173,
        'L1_MU10_2J20' : 278,
        'L1_MU20_J50' : 82,
        'L1_MU10_2J15_J20' : 255,
        #'L1_MU6_J30p0ETA49_2J20p0ETA49' : 382,

        'L1_EM20VH_3J20' : 26,
        'L1_EM18VHI_3J20' : 172,

        'L1_3J15p0ETA25_XE40' : 184,

        'L1_J40_XE50' : 142,
        'L1_2J50_XE40' : 175,
        'L1_J40_XE60' : 176,


        # NB: 508 is reserved for the zero bias trigger, and 509-511 for the CALREQ triggers (at the moment, ATR-22654)

    }

if __name__ == "__main__": print_available()
