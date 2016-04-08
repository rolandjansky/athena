# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TriggerMenuPython.Physics_pp_v2 as physics_menu

from AthenaCommon.Logging import logging
from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.TriggerPythonConfig import TriggerPythonConfig
from TriggerMenuPython.Lvl1Flags import Lvl1Flags, addToItemMap, addToThresholdMap
    
#---------------------------------------------------------------------
# Just copy the MC 10^31 menu with different prescale set
#---------------------------------------------------------------------
def setupMenu():
    physics_menu.setupMenu()
    
    TriggerFlags.Lvl1.items = TriggerFlags.Lvl1.items() + [
        'L1_2MU6_EM5',
        'L1_2TAU11I',
        'L1_2TAU6_EM5',  
        #'L1_4MU0',
        'L1_2EM3_EM10',
        'L1_2EM5_EM12',
        'L1_J50_XE40',
        'L1_2MU0_XE20',
        'L1_2MU0_XE25',
        ]

    TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
        '2mu10_loose1',
        '2mu10_noOvlpRm',
        '3mu6_MSonly_noOvlpRm',
        '2mu10_NoAlg',
        'mu0_empty_NoAlg',
        'mu0_unpaired_noniso_NoAlg',
        'mu4_mu6',
        'mu20_MSonly',
        'mu40_MSonly',
        'mu60_MSonly','mu80_MSonly','mu100_MSonly',
        'mu10_MG',
        #'2MUL1_l2j30_HV',
        '2MUL1_l2j30_HV_AllMS',
        'mu0_missingRoi',
        'mu4_unpaired_iso', 
        'mu4_firstempty',
        'mu6_l2cal',
        # EF based isolation test #83402
        'mu15ii_debug',
        ###
        #'mu10_MSonly_Ecut3','mu10_MSonly_Ecut2',
        #'mu10_Ecut3',       'mu10_Ecut2',
        'mu15_MG',
        # exotics request
        'mu20_mu7_EFFS', 'mu15_mu7_EFFS',
        'mu40_slow',
        'mu40_slow_empty',
        'mu40_slow_outOfTime',
        #
        'mu20_L2Star_IDTrkNoCut',
        'mu4_IDTrkNoCut',
        ]


    TriggerFlags.EgammaSlice.signatures = TriggerFlags.EgammaSlice.signatures() + [
        #electrons
        '2e5_medium',
        '2e5_medium_SiTrk',
        '2e5_medium_TRT',
        'e5_tight_TRT', 
        'e5_tight_e5_NoCut', 
        'e10_NoCut', ## introduced back in <-- just use e10_loose with a PT
        'e10_loose',
        'e15_loose',
        'e20_loose_TRT',
        'e25_medium',
        '2e15_loose',
        'e25_loose',
        'e30_loose',
        'em105_passHLT',
        'e15_tight1',
        'e18_medium',
        'e18_tight',
        'e20_tight',
        'e22_tight',
        '2e11T_medium',
        #photons
        'g100_loose',
        '2g10_loose',
        'g20_tight', 
        'g50_loose', 
        'g17_etcut',
        'g27_etcut', 
        'g22_hiptrt',
        #monitoring
        'e20_loose_IDTrkNoCut',
        # cosmic
        'e5_NoCut_firstempty',
	#Ringer and SW (requested to be here)
        'e5_NoCut_Ringer',
        'e5_NoCut_L2SW',
	#conversions
	'g20_loose_cnv',
        #jpsi
        'e5_tight_e5_NoCut_Jpsi',
        'e5_tight_e7_etcut_Jpsi', 
        #
        '2g50_etcut', 
        'g30_loose_g20_loose',
        'e30_medium', 
        #
        'e20_L2Star_IDTrkNoCut',
        '2e15vh_loose_Zee', '2e15vh_loose1_Zee',
#        'e15_loose_e12T_loose_Zee', 
#        'e25_loose2_xe20_noMu_recoil40',
        ]
    
    TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [
        'tauNoCut_firstempty',
        'tauNoCut_unpaired_iso',
        'tauNoCut_unpaired_noniso',
        #--> propagated from IBv3, for 1e30
        'tau20_loose', 
        # 1E31
        'tau38_loose','tau50_loose', # primary and support (Heavy Higgs, Exotics)
        'tau50_loose_IdScan',
        'tau38_medium', # primary and support, pileup (Heavy Higgs, Exotics)
        '2tau16_loose',
        #'2tau20_loose', # primary and support (Higgs, Z)
        # supporting and monitoring:
        'tau20_loose',        
        # 1E32 justifications same as above
        'tau125_loose',
        'tau84_medium', 
        '2tau29_loose',
        '2tau38_loose',
        '2tau29_loose1',
        '2tau29i_medium1',
        #'tauNoCut', # needs to be set as L2 pass_through item (PT flag needs to be changed)
        'L1TAU30_NoAlg',
        #
        'tau29_medium_tau38_medium', 
        'tau29_medium_tau38_loose',
        #'tau29_medium_tau20_loose_1prong', 'tau29_medium1_tau20_loose_1prong', 
        'tau29_loose_tau50_loose', 
        'tau29_loose_tau38_loose', 
        'tau20_loose_tau29_loose', 
        'tau100_loose',
        #'tau16_L2Star_IDTrkNoCut',
        'tau38_medium1_tau29_medium1',
        'tau38T_medium1_tau29T_medium1', 
        ]

    TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + [
        #nocut jets 
        'L1J175_NoAlg',
        'L1J250_NoAlg',
        ## v2 requests
        'j75_j30_a4tc_EFFS',
        #'2j135_j30_a4tc_EFFS', (#79562)
#        'j100_j55_j40_a4tc_EFFS',
        ##hidden valley
        #'l2j30_Trackless_HV',
        #new JE triggers
#        'je135_a4tc_EFFS', 'je195_a4tc_EFFS',
#        'je255_a4tc_EFFS', 'je350_a4tc_EFFS',
        'je420_NoEF','je500_NoEF',
        #dphi
        'j70_j25_dphi03_NoEF',
        'j75_j30_a4tc_EFFS_L2dphi04',
        'j100_j30_a4tc_EFFS_L2dphi04',
        'j135_j30_a4tc_EFFS_L2dphi04',
        #mct triggers
        'j75_j30_a4tc_EFFS_L2anymct100',
        #'j75_j30_a4tc_EFFS_L2leadingmct150',
        #'j75_j30_a4tc_EFFS_L2leadingmct175',
        'j75_j30_a4tc_EFFS_L2anymct150',
        'j75_j30_a4tc_EFFS_L2anymct175',
        ## NEW EF MCT
        'j75_j30_a4tc_EFFS_leadingmct150',
        ## multijets
        '6j40_a4tc_EFFS', '6j45_a4tc_EFFS',
        #stopped gluinos in non-coll BCID's
        'j30_cosmic','j50_cosmic',
        'j30_firstempty','j50_firstempty',
         # AntiKt algorithm
        'j35_a4tc_EFFS',
        'j45_a4tc_EFFS',         
        'j50_a4tc_EFFS',
#        'j95_a4tc_EFFS', 
        #HV+deta trigger w/ jet-EF
#       # 'j35_L1TAU_HV',
        #deta triggers
        #        '2j30_a4tc_EFFS_deta3_5', '2j30_a4tc_EFFS_deta5',
        #        'j55_j30_a4tc_EFFS_deta3_5', 'j55_j30_a4tc_EFFS_deta5',
        #        'j75_j30_a4tc_EFFS_deta3_5', 'j75_j30_a4tc_EFFS_deta5',
        #        'j100_j30_a4tc_EFFS_deta3_5','j100_j30_a4tc_EFFS_deta5',
        #'2j20_deta3_5',
        #        '2j20_deta5',
        #        '2j30_deta3_5', '2j35_deta3_5',
        #topo-cluster jets --> moving to Physics
        #        'j20_a4tc_EFFS', 'j30_a4tc_EFFS',
        #        'fj30_a4tc_EFFS',
        #        '3j30_a4tc_EFFS','4j30_a4tc_EFFS',
#        'je135_NoEF',
        'je195_NoEF',
        
        'je255_NoEF','je300_NoEF','je350_NoEF',
#         #Old style HV triggers
        'j35_a4tc_EFFS_L1TAU_HV',
        'j35_a4tc_EFFS_L1TAU_HV_firstempty', 'j35_a4tc_EFFS_L1TAU_HV_unpaired_iso', 'j35_a4tc_EFFS_L1TAU_HV_unpaired_noniso',
        'j35_a4tc_EFFS_L1TAU_HV_cosmic',
        ]        
    

    TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [
        'b10_medium_4L1J10',
        ###
        'b15_IDTrkNoCut', 'b20_IDTrkNoCut', 
        '2b10_medium_3L1J10',
        'b10_tight_L1JE100', 
        '2b10_medium_L1JE140',
        'mu4_L1J20_matched',
        'b10_tight_4j30_a4tc_EFFS',
        'mu4_j180_a4tc_EFFS_L1matched',
        'mu4_j20_a4tc_EFFS_matched',
        #
        'b10_tight_j75_j55_2j30_a4tc_EFFS',
        #
        '2b20_medium_3j45_a4tc_EFFS',
        '3b15_medium_4j40_a4tc_EFFS',
        '2b15_medium_3j40_a4tc_EFFS',
        '3b10_medium_4j30_a4tc_EFFS',
        '2b10_tight_4j30_a4tc_EFFS',
        '2b15_medium_j75_j40_a4tc_EFFS',
        '2b20_medium_j75_j45_a4tc_EFFS',
        '2b10_medium_j100_j30_a4tc_EFFS',
        #
        'b10_L2Star_IDTrkNoCut',
        ]
       
    TriggerFlags.BphysicsSlice.signatures = TriggerFlags.BphysicsSlice.signatures() + [
        'mu4_DiMu',
        'mu4_DiMu_noOS',
        'mu4_Jpsimumu',
        'mu4_Jpsimumu_FS',
        'mu4_Jpsimumu_tight',
        'mu6_Jpsimumu_tight',
        'mu4_Bmumu',
        'mu6_Upsimumu_FS',
        'mu6_DiMu',
        'mu6_Bmumu',
        'mu4_BmumuX',
        'mu6_BmumuX',
        'mu10_Upsimumu_FS',        
        'mu10_Jpsimumu',
        'mu4_Upsimumu_tight_FS',
        'mu6_Upsimumu_tight_FS',
        #supporting
        'mu4_DiMu_MG',
        'mu4_DiMu_MG_FS',
        'mu4_DiMu_SiTrk',
        'mu4_DiMu_SiTrk_FS',
        'mu4_DiMu_FS',
        #double object
        '2mu4_DiMu_noVtx',
        '2mu6_Bmumu',
        '2mu6_Jpsimumu',
        '2mu6_Upsimumu',
        '2mu6_Bmumux',
        #tracking monitoring
        'mu4mu6_Jpsimumu_IDTrkNoCut',
        #validation
        'mu4_Trk_Jpsi_loose',
        'mu4_Trk_Jpsi_looser',
        #DiMuon Triggers for 3e33 Menu Tests 
        'mu4mu6_DiMu_DY', 
        '2mu4i_DiMu_DY_noVtx_noOS','mu4imu6i_DiMu_DY_noVtx_noOS', 'mu4mu6_DiMu_DY_noVtx_noOS',
        ]

    TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + [
        'xe35_noMu',
        #xs
##         'xs15_noMu', 'xs20_noMu', 'xs22_noMu', 
##         'xs40_noMu', 'xs45_loose_noMu', 'xs80_loose_noMu',
##         'xs60_noMu', 'xs65_noMu', 'xs70_noMu', 'xs80_noMu',
        ]

    TriggerFlags.MinBiasSlice.signatures = TriggerFlags.MinBiasSlice.signatures() + [
        ]
    
    TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures()+[
        'tau29_loose_j45_a4tc_EFFS_xe40_noMu',
        # Combined slice
        'e20_loose_xe20_noMu',
        'e20_loose_xe30_noMu',        
        'e15_medium_xe35_noMu',
        'e15_medium_xe40_loose_noMu',
        'e15_tight_xe30_noMu',
        'e5_medium_mu4',
        'e10_loose_mu6',
        'e10_loose_mu10',        
        'g20_loose_xe20_noMu',
        'g20_loose_xe30_noMu',
        'g25_loose_xe30_noMu',
        # Combined taus:
        #--> propagated from IBv3 for 1e30:
        'tau16_loose_xe20_noMu', # primary and two support (W, Higgs, Exotics)
        # for 1e32:
        'tau16_loose_xe25_noMu',
        'tau16_loose_xe30_noMu',
        'tau16_loose_e10_loose','tau16_loose_e15_loose',# primary and support
        'tau16_loose_e15_tight',
        #'tau16_loose_2b15',
        'tau16_medium_xe22_noMu',
        'tau16_medium_xe25_noMu',
        'tau16_medium_xe25_tight_noMu',
        'g17_etcut_EFxe20_noMu',
        'g17_etcut_EFxe30_noMu',
#        'L1J55_EFxe20_noMu', 'L1J55_EFxe25_noMu', 'L1J55_EFxe30_noMu', 'L1J55_EFxe35_noMu', 'L1J55_EFxe40_noMu', 
        #tau comb items for rate pred 3-5E33
        'tau20_medium_e15_tight',
        'tau20T_medium_e15_tight',
        'tau29_loose_xs75_noMu',
        'tau29T_medium_xs70_noMu',
        'tau29T_loose_xs75_noMu',
        ###
        ###
        # Bphysics and others
        #'mu4_j20',
        'mu4_j30',
#        'mu10_j30',
#        'l2j25_mv_mbsptrk',
#        'l2j30_mv_mbsptrk',
        # Combined slice
        #'e5_loose_mu4', needs MU0_EM3, n/a
        'e10_loose_mu6',
        'e10_loose_mu10',        
        'e20_loose_xe20',
        'e20_loose_xe30',
#        '2j35_xe20_noMu',
#        '2j35_xe30_noMu',
#        '2j35_xe40_noMu',
#        'j50_xe20_noMu',
#        'j50_xe30_noMu',
#        'j50_xe40_noMu',
        'g20_loose_xe20_noMu',
        'g20_loose_xe30_noMu',
        'g27_etcut_EFxe30_noMu',
        # Combined taus:
        '2tau29_loose1_EFxe15_noMu', '2tau29_loose1_EFxe20_noMu', 
        #--> propagated from IBv3 for 1e30:
        # for 1e32:
        #'tau16_loose_3j35',
        #
        #'4j30_mu15',
#        '4j30_e15_medium',
        '2mu6_e10_loose',
        '2mu6_g10_loose',
#        'j35_xe30_e15_medium',
#        'j35_xe30_mu15',
        #
        #'tau38_loose_xe30_noMu', 
        'tau29_loose_e15_tight',
        #'tau20_loose_e15_medium',
        'tau20_loose_mu15', 'tau29_loose_mu15', 
#        'tau29_loose_tau38_loose_j30', 
#        'tau29_loose_xe30_noMu', 
        #'tau29_loose_xe35_noMu',
        #'tau16_medium_e15_tight',
        'tau16_medium_e15_medium',
        'tau16_medium_mu15',
        #'tau16_medium_e15_tight_j20', 'tau16_medium_mu15_j20', 
        #'tau16_medium_e15_tight_j30', #'tau16_medium_mu15_j30', 
        'tau16_loose_mu15', 
        #savannah 76221
        'e15_medium_mu6',
        #savannah 77137
        'g40_loose_EFxe35_noMu',
        'g40_loose_xe40_noMu',
        'g40_loose_xs35_noMu',
        #jet+met requested for MC_pp_v2
        #'j75_a4_EFFS_EFxe40_noMu',
#        'j100_a4_EFFS_EFxe65_noMu',
#        'j75_a4_EFFS_xe65_loose_noMu',
#        '2j45_a4_EFFS_xe55_loose_noMu',
#        '2j45_a4_EFFS_xe70_loose_noMu',
#        '2j40_a4_EFFS_xe45_noMu_L1XE30',
#        '3j45_a4_EFFS_EFxe40_noMu',
#        '3j55_a4_EFFS_EFxe50_noMu',
        'j75_a4tc_EFFS_xe40_loose_noMu_dphijxe03',
        'j55_a4tc_EFFS_xe55_medium_noMu_dphi2j30xe10',
        # je+met
#        'je195_NoEF_EFxe50_noMu',
#        'je255_NoEF_EFxe45_noMu',
#        'je255_NoEF_EFxe50_noMu',
#        'je255_NoEF_EFxe60_noMu',
        # jet+xs
#        'j75_a4_EFFS_xs25_noMu',
        'j75_a4tc_EFFS_xs35_noMu',
#        'j75_a4tc_EFFS_L1XS_EFxe45_noMu',
#        'j75_a4_EFFS_L1XS_EFxe55_noMu',
        # tau+jet+met
        'tau16_j75_a4tc_EFFS_xe40_loose_noMu',
        # exotic request
        '2mu6_MSonly_g10_loose_noOvlpRm',
        'mu15_EFxe45',
        'mu15_j90_a4EFFS',
        #'mu15_3j20_j70_a4EFFS',
        #'mu15_3j20_j90_a4EFFS',
        #
        'e15_tight_xe30_noMu',
        #savannah : 76270
        'tau29_medium_xs80_loose_noMu',
        'tau29_medium_xs45_loose_noMu_3L1J10',
        #
        #'e5_medium_mu4_topoAngle_medium', 'e5_tight_mu4_topoAngle_medium', 
        #'e5_medium_mu4_topoAngle_tight', 'e5_tight_mu4_topoAngle_tight',
        'e5_medium_mu6_topo_loose', 
        #
        'b10_medium_EFxe25_noMu_L1JE100', 
        'b10_medium_EFxe25_noMu_L1_2J10J50', 
        # '3j20_a4tc_EFFS_e20_loose',
        # elec+Jet
        'e20_loose_2j10_a4tc_EFFS', 'e20_loose_3j10_a4tc_EFFS', 'e20_loose_4j10_a4tc_EFFS',
        # Test EM+XS
        'g13_etcut_xs60_noMu',  'e13_etcutTRT_xs60_noMu',
        'g40_loose_b10_medium',
        'MU0_j45_a4tc_EFFS_xe45_loose_noMu',  
        'tau29_loose_xs60_loose_noMu_3L1J10',
        'mu15_medium_EFxs65_noMu', 'mu15_medium_EFxs70_noMu',
        'mu15_medium_EFxs75_noMu',
        'tau29_loose_xs65_loose_noMu',  'tau29_loose_xs75_loose_noMu',
        #
        'g40_tight_b15_medium',
        'e18_medium_xe30_noMu', 'e15_medium_xe35_loose_noMu',
        'mu15_xe35_noMu',
        # susy items
        '2mu4_EFxe20_noMu', '2mu4_xe30_noMu', '2mu4_xe40_noMu', 
        ]
       
    TriggerFlags.CalibSlice.signatures              = TriggerFlags.CalibSlice.signatures() + []
    TriggerFlags.CosmicSlice.signatures             = TriggerFlags.CosmicSlice.signatures() + []
    TriggerFlags.GenericSlice.signatures            = TriggerFlags.GenericSlice.signatures() + []
    TriggerFlags.MonitorSlice.signatures            = TriggerFlags.MonitorSlice.signatures()  + []
    TriggerFlags.BeamSpotSlice.signatures           = TriggerFlags.BeamSpotSlice.signatures() + []
    TriggerFlags.StreamingSlice.signatures          = TriggerFlags.StreamingSlice.signatures() + []
    TriggerFlags.HeavyIonSlice.signatures           = TriggerFlags.HeavyIonSlice.signatures() + []
    TriggerFlags.PerL1ItemStreamingSlice.signatures = TriggerFlags.PerL1ItemStreamingSlice.signatures() + []
  
     
Prescales = physics_menu.Prescales
StreamConfig = physics_menu.StreamConfig
TriggerGroups = physics_menu.TriggerGroups
HigherGroups = {
    'RATE:FS': [#'mu4_L2MSonly_EFFS_passL2',
                #'mu4_MSonly_EFFS_passL2',
                #'mu4_MSonly_MB2_noL2_EFFS',
                'mu6_Upsimumu_FS',
                'mu4_DiMu_MG_FS',
                'mu4_DiMu_SiTrk_FS',
                'mu4_DiMu_FS',
                'L2IdScan_FS',
                'L2SiTrack_FS',
                'EFInsideOut_FS',
                'EFTRTOnly_FS',
                'InDetMon_FS',
                'beamspot_vertex_FSTracks_SiTrk',
                ]
    }

        
if True: # trigger for MC production with prescale on some chains
    Prescales.L1Prescales = {
        'L1_EM3':    1, 
        'L1_EM5':    1,
        'L1_EM10':   1, 
        'L1_EM10I':  1, 
        'L1_EM14':   1, 
        'L1_EM14I':  1, 
        'L1_EM19I':  1, 
        'L1_EM85':   1, 
        'L1_2EM3':   1, 
        'L1_2EM5':   1,
        'L1_2EM10':  1,
        'L1_2EM14':  1, 
        'L1_2EM14I': 1, 
        'L1_2EM19I': 1, 
        'L1_2EM3_EM5': 1,
        'L1_3EM10': 1, 
        'L1_MU0': 1, 
        'L1_MU6': 1, 
        'L1_MU10': 1, 
        'L1_MU15': 1, 
        'L1_MU20': 1, 
        'L1_MU40': 1, 
        'L1_2MU0': 1, 
        'L1_2MU0_MU6': 1, 
        'L1_2MU6': 1, 
        'L1_2MU10': 1, 
        'L1_2MU15': 1, 
        'L1_2MU20': 1, 
        'L1_3MU0': 1,
        'L1_4MU0': 1,
        'L1_3MU6': 1, 
        'L1_TAU5': 1, 
        'L1_TAU6': 1, 
        'L1_TAU6I': 1, 
        'L1_TAU11': 1, 
        'L1_TAU11I': 1, 
        'L1_TAU20': 1, 
        'L1_2TAU6': 1, 
        'L1_2TAU11I': 1, 
        'L1_2TAU11': 1, 
        'L1_J5': 1, 
        'L1_J10': 1, 
        'L1_J15': 1, 
        'L1_J30': 1, 
        'L1_J55': 1, 
        'L1_J75': 1, 
        'L1_3J10': 1, 
        'L1_3J15': 1, 
        'L1_3J30': 1, 
        'L1_4J10': 1, 
        'L1_4J15': 1, 
        'L1_4J30': 1, 
        'L1_3J10_J30': 1, 
        'L1_3J15_J30': 1, 
        'L1_4J10_3J15': 1, 
        'L1_FJ5': 1, 
        'L1_FJ10': 1, 
        'L1_FJ15': 1, 
        'L1_FJ30': 1, 
        'L1_2FJ5': 1, 
        'L1_XE10': 1, 
        'L1_XE15': 1, 
        'L1_XE20': 1, 
        'L1_XE25': 1, 
        'L1_XE30': 1, 
        'L1_XE35': 1, 
        'L1_TE10': 1, 
        'L1_TE30': 1, 
        'L1_TE100': 1, 
        'L1_TE180': 1, 
        'L1_EM14_XE10': 1, 
        'L1_2EM5_MU6': 1,
        'L1_2EM10_MU6': 1, 
        'L1_2EM14_MU6': 1, 
        'L1_EM5_MU6': 1, 
        'L1_2EM3_TAU6': 1, 
        'L1_EM10_MU15': 1, 
        'L1_EM10I_MU15': 1, 
        'L1_2TAU5_EM5': 1, 
        'L1_2TAU5_TAU6_EM5': 1, 
        'L1_2TAU6I_EM10I': 1, 
        'L1_MU15_XE10': 1, 
        'L1_MU20_XE15': 1, 
        'L1_MU0_J5': 1, 
        'L1_MU0_J10': 1, 
        'L1_MU0_J15': 1, 
        'L1_MU0_J30': 1, 
        'L1_MU0_J55': 1, 
        'L1_MU6_J5': 1, 
        'L1_MU10_J5': 1, 
        'L1_MU0_EM3': 1, 
        'L1_MU6_EM3': 1, 
        'L1_2MU6_EM5': 1,
        'L1_TAU6_MU6': 1, 
        'L1_TAU6_MU10': 1, 
        'L1_J30_XE15': 1, 
        'L1_2J15_XE15': 1, 
        'L1_J15_XE15_EM10I': 1, 
        'L1_4J10_EM10I': 1,
        'L1_4J10_MU15': 1,
        'L1_TAU11_XE20': 1, 
        'L1_TAU11_XE10': 1, 
        'L1_TAU6I_2J5_J30': 1, 
        'L1_TAU6I_5J5_4J15': 1, 
        'L1_TAU6I_3J5_2J15': 1, 
        'L1_RD0_FILLED': 1, 
        'L1_MBTS_1': 1, 
        'L1_MBTS_2': 1, 
        'L1_MBTS_1_1': 1, 
        'L1_LUCID_A': 1, 
        'L1_LUCID_C': 1, 
        'L1_LUCID_A_C': 1, 
        'L1_LUCID': 1, 
#        'L1_J10_MV': 1, 
        'L1_TAU5_XE10' : 1, 
        'L1_TAU6I_XE10': 1, 
        'L1_ZDC_A': 1, 
        'L1_ZDC_C': 1, 
        'L1_ZDC_AND': 1, 
        'L1_ZDC_A_EMPTY': 1, 
        'L1_ZDC_C_EMPTY': 1, 
        'L1_ZDC_AND_EMPTY': 1, 
        'L1_ZDC_EMPTY': 1, 
        }

    #Copied from Physics_pp_v1.py
    Prescales.HLTPrescales = {
        # Need to have rerun=1:
        'xe_NoCut_allL1'        : [    0,    0,    0,    0,  1],  # = any event passed by L1 calo, muon, minbias, random triggers, followed by L2_xe30 in pass-through and EF_xe30 with rerun=1
        'xe30_allL1'            : [    0,    0,    0,    0,  1],  # = any event passed by L1 calo, muon, minbias, random triggers, followed by L2_xe30 in pass-through and EF_xe30 with rerun=1
        'xe30_allL1_allCells'   : [    0,    0,    0,    0,  1],  # (same as above, with noise suppression at EF)
        'xe30_allL1_FEB'        : [    0,    0,    0,    0,  1],  # (same as above, with FEB headers at EF)
        'xe30_unbiased'        : [    0,    0,    0,    0,  1],# = L1_random followed by L2_xe30 in pass-through and EF_xe30 with rerun=1
        'xe30_unbiased_allCells': [    0,    0,    0,    0,  1], #  (same as above, with noise suppression at EF)
        'xe30_unbiased_FEB'     : [    0,    0,    0,    0,  1], # (same as above, with FEB headers at EF)
        #####
        'tau16_loose'        : [    1,    1,    0,    0,  1],
        'tau29_loose'        : [    1,    1,    0,    0,  1],
        'tau29_medium'       : [    1,    1,    0,    0,  1],
        # Generic monitoring
        'allL1_NoAlg'              : [   -1,   -1],
        'L1Calo_NoAlg'             : [   -1,   -1],
        'L1Muon_NoAlg'             : [   -1,   -1],
        'L1MinBias_NoAlg'          : [   -1,   -1],
        #cosmic chains
        'CosmicsAllTeTRTxK_TRTTrkHypo_Pixel'              : [    -1,    -1,    0,    0],
        'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics'         : [    -1,    -1,    0,    0],
        'CosmicsAllTeIDSCAN_AllPhysics_TrkHypo'           : [    -1,    -1,    0,    0],
        'CosmicsAllTeSiTrack_AllPhysics_TrkHypo'          : [    -1,    -1,    0,    0],
        'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo'     : [    -1,    -1,    0,    0],
        'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo'    : [    -1,    -1,    0,    0],
        'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField' : [    -1,    -1,    0,    0],
        'Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_Pixel'         : [    -1,    -1,    0,    0],
        'Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_AllID'         : [    -1,    -1,    0,    0],
        'CosmicsAllTeEFID'                                : [    -1,    -1,    0,    0],

        'PreS_tight': [    1,    1,    1000,    0],
        }
    
    # prescale set for MC production to enable only those chains in the physics menu
    Prescales.L1Prescales_tight_mc_prescale = {
        'L1_EM3': 1, 
        'L1_EM5': 1,
        'L1_EM7': 1, 
        'L1_EM10': 1, 
        'L1_EM12': 1, 
        'L1_EM14': 1, 
        'L1_EM16': 1, 
        'L1_EM30': 1, 
        'L1_2EM3': 1,
        'L1_2EM5': 1, 
        'L1_2EM7': 1, 
        'L1_2EM14': 1, 
        'L1_2EM10': 1, 
        'L1_3EM5': 1, 
        'L1_2EM3_EM7': 1, 
        'L1_MU0': 1, 
        'L1_MU6': 1, 
        'L1_MU10': 1, 
        'L1_MU11': 1, 
        'L1_MU15': 1, 
        'L1_MU20': 1, 
        'L1_2MU0': 1, 
        'L1_2MU6': 1, 
        'L1_2MU10': 1, 
        'L1_3MU0': 1,
        'L1_4MU0': 1, 
        'L1_3MU6': 1,
        'L1_TAU5': 1, 
        'L1_TAU6': 1, 
        'L1_TAU8': 1, 
        'L1_TAU50': 1, 
        'L1_TAU11': 1, 
        'L1_TAU20': 1,
        'L1_TAU30': 1, 
        'L1_2TAU6': 1, 
        'L1_2TAU11': 1, 
        'L1_J10': 1, 
        'L1_J15': 1, 
        'L1_J20': 1, 
        'L1_J30': 1, 
        'L1_J50': 1, 
        'L1_J75': 1, 
        'L1_J175': 1,
        'L1_J250': 1, 
        'L1_2J20': 1, 
        'L1_3J10': 1, 
        'L1_3J15': 1,
        'L1_3J20': 1,
        'L1_3J50': 1,
        'L1_3J75': 1, 
        'L1_4J10': 1, 
        'L1_4J30': 1, 
        'L1_5J10': 1,
        'L1_5J20': 1,
        'L1_6J10': 1, 
        'L1_3J10_J50': 1,
        'L1_4J15': 1,
        'L1_2J10_J50': 1,
        'L1_FJ10': 1, 
        'L1_FJ30': 1, 
        'L1_FJ50': 1,
        'L1_FJ75': 1, 
        'L1_2FJ10': 1,
        'L1_2FJ30': 1,
        'L1_2FJ50': 1,
        'L1_J10_FJ10': 1,
        'L1_J30_FJ30': 1,
        'L1_J50_FJ50': 1,
        'L1_JE60': 1,
        'L1_JE100': 1,
        'L1_JE140': 1,
        'L1_JE200': 1,
        'L1_XE10': 1,
        'L1_XE60': 1,
        'L1_XE20': 1,
        'L1_XE25': 1,
        'L1_XE30': 1,
        'L1_XE35': 1,
        'L1_XE40': 1,
        'L1_XE50': 1,
        'L1_TE20': 1,
        'L1_TE180': 1,
        'L1_TE300': 1,
        'L1_TE400': 1,
        'L1_TE500': 1,
        'L1_TE600': 1,
        'L1_TE700': 1,
        'L1_TE800': 1,
        'L1_ZB' : -1,
        'L1_BPTX0_BGRP0': -1,
        'L1_BPTX1_BGRP0': -1,
        'L1_CALREQ0': -1,
        'L1_CALREQ1': -1,
        'L1_CALREQ2': -1,
        'L1_NIM_S9C1C19': -1,
        'L1_NIM_S9C1C20': -1,
        'L1_TRT': -1,
        'L1_L1A_Mon': -1,
        'L1_MBLZ': -1,
        }

#Tight prescales for high-PT production
Prescales.HLTPrescales_tight_mc_prescale = {  
    
    #muon   
    'mu20_IDTrkNoCut'   : [    10,    1,    0,    0,    -1],
    'mu10_l2cal'   : [    -1,    -1,    0,    0,    -1],
    'mu20_muCombTag_NoEF'   : [    -1,    -1,    0,    0,    -1],
    'mu4_cosmic'   : [    -1,    -1,    0,    0,    -1], 
    'mu4_MSonly_cosmic'   : [    -1,    -1,    0,    0,    -1],
    'mu4_L1MU11_cosmic'   : [    -1,    -1,    0,    0,    -1], 
    'mu4_L1MU11_MSonly_cosmic'   : [    -1,    -1,    0,    0,    -1],
    'L1Calo_NoAlg'   : [    -1,    -1,    0,    0,    -1],
    'SeededStreamerL1CaloXS'   : [    -1,    -1,    0,    0,    -1],
    'Standby'   : [    -1,    -1,    0,    0,    -1], 
    'Background'   : [    -1,    -1,    0,    0,    -1],
    'mu0_firstempty_NoAlg'   : [    -1,    -1,    0,    0,    -1],
    'mu0_unpaired_iso_NoAlg'   : [    -1,    -1,    0,    0,    -1],
    'mu0_empty_NoAlg'   : [    -1,    -1,    0,    0,    -1],
    'mu11_empty_NoAlg'   : [    -1,    -1,    0,    0,    -1],
    '2mu10_noOvlpRm'   : [    -1,    -1,    0,    0,    -1],
    '3mu6_MSonly_noOvlpRm'   : [    -1,    -1,    0,    0,    -1],
    'mu0_empty_NoAlg'   : [    -1,    -1,    0,    0,    -1],
    'mu0_unpaired_noniso_NoAlg'   : [    -1,    -1,    0,    0,    -1],
    'mu0_missingRoi'   : [    -1,    -1,    0,    0,    -1],
    'mu4_unpaired_iso'   : [    -1,    -1,    0,    0,    -1], 
    'mu4_firstempty'   : [    -1,    -1,    0,    0,    -1],
    'mu6_l2cal'   : [    -1,    -1,    0,    0,    -1],
    'mu10_MSonly_Ecut3'   : [    -1,    -1,    0,    0,    -1],
    'mu10_MSonly_Ecut2'   : [    -1,    -1,    0,    0,    -1],
    'mu10_Ecut3'   : [    -1,    -1,    0,    0,    -1],
    'mu10_Ecut2'   : [    -1,    -1,    0,    0,    -1],
    #egamma
    '2e5_medium': [    -1,    -1,    0,    0,    -1],
    '2e5_medium_SiTrk': [    -1,    -1,    0,    0,    -1],
    '2e5_medium_TRT': [    -1,    -1,    0,    0,    -1],
    'e5_tight': [    -1,    -1,    0,    0,    -1], 
    'e5_tight_TRT': [    -1,    -1,    0,    0,    -1], 
    'e5_tight_e5_NoCut': [    -1,    -1,    0,    0,    -1], 
    'e10_NoCut': [    -1,    -1,    0,    0,    -1],
    'e20_loose_TRT': [    -1,    -1,    0,    0,    -1],
    'e20_loose_IDTrkNoCut': [    10,    1,    0,    0,    -1],
    'e5_NoCut_firstempty': [    -1,    -1,    0,    0,    -1],
    'e5_tight_e5_NoCut_Jpsi': [    -1,    -1,    0,    0,    -1],
    'e5_tight_e4_etcut_Jpsi': [    -1,    -1,    0,    0,    -1],
    'e5_tight_e7_etcut_Jpsi': [    -1,    -1,    0,    0,    -1],
    'e5_tight_e9_etcut_Jpsi': [    -1,    -1,    0,    0,    -1],
    'e5_tight_e4_etcut_Jpsi_SiTrk': [    -1,    -1,    0,    0,    -1],
    'e5_tight_e4_etcut_Jpsi_TRT': [    -1,    -1,    0,    0,    -1],
    '2e5_tight'   : [    -1,    -1,    0,    0,    -1],
    '2e5_tight_Jpsi'   : [    -1,    -1,    0,    0,    -1],
    'e9_tight_e5_tight_Jpsi'   : [    -1,    -1,    0,    0,    -1],
    'e20_medium_SiTrk'   : [    -1,    -1,    0,    0,    -1],
    'e20_medium_TRT'   : [    -1,    -1,    0,    0,    -1],
    'e20_medium_IDTrkNoCut'   : [    10,    1,    0,    0,    -1],
    'g5_NoCut_cosmic'   : [    -1,    -1,    0,    0,    -1],
    #tau
    'tauNoCut_firstempty'   : [    -1,    -1,    0,    0,    -1],
    'tauNoCut_unpaired_iso'   : [    -1,    -1,    0,    0,    -1],
    'tauNoCut_unpaired_noniso'   : [    -1,    -1,    0,    0,    -1],
    'tau12_IDTrkNoCut'   : [    10,    1,    0,    0,    -1],
    'tau16_IDTrkNoCut'   : [    10,    1,    0,    0,    -1],
    'L1TAU30_NoAlg'   : [    -1,    -1,    0,    0,    -1],
    'tauNoCut'   : [    1,     1,    1,    1,    -1],
    'tau50_IDTrkNoCut'   : [    10,    1,    0,    0,    -1],
    'tauNoCut_cosmic'   : [    -1,    -1,    0,    0,    -1],
    #jet
    'j10_empty_NoAlg'   : [    -1,    -1,    0,    0,    -1],
    'j30_empty_NoAlg'   : [    -1,    -1,    0,    0,    -1],
    'fj10_empty_NoAlg'   : [    -1,    -1,    0,    0,    -1],

    'je135_NoEF'   : [    -1,    -1,    0,    0,    -1],
    'je195_NoEF'   : [    -1,    -1,    0,    0,    -1],
    'je255_NoEF'   : [    -1,    -1,    0,    0,    -1],
    'je300_NoEF'   : [    -1,    -1,    0,    0,    -1],
    'je350_NoEF'   : [    -1,    -1,    0,    0,    -1],
    'je420_NoEF'   : [    -1,    -1,    0,    0,    -1],
    'je500_NoEF'   : [    -1,    -1,    0,    0,    -1],
    'j30_cosmic'   : [    -1,    -1,    0,    0,    -1],
    'j50_cosmic'   : [    -1,    -1,    0,    0,    -1],
    'j30_firstempty'   : [    -1,    -1,    0,    0,    -1],
    'j50_firstempty'   : [    -1,    -1,    0,    0,    -1],
    'j50_unpaired'   : [    -1,    -1,    0,    0,    -1],
    #bjet
    'b10_IDTrkNoCut'   : [    10,    1,    0,    0,    -1],
    'b15_IDTrkNoCut'   : [    10,    1,    0,    0,    -1],
    'b20_IDTrkNoCut'   : [    10,    1,    0,    0,    -1],
    'b10_EFj10_a4tc_EFFS_IDTrkNoCut'   : [    10,    1,    0,    0,    -1],
    #bphysics
    'mu4_Upsimumu_FS'   : [    -1,    -1,    0,    0,    -1],
    '2mu4_DiMu_noVtx_noOS'   : [    -1,    -1,    0,    0,    -1],
    'mu4_Jpsimumu_FS'   : [    -1,    -1,    0,    0,    -1],
    'mu4_Upsimumu_SiTrk_FS'   : [    -1,    -1,    0,    0,    -1],
    'mu4_Upsimumu_tight_FS'   : [    -1,    -1,    0,    0,    -1],
    'mu6_Upsimumu_FS'   : [    -1,    -1,    0,    0,    -1],
    'mu6_Upsimumu_tight_FS'   : [    -1,    -1,    0,    0,    -1],
    'mu10_Upsimumu_FS'   : [    -1,    -1,    0,    0,    -1],
    'mu10_Upsimumu_tight_FS'   : [    -1,    -1,    0,    0,    -1],
    'mu4_DiMu_FS_noOS'   : [    -1,    -1,    0,    0,    -1],
    'mu4_DiMu_MG'   : [    -1,    -1,    0,    0,    -1],
    'mu4_DiMu_MG_FS'   : [    -1,    -1,    0,    0,    -1],
    'mu4_DiMu_SiTrk'   : [    -1,    -1,    0,    0,    -1],
    'mu4_DiMu_SiTrk_FS'   : [    -1,    -1,    0,    0,    -1],
    '2mu4_DiMu_SiTrk'   : [    -1,    -1,    0,    0,    -1],
    'mu4_DiMu_FS'   : [    -1,    -1,    0,    0,    -1],
    '2mu4_DiMu_noVtx'   : [    -1,    -1,    0,    0,    -1],
    '2mu4_Jpsimumu_IDTrkNoCut'   : [    10,    1,    0,    0,    -1],
    'mu4mu6_Jpsimumu_IDTrkNoCut'   : [    10,    1,    0,    0,    -1],
    'mu4_Trk_Upsi_FS'   : [    -1,    -1,    0,    0,    -1],
    #met
    'xe_NoCut_allL1'   : [    1,    1,    0,    0,    1],
    #min bias
    'mbMbts_2_SpTrkMh'   : [    -1,    -1,    0,    0,    -1],
    'mbMbts_2_SpTrkMh120'   : [    -1,    -1,    0,    0,    -1],
    'mbTE20_eff'   : [    -1,    -1,    0,    0,    -1],
    'mbSpTrkVtxMh'   : [    -1,    -1,    0,    0,    -1],
    'mbSpTrkVtxMh_eff'   : [    -1,    -1,    0,    0,    -1],
    'mbSpTrkVtxMh_medium'   : [    -1,    -1,    0,    0,    -1],
    'mbSpTrkVtxMh_medium_eff'   : [    -1,    -1,    0,    0,    -1],
    'mbSpTrkVtxMh_tight'   : [    -1,    -1,    0,    0,    -1],
    'mbSpTrkVtxMh_tight_eff'   : [    -1,    -1,    0,    0,    -1],
    'rd0_filled_NoAlg'   : [    1,    1,    0,    0,    -1],
    'mbRmMbts_1_NoCut_Time'   : [    -1,    -1,    0,    0,    -1],
    'mbLucid_eff'   : [    -1,    -1,    0,    0,    -1],
    'mbMbts_1_eff'   : [    -1,    -1,    0,    0,    -1],
    'mbZdc_eff'   : [    -1,    -1,    0,    0,    -1],
    'mbSpTrk'   : [    -1,    -1,    0,    0,    -1],
    'mb_BCM_Wide_eff'   : [    -1,    -1,    0,    0,    -1],
    'mbSpBg_unpaired_iso'   : [    -1,    -1,    0,    0,    -1],
    'mbRd1_eff'   : [    -1,    -1,    0,    0,    -1],
    'mbRd0_eff_unpaired_iso'   : [    -1,    -1,    0,    0,    -1],
    'mbSpTrk_unpaired_iso'   : [    -1,    -1,    0,    0,    -1],
    'L1TRT_NoAlg'   : [    -1,    -1,    0,    0,    -1],
    'rd0_empty_NoAlg'   : [    -1,    -1,    0,    0,    -1],
    'rd1_empty_NoAlg'   : [    -1,    -1,    0,    0,    -1],
    'zerobias_NoAlg'   : [    -1,    -1,    0,    0,    -1],
    #calib
    'g20_larcalib'   : [    -1,    -1,    0,    0,    -1],
    'g40_larcalib'   : [    -1,    -1,    0,    0,    -1],
    'g50_larcalib'   : [    -1,    -1,    0,    0,    -1], 
    'trk6_IDSCAN_IDCalib'   : [    -1,    -1,    0,    0,    -1],
    'trk9_Central_Tau_IDCalib'   : [    -1,    -1,    0,    0,    -1],
    'trk9_Fwd_Tau_IDCalib'   : [    -1,    -1,    0,    0,    -1],
    'trk16_Central_Tau_IDCalib'   : [    -1,    -1,    0,    0,    -1],
    'trk16_Fwd_Tau_IDCalib'   : [    -1,    -1,    0,    0,    -1],
    'trk29_Central_Tau_IDCalib'   : [    -1,    -1,    0,    0,    -1],
    'trk29_Fwd_Tau_IDCalib'   : [    -1,    -1,    0,    0,    -1],
    'hadCalib_trk9'   : [    -1,    -1,    0,    0,    -1],
    'hadCalib_trk9phi'   : [    -1,    -1,    0,    0,    -1],
    'em3_empty_larcalib'   : [    -1,    -1,    0,    0,    -1],
    'tau5_empty_larcalib'   : [    -1,    -1,    0,    0,    -1],
    'j10_empty_larcalib'   : [    -1,    -1,    0,    0,    -1],
    'fj10_empty_larcalib'   : [    -1,    -1,    0,    0,    -1],
    # Generic monitoring
    'allL1_NoAlg'              : [   -1,   -1],
    'L1Calo_NoAlg'             : [   -1,   -1],
    'L1Muon_NoAlg'             : [   -1,   -1],
    'L1MinBias_NoAlg'          : [   -1,   -1],
    'InDetMon_FS'              : [    1000,    1000,    0,    0],
    #cosmic chains
    'L1TRT_NoAlg'                                     : [    -1,    -1,    0,    0],
    'CosmicsAllTeTRTxK_TRTTrkHypo_Pixel'              : [    -1,    -1,    0,    0],
    'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics'         : [    -1,    -1,    0,    0],
    'CosmicsAllTeIDSCAN_AllPhysics_TrkHypo'           : [    -1,    -1,    0,    0],
    'CosmicsAllTeSiTrack_AllPhysics_TrkHypo'          : [    -1,    -1,    0,    0],
    'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo'     : [    -1,    -1,    0,    0],
    'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo'    : [    -1,    -1,    0,    0],
    'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField' : [    -1,    -1,    0,    0],
    'Cosmics_MU11_TRTxK_TRTTrkHypo_Pixel'         : [    -1,    -1,    0,    0],
    'Cosmics_MU11_TRTxK_TRTTrkHypo_Pixel_IDCalib' : [    -1,    -1,    0,    0],
    'CosmicsAllTeEFID'                                : [    -1,    -1,    0,    0],

    'PreS_tight': [    -1,    -1,    1000,    0],
    'PreS_EM_tight': [    -1,    -1,    1000,    0],
    'PreS_TAU_tight': [    -1,    -1,    1000,    0],
    'PreS_J_tight': [    -1,    -1,    1000,    0],
    
    'PixelNoise' : [    -1,    -1,    0,    0,    -1],
    'SCTNoise'   : [    -1,    -1,    0,    0,    -1],
        
    'cost_monitor' : [      -1,       -1],
    'eb_physics'   : [    -1,    -1,    0,    0,    -1],
    'high_eb_physics'   : [    -1,    -1,    0,    0,    -1],
    'eb_physics_noL1PS'   : [    -1,    -1,    0,    0,    -1],
    'eb_random'   : [    -1,    -1,    0,    0,    -1],
    'eb_physics_unpaired_iso'   : [    -1,    -1,    0,    0,    -1],
    'eb_physics_unpaired_noniso'   : [    -1,    -1,    0,    0,    -1],
    'eb_physics_firstempty'   : [    -1,    -1,    0,    0,    -1],
    'eb_physics_empty'   : [    -1,    -1,    0,    0,    -1],
    'eb_random_unpaired_iso'   : [    -1,    -1,    0,    0,    -1],
    'eb_random_firstempty'   : [    -1,    -1,    0,    0,    -1],
    'eb_random_empty'   : [    -1,    -1,    0,    0,    -1],
    'ROBRequestMon'   : [    -1,    -1,    0,    0,    -1],
    'HLTTimeBurner'              : [   -1,   -1,    0,    0,    -1],
    'beamspot_vertex_FSTracks_SiTrk'   : [    -1,    -1,    0,    0,    -1],
    'beamspot_vertex_FSTracks_SiTrk_MBTS'   : [    -1,    -1,    0,    0,    -1],
    'beamspot_vertex_FSTracks_IDScan'   : [    -1,    -1,    0,    0,    -1],
    'beamspot_vertex_activeTE_SiTrk_peb'   : [    1000,    1000,    0,    0,    -1],
    'beamspot_vertex_activeTE_SiTrk_peb_MBTS'   : [    1000,    1000,    0,    0,    -1],
    'beamspot_vertex_allTE_SiTrk_peb'   : [    1000,    1000,    0,    0,    -1],
    'beamspot_vertex_allTE_SiTrk_peb_MBTS'   : [    1000,    1000,    0,    0,    -1],
    'beamspot_vertex_activeTE_IDScan_peb'   : [    1000,    1000,    0,    0,    -1],
    'L1Calo_NoAlg'   : [    -1,    -1,    0,    0,    -1],
    'L1Muon_NoAlg'   : [    -1,    -1,    0,    0,    -1],
    'L1MinBias_NoAlg'   : [    -1,    -1,    0,    0,    -1],
    'SeededStreamerL1CaloXS'   : [    -1,    -1,    0,    0,    -1],
    'Standby'   : [    -1,    -1,    0,    0,    -1], 
    'Background'   : [    -1,    -1,    0,    0,    -1],

    #### Asked for by Dennis -- but add 30 kb to AOD size
    'e5_NoCut_Ringer' : [ 1000, 1000, 0, 0, -1],
    'e5_NoCut_L2SW'   : [ 1000, 1000, 0, 0, -1],
    'g20_loose_cnv'   : [ 1000, 1000, 0, 0, -1], 

        }

#Un-prescale calibration triggers for low-pt production
Prescales.L1Prescales_loose_mc_prescale =  Prescales.L1Prescales.copy()
Prescales.HLTPrescales_loose_mc_prescale = Prescales.HLTPrescales.copy()

def updatePS(ps_set, key, ps_list):
    if key in ps_set:
        del ps_set[key]
        
    ps_set[key] = ps_list

updatePS(Prescales.HLTPrescales_loose_mc_prescale, 'xe_NoCut_allL1',     [1,1,0,0,1])    
