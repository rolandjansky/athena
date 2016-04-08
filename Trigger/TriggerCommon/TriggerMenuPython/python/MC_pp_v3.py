# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TriggerMenuPython.Physics_pp_v3 as physics_menu

from AthenaCommon.Logging import logging
from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.TriggerPythonConfig import TriggerPythonConfig
from TriggerMenuPython.Lvl1Flags import Lvl1Flags, addToItemMap, addToThresholdMap
    
#---------------------------------------------------------------------
# Just copy the MC 10^31 menu with different prescale set
#---------------------------------------------------------------------
def setupMenu():
    physics_menu.setupMenu()

    TriggerFlags.Lvl1.items = TriggerFlags.Lvl1.items() + []

    TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
        #dR veto
        'mu20_medium_j10dR02veto','mu20_medium_j30dR02veto',
        'mu20_medium_j10dR04veto','mu20_medium_j30dR04veto',
        #Isolated Di-Muon Trigger
        'mu4Tii_debug','mu6ii_debug',
        # muon isolation chains for test
        'mu18i1_medium', 'mu18i2_medium', 
        'mu20i1_medium', 'mu20i2_medium', 'mu20i3_medium', 'mu20i4_medium',
        'mu4T_IDTrkNoCut',
        #L2Star chains,
        'mu18_medium_L2StarA',
        'mu18_medium_L2StarB',
        'mu18_medium_L2StarC',
        #MultiMuons
        'mu24_mu6_EFFS_medium',

           # TrigMuSuperEF chains
        #'mu18_OR_medium', 
        
        # TrigMuSuperEF iso chains
        ##'mu18i1_OR', 'mu18i2_OR', 'mu18i3_OR', 'mu18i4_OR', 'mu18i5_OR',
        ##'mu24i1_OR', 'mu24i2_OR', 'mu24i3_OR', 'mu24i4_OR',

        'mu18i4_medium_L2StarA',
        'mu18i4_medium_L2StarB',
        'mu18i4_medium_L2StarC',
        


        #'mu24i5_OR_tight',

        # TrigMuSuperEF special performance comparison chains
        'mu18_OR_medium_MGfirst',
        'mu18_OR_medium_TMEFfirst',
        'mu18_OR_medium_both',

        'mu24i5_OR_tight_MGfirst',
        'mu24i5_OR_tight_TMEFfirst',
        'mu24i5_OR_tight_both',

        # TrigMuSuperEF special validation chains
        'mu18_OR_medium_MGonly',
        'mu18_OR_medium_TMEFonly',
        #'mu24i1_OR_tight_TMEFonly',
        #'mu24i2_OR_tight_TMEFonly',
        #'mu24i3_OR_tight_TMEFonly',
        #'mu24i4_OR_tight_TMEFonly',
        'mu24i5_OR_tight_TMEFonly',
        'mu24i5_OR_tight_MGonly',

        'mu24i5_tight',
        'mu20_L2Star_IDTrkNoCut',

        'mu18_l2muonSA_medium',
        '2mu10_l2muonSA',

        '2mu10_L2StarA',
        '2mu10_L2StarB',
        '2mu10_L2StarC',

        'mu40_MSonly_barrel_medium1',
        
        'mu18ia1_medium', 'mu18ia2_medium',
        'mu18ia3_medium', 'mu18ia4_medium',
        'mu24ia1_tight', 'mu24ia2_tight',
        'mu24ia3_tight', 'mu24ia4_tight',
        'mu6ia1_MSonly_wOvlpRm', 'mu6ia2_MSonly_wOvlpRm',
        '2mu6ia1_MSonly', '2mu6ia2_MSonly',
        '2mu6ia3_MSonly', '2mu6ia4_MSonly',
        '2mu6_MSonly',
        '2mu10ia1_MSonly', 'mu10_mu6ia1_MSonly',
        '2mu10ia2_MSonly', 'mu10_mu6ia2_MSonly',
        '2mu10ia3_MSonly', 'mu10_mu6ia3_MSonly',
        '2mu10ia4_MSonly', 'mu10_mu6ia4_MSonly',        
        ]
    
    TriggerFlags.EgammaSlice.signatures = TriggerFlags.EgammaSlice.signatures() + [
        '2e15vh_loose_Zee', '2e15vh_loose1_Zee',
        'e25_loose2_xe20_noMu_recoil40',
        'e25i6vh_medium2',
        'e22vh_loose1_e10_loose1','e22vh_medium1_e5_medium', 'e10_loose1',
        'e25vh_medium1', 'e22_etcut_e10_loose1', 'e22_loose1_e10_etcut',
        'e5_medium_L2StarA', 'e5_medium_L2StarB', 'e5_medium_L2StarC',
        'e12_medium_L2StarA', 'e12_medium_L2StarB', 'e12_medium_L2StarC', 
        'e22vh_medium_L2StarA', 'e22vh_medium_L2StarB', 'e22vh_medium_L2StarC',
        'e22vh_medium_L2Star_IDTrkNoCut',
        ]
    
    TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [
        #'tauNoCut_MV',
        #'tau20_medium1_pv', #config does not exist
        #2step tracking test chain
        ##'tau29_medium_2stTest', #TEMPORARILY DISABLED!!!
        #'tau29_medium_L2Star_IDTrkNoCut', #config does not exist

        ##removed due to warnings in mc v3 (#97670)
        #'tau20_medium_llh',
        #'tau20_medium_bdt',
        #'tau20_medium1_llh',
        #'tau20_medium1_bdt',
        #'tau29_medium_llh',
        #'tau29_medium_bdt',
        #'tau29_medium1_llh',
        #'tau29_medium1_bdt',
        #'tau29_medium_L2StarA', 'tau29_medium_L2StarB', 'tau29_medium_L2StarC', 
        #'tau125_medium1_L2StarA', 'tau125_medium1_L2StarB', 'tau125_medium1_L2StarC',

        ]

    TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + [
        'j55_a4tchad_EFFS', 'j145_a4tchad_EFFS', 'fj70_a4tchad_EFFS',
        #topotowers
        'j15_a4tt_EFFS','j30_a4tt_EFFS', 'j100_a4tt_EFFS',
        'j240_a4tt_EFFS', 'fj30_a4tt_EFFS', 'fj100_a4tt_EFFS',
        #L1.5 jets
#        'NoCut_a4JE_L15FS_NoEF',
#        'NoCut_a4JEcal_L15FS_NoEF',
#        'NoCut_a4JEhad_L15FS_NoEF',
#        'NoCut_a4JE_L15FS_L14J10_NoEF',
#        'j0_a4JE_L15FS_j0_NoEF',
#        'j0_a4JE_L15FS_j0_L14J10_NoEF',
#        'NoCut_a4JE_pu_sub_L15FS_NoEF',
#        'NoCut_a4TT_L15FS_NoEF',
#        'NoCut_a4TTcal_L15FS_NoEF',
#        'NoCut_a4TThad_L15FS_NoEF',
#        'NoCut_a4TT_L15FS_L14J10_NoEF',
#        'j0_a4TT_L15FS_j0_NoEF',
#        'j0_a4TT_L15FS_j0_L14J10_NoEF',
#        'NoCut_a4TT_pu_sub_L15FS_NoEF',
        #HT L2
#        'L2_ht150_a4JE_L15FS_NoEF',
#        'L2_ht150_a4TT_L15FS_NoEF',
#        'L2_ht150_a4JE_L15FS_L14J10_NoEF',
#        'L2_ht150_a4TT_L15FS_L14J10_NoEF',
        # jet cleaning
        'j30_a4tc_EFFS_efclean', 'j30_a4tc_EFFS_efcleaninv', 
        '6j30_a4tcem_L2a4cc',
        'j100_LArNoiseBurst',        
        'j30_LArNoiseBurst_empty',
        'j55_LArNoiseBurst_firstempty',
        'j55_LArNoiseBurst_empty'
        ]
    
    TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [
        '2b10_medium_j75_j45_a4tc_EFFS',
        '2b10_medium_j75_2j30_a4tc_EFFS',
        '2b10_medium_4j40_a4tc_EFFS',
        'b15_medium_4j45_a4tc_EFFS',
        'b15_medium_4L1J15',
        'b10_medium_4j30_a4tc_EFFS',
        'b15_medium_j100_j40_a4tc_EFFS_ht300',
        'b75_medium_j100_a4tc_EFFS_ht300',
        '2b15_medium_4L1J15',
        '2b15_medium_4j45_a4tc_EFFS',
        '2b15_medium_j75_j40_a4tc_EFFS_ht350',
        ##removed due to warnings in mc v3 (#97670)
        #'b10_L2Star_IDTrkNoCut',
        ]

    TriggerFlags.BphysicsSlice.signatures = TriggerFlags.BphysicsSlice.signatures() + [
        #Isolated Di-Muon Trigger
        'mu4Tiimu6ii_DiMu_DY', 'mu4Tiimu6ii_DiMu_DY14_noVtx_noOS', 'mu4Tiimu6ii_DiMu_DY20',
        '2mu6_Bmumu',
        '2mu6_Jpsimumu',
        '2mu6_Upsimumu',
        #L2Star chains,
        '2mu4T_DiMu_L2StarA',
        '2mu4T_DiMu_L2StarB',
        '2mu4T_DiMu_L2StarC',
        #l2muonSA
        '2mu4T_DiMu_l2muonSA',
        ]
    
    TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + [
        ]
    
    TriggerFlags.MinBiasSlice.signatures = TriggerFlags.MinBiasSlice.signatures() + []
    
    TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures() + [
        #Tau++
        'tau29T_medium_xe35_noMu_3L1J10',
        ##'tau29T_medium_xe40_tight_noMu',
        'tau29T_medium_xs45_loose_noMu_3L1J10',
        'tau29T_medium_xs45_noMu_3L1J10',
        'tau29T_medium1_xe35_noMu_3L1J10',
        'tau29T_medium1_xs45_noMu_3L1J10',
        #jet+met
        'j75_j45_a4tc_EFFS_xe55_noMu',
        'j100_a4tc_EFFS_EFxe70_noMu',
        'j100_2j30_a4tc_EFFS_EFxe70_noMu',
        'j100_2j40_a4tc_EFFS_EFxe30_noMu_ht350',
        'j75_a4tc_EFFS_xe65_noMu',
        'j75_a4tc_EFFS_xe65_noMu_dphi2j30xe10',
        'b15_medium_2j55_a4tc_EFFS_xe50_noMu',
        'b15_medium_j100_j40_a4tc_EFFS_EFxe30_noMu',
        'b15_medium_j75_j40_a4tc_EFFS_xe50_noMu',
        'b75_medium_j100_a4tc_EFFS_EFxe30_noMu',
        'b75_medium_j100_a4tc_EFFS_EFxe20_noMu_ht200',
        '2b15_medium_j75_j40_a4tc_EFFS_EFxe30_noMu',
        #muon + electron Bphysics
        '2mu4T_DiMu_e5_medium',
        'e22vh_medium1_mu10_EFFS', 'e22_etcut_mu10','e10_etcut_mu10',
        #mu+photon+masscut
        'mu10_g10_loose_TauMass', 'mu20_tight_g5_loose_TauMass', 'mu4T_g20Tvh_loose_TauMass', #'mu4T_g22Tvh_loose_TauMass',
        'mu10_g10_loose', 'mu20_tight_g5_loose', 'mu4T_g20Tvh_loose', #'mu4T_g22Tvh_loose',
        '2mu10_MSonly_g10_loose',
        #mu+MET
        'mu24_medium_EFxe40_noMu',
        ]

    TriggerFlags.CalibSlice.signatures = TriggerFlags.CalibSlice.signatures() + []
    TriggerFlags.CosmicSlice.signatures = TriggerFlags.CosmicSlice.signatures() + []
    TriggerFlags.GenericSlice.signatures = TriggerFlags.GenericSlice.signatures() + []
    TriggerFlags.MonitorSlice.signatures = TriggerFlags.MonitorSlice.signatures()  + []
    TriggerFlags.BeamSpotSlice.signatures = TriggerFlags.BeamSpotSlice.signatures() + []
    TriggerFlags.StreamingSlice.signatures = TriggerFlags.StreamingSlice.signatures() + []
    TriggerFlags.HeavyIonSlice.signatures = TriggerFlags.HeavyIonSlice.signatures() + []
    TriggerFlags.PerL1ItemStreamingSlice.signatures = TriggerFlags.PerL1ItemStreamingSlice.signatures() + []


Prescales = physics_menu.Prescales
StreamConfig = physics_menu.StreamConfig
TriggerGroups = physics_menu.TriggerGroups
HigherGroups = physics_menu.HigherGroups

for key in Prescales.L1Prescales.keys():
    Prescales.L1Prescales[key] = 1

Prescales.L1Prescales_loose_mc_prescale = Prescales.L1Prescales
Prescales.HLTPrescales_loose_mc_prescale = Prescales.HLTPrescales

Prescales.L1Prescales_tight_mc_prescale = Prescales.L1Prescales
Prescales.HLTPrescales_tight_mc_prescale = Prescales.HLTPrescales_tight_mc_prescale = {  
    
    #muon   
    'mu20_IDTrkNoCut'                                 : [    10,    1,    0,    0,    -1],
    'mu10_l2cal'                                      : [    -1,    -1,    0,    0,    -1],
    'mu20_muCombTag_NoEF'                             : [    -1,    -1,    0,    0,    -1],
    'mu4_cosmic'                                      : [    -1,    -1,    0,    0,    -1], 
    'mu4_MSonly_cosmic'                               : [    -1,    -1,    0,    0,    -1],
    'mu4_L1MU11_cosmic'                               : [    -1,    -1,    0,    0,    -1], 
    'mu4_L1MU11_MSonly_cosmic'                        : [    -1,    -1,    0,    0,    -1],
    'L1Calo_NoAlg'                                    : [    -1,    -1,    0,    0,    -1],
    'SeededStreamerL1CaloXS'                          : [    -1,    -1,    0,    0,    -1],
    'Standby'                                         : [    -1,    -1,    0,    0,    -1], 
    'Background'                                      : [    -1,    -1,    0,    0,    -1],
    'mu4_firstempty_NoAlg'                            : [    -1,    -1,    0,    0,    -1],
    'mu4_unpaired_iso_NoAlg'                          : [    -1,    -1,    0,    0,    -1],
    'mu4_empty_NoAlg'                                 : [    -1,    -1,    0,    0,    -1],
    'mu11_empty_NoAlg'                                : [    -1,    -1,    0,    0,    -1],
    '2mu10_noOvlpRm'                                  : [    -1,    -1,    0,    0,    -1],
    '3mu6_MSonly_noOvlpRm'                            : [    -1,    -1,    0,    0,    -1],
    'mu4_empty_NoAlg'                                 : [    -1,    -1,    0,    0,    -1],
    'mu4_unpaired_noniso_NoAlg'                       : [    -1,    -1,    0,    0,    -1],
    'mu4_unpaired_iso'                                : [    -1,    -1,    0,    0,    -1], 
    'mu4_firstempty'                                  : [    -1,    -1,    0,    0,    -1],
    'mu6_l2cal'                                       : [    -1,    -1,    0,    0,    -1],
    'mu10_MSonly_Ecut3'                               : [    -1,    -1,    0,    0,    -1],
    'mu10_MSonly_Ecut2'                               : [    -1,    -1,    0,    0,    -1],
    'mu10_Ecut3'                                      : [    -1,    -1,    0,    0,    -1],
    'mu10_Ecut2'                                      : [    -1,    -1,    0,    0,    -1],
    #egamma
    '2e5_medium'                                      : [    -1,    -1,    0,    0,    -1],
    '2e5_medium_SiTrk'                                : [    -1,    -1,    0,    0,    -1],
    '2e5_medium_TRT'                                  : [    -1,    -1,    0,    0,    -1],
    'e5_tight'                                        : [    -1,    -1,    0,    0,    -1], 
    'e5_tight_TRT'                                    : [    -1,    -1,    0,    0,    -1], 
    'e5_tight_e5_NoCut'                               : [    -1,    -1,    0,    0,    -1], 
    'e10_NoCut'                                       : [    -1,    -1,    0,    0,    -1],
    'e22vh_loose_TRT'                                 : [    -1,    -1,    0,    0,    -1],
    'e22vh_loose_IDTrkNoCut'                          : [    10,     1,    0,    0,    -1],
    'e5_NoCut_firstempty'                             : [    -1,    -1,    0,    0,    -1],
    'e5_tight_e5_NoCut_Jpsi'                          : [    -1,    -1,    0,    0,    -1],
    'e5_tight_e4_etcut_Jpsi'                          : [    -1,    -1,    0,    0,    -1],
    'e5_tight_e7_etcut_Jpsi'                          : [    -1,    -1,    0,    0,    -1],
    'e5_tight_e9_etcut_Jpsi'                          : [    -1,    -1,    0,    0,    -1],
    'e5_tight_e4_etcut_Jpsi_SiTrk'                    : [    -1,    -1,    0,    0,    -1],
    'e5_tight_e4_etcut_Jpsi_TRT'                      : [    -1,    -1,    0,    0,    -1],
    '2e5_tight'                                       : [    -1,    -1,    0,    0,    -1],
    '2e5_tight_Jpsi'                                  : [    -1,    -1,    0,    0,    -1],
    'e9_tight_e5_tight_Jpsi'                          : [    -1,    -1,    0,    0,    -1],
    'e22vh_medium_SiTrk'                              : [    -1,    -1,    0,    0,    -1],
    'e22vh_medium_TRT'                                : [    -1,    -1,    0,    0,    -1],
    'e22vh_medium_IDTrkNoCut'                         : [    10,     1,    0,    0,    -1],
    'g5_NoCut_cosmic'                                 : [    -1,    -1,    0,    0,    -1],
    #tau
    'tauNoCut_firstempty'                             : [    -1,    -1,    0,    0,    -1],
    'tauNoCut_unpaired_iso'                           : [    -1,    -1,    0,    0,    -1],
    'tauNoCut_unpaired_noniso'                        : [    -1,    -1,    0,    0,    -1],
    'tau12_IDTrkNoCut'                                : [    10,     1,    0,    0,    -1],
    'tau16_IDTrkNoCut'                                : [    10,     1,    0,    0,    -1],
    'L1TAU30_NoAlg'                                   : [    -1,    -1,    0,    0,    -1],
    'tauNoCut'                                        : [    1,      1,    1,    1,    -1],
    'tau50_IDTrkNoCut'                                : [    10,     1,    0,    0,    -1],
    'tauNoCut_cosmic'                                 : [    -1,    -1,    0,    0,    -1],
    'tauNoCut_MV'                                     : [    1,      1,    1,    1,    -1],
    'tau20_medium_llh'                                : [    1,      1,    1,    1,    -1],
    'tau20_medium_bdt'                                : [    1,      1,    1,    1,    -1],
    'tau20_medium1_llh'                               : [    1,      1,    1,    1,    -1],
    'tau20_medium1_bdt'                               : [    1,      1,    1,    1,    -1],
    'tau29_medium_llh'                                : [    1,      1,    1,    1,    -1],
    'tau29_medium_bdt'                                : [    1,      1,    1,    1,    -1],
    'tau29_medium1_llh'                               : [    1,      1,    1,    1,    -1],
    'tau29_medium1_bdt'                               : [    1,      1,    1,    1,    -1],
    #jet
    'j10_empty_NoAlg'                                 : [    -1,    -1,    0,    0,    -1],
    'j30_empty_NoAlg'                                 : [    -1,    -1,    0,    0,    -1],
    'fj10_empty_NoAlg'                                : [    -1,    -1,    0,    0,    -1],
    'je135_NoEF'                                      : [    -1,    -1,    0,    0,    -1],
    'je195_NoEF'                                      : [    -1,    -1,    0,    0,    -1],
    'je255_NoEF'                                      : [    -1,    -1,    0,    0,    -1],
    'je300_NoEF'                                      : [    -1,    -1,    0,    0,    -1],
    'je350_NoEF'                                      : [    -1,    -1,    0,    0,    -1],
    'je420_NoEF'                                      : [    -1,    -1,    0,    0,    -1],
    'je500_NoEF'                                      : [    -1,    -1,    0,    0,    -1],
    'j30_cosmic'                                      : [    -1,    -1,    0,    0,    -1],
    'j50_cosmic'                                      : [    -1,    -1,    0,    0,    -1],
    'j30_firstempty'                                  : [    -1,    -1,    0,    0,    -1],
    'j50_firstempty'                                  : [    -1,    -1,    0,    0,    -1],
    'j50_unpaired'                                    : [    -1,    -1,    0,    0,    -1],
    #bjet
    'b10_j30_a4tc_EFFS_IDTrkNoCut'                    : [    1,     1,    0,    0,    -1],
    'b10_IDTrkNoCut'                                  : [    10,     1,    0,    0,    -1],
    'b15_IDTrkNoCut'                                  : [    10,     1,    0,    0,    -1],
    'b20_IDTrkNoCut'                                  : [    10,     1,    0,    0,    -1],
    #bphysics
    'mu4T_Upsimumu_FS'                                 : [    -1,    -1,    0,    0,    -1],
    '2mu4T_DiMu_noVtx_noOS'                           : [    -1,    -1,    0,    0,    -1],
    'mu4_Jpsimumu_FS'                                 : [    -1,    -1,    0,    0,    -1],
    'mu4T_Upsimumu_SiTrk_FS'                           : [    -1,    -1,    0,    0,    -1],
    'mu6_Upsimumu_FS'                                 : [    -1,    -1,    0,    0,    -1],
    'mu10_Upsimumu_FS'                                : [    -1,    -1,    0,    0,    -1],
    'mu10_Upsimumu_tight_FS'                          : [    -1,    -1,    0,    0,    -1],
    'mu4_DiMu_FS_noOS'                                : [    -1,    -1,    0,    0,    -1],
    'mu4_DiMu_MG'                                     : [    -1,    -1,    0,    0,    -1],
    'mu4_DiMu_MG_FS'                                  : [    -1,    -1,    0,    0,    -1],
    'mu4_DiMu_SiTrk'                                  : [    -1,    -1,    0,    0,    -1],
    'mu4_DiMu_SiTrk_FS'                               : [    -1,    -1,    0,    0,    -1],
    '2mu4T_DiMu_SiTrk'                                : [    -1,    -1,    0,    0,    -1],
    'mu4T_DiMu_FS'                                     : [    -1,    -1,    0,    0,    -1],
    '2mu4T_DiMu_noVtx'                                : [    -1,    -1,    0,    0,    -1],
    '2mu4T_Jpsimumu_IDTrkNoCut'                       : [    10,     1,    0,    0,    -1],
    'mu4mu6_Jpsimumu_IDTrkNoCut'                      : [    10,     1,    0,    0,    -1],
    'mu4T_Trk_Upsi_FS'                                : [    -1,    -1,    0,    0,    -1],
    'mu4T_Upsimumu_tight_FS'                                : [    -1,    -1,    0,    0,    -1],
    #met
    'xe_NoCut_allL1'                                  : [     1,     1,    0,    0,    1],
    #min bias
    'mbMbts_2_SpTrkMh'                                : [    -1,    -1,    0,    0,    -1],
    'mbMbts_2_SpTrkMh120'                             : [    -1,    -1,    0,    0,    -1],
    'mbTE20_eff'                                      : [    -1,    -1,    0,    0,    -1],
    'mbSpTrkVtxMh'                                    : [    -1,    -1,    0,    0,    -1],
    'mbSpTrkVtxMh_eff'                                : [    -1,    -1,    0,    0,    -1],
    'mbSpTrkVtxMh_medium'                             : [    -1,    -1,    0,    0,    -1],
    'mbSpTrkVtxMh_medium_eff'                         : [    -1,    -1,    0,    0,    -1],
    'mbSpTrkVtxMh_tight'                              : [    -1,    -1,    0,    0,    -1],
    'mbSpTrkVtxMh_tight_eff'                          : [    -1,    -1,    0,    0,    -1],
    'rd0_filled_NoAlg'                                : [    1,      1,    0,    0,    -1],
    'mbRmMbts_1_NoCut_Time'                           : [    -1,    -1,    0,    0,    -1],
    'mbLucid_eff'                                     : [    -1,    -1,    0,    0,    -1],
    'mbMbts_1_eff'                                    : [    -1,    -1,    0,    0,    -1],
    'mbZdc_eff'                                       : [    -1,    -1,    0,    0,    -1],
    'mbSpTrk'                                         : [    -1,    -1,    0,    0,    -1],
    'mb_BCM_Wide_eff'                                 : [    -1,    -1,    0,    0,    -1],
    'mbSpBg_unpaired_iso'                             : [    -1,    -1,    0,    0,    -1],
    'mbRd1_eff'                                       : [    -1,    -1,    0,    0,    -1],
    'mbRd0_eff_unpaired_iso'                          : [    -1,    -1,    0,    0,    -1],
    'mbSpTrk_unpaired_iso'                            : [    -1,    -1,    0,    0,    -1],
    'L1TRT_NoAlg'                                     : [    -1,    -1,    0,    0,    -1],
    'rd0_empty_NoAlg'                                 : [    -1,    -1,    0,    0,    -1],
    'rd1_empty_NoAlg'                                 : [    -1,    -1,    0,    0,    -1],
    'zerobias_NoAlg'                                  : [    -1,    -1,    0,    0,    -1],
    #calib
    'g20_larcalib'                                    : [    -1,    -1,    0,    0,    -1],
    'g40_larcalib'                                    : [    -1,    -1,    0,    0,    -1],
    'g50_larcalib'                                    : [    -1,    -1,    0,    0,    -1], 
    'trk6_IDSCAN_IDCalib'                             : [    -1,    -1,    0,    0,    -1],
    'trk9_Central_Tau_IDCalib'                        : [    -1,    -1,    0,    0,    -1],
    'trk9_Fwd_Tau_IDCalib'                            : [    -1,    -1,    0,    0,    -1],
    'trk16_Central_Tau_IDCalib'                       : [    -1,    -1,    0,    0,    -1],
    'trk16_Fwd_Tau_IDCalib'                           : [    -1,    -1,    0,    0,    -1],
    'trk29_Central_Tau_IDCalib'                       : [    -1,    -1,    0,    0,    -1],
    'trk29_Fwd_Tau_IDCalib'                           : [    -1,    -1,    0,    0,    -1],
    'hadCalib_trk9'                                   : [    -1,    -1,    0,    0,    -1],
    'hadCalib_trk9phi'                                : [    -1,    -1,    0,    0,    -1],
    'em3_empty_larcalib'                              : [    -1,    -1,    0,    0,    -1],
    'tau5_empty_larcalib'                             : [    -1,    -1,    0,    0,    -1],
    'j10_empty_larcalib'                              : [    -1,    -1,    0,    0,    -1],
    'fj10_empty_larcalib'                             : [    -1,    -1,    0,    0,    -1],
    # Generic monitoring
    'allL1_NoAlg'                                     : [   -1,   -1],
    'L1Calo_NoAlg'                                    : [   -1,   -1],
    'L1Muon_NoAlg'                                    : [   -1,   -1],
    'L1MinBias_NoAlg'                                 : [   -1,   -1],
    'InDetMon_FS'                                     : [    1000,    1000,    0,    0],
    #cosmic chains
    'L1TRT_NoAlg'                                     : [    -1,    -1,    0,    0],
    'CosmicsAllTeTRTxK_TRTTrkHypo_Pixel'              : [    -1,    -1,    0,    0],
    'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics'         : [    -1,    -1,    0,    0],
    'CosmicsAllTeIDSCAN_AllPhysics_TrkHypo'           : [    -1,    -1,    0,    0],
    'CosmicsAllTeSiTrack_AllPhysics_TrkHypo'          : [    -1,    -1,    0,    0],
    'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo'     : [    -1,    -1,    0,    0],
    'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo'    : [    -1,    -1,    0,    0],
    'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField' : [    -1,    -1,    0,    0],
    'Cosmics_MU11_TRTxK_TRTTrkHypo_Pixel'             : [    -1,    -1,    0,    0],
    'Cosmics_MU11_TRTxK_TRTTrkHypo_Pixel_IDCalib'     : [    -1,    -1,    0,    0],
    'CosmicsAllTeEFID'                                : [    -1,    -1,    0,    0],
    'PreS_tight'                                      : [    -1,    -1,    1000,    0],
    'PreS_EM_tight'                                   : [    -1,    -1,    1000,    0],
    'PreS_TAU_tight'                                  : [    -1,    -1,    1000,    0],
    'PreS_J_tight'                                    : [    -1,    -1,    1000,    0],
    'PixelNoise'                                      : [    -1,    -1,    0,    0,    -1],
    'SCTNoise'                                        : [    -1,    -1,    0,    0,    -1],
    'cost_monitor'                                    : [    -1,    -1],
    'eb_physics'                                      : [    -1,    -1,    0,    0,    -1],
    'high_eb_physics'                                 : [    -1,    -1,    0,    0,    -1],
    'eb_physics_noL1PS'                               : [    -1,    -1,    0,    0,    -1],
    'eb_random'                                       : [    -1,    -1,    0,    0,    -1],
    'eb_physics_unpaired_iso'                         : [    -1,    -1,    0,    0,    -1],
    'eb_physics_unpaired_noniso'                      : [    -1,    -1,    0,    0,    -1],
    'eb_physics_firstempty'                           : [    -1,    -1,    0,    0,    -1],
    'eb_physics_empty'                                : [    -1,    -1,    0,    0,    -1],
    'eb_random_unpaired_iso'                          : [    -1,    -1,    0,    0,    -1],
    'eb_random_firstempty'                            : [    -1,    -1,    0,    0,    -1],
    'eb_random_empty'                                 : [    -1,    -1,    0,    0,    -1],
    'ROBRequestMon'                                   : [    -1,    -1,    0,    0,    -1],
    'HLTTimeBurner'                                   : [    -1 ,   -1,    0,    0,    -1],
    'beamspot_vertex_FSTracks_SiTrk'                  : [    -1,    -1,    0,    0,    -1],
    'beamspot_vertex_FSTracks_SiTrk_backup'           : [    -1,    -1,    0,    0,    -1],
    'beamspot_vertex_FSTracks_SiTrk_MBTS'             : [    -1,    -1,    0,    0,    -1],
    'beamspot_vertex_activeTE_SiTrk_peb'              : [    -1,    -1,    0,    0,    -1],
    'beamspot_vertex_activeTE_SiTrk_peb_backup'       : [    -1,    -1,    0,    0,    -1],
    'beamspot_vertex_activeTE_SiTrk_peb_MBTS'         : [    -1,    -1,    0,    0,    -1],     
    'beamspot_vertex_allTE_SiTrk_peb'                 : [    -1,    -1,    0,    0,    -1],
    'beamspot_vertex_allTE_SiTrk_peb_backup'          : [    -1,    -1,    0,    0,    -1],
    'beamspot_vertex_allTE_SiTrk_peb_MBTS'            : [    -1,    -1,    0,    0,    -1],     
    'beamspot_vertex_FSTracks_IDScan'                 : [    -1,    -1,    0,    0,    -1],
    'beamspot_vertex_activeTE_IDScan_peb'             : [    -1,    -1,    0,    0,    -1],
    'L1Calo_NoAlg'                                    : [    -1,    -1,    0,    0,    -1],
    'L1Muon_NoAlg'                                    : [    -1,    -1,    0,    0,    -1],
    'L1MinBias_NoAlg'                                 : [    -1,    -1,    0,    0,    -1],
    'SeededStreamerL1CaloXS'                          : [    -1,    -1,    0,    0,    -1],
    'Standby'                                         : [    -1,    -1,    0,    0,    -1], 
    'Background'                                      : [    -1,    -1,    0,    0,    -1],
    'e5_NoCut_Ringer'                                 : [ 1000, 1000, 0, 0, -1],
    'e5_NoCut_L2SW'                                   : [ 1000, 1000, 0, 0, -1],
    'g20_loose_cnv'                                   : [ 1000, 1000, 0, 0, -1],
    'fj10_a4tc_EFFS_1vx'                              : [    1, 1000,    0,    0,  0],
    'j10_a4tc_EFFS_1vx'                               : [    1, 1000,    0,    0,  0],
    'j20_a4hi_EFFS'                                   : [    1, -1,    0,     0,  0],
    'j15_a2hi_EFFS'                                   : [    1, -1,    0,    0,  0],
    'j20_a2hi_EFFS'                                   : [    1, -1,    0,    0,  0],
    }

