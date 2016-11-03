# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def defineMenu():
    """
    MC and Physics menu are identical
    """
    
    import Menu_MC_pp_v6
    from TriggerMenu.l1.Lvl1Flags import Lvl1Flags

    Menu_MC_pp_v6.defineMenu()

    Lvl1Flags.thresholds += [
        ]

    Lvl1Flags.items += [
        # for running high rate tests in secondary CTP partitions
        'L1_RD2_BGRP14', 'L1_RD3_BGRP15',
        ]

    #---------------------------------
    # L1menu in MCppV5 is basis 
    # BeamSplashes: adapt some thresholds and items
    #---------------------------------
    Lvl1Flags.ThresholdMap = {

#        '15MINDPHI-EM15s6-XE0':'',
#        '35MT-EM12s6-XE0':'',
#        'DISAMB-0DR28-EM15his2-TAU12abi':'',
#        'LATE-MU10s1':'',        
#        'SC111-CJ15ab.ETA26':'',
#        'SC85-CJ15ab.ETA26':'',
#        '10MINDPHI-AJ20s2-XE50': '',

#        '2INVM9-2MU4ab': '', 
#        '2INVM9-MU6ab-MU4ab': '',

#        '0DR22-2MU6ab':'',
#        '7INVM15-2MU4ab' :'',
#        '0DR22-MU6ab-MU4ab' :'',
#        '0DR15-2MU4ab':'',
#        '0DR24-2MU4ab':'',
#        '0DR15-2MU6ab':'',
 
#       '15MINDPHI-EM15s6-XE0':'',
#        '15MINDPHI-EM12s6-XE0':'',
#        '10MINDPHI-J20s2-XE30':'',
#        '0DR25-TAU20abi-TAU12abi':'',
#        '1DISAMB-J25ab-0DR25-TAU20abi-TAU12abi':'',
#        'DISAMB-30INVM-EM20his2-TAU12ab':'',
#        '400INVM9999-AJ30s6.ETA31-AJ20s6.31ETA49':'',

#        'EM7'       : 'EM3HI',
#        'EM8VH'     : 'EM7',
#        'EM8I'      : 'EM8VH',
#        'EM10'      : 'EM8I',
#        'EM10VH'    : 'EM10VH',
#        'EM12'      : 'EM12',
#        'EM13VH'    : 'EM15',        
#        'EM15'      : 'EM15VH',
#        'EM15VH'    : 'EM15I',
#        'EM15I'     : 'EM15HI',
#        'EM15HI'    : 'EM18VH',
#        'EM18VH'    : 'EM20A',
#        'EM20VH'    : 'EM20C',
#        'EM20VHI'   : 'EM20VH',
#        'EM22VHI'   : 'EM20VHI',
#
#
#        'J75'       : 'J75A',    
#        'J85'       : 'J75C',
#        'J100'      : 'J85',         
#        'J120'      : 'J100',         
#        
#        'HA20'      : 'HA20A', 
#        'HA20IL'    : 'HA20C', 
#        'HA20IM'    : 'HA20', 
#        'HA20IT'    : 'HA20IL', 
#        'HA25'      : 'HA20IM', 
#        'HA25IT'    : 'HA20IT', 
#        'HA30'      : 'HA25', 
#        'HA40'      : 'HA25IT', 
#        'HA60'      : 'HA40', 
				#'2INVM9-2MU4ab':'2INVM8-2MU4ab',
        #'2INVM9-MU6ab-MU4ab':'2INVM8-MU6ab-MU4ab', 
#        'KF-XE40-AJall':'KF-XE35-AJall',
#        'KF-XE50-AJall':'KF-XE45-AJall',
#        '1INVM5-EMs1-EMs6'  :'1INVM5-EMs1-EMall',
#        '1INVM5-EM7s1-EMs6' :'1INVM5-EM7s1-EMall',
#        '1INVM5-EM12s1-EMs6':'1INVM5-EM12s1-EMall',
#        '100RATIO-0MATCH-TAU30si2-EMall': '210RATIO-0MATCH-TAU30si2-EMall',

        }

    Lvl1Flags.ItemMap = {

#        'L1_LFV-MU6':'',
#        'L1_W-15DPHI-EMXE-1':'',
#        'L1_W-15DPHI-EM15XE-1':'',
#        'L1_J40_DPHI-J20s2XE30':'',
#        'L1_J40_XE50_DPHI-J20s2XE50':'',
#        'L1_MU4_J20_XE30_DPHI-J20s2XE30':'',
#        'L1_2MU4_J20_XE30_DPHI-J20s2XE30':'',
#        'L1_DR25-TAU20ITAU12I':'',
#        'L1_DR25-TAU20ITAU12I-J25':'',
#        'L1_30M-EM20ITAU12':'',
#        'L1_MJJ-400-CF':'',

#        'L1_DPHI-AJ20s2XE50':'', 
#        'L1_EM12_W-MT35_XS40_W-05DPHI-JXE-0_W-05DPHI-EMXE':'', 
#        "L1_EM12_W-MT35":'', 
#        "L1_EM12_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EMXE":'', 
#        "L1_EM12_W-MT35_XS50":'',
        'L1_EM12_W-MT35_W-90RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EMX':'',
#        "L1_EM12_W-MT35_W-90RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EMXE":'',
#        "L1_EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE":'',
#        "L1_EM15_W-MT35_XS40_W-15DPHI-JXE-0_W-15DPHI-EM15XE":'', 
#        "L1_EM15_W-MT35_XS60_W-15DPHI-JXE-0_W-15DPHI-EM15XE":'',
#        "L1_EM10VH_W-MT35_XS50":'', 
#        "L1_W-15DPHI-EM15XE-0":'',

#        "L1_EM12_W-MT35_XS30_W-15DPHI-JXE-0_W-15DPHI-EMXE":'', 
#        "L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0":'', 
#        "L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS30":'', 
#        "L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20":'', 
#        "L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE":'',

#        'L1_DR-EM15TAU12I':'', 
#        'L1_LATE-MU10_XE50':'', 
#        'L1_LATE-MU10_J50':'', 
#        'L1_SC111':'', 
#        'L1_SC85':'',

#        'L1_BPH-8M15-2MU6_BPH-0DR22-2MU6':'',
#        'L1_BPH-7M15-2MU4_BPH-0DR24-2MU4':'',
#        'L1_BPH-7M15-2MU4_BPH-0DR24-2MU4-B':'',
#        'L1_BPH-7M15-2MU4_BPH-0DR24-2MU4-BO':'',
#        'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4':'',
#        'L1_BPH-2M8-2MU4_BPH-0DR15-2MU4':'',
        #'L1_BPH-2M9-2MU4_BPH-0DR15-2MU4':'',
#        'L1_BPH-2M8-2MU4-B_BPH-0DR15-2MU4':'',
        #'L1_BPH-2M8-2MU4-BO_BPH-0DR15-2MU4':'',
#        'L1_BPH-7M15-2MU4_BPH-0DR24-2MU4':'',

#        'L1_HT150-J20s5.ETA31_MJJ-400':'',
#        'L1_HT150-J20s5.ETA31_MJJ-400-CF':'',
        'L1_J40.0ETA25_2J30_J20.31ETA49':'',
#        'L1_EM22VHI' : 'L1_EM20A' , #eta [1.6,2.].
#        'L1_EM13VH'    : 'L1_EM20C',     #eta[-2,-1.6] 
#        'L1_J75'       : 'L1_J75A',         #[1.6,2.4]
#        'L1_J120'      : 'L1_J75C',         #[-2.4,-1.6]  
#        'L1_EM10'      : 'L1_EM3HI',
#        'L1_TAU30'     : 'L1_TAU20A', 
#        'L1_TAU60'     : 'L1_TAU20C', 
#        
#        'L1_2EM13VH' : '',
#        'L1_3J75' : '',
#        'L1_J75_3J20' : '',
#        'L1_J75_XE40' :'',
#        'L1_MU6_J75': '',
#        'L1_KF-XE40':'L1_KF-XE35',
#        'L1_KF-XE50':'L1_KF-XE45',
        #'L1_BPH-2M9-2MU4':'L1_BPH-2M8-2MU4',
        #'L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4':'L1_BPH-2M8-MU6MU4_BPH-0DR15-MU6MU4',
        }
    
    #----------------------------------------------
    def remapThresholds():
        threshToRemove = []
        for index, thresholdName in enumerate(Lvl1Flags.thresholds()):
            if thresholdName in Lvl1Flags.ThresholdMap():
                if (Lvl1Flags.ThresholdMap()[thresholdName] != ''):
                    Lvl1Flags.thresholds()[index] = Lvl1Flags.ThresholdMap()[thresholdName]
                else:
                    threshToRemove.append(index)

        for t in reversed(threshToRemove):
            del Lvl1Flags.thresholds()[t]
    #----------------------------------------------
                    
    remapThresholds()

    #----------------------------------------------
    def remapItems():  
        itemsToRemove = []
        for itemIndex, itemName in enumerate(Lvl1Flags.items()):
            if (itemName in Lvl1Flags.ItemMap()):
                if (Lvl1Flags.ItemMap()[itemName] != ''):
                    Lvl1Flags.items()[itemIndex] = Lvl1Flags.ItemMap()[itemName]                                                
                else: 
                    itemsToRemove.append(itemIndex)

        for i in reversed(itemsToRemove):
            del Lvl1Flags.items()[i]
    #----------------------------------------------
                                           
    remapItems()
