# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


import TriggerMenu.menu.Physics_pp_v8 as physics_menu

from TriggerJobOpts.TriggerFlags          import TriggerFlags
from copy                                 import deepcopy

import re

from AthenaCommon.Logging import logging
log = logging.getLogger( 'MC_pp_v8.py' )


#---------------------------------------------------------------------
#---------------------------------------------------------------------
def setupMenu():

    physics_menu.setupMenu()
    PhysicsStream="Main"

    ### Remove HLT items that have a remapped L1 threshold and therefore not available in MC
    L1toRemove = []

    for prop in dir(TriggerFlags):
        if prop[-5:]!='Slice': continue
        sliceName=prop
        m_slice=getattr(TriggerFlags,sliceName).signatures()
        for chain in reversed(m_slice):
            if chain[1] in L1toRemove:
                del m_slice[m_slice.index(chain)]

    # stream, BW and RATE tags for Bphysics items that appear in Muon and Bphysics slice.signatures
    BPhysicsStream     = "BphysLS"
    #BMultiMuonStream   = "Main"  
    RATE_BphysTag      = 'RATE:Bphysics'
    #RATE_BMultiMuonTag = 'RATE:MultiMuon'  # can become RATE:BMultiMuon' with one letter change 
    BW_BphysTag        = 'BW:Bphys'
    #RATE_DYTag         = 'RATE:Muon'
    #BW_DYTag           = 'BW:Muon'   
    
    
    # ---------------------------------------------------------------------------------------
    # INPUT FORMAT FOR CHAINS:
    # ['chainName',  'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep], OPTIONAL: [mergingStrategy, offset,[mergingOrder] ]], topoStartsFrom = False
    # ----------------------------------------------------------------------------------------

    TriggerFlags.TestSlice.signatures = TriggerFlags.TestSlice.signatures() + [
			 ]

    TriggerFlags.AFPSlice.signatures = TriggerFlags.AFPSlice.signatures() + [
        ]



    TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
        # ATR-19382
        ['mu10_ivarmedium_mu10',          'L1_2MU10', [], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu10_ivarmedium','mu10']]],
        
        ['mu10_ivarmedium_mu10_20invm60',          'L1_2MU10', [], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu10_ivarmedium', 'mu10_20invm60']]],
        ['mu20_ivarmedium_mu8noL1_20invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu8noL1_20invm60']]],
        ['mu20_ivarmedium_mu6noL1_20invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu6noL1_20invm60']]],
        ['mu20_ivarmedium_mu4noL1_20invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu4noL1_20invm60']]],
        ['mu20_ivarmedium_mu2noL1_20invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu2noL1_20invm60']]],
        
        ['mu10_ivarmedium_mu10_10invm60',          'L1_2MU10', [], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu10_ivarmedium', 'mu10_10invm60']]],
        ['mu20_ivarmedium_mu8noL1_10invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu8noL1_10invm60']]],
        ['mu20_ivarmedium_mu6noL1_10invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu6noL1_10invm60']]],
        ['mu20_ivarmedium_mu4noL1_10invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu4noL1_10invm60']]],
        ['mu20_ivarmedium_mu2noL1_10invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu2noL1_10invm60']]],
        
        ['mu10_ivarmedium_mu10_0invm60',          'L1_2MU10', [], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu10_ivarmedium', 'mu10_0invm60']]],
        ['mu20_ivarmedium_mu8noL1_0invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu8noL1_0invm60']]],
        ['mu20_ivarmedium_mu6noL1_0invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu6noL1_0invm60']]],
        ['mu20_ivarmedium_mu4noL1_0invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu4noL1_0invm60']]],
        ['mu20_ivarmedium_mu2noL1_0invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu2noL1_0invm60']]],
        
        ['mu10_ivarmedium_mu10_20invm80',          'L1_2MU10', [], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu10_ivarmedium', 'mu10_20invm80']]],
        ['mu20_ivarmedium_mu8noL1_20invm80',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu8noL1_20invm80']]],
        ['mu20_ivarmedium_mu6noL1_20invm80',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu6noL1_20invm80']]],
        ['mu20_ivarmedium_mu4noL1_20invm80',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu4noL1_20invm80']]],
        ['mu20_ivarmedium_mu2noL1_20invm80',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu2noL1_20invm80']]],
        
        #ATR-19267
        ['mu15',                   'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu18',                   'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ]


    if TriggerFlags.doFTK():
        TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
            ['mu24_idperf_FTK_L1MU20MU21_FTK',       'L1_MU20MU21_FTK', ['L1_MU20_FTK'], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1], 
            ['mu6_idperf_FTK_L1MU6_FTK',             'L1_MU6_FTK',            [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
            ['mu24_idperf_FTKRefit_L1MU20MU21_FTK',  'L1_MU20MU21_FTK', ['L1_MU20_FTK'], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1], 
            ['mu6_idperf_FTKRefit_L1MU6_FTK',        'L1_MU6_FTK',            [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
            ['mu22_mu8noL1_FTKFS_L1MU20MU21_FTK',    'L1_MU20MU21_FTK', ['L1_MU20_FTK',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','mu8noL1_FTKFS']]],
            ['mu26_ivarmedium_FTK_L1MU20MU21_FTK',   'L1_MU20MU21_FTK', ['L1_MU20_FTK'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
            ['mu26noL1_FTKFS_L1J100_FTK',            'L1_J100_FTK', [], [PhysicsStream],  ['RATE:MuonJet', 'BW:Muon','BW:Jet'], -1],
            ]

    TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + [
        ['j70_j50_0eta490_invm900j50_dphi20_deta40_L1MJJ-400-NFF', 'L1_MJJ-400-NFF', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1], #ATR-19377
        # Large-R jets with jet moment cuts - for testing JetAttrs hypo (Aug. 2018, AS)
        ['j0_a10t_lcw_jes_subj360Iwidth',            'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j0_a10t_lcw_jes_subj360I0width5',          'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j0_a10t_lcw_jes_subj360I0ktdr5',           'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j0_a10t_lcw_jes_subj360Iktdr',             'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j0_a10t_lcw_jes_subj360Iktdr3Iwidth2',     'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1], #inclusive trigger
        ['j0_a10t_lcw_jes_subj360IktdrI9width',      'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],

        #ATR-19271
        ['j0_perf_L1RD0_FILLED',   'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j15',                         'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j380',                        'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j420_a10t_lcw_jes_30smcINF_L1SC111',              'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j440_a10t_lcw_jes_30smcINF_L1SC111',              'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['2j330_a10t_lcw_jes_30smcINF_L1SC111',             'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['6j45_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_0eta240_L14J20',         'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j35_gsc45_boffperf_split_0eta240_L14J20',        'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['ht700_L1HT150-J20.ETA31',     'L1_HT150-J20.ETA31', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['ht700_L1HT150-J20s5.ETA31',   'L1_HT150-J20s5.ETA31', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['ht1000_L1HT190-J15.ETA21',    'L1_HT190-J15.ETA21', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['ht1000_L1HT190-J15s5.ETA21',  'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
			 ]


    if TriggerFlags.doFTK():
        TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + [
        ['j0_perf_ftk_L1RD0_FILLED',   'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j0_perf_ftkrefit_L1RD0_FILLED',   'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j0_gsc0_boffperf_split_L1RD0_FILLED', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j0_gsc0_boffperf_split_FTK_L1RD0_FILLED', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j0_gsc0_boffperf_split_FTKRefit_L1RD0_FILLED', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ]


    TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [
        ['j35_boffperf_split_L1J15',  'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j150_boffperf_split_L1J40', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_boffperf_split_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_bmv2c1050_split_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j150_boffperf_split_j50_boffperf_split_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j150_bmv2c1050_split_j50_bmv2c1050_split_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],


        # ATR-19309
##### Non-FTK version #2b60 x4 L1 seeds 
        ['j55_gsc80_boffperf_j45_gsc55_boffperf_j28_j20_AND_j28_b60_j20_b60_L1V1', 'L1_J45.0ETA20_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1, ['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf","j28","j20_AND","j28_b60","j20_b60_L1V1"]]],
        ['j55_gsc80_boffperf_j45_gsc55_boffperf_j28_j20_AND_j28_b60_j20_b60_L1V2', 'L1_J50_2J40.0ETA25_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf","j28","j20_AND","j28_b60","j20_b60_L1V2"]]],
        ['j55_gsc80_boffperf_j45_gsc55_boffperf_j28_j20_AND_j28_b60_j20_b60_L1V3', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf","j28","j20_AND","j28_b60","j20_b60_L1V3"]]], 
        ['j55_gsc80_boffperf_j45_gsc55_boffperf_j28_j20_AND_j28_b60_j20_b60_L1V4', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf","j28","j20_AND","j28_b60","j20_b60_L1V4"]]], 
        #3b70 x4 L1 seeds
        ['j55_gsc80_boffperf_j45_gsc55_boffperf_j28_j20_AND_j45_gsc55_b70_j28_b70_j20_b70_L1V1', 'L1_J45.0ETA20_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf","j28","j20_AND","j45_gsc55_b70","j28_b70","j20_b70_L1V1"]]], 
        ['j55_gsc80_boffperf_j45_gsc55_boffperf_j28_j20_AND_j45_gsc55_b70_j28_b70_j20_b70_L1V2', 'L1_J50_2J40.0ETA25_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf","j28","j20_AND","j45_gsc55_b70","j28_b70","j20_b70_L1V2"]]], 
        ['j55_gsc80_boffperf_j45_gsc55_boffperf_j28_j20_AND_j45_gsc55_b70_j28_b70_j20_b70_L1V3', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf","j28","j20_AND","j45_gsc55_b70","j28_b70","j20_b70_L1V3"]]], 
        ['j55_gsc80_boffperf_j45_gsc55_boffperf_j28_j20_AND_j45_gsc55_b70_j28_b70_j20_b70_L1V4', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf","j28","j20_AND","j45_gsc55_b70","j28_b70","j20_b70_L1V4"]]], 
        #2b70+2b85 x4 L1 seeds 
        ['j55_gsc80_b85_j45_gsc55_b85_j28_b85_j20_b85_AND_j28_b70_j20_b70_L1V1', 'L1_J45.0ETA20_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b85","j45_gsc55_b85","j28_b85","j20_b85_AND","j28_b70","j20_b70_L1V1"]]], 
        ['j55_gsc80_b85_j45_gsc55_b85_j28_b85_j20_b85_AND_j28_b70_j20_b70_L1V2', 'L1_J50_2J40.0ETA25_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b85","j45_gsc55_b85","j28_b85","j20_b85_AND","j28_b70","j20_b70_L1V2"]]], 
        ['j55_gsc80_b85_j45_gsc55_b85_j28_b85_j20_b85_AND_j28_b70_j20_b70_L1V3', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b85","j45_gsc55_b85","j28_b85","j20_b85_AND","j28_b70","j20_b70_L1V3"]]], 
        ['j55_gsc80_b85_j45_gsc55_b85_j28_b85_j20_b85_AND_j28_b70_j20_b70_L1V4', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b85","j45_gsc55_b85","j28_b85","j20_b85_AND","j28_b70","j20_b70_L1V4"]]], 
        #4b77 x4 L1 seeds 
        ['j55_gsc80_b77_j45_gsc55_b77_j28_b77_j20_b77_L1V1', 'L1_J45.0ETA20_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b77","j45_gsc55_b77","j28_b77","j20_b77_L1V1"]]], 
        ['j55_gsc80_b77_j45_gsc55_b77_j28_b77_j20_b77_L1V2', 'L1_J50_2J40.0ETA25_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b77","j45_gsc55_b77","j28_b77","j20_b77_L1V2"]]], 
        ['j55_gsc80_b77_j45_gsc55_b77_j28_b77_j20_b77_L1V3', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b77","j45_gsc55_b77","j28_b77","j20_b77_L1V3"]]], 
        ['j55_gsc80_b77_j45_gsc55_b77_j28_b77_j20_b77_L1V4', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b77","j45_gsc55_b77","j28_b77","j20_b77_L1V4"]]],
#

        ]

    if TriggerFlags.doFTK():
        TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [
           ['j35_boffperf_split_FTK_L1J15_FTK', 'L1_J15_FTK', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j35_boffperf_split_FTKVtx_L1J15_FTK', 'L1_J15_FTK', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j35_boffperf_split_FTKRefit_L1J15_FTK', 'L1_J15_FTK', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

           ['j150_boffperf_split_FTK_L1J40_FTK', 'L1_J40_FTK', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j150_boffperf_split_FTKVtx_L1J40_FTK', 'L1_J40_FTK', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j150_boffperf_split_FTKRefit_L1J40_FTK', 'L1_J40_FTK', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

           ['j225_boffperf_split_FTK_L1J100_FTK',    'L1_J100_FTK', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j225_boffperf_split_FTKVtx_L1J100_FTK',    'L1_J100_FTK', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j225_boffperf_split_FTKRefit_L1J100_FTK',    'L1_J100_FTK', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

           ['j225_bmv2c1050_split_FTK_L1J100_FTK', 'L1_J100_FTK', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j225_bmv2c1050_split_FTKVtx_L1J100_FTK', 'L1_J100_FTK', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j225_bmv2c1050_split_FTKRefit_L1J100_FTK', 'L1_J100_FTK', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

           ['j150_boffperf_split_j50_boffperf_split_FTK_L1J100_FTK', 'L1_J100_FTK', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
           ['j150_boffperf_split_j50_boffperf_split_FTKVtx_L1J100_FTK', 'L1_J100_FTK', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
           ['j150_boffperf_split_j50_boffperf_split_FTKRefit_L1J100_FTK', 'L1_J100_FTK', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

           ['j150_bmv2c1050_split_j50_bmv2c1050_split_FTK_L1J100_FTK', 'L1_J100_FTK', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
           ['j150_bmv2c1050_split_j50_bmv2c1050_split_FTKVtx_L1J100_FTK', 'L1_J100_FTK', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
           ['j150_bmv2c1050_split_j50_bmv2c1050_split_FTKRefit_L1J100_FTK', 'L1_J100_FTK', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

           ['2j35_boffperf_split_FTK_2j35_L14J15.0ETA25_FTK', 'L1_4J15.0ETA25_FTK', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
           ['2j35_boffperf_split_FTKVtx_2j35_L14J15.0ETA25_FTK', 'L1_4J15.0ETA25_FTK', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
           ['2j35_boffperf_split_FTKRefit_2j35_L14J15.0ETA25_FTK', 'L1_4J15.0ETA25_FTK', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

           ['2j35_bmv2c1050_split_FTK_2j35_L14J15.0ETA25_FTK', 'L1_4J15.0ETA25_FTK', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
           ['2j35_bmv2c1050_split_FTKVtx_2j35_L14J15.0ETA25_FTK', 'L1_4J15.0ETA25_FTK', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
           ['2j35_bmv2c1050_split_FTKRefit_2j35_L14J15.0ETA25_FTK', 'L1_4J15.0ETA25_FTK', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

           #ATR-19309
           ##### FTK-refit version #2b60 x4 L1 seeds 
           ['j55_gsc80_boffperf_j45_gsc55_boffperf_FTKRefit_j28_j20_AND_j28_b60_j20_b60_FTKRefit_L1V1', 'L1_J45.0ETA20_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf_FTKRefit","j28","j20_AND","j28_b60","j20_b60_FTKRefit_L1V1"]]], 
           ['j55_gsc80_boffperf_j45_gsc55_boffperf_FTKRefit_j28_j20_AND_j28_b60_j20_b60_FTKRefit_L1V2', 'L1_J50_2J40.0ETA25_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf_FTKRefit","j28","j20_AND","j28_b60","j20_b60_FTKRefit_L1V2"]]], 
           ['j55_gsc80_boffperf_j45_gsc55_boffperf_FTKRefit_j28_j20_AND_j28_b60_j20_b60_FTKRefit_L1V3', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf_FTKRefit","j28","j20_AND","j28_b60","j20_b60_FTKRefit_L1V3"]]], 
           ['j55_gsc80_boffperf_j45_gsc55_boffperf_FTKRefit_j28_j20_AND_j28_b60_j20_b60_FTKRefit_L1V4', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf_FTKRefit","j28","j20_AND","j28_b60","j20_b60_FTKRefit_L1V4"]]], 
           #3b70 x4 L1 seeds 
           ['j55_gsc80_boffperf_j45_gsc55_boffperf_FTKRefit_j28_j20_AND_j45_gsc55_b70_j28_b70_j20_b70_FTKRefit_L1V1', 'L1_J45.0ETA20_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf_FTKRefit","j28","j20_AND","j45_gsc55_b70","j28_b70","j20_b70_FTKRefit_L1V1"]]], 
           ['j55_gsc80_boffperf_j45_gsc55_boffperf_FTKRefit_j28_j20_AND_j45_gsc55_b70_j28_b70_j20_b70_FTKRefit_L1V2', 'L1_J50_2J40.0ETA25_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf_FTKRefit","j28","j20_AND","j45_gsc55_b70","j28_b70","j20_b70_FTKRefit_L1V2"]]], 
           ['j55_gsc80_boffperf_j45_gsc55_boffperf_FTKRefit_j28_j20_AND_j45_gsc55_b70_j28_b70_j20_b70_FTKRefit_L1V3', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf_FTKRefit","j28","j20_AND","j45_gsc55_b70","j28_b70","j20_b70_FTKRefit_L1V3"]]], 
           ['j55_gsc80_boffperf_j45_gsc55_boffperf_FTKRefit_j28_j20_AND_j45_gsc55_b70_j28_b70_j20_b70_FTKRefit_L1V4', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf_FTKRefit","j28","j20_AND","j45_gsc55_b70","j28_b70","j20_b70_FTKRefit_L1V4"]]], 
           #2b70+2b85 x4 L1 seeds 
           ['j55_gsc80_b85_j45_gsc55_b85_j28_b85_j20_b85_FTKRefit_AND_j28_b70_j20_b70_FTKRefit_L1V1', 'L1_J45.0ETA20_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b85","j45_gsc55_b85","j28_b85","j20_b85_FTKRefit_AND","j28_b70","j20_b70_FTKRefit_L1V1"]]], 
           ['j55_gsc80_b85_j45_gsc55_b85_j28_b85_j20_b85_FTKRefit_AND_j28_b70_j20_b70_FTKRefit_L1V2', 'L1_J50_2J40.0ETA25_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b85","j45_gsc55_b85","j28_b85","j20_b85_FTKRefit_AND","j28_b70","j20_b70_FTKRefit_L1V2"]]], 
           ['j55_gsc80_b85_j45_gsc55_b85_j28_b85_j20_b85_FTKRefit_AND_j28_b70_j20_b70_FTKRefit_L1V3', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b85","j45_gsc55_b85","j28_b85","j20_b85_FTKRefit_AND","j28_b70","j20_b70_FTKRefit_L1V3"]]], 
           ['j55_gsc80_b85_j45_gsc55_b85_j28_b85_j20_b85_FTKRefit_AND_j28_b70_j20_b70_FTKRefit_L1V4', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b85","j45_gsc55_b85","j28_b85","j20_b85_FTKRefit_AND","j28_b70","j20_b70_FTKRefit_L1V4"]]], 
           #4b77 x4 L1 seeds 
           ['j55_gsc80_b77_j45_gsc55_b77_j28_b77_j20_b77_FTKRefit_L1V1', 'L1_J45.0ETA20_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b77","j45_gsc55_b77","j28_b77","j20_b77_FTKRefit_L1V1"]]], 
           ['j55_gsc80_b77_j45_gsc55_b77_j28_b77_j20_b77_FTKRefit_L1V2', 'L1_J50_2J40.0ETA25_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b77","j45_gsc55_b77","j28_b77","j20_b77_FTKRefit_L1V2"]]], 
           ['j55_gsc80_b77_j45_gsc55_b77_j28_b77_j20_b77_FTKRefit_L1V3', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b77","j45_gsc55_b77","j28_b77","j20_b77_FTKRefit_L1V3"]]], 
           ['j55_gsc80_b77_j45_gsc55_b77_j28_b77_j20_b77_FTKRefit_L1V4', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b77","j45_gsc55_b77","j28_b77","j20_b77_FTKRefit_L1V4"]]]

           
           ]


    TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + [

        #ATR-19270
        ['xe100_trktc_lcw_L1XE50', 'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe110_trktc_lcw_L1XE50', 'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_trktc_lcw_L1XE50', 'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_trktc_lcw', '', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_pufittrack_lcw_nojcalib_xe105_pufit_xe65_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe105_pufit_xe65_L1XE50','xe90_pufittrack_lcw_nojcalib']]],
        #['xe0noL1_pufittrack_lcw_nojcalib_xe100_pufit_xe65',   '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1, ['serial',-1,['xe100_pufit','xe65','xe0noL1_pufittrack_lcw_nojcalib']]],
        ['xe0noL1_l2fsperf_pufittrack_lcw_nojcalib', '', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe110_trkmht_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_trkmht', '', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

]

    if TriggerFlags.doFTK():
        TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + [
            ['xe0noL1_l2fsperf_trkmht_FTK', '', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe0noL1_l2fsperf_trktc_lcw_FTK', '', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

            ['xe50_trkmht_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe55_trkmht_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe60_trkmht_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe65_trkmht_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe70_trkmht_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe75_trkmht_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe80_trkmht_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe85_trkmht_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe90_trkmht_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe95_trkmht_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe100_trkmht_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe105_trkmht_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe110_trkmht_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe115_trkmht_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe120_trkmht_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe125_trkmht_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe130_trkmht_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe135_trkmht_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe140_trkmht_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe145_trkmht_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe150_trkmht_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

            ['xe50_trktc_lcw_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe55_trktc_lcw_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe60_trktc_lcw_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe65_trktc_lcw_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe70_trktc_lcw_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe75_trktc_lcw_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe80_trktc_lcw_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe85_trktc_lcw_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe90_trktc_lcw_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe95_trktc_lcw_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe100_trktc_lcw_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe105_trktc_lcw_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe110_trktc_lcw_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe115_trktc_lcw_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe120_trktc_lcw_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe125_trktc_lcw_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe130_trktc_lcw_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe135_trktc_lcw_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe140_trktc_lcw_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe145_trktc_lcw_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            ['xe150_trktc_lcw_FTK_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
]

       

    TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [
        # ATR-19359
        ['tau20_mediumRNN_tracktwoMVA_L1TAU5',                   'L1_TAU5', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau20_mediumRNN_tracktwoMVA_L1TAU8',                   'L1_TAU8', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_mediumRNN_tracktwoMVA_L1TAU5',                   'L1_TAU5', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_mediumRNN_tracktwoMVA_L1TAU8',                   'L1_TAU8', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],

        #ATR-19269
        ['tau0_perf_ptonly_L1TAU12',               'L1_TAU12', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau0_perf_ptonly_L1TAU12IM', 'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau0_perf_ptonly_L1TAU8', 'L1_TAU8', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
			 ]

    if TriggerFlags.doFTK():
            TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [
                ['tau12_idperf_FTK',              'L1_TAU12IM_FTK', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
                ['tau12_perf_FTK',                'L1_TAU12IM_FTK', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
                ['tau12_perf0_FTK',               'L1_TAU12IM_FTK', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
                ['tau12_perf_FTKNoPrec',          'L1_TAU12IM_FTK', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
                ['tau12_perf0_FTKNoPrec',         'L1_TAU12IM_FTK', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
                ['tau12_medium0_FTK',             'L1_TAU12IM_FTK', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
                ['tau12_medium1_FTK',             'L1_TAU12IM_FTK', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
                ['tau12_medium0_FTKNoPrec',       'L1_TAU12IM_FTK', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
                ['tau12_medium1_FTKNoPrec',       'L1_TAU12IM_FTK', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],

                # standard single tau chains seeded from L1 FTK tau seeds for monitoring in CPS
                ['tau25_idperf_tracktwo_L1TAU12IM_FTK',              'L1_TAU12IM_FTK', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
                ['tau25_perf_tracktwo_L1TAU12IM_FTK',                'L1_TAU12IM_FTK', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
                ['tau25_medium1_tracktwo_L1TAU12IM_FTK',             'L1_TAU12IM_FTK', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],

                           ]


    TriggerFlags.EgammaSlice.signatures = TriggerFlags.EgammaSlice.signatures() + [

        #ATR-19268
        ['g20_tight_icaloloose',             'L1_EM18VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['e10_lhmedium_nod0_ivarloose', 'L1_EM8VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_lhvloose_nod0_j70_j50_0eta490_invm1000j50_dphi28_xe50_pufit_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_EM3', '', '',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["e5_lhvloose_nod0","j70","j50_0eta490","xe50_pufit"]]],
        ['e5_lhvloose_nod0_j70_j50_0eta490_invm1000j50_xe50_pufit_L1MJJ-700', 'L1_MJJ-700', ['L1_EM3', '', '',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["e5_lhvloose_nod0","j70","j50_0eta490","xe50_pufit"]]],
        ['e5_lhvloose_nod0_j70_j50_0eta490_invm1000j50_dphi28_xe50_pufit_L1MJJ-700', 'L1_MJJ-700', ['L1_EM3', '', '',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["e5_lhvloose_nod0","j70","j50_0eta490","xe50_pufit"]]],
        ['g27_medium_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g45_loose_L1EM15VH', 'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['e17_lhmedium_nod0_L1EM15VHI', 'L1_EM15VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_ivarloose', 'L1_EM20VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
			 ]


    ###########################################################################################################
    #   Bphysics
    ###########################################################################################################
    TriggerFlags.BphysicsSlice.signatures = TriggerFlags.BphysicsSlice.signatures() + [
	
        ]

    if TriggerFlags.doFTK():
        TriggerFlags.BphysicsSlice.signatures = TriggerFlags.BphysicsSlice.signatures() + [
            ['2mu4_bBmumuxv2_Ftk',                'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],

]



    TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures() + [
        # ATR-19359
        # e+tau
        ['e24_lhmedium_nod0_ivarloose_L1EM22VHI_tau20_mediumRNN_tracktwoMVA_L1TAU5',  'L1_EM22VHI', ['L1_EM22VHI', 'L1_TAU5'], [PhysicsStream], ['RATE:ElectronTau', 'BW:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e24_lhmedium_nod0_ivarloose_L1EM22VHI","tau20_mediumRNN_tracktwoMVA_L1TAU5"]]],
        ['e24_lhmedium_nod0_ivarloose_L1EM22VHI_tau20_mediumRNN_tracktwoMVA_L1TAU8',    'L1_EM22VHI', ['L1_EM22VHI', 'L1_TAU8'], [PhysicsStream], ['RATE:ElectronTau', 'BW:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e24_lhmedium_nod0_ivarloose_L1EM22VHI","tau20_mediumRNN_tracktwoMVA_L1TAU8"]]],
        ['e24_lhmedium_nod0_ivarloose_L1EM22VHI_tau20_mediumRNN_tracktwoMVA_L1TAU5_03dRtt',  'L1_EM22VHI', ['L1_EM22VHI', 'L1_TAU5'], [PhysicsStream], ['RATE:ElectronTau', 'BW:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e24_lhmedium_nod0_ivarloose_L1EM22VHI","tau20_mediumRNN_tracktwoMVA_L1TAU5"]]],
        ['e24_lhmedium_nod0_ivarloose_L1EM22VHI_tau20_mediumRNN_tracktwoMVA_L1TAU8_03dRtt',    'L1_EM22VHI', ['L1_EM22VHI', 'L1_TAU8'], [PhysicsStream], ['RATE:ElectronTau', 'BW:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e24_lhmedium_nod0_ivarloose_L1EM22VHI","tau20_mediumRNN_tracktwoMVA_L1TAU8"]]],
        # mu+tau
        ['mu20_tau20_mediumRNN_tracktwoMVA_L1TAU5',  'L1_MU20MU21', ['L1_MU20', 'L1_TAU5'], [PhysicsStream], ['RATE:MuonTau', 'BW:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu20","tau20_mediumRNN_tracktwoMVA_L1TAU5"]]],
        ['mu20_tau20_mediumRNN_tracktwoMVA_L1TAU8',  'L1_MU20MU21', ['L1_MU20', 'L1_TAU8'], [PhysicsStream], ['RATE:MuonTau', 'BW:MuonTau','BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu20","tau20_mediumRNN_tracktwoMVA_L1TAU8"]]],
        ['mu20_ivarloose_tau20_mediumRNN_tracktwoMVA_L1TAU5',  'L1_MU20MU21', ['L1_MU20', 'L1_TAU5'], [PhysicsStream], ['RATE:MuonTau', 'BW:MuonTau','BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu20_ivarloose","tau20_mediumRNN_tracktwoMVA_L1TAU5"]]],
        ['mu20_ivarloose_tau20_mediumRNN_tracktwoMVA_L1TAU8',  'L1_MU20MU21', ['L1_MU20', 'L1_TAU8'], [PhysicsStream], ['RATE:MuonTau', 'BW:MuonTau','BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu20_ivarloose","tau20_mediumRNN_tracktwoMVA_L1TAU8"]]],
        ['mu20_ivarmedium_tau20_mediumRNN_tracktwoMVA_L1TAU5',  'L1_MU20MU21', ['L1_MU20', 'L1_TAU5'], [PhysicsStream], ['RATE:MuonTau', 'BW:MuonTau','BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu20_ivarmedium","tau20_mediumRNN_tracktwoMVA_L1TAU5"]]],
        ['mu20_ivarmedium_tau20_mediumRNN_tracktwoMVA_L1TAU8',  'L1_MU20MU21', ['L1_MU20', 'L1_TAU8'], [PhysicsStream], ['RATE:MuonTau', 'BW:MuonTau','BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu20_ivarmedium","tau20_mediumRNN_tracktwoMVA_L1TAU8"]]],
        ['mu20_tau20_mediumRNN_tracktwoMVA_L1TAU5_03dRtt',  'L1_MU20MU21', ['L1_MU20', 'L1_TAU5'], [PhysicsStream], ['RATE:MuonTau', 'BW:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu20","tau20_mediumRNN_tracktwoMVA_L1TAU5"]]],
        ['mu20_tau20_mediumRNN_tracktwoMVA_L1TAU8_03dRtt',  'L1_MU20MU21', ['L1_MU20', 'L1_TAU8'], [PhysicsStream], ['RATE:MuonTau', 'BW:MuonTau','BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu20","tau20_mediumRNN_tracktwoMVA_L1TAU8"]]],
        ['mu20_ivarloose_tau20_mediumRNN_tracktwoMVA_L1TAU5_03dRtt',  'L1_MU20MU21', ['L1_MU20', 'L1_TAU5'], [PhysicsStream], ['RATE:MuonTau', 'BW:MuonTau','BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu20_ivarloose","tau20_mediumRNN_tracktwoMVA_L1TAU5"]]],
        ['mu20_ivarloose_tau20_mediumRNN_tracktwoMVA_L1TAU8_03dRtt',  'L1_MU20MU21', ['L1_MU20', 'L1_TAU8'], [PhysicsStream], ['RATE:MuonTau', 'BW:MuonTau','BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu20_ivarloose","tau20_mediumRNN_tracktwoMVA_L1TAU8"]]],
        ['mu20_ivarmedium_tau20_mediumRNN_tracktwoMVA_L1TAU5_03dRtt',  'L1_MU20MU21', ['L1_MU20', 'L1_TAU5'], [PhysicsStream], ['RATE:MuonTau', 'BW:MuonTau','BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu20_ivarmedium","tau20_mediumRNN_tracktwoMVA_L1TAU5"]]],
        ['mu20_ivarmedium_tau20_mediumRNN_tracktwoMVA_L1TAU8_03dRtt',  'L1_MU20MU21', ['L1_MU20', 'L1_TAU8'], [PhysicsStream], ['RATE:MuonTau', 'BW:MuonTau','BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu20_ivarmedium","tau20_mediumRNN_tracktwoMVA_L1TAU8"]]],
        # VBF ditau
        ['tau25_mRNN_t2MVA_L1TAU5_tau20_mRNN_t2MVA_L1TAU5_j70_j50_0eta490_invm900j50', 'L1_MJJ-500-NFF', ['L1_TAU5', 'L1_TAU5','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["tau25_mRNN_t2MVA_L1TAU5", "tau20_mRNN_t2MVA_L1TAU5","j70","j50_0eta490_invm900j50"]]],
        ['tau25_mRNN_t2MVA_L1TAU5_tau20_mRNN_t2MVA_L1TAU5_j70_j50_0eta490_invm900j50_03dRtt', 'L1_MJJ-500-NFF', ['L1_TAU5', 'L1_TAU5','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["tau25_mRNN_t2MVA_L1TAU5", "tau20_mRNN_t2MVA_L1TAU5","j70","j50_0eta490_invm900j50"]]],
        ['tau25_mRNN_t2MVA_L1TAU8_tau20_mRNN_t2MVA_L1TAU8_j70_j50_0eta490_invm900j50', 'L1_MJJ-500-NFF', ['L1_TAU8', 'L1_TAU8','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["tau25_mRNN_t2MVA_L1TAU8", "tau20_mRNN_t2MVA_L1TAU8","j70","j50_0eta490_invm900j50"]]],
        ['tau25_mRNN_t2MVA_L1TAU8_tau20_mRNN_t2MVA_L1TAU8_j70_j50_0eta490_invm900j50_03dRtt', 'L1_MJJ-500-NFF', ['L1_TAU8', 'L1_TAU8','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["tau25_mRNN_t2MVA_L1TAU8", "tau20_mRNN_t2MVA_L1TAU8","j70","j50_0eta490_invm900j50"]]],

                   ]


    TriggerFlags.MinBiasSlice.signatures = TriggerFlags.MinBiasSlice.signatures() + []
    TriggerFlags.BeamspotSlice.signatures = TriggerFlags.BeamspotSlice.signatures()+ []
    TriggerFlags.CalibSlice.signatures   = TriggerFlags.CalibSlice.signatures() + []
    TriggerFlags.CosmicSlice.signatures  = TriggerFlags.CosmicSlice.signatures() + []
    TriggerFlags.StreamingSlice.signatures = TriggerFlags.StreamingSlice.signatures() + []
    TriggerFlags.EnhancedBiasSlice.signatures = TriggerFlags.EnhancedBiasSlice.signatures() + []

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
    disablePatterns=[#re.compile('beamspot'),  #all beamspot chains                     
                     ]
    disableEFPatterns=[]                                          
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


######################################################
Prescales = physics_menu.Prescales
#StreamConfig = physics_menu.StreamConfig

######################################################
# DEFAULT MC prescales
######################################################
Prescales.L1Prescales = dict([(ctpid,1) for ctpid in Prescales.L1Prescales])  # setting all L1 prescales to 1

ps_online_list=[]
ps_cosmic_list=[]
ps_calibmon_list=[
    #'idpsl1_L1IDprescaled'
    ]    
ps_eb_list=[]
ps_beamspot_list=[]
ps_fwdphys_list=[]
ps_minb_list=[]

ps_rerun_list = [
      'xe0noL1_l2fsperf',
      'xe0noL1_l2fsperf_mht',
      'xe0noL1_l2fsperf_mht_em',
      'xe0noL1_l2fsperf_pufit',
      'xe0noL1_l2fsperf_tc_em',
      'xe0noL1_l2fsperf_tc_lcw',
      'xe0noL1_l2fsperf_trktc_lcw',
      'xe0noL1_l2fsperf_pufittrack_lcw_nojcalib',
      'xe0noL1_l2fsperf_trkmht',
      ]

if TriggerFlags.doFTK():
    ps_rerun_list = [
      'xe0noL1_l2fsperf_trkmht_FTK',
      'xe0noL1_l2fsperf_trktc_lcw_FTK',
          ]

ps_larnoiseburst_rerun_list = []

ps_perform_list = [
    #'mu4_j15_dr05' ,
    #'mu4_j35_dr05'  ,
    #'mu6_j85_dr05' ,
    #'e5_lhloose_nod0_idperf',

    'j0_a10t_lcw_jes_subj360Iwidth',
    'j0_a10t_lcw_jes_subj360I0width5',
    'j0_a10t_lcw_jes_subj360I0ktdr5',
    'j0_a10t_lcw_jes_subj360Iktdr',
    'j0_a10t_lcw_jes_subj360Iktdr3Iwidth2',
    'j0_a10t_lcw_jes_subj360IktdrI9width',
    'j0_perf_L1RD0_FILLED',
    'j0_perf_ftkrefit_L1RD0_FILLED',
    'j0_gsc0_boffperf_split_L1RD0_FILLED',
    'j0_gsc0_boffperf_split_FTK_L1RD0_FILLED',
    'j0_gsc0_boffperf_split_FTKRefit_L1RD0_FILLED',

    'xe50',
    'tau160_idperf_tracktwo_L1TAU100',
    'tau160_idperf_tracktwoEF_L1TAU100',
    'tau160_idperf_tracktwoMVA_L1TAU100',
    'tau160_perf_tracktwo_L1TAU100',
    'tau160_perf_tracktwoEF_L1TAU100',
    'tau160_perf_tracktwoMVA_L1TAU100',
    'tau125_medium1_tracktwo',
    'tau160_idperf_track_L1TAU100',
    'tau25_idperf_track',
    'tau25_medium1_tracktwo',
    'tau25_perf_tracktwo',
    'tau25_perf_tracktwo_L1TAU12',
    'tau20_mediumRNN_tracktwoMVA_L1TAU5',
    'tau20_mediumRNN_tracktwoMVA_L1TAU8',
    'tau25_mediumRNN_tracktwoMVA_L1TAU5',
    'tau25_mediumRNN_tracktwoMVA_L1TAU8',
    'tau35_medium1_tracktwoMVA_L1TAU12IM',
    'tau80_medium1_tracktwoMVA_L1TAU60',
    'e5_lhmedium_nod0',
    'e5_lhloose_nod0_idperf',
    ]

ps_Jpsi_list = []

ps_ftk_list = []

ps_Bphys_list = []

if TriggerFlags.doFTK():
    ps_ftk_list = [
          'mu6_idperf_FTK_L1MU6_FTK',
          'mu24_idperf_FTK_L1MU20MU21_FTK',
          'mu6_idperf_FTKRefit_L1MU6_FTK',
          'mu24_idperf_FTKRefit_L1MU20MU21_FTK',
          'j35_boffperf_split_FTK_L1J15_FTK',
          'j150_boffperf_split_FTK_L1J40_FTK',
          'j35_boffperf_split_FTKVtx_L1J15_FTK',
          'j150_boffperf_split_FTKVtx_L1J40_FTK',
          'j35_boffperf_split_FTKRefit_L1J15_FTK',
          'j150_boffperf_split_FTKRefit_L1J40_FTK',
          'j225_boffperf_split_FTK_L1J100_FTK',
          'j225_boffperf_split_FTKVtx_L1J100_FTK',
          'j225_boffperf_split_FTKRefit_L1J100_FTK',
          ]       

    ps_streamers_list = []

#-----------------------------------------------------


chain_list=ps_online_list+ps_cosmic_list+ps_minb_list
if not TriggerFlags.doFTK():
    chain_list+=ps_ftk_list

Prescales.HLTPrescales.update(dict(map(None,chain_list,len(chain_list)*[ [-1, 0, -1] ])))

chain_list=ps_larnoiseburst_rerun_list
Prescales.HLTPrescales.update(dict(map(None,chain_list,len(chain_list)*[ [0, 0, 1] ])))
######################################################

######################################################
# No MC prescales
######################################################
Prescales.L1Prescales_no_prescale  = deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_no_prescale = {}
######################################################

######################################################
# LOOSE mc prescales
######################################################
Prescales.L1Prescales_loose_mc_prescale  = deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_loose_mc_prescale = {}

chain_list=ps_online_list+ps_cosmic_list+ps_calibmon_list+ps_beamspot_list
if not TriggerFlags.doFTK():
    chain_list+=ps_ftk_list

Prescales.HLTPrescales_loose_mc_prescale.update(dict(map(None,chain_list,len(chain_list)*[ [-1, 0,-1] ])))

chain_list=ps_rerun_list
Prescales.HLTPrescales_loose_mc_prescale.update(dict(map(None,chain_list,len(chain_list)*[ [0, 0, 1] ])))
######################################################

######################################################
# TIGHTPERF mc prescales for performance
######################################################
Prescales.L1Prescales_tightperf_mc_prescale= deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_tightperf_mc_prescale = deepcopy(Prescales.HLTPrescales_loose_mc_prescale)

chain_list=ps_eb_list+ps_fwdphys_list+ps_minb_list+ps_Bphys_list#+ps_Jpsi_list # enabling JPSI ee again ATR-15162
if not TriggerFlags.doFTK():
    chain_list+=ps_ftk_list

Prescales.HLTPrescales_tightperf_mc_prescale.update(dict(map(None,chain_list,len(chain_list)*[ [-1, 0,-1] ])))
Prescales.L1Prescales_TriggerValidation_mc_prescale= deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_TriggerValidation_mc_prescale = deepcopy(Prescales.HLTPrescales_tightperf_mc_prescale)
######################################################
# TIGHT mc prescales
######################################################
Prescales.L1Prescales_tight_mc_prescale  = deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_tight_mc_prescale = deepcopy(Prescales.HLTPrescales_tightperf_mc_prescale)
#chain_list=ps_streamers_list + ps_perform_list + ps_Bphys_list#+ps_Jpsi_list # enabling JPSI ee again ATR-15162
chain_list=ps_perform_list + ps_Bphys_list#+ps_Jpsi_list # enabling JPSI ee again ATR-15162

#Prescales.HLTPrescales_tight_mc_prescale = deepcopy(Prescales.HLTPrescales_loose_mc_prescale)
#chain_list=ps_eb_list+ps_fwdphys_list+ps_minb_list+ps_ftk_list+ps_perform_list
Prescales.HLTPrescales_tight_mc_prescale.update(dict(map(None,chain_list,len(chain_list)*[ [-1, 0,-1] ])))
Prescales.L1Prescales_BulkMCProd_mc_prescale  = deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_BulkMCProd_mc_prescale = deepcopy(Prescales.HLTPrescales_tight_mc_prescale)
Prescales.L1Prescales_CPSampleProd_mc_prescale  = deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_CPSampleProd_mc_prescale = deepcopy(Prescales.HLTPrescales_tight_mc_prescale)
######################################################

######################################################
Prescales.L1Prescales_upgrade_mc_prescale  = deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_upgrade_mc_prescale = deepcopy(Prescales.HLTPrescales_tight_mc_prescale)
# Note: "upgrade" prescales are set with regular expressions at the end
#       of the setupMenu() function above 
######################################################
