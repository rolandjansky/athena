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
        'L1_eEM3', 'L1_eEM8', 'L1_eEM15', 'L1_eEM20',
        'L1_eEM22', 'L1_eEM22M',

        ## 
        # MU
        ##
        'L1_MU3V', 'L1_MU5VF', 'L1_MU8F', 'L1_MU8VF', 'L1_MU14FCH', 'L1_MU14FCHR',
        'L1_MU3VF', 'L1_MU8FC', 'L1_MU15VFCH', 'L1_MU10BOM',
        'L1_2MU3V', 'L1_2MU5VF', 'L1_2MU8F', 'L1_MU8VF_2MU5VF', 'L1_MU5VF_2MU3V',
        'L1_3MU3V', 'L1_3MU5VF', 'L1_MU5VF_3MU3V', 'L1_4MU3V',
        'L1_2MU5VF_3MU3V', 'L1_2MU8VF',

        'L1_2MU14FCH_OVERLAY',
        'L1_MU3V_EMPTY', 'L1_MU5VF_EMPTY', 'L1_MU3V_FIRSTEMPTY', 'L1_MU8VF_EMPTY',
        'L1_MU3V_UNPAIRED_ISO',

        ##
        # combined lepton (e and mu)
        ##
        'L1_2EM3', 'L1_2EM7', 'L1_2EM15', 'L1_2EM15VH', 'L1_2EM15VHI',
        'L1_2EM20VH', 'L1_EM20VH_3EM10VH',
        'L1_EM7_MU8F', 'L1_2EM8VH_MU8F', 'L1_EM15VH_MU8F', 'L1_EM7_MU14FCH',

        # combined em - jet
        'L1_EM18VHI_3J20',
        'L1_EM20VH_3J20',
        
        # combined mu - jet
        'L1_MU3V_J15', 

        #ATR-13743 
        'L1_MU8F_2J20','L1_MU8F_3J20', 'L1_MU8F_2J15_J20',
        'L1_MU14FCH_J40',
        'L1_MU14FCH_XE30',
        'L1_MU14FCH_J50',

        # single jet
        'L1_J15','L1_J20','L1_J25', 'L1_J30', 'L1_J40', 'L1_J50' ,'L1_J75','L1_J85', 'L1_J100', 'L1_J400',
        'L1_J400_LAR',
        'L1_J20p31ETA49', 'L1_J30p31ETA49', 'L1_J50p31ETA49', 'L1_J75p31ETA49', 'L1_J15p31ETA49',
        'L1_J12_EMPTY','L1_J12_FIRSTEMPTY', 'L1_J12_UNPAIRED_ISO', 'L1_J12_UNPAIRED_NONISO', 'L1_J12_ABORTGAPNOTCALIB',
        'L1_J15p31ETA49_UNPAIRED_ISO',
        'L1_J30_EMPTY', 'L1_J30_FIRSTEMPTY', 'L1_J30p31ETA49_EMPTY', 'L1_J30p31ETA49_UNPAIRED_ISO', 'L1_J30p31ETA49_UNPAIRED_NONISO',
        'L1_J50_UNPAIRED_ISO', 'L1_J50_UNPAIRED_NONISO', 'L1_J50_ABORTGAPNOTCALIB',         
        'L1_J100_FIRSTEMPTY',
        'L1_J12_BGRP12',

       
        # multi jet
        'L1_J45p0ETA21_3J15p0ETA25',
        'L1_J50_2J40p0ETA25_3J15p0ETA25',
        'L1_3J50', 'L1_4J15', 'L1_4J20',
        'L1_3J15p0ETA25_XE40',
        'L1_6J15',
        'L1_J85_3J30', 

        # multi jet forward
        'L1_J25p0ETA23_2J15p31ETA49',
        'L1_J40p0ETA25_2J15p31ETA49',
        'L1_J40p0ETA25_2J25_J20p31ETA49',
        
        # multi jet central
        'L1_3J25p0ETA23',
        'L1_3J35p0ETA23',
        'L1_4J15p0ETA25',
        'L1_5J15p0ETA25', 

        # combined jet
        'L1_2J15_XE55', 'L1_J40_XE50',
        'L1_2J50_XE40', 'L1_J40_XE60',
        
        # XE
        'L1_XE35', 'L1_XE40', 'L1_XE45', 'L1_XE50', 
        'L1_XE55', 'L1_XE60', 'L1_XE30', 'L1_XE300',
                
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

        # VDM

        # TRT
        'L1_TRT_FILLED', 'L1_TRT_EMPTY',

        # TGC
        'L1_TGC_BURST',

        # LHCF
    
        #CALREQ
        'L1_CALREQ1',
        'L1_CALREQ2',

        # ZB
        'L1_ZB',

        # BPTX
        
        # BCM
        'L1_BCM_AC_CA_BGRP0', 'L1_BCM_Wide_EMPTY','L1_BCM_Wide_UNPAIRED_ISO','L1_BCM_Wide_UNPAIRED_NONISO',
        'L1_BCM_AC_UNPAIRED_ISO','L1_BCM_CA_UNPAIRED_ISO',
        'L1_BCM_AC_UNPAIRED_NONISO','L1_BCM_CA_UNPAIRED_NONISO',
        'L1_BCM_AC_ABORTGAPNOTCALIB', 'L1_BCM_CA_ABORTGAPNOTCALIB','L1_BCM_Wide_ABORTGAPNOTCALIB',
        'L1_BCM_AC_CALIB', 'L1_BCM_CA_CALIB','L1_BCM_Wide_CALIB',

        # AFP

        # MBTS
        'L1_MBTS_1_EMPTY', 'L1_MBTS_1_1_EMPTY', 'L1_MBTS_2_EMPTY',

        #--------------------------------
        # TOPO items
        #--------------------------------

        'L1_LAR-ZEE', 'L1_LAR-ZEE-eEM',



                
        #ATR-17320
        'L1_CEP-CjJ60',
        'L1_CEP-CjJ50' ,

        #ATR-21371
        'L1_ALFA_ANY',
        'L1_ALFA_ELAST15', 'L1_ALFA_ELAST18',
        'L1_ALFA_B7L1U','L1_ALFA_B7L1L','L1_ALFA_A7L1U','L1_ALFA_A7L1L','L1_ALFA_A7R1U','L1_ALFA_A7R1L','L1_ALFA_B7R1U','L1_ALFA_B7R1L',
        'L1_ALFA_SYST9', 'L1_ALFA_SYST10', 'L1_ALFA_SYST11', 'L1_ALFA_SYST12', 'L1_ALFA_SYST17', 'L1_ALFA_SYST18',

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
        'L1_EM15' : 6,
        'L1_EM15VH' : 7,
        'L1_EM18VHI' : 8,
        'L1_EM20VH' : 9,
        'L1_EM20VHI' : 10,
        'L1_EM22VHI' : 11,
        'L1_EM3_EMPTY' : 12,
        'L1_EM7_EMPTY' : 13,
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
        'L1_J20p31ETA49' : 103,
        'L1_J30p31ETA49' : 104,
        'L1_J50p31ETA49' : 105,
        'L1_J75p31ETA49' : 106,       
        'L1_J15p31ETA49' : 109,

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

        'L1_3J40' : 128,
        'L1_3J50' : 130,
        'L1_4J15' : 131,
        'L1_4J20' : 132,
        'L1_3J15p0ETA25_XE40' : 184,
        'L1_J45p0ETA21_3J15p0ETA25' : 86,
        'L1_J50_2J40p0ETA25_3J15p0ETA25' : 87,

        'L1_6J15' : 135,
        'L1_J85_3J30' : 480,
        #'L1_J30p0ETA49_2J20p0ETA49' : 137,

        'L1_5J15p0ETA25' : 140,
        'L1_2J15_XE55' : 141,
        'L1_J40_XE50' : 142,

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
        'L1_TE30' : 138,

        'L1_BCM_Wide_UNPAIRED_NONISO' : 161,
        'L1_CEP-CJ60' : 162,
        'L1_BCM_AC_UNPAIRED_ISO' : 163,
        'L1_CEP-CJ50' : 80,
        'L1_MBTS_1_EMPTY' : 164,
        'L1_MBTS_1_1_EMPTY' : 168,
        'L1_MBTS_2_EMPTY' : 166,
        'L1_EM20VH_3J20' : 26,
        'L1_EM18VHI_3J20' : 172,
        'L1_2J50_XE40' : 175,
        'L1_J40_XE60' : 176,
        'L1_J25p0ETA23_2J15p31ETA49': 335 , 
        'L1_J40p0ETA25_2J15p31ETA49' : 181,
        'L1_J40p0ETA25_2J25_J20p31ETA49' : 182,

        ## noid
        'L1_RD0_FILLED' : 200,
        'L1_RD0_UNPAIRED_ISO' : 201,
        'L1_RD0_EMPTY' : 202,
        'L1_RD0_FIRSTEMPTY' : 209,
        'L1_RD1_EMPTY' : 204,
        'L1_RD2_FILLED' : 205,
        'L1_RD2_EMPTY' : 206,
        'L1_RD3_FILLED' : 207,
        'L1_RD3_EMPTY' : 208,
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

        'L1_ZB' : 240,

        'L1_HT190-J15pETA21' : 258,
        'L1_HT190-J15s5pETA21' : 259,
        'L1_HT150-J20pETA31' : 260,
        'L1_JPSI-1M5-EM7' : 263,
        'L1_JPSI-1M5-EM12' : 264,
        'L1_MJJ-400-0DPHI20' : 270,
        'L1_MJJ-400-0DPHI22' : 272,
        'L1_MJJ-400-0DPHI24' : 274,
        'L1_MJJ-400-0DPHI26' : 276,
        'L1_EM22VH' : 295,

        ### ATR-14350 - these should be replaced in 2017 with items 296 and 298
        'L1_LFV-EM8I' : 320,
        'L1_LFV-EM15I' : 321,
        'L1_MJJ-800' : 327,
        'L1_MJJ-400' : 329,
        'L1_MJJ-100' : 333,
        'L1_J50_DETA20-J50J' : 275,
        'L1_DPHI-2EM3' : 288, 
        'L1_HT150-JJ15pETA49' : 334,
        'L1_J4-MATCH' : 336,
        'L1_LLP-RO' : 338,
        'L1_LLP-NOMATCH' : 339,

        'L1_LAR-EM' : 351,
        'L1_LAR-J' : 352,
        'L1_LAR-ZEE' : 71,
        'L1_LAR-ZEE_LAR-EM' : 285,
        'L1_SC111-CJ15' : 356,
        'L1_SC85-CJ15' : 357,

        'L1_EM7_FIRSTEMPTY': 371, 
        'L1_RD0_ABORTGAPNOTCALIB':372,
        'L1_3J25p0ETA23' : 373,
        'L1_3J35p0ETA23' : 425,
        #'L1_4J20p0ETA49' : 383,
        'L1_HT150-JJ15pETA49_MJJ-400' : 416,

        ### ATR-15062
        
        'L1_EM7_UNPAIRED_ISO': 389,
        'L1_EM24VHI'           : 392,
        #
        'L1_EM20VH_FIRSTEMPTY': 411, 
        'L1_J100_FIRSTEMPTY': 414, 
        
        # LUCID
        'L1_LUCID' : 212,
        'L1_LUCID_EMPTY' : 213,
        'L1_LUCID_UNPAIRED_ISO' : 214,
        'L1_LUCID_BGRP9' : 471,
        'L1_LUCID_BGRP11' : 472,

        # TRT
        'L1_TRT_FILLED' : 218,
        'L1_TRT_EMPTY' : 219,

        # ALFA
        'L1_ALFA_A7L1L': 492,
        'L1_ALFA_A7L1U': 491,
        'L1_ALFA_A7R1L': 494,
        'L1_ALFA_A7R1U': 493,
        'L1_ALFA_ANY'  : 490,
        'L1_ALFA_B7L1L': 497,
        'L1_ALFA_B7L1U': 489,
        'L1_ALFA_B7R1L': 496,
        'L1_ALFA_B7R1U': 495,


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

        # NB: 508 is reserved for the zero bias trigger, and 509-511 for the CALREQ triggers (at the moment, ATR-22654)

    }

if __name__ == "__main__": print_available()
