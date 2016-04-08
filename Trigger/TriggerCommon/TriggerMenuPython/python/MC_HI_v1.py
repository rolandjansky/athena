# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TriggerMenuPython.Physics_HI_v1 as PhysicsHIV1

from AthenaCommon.Logging import logging
from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenuPython.TriggerPythonConfig import TriggerPythonConfig

def setupMCHIV1Menu():
    PhysicsHIV1.setupMenu()
    
    TriggerFlags.MonitorSlice.signatures = TriggerFlags.MonitorSlice.signatures() + [
        ]

    TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [    
	]

    TriggerFlags.EgammaSlice.signatures = TriggerFlags.EgammaSlice.signatures() + [
        'g5_loose_EFFS_L1ZDC',
        'g9_etcut',
        #'g5_hiptrt',
        #electrons
        'e5_loose_L1VTE50',
        'g5_loose_L1TE50',
        ]

    TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
        'mu4_MSonly_EFFS_L1TE5_NZ',
        'mu4_MSonly_EFFS_L1MU0_NZ',
        'mu6', 'mu6T',
        'mu6T_IDTrkNoCut',
        #
        ]

    TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures() + [
        ]
 
    TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + [
        ]

    TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + [
        #pp jets
        'j15_a4_EFFS_L1ZDC',
        'j20_a4_EFFS_L1ZDC',
        'j40_a4_EFFS_L1ZDC',
        'j75_a4_EFFS_L1ZDC',
         #L2 Hi jets
        'j15_Ecorr',
        'j20_Ecorr',
        'fj20_Ecorr',        
        ]

    TriggerFlags.MinBiasSlice.signatures = TriggerFlags.MinBiasSlice.signatures() + [
        'mbMbts_3_3',
        'mbMbts_3_3_eff_HI',
        'mbMbts_3_3_unpaired_iso',
        'mbMbts_3_3_eff_unpaired_iso_HI',
        #
        'mbZdc_and_Mbts_1_1',
        'mbZdc_and_Mbts_2_2',
        'mbZdc_a_c_Mbts_1_1',
        'mbZdc_a_c_Mbts_2_2',
        #Zdc at L2
        'mbZdc_a_c_L2Zdc_eff',
        'mbTe10_L2Zdc_eff',
        ]

#for reprocessing
    TriggerFlags.HeavyIonSlice.signatures = TriggerFlags.HeavyIonSlice.signatures() + [
       ]

    TriggerFlags.CalibSlice.signatures = TriggerFlags.CalibSlice.signatures()+[
       'j15_larcalib_hi', 'j20_larcalib_hi', 
       'fj20_larcalib_hi', 
       ]
    
Prescales = PhysicsHIV1.Prescales
StreamConfig = PhysicsHIV1.StreamConfig
TriggerGroups = PhysicsHIV1.TriggerGroups

for key in Prescales.L1Prescales.keys():
    Prescales.L1Prescales[key] = 1
    
Prescales.L1Prescales_pPb_mc_prescale = Prescales.L1Prescales
Prescales.HLTPrescales_pPb_mc_prescale = Prescales.HLTPrescales_pPb_mc_prescale = {
    # Need to have rerun=1:
    'xe30_allL1'               : [    0,    0,    0,    0,  1],  # = any event passed by L1 calo, muon, minbias, random triggers, followed by L2_xe30 in pass-through and EF_xe30 with rerun=1
    'xe30_allL1_allCells'      : [    -0,    -0,    0,    0,  1],  # (same as above, with noise suppression at EF)
    'xe30_allL1_FEB'           : [    -0,    -0,    0,    0,  1],  # (same as above, with FEB headers at EF)
    'xe_NoCut_allL1_noMu_Mon'  : [    0,    0,    0,    0,  1],
    'xe_NoCut_L1MBTS_1_1_noMu_Mon'  : [    0,    0,    0,    0,  1],
    'xe_NoCut_L1MBTS_2_VTE50_noMu_Mon'  : [    0,    0,    0,    0,  1],
    'allL1_NoAlg'              : [   -1,   -1],
    #cosmic chains
    'CosmicsAllTeTRTxK_TRTTrkHypo_Pixel'              : [    -1,    -1,    0,    0],
    'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics'         : [    -1,    -1,    0,    0],
    'CosmicsAllTeIDSCAN_AllPhysics_TrkHypo'           : [    -1,    -1,    0,    0],
    'CosmicsAllTeSiTrack_AllPhysics_TrkHypo'          : [    -1,    -1,    0,    0],
    'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo'     : [    -1,    -1,    0,    0],
    'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo'    : [    -1,    -1,    0,    0],
    'CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField' : [    -1,    -1,    0,    0],
    'Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_Pixel'         : [    -1,    -1,    0,    0],
    'CosmicsAllTeEFID'                                : [    -1,    -1,    0,    0],
    'prompt'                                          : [    -1,    -1,    0,    0],
    'allL1_NoAlg'                                     : [    -1,    -1,    0,    0],
    "SeededStreamerRNDM"                              : [    -1,    -1,    0,    0],
    "SeededStreamerL1Calo"                            : [    -1,    -1,    0,    0],
    "SeededStreamerMinBias"                           : [    -1,    -1,    0,    0],
    "SeededStreamerMBTS"                              : [    -1,    -1,    0,    0],
    "SeededStreamerBCM_LUCID_ZDC"                     : [    -1,    -1,    0,    0],
    "SeededStreamerMuonswBeam"                        : [    -1,    -1,    0,    0],
    #L1.5 jets
    "NoCut_a4JE_L15FS_NoEF"                           : [    -1,    -1,    0,    0],
    "NoCut_a4TT_L15FS_NoEF"                           : [    -1,    -1,    0,    0],
    "NoCut_a4JE_L15FS_NoEF_L1TE10"                    : [    -1,    -1,    0,    0],
    "NoCut_a4TT_L15FS_NoEF_L1TE10"                    : [    -1,    -1,    0,    0],
    #HI stuff
    "g5_loose_EFFS_L1ZDC"                             : [    -1,    -1,    0,    0],
    "centrality100_60"                                : [    1,    1,    0,    0,  1],
    "centrality80_60"                                 : [    1,    1,    0,    0,  1],
    'mbZdc_a_c_L1TE50_NoAlg'                          : [    1,    1,    0,    0,  1],
    'L1TE50_NoAlg'                                    : [    1,    1,    0,    0,  1],
    'mbZdc_a_c_NoAlg'                                 : [    1,    1,    0,    0,  1],
    'L1ZDC_VTE50_mv_loose2_upc'                       : [    1,    1,    0,    0,  1],
    'L1ZDC_OR_mv_loose2_upc'                          : [    1,    1,    0,    0,  1],
    #pPb triggers
    'mbSpTrk_L1ZDC'                                   : [    1,    1,    0,    0,  1],
    'mbSpTrk_L1MBTS_1'                                : [    1,    1,    0,    0,  1],
    'mbSpTrk_L1MBTS_2'                                : [    1,    1,    0,    0,  1],
    'mbSpTrk_L1MBTS_1_1'                              : [    1,    1,    0,    0,  1],
    'mbZdc_NoAlg'                                     : [    1,    1,    0,    0,  1],
    'mu4_MSonly_EFFS_L1ZDC_OR'                        : [    1,    1,    0,    0,  1],
    'mu4_MSonly_EFFS_L1MBTS'                          : [    1,    1,    0,    0,  1],
    '2mu2_MSonly_EFFS_L1ZDC_OR'                       : [    1,    1,    0,    0,  1],
    '2mu2_MSonly_EFFS_L1MBTS'                         : [    1,    1,    0,    0,  1],
    'j15_a2hi_EFFS_L1MBTS'                            : [    1,    1,    0,    0,  0],
    'j20_a2hi_EFFS_L1MBTS'                            : [    1,    1,    0,    0,  0],
    'j15_a4hi_EFFS_L1MBTS'                            : [    1,    1,    0,    0,  0],
    'j20_a4hi_EFFS_L1MBTS'                            : [    1,    1,    0,    0,  0],
    'j15_a4hi_EFFS_L1TE10'                            : [    1,    1,    0,    0,  0],
    'j20_a4hi_EFFS_L1TE10'                            : [    1,    1,    0,    0,  0],
    }   
#def updatePS(ps_set, key, ps_list):
#    if key in ps_set:
#        del ps_set[key]
#        
#    ps_set[key] = ps_list

#updatePS(Prescales.HLTPrescales, 'g0_mbts_c',                       [1,1,0,0])
