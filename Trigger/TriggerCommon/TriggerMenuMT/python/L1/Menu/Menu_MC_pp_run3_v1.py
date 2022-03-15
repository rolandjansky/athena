# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.L1.Base.L1MenuFlags import L1MenuFlags
import TriggerMenuMT.L1.Menu.Menu_Physics_pp_run3_v1 as physics_menu

def defineMenu():

    physics_menu.defineMenu()


    # Add new items to the item list in the Physics menu
    l1items = L1MenuFlags.items()
    l1items += [

        'L1_MU14FCH_jJ80',
        'L1_MU14FCH_jXE70',

        # ATR-24037 
        'L1_jXEPerf100',

        # jLJ, Production thresholds, not used in commissioning
        'L1_jLJ60', 'L1_jLJ100', 'L1_jLJ160', 'L1_jLJ200',

        # TOPO
        'L1_BTAG-MU5VFjJ90',
        'L1_BPH-0M9-EM7-EM5',
        'L1_BPH-0DR3-EM7J15',
    ]

    # To replace thresholds in the physics menu
    # Do not use for L1Topo decision threshold!
    L1MenuFlags.ThresholdMap = {
        #example: 'jXE100' :'',
    }

    # To replace items in the physics menu
    L1MenuFlags.ItemMap = {

        # non-primary EM
        'L1_EM8VH':'', 
        'L1_EM18VHI':'',
        'L1_EM20VHI':'',
        'L1_2EM7':'',
        'L1_EM20VHI':'',

        # non-primary eEM
        'L1_eEM7':'',
        'L1_eEM10L':'',
        'L1_eEM15':'',
        'L1_eEM18':'',
        'L1_eEM22M':'',
        'L1_eEM24VM':'',
        'L1_3eEM12L':'',

        # non-primary TAU
        'L1_jTAU20':'',
        'L1_jTAU30':'',
        'L1_jTAU30M':'',
        'L1_eTAU20L':'',
        'L1_eTAU35':'',
        'L1_eTAU40HM':'',
        'L1_2TAU8':'',
        'L1_EM15VHI_2TAU12IM':'',
        'L1_MU8F_TAU12IM':'',
        'L1_MU8F_TAU12IM_J25_2J12':'',
        'L1_EM15VHI_2TAU12IM_J25_3J12':'',
        'L1_MU8F_TAU20IM_J25_2J20':'',
        'L1_TAU20IM_2TAU12IM_J25_2J20_3J12':'',
        'L1_TAU20IM_2J20_XE45':'',
        'L1_TAU20IM_2J20_XE50':'',
        'L1_TAU20IM_2TAU12IM_XE35':'',

        # non-primary MU 
        'L1_MU8VF':'',
        'L1_MU14FCHR':'',
        'L1_MU3VF':'',
        'L1_MU8FC':'', 
        'L1_MU15VFCH':'',
        'L1_2MU8VF':'',
        'L1_2MU14FCH_OVERLAY':'',
        'L1_MU3VC':'',
        'L1_MU4BO':'',
        'L1_MU3EOF':'',
        'L1_MU8FH':'',
        'L1_MU8EOF':'',
        'L1_MU9VF':'',
        'L1_MU9VFC':'',
        'L1_MU12FCH':'',
        'L1_MU14EOF':'',
        'L1_MU15VFCHR':'',
        'L1_MU18VFCH':'',
        'L1_MU20VFC':'',

        # non-primary J
        'L1_J12':'',
        'L1_J25':'',
        'L1_J85':'',
        'L1_J12_BGRP12':'',
        'L1_jJ30p0ETA25':'',
        'L1_jJ40p0ETA25':'',
        'L1_jJ55':'',
        'L1_jJ55p0ETA23':'',
        'L1_jJ70p0ETA23':'',
        'L1_jJ80':'',
        'L1_jJ80p0ETA25':'',
        'L1_jJ85p0ETA21':'',
        'L1_jJ140':'',
        'L1_jJ180':'',
        'L1_jLJ180':'',

        # other non-primary
        'L1_jEM20':'',
        'L1_jEM20M':'',
        'L1_gJ20':'',
        'L1_gJ30':'',
        'L1_gJ40':'',
        'L1_gJ50':'',
        'L1_gJ100':'',

        # combined non-primary
        'L1_MU8F_2J20':'',
        'L1_MU8F_3J20':'',

        # MU non-FILLED
        'L1_MU3V_FIRSTEMPTY':'', 
        'L1_MU8VF_EMPTY':'',
        'L1_MU14FCH_EMPTY':'',
        'L1_MU14FCH_UNPAIRED_ISO':'',

        # EM non-FILLED
        'L1_EM3_EMPTY':'', 
        'L1_EM7_FIRSTEMPTY':'',
        'L1_EM20VH_FIRSTEMPTY':'',

        # TAU non-FILLED
        'L1_TAU8_EMPTY':'', 
        'L1_TAU8_FIRSTEMPTY':'', 
        'L1_TAU8_UNPAIRED_ISO':'', 
        'L1_TAU40_EMPTY':'', 
        'L1_TAU40_UNPAIRED_ISO':'',

        # J non-FILLED
        'L1_J12_EMPTY':'',
        'L1_J12_FIRSTEMPTY':'', 
        'L1_J12_UNPAIRED_ISO':'', 
        'L1_J12_UNPAIRED_NONISO':'', 
        'L1_J12_ABORTGAPNOTCALIB':'',
        'L1_J15p31ETA49_UNPAIRED_ISO':'',
        'L1_J30p31ETA49_EMPTY':'', 
        'L1_J30p31ETA49_UNPAIRED_ISO':'', 
        'L1_J30p31ETA49_UNPAIRED_NONISO':'',
        'L1_J50_UNPAIRED_ISO':'', 
        'L1_J50_UNPAIRED_NONISO':'', 
        'L1_J50_ABORTGAPNOTCALIB':'',
        'L1_J100_FIRSTEMPTY':'',

        # Others
        'L1_J400_LAR':'',
        'L1_jJ500_LAR':'',

        'L1_TRT_EMPTY':'',
        'L1_TRT_FILLED':'',

        'L1_RD0_UNPAIRED_ISO':'',
        'L1_RD0_ABORTGAPNOTCALIB':'',
        'L1_RD0_FIRSTEMPTY':'', 
        'L1_RD0_BGRP11':'',
        'L1_RD0_BGRP7':'',
        'L1_RD1_EMPTY':'',
        'L1_RD2_EMPTY':'',
        'L1_RD2_FILLED':'',
        'L1_RD3_EMPTY':'',
        'L1_RD3_FILLED':'',

        'L1_TGC_BURST':'',

        'L1_ZDC_A':'', 
        'L1_ZDC_C':'', 
        'L1_ZDC_AND':'',

        'L1_LUCID_A':'', 
        'L1_LUCID_C':'',

        'L1_BPTX0_BGRP12':'',
        'L1_BPTX1_BGRP12':'',

        'L1_CALREQ1':'',
        'L1_CALREQ2':'',

        'L1_MBTS_A':'',
        'L1_MBTS_C':'',
        'L1_MBTS_1_EMPTY':'',
        'L1_MBTS_1_1_EMPTY':'',
        'L1_MBTS_2_EMPTY':'',
        'L1_MBTS_1_UNPAIRED_ISO':'',
        'L1_MBTS_1_1_UNPAIRED_ISO':'',
        'L1_MBTS_2_UNPAIRED_ISO':'',
        'L1_MBTS_1':'',
        'L1_MBTS_1_1':'',
        'L1_MBTS_2':'',
        'L1_MBTS_4_A':'',
        'L1_MBTS_4_C':'',
        'L1_MBTS_1_A':'',
        'L1_MBTS_1_C':'',
        'L1_MBTS_1_A_EMPTY':'',
        'L1_MBTS_1_C_EMPTY':'',

        'L1_MBTSA0':'',
        'L1_MBTSA1':'',
        'L1_MBTSA2':'',
        'L1_MBTSA3':'',
        'L1_MBTSA4':'',
        'L1_MBTSA5':'',
        'L1_MBTSA6':'',
        'L1_MBTSA7':'',
        'L1_MBTSA8':'',
        'L1_MBTSA9':'',
        'L1_MBTSA10':'',
        'L1_MBTSA11':'',
        'L1_MBTSA12':'',
        'L1_MBTSA13':'',
        'L1_MBTSA14':'', 
        'L1_MBTSA15':'',
        'L1_MBTSC0':'', 
        'L1_MBTSC1':'',
        'L1_MBTSC2':'', 
        'L1_MBTSC3':'',
        'L1_MBTSC4':'',
        'L1_MBTSC5':'', 
        'L1_MBTSC6':'',
        'L1_MBTSC7':'', 
        'L1_MBTSC8':'',
        'L1_MBTSC9':'', 
        'L1_MBTSC10':'', 
        'L1_MBTSC11':'', 
        'L1_MBTSC12':'', 
        'L1_MBTSC13':'', 
        'L1_MBTSC14':'', 
        'L1_MBTSC15':'', 

        'L1_ALFA_ANY':'',
        'L1_ALFA_ELAST15':'', 
        'L1_ALFA_ELAST18':'',
        'L1_ALFA_B7L1U':'',
        'L1_ALFA_B7L1L':'',
        'L1_ALFA_A7L1U':'',
        'L1_ALFA_A7L1L':'',
        'L1_ALFA_A7R1U':'',
        'L1_ALFA_A7R1L':'',
        'L1_ALFA_B7R1U':'',
        'L1_ALFA_B7R1L':'',
        'L1_ALFA_SYST9':'', 
        'L1_ALFA_SYST10':'', 
        'L1_ALFA_SYST11':'', 
        'L1_ALFA_SYST12':'', 
        'L1_ALFA_SYST17':'', 
        'L1_ALFA_SYST18':'',

        'L1_BCM_Wide_BGRP12':'', 
        'L1_BCM_AC_CA_BGRP12':'', 
        'L1_BCM_Wide_EMPTY':'', 
        'L1_BCM_Wide_UNPAIRED_ISO':'', 
        'L1_BCM_Wide_UNPAIRED_NONISO':'',
        'L1_BCM_AC_UNPAIRED_ISO':'',
        'L1_BCM_CA_UNPAIRED_ISO':'',
        'L1_BCM_AC_UNPAIRED_NONISO':'',
        'L1_BCM_CA_UNPAIRED_NONISO':'',
        'L1_BCM_AC_ABORTGAPNOTCALIB':'', 
        'L1_BCM_CA_ABORTGAPNOTCALIB':'',
        'L1_BCM_Wide_ABORTGAPNOTCALIB':'',
        'L1_BCM_AC_CALIB':'', 
        'L1_BCM_CA_CALIB':'',
        'L1_BCM_Wide_CALIB':'',
        'L1_BCM_AC_UNPAIREDB1':'', 
        'L1_BCM_CA_UNPAIREDB2':'',
        'L1_J12_UNPAIREDB1':'', 
        'L1_J12_UNPAIREDB2':'',

        'L1_AFP_A_OR_C_UNPAIRED_ISO':'',
        'L1_AFP_A_OR_C_UNPAIRED_NONISO':'',
        'L1_AFP_A_OR_C_EMPTY':'',
        'L1_AFP_A_OR_C_FIRSTEMPTY':'',
        'L1_AFP_FSA_BGRP12':'',
        'L1_AFP_FSC_BGRP12':'',
        'L1_AFP_NSA_BGRP12':'',
        'L1_AFP_NSC_BGRP12':'',
        'L1_AFP_A':'',
        'L1_AFP_C':'',
        'L1_AFP_A_AND_C':'',
        'L1_AFP_A_OR_C':'',
        'L1_AFP_A_OR_C_MBTS_2':'',
        'L1_AFP_A_AND_C_MBTS_2':'',

    } 

    #----------------------------------------------
    def remapItems():  
        itemsToRemove = []
        for itemIndex, itemName in enumerate(L1MenuFlags.items()):
            if (itemName in L1MenuFlags.ItemMap()):
                if (L1MenuFlags.ItemMap()[itemName] != ''):
                    L1MenuFlags.items()[itemIndex] = L1MenuFlags.ItemMap()[itemName]                                                
                else: 
                    itemsToRemove.append(itemIndex)

        for i in reversed(itemsToRemove):
            del L1MenuFlags.items()[i]
    #----------------------------------------------
                                           
    remapItems()

