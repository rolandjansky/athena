# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def defineMenu():
    """
    MC and Physics menu are identical
    """
    
    import Menu_MC_pp_v5
    from TriggerMenu.l1.Lvl1Flags import Lvl1Flags

    Menu_MC_pp_v5.defineMenu()

    # the ALFA items are in the physics menu only
    Lvl1Flags.thresholds += [
        # ALFA (the replication is needed to build all the combinations in the CTPCore)
        'ALFA_B7R1L', 'ALFA_B7R1U', 'ALFA_A7R1L', 'ALFA_A7R1U', 'ALFA_A7L1L', 'ALFA_A7L1U', 'ALFA_B7L1L', 'ALFA_B7L1U',
        'ALFA2_B7R1L', 'ALFA2_B7R1U', 'ALFA2_A7R1L', 'ALFA2_A7R1U', 'ALFA2_A7L1L', 'ALFA2_A7L1U', 'ALFA2_B7L1L', 'ALFA2_B7L1U',
        'ALFA3_B7R1L', 'ALFA3_B7R1U', 'ALFA3_A7R1L', 'ALFA3_A7R1U', 'ALFA3_A7L1L', 'ALFA3_A7L1U', 'ALFA3_B7L1L', 'ALFA3_B7L1U',
        'ALFA4_B7R1L', 'ALFA4_B7R1U', 'ALFA4_A7R1L', 'ALFA4_A7R1U', 'ALFA4_A7L1L', 'ALFA4_A7L1U', 'ALFA4_B7L1L', 'ALFA4_B7L1U',
        'ALFA_B7R1L_OD', 'ALFA_B7R1U_OD', 'ALFA_A7R1L_OD', 'ALFA_A7R1U_OD', 'ALFA_A7L1L_OD', 'ALFA_A7L1U_OD', 'ALFA_B7L1L_OD', 'ALFA_B7L1U_OD', 
        ]


    Lvl1Flags.items += [
        #ALFA
        'L1_ALFA_ELAST1',
        'L1_ALFA_ELAST2',
        'L1_ALFA_ELAST11','L1_ALFA_ELAST12',
        'L1_ALFA_ELAST13','L1_ALFA_ELAST14','L1_ALFA_ELAST15','L1_ALFA_ELAST15_Calib',
        'L1_ALFA_ELAST16','L1_ALFA_ELAST17','L1_ALFA_ELAST18','L1_ALFA_ELAST18_Calib',
        'L1_ALFA_SDIFF1','L1_ALFA_SDIFF2','L1_ALFA_SDIFF3','L1_ALFA_SDIFF4','L1_ALFA_SDIFF5','L1_ALFA_SDIFF6','L1_ALFA_SDIFF7','L1_ALFA_SDIFF8',      
        'L1_MBTS_1_A_ALFA_C','L1_MBTS_1_C_ALFA_A','L1_MBTS_1_A_ALFA_C_UNPAIRED_ISO','L1_MBTS_1_C_ALFA_A_UNPAIRED_ISO',
        'L1_MBTS_1_ALFA_ANY',
        'L1_MBTS_2_A_ALFA_C','L1_MBTS_2_C_ALFA_A','L1_MBTS_2_A_ALFA_C_UNPAIRED_ISO','L1_MBTS_2_C_ALFA_A_UNPAIRED_ISO','L1_MBTS_2_ALFA',
        'L1_LUCID_A_ALFA_C','L1_LUCID_C_ALFA_A','L1_LUCID_A_ALFA_C_UNPAIRED_ISO','L1_LUCID_C_ALFA_A_UNPAIRED_ISO','L1_LUCID_ALFA',                                 
        'L1_ZDC_A_ALFA_C','L1_ZDC_C_ALFA_A','L1_ZDC_ALFA',
        'L1_EM3_ALFA_ANY',
        'L1_EM3_ALFA_ANY_UNPAIRED_ISO',

        # VETO MBTS currently not possible with TMC
        'L1_EM3_ALFA_ANY_VETO_MBTS',
        'L1_EM3_ALFA_EINE_VETO_MBTS',
        'L1_TRT_ALFA_ANY_VETO_MBTS',
        'L1_TRT_ALFA_ANY_VETO_MBTS_mbSpTrk',                                   
        'L1_TRT_ALFA_EINE_VETO_MBTS',
        'L1_ALFA_ELASTIC_VETO_MBTS',
        'L1_ALFA_ELASTIC_VETO_MBTS_UNPAIRED_ISO',
        'L1_ALFA_ANTI_ELASTIC_VETO_MBTS',
        'L1_ALFA_ANTI_ELASTIC_VETO_MBTS_UNPAIRED_ISO',
        'L1_ALFA_ANY_VETO_MBTS',
        'L1_ALFA_ANY_VETO_MBTS_UNPAIRED_ISO',        


        'L1_EM3_ALFA_EINE',
        'L1_EM3_ALFA_MBTS_A','L1_EM3_ALFA_MBTS_C','L1_EM3_ALFA_MBTS_A_UNPAIRED_ISO','L1_EM3_ALFA_MBTS_C_UNPAIRED_ISO',
        'L1_J12_ALFA_ANY','L1_J12_ALFA_ANY_UNPAIRED_ISO',                              
        'L1_TE5_ALFA_ANY','L1_TE5_ALFA_ANY_UNPAIRED_ISO','L1_TE5_ALFA_EINE',
        'L1_TRT_ALFA_ANY','L1_TRT_ALFA_ANY_UNPAIRED_ISO',
        'L1_TRT_ALFA_ANY_mbSpTrk','L1_TRT_ALFA_ANY_mbSpTrk_UNPAIRED_ISO',
        'L1_TRT_ALFA_EINE','L1_TRT_ALFA_EINE_mbSpTrk',
        'L1_LHCF_ALFA_ANY_A','L1_LHCF_ALFA_ANY_C','L1_LHCF_ALFA_ANY_A_UNPAIRED_ISO','L1_LHCF_ALFA_ANY_C_UNPAIRED_ISO',
        'L1_BGRP1_ALFA_BGT','L1_BGRP4_ALFA_BGT','L1_BGRP10_ALFA_BGT',
        'L1_ALFA_SHOW1', 'L1_ALFA_SHOW2', 'L1_ALFA_SHOW3','L1_ALFA_SHOW4',
        'L1_ALFA_SHOWSYST1','L1_ALFA_SHOWSYST2','L1_ALFA_SHOWSYST3','L1_ALFA_SHOWSYST4','L1_ALFA_SHOWSYST5',
        'L1_ALFA_SYST1' ,'L1_ALFA_SYST2' ,'L1_ALFA_SYST3' ,'L1_ALFA_SYST4' ,
        'L1_ALFA_SYST9' ,'L1_ALFA_SYST10','L1_ALFA_SYST11','L1_ALFA_SYST12',
        'L1_ALFA_SYST17','L1_ALFA_SYST18',
        'L1_ALFA_ANY', 'L1_ALFA_ANY_JET','L1_ALFA_ANY_mbSpTrk',                             
        'L1_ALFA_ANY_EMPTY','L1_ALFA_ANY_FIRSTEMPTY','L1_ALFA_ANY_UNPAIRED_ISO','L1_ALFA_ANY_UNPAIRED_NONISO',
        'L1_ALFA_ANY_BGRP10','L1_ALFA_ANY_ABORTGAPNOTCALIB','L1_ALFA_ANY_CALIB',
        'L1_ALFA_B7L1U','L1_ALFA_B7L1L','L1_ALFA_A7L1U','L1_ALFA_A7L1L','L1_ALFA_A7R1U','L1_ALFA_A7R1L','L1_ALFA_B7R1U','L1_ALFA_B7R1L',
        'L1_ALFA_ANY_A_EMPTY','L1_ALFA_ANY_C_EMPTY',
        'L1_ALFA_B7L1U_OD','L1_ALFA_B7L1L_OD','L1_ALFA_A7L1U_OD','L1_ALFA_A7L1L_OD','L1_ALFA_A7R1U_OD','L1_ALFA_A7R1L_OD','L1_ALFA_B7R1U_OD','L1_ALFA_B7R1L_OD',
        'L1_ALFA_B7L1_OD','L1_ALFA_A7L1_OD','L1_ALFA_B7R1_OD','L1_ALFA_A7R1_OD',
        ]




    #---------------------------------
    # L1menu in MCppV5 is basis 
    # BeamSplashes: adapt some thresholds and items
    #---------------------------------
    Lvl1Flags.ThresholdMap = {
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

        'HT190-AJ15all.ETA20': 'HT200-AJ20s5.ETA49',
        'HT190-J15s5.ETA20': 'HT190-AJ20s5.ETA49',
        'HT150-AJ20all.ETA30': 'HT1-AJ0all.ETA49',
        'HT150-J20s5.ETA30': '',

        '10MINDPHI-Js2-XE50': '',
        '1INVM5-EMs2-EMall': '1INVM5-EMs2-EMs',
        '1INVM5-EM7s2-EMall': '1INVM5-EM6s2-EMs',
        '1INVM5-EM12s2-EMall': '1INVM5-EM12s2-EMs',

        '05MINDPHI-AJj20s6-XE0': '',
        '10MINDPHI-AJj20s6-XE0': '',
        '15MINDPHI-AJj20s6-XE0': '',
        '10MINDPHI-EM10s6-XE0': '',
        '15MINDPHI-EM10s6-XE0': '',
        '05MINDPHI-EM15s6-XE0': '',
        '05RATIO-XE0-HT0-AJj15all.ETA49': '',
        '08RATIO-XE0-HT0-AJj15all.ETA49': '',
        '90RATIO2-XE0-HT0-AJj15all.ETA49': '',
        '250RATIO2-XE0-HT0-AJj15all.ETA49': '',
        'HT20-AJj15all.ETA49': '',
        'NOT-02MATCH-EM10s1-AJj15all.ETA49': '',
        '05RATIO-XE0-SUM0-EM10s1-HT0-AJj15all.ETA49': '',
        '25MT-EM10s6-XE0': '',
        '30MT-EM10s6-XE0': '',
        '35MT-EM15s6-XE0': '',
        
        '0DR04-MU4ab-CJ15ab': '',
        '0DR04-MU4ab-CJ30ab': '',
        '0DR04-MU6ab-CJ20ab': '',
        '0DR04-MU6ab-CJ25ab': '',
        '0DR04-MU4ab-CJ20ab': '',
        '2INVM999-CMU4ab-MU4ab': '',
        '2INVM999-2CMU4ab': '',
        '2INVM999-ONEBARREL-MU6ab-MU4ab': '',
        '2INVM999-CMU6ab-CMU4ab': '',
        '4INVM8-CMU4ab-MU4ab': '',
        '4INVM8-2CMU4ab': '',
        '4INVM8-ONEBARREL-MU6ab-MU4ab': '',
        '4INVM8-CMU6ab-CMU4ab': '',
        'MULT-CMU4ab-0':'',
        'MULT-CMU4ab-1':'',
        'MULT-CMU6ab-0':'',
        'MULT-CMU6ab-1':'',
        
        '0DETA04-0DPHI03-EM8abi-MU10ab': '',
        '0DETA04-0DPHI03-EM15abi-MUab': '',
        '10MINDPHI-Js2-XE50': '',
        '10MINDPHI-J20s2-XE50': '',
        '10MINDPHI-J20ab-XE50': '',
        '10MINDPHI-CJ20ab-XE50': '',
        '900INVM9999-AJ30s6-AJ20s6': '4INVM9999-AJ0s6-AJ0s6',
        '800INVM9999-AJ30s6-AJ20s6': '3INVM9999-AJ0s6-AJ0s6',
        '700INVM9999-AJ30s6-AJ20s6': '2INVM9999-AJ0s6-AJ0s6',
        '500INVM9999-AJ30s6-AJ20s6': '1INVM9999-AJ0s6-AJ0s6',
        '400INVM9999-AJ30s6-AJ20s6': '',
        '350INVM9999-AJ30s6-AJ20s6': '350INVM9999-J30s6-J20s6',
        '300INVM9999-AJ30s6-AJ20s6': '300INVM9999-J30s6-J20s6',
        '200INVM9999-AJ30s6-AJ20s6': '250INVM9999-J30s6-J20s6',
        '100INVM9999-AJ30s6-AJ20s6': '200INVM9999-J30s6-J20s6',
        'HT150-AJj15all.ETA49': '',
        #'0DETA10-Js1-Js2': '',
        '0MATCH-4AJ20.ETA32-4AJj15': '',
        '210RATIO-0MATCH-TAU30si2-EMall': '',
        'NOT-0MATCH-TAU30si2-EMall': '',
        '1DISAMB-TAU12abi-J25ab': '',
        'DISAMB-EM15abhi-TAU40ab': '',
        '1DISAMB-TAU20ab-J20ab': '',
        'DISAMB-EM15abhi-TAU12abi': '',
        '1DISAMB-EM15his2-TAU12abi-J25ab': '',
        '1DISAMB-J25ab-0DR28-EM15his2-TAU12abi': '',
        '1DISAMB-TAU20abi-TAU12abi-J25ab': '',
        '0DETA20-0DPHI20-TAU20abi-TAU12abi': '',
        '1DISAMB-J25ab-0DR28-TAU20abi-TAU12abi': '',
        'LAR-EM50s1': '',
        'LAR-J100s1': '',
        '2DR15-CMU4ab-MU4ab': '',
        '2DR15-2CMU4ab': '',
        '2DR15-ONEBARREL-MU6ab-MU4ab': '',
        '2DR15-CMU6ab-CMU4ab': '',
        'KF-XE35-AJall': '',
        'KF-XE45-AJall': '',
        'KF-XE55-AJall': '',
        'KF-XE60-AJall': '',
        'KF-XE65-AJall': '',
        'KF-XE75-AJall': '',
        
        #M9 thresholds
        #'2DR99-2MU4ab': '',
        #'0DR10-MU10ab-MU6ab': '',
        #'2DR15-MU6ab-MU4ab': '',
        #'2DR15-2MU4ab': '',
        #'2DR15-2MU6ab': '',
        #'2INVM999-2MU4ab': '',
        #'2INVM999-2MU6ab': '',
        #'2INVM999-MU6ab-MU4ab': '',
        #'4INVM8-2MU4ab': '',
        #'4INVM8-2MU6ab': '',
        #'4INVM8-MU6ab-MU4ab': '',
        #'5DETA99-5DPHI99-2MU4ab': '',
        #'5DETA99-5DPHI99-MU6ab-MU4ab': '',
        #'5DETA99-5DPHI99-2MU6ab': '',
        #'0DR28-MU10ab-TAU12abi': '',
        #'0DR28-TAU20abi-TAU12abi': '',
        }

    Lvl1Flags.ItemMap = {
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
#
        'L1_BTAG-MU4J15': '',
        'L1_BTAG-MU4J30': '',
        'L1_BTAG-MU6J20': '',
        'L1_BTAG-MU6J25': '',
        'L1_BTAG-MU4J20': '',
        'L1_3J15_BTAG-MU4J15': '',
        'L1_3J15_BTAG-MU4J30': '',
        'L1_3J15_BTAG-MU6J25': '',
        'L1_3J20_BTAG-MU4J20': '',
        #'L1_3J20_BTAG-MU4J30': '',
        #'L1_3J20_BTAG-MU6J25': '',
        'L1_J40_DPHI-Js2XE50': '',
        'L1_J40_DPHI-J20s2XE50': '',
        'L1_J40_DPHI-J20XE50': '',
        'L1_J40_DPHI-CJ20XE50': '',
        'L1_HT190-J15.ETA20': '',
        'L1_HT190-J15s5.ETA20': 'L1_HT200-J20s5.ETA49',
        'L1_HT150-J20.ETA30': 'L1_HT1-J0.ETA49',
        'L1_HT150-J20s5.ETA30': 'L1_HT190-J20s5.ETA49',
        'L1_JPSI-1M5': 'L1_JPSI-1M5-EMs',
        'L1_JPSI-1M5-EM7': 'L1_JPSI-1M5-EM6s',
        'L1_JPSI-1M5-EM12': 'L1_JPSI-1M5-EM12s',
        'L1_W-05DPHI-JXE-0': '',
        'L1_W-10DPHI-JXE-0': '',
        'L1_W-15DPHI-JXE-0': '',
        'L1_W-10DPHI-EMXE-0': '',
        'L1_W-15DPHI-EMXE-0': '',
        'L1_W-05DPHI-EMXE-1': '',
        'L1_W-05RO-XEHT-0': '',
        'L1_W-08RO-XEHT-0': '',
        'L1_W-90RO2-XEHT-0': '',
        'L1_W-250RO2-XEHT-0': '',
        'L1_W-HT20-JJ15.ETA49': '',
        'L1_W-NOMATCH': '',
        'L1_W-NOMATCH_W-05RO-XEEMHT': '',
        'L1_EM10_W-MT25': '',
        'L1_EM10_W-MT30': '',
        'L1_EM15_W-MT35': '',
        'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0': '',
        'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS30': '',
        'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20': '',
        'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE': '',
        'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_W-250RO2-XEHT-0': '',
        'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-15DPHI-EMXE_XS60': '',
        'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-15DPHI-EMXE_XS30': '',
        'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-15DPHI-EMXE': '',
        'L1_BPH-DR-2MU4_BPH-2M-2MU4': '',
        'L1_BPH-DR-2MU4_BPH-4M8-2MU4': '',
        'L1_BPH-DR-2MU6_BPH-2M-2MU6': '',
        'L1_BPH-DR-2MU6_BPH-4M8-2MU6': '',
        'L1_BPH-2M-2MU4-B': '',
        'L1_BPH-2M-2MU4-BO': '',
        'L1_BPH-2M-MU6MU4-B': '',
        'L1_BPH-2M-MU6MU4-BO': '',
        'L1_BPH-4M8-2MU4-B': '',
        'L1_BPH-4M8-2MU4-BO': '',
        'L1_BPH-4M8-MU6MU4-B': '',
        'L1_BPH-4M8-MU6MU4-BO': '',
        'L1_BPH-DR-2MU4-B_BPH-2M-2MU4-B':'',
        'L1_BPH-DR-2MU4-B_BPH-4M8-2MU4-B':'',
        'L1_BPH-DR-2MU4-BO_BPH-2M-2MU4-BO':'',
        'L1_BPH-DR-2MU4-BO_BPH-4M8-2MU4-BO':'',
        'L1_MULT0-CMU4': '',
        'L1_MULT1-CMU4': '',
        'L1_MULT0-CMU6': '',
        'L1_MULT1-CMU6': '',
        'L1_LFV-EM8I': '',
        'L1_LFV-EM15I': '',
        'L1_DPHI-Js2XE50': '',
        'L1_DPHI-J20s2XE50': '',
        'L1_DPHI-J20XE50': '',
        'L1_DPHI-CJ20XE50': '',
        'L1_MJJ-900': 'L1_MJJ-4',
        'L1_MJJ-800': 'L1_MJJ-3',
        'L1_MJJ-700': 'L1_MJJ-2',
        'L1_MJJ-400': 'L1_MJJ-1',
        'L1_MJJ-350': 'L1_MJJ-350-0',
        'L1_MJJ-300': 'L1_MJJ-300-0',
        'L1_MJJ-200': 'L1_MJJ-250-0',
        'L1_MJJ-100': 'L1_MJJ-200-0',
        'L1_HT150-JJ15.ETA49': '',
        #'L1_DETA-JJ': '',
        'L1_J4-MATCH': '',
        'L1_W-05RO-XEEMHT': '',
        'L1_LLP-RO': '',
        'L1_LLP-NOMATCH': '',
        'L1_TAU12I-J25': '',
        'L1_EM15-TAU40': '',
        'L1_TAU20-J20': '',
        'L1_EM15-TAU12I': '',
        'L1_EM15TAU12I-J25': '',
        'L1_DR-EM15TAU12I-J25': '',
        'L1_TAU20ITAU12I-J25': '',
        'L1_BOX-TAU20ITAU12I': '',
        'L1_DR-TAU20ITAU12I-J25': '',
        'L1_LAR-EM': '',
        'L1_LAR-J': '',
        'L1_KF-XE35': '',
        'L1_KF-XE45': '',
        'L1_KF-XE55': '',
        'L1_KF-XE60': '',
        'L1_KF-XE65': '',
        'L1_KF-XE75': '',
        'L1_BPH-DR-2MU4-B': '',
        'L1_BPH-DR-2MU4-BO': '',
        'L1_BPH-DR-MU6MU4-B': '',
        'L1_BPH-DR-MU6MU4-BO': '',
        'L1_MU6_MJJ-200': '',
        'L1_MU6_MJJ-300': '',
        'L1_MU6_MJJ-400': '',
        'L1_MU6_MJJ-500': '',
        'L1_J30_2J20_4J20.0ETA49_MJJ-400': '',
        'L1_J30_2J20_4J20.0ETA49_MJJ-700': '',
        'L1_J30_2J20_4J20.0ETA49_MJJ-800': '',
        'L1_J30_2J20_4J20.0ETA49_MJJ-900': '',
        'L1_3J20_4J20.0ETA49_MJJ-400': '',
        'L1_3J20_4J20.0ETA49_MJJ-700': '',
        'L1_3J20_4J20.0ETA49_MJJ-800': '',
        'L1_3J20_4J20.0ETA49_MJJ-900': '',
        'L1_XE35_MJJ-200': '',
        'L1_J25_3J12_EM15-TAU12I': '',
        'L1_J25_2J12_DR-MU10TAU12I': '',
        'L1_J25_2J20_3J12_BOX-TAU20ITAU12I': '',
        'L1_J25_2J20_3J12_DR-TAU20ITAU12I': '',
        'L1_DR-MU10TAU12I_TAU12I-J25': '',
        'L1_DR-MU10TAU12I_TAU12I-J25': '',
        'L1_MU10_TAU12I-J25': '',
        'L1_XE45_TAU20-J20': '',
        'L1_XE35_EM15-TAU12I': '',
        'L1_XE40_EM15-TAU12I': '',

        # M9 items
        #'L1_LFV-MU': '',
        #'L1_DY-DR-2MU4': '',
        #'L1_BPH-DR-2MU4': '',      
        #'L1_BPH-DR-2MU6': '',
        #'L1_BPH-DR-MU6MU4': '',
        #'L1_BPH-2M-2MU4': '',
        #'L1_BPH-2M-2MU6': '',
        #'L1_BPH-2M-MU6MU4': '',
        #'L1_BPH-4M8-2MU4': '',
        #'L1_BPH-4M8-2MU6': '',
        #'L1_BPH-4M8-MU6MU4': '',
        #'L1_DY-BOX-2MU4': '',
        #'L1_DY-BOX-MU6MU4': '',
        #'L1_DY-BOX-2MU6': '',
        #'L1_DR-TAU20ITAU12I': '',
        #'L1_DR-MU10TAU12I': '',

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
