# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################
# SliceDef file for MultiMuon chains
###########################################################################
__author__  = 'R.Mackeprang'
__version__=""
__doc__="Definition of multi muon chains as used in Physics_pp_v1 menu"

from TriggerMenuPython.MultiMuon import *

#                         sig       l1item   l2 chains to merge    l2 counter  ef chains to merge     ef counter
MultiMuons = [ 
               MultiMuon( '2mu4T',  'L1_2MU4',  ['L2_mu4T_wOvlpRm']*2,        '288',     ['EF_mu4T_wOvlpRm']*2,              '288'),
               MultiMuon( '3mu4T',  'L1_3MU4',  ['L2_mu4T_wOvlpRm']*3,        '291',     ['EF_mu4T_wOvlpRm']*3,              '291'),
               MultiMuon( '4mu4T',  'L1_3MU4',  ['L2_mu4T_wOvlpRm']*4,        '292',     ['EF_mu4T_wOvlpRm']*4,              '292'),
               MultiMuon( '2mu6',   'L1_2MU6',  ['L2_mu6_wOvlpRm']*2,         '135',     ['EF_mu6_wOvlpRm']*2,               '135'),
               MultiMuon( '3mu6',   'L1_3MU6',  ['L2_mu6_wOvlpRm']*3,         '137',     ['EF_mu6_wOvlpRm']*3,               '137'),
               MultiMuon( '3mu6_MSonly',          'L1_3MU6',  ['L2_mu6_MSonly_wOvlpRm']*3,  '278',  ['EF_mu6_MSonly_wOvlpRm']*3,  '278'),
               MultiMuon( '3mu6_MSonly_EMPTY',        'L1_2MU4_EMPTY',  ['L2_mu6_MSonly_wOvlpRm']*3,  '279',  ['EF_mu6_MSonly_wOvlpRm']*3,  '279'),
               MultiMuon( '3mu6_MSonly_UNPAIRED_ISO', 'L1_2MU6_UNPAIRED_ISO',  ['L2_mu6_MSonly_wOvlpRm']*3,  '280',  ['EF_mu6_MSonly_wOvlpRm']*3,  '280'),
               MultiMuon( '3mu6_MSonly_noOvlpRm', 'L1_3MU6',  ['L2_mu6_MSonly']*3,          '6290',  ['EF_mu6_MSonly']*3,         '6290'),
               MultiMuon( '3mu6_MSonly_nonfilled','L1_2MU6_EMPTY, L1_2MU6_FIRSTEMPTY, L1_2MU6_UNPAIRED_ISO, L1_2MU6_UNPAIRED_NONISO',\
                          ['L2_mu6_MSonly']*3,  '472',  ['EF_mu6_MSonly']*3, '472'),
               MultiMuon( '2mu10',  'L1_2MU10', ['L2_mu10_wOvlpRm']*2,        '914',     ['EF_mu10_wOvlpRm']*2,              '914'),


               ]


if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus():
   MultiMuons += [
      MultiMuon( '2mu8',                 'L1_2MU6',  ['L2_mu8_wOvlpRm']*2,              '491',  ['EF_mu8_wOvlpRm']*2,              '491'),
      MultiMuon( '2mu13',                'L1_2MU10', ['L2_mu13_wOvlpRm']*2,             '823',  ['EF_mu13_wOvlpRm']*2,             '823'),
      MultiMuon( '2mu13_l2muonSA',       'L1_2MU10', ['L2_mu13_wOvlpRm_l2muonSA']*2,    '822',  ['EF_mu13_wOvlpRm_l2muonSA']*2,    '822'),
      MultiMuon( '2mu13_muFast',         'L1_2MU10', ['L2_mu13_wOvlpRm_muFast']*2,     '7822',  ['EF_mu13_wOvlpRm_muFast']*2,     '7822'),
      MultiMuon( '2mu15',                'L1_2MU10',['L2_mu15_wOvlpRm']*2,              '392',  ['EF_mu15_wOvlpRm']*2,             '392'),
      ]


if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus():
   MultiMuons += [
      MultiMuon( '2mu6ia1_MSonly',          'L1_2MU6',  ['L2_mu6_MSonly_wOvlpRm']*2,  '6045',  ['EF_mu6ia1_MSonly_wOvlpRm']*2,  '6045'),
      MultiMuon( '2mu6ia2_MSonly',          'L1_2MU6',  ['L2_mu6_MSonly_wOvlpRm']*2,  '6046',  ['EF_mu6ia2_MSonly_wOvlpRm']*2,  '6046'),
      MultiMuon( '2mu6ia3_MSonly',          'L1_2MU6',  ['L2_mu6_MSonly_wOvlpRm']*2,  '6047',  ['EF_mu6ia3_MSonly_wOvlpRm']*2,  '6047'),
      MultiMuon( '2mu6ia4_MSonly',          'L1_2MU6',  ['L2_mu6_MSonly_wOvlpRm']*2,  '6048',  ['EF_mu6ia4_MSonly_wOvlpRm']*2,  '6048'),
      MultiMuon( '2mu10ia1_MSonly',         'L1_2MU10', ['L2_mu10_MSonly_wOvlpRm']*2, '6049',  ['EF_mu10ia1_MSonly_wOvlpRm']*2, '6049'),
      MultiMuon( '2mu10ia2_MSonly',         'L1_2MU10', ['L2_mu10_MSonly_wOvlpRm']*2, '6050',  ['EF_mu10ia2_MSonly_wOvlpRm']*2, '6050'),
      MultiMuon( '2mu10ia3_MSonly',         'L1_2MU10', ['L2_mu10_MSonly_wOvlpRm']*2, '6051',  ['EF_mu10ia3_MSonly_wOvlpRm']*2, '6051'),
      MultiMuon( '2mu10ia4_MSonly',         'L1_2MU10', ['L2_mu10_MSonly_wOvlpRm']*2, '6052',  ['EF_mu10ia4_MSonly_wOvlpRm']*2, '6052'),
      MultiMuon( 'mu10_mu6ia1_MSonly',      'L1_2MU6',  ['L2_mu10L_MSonly_wOvlpRm', 'L2_mu6_MSonly_wOvlpRm'], '6053',\
                 ['EF_mu10Lia1_MSonly_wOvlpRm', 'EF_mu6ia1_MSonly_wOvlpRm'],  '6053'),
      MultiMuon( 'mu10_mu6ia2_MSonly',      'L1_2MU6',  ['L2_mu10L_MSonly_wOvlpRm', 'L2_mu6_MSonly_wOvlpRm'], '6054',\
                 ['EF_mu10Lia2_MSonly_wOvlpRm', 'EF_mu6ia2_MSonly_wOvlpRm'],  '6054'),
      MultiMuon( 'mu10_mu6ia3_MSonly',      'L1_2MU6',  ['L2_mu10L_MSonly_wOvlpRm', 'L2_mu6_MSonly_wOvlpRm'], '6055',\
                 ['EF_mu10Lia3_MSonly_wOvlpRm', 'EF_mu6ia3_MSonly_wOvlpRm'],  '6055'),
      MultiMuon( 'mu10_mu6ia4_MSonly',      'L1_2MU6',  ['L2_mu10L_MSonly_wOvlpRm', 'L2_mu6_MSonly_wOvlpRm'], '6056',\
                 ['EF_mu10Lia4_MSonly_wOvlpRm', 'EF_mu6ia4_MSonly_wOvlpRm'],  '6056'),
      MultiMuon( '2mu10_L2StarA',        'L1_2MU10',    ['L2_mu10_L2StarA_wOvlpRm']*2, '271',  ['EF_mu10_L2StarA_wOvlpRm']*2, '271'),
      MultiMuon( '2mu10_L2StarB',        'L1_2MU10',    ['L2_mu10_L2StarB_wOvlpRm']*2, '272',  ['EF_mu10_L2StarB_wOvlpRm']*2, '272'),
      MultiMuon( '2mu10_L2StarC',        'L1_2MU10',    ['L2_mu10_L2StarC_wOvlpRm']*2, '273',  ['EF_mu10_L2StarC_wOvlpRm']*2, '273'),
      ]


if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or \
       TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus():
   MultiMuons += [
      MultiMuon( '2mu6_MSonly',     'L1_2MU6',     ['L2_mu6_MSonly_wOvlpRm']*2,    '6040',  ['EF_mu6_MSonly_wOvlpRm']*2,    '6040'),
      MultiMuon( '2mu10_empty',     'L1_2MU6_EMPTY', ['L2_mu10_empty_wOvlpRm']*2,  '953',   ['EF_mu10_empty_wOvlpRm']*2,    '953'),
      MultiMuon( '2mu10_l2muonSA',  'L1_2MU10',    ['L2_mu10_l2muonSA_wOvlpRm']*2, '7914',  ['EF_mu10_l2muonSA_wOvlpRm']*2, '7914'), 
      ]

   if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU0ThresholdMenus():
      MultiMuons += [
         MultiMuon( '2mu10_loose',          'L1_2MU0',       ['L2_mu10_loose_wOvlpRm']*2,       '274',  ['EF_mu10_loose_wOvlpRm']*2,       '274'),
         MultiMuon( '2mu10_loose_noOvlpRm', 'L1_2MU0',       ['L2_mu10_loose']*2,               '933',  ['EF_mu10_loose']*2,               '933'),
         MultiMuon( '2mu10_loose_empty',    'L1_2MU0_EMPTY', ['L2_mu10_loose_empty_wOvlpRm']*2, '952',  ['EF_mu10_loose_empty_wOvlpRm']*2, '952'),
         ]
   if TriggerFlags.triggerMenuSetup() in Lvl1Flags.MU4ThresholdMenus():
      MultiMuons += [
         MultiMuon( '2mu10_loose',          'L1_2MU4',       ['L2_mu10_loose_wOvlpRm']*2,       '274',  ['EF_mu10_loose_wOvlpRm']*2,       '274'),
         MultiMuon( '2mu10_loose_noOvlpRm', 'L1_2MU4',       ['L2_mu10_loose']*2,               '933',  ['EF_mu10_loose']*2,               '933'),
         MultiMuon( '2mu10_loose_empty',    'L1_2MU4_EMPTY', ['L2_mu10_loose_empty_wOvlpRm']*2, '952',  ['EF_mu10_loose_empty_wOvlpRm']*2, '952'),
         ]


if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus():
   MultiMuons += [
      MultiMuon( '2mu4',           'L1_2MU0',  ['L2_mu4_wOvlpRm']*2, '288',     ['EF_mu4_wOvlpRm']*2,               '288'),
      MultiMuon( '3mu4',           'L1_3MU0',  ['L2_mu4_wOvlpRm']*3, '291',     ['EF_mu4_wOvlpRm']*3,               '291'),
      MultiMuon( '4mu4',           'L1_3MU0',  ['L2_mu4_wOvlpRm']*4, '292',     ['EF_mu4_wOvlpRm']*4,               '292'),
      MultiMuon( '2mu4_MSonly',    'L1_2MU0',  ['L2_mu4_MSonly_wOvlpRm']*2, '886', ['EF_mu4_MSonly_wOvlpRm']*2,     '886'),
      MultiMuon( 'mu4_mu6','L1_2MU0',  ['L2_mu6','L2_mu4'],          '311',     ['EF_mu6','EF_mu4'],                '311'), 
      MultiMuon( '2mu6_NL',        'L1_2MU6_NL', ['L2_mu6_NL']*2,    '993',     ['EF_mu6_NL']*2,                    '993'),
      MultiMuon( '2mu10_loose1',   'L1_2MU6',  ['L2_mu10_loose1_wOvlpRm']*2, '7274', ['EF_mu10_loose1_wOvlpRm']*2,  '7274'),
      MultiMuon( '2mu10_noOvlpRm', 'L1_2MU0',  ['L2_mu10_loose']*2,  '6274',    ['EF_mu10_loose']*2,                '6274'),
      ]




