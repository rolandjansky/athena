
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# Run this file in order to print out the empty slots

from ..Base.L1MenuFlags import L1MenuFlags

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
        'L1_EM3','L1_EM7','L1_EM12', 'L1_EM8VH',
        'L1_EM10VH', 'L1_EM15',
        #ATR-19302
        'L1_EM15VH', 'L1_EM18VHI', 'L1_EM20VH', 'L1_EM20VHI', 'L1_EM22VH', 'L1_EM22VHI', 'L1_EM24VHI', # see Jira ATR-10334
        #'L1_EM22VHLIL', #'L1_EM50V',
        'L1_EM3_EMPTY', 'L1_EM7_EMPTY', 'L1_EM7_UNPAIRED_ISO', 'L1_EM7_FIRSTEMPTY',
        'L1_EM20VH_FIRSTEMPTY',
        # new calo
        'L1_eEM3', 'L1_eEM8', 'L1_eEM10', 'L1_eEM15', 'L1_eEM20',
        'L1_eEM22', 'L1_eEM22VHI', 'L1_eEM20VH', 'L1_eEM20VHI', 'L1_2eEM8',
        'L1_2eEM10',

        # see savannah https://savannah.cern.ch/bugs/?103935

        # combined EM -jet
#        'L1_JJ15.23ETA49', # 4x4 window
#        'L1_J12.0ETA23',

        ## 
        # single MU
        ##
        'L1_MU4', 'L1_MU6', 'L1_MU10', 'L1_MU11', 'L1_MU20', 'L1_MU21',
        'L1_MU4_EMPTY', 'L1_MU6_EMPTY', 'L1_MU4_FIRSTEMPTY', 'L1_MU11_EMPTY',
        'L1_MU4_UNPAIRED_ISO',

        ##
        # multi lepton e+mu
        ##
        'L1_2EM3', 'L1_2EM7', 'L1_2EM10VH', 'L1_2EM15', 'L1_2EM15VH',   
        'L1_2EM20VH',  'L1_EM7_2EM3', 'L1_EM12_2EM3', 'L1_EM15VH_3EM7', 'L1_EM20VH_3EM10VH', 
        'L1_EM7_MU10',
        'L1_2MU4', 'L1_2MU6', 'L1_2MU10', 'L1_2MU20_OVERLAY',  'L1_MU10_2MU6', 'L1_MU11_2MU6', 'L1_3MU4', 'L1_MU6_2MU4', 'L1_3MU6',  'L1_4MU4', 'L1_MU6_3MU4', 'L1_2MU6_3MU4', 'L1_2MU11', 'L1_MU11_2MU10',
        'L1_2EM8VH_MU10', 'L1_EM15VH_MU10',  'L1_EM7_MU20', 'L1_EM8VH_MU20', 'L1_EM8I_MU10',
        # new calo
        'L1_2eEM15VHI', 'L1_2eEM20VH', 'L1_2eEM8VH_MU10',
        'L1_eEM15VH_MU10',
        'L1_eEM20VH_3eEM10VH',
        'L1_eEM7_MU20',

        # single tau
        'L1_TAU8', 'L1_TAU12', 'L1_TAU12IM',   'L1_TAU20IM', 'L1_TAU20', 'L1_TAU30', 'L1_TAU40', 'L1_TAU60', 'L1_TAU100',
        'L1_TAU30_EMPTY','L1_TAU30_UNPAIRED_ISO', 'L1_TAU8_EMPTY', 
        'L1_TAU8_UNPAIRED_ISO','L1_TAU8_FIRSTEMPTY',
        # new calo
        'L1_eTAU12', 'L1_eTAU100',

        # multi tau
        'L1_TAU20IM_2TAU12IM', 'L1_TAU60_2TAU40', 'L1_2TAU8', 'L1_TAU20_2TAU12',
        # new calo
        'L1_eTAU60_2eTAU40',

        # combined tau - lepton
        'L1_EM15VHI_2TAU12IM',
        'L1_MU10_TAU12IM',  
        'L1_MU10_TAU12IM_J25_2J12',
        'L1_MU10_TAU12IM_3J12',
        'L1_EM15VHI_2TAU12IM_J25_3J12',
        'L1_EM15VHI_TAU40_2TAU15',
        'L1_MU10_TAU20IM',
        'L1_MU11_TAU20IM',
        'L1_MU10_TAU20IM_J25_2J20',
        # new calo
        'L1_MU10_eTAU20IM',

        # combined tau - jet
        'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',
        'L1_TAU20IM_2TAU12IM_4J12.0ETA23',
        'L1_DR-MU10TAU12I_TAU12I-J25',
        # new calo
        'L1_eTAU20IM_2jJ20_gXERHO45',
        'L1_eTAU20IM_2eTAU12IM_4jJ12.0ETA23',
        'L1_eTAU25IM_2eTAU20IM_2jJ25_3jJ20',

        # combined tau - xe
        'L1_TAU20IM_2J20_XE45',
        'L1_EM15VHI_2TAU12IM_4J12',
        'L1_EM20VHI_TAU20IM_2TAU20_J25_3J20',
        'L1_MU10_TAU12IM_XE35',  
        'L1_TAU20IM_2TAU12IM_XE35', 
        'L1_TAU40_2TAU12IM_XE40',  
        'L1_eTAU40_2eTAU12IM_gXERHO40',

        # combined em - jet
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
        'L1_MU6_J20', 'L1_MU6_J30.0ETA49_2J20.0ETA49', 'L1_MU6_J40', 'L1_MU6_J75',
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
        'L1_J12', 'L1_J15','L1_J20','L1_J25', 'L1_J30', 'L1_J40', 'L1_J50' ,'L1_J75','L1_J85', 'L1_J100', 'L1_J120', 'L1_J400',
        'L1_J20.31ETA49', 'L1_J30.31ETA49', 'L1_J50.31ETA49', 'L1_J75.31ETA49', 'L1_J15.31ETA49',
        'L1_J12_EMPTY','L1_J12_FIRSTEMPTY', 'L1_J12_UNPAIRED_ISO', 'L1_J12_UNPAIRED_NONISO', 'L1_J12_ABORTGAPNOTCALIB',
        'L1_J15.31ETA49_UNPAIRED_ISO',
        'L1_J30_EMPTY', 'L1_J30_FIRSTEMPTY', 'L1_J30.31ETA49_EMPTY', 'L1_J30.31ETA49_UNPAIRED_ISO',
        'L1_J30.31ETA49_UNPAIRED_NONISO',
        'L1_J50_UNPAIRED_ISO', 'L1_J50_UNPAIRED_NONISO', 'L1_J50_ABORTGAPNOTCALIB',         
        'L1_J100_FIRSTEMPTY',
        'L1_jJ100',
       
        
        # multi jet
        'L1_J45.0ETA20_3J15.0ETA25',
        'L1_J50_2J40.0ETA25_3J15.0ETA25',
        'L1_3J15', 'L1_3J40', 'L1_3J50', 'L1_4J15', 'L1_4J20',
        'L1_4J20.0ETA49',
        'L1_3J15.0ETA25_XE40',
        'L1_6J15',
        'L1_J85_3J30',
        'L1_J30.0ETA49_2J20.0ETA49',
        'L1_3jJ50',
        'L1_4jJ15',
        'L1_jJ25.0ETA23_2jJ15.31ETA49',
        'L1_jJ40.0ETA25_2jJ25_jJ20.31ETA49',
        'L1_jJ75.31ETA49',
        'L1_jJ85_3jJ30',

        # multi jet forward
        'L1_J25.0ETA23_2J15.31ETA49',
        'L1_J40.0ETA25_2J15.31ETA49',
        'L1_J40.0ETA25_2J25_J20.31ETA49',
        
        # multi jet central
        'L1_3J25.0ETA23',
        'L1_3J35.0ETA23',
        'L1_4J15.0ETA25',
        'L1_5J15.0ETA25', 
        'L1_3jJ15.0ETA25_gXERHO40',
        'L1_3jJ35.0ETA23',
        'L1_4jJ15.0ETA25',

        # combined jet
        'L1_2J15_XE55', 'L1_J40_XE50',
        'L1_2J50_XE40', 'L1_J40_XE60',
        # new calo
        'L1_2jJ50_gXERHO40',
        

        # XE
        'L1_XE10', 'L1_XE35', 'L1_XE40', 'L1_XE45', 'L1_XE50', 
        'L1_XE55', 'L1_XE60', 'L1_XE70', 'L1_XE80', 'L1_XE30', 'L1_XE300',
        # new calo
        'L1_gXERHO50',
        'L1_gXEPUFIT50',
        'L1_gXERHO20',
        'L1_gXEPUFIT20',
        'L1_gXE50',
        'L1_jXE50',
        
        #XS
        'L1_EM12_XS20', 'L1_EM15_XS30',
        
        # TE
        'L1_TE5', 'L1_TE10', 'L1_TE15', 'L1_TE20', 'L1_TE25', 'L1_TE30', 'L1_TE40', 'L1_TE50',


        # RNDM
        'L1_RD0_FILLED', 'L1_RD0_UNPAIRED_ISO',  'L1_RD0_EMPTY', 'L1_RD0_ABORTGAPNOTCALIB',
        'L1_RD0_UNPAIRED_NONISO', 'L1_RD0_BGRP9', 'L1_RD0_BGRP10', 'L1_RD0_BGRP11', 
        'L1_RD1_BGRP10', 'L1_RD1_FILLED', 'L1_RD1_EMPTY',
        'L1_RD2_EMPTY', 'L1_RD2_BGRP12', 'L1_RD2_FILLED',
        'L1_RD3_FILLED', 'L1_RD3_EMPTY', 'L1_RD3_BGRP15',

        #LUCID
        'L1_LUCID', 'L1_LUCID_EMPTY', 'L1_LUCID_UNPAIRED_ISO',
        'L1_LUCID_BGRP9', 'L1_LUCID_BGRP11',


        # ZDC

        # VDM

        # TRT
        'L1_TRT_FILLED', 'L1_TRT_EMPTY',

        # TGC
        'L1_TGC_BURST',

        # LHCF
    
        #CALREQ
        'L1_CALREQ2',

        # ZB
        'L1_ZB',

        # BPTX
        
        # BCM
         #'L1_BCM_AC_CA_UNPAIRED_ISO',
        'L1_BCM_AC_CA_BGRP0', 'L1_BCM_Wide_EMPTY','L1_BCM_Wide_UNPAIRED_ISO','L1_BCM_Wide_UNPAIRED_NONISO',
        'L1_BCM_AC_UNPAIRED_ISO','L1_BCM_CA_UNPAIRED_ISO',
        'L1_BCM_AC_UNPAIRED_NONISO','L1_BCM_CA_UNPAIRED_NONISO',
        'L1_BCM_AC_ABORTGAPNOTCALIB', 'L1_BCM_CA_ABORTGAPNOTCALIB','L1_BCM_Wide_ABORTGAPNOTCALIB',
        'L1_BCM_AC_CALIB', 'L1_BCM_CA_CALIB','L1_BCM_Wide_CALIB',

        # AFP

        # MBTS
        'L1_MBTS_1_EMPTY',

        #--------------------------------
        # TOPO items
        #--------------------------------

        # B-jet
        'L1_BTAG-MU4J15', 'L1_BTAG-MU6J20',
        #'L1_BTAG-MU4J20',
        #'L1_3J20_BTAG-MU4J30', 'L1_3J20_BTAG-MU6J25',

        # ZH
            #'L1_J40_DPHI-J20s2XE30',

        #Higgsino
        'L1_2MU4_J40_XE50',
        'L1_MU4_J30_XE40_DPHI-J20s2XE30','L1_MU4_J50_XE50_DPHI-J20s2XE30',
        'L1_MU4_XE60',

        # HT
        'L1_HT190-J15.ETA21',
        'L1_HT190-J15s5.ETA21',
        'L1_HT150-J20.ETA31',
        'L1_HT150-J20s5.ETA31',   

        'L1_HT150-J20s5.ETA31_MJJ-400-CF',
        
        # Jpsi T&P
        'L1_JPSI-1M5-EM7', 'L1_JPSI-1M5-EM12',    
#            
        'L1_DY-DR-2MU4',
        #'L1_DY-BOX-MU6MU4',
        'L1_DY-BOX-2MU6',
        'L1_LFV-MU6',
        'L1_LFV-MU11',
        'L1_LFV-EM8I',
        'L1_LFV-EM15I',
        'L1_DPHI-J20s2XE50',
        'L1_J40_XE50_DPHI-J20s2XE50',
#        'L1_DPHI-J20XE50',
#        'L1_DPHI-CJ20XE50',
        'L1_2MU4_J20_XE30_DPHI-J20s2XE30',
        'L1_MJJ-700',
        'L1_MJJ-800',
        'L1_MJJ-400',
        #'L1_MJJ-350',
#        'L1_MJJ-300',
#        'L1_MJJ-200',
        'L1_MJJ-100',
#        'L1_MJJ-600-NFF',
        'L1_MJJ-500-NFF',
        'L1_MJJ-400-NFF',
#        'L1_MJJ-200-NFF',
#        'L1_63DETA-J30J15',
        'L1_J50_DETA20-J50J',
        'L1_DPHI-2EM3',
#        'L1_DPHI-2EM3_VTE5.24ETA49_ALFA_EINE',
        'L1_HT150-JJ15.ETA49',
        'L1_HT150-JJ15.ETA49_MJJ-400',
        'L1_J4-MATCH',
        'L1_LLP-RO',
        'L1_LLP-NOMATCH',
        'L1_DR-MU10TAU12I',    
#        'L1_TAU12I-J25',    
        'L1_EM15TAU12I-J25',
        'L1_DR-EM15TAU12I-J25',    
        'L1_TAU20ITAU12I-J25',
        'L1_DR-TAU20ITAU12I',    
        'L1_DR-TAU20ITAU12I-J25',

        'L1_30M-EM20ITAU12',
        'L1_MJJ-400-CF',
    
        'L1_LAR-EM',
        'L1_LAR-J',
        'L1_LAR-ZEE',
        'L1_LAR-ZEE_LAR-EM',
        'L1_BPH-2M9-2MU4_BPH-0DR15-2MU4',
        'L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4',
        'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',
        'L1_BPH-8M15-2MU6_BPH-0DR22-2MU6',
        #'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4-BO',

        #ATR-19720
        'L1_BPH-8M15-0DR22-2MU6',
        'L1_BPH-2M9-2DR15-2MU6',
        'L1_BPH-2M9-0DR15-MU6MU4',
        #'L1_BPH-8M15-0DR22-MU6MU4-BO',
        'L1_BPH-2M9-0DR15-2MU4',

        'L1_BPH-0M9-EM7-EM5',        
        'L1_BPH-0DR3-EM7J15',
        'L1_BPH-0M9-EM7-EM5_MU6',
        'L1_BPH-0DR3-EM7J15_MU6',
        'L1_BPH-0M9-EM7-EM5_2MU4',
        'L1_BPH-0DR3-EM7J15_2MU4',

        # combined L1Topo for VBF
        
        # INVM + DPHI 
        'L1_MJJ-400-NFF-0DPHI20',
        'L1_MJJ-400-NFF-0DPHI22',
        'L1_MJJ-400-NFF-0DPHI24',
        'L1_MJJ-400-NFF-0DPHI26',
        #'L1_MJJ-400-0DPHI20',
        #'L1_MJJ-400-0DPHI22',
        #'L1_MJJ-400-0DPHI24',
        #'L1_MJJ-400-0DPHI26',
   
        ### ATR-15062

        'L1_LATE-MU10_XE50','L1_LATE-MU10_XE40', 'L1_LATE-MU10_J50',

        'L1_TAU60_DR-TAU20ITAU12I',

        'L1_SC111-CJ15',
        'L1_SC85-CJ15',
        
        ##     # Partition 1
        ##     'L1_CALREQ0_P1', 'L1_CALREQ1_P1', 'L1_CALREQ2_P1',

        #ATR-18815
        'L1_LFV-EM8I-MU11',
        'L1_LFV-EM12I-MU6',

        #ATR-19355
        'L1_BPH-0M10-3MU4',

        #ATR-18824
        'L1_ZAFB-04DPHI-EM15I',
        'L1_ZAFB-25DPHI-EM15I',
        # 'L1_ZAFB-25DPHI-EM18I',

        #ATR-19302: 
        #'L1_DPHI-M70-2EM10I',
        'L1_DPHI-M70-2EM12I',

        #ATR-19510
        'L1_DY-BOX-2MU4',
                
        #ATR-17320
        'L1_CEP-CJ60',
        'L1_CEP-CJ50' ,
        'L1_CEP-CJ50.ETA21',
        
        'L1_ALFA_ANY'

        ]
    









#CTP IDs are taken from this mapping. Every L1 item needs a unique ctpid.
# Run this file as python python/l1menu/Menu_MC_pp_v7.py to print out available IDs
# 463-464 are reserved for L1_RD2_BGRP14 and L1_RD3_BGRP15 (in MC_Physics_pp_v7)
# 509-511 are reserved for CALREQ
    
    L1MenuFlags.CtpIdMap = {
 
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
        'L1_2EM15' : 27,
        'L1_2EM15VH' : 28,
        'L1_EM7_2EM3' : 29,
        'L1_EM12_2EM3' : 30,
        'L1_EM15VH_3EM7' : 31,
        'L1_2EM20VH':400,
        'L1_EM20VH_3EM10VH':431, 
        'L1_2MU4' : 32,
        'L1_2MU6' : 33,
        'L1_2MU10' : 34,
        'L1_2MU11' : 183,
        'L1_2MU20_OVERLAY' : 35,
        'L1_MU10_2MU6' : 36,
        'L1_MU11_2MU6' : 37,
        'L1_MU11_2MU10' : 185,
        'L1_3MU4' : 38,
        'L1_MU6_2MU4' : 39,
        'L1_3MU6' : 40,
        'L1_4MU4' : 46,
        'L1_4J15.0ETA25' : 41,
        'L1_2EM8VH_MU10' : 43,
        'L1_EM15VH_MU10' : 44,
        'L1_EM7_MU20' : 483,
        'L1_EM8I_MU10' : 399,
        'L1_EM8VH_MU20' : 484,
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
        'L1_EM15VHI_TAU40_2TAU15' : 62,
        'L1_2TAU8' : 370,
        'L1_EM20VHI_TAU20IM_2TAU20_J25_3J20' : 69,
        'L1_MU10_TAU12IM' : 63,
        'L1_MU10_TAU12IM_J25_2J12' : 64,
        'L1_MU10_TAU12IM_3J12' : 482,
        'L1_EM7_MU10':65,
        'L1_TAU30_EMPTY':341,
        'L1_MU10_TAU20IM' : 67,
        'L1_MU11_TAU20IM' : 430,
        'L1_MU10_TAU20IM_J25_2J20' : 377,


        'L1_TAU20IM_2TAU12IM_J25_2J20_3J12' : 70,
        'L1_TAU20IM_2TAU12IM_4J12.0ETA23' : 316,
        'L1_DR-MU10TAU12I_TAU12I-J25' : 72,
        'L1_TAU20IM_2J20_XE45' : 74,
        'L1_TAU30_UNPAIRED_ISO' :343,
        'L1_TAU60_DR-TAU20ITAU12I' : 76,
        'L1_MU10_TAU12IM_XE35' : 81,
        'L1_TAU20IM_2TAU12IM_XE35' : 83,
        'L1_TAU40_2TAU12IM_XE40' : 429,
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
        
        'L1_J15.31ETA49' : 109,
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

        'L1_3J15' : 126,
        'L1_3J40' : 128,
        'L1_3J50' : 130,
        'L1_4J15' : 131,
        'L1_4J20' : 132,
        'L1_3J15.0ETA25_XE40' : 184,
        'L1_J45.0ETA20_3J15.0ETA25' : 374,
        'L1_J50_2J40.0ETA25_3J15.0ETA25' : 375,
 
#        'L1_3J75' : 133,
#        'L1_4J30' : 134,
        'L1_6J15' : 135,
        'L1_J85_3J30' : 480,
        'L1_J30.0ETA49_2J20.0ETA49' : 137,

       
        'L1_5J15.0ETA25' : 140,
        'L1_2J15_XE55' : 141,
        'L1_J40_XE50' : 142,
        #'L1_J75_XE40' : 143,

        #'L1_XE25': 82,
        'L1_XE10': 68,
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
        'L1_TE15' : 160,
        'L1_TE20' : 159,
        'L1_TE25' : 152,
        'L1_TE30' : 138,
        'L1_TE40' : 82,
        'L1_TE50' : 143,

        'L1_BCM_Wide_UNPAIRED_NONISO' : 161,
        'L1_BCM_AC_UNPAIRED_ISO' : 163,

        'L1_MBTS_1_EMPTY' : 164,

        'L1_EM20VH_3J20' : 26,
        'L1_MU10_3J20' : 173,

        'L1_2J50_XE40' : 175,
        'L1_J40_XE60' : 176,
        'L1_MU10_2J20' : 278,
        'L1_MU20_J50' : 198, 
        'L1_TAU60_2TAU40' : 458,
        'L1_MU10_2J15_J20' : 255,
        
        'L1_BPH-0M9-EM7-EM5' : 80,
        'L1_BPH-0DR3-EM7J15' : 84,
        'L1_BPH-0M9-EM7-EM5_MU6' : 124,
        'L1_BPH-0DR3-EM7J15_MU6' : 134,
        'L1_BPH-0M9-EM7-EM5_2MU4' : 153,
        'L1_BPH-0DR3-EM7J15_2MU4' : 156,

        'L1_J25.0ETA23_2J15.31ETA49': 335 , 
        'L1_J40.0ETA25_2J15.31ETA49' : 181,
        'L1_J40.0ETA25_2J25_J20.31ETA49' : 182,


         ## noid
        'L1_HT150-J20s5.ETA31_MJJ-400-CF': 273,
        
        'L1_RD0_FILLED' : 200,
        'L1_RD0_UNPAIRED_ISO' : 201,
        'L1_RD0_EMPTY' : 202,
        'L1_RD1_EMPTY' : 204,
        'L1_RD2_FILLED' : 205,
        'L1_TGC_BURST' : 220,
        'L1_BCM_AC_CA_BGRP0' : 223,
        'L1_BCM_Wide_EMPTY' : 224,
        'L1_BCM_Wide_UNPAIRED_ISO' : 225,
        
      
        'L1_BCM_CA_UNPAIRED_ISO' : 229,
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
        'L1_BTAG-MU6J20' : 243,
        'L1_J40_XE50_DPHI-J20s2XE50': 245,
        'L1_2MU4_J20_XE30_DPHI-J20s2XE30' : 271,

        'L1_2MU4_J40_XE50' : 449,
        'L1_MU4_J30_XE40_DPHI-J20s2XE30' : 452,
        'L1_MU4_J50_XE50_DPHI-J20s2XE30' : 453,
        'L1_MU4_XE60' : 454,
        
        'L1_HT190-J15.ETA21' : 258,
        'L1_HT190-J15s5.ETA21' : 259,
        'L1_HT150-J20.ETA31' : 260,
        'L1_HT150-J20s5.ETA31' : 261,
        'L1_JPSI-1M5-EM7' : 263,
        'L1_JPSI-1M5-EM12' : 264,

        'L1_MJJ-400-NFF-0DPHI20' : 266,
        'L1_MJJ-400-NFF-0DPHI22' : 267,
        'L1_MJJ-400-NFF-0DPHI24' : 268,
        'L1_MJJ-400-NFF-0DPHI26' : 269,
        'L1_MJJ-400-0DPHI20' : 270,
        'L1_MJJ-400-0DPHI22' : 272,
        'L1_MJJ-400-0DPHI24' : 274,
        'L1_MJJ-400-0DPHI26' : 276,
        'L1_EM22VH' : 295,
        'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6' : 300,
        'L1_BPH-8M15-2MU6_BPH-0DR22-2MU6'  : 301,
        'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4-BO' : 488,

        ### ATR-14350 - these should be replaced in 2017 with items 296 and 298
        'L1_BPH-2M9-2MU4_BPH-0DR15-2MU4' : 426,
        'L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4' : 427,

        'L1_DY-DR-2MU4' : 315,
        #'L1_DY-BOX-MU6MU4' : 317,
        'L1_DY-BOX-2MU6' : 318,
        'L1_LFV-MU6' : 332,
        'L1_LFV-MU11' : 364,
        'L1_LFV-EM8I' : 320,
        'L1_LFV-EM15I' : 321,
        'L1_DPHI-J20s2XE50' : 323,
        'L1_MJJ-700' : 216,
        'L1_MJJ-800' : 327,
        'L1_MJJ-400' : 329,
        'L1_MJJ-100' : 333,
        'L1_MJJ-500-NFF' : 108,
        'L1_MJJ-400-NFF' : 129,
        'L1_J50_DETA20-J50J' : 275,
        'L1_DPHI-2EM3' : 288, 
        'L1_HT150-JJ15.ETA49' : 334,
        'L1_J4-MATCH' : 336,
        'L1_LLP-RO' : 338,
        'L1_LLP-NOMATCH' : 339,
        'L1_DR-MU10TAU12I' : 340,
        'L1_EM15TAU12I-J25' : 345,
        'L1_DR-EM15TAU12I-J25' : 346,
        'L1_TAU20ITAU12I-J25' : 347,
        'L1_DR-TAU20ITAU12I' : 348,
        'L1_DR-TAU20ITAU12I-J25' : 350,


        'L1_30M-EM20ITAU12' : 387,
        'L1_MJJ-400-CF': 397,

        'L1_LAR-EM' : 351,
        'L1_LAR-J' : 352,
        'L1_LAR-ZEE' : 71,
        'L1_LAR-ZEE_LAR-EM' : 285,
        'L1_LATE-MU10_XE50': 354,
        'L1_LATE-MU10_XE40': 466,
        'L1_LATE-MU10_J50' : 355,
        'L1_SC111-CJ15' : 356,
        'L1_SC85-CJ15' : 357,

        'L1_EM7_FIRSTEMPTY': 371, 
        'L1_RD0_ABORTGAPNOTCALIB':372,
        'L1_3J25.0ETA23' : 373,
        'L1_3J35.0ETA23' : 425,
        'L1_MU6_J30.0ETA49_2J20.0ETA49' : 382,
        'L1_4J20.0ETA49' : 383,
        'L1_HT150-JJ15.ETA49_MJJ-400' : 416,

        ### ATR-15062
        
        
        'L1_TAU8_UNPAIRED_ISO': 388,
        'L1_EM7_UNPAIRED_ISO': 389,
        'L1_TAU8_FIRSTEMPTY': 391,
        'L1_EM24VHI'           : 392,
        'L1_MU6_3MU4'  :403,
        'L1_2MU6_3MU4' :404,
        #
        'L1_EM20VH_FIRSTEMPTY':411, 
        'L1_J100_FIRSTEMPTY':414, 



        
        #ATR-19302
        #'L1_EM30VHI' : 188,
        #ATR-19355
        'L1_BPH-0M10-3MU4' : 189,

        #ATR-18815
        'L1_LFV-EM8I-MU11' : 190,
        'L1_LFV-EM12I-MU6' : 191,

        #ATR-18824
        'L1_ZAFB-04DPHI-EM15I' : 192,
        'L1_ZAFB-25DPHI-EM15I' : 193,
        'L1_ZAFB-25DPHI-EM18I' : 194,

        #ATR-19302: 
        'L1_DPHI-M70-2EM10I' : 195,
        'L1_DPHI-M70-2EM12I' : 196,

        #ATR-19720
        'L1_BPH-8M15-0DR22-2MU6' : 299,
        'L1_BPH-2M9-2DR15-2MU6' : 302,
        'L1_BPH-2M9-0DR15-MU6MU4' : 303,
        'L1_BPH-8M15-0DR22-MU6MU4-BO' : 488,
        'L1_BPH-2M9-0DR15-2MU4' : 487,

        #ATR-19510
        'L1_DY-BOX-2MU4' : 199,
        
        # LUCID
        'L1_LUCID' : 212,
        'L1_LUCID_EMPTY' : 213,
        'L1_LUCID_UNPAIRED_ISO' : 214,
        'L1_LUCID_BGRP9' : 471,
        'L1_LUCID_BGRP11' : 472,

        # TRT
        'L1_TRT_FILLED' : 218,
        'L1_TRT_EMPTY' : 219,

        # new items
        'L1_gXERHO20' : 239,
        'L1_gXERHO50' : 241,
        'L1_gXEPUFIT20' : 242,
        'L1_gXEPUFIT50' : 244,
        'L1_gXE50' : 257,
        'L1_jXE50' : 265,

        'L1_eEM15VHI_2eTAU12IM_jJ25_3jJ12' : 274,
        'L1_eTAU12' : 277,
        'L1_eTAU100' : 283,

        'L1_jJ100' : 284,
        'L1_2jJ50_gXERHO40' : 287,
        'L1_3jJ15.0ETA25_gXERHO40' : 289,
        'L1_3jJ35.0ETA23' : 296,
        'L1_3jJ50' : 297,
        'L1_4jJ15' : 298,
        'L1_4jJ15.0ETA25' : 304,
        'L1_jJ25.0ETA23_2jJ15.31ETA49' : 306,
        'L1_jJ40.0ETA25_2jJ25_jJ20.31ETA49' : 307,
        'L1_jJ75.31ETA49' : 308,
        'L1_jJ85_3jJ30' : 310,

        'L1_eEM3': 311, 
        'L1_eEM8': 312, 
        'L1_eEM10': 313, 
        'L1_eEM15': 314, 
        'L1_eEM20': 324,
        'L1_2eEM8': 325,         
        'L1_2eEM10': 326,         
        'L1_eEM22' : 328,
        'L1_2eEM15VHI' : 349,
        'L1_2eEM20VH' : 359,
        'L1_2eEM8VH_MU10' : 417,
        'L1_eEM15VHI_2eTAU12IM_4jJ12' : 462,
        'L1_eEM15VHI_2eTAU12IM_gXERHO35' : 477,
        'L1_eEM15VH_MU10' : 478,
        'L1_eEM18VHI_3jJ20' : 489,
        # 'L1_eEM20VHI_eTAU20IM_2eTAU20_jJ25_3jJ20' :  490,
        'L1_ALFA_ANY' : 490,
        'L1_eEM20VH_3eEM10VH' : 491,
        'L1_eEM22VHI' : 492,
        'L1_eEM20VHI' : 493, 
        'L1_eEM20VH' : 494, 
        'L1_eEM7_MU20' : 495,

        'L1_MU10_2jJ15_jJ20' : 496,
        'L1_MU10_2jJ20' : 497,
        'L1_MU10_eTAU12IM_3jJ12' : 498,
        'L1_MU10_eTAU12IM_jJ25_2jJ12' : 499,
        'L1_MU10_eTAU12IM_gXERHO35' : 500,
        'L1_MU10_eTAU20IM' : 501,
        'L1_MU20_jJ40' : 502,
        'L1_MU20_gXERHO30' : 503,
        'L1_eTAU20IM_2jJ20_gXERHO45' : 504,
        'L1_eTAU20IM_2eTAU12IM_4jJ12.0ETA23' : 505,
        'L1_eTAU25IM_2eTAU20IM_2jJ25_3jJ20' : 506,
        'L1_eTAU40_2eTAU12IM_gXERHO40' : 507,
        'L1_eTAU60_2eTAU40' : 508,

        # RNDM
        'L1_RD0_UNPAIRED_NONISO' : 178,
        'L1_RD0_BGRP9' : 210,
        'L1_RD0_BGRP10' : 473,
        'L1_RD0_BGRP11' : 211,
        'L1_RD1_FILLED' : 203,
        'L1_RD1_BGRP10' : 188,
        'L1_RD2_EMPTY' : 206,
        'L1_RD2_BGRP12' : 390,
        'L1_RD3_FILLED' : 207,
        'L1_RD3_EMPTY' : 208,
        'L1_RD2_BGRP14' : 463,
        'L1_RD3_BGRP15' : 464,

        'L1_CALREQ2' : 511,  # never use 509-511 for anything else than CALREQ triggers

    }



