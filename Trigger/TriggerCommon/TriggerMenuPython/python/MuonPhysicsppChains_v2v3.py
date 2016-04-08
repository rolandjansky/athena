# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################
# SliceDef file for Muon chains
###########################################################################
__author__  = 'R.Mackeprang'
__version__=""
__doc__="Definition of muon chains as used in Physics_pp_v2/v3/HI menu"

from TriggerMenuPython.MuonDef_v2v3 import *

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv1Menus() or \
       TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus():
    mu6Seed='L1_MU0'
    mu6Thr='MU0'
    mu10Seed='L1_MU0'
    mu10Thr='MU0'
    mu13Seed='L1_MU0'
    mu13Thr='MU0'
else:
    mu6Seed='L1_MU6'
    mu6Thr='MU6'
    mu10Seed='L1_MU10'
    mu10Thr='MU10'
    mu13Seed='L1_MU10'
    mu13Thr='MU10'

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
    mu10looseSeed='L1_MU0'
    mu10looseThr='MU0'
    mu4emptySeed='L1_MU0_EMPTY'
    mu4emptyThr='MU0'
else:
    mu10looseSeed='L1_MU4'
    mu10looseThr='MU4'
    mu4emptySeed='L1_MU4_EMPTY'
    mu4emptyThr='MU4'
    

# muFast threshold for MSonly chains
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

    

#########################
Config   = L2EFChain_mu_Seeded_EFFS.Config
L2Config = L2EFChain_mu_Seeded_EFFS.L2Config
EFConfig = L2EFChain_mu_Seeded_EFFS.EFConfig

Muons = [
   L2EFChain_mu_Seeded_EFFS('mu15_mu10_EFFS', 
                            'L2_mu15', 131, 'L1_MU10',
                            'EF_mu15_mu10_EFFS', 943,
                            ['MU10'],
                            Config(L2Config('_mu15',
                                      MufastHypoConfig('Muon', mufastThr_15GeV),
                                      MucombHypoConfig('Muon', '15GeV')),
                                   EFConfig('_mu15_mu10_EFFS',
                                      TrigMuonEFCombinerHypoConfig('Muon', '15GeV'),
                                      TrigMuonEFCombinerMultiHypoConfig('Muon', '15GeV','10GeV')))),

      L2EFChain_mu_Seeded_EFFS('mu15_mu10_EFFS_medium',
                            'L2_mu15_medium', 136, 'L1_MU11',
                            'EF_mu15_mu10_EFFS_medium', 845,
                            ['MU11'],
                            Config(L2Config('_mu15_medium',
                                            MufastHypoConfig('Muon', mufastThr_15GeV),
                                            MucombHypoConfig('Muon', '15GeV')),
                                   EFConfig('_mu15_mu10_EFFS_medium',
                                            TrigMuonEFCombinerHypoConfig('Muon', '15GeV'),
                                            TrigMuonEFCombinerMultiHypoConfig('Muon', '15GeV','10GeV')))),

      L2EFChain_mu_Seeded_EFFS('mu15_mu10_EFFS_tight',
                            'L2_mu15_tight', 545, 'L1_MU15',
                            'EF_mu15_mu10_EFFS_tight', 545,
                            ['MU15'],
                            Config(L2Config('_mu15_tight',
                                            MufastHypoConfig('Muon', mufastThr_15GeV),
                                            MucombHypoConfig('Muon', '15GeV')),
                                   EFConfig('_mu15_mu10_EFFS_tight',
                                            TrigMuonEFCombinerHypoConfig('Muon', '15GeV'),
                                            TrigMuonEFCombinerMultiHypoConfig('Muon', '15GeV','10GeV')))),

   L2EFChain_mu_Seeded_EFFS('mu15_2mu4_EFFS_medium', 
                            'L2_mu15_medium', 957, 'L1_MU11',
                            'EF_mu15_2mu4_EFFS_medium', 942,
                            ['MU11'],
                            Config(L2Config('_mu15_medium',
                                      MufastHypoConfig('Muon', mufastThr_15GeV),
                                      MucombHypoConfig('Muon', '15GeV')),
                                   EFConfig('_mu15_2mu4_EFFS_medium',
                                            TrigMuonEFCombinerHypoConfig('Muon', '15GeV'),
                                            TrigMuonEFCombinerMultiHypoConfig('Muon', '15GeV','4GeV','4GeV')))),
   

      L2EFChain_mu_Seeded_EFFS('mu20_mu10_EFFS_tight',
                            'L2_mu20_tight', 532, 'L1_MU15',
                            'EF_mu20_mu10_EFFS_tight', 532,
                            ['MU15'],
                            Config(L2Config('_mu20_tight',
                                            MufastHypoConfig('Muon', mufastThr_20GeV),
                                            MucombHypoConfig('Muon', '20GeV')),
                                   EFConfig('_mu20_mu10_EFFS_tight',
                                            TrigMuonEFCombinerHypoConfig('Muon', '20GeV'),
                                            TrigMuonEFCombinerMultiHypoConfig('Muon', '20GeV','10GeV')))),

   L2EFChain_mu_Seeded_EFFS('mu20_mu7_EFFS', 
                            'L2_mu20', 7176, 'L1_MU10',
                            'EF_mu20_mu7_EFFS', 7176,
                            ['MU10'],
                            Config(L2Config('_mu20',
                                      MufastHypoConfig('Muon', mufastThr_20GeV),
                                      MucombHypoConfig('Muon', '20GeV')),
                                   EFConfig('_mu20_mu7_EFFS',
                                      TrigMuonEFCombinerHypoConfig('Muon', '20GeV'),
                                      TrigMuonEFCombinerMultiHypoConfig('Muon', '20GeV','7GeV')))),

   L2EFChain_mu_Seeded_EFFS('mu15_mu7_EFFS', 
                            'L2_mu15', 131, 'L1_MU10',
                            'EF_mu15_mu7_EFFS', 7476,
                            ['MU10'],
                            Config(L2Config('_mu15',
                                      MufastHypoConfig('Muon', mufastThr_15GeV),
                                      MucombHypoConfig('Muon', '15GeV')),
                                   EFConfig('_mu15_mu7_EFFS',
                                      TrigMuonEFCombinerHypoConfig('Muon', '15GeV'),
                                      TrigMuonEFCombinerMultiHypoConfig('Muon', '15GeV','7GeV')))),
   #E.P.
   L2EFChain_mu_Seeded_EFFS('mu24_mu6_EFFS_medium',
                            'L2_mu24_medium', 150, 'L1_MU11',
                            'EF_mu24_mu6_EFFS_medium', 846,
                            ['MU11'],
                            Config(L2Config('_mu24_medium',
                                            MufastHypoConfig('Muon', mufastThr_24GeV),
                                            MucombHypoConfig('Muon', '24GeV')),
                                   EFConfig('_mu24_mu6_EFFS_medium',
                                            TrigMuonEFCombinerHypoConfig('Muon', '24GeV'),
                                            TrigMuonEFCombinerMultiHypoConfig('Muon', '24GeV','6GeV')))),
   ]


#########################
Config   = L2EFChain_mu_EFFSonly.Config
L2Config = L2EFChain_mu_EFFSonly.L2Config
EFConfig = L2EFChain_mu_EFFSonly.EFConfig

Muons += [
       L2EFChain_mu_EFFSonly('mu10_EFFS',
                             'L2_mu10_EFFS', 8131, '',
                             'EF_mu10_EFFS', 8131,
                             [''],
                             Config(L2Config('_mu10_EFFS'),
                                    EFConfig('_mu10_EFFS',
                                             TrigMuonEFCombinerMultiHypoConfig('Muon', '10GeV')))),
       ]


#########################
#out of time
Config   = L2EFChain_out_of_time.Config
L2Config = L2EFChain_out_of_time.L2Config
EFConfig = L2EFChain_out_of_time.EFConfig

#Config   = L2EFChain_mu_cal.Config
#L2Config = L2EFChain_mu_cal.L2Config
#EFConfig = L2EFChain_mu_cal.EFConfig

Muons += [
    
    # out of time muons, no EF
    L2EFChain_out_of_time('mu0_outOfTime1', 
                          'L2_mu0_outOfTime1', 7000, 'L1_MU0', 'EF_mu0_outOfTime1', 7000,
                          [],
                          Config(L2Config('_mu0_outOfTime1', MufastOTRHypoConfig('Seg', '1')), 
                                 EFConfig('_mu0_outOfTime1'))),
    
    L2EFChain_out_of_time('mu0_outOfTime2', 
                          'L2_mu0_outOfTime2', 7001, 'L1_MU0', 'EF_mu0_outOfTime2', 7001,
                          [],
                          Config(L2Config('_mu0_outOfTime2', MufastOTRHypoConfig('Seg', '2')), 
                                 EFConfig('_mu0_outOfTime2'))),

    L2EFChain_mu_cal('mu6_l2cal',
                     'L2_mu6_cal', 1100, 'L1_MU6',
                     '', -1 ,
                     ['MU6'],
                     Config(L2Config('_mu6_cal', MufastCALHypoConfig('Mcal','Rejection')),None)),

    L2EFChain_mu_cal('mu10_l2cal',
                     'L2_mu10_cal', 1101, 'L1_MU10',
                     '', -1 ,
                     ['MU10'],
                     Config(L2Config('_mu10_cal', MufastCALHypoConfig('Mcal','Rejection')),None)),

    L2EFChain_mu_cal('mu10_l2cal_medium',
                     'L2_mu10_cal_medium', 395, 'L1_MU11',
                     '', -1 ,
                     ['MU11'],
                     Config(L2Config('_mu10_cal_medium', MufastCALHypoConfig('Mcal','Rejection')),None)),

    L2EFChain_mu_cal('mu10_l2cal_tight',
                     'L2_mu10_cal_tight', 396, 'L1_MU15',
                     '', -1 ,
                     ['MU15'],
                     Config(L2Config('_mu10_cal_tight', MufastCALHypoConfig('Mcal','Rejection')),None)),
    
    L2EFChain_mu_cal('mu0_l2cal_empty', 
                     'L2_mu0_cal_empty', 1103, mu4emptySeed, 
                     '', -1 , 
                     [mu4emptyThr], 
                     Config(L2Config('_mu0_cal_empty', MufastCALHypoConfig('Mcal','Rejection')),None)),

    L2EFChain_mu_cal('mu4_l2cal_empty', 
                     'L2_mu4_cal_empty', 1103, mu4emptySeed, 
                     '', -1 , 
                     [mu4emptyThr], 
                     Config(L2Config('_mu4_cal_empty', MufastCALHypoConfig('Mcal','Rejection')),None)),     
    ]



###############
#MuGirl
Config   = L2EFChain_mugirl.Config
L2Config = L2EFChain_mugirl.L2Config
EFConfig = L2EFChain_mugirl.EFConfig

Muons += [
    # mu10_MG
    L2EFChain_mugirl('mu10_MG', 
                     'L2_mu10_MG', 601, mu10Seed, 'EF_mu10_MG', 601,
                     [mu10Thr],
                     Config(L2Config('_mu10_MG',
                                     MufastHypoConfig('Muon', mufastThr_10GeV),
                                     MucombHypoConfig('Muon', '10GeV')),
                            EFConfig('_mu10_MG',
                                     TrigMuGirlHypoConfig('Muon', '10GeV')))),
  
    L2EFChain_mugirl('mu13_MG', 
                     'L2_mu13_MG', 321, mu13Seed, 'EF_mu13_MG', 321,
                     [mu13Thr],
                     Config(L2Config('_mu13_MG',
                                     MufastHypoConfig('Muon', mufastThr_13GeV),
                                     #MufastHypoConfig('Muon', '4GeV_V2'),
                                     MucombHypoConfig('Muon', '13GeV')),
                            EFConfig('_mu13_MG',
                                     TrigMuGirlHypoConfig('Muon', '13GeV')))),


    L2EFChain_mugirl('mu15_MG', 
                     'L2_mu15_MG', 322, 'L1_MU10', 'EF_mu15_MG', 322,
                     ['MU10'],
                     Config(L2Config('_mu15_MG',
                                     MufastHypoConfig('Muon', mufastThr_15GeV),
                                     MucombHypoConfig('Muon', '15GeV')),
                            EFConfig('_mu15_MG',
                                     TrigMuGirlHypoConfig('Muon', '15GeV')))),

    L2EFChain_900_mugirl('mu4_MG', 
                         'L2_mu4_MG', 861, 'L1_MU0', 'EF_mu4_MG', 861,
                         ['MU0'],
                         Config(L2Config('_mu4_MG',
                                         MufastHypoConfig('900GeV', mufastThr_4GeV),
                                         MucombHypoConfig('900GeV', '4GeV')),
                                EFConfig('_mu4_MG',
                                         TrigMuGirlHypoConfig('900GeV', '4GeV')))),

    L2EFChain_mugirl('mu18_MG', 
                     'L2_mu18_MG', 975, 'L1_MU10', 'EF_mu18_MG', 975,
                     ['MU10'],
                     Config(L2Config('_mu18_MG',
                                     MufastHypoConfig('Muon', mufastThr_18GeV),
                                     MucombHypoConfig('Muon', '18GeV')),
                            EFConfig('_mu18_MG',
                                     TrigMuGirlHypoConfig('Muon','18GeV')))),

    L2EFChain_mugirl('mu18_MG_L1J10', 
                     'L2_mu18_MG_L1J10', 979, 'L1_MU10_J10', 'EF_mu18_MG_L1J10', 979,
                     ['MU10'],
                     Config(L2Config('_mu18_MG_L1J10',
                                     MufastHypoConfig('Muon', mufastThr_18GeV),
                                     MucombHypoConfig('Muon', '18GeV')),
                            EFConfig('_mu18_MG_L1J10',
                                     TrigMuGirlHypoConfig('Muon','18GeV')))),
    
    L2EFChain_mugirl('mu18_MG_medium',
                     'L2_mu18_MG_medium', 824, 'L1_MU11', 'EF_mu18_MG_medium', 824,
                     ['MU11'],
                     Config(L2Config('_mu18_medium',
                                     MufastHypoConfig('Muon', mufastThr_18GeV),
                                     MucombHypoConfig('Muon', '18GeV')),
                            EFConfig('_mu18_MG_medium',
                                     TrigMuGirlHypoConfig('Muon','18GeV')))),

    
    L2EFChain_mugirl('mu22_MG', 
                     'L2_mu22_MG', 950, 'L1_MU10', 'EF_mu22_MG', 950,
                     ['MU10'],
                     Config(L2Config('_mu22_MG',
                                     MufastHypoConfig('Muon', mufastThr_22GeV),
                                     MucombHypoConfig('Muon', '22GeV')),
                            EFConfig('_mu22_MG',
                                     TrigMuGirlHypoConfig('Muon','22GeV')))),


    L2EFChain_mugirl('mu22_MG_medium',
                     'L2_mu22_MG_medium', 954, 'L1_MU11', 'EF_mu22_MG_medium', 954,
                     ['MU11'],
                     Config(L2Config('_mu22_medium',
                                     MufastHypoConfig('Muon', mufastThr_22GeV),
                                     MucombHypoConfig('Muon', '22GeV')),
                            EFConfig('_mu22_MG_medium',
                                     TrigMuGirlHypoConfig('Muon','22GeV')))),
        
    
    L2EFChain_mugirl('mu20_MG', 
                     'L2_mu20_MG', 956, 'L1_MU10', 'EF_mu20_MG', 956,
                     ['MU10'],
                     Config(L2Config('_mu20_MG',
                                     MufastHypoConfig('Muon', mufastThr_20GeV),
                                     MucombHypoConfig('Muon', '20GeV')),
                            EFConfig('_mu20_MG',
                                     TrigMuGirlHypoConfig('Muon','20GeV')))),

    
    L2EFChain_mugirl('mu20_MG_medium',
                     'L2_mu20_MG_medium', 826, 'L1_MU11', 'EF_mu20_MG_medium', 826,
                     ['MU11'],
                     Config(L2Config('_mu20_medium',
                                     MufastHypoConfig('Muon', mufastThr_20GeV),
                                     MucombHypoConfig('Muon', '20GeV')),
                            EFConfig('_mu20_MG_medium',
                                     TrigMuGirlHypoConfig('Muon','20GeV')))),

    L2EFChain_mugirl('mu24_MG_medium',
                     'L2_mu24_MG_medium', 444, 'L1_MU11', 'EF_mu24_MG_medium', 444,
                     ['MU11'],
                     Config(L2Config('_mu24_medium',
                                     MufastHypoConfig('Muon', mufastThr_24GeV),
                                     MucombHypoConfig('Muon', '24GeV')),
                            EFConfig('_mu24_MG_medium',
                                     TrigMuGirlHypoConfig('Muon','24GeV')))),

    L2EFChain_mugirl('mu30_MG_medium',
                     'L2_mu30_MG_medium', 825, 'L1_MU11', 'EF_mu30_MG_medium', 825,
                     ['MU11'],
                     Config(L2Config('_mu30_medium',
                                     MufastHypoConfig('Muon', mufastThr_30GeV),
                                     MucombHypoConfig('Muon', '30GeV')),
                            EFConfig('_mu30_MG_medium',
                                     TrigMuGirlHypoConfig('Muon','30GeV')))),

    L2EFChain_mugirl('mu24_MG_tight',
                     'L2_mu24_MG_tight', 841, 'L1_MU15', 'EF_mu24_MG_tight', 841,
                     ['MU15'],
                     Config(L2Config('_mu24_tight',
                                     MufastHypoConfig('Muon', mufastThr_24GeV),
                                     MucombHypoConfig('Muon', '24GeV')),
                            EFConfig('_mu24_MG_tight',
                                     TrigMuGirlHypoConfig('Muon','24GeV')))),

    L2EFChain_mugirl('mu30_MG_tight',
                     'L2_mu30_MG_tight', 415, 'L1_MU15', 'EF_mu30_MG_tight', 415,
                     ['MU15'],
                     Config(L2Config('_mu30_tight',
                                     MufastHypoConfig('Muon', mufastThr_30GeV),
                                     MucombHypoConfig('Muon', '30GeV')),
                            EFConfig('_mu30_MG_tight',
                                     TrigMuGirlHypoConfig('Muon','30GeV')))),

    
    ]


#SLOW PARTICLE  --- sofia
Config   = L2EFChain_mugirlslow.Config
L2Config = L2EFChain_mugirlslow.L2Config
EFConfig = L2EFChain_mugirlslow.EFConfig

Muons += [
    # mu40_SLOW -> name change
    L2EFChain_mugirlslow('mu40_slow',
                     'L2_mu40_slow', 992, 'L1_MU10', 'EF_mu40_slow', 992,
                     ['MU10'],
                     Config(L2Config('_mu40_slow',
                                     MufastStauHypoConfig('Muon', '40GeV'),
                                     MucombStauHypoConfig('Muon', '40GeV')),
                            EFConfig('_mu40_slow',
                                     TrigMuGirlStauHypoConfig('Muon', '40GeV')))),

    
    # mu40_SLOW_medium
    L2EFChain_mugirlslow('mu40_slow_medium',
                         'L2_mu40_slow_medium', 860, 'L1_MU11', 'EF_mu40_slow_medium', 860,
                         ['MU11'],
                         Config(L2Config('_mu40_slow_medium',
                                         MufastStauHypoConfig('Muon', '40GeV'),
                                         MucombStauHypoConfig('Muon', '40GeV')),
                                EFConfig('_mu40_slow_medium',
                                         TrigMuGirlStauHypoConfig('Muon', '40GeV')))),
        

    ]

Muons += [
    
    # mu40_SLOW_FIRSTEMPTY
    L2EFChain_mugirlslow_empty('mu40_slow_empty',
                               'L2_mu40_slow_empty', 912, 'L1_MU10_EMPTY', 'EF_mu40_slow_empty', 912,
                               ['MU10'],
                               Config(L2Config('_mu40_slow_empty',
                                               MufastStauHypoConfig('Muon', '40GeV'),
                                               MucombStauHypoConfig('Muon', '40GeV')),
                                      EFConfig('_mu40_slow_empty',
                                               TrigMuGirlStauHypoConfig('Muon', '40GeV')))),
    ]

# mu20_SLOW OUT-OF-TIME
Config   = L2EFChain_mugirlslow_out.Config
L2Config = L2EFChain_mugirlslow_out.L2Config
EFConfig = L2EFChain_mugirlslow_out.EFConfig

Muons += [
    L2EFChain_mugirlslow_out('mu40_slow_outOfTime',
                     'L2_mu40_slow_outOfTime', 913, 'L1_MU10', 'EF_mu40_slow_outOfTime', 913,
                     ['MU10'],
                     Config(L2Config('_mu40_slow_outOfTime',
                                     MufastStauHypoConfig('Muon', '40GeV'),
                                     MucombStauHypoConfig('Muon', '40GeV')),
                            EFConfig('_mu40_slow_outOfTime',
                                     TrigMuGirlStauHypoConfig('Muon', '40GeV')))),

    L2EFChain_mugirlslow_out('mu40_slow_outOfTime_medium',
                     'L2_mu40_slow_outOfTime_medium', 870, 'L1_MU11', 'EF_mu40_slow_outOfTime_medium', 862,
                     ['MU11'],
                     Config(L2Config('_mu40_slow_outOfTime_medium',
                                     MufastStauHypoConfig('Muon', '40GeV'),
                                     MucombStauHypoConfig('Muon', '40GeV')),
                            EFConfig('_mu40_slow_outOfTime_medium',
                                     TrigMuGirlStauHypoConfig('Muon', '40GeV')))),
    ]


#----------------------
#Hidden Valley
#----------------------
Config   = L2EFChain_HV.Config
L2Config = L2EFChain_HV.L2Config
EFConfig = L2EFChain_HV.EFConfig

Muons += [
    L2EFChain_HV('2MUL1_l2j30_HV', 
                 'L2_2MUL1_l2j30_HV', 989, 'L1_2MU6', 'EF_2MUL1_l2j30_HV', 989,
                 ['MU6', 'L2_j30'],
                 Config(L2Config('_2MUL1_l2j30_HV'),
                        EFConfig('_2MUL1_l2j30_HV'))),

    L2EFChain_HV('2MUL1_l2j30_HV_cosmic', 
                 'L2_2MUL1_l2j30_HV_cosmic', 971, 'L1_2MU6_EMPTY', 'EF_2MUL1_l2j30_HV_cosmic', 971,
                 ['MU6', 'L2_j30'],
                 Config(L2Config('_2MUL1_l2j30_HV_cosmic'),
                        EFConfig('_2MUL1_l2j30_HV_cosmic'))),

    L2EFChain_HV('2MUL1_l2j30_HV_firstempty', 
                 'L2_2MUL1_l2j30_HV_firstempty', 972, 'L1_2MU6_FIRSTEMPTY', 'EF_2MUL1_l2j30_HV_firstempty', 972,
                 ['MU6', 'L2_j30'],
                 Config(L2Config('_2MUL1_l2j30_HV_firstempty'),
                        EFConfig('_2MUL1_l2j30_HV_firstempty'))),

    L2EFChain_HV('2MUL1_l2j30_HV_unpaired_iso', 
                 'L2_2MUL1_l2j30_HV_unpaired_iso', 973, 'L1_2MU6_UNPAIRED_ISO', 'EF_2MUL1_l2j30_HV_unpaired_iso', 973,
                 ['MU6', 'L2_j30'],
                 Config(L2Config('_2MUL1_l2j30_HV_unpaired_iso'),
                        EFConfig('_2MUL1_l2j30_HV_unpaired_iso'))),
    
    L2EFChain_HV('2MUL1_l2j30_HV_unpaired_noniso', 
                 'L2_2MUL1_l2j30_HV_unpaired_noniso', 10, 'L1_2MU6_UNPAIRED_NONISO', 'EF_2MUL1_l2j30_HV_unpaired_noniso', 10,
                 ['MU6', 'L2_j30'],
                 Config(L2Config('_2MUL1_l2j30_HV_unpaired_noniso'),
                        EFConfig('_2MUL1_l2j30_HV_unpaired_noniso'))),

    ]


Config   = L2EFChain_HV_AllMS.Config
L2Config = L2EFChain_HV_AllMS.L2Config
EFConfig = L2EFChain_HV_AllMS.EFConfig

Muons += [
    L2EFChain_HV_AllMS('2MUL1_l2j30_HV_AllMS', 
                       'L2_2MUL1_l2j30_HV_AllMS', 2989, 'L1_2MU6', 'EF_2MUL1_l2j30_HV_AllMS', 2989,
                       ['MU6', 'L2_j30'],
                       Config(L2Config('_2MUL1_l2j30_HV_AllMS'),
                              EFConfig('_2MUL1_l2j30_HV_AllMS'))),
    ]



Config   = L2EFChain_mu.Config
L2Config = L2EFChain_mu.L2Config
EFConfig = L2EFChain_mu.EFConfig

# -----------------------------------

IsoConfig   = L2EFChain_mu_iso_l2ef.Config
IsoL2Config = L2EFChain_mu_iso_l2ef.L2Config
IsoEFConfig = L2EFChain_mu_iso_l2ef.EFConfig

EFOnlyIsoConfig   = L2EFChain_mu_iso_efonly.Config
EFOnlyIsoL2Config = L2EFChain_mu_iso_efonly.L2Config
EFOnlyIsoEFConfig = L2EFChain_mu_iso_efonly.EFConfig

# -----------------------------------

Muons += [
    #mu6
    L2EFChain_mu_noiso('mu6', 
                       'L2_mu6', 129, mu6Seed, 'EF_mu6', 129,
                       [mu6Thr],
                       Config(L2Config('_mu6',
                                       MufastHypoConfig('Muon', mufastThr_6GeV),
                                       MucombHypoConfig('Muon', '6GeV')),
                              EFConfig('_mu6',
                                       TrigMuonEFCombinerHypoConfig('Muon', '6GeV'))),rerun=1),
#needed for 3mu6
    L2EFChain_mu_noiso_wOvlpRm('mu6_wOvlpRm', 
                       'L2_mu6_wOvlpRm', 921, mu6Seed, 'EF_mu6_wOvlpRm', 921,
                       [mu6Thr],
                       Config(L2Config('_mu6_wOvlpRm',
                                       MufastHypoConfig('Muon', mufastThr_6GeV),
                                       MucombHypoConfig('Muon', '6GeV')),
                              EFConfig('_mu6_wOvlpRm',
                                       TrigMuonEFCombinerHypoConfig('Muon', '6GeV')))),


    L2EFChain_mu_noiso('mu6_NL', 
                       'L2_mu6_NL', 993, 'L1_MU6_NL', 'EF_mu6_NL', 993,
                       ['MU6'],
                       Config(L2Config('_mu6_NL',
                                       MufastHypoConfig('Muon', mufastThr_6GeV),
                                       MucombHypoConfig('Muon', '6GeV')),
                              EFConfig('_mu6_NL',
                                       TrigMuonEFCombinerHypoConfig('Muon', '6GeV')))),


    #mu18
    L2EFChain_mu_noiso('mu18', 
                       'L2_mu18', 937, 'L1_MU10', 'EF_mu18', 937,
                       ['MU10'],
                       Config(L2Config('_mu18',
                                       MufastHypoConfig('Muon', mufastThr_18GeV),
                                       MucombHypoConfig('Muon', '18GeV')), 
                              EFConfig('_mu18',
                                       TrigMuonEFCombinerHypoConfig('Muon','18GeV')))),

    L2EFChain_mu_noiso('mu18_L1J10', 
                       'L2_mu18_L1J10', 936, 'L1_MU10_J10', 'EF_mu18_L1J10', 936,
                       ['MU10'],
                       Config(L2Config('_mu18_L1J10',
                                       MufastHypoConfig('Muon', mufastThr_18GeV),
                                       MucombHypoConfig('Muon', '18GeV')), 
                              EFConfig('_mu18_L1J10',
                                       TrigMuonEFCombinerHypoConfig('Muon','18GeV')))),
    
    #mu18_medium
    L2EFChain_mu_noiso('mu18_medium',
                       'L2_mu18_medium', 941, 'L1_MU11', 'EF_mu18_medium', 941,
                       ['MU11'],
                       Config(L2Config('_mu18_medium',
                                       MufastHypoConfig('Muon', mufastThr_18GeV),
                                       MucombHypoConfig('Muon', '18GeV')),
                              EFConfig('_mu18_medium',
                                       TrigMuonEFCombinerHypoConfig('Muon','18GeV')))),

        
    #mu22
    L2EFChain_mu_noiso('mu22', 
                       'L2_mu22', 960, 'L1_MU10', 'EF_mu22', 960,
                       ['MU10'],
                       Config(L2Config('_mu22',
                                       MufastHypoConfig('Muon', mufastThr_22GeV),
                                       MucombHypoConfig('Muon', '22GeV')), 
                              EFConfig('_mu22',
                                       TrigMuonEFCombinerHypoConfig('Muon','22GeV')))),


    #mu22_medium
    L2EFChain_mu_noiso('mu22_medium',
                       'L2_mu22_medium', 827, 'L1_MU11', 'EF_mu22_medium', 827,
                       ['MU11'],
                       Config(L2Config('_mu22_medium',
                                       MufastHypoConfig('Muon', mufastThr_22GeV),
                                       MucombHypoConfig('Muon', '22GeV')),
                              EFConfig('_mu22_medium',
                                       TrigMuonEFCombinerHypoConfig('Muon','22GeV')))),
        

    L2EFChain_L2StarA_wOvlpRm('mu10_L2StarA_wOvlpRm',
                              'L2_mu10_L2StarA_wOvlpRm', 6227, 'L1_MU10',
                              'EF_mu10_L2StarA_wOvlpRm', 6227,
                              ['MU10'],
                              Config(L2Config('_mu10_L2StarA_wOvlpRm',
                                              MufastHypoConfig('Muon', mufastThr_10GeV),
                                              MucombHypoConfig('Muon', '10GeV')),
                                     EFConfig('_mu10_L2StarA_wOvlpRm',
                                              TrigMuonEFCombinerHypoConfig('Muon', '10GeV')))),
    
    L2EFChain_L2StarB_wOvlpRm('mu10_L2StarB_wOvlpRm',
                              'L2_mu10_L2StarB_wOvlpRm', 6227, 'L1_MU10',
                              'EF_mu10_L2StarB_wOvlpRm', 6227,
                              ['MU10'],
                              Config(L2Config('_mu10_L2StarB_wOvlpRm',
                                              MufastHypoConfig('Muon', mufastThr_10GeV),
                                              MucombHypoConfig('Muon', '10GeV')),
                                     EFConfig('_mu10_L2StarB_wOvlpRm',
                                              TrigMuonEFCombinerHypoConfig('Muon', '10GeV')))),
    

    L2EFChain_L2StarC_wOvlpRm('mu10_L2StarC_wOvlpRm',
                              'L2_mu10_L2StarC_wOvlpRm', 6227, 'L1_MU10',
                              'EF_mu10_L2StarC_wOvlpRm', 6227,
                              ['MU10'],
                              Config(L2Config('_mu10_L2StarC_wOvlpRm',
                                              MufastHypoConfig('Muon', mufastThr_10GeV),
                                              MucombHypoConfig('Muon', '10GeV')),
                                     EFConfig('_mu10_L2StarC_wOvlpRm',
                                              TrigMuonEFCombinerHypoConfig('Muon', '10GeV')))),
    
    L2EFChain_mu_noiso_wOvlpRm_900('mu4wOvlpRm', 
                                   'L2_mu4_wOvlpRm', 6127, 'L1_MU0',
                                   'EF_mu4_wOvlpRm', 6127,
                                   ['MU0'],
                                   Config(L2Config('_mu4_wOvlpRm',
                                                   MufastHypoConfig('900GeV', mufastThr_4GeV),
                                                   MucombHypoConfig('900GeV', '4GeV')),
                                          EFConfig('_mu4_wOvlpRm',
                                                   TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),

    L2EFChain_mu_noiso_wOvlpRm_900('mu4T_wOvlpRm', 
                                   'L2_mu4T_wOvlpRm', 6127, 'L1_MU4',
                                   'EF_mu4T_wOvlpRm', 6127,
                                  ['MU4'],
                                   Config(L2Config('_mu4T_wOvlpRm',
                                                   MufastHypoConfig('900GeV', mufastThr_4GeV),
                                                   MucombHypoConfig('900GeV', '4GeV')),
                                          EFConfig('_mu4T_wOvlpRm',
                                                   TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),    
   
    L2EFChain_mu_noiso_wOvlpRm('mu10_wOvlpRm', 
                               'L2_mu10_wOvlpRm', 936, 'L1_MU10',
                               'EF_mu10_wOvlpRm', 936,
                               ['MU10'],
                               Config(L2Config('_mu10_wOvlpRm',
                                               MufastHypoConfig('Muon', mufastThr_10GeV),
                                               MucombHypoConfig('Muon', '10GeV')),
                                      EFConfig('_mu10_wOvlpRm',
                                               TrigMuonEFCombinerHypoConfig('Muon', '10GeV')))),
    
    L2EFChain_mu_noiso_wOvlpRm('mu10_empty_wOvlpRm', 
                               'L2_mu10_empty_wOvlpRm', 935, 'L1_MU10_EMPTY',
                               'EF_mu10_empty_wOvlpRm', 935,
                               ['MU10'],
                               Config(L2Config('_mu10_empty_wOvlpRm',
                                               MufastHypoConfig('Muon', mufastThr_10GeV),
                                               MucombHypoConfig('Muon', '10GeV')),
                                      EFConfig('_mu10_empty_wOvlpRm',
                                               TrigMuonEFCombinerHypoConfig('Muon', '10GeV')))),

    L2EFChain_mu_noiso_wOvlpRm('mu10_loose_empty_wOvlpRm', 
                               'L2_mu10_loose_empty_wOvlpRm', 934, mu4emptySeed, 
                               'EF_mu10_loose_empty_wOvlpRm', 934,
                               [mu4emptyThr],  
                               Config(L2Config('_mu10_loose_empty_wOvlpRm',
                                               MufastHypoConfig('Muon', mufastThr_10GeV),
                                               MucombHypoConfig('Muon', '10GeV')),
                                      EFConfig('_mu10_loose_empty_wOvlpRm',
                                               TrigMuonEFCombinerHypoConfig('Muon', '10GeV')))),

    L2EFChain_mu_noiso_wOvlpRm('mu10_loose_wOvlpRm', 
                               'L2_mu10_loose_wOvlpRm', 6128, mu10looseSeed,
                               'EF_mu10_loose_wOvlpRm', 6128,
                               [mu10looseThr],
                               Config(L2Config('_mu10_loose_wOvlpRm',
                                               MufastHypoConfig('Muon', mufastThr_10GeV),
                                               MucombHypoConfig('Muon', '10GeV')),
                                      EFConfig('_mu10_loose_wOvlpRm',
                                               TrigMuonEFCombinerHypoConfig('Muon', '10GeV')))),

    L2EFChain_mu_noiso_wOvlpRm('mu10_loose1_wOvlpRm', 
                               'L2_mu10_loose1_wOvlpRm', 7128, 'L1_MU6',
                               'EF_mu10_loose1_wOvlpRm', 7128,
                               ['MU6'],
                               Config(L2Config('_mu10_loose1_wOvlpRm',
                                               MufastHypoConfig('Muon', mufastThr_10GeV),
                                               MucombHypoConfig('Muon', '10GeV')),
                                      EFConfig('_mu10_loose1_wOvlpRm',
                                               TrigMuonEFCombinerHypoConfig('Muon', '10GeV')))),
    
    L2EFChain_mu_900_MSonly_wOvlpRm('mu4_MSonly_wOvlpRm', 
                                    'L2_mu4_MSonly_wOvlpRm', 6809, 'L1_MU0', 
                                    'EF_mu4_MSonly_wOvlpRm', 6809,
                                    ['MU0'],
                                    Config(L2Config('_mu4_MSonly_wOvlpRm', 
                                                    MufastHypoConfig('900GeV', mufastThr_4GeV), 
                                                    None), 
                                           EFConfig('_mu4_MSonly_wOvlpRm', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV')))),
    
    L2EFChain_mu_MSonly_wOvlpRm('mu6_MSonly_wOvlpRm', 
                                'L2_mu6_MSonly_wOvlpRm', 6129, 'L1_MU6', 'EF_mu6_MSonly_wOvlpRm', 6129,
                                ['MU6'],
                                Config(L2Config('_mu6_MSonly_wOvlpRm', 
                                                MufastHypoConfig('Muon', mufastThr_6GeV), 
                                                None),
                                       EFConfig('_mu6_MSonly_wOvlpRm', TrigMuonEFExtrapolatorHypoConfig('Muon','6GeV')))),
    
    L2EFChain_mu_MSonly_wOvlpRm('mu10_MSonly_wOvlpRm', 
                                'L2_mu10_MSonly_wOvlpRm', 6171, 'L1_MU10', 'EF_mu10_MSonly_wOvlpRm', 6171,
                                ['MU10'],
                                Config(L2Config('_mu10_MSonly_wOvlpRm', 
                                                MufastHypoConfig('Muon', mufastThr_10GeV), 
                                                None),
                                       EFConfig('_mu10_MSonly_wOvlpRm', TrigMuonEFExtrapolatorHypoConfig('Muon','10GeV')))),

    L2EFChain_mu_MSonly_wOvlpRm('mu10L_MSonly_wOvlpRm', 
                                'L2_mu10L_MSonly_wOvlpRm', 6172, 'L1_MU6', 'EF_mu10L_MSonly_wOvlpRm', 6172,
                                ['MU6'],
                                Config(L2Config('_mu10L_MSonly_wOvlpRm', 
                                                MufastHypoConfig('Muon', mufastThr_10GeV), 
                                                None),
                                       EFConfig('_mu10L_MSonly_wOvlpRm', TrigMuonEFExtrapolatorHypoConfig('Muon','10GeV')))),    
    

    ]

Muons += [
    #PJB
    #mu4 is different - uses 900GeV config
        L2EFChain_mu_900('mu4', 
                         'L2_mu4', 127, 'L1_MU0', 'EF_mu4', 127,
                         ['MU0'],
                         Config(L2Config('_mu4',
                                         MufastHypoConfig('900GeV', mufastThr_4GeV),
                                         MucombHypoConfig('900GeV', '4GeV')),
                                EFConfig('_mu4',
                                         TrigMuonEFCombinerHypoConfig('900GeV', '4GeV'))),rerun=1),
        
        L2EFChain_mu_900('mu4T', 
                         'L2_mu4T', 405, 'L1_MU4', 'EF_mu4T', 405,
                         ['MU4'],
                         Config(L2Config('_mu4T',
                                         MufastHypoConfig('900GeV', mufastThr_4GeV),
                                         MucombHypoConfig('900GeV', '4GeV')),
                                EFConfig('_mu4T',
                                         TrigMuonEFCombinerHypoConfig('900GeV', '4GeV'))),rerun=1),
        
    L2EFChain_mu_900('mu4_cosmic', 
                     'L2_mu4_cosmic', 743, mu4emptySeed, 'EF_mu4_cosmic', 743,
                     [mu4emptyThr],
                     Config(L2Config('_mu4_cosmic',
                                     MufastHypoConfig('900GeV', mufastThr_4GeV),
                                     MucombHypoConfig('900GeV', '4GeV')),
                            EFConfig('_mu4_cosmic',
                                     TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),

    L2EFChain_mu_900('mu4_firstempty', 
                     'L2_mu4_firstempty', 2106, 'L1_MU0_FIRSTEMPTY', 'EF_mu4_firstempty', 2106,
                     ['MU0'],
                     Config(L2Config('_mu4_firstempty',
                                     MufastHypoConfig('900GeV', mufastThr_4GeV),
                                     MucombHypoConfig('900GeV', '4GeV')),
                            EFConfig('_mu4_firstempty',
                                     TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),


    L2EFChain_mu_900('mu4_unpaired', 
                     'L2_mu4_unpaired', 2113, 'L1_MU0_UNPAIRED', 'EF_mu4_unpaired', 2113,
                     ['MU0'],
                     Config(L2Config('_mu4_unpaired',
                                     MufastHypoConfig('900GeV', mufastThr_4GeV),
                                     MucombHypoConfig('900GeV', '4GeV')),
                            EFConfig('_mu4_unpaired',
                                     TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),

    L2EFChain_mu_900('mu4_unpaired_iso', 
                     'L2_mu4_unpaired_iso', 2113, 'L1_MU0_UNPAIRED_ISO', 'EF_mu4_unpaired_iso', 2113,
                     ['MU0'],
                     Config(L2Config('_mu4_unpaired_iso',
                                     MufastHypoConfig('900GeV', mufastThr_4GeV),
                                     MucombHypoConfig('900GeV', '4GeV')),
                            EFConfig('_mu4_unpaired_iso',
                                     TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),

 
    L2EFChain_mu_900('mu4_L1MU11_cosmic', 
                     'L2_mu4_L1MU11_cosmic', 2102, 'L1_MU11_EMPTY', 'EF_mu4_L1MU11_cosmic', 2102,
                     ['MU11'],
                     Config(L2Config('_mu4_L1MU11_cosmic',
                                     MufastHypoConfig('900GeV', mufastThr_4GeV),
                                     MucombHypoConfig('900GeV', '4GeV')),
                            EFConfig('_mu4_L1MU11_cosmic',
                                     TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),


    L2EFChain_mu_900_MSonly('mu4_MSonly_cosmic', 
                        'L2_mu4_MSonly_cosmic', 745, 'L1_MU0_EMPTY', 
                        'EF_mu4_MSonly_cosmic', 745,
                        ['MU0'],
                        Config(L2Config('_mu4_MSonly_cosmic', 
                                        MufastHypoConfig('900GeV', mufastThr_4GeV), 
                                        None), 
                               EFConfig('_mu4_MSonly_cosmic', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV')))),


   L2EFChain_mu_900_MSonly('mu4_L1MU11_MSonly_cosmic', 
                            'L2_mu4_L1MU11_MSonly_cosmic', 2103, 'L1_MU11_EMPTY', 
                            'EF_mu4_L1MU11_MSonly_cosmic', 2103,
                            ['MU11'],
                            Config(L2Config('_mu4_L1MU11_MSonly_cosmic', 
                                            MufastHypoConfig('900GeV', mufastThr_4GeV), 
                                            None), 
                                   EFConfig('_mu4_L1MU11_MSonly_cosmic', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV')))),

    L2EFChain_mu_900_MSonly('mu4_MSonly', 
                        'L2_mu4_MSonly', 809, 'L1_MU0', 
                        'EF_mu4_MSonly', 809,
                        ['MU0'],
                        Config(L2Config('_mu4_MSonly', 
                                        MufastHypoConfig('900GeV', mufastThr_4GeV), 
                                        None), 
                               EFConfig('_mu4_MSonly', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV')))),


     
        ]

#mu10
Muons += [
        L2EFChain_mu_noiso('mu10', 
                           'L2_mu10', 130, mu10Seed, 'EF_mu10', 130,
                           [mu10Thr],
                           Config(L2Config('_mu10',
                                           MufastHypoConfig('Muon', mufastThr_10GeV),
                                           MucombHypoConfig('Muon', '10GeV')),
                                  EFConfig('_mu10',
                                           TrigMuonEFCombinerHypoConfig('Muon', '10GeV'))),rerun=1),
        

        L2EFChain_mu_noiso('mu10_loose', 
                           'L2_mu10_loose', 962, mu10looseSeed, 'EF_mu10_loose', 962,
                           [mu10looseThr],
                           Config(L2Config('_mu10_loose',
                                           MufastHypoConfig('Muon', mufastThr_10GeV),
                                           MucombHypoConfig('Muon', '10GeV')),
                                  EFConfig('_mu10_loose',
                                           TrigMuonEFCombinerHypoConfig('Muon', '10GeV'))), rerun=1),

        
        L2EFChain_mu_noiso('mu10_NL', 
                           'L2_mu10_NL', 996, 'L1_MU6_NL', 'EF_mu10_NL', 996,
                           ['MU6'],
                           Config(L2Config('_mu10_NL',
                                           MufastHypoConfig('Muon', mufastThr_10GeV),
                                           MucombHypoConfig('Muon', '10GeV')),
                                  EFConfig('_mu10_NL',
                                           TrigMuonEFCombinerHypoConfig('Muon', '10GeV')))),
        ]


Muons += [

    
    L2EFChain_mu_NoIDTrkCut('mu13_IDTrkNoCut', 
                            'L2_mu13_IDTrkNoCut', 1200, mu13Seed, 'EF_mu13_IDTrkNoCut', 1200,
                            [mu13Thr],
                            Config(L2Config('_mu13_IDTrkNoCut',
                                            MufastHypoConfig('Muon', mufastThr_MSonly_13GeV),
                                            MucombHypoConfig('Muon', 'passthrough')),
                                   EFConfig('_mu13_IDTrkNoCut',
                                            TrigMuonEFExtrapolatorHypoConfig('Muon','13GeV')))),
    #81309 with many vtx cut
    L2EFChain_mu_NoIDTrkCut('mu20_IDTrkNoCut_ManyVx', 
                            'L2_mu20_IDTrkNoCut', 839, 'L1_MU11', 'EF_mu20_IDTrkNoCut_ManyVx', 749,
                            ['MU11'],
                            Config(L2Config('_mu20_IDTrkNoCut',
                                            MufastHypoConfig('Muon', mufastThr_MSonly_20GeV),
                                            MucombHypoConfig('Muon', 'passthrough')),
                                   EFConfig('_mu20_IDTrkNoCut_ManyVx',
                                            EFMbManyVxHypo))),


    L2EFChain_mu_NoIDTrkCut('mu20_IDTrkNoCut', 
                            'L2_mu20_IDTrkNoCut', 1014, 'L1_MU11', 'EF_mu20_IDTrkNoCut', 1014,
                            ['MU11'],
                            Config(L2Config('_mu20_IDTrkNoCut',
                                            MufastHypoConfig('Muon', mufastThr_MSonly_20GeV),
                                            MucombHypoConfig('Muon', 'passthrough')),
                                   EFConfig('_mu20_IDTrkNoCut',
                                            TrigMuonEFExtrapolatorHypoConfig('Muon','20GeV')))),


    L2EFChain_mu_900_NoIDTrkCut('mu4_IDTrkNoCut', 
                            'L2_mu4_IDTrkNoCut', 843, 'L1_MU0', 'EF_mu4_IDTrkNoCut', 843,
                            ['MU0'],
                            Config(L2Config('_mu4_IDTrkNoCut',
                                            MufastHypoConfig('900GeV', mufastThr_4GeV),
                                            MucombHypoConfig('900GeV', 'passthrough')),
                                   EFConfig('_mu4_IDTrkNoCut',
                                            TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV')))),

    L2EFChain_mu_900_NoIDTrkCut('mu4T_IDTrkNoCut', 
                            'L2_mu4T_IDTrkNoCut', 406, 'L1_MU4', 'EF_mu4T_IDTrkNoCut', 406,
                            ['MU4'],
                            Config(L2Config('_mu4T_IDTrkNoCut',
                                            MufastHypoConfig('900GeV', mufastThr_4GeV),
                                            MucombHypoConfig('900GeV', 'passthrough')),
                                   EFConfig('_mu4T_IDTrkNoCut',
                                            TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV')))),    

    L2EFChain_mu_900_NoCut('mu4_NoCut',
                           'L2_mu4_NoCut', 843, 'L1_MU0', 'EF_mu4_NoCut', 843,
                           ['MU0'],
                           Config(L2Config('_mu4_NoCut',
                                           MufastHypoConfig('900GeV', mufastThr_4GeV),
                                           MucombHypoConfig('900GeV', 'passthrough')),
                                  EFConfig('_mu4_NoCut',
                                           TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV')))),
    
    L2EFChain_mu_NoIDTrkCut('mu6_IDTrkNoCut', 
                            'L2_mu6_IDTrkNoCut', 481, mu6Seed, 'EF_mu6_IDTrkNoCut', 481,
                            [mu6Thr],
                            Config(L2Config('_mu6_IDTrkNoCut',
                                            MufastHypoConfig('Muon', mufastThr_6GeV),
                                            MucombHypoConfig('Muon', 'passthrough')),
                                   EFConfig('_mu6_IDTrkNoCut',
                                            TrigMuonEFExtrapolatorHypoConfig('Muon','6GeV')))),


    #mu10i (old version pre pp_v2)
    L2EFChain_mu_iso('mu10i_loose', 
                     'L2_mu10i_loose', 599, mu10Seed, 'EF_mu10i_loose', 599,
                     [mu10Thr],
                     Config(L2Config('_mu10i_loose',
                                     MufastHypoConfig('Muon', mufastThr_10GeV),
                                     MucombHypoConfig('Muon', '10GeV')),
                           EFConfig('_mu10i_loose',
                                     TrigMuonEFCombinerHypoConfig('Muon', '10GeV')))),

    ]
    
    #mu1
Muons += [
        L2EFChain_mu_noiso('mu13', 
                           'L2_mu13', 873, mu13Seed, 'EF_mu13', 873,
                           [mu13Thr],
                           Config(L2Config('_mu13',
                                           #MufastHypoConfig('Muon', '13GeV'),
                                           MufastHypoConfig('Muon', mufastThr_13GeV),
                                           MucombHypoConfig('Muon', '13GeV')),
                                  EFConfig('_mu13',
                                           TrigMuonEFCombinerHypoConfig('Muon', '13GeV')))),
        ]


# --- the new L2Star code present in release 17 and above, configure the chain only there
try:
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_MuonA
    print 'igb L2Star for muons code exists'
    configL2Star=True
except:
    configL2Star=False
    print 'igb L2Star for muons does not exist'

if configL2Star :
    Muons += [
        L2EFChain_mu_L2Star_NoIDTrkCut('mu20_L2Star_IDTrkNoCut', 
                                       'L2_mu20_L2Star_IDTrkNoCut', 1015, 'L1_MU11', 'EF_mu20_L2Star_IDTrkNoCut', 1015,
                                       ['MU11'],
                                       Config(L2Config('_mu20_L2Star_IDTrkNoCut',
                                                       MufastHypoConfig('Muon', mufastThr_MSonly_20GeV),
                                                       MucombHypoConfig('Muon', 'passthrough')),
                                              EFConfig('_mu20_L2Star_IDTrkNoCut',
                                                       TrigMuonEFExtrapolatorHypoConfig('Muon','20GeV')))),

        L2EFChain_mu_900_L2Star_NoCut('mu4_L2Star_NoCut',
                                      'L2_mu4_L2Star_NoCut', 1330, 'L1_MU0', 'EF_mu4_L2Star_NoCut', 1330,
                                      ['MU0'],
                                      Config(L2Config('_mu4_L2Star_NoCut',
                                                      MufastHypoConfig('900GeV', mufastThr_4GeV),
                                                      MucombHypoConfig('900GeV', 'passthrough')),
                                             EFConfig('_mu4_L2Star_NoCut',
                                                      TrigMuonEFCombinerHypoConfig('900GeV','4GeV')))),
        
        L2EFChain_L2Star('mu18_medium_L2StarA',
                         'L2_mu18_medium_L2StarA', 1331, 'L1_MU11', 'EF_mu18_medium_L2StarA', 1331,
                         ['MU11'],
                         Config(L2Config('_mu18_medium_L2StarA',
                                         MufastHypoConfig('Muon', mufastThr_18GeV),
                                         MucombHypoConfig('Muon', '18GeV')),
                                EFConfig('_mu18_medium_L2StarA',
                                         TrigMuonEFCombinerHypoConfig('Muon','18GeV')))),

        L2EFChain_L2StarB('mu18_medium_L2StarB',
                         'L2_mu18_medium_L2StarB', 1336, 'L1_MU11', 'EF_mu18_medium_L2StarB', 1336,
                         ['MU11'],
                         Config(L2Config('_mu18_medium_L2StarB',
                                         MufastHypoConfig('Muon', mufastThr_18GeV),
                                         MucombHypoConfig('Muon', '18GeV')),
                                EFConfig('_mu18_medium_L2StarB',
                                         TrigMuonEFCombinerHypoConfig('Muon','18GeV')))),

        L2EFChain_L2StarC('mu18_medium_L2StarC',
                          'L2_mu18_medium_L2StarC', 1337, 'L1_MU11', 'EF_mu18_medium_L2StarC', 1337,
                          ['MU11'],
                          Config(L2Config('_mu18_medium_L2StarC',
                                          MufastHypoConfig('Muon', mufastThr_18GeV),
                                          MucombHypoConfig('Muon', '18GeV')),
                                 EFConfig('_mu18_medium_L2StarC',
                                          TrigMuonEFCombinerHypoConfig('Muon','18GeV')))),
        
        
        L2EFChain_900_L2Star('mu4_L2StarA',
                             'L2_mu4_L2StarA', 1332, 'L1_MU0', 'EF_mu4_L2StarA', 1332,
                             ['MU4'],
                             Config(L2Config('_mu4_L2StarA',
                                             MufastHypoConfig('900GeV', mufastThr_4GeV),
                                             MucombHypoConfig('900GeV', '4GeV')),
                                    EFConfig('_mu4_L2StarA',
                                             TrigMuonEFCombinerHypoConfig('900GeV','4GeV')))),
        
        L2EFChain_900_L2Star('mu4T_L2StarA',
                             'L2_mu4T_L2StarA', 1332, 'L1_MU4', 'EF_mu4T_L2StarA', 1332,
                             ['MU4'],
                             Config(L2Config('_mu4T_L2StarA',
                                             MufastHypoConfig('900GeV', mufastThr_4GeV),
                                             MucombHypoConfig('900GeV', '4GeV')),
                                    EFConfig('_mu4T_L2StarA',
                                             TrigMuonEFCombinerHypoConfig('900GeV','4GeV')))),
        
        L2EFChain_900_L2StarB('mu4_L2StarB',
                              'L2_mu4_L2StarB', 1342, 'L1_MU0', 'EF_mu4_L2StarB', 1342,
                              ['MU4'],
                              Config(L2Config('_mu4_L2StarB',
                                              MufastHypoConfig('900GeV', mufastThr_4GeV),
                                              MucombHypoConfig('900GeV', '4GeV')),
                                     EFConfig('_mu4_L2StarB',
                                              TrigMuonEFCombinerHypoConfig('900GeV','4GeV')))),
        
        L2EFChain_900_L2StarB('mu4T_L2StarB',
                              'L2_mu4T_L2StarB', 1342, 'L1_MU4', 'EF_mu4T_L2StarB', 1342,
                              ['MU4'],
                              Config(L2Config('_mu4T_L2StarB',
                                              MufastHypoConfig('900GeV', mufastThr_4GeV),
                                              MucombHypoConfig('900GeV', '4GeV')),
                                     EFConfig('_mu4T_L2StarB',
                                              TrigMuonEFCombinerHypoConfig('900GeV','4GeV')))),
        
        L2EFChain_900_L2StarC('mu4_L2StarC',
                              'L2_mu4_L2StarC', 1343, 'L1_MU0', 'EF_mu4_L2StarC', 1343,
                              ['MU4'],
                              Config(L2Config('_mu4_L2StarC',
                                              MufastHypoConfig('900GeV', mufastThr_4GeV),
                                              MucombHypoConfig('900GeV', '4GeV')),
                                     EFConfig('_mu4_L2StarC',
                                              TrigMuonEFCombinerHypoConfig('900GeV','4GeV')))),
        
        L2EFChain_900_L2StarC('mu4T_L2StarC',
                              'L2_mu4T_L2StarC', 1343, 'L1_MU4', 'EF_mu4T_L2StarC', 1343,
                              ['MU4'],
                              Config(L2Config('_mu4T_L2StarC',
                                              MufastHypoConfig('900GeV', mufastThr_4GeV),
                                              MucombHypoConfig('900GeV', '4GeV')),
                                     EFConfig('_mu4T_L2StarC',
                                              TrigMuonEFCombinerHypoConfig('900GeV','4GeV')))),
        
        
        ]

Muons += [



    #mu15
    L2EFChain_mu_noiso('mu15', 
                       'L2_mu15', 131, 'L1_MU10', 'EF_mu15', 131,
                       ['MU10'],
                       Config(L2Config('_mu15',
                                       MufastHypoConfig('Muon', mufastThr_15GeV),
                                       MucombHypoConfig('Muon', '15GeV')),
                              EFConfig('_mu15',
                                 TrigMuonEFCombinerHypoConfig('Muon', '15GeV'))),rerun=1),

    L2EFChain_mu_noiso('mu15_medium', 
                       'L2_mu15_medium', 139, 'L1_MU11', 'EF_mu15_medium', 139,
                       ['MU11'],
                       Config(L2Config('_mu15_medium',
                                       MufastHypoConfig('Muon', mufastThr_15GeV),
                                       MucombHypoConfig('Muon', '15GeV')),
                              EFConfig('_mu15_medium',
                                 TrigMuonEFCombinerHypoConfig('Muon', '15GeV'))),rerun=1),

    #mu15i (old version pre pp_v2)
    L2EFChain_mu_iso('mu15i_loose',
                     'L2_mu15i_loose', 483, 'L1_MU15', 'EF_mu15i_loose', 483,
                     ['MU15'],
                     Config(L2Config('_mu15i_loose',
                                     MufastHypoConfig('Muon', mufastThr_15GeV),
                                     MucombHypoConfig('Muon', '15GeV')), 
                           EFConfig('_mu15i_loose',
                                     TrigMuonEFCombinerHypoConfig('Muon', '15GeV')))),

    #mu15i
    L2EFChain_mu_iso_lowpt('mu15i',
                     'L2_mu15i', 483, 'L1_MU10', 'EF_mu15i', 483,
                     ['MU10'],
                     Config(L2Config('_mu15i',
                                     MufastHypoConfig('Muon', mufastThr_15GeV),
                                     MucombHypoConfig('Muon', '15GeV'), 
                                     MuisoHypoConfig( 'Muon', '15GeV')),
                           EFConfig('_mu15i',
                                     TrigMuonEFCombinerHypoConfig('Muon', '15GeV')))),


    #mu15i_medium
    L2EFChain_mu_iso_lowpt('mu15i_medium',
                           'L2_mu15i_medium', 484, 'L1_MU11', 'EF_mu15i_medium', 484,
                           ['MU11'],
                           Config(L2Config('_mu15i_medium',
                                           MufastHypoConfig('Muon', mufastThr_15GeV),
                                           MucombHypoConfig('Muon', '15GeV'),
                                           MuisoHypoConfig( 'Muon', '15GeV')),
                                  EFConfig('_mu15i_medium',
                                           TrigMuonEFCombinerHypoConfig('Muon', '15GeV')))),

    #mu18i2_medium (Loose)
    L2EFChain_mu_iso_l2ef('mu18i2_medium', 
                          'L2_mu18i2_medium', 5480, 'L1_MU11', 'EF_mu18i2_medium', 5480,
                          ['MU11'],
                          IsoConfig(IsoL2Config('_mu18i2_medium',
                                                MufastHypoConfig('Muon', mufastThr_18GeV),
                                                MucombHypoConfig('Muon', '18GeV'),
                                                MuisoHypoConfig( 'Muon', 'loose')),
                                    IsoEFConfig('_mu18i2_medium',
                                                TrigMuonEFCombinerHypoConfig('Muon', '18GeV'),
                                                TrigMuonEFTrackIsolationHypoConfig('Muon','Loose')))),
    
    #mu18i4_medium (Tight)
    L2EFChain_mu_iso_l2ef('mu18i4_medium', 
                          'L2_mu18i4_medium', 703, 'L1_MU11', 'EF_mu18i4_medium', 703,
                          ['MU11'],
                          IsoConfig(IsoL2Config('_mu18i4_medium',
                                                MufastHypoConfig('Muon', mufastThr_18GeV),
                                                MucombHypoConfig('Muon', '18GeV'),
                                                MuisoHypoConfig( 'Muon', 'tight')),
                                    IsoEFConfig('_mu18i4_medium',
                                                TrigMuonEFCombinerHypoConfig('Muon', '18GeV'),
                                                TrigMuonEFTrackIsolationHypoConfig('Muon','Tight')))),
    

    L2EFChain_mu_iso_l2ef_L2StarA('mu18i4_medium_L2StarA',
                                  'L2_mu18i4_medium_L2StarA', 101, 'L1_MU11', 'EF_mu18i4_medium_L2StarA', 102,
                                  ['MU11'],
                                  IsoConfig(IsoL2Config('_mu18i4_medium_L2StarA',
                                                        MufastHypoConfig('Muon', mufastThr_18GeV),
                                                        MucombHypoConfig('Muon', '18GeV'),
                                                        MuisoHypoConfig( 'Muon', 'tight')),
                                            IsoEFConfig('_mu18i4_medium_L2StarA',
                                                        TrigMuonEFCombinerHypoConfig('Muon', '18GeV'),
                                                        TrigMuonEFTrackIsolationHypoConfig('Muon','Tight')))),
    
    L2EFChain_mu_iso_l2ef_L2StarB('mu18i4_medium_L2StarB',
                                  'L2_mu18i4_medium_L2StarB', 600, 'L1_MU11', 'EF_mu18i4_medium_L2StarB', 601,
                                  ['MU11'],
                                  IsoConfig(IsoL2Config('_mu18i4_medium_L2StarB',
                                                        MufastHypoConfig('Muon', mufastThr_18GeV),
                                                        MucombHypoConfig('Muon', '18GeV'),
                                                        MuisoHypoConfig( 'Muon', 'tight')),
                                            IsoEFConfig('_mu18i4_medium_L2StarB',
                                                        TrigMuonEFCombinerHypoConfig('Muon', '18GeV'),
                                                        TrigMuonEFTrackIsolationHypoConfig('Muon','Tight')))),

   L2EFChain_mu_iso_l2ef_L2StarC('mu18i4_medium_L2StarC',
                                 'L2_mu18i4_medium_L2StarC', 173, 'L1_MU11', 'EF_mu18i4_medium_L2StarC', 174,
                                 ['MU11'],
                                 IsoConfig(IsoL2Config('_mu18i4_medium_L2StarC',
                                                       MufastHypoConfig('Muon', mufastThr_18GeV),
                                                       MucombHypoConfig('Muon', '18GeV'),
                                                       MuisoHypoConfig( 'Muon', 'tight')),
                                           IsoEFConfig('_mu18i4_medium_L2StarC',
                                                       TrigMuonEFCombinerHypoConfig('Muon', '18GeV'),
                                                       TrigMuonEFTrackIsolationHypoConfig('Muon','Tight')))),
    
    


    #mu18i1_medium (EFOnlyLoose)
    L2EFChain_mu_iso_efonly('mu18i1_medium', 
                            'L2_mu18_medium', 700, 'L1_MU11', 'EF_mu18i1_medium', 5482,
                            ['MU11'],
                            EFOnlyIsoConfig(EFOnlyIsoL2Config('_mu18_medium',
                                                              MufastHypoConfig('Muon', mufastThr_18GeV),
                                                              MucombHypoConfig('Muon', '18GeV')),
                                            EFOnlyIsoEFConfig('_mu18i1_medium',
                                                              TrigMuonEFCombinerHypoConfig('Muon', '18GeV'),
                                                              TrigMuonEFTrackIsolationHypoConfig('Muon','EFOnlyLoose')))),

    #mu18i3_medium (EFOnlyTight)
    L2EFChain_mu_iso_efonly('mu18i3_medium', 
                            'L2_mu18_medium', 700, 'L1_MU11', 'EF_mu18i3_medium', 702,
                            ['MU11'],
                            EFOnlyIsoConfig(EFOnlyIsoL2Config('_mu18_medium',
                                                              MufastHypoConfig('Muon', mufastThr_18GeV),
                                                              MucombHypoConfig('Muon', '18GeV')),
                                            EFOnlyIsoEFConfig('_mu18i3_medium',
                                                              TrigMuonEFCombinerHypoConfig('Muon', '18GeV'),
                                                              TrigMuonEFTrackIsolationHypoConfig('Muon','EFOnlyTight')))),

    #mu18i5_medium (EFOnlyMedium)
    L2EFChain_mu_iso_efonly('mu18i5_medium', 
                            'L2_mu18_medium', 700, 'L1_MU11', 'EF_mu18i5_medium', 708,
                            ['MU11'],
                            EFOnlyIsoConfig(EFOnlyIsoL2Config('_mu18_medium',
                                                              MufastHypoConfig('Muon', mufastThr_18GeV),
                                                              MucombHypoConfig('Muon', '18GeV')),
                                            EFOnlyIsoEFConfig('_mu18i5_medium',
                                                              TrigMuonEFCombinerHypoConfig('Muon', '18GeV'),
                                                              TrigMuonEFTrackIsolationHypoConfig('Muon','RelEFOnlyMedium')))),


    #mu20i2_medium
    L2EFChain_mu_iso_l2ef('mu20i2_medium', 
                          'L2_mu20i2_medium', 5484, 'L1_MU11', 'EF_mu20i2_medium', 5484,
                          ['MU11'],
                          IsoConfig(IsoL2Config('_mu20i2_medium',
                                                MufastHypoConfig('Muon', mufastThr_20GeV),
                                                MucombHypoConfig('Muon', '20GeV'),
                                                MuisoHypoConfig( 'Muon', 'loose')),
                                    IsoEFConfig('_mu20i2_medium',
                                                TrigMuonEFCombinerHypoConfig('Muon', '20GeV'),
                                                TrigMuonEFTrackIsolationHypoConfig('Muon','Loose')))),

    #mu20i4_medium
    L2EFChain_mu_iso_l2ef('mu20i4_medium', 
                          'L2_mu20i4_medium', 5485, 'L1_MU11', 'EF_mu20i4_medium', 5485,
                          ['MU11'],
                          IsoConfig(IsoL2Config('_mu20i4_medium',
                                                MufastHypoConfig('Muon', mufastThr_20GeV),
                                                MucombHypoConfig('Muon', '20GeV'),
                                                MuisoHypoConfig( 'Muon', 'tight')),
                                    IsoEFConfig('_mu20i4_medium',
                                                TrigMuonEFCombinerHypoConfig('Muon', '20GeV'),
                                                TrigMuonEFTrackIsolationHypoConfig('Muon','Tight')))),

    #mu20i1_medium
    L2EFChain_mu_iso_efonly('mu20i1_medium', 
                            'L2_mu20_medium', 1942, 'L1_MU11', 'EF_mu20i1_medium', 5486,
                            ['MU11'],
                            EFOnlyIsoConfig(EFOnlyIsoL2Config('_mu20_medium',
                                                              MufastHypoConfig('Muon', mufastThr_20GeV),
                                                              MucombHypoConfig('Muon', '20GeV')),
                                            EFOnlyIsoEFConfig('_mu20i1_medium',
                                                              TrigMuonEFCombinerHypoConfig('Muon', '20GeV'),
                                                              TrigMuonEFTrackIsolationHypoConfig('Muon','EFOnlyLoose')))),

    #mu20i3_medium
    L2EFChain_mu_iso_efonly('mu20i3_medium', 
                            'L2_mu20_medium', 1942, 'L1_MU11', 'EF_mu20i3_medium', 5487,
                            ['MU11'],
                            EFOnlyIsoConfig(EFOnlyIsoL2Config('_mu20_medium',
                                                              MufastHypoConfig('Muon', mufastThr_20GeV),
                                                              MucombHypoConfig('Muon', '20GeV')),
                                            EFOnlyIsoEFConfig('_mu20i3_medium',
                                                              TrigMuonEFCombinerHypoConfig('Muon', '20GeV'),
                                                              TrigMuonEFTrackIsolationHypoConfig('Muon','EFOnlyTight')))),

    
    #mu22i2_medium
    L2EFChain_mu_iso_l2ef('mu22i2_medium', 
                          'L2_mu22i2_medium', 705, 'L1_MU11', 'EF_mu22i2_medium', 705,
                          ['MU11'],
                          IsoConfig(IsoL2Config('_mu22i2_medium',
                                                MufastHypoConfig('Muon', mufastThr_22GeV),
                                                MucombHypoConfig('Muon', '22GeV'),
                                                MuisoHypoConfig( 'Muon', 'loose')),
                                    IsoEFConfig('_mu22i2_medium',
                                                TrigMuonEFCombinerHypoConfig('Muon', '22GeV'),
                                                TrigMuonEFTrackIsolationHypoConfig('Muon','Loose')))),

    #mu22i4_medium
    L2EFChain_mu_iso_l2ef('mu22i4_medium', 
                          'L2_mu22i4_medium', 707, 'L1_MU11', 'EF_mu22i4_medium', 707,
                          ['MU11'],
                          IsoConfig(IsoL2Config('_mu22i4_medium',
                                                MufastHypoConfig('Muon', mufastThr_22GeV),
                                                MucombHypoConfig('Muon', '22GeV'),
                                                MuisoHypoConfig( 'Muon', 'tight')),
                                    IsoEFConfig('_mu22i4_medium',
                                                TrigMuonEFCombinerHypoConfig('Muon', '22GeV'),
                                                TrigMuonEFTrackIsolationHypoConfig('Muon','Tight')))),

    #mu22i1_medium
    L2EFChain_mu_iso_efonly('mu22i1_medium', 
                            'L2_mu22_medium', 701, 'L1_MU11', 'EF_mu22i1_medium', 704,
                            ['MU11'],
                            EFOnlyIsoConfig(EFOnlyIsoL2Config('_mu22_medium',
                                                              MufastHypoConfig('Muon', mufastThr_22GeV),
                                                              MucombHypoConfig('Muon', '22GeV')),
                                            EFOnlyIsoEFConfig('_mu22i1_medium',
                                                              TrigMuonEFCombinerHypoConfig('Muon', '22GeV'),
                                                              TrigMuonEFTrackIsolationHypoConfig('Muon','EFOnlyLoose')))),

    #mu22i3_medium
    L2EFChain_mu_iso_efonly('mu22i3_medium', 
                            'L2_mu22_medium', 701, 'L1_MU11', 'EF_mu22i3_medium', 706,
                            ['MU11'],
                            EFOnlyIsoConfig(EFOnlyIsoL2Config('_mu22_medium',
                                                              MufastHypoConfig('Muon', mufastThr_22GeV),
                                                              MucombHypoConfig('Muon', '22GeV')),
                                            EFOnlyIsoEFConfig('_mu22i3_medium',
                                                              TrigMuonEFCombinerHypoConfig('Muon', '22GeV'),
                                                              TrigMuonEFTrackIsolationHypoConfig('Muon','EFOnlyTight')))),    


    #mu24i5_tight (EFOnlyMedium)
    L2EFChain_mu_iso_efonly('mu24i5_tight', 
                            'L2_mu24_tight', 107, 'L1_MU15', 'EF_mu24i5_tight', 788,
                            ['MU15'],
                            EFOnlyIsoConfig(EFOnlyIsoL2Config('_mu24_tight',
                                                              MufastHypoConfig('Muon', mufastThr_24GeV),
                                                              MucombHypoConfig('Muon', '24GeV')),
                                            EFOnlyIsoEFConfig('_mu24i5_tight',
                                                              TrigMuonEFCombinerHypoConfig('Muon', '24GeV'),
                                                              TrigMuonEFTrackIsolationHypoConfig('Muon','RelEFOnlyMedium')))),




    #mu15ii_debug #83402
    L2EFChain_mu_iso_debug('mu15ii_debug', 
                           'L2_mu15ii_debug', 486, 'L1_MU10', 'EF_mu15ii_debug', 486,
                           ['MU10'],
                           IsoConfig(IsoL2Config('_mu15ii_debug',
                                                 MufastHypoConfig('Muon', mufastThr_15GeV),
                                                 MucombHypoConfig('Muon', '15GeV'),None),
                                     IsoEFConfig('_mu15ii_debug',
                                                 TrigMuonEFCombinerHypoConfig('Muon', '15GeV'),None)),rerun=1),

    #mu4Tii
    L2EFChain_mu_iso_debug('mu4Tii_debug', 
                           'L2_mu4Tii_debug', 1696, 'L1_MU4', 'EF_mu4Tii_debug', 1696,
                           ['MU4'],
                           IsoConfig(IsoL2Config('_mu4Tii_debug',
                                                 MufastHypoConfig('900GeV', mufastThr_4GeV),
                                                 MucombHypoConfig('900GeV', '4GeV'),None),
                                     IsoEFConfig('_mu4Tii_debug',
                                                 TrigMuonEFCombinerHypoConfig('900GeV', '4GeV'),None)),rerun=1),
    
    #mu6ii
    L2EFChain_mu_iso_debug('mu6ii_debug', 
                           'L2_mu6ii_debug', 1697,  mu6Seed, 'EF_mu6ii_debug', 1697,
                           [mu6Thr],
                           IsoConfig(IsoL2Config('_mu6ii_debug',
                                                 MufastHypoConfig('Muon', mufastThr_6GeV),
                                                 MucombHypoConfig('Muon', '6GeV'),None),
                                     IsoEFConfig('_mu6ii_debug',
                                                 TrigMuonEFCombinerHypoConfig('Muon', '6GeV'),None)),rerun=1),


        
    #mu20
    L2EFChain_mu_noiso('mu20', 
                       'L2_mu20', 134, 'L1_MU10', 'EF_mu20', 134,
                       ['MU10'],
                       Config(L2Config('_mu20',
                                       MufastHypoConfig('Muon', mufastThr_20GeV),
                                       MucombHypoConfig('Muon', '20GeV')), 
                              EFConfig('_mu20',
                                       TrigMuonEFCombinerHypoConfig('Muon', '20GeV')))),


    #mu20_medium
    L2EFChain_mu_noiso('mu20_medium',
                       'L2_mu20_medium', 138, 'L1_MU11', 'EF_mu20_medium', 138,
                       ['MU11'],
                       Config(L2Config('_mu20_medium',
                                       MufastHypoConfig('Muon', mufastThr_20GeV),
                                       MucombHypoConfig('Muon', '20GeV')),
                              EFConfig('_mu20_medium',
                                       TrigMuonEFCombinerHypoConfig('Muon', '20GeV')))),

    #mu24_medium
    L2EFChain_mu_noiso('mu24_medium',
                       'L2_mu24_medium', 797, 'L1_MU11', 'EF_mu24_medium', 797,
                       ['MU11'],
                       Config(L2Config('_mu24_medium',
                                       MufastHypoConfig('Muon', mufastThr_24GeV),
                                       MucombHypoConfig('Muon', '24GeV')),
                              EFConfig('_mu24_medium',
                                       TrigMuonEFCombinerHypoConfig('Muon', '24GeV')))),

    #mu30_medium
    L2EFChain_mu_noiso('mu30_medium',
                       'L2_mu30_medium', 748, 'L1_MU11', 'EF_mu30_medium', 748,
                       ['MU11'],
                       Config(L2Config('_mu30_medium',
                                       MufastHypoConfig('Muon', mufastThr_30GeV),
                                       MucombHypoConfig('Muon', '30GeV')),
                              EFConfig('_mu30_medium',
                                       TrigMuonEFCombinerHypoConfig('Muon', '30GeV')))),

    #mu20_tight
    L2EFChain_mu_noiso('mu20_tight',
                       'L2_mu20_tight', 695, 'L1_MU15', 'EF_mu20_tight', 695,
                       ['MU15'],
                       Config(L2Config('_mu20_tight',
                                       MufastHypoConfig('Muon', mufastThr_20GeV),
                                       MucombHypoConfig('Muon', '20GeV')),
                              EFConfig('_mu20_tight',
                                       TrigMuonEFCombinerHypoConfig('Muon', '20GeV')))),

    #mu24_tight
    L2EFChain_mu_noiso('mu24_tight',
                       'L2_mu24_tight', 107, 'L1_MU15', 'EF_mu24_tight', 107,
                       ['MU15'],
                       Config(L2Config('_mu24_tight',
                                       MufastHypoConfig('Muon', mufastThr_24GeV),
                                       MucombHypoConfig('Muon', '24GeV')),
                              EFConfig('_mu24_tight',
                                       TrigMuonEFCombinerHypoConfig('Muon', '24GeV')))),

    #mu30_tight
    L2EFChain_mu_noiso('mu30_tight',
                       'L2_mu30_tight', 582, 'L1_MU15', 'EF_mu30_tight', 582,
                       ['MU15'],
                       Config(L2Config('_mu30_tight',
                                       MufastHypoConfig('Muon', mufastThr_30GeV),
                                       MucombHypoConfig('Muon', '30GeV')),
                              EFConfig('_mu30_tight',
                                       TrigMuonEFCombinerHypoConfig('Muon', '30GeV')))),


    #mu20_empty
    L2EFChain_mu_noiso('mu20_empty', 
                       'L2_mu20_empty', 920, 'L1_MU10_EMPTY',
                       'EF_mu20_empty', 920,
                       ['MU10'],
                       Config(L2Config('_mu20',
                                       MufastHypoConfig('Muon', mufastThr_20GeV),
                                       MucombHypoConfig('Muon', '20GeV')), 
                              EFConfig('_mu20',
                                       TrigMuonEFCombinerHypoConfig('Muon',
                                                                    '20GeV')))),
    

    #mu20i (old version pre pp_v2)
    L2EFChain_mu_iso('mu20i_loose',
                     'L2_mu20i_loose', 132, 'L1_MU10', 'EF_mu20i_loose', 132,
                     ['MU10'],
                     Config(L2Config('_mu20i_loose',
                                     MufastHypoConfig('Muon', mufastThr_20GeV),
                                     MucombHypoConfig('Muon', '20GeV')), 
                            EFConfig('_mu20i_loose',
                                     TrigMuonEFCombinerHypoConfig('Muon', '20GeV')))),

    #mu20i 
    L2EFChain_mu_iso('mu20i',
                     'L2_mu20i', 132, 'L1_MU10', 'EF_mu20i', 132,
                     ['MU10'],
                     Config(L2Config('_mu20i',
                                     MufastHypoConfig('Muon', mufastThr_20GeV),
                                     MucombHypoConfig('Muon', '20GeV'), 
                                     MuisoHypoConfig( 'Muon', '20GeV')),
                     EFConfig('_mu20i',
                                     TrigMuonEFCombinerHypoConfig('Muon', '20GeV')))),


    #mu20i_medium
    L2EFChain_mu_iso('mu20i_medium',
                     'L2_mu20i_medium', 133, 'L1_MU11', 'EF_mu20i_medium', 133,
                     ['MU11'],
                     Config(L2Config('_mu20i_medium',
                                     MufastHypoConfig('Muon', mufastThr_20GeV),
                                     MucombHypoConfig('Muon', '20GeV'),
                                     MuisoHypoConfig( 'Muon', '20GeV')),
                            EFConfig('_mu20i_medium',
                                     TrigMuonEFCombinerHypoConfig('Muon', '20GeV')))),
    
    
    #mu40
    L2EFChain_mu_noiso('mu40',
                       'L2_mu40', 133, 'L1_MU40', 'EF_mu40', 133,
                       ['MU40'],
                       Config(L2Config('_mu40',
                                       MufastHypoConfig('Muon', mufastThr_40GeV),
                                       MucombHypoConfig('Muon', '40GeV')), 
                              EFConfig('_mu40',
                                       TrigMuonEFCombinerHypoConfig('Muon', '40GeV')))),
    
    L2EFChain_mu_MSonly('mu40_MSonly', 
                        'L2_mu40_MSonly', 520, 'L1_MU10',
                        'EF_mu40_MSonly', 520,
                        ['MU10'],
                        Config(L2Config('_mu40_MSonly',
                                        MufastHypoConfig('Muon', mufastThr_MSonly_40GeV),
                                        None),
                               EFConfig('_mu40_MSonly',
                                        TrigMuonEFExtrapolatorHypoConfig('Muon','40GeV')))),
    
    L2EFChain_mu_MSonly('mu40_MSonly_empty', 
                        'L2_mu40_MSonly_empty', 947, 'L1_MU10_EMPTY',
                        'EF_mu40_MSonly_empty', 947,
                        ['MU10'],
                        Config(L2Config('_mu40_MSonly_empty',
                                        MufastHypoConfig('Muon', mufastThr_MSonly_40GeV),
                                        None),
                               EFConfig('_mu40_MSonly_empty',
                                        TrigMuonEFExtrapolatorHypoConfig('Muon','40GeV')))),
    
    L2EFChain_mu_MSonly_Ecut4('mu40_MSonly_tight', 
                              'L2_mu40_MSonly_tight', 948, 'L1_MU10', 
                              'EF_mu40_MSonly_tight', 948, 
                              ['MU10'], 
                              Config(L2Config('_mu40_MSonly_tight', 
                                              MufastHypoConfig('Muon', mufastThr_MSonly_40GeV), 
                                              None), 
                                     EFConfig('_mu40_MSonly_tight', TrigMuonEFExtrapolatorHypoConfig('Muon','40GeV')))),

    L2EFChain_mu_MSonly_Ecut4('mu40_MSonly_tight_L1MU11', 
                              'L2_mu40_MSonly_tight_L1MU11', 949, 'L1_MU11', 
                              'EF_mu40_MSonly_tight_L1MU11', 949, 
                              ['MU11'], 
                              Config(L2Config('_mu40_MSonly_tight_L1MU11', 
                                              MufastHypoConfig('Muon', mufastThr_MSonly_40GeV), 
                                              None), 
                                     EFConfig('_mu40_MSonly_tight_L1MU11', TrigMuonEFExtrapolatorHypoConfig('Muon','40GeV')))),

    L2EFChain_mu_MSonly_Ecut4('mu40_MSonly_tighter', 
                              'L2_mu40_MSonly_tighter', 946, 'L1_MU10', 
                              'EF_mu40_MSonly_tighter', 946, 
                              ['MU10'], 
                              Config(L2Config('_mu40_MSonly_tighter', 
                                              MufastHypoConfig('Muon', mufastThr_MSonly_40GeV_uptoEC2), 
                                              None), 
                                     EFConfig('_mu40_MSonly_tighter', TrigMuonEFExtrapolatorHypoConfig('Muon','40GeV_uptoEC2')))), 
    
    L2EFChain_mu_MSonly('mu60_MSonly', 
                        'L2_mu60_MSonly', 7520, 'L1_MU10',
                        'EF_mu60_MSonly', 7520,
                        ['MU10'],
                        Config(L2Config('_mu60_MSonly',
                                        MufastHypoConfig('Muon', mufastThr_MSonly_60GeV),
                                        None),
                               EFConfig('_mu60_MSonly',
                                        TrigMuonEFExtrapolatorHypoConfig('Muon','60GeV')))),
    
    L2EFChain_mu_MSonly('mu80_MSonly', 
                        'L2_mu80_MSonly', 7521, 'L1_MU10',
                        'EF_mu80_MSonly', 7521,
                        ['MU10'],
                        Config(L2Config('_mu80_MSonly',
                                        MufastHypoConfig('Muon', mufastThr_MSonly_80GeV),
                                        None),
                               EFConfig('_mu80_MSonly',
                                        TrigMuonEFExtrapolatorHypoConfig('Muon', '80GeV')))),
    
    L2EFChain_mu_MSonly('mu100_MSonly', 
                        'L2_mu100_MSonly', 7522, 'L1_MU10',
                        'EF_mu100_MSonly', 7522,
                        ['MU10'],
                        Config(L2Config('_mu100_MSonly',
                                        MufastHypoConfig('Muon', mufastThr_MSonly_100GeV),
                                        None),
                               EFConfig('_mu100_MSonly',
                                        TrigMuonEFExtrapolatorHypoConfig('Muon','100GeV')))),
    
    L2EFChain_mu_MSonly('mu40_MSonly_barrel', 
                        'L2_mu40_MSonly_barrel', 944, 'L1_MU10', 
                        'EF_mu40_MSonly_barrel', 944, 
                        ['MU10'], 
                        Config(L2Config('_mu40_MSonly_barrel', 
                                        MufastHypoConfig('Muon', mufastThr_MSonly_40GeV_barrelOnly), 
                                        None), 
                               EFConfig('_mu40_MSonly_barrel', TrigMuonEFExtrapolatorHypoConfig('Muon','40GeV_barrelOnly')))), 
    
    L2EFChain_mu_MSonly('mu40_MSonly_barrel_medium',
                        'L2_mu40_MSonly_barrel_medium', 874, 'L1_MU11',
                        'EF_mu40_MSonly_barrel_medium', 874,
                        ['MU11'],
                        Config(L2Config('_mu40_MSonly_barrel_medium',
                                        MufastHypoConfig('Muon', mufastThr_MSonly_40GeV_barrelOnly),
                                        None),
                               EFConfig('_mu40_MSonly_barrel_medium', TrigMuonEFExtrapolatorHypoConfig('Muon','40GeV_barrelOnly')))),
    

    L2EFChain_mu_MSonly('mu20_MSonly', 
                        'L2_mu20_MSonly', 560, 'L1_MU10', 
                        'EF_mu20_MSonly', 560,
                        ['MU10'],
                        Config(L2Config('_mu20_MSonly', 
                                        MufastHypoConfig('Muon', mufastThr_MSonly_20GeV), 
                                        None), 
                               EFConfig('_mu20_MSonly', TrigMuonEFExtrapolatorHypoConfig('Muon','20GeV')))),    
    
    L2EFChain_mu_MSonly('mu10_MSonly', 
                        'L2_mu10_MSonly', 812, mu10Seed, 
                        'EF_mu10_MSonly', 812,
                        [mu10Thr],
                        Config(L2Config('_mu10_MSonly', 
                                        MufastHypoConfig('Muon', mufastThr_MSonly_10GeV), 
                                        None),
                               EFConfig('_mu10_MSonly', TrigMuonEFExtrapolatorHypoConfig('Muon','10GeV')))),


# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------

    L2EFChain_mu_MSonly('mu6_MSonly', 
                        'L2_mu6_MSonly', 828, mu6Seed, 
                        'EF_mu6_MSonly', 828,
                        [mu6Thr],
                        Config(L2Config('_mu6_MSonly', 
                                        MufastHypoConfig('Muon', mufastThr_6GeV), 
                                        None),
                               EFConfig('_mu6_MSonly', TrigMuonEFExtrapolatorHypoConfig('Muon','6GeV')))),

    L2EFChain_mu_iso_900_lowpt('mu4i',
                               'L2_mu4i', 6483, 'L1_MU0', 'EF_mu4i', 6483,
                               ['MU0'],
                               Config(L2Config('_mu4i',
                                               MufastHypoConfig('900GeV', mufastThr_4GeV),
                                               MucombHypoConfig('900GeV', '4GeV'), 
                                               MuisoHypoConfig('Muon', 'DY')),
                                      EFConfig('_mu4i',
                                               TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),

    L2EFChain_mu_iso_900_lowpt('mu4Ti',
                               'L2_mu4Ti', 6483, 'L1_MU4', 'EF_mu4Ti', 6483,
                               ['MU4'],
                               Config(L2Config('_mu4Ti',
                                               MufastHypoConfig('900GeV', mufastThr_4GeV),
                                               MucombHypoConfig('900GeV', '4GeV'), 
                                               MuisoHypoConfig('Muon', 'DY')),
                                      EFConfig('_mu4Ti',
                                               TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),    
    
    L2EFChain_mu_iso_900_lowpt('mu6i',
                               'L2_mu6i', 6484, mu6Seed, 'EF_mu6i', 6484,
                               [mu6Thr],
                               Config(L2Config('_mu6i',
                                               MufastHypoConfig('900GeV', mufastThr_6GeV),
                                               MucombHypoConfig('900GeV', '6GeV'), 
                                               MuisoHypoConfig('Muon', 'DY')),
                                      EFConfig('_mu6i',
                                               TrigMuonEFCombinerHypoConfig('900GeV', '6GeV')))),


    L2EFChain_mu_muCombTag_NoEF('mu20_muCombTag_NoEF', 
                                'L2_mu20_muCombTag_NoEF', 8101, 'L1_MU10', 
                                'EF_mu20_muCombTag_NoEF', 8101, 
                                ['MU10'], 
                                Config(L2Config('_mu20_muCombTag_NoEF', 
                                                None, 
                                                MucombHypoConfig('Muon', '20GeV'), 
                                                None), 
                                       EFConfig('_mu20_muCombTag_NoEF', None))),
    

    L2EFChain_mu_muCombTag_NoEF('mu10_muCombTag_NoEF', 
                                'L2_mu10_muCombTag_NoEF', 8102, 'L1_MU20', 
                                'EF_mu10_muCombTag_NoEF', 8102, 
                                ['MU20'], 
                                Config(L2Config('_mu10_muCombTag_NoEF', 
                                                None, 
                                                MucombHypoConfig('Muon', '10GeV'), 
                                                None), 
                                       EFConfig('_mu10_muCombTag_NoEF', None))),


   L2EFChain_mu_muCombTag_NoEF('mu13_muCombTag_NoEF', 
                                'L2_mu13_muCombTag_NoEF', 8103, 'L1_MU20', 
                                'EF_mu13_muCombTag_NoEF', 8103, 
                                ['MU20'], 
                                Config(L2Config('_mu13_muCombTag_NoEF', 
                                                None, 
                                                MucombHypoConfig('Muon', '13GeV'), 
                                                None), 
                                       EFConfig('_mu13_muCombTag_NoEF', None))),


    L2EFChain_mu_sitrack_loose('mu4_SiTrk', 
                               'L2_mu4_SiTrk', 625, 'L1_MU0', 'EF_mu4_SiTrk', 625,
                               ['MU0'],
                               Config(L2Config('_mu4_SiTrk',
                                               MufastHypoConfig('900GeV', mufastThr_4GeV),
                                               MucombHypoConfig('900GeV', '4GeV'),
                                               None), # MuisoHypoConfig()), 
                                      EFConfig('_mu4_SiTrk', TrigMuonEFCombinerHypoConfig('900GeV','4GeV')))),

    L2EFChain_mu_sitrack_loose('mu4T_SiTrk', 
                               'L2_mu4T_SiTrk', 625, 'L1_MU4', 'EF_mu4T_SiTrk', 625,
                               ['MU4'],
                               Config(L2Config('_mu4T_SiTrk',
                                               MufastHypoConfig('900GeV', mufastThr_4GeV),
                                               MucombHypoConfig('900GeV', '4GeV'),
                                               None), # MuisoHypoConfig()), 
                                      EFConfig('_mu4T_SiTrk', TrigMuonEFCombinerHypoConfig('900GeV','4GeV')))),    

   
    L2EFChain_mu_900_MSonly_EFFS('mu4_MSonly_EFFS_passL2', 
                                 'L2_mu4_MSonly_EFFS_passL2', 2073, 'L1_MU0', 
                                 'EF_mu4_MSonly_EFFS_passL2', 2073,
                                 ['MU0'],
                                 Config(L2Config('_mu4_MSonly_EFFS_passL2', 
                                                 MufastHypoConfig('900GeV', 'passthrough'), None), 
                                        EFConfig('_mu4_MSonly_EFFS_passL2', TrigMuonEFExtrapolatorHypoConfig('900GeV','4GeV')))),

    

    ]




###########################################################################
###########################################################################

Config   = L2EFChain_mu_MSonly_rejectFailedExtrap.Config
L2Config = L2EFChain_mu_MSonly_rejectFailedExtrap.L2Config
EFConfig = L2EFChain_mu_MSonly_rejectFailedExtrap.EFConfig

Muons += [

    L2EFChain_mu_MSonly_rejectFailedExtrap('mu40_MSonly_barrel_medium1',
                                           'L2_mu40_MSonly_barrel_medium1', 7874, 'L1_MU11',
                                           'EF_mu40_MSonly_barrel_medium1', 7874,
                                           ['MU11'],
                                           Config(L2Config('_mu40_MSonly_barrel_medium1',
                                                           MufastHypoConfig('Muon', mufastThr_MSonly_40GeV_barrelOnly),
                                                           None),
                                                  EFConfig('_mu40_MSonly_barrel_medium1', TrigMuonEFExtrapolatorHypoConfig('Muon','40GeV_barrelOnly')))),
    

]


###########################################################################
# dR veto
###########################################################################
from TriggerMenuPython.ChainTemplate import *

class MergedChain(ChainTemplate):
    def __init__(self, sig, l2, ef=DeduceChainConfig, tomerge=None, OnlyEF=False, groups=None,
                 straighten=False,
                 runSequencially=False):
        if ef == DeduceChainConfig:
            ef = Chain()
        ef.replace_if_default('name', 'EF_%s'%sig)
        ChainTemplate.__init__(self, sig, l2, ef)        

        self.tomerge = tomerge
        self.OnlyEF = OnlyEF
        self.straighten = straighten # Straighten combined chain signature list
        self.runSequencially = runSequencially
        
	#"New" style chains go straight to the TrigPythonConfig
        #"Old" style chains only fill the TPC filled at the last stage
        #To be able to merge the two, first try the merge during init
        #If this fails, try again during menu generation
        try:
            self.merge(self.tomerge, onlyEF=self.OnlyEF)
            self.seq_generated = True
        except AttributeError:
            self.seq_generated = False

        if groups is not None:
            for x in groups: self.addGroups(x)
        else:
            for x in ['RATE:SingleMuon','BW:Muon']: self.addGroups(x)
        self.addStreams('muons')
        
    def generateMenu(self, tpc):
        if not self.seq_generated:
            self.merge(self.tomerge, onlyEF=self.OnlyEF)
            self.seq_generated = True
        if self.straighten:
            # L2
            telist, tes = [], []
            for sig in self.l2.siglist:
                for te in sig.tes:
                    telist.append(te)
            self.l2.siglist = []
            for te in telist:
                if te not in tes:
                    tes.append(te)
                    self.l2.addHLTSignature([te])
            # EF
            telist, tes = [], []
            for sig in self.ef.siglist:
                for te in sig.tes:
                    telist.append(te)
            self.ef.siglist = []
            for te in telist:
                if te not in tes:
                    tes.append(te)
                    self.ef.addHLTSignature([te])
        if self.runSequencially:
            def reorder(chain):
                # Reorder signature list to have 1 TE in each signature
                # Original siglist[sig][te] reordered to
                # sl[0][0],sl[1][0],sl[2][0], ..., sl[0][1],sl[1][1], ...
                telist = []
                n = max(map(lambda x: x.sigcounter, chain.siglist) )
                n = max(map(lambda x: len(x.tes), chain.siglist) )
                sig1 = 1
                if len(chain.siglist)>0: sig1 = chain.siglist[0].sigcounter
                for i in range(n):
                    for sig in chain.siglist:
                        if len(sig.tes) > i and sig.tes[i] not in telist:
                            telist.append(sig.tes[i])
                chain.siglist = []
                #print 'telist: ', telist
                for te in telist:
                    chain.addHLTSignature([te], sigcounter=sig1)
                    sig1 += 1
            reorder(self.l2)
            reorder(self.ef)
    pass



#default settings of this hypo are 0.2, 30GeV;
from TrigBjetHypo.TrigLeptonJetFexAllTEConfig  import EFLeptonJetFexAllTE_FarOff
FarOffLeptonJet_dR02_Et10 = EFLeptonJetFexAllTE_FarOff("FarOffLeptonJet_dR02_Et10")
FarOffLeptonJet_dR02_Et10.EtThreshold = 10*GeV
FarOffLeptonJet_dR02_Et10.DeltaRCut = 0.2

FarOffLeptonJet_dR02_Et30 = EFLeptonJetFexAllTE_FarOff("FarOffLeptonJet_dR02_Et30")
FarOffLeptonJet_dR02_Et30.EtThreshold = 30*GeV
FarOffLeptonJet_dR02_Et30.DeltaRCut = 0.2

FarOffLeptonJet_dR04_Et10 = EFLeptonJetFexAllTE_FarOff("FarOffLeptonJet_dR04_Et10")
FarOffLeptonJet_dR04_Et10.EtThreshold = 10*GeV
FarOffLeptonJet_dR04_Et10.DeltaRCut = 0.4

FarOffLeptonJet_dR04_Et30 = EFLeptonJetFexAllTE_FarOff("FarOffLeptonJet_dR04_Et30")
FarOffLeptonJet_dR04_Et30.EtThreshold = 30*GeV
FarOffLeptonJet_dR04_Et30.DeltaRCut = 0.4


Muons += [
    MergedChain('mu20_medium_j10dR02veto', None,
                ef=Chain(name='EF_mu20_medium_j10dR02veto', counter=435,seed='L2_mu20_medium'),
                OnlyEF=True).
    addSequence(['EF_mu20_medium','EF_j20_a4tc_EFFS_jetNoCut'],[FarOffLeptonJet_dR02_Et10], 'EF_mu20_medium_j10dR02veto').addSignature(),

    MergedChain('mu20_medium_j30dR02veto', None,
                ef=Chain(name='EF_mu20_medium_j30dR02veto', counter=436,seed='L2_mu20_medium'),
                OnlyEF=True).
    addSequence(['EF_mu20_medium','EF_j20_a4tc_EFFS_jetNoCut'],[FarOffLeptonJet_dR02_Et30], 'EF_mu20_medium_j30dR02veto').addSignature(),

    MergedChain('mu20_medium_j10dR04veto', None,
                ef=Chain(name='EF_mu20_medium_j10dR04veto', counter=439,seed='L2_mu20_medium'),
                OnlyEF=True).
    addSequence(['EF_mu20_medium','EF_j20_a4tc_EFFS_jetNoCut'],[FarOffLeptonJet_dR04_Et10], 'EF_mu20_medium_j10dR04veto').addSignature(),

    MergedChain('mu20_medium_j30dR04veto', None,
                ef=Chain(name='EF_mu20_medium_j30dR04veto', counter=438,seed='L2_mu20_medium'),
                OnlyEF=True).
    addSequence(['EF_mu20_medium','EF_j20_a4tc_EFFS_jetNoCut'],[FarOffLeptonJet_dR04_Et30], 'EF_mu20_medium_j30dR04veto').addSignature(),
]



###########################################################################

Config   = L2EFChain_mu_noroi.Config
L2Config = L2EFChain_mu_noroi.L2Config
EFConfig = L2EFChain_mu_noroi.EFConfig

Muons += [
    # RoI based
    L2EFChain_mu_noroi('mu0_missingRoi', 
                       'L2_mu0_missingRoi', 6971, 'L1_MU0', 'EF_mu0_missingRoi', 6971,
                       ['MU0'],
                       Config(L2Config('_mu0_missingRoi'),
                              EFConfig('_mu0_missingRoi'))),
    L2EFChain_mu_NoAlg('L1MU0_NoAlg', 
                       'L2_L1MU0_NoAlg', 888, 'L1_MU0', 
                       'EF_L1MU0_NoAlg', 888,
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

    L2EFChain_mu_NoAlg('2mu10_NoAlg', 
                       'L2_2mu10_NoAlg', 689, 'L1_2MU10', 
                       'EF_2mu10_NoAlg', 689,
                       [], Config(L2Config(''), EFConfig(''))),
    L2EFChain_mu_NoAlg('mu0_empty_NoAlg', 
                       'L2_mu0_empty_NoAlg', 894, mu4emptySeed, 
                       'EF_mu0_empty_NoAlg', 894,
                       [], Config(L2Config(''), EFConfig(''))),
    L2EFChain_mu_NoAlg('mu4_empty_NoAlg', 
                       'L2_mu4_empty_NoAlg', 894, 'L1_MU4_EMPTY', 
                       'EF_mu4_empty_NoAlg', 894,
                       [], Config(L2Config(''), EFConfig(''))),    
    L2EFChain_mu_NoAlg('mu0_firstempty_NoAlg', 
                       'L2_mu0_firstempty_NoAlg', 892, 'L1_MU0_FIRSTEMPTY', 
                       'EF_mu0_firstempty_NoAlg', 892,
                       [], Config(L2Config(''), EFConfig(''))),
    L2EFChain_mu_NoAlg('mu4_firstempty_NoAlg', 
                       'L2_mu4_firstempty_NoAlg', 892, 'L1_MU4_FIRSTEMPTY', 
                       'EF_mu4_firstempty_NoAlg', 892,
                       [], Config(L2Config(''), EFConfig(''))),    
    L2EFChain_mu_NoAlg('mu0_unpaired_iso_NoAlg', 
                       'L2_mu0_unpaired_iso_NoAlg', 893, 'L1_MU0_UNPAIRED_ISO', 
                       'EF_mu0_unpaired_iso_NoAlg', 893,
                       [], Config(L2Config(''), EFConfig(''))),
    L2EFChain_mu_NoAlg('mu4_unpaired_iso_NoAlg', 
                       'L2_mu4_unpaired_iso_NoAlg', 893, 'L1_MU4_UNPAIRED_ISO', 
                       'EF_mu4_unpaired_iso_NoAlg', 893,
                       [], Config(L2Config(''), EFConfig(''))),    
    L2EFChain_mu_NoAlg('mu0_unpaired_noniso_NoAlg', 
                       'L2_mu0_unpaired_noniso_NoAlg', 896, 'L1_MU0_UNPAIRED_NONISO', 
                       'EF_mu0_unpaired_noniso_NoAlg', 896,
                       [], Config(L2Config(''), EFConfig(''))),
    L2EFChain_mu_NoAlg('L1MU10_firstempty_NoAlg', 
                       'L2_L1MU10_firstempty_NoAlg', 2894, 'L1_MU10_FIRSTEMPTY', 
                       'EF_L1MU10_firstempty_NoAlg', 2894,
                       [], Config(L2Config(''), EFConfig(''))),
    L2EFChain_mu_NoAlg('mu11_empty_NoAlg', 
                       'L2_mu11_empty_NoAlg', 891, 'L1_MU11_EMPTY', 
                       'EF_mu11_empty_NoAlg', 891,
                       [], Config(L2Config(''), EFConfig(''))),
    ]

###########################################################################
###########################################################################
# TrigMuSuperEF - the merged algorithm between TrigMuonEF and TrigMuGirl

Muons += [

    L2EFChain_mu_EFmerged('mu18_OR_medium',        7180, 'L1_MU11', 'MU11', '18GeV'),
    
    ######## TrigMuSuperEFIso chains: name convention: mu*i_OR* ########
    
    #mu18iN_OR
    #L2EFChain_mu_EFmerged('mu18i1_OR_medium', 7185, 'L1_MU11', 'MU15', '18GeV', EFiso='EFOnlyLoose'),
    #L2EFChain_mu_EFmerged('mu18i2_OR_medium', 7186, 'L1_MU11', 'MU15', '18GeV', EFiso='Loose', L2iso='loose'),
    #L2EFChain_mu_EFmerged('mu18i3_OR_medium', 7187, 'L1_MU11', 'MU15', '18GeV', EFiso='EFOnlyTight'),
    #L2EFChain_mu_EFmerged('mu18i4_OR_medium', 7188, 'L1_MU11', 'MU15', '18GeV', EFiso='Tight', L2iso='tight'),
    #L2EFChain_mu_EFmerged('mu18i5_OR_medium', 7189, 'L1_MU11', 'MU15', '18GeV', EFiso='RelEFOnlyMedium'),

    #mu24iN_OR
    #L2EFChain_mu_EFmerged('mu24i1_OR_tight', 7241, 'L1_MU15', 'MU15', '24GeV', EFiso='EFOnlyLoose'),
    #L2EFChain_mu_EFmerged('mu24i2_OR_tight', 7242, 'L1_MU15', 'MU15', '24GeV', EFiso='Loose', L2iso='loose'),
    #L2EFChain_mu_EFmerged('mu24i3_OR_tight', 7243, 'L1_MU15', 'MU15', '24GeV', EFiso='EFOnlyTight'),
    #L2EFChain_mu_EFmerged('mu24i4_OR_tight', 7244, 'L1_MU15', 'MU15', '24GeV', EFiso='Tight', L2iso='tight'),
    L2EFChain_mu_EFmerged('mu24i5_OR_tight', 7245, 'L1_MU15', 'MU15', '24GeV', EFiso='RelEFOnlyMedium'),

    ######### End of TrigMuSuperEFIso #########

    ######### Begin performance comparison chains #########

    L2EFChain_mu_EFmerged_MGfirst  ('mu18_OR_medium_MGfirst',   7193, 'L1_MU11', 'MU11', '18GeV'),
    L2EFChain_mu_EFmerged_TMEFfirst('mu18_OR_medium_TMEFfirst', 7194, 'L1_MU11', 'MU11', '18GeV'),
    L2EFChain_mu_EFmerged_both     ('mu18_OR_medium_both',      7195, 'L1_MU11', 'MU11', '18GeV'),

    L2EFChain_mu_EFmerged_MGfirst  ('mu24i5_OR_tight_MGfirst',   7841, 'L1_MU15', 'MU15', '24GeV', EFiso='RelEFOnlyMedium'),
    L2EFChain_mu_EFmerged_TMEFfirst('mu24i5_OR_tight_TMEFfirst', 7842, 'L1_MU15', 'MU15', '24GeV', EFiso='RelEFOnlyMedium'),
    L2EFChain_mu_EFmerged_both     ('mu24i5_OR_tight_both',      7843, 'L1_MU15', 'MU15', '24GeV', EFiso='RelEFOnlyMedium'),
    
    ######### End performance comparison chains #########
    
    ######### Begin validation chains #########

    L2EFChain_mu_EFmerged_MGonly  ('mu18_OR_medium_MGonly',    7191, 'L1_MU11', 'MU11', '18GeV'),
    L2EFChain_mu_EFmerged_TMEFonly('mu18_OR_medium_TMEFonly',  7192, 'L1_MU11', 'MU11', '18GeV'),

    
    #L2EFChain_mu_EFmerged_TMEFonly('mu24i1_OR_TMEFonly', 7251, 'L1_MU15', 'MU15', '24GeV', EFiso='EFOnlyLoose'),
    #L2EFChain_mu_EFmerged_TMEFonly('mu24i2_OR_TMEFonly', 7252, 'L1_MU15', 'MU15', '24GeV', EFiso='Loose', L2iso='loose'),
    #L2EFChain_mu_EFmerged_TMEFonly('mu24i3_OR_TMEFonly', 7253, 'L1_MU15', 'MU15', '24GeV', EFiso='EFOnlyTight'),
    #L2EFChain_mu_EFmerged_TMEFonly('mu24i4_OR_TMEFonly', 7254, 'L1_MU15', 'MU15', '24GeV', EFiso='Tight', L2iso='tight'),

    L2EFChain_mu_EFmerged_TMEFonly('mu24i5_OR_tight_TMEFonly', 7255, 'L1_MU15', 'MU15', '24GeV', EFiso='RelEFOnlyMedium'),
    L2EFChain_mu_EFmerged_MGonly  ('mu24i5_OR_tight_MGonly',   7256, 'L1_MU15', 'MU15', '24GeV', EFiso='RelEFOnlyMedium'),

    ######### End validation chains #########
    
    ]

###########################################################################


EFOnlyIsoAnnulusConfig   = L2EFChain_mu_iso_annulus.Config
EFOnlyIsoAnnulusL2Config = L2EFChain_mu_iso_annulus.L2Config
EFOnlyIsoAnnulusEFConfig = L2EFChain_mu_iso_annulus.EFConfig

Muons += [

    #Isolation Annulus                                                                                                                                                                                                                       
    L2EFChain_mu_iso_annulus('mu18ia1_medium',
                             'L2_mu18_medium', 700, 'L1_MU11', 'EF_mu18ia1_medium', 5443,
                             ['MU11'],
                             EFOnlyIsoAnnulusConfig(EFOnlyIsoAnnulusL2Config('_mu18_medium',
                                                                             MufastHypoConfig('Muon', mufastThr_18GeV),
                                                                             MucombHypoConfig('Muon', '18GeV')),
                                                    EFOnlyIsoAnnulusEFConfig('_mu18ia1_medium',
                                                                             TrigMuonEFCombinerHypoConfig('Muon', '18GeV'),
                                                                             TrigMuonEFTrackIsolationHypoConfig('Muon','EFOnlyLooseWide')))),

    L2EFChain_mu_iso_annulus('mu18ia2_medium',
                             'L2_mu18_medium', 700, 'L1_MU11', 'EF_mu18ia2_medium', 5444,
                             ['MU11'],
                             EFOnlyIsoAnnulusConfig(EFOnlyIsoAnnulusL2Config('_mu18_medium',
                                                                             MufastHypoConfig('Muon', mufastThr_18GeV),
                                                                             MucombHypoConfig('Muon', '18GeV')),
                                                    EFOnlyIsoAnnulusEFConfig('_mu18ia2_medium',
                                                                             TrigMuonEFCombinerHypoConfig('Muon', '18GeV'),
                                                                             TrigMuonEFTrackIsolationHypoConfig('Muon','EFOnlyMediumWide')))),

    L2EFChain_mu_iso_annulus('mu18ia3_medium',
                             'L2_mu18_medium', 700, 'L1_MU11', 'EF_mu18ia3_medium', 5445,
                             ['MU11'],
                             EFOnlyIsoAnnulusConfig(EFOnlyIsoAnnulusL2Config('_mu18_medium',
                                                                             MufastHypoConfig('Muon', mufastThr_18GeV),
                                                                             MucombHypoConfig('Muon', '18GeV')),
                                                    EFOnlyIsoAnnulusEFConfig('_mu18ia3_medium',
                                                                             TrigMuonEFCombinerHypoConfig('Muon', '18GeV'),
                                                                             TrigMuonEFTrackIsolationHypoConfig('Muon','RelEFOnlyMediumWide')))),

    L2EFChain_mu_iso_annulus('mu18ia4_medium',
                             'L2_mu18_medium', 700, 'L1_MU11', 'EF_mu18ia4_medium', 5446,
                             ['MU11'],
                             EFOnlyIsoAnnulusConfig(EFOnlyIsoAnnulusL2Config('_mu18_medium',
                                                                             MufastHypoConfig('Muon', mufastThr_18GeV),
                                                                             MucombHypoConfig('Muon', '18GeV')),
                                                    EFOnlyIsoAnnulusEFConfig('_mu18ia4_medium',
                                                                             TrigMuonEFCombinerHypoConfig('Muon', '18GeV'),
                                                                             TrigMuonEFTrackIsolationHypoConfig('Muon','EFOnlyTightWide')))),

    L2EFChain_mu_iso_annulus('mu24ia1_tight',
                             'L2_mu24_tight', 107, 'L1_MU15', 'EF_mu24ia1_tight', 5447,
                             ['MU15'],
                             EFOnlyIsoAnnulusConfig(EFOnlyIsoAnnulusL2Config('_mu24_tight',
                                                                             MufastHypoConfig('Muon', mufastThr_24GeV),
                                                                             MucombHypoConfig('Muon', '24GeV')),
                                                    EFOnlyIsoAnnulusEFConfig('_mu24ia1_tight',
                                                                             TrigMuonEFCombinerHypoConfig('Muon', '24GeV'),
                                                                             TrigMuonEFTrackIsolationHypoConfig('Muon','EFOnlyLooseWide')))),

    L2EFChain_mu_iso_annulus('mu24ia2_tight',
                             'L2_mu24_tight', 107, 'L1_MU15', 'EF_mu24ia2_tight', 5448,
                             ['MU15'],
                             EFOnlyIsoAnnulusConfig(EFOnlyIsoAnnulusL2Config('_mu24_tight',
                                                                             MufastHypoConfig('Muon', mufastThr_24GeV),
                                                                             MucombHypoConfig('Muon', '24GeV')),
                                                    EFOnlyIsoAnnulusEFConfig('_mu24ia2_tight',
                                                                             TrigMuonEFCombinerHypoConfig('Muon', '24GeV'),
                                                                             TrigMuonEFTrackIsolationHypoConfig('Muon','EFOnlyMediumWide')))),

    L2EFChain_mu_iso_annulus('mu24ia3_tight',
                             'L2_mu24_tight', 107, 'L1_MU15', 'EF_mu24ia3_tight', 5449,
                             ['MU15'],
                             EFOnlyIsoAnnulusConfig(EFOnlyIsoAnnulusL2Config('_mu24_tight',
                                                                             MufastHypoConfig('Muon', mufastThr_24GeV),
                                                                             MucombHypoConfig('Muon', '24GeV')),
                                                    EFOnlyIsoAnnulusEFConfig('_mu24ia3_tight',
                                                                             TrigMuonEFCombinerHypoConfig('Muon', '24GeV'),
                                                                             TrigMuonEFTrackIsolationHypoConfig('Muon','RelEFOnlyMediumWide')))),
    L2EFChain_mu_iso_annulus('mu24ia4_tight',
                             'L2_mu24_tight', 107, 'L1_MU15', 'EF_mu24ia4_tight', 5450,
                             ['MU15'],
                             EFOnlyIsoAnnulusConfig(EFOnlyIsoAnnulusL2Config('_mu24_tight',
                                                                             MufastHypoConfig('Muon', mufastThr_24GeV),
                                                                             MucombHypoConfig('Muon', '24GeV')),
                                                    EFOnlyIsoAnnulusEFConfig('_mu24ia4_tight',
                                                                             TrigMuonEFCombinerHypoConfig('Muon', '24GeV'),
                                                                             TrigMuonEFTrackIsolationHypoConfig('Muon','EFOnlyTightWide')))),


]

EFOnlyIsoAnnulusConfig   = L2EFChain_mu_MSonly_wOvlpRm_iso_annulus.Config
EFOnlyIsoAnnulusL2Config = L2EFChain_mu_MSonly_wOvlpRm_iso_annulus.L2Config
EFOnlyIsoAnnulusEFConfig = L2EFChain_mu_MSonly_wOvlpRm_iso_annulus.EFConfig

Muons += [

    L2EFChain_mu_MSonly_wOvlpRm_iso_annulus('mu6ia3_MSonly_wOvlpRm', 
                                            'L2_mu6_MSonly_wOvlpRm', 6129, 'L1_MU6', 'EF_mu6ia3_MSonly_wOvlpRm', 6144,
                                            ['MU6'],
                                            EFOnlyIsoAnnulusConfig(EFOnlyIsoAnnulusL2Config('_mu6_MSonly_wOvlpRm', 
                                                                                            MufastHypoConfig('Muon', mufastThr_6GeV), 
                                                                                            None),
                                                                   #EFOnlyIsoAnnulusEFConfig('_mu6ia3_MSonly_wOvlpRm', TrigMuonEFExtrapolatorHypoConfig('Muon','EFOnlyLoose')))),
                                                                   EFOnlyIsoAnnulusEFConfig('_mu6ia3_MSonly_wOvlpRm',
                                                                                            TrigMuonEFExtrapolatorHypoConfig('Muon','6GeV'),
                                                                                            TrigMuonEFTrackIsolationHypoConfig('Muon','RelEFOnlyMediumWide')))),

    L2EFChain_mu_MSonly_wOvlpRm_iso_annulus('mu6ia4_MSonly_wOvlpRm', 
                                            'L2_mu6_MSonly_wOvlpRm', 6129, 'L1_MU6', 'EF_mu6ia4_MSonly_wOvlpRm', 6145,
                                            ['MU6'],
                                            EFOnlyIsoAnnulusConfig(EFOnlyIsoAnnulusL2Config('_mu6_MSonly_wOvlpRm', 
                                                                                            MufastHypoConfig('Muon', mufastThr_6GeV), 
                                                                                            None),
                                                                   #EFOnlyIsoAnnulusEFConfig('_mu6ia4_MSonly_wOvlpRm', TrigMuonEFExtrapolatorHypoConfig('Muon','EFOnlyLoose')))),
                                                                   EFOnlyIsoAnnulusEFConfig('_mu6ia4_MSonly_wOvlpRm',
                                                                                            TrigMuonEFExtrapolatorHypoConfig('Muon','6GeV'),
                                                                                            TrigMuonEFTrackIsolationHypoConfig('Muon','EFOnlyTightWide')))),

    L2EFChain_mu_MSonly_wOvlpRm_iso_annulus('mu6ia1_MSonly_wOvlpRm', 
                                            'L2_mu6_MSonly_wOvlpRm', 6129, 'L1_MU6', 'EF_mu6ia1_MSonly_wOvlpRm', 6142,
                                            ['MU6'],
                                            EFOnlyIsoAnnulusConfig(EFOnlyIsoAnnulusL2Config('_mu6_MSonly_wOvlpRm', 
                                                                                            MufastHypoConfig('Muon', mufastThr_6GeV), 
                                                                                            None),
                                                                   #EFOnlyIsoAnnulusEFConfig('_mu6ia1_MSonly_wOvlpRm', TrigMuonEFExtrapolatorHypoConfig('Muon','EFOnlyLoose')))),
                                                                   EFOnlyIsoAnnulusEFConfig('_mu6ia1_MSonly_wOvlpRm',
                                                                                            TrigMuonEFExtrapolatorHypoConfig('Muon','6GeV'),
                                                                                            TrigMuonEFTrackIsolationHypoConfig('Muon','EFOnlyLooseWide')))),

    L2EFChain_mu_MSonly_wOvlpRm_iso_annulus('mu6ia2_MSonly_wOvlpRm', 
                                            'L2_mu6_MSonly_wOvlpRm', 6129, 'L1_MU6', 'EF_mu6ia2_MSonly_wOvlpRm', 6143,
                                            ['MU6'],
                                            EFOnlyIsoAnnulusConfig(EFOnlyIsoAnnulusL2Config('_mu6_MSonly_wOvlpRm', 
                                                                                            MufastHypoConfig('Muon', mufastThr_6GeV), 
                                                                                            None),
                                                                   #EFOnlyIsoAnnulusEFConfig('_mu6ia2_MSonly_wOvlpRm', TrigMuonEFExtrapolatorHypoConfig('Muon','EFOnlyLoose')))),
                                                                   EFOnlyIsoAnnulusEFConfig('_mu6ia2_MSonly_wOvlpRm',
                                                                                            TrigMuonEFExtrapolatorHypoConfig('Muon','6GeV'),
                                                                                            TrigMuonEFTrackIsolationHypoConfig('Muon','EFOnlyMediumWide')))),
    

    L2EFChain_mu_MSonly_wOvlpRm_iso_annulus('mu10ia1_MSonly_wOvlpRm', 
                                            'L2_mu10_MSonly_wOvlpRm', 6147, 'L1_MU10', 'EF_mu10ia1_MSonly_wOvlpRm', 6147,
                                            ['MU10'],
                                            EFOnlyIsoAnnulusConfig(EFOnlyIsoAnnulusL2Config('_mu10_MSonly_wOvlpRm', 
                                                                                            MufastHypoConfig('Muon', mufastThr_10GeV), 
                                                                                            None),
                                                                   EFOnlyIsoAnnulusEFConfig('_mu10ia1_MSonly_wOvlpRm',
                                                                                            TrigMuonEFExtrapolatorHypoConfig('Muon','10GeV'),
                                                                                            TrigMuonEFTrackIsolationHypoConfig('Muon','EFOnlyLooseWide')))),

    L2EFChain_mu_MSonly_wOvlpRm_iso_annulus('mu10ia2_MSonly_wOvlpRm', 
                                            'L2_mu10_MSonly_wOvlpRm', 6148, 'L1_MU10', 'EF_mu10ia2_MSonly_wOvlpRm', 6148,
                                            ['MU10'],
                                            EFOnlyIsoAnnulusConfig(EFOnlyIsoAnnulusL2Config('_mu10_MSonly_wOvlpRm', 
                                                                                            MufastHypoConfig('Muon', mufastThr_10GeV), 
                                                                                            None),
                                                                   EFOnlyIsoAnnulusEFConfig('_mu10ia2_MSonly_wOvlpRm',
                                                                                            TrigMuonEFExtrapolatorHypoConfig('Muon','10GeV'),
                                                                                            TrigMuonEFTrackIsolationHypoConfig('Muon','EFOnlyMediumWide')))),

    L2EFChain_mu_MSonly_wOvlpRm_iso_annulus('mu10ia3_MSonly_wOvlpRm', 
                                            'L2_mu10_MSonly_wOvlpRm', 6149, 'L1_MU10', 'EF_mu10ia3_MSonly_wOvlpRm', 6149,
                                            ['MU10'],
                                            EFOnlyIsoAnnulusConfig(EFOnlyIsoAnnulusL2Config('_mu10_MSonly_wOvlpRm', 
                                                                                            MufastHypoConfig('Muon', mufastThr_10GeV), 
                                                                                            None),
                                                                   EFOnlyIsoAnnulusEFConfig('_mu10ia3_MSonly_wOvlpRm',
                                                                                            TrigMuonEFExtrapolatorHypoConfig('Muon','10GeV'),
                                                                                            TrigMuonEFTrackIsolationHypoConfig('Muon','RelEFOnlyMediumWide')))),

    L2EFChain_mu_MSonly_wOvlpRm_iso_annulus('mu10ia4_MSonly_wOvlpRm', 
                                            'L2_mu10_MSonly_wOvlpRm', 6150, 'L1_MU10', 'EF_mu10ia4_MSonly_wOvlpRm', 6150,
                                            ['MU10'],
                                            EFOnlyIsoAnnulusConfig(EFOnlyIsoAnnulusL2Config('_mu10_MSonly_wOvlpRm', 
                                                                                            MufastHypoConfig('Muon', mufastThr_10GeV), 
                                                                                            None),
                                                                   EFOnlyIsoAnnulusEFConfig('_mu10ia4_MSonly_wOvlpRm',
                                                                                            TrigMuonEFExtrapolatorHypoConfig('Muon','10GeV'),
                                                                                            TrigMuonEFTrackIsolationHypoConfig('Muon','EFOnlyTightWide')))),    

    L2EFChain_mu_MSonly_wOvlpRm_iso_annulus('mu10Lia1_MSonly_wOvlpRm', 
                                            'L2_mu10L_MSonly_wOvlpRm', 6151, 'L1_MU6', 'EF_mu10Lia1_MSonly_wOvlpRm', 6151,
                                            ['MU6'],
                                            EFOnlyIsoAnnulusConfig(EFOnlyIsoAnnulusL2Config('_mu10L_MSonly_wOvlpRm', 
                                                                                            MufastHypoConfig('Muon', mufastThr_10GeV), 
                                                                                            None),
                                                                   EFOnlyIsoAnnulusEFConfig('_mu10Lia1_MSonly_wOvlpRm',
                                                                                            TrigMuonEFExtrapolatorHypoConfig('Muon','10GeV'),
                                                                                            TrigMuonEFTrackIsolationHypoConfig('Muon','EFOnlyLooseWide')))),

    L2EFChain_mu_MSonly_wOvlpRm_iso_annulus('mu10Lia2_MSonly_wOvlpRm', 
                                            'L2_mu10L_MSonly_wOvlpRm', 6152, 'L1_MU6', 'EF_mu10Lia2_MSonly_wOvlpRm', 6152,
                                            ['MU6'],
                                            EFOnlyIsoAnnulusConfig(EFOnlyIsoAnnulusL2Config('_mu10L_MSonly_wOvlpRm', 
                                                                                            MufastHypoConfig('Muon', mufastThr_10GeV), 
                                                                                            None),
                                                                   EFOnlyIsoAnnulusEFConfig('_mu10Lia2_MSonly_wOvlpRm',
                                                                                            TrigMuonEFExtrapolatorHypoConfig('Muon','10GeV'),
                                                                                            TrigMuonEFTrackIsolationHypoConfig('Muon','EFOnlyMediumWide')))),

    L2EFChain_mu_MSonly_wOvlpRm_iso_annulus('mu10Lia3_MSonly_wOvlpRm', 
                                            'L2_mu10L_MSonly_wOvlpRm', 6153, 'L1_MU6', 'EF_mu10Lia3_MSonly_wOvlpRm', 6153,
                                            ['MU6'],
                                            EFOnlyIsoAnnulusConfig(EFOnlyIsoAnnulusL2Config('_mu10L_MSonly_wOvlpRm', 
                                                                                            MufastHypoConfig('Muon', mufastThr_10GeV), 
                                                                                            None),
                                                                   EFOnlyIsoAnnulusEFConfig('_mu10Lia3_MSonly_wOvlpRm',
                                                                                            TrigMuonEFExtrapolatorHypoConfig('Muon','10GeV'),
                                                                                            TrigMuonEFTrackIsolationHypoConfig('Muon','RelEFOnlyMediumWide')))),

    L2EFChain_mu_MSonly_wOvlpRm_iso_annulus('mu10Lia4_MSonly_wOvlpRm', 
                                            'L2_mu10L_MSonly_wOvlpRm', 6154, 'L1_MU6', 'EF_mu10Lia4_MSonly_wOvlpRm', 6154,
                                            ['MU6'],
                                            EFOnlyIsoAnnulusConfig(EFOnlyIsoAnnulusL2Config('_mu10L_MSonly_wOvlpRm', 
                                                                                            MufastHypoConfig('Muon', mufastThr_10GeV), 
                                                                                            None),
                                                                   EFOnlyIsoAnnulusEFConfig('_mu10Lia4_MSonly_wOvlpRm',
                                                                                            TrigMuonEFExtrapolatorHypoConfig('Muon','10GeV'),
                                                                                            TrigMuonEFTrackIsolationHypoConfig('Muon','EFOnlyTightWide')))),    


    ]





###########################################################################
# TrigL2MuonSA chains
###########################################################################

Config   = L2EFChain_mu_trigL2MuonSA.Config
L2Config = L2EFChain_mu_trigL2MuonSA.L2Config
EFConfig = L2EFChain_mu_trigL2MuonSA.EFConfig

Muons += [

    L2EFChain_mu_trigL2MuonSA('mu18_l2muonSA_medium', 
                              'L2_mu18_l2muonSA_medium', 7137, 'L1_MU11', 'EF_mu18_l2muonSA_medium', 7137,
                              ['MU11'],
                              Config(L2Config('_mu18_l2muonSA_medium',
                                              MufastHypoConfig('Muon', mufastThr_18GeV),
                                              MucombHypoConfig('Muon', '18GeV')), 
                                     EFConfig('_mu18_l2muonSA_medium',
                                              TrigMuonEFCombinerHypoConfig('Muon','18GeV')))),
]

Config   = L2EFChain_mu_trigL2MuonSA_wOvlpRm.Config
L2Config = L2EFChain_mu_trigL2MuonSA_wOvlpRm.L2Config
EFConfig = L2EFChain_mu_trigL2MuonSA_wOvlpRm.EFConfig

Muons += [

    L2EFChain_mu_trigL2MuonSA_wOvlpRm('mu10_l2muonSA_wOvlpRm', 
                                      'L2_mu10_l2muonSA_wOvlpRm', 7936, 'L1_MU10',
                                      'EF_mu10_l2muonSA_wOvlpRm', 7936,
                                      ['MU10'],
                                      Config(L2Config('_mu10_l2muonSA_wOvlpRm',
                                                      MufastHypoConfig('Muon', mufastThr_10GeV),
                                                      MucombHypoConfig('Muon', '10GeV')),
                                             EFConfig('_mu10_l2muonSA_wOvlpRm',
                                                      TrigMuonEFCombinerHypoConfig('Muon', '10GeV')))),

]

Config   = L2EFChain_mu_trigL2MuonSA_900GeV.Config
L2Config = L2EFChain_mu_trigL2MuonSA_900GeV.L2Config
EFConfig = L2EFChain_mu_trigL2MuonSA_900GeV.EFConfig

Muons += [

    L2EFChain_mu_trigL2MuonSA_900GeV('mu4T_l2muonSA', 
                                     'L2_mu4T_l2muonSA', 7736, 'L1_MU4',
                                     'EF_mu4T_l2muonSA', 7736,
                                     ['MU4'],
                                     Config(L2Config('_mu4T_l2muonSA',
                                                     MufastHypoConfig('900GeV', mufastThr_4GeV),
                                                     MucombHypoConfig('900GeV', '4GeV')),
                                            EFConfig('_mu4T_l2muonSA',
                                                     TrigMuonEFCombinerHypoConfig('900GeV', '4GeV')))),

]

###########################################################################
###########################################################################


if __name__ == '__main__':
    triggerPythonConfig = TriggerPythonConfig('hlt.xml', None)
    for m in Muons:
        m.generateMenu(triggerPythonConfig)
    triggerPythonConfig.writeConfigFiles()
