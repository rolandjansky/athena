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
    RATE_BMultiMuonTag = 'RATE:MultiMuon'  # can become RATE:BMultiMuon' with one letter change 
    BW_BphysTag        = 'BW:Bphys'
    #RATE_DYTag         = 'RATE:Muon'
    #BW_DYTag           = 'BW:Muon'   
    
    
    # ---------------------------------------------------------------------------------------
    # INPUT FORMAT FOR CHAINS:
    # ['chainName',  'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep], OPTIONAL: [mergingStrategy, offset,[merginOrder] ]], topoStartsFrom = False
    # ----------------------------------------------------------------------------------------

    TriggerFlags.TestSlice.signatures = TriggerFlags.TestSlice.signatures() + [
			 ]

    TriggerFlags.AFPSlice.signatures = TriggerFlags.AFPSlice.signatures() + [
        ['afp_jetexc_L1J50',  'L1_J50',  [],  [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['afp_jetexc_L1J75',  'L1_J75',  [],  [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['afp_jetexc_L1J100',  'L1_J100',  [],  [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ]



    TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
        ['mu28_ivarmedium',	     'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Primary:20000','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu80',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Primary:20000','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu60',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Primary:20000','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20_ivarmedium_L1MU10', 'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu26',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu22',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream, 'express'], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20_msonly',            'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu14_ivarloose',         'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu6',                    'L1_MU6',            [], [PhysicsStream,'express'], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu6_msonly',             'L1_MU6',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu4',                    'L1_MU4',            [], [PhysicsStream,'express'], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_idperf',            'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
        ['mu20_idperf',            'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
        ['mu40_idperf',            'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
        ['mu10_idperf',            'L1_MU10',           [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
        ['mu6_idperf',             'L1_MU6',            [], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
        ['mu4_idperf',             'L1_MU4',            [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
        ['mu4_nomucomb',                   'L1_MU4',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu6_nomucomb',                   'L1_MU6',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10_nomucomb',                   'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu11_nomucomb',                   'L1_MU11',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_ivarmedium',	     'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream, 'express'], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu26_ivartight',          'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Primary:20000','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu28_ivartight',          'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Primary:20000','RATE:SingleMuon', 'BW:Muon'], -1],
        ['2mu15_L12MU10',          'L1_2MU10',          [], [PhysicsStream], ['Primary:20000','RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu8_msonly',            'L1_3MU6',           [], [PhysicsStream], ['Primary:20000','RATE:MultiMuon', 'BW:Muon'], -1],
        ['mu24_mu8noL1',           'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream, 'express'], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','mu8noL1']]],
        ['mu24_mu10noL1',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','mu10noL1']]],
        ['mu24_mu12noL1' ,         'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','mu12noL1']]],
        ['mu24_2mu4noL1',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','2mu4noL1']]],
        ['mu26_mu8noL1',           'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu26','mu8noL1']]],
        ['mu26_mu10noL1',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu26','mu10noL1']]],
        ['mu28_mu8noL1',           'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu28','mu8noL1']]],
        ['mu22_2mu4noL1',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','2mu4noL1']]],
        ['mu20_2mu2noL1_JpsimumuFS', 'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], [RATE_BMultiMuonTag,'BW:Muon'], -1,['serial',-1,['mu20','2mu2noL1_JpsimumuFS']]],  # OI - who owns these triggers? Bphys?
        ['mu20_2mu4_JpsimumuL2',     'L1_MU20MU21', ['L1_MU20','L1_2MU4'], [PhysicsStream], [RATE_BMultiMuonTag,'BW:Muon'], -1,['serial',-1,['2mu4_JpsimumuL2','mu20']]], # OI - who owns these triggers?
        ['mu22_mu8noL1_mu6noL1',   'L1_MU20MU21',       ['L1_MU20','',''], [PhysicsStream], ['Primary:20000','RATE:MultiMuon', 'BW:Muon'],  -1,['serial',-1,['mu22','mu8noL1','mu6noL1']]],
        ['mu20_msonly_mu15noL1_msonly_nscan05_noComb',   'L1_MU20MU21',   ['L1_MU20','L2_mu20_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu15noL1_msonly_nscan05_noComb']]],
        ['mu20_msonly_mu6noL1_msonly_nscan05', 'L1_MU20MU21',      ['L1_MU20','L2_mu20_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu6noL1_msonly_nscan05']]],
        ['mu10',                   'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10_mgonly_L1LATE-MU10_XE50',         'L1_LATE-MU10_XE50',          [], [PhysicsStream], ['Primary:20000','RATE:SingleMuon', 'BW:Muon'], -1],


        #ATR-19267
        ['mu14',                   'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
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
        ]

    TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + [
        ['j0_perf_ds1_L1J50',      'L1_J50',  [], ['DataScouting_05_Jets'], ['RATE:Jets_DS', 'BW:Jet'], -1],
        ['j0_perf_ds1_L1J40',      'L1_J40',  [], ['DataScouting_05_Jets'], ['RATE:Jets_DS', 'BW:Jet'], -1],
        ['j0_perf_ds1_L1J50_DETA20-J50J',     'L1_J50_DETA20-J50J',  [], ['DataScouting_05_Jets'], ['RATE:Jets_DS', 'BW:Jet'], -1],
        ['j25',                         'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j35',                         'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j45',                         'L1_J15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j45_L1RD0_FILLED',            'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j60',                         'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j60_L1RD0_FILLED',            'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j85',                         'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j110',                        'L1_J30',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j260',                        'L1_J75', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j360',                        'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j400',                        'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j440',                        'L1_J120', [], [PhysicsStream], ['Primary:17000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j450',                        'L1_J120', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j460',                        'L1_J120', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j480',                        'L1_J120', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j500',                        'L1_J120', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j520',                        'L1_J120', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j225_gsc400_boffperf_split',  'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j225_gsc440_boffperf_split',  'L1_J120', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j225_gsc450_boffperf_split',  'L1_J120', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j225_gsc460_boffperf_split',  'L1_J120', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j225_gsc480_boffperf_split',  'L1_J120', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j225_gsc500_boffperf_split',  'L1_J120', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j35_subjes',                  'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_jes',                     'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_sub',                     'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_nojcalib',                'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_lcw',                     'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_lcw_subjes',              'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_lcw_jes',                 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_lcw_sub',                 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_lcw_nojcalib',            'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j450_subjes',                 'L1_J120', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j450_jes',                    'L1_J120', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j450_sub',                    'L1_J120', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j450_nojcalib',               'L1_J120', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j450_lcw',                    'L1_J120', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j450_lcw_subjes',             'L1_J120', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j450_lcw_jes',                'L1_J120', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j450_lcw_sub',                'L1_J120', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j450_lcw_nojcalib',           'L1_J120', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j15_320eta490',               'L1_RD0_FILLED',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j25_320eta490',               'L1_RD0_FILLED',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j35_320eta490',               'L1_RD0_FILLED',  [], [PhysicsStream,'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j45_320eta490',               'L1_J15.31ETA49', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j60_320eta490',               'L1_J20.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j85_320eta490',               'L1_J20.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j175_320eta490',              'L1_J50.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_320eta490_subjes',        'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_320eta490_jes',           'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_320eta490_sub',           'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_320eta490_nojcalib',      'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_320eta490_lcw',           'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_320eta490_lcw_subjes',    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_320eta490_lcw_jes',       'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_320eta490_lcw_sub',       'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_320eta490_lcw_nojcalib',  'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j260_320eta490_subjes',       'L1_J75.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j260_320eta490_jes',          'L1_J75.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j260_320eta490_lcw',          'L1_J75.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j260_320eta490_lcw_subjes',   'L1_J75.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j260_320eta490_lcw_jes',      'L1_J75.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j260_320eta490_lcw_nojcalib', 'L1_J75.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j110_a10_lcw_subjes_L1J30',       'L1_J30', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j175_a10_lcw_subjes_L1J50',       'L1_J50', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j440_a10_lcw_subjes_L1J100',      'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10_lcw_subjes_L1J100',      'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j500_a10_lcw_subjes_L1J100',      'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j520_a10_lcw_subjes_L1J100',      'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j540_a10_lcw_subjes_L1J100',      'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j260_a10_sub_L1J75',              'L1_J75', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j260_a10_nojcalib_L1J75',         'L1_J75', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j260_a10_lcw_sub_L1J75',          'L1_J75', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j260_a10_lcw_nojcalib_L1J75',     'L1_J75', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10_sub_L1J100',             'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10_nojcalib_L1J100',        'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10_lcw_sub_L1J100',         'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10_lcw_nojcalib_L1J100',    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j440_a10_lcw_subjes_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10_lcw_subjes_L1SC111',             'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j500_a10_lcw_subjes_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j520_a10_lcw_subjes_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j540_a10_lcw_subjes_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j260_a10r_L1J75',                 'L1_J75', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j440_a10r_L1J100',                'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10r_L1J100',                'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j500_a10r_L1J100',                'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j520_a10r_L1J100',                'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j540_a10r_L1J100',                'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j440_a10r_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10r_L1SC111',               'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j500_a10r_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j520_a10r_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j540_a10r_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j260_a10t_lcw_jes_L1J75',         'L1_J75', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j420_a10t_lcw_jes_L1J100',        'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10t_lcw_jes_L1J100',        'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j500_a10t_lcw_jes_L1J100',        'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j520_a10t_lcw_jes_L1J100',        'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j540_a10t_lcw_jes_L1J100',        'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j260_a10t_lcw_nojcalib_L1J75',    'L1_J75', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10t_lcw_nojcalib_L1J100',   'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10t_lcw_jes_L1SC111',               'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j500_a10t_lcw_jes_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j520_a10t_lcw_jes_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j540_a10t_lcw_jes_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j390_a10t_lcw_jes_30smcINF_L1J100',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j420_a10t_lcw_jes_30smcINF_L1J100',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j440_a10t_lcw_jes_30smcINF_L1J100',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j390_a10t_lcw_jes_35smcINF_L1J100',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j440_a10t_lcw_jes_35smcINF_L1J100',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j440_a10t_lcw_jes_35smcINF_L1SC111',              'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j390_a10t_lcw_jes_40smcINF_L1J100',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j420_a10t_lcw_jes_40smcINF_L1J100',               'L1_J100', [], [PhysicsStream], ['Primary:17000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j440_a10t_lcw_jes_40smcINF_L1J100',               'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j390_a10t_lcw_jes_L1J100',                        'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j420_a10t_lcw_jes_L1SC111',                       'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['2j330_a10t_lcw_jes_30smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['2j340_a10t_lcw_jes_35smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['2j340_a10t_lcw_jes_35smcINF_L1SC111',             'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['2j350_a10t_lcw_jes_35smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['2j350_a10t_lcw_jes_35smcINF_L1SC111',             'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['2j360_a10t_lcw_jes_35smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['2j370_a10t_lcw_jes_35smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['2j380_a10t_lcw_jes_35smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['2j330_a10t_lcw_jes_40smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['2j340_a10t_lcw_jes_40smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['2j350_a10t_lcw_jes_40smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j325_a10t_lcw_jes_60smcINF_j325_a10t_lcw_jes_L1SC111',   'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j340_a10t_lcw_jes_60smcINF_j340_a10t_lcw_jes_L1SC111',   'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j370_a10t_lcw_jes_40smcINF_j370_a10t_lcw_jes_L1SC111',   'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j370_a10t_lcw_jes_50smcINF_j370_a10t_lcw_jes_L1SC111',   'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j370_a10t_lcw_jes_60smcINF_j370_a10t_lcw_jes_L1SC111',   'L1_SC111-CJ15', [], [PhysicsStream], ['Primary:17000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j380_a10t_lcw_jes_40smcINF_j380_a10t_lcw_jes_L1SC111',   'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j380_a10t_lcw_jes_50smcINF_j380_a10t_lcw_jes_L1SC111',   'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['2j330_a10t_lcw_jes_L1SC111',                      'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['3j160',                       'L1_J75', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['3j225',                       'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['3j250',                       'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['4j25',                        'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['4j90',                        'L1_3J50', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['4j130',                       'L1_3J50', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['4j85_gsc120_boffperf_split',  'L1_3J50', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['4j85_gsc130_boffperf_split',  'L1_3J50', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j25',                        'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['5j45',                        'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['5j70_L14J15',                 'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['5j70_L14J150ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['5j70',                        'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85',                        'L1_4J20', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j90_L14J150ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j90',                        'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j100_L14J150ETA25',          'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j100',                       'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j90_L14J15',                 'L1_4J15', [], [PhysicsStream], ['Primary:17000', 'RATE:MultiJet',  'BW:Jet'], -1],
        ['5j100_L14J15',                'L1_4J15', [], [PhysicsStream], ['Primary:17000', 'RATE:MultiJet',  'BW:Jet'], -1],
        ['5j50_gsc70_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['5j50_gsc70_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['5j55_gsc75_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['5j55_gsc75_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['5j60_gsc85_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j60_gsc90_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j60_gsc90_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85_gsc100_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85_gsc100_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j60_gsc90_boffperf_split_L14J15',         'L1_4J15', [], [PhysicsStream], ['Primary:17000', 'RATE:MultiJet',  'BW:Jet'], -1],
        ['5j65_0eta240_L14J15',         'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['5j65_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['5j65_0eta240',                'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['5j70_0eta240',                'L1_4J20', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j75_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j75_0eta240',                'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85_0eta240',                'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j75_0eta240_L14J15',         'L1_4J15', [], [PhysicsStream], ['Primary:17000', 'RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85_0eta240_L14J15',         'L1_4J15', [], [PhysicsStream], ['Primary:17000', 'RATE:MultiJet',  'BW:Jet'], -1],
        ['5j50_gsc65_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['5j50_gsc65_boffperf_split_0eta240',               'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['5j50_gsc70_boffperf_split_0eta240',               'L1_4J20', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j55_gsc75_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j55_gsc75_boffperf_split_0eta240',               'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j55_gsc75_boffperf_split_0eta240_L14J15', 'L1_4J15', [], [PhysicsStream], ['Primary:17000', 'RATE:MultiJet',  'BW:Jet'], -1],
        ['6j25',                        'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],        
        ['6j45',                        'L1_4J15', [], [PhysicsStream,'express'], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_L14J15',                 'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_L14J150ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60',                        'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j70',                        'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j85_L14J150ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j85',                        'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j85_L14J15',                 'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc60_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc60_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc60_boffperf_split_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_gsc65_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_gsc65_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_gsc65_boffperf_split_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_gsc70_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_gsc70_boffperf_split_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_gsc85_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_gsc85_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_gsc85_boffperf_split_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_gsc85_boffperf_split_L14J15', 'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_0eta240',                'L1_4J15', [], [PhysicsStream,'express'], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_0eta240_L14J15',         'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_0eta240_L14J20',         'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_0eta240_L15J150ETA25',   'L1_5J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j55_0eta240_L14J20',         'L1_4J20', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j55_0eta240_L15J150ETA25',   'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_0eta240_L14J20',         'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_0eta240_L15J150ETA25',   'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_0eta240_L14J15',              'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j35_gsc45_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j35_gsc50_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j35_gsc50_boffperf_split_0eta240_L14J20',        'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j35_gsc50_boffperf_split_0eta240_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc55_boffperf_split_0eta240_L14J20',        'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc55_boffperf_split_0eta240_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc60_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc60_boffperf_split_0eta240_L14J20',        'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc60_boffperf_split_0eta240_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc60_boffperf_split_0eta240_L14J15', 'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_subjes',                 'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_jes',                    'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_sub',                    'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_nojcalib',               'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_lcw',                    'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_lcw_subjes',             'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_lcw_jes',                'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_lcw_sub',                'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_lcw_nojcalib',           'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['7j25',                        'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],      
        ['7j45_L14J20',                 'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j45',                        'L1_6J15', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j50_L14J150ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j50_L14J20',                 'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j50',                        'L1_6J15', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j50_L14J15',                              'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc45_boffperf_split_L14J20',                'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc45_boffperf_split_L15J150ETA25',          'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_L14J150ETA25',          'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_L14J20',                'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_L15J150ETA25',          'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_L14J15',         'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j45_0eta240_L14J20',         'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j45_0eta240_L15J150ETA25',   'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j45_0eta240',                'L1_6J15', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc45_boffperf_split_0eta240_L14J20',        'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc45_boffperf_split_0eta240_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_0eta240_L14J20',        'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_0eta240_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_0eta240_L14J15', 'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['10j40_L14J20',                'L1_4J20' , [], [PhysicsStream], ['Primary:20000','RATE:MultiJet', 'BW:Jet'], -1],
        ['10j40_L16J15',                'L1_6J15' , [], [PhysicsStream], ['Primary:20000','RATE:MultiJet', 'BW:Jet'], -1],
        ['ht700_L1J75',     'L1_J75', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1], 
        ['ht1200_L1J100',   'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet', 'BW:Jet'], -1],
        ['ht1400_L1J100',   'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet', 'BW:Jet'], -1],
        ['j0_1i2c300m500TLA',               'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j0_0i1c500m900TLA',               'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j0_1i2c200m8000TLA',              'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['2j220_j120',                      'L1_J100', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['2j250_j120',                      'L1_J100', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['2j275_j140',                      'L1_J100', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['j80_0eta240_2j60_320eta490',      'L1_J40.0ETA25_2J15.31ETA49', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['2j225_gsc250_boffperf_split_0eta240_j85_gsc140_boffperf_split_0eta240',      'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['j80_0eta240_j60_j45_320eta490',   'L1_J40.0ETA25_2J25_J20.31ETA49', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF','L1_HT150-J20s5.ETA31_MJJ-400-CF', [], [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,[  'ht300', '2j40_0eta490_invm700']]],
        ['j85_cleanL',          'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j85_cleanT',          'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j85_cleanLLP',        'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j70_j50_0eta490_invm900j50_dphi20_deta40_L1MJJ-400-NFF', 'L1_MJJ-400-NFF', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1], #ATR-19377
        ['j70_j50_0eta490_invm1100j70_dphi20_deta45_L1MJJ-500-NFF', 'L1_MJJ-500-NFF',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
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
        ['j175',                        'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j380',                        'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j110_320eta490',              'L1_J30.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j260_a10_lcw_subjes_L1J75',       'L1_J75', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
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
        ['j55_boffperf_split',  'L1_J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j150_boffperf_split_L1J40', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j320_boffperf_split', 'L1_J85', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j45_gsc55_boffperf_split', 'L1_J15', [], [PhysicsStream,'express'], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j35_gsc45_boffperf_split_L1J15', 'L1_J15', [], [PhysicsStream,'express'], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j85_gsc110_boffperf_split', 'L1_J20', [], [PhysicsStream,'express'], ['RATE:SingleBJet', 'BW:BJet'], -1], 
        ['j175_gsc225_boffperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc275_boffperf_split', 'L1_J75', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_boffperf_split_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_bmv2c1050_split_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc300_boffperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc360_boffperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc275_bmv2c1070_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc300_bmv2c1077_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc360_bmv2c1085_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],          
        ['j225_gsc275_bhmv2c1070_split',        'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc300_bhmv2c1077_split',        'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],         
        ['j225_gsc360_bhmv2c1085_split',        'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],         
        ['j50_gsc65_boffperf_split_3j50_gsc65_boffperf_split',      'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j50_gsc65_bmv2c1040_split_3j50_gsc65_boffperf_split',      'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j55_gsc75_boffperf_split_3j55_gsc75_boffperf_split',        'L1_4J20',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j60_gsc85_boffperf_split_3j60_gsc85_boffperf_split',        'L1_4J20',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j150_gsc175_boffperf_split_j45_gsc60_boffperf_split',   'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j150_gsc175_bmv2c1070_split_j45_gsc60_bmv2c1070_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j150_boffperf_split_j50_boffperf_split_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j150_bmv2c1050_split_j50_bmv2c1050_split_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

        ['j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1077_split_L1J85_3J30', 'L1_J85_3J30', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j110_gsc150_boffperf_split_2j45_gsc55_boffperf_split_L1J85_3J30', 'L1_J85_3J30', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_gsc75_bmv2c1050_split_j55_gsc75_boffperf_split',        'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_gsc75_bmv2c1060_split_j55_gsc75_boffperf_split',        'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j60_gsc85_bmv2c1070_split_j60_gsc85_boffperf_split',        'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j60_gsc85_bmv2c1077_split_j60_gsc85_boffperf_split',        'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j35_boffperf_split_2j35_L14J15.0ETA25',            'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet'], -1],
        ['2j35_bmv2c1040_split_2j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j35_bmv2c1050_split_2j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j35_bmv2c1050_split_3j35', 'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j35_gsc45_bmv2c1070_split_3j35_gsc45_boffperf_split',                 'L1_5J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['3j65_boffperf_split_L13J35.0ETA23',        'L1_3J35.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['3j50_gsc65_boffperf_split_L13J35.0ETA23',  'L1_3J35.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],


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
        ['xe0noL1_l2fsperf',         '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_tc_lcw',  '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_tc_em',   '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_mht',     '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_mht_em',     '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_pufit',   '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe50', 'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe110_pufit_xe70_L1XE50', 'L1_XE50', ['L1_XE50','L1_XE50'], [PhysicsStream], ['Primary:20000','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x70_L1XE50'] ]],
        ['xe110_pufit_xe75_L1XE50', 'L1_XE50', ['L1_XE50','L1_XE50'], [PhysicsStream], ['Primary:20000','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x75_L1XE50'] ]],
        ['xe110_pufit_xe65_L1XE55', 'L1_XE55', ['L1_XE55','L1_XE55'], [PhysicsStream], ['Primary:20000','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x65_L1XE55'] ]],
        ['xe110_pufit_xe70_L1XE55', 'L1_XE55', ['L1_XE55','L1_XE55'], [PhysicsStream], ['Primary:20000','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x70_L1XE55'] ]],
        ['xe110_pufit_xe75_L1XE55', 'L1_XE55', ['L1_XE55','L1_XE55'], [PhysicsStream], ['Primary:20000','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x75_L1XE55'] ]],
        ['xe110_pufit_xe65_L1XE60', 'L1_XE60', ['L1_XE60','L1_XE60'], [PhysicsStream], ['Primary:20000','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x65_L1XE60'] ]],
        ['xe110_pufit_xe70_L1XE60', 'L1_XE60', ['L1_XE60','L1_XE60'], [PhysicsStream], ['Primary:20000','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x70_L1XE60'] ]],
        ['xe110_pufit_xe75_L1XE60', 'L1_XE60', ['L1_XE60','L1_XE60'], [PhysicsStream], ['Primary:20000','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x75_L1XE60'] ]],

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
        ['tau25_idperf_track',                     'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Tau', 'BW:ID'], -1],
        ['tau25_idperf_tracktwo',                  'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Tau', 'BW:ID'], -1],
        ['tau25_perf_tracktwo',                    'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_tracktwo_L1TAU12',            'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_loose1_tracktwo',                  'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_tracktwo',                 'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_tight1_tracktwo',                  'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_tracktwo_L1TAU12',         'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_idperf_tracktwoEF',                      'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_tracktwoEF',                        'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_tracktwoEF',                     'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_idperf_tracktwoEFmvaTES',                'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_tracktwoEFmvaTES',                  'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1NoPt_tracktwoEFmvaTES',           'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_tracktwoEFmvaTES',           'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_idperf_tracktwoMVA',                      'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_tracktwoMVA',                        'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_tracktwoMVA',                        'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_verylooseRNN_tracktwoMVA',                'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_looseRNN_tracktwoMVA',                    'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_mediumRNN_tracktwoMVA',                   'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_tightRNN_tracktwoMVA',                    'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_verylooseRNN_tracktwo',         'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_looseRNN_tracktwo',             'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_mediumRNN_tracktwo',            'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_tightRNN_tracktwo',             'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_tracktwoMVA_L1TAU12IM',                        'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_tracktwoMVA',                        'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau60_medium1_tracktwoMVA',                        'L1_TAU40', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_medium1_tracktwoMVA_L1TAU60',                        'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_mediumRNN_tracktwoMVA_L1TAU12IM',                        'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_mediumRNN_tracktwoMVA',                        'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau60_mediumRNN_tracktwoMVA',                        'L1_TAU40', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_mediumRNN_tracktwoMVA_L1TAU60',                        'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_loose1_tracktwo',                  'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_tracktwo',                 'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau50_medium1_tracktwo_L1TAU12',         'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau60_medium1_tracktwo',                 'L1_TAU40', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_medium1_tracktwo',                 'L1_TAU40', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_medium1_tracktwo_L1TAU60',         'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_medium1_tracktwo',                'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_medium1_tracktwo',                'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_idperf_track_L1TAU100',           'L1_TAU100', [], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Tau', 'BW:ID'], -1],
        ['tau200_medium1_tracktwo_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['Primary:20000','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau200_medium1_tracktwoEF_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['Primary:20000','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau200_mediumRNN_tracktwoMVA_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['Primary:20000','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_idperf_tracktwo_L1TAU100',       'L1_TAU100', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_perf_tracktwo_L1TAU100',       'L1_TAU100', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_idperf_tracktwoEF_L1TAU100',       'L1_TAU100', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_perf_tracktwoEF_L1TAU100',       'L1_TAU100', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_idperf_tracktwoMVA_L1TAU100',       'L1_TAU100', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_perf_tracktwoMVA_L1TAU100',       'L1_TAU100', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_perf_tracktwo_tau25_perf_tracktwo', 'L1_TAU20IM_2TAU12IM' , ['L1_TAU20IM','L1_TAU12IM'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
        ['tau35_loose1_tracktwo_tau25_loose1_tracktwo',   'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_loose1_tracktwo","tau25_loose1_tracktwo"]]],
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo',   'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream, 'express'], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
        ['tau35_medium1_tracktwoEF_tau25_medium1_tracktwoEF',   'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwoEF","tau25_medium1_tracktwoEF"]]],
        ['tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA',   'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_mediumRNN_tracktwoMVA","tau25_mediumRNN_tracktwoMVA"]]],
        ['tau35_tight1_tracktwo_tau25_tight1_tracktwo',   'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwo","tau25_tight1_tracktwo"]]],
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM',   'L1_TAU20IM_2TAU12IM',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
        ['tau35_tight1_tracktwo_tau25_tight1_tracktwo_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwo","tau25_tight1_tracktwo"]]],
        ['tau35_tight1_tracktwo_tau25_tight1_tracktwo_03dR30',   'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream, 'express'], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwo","tau25_tight1_tracktwo"]]],
        ['tau35_tight1_tracktwo_tau25_tight1_tracktwo_03dR30_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwo","tau25_tight1_tracktwo"]]],
        ['tau80_medium1_tracktwo_L1TAU60_tau50_medium1_tracktwo_L1TAU12', 'L1_TAU60',['L1_TAU60','L1_TAU12'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_medium1_tracktwo_L1TAU60","tau50_medium1_tracktwo_L1TAU12"]]],
        ['tau125_medium1_tracktwo_tau50_medium1_tracktwo_L1TAU12', 'L1_TAU60',['L1_TAU60','L1_TAU12'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau125_medium1_tracktwo","tau50_medium1_tracktwo_L1TAU12"]]],
        ['tau35_perf_tracktwo_tau25_perf_tracktwo_ditauM', 'L1_TAU20IM_2TAU12IM' , ['L1_TAU20IM','L1_TAU12IM'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
        ['tau25_singlepion_tracktwo',                    'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_dikaonmass_tracktwo',                    'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_dipion1_tracktwo',               'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_dipion2_tracktwo',               'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_kaonpi1_tracktwo',               'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_kaonpi2_tracktwo',               'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_tracktwo_L1TAU12IM',                  'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_tracktwoEF_L1TAU12IM',                  'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_tracktwoEF',                  'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau60_medium1_tracktwoEF',                  'L1_TAU40', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_medium1_tracktwoEF_L1TAU60',                  'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_tightRNN_tracktwoMVA_L1TAU60',                  'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau60_tightRNN_tracktwoMVA',                  'L1_TAU40', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau40_tightRNN_tracktwoMVA',                  'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_tightRNN_tracktwoMVA',                  'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_tightRNN_tracktwoMVA_L1TAU12IM',                  'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_tightRNN_tracktwoMVA_L1TAU12',                  'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_tight1_tracktwoEF_tau25_tight1_tracktwoEF_03dR30_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwoEF","tau25_tight1_tracktwoEF"]]],
        ['tau35_tight1_tracktwoEF_tau25_tight1_tracktwoEF_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwoEF","tau25_tight1_tracktwoEF"]]],

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
        ['e26_lhtight_idperf',        'L1_EM22VHI',    [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
        ['e28_lhtight_idperf',        'L1_EM24VHI',    [], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
        ['e28_lhtight_idperf_L1EM24VHIM',        'L1_EM24VHIM',    [], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
        ['g10_loose',                            'L1_EM7',  [], [PhysicsStream,'express'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_loose_L1EM15',                     'L1_EM15', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g10_etcut',                     'L1_EM7',   [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_etcut_L1EM12',              'L1_EM12',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g200_etcut',                    'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1], 
        ['g250_etcut',                    'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1], 
        ['g300_etcut_L1EM24VHI',          'L1_EM24VHI', [], [PhysicsStream], ['Primary:20000','RATE:SinglePhoton', 'BW:Egamma'],-1], 
        ['g300_etcut_L1EM24VHIM',          'L1_EM24VHIM', [], [PhysicsStream], ['Primary:20000','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g22_tight',                             'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g25_medium_L1EM22VHI', 'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g25_medium_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g25_medium_L1EM24VHIM', 'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_medium_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_loose_L1EM22VHI', 'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g45_tight_L1EM22VHI', 'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_loose_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['e140_etcut',  'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e140_etcut_L1EM24VHIM',  'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e140_lhvloose_nod0', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
	['e160_etcut',  'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e160_etcut_L1EM24VHIM',  'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e180_etcut',  'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e180_etcut_L1EM24VHIM',  'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g0_hiptrt_L1EM24VHI',                 'L1_EM24VHI', [], [PhysicsStream], ['Primary:20000','RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g0_hiptrt_L1EM24VHIM',                'L1_EM24VHIM', [], [PhysicsStream], ['Primary:20000','RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g15_loose_L1EM7',               'L1_EM7',   [], [PhysicsStream,'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g20_loose_L1EM12',              'L1_EM12',  [], [PhysicsStream,'express'], ['RATE:SinglePhoton', 'BW:Egamma'], -1], # pass through
        ['g40_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g45_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g50_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g80_loose',                     'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g100_loose',                    'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g25_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream,'express'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream,'express'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g60_loose',                     'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g70_loose',                     'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g6_tight_icalotight',                            'L1_EM3',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g10_medium',                    'L1_EM7',   [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g15_loose_L1EM3',               'L1_EM3',   [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_loose',                     'L1_EM15VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g40_tight',                     'L1_EM20VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g45_tight',                     'L1_EM20VHI',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g50_loose',                     'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g60_loose_L1EM15VH',            'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
	['g25_tight_L1EM20VH',            'L1_EM20VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
	['g15_loose_L1EM8VH',             'L1_EM8VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['2g20_tight_icalotight_L12EM15VHI',            'L1_2EM15VHI', [], [PhysicsStream], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g22_tight_icalovloose_L12EM15VHI',           'L1_2EM15VHI', [], [PhysicsStream], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g22_tight_icalovloose',           'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g22_tight_icalotight_L12EM15VHI',            'L1_2EM15VHI', [], [PhysicsStream], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g22_tight_icalotight',            'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g50_loose',                              'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g60_loose_L12EM15VH',                    'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],  
        ['2g60_loose_L12EM20VH',                    'L1_2EM20VH', [], [PhysicsStream], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['g50_loose_L1EM20VH',                     'L1_EM20VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['g60_loose_L1EM20VH',                     'L1_EM20VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g22_tight',                              'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['3g20_loose',                              'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g25_loose_g20_loose',                    'L1_2EM20VH', [], [PhysicsStream], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['3g25_loose',                    'L1_2EM20VH', [], [PhysicsStream], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g20_loose_L12EM15',                      'L1_2EM15', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g20_loose',                   'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g10_loose',                           'L1_2EM7', [], [PhysicsStream,'express'], ['RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['e17_lhloose_cutd0dphideta_L1EM15',     'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhloose_nod0_L1EM15',              'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhloose_nodeta_L1EM15',            'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhloose_nodphires_L1EM15',         'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhvloose_nod0_L1EM22VH',                'L1_EM22VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhvloose_nod0_L1EM22VH',                'L1_EM22VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhmedium_nod0_L1EM22VH',                'L1_EM22VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhmedium_nod0_L1EM22VH',                'L1_EM22VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e9_lhvloose_nod0',                          'L1_EM3',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e9_lhvloose_nod0_L1EM7',                    'L1_EM7',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e9_lhvloose_nod0_L1EM8VH',                  'L1_EM8VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_nod0_L1EM24VHIM_e15_etcut_L1EM7_Zee', 'L1_EM24VHIM',  ['L1_EM24VHIM','L1_EM7'], [PhysicsStream], ['Primary:20000','RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e5_lhtight_nod0_e4_etcut',         'L1_2EM3',      [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e5_lhtight_nod0_e4_etcut_Jpsiee',  'L1_2EM3',      [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e9_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM7',   'L1_JPSI-1M5-EM7',  ['L1_EM7','L1_EM3'],  [PhysicsStream, 'express'], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e5_lhtight_nod0_e9_etcut_Jpsiee_L1JPSI-1M5-EM7',   'L1_JPSI-1M5-EM7',  ['L1_EM3','L1_EM7'],  [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e14_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM12', 'L1_JPSI-1M5-EM12', ['L1_EM12','L1_EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e5_lhtight_nod0_e14_etcut_Jpsiee_L1JPSI-1M5-EM12', 'L1_JPSI-1M5-EM12', ['L1_EM3','L1_EM12'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e5_lhloose_idperf',                   'L1_EM3',       [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
        ['e5_lhmedium_nod0',  'L1_EM3', ['L1_EM3','',''], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_lhtight_idperf',                     'L1_EM3',       [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
        ['e10_lhtight_idperf',                     'L1_EM7',       [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
        ['e24_lhmedium_idperf_L1EM20VH',        'L1_EM20VH',    [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
        ['e17_lhmedium_nod0_L1EM15',                    'L1_EM15',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_L1EM18VHI',                'L1_EM18VHI',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g140_tight',        'L1_EM24VHI', [], [PhysicsStream], ['Primary:20000','RATE:SinglePhoton','BW:Egamma'],-1],
        ['g140_tight_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Primary:20000','RATE:SinglePhoton','BW:Egamma'],-1],
        ['g160_loose',        'L1_EM24VHI', [], [PhysicsStream], ['Primary:17000','RATE:SinglePhoton','BW:Egamma'],-1],
        ['g180_loose',        'L1_EM24VHI', [], [PhysicsStream], ['Primary:17000','RATE:SinglePhoton','BW:Egamma'],-1],
        ['g200_loose',        'L1_EM24VHI', [], [PhysicsStream, 'express'], ['Primary:20000','RATE:SinglePhoton','BW:Egamma'],-1],
        ['g200_loose_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Primary:20000','RATE:SinglePhoton','BW:Egamma'],-1],
        ['2e17_lhloose_nod0_L12EM15',    									'L1_2EM15',     [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1 ],
        ['e5_lhloose_nod0',             									'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e5_lhloose_nod0_idperf',       									'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e10_lhvloose_nod0_L1EM7',       								'L1_EM7',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e15_lhvloose_nod0_L1EM7',       								'L1_EM7',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e20_lhvloose_nod0_L1EM12',       								'L1_EM12',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e25_lhvloose_nod0_L1EM15',  										'L1_EM15',	    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e30_lhvloose_nod0_L1EM15',  										'L1_EM15',	    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e40_lhvloose_nod0_L1EM15',  										'L1_EM15',	    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e50_lhvloose_nod0_L1EM15',  										'L1_EM15',	    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e70_lhvloose_nod0',  														'L1_EM24VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e80_lhvloose_nod0',  														'L1_EM24VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e100_lhvloose_nod0', 														'L1_EM24VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e120_lhvloose_nod0', 														'L1_EM24VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e80_lhmedium_nod0',              'L1_EM22VHI',   [], [PhysicsStream], ['Primary:17000','RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e80_lhmedium_nod0_L1EM24VHI',    'L1_EM24VHI',   [], [PhysicsStream], ['Primary:20000','RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e5_etcut',                             'L1_EM3',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e10_etcut_L1EM7',                      'L1_EM7',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e15_etcut_L1EM7',                      'L1_EM7',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e20_etcut_L1EM12',                     'L1_EM12',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],         
        ['e25_etcut_L1EM15',                     'L1_EM15',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e30_etcut_L1EM15',                     'L1_EM15',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e40_etcut_L1EM15',                     'L1_EM15',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e50_etcut_L1EM15',                     'L1_EM15',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e60_etcut',                            'L1_EM24VHI',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e70_etcut',                            'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e80_etcut',                            'L1_EM24VHI',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e100_etcut',                           'L1_EM24VHI',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e120_etcut',                           'L1_EM24VHI',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e200_etcut',                           'L1_EM24VHI',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
	['e5_lhvloose_nod0',                         'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e12_lhvloose_nod0_L1EM10VH',               'L1_EM10VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e17_lhvloose_nod0',                        'L1_EM15VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
	['e20_lhvloose_nod0',                        'L1_EM15VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e40_lhvloose_nod0',                       'L1_EM20VH',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e60_lhvloose_nod0',                        'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e24_lhmedium_nod0_L1EM18VHI',            'L1_EM18VHI',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhtight_nod0_L1EM20VH',                 'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
	['e7_lhmedium_nod0',                           'L1_EM3',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],        
	['e9_lhloose_nod0',                           'L1_EM7',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
	['e9_lhmedium_nod0',                          'L1_EM7',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_lhmedium_nod0',                        'L1_EM8VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhmedium_nod0',                         'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhmedium_nod0_ivarloose_L1EM15VHI',     'L1_EM15VHI',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e20_lhmedium_nod0',                         'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_L1EM15VH',                'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e60_lhloose_nod0',                          'L1_EM22VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e70_lhloose_nod0',                          'L1_EM22VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e80_lhloose_nod0',                          'L1_EM22VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e80_lhloose_nod0_L1EM24VHI',                          'L1_EM24VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_lhtight_nod0',                           'L1_EM3',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
				['e5_lhtight_nod0_e4_etcut_Jpsiee_L1RD0_FILLED',       'L1_RD0_FILLED',      ['L1_EM3','L1_EM3'],  [PhysicsStream, 'express'], ['RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e9_lhtight_nod0',                           'L1_EM7',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e14_lhtight_nod0',                          'L1_EM12',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhmedium_nod0',              'L1_EM22VHI',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_lhloose_cutd0dphideta_L1EM10VH',   'L1_EM10VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_lhloose_nod0_L1EM10VH',                 'L1_EM10VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_lhloose_nodeta_L1EM10VH',          'L1_EM10VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_lhloose_nodphires_L1EM10VH',       'L1_EM10VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_lhloose_nod0',                          'L1_EM8VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhloose_cutd0dphideta',            'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g25_loose',                     'L1_EM15VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g25_medium',                    'L1_EM15VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_loose',                     'L1_EM15VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_medium',                    'L1_EM15VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g24_loose',                     'L1_EM20VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['2e19_lhvloose_nod0',         'L1_2EM15VHI', [], [PhysicsStream], ['Primary:20000','RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e24_lhloose_nod0_2e12_lhloose_nod0_L1EM20VH_3EM10VH',   'L1_EM20VH_3EM10VH', ['L1_EM20VH','L1_3EM10VH'], [PhysicsStream], ['Primary:20000','RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_2g12_medium',                         'L1_EM20VH_3EM10VH', ['L1_EM20VH','L1_3EM10VH'], [PhysicsStream], ['Primary:20000','RATE:ElectronPhoton', 'BW:Egamma'],-1,['parallel',-1,[] ]],
        ['g25_medium_L1EM20VH',           'L1_EM20VH',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_medium_L1EM20VH',                  'L1_EM20VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_loose_L1EM20VH',            'L1_EM20VH',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_L1EM15VH_g25_medium',  'L1_2EM15VH', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        ['e20_lhmedium_nod0_g35_loose',            'L1_2EM15VH', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        ['g35_loose_g25_loose_L12EM20VH',            'L1_2EM20VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_L1EM20VH',           'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_nod0',                  'L1_EM24VHI', [], [PhysicsStream], ['Primary:17000','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_nod0_ivarloose',        'L1_EM24VHI', [], [PhysicsStream,'express'], ['Primary:20000','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_nod0_ivarloose_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Primary:20000','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_nod0_ivarloose_L1EM22VHI',  'L1_EM22VHI', [], [PhysicsStream], ['Primary:17000','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e250_etcut',                           'L1_EM24VHI',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e300_etcut_L1EM24VHIM',                 'L1_EM24VHIM',  [], [PhysicsStream], ['Primary:20000','RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e60_lhmedium_nod0_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['Primary:20000','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e60_lhmedium_nod0_L1EM24VHIM', 'L1_EM24VHIM', [], [PhysicsStream], ['Primary:20000','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e140_lhloose_nod0_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['Primary:20000','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e140_lhloose_nod0_L1EM24VHIM', 'L1_EM24VHIM', [], [PhysicsStream], ['Primary:20000','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e32_lhtight_nod0_ivarloose',        'L1_EM24VHI', [], [PhysicsStream], ['Primary:20000','RATE:SingleElectron', 'BW:Egamma'],-1],  
        ['e24_lhvloose_nod0_L1EM20VH',                'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhvloose_nod0',        'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhmedium_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e80_lhmedium_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Primary:20000','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhvloose_nod0_L1EM15VHI',        'L1_EM15VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e19_lhvloose_nod0',        'L1_EM15VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g20_tight_L1EM15VHI',                     'L1_EM15VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_tight_icalovloose_L1EM15VHI',                     'L1_EM15VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_tight_icalotight_L1EM15VHI',                     'L1_EM15VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g22_tight_L1EM15VHI',                     'L1_EM15VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g22_tight_icalovloose_L1EM15VHI',                     'L1_EM15VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g22_tight_icalovloose',            'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g22_tight_icalotight_L1EM15VHI',                     'L1_EM15VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g22_tight_icalotight',             'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g25_loose_L1EM20VH',                     'L1_EM20VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['e24_lhloose_nod0_L1EM20VH',        'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g12_loose',                     'L1_EM10VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g12_medium',                     'L1_EM10VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['e60_lhloose_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e70_lhloose_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e80_lhloose_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g70_loose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g80_loose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g80_loose_icalovloose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['e60_etcut_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e70_etcut_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e80_etcut_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e100_etcut_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e120_etcut_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e200_etcut_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e250_etcut_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e70_lhvloose_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e80_lhvloose_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e100_lhvloose_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e120_lhvloose_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e140_lhvloose_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e160_lhvloose_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e160_lhvloose_nod0',        'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g60_loose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g100_loose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g120_loose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g140_loose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['Primary:17000','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g160_loose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['Primary:17000','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g180_loose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['Primary:17000','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['e20_lhmedium_nod0_L1EM18VHI',        'L1_EM18VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e20_lhtight_nod0_ivarloose_L1EM18VHI',        'L1_EM18VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g35_loose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_tight_icalotight_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_tight_icalotight_L1EM24VHI',                     'L1_EM24VHI',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g40_tight_icalotight_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g40_tight_icalotight_L1EM24VHI',                      'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g85_tight_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g85_tight_icalovloose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g85_tight_icalovloose_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g100_tight_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g100_tight_icalovloose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g100_tight_icalovloose_L1EM24VHI',                     'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['e25_mergedtight_ivarloose',           'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e25_mergedtight',                      'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e30_mergedtight',                      'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['g35_medium_icalovloose',           'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g35_medium_icalotight',           'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_lhvloose_nod0_bBeexM6000t',  '',   ['L1_EM3'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag],-1],
        ['2e5_lhvloose_nod0_bBeexM6000t',  '',   ['L1_EM3','L1_EM3'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag],-1],

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
        ['mu11_mu6_bNocut_L1LFV-MU11',                'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bDimu_noinvm_novtx_ss_L1LFV-MU11', 'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bDimu_novtx_noos_L1LFV-MU11',      'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['3mu4_bDimu2700',                'L1_3MU4', [], [BPhysicsStream],             [RATE_BphysTag,BW_BphysTag], -1],
        ['mu6_mu4_bDimu',                 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu6_bDimu',                    'L1_2MU6', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1], 
        ['mu10_bJpsi_TrkPEBmon',        'L1_MU10',  [], [BPhysicsStream,'express'], [RATE_BphysTag,BW_BphysTag], -1],
        ['3mu6_bJpsi',                   'L1_3MU6', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['3mu6_bTau',                    'L1_3MU6', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['3mu6_bUpsi',                   'L1_3MU6', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu6_2mu4_bDimu6000', 'L1_MU6_3MU4', ['L1_MU6','L1_3MU4'], [BPhysicsStream], [RATE_BphysTag, BW_BphysTag], -1],
        ['mu4_mu4_idperf_bJpsimumu_noid',  'L1_2MU4', [], [PhysicsStream, 'express'], ['RATE:IDMonitoring',BW_BphysTag, 'BW:ID'], -1],  
        ['2mu6_bBmumuxv2_L1LFV-MU6',              'L1_LFV-MU6',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bPhi_L1LFV-MU6',                   'L1_LFV-MU6',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bDimu_L1LFV-MU6',                  'L1_LFV-MU6',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bDimu_novtx_noos_L1LFV-MU6',       'L1_LFV-MU6',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu4_bBmumu_L1BPH-2M9-2MU4_BPH-0DR15-2MU4',         'L1_BPH-2M9-2MU4_BPH-0DR15-2MU4', ['L1_2MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu4_bBmumu_Lxy0_L1BPH-2M9-2MU4_BPH-0DR15-2MU4',    'L1_BPH-2M9-2MU4_BPH-0DR15-2MU4', ['L1_2MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu4_bJpsimumu_Lxy0_L1BPH-2M9-2MU4_BPH-0DR15-2MU4', 'L1_BPH-2M9-2MU4_BPH-0DR15-2MU4', ['L1_2MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu4_bBmumux_BsmumuPhi_L1BPH-2M9-2MU4_BPH-0DR15-2MU4', 'L1_BPH-2M9-2MU4_BPH-0DR15-2MU4', ['L1_2MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu4_bBmumux_BpmumuKp_L1BPH-2M9-2MU4_BPH-0DR15-2MU4', 'L1_BPH-2M9-2MU4_BPH-0DR15-2MU4', ['L1_2MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bBmumu_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4','L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bBmumu_Lxy0_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4','L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bJpsimumu_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4','L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bJpsimumu_Lxy0_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4','L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream, 'express'], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bBmumux_BsmumuPhi_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4','L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bBmumux_BpmumuKp_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4','L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bDimu_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4','L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bDimu_novtx_noos_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4','L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu20_mu6noL1_bNocut',             'L1_MU20MU21',      ['L1_MU20','L2_mu20'],        [BPhysicsStream], [RATE_BphysTag,'BW:Muon'], -1,['serial',-1,['mu20','mu6noL1']]],  
        ['2mu6_bDimu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',      'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',     ['L1_2MU6'], [BPhysicsStream, 'express'], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bDimu_novtx_noos_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',      'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bBmumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',     'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',     ['L1_2MU6'], [BPhysicsStream, 'express'], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bUpsimumu_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4-BO', 'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4-BO',     ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bDimu_noinvm_novtx_ss', 'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu11_mu6_bDimu_novtx_noos',      'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream, 'express'], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu11_mu6_bNocut',                 'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu11_mu6noL1_bNocut_L1MU11_2MU6',  'L1_MU11_2MU6', ['L1_MU11','L2_mu6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1,['serial',-1,['mu11','mu6noL1']]],
	
        ]

    if TriggerFlags.doFTK():
        TriggerFlags.BphysicsSlice.signatures = TriggerFlags.BphysicsSlice.signatures() + [
            ['2mu4_bBmumuxv2_Ftk',                'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],

]



    TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures() + [
        ['2j45_gsc55_boffperf_split_ht300_L1HT190-J15s5.ETA21',  'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet_combined'], -1,['serial',-1,[ 'ht300', '2j45_gsc55_boffperf_split'  ]]], 
        ['2j45_gsc55_bmv2c1060_split_ht300_L1HT190-J15s5.ETA21', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet_combined'], -1,['serial',-1,[ 'ht300', '2j45_gsc55_bmv2c1060_split' ]]], 
        ['mu4_j45_gsc55_boffperf_split_dr05_dz02', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream, 'express'], ['RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu4', 'j45_gsc55_boffperf_split'] ]],
        ['e5_lhvloose_j70_j50_0eta490_invm1000j50_xe50_pufit_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_EM3','','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["e5_lhvloose","j70","j50_0eta490","xe50_pufit"]]],
        ['e5_lhloose_j70_j50_0eta490_invm1000j50_xe50_pufit_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_EM3','','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["e5_lhloose","j70","j50_0eta490","xe50_pufit"]]],
        ['j70_0eta490_j50_0eta490_2j35_0eta490_invm1200j50_L1MJJ-500-NFF_AND_2j35_bmv2c1070_split', 'L1_MJJ-500-NFF', [], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial', -1,["j70_0eta490","j50_0eta490","2j35_0eta490_invm1200j50_L1MJJ-500-NFF_AND","2j35_bmv2c1070_split"]]],
        ['j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ-500-NFF_AND_2j35_bmv2c1060_split', 'L1_MJJ-500-NFF', [], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial', -1,["j70_0eta490","j50_0eta490","2j35_0eta490_invm1000j50_L1MJJ-500-NFF_AND","2j35_bmv2c1060_split"]]],
        ['e5_lhloose_j70_j50_0eta490_invm1000j50_xe50_pufit_L1MJJ-700', 'L1_MJJ-700', ['L1_EM3','','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["e5_lhloose","j70","j50_0eta490","xe50_pufit"]]],
        ['j70_0eta490_j50_0eta490_2j35_0eta490_invm1200j50_L1MJJ-700_AND_2j35_bmv2c1070_split', 'L1_MJJ-700', [], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial', -1,["j70_0eta490","j50_0eta490","2j35_0eta490_invm1200j50_L1MJJ-700_AND","2j35_bmv2c1070_split"]]],
        ['j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ-700_AND_2j35_bmv2c1060_split', 'L1_MJJ-700', [], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial', -1,["j70_0eta490","j50_0eta490","2j35_0eta490_invm1000j50_L1MJJ-700_AND","2j35_bmv2c1060_split"]]],
        ['e13_etcut_trkcut_xs30_xe30_mt35', 'L1_EM12_XS20', ['L1_EM12','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xs30","xe30"]]],
        ['e13_etcut_trkcut_xs30_j15_perf_xe30_6dphi05_mt35', 'L1_EM12_XS20', ['L1_EM12','','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e13_etcut_trkcut_xs30_j15_perf_xe30_6dphi15_mt35', 'L1_EM12_XS20', ['L1_EM12','','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e13_etcut_trkcut_xs30_j15_perf_xe30_2dphi05_mt35', 'L1_EM12_XS20', ['L1_EM12','','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e13_etcut_trkcut_xs30_j15_perf_xe30_2dphi15_mt35', 'L1_EM12_XS20', ['L1_EM12','','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e18_etcut_trkcut_xs30_xe30_mt35', 'L1_EM15_XS30', ['L1_EM15','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e18_etcut_trkcut","xs30","xe30"]]],
        ['e18_etcut_trkcut_xs30_j15_perf_xe30_6dphi05_mt35', 'L1_EM15_XS30', ['L1_EM15','','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e18_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e18_etcut_trkcut_xs30_j15_perf_xe30_6dphi15_mt35', 'L1_EM15_XS30', ['L1_EM15','','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e18_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e18_etcut_trkcut_xs30_j15_perf_xe30_2dphi05_mt35', 'L1_EM15_XS30', ['L1_EM15','','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e18_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e18_etcut_trkcut_xs30_j15_perf_xe30_2dphi15_mt35', 'L1_EM15_XS30', ['L1_EM15','','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e18_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e18_etcut_trkcut_xs20_j20_perf_xe20_6dphi15_mt35', 'L1_EM15_XS30', ['L1_EM15','','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e18_etcut_trkcut","xs20","j20_perf","xe20"]]],
        ['e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35',      'L1_EM15_XS30', ['L1_EM15','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e18_etcut_trkcut","j20_perf","xe20"]]],
        ['2e12_lhmedium_nod0_mu10', 'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e12_lhmedium_nod0_2mu10', 'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e28_lhmedium_nod0_mu8noL1', 'L1_EM24VHI', ['L1_EM24VHI', ''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['e28_lhmedium_nod0','mu8noL1'] ]],
        ['e28_lhmedium_nod0_L1EM24VHIM_mu8noL1', 'L1_EM24VHIM', ['L1_EM24VHIM', ''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['e28_lhmedium_nod0_L1EM24VHIM','mu8noL1'] ]],
        ['e28_lhmedium_nod0', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1],
        ['e70_lhloose_nod0_L1EM24VHIM_xe70noL1',    'L1_EM24VHIM',['L1_EM24VHIM',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e70_lhloose_nod0_L1EM24VHIM","xe70noL1"]]],
        ['e80_lhloose_nod0_xe80noL1',    'L1_EM22VHI',['L1_EM22VHI',''], [PhysicsStream], ['Primary:17000','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e80_lhloose_nod0","xe80noL1"]]],
        ['e80_lhloose_nod0_L1EM24VHI_xe80noL1',    'L1_EM24VHI',['L1_EM24VHI',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e80_lhloose_nod0_L1EM24VHI","xe80noL1"]]],
        ['e80_lhloose_nod0_L1EM24VHIM_xe80noL1',    'L1_EM24VHIM',['L1_EM24VHIM',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e80_lhloose_nod0_L1EM24VHIM","xe80noL1"]]],
        ['g45_tight_L1EM22VHI_xe45noL1', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g45_tight_L1EM22VHI","xe45noL1"]]],
        ['g45_tight_L1EM24VHI_xe45noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g45_tight_L1EM24VHI","xe45noL1"]]],
        ['g45_tight_L1EM24VHIM_xe45noL1', 'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g45_tight_L1EM24VHIM","xe45noL1"]]],
        ['g45_tight_icalovloose_L1EM24VHIM_xe45noL1', 'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g45_tight_icalovloose_L1EM24VHIM","xe45noL1"]]],
        ['g80_loose_L1EM24VHI_xe80noL1',           'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['Primary:17000','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g80_loose_L1EM24VHI","xe80noL1"]]],
        ['g80_loose_L1EM24VHIM_xe80noL1',           'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['Primary:17000','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g80_loose_L1EM24VHIM","xe80noL1"]]],
        ['g80_loose_icalovloose_L1EM24VHIM_xe80noL1',           'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g80_loose_icalovloose_L1EM24VHIM","xe80noL1"]]],
        ['g45_tight_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1],
        ['g45_tight_icalovloose_L1EM22VHI_xe45noL1', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g45_tight_icalovloose_L1EM22VHI","xe45noL1"]]],
        ['g45_tight_icalovloose_L1EM24VHI_xe45noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g45_tight_icalovloose_L1EM24VHI","xe45noL1"]]],
        ['g80_loose_icalovloose_L1EM24VHI_xe80noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g80_loose_icalovloose_L1EM24VHI","xe80noL1"]]],
        ['g100_tight_icalovloose_L1EM22VHI_3j50noL1', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['Primary:17000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g100_tight_icalovloose_L1EM22VHI","3j50noL1"]]],
        ['g85_tight_icalovloose_L1EM24VHI_3j50noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g85_tight_icalovloose_L1EM24VHI","3j50noL1"]]],
        ['g100_tight_icalovloose_L1EM24VHI_3j50noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g100_tight_icalovloose_L1EM24VHI","3j50noL1"]]],
        ['2mu4_invm1_j20_xe60_pufit_2dphi10_L12MU4_J20_XE30_DPHI-J20s2XE30',  'L1_2MU4_J20_XE30_DPHI-J20s2XE30', [], [PhysicsStream], ['Primary:20000','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["2mu4_invm1","j20","xe60_pufit"] ]],
        ['e5_lhloose_nod0_mu4_j30_xe40_pufit_2dphi10_L1MU4_J30_XE40_DPHI-J20s2XE30',  'L1_MU4_J30_XE40_DPHI-J20s2XE30', ['L1_EM3','','',''], [PhysicsStream], ['Primary:20000','RATE:MuonJetMET', 'BW:Egamma', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["e5_lhloose_nod0","mu4","j30","xe40_pufit"] ]],
        ['e5_lhmedium_nod0_j50_xe80_pufit_2dphi10_L1J40_XE50_DPHI-J20s2XE50',  'L1_J40_XE50_DPHI-J20s2XE50', ['L1_EM3','',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJetMET', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e5_lhmedium_nod0","j50","xe80_pufit"] ]],
        ['e60_etcut_trkcut_j15_perf_xe60_6dphi05_mt35','L1_EM24VHIM', ['L1_EM24VHIM','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e60_etcut_trkcut","j15_perf","xe60"]]],
        ['mu4_j15_boffperf_split_dr05_dz02', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu4', 'j15_boffperf_split'] ]],
        ['mu4_j25_boffperf_split_dr05_dz02', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu4', 'j25_boffperf_split'] ]],
        ['mu4_j35_boffperf_split_dr05_dz99', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu4', 'j35_boffperf_split'] ]],
        ['mu6_j110_gsc150_boffperf_split_dr05_dz99', 'L1_MU6_J40',     ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j110_gsc150_boffperf_split'] ]],
        ['mu6_j225_gsc320_boffperf_split_dr05_dz99', 'L1_MU6_J75',     ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j225_gsc320_boffperf_split'] ]],
        ['mu4_j35_boffperf_split', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu4', 'j35_boffperf_split'] ]],
        ['mu4_j35_boffperf_split_dr05_dz02', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu4', 'j35_boffperf_split'] ]],
        ['mu6_j60_gsc85_boffperf_split_dr05_dz02',  'L1_MU6_J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j60_gsc85_boffperf_split'] ]],
        ['mu6_j85_gsc110_boffperf_split_dr05_dz02', 'L1_MU6_J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j85_gsc110_boffperf_split'] ]],
        ['mu6_j110_gsc150_boffperf_split_dr05_dz02', 'L1_MU6_J40', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j110_gsc150_boffperf_split'] ]],
        ['mu6_j110_gsc150_boffperf_split', 'L1_MU6_J40',     ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j110_gsc150_boffperf_split'] ]],
        ['mu6_j150_gsc175_boffperf_split_dr05_dz02', 'L1_MU6_J40', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j150_gsc175_boffperf_split']]],
        ['mu6_j175_gsc260_boffperf_split_dr05_dz02', 'L1_MU6_J75', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j175_gsc260_boffperf_split'] ]],
        ['mu6_j225_gsc320_boffperf_split_dr05_dz02', 'L1_MU6_J75', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j225_gsc320_boffperf_split'] ]],
        ['mu6_j225_gsc320_boffperf_split', 'L1_MU6_J75',     ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j225_gsc320_boffperf_split'] ]],
        ['mu6_j225_gsc400_boffperf_split_dr05_dz02', 'L1_MU6_J75', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j225_gsc400_boffperf_split'] ]],
        ['mu4_j35_boffperf_split_dr05_dz02_L1BTAG-MU4J15', 'L1_BTAG-MU4J15', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu4', 'j35_boffperf_split'] ], False],
        ['mu6_j60_gsc85_boffperf_split_dr05_dz02_L1BTAG-MU6J20', 'L1_BTAG-MU6J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j60_gsc85_boffperf_split'] ], False],
        ['mu26_ivarmedium_2j35_boffperf_split', 'L1_MU20MU21',    ['L1_MU20',    '', ''], [PhysicsStream], ['RATE:MuonBJet',  'BW:BJet_combined'],  -1, ['serial',-1,['mu26_ivarmedium', '2j35_boffperf_split']]],
        ['e28_lhtight_nod0_ivarloose_L1EM24VHIM_2j35_boffperf_split',        'L1_EM24VHIM', ['L1_EM24VHIM', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet_combined'],  -1, ['serial',-1,['e28_lhtight_nod0_ivarloose_L1EM24VHIM', '2j35_boffperf_split']]],
        ['2g10_medium_mu20',         'L1_MU20MU21',    ['L1_2EM7', 'L1_MU20'],  [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['2g10_medium','mu20'] ]],
        ['j30_jes_PS_llp_L1TAU60',             'L1_TAU60',             [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j30_jes_PS_llp_noiso_L1TAU60',       'L1_TAU60',             [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j30_jes_cleanLLP_PS_llp_L1TAU60',             'L1_TAU60',             [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j30_jes_cleanLLP_PS_llp_pufix_L1TAU60',       'L1_TAU60',             [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j30_jes_cleanLLP_PS_llp_pufix_noiso_L1TAU60',       'L1_TAU60',             [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j30_jes_cleanLLP_PS_llp_noiso_L1TAU60',       'L1_TAU60',             [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j250_jes_cleanLLP_PS_revllp_L1TAU60',             'L1_TAU60',             [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['g35_tight_icalotight_L1EM24VHIM_mu18noL1',        'L1_EM24VHIM', ['L1_EM24VHIM', ''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_tight_icalotight_L1EM24VHIM','mu18noL1'] ]],
        ['g40_tight_icalotight_L1EM24VHIM_mu18noL1',        'L1_EM24VHIM', ['L1_EM24VHIM', ''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g40_tight_icalotight_L1EM24VHIM','mu18noL1'] ]],
        ['g35_tight_icalotight_L1EM24VHIM_mu15noL1_mu2noL1', 'L1_EM24VHIM', ['L1_EM24VHIM', '',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_tight_icalotight_L1EM24VHIM','mu15noL1','mu2noL1']]],
        ['g40_tight_icalotight_L1EM24VHIM_mu15noL1_mu2noL1', 'L1_EM24VHIM', ['L1_EM24VHIM', '',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g40_tight_icalotight_L1EM24VHIM','mu15noL1','mu2noL1']]],
        ['g40_tight_icalotight_L1EM24VHI_mu18noL1',        'L1_EM24VHI', ['L1_EM24VHI', ''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g40_tight_icalotight_L1EM24VHI','mu18noL1'] ]],
        ['g40_tight_icalotight_L1EM24VHI_mu15noL1_mu2noL1', 'L1_EM24VHI', ['L1_EM24VHI', '',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g40_tight_icalotight_L1EM24VHI','mu15noL1','mu2noL1']]],
        ['g35_loose_L1EM24VHIM_mu18',        'L1_EM24VHIM', ['L1_EM24VHIM', 'L1_MU11'], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_loose_L1EM24VHIM','mu18'] ]],
        ['g35_loose_L1EM24VHIM_mu15_mu2noL1', 'L1_EM24VHIM', ['L1_EM24VHIM', 'L1_MU11',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_loose_L1EM24VHIM','mu15','mu2noL1']]],
        ['e30_mergedtight_g35_medium_Heg',       'L1_2EM20VH', [], [PhysicsStream], ['Primary:20000','RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ['e17_lhmedium_nod0_tau80_medium1_tracktwo',   'L1_EM15VHI_TAU40_2TAU15',['L1_EM15VHI','L1_TAU40'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0","tau80_medium1_tracktwo"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1EM15VHI_2TAU12IM',  'L1_EM15VHI_2TAU12IM',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwo"]]],
        ['mu14_ivarloose_tau35_medium1_tracktwo_L1MU10_TAU20IM_J25_2J20',      'L1_MU10_TAU20IM_J25_2J20',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_medium1_tracktwo"]]],   
        ['mu14_iloose_tau25_perf_tracktwo', 'L1_MU10_TAU12IM',['L1_MU10','L1_TAU12IM'], [PhysicsStream, 'express'], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_iloose","tau25_perf_tracktwo"]]],
        ['mu14_ivarloose_tau25_medium1_tracktwo_L1MU10_TAU12IM',  'L1_MU10_TAU12IM',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwo"]]],
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_xe50', 'L1_TAU20IM_2TAU12IM_XE35',['L1_TAU20IM','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo","xe50"]]],
        ['mu14_ivarloose_tau35_medium1_tracktwoEF_L1MU10_TAU20IM_J25_2J20',      'L1_MU10_TAU20IM_J25_2J20',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_medium1_tracktwoEF"]]],   
        ['mu14_ivarloose_tau35_mediumRNN_tracktwoMVA_L1MU10_TAU20IM_J25_2J20',      'L1_MU10_TAU20IM_J25_2J20',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_mediumRNN_tracktwoMVA"]]],   
        ['g25_medium_L1EM24VHI_tau25_kaonpi2_tracktwoEF_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_kaonpi2_tracktwoEF'],False] ],
	    ['g35_medium_L1EM24VHI_tau25_dipion3_tracktwoEF_60mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g35_medium_L1EM24VHI','tau25_dipion3_tracktwoEF'],False] ],
        ['g25_medium_L1EM24VHI_tau25_kaonpi2_tracktwoMVA_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_kaonpi2_tracktwoMVA'],False] ],
        ['g35_medium_L1EM24VHI_tau25_dipion3_tracktwoMVA_60mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g35_medium_L1EM24VHI','tau25_dipion3_tracktwoMVA'],False] ],
        ['j80_xe80',               'L1_J40_XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet', 'BW:MET'], -1,['serial',-1,["j80","xe80"]]], 
        ['e20_lhtight_nod0_3j20_L1EM18VHI_3J20',      'L1_EM18VHI_3J20',    ['L1_EM18VHI',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1, ['serial',-1,["e20_lhtight_nod0","3j20"] ]],
        ['e20_lhmedium_nod0_3j20_L1EM18VHI_3J20', 'L1_EM18VHI_3J20',    ['L1_EM18VHI',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1, ['serial',-1,["e20_lhmedium_nod0","3j20"] ]],
        ['e28_lhvloose_nod0_L1EM22VH_3j20noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e28_lhvloose_nod0_L1EM22VH","3j20noL1"] ]],
        ['e28_lhvloose_nod0_L1EM22VH_4j20noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e28_lhvloose_nod0_L1EM22VH","4j20noL1"] ]],
        ['e28_lhvloose_nod0_L1EM22VH_5j15noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e28_lhvloose_nod0_L1EM22VH","5j15noL1"] ]],
        ['e28_lhvloose_nod0_L1EM22VH_6j15noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e28_lhvloose_nod0_L1EM22VH","6j15noL1"] ]],
        ['e26_lhvloose_nod0_L1EM22VH_3j20noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e26_lhvloose_nod0_L1EM22VH","3j20noL1"] ]],
        ['e26_lhvloose_nod0_L1EM22VH_4j20noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e26_lhvloose_nod0_L1EM22VH","4j20noL1"] ]],
        ['e26_lhvloose_nod0_L1EM22VH_5j15noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e26_lhvloose_nod0_L1EM22VH","5j15noL1"] ]],
        ['e26_lhvloose_nod0_L1EM22VH_6j15noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e26_lhvloose_nod0_L1EM22VH","6j15noL1"] ]],
        ['e28_lhmedium_nod0_L1EM22VH_3j20noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e28_lhmedium_nod0_L1EM22VH","3j20noL1"] ]],
        ['e28_lhmedium_nod0_L1EM22VH_4j20noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e28_lhmedium_nod0_L1EM22VH","4j20noL1"] ]],
        ['e28_lhmedium_nod0_L1EM22VH_5j15noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e28_lhmedium_nod0_L1EM22VH","5j15noL1"] ]],
        ['e28_lhmedium_nod0_L1EM22VH_6j15noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e28_lhmedium_nod0_L1EM22VH","6j15noL1"] ]],
        ['e26_lhmedium_nod0_L1EM22VH_3j20noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e26_lhmedium_nod0_L1EM22VH","3j20noL1"] ]],
        ['e26_lhmedium_nod0_L1EM22VH_4j20noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e26_lhmedium_nod0_L1EM22VH","4j20noL1"] ]],
        ['e26_lhmedium_nod0_L1EM22VH_5j15noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e26_lhmedium_nod0_L1EM22VH","5j15noL1"] ]],
        ['e26_lhmedium_nod0_L1EM22VH_6j15noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e26_lhmedium_nod0_L1EM22VH","6j15noL1"] ]],
        ['mu26_2j20noL1',  'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["mu26","2j20noL1"] ]],
        ['mu26_3j20noL1',  'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["mu26","3j20noL1"] ]],
        ['mu26_4j15noL1',  'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["mu26","4j15noL1"] ]],
        ['mu26_5j15noL1',  'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["mu26","5j15noL1"] ]],
        ['g25_medium_L1EM22VHI_4j35_0eta490_invm700', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1,['serial',-1,["g25_medium_L1EM22VHI","4j35_0eta490_invm700"]]],
        ['g25_loose_L1EM20VH_4j35_0eta490', 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1,['serial',-1,["g25_loose_L1EM20VH","4j35_0eta490"]]],
	['j55_0eta240_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49','L1_J25.0ETA23_2J15.31ETA49',['',''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet_combined' ], -1, ['serial',-1,['j55_0eta240','2j45_320eta490_L1J25.0ETA23_2J15.31ETA49'] ]],
        ['g25_medium_L1EM24VHIM_tau25_dikaonmass_tracktwo_50mVis10000','L1_EM24VHIM', ['L1_EM24VHIM','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHIM','tau25_dikaonmass_tracktwo'],False]],
        ['g25_medium_L1EM24VHIM_tau25_singlepion_tracktwo_50mVis10000','L1_EM24VHIM', ['L1_EM24VHIM','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHIM','tau25_singlepion_tracktwo'],False]],
        ['g25_medium_L1EM24VHIM_tau25_dipion1loose_tracktwo_50mVis10000','L1_EM24VHIM', ['L1_EM24VHIM','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHIM','tau25_dipion1loose_tracktwo'],False]],
        ['g25_medium_L1EM24VHIM_tau25_dipion2_tracktwo_50mVis10000','L1_EM24VHIM', ['L1_EM24VHIM','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHIM','tau25_dipion2_tracktwo'],False]],
        ['g25_medium_L1EM24VHIM_tau25_kaonpi1_tracktwo_50mVis10000','L1_EM24VHIM', ['L1_EM24VHIM','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHIM','tau25_kaonpi1_tracktwo'],False]],
        ['g25_medium_L1EM24VHIM_tau25_kaonpi2_tracktwo_50mVis10000','L1_EM24VHIM', ['L1_EM24VHIM','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHIM','tau25_kaonpi2_tracktwo'],False]],
        ['g25_medium_L1EM24VHI_tau25_kaonpi2_tracktwo_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_kaonpi2_tracktwo'],False]],	
        ['g25_medium_tau25_dikaonmass_tracktwo_50mVis10000_L130M-EM20ITAU12','L1_30M-EM20ITAU12', ['L1_EM20VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium','tau25_dikaonmass_tracktwo'],False]],
        ['g25_medium_tau25_singlepion_tracktwo_50mVis10000_L130M-EM20ITAU12','L1_30M-EM20ITAU12', ['L1_EM20VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium','tau25_singlepion_tracktwo'],False]],
        ['g25_medium_tau25_dipion1loose_tracktwo_50mVis10000_L130M-EM20ITAU12','L1_30M-EM20ITAU12', ['L1_EM20VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium','tau25_dipion1loose_tracktwo'],False]],
        ['g25_medium_tau25_dipion2_tracktwo_50mVis10000_L130M-EM20ITAU12','L1_30M-EM20ITAU12', ['L1_EM20VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium','tau25_dipion2_tracktwo'],False]],
        ['g25_medium_tau25_kaonpi1_tracktwo_50mVis10000_L130M-EM20ITAU12','L1_30M-EM20ITAU12', ['L1_EM20VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium','tau25_kaonpi1_tracktwo'],False]],
        ['g25_medium_tau25_kaonpi2_tracktwo_50mVis10000_L130M-EM20ITAU12','L1_30M-EM20ITAU12', ['L1_EM20VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium','tau25_kaonpi2_tracktwo'],False]],
        ['g85_tight_L1EM24VHIM_3j50noL1', 'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['Primary:17000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g85_tight_L1EM24VHIM","3j50noL1"]]],
        ['g85_tight_icalovloose_L1EM24VHIM_3j50noL1', 'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g85_tight_icalovloose_L1EM24VHIM","3j50noL1"]]],
        ['g100_tight_L1EM24VHIM_3j50noL1', 'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g100_tight_L1EM24VHIM","3j50noL1"]]],
        ['g100_tight_icalovloose_L1EM24VHIM_3j50noL1', 'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g100_tight_icalovloose_L1EM24VHIM","3j50noL1"]]],
        ['g85_tight_L1EM24VHI_3j50noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['Primary:17000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g85_tight_L1EM24VHI","3j50noL1"]]],
        ['g100_tight_L1EM22VHI_3j50noL1', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['Primary:17000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g100_tight_L1EM22VHI","3j50noL1"]]],
        ['g100_tight_L1EM24VHI_3j50noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g100_tight_L1EM24VHI","3j50noL1"]]],
        ['g85_tight', 'L1_EM22VHI', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1],
        ['g85_tight_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1],
        ['g100_tight', 'L1_EM22VHI', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1],
        ['g100_tight_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1],
        ['j40_cleanT_xe85_tc_lcw_2dphi05_L1XE50',         'L1_XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,["j40_cleanT","xe85_tc_lcw_L1XE50"]]],
        ['mu14_3j20_L1MU10_3J20', 'L1_MU10_3J20', ['L1_MU10_3J20', ''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon','BW:Jet'], -1,['serial',-1,['mu14', '3j20'] ],True],
        ['mu6_ivarloose_2j40_0eta490_invm1000_L1MU6_J30.0ETA49_2J20.0ETA49', 'L1_MU6_J30.0ETA49_2J20.0ETA49', ['L1_MU6',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["mu6_ivarloose","2j40_0eta490_invm1000"]]],
        ['g35_medium_L1EM24VHI_tau25_dipion3_tracktwo_60mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g35_medium_L1EM24VHI','tau25_dipion3_tracktwo'],False]],
        ['e26_lhmedium_nod0_5j30_0eta240_L1EM20VH_3J20',    	'L1_EM20VH_3J20',    ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1, ['serial',-1,["e26_lhmedium_nod0","5j30_0eta240"] ]],
        ['mu26_5j30_0eta240_L1MU10_3J20',    	'L1_MU10_3J20',    ['L1_MU10',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jet'],-1, ['serial',-1,["mu26","5j30_0eta240"] ]],
        ['e9_lhvloose_nod0_mu20_mu8noL1_L1EM8VH_MU20',  'L1_EM8VH_MU20',['L1_EM8VH','L1_MU20',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['serial',-1,['mu20','mu8noL1','e9_lhvloose_nod0'] ]],
        ['2g6_loose_L1J100', 'L1_J100', ['L1_2EM3'],  [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1],
        ['2g6_tight_icalotight_L1J50', 'L1_J50', ['L1_2EM3','L1_J50'], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1],
        ['2g6_loose_L1J50', 'L1_J50', ['L1_2EM3','L1_J50'], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1],
        ['2g6_tight_icalotight_L1J40', 'L1_J40', ['L1_2EM3','L1_J40'], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1],
        ['2g6_loose_L1J40', 'L1_J40', ['L1_2EM3','L1_J40'], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1],
        ['g45_loose_5j45_0eta240', 'L1_4J15.0ETA25', ['L1_EM15VH','L1_4J15.0ETA25'], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g45_loose","5j45_0eta240"]]],
        ['e9_lhvloose_nod0_e5_lhvloose_nod0_bBeexM6000t_mu6_nomucomb_L1BPH-0M9-EM7-EM5_MU6',  'L1_BPH-0M9-EM7-EM5_MU6',   ['L1_EM7','L1_EM3','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag],-1,['parallel',-1,[] ]],
        ['e5_lhvloose_nod0_bBeexM6000t_mu6_nomucomb_L1BPH-0DR3-EM7J15_MU6',  'L1_BPH-0DR3-EM7J15_MU6',   ['L1_EM7','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag],-1,['parallel',-1,[] ]],
        ['e5_lhvloose_nod0_bBeexM6000t_2mu4_nomucomb_L1BPH-0DR3-EM7J15_2MU4',  'L1_BPH-0DR3-EM7J15_2MU4',   ['L1_EM7','L1_2MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag],-1,['parallel',-1,[] ]],

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
    'xe50',
    'tau35_perf_tracktwo_tau25_perf_tracktwo',
    'tau160_idperf_tracktwo_L1TAU100',
    'tau160_idperf_tracktwo_L1TAU100',
    'tau160_idperf_tracktwoEF_L1TAU100',
    'tau160_idperf_tracktwoMVA_L1TAU100',
    'tau160_perf_tracktwo_L1TAU100',
    'tau160_perf_tracktwo_L1TAU100',
    'tau160_perf_tracktwoEF_L1TAU100',
    'tau160_perf_tracktwoMVA_L1TAU100',
    'tau125_medium1_tracktwo',
    'tau160_idperf_track_L1TAU100',
    'tau160_idperf_track_L1TAU100',
    'tau160_perf_tracktwo_L1TAU100',
    'tau25_idperf_track',
    'tau25_medium1_tracktwo',
    'tau25_perf_tracktwo',
    'tau25_perf_tracktwo_L1TAU12',
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
