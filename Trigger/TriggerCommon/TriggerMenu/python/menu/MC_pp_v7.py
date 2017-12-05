# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import TriggerMenu.menu.Physics_pp_v7 as physics_menu

from TriggerJobOpts.TriggerFlags          import TriggerFlags
from copy                                 import deepcopy

import re

from AthenaCommon.Logging import logging
log = logging.getLogger( 'MC_pp_v7.py' )


#---------------------------------------------------------------------
#---------------------------------------------------------------------
def setupMenu():

    physics_menu.setupMenu()
    PhysicsStream="Main"

    # stream, BW and RATE tags for Bphysics items that appear in Muon and Bphysics slice.signatures
    BPhysicsStream     = "BphysLS"
    BMultiMuonStream   = "Main"  
    RATE_BphysTag      = 'RATE:Bphysics'
    RATE_BMultiMuonTag = 'RATE:MultiMuon'  # can become RATE:BMultiMuon' with one letter change 
    BW_BphysTag        = 'BW:Bphys'
    RATE_DYTag         = 'RATE:Muon'
    BW_DYTag           = 'BW:Muon'   
    
    
    # ---------------------------------------------------------------------------------------
    # INPUT FORMAT FOR CHAINS:
    # ['chainName',  'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep], OPTIONAL: [mergingStrategy, offset,[merginOrder] ]], topoStartsFrom = False
    # ----------------------------------------------------------------------------------------

    TriggerFlags.TestSlice.signatures = TriggerFlags.TestSlice.signatures() + [
			 ]

    TriggerFlags.AFPSlice.signatures = TriggerFlags.AFPSlice.signatures() + [
        ]



    TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
        ['mu100',                  'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu35_ivarmedium',	     'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu36_ivarmedium',          'L1_MU20MU21',           ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu40_ivarmedium',          'L1_MU20MU21',           ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu50_ivarmedium',          'L1_MU20MU21',           ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],

        ['mu20_r1extr',          'L1_MU20MU21',      ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10_r1extr',          'L1_MU10',                   [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu4_r1extr',           'L1_MU4',                    [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_icalo',           'L1_MU20MU21',      ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_iloosecalo',      'L1_MU20MU21',      ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_imediumcalo',     'L1_MU20MU21',      ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu26_ivarmediumcalo',  'L1_MU20MU21',      ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],

        ['mu20_imedium_mu8noL1',    'L1_MU20MU21',  ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_imedium','mu8noL1']]],
        ['mu20_iloose_mu8noL1',     'L1_MU20MU21',  ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_iloose','mu8noL1']]],
        ['mu20_iloose_2mu4noL1',    'L1_MU20MU21',  ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_iloose','2mu4noL1']]],
        ### ATR-14543
        ['mu20_ivarmedium_mu8noL1',    'L1_MU20MU21',  ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_ivarmedium','mu8noL1']]],
        ['mu20_ivarloose_mu8noL1',     'L1_MU20MU21',  ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_ivarloose','mu8noL1']]],
        ['mu20_ivarloose_2mu4noL1',    'L1_MU20MU21',  ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_ivarloose','2mu4noL1']]],

        # new test chains for dimuon inefficiency
        ['3mu4_noMuCombOvlpRm',    'L1_3MU4',           [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu14_noMuCombOvlpRm',   'L1_2MU10',          [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],

        ['2mu16_L12MU10',          'L1_2MU10',          [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu18_L12MU10',          'L1_2MU10',          [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],


         ['mu60_msonly_mu60noL1_msonly',  'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream],     ['RATE:MultiMuon','BW:Muon'],-1,['serial',-1,['mu60_msonly','mu60noL1_msonly']]],




        ## Primary (narrow scan chains)
        ['mu14_imedium_mu6noL1_msonly',    'L1_MU10',  ['L1_MU10',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu14_imedium','mu6noL1_msonly']]],
        ### ATR-14543
        ['mu14_ivarmedium_mu6noL1_msonly',    'L1_MU10',  ['L1_MU10',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu14_ivarmedium','mu6noL1_msonly']]],
        ['2mu6_mu4', 'L1_2MU6_3MU4', ['L1_2MU6','L1_MU4'], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['mu20_msonly_mu12noL1_msonly_nscan05_noComb',   'L1_MU20MU21',   ['L1_MU20','L2_mu20_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu12noL1_msonly_nscan05_noComb']]],

       # not working for now                                                                                                                                                                                   
        ['mu20_msonly_mu6noL1_msonly_nscan05_noComb',       'L1_MU20MU21',      ['L1_MU20','L2_mu20_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu6noL1_msonly_nscan05_noComb']]],

        ['mu11_msonly_mu6noL1_msonly_nscan05_noComb',       'L1_MU10',      ['L1_MU10','L2_mu11_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11_msonly','mu6noL1_msonly_nscan05_noComb']]],
        ['mu11_msonly_mu6noL1_msonly_nscan05_noComb_L1MU4_EMPTY',       'L1_MU4_EMPTY',        ['L1_MU10','L2_mu11_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11_msonly','mu6noL1_msonly_nscan05_noComb']]],
        ['mu11_msonly_mu6noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_ISO','L1_MU4_UNPAIRED_ISO', ['L1_MU10','L2_mu11_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11_msonly','mu6noL1_msonly_nscan05_noComb']]],

        ### ATR-14377: duplicate noComb chains with leading-leg AND nscan-leg noComb
        ['mu20_msonly_noComb_mu12noL1_msonly_nscan05_noComb',   'L1_MU20MU21',   ['L1_MU20','L2_mu20_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_noComb','mu12noL1_msonly_nscan05_noComb']]],
        ['mu20_msonly_noComb_mu6noL1_msonly_nscan05_noComb',       'L1_MU20MU21',      ['L1_MU20','L2_mu20_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_noComb','mu6noL1_msonly_nscan05_noComb']]],
        ['mu11_msonly_noComb_mu6noL1_msonly_nscan05_noComb',       'L1_MU10',      ['L1_MU10','L2_mu11_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11_msonly_noComb','mu6noL1_msonly_nscan05_noComb']]],
        ['mu11_msonly_noComb_mu6noL1_msonly_nscan05_noComb_L1MU4_EMPTY',       'L1_MU4_EMPTY',        ['L1_MU10','L2_mu11_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11_msonly_noComb','mu6noL1_msonly_nscan05_noComb']]],
        ['mu11_msonly_noComb_mu6noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_ISO','L1_MU4_UNPAIRED_ISO', ['L1_MU10','L2_mu11_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11_msonly_noComb','mu6noL1_msonly_nscan05_noComb']]],

        ### ATR-14377: duplicate noComb chains with leading-leg-only noComb
        ['mu20_msonly_noComb_mu12noL1_msonly_nscan05',   'L1_MU20MU21',   ['L1_MU20','L2_mu20_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_noComb','mu12noL1_msonly_nscan05']]],
        ['mu20_msonly_noComb_mu6noL1_msonly_nscan05',       'L1_MU20MU21',      ['L1_MU20','L2_mu20_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_noComb','mu6noL1_msonly_nscan05']]],
        ['mu11_msonly_noComb_mu6noL1_msonly_nscan05',       'L1_MU10',      ['L1_MU10','L2_mu11_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11_msonly_noComb','mu6noL1_msonly_nscan05']]],
        ['mu11_msonly_noComb_mu6noL1_msonly_nscan05_L1MU4_EMPTY',       'L1_MU4_EMPTY',        ['L1_MU10','L2_mu11_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11_msonly_noComb','mu6noL1_msonly_nscan05']]],
        ['mu11_msonly_noComb_mu6noL1_msonly_nscan05_L1MU4_UNPAIRED_ISO','L1_MU4_UNPAIRED_ISO', ['L1_MU10','L2_mu11_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11_msonly_noComb','mu6noL1_msonly_nscan05']]],


        ### ATR-14377: remove noComb for tests
        ['mu20_msonly_mu15noL1_msonly_nscan05', 'L1_MU20MU21',   ['L1_MU20','L2_mu20_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu15noL1_msonly_nscan05']]],


        ### ATR-14377: nscan with iloosems legs
        ['mu20_msonly_iloosems_mu10noL1_msonly_nscan05_noComb',   'L1_MU20MU21',   ['L1_MU20','L2_mu20_msonly_iloosems'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_iloosems','mu10noL1_msonly_nscan05_noComb']]],
        ['mu20_msonly_iloosems_mu10noL1_msonly_nscan03_noComb',   'L1_MU20MU21',   ['L1_MU20','L2_mu20_msonly_iloosems'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_iloosems','mu10noL1_msonly_nscan03_noComb']]],
        ['mu20_msonly_iloosems_mu15noL1_msonly_nscan05_noComb',   'L1_MU20MU21',   ['L1_MU20','L2_mu20_msonly_iloosems'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_iloosems','mu15noL1_msonly_nscan05_noComb']]],
        ['mu20_msonly_iloosems_mu15noL1_msonly_nscan03_noComb',   'L1_MU20MU21',   ['L1_MU20','L2_mu20_msonly_iloosems'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_iloosems','mu15noL1_msonly_nscan03_noComb']]],
        ['mu20_msonly_iloosems_mu12noL1_msonly_nscan03_noComb',   'L1_MU20MU21',   ['L1_MU20','L2_mu20_msonly_iloosems'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_iloosems','mu12noL1_msonly_nscan03_noComb']]],
        ['mu20_msonly_iloosems_mu6noL1_msonly_nscan05',           'L1_MU20MU21', ['L1_MU20','L2_mu20_msonly_iloosems'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_iloosems','mu6noL1_msonly_nscan05']]],
        ['mu20_msonly_iloosems_mu6noL1_msonly_nscan05_noComb',    'L1_MU20MU21', ['L1_MU20','L2_mu20_msonly_iloosems'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_iloosems','mu6noL1_msonly_nscan05_noComb']]],
        ['mu20_msonly_mu6noL1_msonly_nscan03',                 'L1_MU20MU21', ['L1_MU20','L2_mu20_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu6noL1_msonly_nscan03']]],
        ['mu20_msonly_iloosems_mu6noL1_msonly_nscan03',        'L1_MU20MU21', ['L1_MU20','L2_mu20_msonly_iloosems'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_iloosems','mu6noL1_msonly_nscan03']]],
        ['mu20_msonly_iloosems_mu6noL1_msonly_nscan03_noComb', 'L1_MU20MU21', ['L1_MU20','L2_mu20_msonly_iloosems'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_iloosems','mu6noL1_msonly_nscan03_noComb']]],

       
        ['mu22_iloose_mu8noL1_calotag_0eta010', 'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22_iloose','mu8noL1_calotag_0eta010']]],
        ['mu24_iloose_mu8noL1_calotag_0eta010', 'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24_iloose','mu8noL1_calotag_0eta010']]],
        ### ATR-14543
        ['mu22_ivarloose_mu8noL1_calotag_0eta010', 'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22_ivarloose','mu8noL1_calotag_0eta010']]],
        ['mu24_ivarloose_mu8noL1_calotag_0eta010', 'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24_ivarloose','mu8noL1_calotag_0eta010']]],


        ['mu20_nomucomb_2mu4noL1', 'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_nomucomb','2mu4noL1']]],
        ['mu22_nomucomb_2mu4noL1', 'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22_nomucomb','2mu4noL1']]],
        ['mu20_nomucomb_mu8noL1' , 'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_nomucomb','mu8noL1']]],
        ['mu22_nomucomb_mu8noL1' , 'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22_nomucomb','mu8noL1']]],

        #['mu8',                    'L1_MU6',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1], 
        ['3mu6_nomucomb',                   'L1_3MU6',           [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['mu6_mu4noL1',            'L1_MU6',  ['L1_MU6',''],  [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu6','mu4noL1']]],
        ['mu4_mu4noL1',            'L1_MU4',  ['L1_MU4',''],  [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu4','mu4noL1']]],
	['mu4noL1',                '',  [],  [PhysicsStream], ['RATE:SingleMuon','BW:Muon'], -1, ],
	['mu0noL1',                '',  [],  [PhysicsStream], ['RATE:SingleMuon','BW:Muon'], -1, ],
       
 
        # Toroid-off run (ATR-9923)
        ['mu11_L1MU10_2mu4noL1_nscan03_L1MU10_2MU6', 'L1_MU10_2MU6', ['L1_MU10','L2_mu11_L1MU10'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11_L1MU10','2mu4noL1_nscan03']]],
        ['mu11_2mu4noL1_nscan03_L1MU11_2MU6',      'L1_MU11_2MU6',    ['L1_MU11','L2_mu11'],        [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11','2mu4noL1_nscan03']]],
        ['mu11_nomucomb_mu6noL1_nscan03_L1MU11_2MU6', 'L1_MU11_2MU6', ['L1_MU11','L2_mu6_nomucomb'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11_nomucomb','mu6noL1_nscan03']]],

        # single muon

        ['mu24_ivarloose',           'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_iloose',              'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],

        ['mu0_perf',               'L1_MU4',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1], 
        ['mu26_imedium',           'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu28_imedium',           'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu40',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_imedium',           'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20_imedium_L1MU10',    'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu11',                   'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu14_iloose',            'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        #['mu10',                   'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10_msonly',            'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu4_msonly',             'L1_MU4',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu14_nomucomb',                   'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20_nomucomb',                   'L1_MU20MU21',['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['2mu14_nomucomb',         'L1_2MU10',          [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu10_msonly_L13MU6',    'L1_3MU6',           [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu10',                  'L1_2MU10',          [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu10_nomucomb',         'L1_2MU10',          [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['mu20_mu8noL1',           'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','mu8noL1']]],
        #['2mu6',                    'L1_2MU6',  [], [PhysicsStream], [RATE_BMultiMuonTag, 'BW:Muon'], -1],
        #['2mu4',                    'L1_2MU4',  [], [PhysicsStream], [RATE_BMultiMuonTag, 'BW:Muon'], -1],
        ['mu20_2mu2noL1',            'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','2mu2noL1']]],

        #low-pt multi-muon chains, ATR-15704
        ['3mu4_nomucomb',          'L1_3MU4',           [], [BMultiMuonStream], [RATE_BMultiMuonTag, BW_BphysTag], -1],
        ['3mu4_nomucomb_mu2noL1',  'L1_3MU4', ['L1_3MU4',''], [BMultiMuonStream], [RATE_BMultiMuonTag, BW_BphysTag], -1,['serial',-1,['3mu4_nomucomb','mu2noL1']]],
        ['2mu4_mu3_mu2noL1_L13MU4', 'L1_3MU4', ['L1_2MU4','L1_MU4',''], [BMultiMuonStream], [RATE_BMultiMuonTag, BW_BphysTag], -1,['serial',-1,['2mu4','mu3','mu2noL1']]],
        ['3mu4_nomucomb_mu4noL1',  'L1_3MU4', ['L1_3MU4',''], [BMultiMuonStream], [RATE_BMultiMuonTag, BW_BphysTag], -1,['serial',-1,['3mu4_nomucomb','mu4noL1']]],
        ['4mu4_nomucomb',          'L1_4MU4', ['L1_4MU4',''], [BMultiMuonStream], [RATE_BMultiMuonTag, BW_BphysTag], -1,['serial',-1,['4mu4_nomucomb','']]],

        ['mu18_mu8noL1_calotag_0eta010_L1MU11', 'L1_MU11', ['L1_MU11',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu18','mu8noL1_calotag_0eta010']]],
        ['mu20_mu8noL1_calotag_0eta010', 'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','mu8noL1_calotag_0eta010']]],

        # Narrow scan
        # notes: see ATR-11846
        # inputTE for narrow scan sequence (2nd inputTE) is a dummy string for MuonDef.py
        # To indicate narrow scan uses lastTE of previous sequence and also to avoid duplicated sequences with same TE name, write L2_ TE name of the 1st sequence.

        ['mu20_nomucomb_mu6noL1_nscan03', 'L1_MU20MU21',['L1_MU20','L2_mu20_nomucomb'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1,['serial',-1,['mu20_nomucomb','mu6noL1_nscan03']]],  #OI need to discuss what to do here

        ['mu11_nomucomb_2mu4noL1_nscan03_L1MU11_2MU6', 'L1_MU11_2MU6', ['L1_MU11','L2_mu11_nomucomb'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1,['serial',-1,['mu11_nomucomb','2mu4noL1_nscan03']]], #OI need to discuss what to do here
        ['3mu6_msonly_L1MU4_UNPAIRED_NONISO',                     'L1_MU4_UNPAIRED_NONISO', ['L1_3MU6'], ["Late"], ['RATE:MultiMuon', 'BW:Muon'], -1],


        ['mu20_msonly_mu10noL1_msonly_nscan05_noComb',   'L1_MU20MU21',   ['L1_MU20','L2_mu20_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu10noL1_msonly_nscan05_noComb']]],
        ['mu20_msonly_mu10noL1_msonly_nscan03_noComb',   'L1_MU20MU21',   ['L1_MU20','L2_mu20_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu10noL1_msonly_nscan03_noComb']]],
        ['mu20_msonly_mu10noL1_msonly_nscan05_noComb_L1MU6_EMPTY',       'L1_MU6_EMPTY',        ['L1_MU20','L2_mu20_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu10noL1_msonly_nscan05_noComb']]],
        ['mu20_msonly_mu10noL1_msonly_nscan03_noComb_L1MU6_EMPTY',       'L1_MU6_EMPTY',        ['L1_MU20','L2_mu20_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu10noL1_msonly_nscan03_noComb']]],
        ['mu20_msonly_mu10noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_ISO','L1_MU4_UNPAIRED_ISO', ['L1_MU20','L2_mu20_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu10noL1_msonly_nscan05_noComb']]],
        ['mu20_msonly_mu10noL1_msonly_nscan03_noComb_L1MU4_UNPAIRED_ISO','L1_MU4_UNPAIRED_ISO', ['L1_MU20','L2_mu20_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu10noL1_msonly_nscan03_noComb']]],
        ['mu20_msonly_mu10noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_NONISO','L1_MU4_UNPAIRED_NONISO', ['L1_MU20','L2_mu20_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu10noL1_msonly_nscan05_noComb']]],
        ['mu20_msonly_mu15noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_NONISO','L1_MU4_UNPAIRED_NONISO', ['L1_MU20','L2_mu20_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu15noL1_msonly_nscan05_noComb']]],

        ### ATR-14377:
        ['mu20_msonly_mu15noL1_msonly_nscan03_noComb',   'L1_MU20MU21',   ['L1_MU20','L2_mu20_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu15noL1_msonly_nscan03_noComb']]],
        ['mu20_msonly_mu15noL1_msonly_nscan03_noComb_L1MU6_EMPTY',       'L1_MU6_EMPTY',        ['L1_MU20','L2_mu20_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu15noL1_msonly_nscan03_noComb']]],
        ['mu20_msonly_mu15noL1_msonly_nscan03_noComb_L1MU4_UNPAIRED_ISO','L1_MU4_UNPAIRED_ISO', ['L1_MU20','L2_mu20_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu15noL1_msonly_nscan03_noComb']]],
        ['mu20_msonly_mu15noL1_msonly_nscan03_noComb_L1MU4_UNPAIRED_NONISO','L1_MU4_UNPAIRED_NONISO', ['L1_MU20','L2_mu20_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu15noL1_msonly_nscan03_noComb']]],

        ['mu20_msonly_mu12noL1_msonly_nscan03_noComb',   'L1_MU20MU21',   ['L1_MU20','L2_mu20_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu12noL1_msonly_nscan03_noComb']]],
        ['mu20_msonly_mu12noL1_msonly_nscan03_noComb_L1MU6_EMPTY',       'L1_MU6_EMPTY',        ['L1_MU20','L2_mu20_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu12noL1_msonly_nscan03_noComb']]],
        ['mu20_msonly_mu12noL1_msonly_nscan03_noComb_L1MU4_UNPAIRED_ISO','L1_MU4_UNPAIRED_ISO', ['L1_MU20','L2_mu20_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu12noL1_msonly_nscan03_noComb']]],
        ['mu20_msonly_mu12noL1_msonly_nscan03_noComb_L1MU4_UNPAIRED_NONISO','L1_MU4_UNPAIRED_NONISO', ['L1_MU20','L2_mu20_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu12noL1_msonly_nscan03_noComb']]],        



        # exclusive di-lep
        ['2mu6_10invm30_pt2_z10', 'L1_2MU6', [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['mu20_imedium_L1MU10_2J20', 'L1_MU10_2J20', [], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon','BW:Jet'], -1],                
        ['mu20_msonly_mu6noL1_msonly_nscan05_L1MU4_EMPTY',       'L1_MU4_EMPTY',        ['L1_MU20','L2_mu20_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu6noL1_msonly_nscan05']]],
        ['mu20_msonly_mu6noL1_msonly_nscan05_L1MU6_EMPTY',       'L1_MU6_EMPTY',        ['L1_MU20','L2_mu20_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu6noL1_msonly_nscan05']]],
        ['mu20_msonly_mu6noL1_msonly_nscan05_L1MU4_UNPAIRED_ISO',       'L1_MU4_UNPAIRED_ISO', ['L1_MU20','L2_mu20_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu6noL1_msonly_nscan05']]],
        ['mu20_msonly_mu6noL1_msonly_nscan05_noComb_L1MU4_EMPTY',       'L1_MU4_EMPTY',        ['L1_MU20','L2_mu20_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu6noL1_msonly_nscan05_noComb']]],
        ['mu20_msonly_mu6noL1_msonly_nscan05_noComb_L1MU4_UNPAIRED_ISO','L1_MU4_UNPAIRED_ISO', ['L1_MU20','L2_mu20_msonly'], ["Late"], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu6noL1_msonly_nscan05_noComb']]],

        ### ATR-14377: nscan with different L1 seeds
        ['mu20_msonly_mu6noL1_msonly_nscan05_L1MU20_XE30', 'L1_MU20_XE30',          ['L1_MU20_XE30','L2_mu20_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu6noL1_msonly_nscan05']]],
        ['mu20_msonly_mu6noL1_msonly_nscan05_L1MU20_J40', 'L1_MU20_J40',            ['L1_MU20_J40','L2_mu20_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu6noL1_msonly_nscan05']]],

        #ATR-14377
        ['mu6_dRl1_mu20_msonly_mu6noL1_dRl1_msonly','L1_MU11_2MU6', [['L1_MU6','L1_MU6'],'L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu6_dRl1','mu20_msonly','mu6noL1_dRl1_msonly']]],

        # Calotag items ATR-16142
        ['3mu4_mu2noL1_calotag_0eta010',           'L1_3MU4', ['L1_3MU4',''], [BMultiMuonStream], [RATE_BMultiMuonTag, BW_BphysTag], -1,['serial',-1,['3mu4','mu2noL1_calotag_0eta010']]],
        ['2mu4_mu3_mu2noL1_calotag_0eta010_L13MU4', 'L1_3MU4', ['L1_2MU4','L1_MU4',''], [BMultiMuonStream], [RATE_BMultiMuonTag, BW_BphysTag], -1,['serial',-1,['2mu4','mu3','mu2noL1_calotag_0eta010']]],
			 ]


    if TriggerFlags.doFTK():
        TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
            ['mu24_idperf_FTK',            'L1_MU20MU21',    ['L1_MU20'], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1], 
            ['mu6_idperf_FTK',             'L1_MU6',                  [], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
            ['mu24_idperf_FTKRefit',       'L1_MU20MU21',    ['L1_MU20'], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1], 
            ['mu6_idperf_FTKRefit',        'L1_MU6',                  [], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
            ['mu22_mu8noL1_FTKFS',         'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','mu8noL1_FTKFS']]],
            ['mu26_ivarmedium_FTK',        'L1_MU20MU21',    ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
            ]

    TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + [

        
        #####
        ######################### Performance-style chains
        #####
        
        # Jet data scouting stream chains
        # These record only the trigger jets, but for every event passing the L1 trigger
        ['j0_perf_ds1_L1All',      'L1_All',  [], ['DataScouting_05_Jets'], ['RATE:Cosmic_Jets_DS', 'BW:Jet'], -1],
        ['j0_lcw_jes_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1], 



        #####
        ######################### Single-jet small-R trigger chains
        #####

        # Standard central jet triggers
        ['j10',                     'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
	['j20',                     'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
	['j30',                     'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
	['j30_L1TE5',               'L1_TE5', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
	#['j30_L1TE10',              'L1_TE10', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
	['j40',                     'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
	['j40_L1TE10',              'L1_TE10', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
	#['j40_L1TE20',              'L1_TE20', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
	['j40_L1J12',               'L1_J12', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
	['j50_L1TE20',              'L1_TE20', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        #['j50_L1J12',               'L1_J12', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
	#['j50_L1J15',               'L1_J15', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j55_L1RD0_FILLED',        'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
	#['j60_L1J15',               'L1_J15', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
	#['j75_L1J20',               'L1_J20', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j85_L1J40',               'L1_J40', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j100',                    'L1_J25',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j125',                    'L1_J50', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j200',                    'L1_J50', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j300',                    'L1_J85', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        
        # 2017 calibration central jet triggers
        ['j175_subjesIS',           'L1_J50', [], [PhysicsStream], ['Rate:SingleJet', 'BW:Jet'], -1],
        ['j175_lcw_subjesIS',       'L1_J50', [], [PhysicsStream], ['Rate:SingleJet', 'BW:Jet'], -1],
        ['j420_subjesIS',           'L1_J100', [], [PhysicsStream], ['Rate:SingleJet', 'BW:Jet'], -1],
        ['j420_lcw_subjesIS',       'L1_J100', [], [PhysicsStream], ['Rate:SingleJet', 'BW:Jet'], -1],
        
        # Alternative calibration central jet triggers
        ['j85_nojcalib',            'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j85_lcw',                 'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j85_lcw_jes',             'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j85_lcw_nojcalib',        'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j175_jes',                'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j175_nojcalib',           'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j175_lcw',                'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j175_lcw_jes',            'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j175_lcw_nojcalib',       'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j300_lcw_nojcalib',       'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
 
        # Standard forward jet triggers
	['j10_320eta490',           'L1_RD0_FILLED',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j25_320eta490_L1TE5',     'L1_TE5',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j35_320eta490_L1TE10',    'L1_TE10',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j45_320eta490_L1TE20',    'L1_TE20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
	['j55_320eta490_L1TE20',    'L1_TE20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        # SoftKiller validation and comparison chains
        ['j60_sktc_em_nojcalib',    'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j60_tc_em_nojcalib',      'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j60_tc_em_sub',           'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j60_sktc_lcw_nojcalib',   'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j60_tc_lcw_nojcalib',     'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j60_tc_lcw_sub',          'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        # Moved from Physics menu
        ['j35_sktc_lcw_nojcalib',       'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],



        #####
        ######################### Single-jet large-R trigger chains
        #####

        # Alternative calibration topocluster large-R jet triggers
        ['j300_a10_lcw_sub_L1J75',          'L1_J75', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j360_a10_sub_L1J100',             'L1_J100', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j360_a10_lcw_sub_L1J100',         'L1_J100', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j380_a10_lcw_sub_L1J100',         'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j400_a10_sub_L1J100',             'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j400_a10_lcw_sub_L1J100',         'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j420_a10_sub_L1J100',             'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j420_a10_lcw_sub_L1J100',         'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j440_a10_lcw_sub_L1J100',         'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        # L1Topo simple cone seeded topocluster large-R jet triggers
        # See ATR-14353 for details of the request
        ['j300_a10_lcw_subjes_L1SC85',      'L1_SC85-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j360_a10_lcw_subjes_L1SC111',     'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j360_a10_lcw_sub_L1SC111',        'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j400_a10_lcw_sub_L1SC111',        'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j420_a10_lcw_sub_L1SC111',        'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10_lcw_sub_L1SC111',        'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        
        # L1Topo HT seeded topocluster large-R jet triggers
        ['j360_a10_nojcalib',                       'L1_HT150-J20s5.ETA31', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j360_a10_nojcalib_L1HT150-J20.ETA31',     'L1_HT150-J20.ETA31', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j360_a10_lcw_nojcalib',                   'L1_HT150-J20s5.ETA31', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j360_a10_lcw_nojcalib_L1HT150-J20.ETA31', 'L1_HT150-J20.ETA31', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j460_a10_nojcalib',                       'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j460_a10_nojcalib_L1HT190-J15.ETA21',     'L1_HT190-J15.ETA21', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j460_a10_lcw_nojcalib',                   'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j460_a10_lcw_nojcalib_L1HT190-J15.ETA21', 'L1_HT190-J15.ETA21', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j360_a10_sub',                            'L1_HT150-J20s5.ETA31', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j360_a10_sub_L1HT150-J20.ETA31',          'L1_HT150-J20.ETA31', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j360_a10_lcw_sub',                        'L1_HT150-J20s5.ETA31', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j360_a10_lcw_sub_L1HT150-J20.ETA31',      'L1_HT150-J20.ETA31', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j460_a10_sub',                            'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j460_a10_sub_L1HT190-J15.ETA21',          'L1_HT190-J15.ETA21', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j460_a10_lcw_sub',                        'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j460_a10_lcw_sub_L1HT190-J15.ETA21',      'L1_HT190-J15.ETA21', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        
        # L1Topo HT seeded reclustered large-R jet triggers
        ['j360_a10r',                       'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10r',                       'L1_HT150-J20s5.ETA31', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],


        # Trimmed topocluster large-R jet triggers
        ['j260_a10t_nojcalib_L1J75',        'L1_J75',  [], [PhysicsStream], ['Rate:SingleJet',  'BW:Jet'], -1],
        ['j460_a10t_nojcalib_L1J100',       'L1_J100', [], [PhysicsStream], ['Rate:SingleJet',  'BW:Jet'], -1],
        ['j460_a10t_lcw_nojcalib_L1J100',   'L1_J100', [], [PhysicsStream], ['Rate:SingleJet',  'BW:Jet'], -1],


        
        #####
        ######################### Large-R jet trigger chains with mass cuts and support chains
        #####
        
        # Validation chains, not intended for physics usage
        ['j100_a10_lcw_subjes',             'L1_J20', [], [PhysicsStream], ['Rate:SingleJet', 'BW:Jet'], -1],
        ['j100_a10_lcw_subjes_30smcINF',    'L1_J20', [], [PhysicsStream], ['Rate:SingleJet', 'BW:Jet'], -1],
        ['j100_a10r',                       'L1_J20', [], [PhysicsStream], ['Rate:SingleJet', 'BW:Jet'], -1],
        ['j100_a10r_30smcINF',              'L1_J20', [], [PhysicsStream], ['Rate:SingleJet', 'BW:Jet'], -1],
        ['j100_a10t_lcw_jes',               'L1_J20', [], [PhysicsStream], ['Rate:SingleJet', 'BW:Jet'], -1],
        ['j100_a10t_lcw_jes_30smcINF',      'L1_J20', [], [PhysicsStream], ['Rate:SingleJet', 'BW:Jet'], -1],



        #####
        ######################### Multi-jet small-R trigger chains
        #####

        # 2-jet single threshold triggers
	['2j10',                'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['2j15',                'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['2j20',                'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['2j25',                'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	
        # 2-jet single threshold forward triggers
	['2j10_320eta490',      'L1_RD0_FILLED',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['2j15_320eta490',      'L1_RD0_FILLED',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['2j20_320eta490',      'L1_RD0_FILLED',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['2j25_320eta490',      'L1_RD0_FILLED',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['2j25_320eta490_L1TE5','L1_TE5',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['2j35_320eta490',      'L1_TE10',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],

	# 3-jet single threshold triggers
        ['3j30',                'L1_TE10', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['3j40',                'L1_TE20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['3j50',                'L1_TE30', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        # 3-jet mixed threshold triggers
	['j75_2j30',            'L1_J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j75_2j40',            'L1_J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j100_2j30',           'L1_J25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],	
	['j100_2j40',           'L1_J25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j100_2j50',           'L1_J25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
	['j150_2j50',           'L1_J40', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],



        #####
        ######################### HT trigger chains
        #####

        # L1topo seeded HT triggers
        ['ht400',                       'L1_HT150-J20s5.ETA31', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['ht400_L1HT150-J20.ETA31',     'L1_HT150-J20.ETA31', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['ht550',                       'L1_HT150-J20s5.ETA31', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['ht550_L1HT150-J20.ETA31',     'L1_HT150-J20.ETA31', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['ht700',                       'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['ht700_L1HT190-J15.ETA21',     'L1_HT190-J15.ETA21', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['ht850',                       'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['ht850_L1HT190-J15.ETA21',     'L1_HT190-J15.ETA21', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        #['ht1000',                      'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        #['ht1000_L1HT190-J15.ETA21',    'L1_HT190-J15.ETA21', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],


        #####
        ######################### Analysis-specific jet trigger chains
        #####

        # Dijet invariant mass range plus y* cut triggers, mainly aimed at the dijet trigger-level analysis
        ['j0_0i1c200m400TLA',           'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j0_0i1c400m600TLA',           'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j0_0i1c600m800TLA',           'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j0_0i1c800m1000TLA',          'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j0_1i2c100m300TLA',           'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j0_1i2c500m700TLA',           'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j0_1i2c100m8000TLA',          'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j0_1i2c500m900TLA',           'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	['j0_1i2c600m800TLA',           'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
	
        # Analysis-driven multijet triggers
        ['j85_2j45',                    'L1_J20', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j45_0eta240_2j45_320eta490_L1J15.0ETA25_2J15.31ETA49', 'L1_J15.0ETA25_2J15.31ETA49',[] , [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'],  -1,['serial',-1,["j45_0eta240","2j45_320eta490_L1J15"]]],

        # VBF triggers
        ['j80_0eta240_2j60_320eta490_invm700',  'L1_J40.0ETA25_2J15.31ETA49',['',''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet' ], -1 ], #Bjet rate but needs to be in the Jet slice



        #####
        ######################### Specialty jet trigger chains (mostly for performance studies)
        #####

        # Central+forward (dijet) triggers
        ['j35_j35_320eta490',              'L1_RD0_FILLED',      [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j55_j55_320eta490_L1RD0_FILLED', 'L1_RD0_FILLED',      [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j85_j85_320eta490',              'L1_J20_J20.31ETA49', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],

        # Partial scan 
        ['j200_jes_PS',     'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],         
        ['3j175_jes_PS',    'L1_J100', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],





        # Delayed stream dijet invariant mass range plus y* cut triggers, mainly aimed at the dijet trigger-level analysis
        # See ATR-14091 for details of the request
        ['j0_0i1c500m900TLA_delayed',       'L1_J100',  [], ['ExoDelayed'], ['RATE:ExoDelayed', 'BW:Jet'], -1],
        ['j0_1i2c200m8000TLA_delayed',      'L1_J100',  [], ['ExoDelayed'], ['RATE:ExoDelayed', 'BW:Jet'], -1],
        ['j0_0i1c500m2000TLA_delayed',      'L1_J100',  [], ['ExoDelayed'], ['RATE:ExoDelayed', 'BW:Jet'], -1],

        ['2j250_j140',                      'L1_J100', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['2j250_j150',                      'L1_J100', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['2j275_j120',                      'L1_J100', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['2j220_0eta240_j120_0eta240',      'L1_J100', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['2j250_0eta240_j120_0eta240',      'L1_J100', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['2j250_0eta240_j140_0eta240',      'L1_J100', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['2j275_0eta240_j120_0eta240',      'L1_J100', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['2j275_0eta240_j140_0eta240',      'L1_J100', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        # VBF triggers
        ['2j40_0eta490_invm250_L1XE55',     'L1_XE55', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],

        # Central+forward (dijet) triggers
        ['j15_j15_320eta490',   'L1_RD0_FILLED',      [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j25_j25_320eta490',   'L1_RD0_FILLED',      [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j45_j45_320eta490',   'L1_RD0_FILLED',      [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
			 ]


    if TriggerFlags.doFTK():
        TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + [
            ['j100_ftk',                   'L1_J25',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
            ['j75_ftk_L1J20',              'L1_J20', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
            ] 

    TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [

        # 
        #  1b / 1j
        # 

        # perf / bperf
        ['j0_perf_bperf_L1RD0_EMPTY',  'L1_RD0_EMPTY',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j0_perf_bperf_L1MU10',       'L1_MU10',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j0_perf_bperf_L1J12_EMPTY',  'L1_J12_EMPTY',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

        # bperf
        ['j35_bperf',  'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j55_bperf',  'L1_J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j150_bperf', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j320_bperf', 'L1_J85', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j25_bperf', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j45_bperf', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j110_bperf', 'L1_J30', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1], 
        ['j175_bperf', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_bperf', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j260_bperf', 'L1_J75', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j300_bperf', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j400_bperf', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],


        # boffperf
        ['j25_boffperf', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j45_boffperf', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j110_boffperf', 'L1_J30', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1], 
        ['j175_boffperf', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_boffperf', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j260_boffperf', 'L1_J75', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j300_boffperf', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j400_boffperf', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],


        # bperf_split
        ['j35_bperf_split',  'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j55_bperf_split',  'L1_J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j150_bperf_split', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j320_bperf_split', 'L1_J85', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j25_bperf_split', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j45_bperf_split', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j110_bperf_split', 'L1_J30', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j175_bperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_bperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j260_bperf_split', 'L1_J75', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j300_bperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j400_bperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

        # boffperf_split
        ['j300_boffperf_split', 'L1_J75', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        
        # tagged
        ['j175_bmv2c1085', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],                                                      
        ['j175_bmv2c1085_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],                                                
        ['j225_bloose_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j300_bloose_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

        #
        #  1b / 3j
        #
        # None

        # 
        #  1b / 4j
        # 

        ['j35_bperf_3j35', 'L1_3J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j35_boffperf_3j35', 'L1_3J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

        ['j45_bperf_3j45_L14J20', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

        ['j50_bmv2c1040_split_3j50', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

        ['j65_bperf_3j65_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j65_boffperf_3j65_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j65_bperf_split_3j65_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j65_boffperf_split_3j65_L14J15.0ETA25',            'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j65_bmv2c1070_split_3j65_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],


        ['j70_bperf_3j70', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j70_bperf_3j70_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j70_boffperf_3j70_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j70_boffperf_3j70', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j70_bperf_split_3j70_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j70_bperf_split_3j70', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j70_boffperf_split_3j70_L14J15.0ETA25',            'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j70_boffperf_split_3j70', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j70_boffperf_split_3j70_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j70_bmv2c1070_split_3j70_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j70_bmv2c1070_split_3j70', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j70_bmv2c1077_split_3j70_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],


        ['j75_bperf_3j75', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j75_bperf_3j75_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j75_boffperf_3j75', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j75_boffperf_3j75_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j75_bperf_split_3j75', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j75_bperf_split_3j75_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j75_boffperf_split_3j75', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j75_boffperf_split_3j75_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j75_bmv2c1070_split_3j75', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j75_bmv2c1077_split_3j75_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j75_bmv2c1077_split_3j75', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

        # 
        #  2b / 2j
        #
        ['j80_bmv2c1085_split_j60_bmv2c1085_split_j45_320eta490', 'L1_J40.0ETA25_2J25_J20.31ETA49', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

        ['j150_bperf_j50_bperf', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j150_boffperf_j50_boffperf', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j150_bperf_split_j50_bperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

        ['j175_bperf_j60_bperf', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j175_boffperf_j60_boffperf', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j175_bperf_split_j60_bperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j175_boffperf_split_j60_boffperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j175_bmedium_split_j60_bmedium_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

        #
        # 2b / 3j
        #
        ['j150_2j55_bperf',          'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j150_2j55_boffperf',       'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j150_2j55_bperf_split',    'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j150_2j55_boffperf_split', 'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j150_2j55_bmedium_split',  'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j150_2j55_bmv2c1077',      'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

        ['2j65_bperf_j65', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j65_boffperf_j65', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j65_bperf_split_j65', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j65_boffperf_split_j65',                          'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

        ['2j70_bperf_j70', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j70_boffperf_j70', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j70_bperf_split_j70', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j70_boffperf_split_j70', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

        ['2j75_bperf_j75', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j75_boffperf_j75', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j75_bperf_split_j75', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1], 
        ['2j75_boffperf_split_j75', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

        #
        # 2b / 4j
        #
        ['2j35_bperf_2j35_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j35_boffperf_2j35_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j35_bperf_split_2j35_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j35_bmv2c1060_split_2j35', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j35_bmv2c1070_split_2j35_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j35_bmv2c1070_2j35_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],                             
        ['2j35_bmv2c1070_split_2j35_L14J20', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        

        ['2j45_bperf_2j45', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j45_bperf_2j45_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j45_boffperf_2j45', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j45_boffperf_2j45_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j45_bperf_split_2j45', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j45_bperf_split_2j45_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j45_boffperf_split_2j45_L14J15.0ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet'], -1],        
        ['2j45_boffperf_split_2j45', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j45_boffperf_split_2j45_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j45_bmv2c1070_split_2j45', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j45_btight_split_2j45_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j45_bmv2c1070_split_2j45_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j45_bmv2c1077_split_2j45_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],


        ['2j55_bperf_2j55', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_bperf_2j55_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_boffperf_2j55', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_boffperf_2j55_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_bperf_split_2j55', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_bperf_split_2j55_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_boffperf_split_2j55', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_boffperf_split_2j55_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_bmedium_split_2j55_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_bmv2c1077_split_2j55', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_bmv2c1077_split_2j55_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],


        # 2b / 5j
        ['2j45_boffperf_split_3j45_L14J15.0ETA25',          'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet'], -1],
        
        #  MV2c20 validation
        ['j175_gsc225_bmv2c2040_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j175_gsc225_bmv2c2050_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j175_gsc225_bmv2c2060_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j175_gsc225_bmv2c2070_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j175_gsc225_bmv2c2077_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j175_gsc225_bmv2c2085_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],


        ############
       
        #
        #  muon + bjet
        #

        
        #
        #   2b + VBF Jets
        #
        ['2j55_bperf_split_L13J20_4J20.0ETA49_MJJ-400', 'L1_3J20_4J20.0ETA49_MJJ-400', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_bperf_split_L13J20_4J20.0ETA49_MJJ-700', 'L1_3J20_4J20.0ETA49_MJJ-700', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_bperf_split_L13J20_4J20.0ETA49_MJJ-800', 'L1_3J20_4J20.0ETA49_MJJ-800', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_bperf_split_L13J20_4J20.0ETA49_MJJ-900', 'L1_3J20_4J20.0ETA49_MJJ-900', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

        
        ['2j55_boffperf_split_L13J20_4J20.0ETA49_MJJ-400', 'L1_3J20_4J20.0ETA49_MJJ-400', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_boffperf_split_L13J20_4J20.0ETA49_MJJ-700', 'L1_3J20_4J20.0ETA49_MJJ-700', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_boffperf_split_L13J20_4J20.0ETA49_MJJ-800', 'L1_3J20_4J20.0ETA49_MJJ-800', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_boffperf_split_L13J20_4J20.0ETA49_MJJ-900', 'L1_3J20_4J20.0ETA49_MJJ-900', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],




        # L1Topo VBF MJJ bjets
        ['2j55_bmv2c1085_split_L13J20_4J20.0ETA49_MJJ-400', 'L1_3J20_4J20.0ETA49_MJJ-400', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_bperf_L13J20_4J20.0ETA49_MJJ-400', 'L1_3J20_4J20.0ETA49_MJJ-400', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_bmv2c1085_split_L13J20_4J20.0ETA49_MJJ-700', 'L1_3J20_4J20.0ETA49_MJJ-700', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_bperf_L13J20_4J20.0ETA49_MJJ-700', 'L1_3J20_4J20.0ETA49_MJJ-700', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_bmv2c1085_split_L13J20_4J20.0ETA49_MJJ-800', 'L1_3J20_4J20.0ETA49_MJJ-800', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_bperf_L13J20_4J20.0ETA49_MJJ-800', 'L1_3J20_4J20.0ETA49_MJJ-800', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_bmv2c1085_split_L13J20_4J20.0ETA49_MJJ-900', 'L1_3J20_4J20.0ETA49_MJJ-900', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_bperf_L13J20_4J20.0ETA49_MJJ-900', 'L1_3J20_4J20.0ETA49_MJJ-900', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_bmv2c1077_split_L13J20_4J20.0ETA49_MJJ-400', 'L1_3J20_4J20.0ETA49_MJJ-400', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_bmv2c1077_split_L13J20_4J20.0ETA49_MJJ-700', 'L1_3J20_4J20.0ETA49_MJJ-700', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_bmv2c1077_split_L13J20_4J20.0ETA49_MJJ-800', 'L1_3J20_4J20.0ETA49_MJJ-800', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_bmv2c1077_split_L13J20_4J20.0ETA49_MJJ-900', 'L1_3J20_4J20.0ETA49_MJJ-900', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

      
        
        ['j25_gsc45_boffperf_split',   'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j50_gsc65_boffperf_split', 'L1_J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j60_gsc85_boffperf_split', 'L1_J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j110_gsc150_boffperf_split', 'L1_J30', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j150_gsc175_boffperf_split', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j175_gsc260_boffperf_split', 'L1_J75', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc320_boffperf_split', 'L1_J75', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        # NOTE: gsc400 and higher boffperf chains are candidates for jet primaries
        # As such, they have been switched to RATE:SingleJet and BW:Jet, and moved to the jet section

        #
        # tagged 
        #
        ['j225_bmv2c1040_split',        'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j275_bmv2c1060_split',        'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j300_bmv2c1070_split',        'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j360_bmv2c1077_split',        'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],                   
        ['j175_gsc225_bmv2c1060_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc300_bmv2c1085_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

        ['j225_gsc400_bmv2c1085_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],                   


        #
        #  1b / 3j
        #
        ['j55_gsc80_bmv2c1085_split_2j60_320eta490', 'L1_J40.0ETA25_2J15.31ETA49', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j50_gsc65_bmv2c1050_split_3j50_gsc65_boffperf_split',      'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j55_gsc75_bmv2c1060_split_3j55_gsc75_boffperf_split',       'L1_4J20',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j55_gsc75_bmv2c1070_split_3j55_gsc75_boffperf_split',       'L1_4J20',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j60_gsc85_bmv2c1070_split_3j60_gsc85_boffperf_split',       'L1_4J20',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j60_gsc85_bmv2c1077_split_3j60_gsc85_boffperf_split',       'L1_4J20',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        
        # 
        #  2b / 2j
        #

        ['j175_bmv2c1060_split_j60_bmv2c1060_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j150_gsc175_bmv2c1077_split_j45_gsc60_bmv2c1077_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

        #
        # 2b / 3j
        #

        ['2j55_gsc75_boffperf_split_j55_gsc75_boffperf_split_L13J40', 'L1_3J40', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j50_gsc70_boffperf_split_j50_gsc70_boffperf_split_L13J40', 'L1_3J40', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

        ['j110_gsc150_boffperf_split_2j35_gsc55_boffperf_split',  'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1050_split', 'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1060_split', 'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1070_split', 'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j110_gsc150_boffperf_split_2j35_gsc55_bmv2c1077_split', 'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

        ['2j55_gsc75_boffperf_split_j55_gsc75_boffperf_split',         'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

        #
        # 2b / 4j
        #
        ['2j35_boffperf_split_2j35_L14J20', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j15_gsc35_boffperf_split_2j15_gsc35_boffperf_split_L14J20', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

        ['2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L14J20',        'L1_4J20',        [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j15_gsc35_bmv2c1050_split_2j15_gsc35_boffperf_split_L14J20',        'L1_4J20',        [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j15_gsc35_bmv2c1060_split_2j15_gsc35_boffperf_split_L14J20',        'L1_4J20',        [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j25_gsc45_bmv2c1070_split_2j25_gsc45_boffperf_split_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j35_gsc55_bmv2c1077_split_2j35_gsc55_boffperf_split_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j15_gsc35_bmv2c1070_split_3j15_gsc35_boffperf_split', 'L1_5J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j15_gsc35_bmv2c1077_split_3j15_gsc35_boffperf_split', 'L1_5J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j25_gsc45_bmv2c1077_split_3j25_gsc45_boffperf_split',                 'L1_5J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j25_gsc45_bmv2c1077_split_3j25_gsc45_boffperf_split_L14J15.0ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

        ['3j50_gsc65_bmv2c1070_split_L13J35.0ETA23', 'L1_3J35.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['3j50_gsc65_bmv2c1085_split_L13J35.0ETA23', 'L1_3J35.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

        ['3j15_gsc35_bmv2c1060_split_j15_gsc35_boffperf_split', 'L1_4J20',        [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['3j15_gsc35_bmv2c1070_split_j15_gsc35_boffperf_split', 'L1_4J20',        [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['3j15_gsc35_bmv2c1077_split_j15_gsc35_boffperf_split', 'L1_4J20',        [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
			 ]

    if TriggerFlags.doFTK():
        TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [
           # FTK boffperf chains
            ['j35_boffperf_split_FTK', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
            ['j150_boffperf_split_FTK', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
            ['j225_boffperf_split_FTK',    'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
            
            ['j35_boffperf_split_FTKVtx', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
            ['j150_boffperf_split_FTKVtx', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
            ['j225_boffperf_split_FTKVtx',    'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
            
            ['j35_boffperf_split_FTKRefit', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
            ['j150_boffperf_split_FTKRefit', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
            ['j225_boffperf_split_FTKRefit',    'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
            
            # support chains
            ['2j35_boffperf_2j35_FTK_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
            ['j150_boffperf_j50_boffperf_FTK', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
            
            ['2j35_boffperf_2j35_FTKVtx_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
            ['j150_boffperf_j50_boffperf_FTKVtx', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
            
            ['2j35_boffperf_2j35_FTKRefit_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
            ['j150_boffperf_j50_boffperf_FTKRefit', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
            
            # FTK physics chains
            ['2j35_bmv2c1050_2j35_FTK_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
            ['j225_bmv2c1050_FTK', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
            ['j150_bmv2c1050_j50_bmv2c1050_FTK', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
            
            ['2j35_bmv2c1050_2j35_FTKVtx_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
            ['j225_bmv2c1050_FTKVtx', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
            ['j150_bmv2c1050_j50_bmv2c1050_FTKVtx', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
            
            ['2j35_bmv2c1050_2j35_FTKRefit_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
            ['j225_bmv2c1050_FTKRefit', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
            ['j150_bmv2c1050_j50_bmv2c1050_FTKRefit', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
            ] # Additional FTK for BjetSlise


    TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + [
        ['xe90_L1XE60',                'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],  

 	['xe95_pueta_wEFMu_L1XE50',               'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

 	['xe90_tc_lcw_L1XE60',               'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe90_pueta_L1XE60',                 'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1], 
 	['xe90_mht_L1XE60',                   'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],	
 	['xe100_pueta_L1XE60',                 'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1], 

 	['xe110_tc_lcw_L1XE60',               'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe110_pueta_L1XE60',                'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe110_mht_L1XE60',                  'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe90_mht_L1XE40',                  'L1_XE40',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ### ATR- 15096

 	['xe50_mht_FStracks_L1XE50',                  'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe110_mht_FStracks_L1XE50',                  'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe50_pueta',                             'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ## add chains for MC16 (ATR-15096)
        ['xe100_pufit_L1KF-XE50',           'L1_KF-XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pufit_L1KF-XE55',           'L1_KF-XE55',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],


        ####default chains

        ['xe90',                                   'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_tc_lcw',             'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_tc_em',              'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_mht',                'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_pueta',              'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe90_wEFMu',              'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_tc_lcw_wEFMu',       'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_tc_em_wEFMu',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_mht_wEFMu',          'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_pueta_wEFMu',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_pufit_wEFMu',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe90_tc_em_L1XE50',              'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_pueta_L1XE50',              'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe90_wEFMu_L1XE50',              'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_tc_em_wEFMu_L1XE50',        'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_pueta_wEFMu_L1XE50',        'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_lcw',             'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_em',              'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_mht',                'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pueta',              'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe100_wEFMu',              'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_lcw_wEFMu',       'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_em_wEFMu',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_mht_wEFMu',          'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pueta_wEFMu',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pufit_wEFMu',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],


        ###xe100 from L1_XE80
        ['xe100_L1XE80',                    'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_lcw_L1XE80',             'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_em_L1XE80',              'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_mht_L1XE80',                'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pueta_L1XE80',              'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pufit_L1XE80',              'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe100_wEFMu_L1XE80',              'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_lcw_wEFMu_L1XE80',       'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_em_wEFMu_L1XE80',        'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_mht_wEFMu_L1XE80',          'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pueta_wEFMu_L1XE80',        'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pufit_wEFMu_L1XE80',        'L1_XE80', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

#        ['xe0noL1_l2fsperf_trkmht_FTK', '', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

   #L1_XE35
        ['xe35', 'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_tc_lcw', 'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_tc_em', 'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_pufit', 'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_mht', 'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_mht_em', 'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

#Supporting MET triggers
        ['xe45_pufit_wEFMu',          'L1_XE45',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe45_wEFMu',                'L1_XE45',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe45_mht_wEFMu',            'L1_XE45',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe45_tc_lcw',               'L1_XE45',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe45_tc_em',                'L1_XE45',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe45_mht',                  'L1_XE45',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe45',                      'L1_XE45',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe45_tc_lcw_wEFMu',         'L1_XE45',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe45_tc_em_wEFMu',          'L1_XE45',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe45_pufit',                'L1_XE45',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe45_mht_xe45', 'L1_XE45',['L1_XE45','L1_XE45'], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1, ['serial',-1,['xe45_mht','xe45_L1XE45']]],
			 ]

        

    TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [
        ['tau35_perf_tracktwo_tau25_perf_tracktwo_ditauL', 'L1_TAU20IM_2TAU12IM' , ['L1_TAU20IM','L1_TAU12IM'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
        ['tau35_perf_tracktwo_tau25_perf_tracktwo_ditauT', 'L1_TAU20IM_2TAU12IM' , ['L1_TAU20IM','L1_TAU12IM'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
                                                                       
                                                                             
        ['tau25_idperf_tracktwo2015',                        'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_tracktwo2015',                        'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_tracktwo2015',                        'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],

        # Run-I comparison
        ['tau20_r1medium1',                        'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau20_r1perf',                           'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_r1perf',                           'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau29_r1medium1',                        'L1_TAU20', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau29_r1perf',                           'L1_TAU20', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_r1medium1',                       'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_r1perf',                          'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        # Run-II - No BDT: variations
        ['tau25_perf_calo',                        'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_calo_L1TAU12',                'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_trackcalo',                   'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_tracktwocalo',                'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        # Run-II - With BDT: main calo-based items
        ['tau25_medium1_calo',                     'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_trackcalo',                'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_tracktwocalo',             'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        # Run-II - High-pT variations
        ['tau35_medium1_calo',                     'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_medium1_calo',                     'L1_TAU40', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_medium1_calo',                    'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],

        #photon+tau chain (ATR-11531) 
        ['tau25_dikaon_tracktwo',                    'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_dikaon_tracktwo_L1TAU12',            'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_dikaontight_tracktwo',                    'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_dikaontight_tracktwo_L1TAU12',            'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],

        # Ditau Items
        ['tau35_perf_ptonly_tau25_perf_ptonly_L1TAU20IM_2TAU12IM',   'L1_TAU20IM_2TAU12IM',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_perf_ptonly","tau25_perf_ptonly"]]],

        ['tau35_loose1_tracktwo_tau25_loose1_tracktwo_L1TAU20IM_2TAU12IM',   'L1_TAU20IM_2TAU12IM',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_loose1_tracktwo","tau25_loose1_tracktwo"]]],
        ['tau35_tight1_tracktwo_tau25_tight1_tracktwo_L1TAU20IM_2TAU12IM',   'L1_TAU20IM_2TAU12IM',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwo","tau25_tight1_tracktwo"]]],

        # Works now
        ['tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12',   'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo_L1TAU20","tau25_medium1_tracktwo_L1TAU12"]]],

	#L1Topo TAU+TAU
        ['tau35_loose1_tracktwo_tau25_loose1_tracktwo_L1TAU20ITAU12I-J25',   'L1_TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_loose1_tracktwo","tau25_loose1_tracktwo"]]], 

        ['tau35_loose1_tracktwo_tau25_loose1_tracktwo_L1DR-TAU20ITAU12I',   'L1_DR-TAU20ITAU12I',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_loose1_tracktwo","tau25_loose1_tracktwo"]]],
        ['tau35_loose1_tracktwo_tau25_loose1_tracktwo_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_loose1_tracktwo","tau25_loose1_tracktwo"]]],

	# new L1Topo chains with 0DR25 (ATR-14349,ATR-15225)
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_03dR27_L1DR25-TAU20ITAU12I',   'L1_DR25-TAU20ITAU12I',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_03dR27_L1DR25-TAU20ITAU12I-J25',   'L1_DR25-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
        ['tau35_tight1_tracktwo_tau25_tight1_tracktwo_03dR27_L1DR25-TAU20ITAU12I',   'L1_DR25-TAU20ITAU12I',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwo","tau25_tight1_tracktwo"]]],
        ['tau35_tight1_tracktwo_tau25_tight1_tracktwo_03dR27_L1DR25-TAU20ITAU12I-J25',   'L1_DR25-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwo","tau25_tight1_tracktwo"]]],

        ['tau80_medium1_tracktwo_L1TAU60_tau25_medium1_tracktwo_L1TAU12','L1_TAU60',['L1_TAU60','L1_TAU12'],[PhysicsStream],['RATE:MultiTau', 'BW:Tau'],-1,['serial',-1,["tau80_medium1_tracktwo_L1TAU60","tau25_medium1_tracktwo_L1TAU12"]]],

        ['tau35_perf_tracktwo_L1TAU20_tau25_perf_tracktwo_L1TAU12', 'L1_TAU20_2TAU12' , ['L1_TAU20','L1_TAU12'],[PhysicsStream], ['RATE:MultiTau','BW:Tau'], -1],
        # Energy calibration chain
        ['tau5_perf_ptonly_L1TAU8',                'L1_TAU8', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        # Run-I comparison
        ['tau20_r1_idperf',                        'L1_TAU12', [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Tau', 'BW:ID'], -1],
        ['tau25_r1_idperf',                        'L1_TAU12', [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Tau', 'BW:ID'], -1],
        ['tau25_perf_track_L1TAU12',               'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        # Run-II - With BDT: no pre-selection
        ['tau25_medium1_mvonly',                   'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_loose1_ptonly',                    'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_track',                    'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_tracktwo_L1TAU20',         'L1_TAU20', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_ptonly',                   'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_ptonly_L1TAU20',           'L1_TAU20', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_tight1_tracktwo',                  'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_tight1_ptonly',                    'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_perf_tracktwo',                    'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_perf_ptonly',                      'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_perf_tracktwo',                   'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_perf_ptonly',                     'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_perf_track',                      'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_medium1_track',                    'L1_TAU40', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_medium1_track',                   'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        # Run-II - No BDT: variations
        ['tau25_perf_ptonly',                      'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_ptonly_L1TAU12',              'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        # Run-II - With BDT: main track-based items
        ['tau25_medium1_track',                    'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        # Run-II - With BDT: main calo-based items
        # Run-II - With BDT: pt only
        ['tau25_loose1_ptonly',                    'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_ptonly',                   'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_tight1_ptonly',                    'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_singlepiontight_tracktwo',                    'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],

        #tau+photon (ATR-13061)
        ['tau35_dikaonmasstight_tracktwo_L1TAU12',            'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_dikaonmass_tracktwo_L1TAU12',            'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_dikaonmasstight_tracktwo',                    'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_dipion1loose_tracktwo_L1TAU12',  'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_dipion2_tracktwo_L1TAU12',       'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_kaonpi1_tracktwo_L1TAU12',       'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_kaonpi2_tracktwo_L1TAU12',       'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
			 ]

    if TriggerFlags.doFTK():
            TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [
                
                ['tau20_medium0_FTK_tau12_medium0_FTK_03dR30_L1DR-TAU20ITAU12I-J25_FTK', 'L1_DR-TAU20ITAU12I-J25_FTK' , ['L1_TAU20IM_FTK','L1_TAU12IM_FTK'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
                ['tau20_medium1_FTK_tau12_medium1_FTK_03dR30_L1DR-TAU20ITAU12I-J25_FTK', 'L1_DR-TAU20ITAU12I-J25_FTK' , ['L1_TAU20IM_FTK','L1_TAU12IM_FTK'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
                ['tau20_medium0_FTK_tau12_medium0_FTK_03dR30_ditauL_L1DR-TAU20ITAU12I-J25_FTK', 'L1_DR-TAU20ITAU12I-J25_FTK' , ['L1_TAU20IM_FTK','L1_TAU12IM_FTK'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
                ['tau20_medium1_FTK_tau12_medium1_FTK_03dR30_ditauL_L1DR-TAU20ITAU12I-J25_FTK', 'L1_DR-TAU20ITAU12I-J25_FTK' , ['L1_TAU20IM_FTK','L1_TAU12IM_FTK'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
                ['tau20_medium0_FTKNoPrec_tau12_medium0_FTKNoPrec_03dR30_L1DR-TAU20ITAU12I-J25_FTK', 'L1_DR-TAU20ITAU12I-J25_FTK' , ['L1_TAU20IM_FTK','L1_TAU12IM_FTK'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
                ['tau20_medium1_FTKNoPrec_tau12_medium1_FTKNoPrec_03dR30_L1DR-TAU20ITAU12I-J25_FTK', 'L1_DR-TAU20ITAU12I-J25_FTK' , ['L1_TAU20IM_FTK','L1_TAU12IM_FTK'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
                ['tau20_medium0_FTKNoPrec_tau12_medium0_FTKNoPrec_03dR30_ditauL_L1DR-TAU20ITAU12I-J25_FTK', 'L1_DR-TAU20ITAU12I-J25_FTK' , ['L1_TAU20IM_FTK','L1_TAU12IM_FTK'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
                ['tau20_medium1_FTKNoPrec_tau12_medium1_FTKNoPrec_03dR30_ditauL_L1DR-TAU20ITAU12I-J25_FTK', 'L1_DR-TAU20ITAU12I-J25_FTK' , ['L1_TAU20IM_FTK','L1_TAU12IM_FTK'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],

                ['tau20_medium0_FTK_tau12_medium0_FTK_L1TAU20IM_2TAU12IM_J25_2J20_3J12_FTK', 'L1_TAU20IM_2TAU12IM_J25_2J20_3J12_FTK' , ['L1_TAU20IM_FTK','L1_TAU12IM_FTK'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
                ['tau20_medium1_FTK_tau12_medium1_FTK_L1TAU20IM_2TAU12IM_J25_2J20_3J12_FTK', 'L1_TAU20IM_2TAU12IM_J25_2J20_3J12_FTK' , ['L1_TAU20IM_FTK','L1_TAU12IM_FTK'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
                ['tau20_medium0_FTK_tau12_medium0_FTK_ditauL_L1TAU20IM_2TAU12IM_J25_2J20_3J12_FTK', 'L1_TAU20IM_2TAU12IM_J25_2J20_3J12_FTK' , ['L1_TAU20IM_FTK','L1_TAU12IM_FTK'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
                ['tau20_medium1_FTK_tau12_medium1_FTK_ditauL_L1TAU20IM_2TAU12IM_J25_2J20_3J12_FTK', 'L1_TAU20IM_2TAU12IM_J25_2J20_3J12_FTK' , ['L1_TAU20IM_FTK','L1_TAU12IM_FTK'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
                ['tau20_medium0_FTKNoPrec_tau12_medium0_FTKNoPrec_L1TAU20IM_2TAU12IM_J25_2J20_3J12_FTK', 'L1_TAU20IM_2TAU12IM_J25_2J20_3J12_FTK' , ['L1_TAU20IM_FTK','L1_TAU12IM_FTK'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
                ['tau20_medium1_FTKNoPrec_tau12_medium1_FTKNoPrec_L1TAU20IM_2TAU12IM_J25_2J20_3J12_FTK', 'L1_TAU20IM_2TAU12IM_J25_2J20_3J12_FTK' , ['L1_TAU20IM_FTK','L1_TAU12IM_FTK'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
                ['tau20_medium0_FTKNoPrec_tau12_medium0_FTKNoPrec_ditauL_L1TAU20IM_2TAU12IM_J25_2J20_3J12_FTK', 'L1_TAU20IM_2TAU12IM_J25_2J20_3J12_FTK' , ['L1_TAU20IM_FTK','L1_TAU12IM_FTK'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
                ['tau20_medium1_FTKNoPrec_tau12_medium1_FTKNoPrec_ditauL_L1TAU20IM_2TAU12IM_J25_2J20_3J12_FTK', 'L1_TAU20IM_2TAU12IM_J25_2J20_3J12_FTK' , ['L1_TAU20IM_FTK','L1_TAU12IM_FTK'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
                
                ]


    TriggerFlags.EgammaSlice.signatures = TriggerFlags.EgammaSlice.signatures() + [

        ##########        
        # Single electron triggers


        # Single electron triggers


        ['e24_lhmedium_nod0_ringer_L1EM20VH',          'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_cutd0dphideta_L1EM20VH',        'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e24_lhtight_nod0_ivarloose_L1EM20VH',        'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhtight_cutd0dphideta_iloose_L1EM20VH',  'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e35_lhtight_nod0_ivarloose',        'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],  
        ['e40_lhtight_nod0_ivarloose',        'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],  
        ['e50_lhtight_nod0_ivarloose',        'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 

#        ['e26_lhtight_nod0_ringer_ivarloose',  'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        
        # topocone isolation
        ['e26_lhtight_nod0_ringer_ivarloose_icaloloose',  'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhmedium_nod0_ringer_ivarloose_icaloloose',  'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhtight_nod0_ivarloose_icaloloose',  'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhmedium_nod0_ivarloose_icaloloose',  'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhtight_nod0_icaloloose',  'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhmedium_nod0_icaloloose',  'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],


        ['e60_lhmedium_cutd0dphideta',         'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        # Di-electron triggers

        ['2e12_lhloose_nod0_L12EM10VH',          'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e12_lhloose_nodeta_L12EM10VH',        'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e12_lhloose_nodphires_L12EM10VH',     'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e12_lhloose_cutd0dphideta_L12EM10VH', 'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],

        ['2e17_lhloose_nod0',                    'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e17_lhloose_cutd0dphideta',           'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],

        # Trielectron triggers

        ['e17_lhmedium_nod0_ivarloose_2e9_lhmedium_nod0', 'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],

        # cut-based version of the following triggers staying in MC menu. The LH-based version kept in Physics menu. (ATR-14059)

        # 2e34 rerun items (e+MET triggers)
        ['e17_lhloose_nod0_ringer',            'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        # 2e34 rerun items (e+MET triggers)
        ['e60_lhloose_nod0_L1EM24VHI',  'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e70_lhloose_nod0_L1EM24VHI',  'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        # Single electron/photon chains for Calo sequence optimization (kept as before)
        ['g140_loose_L1EM24VHI',        'L1_EM24VHI', [], [PhysicsStream, 'express'], ['RATE:SinglePhoton','BW:Egamma'],-1],

        # Di-photon triggers

        ['2g20_tight_icalovloose',           'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g20_tight_icaloloose',            'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g20_tight_icalotight',            'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 

        ['2g22_tight_icaloloose',            'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 

        ['2g24_tight',                   'L1_2EM20VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g24_tight_icalovloose',           'L1_2EM20VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g24_tight_icaloloose',            'L1_2EM20VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g24_tight_icalotight',            'L1_2EM20VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 

        # Photon legs

        ['g20_tight_icalovloose',            'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_tight_icaloloose',             'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_tight_icalotight',             'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['g22_tight_icaloloose',             'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['g24_tight',                    'L1_EM20VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1], 
        ['g24_tight_icalovloose',            'L1_EM20VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g24_tight_icaloloose',             'L1_EM20VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g24_tight_icalotight',             'L1_EM20VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['g45_tight_icalovloose',             'L1_EM20VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g80_loose_icalovloose',             'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g85_tight_icalovloose',             'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g100_tight_icalovloose',            'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g45_tight_icalovloose_L1EM22VHI', 'L1_EM22VHI', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1],
        ['g45_tight_icalovloose_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1],
        ['g80_loose_icalovloose_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1],

        ['e20_mergedtight_g35_medium',           'L1_2EM15VH', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],

        ['e20_mergedtight',                      'L1_EM15VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],


        # Fall-back if problem with new tracking kept as before)
        ['e17_etcut_L1EM15',                      'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        
        # W T&P with new L1Topo
        ['e13_etcut_trkcut', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0',  ['L1_EM12'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e13_etcut_trkcut_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE',  ['L1_EM12'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        
        ##########
        # Monopole triggers
        #['g30_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['2g25_tight',                              'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],

        ['e17_lhmedium_nod0_ivarloose',                  'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['g30_loose',                     'L1_EM15VH',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_g35_loose',   'L1_2EM20VH', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 

        
        ['e13_etcut_L1W-NOMATCH',               'L1_W-NOMATCH', ['L1_EM12'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e18_etcut_trkcut_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE', 'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE', ['L1_EM15'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e5_etcut_L1W-05RO-XEHT-0',            'L1_W-05RO-XEHT-0',           ['L1_EM3'],  [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_etcut_L1W-90RO2-XEHT-0',           'L1_W-90RO2-XEHT-0',          ['L1_EM3'],  [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_etcut_L1W-250RO2-XEHT-0',          'L1_W-250RO2-XEHT-0',         ['L1_EM3'],  [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_etcut_L1W-HT20-JJ15.ETA49',        'L1_W-HT20-JJ15.ETA49',       ['L1_EM3'],  [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e13_etcut_L1EM12_W-MT25',             'L1_EM12_W-MT25',             ['L1_EM12'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e18_etcut_L1EM15_W-MT35',             'L1_EM15_W-MT35',             ['L1_EM15'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e18_etcut_trkcut', 'L1_EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE', ['L1_EM15'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],


        ['g0_hiptrt_L1EM20VH',                  'L1_EM20VH',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g0_hiptrt_L1EM20VHI',                 'L1_EM20VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ##['g20_tight',                     'L1_EM15VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],

        # Di-photon triggers
        ['g35_loose_L1EM15_g25_loose_L1EM15',       'L1_2EM15', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g20_tight',                              'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g20_tight_L12EM15VHI',                   'L1_2EM15VHI', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],

        # Tri-photon triggers
        ['2g20_loose_g15_loose',                    'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
 
        ['e5_lhloose',                          'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhtight_L1EM20VH',                 'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 

        ['e26_lhtight_nod0_ringer_ivarloose',  'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhtight_nod0_ringer_ivarloose_L1EM22VHIM',  'L1_EM22VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_nod0_ringer_ivarloose', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_nod0_ringer_ivarloose_L1EM24VHIM', 'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e60_medium',                           'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e60_medium_L1EM24VHI',               	 'L1_EM24VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e120_lhloose_nod0', 'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e80_medium',                     'L1_EM22VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e80_medium_L1EM24VHI',           'L1_EM24VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1 ],
        # Rerun mode
        ['e4_etcut',                             'L1_EM3',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e9_etcut',                             'L1_EM7',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e14_etcut',                            'L1_EM12',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e17_lhmedium_nod0_iloose',                  'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhloose_nod0_ringer',              'L1_EM22VHI',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
#        ['e26_lhmedium_nod0',              'L1_EM22VHI',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhmedium_nod0_ringer',              'L1_EM22VHI',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
#        ['e26_lhtight_nod0',              'L1_EM22VHI',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhtight_nod0_ringer',              'L1_EM22VHI',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

	

        # support for g+tau chains (ATR-13841)
        ['g35_medium_L1EM22VHI',                    'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_medium_L1EM24VHI',                    'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],

#Rerun:         
#Physics (tri-ele):
        ['e17_lhloose_nod0_2e9_lhloose_nod0',         'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],  
        ['e17_lhmedium_nod0_2e9_lhmedium_nod0',       'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],  
        ['e20_lhmedium_nod0_2g10_loose',           'L1_EM15VH_3EM7', ['L1_EM15VH','L1_2EM7'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        ['e20_lhmedium_nod0_2g10_medium',          'L1_EM15VH_3EM7', ['L1_EM15VH','L1_2EM7'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 

        #ATR-15259
        ['e25_mergedtight_g35_medium',           'L1_2EM20VH', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ['e30_mergedtight_g35_medium',           'L1_2EM20VH', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ['2g22_tight_L1EM7_UNPAIRED_NONISO',        'L1_EM7_UNPAIRED_NONISO', [], ["Late"], ['RATE:MultiPhoton', 'BW:Egamma'], -1],
        ['2g50_tight_L1EM7_UNPAIRED_NONISO',        'L1_EM7_UNPAIRED_NONISO', [], ["Late"], ['RATE:MultiPhoton', 'BW:Egamma'], -1],

        # L1Topo W T&P 
        ['e13_etcut_trkcut_L1EM12', 'L1_EM12', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        
     
        # ATR-12916, Egamma Trigger Menu 2016   

        ['e24_lhmedium_ivarloose_L1EM20VH',      'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_ivarloose_L1EM20VH', 'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhtight_cutd0dphideta_ivarloose', 'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        
        ['e60_medium_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e80_medium_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['2e17_lhvloose_nod0_ringer_L12EM15VHI',        'L1_2EM15VHI', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e17_lhvloose_nod0_ringer',          'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiElectron',  'BW:Egamma'],-1],
        ['2e19_lhvloose_nod0_ringer',        'L1_2EM15VHI', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e17_lhvloose_nod0_ringer_L1EM15VHI',        'L1_EM15VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhvloose_nod0_ringer',           'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e19_lhvloose_nod0_ringer',        'L1_EM15VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e26_lhmedium_nod0_ringer_ivarmedium_icalomedium',        'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e28_lhmedium_nod0_ringer_ivarmedium_icalomedium',        'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e60_lhmedium_nod0_ringer',           'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e60_lhmedium_nod0_ringer_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e140_lhloose_nod0_ringer',           'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e140_lhloose_nod0_ringer_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e30_mergedtight_ivarloose',           'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e26_lhtight_nod0_ivarloose_e15_etcut_L1EM7_Zee',    	  'L1_EM22VHI', ['L1_EM22VHI','L1_EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1 ],
        ['e28_lhtight_nod0_ivarloose_e15_etcut_L1EM7_Zee',    	  'L1_EM24VHI', ['L1_EM24VHI','L1_EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1 ],
        ['e26_lhtight_nod0_ivarloose_L1EM22VHIM_e15_etcut_L1EM7_Zee', 'L1_EM22VHIM',  ['L1_EM22VHIM','L1_EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_nod0_ivarloose_L1EM24VHIM_e15_etcut_L1EM7_Zee', 'L1_EM24VHIM',  ['L1_EM24VHIM','L1_EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
			 ]


    ###########################################################################################################
    #   Bphysics
    ###########################################################################################################
    TriggerFlags.BphysicsSlice.signatures = TriggerFlags.BphysicsSlice.signatures() + [
	
        #['2mu6_bMuMu_L12MU6', 'L1_2MU6', [], [PhysicsStream], [], -1], 

        ['2mu4_bBmumu_noL2',    'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu10_bBmumu_noL2',   'L1_2MU10', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],

        ['2mu4_bDimu_novtx_noos_noL2',    'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu6_mu4_bDimu_novtx_noos_noL2', 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bDimu_novtx_noos_noL2',    'L1_2MU6', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu10_bDimu_novtx_noos_noL2',   'L1_2MU10', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],

        ###########  2MU4  ################
        ['mu6_iloose_mu6_11invm60_noos', 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### ATR-14543
        ['mu6_ivarloose_mu6_11invm60_noos', 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        # chains with muon overlap removal
        ['2mu4_wOvlpRm_bDimu',                                 'L1_2MU4',                 [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu4_wOvlpRm_bDimu_novtx_noos',                      'L1_2MU4',                 [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu4_wOvlpRm_bBmumuxv2',                             'L1_2MU4',                         [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['3mu6_wOvlpRm_bDimu',                   'L1_3MU6', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu4_wOvlpRm_bDimu_noinvm_novtx_ss',    'L1_2MU4',     [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],    

        # chains with smart overlap removal , based on EF muons
        ['mu4_bNocut',                    'L1_MU4',            [], [BPhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['2mu4_bNocut',                    'L1_2MU4',  [], [BPhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu4_mu2noL1_bNocut', 'L1_3MU4', ['L1_3MU4',''], [BMultiMuonStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['3mu4','mu2noL1']]],
        ['3mu4_mu4noL1_bNocut', 'L1_3MU4', ['L1_3MU4',''], [BMultiMuonStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['3mu4','mu4noL1']]],

        ['mu20_2mu2noL1_bNocut', 'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','2mu2noL1_bNocut']]],

        
        ['mu20_2mu2noL1_bDimu', 'L1_MU20MU21', ['L1_MU20',''], [BPhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','2mu2noL1_bNocut']]],
        ['mu20_2mu2noL1_bDimu2700', 'L1_MU20MU21', ['L1_MU20',''], [BPhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','2mu2noL1_bNocut']]],
        ['mu20_2mu2noL1_bTau', 'L1_MU20MU21', ['L1_MU20',''], [BPhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','2mu2noL1_bNocut']]],

        # chains with EF B-physics selection skipped
        ['2mu4_bJpsimumu_noEFbph',                    'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu6_mu4_bJpsimumu_noEFbph',                 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu6_bDimu_noEFbph',                    'L1_2MU6', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu6_bJpsimumu_noEFbph',                    'L1_2MU6', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu10_bDimu_noEFbph',                    'L1_2MU10', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu10_bJpsimumu_noEFbph',                    'L1_2MU10', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],

        ['2mu4_bDimu_tightChi2',                    'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu4_bJpsimumu_tightChi2',                    'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu4_bBmumu_tightChi2',                    'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu4_bUpsimumu_tightChi2',                    'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu6_mu4_bDimu_tightChi2',                    'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu6_mu4_bJpsimumu_tightChi2',                    'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu6_mu4_bBmumu_tightChi2',                    'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu6_mu4_bUpsimumu_tightChi2',                    'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu6_bDimu_tightChi2',                    'L1_2MU6', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu6_bJpsimumu_tightChi2',                    'L1_2MU6', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu6_bBmumu_tightChi2',                    'L1_2MU6', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu6_bUpsimumu_tightChi2',                    'L1_2MU6', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        # chains with 'broken' vertexing at EF
        ['2mu4_bDimu_legacyVtx',                    'L1_2MU4',     [],                  [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1], 
        ['mu6_mu4_bDimu_legacyVtx',                 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu6_bDimu_legacyVtx',                    'L1_2MU6', [],                      [PhysicsStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1], 
        ['2mu10_bDimu_legacyVtx',                   'L1_2MU10', [],                     [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu4_bBmumuxv2_legacyVtx',                'L1_2MU4', [],                      [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1], 
        ['mu6_mu4_bBmumuxv2_legacyVtx',             'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu6_bBmumuxv2_legacyVtx',                'L1_2MU6', [],                      [PhysicsStream, 'express'], ['RATE:Bphysics','BW:Bphys'], -1], 
        ['mu10_mu6_bBmumuxv2_legacyVtx',            'L1_MU10_2MU6', ['L1_MU10','L1_MU6'],[BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu10_bBmumuxv2_legacyVtx',               'L1_2MU10', [],                     [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        
        #new narrow-scan muon triggers for tau->3mu        
        ['mu6_2mu4_bTau', 'L1_MU6_3MU4', ['L1_MU6','L1_2MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],

        # Asymmetric 3mu chains
        ['mu6_2mu4_bDimu', 'L1_MU6_3MU4', ['L1_MU6','L1_3MU4'],   [BMultiMuonStream], [RATE_BMultiMuonTag, 'BW:Muon'], -1],
        ['2mu6_mu4_bDimu', 'L1_2MU6_3MU4', ['L1_2MU6','L1_MU4'], [BMultiMuonStream], [RATE_BMultiMuonTag, 'BW:Muon'], -1],
        
        # chains for Bs->mumuPhi and Bs->J/psiPhi 
        ['mu6_mu4_bBmumux_BsJpsiPhi',                    'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu6_bBmumux_BsJpsiPhi',                    'L1_2MU6', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu10_mu6_bBmumux_BsJpsiPhi',            'L1_MU10_2MU6', ['L1_MU10','L1_MU6'],[BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],


        # Narrow-scan chains seeded L1_MU11_2MU6
        ['mu11_llns_mu6noL1_nscan03_bJpsimumu_noL2_L1MU11_2MU6', 'L1_MU11_2MU6', ['L1_MU11','L2_mu6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1,['serial',-1,['mu11_llns','mu6noL1_nscan03']]],
        ['mu11_llns_mu6noL1_nscan03_bDimu_noL2_L1MU11_2MU6', 'L1_MU11_2MU6', ['L1_MU11','L2_mu6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1,['serial',-1,['mu11_llns','mu6noL1_nscan03']]],


        ## Moved from Physics due to missing L1Topo item in Physics menu:
        ['2mu6_bDimu_L1BPH-8M15-2MU6_BPH-0DR22-2MU6',     'L1_BPH-8M15-2MU6_BPH-0DR22-2MU6',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bDimu_novtx_noos_L1BPH-8M15-2MU6_BPH-0DR22-2MU6',     'L1_BPH-8M15-2MU6_BPH-0DR22-2MU6',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bDimu_noL2_L1BPH-8M15-2MU6_BPH-0DR22-2MU6',     'L1_BPH-8M15-2MU6_BPH-0DR22-2MU6',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bDimu_novtx_noos_noL2_L1BPH-8M15-2MU6_BPH-0DR22-2MU6',     'L1_BPH-8M15-2MU6_BPH-0DR22-2MU6',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bDimu_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4',     'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4',     ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bDimu_novtx_noos_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4',     'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4',     ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bDimu_noL2_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4',     'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4',     ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bDimu_novtx_noos_noL2_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4',     'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4',     ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],


          
        ['mu6_mu4_bUpsimumu_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4-B', 'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4-B',     ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False], 


        ['2mu4_bBmumuxv2',                'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu4_bBmumux_BcmumuDsloose',    'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu4_bBmumux_BpmumuKp',         'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        #['2mu4_bJpsimumu',                'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        #['2mu4_bUpsimumu',                'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu4_bBmumu',                   'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu6_mu4_bBmumu_noL2',           'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],

        ['2mu10_bBmumux_BpmumuKp',        'L1_2MU10', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu10_bDimu_noinvm_novtx_ss',   'L1_2MU10', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu10_bBmumu',                  'L1_2MU10', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],

        # Bmumux chains with L2 B-physics selection skipped
        ['2mu4_bBmumuxv2_noL2',                'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu4_bBmumux_BcmumuDsloose_noL2',    'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu4_bBmumux_BpmumuKp_noL2',         'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu6_mu4_bBmumuxv2_noL2',             'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu6_mu4_bBmumux_BcmumuDsloose_noL2', 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu6_mu4_bBmumux_BpmumuKp_noL2',      'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],        
        ['2mu6_bBmumuxv2_noL2',              'L1_2MU6', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu6_bBmumux_BcmumuDsloose_noL2',    'L1_2MU6', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu6_bBmumux_BpmumuKp_noL2',    'L1_2MU6',     [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu10_mu6_bBmumux_BcmumuDsloose_noL2', 'L1_MU10_2MU6', ['L1_MU10','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],      
        ['mu10_mu6_bBmumux_BpmumuKp_noL2',      'L1_MU10_2MU6', ['L1_MU10','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],        
        ['2mu10_bBmumuxv2_noL2',               'L1_2MU10', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu10_bBmumux_BcmumuDsloose_noL2',   'L1_2MU10', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu10_bBmumux_BpmumuKp_noL2',        'L1_2MU10', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],

        ['2mu4_bDimu_noEFbph',                    'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu6_mu4_bDimu_noEFbph',                 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],

        ['2mu4_bDimu_Lxy0',                    'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu4_bJpsimumu_Lxy0',                    'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu4_bBmumu_Lxy0',                    'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu6_mu4_bDimu_Lxy0',                    'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu6_bBmumu_noL2',                    'L1_2MU6', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu10_mu6_bBmumuxv2_noL2', 'L1_MU10_2MU6', ['L1_MU10','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu6_mu4_bJpsi', 'L1_2MU6_3MU4', ['L1_2MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics', 'BW:Bphys'], -1],
        ### ATR-14543
        ['mu6_iloose_mu6_11invm60_noos_novtx', 'L1_2MU6', [], [PhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu6_ivarloose_mu6_11invm60_noos_novtx', 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        # chains with MSOnly muons at L2
        ['2mu4_bDimu_noL2',                    'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu4_bJpsimumu_noL2',                    'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu6_mu4_bDimu_noL2',                 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu6_mu4_bJpsimumu_noL2',                 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu6_bDimu_noL2',                    'L1_2MU6', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu6_bJpsimumu_noL2',                    'L1_2MU6', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
 
# L1 topo # Simple OneBarrel and BarrelOnly #

        # 2mu4-OneBarrel
        ['2mu4_bDimu_noL2_L12MU4-B',      'L1_2MU4-B',     ['L1_2MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu4_bDimu_novtx_noos_noL2_L12MU4-B',      'L1_2MU4-B',     ['L1_2MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu4_bJpsimumu_noL2_L12MU4-B',  'L1_2MU4-B',     ['L1_2MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu4_bBmumu_noL2_L12MU4-B',     'L1_2MU4-B',     ['L1_2MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu4_bBmumuxv2_noL2_L12MU4-B',  'L1_2MU4-B',     ['L1_2MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_noL2_L12MU4-B',  'L1_2MU4-B',     ['L1_2MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],

        # 2mu4-BarrelOnly
        ['2mu4_bDimu_noL2_L12MU4-BO',      'L1_2MU4-BO',     ['L1_2MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu4_bDimu_novtx_noos_noL2_L12MU4-BO',      'L1_2MU4-BO',     ['L1_2MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu4_bJpsimumu_noL2_L12MU4-BO',  'L1_2MU4-BO',     ['L1_2MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu4_bBmumu_noL2_L12MU4-BO',     'L1_2MU4-BO',     ['L1_2MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu4_bBmumuxv2_noL2_L12MU4-BO',  'L1_2MU4-BO',     ['L1_2MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_noL2_L12MU4-BO',  'L1_2MU4-BO',     ['L1_2MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],

        # mu6_mu4-OneBarrel
        ['mu6_mu4_bDimu_noL2_L12MU4-B',      'L1_MU6_2MU4-B',     ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bDimu_novtx_noos_noL2_L12MU4-B',      'L1_MU6_2MU4-B',     ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bJpsimumu_noL2_L12MU4-B',  'L1_MU6_2MU4-B',     ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bBmumu_noL2_L12MU4-B',     'L1_MU6_2MU4-B',     ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bBmumuxv2_noL2_L12MU4-B',  'L1_MU6_2MU4-B',     ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bBmumux_BcmumuDsloose_noL2_L12MU4-B',  'L1_MU6_2MU4-B',     ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],

# mu6_mu4-BarrelOnly
        ['mu6_mu4_bDimu_noL2_L1MU6MU4-BO',      'L1_MU6MU4-BO',     ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bDimu_novtx_noos_noL2_L1MU6MU4-BO',      'L1_MU6MU4-BO',     ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bJpsimumu_noL2_L1MU6MU4-BO',  'L1_MU6MU4-BO',     ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bBmumu_noL2_L1MU6MU4-BO',     'L1_MU6MU4-BO',     ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bBmumuxv2_noL2_L1MU6MU4-BO',  'L1_MU6MU4-BO',     ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bBmumux_BcmumuDsloose_noL2_L1MU6MU4-BO',  'L1_MU6MU4-BO',     ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],

# 2mu6-OneBarrel
        ['2mu6_bDimu_noL2_L12MU6-B',      'L1_2MU6-B',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bDimu_novtx_noos_noL2_L12MU6-B',      'L1_2MU6-B',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bJpsimumu_noL2_L12MU6-B',  'L1_2MU6-B',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bBmumu_noL2_L12MU6-B',     'L1_2MU6-B',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bBmumuxv2_noL2_L12MU6-B',  'L1_2MU6-B',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bBmumux_BcmumuDsloose_noL2_L12MU6-B',  'L1_2MU6-B',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        
# 2mu6-BarrelOnly
        ['2mu6_bDimu_noL2_L12MU6-BO',      'L1_2MU6-BO',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bDimu_novtx_noos_noL2_L12MU6-BO',      'L1_2MU6-BO',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bJpsimumu_noL2_L12MU6-BO',  'L1_2MU6-BO',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bBmumu_noL2_L12MU6-BO',     'L1_2MU6-BO',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bBmumuxv2_noL2_L12MU6-BO',  'L1_2MU6-BO',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bBmumux_BcmumuDsloose_noL2_L12MU6-BO',  'L1_2MU6-BO',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],


        
        

#
# L1Topo nominal
#
       

# 86% bckg rejection WP






      
# 96% bckg rejection WP
        ['2mu6_bDimu_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',      'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bDimu_novtx_noos_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',      'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bJpsimumu_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',  'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bBmumu_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',     'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bBmumuxv2_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',  'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bBmumux_BcmumuDsloose_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',  'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        
       
       ['2mu6_bTau_L1LFV-MU6',  'L1_LFV-MU6', ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
       ['2mu6_bPhi_L1LFV-MU6',  'L1_LFV-MU6', ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],

       ### ATR-15263
       ['2mu4_bBmumux_LbmumuLambda', 'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
       ['mu6_mu4_bBmumux_LbmumuLambda', 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
       ['2mu6_bBmumux_LbmumuLambda', 'L1_2MU6', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],

       ['2mu6_bBmumux_LbmumuLambda_L1LFV-MU6', 'L1_LFV-MU6', ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
       ['2mu4_bBmumux_LbmumuLambda_L1BPH-2M9-2MU4_BPH-0DR15-2MU4', 'L1_BPH-2M9-2MU4_BPH-0DR15-2MU4', ['L1_2MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
       ['mu6_mu4_bBmumux_LbmumuLambda_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4', 'L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
       ['2mu6_bBmumux_LbmumuLambda_L1BPH-2M9-2MU6_BPH-2DR15-2MU6', 'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6', ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],

#
# For semileptonic modes
#

        ['mu10_mu6_bBmumuxv2_noL2_L1LFV-MU',  'L1_LFV-MU',     ['L1_MU10','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu10_mu6_bBmumux_BcmumuDsloose_noL2_L1LFV-MU',  'L1_LFV-MU',     ['L1_MU10','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],

                
        ## Some more Bphysics
        # backup 71% bckg rejection WP
        
        ['mu6_mu4_bDimu_noL2_L1BPH-8M15-MU6MU4',     'L1_BPH-8M15-MU6MU4',     ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bDimu_novtx_noos_noL2_L1BPH-8M15-MU6MU4',     'L1_BPH-8M15-MU6MU4',     ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        
        # backup 95% bckg rejection WP (for Upsilon only)        
        ['2mu6_bDimu_noL2_L1BPH-8M15-2MU6',     'L1_BPH-8M15-2MU6',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bDimu_novtx_noos_noL2_L1BPH-8M15-2MU6',     'L1_BPH-8M15-2MU6',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
		
        ['mu4_iloose_mu4_11invm60_noos', 'L1_2MU4', [], [PhysicsStream], [RATE_DYTag,BW_DYTag], -1],
        ['mu4_iloose_mu4_7invm9_noos',   'L1_2MU4', [], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],
        ['mu6_iloose_mu6_11invm24_noos', 'L1_2MU6', [], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],
        ['mu6_iloose_mu6_24invm60_noos', 'L1_2MU6', [], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],

        ### ATR-14543
        ['mu4_ivarloose_mu4_11invm60_noos', 'L1_2MU4', [], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],
        ['mu4_ivarloose_mu4_7invm9_noos',   'L1_2MU4', [], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],
        ['mu6_ivarloose_mu6_11invm24_noos', 'L1_2MU6', [], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],
        ['mu6_ivarloose_mu6_24invm60_noos', 'L1_2MU6', [], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],

        ['mu4_iloose_mu4_11invm60_noos_novtx', 'L1_2MU4', [], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],
        ['mu4_iloose_mu4_7invm9_noos_novtx',   'L1_2MU4', [], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],
        ['mu6_iloose_mu6_11invm24_noos_novtx', 'L1_2MU6', [], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],
        ['mu6_iloose_mu6_24invm60_noos_novtx', 'L1_2MU6', [], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],

        ### ATR-14543
        ['mu4_ivarloose_mu4_11invm60_noos_novtx', 'L1_2MU4', [], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],
        ['mu4_ivarloose_mu4_7invm9_noos_novtx',   'L1_2MU4', [], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],
        ['mu6_ivarloose_mu6_11invm24_noos_novtx', 'L1_2MU6', [], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],
        ['mu6_ivarloose_mu6_24invm60_noos_novtx', 'L1_2MU6', [], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],
        ['mu4_iloose_mu4_11invm60_noos_L1MU6_2MU4',    'L1_MU6_2MU4',    ['L1_MU4','L1_MU4'], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],
        ['mu4_iloose_mu4_7invm9_noos_L1MU6_2MU4',           'L1_MU6_2MU4',                         ['L1_MU4','L1_MU4'], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],
        ['mu4_iloose_mu4_11invm60_noos_novtx_L1MU6_2MU4',           'L1_MU6_2MU4',                         ['L1_MU4','L1_MU4'], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],
        ['mu4_iloose_mu4_7invm9_noos_novtx_L1MU6_2MU4',           'L1_MU6_2MU4',                         ['L1_MU4','L1_MU4'], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],
        ['mu4_ivarloose_mu4_11invm60_noos_L1MU6_2MU4',    'L1_MU6_2MU4',    ['L1_MU4','L1_MU4'], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],
        ['mu4_ivarloose_mu4_7invm9_noos_L1MU6_2MU4',           'L1_MU6_2MU4',                         ['L1_MU4','L1_MU4'], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],
        ['mu4_ivarloose_mu4_11invm60_noos_novtx_L1MU6_2MU4',           'L1_MU6_2MU4',                         ['L1_MU4','L1_MU4'], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],
        ['mu4_ivarloose_mu4_7invm9_noos_novtx_L1MU6_2MU4',           'L1_MU6_2MU4',                         ['L1_MU4','L1_MU4'], [PhysicsStream],  [RATE_DYTag,BW_DYTag], -1],
			 ]

    if TriggerFlags.doFTK():
        TriggerFlags.BphysicsSlice.signatures = TriggerFlags.BphysicsSlice.signatures() + [
            ['2mu4_bBmumuxv2_Ftk',                'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
            ]


    TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures() + [

        ['e17_lhloose_nod0_ringer_mu14', 'L1_EM15VH_MU10', ['L1_EM15VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],

        ['j20_xe100_mht_L1XE50', 'L1_XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,['j20','xe100_mht_L1XE50']]],
        ['j30_xe100_mht_L1XE50', 'L1_XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,['j30','xe100_mht_L1XE50']]],
        ['j40_xe100_mht_L1XE50', 'L1_XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,['j40','xe100_mht_L1XE50']]],
        
        ['j20_lcw_xe100_mht_L1XE50', 'L1_XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,['j20_lcw','xe100_mht_L1XE50']]],
        ['j30_lcw_xe100_mht_L1XE50', 'L1_XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,['j30_lcw','xe100_mht_L1XE50']]],
        ['j40_lcw_xe100_mht_L1XE50', 'L1_XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,['j40_lcw','xe100_mht_L1XE50']]],
        
        ['j20_cleanT_xe100_mht_L1XE50', 'L1_XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,['j20_cleanT','xe100_mht_L1XE50']]],
        ['j30_cleanT_xe100_mht_L1XE50', 'L1_XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,['j30_cleanT','xe100_mht_L1XE50']]],
        ['j40_cleanT_xe100_mht_L1XE50', 'L1_XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,['j40_cleanT','xe100_mht_L1XE50']]],        

        ['j80_0eta240_j60_j45_320eta490_invm700_AND_j45_bmv2c1070_split_j45_bmv2c1085_split','L1_J40.0ETA25_2J25_J20.31ETA49', ['','','','',''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet' ], -1, ['serial',-1,['j80_0eta240','j60','j45_320eta490_invm700_AND','j45_bmv2c1070_split','j45_bmv2c1085_split'] ]],
                


        # 2e34 single items
#ATR-16089        ['e60_lhloose_L1EM24VHI_xe60noL1',      'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e60_lhloose_L1EM24VHI","xe60noL1"]]],
        ['e60_lhloose_nod0_L1EM24VHI_xe60noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e60_lhloose_nod0_L1EM24VHI","xe60noL1"]]],
#ATR-16089        ['e70_lhloose_L1EM24VHI_xe70noL1',      'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e70_lhloose_L1EM24VHI","xe70noL1"]]],
        ['e70_lhloose_nod0_L1EM24VHI_xe70noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e70_lhloose_nod0_L1EM24VHI","xe70noL1"]]],

        # L1Topo W T&P triggers for 2016
        ['e13_etcut_trkcut_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20_xe20_mt25', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', ['L1_EM12',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xe20"]]],
        ['e13_etcut_trkcut_xe20_mt25', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', ['L1_EM12',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xe20"]]],    
        ['e13_etcut_trkcut_xs15_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', ['L1_EM12',''], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1,['serial',-1,["e13_etcut_trkcut","xs15"]]], # commenting out until megrging is fixed centrally.
        ['e13_etcut_trkcut_xe20_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', ['L1_EM12',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","xe20"]]],
        ['e13_etcut_trkcut_xe20', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', ['L1_EM12',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","xe20"]]],
        ['e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', ['L1_EM12','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","j20_perf","xe15"]]],
        ['e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', ['L1_EM12','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","j20_perf","xe15"]]],
       
        ### ATR-14348: L1Topo egamma chains
        ['e13_etcut_trkcut_xs30_j15_perf_xe30_2dphi05_mt35_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', ['L1_EM12','','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e13_etcut_trkcut_xs30_j15_perf_xe30_2dphi15_mt35_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', ['L1_EM12','','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e13_etcut_trkcut_xs30_xe30_mt35_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', ['L1_EM12','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xs30","xe30"]]],
        ['e13_etcut_trkcut_xs30_j15_perf_xe30_6dphi05_mt35_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', ['L1_EM12','','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e13_etcut_trkcut_xs30_j15_perf_xe30_6dphi15_mt35_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', ['L1_EM12','','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e13_etcut_trkcut_xs30_xe30_mt35_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', ['L1_EM12','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xs30","xe30"]]],
        ['e13_etcut_trkcut_xs30_j15_perf_xe30_2dphi15_mt35_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', ['L1_EM12','','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e13_etcut_trkcut_j20_perf_xe15_6dphi15_mt25_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', ['L1_EM12','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","j20_perf","xe15"]]],
        ['e13_etcut_trkcut_xs30_j15_perf_xe30_6dphi15_mt35_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', ['L1_EM12','','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e13_etcut_trkcut_j20_perf_xe15_6dphi15_mt25_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', ['L1_EM12','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","j20_perf","xe15"]]],
        ['e13_etcut_trkcut_j20_perf_xe15_6dphi15_mt25', 'L1_EM12_XS20', ['L1_EM12','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","j20_perf","xe15"]]],
        # e18
        ['e18_etcut_trkcut_xs30_j15_perf_xe30_2dphi15_mt35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE_XS30', 'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE_XS30', ['L1_EM15','','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e18_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e18_etcut_trkcut_xs30_j15_perf_xe30_6dphi05_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE', 'L1_EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE', ['L1_EM15','','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e18_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e18_etcut_trkcut_xs30_j15_perf_xe30_6dphi15_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE', 'L1_EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE', ['L1_EM15','','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e18_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e18_etcut_trkcut_xs30_j15_perf_xe30_6dphi15_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE', 'L1_EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE', ['L1_EM15','','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e18_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e18_etcut_trkcut_xs30_xe30_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE', 'L1_EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE', ['L1_EM15','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e18_etcut_trkcut","xs30","xe30"]]],
        # e20
        ['e20_etcut_trkcut_xs30_j15_perf_xe30_6dphi15_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE', 'L1_EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE', ['L1_EM15','','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e20_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e20_etcut_trkcut_xs30_xe30_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE', 'L1_EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE', ['L1_EM15','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e20_etcut_trkcut","xs30","xe30"]]],
        ['e20_etcut_trkcut_xs30_j15_perf_xe30_6dphi15_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE', 'L1_EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE', ['L1_EM15','','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e20_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e20_etcut_trkcut_xs30_xe30_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE', 'L1_EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE', ['L1_EM15','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e20_etcut_trkcut","xs30","xe30"]]],
        # e24
        ['e24_etcut_trkcut_xs30_j15_perf_xe30_6dphi15_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE', 'L1_EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE', ['L1_EM15','','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e24_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e24_etcut_trkcut_xs30_xe30_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE', 'L1_EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE', ['L1_EM15','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e24_etcut_trkcut","xs30","xe30"]]],
        ['e24_etcut_trkcut_xs30_j15_perf_xe30_6dphi15_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE', 'L1_EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE', ['L1_EM15','','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e24_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e24_etcut_trkcut_xs30_xe30_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE', 'L1_EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE', ['L1_EM15','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e24_etcut_trkcut","xs30","xe30"]]],


 
        # cut-based version of the following triggers staying in MC menu. The LH-based version kept in Physics menu. (ATR-14059)

        # photon+tri-jet trigger

        ['g45_tight_3j50noL1', 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g45_tight","3j50noL1"]]],
        ['g75_tight_3j50noL1', 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g75_tight","3j50noL1"]]],
        ['g75_tight_3j25noL1_L1EM20VHI', 'L1_EM20VHI', ['L1_EM20VHI',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g75_tight","3j25noL1_L1EM20VHI"]]],
        ['g75_tight_3j25noL1_L1EM22VHI', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g75_tight","3j25noL1_L1EM22VHI"]]],


        ['j80_boffperf_split_L12J50_XE40','L1_2J50_XE40',  ['L1_2J50_XE40',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:Jet'], -1],
        ['j80_boffperf_split_xe70_L1J400ETA25_XE50','L1_J40.0ETA25_XE50',  ['L1_J40.0ETA25_XE50',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:Jet'], -1,['serial',-1,["j80_boffperf_split","xe70_L1J400ETA25_XE50"]]],
        ['j100_boffperf_split_L1XE60','L1_XE60',  ['L1_XE60',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:Jet'], -1],

      ['j80_bmv2c1060_split_L12J50_XE40','L1_2J50_XE40',  ['L1_2J50','XE40'],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:Jet'], -1],
      ['j100_bmv2c1070_split_L1XE60','L1_XE60',  ['L1_2J50',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:Jet'], -1],



        # 
        #  B-jet + hT
        #

        ['2j55_bperf_ht300_L14J20',  'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,['2j55_bperf', 'ht300_L14J20']]],
        ['2j55_bperf_split_ht300_L14J20',  'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,['2j55_bperf_split', 'ht300_L14J20']]],
        ['2j55_boffperf_ht300_L14J20',  'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,['2j55_boffperf', 'ht300_L14J20']]],

        ['j55_bperf_ht500_L14J20',  'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,[  'j55_bperf', 'ht500_L14J20']]],
        ['j55_bperf_split_ht500_L14J20',  'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,[ 'j55_bperf_split', 'ht500_L14J20']]],
        ['j55_boffperf_ht500_L14J20',  'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,[  'j55_boffperf', 'ht500_L14J20']]],
        ['j55_boffperf_split_ht500_L14J20',  'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,[  'j55_boffperf_split', 'ht500_L14J20']]],


        ['2j45_bmv2c1060_split_2j35_0eta490_invm700','L1_HT150-J20s5.ETA31_MJJ-400', [], [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,[  '2j45_bmv2c1060_split', '2j35_0eta490_invm700']]],
        ['ht300_2j40_0eta490_invm700','L1_HT150-J20s5.ETA31_MJJ-400', [], [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,[  'ht300', '2j40_0eta490_invm700']]],

         ['2j45_bmv2c1060_split_2j35_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF','L1_HT150-J20s5.ETA31_MJJ-400-CF' , [], [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,[  '2j45_bmv2c1060_split', '2j35_0eta490_invm700']]],
        

        # jet+xe alternative triggers
        ['j80_xe100',               'L1_J40_XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet', 'BW:MET'], -1,['serial',-1,["j80","xe100"]]],        
        ['j40_cleanT_xe75_L1XE60',    'L1_XE60', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,["j40_cleanT","xe75_L1XE60"]]],
        ['j30_0eta490_cleanT_xe80_L1XE60',    'L1_XE60', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,["j30_0eta490_cleanT","xe80_L1XE60"]]],
        ['j30_0eta490_cleanT_xe100_tc_lcw_L1XE70', 'L1_XE70', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,["j30_0eta490_cleanT","xe100_tc_lcw_L1XE70"]]],
        ['j60_cleanT_xe80_L1J40_XE50',                    'L1_J40_XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,["j60_cleanT","xe80"]]],
        ['j60_cleanT_xe75_L1J40_XE50',                    'L1_J40_XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,["j60_cleanT","xe75"]]],

        ['j80_cleanT_xe80_tc_lcw_2dphi05_L1XE50',         'L1_XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,["j80_cleanT","xe80_tc_lcw_L1XE50"]]],
        ['j80_cleanT_xe85_tc_lcw_2dphi05_L1XE50',         'L1_XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,["j80_cleanT","xe85_tc_lcw_L1XE50"]]],

        # ATR-11975
        ['j65_0eta490_j40_0eta490_invm250_xe80_L1XE70', 'L1_XE70', ['','',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,["j65_0eta490","j40_0eta490_invm250","xe80_L1XE70"]]],


        # Tau + Electron
        # Low-threshold with jets
        ['e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo',  'L1_EM15VHI_2TAU12IM_J25_3J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_iloose","tau25_medium1_tracktwo"]]],

        ['e17_lhmedium_ivarloose_tau25_medium1_tracktwo',  'L1_EM15VHI_2TAU12IM_J25_3J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_ivarloose","tau25_medium1_tracktwo"]]],

        # Those are somehow not working
        ['e17_lhmedium_iloose_tau25_perf_ptonly_L1EM15VHI_2TAU12IM', 'L1_EM15VHI_2TAU12IM',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_iloose","tau25_perf_ptonly"]]],
        ['e17_lhmedium_ivarloose_tau25_perf_ptonly_L1EM15VHI_2TAU12IM', 'L1_EM15VHI_2TAU12IM',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_ivarloose","tau25_perf_ptonly"]]],
        ['e17_lhmedium_tau25_medium1_tracktwo_L1EM15VHI_2TAU12IM',  'L1_EM15VHI_2TAU12IM',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium","tau25_medium1_tracktwo"]]],
       
        #e+tau chains seeded by L1DR-EM15TAU12I which could be the primary e+tau chain with L1topo
        ### cannot do now until l1 item is defined
        ['e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I',  'L1_DR-EM15TAU12I',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_nod0_iloose","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I',  'L1_DR-EM15TAU12I',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwo"]]],

        ['e17_lhmedium_tau25_medium1_tracktwo_L1DR-EM15TAU12I',  'L1_DR-EM15TAU12I',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_nod0_tau25_medium1_tracktwo_L1DR-EM15TAU12I',  'L1_DR-EM15TAU12I',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_medium1_tracktwo"]]], 

        ['e17_lhmedium_iloose_tau25_medium1_tracktwo_L1EM15TAU12I-J25',  'L1_EM15TAU12I-J25',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_iloose","tau25_medium1_tracktwo"]]],

        ['e17_lhmedium_ivarloose_tau25_medium1_tracktwo_L1EM15TAU12I-J25',  'L1_EM15TAU12I-J25',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_ivarloose","tau25_medium1_tracktwo"]]],

	# L1topo e+tau+met
        ['e17_lhmedium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',  'L1_DR-EM15TAU12I-J25',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium","tau25_medium1_tracktwo"]]],
        # Isolated variant
        ['e17_lhmedium_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',  'L1_DR-EM15TAU12I-J25',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_iloose","tau25_medium1_tracktwo"]]],


        # L1Topo EM+TAU Physics
        ['e17_lhmedium_nod0_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',  'L1_DR-EM15TAU12I-J25',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_medium1_tracktwo"]]],

	# tau+ muon chains   
        # L1Topo MU+TAU+J
        ['mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25',  'L1_DR-MU10TAU12I_TAU12I-J25',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14","tau25_medium1_tracktwo"]]],
        ['mu14_tau25_medium1_tracktwo_L1MU10_TAU12I-J25',  'L1_MU10_TAU12I-J25',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14","tau25_medium1_tracktwo"]]],
	
        # Also somehow not working...
        ['mu14_iloose_tau25_perf_ptonly_L1MU10_TAU12IM',      'L1_MU10_TAU12IM',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_iloose","tau25_perf_ptonly"]]],        
        ['mu14_ivarloose_tau25_perf_ptonly_L1MU10_TAU12IM',      'L1_MU10_TAU12IM',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_perf_ptonly"]]],        

	#  2015 version of the tau+photon chains
        ['g35_medium_tau25_perf_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau25_perf_tracktwo_L1TAU12"]]],

        ['g35_medium_tau25_dikaon_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau25_dikaon_tracktwo_L1TAU12"]]],
        ['g35_medium_tau35_dikaon_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau35_dikaon_tracktwo_L1TAU12"]]],
        ['g35_medium_tau25_dikaontight_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau25_dikaontight_tracktwo_L1TAU12"]]],
        ['g35_medium_tau35_dikaontight_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'],-1,['serial',-1,["g35_medium","tau35_dikaontight_tracktwo_L1TAU12"]]],

        #old g+tau chains (2016, ATR-16266)
        #tau+photon (ATR-13061)
        ['g35_medium_tau25_dikaonmasstight_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau25_dikaonmasstight_tracktwo_L1TAU12"]]],
        ['g35_medium_tau35_dikaonmasstight_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau35_dikaonmasstight_tracktwo_L1TAU12"]]],
        ['g35_medium_tau25_dikaonmass_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau25_dikaonmass_tracktwo_L1TAU12"]]],
        ['g35_medium_tau35_dikaonmass_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau35_dikaonmass_tracktwo_L1TAU12"]]],
        ['g35_medium_L1EM24VHI_tau25_dikaonmasstight_tracktwo_L1TAU12',     'L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium_L1EM24VHI","tau25_dikaonmasstight_tracktwo_L1TAU12"]]],
        ['g35_medium_L1EM24VHI_tau25_dikaonmass_tracktwo_L1TAU12',     'L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium_L1EM24VHI","tau25_dikaonmass_tracktwo_L1TAU12"]]],
        ['g35_medium_L1EM24VHI_tau35_dikaonmass_tracktwo_L1TAU12',     'L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium_L1EM24VHI","tau35_dikaonmass_tracktwo_L1TAU12"]]],
        #tau + photon (ATR-13841)
        ['g35_medium_tau25_dipion1_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau25_dipion1_tracktwo_L1TAU12"]]],
        ['g35_medium_tau35_dipion1loose_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau35_dipion1loose_tracktwo_L1TAU12"]]],
        ['g35_medium_tau25_dipion2_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau25_dipion2_tracktwo_L1TAU12"]]],
        ['g35_medium_tau35_dipion2_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau35_dipion2_tracktwo_L1TAU12"]]],
        ['g35_medium_tau25_kaonpi1_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau25_kaonpi1_tracktwo_L1TAU12"]]],
        ['g35_medium_tau35_kaonpi1_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau35_kaonpi1_tracktwo_L1TAU12"]]],
        ['g35_medium_tau25_kaonpi2_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau25_kaonpi2_tracktwo_L1TAU12"]]],
        ['g35_medium_tau35_kaonpi2_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau35_kaonpi2_tracktwo_L1TAU12"]]],
        ['g35_medium_L1EM24VHI_tau25_dipion1_tracktwo_L1TAU12',     'L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium_L1EM24VHI","tau25_dipion1_tracktwo_L1TAU12"]]],
        ['g35_medium_L1EM24VHI_tau35_dipion1loose_tracktwo_L1TAU12',     'L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium_L1EM24VHI","tau35_dipion1loose_tracktwo_L1TAU12"]]],
        ['g35_medium_L1EM24VHI_tau25_dipion2_tracktwo_L1TAU12',     'L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium_L1EM24VHI","tau25_dipion2_tracktwo_L1TAU12"]]],
        ['g35_medium_L1EM24VHI_tau35_dipion2_tracktwo_L1TAU12',     'L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium_L1EM24VHI","tau35_dipion2_tracktwo_L1TAU12"]]],
        ['g35_medium_L1EM24VHI_tau25_kaonpi1_tracktwo_L1TAU12',     'L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium_L1EM24VHI","tau25_kaonpi1_tracktwo_L1TAU12"]]],
        ['g35_medium_L1EM24VHI_tau35_kaonpi1_tracktwo_L1TAU12',     'L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium_L1EM24VHI","tau35_kaonpi1_tracktwo_L1TAU12"]]],
        ['g35_medium_L1EM24VHI_tau25_kaonpi2_tracktwo_L1TAU12',     'L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium_L1EM24VHI","tau25_kaonpi2_tracktwo_L1TAU12"]]],
        ['g35_medium_L1EM24VHI_tau35_kaonpi2_tracktwo_L1TAU12',     'L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium_L1EM24VHI","tau35_kaonpi2_tracktwo_L1TAU12"]]],        

        # photon+pion (ATR-13525)
        ['g35_medium_tau25_singlepion_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau25_singlepion_tracktwo_L1TAU12"]]],
        ['g35_medium_tau25_singlepiontight_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau25_singlepiontight_tracktwo_L1TAU12"]]],
        ['g35_medium_L1EM24VHI_tau25_singlepion_tracktwo_L1TAU12',     'L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium_L1EM24VHI","tau25_singlepion_tracktwo_L1TAU12"]]],
        ['g35_medium_L1EM24VHI_tau25_singlepiontight_tracktwo_L1TAU12',     'L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium_L1EM24VHI","tau25_singlepiontight_tracktwo_L1TAU12"]]],
        # ATR-14643
        ['g25_medium_tau25_dikaonmasstight_tracktwo_60mVis10000','L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium','tau25_dikaonmasstight_tracktwo'],False]],
        ['g25_medium_L1EM24VHI_tau25_dikaonmasstight_tracktwo_60mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dikaonmasstight_tracktwo'],False] ],


        # 4-jet items (VBF H->bb, hh->bbbb)

        ['mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet', 'BW:Muon'], -1,['serial',-1,['mu4', '3j35', 'j35_bloose_split_antimatchdr05mu'] ]],


        ['mu4_3j35_dr05_j35_boffperf_antimatchdr05mu_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet', 'BW:Muon'], -1,['serial',-1,['mu4', '3j35', 'j35_boffperf_antimatchdr05mu'] ]],
        ['mu4_3j35_dr05_j35_boffperf_split_antimatchdr05mu_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet', 'BW:Muon'], -1,['serial',-1,['mu4', '3j35', 'j35_boffperf_split_antimatchdr05mu'] ]],

        ['mu4_3j35_dr05_j35_boffperf_split_antimatchdr05mu_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu4', '3j35', 'j35_boffperf_split_antimatchdr05mu'] ], False],
        ['mu4_3j30_dr05_j30_boffperf_split_antimatchdr05mu_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu4', '3j30', 'j30_boffperf_split_antimatchdr05mu'] ], False],        
        
        ['mu4_4j40_dr05_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu4', '4j40'] ],True],
        ['mu4_3j35_dr05_j35_boffperf_split_antimatchdr05mu_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu4', '3j35', 'j35_boffperf_split_antimatchdr05mu'] ], True],
        ['mu4_4j40_dr05_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu4', '4j40'] ], True],
        ['mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu4', '3j35', 'j35_bloose_split_antimatchdr05mu'] ], True],
        ['mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu4', '3j30', 'j30_bmedium_split_antimatchdr05mu'] ], True],

        ['mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu4', '3j30', 'j30_bmedium_split_antimatchdr05mu'] ]],
        ['mu4_3j30_dr05_j30_boffperf_split_antimatchdr05mu_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu4', '3j30', 'j30_boffperf_split_antimatchdr05mu'] ]],

        ['mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu4', '3j35', 'j35_bloose_split_antimatchdr05mu'] ], False],
        ['mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu4', '3j30', 'j30_bmedium_split_antimatchdr05mu'] , False]],

        
        # VBF triggers
        ['g25_loose_2j40_0eta490_3j25_0eta490_invm700', 'L1_EM22VHI', ['L1_EM22VHI','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g25_loose","2j40_0eta490","3j25_0eta490_invm700"]]],
        ['g25_loose_L1EM20VH_2j40_0eta490_3j25_0eta490_invm700', 'L1_EM20VH', ['L1_EM20VH','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g25_loose_L1EM20VH","2j40_0eta490","3j25_0eta490_invm700"]]],
####	
        ['g25_medium_L1EM22VHI_j35_0eta490_boffperf_split_3j35_0eta490_invm700', 'L1_EM22VHI', ['L1_EM22VHI', '', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet','BW:Egamma'],-1,['serial',-1,['g25_medium_L1EM22VHI', 'j35_0eta490_boffperf_split','3j35_0eta490_invm700']]],
        ['g25_medium_L1EM22VHI_2j35_0eta490_boffperf_split_2j35_0eta490', 'L1_EM22VHI', ['L1_EM22VHI', '', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet','BW:Egamma'],-1,['serial',-1,['g25_medium_L1EM22VHI', '2j35_0eta490_boffperf_split','2j35_0eta490']]],


        ### ATR-15062 L1Topo
        ['g20_tight_icalovloose_j35_0eta490_bmv2c1077_3j35_0eta490_invm500', 'L1_EM18VHI_MJJ-300', ['','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g20_tight_icalovloose","j35_0eta490_bmv2c1077","3j35_0eta490_invm500"]]],
        ['g20_tight_icalovloose_4j35_0eta490_invm500', 'L1_EM18VHI_MJJ-300', ['',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g20_tight_icalovloose","4j35_0eta490_invm500"]]],

        ### ATR-15062 Non-L1Topo
        ['g27_medium_L1EM24VHI_2j25_0eta490_bmv2c1077_2j25_0eta490_invm500', 'L1_EM24VHI', ['L1_EM24VHI','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g27_medium_L1EM24VHI","2j25_0eta490_bmv2c1077","2j25_0eta490_invm500"]]],


        ['j80_bmv2c1077_split_L12J50_XE40','L1_2J50_XE40',  ['L1_2J50_XE40',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:Jet'], -1],
        ['j80_bmv2c1077_split_L1J40_XE60','L1_J40_XE60',  ['L1_J40_XE60',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:Jet'], -1],
        ['j80_bmv2c1077_split_L1J40_XE50','L1_J40_XE50',  ['L1_J40_XE50',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:Jet'], -1],
 
        ['j80_bmv2c1077_split_L1J400ETA25_XE50','L1_J40.0ETA25_XE50',  ['L1_J40.0ETA25_XE50',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:Jet'], -1],
 
        ['j80_bmv2c1077_split_L1XE60','L1_XE60',  ['L1_XE60',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:Jet'], -1],
        ['2j30_bmv2c1085_split_L12J15_XE55','L1_2J15_XE55',  ['L1_2J15','XE55'],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:Jet'], -1],
 
        ['j80_boffperf_split_L1J40_XE60','L1_J40_XE60',  ['L1_J40_XE60',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:Jet'], -1],
        ['j80_boffperf_split_L1J40_XE50','L1_J40_XE50',  ['L1_J40_XE50',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:Jet'], -1],
 
        ['j80_boffperf_split_L1J400ETA25_XE50','L1_J40.0ETA25_XE50',  ['L1_J40.0ETA25_XE50',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:Jet'], -1],
 
        ['j80_boffperf_split_L1XE60','L1_XE60',  ['L1_XE60',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:Jet'], -1],
        ['2j30_boffperf_split_L12J15_XE55','L1_2J15_XE55',  ['L1_2J15','XE55'],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:Jet'], -1],
 
        ['j80_boffperf_L12J50_XE40','L1_2J50_XE40',  ['L1_2J50_XE40',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:Jet'], -1],
        ['j80_boffperf_L1J40_XE60','L1_J40_XE60',  ['L1_J40_XE60',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:Jet'], -1],
        ['j80_boffperf_L1J40_XE50','L1_J40_XE50',  ['L1_J40_XE50',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:Jet'], -1],
 
        ['j80_boffperf_L1J400ETA25_XE50','L1_J40.0ETA25_XE50',  ['L1_J40.0ETA25_XE50',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:Jet'], -1],
 
        ['j80_boffperf_L1XE60','L1_XE60',  ['L1_XE60',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:Jet'], -1],
        ['2j30_boffperf_L12J15_XE55','L1_2J15_XE55',  ['L1_2J15','XE55'],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:Jet'], -1],

        ['2j55_boffperf_split_ht300_L1J100',  'L1_J100',   ['L1_J100', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,['2j55_boffperf_split', 'ht300_L1J100']]],
        ['j55_boffperf_split_ht500_L1J100',  'L1_J100',   ['L1_J100', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,['j55_boffperf_split', 'ht500_L1J100']]],

        # More b-jet triggers please
        ['j80_bmv2c1060_split_xe80_mht_L1XE60','L1_XE60',[],[PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial',-1,['j80_bmv2c1060_split','xe80_mht_L1XE60']]],
        
        #ATR-15044
        ['j80_0eta240_j60_j45_320eta490_AND_2j45_bmv2c1070_split_L1J40.0ETA25_2J30_J20.31ETA49','L1_J40.0ETA25_2J30_J20.31ETA49',[],[PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial',-1,['j80_0eta240','j60','j45_320eta490_AND','2j45_bmv2c1070_split']]],



        
        
        # 3-jet items (bA->bbb)
        ['mu4_2j40_dr05_j40_bloose_split_antimatchdr05mu_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu4', '2j40', 'j40_bloose_split_antimatchdr05mu'] ], False],
        ['mu4_2j35_dr05_j35_bmedium_split_antimatchdr05mu_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu4', '2j35', 'j35_bmedium_split_antimatchdr05mu'] ], False],
        ['mu4_2j40_dr05_j40_bloose_split_antimatchdr05mu_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu4', '2j40', 'j40_bloose_split_antimatchdr05mu'] ], False],
        ['mu4_2j35_dr05_j35_bmedium_split_antimatchdr05mu_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu4', '2j35', 'j35_bmedium_split_antimatchdr05mu'] ], False],


        ['j40_0eta490_j30_0eta490_deta25_xe80',    'L1_XE60', ['','',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,["j40_0eta490","j30_0eta490_deta25","xe80"]]],
        ['j40_0eta490_j30_0eta490_deta25_xe80_tc_lcw',     'L1_XE60', ['','',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,["j40_0eta490","j30_0eta490_deta25","xe80_tc_lcw"]]],
        ['j65_0eta490_j40_0eta490_invm250_xe80',   'L1_XE60', ['','',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,["j65_0eta490","j40_0eta490_invm250","xe80"]]],

        ['2j40_0eta490_invm250_xe80',    'L1_XE60', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,["2j40_0eta490_invm250","xe80"]]],

        ['j50_0eta490_j30_0eta490_deta30_xe80_L1J40_XE50',       'L1_J40_XE50',['','',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1,['serial',-1,["j50_0eta490","j30_0eta490_deta30","xe80"]]],

        ['j40_0eta490_j30_0eta490_deta25_xe60',       'L1_XE60',['','',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1,['serial',-1,["j40_0eta490","j30_0eta490_deta25","xe60"]]],


        # testing jet chains from new topo item
        ['j40_0eta490_xe80_1dphi10_L1DPHI-AJ20s2XE50', 'L1_DPHI-AJ20s2XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1,['serial',-1,["j40_0eta490","xe80" ]]],
        ['2j30_0eta490_deta25_xe80_L1DPHI-AJ20s2XE50', 'L1_DPHI-AJ20s2XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1,['serial',-1,["2j30_0eta490_deta25","xe80" ]]],
        ['2j30_0eta490_deta35_xe80_tc_lcw_L1DPHI-AJ20s2XE50', 'L1_DPHI-AJ20s2XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1,['serial',-1,["2j30_0eta490_deta35","xe80_tc_lcw" ]]],

        ['2j30_0eta490_deta35_xe60_L1DPHI-AJ20s2XE50', 'L1_DPHI-AJ20s2XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1,['serial',-1,["2j30_0eta490_deta35","xe60" ]]],
        

        ['2j55_bmv2c1060_split_ht300_L14J20', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial',-1,['2j55_bmv2c1060_split', 'ht300_L14J20']]],


	# pp reference run
	['mu4_j25_dr05', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j25'] ]],

        # #supporting triggers
        ['mu4_j15_dr05', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j15'] ]],
        ['mu4_j35_dr05', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j35'] ]],
 
        ['mu6_j85_dr05',  'L1_MU6_J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j85'] ]], 
        ['mu6_j110_dr05', 'L1_MU6_J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j110'] ]],
        ['mu6_j150_dr05', 'L1_MU6_J40', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j150'] ]],
        ['mu6_j175_dr05', 'L1_MU6_J40', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j175']]],
        ['mu6_j260_dr05', 'L1_MU6_J75', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j260'] ]],
        ['mu6_j320_dr05', 'L1_MU6_J75', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j320'] ]],
        ['mu6_j400_dr05', 'L1_MU6_J75', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j400'] ]],


        # Extra calibration items seeded from L1Topo
        ['mu4_j55_boffperf_split_dr05_dz02_L1BTAG-MU4J15', 'L1_BTAG-MU4J15', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j55_boffperf_split'] ], False],
        ['mu6_j110_boffperf_split_dr05_dz02_L1BTAG-MU6J20', 'L1_BTAG-MU6J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j110_boffperf_split'] ], False],
        ['mu6_j150_boffperf_split_dr05_dz02_L1BTAG-MU6J20', 'L1_BTAG-MU6J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j150_boffperf_split'] ], False],
        ['mu6_j175_boffperf_split_dr05_dz02_L1BTAG-MU6J20', 'L1_BTAG-MU6J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j175_boffperf_split'] ], False],
        ['mu6_j260_boffperf_split_dr05_dz02_L1BTAG-MU6J20', 'L1_BTAG-MU6J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j260_boffperf_split'] ], False],
        ['mu6_j320_boffperf_split_dr05_dz02_L1BTAG-MU6J20', 'L1_BTAG-MU6J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j320_boffperf_split'] ], False],
        ['mu6_j400_boffperf_split_dr05_dz02_L1BTAG-MU6J20', 'L1_BTAG-MU6J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j400_boffperf_split'] ], False],

        #Calibration moved from physics
        ['2mu14_2j35_bperf',                 'L1_2MU10',   ['L1_2MU10',   ''],     [PhysicsStream], ['RATE:MuonBJet',  'BW:BJet'],  -1, ['serial',-1,['2mu14', '2j35_bperf']]],
        ['2e24_lhvloose_nod0_2j35_bperf',                  'L1_2EM20VH', ['L1_2EM20VH', ''],     [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet'],   -1, ['serial',-1,['2e24_lhvloose_nod0', '2j35_bperf']]],
        ['e28_lhmedium_nod0_L1EM24VHI_mu8noL1_2j35_bperf',    'L1_EM24VHI', ['L1_EM24VHI', '', ''], [PhysicsStream], ['RATE:EgammaMuon',  'BW:BJet'], -1, ['serial',-1,['e28_lhmedium_nod0_L1EM24VHI','mu8noL1','2j35_bperf'] ]],
        ['mu26_ivarmedium_j150_bperf_j35_bperf',     'L1_MU20MU21',    ['L1_MU20',    '', ''], [PhysicsStream], ['RATE:MuonBJet',  'BW:BJet'],  -1, ['serial',-1,['mu26_ivarmedium', 'j150_bperf','j35_bperf']]],
        ['mu26_ivarmedium_2j35_bperf',                'L1_MU20MU21',    ['L1_MU20',    '', ''], [PhysicsStream], ['RATE:MuonBJet',  'BW:BJet'],  -1, ['serial',-1,['mu26_ivarmedium', '2j35_bperf']]],
        ['e28_lhtight_nod0_ivarloose_j150_bperf_j35_bperf',                                 'L1_EM24VHI', ['L1_EM24VHI', '', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet'],  -1, ['serial',-1,['e28_lhtight_nod0_ivarloose', 'j150_bperf','j35_bperf']]],
        ['e26_lhtight_nod0_ivarloose_L1EM22VHIM_2j35_bperf',                       'L1_EM22VHIM', ['L1_EM22VHIM', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet'],  -1, ['serial',-1,['e26_lhtight_nod0_ivarloose_L1EM22VHIM', '2j35_bperf']]],
        ['e26_lhtight_nod0_ivarloose_L1EM22VHIM_2j35_bperf_split',                 'L1_EM22VHIM', ['L1_EM22VHIM', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet'],  -1, ['serial',-1,['e26_lhtight_nod0_ivarloose_L1EM22VHIM', '2j35_bperf_split']]],
        ['e28_lhtight_nod0_ivarloose_L1EM24VHIM_2j35_bperf',                       'L1_EM24VHIM', ['L1_EM24VHIM', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet'],  -1, ['serial',-1,['e28_lhtight_nod0_ivarloose_L1EM24VHIM', '2j35_bperf']]],
        ['e28_lhtight_nod0_ivarloose_L1EM24VHIM_2j35_bperf_split',                 'L1_EM24VHIM', ['L1_EM24VHIM', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet'],  -1, ['serial',-1,['e28_lhtight_nod0_ivarloose_L1EM24VHIM', '2j35_bperf_split']]],


        # Jet + MET
        ['j40_cleanT_xe80_L1XE50',    'L1_XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,["j40_cleanT","xe80_L1XE50"]]],

        # LLP chains
        ['j30_jes_cleanLLP_PS_llp_L1TAU30',             'L1_TAU30',             [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j30_jes_cleanLLP_PS_llp_L1TAU40',             'L1_TAU40',             [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        ['j30_jes_cleanLLP_PS_llp_pufix_L1TAU100',      'L1_TAU100',             [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j30_jes_cleanLLP_PS_llp_pufix_noiso_L1TAU100','L1_TAU100',             [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        ['j30_jes_cleanLLP_PS_llp_pufix_L1LLP-NOMATCH', 'L1_LLP-NOMATCH',       [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1, False],
        ['j30_jes_cleanLLP_PS_llp_pufix_noiso_L1LLP-NOMATCH', 'L1_LLP-NOMATCH',       [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1, False],

        ['j30_jes_cleanLLP_PS_llp_pufix_L1LLP-RO',       'L1_LLP-RO',       [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1, False],
        ['j30_jes_cleanLLP_PS_llp_pufix_noiso_L1LLP-RO', 'L1_LLP-RO',       [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1, False],

        ['j0_muvtx',                    'L1_2MU10',            [], [PhysicsStream], ['RATE:MuonJet','BW:Muon', 'BW:Jet'], -1],
        ['j0_muvtx_noiso',              'L1_2MU10',            [], [PhysicsStream], ['RATE:MuonJet','BW:Muon', 'BW:Jet'], -1],


        # reversed calo-ratio triggers
        ['j250_jes_cleanLLP_PS_revllp_L1TAU100',             'L1_TAU100',             [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j230_jes_cleanLLP_PS_revllp_L1TAU100',             'L1_TAU100',             [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j200_jes_cleanLLP_PS_revllp_trkiso_L1TAU100',             'L1_TAU100',             [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],


        ['e26_lhtight_nod0_iloose_2j35_bperf',   'L1_EM22VHI',      ['L1_EM22VHI', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet'],-1,['serial',-1,['e26_lhtight_nod0_iloose', '2j35_bperf'] ] ], 

        ['e26_lhtight_iloose_2j35_bperf', 'L1_EM22VHI', ['L1_EM22VHI', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet'],-1,['serial',-1,['e26_lhtight_iloose', '2j35_bperf']]],
        ['e26_lhtight_iloose_2j35_bperf_split', 'L1_EM22VHI', ['L1_EM22VHI', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet'],-1,['serial',-1,['e26_lhtight_iloose', '2j35_bperf_split']]],
        ['e26_lhtight_iloose_2j35_boffperf', 'L1_EM22VHI', ['L1_EM22VHI', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet'],-1,['serial',-1,['e26_lhtight_iloose', '2j35_boffperf']]],
        ['e26_lhtight_iloose_2j35_boffperf_split', 'L1_EM22VHI', ['L1_EM22VHI', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet'],-1,['serial',-1,['e26_lhtight_iloose', '2j35_boffperf_split']]],		


        #VBF +photon
        ['mu6_2j40_0eta490_invm1000_L1MU6_J30.0ETA49_2J20.0ETA49', 'L1_MU6_J30.0ETA49_2J20.0ETA49', ['L1_MU6',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["mu6","2j40_0eta490_invm1000"]]],


        # 4-jet items (VBF H->bb, hh->bbbb)
        ['mu4_j40_dr05_3j40_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j40', '3j40'] ]],

        # 
        #  B-jet + hT
        #
        ['2j55_bmedium_ht300_L14J20',              'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,['2j55_bmedium', 'ht300_L14J20']]],
        ['j55_btight_ht500_L14J20',              'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,['j55_btight', 'ht500_L14J20']]],
        
        ['2j55_bmv2c1077_split_ht300_L14J20',              'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,['2j55_bmv2c1077_split', 'ht300_L14J20']]],
        ['j55_bmv2c1077_split_ht500_L14J20',              'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,[ 'ht500_L14J20', 'j55_bmv2c1077_split']]],

        ['2j55_bmv2c1070_ht300_L14J20',  'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,['2j55_bmv2c1070', 'ht300_L14J20']]],
        ['j55_bmv2c1060_ht500_L14J20',              'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,['j55_bmv2c1060',  'ht500_L14J20']]],
        ['j55_bmv2c1070_ht500_L14J20',              'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,[ 'j55_bmv2c1070', 'ht500_L14J20']]],

        ['2j55_bmv2c1070_split_ht300_L14J20',  'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,[ '2j55_bmv2c1070_split', 'ht300_L14J20']]],
        ['j55_bmv2c1070_split_ht500_L14J20',              'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,[ 'j55_bmv2c1070_split', 'ht500_L14J20']]],


        # L1Topo LFV
        ['g10_etcut_mu10_L1LFV-EM8I',         'L1_LFV-EM8I',  ['L1_EM7', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g10_etcut","mu10"]] ],
        ['g20_etcut_mu4_L1LFV-EM15I',         'L1_LFV-EM15I', ['L1_EM15', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g20_etcut","mu4"]] ],
        ['g10_etcut_mu10_taumass',            'L1_LFV-EM8I',  ['L1_EM7', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g10_etcut","mu10"]] ],
        ['g20_etcut_mu4_taumass',             'L1_LFV-EM15I', ['L1_EM15', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g20_etcut","mu4"]] ],
# taumass chains
        ['g10_etcut_mu10_iloose_taumass_L1LFV-EM8I',         'L1_LFV-EM8I',  ['L1_EM7', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g10_etcut","mu10_iloose"]] ],
        ['g20_etcut_mu4_iloose_taumass_L1LFV-EM15I',         'L1_LFV-EM15I',  ['L1_EM15', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g20_etcut","mu4_iloose"]] ],
        ### ATR-14543
        ['g10_etcut_mu10_ivarloose_taumass_L1LFV-EM8I',         'L1_LFV-EM8I',  ['L1_EM7', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g10_etcut","mu10_ivarloose"]] ],
        ['g20_etcut_mu4_ivarloose_taumass_L1LFV-EM15I',         'L1_LFV-EM15I',  ['L1_EM15', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g20_etcut","mu4_ivarloose"]] ],

        # L1Topo W T&P 
        ['e18_etcut_trkcut_xe35', 'L1_EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE',['L1_EM15',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e18_etcut_trkcut","xe35"]]],
        ['e18_etcut_trkcut_xe35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE_XS30', 'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE_XS30',['L1_EM15',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e18_etcut_trkcut","xe35"]]],
        ['e18_etcut_trkcut_xe35_mt35', 'L1_EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE', ['L1_EM15',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e18_etcut_trkcut","xe35"]]],
        ['e18_etcut_trkcut_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE_XS30_xe35_mt35', 'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE_XS30', ['L1_EM15',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e18_etcut_trkcut","xe35"]]],

        # L1Topo J+XE+DPHI
        ['j80_xe80_1dphi10_L1J40_DPHI-J20s2XE50', 'L1_J40_DPHI-J20s2XE50',['',''], [PhysicsStream],  ['RATE:JetMET', 'BW:Jet'], -1,['serial',-1,["j80","xe80"]]],
        ['j80_xe80_1dphi10_L1J40_DPHI-J20XE50', 'L1_J40_DPHI-J20XE50',['',''], [PhysicsStream],  ['RATE:JetMET', 'BW:Jet'], -1,['serial',-1,["j80","xe80"]]],
        ['j80_xe80_1dphi10_L1J40_DPHI-CJ20XE50','L1_J40_DPHI-CJ20XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1,['serial',-1,["j80","xe80"]]],
        ['j100_xe80_L1J40_DPHI-J20s2XE50',  'L1_J40_DPHI-J20s2XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1,['serial',-1,["j100","xe80"]]],
        ['j100_xe80_L1J40_DPHI-J20XE50',  'L1_J40_DPHI-J20XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1,['serial',-1,["j100","xe80"]]],
        ['j100_xe80_L1J40_DPHI-CJ20XE50', 'L1_J40_DPHI-CJ20XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1,['serial',-1,["j100","xe80"]]],
	

        #ATR-15243

        ['2mu4_xe40_mht_L12MU4_J20_XE30_DPHI-J20s2XE30',  'L1_2MU4_J20_XE30_DPHI-J20s2XE30', [], [PhysicsStream], ['RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["2mu4","xe40_mht"] ]],

        ['2e5_lhmedium_nod0_j50_xe80_mht_L1J40_XE50_DPHI-J20s2XE50',  'L1_J40_XE50_DPHI-J20s2XE50', ['L1_EM3','',''], [PhysicsStream], ['RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["2e5_lhmedium_nod0","j50","xe80_mht"] ]],


        ['e5_lhmedium_nod0',  'L1_EM3', ['L1_EM3','',''], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
       

        # L1Topo VBF 
        ['g15_loose_2j40_0eta490_3j25_0eta490', 'L1_MJJ-400', ['L1_EM12','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1,['serial',-1,["g15_loose","2j40_0eta490","3j25_0eta490"]]],
        ['g20_loose_2j40_0eta490_3j25_0eta490', 'L1_MJJ-800', ['L1_EM15VH','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1,['serial',-1,["g20_loose","2j40_0eta490","3j25_0eta490"]]],
        ['mu6_2j40_0eta490_invm400', 'L1_MU6_MJJ-200', ['L1_MU6',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu6","2j40_0eta490_invm400"]]],
        
        # Asymmetric items
        ['mu6_j50_dr05_2j35_L13J15_BTAG-MU6J25', 'L1_3J15_BTAG-MU6J25', ['L1_MU6', '', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu6', 'j50', '2j35'] ], False],
        ['mu4_j60_dr05_2j35_L13J15_BTAG-MU4J30', 'L1_3J15_BTAG-MU4J30', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet'], -1,['serial',-1,['mu4', 'j60', '2j35'] ], False],


     
        
	# e+tau
        # Low-threshold with jets
        ['e17_lhmedium_tau25_medium1_tracktwo',  'L1_EM15VHI_2TAU12IM_J25_3J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium","tau25_medium1_tracktwo"]]],

        # Isolated Variant
        ['e17_lhmedium_iloose_tau25_medium1_tracktwo',  'L1_EM15VHI_2TAU12IM_J25_3J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_iloose","tau25_medium1_tracktwo"]]],

        # High-threshold
        ['e17_lhmedium_tau80_medium1_tracktwo',   'L1_EM15VHI_TAU40_2TAU15',['L1_EM15VHI','L1_TAU40'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium","tau80_medium1_tracktwo"]]],

	# support for l1topo chains
        ['e17_lhmedium_nod0_tau25_medium1_tracktwo_L1EM15VHI_2TAU12IM',  'L1_EM15VHI_2TAU12IM',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_medium1_tracktwo"]]],
        #mu+tau chains seeded by L1MU10TAU20IM, needed as supporting triggers for the primary chains with either L1 jet or L1topo
        ['mu14_tau25_medium1_tracktwo_L1MU10_TAU12IM',  'L1_MU10_TAU12IM',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14","tau25_medium1_tracktwo"]]],       

        
        # Tau + e + MET
        ['e17_lhmedium_tau25_medium1_tracktwo_xe50', 'L1_EM15VHI_2TAU12IM_XE35',['L1_EM15VHI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium","tau25_medium1_tracktwo", "xe50"]]],
        ['e17_lhmedium_iloose_tau25_medium1_tracktwo_xe50', 'L1_EM15VHI_2TAU12IM_XE35',['L1_EM15VHI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_iloose","tau25_medium1_tracktwo", "xe50"]]],
        ['e17_lhmedium_nod0_tau25_medium1_tracktwo',  'L1_EM15VHI_2TAU12IM_J25_3J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_medium1_tracktwo"]]],

        ['e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_xe50', 'L1_EM15VHI_2TAU12IM_XE35',['L1_EM15VHI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_iloose","tau25_medium1_tracktwo", "xe50"]]],

        # Tau + Muon
        ['mu14_tau25_medium1_tracktwo',      'L1_MU10_TAU12IM_J25_2J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14","tau25_medium1_tracktwo"]]],     
        ['mu14_tau35_medium1_tracktwo',  'L1_MU10_TAU20IM',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14","tau35_medium1_tracktwo"]]],

        # ATR-15226
        ## ATR-14355: L1Topo bjet/HT items
        
        #
        # b + ht
        #

        # 4J20
        ['j35_gsc55_boffperf_split_ht500_L14J20', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet_combined'],  -1,['serial',-1,[ 'ht500_L14J20','j35_gsc55_boffperf_split']]],
        ['j35_gsc55_bmv2c1040_split_ht500_L14J20', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet_combined'],  -1,['serial',-1,[ 'ht500_L14J20','j35_gsc55_bmv2c1040_split']]],
        ['j35_gsc55_bmv2c1050_split_ht500_L14J20', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet_combined'],  -1,['serial',-1,[ 'ht500_L14J20','j35_gsc55_bmv2c1050_split']]],
        ['j55_gsc75_bmv2c1040_split_ht500_L14J20', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet_combined'],  -1,['serial',-1,[ 'ht500_L14J20','j55_gsc75_bmv2c1040_split']]],
        ['j55_gsc75_bmv2c1050_split_ht500_L14J20', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet_combined'],  -1,['serial',-1,[ 'ht500_L14J20','j55_gsc75_bmv2c1050_split']]],

        ['j35_gsc55_bmv2c1050_split_ht700_L14J20', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet_combined'],  -1,['serial',-1,[ 'ht700_L14J20','j35_gsc55_bmv2c1050_split']]],
        ['j35_gsc55_bmv2c1060_split_ht700_L14J20', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet_combined'],  -1,['serial',-1,[ 'ht700_L14J20','j35_gsc55_bmv2c1060_split']]],


        #
        # 2b +ht
        #
        ['2j55_boffperf_split_ht300_L14J20',        'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet_combined'],  -1,['serial',-1,[ 'ht300_L14J20','2j55_boffperf_split']]],
        ['2j35_gsc55_boffperf_split_ht300_L14J20',  'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet_combined'],  -1,['serial',-1,[ 'ht300_L14J20','2j35_gsc55_boffperf_split']]],
        ['2j35_gsc55_bmv2c1040_split_ht300_L14J20', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet_combined'],  -1,['serial',-1,[ 'ht300_L14J20','2j35_gsc55_bmv2c1040_split']]],
        ['2j35_gsc55_bmv2c1050_split_ht300_L14J20', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet_combined'],  -1,['serial',-1,[ 'ht300_L14J20','2j35_gsc55_bmv2c1050_split']]],
        ['2j35_gsc55_bmv2c1060_split_ht300_L14J20', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet_combined'],  -1,['serial',-1,[ 'ht300_L14J20','2j35_gsc55_bmv2c1060_split']]],
        ['2j35_gsc55_bmv2c1070_split_ht300_L14J20', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet_combined'],  -1,['serial',-1,[ 'ht300_L14J20','2j35_gsc55_bmv2c1070_split']]],
        ['j60_gsc100_bmv2c1060_split_xe80_mht_L1XE60','L1_XE60',[],[PhysicsStream], ['RATE:MultiBJet', 'BW:BJet_combined', 'BW:MET'], -1,['serial',-1,['xe80_mht_L1XE60','j60_gsc100_bmv2c1060_split']]],
        ['2j45_bmv2c1070_split_xe80_mht_L12J15_XE55','L1_2J15_XE55',[],[PhysicsStream], ['RATE:MultiBJet', 'BW:BJet_combined', 'BW:MET'], -1,['serial',-1,['xe80_mht','2j45_bmv2c1070_split']]],

        ['e24_lhmedium_nod0_L1EM20VHI_mu8noL1', 'L1_EM20VHI', ['L1_EM20VHI', ''], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['e24_lhmedium_nod0_L1EM20VHI','mu8noL1'] ]],

        ['e60_lhloose_nod0_xe60noL1',    'L1_EM22VHI',['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e60_lhloose_nod0","xe60noL1"]]],
        ['e60_lhloose_nod0_L1EM24VHIM_xe60noL1',    'L1_EM24VHIM',['L1_EM24VHIM',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e60_lhloose_nod0_L1EM24VHIM","xe60noL1"]]],

        ['g40_tight_xe40noL1',           'L1_EM20VH',  ['L1_EM20VH',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g40_tight","xe40noL1"]]],
        ['g45_tight_xe45noL1',           'L1_EM20VH',  ['L1_EM20VH',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g45_tight","xe45noL1"]]],
        ['g60_loose_xe60noL1',           'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g60_loose","xe60noL1"]]],
        ['g70_loose_xe70noL1',           'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g70_loose","xe70noL1"]]],
        ['g60_loose_L1EM24VHI_xe60noL1',           'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g60_loose_L1EM24VHI","xe60noL1"]]],
        ['g70_loose_L1EM24VHI_xe70noL1',           'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g70_loose_L1EM24VHI","xe70noL1"]]],
        ['g70_loose_L1EM24VHIM_xe70noL1',           'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g70_loose_L1EM24VHIM","xe70noL1"]]],

        ['g40_tight_icalovloose_xe40noL1', 	        'L1_EM20VHI', ['L1_EM20VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g40_tight_icalovloose","xe40noL1"]]],
        ['g45_tight_icalovloose_xe40noL1',           'L1_EM20VHI', ['L1_EM20VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g45_tight_icalovloose","xe40noL1"]]],
        ['g45_tight_icalovloose_xe45noL1', 	        'L1_EM20VHI', ['L1_EM20VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g45_tight_icalovloose","xe45noL1"]]],

        ['j100_xe80',                    'L1_J40_XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet', 'BW:MET'], -1,['serial',-1,["j100","xe80"]]],
        

	# ATR-14717
        ['j50_xe80_pufit_2dphi10_L1XE60',  'L1_XE60', [], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,["j50","xe80_pufit_L1XE60"] ]],
        ['j70_xe100_pufit_2dphi10_L1XE60',  'L1_XE60', [], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,["j70","xe100_pufit_L1XE60"] ]],

        # Wgg triggers

        ['e20_lhmedium_2g10_loose',  'L1_EM15VH_3EM7', ['L1_EM15VH','L1_2EM7'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ['e20_lhmedium_2g10_medium', 'L1_EM15VH_3EM7', ['L1_EM15VH','L1_2EM7'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ['j30_muvtx_L1MU4_UNPAIRED_NONISO', 'L1_MU4_UNPAIRED_NONISO', [], ["Late"], ['RATE:MuonJet','BW:Muon', 'BW:Jet'], -1],

        ['j30_jes_cleanLLP_PS_llp_L1TAU8_EMPTY',        'L1_TAU8_EMPTY',        [], ["Late"], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j30_jes_cleanLLP_PS_llp_L1TAU8_UNPAIRED_ISO', 'L1_TAU8_UNPAIRED_ISO', [], ["Late"], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j30_jes_cleanLLP_PS_llp_L1TAU8_UNPAIRED_NONISO', 'L1_TAU8_UNPAIRED_NONISO', [], ["Late"], ['RATE:SingleJet', 'BW:Jet'], -1],

        ['j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_EMPTY',        'L1_TAU8_EMPTY',        [], ["Late"], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_UNPAIRED_ISO',        'L1_TAU8_UNPAIRED_ISO',        [], ["Late"], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j30_jes_cleanLLP_PS_llp_noiso_L1TAU8_UNPAIRED_NONISO',     'L1_TAU8_UNPAIRED_NONISO',        [], ["Late"], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j30_muvtx_noiso_L1MU4_UNPAIRED_NONISO', 'L1_MU4_UNPAIRED_NONISO', [], ["Late"], ['RATE:MuonJet','BW:Muon', 'BW:Jet'], -1],
        ['g15_loose_2mu10_msonly_L1MU4_UNPAIRED_NONISO', 'L1_MU4_UNPAIRED_NONISO',['L1_EM8VH','L1_2MU10'], ["Late"], ['RATE:EgammaMuon', 'BW:Egamma','BW:Muon'],-1,['parallel',-1,[] ]],


        # Electron + Muon
        ['g10_etcut_L1EM8I_mu10_taumass',      'L1_EM8I_MU10',  ['L1_EM8I', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g10_etcut_L1EM8I","mu10"]] ],
        ['g20_etcut_L1EM15_mu4_iloose_taumass',      'L1_EM15I_MU4',  ['L1_EM15', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g20_etcut_L1EM15","mu4_iloose"]] ],

        ### ATR-14543
        ['g10_loose_L1EM8I_mu10_ivarloose_taumass',      'L1_EM8I_MU10',  ['L1_EM8I', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g10_loose_L1EM8I","mu10_ivarloose"]] ],
        ['g20_loose_L1EM15_mu4_ivarloose_taumass',      'L1_EM15I_MU4',  ['L1_EM15', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g20_loose_L1EM15","mu4_ivarloose"]] ],
        ['g10_etcut_L1EM8I_mu10_ivarloose_taumass',      'L1_EM8I_MU10',  ['L1_EM8I', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g10_etcut_L1EM8I","mu10_ivarloose"]] ],
        ['g20_etcut_L1EM15_mu4_ivarloose_taumass',      'L1_EM15I_MU4',  ['L1_EM15', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g20_etcut_L1EM15","mu4_ivarloose"]] ],

        # HLT photon and di-muon chains
        ['g35_loose_L1EM22VHI_mu18noL1',        'L1_EM22VHI', ['L1_EM22VHI', ''], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_loose_L1EM22VHI','mu18noL1'] ]],
        ['g35_loose_L1EM22VHI_mu15noL1_mu2noL1', 'L1_EM22VHI', ['L1_EM22VHI', '',''], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_loose_L1EM22VHI','mu15noL1','mu2noL1']]],
        # alternative HLT photon and di-muon chains with L1EM24VHI seed
        ['g35_loose_L1EM24VHI_mu18noL1',        'L1_EM24VHI', ['L1_EM24VHI', ''], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_loose_L1EM24VHI','mu18noL1'] ]],
        ['g35_loose_L1EM24VHIM_mu18noL1',        'L1_EM24VHIM', ['L1_EM24VHIM', ''], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_loose_L1EM24VHIM','mu18noL1'] ]],
        ['g35_loose_L1EM24VHI_mu15noL1_mu2noL1', 'L1_EM24VHI', ['L1_EM24VHI', '',''], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_loose_L1EM24VHI','mu15noL1','mu2noL1']]],
        ['g35_loose_L1EM24VHIM_mu15noL1_mu2noL1', 'L1_EM24VHIM', ['L1_EM24VHIM', '',''], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_loose_L1EM24VHIM','mu15noL1','mu2noL1']]],
 


	# COMBINED TAU CHAINS

	# e+tau
        #new ones   
	                                                                                                                     
	['e17_lhmedium_nod0_ivarloose_tau35_medium1_tracktwo_L1EM15VHI_TAU20IM_2TAU15_J25_2J20_3J15',   'L1_EM15VHI_TAU20IM_2TAU15_J25_2J20_3J15',['L1_EM15VHI','L1_TAU20IM'], [PhysicsStream],['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau35_medium1_tracktwo"]]],

	# support for l1topo chains
        ['e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_L1EM15VHI_2TAU12IM',  'L1_EM15VHI_2TAU12IM',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_nod0_iloose","tau25_medium1_tracktwo"]]],
        ### ATR-14543
        ['mu14_ivarloose_tau25_perf_tracktwo', 'L1_MU10_TAU12IM',['L1_MU10','L1_TAU12IM'], [PhysicsStream, 'express'], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_perf_tracktwo"]]],

        ['mu14_iloose_tau25_medium1_tracktwo',      'L1_MU10_TAU12IM_J25_2J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_iloose","tau25_medium1_tracktwo"]]],        
        ['mu14_iloose_tau35_medium1_tracktwo',  'L1_MU10_TAU20IM',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_iloose","tau35_medium1_tracktwo"]]],

        #mu+tau chains seeded by L1MU10TAU20IM, needed as supporting triggers for the primary chains with either L1 jet or L1topo
        ['mu14_iloose_tau25_medium1_tracktwo_L1MU10_TAU12IM',  'L1_MU10_TAU12IM',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_iloose","tau25_medium1_tracktwo"]]],
        
        # Ditau + MET
        ['mu14_iloose_tau25_medium1_tracktwo_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14_iloose", "tau25_medium1_tracktwo","xe50"]]],        
        ['e24_lhvloose_nod0_L1EM20VH_3j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e24_lhvloose_nod0_L1EM20VH","3j20noL1"] ]],
        ['e24_lhvloose_nod0_L1EM20VH_4j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e24_lhvloose_nod0_L1EM20VH","4j20noL1"] ]],
        ['e24_lhvloose_nod0_L1EM20VH_5j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e24_lhvloose_nod0_L1EM20VH","5j15noL1"] ]],
        ['e24_lhvloose_nod0_L1EM20VH_6j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e24_lhvloose_nod0_L1EM20VH","6j15noL1"] ]],
        ['e24_lhmedium_nod0_L1EM20VH_3j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e24_lhmedium_nod0_L1EM20VH","3j20noL1"] ]],
        ['e24_lhmedium_nod0_L1EM20VH_4j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e24_lhmedium_nod0_L1EM20VH","4j20noL1"] ]],
        ['e24_lhmedium_nod0_L1EM20VH_5j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e24_lhmedium_nod0_L1EM20VH","5j15noL1"] ]],
        ['e24_lhmedium_nod0_L1EM20VH_6j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e24_lhmedium_nod0_L1EM20VH","6j15noL1"] ]],
#        ['g25_medium_L1EM22VHI_j35_0eta490_bmv2c1077_3j35_0eta490_invm700', 'L1_EM22VHI', ['L1_EM22VHI', '', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet_combined','BW:Egamma'],-1,['serial',-1,['g25_medium_L1EM22VHI', 'j35_0eta490_bmv2c1077','3j35_0eta490_invm700']]],
#        ['g25_medium_L1EM22VHI_2j35_0eta490_bmv2c1077_2j35_0eta490', 'L1_EM22VHI', ['L1_EM22VHI', '', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet_combined','BW:Egamma'],-1,['serial',-1,['g25_medium_L1EM22VHI', '2j35_0eta490_bmv2c1077','2j35_0eta490']]],
        ['g25_medium_L1EM22VHI_j35_0eta490_bmv2c1077_3j35_0eta490_invm700', 'L1_EM22VHI', ['L1_EM22VHI', '', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet_combined','BW:Egamma'],-1,['serial',-1,['g25_medium_L1EM22VHI','3j35_0eta490_invm700','j35_0eta490_bmv2c1077']]],
        ['g25_medium_L1EM22VHI_2j35_0eta490_bmv2c1077_2j35_0eta490', 'L1_EM22VHI', ['L1_EM22VHI', '', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet_combined','BW:Egamma'],-1,['serial',-1,['g25_medium_L1EM22VHI', '2j35_0eta490','2j35_0eta490_bmv2c1077']]],

	# Hbb VBF trigger (ATR-13526)
        ['j80_0eta240_j60_j45_320eta490_invm700_AND_j45_bmv2c1070_split_j45_bmv2c1077_split','L1_J40.0ETA25_2J25_J20.31ETA49', ['','','','',''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet_combined' ], -1, ['serial',-1,['j80_0eta240','j60','j45_320eta490_invm700_AND','j45_bmv2c1070_split','j45_bmv2c1077_split'] ]],
        ['j55_bmv2c1070_split_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49','L1_J25.0ETA23_2J15.31ETA49',['',''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet_combined' ], -1, ['serial',-1,['2j45_320eta490_L1J25.0ETA23_2J15.31ETA49','j55_bmv2c1070_split'] ]],


        ['g75_tight_L1EM22VHI_3j50noL1', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g75_tight_L1EM22VHI","3j50noL1"]]],





#        ['j80_bmv2c1060_split_xe60_L12J50_XE40','L1_2J50_XE40',  [],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:BJet_combined'], -1,['serial',-1,["j80_bmv2c1060_split","xe60"]]], 
     #ATR-14196
#     	['j80_bmv2c1050_split_xe60_L12J50_XE40','L1_2J50_XE40',  [],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:BJet_combined'], -1,['serial',-1,["j80_bmv2c1050_split","xe60"]]],
#      	['j80_bmv2c1077_split_xe70_L1J400ETA25_XE50','L1_J40.0ETA25_XE50',  ['L1_J40.0ETA25','XE50'],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:BJet_combined'], -1,['serial',-1,["j80_bmv2c1077_split","xe70_L1J400ETA25_XE50"]]],

        ['j80_bmv2c1060_split_xe60_L12J50_XE40','L1_2J50_XE40',  [],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:BJet_combined'], -1,['serial',-1,["xe60","j80_bmv2c1060_split"]]],
        ['j80_bmv2c1077_split_xe70_L1J400ETA25_XE50','L1_J40.0ETA25_XE50',  ['L1_J40.0ETA25','XE50'],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:BJet_combined'], -1,['serial',-1,["xe70_L1J400ETA25_XE50","j80_bmv2c1077_split"]]],


        # MET+jet chains with cleaning
        ['j40_cleanT_xe80_L1XE60',    'L1_XE60', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,["j40_cleanT","xe80_L1XE60"]]],

        # muon plus multijets
        ['mu14_iloose_3j20_L1MU10_3J20', 'L1_MU10_3J20', ['L1_MU10_3J20', ''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon','BW:Jet'], -1,['serial',-1,['mu14_iloose', '3j20'] ],True],

        ['j30_muvtx_L1MU4_EMPTY',        'L1_MU4_EMPTY',        [], ["Late"], ['RATE:MuonJet','BW:Muon', 'BW:Jet'], -1],
        ['j30_muvtx_noiso_L1MU4_EMPTY',        'L1_MU4_EMPTY',        [], ["Late"], ['RATE:MuonJet','BW:Muon', 'BW:Jet'], -1],
        ['e26_lhmedium_5j30_0eta240_L1EM20VH_3J20',    	'L1_EM20VH_3J20',    ['L1_EM20VH_3J20',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1, ['serial',-1,["e26_lhmedium","5j30_0eta240"] ]],
        ['e24_lhmedium_5j30_0eta240_L1EM20VH_3J20',    	'L1_EM20VH_3J20',    ['L1_EM20VH_3J20',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1, ['serial',-1,["e24_lhmedium","5j30_0eta240"] ]],

        ['mu20_mu8noL1_e9_lhvloose_nod0_L1EM7_MU11',    'L1_EM7_MU11',  ['L1_MU11','','L1_EM7'],   [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['serial',-1,['mu20','mu8noL1','e9_lhvloose_nod0'] ]],
			 ]






    TriggerFlags.MinBiasSlice.signatures = TriggerFlags.MinBiasSlice.signatures() + [
        #HMT
        ['mb_sp2000_trk70_hmt', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],        
        ['mb_sp2000_pusup600_trk70_hmt', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['mb_sptrk_vetombts2in_L1RD0_FILLED', 'L1_RD0_FILLED', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

	#supporting HMT triggers
	['mb_sp1800_hmtperf_L1TE40', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

        ]

    #Beamspot chanis first try ATR-9847                                                                                                                
    TriggerFlags.BeamspotSlice.signatures = TriggerFlags.BeamspotSlice.signatures()+ [
        ]
      

    if TriggerFlags.doFTK():
        TriggerFlags.BeamspotSlice.signatures = TriggerFlags.BeamspotSlice.signatures() + [                                                                 
            ['beamspot_idperf_FTK',          'L1_4J20', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
            ['beamspot_trkFS_FTK',           'L1_4J20', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
            ['beamspot_trkFS_FTKRefit', 'L1_4J20', [], ["BeamSpot"], ['RATE:BeamSpot', 'BW:BeamSpot'], -1],
            ['beamspot_idperf_FTKRefit', 'L1_4J20', [], ["BeamSpot"], ['RATE:BeamSpot', 'BW:BeamSpot'], -1],
            ]

    ## The following slices are only needed for Physics runnings and LS1 menus
    ## To be commented out for DC14
    TriggerFlags.CalibSlice.signatures   = TriggerFlags.CalibSlice.signatures() + []
    TriggerFlags.CosmicSlice.signatures  = TriggerFlags.CosmicSlice.signatures() + [
        ['id_cosmicid',               '', [], ['HLT_IDCosmic', 'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],
    ]
    TriggerFlags.StreamingSlice.signatures = TriggerFlags.StreamingSlice.signatures() + [

        # backups for L1Topo VBF items
        ['noalg_L1MU6_J30.0ETA49_2J20.0ETA49', 'L1_MU6_J30.0ETA49_2J20.0ETA49', [], [PhysicsStream], ['RATE:SeededStreamers', "BW:Muon"], -1],

        # LowLumiUnprescaled streamers
        ['noalg_L12EM10VH',                        'L1_2EM10VH',                        [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Egamma'], -1],
        ['noalg_L1EM15VH_MU10',                    'L1_EM15VH_MU10',                    [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Egamma', 'BW:Muon'], -1],
        ['noalg_L1EM15I_MU4',                       'L1_EM15I_MU4',                       [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Egamma', 'BW:Muon'], -1],
        ['noalg_L1EM7_MU10',                       'L1_EM7_MU10',                       [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Egamma', 'BW:Muon'], -1],
        ['noalg_L12EM8VH_MU10',                    'L1_2EM8VH_MU10',                    [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Egamma', 'BW:Muon'], -1],
        ['noalg_L1TAU20IM_2TAU12IM_J25_2J20_3J12', 'L1_TAU20IM_2TAU12IM_J25_2J20_3J12', [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1EM15VHI_2TAU12IM_XE35',           'L1_EM15VHI_2TAU12IM_XE35',           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1MU10_TAU12IM_XE35',              'L1_MU10_TAU12IM_XE35',              [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1TAU20IM_2TAU12IM_XE35',          'L1_TAU20IM_2TAU12IM_XE35',          [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1EM15VHI_2TAU12IM',                'L1_EM15VHI_2TAU12IM',                [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1EM15VHI_2TAU12IM_J25_3J12',       'L1_EM15VHI_2TAU12IM_J25_3J12',       [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1EM15VHI_TAU40_2TAU15',            'L1_EM15VHI_TAU40_2TAU15',            [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1MU10_TAU12IM_J25_2J12',          'L1_MU10_TAU12IM_J25_2J12',          [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1MU10_TAU12IM',                   'L1_MU10_TAU12IM',                   [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L14J15',                           'L1_4J15',                           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Jet'], -1],
        ['noalg_L13J25.0ETA23',                    'L1_3J25.0ETA23',                    [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Jet'], -1],
        ['noalg_L13J40',                           'L1_3J40',                           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Jet'], -1],
        ['noalg_L12J15_XE55',                      'L1_2J15_XE55',                      [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Jet'], -1],
        ['noalg_L1MU6_J40',                        'L1_MU6_J40',                        [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],

        
        ]

 
    #TriggerFlags.GenericSlice.signatures = TriggerFlags.GenericSlice.signatures() + []

    # Random Seeded EB chains which select at the HLT based on L1 TBP bits
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

ps_online_list=[
    'costmonitor',
    'noalg_L1All',
    'noalg_L1Standby',
    'noalg_L1TGC_BURST',
    'noalg_standby_L1RD0_EMPTY',
    'noalg_standby_L1RD0_FILLED',
    'robrequest',
    'robrequest_L1RD0_EMPTY',
    'timeburner',
]
ps_cosmic_list=[
    'e0_perf_L1EM3_EMPTY',
    'g0_perf_L1EM3_EMPTY',
    'ht0_L1J12_EMPTY',
    'id_cosmicid',
    'id_cosmicid_L1MU11_EMPTY',
    'id_cosmicid_L1MU4_EMPTY',
    'j0_L1J12_EMPTY',
    'j0_perf_bperf_L1J12_EMPTY',
    'j0_perf_bperf_L1RD0_EMPTY',
    'mb_sptrk_costr',
    'mb_sptrk_costr_L1RD0_EMPTY',
    'mu4_cosmic_L1MU11_EMPTY',
    'mu4_cosmic_L1MU4_EMPTY',
    'mu4_msonly_cosmic_L1MU11_EMPTY',
    'mu4_msonly_cosmic_L1MU4_EMPTY',
]
ps_calibmon_list=[
    'alfacalib',
    'conej140_320eta490_larpebj',
    'conej165_larpebj',
    'conej40_larpebj',
    'conej75_320eta490_larpebj',
    'cscmon_L1All',
    'g12_loose_larpeb',
    'g20_loose_larpeb_L1EM15',
    'g3_loose_larpeb',
    'g40_loose_larpeb',
    'g60_loose_larpeb',
    'g80_loose_larpeb',
    'ibllumi_L1RD0_ABORTGAPNOTCALIB',
    'ibllumi_L1RD0_FILLED',
    'ibllumi_L1RD0_UNPAIRED_ISO',
    'idcalib_trk16_central',
    'idcalib_trk16_fwd',
    'idcalib_trk29_central',
    'idcalib_trk29_fwd',
    'idcalib_trk9_central',
    'idcalib_trk9_fwd',
#    'idcalib_trk9_central_L1MBTS_1_VTE70',
#    'idcalib_trk9_fwd_L1MBTS_1_VTE70',
    'j40_L1ZB',
    'j0_perf_ds1_L1All',
    'l1calocalib',
    'l1calocalib_L1BGRP9',
    'larcalib_L1EM3_EMPTY',
    'larcalib_L1EM7_EMPTY',
    'larcalib_L1J12_EMPTY',
    'larcalib_L1J3031ETA49_EMPTY',
    'larcalib_L1TAU8_EMPTY',
    'larhec_L1EM7_FIRSTEMPTY',
    'larhec_L1J12_FIRSTEMPTY',
    'larhec_L1J30_FIRSTEMPTY',
    'larhec_L1TAU8_FIRSTEMPTY',
    'larnoiseburst_L1All',
    'larnoiseburst_L1J100',
    'larnoiseburst_L1J40_XE60',
    'larnoiseburst_L1J75',
    'larnoiseburst_L1XE60',
    'larnoiseburst_L1XE70',
    'larnoiseburst_L1XE80',
    'larnoiseburst_loose_L1All',
    'larnoiseburst_loose_rerun',
    'larnoiseburst_rerun',

    'larnoiseburst_L1XE55',

    'larps_L1EM3_EMPTY',
    'larps_L1EM7_EMPTY',
    'larps_L1EM7_FIRSTEMPTY',
    'larps_L1J12_EMPTY',
    'larps_L1J12_FIRSTEMPTY',
    'larps_L1J3031ETA49_EMPTY',
    'larps_L1J30_EMPTY',
    'larps_L1J30_FIRSTEMPTY',
    'larps_L1TAU8_EMPTY',
    'lumipeb_L1RD0_ABORTGAPNOTCALIB',
    'lumipeb_L1RD0_EMPTY',
    'lumipeb_L1RD0_FILLED',
    'lumipeb_L1RD0_UNPAIRED_ISO',
    'lumipeb_L1MBTS_1',
    'lumipeb_L1MBTS_1_UNPAIRED_ISO',
    'lumipeb_L1MBTS_2',
    'lumipeb_L1MBTS_2_UNPAIRED_ISO',
    'lumipeb_L1MBTS_2_LUCID',
    'lumipeb_vdm_L1MBTS_1',
    'lumipeb_vdm_L1MBTS_1_BGRP11',
    'lumipeb_vdm_L1MBTS_1_BGRP9',
    'lumipeb_vdm_L1MBTS_1_UNPAIRED_ISO',
    'lumipeb_vdm_L1MBTS_2',
    'lumipeb_vdm_L1MBTS_2_BGRP11',
    'lumipeb_vdm_L1MBTS_2_BGRP9',
    'lumipeb_vdm_L1MBTS_2_UNPAIRED_ISO',
    'lumipeb_vdm_L1MBTS_2_LUCID',
    'lumipeb_vdm_L1MBTS_2_LUCID_BGRP11',
    'lumipeb_vdm_L1MBTS_2_LUCID_BGRP9',
    'lumipeb_vdm_L1MBTS_2_LUCID_UNPAIRED_ISO',
    'lumipeb_vdm_L1RD0_BGRP11',
    'lumipeb_vdm_L1RD0_BGRP9',
    'lumipeb_vdm_L1RD0_FILLED',
    'lumipeb_vdm_L1RD0_UNPAIRED_ISO',
    'mu0_muoncalib',
    'mu0_muoncalib_ds3',
    'mu0_muoncalib_ds3_L1MU4_EMPTY',
    'mu0_muoncalib_L1MU4_EMPTY',
    'noalg_bkg_L1Bkg',
    'noalg_bkg_L1J12',
    'noalg_bkg_L1J12_BGRP12',
    'noalg_bkg_L1J30.31ETA49',
    'noalg_bkg_L1J30.31ETA49_BGRP12',
    'noalg_cosmiccalo_L1EM3_EMPTY',
    'noalg_cosmiccalo_L1J12_EMPTY',
    'noalg_cosmiccalo_L1EM7_EMPTY',
    'noalg_cosmiccalo_L1J12_FIRSTEMPTY',
    'noalg_cosmiccalo_L1J3031ETA49_EMPTY',
    'noalg_cosmiccalo_L1J30_EMPTY',
    'noalg_cosmiccalo_L1J30_FIRSTEMPTY',
    'noalg_cosmiccalo_L1RD1_EMPTY',
    'noalg_cosmiccalo_L1RD1_BGRP10',
    'noalg_cosmicmuons_L1MU11_EMPTY',
    'noalg_cosmicmuons_L1MU4_EMPTY',
    'noalg_idcosmic_L1TRT_EMPTY',
    'noalg_idcosmic_L1TRT_FILLED',
    'noalg_idmon_L1RD0_EMPTY',
    'noalg_idmon_L1RD0_FILLED',
    'noalg_idmon_L1RD0_UNPAIRED_ISO',
    'noalg_L1Calo',
    'noalg_L1Calo_EMPTY',
    'noalg_L1CALREQ2',
    'noalg_larcells_L1LAR-EM',
    'noalg_larcells_L1LAR-J',
    'noalg_to_L12MU20_OVERLAY',
    'noalg_zb_L1ZB',
    'satu20em_l1satmon_L1EM20VH_FIRSTEMPTY',
    'satu20em_l1satmon_L1EM22VHI_FIRSTEMPTY',
    'satu20em_l1satmon_L1J100',
    'satu20em_l1satmon_L1J100.31ETA49',
    'satu20em_l1satmon_L1J100.31ETA49_FIRSTEMPTY',
    'satu20em_l1satmon_L1J100_FIRSTEMPTY',
    'sct_noise',
    'tilecalib_laser',
    'rpcpeb_L1RD0_EMPTY',
    'idpsl1_L1IDprescaled'
    ]    
ps_eb_list=[
    'eb_high_L1RD2_FILLED',
    'eb_low_L1RD2_FILLED',
    'noalg_eb_L1ABORTGAPNOTCALIB_noPS',
    'noalg_eb_L1EMPTY_noPS',
    'noalg_eb_L1FIRSTEMPTY_noPS',
    'noalg_eb_L1PhysicsHigh_noPS',
    'noalg_eb_L1PhysicsLow_noPS',
    'noalg_eb_L1RD3_EMPTY',
    'noalg_eb_L1RD3_FILLED',
    'noalg_eb_L1UNPAIRED_ISO_noPS',
    'noalg_eb_L1UNPAIRED_NONISO_noPS',
    ]
ps_beamspot_list=[
    'beamspot_activeTE_trkfast_peb',
    'beamspot_activeTE_trkfast_peb_L1TRT_EMPTY',
    'beamspot_activeTE_trkfast_peb_L1TRT_FILLED',
    'beamspot_activeTE_trkfast_pebTRT',
    'beamspot_activeTE_trkfast_pebTRT_L1TRT_EMPTY',
    'beamspot_activeTE_trkfast_pebTRT_L1TRT_FILLED',
    'beamspot_allTE_trkfast_peb',
    'beamspot_allTE_trkfast_peb_L1TRT_EMPTY',
    'beamspot_allTE_trkfast_peb_L1TRT_FILLED',
    'beamspot_allTE_trkfast_pebTRT',
    'beamspot_allTE_trkfast_pebTRT_L1TRT_EMPTY',
    'beamspot_allTE_trkfast_pebTRT_L1TRT_FILLED',
    'beamspot_trkFS_trkfast',
    'beamspot_trkFS_trkfast_L1TRT_EMPTY',
    'beamspot_trkFS_trkfast_L1TRT_FILLED',
    'beamspot_trkFS_trkfast_L1J15',
    'beamspot_allTE_trkfast_peb_L1J15',
    'beamspot_activeTE_trkfast_peb_L1J15',
    'beamspot_allTE_trkfast_peb_L13J15',
    'beamspot_trkFS_trkfast_L13J15',
    'beamspot_activeTE_trkfast_peb_L13J15',
#    'beamspot_activeTE_trkfast_peb_L1MBTS_1_VTE70',
#    'beamspot_allTE_trkfast_peb_L1MBTS_1_VTE70',
#    'beamspot_trkFS_trkfast_L1MBTS_1_VTE70'
    ]
ps_fwdphys_list=[
    'lhcfpeb',
    'lhcfpeb_L1LHCF_EMPTY',
    'lhcfpeb_L1LHCF_UNPAIRED_ISO',
    'noalg_L1LHCF',
    'noalg_L1LHCF_EMPTY',
    'noalg_L1LHCF_UNPAIRED_ISO',
]
ps_minb_list=[
    'mb_mbts_L1MBTS_1',
    'mb_mbts_L1MBTS_1_1',
    'mb_mbts_L1MBTS_1_1_EMPTY',
    'mb_mbts_L1MBTS_1_1_UNPAIRED_ISO',
    'mb_mbts_L1MBTS_1_EMPTY',
    'mb_mbts_L1MBTS_1_UNPAIRED_ISO',
    'mb_mbts_L1MBTS_2',
    'mb_mbts_L1MBTS_2_EMPTY',
    'mb_mbts_L1MBTS_2_UNPAIRED_ISO',
    'mb_idperf_L1MBTS_2',
    'mb_perf_L1LUCID',
    'mb_perf_L1LUCID_EMPTY',
    'mb_perf_L1LUCID_UNPAIRED_ISO',
    'mb_perf_L1MBTS_2',
    'mb_perf_L1RD1_FILLED',
    'mb_sptrk_vetombts2in_L1ZDC_AND',
    'mb_sptrk_vetombts2in_L1ZDC_A_C',
    'mb_sp600_pusup300_trk40_hmt_L1TE5',
    'mb_sp700_pusup350_trk50_hmt_L1TE5',
    'mb_sp900_pusup400_trk50_hmt_L1TE5',
    'mb_sp900_pusup400_trk60_hmt_L1TE5',
    'mb_sp1000_pusup450_trk70_hmt_L1TE5',
    'mb_sp1100_pusup450_trk70_hmt_L1TE5',
    'mb_sp1200_pusup500_trk80_hmt_L1TE5',
    'mb_sp1200_pusup500_trk90_hmt_L1TE5',
    'mb_sp1200_pusup500_trk100_hmt_L1TE5',
    'mb_sp1400_pusup550_trk90_hmt_L1TE5',
    'mb_sp1600_pusup600_trk100_hmt_L1TE5',
    'mb_sp1800_pusup700_trk110_hmt_L1TE5',
    'mb_sp2100_pusup900_trk120_hmt_L1TE5',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE5',
    'mb_sp600_pusup300_trk40_hmt_L1TE10',
    'mb_sp700_pusup350_trk50_hmt_L1TE10',
    'mb_sp900_pusup400_trk60_hmt_L1TE10',
    'mb_sp1000_pusup450_trk70_hmt_L1TE10',
    'mb_sp1100_pusup450_trk70_hmt_L1TE10',
    'mb_sp1200_pusup500_trk80_hmt_L1TE10',
    'mb_sp1400_pusup550_trk90_hmt_L1TE10',
    'mb_sp1600_pusup600_trk100_hmt_L1TE10',
    'mb_sp1700_pusup650_trk110_hmt_L1TE10',
    'mb_sp1800_pusup700_trk110_hmt_L1TE10',
    'mb_sp1900_pusup700_trk120_hmt_L1TE10',
    'mb_sp2100_pusup900_trk120_hmt_L1TE10',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE10',
    'mb_sp2500_pusup1100_trk140_hmt_L1TE10',
    'mb_sp700_pusup350_trk50_hmt_L1TE20',
    'mb_sp900_pusup400_trk60_hmt_L1TE20',
    'mb_sp1100_pusup450_trk70_hmt_L1TE20',
    'mb_sp1200_pusup500_trk80_hmt_L1TE20',
    'mb_sp1400_pusup550_trk90_hmt_L1TE20',
    'mb_sp1600_pusup600_trk100_hmt_L1TE20',
    'mb_sp1700_pusup650_trk110_hmt_L1TE20',
    'mb_sp1800_pusup700_trk110_hmt_L1TE20',
    'mb_sp1900_pusup700_trk120_hmt_L1TE20',
    'mb_sp2100_pusup750_trk130_hmt_L1TE20',
    'mb_sp2100_pusup900_trk120_hmt_L1TE20',
    'mb_sp2200_pusup800_trk140_hmt_L1TE20',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE20',
    'mb_sp2400_pusup850_trk150_hmt_L1TE20',
    'mb_sp2500_pusup1100_trk140_hmt_L1TE20',
    'mb_sp2700_pusup1200_trk150_hmt_L1TE20',
    'mb_sp1100_pusup450_trk70_hmt_L1TE30',
    'mb_sp1200_pusup500_trk80_hmt_L1TE30',
    'mb_sp1400_pusup550_trk90_hmt_L1TE30',
    'mb_sp1600_pusup600_trk100_hmt_L1TE30',
    'mb_sp1700_pusup650_trk110_hmt_L1TE30',
    'mb_sp1800_pusup700_trk110_hmt_L1TE30',
    'mb_sp1900_pusup700_trk120_hmt_L1TE30',
    'mb_sp2100_pusup750_trk130_hmt_L1TE30',
    'mb_sp2100_pusup900_trk120_hmt_L1TE30',
    'mb_sp2200_pusup800_trk140_hmt_L1TE30',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE30',
    'mb_sp2400_pusup850_trk150_hmt_L1TE30',
    'mb_sp2500_pusup1100_trk140_hmt_L1TE30',
    'mb_sp2700_pusup1200_trk150_hmt_L1TE30',
    'mb_sp2900_pusup1300_trk160_hmt_L1TE30',
    'mb_sp1400_pusup550_trk90_hmt_L1TE40',
    'mb_sp1600_pusup600_trk100_hmt_L1TE40',
    'mb_sp1700_pusup650_trk110_hmt_L1TE40',
    'mb_sp1900_pusup700_trk120_hmt_L1TE40',
    'mb_sp2100_pusup750_trk130_hmt_L1TE40',
    'mb_sp2200_pusup800_trk140_hmt_L1TE40',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE40',
    'mb_sp2400_pusup850_trk150_hmt_L1TE40',
    'mb_sp2500_pusup1100_trk140_hmt_L1TE40',
    'mb_sp2700_pusup1200_trk150_hmt_L1TE40',
    'mb_sp2900_pusup1300_trk160_hmt_L1TE40',
    'mb_sp1600_pusup600_trk100_hmt_L1TE50',
    'mb_sp1700_pusup650_trk110_hmt_L1TE50',
    'mb_sp1900_pusup700_trk120_hmt_L1TE50',
    'mb_sp2100_pusup750_trk130_hmt_L1TE50',
    'mb_sp2200_pusup800_trk140_hmt_L1TE50',
    'mb_sp2400_pusup850_trk150_hmt_L1TE50',
    #'mb_sp1700_pusup650_trk110_hmt_L1TE60',
    #'mb_sp1900_pusup700_trk120_hmt_L1TE60',
    #'mb_sp2100_pusup750_trk130_hmt_L1TE60',
    #'mb_sp2200_pusup800_trk140_hmt_L1TE60',
    #'mb_sp2400_pusup850_trk150_hmt_L1TE60',
    #'mb_sp1900_pusup700_trk120_hmt_L1TE70',
    #'mb_sp2100_pusup750_trk130_hmt_L1TE70',
    #'mb_sp2200_pusup800_trk140_hmt_L1TE70',
    #'mb_sp2400_pusup850_trk150_hmt_L1TE70',
    'mb_sp600_trk40_hmt_L1TE5',
    'mb_sp700_trk50_hmt_L1TE5',
    'mb_sp900_trk50_hmt_L1TE5',
    'mb_sp900_trk60_hmt_L1TE5',
    'mb_sp1000_trk70_hmt_L1TE5',
    'mb_sp1100_trk70_hmt_L1TE5',
    'mb_sp1200_trk80_hmt_L1TE5',
    'mb_sp1200_trk90_hmt_L1TE5',
    'mb_sp1200_trk100_hmt_L1TE5',
    'mb_sp1400_trk90_hmt_L1TE5',
    'mb_sp1600_trk100_hmt_L1TE5',
    'mb_sp1800_trk110_hmt_L1TE5',
    'mb_sp2100_trk120_hmt_L1TE5',
    'mb_sp2300_trk130_hmt_L1TE5',
    'mb_sp600_trk40_hmt_L1TE10',
    'mb_sp700_trk50_hmt_L1TE10',
    'mb_sp900_trk60_hmt_L1TE10',
    'mb_sp1000_trk70_hmt_L1TE10',
    'mb_sp1100_trk70_hmt_L1TE10',
    'mb_sp1200_trk80_hmt_L1TE10',
    'mb_sp1400_trk90_hmt_L1TE10',
    'mb_sp1600_trk100_hmt_L1TE10',
    'mb_sp1700_trk110_hmt_L1TE10',
    'mb_sp1800_trk110_hmt_L1TE10',
    'mb_sp1900_trk120_hmt_L1TE10',
    'mb_sp2100_trk120_hmt_L1TE10',
    'mb_sp2300_trk130_hmt_L1TE10',
    'mb_sp2500_trk140_hmt_L1TE10',
    'mb_sp700_trk50_hmt_L1TE20',
    'mb_sp900_trk60_hmt_L1TE20',
    'mb_sp1100_trk70_hmt_L1TE20',
    'mb_sp1200_trk80_hmt_L1TE20',
    'mb_sp1400_trk90_hmt_L1TE20',
    'mb_sp1600_trk100_hmt_L1TE20',
    'mb_sp1700_trk110_hmt_L1TE20',
    'mb_sp1800_trk110_hmt_L1TE20',
    'mb_sp1900_trk120_hmt_L1TE20',
    'mb_sp2100_trk130_hmt_L1TE20',
    'mb_sp2100_trk120_hmt_L1TE20',
    'mb_sp2200_trk140_hmt_L1TE20',
    'mb_sp2300_trk130_hmt_L1TE20',
    'mb_sp2400_trk150_hmt_L1TE20',
    'mb_sp2500_trk140_hmt_L1TE20',
    'mb_sp2700_trk150_hmt_L1TE20',
    'mb_sp1100_trk70_hmt_L1TE30',
    'mb_sp1200_trk80_hmt_L1TE30',
    'mb_sp1400_trk90_hmt_L1TE30',
    'mb_sp1600_trk100_hmt_L1TE30',
    'mb_sp1700_trk110_hmt_L1TE30',
    'mb_sp1800_trk110_hmt_L1TE30',
    'mb_sp1900_trk120_hmt_L1TE30',
    'mb_sp2100_trk130_hmt_L1TE30',
    'mb_sp2100_trk120_hmt_L1TE30',
    'mb_sp2200_trk140_hmt_L1TE30',
    'mb_sp2300_trk130_hmt_L1TE30',
    'mb_sp2400_trk150_hmt_L1TE30',
    'mb_sp2500_trk140_hmt_L1TE30',
    'mb_sp2700_trk150_hmt_L1TE30',
    'mb_sp2900_trk160_hmt_L1TE30',
    'mb_sp1400_trk90_hmt_L1TE40',
    'mb_sp1600_trk100_hmt_L1TE40',
    'mb_sp1700_trk110_hmt_L1TE40',
    'mb_sp1900_trk120_hmt_L1TE40',
    'mb_sp2100_trk130_hmt_L1TE40',
    'mb_sp2200_trk140_hmt_L1TE40',
    'mb_sp2300_trk130_hmt_L1TE40',
    'mb_sp2400_trk150_hmt_L1TE40',
    'mb_sp2500_trk140_hmt_L1TE40',
    'mb_sp2700_trk150_hmt_L1TE40',
    'mb_sp2900_trk160_hmt_L1TE40',
    'mb_sp1600_trk100_hmt_L1TE50',
    'mb_sp1700_trk110_hmt_L1TE50',
    'mb_sp1900_trk120_hmt_L1TE50',
    'mb_sp2100_trk130_hmt_L1TE50',
    'mb_sp2200_trk140_hmt_L1TE50',
    'mb_sp2400_trk150_hmt_L1TE50',
    #'mb_sp1700_trk110_hmt_L1TE60',
    #'mb_sp1900_trk120_hmt_L1TE60',
    #'mb_sp2100_trk130_hmt_L1TE60',
    #'mb_sp2200_trk140_hmt_L1TE60',
    #'mb_sp2400_trk150_hmt_L1TE60',
    #'mb_sp1900_trk120_hmt_L1TE70',
    #'mb_sp2100_trk130_hmt_L1TE70',
    #'mb_sp2200_trk140_hmt_L1TE70',
    #'mb_sp2400_trk150_hmt_L1TE70',
    'mb_sp700_trk50_hmt_L1TE15',
    'mb_sp900_trk60_hmt_L1TE15',
    'mb_sp1100_trk70_hmt_L1TE15',
    'mb_sp1200_trk80_hmt_L1TE15',
    'mb_sp1400_trk90_hmt_L1TE15',
    'mb_sp1600_trk100_hmt_L1TE15',
    'mb_sp1700_trk110_hmt_L1TE15',
    'mb_sp1900_trk120_hmt_L1TE15',
    'mb_sp1100_trk70_hmt_L1TE25',
    'mb_sp1200_trk80_hmt_L1TE25',
    'mb_sp1400_trk90_hmt_L1TE25',
    'mb_sp1600_trk100_hmt_L1TE25',
    'mb_sp1700_trk110_hmt_L1TE25',
    'mb_sp1900_trk120_hmt_L1TE25',
    'mb_sp700_pusup350_trk50_hmt_L1TE15',
    'mb_sp900_pusup400_trk60_hmt_L1TE15',
    'mb_sp1100_pusup450_trk70_hmt_L1TE15',
    'mb_sp1200_pusup500_trk80_hmt_L1TE15',
    'mb_sp1400_pusup550_trk90_hmt_L1TE15',
    'mb_sp1600_pusup600_trk100_hmt_L1TE15',
    'mb_sp1700_pusup650_trk110_hmt_L1TE15',
    'mb_sp1900_pusup700_trk120_hmt_L1TE15',
    'mb_sp1100_pusup450_trk70_hmt_L1TE25',
    'mb_sp1200_pusup500_trk80_hmt_L1TE25',
    'mb_sp1400_pusup550_trk90_hmt_L1TE25',
    'mb_sp1600_pusup600_trk100_hmt_L1TE25',
    'mb_sp1700_pusup650_trk110_hmt_L1TE25',
    'mb_sp1900_pusup700_trk120_hmt_L1TE25',
    #'mb_sp900_pusup400_trk50_hmt_L1TE5.0ETA24',
    #'mb_sp900_pusup400_trk60_hmt_L1TE5.0ETA24',
    #'mb_sp1000_pusup450_trk70_hmt_L1TE5.0ETA24',
    #'mb_sp1200_pusup500_trk80_hmt_L1TE5.0ETA24',
    #'mb_sp1200_pusup500_trk90_hmt_L1TE5.0ETA24',
    #'mb_sp1200_pusup500_trk100_hmt_L1TE5.0ETA24',
    #'mb_sp1800_pusup700_trk110_hmt_L1TE5.0ETA24',
    #'mb_sp2100_pusup900_trk120_hmt_L1TE5.0ETA24',
    #'mb_sp2300_pusup1000_trk130_hmt_L1TE5.0ETA24',
    #'mb_sp1000_pusup450_trk70_hmt_L1TE10.0ETA24',
    #'mb_sp1200_pusup500_trk80_hmt_L1TE10.0ETA24',
    #'mb_sp1400_pusup550_trk90_hmt_L1TE10.0ETA24',
    #'mb_sp1600_pusup600_trk100_hmt_L1TE10.0ETA24',
    #'mb_sp1800_pusup700_trk110_hmt_L1TE10.0ETA24',
    #'mb_sp2100_pusup900_trk120_hmt_L1TE10.0ETA24',
    #'mb_sp2300_pusup1000_trk130_hmt_L1TE10.0ETA24',
    #'mb_sp2500_pusup1100_trk140_hmt_L1TE10.0ETA24',
    #'mb_sp1200_pusup500_trk80_hmt_L1TE15.0ETA24',
    #'mb_sp1400_pusup550_trk90_hmt_L1TE15.0ETA24',
    #'mb_sp1600_pusup600_trk100_hmt_L1TE15.0ETA24',
    #'mb_sp1800_pusup700_trk110_hmt_L1TE15.0ETA24',
    #'mb_sp2100_pusup900_trk120_hmt_L1TE15.0ETA24',
    #'mb_sp2300_pusup1000_trk130_hmt_L1TE15.0ETA24',
    #'mb_sp2500_pusup1100_trk140_hmt_L1TE15.0ETA24',
    #'mb_sp1400_pusup550_trk90_hmt_L1TE20.0ETA24',
    #'mb_sp1600_pusup600_trk100_hmt_L1TE20.0ETA24',
    #'mb_sp1800_pusup700_trk110_hmt_L1TE20.0ETA24',
    #'mb_sp2100_pusup900_trk120_hmt_L1TE20.0ETA24',
    #'mb_sp2300_pusup1000_trk130_hmt_L1TE20.0ETA24',
    #'mb_sp2500_pusup1100_trk140_hmt_L1TE20.0ETA24',
    #'mb_sp2700_pusup1200_trk150_hmt_L1TE20.0ETA24',
    #'mb_sp1600_pusup600_trk100_hmt_L1TE25.0ETA24',
    #'mb_sp1800_pusup700_trk110_hmt_L1TE25.0ETA24',
    #'mb_sp2100_pusup900_trk120_hmt_L1TE25.0ETA24',
    #'mb_sp2300_pusup1000_trk130_hmt_L1TE25.0ETA24',
    #'mb_sp2500_pusup1100_trk140_hmt_L1TE25.0ETA24',
    #'mb_sp2700_pusup1200_trk150_hmt_L1TE25.0ETA24',
    #'mb_sp2900_pusup1300_trk160_hmt_L1TE25.0ETA24',
    #'mb_sp1800_pusup700_trk110_hmt_L1TE30.0ETA24',
    #'mb_sp2100_pusup900_trk120_hmt_L1TE30.0ETA24',
    #'mb_sp2300_pusup1000_trk130_hmt_L1TE30.0ETA24',
    #'mb_sp2500_pusup1100_trk140_hmt_L1TE30.0ETA24',
    #'mb_sp2700_pusup1200_trk150_hmt_L1TE30.0ETA24',
    #'mb_sp2900_pusup1300_trk160_hmt_L1TE30.0ETA24',
    #'mb_sp2300_pusup1000_trk130_hmt_L1TE40.0ETA24',
    #'mb_sp2500_pusup1100_trk140_hmt_L1TE40.0ETA24',
    #'mb_sp2700_pusup1200_trk150_hmt_L1TE40.0ETA24',
    #'mb_sp2900_pusup1300_trk160_hmt_L1TE40.0ETA24',
    'mu4_mb_sp600_trk40_hmt_L1MU4_TE10',
    'mu4_mb_sp700_trk50_hmt_L1MU4_TE10',
    'mu4_mb_sp900_trk60_hmt_L1MU4_TE10',
    'mu4_mb_sp1100_trk70_hmt_L1MU4_TE10',
    'mu4_mb_sp1200_trk80_hmt_L1MU4_TE10',
    'mu4_mb_sp1400_trk90_hmt_L1MU4_TE10',
    'mu4_mb_sp1600_trk100_hmt_L1MU4_TE10',
    'mu4_mb_sp600_pusup300_trk40_hmt_L1MU4_TE10',
    'mu4_mb_sp700_pusup350_trk50_hmt_L1MU4_TE10',
    'mu4_mb_sp900_pusup400_trk60_hmt_L1MU4_TE10',
    'mu4_mb_sp1100_pusup450_trk70_hmt_L1MU4_TE10',
    'mu4_mb_sp1200_pusup500_trk80_hmt_L1MU4_TE10',
    'mu4_mb_sp1400_pusup550_trk90_hmt_L1MU4_TE10',
    'mu4_mb_sp1600_pusup600_trk100_hmt_L1MU4_TE10',
    'mu4_mb_sp1100_trk70_hmt_L1MU4_TE20',
    'mu4_mb_sp1200_trk80_hmt_L1MU4_TE20',
    'mu4_mb_sp1400_trk90_hmt_L1MU4_TE20',
    'mu4_mb_sp1600_trk100_hmt_L1MU4_TE20',
    'mu4_mb_sp1700_trk110_hmt_L1MU4_TE20',
    'mu4_mb_sp1100_pusup450_trk70_hmt_L1MU4_TE20',
    'mu4_mb_sp1200_pusup500_trk80_hmt_L1MU4_TE20',
    'mu4_mb_sp1400_pusup550_trk90_hmt_L1MU4_TE20',
    'mu4_mb_sp1600_pusup600_trk100_hmt_L1MU4_TE20',
    'mu4_mb_sp1700_pusup650_trk110_hmt_L1MU4_TE20',
    'mu4_mb_sp1100_trk70_hmt_L1MU4_TE30',
    'mu4_mb_sp1200_trk80_hmt_L1MU4_TE30',
    'mu4_mb_sp1400_trk90_hmt_L1MU4_TE30',
    'mu4_mb_sp1600_trk100_hmt_L1MU4_TE30',
    'mu4_mb_sp1700_trk110_hmt_L1MU4_TE30',
    'mu4_mb_sp1100_pusup450_trk70_hmt_L1MU4_TE30',
    'mu4_mb_sp1200_pusup500_trk80_hmt_L1MU4_TE30',
    'mu4_mb_sp1400_pusup550_trk90_hmt_L1MU4_TE30',
    'mu4_mb_sp1600_pusup600_trk100_hmt_L1MU4_TE30',
    'mu4_mb_sp1700_pusup650_trk110_hmt_L1MU4_TE30',
    'mu4_mb_sp1200_trk80_hmt_L1MU4_TE40',
    'mu4_mb_sp1400_trk90_hmt_L1MU4_TE40',
    'mu4_mb_sp1600_trk100_hmt_L1MU4_TE40',
    'mu4_mb_sp1700_trk110_hmt_L1MU4_TE40',
    #'mu4_mb_sp1400_trk90_hmt_L1MU4_TE50',
    #'mu4_mb_sp1600_trk100_hmt_L1MU4_TE50',
    #'mu4_mb_sp1700_trk110_hmt_L1MU4_TE50',
    #'mu4_mb_sp1400_pusup550_trk90_hmt_L1MU4_TE50',
    #'mu4_mb_sp1600_pusup600_trk100_hmt_L1MU4_TE50',
    #'mu4_mb_sp1700_pusup650_trk110_hmt_L1MU4_TE50',
    'mu4_mb_sp1200_pusup500_trk80_hmt_L1MU4_TE40',
    'mu4_mb_sp1400_pusup550_trk90_hmt_L1MU4_TE40',
    'mu4_mb_sp1600_pusup600_trk100_hmt_L1MU4_TE40',
    'mu4_mb_sp1700_pusup650_trk110_hmt_L1MU4_TE40',
    'j40_mb_sp600_pusup300_trk40_hmt',
    'j40_mb_sp700_pusup350_trk50_hmt',
    'j40_mb_sp900_pusup400_trk60_hmt',
    'j40_mb_sp1100_pusup450_trk70_hmt',
    'j40_mb_sp1200_pusup500_trk80_hmt',
    'j40_mb_sp1400_pusup550_trk90_hmt',
    'j40_mb_sp1600_pusup600_trk100_hmt',
    'j40_mb_sp600_trk40_hmt',
    'j40_mb_sp700_trk50_hmt',
    'j40_mb_sp900_trk60_hmt',
    'j40_mb_sp1100_trk70_hmt',
    'j40_mb_sp1200_trk80_hmt',
    'j40_mb_sp1400_trk90_hmt',
    'j40_mb_sp1600_trk100_hmt',
    'j50_mb_sp600_pusup300_trk40_hmt',
    'j50_mb_sp700_pusup350_trk50_hmt',
    'j50_mb_sp900_pusup400_trk60_hmt',
    'j50_mb_sp1100_pusup450_trk70_hmt',
    'j50_mb_sp1200_pusup500_trk80_hmt',
    'j50_mb_sp1400_pusup550_trk90_hmt',
    'j50_mb_sp1600_pusup600_trk100_hmt',
    'j50_mb_sp600_trk40_hmt',
    'j50_mb_sp700_trk50_hmt',
    'j50_mb_sp900_trk60_hmt',
    'j50_mb_sp1100_trk70_hmt',
    'j50_mb_sp1200_trk80_hmt',
    'j50_mb_sp1400_trk90_hmt',
    'j50_mb_sp1600_trk100_hmt',
    'j60_mb_sp600_pusup300_trk40_hmt',
    'j60_mb_sp700_pusup350_trk50_hmt',
    'j60_mb_sp900_pusup400_trk60_hmt',
    'j60_mb_sp1100_pusup450_trk70_hmt',
    'j60_mb_sp1200_pusup500_trk80_hmt',
    'j60_mb_sp1400_pusup550_trk90_hmt',
    'j60_mb_sp1600_pusup600_trk100_hmt',
    'j60_mb_sp600_trk40_hmt',
    'j60_mb_sp700_trk50_hmt',
    'j60_mb_sp900_trk60_hmt',
    'j60_mb_sp1100_trk70_hmt',
    'j60_mb_sp1200_trk80_hmt',
    'j60_mb_sp1400_trk90_hmt',
    'j60_mb_sp1600_trk100_hmt',
    'j110_mb_sp600_pusup300_trk40_hmt',
    'j110_mb_sp700_pusup350_trk50_hmt',
    'j110_mb_sp900_pusup400_trk60_hmt',
    'j110_mb_sp1100_pusup450_trk70_hmt',
    'j110_mb_sp1200_pusup500_trk80_hmt',
    'j110_mb_sp1400_pusup550_trk90_hmt',
    'j110_mb_sp1600_pusup600_trk100_hmt',
    'j110_mb_sp600_trk40_hmt',
    'j110_mb_sp700_trk50_hmt',
    'j110_mb_sp900_trk60_hmt',
    'j110_mb_sp1100_trk70_hmt',
    'j110_mb_sp1200_trk80_hmt',
    'j110_mb_sp1400_trk90_hmt',
    'j110_mb_sp1600_trk100_hmt',
    'j75_mb_sp600_pusup300_trk40_hmt',
    'j75_mb_sp700_pusup350_trk50_hmt',
    'j75_mb_sp900_pusup400_trk60_hmt',
    'j75_mb_sp1100_pusup450_trk70_hmt',
    'j75_mb_sp1200_pusup500_trk80_hmt',
    'j75_mb_sp1400_pusup550_trk90_hmt',
    'j75_mb_sp1600_pusup600_trk100_hmt',
    'j75_mb_sp600_trk40_hmt',
    'j75_mb_sp700_trk50_hmt',
    'j75_mb_sp900_trk60_hmt',
    'j75_mb_sp1100_trk70_hmt',
    'j75_mb_sp1200_trk80_hmt',
    'j75_mb_sp1400_trk90_hmt',
    'j75_mb_sp1600_trk100_hmt',
    'j85_mb_sp600_pusup300_trk40_hmt',
    'j85_mb_sp700_pusup350_trk50_hmt',
    'j85_mb_sp900_pusup400_trk60_hmt',
    'j85_mb_sp1100_pusup450_trk70_hmt',
    'j85_mb_sp1200_pusup500_trk80_hmt',
    'j85_mb_sp1400_pusup550_trk90_hmt',
    'j85_mb_sp1600_pusup600_trk100_hmt',
    'j85_mb_sp600_trk40_hmt',
    'j85_mb_sp700_trk50_hmt',
    'j85_mb_sp900_trk60_hmt',
    'j85_mb_sp1100_trk70_hmt',
    'j85_mb_sp1200_trk80_hmt',
    'j85_mb_sp1400_trk90_hmt',
    'j85_mb_sp1600_trk100_hmt',
    'mb_sp400_trk40_hmt_L1MBTS_1_1',
    'mb_sp600_trk45_hmt_L1MBTS_1_1',
    'mb_sp700_trk55_hmt_L1MBTS_1_1',
    'mb_sp900_trk60_hmt_L1MBTS_1_1',
    'mb_sp900_trk65_hmt_L1MBTS_1_1',
    'mb_sp1000_trk70_hmt_L1MBTS_1_1',
    'mb_sp1200_trk75_hmt_L1MBTS_1_1',
    'mb_sp1400_trk80_hmt_L1MBTS_1_1',
    #'mb_sp900_trk50_hmt_L1TE5.0ETA24',
    #'mb_sp900_trk60_hmt_L1TE5.0ETA24',
    #'mb_sp1000_trk70_hmt_L1TE5.0ETA24',
    #'mb_sp1200_trk80_hmt_L1TE5.0ETA24',
    #'mb_sp1200_trk90_hmt_L1TE5.0ETA24',
    #'mb_sp1200_trk100_hmt_L1TE5.0ETA24',
    #'mb_sp1800_trk110_hmt_L1TE5.0ETA24',
    #'mb_sp2100_trk120_hmt_L1TE5.0ETA24',
    #'mb_sp2300_trk130_hmt_L1TE5.0ETA24',
    #'mb_sp1000_trk70_hmt_L1TE10.0ETA24',
    #'mb_sp1200_trk80_hmt_L1TE10.0ETA24',
    #'mb_sp1400_trk90_hmt_L1TE10.0ETA24',
    #'mb_sp1600_trk100_hmt_L1TE10.0ETA24',
    #'mb_sp1800_trk110_hmt_L1TE10.0ETA24',
    #'mb_sp2100_trk120_hmt_L1TE10.0ETA24',
    #'mb_sp2300_trk130_hmt_L1TE10.0ETA24',
    #'mb_sp2500_trk140_hmt_L1TE10.0ETA24',
    #'mb_sp1200_trk80_hmt_L1TE15.0ETA24',
    #'mb_sp1400_trk90_hmt_L1TE15.0ETA24',
    #'mb_sp1600_trk100_hmt_L1TE15.0ETA24',
    #'mb_sp1800_trk110_hmt_L1TE15.0ETA24',
    #'mb_sp2100_trk120_hmt_L1TE15.0ETA24',
    #'mb_sp2300_trk130_hmt_L1TE15.0ETA24',
    #'mb_sp2500_trk140_hmt_L1TE15.0ETA24',
    #'mb_sp1400_trk90_hmt_L1TE20.0ETA24',
    #'mb_sp1600_trk100_hmt_L1TE20.0ETA24',
    #'mb_sp1800_trk110_hmt_L1TE20.0ETA24',
    #'mb_sp2100_trk120_hmt_L1TE20.0ETA24',
    #'mb_sp2300_trk130_hmt_L1TE20.0ETA24',
    #'mb_sp2500_trk140_hmt_L1TE20.0ETA24',
    #'mb_sp2700_trk150_hmt_L1TE20.0ETA24',
    #'mb_sp1600_trk100_hmt_L1TE25.0ETA24',
    #'mb_sp1800_trk110_hmt_L1TE25.0ETA24',
    #'mb_sp2100_trk120_hmt_L1TE25.0ETA24',
    #'mb_sp2300_trk130_hmt_L1TE25.0ETA24',
    #'mb_sp2500_trk140_hmt_L1TE25.0ETA24',
    #'mb_sp2700_trk150_hmt_L1TE25.0ETA24',
    #'mb_sp2900_trk160_hmt_L1TE25.0ETA24',
    #'mb_sp1800_trk110_hmt_L1TE30.0ETA24',
    #'mb_sp2100_trk120_hmt_L1TE30.0ETA24',
    #'mb_sp2300_trk130_hmt_L1TE30.0ETA24',
    #'mb_sp2500_trk140_hmt_L1TE30.0ETA24',
    #'mb_sp2700_trk150_hmt_L1TE30.0ETA24',
    #'mb_sp2900_trk160_hmt_L1TE30.0ETA24',
    #'mb_sp2300_trk130_hmt_L1TE40.0ETA24',
    #'mb_sp2500_trk140_hmt_L1TE40.0ETA24',
    #'mb_sp2700_trk150_hmt_L1TE40.0ETA24',
    #'mb_sp2900_trk160_hmt_L1TE40.0ETA24',
    'mb_sp800_hmtperf_L1TE5',
    'mb_sp800_hmtperf_L1TE10',
    'mb_sp1200_hmtperf_L1TE10',
    'mb_sp1600_hmtperf_L1TE20',
    'mb_sp2100_hmtperf_L1TE20',
    'mb_sp2100_hmtperf_L1TE30',
    'mb_sp2100_hmtperf_L1TE40',
    #'mb_sp800_hmtperf_L1TE5.0ETA24',
    #'mb_sp800_hmtperf_L1TE10.0ETA24',
    #'mb_sp1200_hmtperf_L1TE10.0ETA24',
    #'mb_sp1200_hmtperf_L1TE15.0ETA24',
    #'mb_sp1600_hmtperf_L1TE15.0ETA24',
    #'mb_sp1600_hmtperf_L1TE20.0ETA24',
    #'mb_sp2100_hmtperf_L1TE20.0ETA24',
    #'mb_sp2100_hmtperf_L1TE25.0ETA24',
    #'mb_sp2100_hmtperf_L1TE30.0ETA24',
    #'mb_sp2100_hmtperf_L1TE40.0ETA24',
    'mb_sp2000_trk70_hmt',
    'mb_sp2000_pusup600_trk70_hmt',
    'mb_sp1800_hmtperf_L1TE40',
    'mb_sp2_hmtperf_L1MBTS_1_1',
    'mb_sp2_hmtperf',
    'mb_sp500_hmtperf',
    'mb_sp1800_hmtperf',
    'mb_sptrk',
    'mb_sptrk_costr',
    'mb_sptrk_costr_L1RD0_EMPTY',
    'mb_sptrk_L1RD0_EMPTY',
    'mb_sptrk_L1RD0_UNPAIRED_ISO',
    'mb_sp_L1RD0_UNPAIRED_ISO',
    'mb_sp_ncb_L1RD0_UNPAIRED_ISO',
    'mb_sp_blayer_L1RD0_UNPAIRED_ISO',
    'mb_sp_L1RD0_FILLED',
    'mb_sptrk_L1RD3_FILLED',
    'mb_sptrk_noisesup',
    'mb_sptrk_noisesup_L1RD0_EMPTY',
    'mb_sptrk_noisesup_L1RD0_UNPAIRED_ISO',
    'mb_sptrk_noisesup_L1RD3_FILLED',
    'mb_sptrk_pt4_L1MBTS_1',
    'mb_sptrk_pt4_L1MBTS_1_1',
    'mb_sptrk_pt4_L1MBTS_2',
    'mb_sptrk_pt4_L1RD3_FILLED',
    'mb_sptrk_pt6_L1MBTS_1',
    'mb_sptrk_pt6_L1MBTS_1_1',
    'mb_sptrk_pt6_L1MBTS_2',
    'mb_sptrk_pt6_L1RD3_FILLED',
    'mb_sptrk_pt8_L1MBTS_1',
    'mb_sptrk_pt8_L1MBTS_1_1',
    'mb_sptrk_pt8_L1MBTS_2',
    'mb_sptrk_pt8_L1RD3_FILLED',
    'mb_sptrk_vetombts2in_L1RD0_FILLED',
    'mb_sptrk_trk80_L1MBTS_2',
    'mb_sptrk_pt2_L1MBTS_2',
##     'mb_sptrk_L1MBTS_1_VTE2',
##     'mb_sptrk_L1MBTS_1_VTE3',
##     'mb_sptrk_L1MBTS_1_VTE4',
##     'mb_sptrk_L1MBTS_1_VTE5',
##     'mb_sptrk_L1MBTS_1_VTE10',
##     'mb_sptrk_L1MBTS_2_VTE2',
##     'mb_sptrk_L1MBTS_2_VTE3',
##     'mb_sptrk_L1MBTS_2_VTE4',
##     'mb_sptrk_L1MBTS_2_VTE5',
##     'mb_sptrk_L1MBTS_2_VTE10',
##     'mb_sptrk_L1VTE2',
##     'mb_sptrk_L1VTE3',
##     'mb_sptrk_L1VTE4',
##     'mb_sptrk_L1VTE5',
##     'mb_sptrk_L1VTE10',
#    'mb_sptrk_L1MBTS_1',
    
    'noalg_mb_L1LUCID',
    'noalg_mb_L1LUCID_EMPTY',
    'noalg_mb_L1LUCID_UNPAIRED_ISO',
    'noalg_mb_L1MBTS_1',
    'noalg_mb_L1MBTS_1_1',
    'noalg_mb_L1MBTS_1_1_EMPTY',
    'noalg_mb_L1MBTS_1_1_UNPAIRED_ISO',
    'noalg_mb_L1MBTS_1_EMPTY',
    'noalg_mb_L1MBTS_1_UNPAIRED_ISO',
    'noalg_mb_L1MBTS_2',
    'noalg_mb_L1MBTS_2_EMPTY',
    'noalg_mb_L1MBTS_2_UNPAIRED_ISO',
    'noalg_mb_L1RD0_EMPTY',
    'noalg_mb_L1RD0_FILLED',
    'noalg_mb_L1RD0_UNPAIRED_ISO',
    'noalg_mb_L1RD1_FILLED',
    'noalg_mb_L1RD2_EMPTY',
    'noalg_mb_L1RD2_FILLED',
    'noalg_mb_L1RD3_EMPTY',
    'noalg_mb_L1RD3_FILLED',
    #'noalg_mb_L1TE5.0ETA24',
 ##    'noalg_mb_L1TE2',
##     'noalg_mb_L1TE3',
##     'noalg_mb_L1TE4',
    'noalg_mb_L1TE5',
    'noalg_mb_L1TE10',
    #'noalg_mb_L1TE10.0ETA24',
    'noalg_mb_L1TE20',
    #'noalg_mb_L1TE20.0ETA24',
    'noalg_mb_L1TE30',
    #'noalg_mb_L1TE30.0ETA24',
    'noalg_mb_L1TE40',
    #'noalg_mb_L1TE40.0ETA24',
    'noalg_mb_L1TE50',
    'noalg_mb_L1TE15',
    'noalg_mb_L1TE25',
    #'noalg_mb_L1TE60',
    #'noalg_mb_L1TE70',
    #'noalg_mb_L1TE70.0ETA24',
    'noalg_mb_L1ZDC_A',
    'noalg_mb_L1ZDC_C',
    'noalg_mb_L1ZDC_AND',
    'noalg_mb_L1ZDC_A_C',
#    'mb_sp_L1MBTS_1_OVERLAY',
#    'mb_sptrk_L1MBTS_1_OVERLAY',
#    'noalg_L1MBTS_1_OVERLAY',
#    'noalg_L1TE20_OVERLAY',
    ]

ps_rerun_list = [

    'xe0noL1_l2fsperf',
    'xe0noL1_l2fsperf_mht',
    'xe0noL1_l2fsperf_mht_em',
    'xe0noL1_l2fsperf_pufit',
    'xe0noL1_l2fsperf_tc_em',
    'xe0noL1_l2fsperf_tc_lcw',

    ]

if TriggerFlags.doFTK():
    ps_rerun_list = [
        'xe0noL1_l2fsperf_trkmht_FTK',
        ]

ps_larnoiseburst_rerun_list = [
    'larnoiseburst_loose_rerun',
    'larnoiseburst_rerun',
    ]

ps_perform_list = [
    # clean up (ATR-16514)

    'mu0noL1',

    '2j35_bperf_split_2j35_L13J25.0ETA23',
    '2j55_bperf_split_L13J20_4J20.0ETA49_MJJ-400',
    'j150_bperf_split',
    'j150_bperf_split_j50_bperf_split',
    'j35_bperf',
    'j35_bperf_split',
    'j55_bperf_split',
    'j65_bperf_split_3j65_L13J25.0ETA23',
    'j70_bperf_split_3j70',
    'j70_bperf_split_3j70_L13J25.0ETA23',
    'j75_bperf_split_3j75',
    'j75_bperf_split_3j75_L13J25.0ETA23',

    'mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I',
    'mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25',
    'mu14_tau25_medium1_tracktwo_L1MU10_TAU12I-J25',
    'mu14_iloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I',
    'mu14_iloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25',
    'mu14_iloose_tau25_medium1_tracktwo_L1MU10_TAU12I-J25',
    'e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I',
    'e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',
    'e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_L1EM15TAU12I-J25',
    'e17_lhmedium_nod0_tau25_medium1_tracktwo_L1DR-EM15TAU12I',
    'e17_lhmedium_nod0_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',
    'e17_lhmedium_nod0_tau25_medium1_tracktwo_L1EM15TAU12I-J25',
    'e17_lhmedium_tau25_medium1_tracktwo_L1DR-EM15TAU12I',
    'e17_lhmedium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',
    'e17_lhmedium_ivarloose_tau25_medium1_tracktwo_L1EM15TAU12I-J25',
    'e17_lhmedium_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',
    'e17_lhmedium_iloose_tau25_medium1_tracktwo_L1EM15TAU12I-J25',



    'xe100_mht_xe65_L1XE70',
    'xe100_mht_xe70_L1XE70',
    'xe100_mht_xe75_L1XE70',
    'xe100_mht_xe80_L1XE70',
    'xe100_pufit_L1XE70',
    'xe100_pufit_wEFMu_L1XE70',
    'xe110_mht_xe65_L1XE70',
    'xe110_mht_xe70_L1XE70',
    'xe110_mht_xe75_L1XE70',
    'xe110_mht_xe80_L1XE70',
    'xe110_pufit_L1XE70',
    'xe110_pufit_wEFMu_L1XE70',
    'xe110_trkmht_xe100_mht_L1XE70',
    'xe120_mht_L1XE70',
    'xe120_mht_em_L1XE70',
    'xe120_pufit_L1XE70',
    'xe120_pufit_wEFMu_L1XE70',
    'xe120_tc_em_L1XE70',
    'xe120_tc_lcw_L1XE70',
    'xe120_trkmht_wEFMu_xe100_mht_L1XE70',
    'xe120_trkmht_xe100_mht_L1XE70',
    'xe130_trkmht_xe100_mht_L1XE70',
    'xe140_trkmht_xe100_mht_L1XE70',
    'xe90_pufit_L1XE70',
    'xe90_pufit_wEFMu_L1XE70',
    'xe95_pufit_L1XE70',
    'xe95_pufit_wEFMu_L1XE70',
    'xe100_L1XE80',
    'xe100_mht_L1XE80',
    'xe100_mht_wEFMu_L1XE80',
    'xe100_pueta_L1XE80',
    'xe100_pueta_wEFMu_L1XE80',
    'xe100_pufit_L1XE80',
    'xe100_pufit_wEFMu_L1XE80',
    'xe100_tc_em_L1XE80',
    'xe100_tc_em_wEFMu_L1XE80',
    'xe100_tc_lcw_L1XE80',
    'xe100_tc_lcw_wEFMu_L1XE80',
    'xe100_wEFMu_L1XE80',
    'xe100_pueta',
    'xe100_pueta_L1XE60',
    'xe100_pueta_wEFMu',
    'xe110_pueta_L1XE60',
    'xe50_pueta',
    'xe90_pueta',
    'xe90_pueta_L1XE50',
    'xe90_pueta_L1XE60',
    'xe90_pueta_wEFMu',
    'xe90_pueta_wEFMu_L1XE50',
    'xe95_pueta_wEFMu_L1XE50',
    'xe110_mht_FStracks_L1XE50',
    'xe35_mht',
    'xe35_mht_em',
    'xe35_pufit',
    'xe35_tc_em',
    'xe35_tc_lcw',
    'xe45',
    'xe45_mht',
    'xe45_mht_wEFMu',
    'xe45_mht_xe45',
    'xe45_pufit',
    'xe45_pufit_wEFMu',
    'xe45_tc_em',
    'xe45_tc_em_wEFMu',
    'xe45_tc_lcw',
    'xe45_tc_lcw_wEFMu',
    'xe45_wEFMu',
    'xe50',
    'xe50_mht_FStracks_L1XE50',
    'xe110_mht_em_L1KF-XE60',
    'xe110_mht_em_L1KF-XE65',
    'xe110_mht_em_L1KF-XE75',
    'xe110_mht_em_L1XE50',
    'xe110_mht_em_L1XE55',
    'xe110_mht_em_wEFMu_L1XE50',
    'xe110_mht_em_wEFMu_L1XE55',
    'xe120_mht_em_L1KF-XE60',
    'xe120_mht_em_L1KF-XE65',
    'xe120_mht_em_L1KF-XE75',
    'xe120_mht_em_L1XE50',
    'xe120_mht_em_L1XE55',
    'xe120_mht_em_L1XE60',
    'xe120_mht_em_wEFMu_L1XE60',

    'mu20_r1extr',
    'mu10_r1extr',
    'mu4_r1extr',
    'mu4noL1',
    'mu0_perf',

    'tau0_perf_ptonly_L1TAU100',
    'tau125_perf_tracktwo',
    'tau35_perf_tracktwo',
    'tau35_medium1_tracktwo_L1TAU20',
    'e17_lhmedium_iloose_tau25_perf_ptonly_L1EM15VHI_2TAU12IM',
    'mu14_iloose_tau25_perf_ptonly_L1MU10_TAU12IM',
    'tau160_idperf_tracktwo',
    'tau160_idperf_tracktwo_L1TAU100',
    'tau160_perf_tracktwo',
    'tau160_perf_tracktwo_L1TAU100',
    'tau35_perf_tracktwo',
    'tau35_perf_tracktwo_L1TAU20_tau25_perf_tracktwo_L1TAU12',
    'tau0_perf_ptonly_L1TAU12',
    'tau0_perf_ptonly_L1TAU60',
    'tau125_medium1_calo',
    'tau125_medium1_track',
    'tau125_perf_ptonly',
    'tau125_r1medium1',
    'tau125_r1perf',
    'tau160_idperf_track',
    'tau160_perf_track',
    'tau20_r1_idperf',
    'tau20_r1medium1',
    'tau20_r1perf',
    'tau25_idperf_track',
    'tau25_idperf_tracktwo2015',
    'tau25_loose1_ptonly',
    'tau25_medium1_calo',
    'tau25_medium1_mvonly',
    'tau25_medium1_ptonly',
    'tau25_medium1_track',
    'tau25_medium1_trackcalo',
    'tau25_medium1_tracktwo2015',
    'tau25_medium1_tracktwocalo',
    'tau25_perf_calo',
    'tau25_perf_calo_L1TAU12',
    'tau25_perf_ptonly',
    'tau25_perf_ptonly_L1TAU12',
    'tau25_perf_track',
    'tau25_perf_track_L1TAU12',
    'tau25_perf_trackcalo',
    'tau25_perf_tracktwo2015',
    'tau25_perf_tracktwo_L1TAU12',
    'tau25_perf_tracktwocalo',
    'tau25_r1_idperf',
    'tau25_r1perf',
    'tau25_tight1_ptonly',
    'tau29_r1medium1',
    'tau29_r1perf',
    'tau35_loose1_ptonly',
    'tau35_medium1_calo',
    'tau35_medium1_ptonly',
    'tau35_medium1_ptonly_L1TAU20',
    'tau35_medium1_track',
    'tau35_perf_ptonly',
    'tau35_tight1_ptonly',
    'tau5_perf_ptonly_L1TAU8',
    'tau80_medium1_calo',
    'tau80_medium1_track',
    'tau35_perf_ptonly_tau25_perf_ptonly_L1TAU20IM_2TAU12IM',
    '2j30_boffperf_L12J15_XE55',
    '2j35_boffperf_2j35_L13J25.0ETA23',
    '2j35_bperf_2j35_L13J25.0ETA23',
    '2j45_boffperf_2j45',
    '2j45_boffperf_2j45_L13J25.0ETA23',
    '2j45_bperf_2j45',
    '2j45_bperf_2j45_L13J25.0ETA23',
    '2j55_boffperf_2j55',
    '2j55_boffperf_2j55_L13J25.0ETA23',
    '2j55_boffperf_ht300_L14J20',
    '2j55_bperf_2j55',
    '2j55_bperf_2j55_L13J25.0ETA23',
    '2j55_bperf_L13J20_4J20.0ETA49_MJJ-400',
    '2j55_bperf_L13J20_4J20.0ETA49_MJJ-700',
    '2j55_bperf_L13J20_4J20.0ETA49_MJJ-800',
    '2j55_bperf_L13J20_4J20.0ETA49_MJJ-900',
    '2j55_bperf_ht300_L14J20',
    '2j65_boffperf_j65',
    '2j65_bperf_j65',
    '2j70_boffperf_j70',
    '2j70_bperf_j70',
    '2j75_boffperf_j75',
    '2j75_bperf_j75',
    '2mu14_2j35_boffperf',
    '2mu14_2j35_bperf',
    'e26_lhtight_iloose_2j35_boffperf',
    'e26_lhtight_iloose_2j35_bperf',
    'e26_lhtight_nod0_iloose_2j35_bperf',
    'j0_perf_bperf_L1J12_EMPTY',
    'j0_perf_bperf_L1MU10',
    'j0_perf_bperf_L1RD0_EMPTY',
    'j150_2j55_boffperf',
    'j150_2j55_bperf',
    'j110_boffperf',
    'j110_bperf',
    'j150_boffperf',
    'j150_boffperf_j50_boffperf',
    'j150_bperf',
    'j150_bperf_j50_bperf',
    'j175_boffperf',
    'j175_boffperf_j60_boffperf',
    'j175_bperf',
    'j175_bperf_j60_bperf',
    'j225_boffperf',
    'j225_bperf',
    'j25_boffperf',
    'j25_bperf',
    'j260_boffperf',
    'j260_bperf',
    'j300_boffperf',
    'j300_bperf',
    'j320_boffperf',
    'j320_bperf',
    'j400_boffperf',
    'j400_bperf',
    'j45_boffperf',
    'j45_bperf',
    'j45_bperf_3j45_L14J20',
    'j55_boffperf',
    'j55_boffperf_ht500_L14J20',
    'j55_bperf',
    'j55_bperf_ht500_L14J20',
    'j65_boffperf_3j65_L13J25.0ETA23',
    'j65_bperf_3j65_L13J25.0ETA23',
    'j70_boffperf_3j70',
    'j70_boffperf_3j70_L13J25.0ETA23',
    'j70_bperf_3j70',
    'j70_bperf_3j70_L13J25.0ETA23',
    'j75_boffperf_3j75',
    'j75_boffperf_3j75_L13J25.0ETA23',
    'j75_bperf_3j75',
    'j75_bperf_3j75_L13J25.0ETA23',
    'j80_boffperf_L12J50_XE40',
    'j80_boffperf_L1J400ETA25_XE50',
    'j80_boffperf_L1J40_XE50',
    'j80_boffperf_L1J40_XE60',
    'j80_boffperf_L1XE60',
    'mu4_j15_dr05' ,
    'mu4_j35_dr05'  ,
    'mu6_j110_dr05',
    'mu6_j150_dr05',
    'mu6_j175_dr05',
    'mu6_j260_dr05',
    'mu6_j320_dr05',
    'mu6_j400_dr05',
    'mu6_j85_dr05' ,
    'j0_perf_boffperf_L1J12_EMPTY',
    'j0_perf_boffperf_L1MU10',
    'j0_perf_boffperf_L1RD0_EMPTY',
    'j35_bperf_3j35',
    'j25_bperf_split',
    'j45_bperf_split',
    'j110_bperf_split',
    'j175_bperf_split',
    'j225_bperf_split',
    'j260_bperf_split',
    'j300_bperf_split',
    'j400_bperf_split',
    '2j70_bperf_split_j70',
    '2j75_bperf_split_j75',
    '2j45_bperf_split_2j45_L13J25.0ETA23',
    '2j55_bperf_split_2j55',
    '2j55_bperf_split_2j55_L13J25.0ETA23',
    '2j55_bperf_split_L13J20_4J20.0ETA49_MJJ-700',
    '2j55_bperf_split_L13J20_4J20.0ETA49_MJJ-800',
    '2j55_bperf_split_L13J20_4J20.0ETA49_MJJ-900',
    'j15_gsc35_boffperf_split',
    'j25_bperf_split',
    'j25_boffperf',
    'e4_etcut',
    'e5_lhmedium_nod0',
    'e5_lhloose_nod0_idperf'
    ]

ps_Jpsi_list = [
    'e5_lhtight_e4_etcut_Jpsiee',
    'e5_lhtight_nod0_e4_etcut_Jpsiee',
    'e5_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5',
    'e9_lhtight_e4_etcut_Jpsiee',
    'e9_lhtight_nod0_e4_etcut_Jpsiee',
    'e9_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM7',
    'e5_lhtight_e9_etcut_Jpsiee',
    'e5_lhtight_nod0_e9_etcut_Jpsiee',
    'e5_lhtight_nod0_e9_etcut_Jpsiee_L1JPSI-1M5-EM7',
    'e14_lhtight_e4_etcut_Jpsiee',
    'e14_lhtight_nod0_e4_etcut_Jpsiee',
    'e14_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM12',
    'e5_lhtight_e14_etcut_Jpsiee',
    'e5_lhtight_nod0_e14_etcut_Jpsiee',
    'e5_lhtight_nod0_e14_etcut_Jpsiee_L1JPSI-1M5-EM12',
    'e24_lhtight_L1EM20VH_e15_etcut_L1EM7_Zee',
    'e26_lhtight_e15_etcut_L1EM7_Zee',
]


ps_ftk_list = [
]

ps_Bphys_list = [
    '2mu4_bBmumux_BcmumuDsloose',
    '2mu4_bBmumux_BcmumuDsloose_noL2',
    'mu6_mu4_bBmumux_BcmumuDsloose_noL2',
    '2mu6_bBmumux_BcmumuDsloose_noL2',
    'mu10_mu6_bBmumux_BcmumuDsloose_noL2',
    '2mu10_bBmumux_BcmumuDsloose_noL2',
    '2mu4_bBmumux_BcmumuDsloose_noL2_L12MU4-B',
    '2mu4_bBmumux_BcmumuDsloose_noL2_L12MU4-BO',
    'mu6_mu4_bBmumux_BcmumuDsloose_noL2_L12MU4-B',
    'mu6_mu4_bBmumux_BcmumuDsloose_noL2_L1MU6MU4-BO',
    '2mu6_bBmumux_BcmumuDsloose_noL2_L12MU6-B',
    '2mu6_bBmumux_BcmumuDsloose_noL2_L12MU6-BO',
    'mu10_mu6_bBmumux_BcmumuDsloose_noL2_L1LFV-MU',
    
    
    
    
    
    '2mu6_bBmumux_BcmumuDsloose_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
    
    
    '2mu4_bBmumux_BcmumuDsloose_L1BPH-2M9-2MU4_BPH-0DR15-2MU4',
    'mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4',
    '2mu6_bBmumux_BcmumuDsloose_L1LFV-MU6',
    '2mu4_bBmumux_LbmumuLambda',
    'mu6_mu4_bBmumux_LbmumuLambda',
    '2mu6_bBmumux_LbmumuLambda',
    '2mu6_bBmumux_LbmumuLambda_L1LFV-MU6',
    '2mu4_bBmumux_LbmumuLambda_L1BPH-2M9-2MU4_BPH-0DR15-2MU4',
    'mu6_mu4_bBmumux_LbmumuLambda_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4',
    '2mu6_bBmumux_LbmumuLambda_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
    'mu6_mu4_bBmumux_BcmumuDsloose',
    'mu10_mu6_bBmumux_BcmumuDsloose',
    '2mu6_bBmumux_BcmumuDsloose',
    '2mu10_bBmumux_BcmumuDsloose',
    
    
    
    '2mu6_bBmumux_BcmumuDsloose_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
    '2mu4_bBmumux_BcmumuDsloose_L12MU4-B',
    '2mu4_bBmumux_BcmumuDsloose_L12MU4-BO',
    'mu6_mu4_bBmumux_BcmumuDsloose_L12MU4-B',
    'mu6_mu4_bBmumux_BcmumuDsloose_L1MU6MU4-BO',
    '2mu6_bBmumux_BcmumuDsloose_L12MU6-B',
    '2mu6_bBmumux_BcmumuDsloose_L12MU6-BO',
    'mu10_mu6_bBmumux_BcmumuDsloose_L1LFV-MU',
]

if TriggerFlags.doFTK():
    ps_ftk_list = [
        'beamspot_idperf_FTK',
        'beamspot_trkFS_FTK',
        'beamspot_trkFS_FTKRefit', 
        'beamspot_idperf_FTKRefit',
        'mu6_idperf_FTK',
        'mu24_idperf_FTK',
        'mu6_idperf_FTKRefit',
        'mu24_idperf_FTKRefit',
        'tau12_idperf_FTK',
        'tau12_perf_FTK',
        'tau12_perf0_FTK',
        'tau12_perf_FTKNoPrec',
        'tau12_perf0_FTKNoPrec',
        'tau12_medium0_FTK',
        'tau12_medium1_FTK',
        'tau12_medium0_FTKNoPrec',
        'tau12_medium1_FTKNoPrec',
        'tau25_idperf_tracktwo_L1TAU12IM_FTK',
        'tau25_perf_tracktwo_L1TAU12IM_FTK',
        'tau25_medium1_tracktwo_L1TAU12IM_FTK',
        'tau20_medium0_FTK_tau12_medium0_FTK_03dR30_L1DR-TAU20ITAU12I-J25_FTK',
        'tau20_medium1_FTK_tau12_medium1_FTK_03dR30_L1DR-TAU20ITAU12I-J25_FTK',
        'tau20_medium0_FTK_tau12_medium0_FTK_03dR30_ditauL_L1DR-TAU20ITAU12I-J25_FTK',
        'tau20_medium1_FTK_tau12_medium1_FTK_03dR30_ditauL_L1DR-TAU20ITAU12I-J25_FTK',
        'tau20_medium0_FTKNoPrec_tau12_medium0_FTKNoPrec_03dR30_L1DR-TAU20ITAU12I-J25_FTK',
        'tau20_medium1_FTKNoPrec_tau12_medium1_FTKNoPrec_03dR30_L1DR-TAU20ITAU12I-J25_FTK',
        'tau20_medium0_FTKNoPrec_tau12_medium0_FTKNoPrec_03dR30_ditauL_L1DR-TAU20ITAU12I-J25_FTK',
        'tau20_medium1_FTKNoPrec_tau12_medium1_FTKNoPrec_03dR30_ditauL_L1DR-TAU20ITAU12I-J25_FTK',
        'tau20_medium0_FTK_tau12_medium0_FTK_L1TAU20IM_2TAU12IM_J25_2J20_3J12_FTK',
        'tau20_medium1_FTK_tau12_medium1_FTK_L1TAU20IM_2TAU12IM_J25_2J20_3J12_FTK',
        'tau20_medium0_FTK_tau12_medium0_FTK_ditauL_L1TAU20IM_2TAU12IM_J25_2J20_3J12_FTK',
        'tau20_medium1_FTK_tau12_medium1_FTK_ditauL_L1TAU20IM_2TAU12IM_J25_2J20_3J12_FTK',
        'tau20_medium0_FTKNoPrec_tau12_medium0_FTKNoPrec_L1TAU20IM_2TAU12IM_J25_2J20_3J12_FTK',
        'tau20_medium1_FTKNoPrec_tau12_medium1_FTKNoPrec_L1TAU20IM_2TAU12IM_J25_2J20_3J12_FTK', 
        'tau20_medium0_FTKNoPrec_tau12_medium0_FTKNoPrec_ditauL_L1TAU20IM_2TAU12IM_J25_2J20_3J12_FTK',
        'tau20_medium1_FTKNoPrec_tau12_medium1_FTKNoPrec_ditauL_L1TAU20IM_2TAU12IM_J25_2J20_3J12_FTK',
        'mu26_ivarmedium_tau12_idperf_FTK',
        'mu26_ivarmedium_tau12_perf0_FTK',
        'mu26_ivarmedium_tau12_perf_FTK',
        'mu26_ivarmedium_tau12_perf0_FTKNoPrec',
        'mu26_ivarmedium_tau12_perf_FTKNoPrec',
        'mu26_ivarmedium_tau12_medium0_FTK',
        'mu26_ivarmedium_tau12_medium1_FTK',
        'mu26_ivarmedium_tau12_medium0_FTKNoPrec',
        'mu26_ivarmedium_tau12_medium1_FTKNoPrec',
        'mu26_ivarmedium_tau25_idperf_tracktwo_L1TAU12IM_FTK',
        'mu26_ivarmedium_tau25_perf_tracktwo_L1TAU12IM_FTK',
        'mu26_ivarmedium_tau25_medium1_tracktwo_L1TAU12IM_FTK',
        'j15_bperf_split_FTK',
        'j35_bperf_split_FTK',
        'j45_bperf_split_FTK',
        'j150_bperf_split_FTK',
        'j15_bperf_split_FTKVtx',
        'j35_bperf_split_FTKVtx',
        'j45_bperf_split_FTKVtx',
        'j150_bperf_split_FTKVtx',
        'j15_bperf_split_FTKRefit',
        'j35_bperf_split_FTKRefit',
        'j45_bperf_split_FTKRefit',
        'j150_bperf_split_FTKRefit',
        'j15_boffperf_split_FTK',
        'j35_boffperf_split_FTK',
        'j45_boffperf_split_FTK',
        'j150_boffperf_split_FTK', 
        'j15_boffperf_split_FTKVtx',
        'j35_boffperf_split_FTKVtx',
        'j45_boffperf_split_FTKVtx',
        'j150_boffperf_split_FTKVtx',
        'j15_boffperf_split_FTKRefit',
        'j35_boffperf_split_FTKRefit', 
        'j45_boffperf_split_FTKRefit', 
        'j150_boffperf_split_FTKRefit',
        '2j35_btight_split_2j35_L13J25.0ETA23_FTK', 
        '2j45_btight_split_2j45_L13J25.0ETA23_FTK', 
        'j175_bmedium_split_j60_bmedium_split_FTK',
        '2j35_btight_split_2j35_L13J25.0ETA23_FTKVtx', 
        '2j45_btight_split_2j45_L13J25.0ETA23_FTKVtx',
        'j175_bmedium_split_j60_bmedium_split_FTKVtx',
        '2j35_btight_split_2j35_L13J25.0ETA23_FTKRefit',
        '2j45_btight_split_2j45_L13J25.0ETA23_FTKRefit', 
        'j175_bmedium_split_j60_bmedium_split_FTKRefit',
        '2j35_bmv2c1070_2j35_L13J25.0ETA23_FTK',
        '2j45_bmv2c1077_2j45_L13J25.0ETA23_FTK',
        'j175_bmv2c1085_FTK',
        '2j35_bmv2c1070_2j35_L13J25.0ETA23_FTKVtx',
        '2j45_bmv2c1077_2j45_L13J25.0ETA23_FTKVtx',
        'j175_bmv2c1085_FTKVtx',
        '2j35_bmv2c1070_2j35_L13J25.0ETA23_FTKRefit',
        '2j45_bmv2c1077_2j45_L13J25.0ETA23_FTKRefit',
        'j175_bmv2c1085_FTKRefit',
        'j225_boffperf_split_FTK',
        'j225_boffperf_split_FTKVtx',
        'j225_boffperf_split_FTKRefit',
        '2j35_boffperf_2j35_FTK_L14J15.0ETA25',
        'j150_boffperf_j50_boffperf_FTK',
        '2j35_boffperf_2j35_FTKVtx_L14J15.0ETA25',
        'j150_boffperf_j50_boffperf_FTKVtx',
        '2j35_boffperf_2j35_FTKRefit_L14J15.0ETA25',
        'j150_boffperf_j50_boffperf_FTKRefit',
        '2j35_bmv2c1050_2j35_FTK_L14J15.0ETA25',
        'j225_bmv2c1050_FTK',
        'j150_bmv2c1050_j50_bmv2c1050_FTK',
        '2j35_bmv2c1050_2j35_FTKVtx_L14J15.0ETA25',
        'j225_bmv2c1050_FTKVtx',
        'j150_bmv2c1050_j50_bmv2c1050_FTKVtx',
        '2j35_bmv2c1050_2j35_FTKRefit_L14J15.0ETA25',
        'j225_bmv2c1050_FTKRefit',
        'j150_bmv2c1050_j50_bmv2c1050_FTKRefit',
        ]       

ps_streamers_list = [
     'noalg_L12EM10VH',
     'noalg_L12EM15',
     'noalg_L12EM3',
     'noalg_L12EM7',
     'noalg_L12EM8VH_MU10',
     'noalg_L12J15_XE55',
     'noalg_L12MU10',
     'noalg_L12MU4',
     'noalg_L12MU6',
     'noalg_L13J25.0ETA23',
     'noalg_L13J40',
     'noalg_L13MU4',
     'noalg_L14J15',
     'noalg_L1AFP_A',
     'noalg_L1AFP_A_AND_C',
     'noalg_L1AFP_A_AND_C_J12',
     'noalg_L1AFP_A_AND_C_MBTS_2',
     'noalg_L1AFP_A_AND_C_SPECTOF_J100',
     'noalg_L1AFP_A_AND_C_SPECTOF_J50',
     'noalg_L1AFP_A_AND_C_SPECTOF_J75',
     'noalg_L1AFP_A_AND_C_TE5',
     'noalg_L1AFP_A_OR_C',
     'noalg_L1AFP_A_OR_C_EMPTY',
     'noalg_L1AFP_A_OR_C_FIRSTEMPTY',
     'noalg_L1AFP_A_OR_C_J12',
     'noalg_L1AFP_A_OR_C_MBTS_2',
     'noalg_L1AFP_A_OR_C_TE5',
     'noalg_L1AFP_A_OR_C_UNPAIRED_ISO',
     'noalg_L1AFP_A_OR_C_UNPAIRED_NONISO',
     'noalg_L1AFP_C',
     'noalg_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4',
     'noalg_L1DR-MU10TAU12I',
     'noalg_L1DR-TAU20ITAU12I',
     'noalg_L1DY-BOX-2MU6',
     'noalg_L1DY-DR-2MU4',
     'noalg_L1EM10VH',
     'noalg_L1EM12',
     'noalg_L1EM15',
     'noalg_L1EM15I_MU4',
     'noalg_L1EM15VH',
     'noalg_L1EM15VHI_2TAU12IM',
     'noalg_L1EM15VHI_2TAU12IM_J25_3J12',
     'noalg_L1EM15VHI_2TAU12IM_XE35',
     'noalg_L1EM15VHI_TAU40_2TAU15',
     'noalg_L1EM15VH_3EM7',
     'noalg_L1EM15VH_MU10',
     'noalg_L1EM20VH',
     'noalg_L1EM22VHI',
     'noalg_L1EM3',
     'noalg_L1EM3_AFP_A_AND_C',
     'noalg_L1EM3_AFP_A_OR_C',
     'noalg_L1EM7',
     'noalg_L1EM7_MU10',
     'noalg_L1EM8VH',
     'noalg_L1J100',
     'noalg_L1J100.31ETA49',
     'noalg_L1J12',
     'noalg_L1J120',
     'noalg_L1J15',
     'noalg_L1J15.31ETA49',
     'noalg_L1J20',
     'noalg_L1J20.28ETA31',
     'noalg_L1J20.31ETA49',
     'noalg_L1J20_J20.31ETA49',
     'noalg_L1J25',
     'noalg_L1J30',
     'noalg_L1J30.31ETA49',
     'noalg_L1J40',
     'noalg_L1J400',
     'noalg_L1J50',
     'noalg_L1J50.31ETA49',
     'noalg_L1J75',
     'noalg_L1J75.31ETA49',
     'noalg_L1J85',
     'noalg_L1LFV-MU',
     'noalg_L1LowLumi',
     'noalg_L1MBTS_1_BGRP11',
     'noalg_L1MBTS_1_BGRP9',
     'noalg_L1MBTS_2',
     'noalg_L1MBTS_2_BGRP11',
     'noalg_L1MBTS_2_BGRP9',
     'noalg_L1MJJ-100',
     'noalg_L1MU10',
     'noalg_L1MU10_TAU12IM',
     'noalg_L1MU10_TAU12IM_J25_2J12',
     'noalg_L1MU10_TAU12IM_XE35',
     'noalg_L1MU11',
     'noalg_L1MU11_EMPTY',
     'noalg_L1MU20',
     'noalg_L1MU20MU21',
     'noalg_L1MU20MU21_FIRSTEMPTY',
     'noalg_L1MU20_FIRSTEMPTY',
     'noalg_L1MU21',
     'noalg_L1MU21_FIRSTEMPTY',
     'noalg_L1MU4',
     'noalg_L1MU4_AFP_A_AND_C',
     'noalg_L1MU4_AFP_A_OR_C',
     'noalg_L1MU4_EMPTY',
     'noalg_L1MU4_FIRSTEMPTY',
     'noalg_L1MU4_UNPAIRED_ISO',
     'noalg_L1MU6',
     'noalg_L1MU6_2MU4',
     'noalg_L1MU6_J20',
     'noalg_L1MU6_J30.0ETA49_2J20.0ETA49',
     'noalg_L1MU6_J40',
     'noalg_L1RD0_BGRP11',
     'noalg_L1RD0_BGRP9',
     'noalg_L1RD0_EMPTY',
     'noalg_L1RD0_FILLED',
     'noalg_L1RD1_FILLED',
     'noalg_L1RD2_EMPTY',
     'noalg_L1RD2_FILLED',
     'noalg_L1RD3_EMPTY',
     'noalg_L1RD3_FILLED',
     'noalg_L1TAU12',
     'noalg_L1TAU12IM',
     'noalg_L1TAU20',
     'noalg_L1TAU20IM',
     'noalg_L1TAU20IM_2TAU12IM',
     'noalg_L1TAU20IM_2TAU12IM_J25_2J20_3J12',
     'noalg_L1TAU20IM_2TAU12IM_XE35',
     'noalg_L1TAU20_2TAU12',
     'noalg_L1TAU30',
     'noalg_L1TAU40',
     'noalg_L1TAU60',
     'noalg_L1TAU8',
     'noalg_L1TE10',
     #'noalg_L1TE10.0ETA24',
     'noalg_L1TE20',
     #'noalg_L1TE20.0ETA24',
     'noalg_L1TE30',
     #'noalg_L1TE30.0ETA24',
     'noalg_L1TE40',
     #'noalg_L1TE40.0ETA24',
     'noalg_L1TE5',
     #'noalg_L1TE5.0ETA24',
     'noalg_L1TE50',
     #'noalg_L1TE60',
     #'noalg_L1TE70',
     'noalg_L1TE15',
     'noalg_L1TE25',
     'noalg_L1Topo',
     'noalg_L1XE10',
     'noalg_L1XE30',
     'noalg_L1XE300',
     'noalg_L1XE35',
     'noalg_L1XE40',
     'noalg_L1XE45',
     'noalg_L1XE50',
     'noalg_L1XE55',
     'noalg_L1XE60',
     'noalg_L1XE70',
     'noalg_L1XE80',
     'noalg_bkg_L1J15.31ETA49_UNPAIRED_ISO',
     'noalg_bkg_L1MBTS_2_UNPAIRED_ISO',
     'noalg_bkg_L1MBTS_4_A_UNPAIRED_ISO',
     'noalg_bkg_L1MBTS_4_C_UNPAIRED_ISO',
     'noalg_bkg_L1MU4_UNPAIRED_ISO',
     'noalg_bkg_L1RD0_UNPAIRED_ISO',
     'noalg_l1calo_L1EM12',
     'noalg_l1calo_L1EM15',
     'noalg_l1calo_L1EM7',
     'noalg_l1calo_L1J100',
     'noalg_l1calo_L1J10031ETA49',
     'noalg_l1calo_L1J120',
     'noalg_l1calo_L1J400',
     'noalg_l1calo_L1J5031ETA49',
     'noalg_l1calo_L1J75',
     'noalg_l1calo_L1J7531ETA49',
     'noalg_l1calo_L1TAU12',
     'noalg_l1calo_L1TAU20',
     'noalg_l1calo_L1TAU8',
     'noalg_l1topo_L12EM3',
     'noalg_l1topo_L12MU4',
     'noalg_l1topo_L12MU4-B',
     'noalg_l1topo_L12MU4-BO',
     'noalg_l1topo_L12MU6',
     'noalg_l1topo_L12MU6-B',
     'noalg_l1topo_L12MU6-BO',
     'noalg_l1topo_L13J15_BTAG-MU4J15',
     'noalg_l1topo_L13J15_BTAG-MU4J30',
     'noalg_l1topo_L13J15_BTAG-MU6J25',
     'noalg_l1topo_L13J20_4J20.0ETA49_MJJ-400',
     'noalg_l1topo_L13J20_4J20.0ETA49_MJJ-700',
     'noalg_l1topo_L13J20_4J20.0ETA49_MJJ-800',
     'noalg_l1topo_L13J20_4J20.0ETA49_MJJ-900',
     'noalg_l1topo_L13J20_BTAG-MU4J20',
     'noalg_l1topo_L14J15',
     'noalg_l1topo_L14J20.0ETA49',
     'noalg_l1topo_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
     'noalg_l1topo_L1BPH-8M15-2MU6',
     'noalg_l1topo_L1BPH-8M15-MU6MU4',
     'noalg_l1topo_L1BTAG-MU4J15',
     'noalg_l1topo_L1BTAG-MU6J20',
     'noalg_l1topo_L1DPHI-CJ20XE50',
     'noalg_l1topo_L1DPHI-J20XE50',
     'noalg_l1topo_L1DPHI-J20s2XE50',
     'noalg_l1topo_L1DR-EM15TAU12I-J25',
     'noalg_l1topo_L1DR-MU10TAU12I',
     'noalg_l1topo_L1DR-MU10TAU12I_TAU12I-J25',
     'noalg_l1topo_L1DR-TAU20ITAU12I',
     'noalg_l1topo_L1DR-TAU20ITAU12I-J25',
     'noalg_l1topo_L1DY-BOX-2MU6',
     'noalg_l1topo_L1DY-DR-2MU4',
     'noalg_l1topo_L1EM12_2EM3',
     'noalg_l1topo_L1EM12_W-MT25',
     'noalg_l1topo_L1EM15',
     'noalg_l1topo_L1EM15I_MU4',
     'noalg_l1topo_L1EM15TAU12I-J25',
     'noalg_l1topo_L1EM15_W-MT35',
     'noalg_l1topo_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE',
     'noalg_l1topo_L1EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EM15XE_XS30',
     'noalg_l1topo_L1EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE',
     'noalg_l1topo_L1EM3',
     'noalg_l1topo_L1EM7',
     'noalg_l1topo_L1EM7_2EM3',
     'noalg_l1topo_L1EM7_MU10',
     'noalg_l1topo_L1EM8I_MU10',
     'noalg_l1topo_L1HT150-J20.ETA31',
     'noalg_l1topo_L1HT150-J20s5.ETA31',
     'noalg_l1topo_L1HT150-JJ15.ETA49',
     'noalg_l1topo_L1HT150-JJ15.ETA49_MJJ-400',
     'noalg_l1topo_L1HT190-J15.ETA21',
     'noalg_l1topo_L1HT190-J15s5.ETA21',
     'noalg_l1topo_L1J100',
     'noalg_l1topo_L1J15',
     'noalg_l1topo_L1J20',
     'noalg_l1topo_L1J25',
     'noalg_l1topo_L1J30.0ETA49_2J20.0ETA49',
     'noalg_l1topo_L1J4-MATCH',
     'noalg_l1topo_L1J40_DPHI-CJ20XE50',
     'noalg_l1topo_L1J40_DPHI-J20XE50',
     'noalg_l1topo_L1J40_DPHI-J20s2XE50',
     'noalg_l1topo_L1J40_XE50',
     'noalg_l1topo_L1JPSI-1M5',
     'noalg_l1topo_L1JPSI-1M5-EM12',
     'noalg_l1topo_L1JPSI-1M5-EM7',
     'noalg_l1topo_L1KF-XE55',
     'noalg_l1topo_L1KF-XE60',
     'noalg_l1topo_L1KF-XE65',
     'noalg_l1topo_L1KF-XE75',
     'noalg_l1topo_L1LAR-EM',
     'noalg_l1topo_L1LAR-J',
     'noalg_l1topo_L1LFV-EM15I',
     'noalg_l1topo_L1LFV-EM8I',
     'noalg_l1topo_L1LFV-MU',
     'noalg_l1topo_L1LLP-NOMATCH',
     'noalg_l1topo_L1MJJ-100',
     'noalg_l1topo_L1MJJ-400',
     'noalg_l1topo_L1MJJ-800',
     'noalg_l1topo_L1MU10_2MU6',
     'noalg_l1topo_L1MU10_TAU12I-J25',
     'noalg_l1topo_L1MU10_TAU12IM',
     'noalg_l1topo_L1MU4',
     'noalg_l1topo_L1MU6MU4-BO',
     'noalg_l1topo_L1MU6_2MU4',
     'noalg_l1topo_L1MU6_2MU4-B',
     'noalg_l1topo_L1MU6_J20',
     'noalg_l1topo_L1MU6_MJJ-200',
     'noalg_l1topo_L1TAU12IM',
     'noalg_l1topo_L1TAU20IM_2TAU12IM',
     'noalg_l1topo_L1TAU20ITAU12I-J25',
     'noalg_l1topo_L1TAU30',
     'noalg_l1topo_L1TAU40',
     'noalg_l1topo_L1W-05RO-XEHT-0',
     'noalg_l1topo_L1W-250RO2-XEHT-0',
     'noalg_l1topo_L1W-90RO2-XEHT-0',
     'noalg_l1topo_L1W-HT20-JJ15.ETA49',
     'noalg_l1topo_L1W-NOMATCH',
     'noalg_l1topo_L1XE35',
     'noalg_l1topo_L1XE45',
     'noalg_l1topo_L1XE55',
     'noalg_l1topo_L1XE60',
     'noalg_l1topo_L1XE70',
     'noalg_mb_L1EM3',
     'noalg_mb_L1J12',
     'noalg_mb_L1MU4',
     'noalg_standby_L13J15',
     'noalg_standby_L14J15',
     'noalg_standby_L1J15',
]

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
# TIGHT mc prescales for performance
######################################################
Prescales.L1Prescales_tightperf_mc_prescale= deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_tightperf_mc_prescale = deepcopy(Prescales.HLTPrescales_loose_mc_prescale)

chain_list=ps_eb_list+ps_fwdphys_list+ps_minb_list+ps_Bphys_list#+ps_Jpsi_list # enabling JPSI ee again ATR-15162
if not TriggerFlags.doFTK():
    chain_list+=ps_ftk_list

Prescales.HLTPrescales_tightperf_mc_prescale.update(dict(map(None,chain_list,len(chain_list)*[ [-1, 0,-1] ])))

######################################################
# TIGHT mc prescales
######################################################
Prescales.L1Prescales_tight_mc_prescale  = deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_tight_mc_prescale = deepcopy(Prescales.HLTPrescales_tightperf_mc_prescale)
chain_list=ps_streamers_list + ps_perform_list + ps_Bphys_list#+ps_Jpsi_list # enabling JPSI ee again ATR-15162

#Prescales.HLTPrescales_tight_mc_prescale = deepcopy(Prescales.HLTPrescales_loose_mc_prescale)
#chain_list=ps_eb_list+ps_fwdphys_list+ps_minb_list+ps_ftk_list+ps_perform_list
Prescales.HLTPrescales_tight_mc_prescale.update(dict(map(None,chain_list,len(chain_list)*[ [-1, 0,-1] ])))
######################################################

######################################################
Prescales.L1Prescales_upgrade_mc_prescale  = deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_upgrade_mc_prescale = deepcopy(Prescales.HLTPrescales_tight_mc_prescale)
# Note: "upgrade" prescales are set with regular expressions at the end
#       of the setupMenu() function above 
######################################################
