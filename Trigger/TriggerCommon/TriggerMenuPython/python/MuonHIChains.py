# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################
# SliceDef file for Muon chains
###########################################################################
__author__  = 'I.Grabowska-Bold'
__version__=""
__doc__="Definition of muon chains as used in Physics_HI_v1/2 menu"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)

from TriggerMenuPython.MuonDef import *

Muons = []


# definde seed for HI
#minbias_hi_seed = "L1_ZDC_A_C"

Muons += [

    #mu4 is different - uses 900GeV config
    L2EFChain_mu_autoconfig('mu2',  537, l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0', rerun=1),
    L2EFChain_mu_autoconfig('mu4',  127, l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0', rerun=1),
    L2EFChain_mu_autoconfig('mu4T', 964, l2_inputTEs=['MU4'], l2_lower_chain_name='L1_MU4'),
    L2EFChain_mu_autoconfig('mu4_cosmic',  743, l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0_EMPTY'),
    L2EFChain_mu_autoconfig('mu4T_cosmic',  963, l2_inputTEs=['MU4'], l2_lower_chain_name='L1_MU4_EMPTY'),
    L2EFChain_mu_autoconfig('mu4_firstempty',  2106, l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0_FIRSTEMPTY'),
    L2EFChain_mu_autoconfig('mu4_unpaired',  2113, l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0_UNPAIRED'),
    L2EFChain_mu_autoconfig('mu4_unpaired_iso',  2113, l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0_UNPAIRED_ISO'),
    L2EFChain_mu_autoconfig('mu4_comm_cosmic',  2102, l2_inputTEs=['MU0_COMM'], l2_lower_chain_name='L1_MU0_COMM_EMPTY'),
    L2EFChain_mu_autoconfig('mu4_comm_unpaired',  2115, l2_inputTEs=['MU0_COMM'], l2_lower_chain_name='L1_MU0_COMM_UNPAIRED'),
    L2EFChain_mu_autoconfig('mu4_comm_unpaired_iso',  2115, l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0_UNPAIRED_ISO'),
    L2EFChain_mu_autoconfig('mu4_comm_firstempty',  2116, l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0_FIRSTEMPTY'),
    L2EFChain_mu_autoconfig('mu4_MV', 931,  l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0_MV'),
    L2EFChain_mu_autoconfig('mu4_L1VTE50', 937,  l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0_VTE50'),
    L2EFChain_mu_autoconfig('mu4_barrel_L1VTE50', 703,  l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0_VTE50'),
    L2EFChain_mu_autoconfig('mu6_L1VTE50', 700,  l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0_VTE50'),
    L2EFChain_mu_autoconfig('mu8', 373,  l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0'),
#    L2EFChain_mu_autoconfig('mu8_newl2muonSA', 372,  l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0', l2_alg_name='MuonNewGeometry'),
    L2EFChain_mu_autoconfig('mu10', 374,  l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0'),
    L2EFChain_mu_autoconfig('mu10_tight', 8130,  l2_inputTEs=['MU10'], l2_lower_chain_name='L1_MU10'),
    L2EFChain_mu_autoconfig('mu15_medium', 139,  l2_inputTEs=['MU11'], l2_lower_chain_name='L1_MU11'),
    L2EFChain_mu_autoconfig('mu13_tight', 8131,  l2_inputTEs=['MU10'], l2_lower_chain_name='L1_MU10'),
    L2EFChain_mu_autoconfig('mu6',  129, l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0'),
    L2EFChain_mu_autoconfig('mu6T', 130, l2_inputTEs=['MU4'], l2_lower_chain_name='L1_MU4'),
    L2EFChain_mu_autoconfig('mu15', 131, l2_inputTEs=['MU10'], l2_lower_chain_name='L1_MU10'),
    L2EFChain_mu_autoconfig('mu20', 134, l2_inputTEs=['MU10'], l2_lower_chain_name='L1_MU10'),
    L2EFChain_mu_autoconfig('mu40', 133, l2_inputTEs=['MU40'], l2_lower_chain_name='L1_MU40'),

    L2EFChain_mu_autoconfig('mu4_wOvlpRm', 1930),
    ]

Muons += [
    L2EFChain_mu_MSonly('mu4_MSonly_cosmic', 1400, l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0_EMPTY'),
    L2EFChain_mu_MSonly('mu4T_MSonly_cosmic', 962, l2_inputTEs=['MU4'], l2_lower_chain_name='L1_MU4_EMPTY'),
    L2EFChain_mu_MSonly('mu4_comm_MSonly_cosmic', 2103, l2_inputTEs=['MU0_COMM'], l2_lower_chain_name='L1_MU0_COMM_EMPTY'),
    L2EFChain_mu_MSonly('mu4_MSonly', 809, l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0'),
    L2EFChain_mu_MSonly('mu4_MSonly_L1TE50', 945,  l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0_TE50'),

    L2EFChain_mu_MSonly('mu4T_MSonly', 961, l2_inputTEs=['MU4'], l2_lower_chain_name='L1_MU4'),
    L2EFChain_mu_MSonly('mu4T_MSonly_L1TE50', 944,  l2_inputTEs=['MU4'], l2_lower_chain_name='L1_MU4_TE50'),

    L2EFChain_mu_MSonly('mu4_MSonly_barrel', 501, l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0'),
    L2EFChain_mu_MSonly('mu4_MSonly_barrel_L1TE50', 702,  l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0_TE50'),
    L2EFChain_mu_MSonly('mu4T_MSonly_barrel', 125, l2_inputTEs=['MU4'], l2_lower_chain_name='L1_MU4'),
    L2EFChain_mu_MSonly('mu4_MV_MSonly', 599,  l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0_MV'),
    L2EFChain_mu_MSonly('mu6_MSonly_L1TE50', 701,  l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0_TE50'),
    L2EFChain_mu_MSonly('mu6_MSonly', 828, l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0'),
    L2EFChain_mu_MSonly('mu6T_MSonly', 957, l2_inputTEs=['MU4'], l2_lower_chain_name='L1_MU4'),
    L2EFChain_mu_MSonly('mu10_MSonly', 812, l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0'),
    L2EFChain_mu_MSonly('mu10T_MSonly', 956, l2_inputTEs=['MU4'], l2_lower_chain_name='L1_MU4'),
    L2EFChain_mu_MSonly('mu13_MSonly', 811, l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0'),
    L2EFChain_mu_MSonly('mu13T_MSonly', 955, l2_inputTEs=['MU4'], l2_lower_chain_name='L1_MU4'),

    L2EFChain_mu_MSonly('mu4_MSonly_wOvlpRm', 6809,l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0'),
    L2EFChain_mu_MSonly('mu4T_MSonly_wOvlpRm', 6810,l2_inputTEs=['MU4'], l2_lower_chain_name='L1_MU4'),
    L2EFChain_mu_MSonly('mu6T_MSonly_wOvlpRm', 6130,l2_inputTEs=['MU4'], l2_lower_chain_name='L1_MU4'),
    L2EFChain_mu_MSonly('mu6_MSonly_wOvlpRm', 6129,l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0'),
    L2EFChain_mu_MSonly('mu4_IDTrkNoCut',       843, l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0'),
    L2EFChain_mu_MSonly('mu4T_IDTrkNoCut',       959, l2_inputTEs=['MU4'], l2_lower_chain_name='L1_MU4'),
    L2EFChain_mu_MSonly('mu6_IDTrkNoCut',       810, l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0'),
    L2EFChain_mu_MSonly('mu6T_IDTrkNoCut',       958, l2_inputTEs=['MU4'], l2_lower_chain_name='L1_MU4'),
    L2EFChain_mu_MSonly('mu8_IDTrkNoCut',       262, l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0'),  
    L2EFChain_mu_MSonly('mu10_IDTrkNoCut',       263, l2_inputTEs=['MU0'], l2_lower_chain_name='L1_MU0'),
    ]

mu2_tight_TMEFonly = L2EFChain_mu_autoconfig('mu2_TMEFonly',  537, 418, rerun=1)
mu4_tight_TMEFonly = L2EFChain_mu_autoconfig('mu4_TMEFonly',  127, 419, rerun=1)


Muons += [
    #L2EFChain_mu_Seeded_EFFS('2mu2_EFFS_L1MU0',  545),
    L2EFChain_mu_EFFSonly('2mu2_EFFS_L1MU0',  132, 2, TrigMuonEFCombinerMultiHypoConfig('900GeV', '2GeV', '2GeV'), l2_lower_chain_name='L1_MU0'),
    L2EFChain_mu_EFFSonly('mu4_mu2_EFFS_L1MU0',  664, 2, TrigMuonEFCombinerMultiHypoConfig('900GeV', '2GeV', '4GeV'), l2_lower_chain_name='L1_MU0'),
    mu2_tight_TMEFonly,
    mu4_tight_TMEFonly,
    L2EFChain_mu_Seeded_EFFS('2mu2_EFFS',  mu2_tight_TMEFonly,  '2mu2_EFFS_L1MU0',  534),
    L2EFChain_mu_Seeded_EFFS('mu4_mu2_EFFS',  mu4_tight_TMEFonly,  'mu4_mu2_EFFS_L1MU0',  416),
    ]

#streamers for L1
Muons +=[
    L2EFChain_mu_NoAlg('L1MU0_NoAlg', 287, 'L1_MU0'),
    L2EFChain_mu_NoAlg('L1MU4_NoAlg', 288, 'L1_MU4'),
    L2EFChain_mu_NoAlg('L1MU11_NoAlg', 286, 'L1_MU11'),
    L2EFChain_mu_NoAlg('L1MU15_NoAlg', 684, 'L1_MU15'),
    L2EFChain_mu_NoAlg('L1MU20_NoAlg', 685, 'L1_MU20'),
    ]


#calibration
Muons += [
    L2EFChain_mu_cal('mu4_l2cal_empty', 1200, 'L1_MU0_EMPTY'),
    L2EFChain_mu_cal('mu4T_l2cal', 1300, 'L1_MU4'),
    L2EFChain_mu_cal('mu6_l2cal', 1100, 'L1_MU6'),
    ]

#Full scan muons
# definde seed for HI
minbias_hi_seed = "L1_ZDC_A_C"

Muons += [
    L2EFChain_mu_EFFSonly_HI('mu4_MSonly_MB2_noL2_EFFS', 2076,
                             EFCB_FS_Hypo=TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV'), l2_inputTEs=[""], l2_lower_chain_name='L1_MBTS_2'),
    L2EFChain_mu_EFFSonly_HI('mu4_MSonly_EFFS_L1ZDC_OR', 850,
                             EFCB_FS_Hypo=TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV'), l2_inputTEs=[""], l2_lower_chain_name='L1_ZDC'),
    L2EFChain_mu_EFFSonly_HI('mu4_MSonly_EFFS_L1MBTS', 851, EFCB_FS_Hypo=TrigMuonEFExtrapolatorHypoConfig('Muon','4GeV'),l2_inputTEs=[""], l2_lower_chain_name='L1_MBTS_2'),

    
    L2EFChain_mu_EFFSonly_HI('mu4_MSonly_EFFS_L1ZDC', 303,
                             EFCB_FS_Hypo=TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV'), l2_inputTEs=[""], l2_lower_chain_name=minbias_hi_seed),
    L2EFChain_mu_EFFSonly_HI('mu4_MSonly_EFFS_L1TE10', 836,
                             EFCB_FS_Hypo=TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV'), l2_inputTEs=[""], l2_lower_chain_name='L1_TE10'),
    L2EFChain_mu_EFFSonly_HI('mu4_MSonly_EFFS_L1TE20', 814,
                             EFCB_FS_Hypo=TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV'), l2_inputTEs=[""], l2_lower_chain_name='L1_TE20'),
    L2EFChain_mu_EFFSonly_HI('mu4_MSonly_EFFS_L1TE50', 815,
                             EFCB_FS_Hypo=TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV'), l2_inputTEs=[""], l2_lower_chain_name='L1_TE50'),
    L2EFChain_mu_EFFSonly_HI('mu10_MSonly_EFFS_L1ZDC', 801,
                             EFCB_FS_Hypo=TrigMuonEFExtrapolatorHypoConfig('Muon','10GeV'), l2_inputTEs=[""], l2_lower_chain_name=minbias_hi_seed),
    L2EFChain_mu_EFFSonly_HI('mu10_MSonly_EFFS_L1TE10', 757,
                             EFCB_FS_Hypo=TrigMuonEFExtrapolatorHypoConfig('Muon','10GeV'), l2_inputTEs=[""], l2_lower_chain_name='L1_TE10'),
    L2EFChain_mu_EFFSonly_HI('mu10_MSonly_EFFS_L1TE20', 758,
                             EFCB_FS_Hypo=TrigMuonEFExtrapolatorHypoConfig('Muon','10GeV'), l2_inputTEs=[""], l2_lower_chain_name='L1_TE20'),
    L2EFChain_mu_EFFSonly_HI('mu10_MSonly_EFFS_L1TE50', 759,
                             EFCB_FS_Hypo=TrigMuonEFExtrapolatorHypoConfig('Muon','10GeV'), l2_inputTEs=[""], l2_lower_chain_name='L1_TE50'),
    L2EFChain_mu_EFFSonly_HI('mu13_MSonly_EFFS_L1ZDC', 802,
                             EFCB_FS_Hypo=TrigMuonEFExtrapolatorHypoConfig('Muon','13GeV'), l2_inputTEs=[""], l2_lower_chain_name=minbias_hi_seed),
    L2EFChain_mu_EFFSonly_HI('mu13_MSonly_EFFS_L1TE10', 760,
                             EFCB_FS_Hypo=TrigMuonEFExtrapolatorHypoConfig('Muon','13GeV'), l2_inputTEs=[""], l2_lower_chain_name='L1_TE10'),
    L2EFChain_mu_EFFSonly_HI('mu13_MSonly_EFFS_L1TE20', 761,
                             EFCB_FS_Hypo=TrigMuonEFExtrapolatorHypoConfig('Muon','13GeV'), l2_inputTEs=[""], l2_lower_chain_name='L1_TE20'),
    L2EFChain_mu_EFFSonly_HI('mu13_MSonly_EFFS_L1TE50', 762,
                             EFCB_FS_Hypo=TrigMuonEFExtrapolatorHypoConfig('Muon','13GeV'), l2_inputTEs=[""], l2_lower_chain_name='L1_TE50'),
    L2EFChain_mu_EFFSonly_HI('mu4_MSonly_barrel_EFFS_L1ZDC', 500,
                             EFCB_FS_Hypo=TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV_barrelOnly'), l2_inputTEs=[""], l2_lower_chain_name=minbias_hi_seed),
    L2EFChain_mu_EFFSonly_HI('mu4_MSonly_EFFS_L1MU0_NZ', 979,
                             EFCB_FS_Hypo=TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV'), l2_inputTEs=[""], l2_lower_chain_name='L1_MU0_NZ'),
    L2EFChain_mu_EFFSonly_HI('mu4_MSonly_EFFS_L1TE5_NZ', 978,
                             EFCB_FS_Hypo=TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV'), l2_inputTEs=[""], l2_lower_chain_name='L1_TE5_NZ'),
    

    L2EFChain_mu_EFFSonly_HI('2mu4_MSonly_EFFS_L1ZDC', 803,EFCB_FS_Hypo=TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '4GeV', '4GeV'),l2_inputTEs=[""], l2_lower_chain_name='L1_ZDC_A_C',n_muons=2),
    L2EFChain_mu_EFFSonly_HI('2mu4_MSonly_EFFS_L1TE10', 816,EFCB_FS_Hypo=TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '4GeV', '4GeV'),l2_inputTEs=[""], l2_lower_chain_name='L1_TE10',n_muons=2),
    L2EFChain_mu_EFFSonly_HI('2mu4_MSonly_EFFS_L1TE20', 817,EFCB_FS_Hypo=TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '4GeV', '4GeV'),l2_inputTEs=[""], l2_lower_chain_name='L1_TE20',n_muons=2),
    L2EFChain_mu_EFFSonly_HI('2mu4_MSonly_EFFS_L1TE50', 818,EFCB_FS_Hypo=TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '4GeV', '4GeV'),l2_inputTEs=[""], l2_lower_chain_name='L1_TE50',n_muons=2),
    L2EFChain_mu_EFFSonly_HI('2mu2_MSonly_EFFS_L1ZDC_OR', 852,EFCB_FS_Hypo=TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '2GeV', '2GeV'),l2_inputTEs=[""], l2_lower_chain_name='L1_ZDC',n_muons=2),
    L2EFChain_mu_EFFSonly_HI('2mu2_MSonly_EFFS_L1MBTS', 853,EFCB_FS_Hypo=TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '2GeV', '2GeV'),l2_inputTEs=[""], l2_lower_chain_name='L1_MBTS_2',n_muons=2),
    L2EFChain_mu_EFFSonly_HI('2mu2_MSonly_EFFS_L1ZDC', 834,EFCB_FS_Hypo=TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '2GeV', '2GeV'),l2_inputTEs=[""], l2_lower_chain_name='L1_ZDC_A_C',n_muons=2),
    L2EFChain_mu_EFFSonly_HI('2mu2_MSonly_EFFS_L1TE10', 751,EFCB_FS_Hypo=TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '2GeV', '2GeV'),l2_inputTEs=[""], l2_lower_chain_name='L1_TE10',n_muons=2),
    L2EFChain_mu_EFFSonly_HI('2mu2_MSonly_EFFS_L1TE20', 752,EFCB_FS_Hypo=TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '2GeV', '2GeV'),l2_inputTEs=[""], l2_lower_chain_name='L1_TE20',n_muons=2),
    L2EFChain_mu_EFFSonly_HI('2mu2_MSonly_EFFS_L1TE50', 753,EFCB_FS_Hypo=TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '2GeV', '2GeV'),l2_inputTEs=[""], l2_lower_chain_name='L1_TE50',n_muons=2),
    L2EFChain_mu_EFFSonly_HI('mu4_mu2_MSonly_EFFS_L1ZDC', 819,EFCB_FS_Hypo=TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '2GeV', '4GeV'),l2_inputTEs=[""], l2_lower_chain_name='L1_ZDC_A_C',n_muons=2),
    L2EFChain_mu_EFFSonly_HI('mu4_mu2_MSonly_EFFS_L1TE10', 754,EFCB_FS_Hypo=TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '2GeV', '4GeV'),l2_inputTEs=[""], l2_lower_chain_name='L1_TE10',n_muons=2),
    L2EFChain_mu_EFFSonly_HI('mu4_mu2_MSonly_EFFS_L1TE20', 755,EFCB_FS_Hypo=TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '2GeV', '4GeV'),l2_inputTEs=[""], l2_lower_chain_name='L1_TE20',n_muons=2),
    L2EFChain_mu_EFFSonly_HI('mu4_mu2_MSonly_EFFS_L1TE50', 756,EFCB_FS_Hypo=TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '2GeV', '4GeV'),l2_inputTEs=[""], l2_lower_chain_name='L1_TE50',n_muons=2),
    
    L2EFChain_mu_EFFSonly_HI('2mu4_MSonly_EFFS_L1MU0', 839,EFCB_FS_Hypo=TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '4GeV', '4GeV'),l2_inputTEs=[""], l2_lower_chain_name='L1_MU0',n_muons=2),
    L2EFChain_mu_EFFSonly_HI('2mu2_MSonly_EFFS_L1MU0', 622,EFCB_FS_Hypo=TrigMuonEFExtrapolatorMultiHypoConfig('900GeV', '2GeV', '2GeV'),l2_inputTEs=[""], l2_lower_chain_name='L1_MU0',n_muons=2),
    
    ]

###########################################################################
#multi muons
from TriggerMenuPython.MultiMuon import *

MultiMuons = [
    
    MultiMuon( '2mu4_MSonly',          'L1_2MU0',  ['L2_mu4_MSonly_wOvlpRm']*2,   '62',  ['EF_mu4_MSonly_wOvlpRm']*2,  '62'),

    MultiMuon( '2mu6_MSonly',          'L1_2MU0',  ['L2_mu6_MSonly_wOvlpRm']*2,   '730',  ['EF_mu6_MSonly_wOvlpRm']*2,  '730'),

    MultiMuon( '2mu4T_MSonly',         'L1_2MU4',  ['L2_mu4T_MSonly_wOvlpRm']*2,   '949',  ['EF_mu4T_MSonly_wOvlpRm']*2,  '949'),
    MultiMuon( '2mu6T_MSonly',         'L1_2MU4',  ['L2_mu6T_MSonly_wOvlpRm']*2,   '731',  ['EF_mu6T_MSonly_wOvlpRm']*2,  '731'),
    MultiMuon( '2mu4',                 'L1_2MU0',  ['L2_mu4_wOvlpRm']*2,   '621',  ['EF_mu4_wOvlpRm']*2,  '621'),
    
    ]
#commissioning chains for 2010
###########################################################################

if __name__ == '__main__':
    triggerPythonConfig = TriggerPythonConfig('hlt.xml', None)
    for m in Muons:
        m.generateMenu(triggerPythonConfig)
    triggerPythonConfig.writeConfigFiles()
