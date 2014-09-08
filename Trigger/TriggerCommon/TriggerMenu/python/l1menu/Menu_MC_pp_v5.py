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
        'EM3', 'EM6', 'EM12', 'EM7V', 'EM8VH', 'EM15V', 'EM15VH', 'EM15VHI', 'EM18VH', 'EM18VHI', 'EM18VHIrel', 'EM20V', 'EM20VHI', 'EM22VHI', 'EM24VHI', 'EM50', # see savannah https://savannah.cern.ch/bugs/?103935

        # 9 x TAU
        'HA6', 'HA8', 'HA12', 'HA12I', 'HA15', 'HA20', 'HA20I', 'HA30', 'HA40', 'HA60', 

        # 8 x JET
        'J12', 'J15', 'J20', 'J25', 'J30', 'J40', 'J50', 'J75', 'J100', 'J400',

        # central jets
        "J17.ETA22", "J25.ETA22", "J15.ETA24", "FJ15.23ETA",

        # 0 x JF

        # 6 x FJ
        'FJ15', 'FJ20', 'FJ30', 'FJ50', 'FJ75', 'FJ100', 
        

        # 8 x TE
        'TE20', 'TE30', 'TE40', # 'TE1000', 'TE1200', 'TE1400',# --> the last three are left in as a forced threshold.

        # 8 x XS
        # 'XS25', 'XS30', 'XS35', 'XS45', 'XS50', 'XS55', 'XS60', 'XS65',

        # 8 x XE
        'XE35', 'XE40', 'XE45', 'XE50', 'XE60', 'XE70', 'XE80', 'XE90',
        
        # 2 x MBTS
        'MBTS_A', 'MBTS_C',

#        # 2  x BPTX
#        'BPTX0','BPTX1',
#
        # 2 x LUCID
        'LUCID_A','LUCID_C',
#
#        # 4 x BCM
#        'BCM_AtoC', 'BCM_CtoA', 'BCM_Wide', 'BCM_Comb',
#
#        # 3 x ZDC
#        'ZDC_A', 'ZDC_C','ZDC_AND',
#
#        # LUCID_COMM 
#        'NIMDIR5',
#
#        # TRT
#        'NIMDIR25',
#
#        # L1A_MON
#        'NIMDIR26',
#
#        # LHCF
#        'NIMDIR36',
#
#        # 3 x CALREQ
#        'NIM28','NIM29','NIM30','NIM_MUB', 'NIM_MUE',
#
#        # 1 x ZB
#        'ZB_EM12',
#
#
#        ### Forced for MBTSSI
#        'MBTS_A0', 'MBTS_A1', 'MBTS_A2', 'MBTS_A3', 'MBTS_A4'  , 'MBTS_A5', 'MBTS_A6', 'MBTS_A7', 
#        'MBTS_A8', 'MBTS_A9', 'MBTS_A10', 'MBTS_A11', 'MBTS_A12' , 'MBTS_A13', 'MBTS_A14', 'MBTS_A15', 
#        'MBTS_C0', 'MBTS_C1', 'MBTS_C2', 'MBTS_C3', 'MBTS_C4'  , 'MBTS_C5', 'MBTS_C6', 'MBTS_C7', 
#        'MBTS_C8', 'MBTS_C9', 'MBTS_C10', 'MBTS_C11', 'MBTS_C12' , 'MBTS_C13', 'MBTS_C14', 'MBTS_C15',
#
#        # Forced for ALFA
#        'NIM_B7R1L','NIM_B7R1U','NIM_B7L1L','NIM_B7L1U',
#        'NIM_A7R1L','NIM_A7R1U','NIM_A7L1L','NIM_A7L1U',
#        'NIM_B7R1L_OD','NIM_B7R1U_OD','NIM_B7L1L_OD','NIM_B7L1U_OD',
#        'NIM_A7R1L_OD','NIM_A7R1U_OD','NIM_A7L1L_OD','NIM_A7L1U_OD',
#
#        # For L1_ALFA
#        'NIM_ALFA_LOG', 'NIM_ALFA_LOG_OD'


        # L1 Topo inputs
        #'JetDeltaPhiNarrow',
        #'TOPO_HT', 'TOPO_JPSI', 'TOPO_0', 'TOPO_1', 'TOPO_2', 'TOPO_3', 'TOPO_4', 'TOPO_5', 'TOPO_6',


        ]


    Lvl1Flags.items = [

        # single EM
        'L1_EM3', 'L1_EM6', 'L1_EM12', 'L1_EM7V', 'L1_EM8VH', 'L1_EM15V', 'L1_EM15VH', 'L1_EM15VHI', 'L1_EM18VH', 'L1_EM18VHI',  'L1_EM18VHIrel', 'L1_EM20V', 'L1_EM20VHI', 'L1_EM22VHI', 'L1_EM24VHI', 'L1_EM50',
        # see savannah https://savannah.cern.ch/bugs/?103935

        # combined EM -jet
        'L1_EM15VH_FJ15.23ETA',

        # single MU
        "L1_MU4", "L1_MU6", "L1_MU10", "L1_MU11", "L1_MU15", "L1_MU20",

        # multi lepton
        "L1_2EM3", "L1_2EM8VH", "L1_2EM15VH", "L1_2EM3_EM6", "L1_2EM3_EM12", "L1_3EM7V_EM15VH", "L1_3EM7V_EM15VHI", "L1_3EM7V_2EM8VH_EM15VH", "L1_3EM7V_2EM8VH_EM15VHI",
        "L1_2MU4", "L1_2MU6", "L1_2MU10", "L1_MU6_MU10", "L1_MU4_MU10", "L1_3MU6", 
        "L1_EM7V_MU10", "L1_EM15V_MU4", "L1_2EM8VH_MU10", "L1_EM8VH_2MU6", "L1_EM15VH_MU10", "L1_EM3_MU20",

        # single tau
        "L1_TAU6", "L1_TAU8", "L1_TAU12", "L1_TAU12I", "L1_TAU20", "L1_TAU30", "L1_TAU40", "L1_TAU60", # "L1_2TAUXX",

        # multi tau
        "L1_2TAU12_TAU20", "L1_2TAU12I_TAU20",

        # combined tau - lepton
        "L1_EM15VH_2TAU12", "L1_EM15VH_2TAU20", "L1_EM15VHI_2TAU12I", "L1_EM15VH_TAU40_2TAU15", "L1_EM15VHI_2TAU15_TAU40", "L1_EM15VHI_2TAU12I_FJ20", "L1_EM15VH_2TAU12_J25_2J15_3J12", "L1_EM15VHI_2TAU12I_J25_2J15_3J12",

        "L1_TAU12_MU10", "L1_TAU12I_MU10", "L1_MU10_TAU12_J25_2J12", "L1_MU10_TAU20", "L1_TAU20I_MU10",

        # combined tau - jet
         "L1_TAU12I_MU10_J25", "L1_TAU12I_MU10_FJ20", "L1_TAU20_2TAU12_J25_2J20_3J12", "L1_2TAU12I_TAU20_J25_2J15_3J12",

        # combined tau - xe
        "L1_TAU20_2J20_XE45", "L1_EM15VHI_2TAU12I_XE35", "L1_TAU12I_MU10_XE35", "L1_TAU20_2TAU12_XE35", "L1_2TAU12I_TAU20I_XE35",


        # single jet
        "L1_J20", "L1_J30", "L1_J50", "L1_J75", "L1_J100", "L1_J400",
        "L1_FJ20", "L1_FJ30", "L1_FJ50", "L1_FJ75", "L1_FJ100",

        # multi jet
        "L1_3J15", "L1_3J20",'L1_3J15.ETA24', "L1_3J50", "L1_4J20", "L1_3J75", "L1_4J30",

        # multi jet central
        "L1_4J17.ETA22", "L1_3J25.ETA22", "L1_5J15.ETA24",

        
        # combined jet
        "L1_J40_XE50", "L1_J75_XE40",

        # XE
        "L1_XE35", "L1_XE45", "L1_XE50", "L1_XE60", "L1_XE70", "L1_XE80", "L1_XE90", # "L1_XSXX",
        
        # TE
        "L1_TE20", "L1_TE30", "L1_TE40",   

        # MBTS
        "L1_MBTS_1", "L1_MBTS_2", "L1_MBTS_1_1", "L1_MBTS_2_UNPAIRED_ISO",

        # RNDM
        'L1_RD0_FILLED', 'L1_RD0_UNPAIRED_ISO','L1_RD0_EMPTY',
        # 'L1_RD0_FIRSTEMPTY', 'L1_RD1_FILLED', 'L1_RD1_EMPTY',
        

        #LUCID
        'L1_LUCID', 'L1_LUCID_EMPTY', 'L1_LUCID_UNPAIRED_ISO', 

        # Topo
        #'L1_TOPO_JetDPhi',

        # "L1_2TAU12I_TAU20_FJ20:DR28",
        # "L1_J40_XE50_DPHI1-MET-CJ20L",
        # "L1_5J15.ETA25",
        # "L1_EM15VHI_2TAU12I_J25:DR28",
        # "L1_2TAU12I_TAU20_J25:DR28",
        # "L1_JPSI T&P TOPO",
        # "L1_2MU6_DR02",
        # "L1_2MU4_DYL1Topo",
        # "L1_2MU4_MU6_DYL1Topo",
        # "L1_2MU6_Barrel",      # needs to be a L1Topo
        # "L1_2MU6_BarrelOnly",  # " 
        # "L1_MU6_MU10_Barrel",  # "
        # "L1_TAU30_CaloRatio",
        # "L1_EM15_MU4_DETA-DPHI",
        # "L1_EM8_MU10_DETA-DPHI",
        # "L1_VBF Triggers (J,FJ,L1Topo)?",
        # "L1_2FJ25_Mass-DETA",
        # "L1_2J20_J30_MU4-DETA02-MU-JL",
        # "L1_DijetSearch-DataScouting",
        # "L1_HT200",




##         # Partition 1
##         'L1_CALREQ0_P1', 'L1_CALREQ1_P1', 'L1_CALREQ2_P1',
        ]



    ### the CTP ID mapping still has to be defined, currently it is taken from the order of the items

    Lvl1Flags.CtpIdMap = {
#        'L1_EM3'                       : 0,
#        'L1_EM6'                       : 1,
#        'L1_EM16V'                     : 2, 
#        etc
        }
    
    Lvl1Flags.prescales = {}
    
