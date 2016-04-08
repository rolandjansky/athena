# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################
# SliceDef file for Muon chains
###########################################################################
__author__  = 'I.Grabowska-Bold'
__version__=""
__doc__="Definition of muon chains as used in Physics_HI_v1 menu"

from TriggerMenuPython.MuonDef_v2v3 import *

# muFast threshold for combiner chains
mufastThr_4GeV='4GeV_v11a'
mufastThr_6GeV='6GeV_v11a'
mufastThr_10GeV='6GeV_v11a'
mufastThr_13GeV='6GeV_v11a'
mufastThr_15GeV='6GeV_v11a'
mufastThr_18GeV='6GeV_v11a'
mufastThr_20GeV='6GeV_v11a'
mufastThr_22GeV='6GeV_v11a'
mufastThr_24GeV='6GeV_v11a'
mufastThr_30GeV='6GeV_v11a'
mufastThr_40GeV='6GeV_v11a'
#
# muFast threshold for MSonly chains
mufastThr_MSonly_4GeV_barrelOnly='4GeV_barrelOnly_v11a'
mufastThr_MSonly_10GeV='10GeV_v11a'
mufastThr_MSonly_13GeV='13GeV_v11a'
mufastThr_MSonly_20GeV='20GeV_v11a'
mufastThr_MSonly_30GeV='30GeV_v11a'
mufastThr_MSonly_40GeV='40GeV_v11a'
mufastThr_MSonly_40GeV_barrelOnly='40GeV_barrelOnly_v11a'
mufastThr_MSonly_40GeV_uptoEC2='40GeV_uptoEC2_v11a'
mufastThr_MSonly_60GeV='40GeV_v11a'
mufastThr_MSonly_80GeV='40GeV_v11a'
mufastThr_MSonly_100GeV='40GeV_v11a'

#commissioning unseeded items
Config   = L2EFChain_mu_Unseeded_EFFS.Config
L2create = L2EFChain_mu_Unseeded_EFFS.L2Config
EFcreate = L2EFChain_mu_Unseeded_EFFS.EFConfig

# definde seed for HI
minbias_hi_seed = "L1_ZDC_A_C"

Muons = [
    #commissioning item
    L2EFChain_mu_Unseeded_EFFS('mu4_MSonly_MB2_noL2_EFFS', 
                               'L2_mu4_MSonly_MB2_noL2_EFFS', 2076, 'L1_MBTS_2',
                               'EF_mu4_MSonly_MB2_noL2_EFFS', 2076,
                               [''],
                               Config(L2create('_mu4_MSonly_MB2_noL2_EFFS'),
                                      EFcreate('_mu4_MSonly_MB2_noL2_EFFS', TrigMuonEFExtrapolatorHypoConfig('Muon','4GeV'))),
                               prescale=1, pass_through=0),

    #pPb
    L2EFChain_mu_Unseeded_EFFS('mu4_MSonly_EFFS_L1ZDC_OR', 
                               'L2_mu4_MSonly_EFFS_L1ZDC_OR', 850, 'L1_ZDC',
                               'EF_mu4_MSonly_EFFS_L1ZDC_OR', 850,
                               [''],
                               Config(L2create('_mu4_MSonly_EFFS_L1ZDC_OR'),
                                      EFcreate('_mu4_MSonly_EFFS_L1ZDC_OR', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('mu4_MSonly_EFFS_L1MBTS', 
                               'L2_mu4_MSonly_EFFS_L1MBTS', 851, 'L1_MBTS_2',
                               'EF_mu4_MSonly_EFFS_L1MBTS', 851,
                               [''],
                               Config(L2create('_mu4_MSonly_EFFS_L1MBTS'),
                                      EFcreate('_mu4_MSonly_EFFS_L1MBTS', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('2mu2_MSonly_EFFS_L1ZDC_OR',
                               'L2_2mu2_MSonly_EFFS_L1ZDC_OR', 852, 'L1_ZDC',
                               'EF_2mu2_MSonly_EFFS_L1ZDC_OR', 852,
                               [''],
                               Config(L2create('_2mu2_MSonly_EFFS_L1ZDC_OR'),
                                      EFcreate('_2mu2_MSonly_EFFS_L1ZDC_OR', TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '2GeV', '2GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('2mu2_MSonly_EFFS_L1MBTS',
                               'L2_2mu2_MSonly_EFFS_L1MBTS', 853, 'L1_MBTS_2',
                               'EF_2mu2_MSonly_EFFS_L1MBTS', 853,
                               [''],
                               Config(L2create('_2mu2_MSonly_EFFS_L1MBTS'),
                                      EFcreate('_2mu2_MSonly_EFFS_L1MBTS', TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '2GeV', '2GeV'))),
                               prescale=1, pass_through=0),

    #PbPb
    L2EFChain_mu_Unseeded_EFFS('mu4_MSonly_EFFS_L1ZDC', 
                               'L2_mu4_MSonly_EFFS_L1ZDC', 1000, minbias_hi_seed,
                               'EF_mu4_MSonly_EFFS_L1ZDC', 1000,
                               [''],
                               Config(L2create('_mu4_MSonly_EFFS_L1ZDC'),
                                      EFcreate('_mu4_MSonly_EFFS_L1ZDC', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('mu4_MSonly_EFFS_L1TE10',
                               'L2_mu4_MSonly_EFFS_L1TE10', 836, 'L1_TE10',
                               'EF_mu4_MSonly_EFFS_L1TE10', 836,
                               [''],
                               Config(L2create('_mu4_MSonly_EFFS_L1TE10'),
                                      EFcreate('_mu4_MSonly_EFFS_L1TE10', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('mu4_MSonly_EFFS_L1TE20',
                               'L2_mu4_MSonly_EFFS_L1TE20', 814, 'L1_TE20',
                               'EF_mu4_MSonly_EFFS_L1TE20', 814,
                               [''],
                               Config(L2create('_mu4_MSonly_EFFS_L1TE20'),
                                      EFcreate('_mu4_MSonly_EFFS_L1TE20', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('mu4_MSonly_EFFS_L1TE50',
                               'L2_mu4_MSonly_EFFS_L1TE50', 815, 'L1_TE50',
                               'EF_mu4_MSonly_EFFS_L1TE50', 815,
                               [''],
                               Config(L2create('_mu4_MSonly_EFFS_L1TE50'),
                                      EFcreate('_mu4_MSonly_EFFS_L1TE50', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV'))),
                               prescale=1, pass_through=0),
    
    L2EFChain_mu_Unseeded_EFFS('2mu4_MSonly_EFFS_L1ZDC', 
                               'L2_2mu4_MSonly_EFFS_L1ZDC', 803, minbias_hi_seed,
                               'EF_2mu4_MSonly_EFFS_L1ZDC', 803,
                               [''],
                               Config(L2create('_2mu4_MSonly_EFFS_L1ZDC'),
                                      EFcreate('_2mu4_MSonly_EFFS_L1ZDC', TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '4GeV', '4GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('2mu4_MSonly_EFFS_L1TE10', 
                               'L2_2mu4_MSonly_EFFS_L1TE10', 816, 'L1_TE10',
                               'EF_2mu4_MSonly_EFFS_L1TE10', 816,
                               [''],
                               Config(L2create('_2mu4_MSonly_EFFS_L1TE10'),
                                      EFcreate('_2mu4_MSonly_EFFS_L1TE10', TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '4GeV', '4GeV'))),
                               prescale=1, pass_through=0),


    L2EFChain_mu_Unseeded_EFFS('2mu4_MSonly_EFFS_L1TE20', 
                               'L2_2mu4_MSonly_EFFS_L1TE20', 817, 'L1_TE20',
                               'EF_2mu4_MSonly_EFFS_L1TE20', 817,
                               [''],
                               Config(L2create('_2mu4_MSonly_EFFS_L1TE20'),
                                      EFcreate('_2mu4_MSonly_EFFS_L1TE20', TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '4GeV', '4GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('2mu4_MSonly_EFFS_L1TE50', 
                               'L2_2mu4_MSonly_EFFS_L1TE50', 818, 'L1_TE50',
                               'EF_2mu4_MSonly_EFFS_L1TE50', 818,
                               [''],
                               Config(L2create('_2mu4_MSonly_EFFS_L1TE50'),
                                      EFcreate('_2mu4_MSonly_EFFS_L1TE50', TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '4GeV', '4GeV'))),
                               prescale=1, pass_through=0),
    

    L2EFChain_mu_Unseeded_EFFS('2mu2_MSonly_EFFS_L1ZDC',
                               'L2_2mu2_MSonly_EFFS_L1ZDC', 834, minbias_hi_seed,
                               'EF_2mu2_MSonly_EFFS_L1ZDC', 834,
                               [''],
                               Config(L2create('_2mu2_MSonly_EFFS_L1ZDC'),
                                      EFcreate('_2mu2_MSonly_EFFS_L1ZDC', TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '2GeV', '2GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('2mu2_MSonly_EFFS_L1TE10',
                               'L2_2mu2_MSonly_EFFS_L1TE10', 751, 'L1_TE10',
                               'EF_2mu2_MSonly_EFFS_L1TE10', 751,
                               [''],
                               Config(L2create('_2mu2_MSonly_EFFS_L1TE10'),
                                      EFcreate('_2mu2_MSonly_EFFS_L1TE10', TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '2GeV', '2GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('2mu2_MSonly_EFFS_L1TE20',
                               'L2_2mu2_MSonly_EFFS_L1TE20', 752, 'L1_TE20',
                               'EF_2mu2_MSonly_EFFS_L1TE20', 752,
                               [''],
                               Config(L2create('_2mu2_MSonly_EFFS_L1TE20'),
                                      EFcreate('_2mu2_MSonly_EFFS_L1TE20', TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '2GeV', '2GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('2mu2_MSonly_EFFS_L1TE50',
                               'L2_2mu2_MSonly_EFFS_L1TE50', 753, 'L1_TE50',
                               'EF_2mu2_MSonly_EFFS_L1TE50', 753,
                               [''],
                               Config(L2create('_2mu2_MSonly_EFFS_L1TE50'),
                                      EFcreate('_2mu2_MSonly_EFFS_L1TE50', TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '2GeV', '2GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('mu4_mu2_MSonly_EFFS_L1ZDC',
                               'L2_mu4_mu2_MSonly_EFFS_L1ZDC', 819, minbias_hi_seed,
                               'EF_mu4_mu2_MSonly_EFFS_L1ZDC', 819,
                               [''],
                               Config(L2create('_mu4_mu2_MSonly_EFFS_L1ZDC'),
                                      EFcreate('_mu4_mu2_MSonly_EFFS_L1ZDC', TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '4GeV', '2GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('mu4_mu2_MSonly_EFFS_L1TE10',
                               'L2_mu4_mu2_MSonly_EFFS_L1TE10', 754, 'L1_TE10',
                               'EF_mu4_mu2_MSonly_EFFS_L1TE10', 754,
                               [''],
                               Config(L2create('_mu4_mu2_MSonly_EFFS_L1TE10'),
                                      EFcreate('_mu4_mu2_MSonly_EFFS_L1TE10', TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '4GeV', '2GeV'))),
                               prescale=1, pass_through=0),


    L2EFChain_mu_Unseeded_EFFS('mu4_mu2_MSonly_EFFS_L1TE20',
                               'L2_mu4_mu2_MSonly_EFFS_L1TE20', 755, 'L1_TE20',
                               'EF_mu4_mu2_MSonly_EFFS_L1TE20', 755,
                               [''],
                               Config(L2create('_mu4_mu2_MSonly_EFFS_L1TE20'),
                                      EFcreate('_mu4_mu2_MSonly_EFFS_L1TE20', TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '4GeV', '2GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('mu4_mu2_MSonly_EFFS_L1TE50',
                               'L2_mu4_mu2_MSonly_EFFS_L1TE50', 756, 'L1_TE50',
                               'EF_mu4_mu2_MSonly_EFFS_L1TE50', 756,
                               [''],
                               Config(L2create('_mu4_mu2_MSonly_EFFS_L1TE50'),
                                      EFcreate('_mu4_mu2_MSonly_EFFS_L1TE50', TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '4GeV', '2GeV'))),
                               prescale=1, pass_through=0),


    L2EFChain_mu_Unseeded_EFFS('mu10_MSonly_EFFS_L1ZDC', 
                               'L2_mu10_MSonly_EFFS_L1ZDC', 801, minbias_hi_seed,
                               'EF_mu10_MSonly_EFFS_L1ZDC', 801,
                               [''],
                               Config(L2create('_mu10_MSonly_EFFS_L1ZDC'),
                                      EFcreate('_mu10_MSonly_EFFS_L1ZDC', TrigMuonEFExtrapolatorHypoConfig('Muon','10GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('mu10_MSonly_EFFS_L1TE10', 
                               'L2_mu10_MSonly_EFFS_L1TE10', 757, 'L1_TE10',
                               'EF_mu10_MSonly_EFFS_L1TE10', 757,
                               [''],
                               Config(L2create('_mu10_MSonly_EFFS_L1TE10'),
                                      EFcreate('_mu10_MSonly_EFFS_L1TE10', TrigMuonEFExtrapolatorHypoConfig('Muon','10GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('mu10_MSonly_EFFS_L1TE20', 
                               'L2_mu10_MSonly_EFFS_L1TE20', 758, 'L1_TE20',
                               'EF_mu10_MSonly_EFFS_L1TE20', 758,
                               [''],
                               Config(L2create('_mu10_MSonly_EFFS_L1TE20'),
                                      EFcreate('_mu10_MSonly_EFFS_L1TE20', TrigMuonEFExtrapolatorHypoConfig('Muon','10GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('mu10_MSonly_EFFS_L1TE50', 
                               'L2_mu10_MSonly_EFFS_L1TE50', 759, 'L1_TE50',
                               'EF_mu10_MSonly_EFFS_L1TE50', 759,
                               [''],
                               Config(L2create('_mu10_MSonly_EFFS_L1TE50'),
                                      EFcreate('_mu10_MSonly_EFFS_L1TE50', TrigMuonEFExtrapolatorHypoConfig('Muon','10GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('mu13_MSonly_EFFS_L1ZDC', 
                               'L2_mu13_MSonly_EFFS_L1ZDC', 802, minbias_hi_seed,
                               'EF_mu13_MSonly_EFFS_L1ZDC', 802,
                               [''],
                               Config(L2create('_mu13_MSonly_EFFS_L1ZDC'),
                                      EFcreate('_mu13_MSonly_EFFS_L1ZDC', TrigMuonEFExtrapolatorHypoConfig('Muon','13GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('mu13_MSonly_EFFS_L1TE10', 
                               'L2_mu13_MSonly_EFFS_L1TE10', 760, 'L1_TE10',
                               'EF_mu13_MSonly_EFFS_L1TE10', 760,
                               [''],
                               Config(L2create('_mu13_MSonly_EFFS_L1TE10'),
                                      EFcreate('_mu13_MSonly_EFFS_L1TE10', TrigMuonEFExtrapolatorHypoConfig('Muon','13GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('mu13_MSonly_EFFS_L1TE20', 
                               'L2_mu13_MSonly_EFFS_L1TE20', 761, 'L1_TE20',
                               'EF_mu13_MSonly_EFFS_L1TE20', 761,
                               [''],
                               Config(L2create('_mu13_MSonly_EFFS_L1TE20'),
                                      EFcreate('_mu13_MSonly_EFFS_L1TE20', TrigMuonEFExtrapolatorHypoConfig('Muon','13GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('mu13_MSonly_EFFS_L1TE50', 
                               'L2_mu13_MSonly_EFFS_L1TE50', 762, 'L1_TE50',
                               'EF_mu13_MSonly_EFFS_L1TE50', 762,
                               [''],
                               Config(L2create('_mu13_MSonly_EFFS_L1TE50'),
                                      EFcreate('_mu13_MSonly_EFFS_L1TE50', TrigMuonEFExtrapolatorHypoConfig('Muon','13GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('mu4_MSonly_barrel_EFFS_L1ZDC', 
                               'L2_mu4_MSonly_barrel_EFFS_L1ZDC', 500, minbias_hi_seed,
                               'EF_mu4_MSonly_barrel_EFFS_L1ZDC', 500,
                               [''],
                               Config(L2create('_mu4_MSonly_barrel_EFFS_L1ZDC'),
                                      EFcreate('_mu4_MSonly_barrel_EFFS_L1ZDC', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV_barrelOnly'))),
                               prescale=1, pass_through=0),
    

    L2EFChain_mu_Unseeded_EFFS('mu4_MSonly_EFFS_L1MU0_NZ', 
                               'L2_mu4_MSonly_EFFS_L1MU0_NZ', 979, 'L1_MU0_NZ',
                               'EF_mu4_MSonly_EFFS_L1MU0_NZ', 979,
                               [''],
                               Config(L2create('_mu4_MSonly_EFFS_L1MU0_NZ'),
                                      EFcreate('_mu4_MSonly_EFFS_L1MU0_NZ', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV'))),
                               prescale=1, pass_through=0),

    L2EFChain_mu_Unseeded_EFFS('mu4_MSonly_EFFS_L1TE5_NZ', 
                               'L2_mu4_MSonly_EFFS_L1TE5_NZ', 978, 'L1_TE5_NZ',
                               'EF_mu4_MSonly_EFFS_L1TE5_NZ', 978,
                               [''],
                               Config(L2create('_mu4_MSonly_EFFS_L1TE5_NZ'),
                                      EFcreate('_mu4_MSonly_EFFS_L1TE5_NZ', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV'))),
                               prescale=1, pass_through=0),

    ]
#########################

Config   = L2EFChain_mu_NoTRT_NoL2Trk_noiso.Config
L2Config = L2EFChain_mu_NoTRT_NoL2Trk_noiso.L2Config
EFConfig = L2EFChain_mu_NoTRT_NoL2Trk_noiso.EFConfig

Muons += [
    
    #mu4 is different - uses 900GeV config
    L2EFChain_mu_900_NoTRT_NoL2Trk('mu4_loose', 
                                   'L2_mu4_loose', 704, 'L1_MU0', 'EF_mu4_loose', 704,
                                   ['MU0'],
                                   Config(L2Config('_mu4_loose',
                                                   MufastHypoConfig('900GeV', mufastThr_4GeV),
                                                   None),
                                          EFConfig('_mu4_loose',
                                                   TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),
    ]


Config   = L2EFChain_mu_NoTRT_noiso.Config
L2Config = L2EFChain_mu_NoTRT_noiso.L2Config
EFConfig = L2EFChain_mu_NoTRT_noiso.EFConfig

Muons += [

    #mu4 is different - uses 900GeV config
    L2EFChain_mu_900_NoTRT('mu4', 
                     'L2_mu4', 127, 'L1_MU0', 'EF_mu4', 127,
                     ['MU0'],
                     Config(L2Config('_mu4',
                                     MufastHypoConfig('900GeV', mufastThr_4GeV),
                                     MucombHypoConfig('900GeV', '4GeV')),
                            EFConfig('_mu4',
                                     TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),

    L2EFChain_mu_900_NoTRT('mu4T', 
                           'L2_mu4T', 964, 'L1_MU4', 'EF_mu4T', 964,
                           ['MU4'],
                           Config(L2Config('_mu4T',
                                           MufastHypoConfig('900GeV', mufastThr_4GeV),
                                           MucombHypoConfig('900GeV', '4GeV')),
                                  EFConfig('_mu4T',
                                           TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),

    L2EFChain_mu_900_NoTRT('mu4_cosmic', 
                     'L2_mu4_cosmic', 743, 'L1_MU0_EMPTY', 'EF_mu4_cosmic', 743,
                     ['MU0'],
                     Config(L2Config('_mu4_cosmic',
                                     MufastHypoConfig('900GeV', mufastThr_4GeV),
                                     MucombHypoConfig('900GeV', '4GeV')),
                            EFConfig('_mu4_cosmic',
                                     TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),

    L2EFChain_mu_900_NoTRT('mu4T_cosmic', 
                           'L2_mu4T_cosmic', 963, 'L1_MU4_EMPTY', 'EF_mu4T_cosmic', 963,
                           ['MU4'],
                           Config(L2Config('_mu4T_cosmic',
                                           MufastHypoConfig('900GeV', mufastThr_4GeV),
                                           MucombHypoConfig('900GeV', '4GeV')),
                                  EFConfig('_mu4T_cosmic',
                                           TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),

    L2EFChain_mu_900_NoTRT('mu4_firstempty', 
                     'L2_mu4_firstempty', 2106, 'L1_MU0_FIRSTEMPTY', 'EF_mu4_firstempty', 2106,
                     ['MU0'],
                     Config(L2Config('_mu4_firstempty',
                                     MufastHypoConfig('900GeV', mufastThr_4GeV),
                                     MucombHypoConfig('900GeV', '4GeV')),
                            EFConfig('_mu4_firstempty',
                                     TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),


    L2EFChain_mu_900_NoTRT('mu4_unpaired', 
                     'L2_mu4_unpaired', 2113, 'L1_MU0_UNPAIRED', 'EF_mu4_unpaired', 2113,
                     ['MU0'],
                     Config(L2Config('_mu4_unpaired',
                                     MufastHypoConfig('900GeV', mufastThr_4GeV),
                                     MucombHypoConfig('900GeV', '4GeV')),
                            EFConfig('_mu4_unpaired',
                                     TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),

    L2EFChain_mu_900_NoTRT('mu4_unpaired_iso', 
                     'L2_mu4_unpaired_iso', 2113, 'L1_MU0_UNPAIRED_ISO', 'EF_mu4_unpaired_iso', 2113,
                     ['MU0'],
                     Config(L2Config('_mu4_unpaired_iso',
                                     MufastHypoConfig('900GeV', mufastThr_4GeV),
                                     MucombHypoConfig('900GeV', '4GeV')),
                            EFConfig('_mu4_unpaired_iso',
                                     TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),

    L2EFChain_mu_900_NoTRT('mu4_comm_cosmic', 
                     'L2_mu4_comm_cosmic', 2102, 'L1_MU0_COMM_EMPTY', 'EF_mu4_comm_cosmic', 2102,
                     ['MU0_COMM'],
                     Config(L2Config('_mu4_comm_cosmic',
                                     MufastHypoConfig('900GeV', mufastThr_4GeV),
                                     MucombHypoConfig('900GeV', '4GeV')),
                            EFConfig('_mu4_comm_cosmic',
                                     TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),
    
    L2EFChain_mu_900_NoTRT('mu4_comm_unpaired', 
                     'L2_mu4_comm_unpaired', 2115, 'L1_MU0_COMM_UNPAIRED', 'EF_mu4_comm_unpaired', 2115,
                     ['MU0_COMM'],
                     Config(L2Config('_mu4_comm_unpaired',
                                     MufastHypoConfig('900GeV', mufastThr_4GeV),
                                     MucombHypoConfig('900GeV', '4GeV')),
                            EFConfig('_mu4_comm_unpaired',
                                     TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),

    L2EFChain_mu_900_NoTRT('mu4_comm_unpaired_iso', 
                     'L2_mu4_comm_unpaired_iso', 2115, 'L1_MU0_COMM_UNPAIRED_ISO', 'EF_mu4_comm_unpaired_iso', 2115,
                     ['MU0_COMM'],
                     Config(L2Config('_mu4_comm_unpaired_iso',
                                     MufastHypoConfig('900GeV', mufastThr_4GeV),
                                     MucombHypoConfig('900GeV', '4GeV')),
                            EFConfig('_mu4_comm_unpaired_iso',
                                     TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),

    L2EFChain_mu_900_NoTRT('mu4_comm_firstempty', 
                     'L2_mu4_comm_firstempty', 2116, 'L1_MU0_COMM_FIRSTEMPTY', 'EF_mu4_comm_firstempty', 2116,
                     ['MU0_COMM'],
                     Config(L2Config('_mu4_comm_firstempty',
                                     MufastHypoConfig('900GeV', mufastThr_4GeV),
                                     MucombHypoConfig('900GeV', '4GeV')),
                            EFConfig('_mu4_comm_firstempty',
                                     TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),
    
    L2EFChain_mu_900_MSonly('mu4_MSonly_cosmic', 
                        'L2_mu4_MSonly_cosmic', 745, 'L1_MU0_EMPTY', 
                        'EF_mu4_MSonly_cosmic', 745,
                        ['MU0'],
                        Config(L2Config('_mu4_MSonly_cosmic', 
                                        MufastHypoConfig('900GeV', mufastThr_4GeV), 
                                        None), 
                               EFConfig('_mu4_MSonly_cosmic', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV')))),

    L2EFChain_mu_900_MSonly('mu4T_MSonly_cosmic', 
                            'L2_mu4T_MSonly_cosmic', 962, 'L1_MU4_EMPTY', 
                            'EF_mu4T_MSonly_cosmic', 962,
                            ['MU4'],
                            Config(L2Config('_mu4T_MSonly_cosmic', 
                                            MufastHypoConfig('900GeV', mufastThr_4GeV), 
                                            None), 
                                   EFConfig('_mu4T_MSonly_cosmic', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV')))),

    L2EFChain_mu_900_MSonly('mu4_comm_MSonly_cosmic', 
                            'L2_mu4_comm_MSonly_cosmic', 2103, 'L1_MU0_COMM_EMPTY', 
                            'EF_mu4_comm_MSonly_cosmic', 2103,
                            ['MU0_COMM'],
                            Config(L2Config('_mu4_comm_MSonly_cosmic', 
                                            MufastHypoConfig('900GeV', mufastThr_4GeV), 
                                            None), 
                                   EFConfig('_mu4_comm_MSonly_cosmic', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV')))),
    
    L2EFChain_mu_900_MSonly('mu4_MSonly', 
                        'L2_mu4_MSonly', 809, 'L1_MU0', 
                        'EF_mu4_MSonly', 809,
                        ['MU0'],
                        Config(L2Config('_mu4_MSonly', 
                                        MufastHypoConfig('900GeV', mufastThr_4GeV), 
                                        None), 
                               EFConfig('_mu4_MSonly', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV')))),

    L2EFChain_mu_900_MSonly('mu4_MSonly_L1TE50', 
                            'L2_mu4_MSonly_L1TE50', 945, 'L1_MU0_TE50', 
                            'EF_mu4_MSonly_L1TE50', 945,
                            ['MU0'],
                            Config(L2Config('_mu4_MSonly_L1TE50', 
                                            MufastHypoConfig('900GeV', mufastThr_4GeV), 
                                            None), 
                                   EFConfig('_mu4_MSonly_L1TE50', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV')))),


    L2EFChain_mu_900_MSonly('mu4T_MSonly', 
                            'L2_mu4T_MSonly', 961, 'L1_MU4', 
                            'EF_mu4T_MSonly', 961,
                            ['MU4'],
                            Config(L2Config('_mu4T_MSonly', 
                                            MufastHypoConfig('900GeV', mufastThr_4GeV), 
                                            None), 
                                   EFConfig('_mu4T_MSonly', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV')))),

    L2EFChain_mu_900_MSonly('mu4T_MSonly_L1TE50', 
                            'L2_mu4T_MSonly_L1TE50', 944, 'L1_MU4_TE50', 
                            'EF_mu4T_MSonly_L1TE50', 944,
                            ['MU4'],
                            Config(L2Config('_mu4T_MSonly_L1TE50', 
                                            MufastHypoConfig('900GeV', mufastThr_4GeV), 
                                            None), 
                                   EFConfig('_mu4T_MSonly_L1TE50', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV')))),

    L2EFChain_mu_900_MSonly('mu4_MSonly_barrel', 
                            'L2_mu4_MSonly_barrel', 501, 'L1_MU0', 
                            'EF_mu4_MSonly_barrel', 501,
                            ['MU0'],
                            Config(L2Config('_mu4_MSonly_barrel', 
                                            MufastHypoConfig('900GeV', mufastThr_MSonly_4GeV_barrelOnly), 
                                            None), 
                                   EFConfig('_mu4_MSonly_barrel', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV_barrelOnly')))),

    L2EFChain_mu_900_MSonly('mu4_MSonly_barrel_L1TE50', 
                            'L2_mu4_MSonly_barrel_L1TE50', 702, 'L1_MU0_TE50', 
                            'EF_mu4_MSonly_barrel_L1TE50', 702,
                            ['MU0'],
                            Config(L2Config('_mu4_MSonly_barrel_L1TE50', 
                                            MufastHypoConfig('900GeV', mufastThr_MSonly_4GeV_barrelOnly), 
                                            None), 
                                   EFConfig('_mu4_MSonly_barrel_L1TE50', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV_barrelOnly')))),
    

    L2EFChain_mu_900_MSonly('mu4T_MSonly_barrel', 
                            'L2_mu4T_MSonly_barrel', 960, 'L1_MU4', 
                            'EF_mu4T_MSonly_barrel', 960,
                            ['MU4'],
                            Config(L2Config('_mu4T_MSonly_barrel', 
                                            MufastHypoConfig('900GeV', mufastThr_MSonly_4GeV_barrelOnly), 
                                            None), 
                                   EFConfig('_mu4T_MSonly_barrel', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV_barrelOnly')))),


    L2EFChain_mu_900_MSonly('mu4_MV_MSonly', 
                            'L2_mu4_MV_MSonly', 599, 'L1_MU0_MV', 
                            'EF_mu4_MV_MSonly', 599,
                            ['MU0'],
                        Config(L2Config('_mu4_MV_MSonly', 
                                        MufastHypoConfig('900GeV', mufastThr_4GeV), 
                                        None), 
                               EFConfig('_mu4_MV_MSonly', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV')))),



    L2EFChain_mu_900_NoTRT('mu4_MV', 
                           'L2_mu4_MV', 931, 'L1_MU0_MV', 'EF_mu4_MV', 931,
                           ['MU0'],
                           Config(L2Config('_mu4_MV',
                                           MufastHypoConfig('900GeV', mufastThr_4GeV),
                                           MucombHypoConfig('900GeV', '4GeV')),
                                  EFConfig('_mu4_MV',
                                           TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),

    L2EFChain_mu_900_NoTRT('mu4_L1VTE50', 
                           'L2_mu4_L1VTE50', 937, 'L1_MU0_VTE50', 'EF_mu4_L1VTE50', 937,
                           ['MU0'],
                           Config(L2Config('_mu4_L1VTE50',
                                           MufastHypoConfig('900GeV', mufastThr_4GeV),
                                           MucombHypoConfig('900GeV', '4GeV')),
                                  EFConfig('_mu4_L1VTE50',
                                           TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),


    L2EFChain_mu_900_NoTRT('mu4_barrel_L1VTE50', 
                           'L2_mu4_barrel_L1VTE50', 703, 'L1_MU0_VTE50', 'EF_mu4_barrel_L1VTE50', 703,
                           ['MU0'],
                           Config(L2Config('_mu4_barrel_L1VTE50',
                                           MufastHypoConfig('900GeV', mufastThr_MSonly_4GeV_barrelOnly),
                                           MucombHypoConfig('900GeV', '4GeV')),
                                  EFConfig('_mu4_barrel_L1VTE50',
                                           TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),

    L2EFChain_mu_900_NoTRT_NoIDTrkCut('mu4_IDTrkNoCut', 
                                      'L2_mu4_IDTrkNoCut', 843, 'L1_MU0', 'EF_mu4_IDTrkNoCut', 843,
                                      ['MU0'],
                                      Config(L2Config('_mu4_IDTrkNoCut',
                                                      MufastHypoConfig('900GeV', mufastThr_4GeV),
                                                      MucombHypoConfig('900GeV', 'passthrough')),
                                             EFConfig('_mu4_IDTrkNoCut',
                                                      TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV')))),

    L2EFChain_mu_900_NoTRT_NoIDTrkCut('mu4T_IDTrkNoCut', 
                                      'L2_mu4T_IDTrkNoCut', 959, 'L1_MU4', 'EF_mu4T_IDTrkNoCut', 959,
                                      ['MU4'],
                                      Config(L2Config('_mu4T_IDTrkNoCut',
                                                      MufastHypoConfig('900GeV', mufastThr_4GeV),
                                                      MucombHypoConfig('900GeV', 'passthrough')),
                                             EFConfig('_mu4T_IDTrkNoCut',
                                                      TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV')))),

    L2EFChain_mu_NoTRT_noiso('mu6_L1VTE50', 
                           'L2_mu6_L1VTE50', 700, 'L1_MU0_VTE50', 'EF_mu6_L1VTE50', 700,
                           ['MU0'],
                           Config(L2Config('_mu6_L1VTE50',
                                           MufastHypoConfig('Muon', mufastThr_6GeV),
                                           MucombHypoConfig('Muon', '6GeV')),
                                  EFConfig('_mu6_L1VTE50',
                                           TrigMuonEFCombinerHypoConfig('Muon', '6GeV')))),



    L2EFChain_mu_MSonly('mu6_MSonly_L1TE50', 
                            'L2_mu6_MSonly_L1TE50', 701, 'L1_MU0_TE50', 
                            'EF_mu6_MSonly_L1TE50', 701,
                            ['MU0'],
                            Config(L2Config('_mu6_MSonly_L1TE50', 
                                            MufastHypoConfig('Muon', mufastThr_6GeV), 
                                            None), 
                                   EFConfig('_mu6_MSonly_L1TE50', TrigMuonEFExtrapolatorHypoConfig('MUon','6GeV')))),


    L2EFChain_mu_NoTRT_NoIDTrkCut('mu6_IDTrkNoCut', 
                            'L2_mu6_IDTrkNoCut', 810, 'L1_MU0', 'EF_mu6_IDTrkNoCut', 810,
                            ['MU0'],
                            Config(L2Config('_mu6_IDTrkNoCut',
                                            MufastHypoConfig('Muon', mufastThr_6GeV),
                                            MucombHypoConfig('Muon', 'passthrough')),
                                   EFConfig('_mu6_IDTrkNoCut',
                                            TrigMuonEFExtrapolatorHypoConfig('Muon','6GeV')))),
    
    L2EFChain_mu_NoTRT_NoIDTrkCut('mu6T_IDTrkNoCut', 
                                  'L2_mu6T_IDTrkNoCut', 958, 'L1_MU4', 'EF_mu6T_IDTrkNoCut', 958,
                                  ['MU4'],
                                  Config(L2Config('_mu6T_IDTrkNoCut',
                                                  MufastHypoConfig('Muon', mufastThr_6GeV),
                                                  MucombHypoConfig('Muon', 'passthrough')),
                                         EFConfig('_mu6T_IDTrkNoCut',
                                                  TrigMuonEFExtrapolatorHypoConfig('Muon','6GeV')))),

    L2EFChain_mu_900_MSonly_wOvlpRm('mu4_MSonly_wOvlpRm', 
                                    'L2_mu4_MSonly_wOvlpRm', 6809, 'L1_MU0', 
                                    'EF_mu4_MSonly_wOvlpRm', 6809,
                                    ['MU0'],
                                    Config(L2Config('_mu4_MSonly_wOvlpRm', 
                                                    MufastHypoConfig('900GeV', mufastThr_4GeV), 
                                                    None), 
                                           EFConfig('_mu4_MSonly_wOvlpRm', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV')))),


    L2EFChain_mu_900_MSonly_wOvlpRm('mu4T_MSonly_wOvlpRm', 
                                    'L2_mu4T_MSonly_wOvlpRm', 6810, 'L1_MU4', 
                                    'EF_mu4T_MSonly_wOvlpRm', 6810,
                                    ['MU4'],
                                    Config(L2Config('_mu4T_MSonly_wOvlpRm', 
                                                    MufastHypoConfig('900GeV', mufastThr_4GeV), 
                                                    None), 
                                           EFConfig('_mu4T_MSonly_wOvlpRm', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV')))),
    

    L2EFChain_mu_MSonly('mu6_MSonly', 
                        'L2_mu6_MSonly', 828, 'L1_MU0', 
                        'EF_mu6_MSonly', 828,
                        ['MU0'],
                        Config(L2Config('_mu6_MSonly', 
                                        MufastHypoConfig('Muon', mufastThr_6GeV), 
                                        None),
                               EFConfig('_mu6_MSonly', TrigMuonEFExtrapolatorHypoConfig('Muon','6GeV')))),

    L2EFChain_mu_MSonly('mu6T_MSonly', 
                        'L2_mu6T_MSonly', 957, 'L1_MU4', 
                        'EF_mu6T_MSonly', 957,
                        ['MU4'],
                        Config(L2Config('_mu6T_MSonly', 
                                        MufastHypoConfig('Muon', mufastThr_6GeV), 
                                        None),
                               EFConfig('_mu6T_MSonly', TrigMuonEFExtrapolatorHypoConfig('Muon','6GeV')))),
    
    L2EFChain_mu_MSonly_wOvlpRm('mu6_MSonly_wOvlpRm', 
                                'L2_mu6_MSonly_wOvlpRm', 6129, 'L1_MU0', 'EF_mu6_MSonly_wOvlpRm', 6129,
                                ['MU0'],
                                Config(L2Config('_mu6_MSonly_wOvlpRm', 
                                                MufastHypoConfig('Muon', mufastThr_6GeV), 
                                                None),
                                       EFConfig('_mu6_MSonly_wOvlpRm', TrigMuonEFExtrapolatorHypoConfig('Muon','6GeV')))),

    L2EFChain_mu_MSonly_wOvlpRm('mu6T_MSonly_wOvlpRm', 
                                'L2_mu6T_MSonly_wOvlpRm', 6130, 'L1_MU4', 'EF_mu6T_MSonly_wOvlpRm', 6130,
                                ['MU4'],
                                Config(L2Config('_mu6T_MSonly_wOvlpRm', 
                                                MufastHypoConfig('Muon', mufastThr_6GeV), 
                                                None),
                                       EFConfig('_mu6T_MSonly_wOvlpRm', TrigMuonEFExtrapolatorHypoConfig('Muon','6GeV')))),

    L2EFChain_mu_MSonly('mu10_MSonly', 
                        'L2_mu10_MSonly', 812, 'L1_MU0', 
                        'EF_mu10_MSonly', 812,
                        ['MU0'],
                        Config(L2Config('_mu10_MSonly', 
                                        MufastHypoConfig('Muon', mufastThr_MSonly_10GeV), 
                                        None),
                               EFConfig('_mu10_MSonly', TrigMuonEFExtrapolatorHypoConfig('Muon','10GeV')))),

    L2EFChain_mu_MSonly('mu10T_MSonly', 
                        'L2_mu10T_MSonly', 956, 'L1_MU4', 
                        'EF_mu10T_MSonly', 956,
                        ['MU4'],
                        Config(L2Config('_mu10T_MSonly', 
                                        MufastHypoConfig('Muon', mufastThr_MSonly_10GeV), 
                                        None),
                               EFConfig('_mu10T_MSonly', TrigMuonEFExtrapolatorHypoConfig('Muon','10GeV')))),

    L2EFChain_mu_MSonly('mu13_MSonly', 
                        'L2_mu13_MSonly', 811, 'L1_MU0', 
                        'EF_mu13_MSonly', 811,
                        ['MU0'],
                        Config(L2Config('_mu13_MSonly', 
                                        MufastHypoConfig('Muon', mufastThr_MSonly_13GeV), 
                                        None),
                               EFConfig('_mu13_MSonly', TrigMuonEFExtrapolatorHypoConfig('Muon','13GeV')))),

    L2EFChain_mu_MSonly('mu13T_MSonly', 
                        'L2_mu13T_MSonly', 955, 'L1_MU4', 
                        'EF_mu13T_MSonly', 955,
                        ['MU4'],
                        Config(L2Config('_mu13T_MSonly', 
                                        MufastHypoConfig('Muon', mufastThr_MSonly_13GeV), 
                                        None),
                               EFConfig('_mu13T_MSonly', TrigMuonEFExtrapolatorHypoConfig('Muon','13GeV')))),
    
    L2EFChain_mu_NoTRT_noiso('mu10', 
                             'L2_mu10', 130, 'L1_MU0', 'EF_mu10', 130,
                             ['MU0'],
                             Config(L2Config('_mu10',
                                             MufastHypoConfig('Muon', mufastThr_10GeV),
                                             MucombHypoConfig('Muon', '10GeV')),
                                    EFConfig('_mu10',
                                             TrigMuonEFCombinerHypoConfig('Muon', '10GeV')))),

    L2EFChain_mu_NoTRT_noiso('mu10_tight',
                       'L2_mu10_tight', 8130, 'L1_MU10', 'EF_mu10_tight', 8130,
                       ['MU10'],
                       Config(L2Config('_mu10_tight',
                                       MufastHypoConfig('Muon', mufastThr_10GeV),
                                       MucombHypoConfig('Muon', '10GeV')),
                              EFConfig('_mu10_tight',
                                       TrigMuonEFCombinerHypoConfig('Muon', '10GeV')))),


    L2EFChain_mu_NoTRT_noiso('mu15_medium', 
                             'L2_mu15_medium', 139, 'L1_MU11', 'EF_mu15_medium', 139,
                             ['MU11'],
                             Config(L2Config('_mu15_medium',
                                             MufastHypoConfig('Muon', mufastThr_15GeV),
                                             MucombHypoConfig('Muon', '15GeV')),
                                    EFConfig('_mu15_medium',
                                             TrigMuonEFCombinerHypoConfig('Muon', '15GeV'))),rerun=1),
    
     L2EFChain_mu_NoTRT_noiso('mu13_tight', 
                       'L2_mu13_tight', 8131, 'L1_MU10', 'EF_mu13_tight', 8131,
                       ['MU10'],
                       Config(L2Config('_mu13_tight',
                                       MufastHypoConfig('Muon', mufastThr_13GeV),
                                       MucombHypoConfig('Muon', '13GeV')),
                        EFConfig('_mu13_tight',
                                 TrigMuonEFCombinerHypoConfig('Muon', '13GeV')))),
    
    L2EFChain_mu_NoTRT_noiso('mu6', 
                             'L2_mu6', 129, 'L1_MU0', 'EF_mu6', 129,
                             ['MU0'],
                             Config(L2Config('_mu6',
                                             MufastHypoConfig('Muon', mufastThr_6GeV),
                                             MucombHypoConfig('Muon', '6GeV')),
                                    EFConfig('_mu6',
                                             TrigMuonEFCombinerHypoConfig('Muon', '6GeV')))),

    L2EFChain_mu_NoTRT_noiso('mu6T', 
                             'L2_mu6T', 130, 'L1_MU4', 'EF_mu6T', 130,
                             ['MU4'],
                             Config(L2Config('_mu6T',
                                             MufastHypoConfig('Muon', mufastThr_6GeV),
                                             MucombHypoConfig('Muon', '6GeV')),
                                    EFConfig('_mu6T',
                                             TrigMuonEFCombinerHypoConfig('Muon', '6GeV')))),

    L2EFChain_mu_NoTRT_noiso('mu15', 
                             'L2_mu15', 131, 'L1_MU10', 'EF_mu15', 131,
                             ['MU10'],
                             Config(L2Config('_mu15',
                                             MufastHypoConfig('Muon', mufastThr_15GeV),
                                             MucombHypoConfig('Muon', '15GeV')),
                                    EFConfig('_mu15',
                                             TrigMuonEFCombinerHypoConfig('Muon', '15GeV')))),

    L2EFChain_mu_NoTRT_noiso('mu20', 
                             'L2_mu20', 134, 'L1_MU10', 'EF_mu20', 134,
                             ['MU10'],
                             Config(L2Config('_mu20',
                                             MufastHypoConfig('Muon', mufastThr_20GeV),
                                             MucombHypoConfig('Muon', '20GeV')), 
                                    EFConfig('_mu20',
                                             TrigMuonEFCombinerHypoConfig('Muon', '20GeV')))),
 
     L2EFChain_mu_NoTRT_noiso('mu40',
                       'L2_mu40', 133, 'L1_MU40', 'EF_mu40', 133,
                       ['MU40'],
                       Config(L2Config('_mu40',
                                       MufastHypoConfig('Muon', mufastThr_40GeV),
                                       MucombHypoConfig('Muon', '40GeV')), 
                              EFConfig('_mu40',
                                       TrigMuonEFCombinerHypoConfig('Muon', '40GeV')))),
    L2EFChain_mu_iso('mu10i_loose',
                     'L2_mu10i_loose', 599, 'L1_MU0', 'EF_mu10i_loose', 599,
                     ['MU0'],
                     Config(L2Config('_mu10i_loose',
                                     MufastHypoConfig('Muon', mufastThr_10GeV),
                                     MucombHypoConfig('Muon', '10GeV')),
                            EFConfig('_mu10i_loose',
                                     TrigMuonEFCombinerHypoConfig('Muon', '10GeV')))),

    L2EFChain_mu_iso('mu15i_loose',
                     'L2_mu15i_loose', 483, 'L1_MU15', 'EF_mu15i_loose', 483,
                     ['MU15'],
                     Config(L2Config('_mu15i_loose',
                                     MufastHypoConfig('Muon', mufastThr_15GeV),
                                     MucombHypoConfig('Muon', '15GeV')), 
                            EFConfig('_mu15i_loose',
                                     TrigMuonEFCombinerHypoConfig('Muon', '15GeV')))),

    L2EFChain_mu_iso('mu20i_loose',
                     'L2_mu20i_loose', 132, 'L1_MU10', 'EF_mu20i_loose', 132,
                     ['MU10'],
                     Config(L2Config('_mu20i_loose',
                                     MufastHypoConfig('Muon', mufastThr_20GeV),
                                     MucombHypoConfig('Muon', '20GeV')), 
                            EFConfig('_mu20i_loose',
                                     TrigMuonEFCombinerHypoConfig('Muon', '20GeV')))),

    ]


Config   = L2EFChain_mu_noroi.Config
L2Config = L2EFChain_mu_noroi.L2Config
EFConfig = L2EFChain_mu_noroi.EFConfig

Muons +=[
    L2EFChain_mu_NoAlg('L1MU0_NoAlg', 
                       'L2_L1MU0_NoAlg', 287, 'L1_MU0', 
                       'EF_L1MU0_NoAlg', 287,
                       [], Config(L2Config(''), EFConfig(''))),
    L2EFChain_mu_NoAlg('L1MU4_NoAlg', 
                       'L2_L1MU4_NoAlg', 288, 'L1_MU4', 
                       'EF_L1MU4_NoAlg', 288,
                       [], Config(L2Config(''), EFConfig(''))),
    L2EFChain_mu_NoAlg('L1MU11_NoAlg', 
                       'L2_L1MU11_NoAlg', 286, 'L1_MU11', 
                       'EF_L1MU11_NoAlg', 286,
                       [], Config(L2Config(''), EFConfig(''))),
    L2EFChain_mu_NoAlg('L1MU15_NoAlg', 
                       'L2_L1MU15_NoAlg', 684, 'L1_MU15', 
                       'EF_L1MU15_NoAlg', 684,
                       [], Config(L2Config(''), EFConfig(''))),
    L2EFChain_mu_NoAlg('L1MU20_NoAlg', 
                       'L2_L1MU20_NoAlg', 685, 'L1_MU20', 
                       'EF_L1MU20_NoAlg', 685,
                       [], Config(L2Config(''), EFConfig(''))),
    ]


Config = L2EFChain_mu_tile_v1.Config
L2Config = L2EFChain_mu_tile_v1.L2Config
EFConfig = L2EFChain_mu_tile_v1.EFConfig

Muons += [
    
    L2EFChain_mu_tile_v1('mu4_tile_cosmic', 
                      'L2_mu4_tile_cosmic', 747, 'L1_MU0_EMPTY', 'EF_mu4_tile_cosmic', 747,
                      ['MU0'],
                      Config(L2Config('_mu4_tile_cosmic',
                                      TileMuHypoConfig('900GeV', '4GeV') ),
                             EFConfig('_mu4_tile_cosmic',
                                      TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),
    
    ]

Config   = L2EFChain_out_of_time.Config
L2Config = L2EFChain_out_of_time.L2Config
EFConfig = L2EFChain_out_of_time.EFConfig

Muons += [
    L2EFChain_mu_cal('mu4_l2cal_empty', 
                     'L2_mu4_cal_empty', 1200, 'L1_MU0_EMPTY', 
                     '', -1 , 
                     ['MU0'], 
                     Config(L2Config('_mu4_cal_empty', MufastCALHypoConfig('Mcal','Rejection')),None)),

    L2EFChain_mu_cal('mu4T_l2cal', 
                     'L2_mu4T_cal', 1300, 'L1_MU4', 
                     '', -1 , 
                     ['MU4'], 
                     Config(L2Config('_mu4T_cal', MufastCALHypoConfig('Mcal','Rejection')),None)),
    
    L2EFChain_mu_cal('mu6_l2cal',
                     'L2_mu6_cal', 1100, 'L1_MU6',
                     '', -1 ,
                     ['MU6'],
                     Config(L2Config('_mu6_cal', MufastCALHypoConfig('Mcal','Rejection')),None)),
    ]

Config   = L2EFChain_mu_Seeded_EFFS_HI.Config
L2Config = L2EFChain_mu_Seeded_EFFS_HI.L2Config
EFConfig = L2EFChain_mu_Seeded_EFFS_HI.EFConfig

Muons += [
    L2EFChain_mu_Seeded_EFFS_HI('2mu4_MSonly_EFFS_L1MU0', 
                                'L2_mu4_MSonly', 839, 'L1_MU0',
                                'EF_2mu4_MSonly_EFFS_L1MU0', 839,
                                ['MU0'],
                                Config(L2Config('_mu4_MSonly',
                                                MufastHypoConfig('900GeV', mufastThr_4GeV), None),
                                       EFConfig('_2mu4_MSonly_EFFS_L1MU0',
                                                TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV'),
                                                TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '4GeV', '4GeV')))),
    ]

###########################################################################
#multi muons
from TriggerMenuPython.MultiMuon import *

MultiMuons = [
    MultiMuon( '2mu4_MSonly',          'L1_2MU0',  ['L2_mu4_MSonly_wOvlpRm']*2,   '886',  ['EF_mu4_MSonly_wOvlpRm']*2,  '886'),

    MultiMuon( '2mu6_MSonly',          'L1_2MU0',  ['L2_mu6_MSonly_wOvlpRm']*2,   '730',  ['EF_mu6_MSonly_wOvlpRm']*2,  '730'),

    MultiMuon( '2mu4T_MSonly',          'L1_2MU4',  ['L2_mu4T_MSonly_wOvlpRm']*2,   '949',  ['EF_mu4T_MSonly_wOvlpRm']*2,  '949'),
    MultiMuon( '2mu6T_MSonly',          'L1_2MU4',  ['L2_mu6T_MSonly_wOvlpRm']*2,   '731',  ['EF_mu6T_MSonly_wOvlpRm']*2,  '731'),
    ]
#commissioning chains for 2010
###########################################################################

if __name__ == '__main__':
    triggerPythonConfig = TriggerPythonConfig('hlt.xml', None)
    for m in Muons:
        m.generateMenu(triggerPythonConfig)
    triggerPythonConfig.writeConfigFiles()
