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



# Run this file as python python/l1menu/Menu_MC_pp_v7.py to print out available IDs
# CTP IDs 509-511 are reserved for CALREQ
    
    L1MenuFlags.CtpIdMap = {
 

        # NB: 508 is reserved for the zero bias trigger, and 509-511 for the CALREQ triggers (at the moment, ATR-22654)

    }

if __name__ == "__main__": print_available()
