# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TriggerMenuPython.Physics_pp_v4 as physics_menu

from AthenaCommon.Logging import logging
from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.TriggerPythonConfig import TriggerPythonConfig
from TriggerMenuPython.Lvl1Flags import Lvl1Flags, addToItemMap, addToThresholdMap
from copy import deepcopy
import re
log = logging.getLogger( 'MC_pp_v4.py' )
#---------------------------------------------------------------------
# Just copy the MC v3 menu with empty HLT
#---------------------------------------------------------------------
def setupMenu():
    physics_menu.setupMenu()
    #TriggerFlags.Lvl1.items = TriggerFlags.Lvl1.items() + ['L1_XE35_BGRP7']
    TriggerFlags.Lvl1.items = TriggerFlags.Lvl1.items() 

    TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
        #'mu18_L2_2mu4T_Jpsimumu',
        #'mu18_medium_L2EFMerged',
        'mu24_tight_FTK',
        'mu20it_tight_FTK',
        'mu6_FTK',
        'mu15_NSWemul_NoL2',
        'mu15_NSWemul_NoEF',
        'mu20_NSWemul_NoL2',
        'mu20_NSWemul_NoEF',
        #'mu22_IDTrkNoCut_tight_R1',
        ]

    TriggerFlags.EgammaSlice.signatures = TriggerFlags.EgammaSlice.signatures() + [
        #'e24vh_medium1_IDTrkNoCut_R1', 
        'e24vhi_medium1_AthSel',
        'e24vhi_medium1_AthLHSel',        
        #next 5 new in 92627
        '2g30_loose',
        '2g20_loose',
        ##JPsi
        #'2e5_tight_Jpsi',
        #'e5_tight_e5_NoCut',
        ##Supporting triggers
        'e45_etcut',
        'e60_etcut',
        #'e22vh_medium1_SiTrk' ,
        #'e22vh_medium1_SiTrk',
        #'e22vh_medium1_IdScan',
        #'e22vh_medium1_TRT' ,
        #'e5_tight1_e4_etcut_Jpsi_SiTrk',
        #'e5_tight1_e4_etcut_Jpsi_IdScan',
        #'e5_tight1_e4_etcut_Jpsi_TRT',
        '2g15vh_loose_g10_loose',
        '3g15vh_medium',
        '2g15vh_medium',
        'g20vh_medium',
        'g15vh_loose',
        'g20Tvh_medium',
        'g12Tvh_medium',
        #FTK
        'e24vh_medium1_FTK' ,
        '2e12Tvh_loose1_FTK' ,
        ##exotic monopoles
        'g_nocut_hiptrtL2',
        'g_nocut_hiptrtL2_L1EM30',
        # mu+gamma
        'g5_loose',
        'e24vhi_medium1_L2ET21',
        'e24vhi_medium1_L2ET22',
        'e24vhi_medium1_L2ET23',
        'e24vhi_medium1_L2ET24',

        #Brem chain
        'e24vhi_medium1_L2StarABrem',
        #Savannah #101482
        'e24vhi_medium1_noL2ID',
        'e24vhi_medium1_noL2IDnoEFID',
        'e24vhi_medium1_noL2IDnoEF',
        'e24vhi_medium1_noL2noEFID',        
        'e24vhi_medium1_noL2',
        ]
    
    
    TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [
        #'tau29_IDTrkNoCut_R1',
        'tauNoCut_cosmic',
        'tau20Ti_medium',
        'tau29_medium',
        'tau29i_medium1',
        'tau29i_medium',
        'tau50_medium',
        'tau38T_medium',
        'tau29T_tight1',
        'tau29T_tight1_llh',
        '2tau29i_medium1',
        '2tau38T_medium',
        'tau20_medium_llh',
        'tau29_medium_llh',
        'tau29_medium1_llh',
        'tauNoCut_FTK',
        'tau20_medium_FTK',
        #L2loose tau chains
        'tau18_medium1_L2loose',
        'tau20Ti_medium1_L2loose',
        'tau29T_medium1_L2loose',
        'tau29Ti_medium1_L2loose',
        'tau29Ti_medium1_tau20Ti_medium1_L2loose',
        'tau29Ti_medium1_tau18Ti_medium1_L2loose',
        'tau29Ti_medium1_L2loose_tau20Ti_medium1_L2loose',

        'tau27T_loose3',
        'tau18Ti_loose3',
        'tau18Ti_loose2',
        'tau20_loose3',
        'tau20_loose2',
        'tau27Ti_loose3_tau18Ti_loose3',
        'tau27Ti_loose3',
        'tau27Ti_loose2',
        ### Branch for new L2 opt
        #'tau27Ti_loose2_tau18Ti_loose2_deta20',
        'tau27Ti_loose3_tau18Ti_loose3_deta25',
        'tau27Ti_loose3_tau18Ti_loose3_deta20',

        ##L2 pass trough

        'tau29Ti_loose1_L2NoCut_tau20Ti_loose1_L2NoCut',
        'tau29Ti_loose2_L2NoCut_tau20Ti_loose2_L2NoCut',
        'tau29Ti_medium1_L2NoCut_tau20Ti_medium1_L2NoCut',
        'tau20T_loose1_L2NoCut', 
        'tau20T_loose2_L2NoCut', 
        'tau20T_medium1_L2NoCut',
        'tau20Ti_loose1_L2NoCut',
        'tau20Ti_loose2_L2NoCut',
        'tau20Ti_medium1_L2NoCut',
        'tau29Ti_loose1_L2NoCut',
        'tau29Ti_loose2_L2NoCut',
        'tau29Ti_medium1_L2NoCut',
        
        ]

    TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + [
        #EP.'L1JE140_NoAlg',
        #EP.'L1JE200_NoAlg',
        #EP.'L1JE350_NoAlg',
        # needs to be added back in with new JetDef template
        'j165_u0uchad_LArNoiseBurstT',
        'j30_u0uchad_empty_LArNoiseBurst',
        'j55_u0uchad_firstempty_LArNoiseBurstT',
        # L2FS jets
        'fj45_a4tchad_L2FS', 'fj55_a4tchad_L2FS',
        # Random seeded chains
        'j15_a4tchad',
        'j25_a4tchad',
        'j35_a4tchad',
        'fj15_a4tchad',
        'fj25_a4tchad',
        'fj35_a4tchad',
        '3j25_a4tchad',
        '4j25_a4tchad',
        '5j25_a4tchad',
        'j35_a10tcem',
        'j45_a10tcem_L1RD0', 
        'j45_a4tchad_L1RD0',
        'fj45_a4tchad_L1RD0',
        # Inclusive jets
        'j45_a4tchad', 
        'j55_a4tchad', 
        'j80_a4tchad', 
        'j110_a4tchad', 
        'j145_a4tchad', 
        'j180_a4tchad',
        'j220_a4tchad',
        'j280_a4tchad',
        'j360_a4tchad',
        'j460_a4tchad',
        # Fat jets
        'j180_a10tcem',
        'j240_a10tcem',
        'j240_a10tclcw',
        'j360_a10tcem',
        'j360_a10tclcw',
        'j460_a10tcem',
        'j460_a10tclcw',
        'j80_a10tcem_L2FS',
        'j110_a10tcem_L2FS',
        'j145_a10tcem_L2FS',
        '2j240_a10tcem',
        '2j240_a10tclcw',
        '2j240_a10tclcw_L2FS',
        '2j240_a10tcem_L2FS',
        'j110_a10tcem_L2FS_2j55_a4tchad_4L1J15', 
        # topotowers
        'j180_a4tthad',
        'j220_a4tthad',
        'j260_a4tthad',
        'j380_a4tthad',
        '3j180_a4tthad',
        '4j55_a4tthad_L2FS',
        # LC topo
        'j35_a4tclcw',
        'fj35_a4tclcw',
        'j55_a4tclcw',
        'j180_a4tclcw',
        'j280_a10tclcw_L2FS',
        'fj145_a4tclcw',
        'j35_a4tthad',
        'j360_a4tclcw',
        'j360_a4tthad',
        '3j170_a4tclcw_L1J75',
        '4j80_a4tclcw_L2FS',
        '4j110_a4tclcw_L2FS',
        '5j45_a4tclcw_L2FS',
        '6j45_a4tclcw_L2FS_5L2j15',
        '7j35_a4tclcw_L2FS_5L2j15',
        # Narrow jets
        '2j100_a2tcem',
        '2j150_a2tcem',
        '2j200_a2tcem',
        '2j250_a2tcem',
        # forward jets
        'fj80_a4tchad', 
        'fj110_a4tchad', 
        'fj145_a4tchad',        
        'fj180_a4tchad',
        'fj220_a4tchad',
        # Multijets
        'j110_2j55_a4tchad_L2FS',
        'j110_2j55_a4tchad',
        'j45_a4tchad_L2FS',
        'j55_a4tchad_L2FS',
        'j65_a4tchad_L2FS',
        'j45_a4tchad_L2FS_L1RD0',
        '3j35_a4tchad_L2FS_4L1J15',
        '3j45_a4tchad_L2FS_4L1J15',
        '3j55_a4tchad_L2FS',
        #EP. '3j45_a4tchad_L2FS_L1RD0',
        '4j45_a4tchad_L2FS_L1RD0',
        '5j45_a4tchad_L2FS_L1RD0',
        #EP. '3j45_a4tchad_L2FS',
        '4j45_a4tchad_L2FS_4L1J10',
        '5j45_a4tchad_L2FS',
        '6j45_a4tchad_L2FS',
        '7j45_a4tchad_L2FS',
        '4j55_a4tchad_L2FS',
        '5j55_a4tchad_L2FS',
        '6j55_a4tchad_L2FS',
        '7j55_a4tchad_L2FS',
        '5j55_a4tchad_L2FS_5L1J15',
        '6j55_a4tchad_L2FS_5L1J15',
        '7j55_a4tchad_L2FS_5L1J15',
        '4j65_a4tchad_L2FS',
        '5j65_a4tchad_L2FS',
        '6j65_a4tchad_L2FS',
        '7j65_a4tchad_L2FS',
        '4j65_a4tchad_L2FS_5L1J15',
        '5j65_a4tchad_L2FS_5L1J15',
        '6j65_a4tchad_L2FS_5L1J15',
        '7j65_a4tchad_L2FS_5L1J15',
        '4j65_a4tchad_L2FS_4L1J20',
        '5j65_a4tchad_L2FS_4L1J20',
        '4j80_a4tchad_L2FS',
        '5j80_a4tchad_L2FS',
        '4j80_a4tchad_L2FS_5L1J15',
        '5j80_a4tchad_L2FS_5L1J15',
        '4j80_a4tchad_L2FS_4L1J20',
        '5j80_a4tchad_L2FS_4L1J20',
        '4j110_a4tchad_L2FS',
        '5j110_a4tchad_L2FS',
        '4j110_a4tchad_L2FS_5L1J15',
        '5j110_a4tchad_L2FS_5L1J15',
        '4j110_a4tchad_L2FS_4L1J20',
        '5j110_a4tchad_L2FS_4L1J20',
        '3j170_a4tchad_L1J75', 
        '3j180_a4tchad_L1J75', 
        '4j55_a4tchad_L2FSj15had',
        '4j55_a4tchad_L2FSj25had',
        '5j55_a4tchad_L2FSj15had',
        '5j55_a4tchad_L2FSj25had',
        '6j55_a4tchad_L2FSj15had',
        '6j55_a4tchad_L2FSj25had',
        '5j55_a4tchad_L2PS',
        '5j55_a4tchad_L2FSPS',
        '6j55_a4tchad_L2PS',
        '6j55_a4tchad_L2FSPS',
        '6j35_a4tchad_L2FS_5L2j15',
        '7j35_a4tchad_L2FS_5L2j15',
        '4j45_a4tchad_L2FS',
        '5j45_a4tchad_L2FS_5L1J10',
        '6j45_a4tchad_L2FS_5L2j15',
        '7j45_a4tchad_L2FS_5L2j15',
        '6j55_a4tchad_L2FS_5L2j15',
        # HT Chains
        'j145_a4tchad_ht500_L2FS',
        'j145_a4tchad_ht600_L2FS',
        'j145_a4tchad_ht700_L2FS',
        'j170_a4tchad_ht500',
        'j170_a4tchad_ht600',
        'j170_a4tchad_ht700',
        
        # Low-mu chains
        'j15_a4tchad_L1MBTS',
        'j25_a4tchad_L1MBTS',
        'j35_a4tchad_L1MBTS',
        'fj15_a4tchad_L1MBTS',
        'fj25_a4tchad_L1MBTS',
        'fj35_a4tchad_L1MBTS',
        '3j25_a4tchad_L1MBTS',
        '4j25_a4tchad_L1MBTS',
        '5j25_a4tchad_L1MBTS',
        
        'j10_a4tchadloose_L1MBTS',
        'fj10_a4tchadloose_L1MBTS',
        '3j10_a4tchadloose_L1MBTS',
        '4j10_a4tchadloose_L1MBTS',
        '5j10_a4tchadloose_L1MBTS',
        
        'fj45_a4tchad_L2FS_L1MBTS',
        'fj55_a4tchad_L2FS_L1MBTS',

        'j15_a4tchad_L1TE20',
        'j25_a4tchad_L1TE20',
        'j35_a4tchad_L1TE20',
        'fj15_a4tchad_L1TE20',
        'fj25_a4tchad_L1TE20',
        'fj35_a4tchad_L1TE20',
        '2j15_a4tchad_deta35_FC_L1TE20',
        '2j25_a4tchad_deta35_FC_L1TE20',
        '2j35_a4tchad_deta35_FC_L1TE20',
        'j15_fj15_a4tchad_deta50_FC_L1TE20',
        'j25_fj25_a4tchad_deta50_FC_L1TE20',
        'j35_fj35_a4tchad_deta50_FC_L1TE20',
        'fj45_a4tchad_L2FS_L1TE20',
        'fj55_a4tchad_L2FS_L1TE20',
        '2fj45_a4tchad_deta50_FB_L2FS_L1TE20',
        '2fj55_a4tchad_deta50_FB_L2FS_L1TE20',
        'j10_a4tchadloose',
        'fj10_a4tchadloose',

        # dEta Low-mu chains
        'j10_fj10_a4tchadloose_deta50_FC_L1MBTS',
        'j15_fj15_a4tchad_deta50_FC_L1MBTS',
        'j25_fj25_a4tchad_deta50_FC_L1MBTS',
        'j35_fj35_a4tchad_deta50_FC_L1MBTS',
        '2j10_a4tchadloose_deta35_FC_L1MBTS',
        '2j15_a4tchad_deta35_FC_L1MBTS',
        '2j25_a4tchad_deta35_FC_L1MBTS',
        '2j35_a4tchad_deta35_FC_L1MBTS',
        '2fj45_a4tchad_deta50_FB_L2FS_L1MBTS',
        '2fj55_a4tchad_deta50_FB_L2FS_L1MBTS',

        # HV Chain
        'j35_a4tcem_L1TAU_LOF_HV',
        'l2j25_a4tcem_Trackless_HV',
        'l2j30_a4tcem_2L1MU10_ExtendedEta_HV',
        'j35_a4tcem_L1TAU_LOF_HV_EMPTY',
        'l2j25_a4tcem_Trackless_HV_EMPTY',
        'j35_a4tcem_L1TAU_LOF_HV_UNPAIRED_ISO', 
        'l2j25_a4tcem_Trackless_HV_UNPAIRED_ISO',

        #trigger for Z->bb
        'j145_j100_j35_a4tchad',

        #dummy - only to test, needed only in combination
        #'roi_dummy_J15',

        #3jet+HT triggers for the delayed stream
        'j145_2j45_a4tchad_ht450_L2FS',
        'j145_2j45_a4tchad_ht500_L2FS',
        'j145_2j45_a4tchad_ht550_L2FS',
    
        ]

    
    TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [
        #FTK
        'b55_NoCut_j55_a4tchad_FTK',

        #4-LVL1 jet seeded trigger (#91805, #92019)        
        '2b55_medium_4j55_a4tchad',
        'b45_medium_3j45_a4tchad_L2FS_4L1J15',
        'b35_medium_3j35_a4tchad_L2FS_4L1J15',
        '2b55_medium_3j55_a4tchad_L2FS_4L1J15',
        '2b45_medium_3j45_a4tchad_L2FS_4L1J15',
        'b55_loose_4j55_a4tchad_L2FS',

        # not combined b-jet triggers (#91828)
        #(i) 1b/2j and 2b/2j triggers
        #EP. '2b55_loose_j110_j55_a4tchad',
        #EP. '2b55_loose_j110_j55_a4tchad_1bL2',
        #EP. 'b55_medium_j110_j55_a4tchad',
        #(ii)1b/4j and 2b/4j 
        '2b55_medium_4j55_a4tchad',
        'b55_loose_4j55_a4tchad',
        '2b55_medium_4j55_a4tchad_L2FS',
        
        #(iii) non-standard 1b/4j and 2b/4j triggers
        'b45_medium_3j45_a4tchad_4L1J15',
        'b35_medium_3j35_a4tchad_4L1J15',
        '2b55_medium_3j55_a4tchad_4L1J15',
        '2b45_medium_3j45_a4tchad_4L1J15',
        
        # b-tagged fat jets (#91900)
        'b110_loose_j110_a10tcem_L2FS_L1J75',
        'b145_loose_j145_a10tcem_L2FS',
        'b180_loose_j180_a10tcem_L2FS',
        'b180_loose_j180_a10tcem_L2j140',
        'b220_loose_j220_a10tcem',
        'b280_loose_j280_a10tcem',

        #HT triggers
        #EP. '2b55_loose_j110_j55_a4tchad_ht500',
        #EP. '2b55_medium_j110_j55_a4tchad_ht500',
        '2b55_medium_j165_j55_a4tchad_ht500',
        '2b80_medium_j165_j80_a4tchad_ht500',

        # Additional Multijet Triggers (#92451)
        'b55_medium_4j55_a4tchad_4L1J10',
        'b55_medium_4j55_a4tchad_L2FS_4L1J10',
        'b45_medium_4j45_a4tchad_4L1J10',
        'b45_medium_4j45_a4tchad_L2FS_4L1J10',
        '2b35_loose_3j35_a4tchad_4L1J10',
        '2b35_loose_3j35_a4tchad_L2FS_4L1J10',
        
        'b35_loose_4j35_a4tchad_L2FS_5L1J10',
        'b35_NoCut_4j35_a4tchad_L2FS_5L1J10',
        'b35_loose_4j35_a4tchad_5L1J10',
        'b35_NoCut_4j35_a4tchad_5L1J10',

    ]



    TriggerFlags.BphysicsSlice.signatures = TriggerFlags.BphysicsSlice.signatures() + [
        '2mu6_DiMu_FTK',
        'mu6_DiMu_FS_FTK',
        #'mu4T_Trk_Jpsi',
        '2mu6_Bmumu_Lxy',
        '2mu4T_Bmumu_Lxy',
        'mu4Tmu6_Bmumu_Lxy',
        '2mu4T_Bmumu_Lxy_BarrelOnly',
        '2mu4T_Bmumu_Lxy_Barrel', 'mu4Tmu6_Bmumu_Lxy_Barrel', 
        'mu6_Jpsimumu_L2StarB',
        #'2mu4T_Jpsimumu_IDTrkNoCut_R1',        
        #########"2mu6_Bmumux_L2StarB",

        '2mu6_Bmumux_v2_L2StarA',
        #'2mu4T_Bmumux_v2_Barrel',
        #'2mu4T_Bmumux_v2_BarrelOnly',
        #'mu4Tmu6_Bmumux_v2_Barrel',

        # barrel only Jpsi chains
        '2mu4T_DiMu_BarrelOnly',
        '2mu4T_DiMu_Barrel',
        'mu4Tmu6_DiMu_Barrel',

        'mu4Tmu6_DiMu_noVtx_noOS',

        #98121
        #'2mu6_Bmumux_BcMuMuDs_loose_L2StarB',
        '2mu6_Bmumux_BcMuMuDs_tight_L2StarB', 
        '2mu6_Bmumux_v3_loose_L2StarB',
        '2mu6_Bmumux_v3_tight_L2StarB',
        ]
    
    TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + [
        #'xe55_LArNoiseBurst',
        'xe40',
        'xe70_tight',
        'xs120',
        ##TopoCluster
        'xe30_tclcw',
        'xe70_tight_tclcw',
        #'xe80_tclcw'      ,
        #'xe80_tight_tclcw',
        #'xe90_tclcw'      ,
        #'xe90_tight_tclcw',
        #'xe100_tclcw'      ,
        #'xe100_tight_tclcw',
        #V4
        'xe55_tclcw',
        #'xe60_tclcw_loose',
        'xe60_tclcw',
        'xe70_tclcw_loose','xe70_tclcw_veryloose',
        'xe80_tclcw_loose',
        'xe90_tclcw_veryloose',
        'xe100_tclcw_veryloose',
        ##XS
        #'xs100_tchad',
        #'xs120_tchad',
        'xe_NoCut_allL1',
        # For cross-checks
        'xe60_tclcw',
        'xe60T_tclcw',
        #'xe60_tclcw_loose',
        'xe60_tclcw_loose_v2',
        #'xe60T_tclcw_loose',
        'xe70_tclcw_loose',
        'xe70_tclcw_veryloose',
        'xe70_tclcw_veryloose_v2',
        'xe70T_tclcw_veryloose',
        #'xe80_tclcw_veryloose', 'xe80T_tclcw_veryloose',
        'xe80_tclcw_veryloose_v2', 'xe80T_tclcw_veryloose_v2',
        ]

    TriggerFlags.L1CaloSlice.signatures = [
        'L1Calocalibration',
        ]
    
    TriggerFlags.MinBiasSlice.signatures = TriggerFlags.MinBiasSlice.signatures() + [
        'VdM_mbSpTrk',
        ]
    
    TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures() + [
        
        'mu4T_j360_a4tchad_matched',
        'mu4T_j360_a4tchad_L2FS_matched',

        ### tau+XX
        'tau20T_medium_mu15',
        'tau29T_medium_xe40_tight',
        'tau29T_medium_xe45_tight',
        'tau29Ti_medium_xe40_tight',
        'tau29Ti_medium_xe45_tight',
        #'tau38T_tight_xe40_tight',
        ### egamma+XX
        'g40_loose_EFxe50',
        'g40_loose_L2EFxe50',
        'e12Tvh_medium1_mu10', 'e12Tvh_loose1_mu8',
        'e12Tvh_medium1_mu6',
        'e18_loose1_g35_loose',
        'e18_loose1_g35_medium',
        'e18_medium1_g35_loose',
        'e18_medium1_g25_medium',
        'e18_medium1_g35_medium',
        #92131
        'j220_a10tcem_e45_loose1',
        'j240_a10tcem_e45_etcut',
        'j220_a10tcem_e45_etcut',
        'j240_a10tcem_e60_etcut',
        'j220_a10tcem_e60_etcut',
        ### mu+XX
        'mu18_g20vh_loose', 
        'mu18_2g15_loose',
        '2mu10_MSonly_g10_loose_EMPTY', '2mu10_MSonly_g10_loose_UNPAIRED_ISO',
        '2mu4T_xe50_tclcw',
        'mu24_tight_2j35_a4tchad',
        #'mu24_tight_3j35_a4tchad', #already in physics menu
        'mu10i_g10_loose_TauMass', 
        'mu4Ti_g20Tvh_loose_TauMass', 'mu10i_loose_g12Tvh_loose_TauMass', 
        'mu10i_g10_loose', 'mu20i_tight_g5_loose',
        'mu4Ti_g20Tvh_loose', 'mu10i_loose_g12Tvh_loose', 
         
        ###W T&P
        #'e15_etcutTrk_xs60_dphi2j15xe20',
        #'e15_etcutTrk_xs60',
        ### tau+em chains
        'tau20Ti_medium_e18vh_medium1',
        'tau50_medium1_e18vh_medium1',
        'tau38T_medium_e18vh_medium1',
        'tau50_medium_e15vh_medium1',
        ##Jet+METs
        'j110_a4tchad_xe65_tclcw',
        'j145_a4tchad_L2EFxe60_tclcw',
        'j170_a4tchad_EFxe50_tclcw',
        'j170_a4tchad_EFxe60_tclcw',
        'j110_a4tchad_xe100_tclcw',
        ##ZH
        'j80_a4tchad_xe55_tclcw',
        'j80_a4tchad_xe60_tclcw',
        'j80_a4tchad_xe70_tclcw',
        'j110_a4tchad_xe55_tclcw',
        'j110_a4tchad_xe60_tclcw',
        ##
        'j80_a4tchad_xe70_tclcw_loose',
        'j80_a4tchad_xe80_tclcw_loose',
        'j110_a4tchad_xe50_tclcw',
        'j110_a4tchad_xe60_tclcw_veryloose',
        'j110_a4tchad_xe60_tclcw_loose',
        'j110_a4tchad_xe70_tclcw_veryloose',
        'j110_a4tchad_xe60_tclcw',
        'j110_a4tchad_xe70_tclcw_loose',
        'j110_a4tchad_xe80_tclcw_loose',
        'j110_a4tchad_xe100_tclcw_veryloose',

        'j80_a4tchad_xe60_tclcw_loose',
        'j80_a4tchad_xe70_tclcw_veryloose',
        
        'b145_mediumEF_j145_a4tchad_L2EFxe60_tclcw',
        #'b55_mediumEF_j110_j55_a4tchad_xe60_tclcw',
        #'b45_mediumEF_j110_j45_a4tchad_xe60_tclcw',
        ##Fat Jets+MET
        'j180_a10tcem_EFxe50_tclcw',
        'j180_a10tclcw_EFxe50_tclcw',
        'j145_a10tcem_L2FS_L2xe60_tclcw',

        #E.P.
        #'b80_loose_j80_a4tchad_xe60_tclcw_loose',
        #'xe60_tclcw_loose_b35_mediumEF_j35_a4tchad',
        'j40_j20_a4tchad_xe60_tclcw',
        #'j40_a4tchad_xe60_tclcw_dphi2j45xe10',

        'mu24_j65_a4tchad_EFxe50_tclcw',
        
        ##Fat Jets+MET+b 
        'b180_loose_j180_a10tcem_L2j140_EFxe50_tclcw',
        'b180_medium_j180_a10tcem_L2j140_EFxe50_tclcw',
        #savannah 91719
        'j145_a4tchad_L2EFxe80_tclcw',

        #mu+jets with MU6_2J20 seed
        'mu4T_b55_medium_2j55_a4tchad_L1J20_matched',
        'mu4T_2b55_loose_2j55_a4tchad_L1J20_matched',
        'mu4T_b65_medium_2j65_a4tchad_matched',

        #L2loose        
        'tau18_medium1_L2loose_mu15',
        'tau29T_medium1_L2loose_xe50_tclcw_tight',
        'tau20Ti_medium1_L2loose_e18vh_medium1',

        #TauMass
        'mu20i_tight_g5_mugamma_TauMass',
        'mu10i_g10_mugamma_TauMass',
        'mu10i_loose_g12Tvh_mugamma_TauMass',
        'mu4Ti_g20Tvh_mugamma_TauMass',
        ###tau branch with L2 opt.
        'tau20_loose3_mu15',
        'tau18Ti_loose3_e18vh_medium1',
        'tau27T_loose3_xe50_tclcw_tight', 
        'tau20_medium1_mu15_vbf',
        'tau20_loose3_mu15_vbf',
        'tau20_loose2_mu15_vbf',
        'mu15_vbf_L1TAU8_MU10',
        'tau27Ti_loose3_tau18Ti_loose3_vbf',
        'tau18Ti_loose3_e18vh_medium1_vbf',

        #L2 pass trough
        'tau20Ti_loose1_L2NoCut_e18vh_medium1',
        'tau20Ti_loose2_L2NoCut_e18vh_medium1',
        'tau20Ti_medium1_L2NoCut_e18vh_medium1',
        'tau20T_loose1_L2NoCut_mu15',
        'tau20T_loose2_L2NoCut_mu15',
        'tau20T_medium1_L2NoCut_mu15',
        'tau29Ti_loose1_L2NoCut_xe55_tclcw',
        'tau29Ti_loose2_L2NoCut_xe55_tclcw',
        'tau29Ti_medium1_L2NoCut_xe55_tclcw',               
        ]

    TriggerFlags.CalibSlice.signatures = TriggerFlags.CalibSlice.signatures() + []
    TriggerFlags.L1CaloSlice.signatures = TriggerFlags.L1CaloSlice.signatures() + []
    TriggerFlags.CosmicSlice.signatures = TriggerFlags.CosmicSlice.signatures() + []
    TriggerFlags.GenericSlice.signatures = TriggerFlags.GenericSlice.signatures() + [
        ]
    TriggerFlags.MonitorSlice.signatures = TriggerFlags.MonitorSlice.signatures() + [
        #'InDetMon_FS_R1',
        ]
    TriggerFlags.BeamSpotSlice.signatures = TriggerFlags.BeamSpotSlice.signatures() + [
        'beamspot_vertex_FSTracks_L2Star_A',
        'beamspot_vertex_activeTE_L2Star_A_peb',
        'beamspot_vertex_allTE_L2Star_A_peb',
        'beamspot_vertex_FSTracks_L2Star_F',
        ]
    TriggerFlags.StreamingSlice.signatures = TriggerFlags.StreamingSlice.signatures() + []
    TriggerFlags.HeavyIonSlice.signatures = TriggerFlags.HeavyIonSlice.signatures() + []
    TriggerFlags.PerL1ItemStreamingSlice.signatures = TriggerFlags.PerL1ItemStreamingSlice.signatures() + []

    # Modify prescales for upgrade to disable slow chains
    # This is done inside setupMenu(), so we can dynamically get list of triggers enabled
    # instead of having large, out-of-date hardcoded list
    
    #get list of all signatures
    signatureList=[]
    for prop in dir(TriggerFlags):
        if prop[-5:]=='Slice':
            sliceName=prop
            slice=getattr(TriggerFlags,sliceName)
            if slice.signatures():
                signatureList.extend(slice.signatures())
            else:
                log.debug('SKIPPING '+str(sliceName))
    #print 'FOUND',len(signatureList),signatureList
    disablePatterns=[re.compile('beamspot'),  #all beamspot chains
                     re.compile('b\d+'),  #all b-jet chains
                     re.compile('mu.+_FS'),   #all muon EF fullscale
                     re.compile('^mu.+psi'),  #all single-muon J/psi/Upsilon
                     re.compile('^\d*e.+psi'),#all electron J/psi/Upsilon
                     re.compile('^e5'),       #all EM3 seeded electrons
                     re.compile('^g5'),       #all EM3 seeded electrons
                     re.compile('^e7'),       #all EM3 seeded electrons
                     re.compile('2mu4T.+e5_'),#all EM3 seeded electrons
                     re.compile('tauNoCut'),  #various tau NoCut chains
                     re.compile('InDetMon_FS'), #Cannot run fullscan tracking either
                     re.compile('Bmumux'),    #don't run the larger B-physics RoI tracking
                     re.compile('_matched'),  #all b-jet muon+jet chains
                     ]
    disableEFPatterns=[
                     re.compile('b\d+_mediumEF'),  #EF only b-jet chains
                     re.compile('b\d+_looseEF'),  #EF only b-jet chains
                     re.compile('mu.+EFFS'),  #all muon EF fullscale
                     re.compile('mu4T_j[123]5_a4tchad_matched'), #uses regular mu4T at L2
                     ]
    toDisable=[]
    toDisableEF=[]
    for signature in signatureList:
        for pattern in disableEFPatterns+disablePatterns:
            if re.search(pattern,signature):
                if pattern in disableEFPatterns:
                    toDisableEF.append(signature)
                else:
                    toDisable.append(signature)
                break
    log.debug('DISABLE The following chains are disabled in the v4 upgrade menu: '+str(toDisable))
    for signature in toDisable:
        Prescales.HLTPrescales_upgrade_mc_prescale[signature]=[   -1,    -1,    0,    0,  -1]
    for signature in toDisableEF:
        Prescales.HLTPrescales_upgrade_mc_prescale[signature]=[    1,    -1,    0,    0,  -1]
    
Prescales = physics_menu.Prescales
StreamConfig = physics_menu.StreamConfig
TriggerGroups = physics_menu.TriggerGroups
HigherGroups = physics_menu.HigherGroups

for key in Prescales.L1Prescales.keys():
    Prescales.L1Prescales[key] = 1

Prescales.L1Prescales_loose_mc_prescale = Prescales.L1Prescales
Prescales.HLTPrescales_loose_mc_prescale = Prescales.HLTPrescales

    #   Signature name        |
    #      [ L2prescale, EFprescale, L2pass-through, EF-pass-through, rerun]
Prescales.L1Prescales_tight_mc_prescale = Prescales.L1Prescales
Prescales.HLTPrescales_tight_mc_prescale = Prescales.HLTPrescales_tight_mc_prescale = {
    'hadCalib_trk9'                                : [    -1,    -1,    0,    0,  -1], 
    'hadCalib_trk9phi'                             : [    -1,    -1,    0,    0,  -1],  
    'hadCalib_trk18'                               : [    -1,    -1,    0,    0,  -1], 
    'hadCalib_trk9_L1HA8'                          : [    -1,    -1,    0,    0,  -1], 
    'hadCalib_trk18_L1HA8'                         : [    -1,    -1,    0,    0,  -1], 
    'hadCalib_trk9_L1RD0'                          : [    -1,    -1,    0,    0,  -1], 
    'hadCalib_trk18_L1RD0'                         : [    -1,    -1,    0,    0,  -1], 

    'trk9_Central_Tau_IDCalib'                     : [    -1,    -1,    0,    0,   -1],
    'trk9_Fwd_Tau_IDCalib'                         : [    -1,    -1,    0,    0,   -1],
    'trk16_Central_Tau_IDCalib'                    : [    -1,    -1,    0,    0,   -1],
    'trk16_Fwd_Tau_IDCalib'                        : [    -1,    -1,    0,    0,   -1],
    'trk29_Central_Tau_IDCalib'                    : [    -1,    -1,    0,    0,   -1],
    'trk29_Fwd_Tau_IDCalib'                        : [    -1,    -1,    0,    0,   -1],   

    'g12_larcalib'                                 : [    -1,    -1,    0,    0,   -1],
    'g20_larcalib'                                 : [    -1,    -1,    0,    0,   -1],
    'g40_larcalib'                                 : [    -1,    -1,    0,    0,   -1],
    'g60_larcalib'                                 : [    -1,    -1,    0,    0,   -1],
    'g80_larcalib'                                 : [    -1,    -1,    0,    0,   -1],
    'fj75_c4cchad_larcalib'                        : [    -1,    -1,    0,    0,   -1],
    
    'em3_empty_larcalib'                           : [    -1,    -1,    0,    0,   -1],
    'em6_empty_larcalib'                           : [    -1,    -1,    0,    0,   -1],
    'tau8_empty_larcalib'                          : [    -1,    -1,    0,    0,   -1],
    'j10_empty_larcalib'                           : [    -1,    -1,    0,    0,   -1],
    'fj30_empty_larcalib'                          : [    -1,    -1,    0,    0,   -1],     

    'tauNoCut_cosmic'                              : [    -1,    -1,    0,    0,  -1],
    'CosmicsAllTeTRTxK_TRTTrkHypo_Pixel'           : [    -1,    -1,    0,    0,  -1],
    'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics'      : [    -1,    -1,    0,    0,  -1],
    'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo' : [    -1,    -1,    0,    0,  -1],
    'CosmicsAllTeSiTrack_AllPhysics_TrkHypo'       : [    -1,    -1,    0,    0,  -1],
    'CosmicsAllTeIDSCAN_AllPhysics_TrkHypo'        : [    -1,    -1,    0,    0,  -1],
    'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo'  : [    -1,    -1,    0,    0,  -1],
    'Cosmics_MU11_TRTxK_TRTTrkHypo_Pixel'          : [    -1,    -1,    0,    0,  -1],
    'Cosmics_MU11_TRTxK_TRTTrkHypo_AllID'          : [    -1,    -1,    0,    0,  -1],
    'CosmicsAllTeEFID'                             : [    -1,    -1,    0,    0,  -1],
    'mu4T_cosmic'                                  : [    -1,    -1,    0,    0,  -1],
    'mu4_L1MU11_cosmic'                            : [    -1,    -1,    0,    0,  -1],
    'mu4_L1MU11_MSonly_cosmic'                     : [    -1,    -1,    0,    0,  -1],      

    # Generic monitoring
    'allL1_NoAlg'                                  : [   -1,   -1],
    'L1Calo_NoAlg'                                 : [   -1,   -1],
    'L1Muon_NoAlg'                                 : [   -1,   -1],
    'L1MinBias_NoAlg'                              : [   -1,   -1],
        
    'InDetMon_FS'                                  : [    100,    1,    0,    0],

    'Standby'                                      : [    -1,    -1,    0,    0,  -1],
    'PixelNoise'                                   : [    -1,    -1,    0,    0,    -1],
    'SCTNoise'                                     : [    -1,    -1,    0,    0,    -1],
    'cost_monitor'                                 : [    -1,    -1],                    
    'ROBRequestMon'                                : [    -1,    -1,    0,    0,    -1],
    'HLTTimeBurner'                                : [    -1 ,   -1,    0,    0,    -1],

    'rd0_filled_NoAlg'                             : [    1,      1,    0,    0,    -1],
    'mbSpTrkVtxMh_eff'                             : [    -1,    -1,    0,    0,    -1],
    'mbSpTrkVtxMh'                                 : [    -1,    -1,    0,    0,    -1],
    'mbSpTrkVtxMh_tight_eff'                       : [    -1,    -1,    0,    0,    -1],
    'mbSpTrkVtxMh_tight'                           : [    -1,    -1,    0,    0,    -1],
    'mbLucid_eff'                                  : [    -1,    -1,    0,    0,    -1],
    'mbTE20_eff'                                   : [    -1,    -1,    0,    0,    -1],
    'mbSpTrkVtxMh_medium_eff'                      : [    -1,    -1,    0,    0,    -1],
    'mbSpTrkVtxMh_medium'                          : [    -1,    -1,    0,    0,    -1],
    'mb_BCM_Wide_eff'                              : [    -1,    -1,    0,    0,    -1],
    'mbRd1_eff'                                    : [    -1,    -1,    0,    0,    -1],
    'mbZdc_eff'                                    : [    -1,    -1,    0,    0,    -1],
    'mbSpTrk'                                      : [    -1,    -1,    0,    0,    -1],
    'VdM_mbSpTrk'                                  : [    -1,    -1,    0,    0,    -1],
    'mbSpTrk_noiseSup'                             : [    -1,    -1,    0,    0,    -1],            
    'mbMbts_1_NoAlg'                               : [    -1,    -1,    0,    0,    -1],
    'mbMbts_2_NoAlg'                               : [    -1,    -1,    0,    0,    -1],
    'mbMbts_1_eff'                                 : [    -1,    -1,    0,    0,    -1],
    'L1TRT_NoAlg'                                  : [    -1,    -1,    0,    0,    -1], 
    'rd1_empty_NoAlg'                              : [    -1,    -1,    0,    0,    -1], 
    'zerobias_NoAlg'                               : [    -1,    -1,    0,    0,    -1],
    'zerobias_Overlay_NoAlg'                       : [    -1,    -1,    0,    0,    -1], 

    'eb_physics'                                   : [    -1,    -1,    0,    0,    -1],
    'eb_physics_empty'                             : [    -1,    -1,    0,    0,    -1],
    'eb_physics_firstempty'                        : [    -1,    -1,    0,    0,    -1],
    'eb_physics_noL1PS'                            : [    -1,    -1,    0,    0,    -1],
    'eb_physics_unpaired_iso'                      : [    -1,    -1,    0,    0,    -1],
    'eb_physics_unpaired_noniso'                   : [    -1,    -1,    0,    0,    -1],
    'high_eb_physics'                              : [    -1,    -1,    0,    0,    -1],

    'mu4T_cosmic'                                  : [    -1,    -1,    0,    0,    -1],
    'mu4_MSonly_cosmic'                            : [    -1,    -1,    0,    0,    -1],
    'mu4_L1MU11_cosmic'                            : [    -1,    -1,    0,    0,    -1],
    'mu4_L1MU11_MSonly_cosmic'                     : [    -1,    -1,    0,    0,    -1],
    'mu4_empty_NoAlg'                              : [    -1,    -1,    0,    0,    -1], 
    'mu11_empty_NoAlg'                             : [    -1,    -1,    0,    0,    -1], 
    'mu24_muCombTag_NoEF_tight'                    : [    -1,    -1,    0,    0,    -1],
    'mu10_muCombTag_NoEF_L1MU40'                   : [    -1,    -1,    0,    0,    -1],
    'mu15_muCombTag_NoEF_L1MU40'                   : [    -1,    -1,    0,    0,    -1],
    'mu20_muCombTag_NoEF_L1MU40'                   : [    -1,    -1,    0,    0,    -1],

    ##MET
    'xe_NoCut_allL1'                               : [     0,     0,    0,    0,    1],
    'xe_NoCut_tclcw_allL1'                         : [     0,     0,    0,    0,    1],
    'xe_NoCut_FEB_allL1'                           : [     0,     0,    0,    0,    1],
    '2e12Tvh_loose1'                               : [     1,     1,    0,    0,    1],
    'e7T_loose1'                                   : [     1,     1,    0,    0,    1],

    #Beamspot
    'beamspot_vertex_FSTracks_L2Star_A'            : [    -1,    -1,    0,    0,  -1], 
    'beamspot_vertex_activeTE_L2Star_A_peb'        : [    -1,    -1,    0,    0,  -1], 
    'beamspot_vertex_allTE_L2Star_A_peb'           : [    -1,    -1,    0,    0,  -1], 
    'beamspot_vertex_FSTracks_L2Star_B'            : [    -1,    -1,    0,    0,  -1], 
    'beamspot_vertex_activeTE_L2Star_B_peb'        : [    -1,    -1,    0,    0,  -1], 
    'beamspot_vertex_allTE_L2Star_B_peb'           : [    -1,    -1,    0,    0,  -1],
    #Bjets:
    'b15_looseEF_j15_a4tchad'                      : [    -1,    -1,    0,    0,  -1],
    'b25_looseEF_j25_a4tchad'                      : [    -1,    -1,    0,    0,  -1],
#    'b35_looseEF_j35_a4tchad'                      : [    -1,    -1,    0,    0,  -1],
#    'b45_looseEF_j45_a4tchad'                      : [    -1,    -1,    0,    0,  -1],
#    'b80_looseEF_j80_a4tchad'                      : [    -1,    -1,    0,    0,  -1],
    'mu4T_j15_a4tchad_matched'                     : [     1,  1000,    0,    0,  -1],
    'mu4T_j25_a4tchad_matched'                     : [     1,  1000,    0,    0,  -1],
    'mu4T_j35_a4tchad_matched'                     : [     1,     1,    0,    0,  -1],
    'mu4T_j15_a4tchad_matchedZ'                    : [     1,  1000,    0,    0,  -1],
    'mu4T_j25_a4tchad_matchedZ'                    : [     1,  1000,    0,    0,  -1],
    'mu4T_j35_a4tchad_matchedZ'                    : [     1,    1,    0,    0,  -1],
#    'mu4T_j45_a4tchad_matched'                     : [    -1,    -1,    0,    0,  -1],
#    'mu4T_j65_a4tchad_matched'                     : [    -1,    -1,    0,    0,  -1],
#    'mu4T_j80_a4tchad_matched'                     : [    -1,    -1,    0,    0,  -1],
    'b15_NoCut_j15_a4tchad'                        : [    -1,    -1,    0,    0,  -1],
#    'b35_NoCut_4j35_a4tchad_L2FS'                  : [    -1,    -1,    0,    0,  -1],
#    'b55_NoCut_j55_a4tchad_L2FS'                   : [    -1,    -1,    0,    0,  -1],

    'mu6_Jpsimumu_tight'                           : [    -1,    -1,    0,    0,    -1],
    #'mu6_Trk_Jpsi_loose'                           : [    -1,    -1,    0,    0,    -1],
    #'2mu4T_Jpsimumu_IDTrkNoCut'                    : [    -1,    -1,    0,    0,    -1],
    #'mu4Tmu6_Jpsimumu_IDTrkNoCut'                  : [    -1,    -1,    0,    0,    -1],
    #'mu4T_Trk_Jpsi'                                : [    -1,    -1,    0,    0,    -1],
    'mu10_Jpsimumu'                                : [    -1,    -1,    0,    0,    -1],
    'mu10_Upsimumu_tight_FS'                       : [    -1,    -1,    0,    0,    -1],
    'tau29_medium_2stTest'                         : [    -1,    -1,    0,    0,    -1],
    'e5_tight1_e5_NoCut'                           : [    -1,    -1,    0,    0,    -1],
    'e5_tight_e5_NoCut'                            : [    -1,    -1,    0,    0,    -1],
    'e5_tight1_e4_etcut_Jpsi_L2StarB'               : [    -1,    -1,    0,    0,    -1],
    'e5_tight1_e4_etcut_Jpsi_L2StarC'               : [    -1,    -1,    0,    0,    -1],
    'e5_tight1_e4_etcut_Jpsi_SiTrk'                 : [    -1,    -1,    0,    0,    -1],
    'e5_tight1_e4_etcut_Jpsi_IdScan'                : [    -1,    -1,    0,    0,    -1],

    
    'tau27Ti_medium1_L2loose'                : [    -1,    -1,    0,    0,    1],
    'tau18Ti_medium1_L2loose'                : [    -1,    -1,    0,    0,    1],
    'tau27T_medium1_L2loose'                 : [    -1,    -1,    0,    0,    1],
    'tau20_medium1_L2loose'                  : [    -1,    -1,    0,    0,    1],
 
    }

Prescales.L1Prescales_upgrade_mc_prescale = Prescales.L1Prescales
Prescales.HLTPrescales_upgrade_mc_prescale = deepcopy(Prescales.HLTPrescales_tight_mc_prescale)
# Note: "upgrade" prescales are set with regular expressions at the end
#       of the setupMenu() function above 
