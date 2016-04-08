# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TriggerMenu.menu.Physics_pp_v6 as physics_menu

from TriggerJobOpts.TriggerFlags          import TriggerFlags
from copy                                 import deepcopy



from AthenaCommon.Logging import logging
log = logging.getLogger( 'MC_pp_v6.py' )


#---------------------------------------------------------------------
#---------------------------------------------------------------------
def setupMenu():

    physics_menu.setupMenu()
    PhysicsStream="Main"

    # ---------------------------------------------------------------------------------------
    # INPUT FORMAT FOR CHAINS:
    # ['chainName',  'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep], OPTIONAL: [mergingStrategy, offset,[merginOrder] ]], topoStartsFrom = False
    # ----------------------------------------------------------------------------------------

    TriggerFlags.TestSlice.signatures = TriggerFlags.TestSlice.signatures() + [
        #['TestChain400',    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ]

    TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [

        ['mu20_ivarloose_L1MU15',     'L1_MU15',	   [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_ivarloose_L1MU15',     'L1_MU15',	   [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],


        ['mu20_r1extr',            'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10_r1extr',            'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu4_r1extr',             'L1_MU4',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_icalo',             'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_iloosecalo',       'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_imediumcalo',       'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],

        ['mu20_imedium_mu8noL1',    'L1_MU20',  ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_imedium','mu8noL1']]],
        ['mu20_iloose_mu8noL1',     'L1_MU20',  ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_iloose','mu8noL1']]],
        ['mu20_iloose_2mu4noL1',    'L1_MU20',  ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_iloose','2mu4noL1']]],

        ['mu18_l2idonly_mu8noL1', 'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu18_l2idonly','mu8noL1']]],
        ['mu22_l2idonly_mu8noL1', 'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22_l2idonly','mu8noL1']]],
        ['mu24_l2idonly_mu8noL1', 'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24_l2idonly','mu8noL1']]],

        ['mu18_l2idonly_2mu4noL1', 'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu18_l2idonly','2mu4noL1']]],
        ['mu22_l2idonly_2mu4noL1', 'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22_l2idonly','2mu4noL1']]],
        ['mu24_l2idonly_2mu4noL1', 'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24_l2idonly','2mu4noL1']]],

        # new test chains for dimuon inefficiency
        ['3mu4_noMuCombOvlpRm',    'L1_3MU4',           [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu14_noMuCombOvlpRm',   'L1_2MU10',          [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],



        #['2mu6_10invm30_pt2_z10_L1DY-BOX-2MU6', 'L1_DY-BOX-2MU6', ['L1_MU6','L1_MU6'], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1, False],

        # ['mu0noL1_fsperf',          '',         [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'],-1], # problematic ATR-11128 

        # commented out as no overlap removal is implemented yet (ATR-10739)
        # ['mu14_iloose_mu14',        'L1_2MU10', [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],

        # T&P chains
        #['mu24_imedium_mu20noL1_msonly',  'L1_MU20', [], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'],-1],        
        #['mu24_imedium_mu20noL1_msonly',  'L1_MU20', [], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'],-1],       





        ## Primary (narrow scan chains)
        #['mu11_2mu4noL1_nscan03_L1MU11_2MU6', 'L1_MU11_2MU6', ['L1_MU11_2MU6','L1_MU11'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11','2mu4noL1_nscan03']]],        
        ['mu14_imedium_mu6noL1_msonly',    'L1_MU10',  ['L1_MU10',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu14_imedium','mu6noL1_msonly']]],
    
        #new narrow-scan muon triggers for tau->3mu
        ['mu6_2mu4', 'L1_MU6_3MU4', ['L1_MU6','L1_3MU4'], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu6_mu4', 'L1_2MU6_3MU4', ['L1_2MU6','L1_MU4'], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],

        #nscan05 triggers rejecting CB muons
        ['mu11_msonly_mu6noL1_msonly_nscan05_noComb',       'L1_MU10',      ['L1_MU10','L2_mu11_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11_msonly','mu6noL1_msonly_nscan05_noComb']]],
        ['mu20_msonly_mu6noL1_msonly_nscan05_noComb',       'L1_MU20',      ['L1_MU20','L2_mu20_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu6noL1_msonly_nscan05_noComb']]],
#        ['mu20_msonly_mu10noL1_msonly_nscan05_noComb',   'L1_MU20',   ['L1_MU20','L2_mu20_msonly'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu10noL1_msonly_nscan05']]],


        #calorimeter-taged muon trigger ATR-12103
        #['mu20noL1_calotag_0eta010', '', [], [PhysicsStream], ['RATE:SingleMuon','BW:Muon'], -1,],
        #['mu20noL1_calotag_0eta500', '', [], [PhysicsStream], ['RATE:SingleMuon','BW:Muon'], -1,],
        #['mu12_mu20noL1_calotag', 'L1_MU10', ['L1_MU10',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu12','mu20noL1_calotag']]],
        #['mu12_2mu20noL1_calotag', 'L1_MU10', ['L1_MU10',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu12','2mu20noL1_calotag']]],

        ['mu18_iloose_mu8noL1_calotag_0eta010', 'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu18_iloose','mu8noL1_calotag_0eta010']]],
        ['mu22_iloose_mu8noL1_calotag_0eta010', 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22_iloose','mu8noL1_calotag_0eta010']]],
        ['mu24_iloose_mu8noL1_calotag_0eta010', 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24_iloose','mu8noL1_calotag_0eta010']]],
        ['mu18_mu8noL1_calotag_0eta010', 'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu18','mu8noL1_calotag_0eta010']]],
        ['mu22_mu8noL1_calotag_0eta010', 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','mu8noL1_calotag_0eta010']]],
        ['mu24_mu8noL1_calotag_0eta010', 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','mu8noL1_calotag_0eta010']]],

				['mu20_nomucomb_2mu4noL1', 'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_nomucomb','2mu4noL1']]],
				['mu22_nomucomb_2mu4noL1', 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22_nomucomb','2mu4noL1']]],
				['mu20_nomucomb_mu8noL1' , 'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_nomucomb','mu8noL1']]],
				['mu22_nomucomb_mu8noL1' , 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22_nomucomb','mu8noL1']]],



        ['mu10_mgonly_L1LATEMU10_J50',          'L1_LATEMU10_J50',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10_mgonly_L1LATEMU10_XE50',         'L1_LATEMU10_XE50',          [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20_imedium_L1MU10_2J20', 'L1_MU10_2J20', [], [PhysicsStream], ['RATE:Muonjet', 'BW:Muon','BW:Jet'], -1],                
        ['mu20_ivarmedium_L1MU10_2J20', 'L1_MU10_2J20', [], [PhysicsStream], ['RATE:Muonjet', 'BW:Muon','BW:Jet'], -1],                


        ]


    if TriggerFlags.doFTK():
        TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
            ['mu24_idperf_FTK',            'L1_MU20',           [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1], 
            ['mu6_idperf_FTK',             'L1_MU6',            [], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
            ['mu24_idperf_FTKRefit',       'L1_MU20',           [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1], 
            ['mu6_idperf_FTKRefit',        'L1_MU6',            [], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
            ]

    TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + [

        #multi-jet
        ['5j65_0eta240', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['5j70_0eta240', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],

        #Forward jets
        #['j60_240eta490',          'L1_J15.23ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        # Test chains, to be removed
        ['j85_cleanLLP',                    'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],


        ['j0_0i1c200m400TLA',             'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j0_0i1c400m600TLA',             'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j0_0i1c600m800TLA',             'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j0_0i1c800m1000TLA',             'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j0_1i2c100m300TLA',             'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j0_1i2c300m500TLA',             'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j0_1i2c500m700TLA',             'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j0_1i2c100m8000TLA',             'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j0_1i2c200m8000TLA',             'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],

        ['j0_0i1c500m900TLA',             'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
	['j0_1i2c500m900TLA',             'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
	['j0_1i2c600m800TLA',             'L1_J100',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
	
        ## Additional jet chains(ATR-12772)
        ['j60_TT', 'L1_J20', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        ['10j40_L16J15', 'L1_6J15' , [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],


        ['j400_a10_lcw_sub_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_a10_sub_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_a10_lcw_sub_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_a10_sub_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        ['j360_a10_lcw_sub_L1SC111', 'L1_SC111', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_a10_lcw_sub_L1SC111', 'L1_SC111', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_a10_lcw_sub_L1SC111', 'L1_SC111', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j460_a10_lcw_sub_L1SC111', 'L1_SC111', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        
        ['j80_0eta240_2j60_320eta490', 'L1_J40.0ETA25_2J15.31ETA49', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j80_0eta240_j60_j45_320eta490', 'L1_J40.0ETA25_2J25_J20.31ETA49', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        
				#Alternative calibrations for single jet: ATR:-13369
        ['j380_jes',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j380_lcw',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j380_lcw_jes',           'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j380_lcw_nojcalib',      'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j380_nojcalib',          'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_jes',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_lcw',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_lcw_jes',           'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_nojcalib',          'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_sub',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_jes',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_lcw',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_lcw_jes',           'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_lcw_nojcalib',      'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_nojcalib',          'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j175_320eta490_jes',       'L1_J50.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j175_320eta490_lcw',       'L1_J50.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j175_320eta490_lcw_jes',    'L1_J50.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j175_320eta490_lcw_nojcalib',    'L1_J50.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j175_320eta490_nojcalib',        'L1_J50.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j260_320eta490_jes',       'L1_J75.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j260_320eta490_lcw',       'L1_J75.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j260_320eta490_lcw_jes',    'L1_J75.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j260_320eta490_lcw_nojcalib',    'L1_J75.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j260_320eta490_nojcalib',        'L1_J75.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j300_a10_sub_L1J75', 'L1_J75', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j300_a10_lcw_sub_L1J75', 'L1_J75', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
# Not yet in L1 menu        ['j300_a10_lcw_sub_L1SC85', 'L1_SC85', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j360_a10_lcw_sub_L1J100'               , 'L1_J100',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j380_a10_lcw_sub_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j440_a10_lcw_sub_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
 
        # VBF triggers
        ['2j40_0eta490_invm250_L1XE55', 'L1_XE55',         [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],

        ]

    TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [

        ['j0_perf_bperf_L1RD0_EMPTY',  'L1_RD0_EMPTY',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j0_perf_bperf_L1MU10',       'L1_MU10',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j0_perf_bperf_L1J12_EMPTY',  'L1_J12_EMPTY',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['2j75_bperf_j75', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j70_bperf_j70', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j100_2j55_bperf', 'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bperf_2j55', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j45_bperf_2j45', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j75_bperf_3j75', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j70_bperf_3j70', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['2j55_bperf_2j55_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
#        ['2j45_bperf_2j45_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j75_bperf_3j75_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
#        ['j70_bperf_3j70_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j175_bperf_j60_bperf', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j300_bperf', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],

        # low lumi menu
        ['2j65_bperf_j65', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j45_bperf_2j45_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j35_bperf_2j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j70_bperf_3j70_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j65_bperf_3j65_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['2j45_bperf_2j45_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j35_bperf_2j35_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j70_bperf_3j70_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j65_bperf_3j65_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j225_bperf', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j150_bperf_j50_bperf', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j175_bperf', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],

        # boffperf 
#        ['j55_boffperf', 'L1_J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
#        ['j45_boffperf_3j45', 'L1_3J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
#        ['j45_boffperf_3j45_L13J15.0ETA25', 'L1_3J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
#        ['j45_boffperf_3j45_L13J20', 'L1_3J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],



        
        ### split configuration
        # main menu
        ['2j75_bperf_split_j75', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1], 
        ['2j70_bperf_split_j70', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j100_2j55_bperf_split', 'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bperf_split_2j55', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j45_bperf_split_2j45', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j75_bperf_split_3j75', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j70_bperf_split_3j70', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['2j55_bperf_split_2j55_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
#        ['2j45_bperf_split_2j45_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j75_bperf_split_3j75_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
#        ['j70_bperf_split_3j70_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j175_bperf_split_j60_bperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j300_bperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        
        # low lumi menu
        ['2j65_bperf_split_j65', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j45_bperf_split_2j45_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j35_bperf_split_2j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j70_bperf_split_3j70_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j65_bperf_split_3j65_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['2j45_bperf_split_2j45_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j35_bperf_split_2j35_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j70_bperf_split_3j70_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j65_bperf_split_3j65_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j225_bperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j150_bperf_split_j50_bperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j175_bperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],

        # boffperf chains
        ### ['j55_boffperf_split',                       'L1_J20',[],  [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ### ['j45_boffperf_split_3j45',                  'L1_3J15',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ### ['j45_boffperf_split_3j45_L13J150ETA24',     'L1_3J15.0ETA24',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ### ['j45_boffperf_split_3j45_L13J20',           'L1_3J20',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
       
        ### the existing chains        
        ### ['j175_bmedium',                 'L1_J100',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ### ['j75_bmedium_3j75',             'L1_4J20',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ### ['2j55_bmedium_2j55',            'L1_4J20',[], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ### ['2j45_bmedium_3j45',            'L1_5J15.0ETA24',[], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ### ['j175_bmedium_j60_bmedium',     'L1_J100',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],        
        ### ['j300_bloose',                  'L1_J100',[], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ###
        ### ['j175_bmedium_split',                    'L1_J100',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ### ['j75_bmedium_split_3j75',                'L1_4J20',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ### ['2j55_bmedium_split_2j55',               'L1_4J20',[], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ### ['2j45_bmedium_split_3j45',               'L1_5J15.0ETA24',[], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ###['j175_bmedium_split_j60_bmedium_split',  'L1_J100',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ### ['j300_bloose_split',                     'L1_J100',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        # split configuration:
        ### ['j55_bperf_split',                       'L1_J20',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ### ['j45_bperf_split_3j45',                  'L1_3J15',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ### ['j45_bperf_split_3j45_L13J150ETA24',     'L1_3J15.0ETA24',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ### ['j45_bperf_split_3j45_L13J20',           'L1_3J20',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],

#        ['mu4_3j35_dr05_j35_bperf_split_antimatchdr05mu_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu4', '3j35', 'j35_bperf_split_antimatchdr05mu'] ]],  
#        ['mu4_3j30_dr05_j30_bperf_split_antimatchdr05mu_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu4', '3j30', 'j30_bperf_split_antimatchdr05mu'] ]],
#        
#        ['mu4_3j35_dr05_j35_bperf_split_antimatchdr05mu_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j35', 'j35_bperf_split_antimatchdr05mu'] ], False],
#        ['mu4_3j30_dr05_j30_bperf_split_antimatchdr05mu_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j30', 'j30_bperf_split_antimatchdr05mu'] , False]],
#        
#        ['mu4_3j35_dr05_j35_bperf_split_antimatchdr05mu_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j35', 'j35_bperf_split_antimatchdr05mu'] ], False],
#        ['mu4_3j30_dr05_j30_bperf_split_antimatchdr05mu_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j30', 'j30_bperf_split_antimatchdr05mu'] ], False],
#        
#        ['mu4_2j40_dr05_j40_bperf_split_antimatchdr05mu_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j40', 'j40_bperf_split_antimatchdr05mu'] ], False],
#        ['mu4_2j35_dr05_j35_bperf_split_antimatchdr05mu_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j35', 'j35_bperf_split_antimatchdr05mu'] ], False],
#        ['mu4_2j40_dr05_j40_bperf_split_antimatchdr05mu_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j40', 'j40_bperf_split_antimatchdr05mu'] ], False],
#        ['mu4_2j35_dr05_j35_bperf_split_antimatchdr05mu_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j35', 'j35_bperf_split_antimatchdr05mu'] ], False],
#        
#        ['mu4_2j40_dr05_j40_bperf_split_antimatchdr05mu_L1MU4_3J20', 'L1_MU4_3J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j40', 'j40_bperf_split_antimatchdr05mu'] ]],
#        ['mu4_2j35_dr05_j35_bperf_split_antimatchdr05mu_L1MU4_3J20', 'L1_MU4_3J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j35', 'j35_bperf_split_antimatchdr05mu'] ]],
#
#
#
#        ['mu4_3j35_dr05_j35_boffperf_split_antimatchdr05mu_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu4', '3j35', 'j35_boffperf_split_antimatchdr05mu'] ]],  
#        ['mu4_3j30_dr05_j30_boffperf_split_antimatchdr05mu_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j30', 'j30_boffperf_split_antimatchdr05mu'] ]],
#        
#        ['mu4_3j35_dr05_j35_boffperf_split_antimatchdr05mu_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j35', 'j35_boffperf_split_antimatchdr05mu'] ], False],
#        ['mu4_3j30_dr05_j30_boffperf_split_antimatchdr05mu_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j30', 'j30_boffperf_split_antimatchdr05mu'] , False]],
#        

#        
#        ['mu4_2j40_dr05_j40_boffperf_split_antimatchdr05mu_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j40', 'j40_boffperf_split_antimatchdr05mu'] ], False],
#        ['mu4_2j35_dr05_j35_boffperf_split_antimatchdr05mu_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j35', 'j35_boffperf_split_antimatchdr05mu'] ], False],
#        ['mu4_2j40_dr05_j40_boffperf_split_antimatchdr05mu_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j40', 'j40_boffperf_split_antimatchdr05mu'] ], False],
#        ['mu4_2j35_dr05_j35_boffperf_split_antimatchdr05mu_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j35', 'j35_boffperf_split_antimatchdr05mu'] ], False],
#        
#        ['mu4_2j40_dr05_j40_boffperf_split_antimatchdr05mu_L1MU4_3J20', 'L1_MU4_3J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j40', 'j40_boffperf_split_antimatchdr05mu'] ]],
#        ['mu4_2j35_dr05_j35_boffperf_split_antimatchdr05mu_L1MU4_3J20', 'L1_MU4_3J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j35', 'j35_boffperf_split_antimatchdr05mu'] ]],
#


        ['2j55_bperf_split_L1J30_2J20_4J20.0ETA49_MJJ-400', 'L1_J30_2J20_4J20.0ETA49_MJJ-400', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bperf_split_L1J30_2J20_4J20.0ETA49_MJJ-700', 'L1_J30_2J20_4J20.0ETA49_MJJ-700', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bperf_split_L1J30_2J20_4J20.0ETA49_MJJ-800', 'L1_J30_2J20_4J20.0ETA49_MJJ-800', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bperf_split_L1J30_2J20_4J20.0ETA49_MJJ-900', 'L1_J30_2J20_4J20.0ETA49_MJJ-900', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bperf_split_L13J20_4J20.0ETA49_MJJ-400', 'L1_3J20_4J20.0ETA49_MJJ-400', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bperf_split_L13J20_4J20.0ETA49_MJJ-700', 'L1_3J20_4J20.0ETA49_MJJ-700', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bperf_split_L13J20_4J20.0ETA49_MJJ-800', 'L1_3J20_4J20.0ETA49_MJJ-800', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bperf_split_L13J20_4J20.0ETA49_MJJ-900', 'L1_3J20_4J20.0ETA49_MJJ-900', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],


        ['2j55_boffperf_split_L1J30_2J20_4J20.0ETA49_MJJ-400', 'L1_J30_2J20_4J20.0ETA49_MJJ-400', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_boffperf_split_L1J30_2J20_4J20.0ETA49_MJJ-700', 'L1_J30_2J20_4J20.0ETA49_MJJ-700', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_boffperf_split_L1J30_2J20_4J20.0ETA49_MJJ-800', 'L1_J30_2J20_4J20.0ETA49_MJJ-800', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_boffperf_split_L1J30_2J20_4J20.0ETA49_MJJ-900', 'L1_J30_2J20_4J20.0ETA49_MJJ-900', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_boffperf_split_L13J20_4J20.0ETA49_MJJ-400', 'L1_3J20_4J20.0ETA49_MJJ-400', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_boffperf_split_L13J20_4J20.0ETA49_MJJ-700', 'L1_3J20_4J20.0ETA49_MJJ-700', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_boffperf_split_L13J20_4J20.0ETA49_MJJ-800', 'L1_3J20_4J20.0ETA49_MJJ-800', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_boffperf_split_L13J20_4J20.0ETA49_MJJ-900', 'L1_3J20_4J20.0ETA49_MJJ-900', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],

        ['2j75_boffperf_j75', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j70_boffperf_j70', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j100_2j55_boffperf', 'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_boffperf_2j55', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j45_boffperf_2j45', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j75_boffperf_3j75', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j70_boffperf_3j70', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['2j55_boffperf_2j55_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
#        ['2j45_boffperf_2j45_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j75_boffperf_3j75_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
#        ['j70_boffperf_3j70_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j175_boffperf_j60_boffperf', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j300_boffperf', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],


        ['2j75_boffperf_split_j75', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j70_boffperf_split_j70', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j100_2j55_boffperf_split', 'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_boffperf_split_2j55', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j45_boffperf_split_2j45', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j75_boffperf_split_3j75', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j70_boffperf_split_3j70', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['2j55_boffperf_split_2j55_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j45_boffperf_split_2j45_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j75_boffperf_split_3j75_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j70_boffperf_split_3j70_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j175_boffperf_split_j60_boffperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j300_boffperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],

        ['j80_bmv2c2085_split_2j60_320eta490', 'L1_J40.0ETA25_2J15.31ETA49', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j80_bmv2c2085_split_j60_bmv2c2085_split_j45_320eta490', 'L1_J40.0ETA25_2J25_J20.31ETA49', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],



        
        ]
    if TriggerFlags.doFTK():
        TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [
            # FTK bperf chains
            ['j15_bperf_split_FTK', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            ['j35_bperf_split_FTK', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            ['j45_bperf_split_FTK', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            ['j150_bperf_split_FTK', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            
            ['j15_bperf_split_FTKVtx', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            ['j35_bperf_split_FTKVtx', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            ['j45_bperf_split_FTKVtx', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            ['j150_bperf_split_FTKVtx', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            
            ['j15_bperf_split_FTKRefit', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            ['j35_bperf_split_FTKRefit', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            ['j45_bperf_split_FTKRefit', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            ['j150_bperf_split_FTKRefit', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            
            # FTK boffperf chains
            
            ['j15_boffperf_split_FTK', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            ['j35_boffperf_split_FTK', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            ['j45_boffperf_split_FTK', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            ['j150_boffperf_split_FTK', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],

            ['j15_boffperf_split_FTKVtx', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            ['j35_boffperf_split_FTKVtx', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            ['j45_boffperf_split_FTKVtx', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            ['j150_boffperf_split_FTKVtx', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            
            ['j15_boffperf_split_FTKRefit', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            ['j35_boffperf_split_FTKRefit', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            ['j45_boffperf_split_FTKRefit', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            ['j150_boffperf_split_FTKRefit', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            
            # FTK physics chains 
            ['2j35_btight_split_2j35_L13J25.0ETA23_FTK', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
            ['2j45_btight_split_2j45_L13J25.0ETA23_FTK', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
            ['j175_bmedium_split_j60_bmedium_split_FTK', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        
            ['2j35_btight_split_2j35_L13J25.0ETA23_FTKVtx', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
            ['2j45_btight_split_2j45_L13J25.0ETA23_FTKVtx', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
            ['j175_bmedium_split_j60_bmedium_split_FTKVtx', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
            
            ['2j35_btight_split_2j35_L13J25.0ETA23_FTKRefit', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
            ['2j45_btight_split_2j45_L13J25.0ETA23_FTKRefit', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
            ['j175_bmedium_split_j60_bmedium_split_FTKRefit', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
            
            ['2j35_bmv2c2070_2j35_L13J25.0ETA23_FTK', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
            ['2j45_bmv2c2077_2j45_L13J25.0ETA23_FTK', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
            ['j175_bmv2c2085_FTK', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            
            ['2j35_bmv2c2070_2j35_L13J25.0ETA23_FTKVtx', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
            ['2j45_bmv2c2077_2j45_L13J25.0ETA23_FTKVtx', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
            ['j175_bmv2c2085_FTKVtx', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            
            ['2j35_bmv2c2070_2j35_L13J25.0ETA23_FTKRefit', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
            ['2j45_bmv2c2077_2j45_L13J25.0ETA23_FTKRefit', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
            ['j175_bmv2c2085_FTKRefit', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
            
            
            ]
        
        
    TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + [ 
            
	# ATR-10891 HMT min bias
	['te40_L1MBTS_1_1',              'L1_MBTS_1_1',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        #['te50_L1MBTS_1_1',              'L1_MBTS_1_1',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['te60_L1MBTS_1_1',              'L1_MBTS_1_1',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['te70_L1MBTS_1_1',              'L1_MBTS_1_1',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['te80_L1MBTS_1_1',              'L1_MBTS_1_1',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['te50_L1TE20',                  'L1_TE20',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['te60_L1TE40',                  'L1_TE40',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
	['te50_L1TE20.0ETA24',           'L1_TE20.0ETA24',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['te60_L1TE40.0ETA24',           'L1_TE40.0ETA24',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
	
        ['xe90_L1XE60',                'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],  
        ['xe100_L1XE60',               'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],  
        ['xe110_L1XE60',               'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],


 	['xe85_tc_lcw_L1XE50',               'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe85_L1XE50',               'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe85_mht_L1XE50',               'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe85_tc_em_L1XE50',               'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe95_pueta_L1XE50',               'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe95_pufit_L1XE50',               'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe85_tc_lcw_wEFMu_L1XE50',               'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe85_wEFMu_L1XE50',               'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe85_mht_wEFMu_L1XE50',               'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe95_pueta_wEFMu_L1XE50',               'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe95_pufit_wEFMu_L1XE50',               'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe85_tc_em_wEFMu_L1XE50',               'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

 	['xe90_tc_lcw_L1XE60',               'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe90_pueta_L1XE60',                 'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1], 
 	['xe90_pufit_L1XE60',                 'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe90_mht_L1XE60',                   'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],	
	
 	['xe100_tc_lcw_L1XE60',               'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe100_pueta_L1XE60',                 'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1], 
 	['xe100_pufit_L1XE60',                 'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe100_mht_L1XE60',                   'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

 	['xe110_tc_lcw_L1XE60',               'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe110_pueta_L1XE60',                'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe110_pufit_L1XE60',                'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe110_mht_L1XE60',                  'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],


 	['xe110_pueta_L1XE50',                'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe110_pufit_L1XE50',                'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe110_tc_em_L1XE50',                'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe110_L1XE50',                'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe110_wEFMu_L1XE50',                'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
 	['xe110_tc_em_wEFMu_L1XE50',                'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe120_pueta_L1XE50',                'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_pufit_L1XE50',                'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe45_pufit_wEFMu',          'L1_XE45',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe45_wEFMu',                'L1_XE45',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe45_mht_wEFMu',            'L1_XE45',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe45_pueta',                'L1_XE45',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe45_tc_lcw',               'L1_XE45',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe45_tc_em',                'L1_XE45',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe45_mht',                  'L1_XE45',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe45_pueta_wEFMu',          'L1_XE45',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe45_tc_lcw_wEFMu',         'L1_XE45',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe45_tc_em_wEFMu',          'L1_XE45',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe45_pufit',                'L1_XE45',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe45',                             'L1_XE45',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ]

    TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [
        ['tau35_perf_tracktwo_tau25_perf_tracktwo_ditauL', 'L1_TAU20IM_2TAU12IM' , ['L1_TAU20IM','L1_TAU12IM'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
        ['tau35_perf_tracktwo_tau25_perf_tracktwo_ditauM', 'L1_TAU20IM_2TAU12IM' , ['L1_TAU20IM','L1_TAU12IM'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
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
	
        ['tau160_medium1HighptL_tracktwo',         'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_medium1HighptM_tracktwo',         'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_medium1HighptH_tracktwo',         'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],


        ['tau35_loose1_tracktwo_tau25_loose1_tracktwo_L1TAU20ITAU12I-J25',   'L1_TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_loose1_tracktwo","tau25_loose1_tracktwo"]]], 
        ['tau35_loose1_tracktwo_tau25_loose1_tracktwo_L1DR-TAU20ITAU12I',   'L1_DR-TAU20ITAU12I',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_loose1_tracktwo","tau25_loose1_tracktwo"]]],
        ['tau35_loose1_tracktwo_tau25_loose1_tracktwo_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_loose1_tracktwo","tau25_loose1_tracktwo"]]],
        ['tau60_medium1_tracktwo_tau35_medium1_tracktwo','L1_TAU40_2TAU20IM',['L1_TAU40','L1_TAU20IM'],[PhysicsStream],['RATE:MultiTau', 'BW:Tau'],-1,['serial',-1,["tau60_medium1_tracktwo","tau35_medium1_tracktwo"]]],
        ['tau80_medium1_tracktwo_L1TAU60_tau25_medium1_tracktwo_L1TAU12','L1_TAU60',['L1_TAU60','L1_TAU12'],[PhysicsStream],['RATE:MultiTau', 'BW:Tau'],-1,['serial',-1,["tau80_medium1_tracktwo_L1TAU60","tau25_medium1_tracktwo_L1TAU12"]]],


        # photon+pion chains (ATR-13525) 
        ['tau25_singlepion_tracktwo',                    'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_singlepiontight_tracktwo',                    'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],


        ]

    if TriggerFlags.doFTK():
            TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [
                ['tau0_idperf_FTK',              'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
                ['tau0_idperf_FTKRefit',         'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
                ['tau0_perf_FTK',                'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
                ['tau0_perf_FTKRefit',           'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
                ['tau25_perf_FTK',               'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
                ['tau25_perf_FTKRefit',          'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
                ['tau25_idperf_FTK',             'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
                ['tau25_idperf_FTKRefit',        'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
                ['tau25_medium1_FTK',            'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
                ['tau25_medium1_FTKRefit',       'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
                
                ['tau20_idperf_FTK_tau12_idperf_FTK', 'L1_TAU20IM_2TAU12IM' , ['L1_TAU20IM','L1_TAU12IM'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
                ['tau20_perf_FTK_tau12_perf_FTK', 'L1_TAU20IM_2TAU12IM' , ['L1_TAU20IM','L1_TAU12IM'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
                ['tau20_perf_FTK_tau12_perf_FTK_ditauL', 'L1_TAU20IM_2TAU12IM' , ['L1_TAU20IM','L1_TAU12IM'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
                ['tau20_perf_FTK_tau12_perf_FTK_ditauM', 'L1_TAU20IM_2TAU12IM' , ['L1_TAU20IM','L1_TAU12IM'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],
                ['tau20_perf_FTK_tau12_perf_FTK_ditauT', 'L1_TAU20IM_2TAU12IM' , ['L1_TAU20IM','L1_TAU12IM'],[PhysicsStream, 'express'], ['RATE:MultiTau','BW:Tau'], -1],

                
                ]


    TriggerFlags.EgammaSlice.signatures = TriggerFlags.EgammaSlice.signatures() + [

        ##########        
        # Single electron triggers

        ['e24_medium_iloose_L2StarA_L1EM20VH',   'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e24_medium_L2Star_idperf_L1EM20VH',    'L1_EM20VH',    [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1], 
        
        ['e24_lhmedium_iloose_L2StarA_L1EM20VH', 'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e24_lhmedium_L2Star_idperf_L1EM20VH',  'L1_EM20VH',    [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1], 
        #
        ['e5_loose_L2StarA',                     'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        #['e5_loose1_idperf',                    'L1_EM3',    [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1],
        #['e5_loose1_L2Star_idperf',             'L1_EM3',    [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1],
        
        ['e5_lhloose_L2StarA',                  'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e5_lhloose_L2Star_idperf',            'L1_EM3',       [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1],
        #        
        
        #        

        # technical chains for forward electrons
        #['e17_loose_L1EM15VHJ1523ETA49',       'L1_EM15VH_J15.23ETA49', ['L1_EM15VH','L1_J15.23ETA49'], [PhysicsStream], ['RATE:Combined', 'BW:Egamma'],-1], 
        #['e17_lhloose_L1EM15VHJ1523ETA49',     'L1_EM15VH_J15.23ETA49', ['L1_EM15VH','L1_J15.23ETA49'], [PhysicsStream], ['RATE:Combined', 'BW:Egamma'],-1], 
        ['e17_loose_L1EM15VHJJ1523ETA49',       'L1_EM15VH_JJ15.23ETA49', ['L1_EM15VH','L1_JJ15.23ETA49'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
        ['e17_lhloose_L1EM15VHJJ1523ETA49',     'L1_EM15VH_JJ15.23ETA49', ['L1_EM15VH','L1_JJ15.23ETA49'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
     
        # ATR-12916, Egamma Trigger Menu 2016   

        ['e17_medium_ivarloose_L1EM15HI',        'L1_EM15HI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhmedium_ivarloose_L1EM15HI',      'L1_EM15HI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhmedium_nod0_iloose_L1EM15HI',    'L1_EM15HI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhmedium_nod0_ivarloose_L1EM15HI', 'L1_EM15HI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e24_lhmedium_ivarloose_L1EM18VH',      'L1_EM18VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_ivarloose_L1EM18VH', 'L1_EM18VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e24_lhmedium_ivarloose_L1EM20VH',      'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_L1EM20VH',           'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_ivarloose_L1EM20VH', 'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_ringer_L1EM20VH',    'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e24_lhmedium_ivarloose',             'L1_EM20VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_iloose',           'L1_EM20VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_ivarloose',        'L1_EM20VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_ringer_iloose',    'L1_EM20VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_ringer_ivarloose', 'L1_EM20VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e24_lhtight_ivarloose',             'L1_EM20VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhtight_nod0_ringer_iloose',    'L1_EM20VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhtight_nod0_ringer_ivarloose', 'L1_EM20VHI', [], [PhysicsStream], ['RATE:SingleElectron'  , 'BW:Egamma'],-1],

        ['e26_lhtight',                       'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhtight_ivarloose',             'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhtight_nod0',                  'L1_EM22VHI', [], [PhysicsStream,'express'], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhtight_nod0_ivarloose',        'L1_EM22VHI', [], [PhysicsStream,'express'], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhtight_smooth',                'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhtight_smooth_ivarloose',      'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e28_lhtight',                       'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_iloose',                'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_ivarloose',             'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_nod0',                  'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_nod0_iloose',           'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_nod0_ivarloose',        'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_smooth',                'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_smooth_iloose',         'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_smooth_ivarloose',      'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        # Supporting trigger with PS
        ['e0_L2Star_perf_L1EM15',       'L1_EM15',[], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],         
        
        # Et cut only chains
        ['e250_etcut',                           'L1_EM22VHI',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e300_etcut',                           'L1_EM22VHI',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        # cut-based
        ['e80_medium',                           'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e80_medium_L1EM24VHI',                 'L1_EM24VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],  
        
        # Rerun mode, cut-based

        # Rerun mode, likelihood

        # Single electron/photon chains for Calo sequence optimization (kept as before)
        ['e24_tight_iloose_etisem_L1EM20VH',      'L1_EM20VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        # 2e34 single items
        ['e60_lhmedium_L1EM24VHI',      'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e60_lhmedium_nod0_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e140_loose_L1EM24VHI',        'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e140_lhloose_L1EM24VHI',      'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e140_lhloose_nod0_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g140_loose_L1EM24VHI',        'L1_EM24VHI', [], [PhysicsStream, 'express'], ['RATE:SinglePhoton','BW:Egamma'],-1],

        # 2e34 rerun items
        ['e60_lhloose_L1EM24VHI',       'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e60_lhloose_nod0_L1EM24VHI',  'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e70_lhloose_L1EM24VHI',       'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e70_lhloose_nod0_L1EM24VHI',  'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        # Loose chains with etcut variations
        #['e120_loose1',  'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        
        # Single electron Run1 PID
        #['e24_medium1_iloose_L1EM18VH',  'L1_EM18VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        #['e24_medium1_L1EM18VH',            'L1_EM18VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        #['e24_tight1_iloose_L1EM20VH',      'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        #['e24_tight1_iloose',               'L1_EM20VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        #['e26_tight1_iloose',               'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        #['e60_medium1',                     'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ##########

        # Tri-electron triggers
        # Multielectron chains with m_ee cut for Run2 PID (supporting L1Topo J/psi)             
        
        # Multielectron chains with m_ee cut for Run1 PID
        #['e5_tight1_e4_etcut',         'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        #['e5_tight1_e4_etcut_Jpsiee',  'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],

        ##########
        # Single photon triggers        
        #['g120_loose1',                   'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton','BW:Egamma'],-1], 

        # Supporting triggers with 

        
        ##########        
        #['g35_loose1_g25_loose1',                   'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 

        # Fall-back if problem with new tracking kept as before)
        ['e17_etcut_L1EM15',                      'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_etcut_L2StarA_L1EM15',              'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_etcut_L2StarC_L1EM15',              'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_etcut_L2Star_idperf_L1EM15',        'L1_EM15', [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1],
        ['e17_etcut_idperf_L1EM15',               'L1_EM15', [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1],
        
        # extra id perf chains for HLT ID group (kept as before)
        #['e24_medium1_L2Star_idperf_L1EM18VH',    'L1_EM18VH', [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1],
        #['e24_medium1_idperf_L1EM18VH',           'L1_EM18VH', [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1],
        #['e24_medium1_iloose_L2StarA_L1EM18VH',   'L1_EM18VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        # W T&P with new L1Topo
        ['e5_etcut_L1W-15DPHI-EMXE-0',          'L1_W-15DPHI-EMXE-0',         ['L1_EM3'],  [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e13_etcut_trkcut', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0',  ['L1_EM12'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e13_etcut_trkcut_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE',  ['L1_EM12'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        

        #['g45_tight_iloose',                      'L1_EM20VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],

        ]


    ###########################################################################################################
    #   Bphysics
    ###########################################################################################################
    TriggerFlags.BphysicsSlice.signatures = TriggerFlags.BphysicsSlice.signatures() + [

        #['2mu6_bMuMu_L12MU6', 'L1_2MU6', [], [PhysicsStream], [], -1], 

        ['2mu4_bBmumu_noL2',    'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bBmumu_noL2',   'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        ['2mu4_bDimu_novtx_noos_noL2',    'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bDimu_novtx_noos_noL2', 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bDimu_novtx_noos_noL2',    'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bDimu_novtx_noos_noL2',   'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        ###########  2MU4  ################
        ['2mu4_bBmumuxv3',                                            'L1_2MU4',                         [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bBmumuxv3',                                            'L1_2MU6',                         [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bBmumuxv3',                                            'L1_MU6_2MU4',                  ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_muL2_bBmumuxv3',                                            'L1_2MU4',                         [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],   
        ['2mu6_muL2_bBmumuxv3',                                            'L1_2MU6',                         [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],       
        ['mu6_muL2_mu4_muL2_bBmumuxv3',           'L1_MU6_2MU4',                         ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu10_muL2_mu6_muL2_bBmumuxv3',           'L1_MU10_2MU6',                         ['L1_MU10','L1_MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_iloose_mu6_11invm60_noos', 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        # chains with muon overlap removal
        ['2mu4_wOvlpRm_bDimu',                                 'L1_2MU4',                 [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_wOvlpRm_bDimu_novtx_noos',                      'L1_2MU4',                 [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_wOvlpRm_bBmumuxv2',                             'L1_2MU4',                         [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['3mu6_wOvlpRm_bDimu',                   'L1_3MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_wOvlpRm_bDimu_noinvm_novtx_ss',    'L1_2MU4',     [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],    
        
        # chains with EF B-physics selection skipped
        ['2mu4_bJpsimumu_noEFbph',                    'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bJpsimumu_noEFbph',                 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bDimu_noEFbph',                    'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bJpsimumu_noEFbph',                    'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bDimu_noEFbph',                    'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bJpsimumu_noEFbph',                    'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        ['2mu4_bDimu_tightChi2',                    'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_bJpsimumu_tightChi2',                    'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_bBmumu_tightChi2',                    'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_bUpsimumu_tightChi2',                    'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bDimu_tightChi2',                    'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bJpsimumu_tightChi2',                    'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bBmumu_tightChi2',                    'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bUpsimumu_tightChi2',                    'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bDimu_tightChi2',                    'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bJpsimumu_tightChi2',                    'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bBmumu_tightChi2',                    'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bUpsimumu_tightChi2',                    'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        
        #new narrow-scan muon triggers for tau->3mu        
        #['3mu4_bTau', 'L1_3MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_2mu4_bTau', 'L1_MU6_3MU4', ['L1_MU6','L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],



        ['mu6_nomucomb_2mu4_nomucomb_bTau_L1MU6_3MU4' , 'L1_MU6_3MU4',  ['L1_MU6','L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_nomucomb_mu4_nomucomb_bTau_L12MU6_3MU4', 'L1_2MU6_3MU4', ['L1_2MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        # Asymmetric 3mu chains
        ['mu6_2mu4_bDimu', 'L1_MU6_3MU4', ['L1_MU6','L1_3MU4'], [PhysicsStream], ['RATE:Bphysics', 'BW:Bphys'], -1],
        ['2mu6_mu4_bDimu', 'L1_2MU6_3MU4', ['L1_2MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics', 'BW:Bphys'], -1],



        # Narrow-scan chains seeded L1_MU11_2MU6
        ['mu11_llns_mu6noL1_nscan03_bJpsimumu_noL2_L1MU11_2MU6', 'L1_MU11_2MU6', ['L1_MU11','L2_mu6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,['serial',-1,['mu11_llns','mu6noL1_nscan03']]],
        ['mu11_llns_mu6noL1_nscan03_bDimu_noL2_L1MU11_2MU6', 'L1_MU11_2MU6', ['L1_MU11','L2_mu6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,['serial',-1,['mu11_llns','mu6noL1_nscan03']]],


        ## Moved from Physics due to missing L1Topo item in Physics menu:
        ['2mu6_bDimu_L1BPH-8M15-2MU6_BPH-0DR22-2MU6',     'L1_BPH-8M15-2MU6_BPH-0DR22-2MU6',     ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bDimu_novtx_noos_L1BPH-8M15-2MU6_BPH-0DR22-2MU6',     'L1_BPH-8M15-2MU6_BPH-0DR22-2MU6',     ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bDimu_noL2_L1BPH-8M15-2MU6_BPH-0DR22-2MU6',     'L1_BPH-8M15-2MU6_BPH-0DR22-2MU6',     ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bDimu_novtx_noos_noL2_L1BPH-8M15-2MU6_BPH-0DR22-2MU6',     'L1_BPH-8M15-2MU6_BPH-0DR22-2MU6',     ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bUpsimumu_L1BPH-8M15-2MU6_BPH-0DR22-2MU6', 'L1_BPH-8M15-2MU6_BPH-0DR22-2MU6',     ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
         ['2mu4_bDimu_L1BPH-7M15-2MU4_BPH-0DR24-2MU4',     'L1_BPH-7M15-2MU4_BPH-0DR24-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_L1BPH-7M15-2MU4_BPH-0DR24-2MU4',     'L1_BPH-7M15-2MU4_BPH-0DR24-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_noL2_L1BPH-7M15-2MU4_BPH-0DR24-2MU4',     'L1_BPH-7M15-2MU4_BPH-0DR24-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_noL2_L1BPH-7M15-2MU4_BPH-0DR24-2MU4',     'L1_BPH-7M15-2MU4_BPH-0DR24-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bUpsimumu_L1BPH-7M15-2MU4_BPH-0DR24-2MU4', 'L1_BPH-7M15-2MU4_BPH-0DR24-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
           ['2mu4_bDimu_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-B',     'L1_BPH-7M15-2MU4_BPH-0DR24-2MU4-B',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-B',     'L1_BPH-7M15-2MU4_BPH-0DR24-2MU4-B',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_noL2_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-B',     'L1_BPH-7M15-2MU4_BPH-0DR24-2MU4-B',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_noL2_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-B',     'L1_BPH-7M15-2MU4_BPH-0DR24-2MU4-B',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bUpsimumu_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-B', 'L1_BPH-7M15-2MU4_BPH-0DR24-2MU4-B',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
           ['2mu4_bDimu_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-BO',     'L1_BPH-7M15-2MU4_BPH-0DR24-2MU4-BO',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-BO',     'L1_BPH-7M15-2MU4_BPH-0DR24-2MU4-BO',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_noL2_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-BO',     'L1_BPH-7M15-2MU4_BPH-0DR24-2MU4-BO',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_noL2_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-BO',     'L1_BPH-7M15-2MU4_BPH-0DR24-2MU4-BO',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bUpsimumu_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-BO', 'L1_BPH-7M15-2MU4_BPH-0DR24-2MU4-BO',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
          ['mu6_mu4_bDimu_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4',     'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4',     ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['mu6_mu4_bDimu_novtx_noos_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4',     'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4',     ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['mu6_mu4_bDimu_noL2_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4',     'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4',     ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['mu6_mu4_bDimu_novtx_noos_noL2_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4',     'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4',     ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['mu6_mu4_bUpsimumu_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4', 'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4',     ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        # 78% bckg rejection WP
        ['2mu4_bDimu_L1BPH-2M8-2MU4_BPH-0DR15-2MU4',    'L1_BPH-2M8-2MU4_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_L1BPH-2M8-2MU4_BPH-0DR15-2MU4',    'L1_BPH-2M8-2MU4_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bJpsimumu_L1BPH-2M8-2MU4_BPH-0DR15-2MU4',  'L1_BPH-2M8-2MU4_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_L1BPH-2M8-2MU4_BPH-0DR15-2MU4',     'L1_BPH-2M8-2MU4_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumuxv2_L1BPH-2M8-2MU4_BPH-0DR15-2MU4',  'L1_BPH-2M8-2MU4_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-2M8-2MU4_BPH-0DR15-2MU4',  'L1_BPH-2M8-2MU4_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_noL2_L1BPH-2M8-2MU4_BPH-0DR15-2MU4',    'L1_BPH-2M8-2MU4_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_noL2_L1BPH-2M8-2MU4_BPH-0DR15-2MU4',    'L1_BPH-2M8-2MU4_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bJpsimumu_noL2_L1BPH-2M8-2MU4_BPH-0DR15-2MU4',  'L1_BPH-2M8-2MU4_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_noL2_L1BPH-2M8-2MU4_BPH-0DR15-2MU4',     'L1_BPH-2M8-2MU4_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumuxv2_noL2_L1BPH-2M8-2MU4_BPH-0DR15-2MU4',  'L1_BPH-2M8-2MU4_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-2M8-2MU4_BPH-0DR15-2MU4',  'L1_BPH-2M8-2MU4_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        # 78% bckg rejection WP + OneBarrel
        ['2mu4_bDimu_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4',      'L1_BPH-2M8-2MU4-B_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4',      'L1_BPH-2M8-2MU4-B_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bJpsimumu_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4',  'L1_BPH-2M8-2MU4-B_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4',     'L1_BPH-2M8-2MU4-B_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumuxv2_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4',  'L1_BPH-2M8-2MU4-B_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4',  'L1_BPH-2M8-2MU4-B_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_noL2_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4',      'L1_BPH-2M8-2MU4-B_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_noL2_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4',      'L1_BPH-2M8-2MU4-B_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bJpsimumu_noL2_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4',  'L1_BPH-2M8-2MU4-B_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_noL2_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4',     'L1_BPH-2M8-2MU4-B_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumuxv2_noL2_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4',  'L1_BPH-2M8-2MU4-B_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4',  'L1_BPH-2M8-2MU4-B_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
          
# 78% bckg rejection WP + BarrelOnly
        ['2mu4_bDimu_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',      'L1_BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',      'L1_BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bJpsimumu_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',  'L1_BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',     'L1_BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumuxv2_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',  'L1_BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',  'L1_BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_noL2_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',      'L1_BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_noL2_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',      'L1_BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bJpsimumu_noL2_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',  'L1_BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_noL2_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',     'L1_BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumuxv2_noL2_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',  'L1_BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_noL2_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',  'L1_BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',     ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        
        ]



    TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures() + [

        # all e/g + X triggers are taken from DC14 and L1 thresholds adjusted
        
#        ['g10_loose_mu10_taumass',   'L1_EM7_MU10', ['L1_EM7', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g10_loose","mu10"]] ],
#        ['g20_loose_mu4_taumass',    'L1_EM15_MU4', ['L1_EM15', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g20_loose","mu4"]] ],
#        ['g10_loose_mu10',           'L1_EM7_MU10', ['L1_EM7', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],
#        ['g20_loose_mu4',            'L1_EM15_MU4', ['L1_EM15', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],
#        

        # 2e34 single items
        ['e60_lhloose_L1EM24VHI_xe60noL1',      'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e60_lhloose_L1EM24VHI","xe60noL1"]]],
        ['e60_lhloose_nod0_L1EM24VHI_xe60noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e60_lhloose_nod0_L1EM24VHI","xe60noL1"]]],
        ['e70_lhloose_L1EM24VHI_xe70noL1',      'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e70_lhloose_L1EM24VHI","xe70noL1"]]],
        ['e70_lhloose_nod0_L1EM24VHI_xe70noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e70_lhloose_nod0_L1EM24VHI","xe70noL1"]]],

				# E+jets cut-based
        ['e24_vloose_L1EM18VH_3j20noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_vloose_L1EM18VH","3j20noL1"] ]],
        ['e24_vloose_L1EM18VH_4j20noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_vloose_L1EM18VH","4j20noL1"] ]],
        ['e24_vloose_L1EM18VH_5j15noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_vloose_L1EM18VH","5j15noL1"] ]],
        ['e24_vloose_L1EM18VH_6j15noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_vloose_L1EM18VH","6j15noL1"] ]],
        ['e24_vloose_L1EM20VH_3j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_vloose_L1EM20VH","3j20noL1"] ]],
        ['e24_vloose_L1EM20VH_4j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_vloose_L1EM20VH","4j20noL1"] ]],
        ['e24_vloose_L1EM20VH_5j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_vloose_L1EM20VH","5j15noL1"] ]],
        ['e24_vloose_L1EM20VH_6j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e24_vloose_L1EM20VH","6j15noL1"] ]],
        ['e26_vloose_L1EM20VH_3j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e26_vloose_L1EM20VH","3j20noL1"]]],
        ['e26_vloose_L1EM20VH_4j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e26_vloose_L1EM20VH","4j20noL1"] ]],
        ['e26_vloose_L1EM20VH_5j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e26_vloose_L1EM20VH","5j15noL1"] ]],
        ['e26_vloose_L1EM20VH_6j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1, ['serial',-1,["e26_vloose_L1EM20VH","6j15noL1"] ]],
         
        # Non-L1Topo W T&P trigger for 2016
        ['e13_etcut_trkcut_xs30_j10_perf_xe25_6dphi15_mt35', 'L1_EM12_XS20', ['L1_EM12','L1_XS20','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e13_etcut_trkcut","xs30","j10_perf","xe25"]]],
        ['e13_etcut_trkcut_xs40_j10_perf_xe25_6dphi05_mt35', 'L1_EM12_XS20', ['L1_EM12','L1_XS20','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e13_etcut_trkcut","xs40","j10_perf","xe25"]]],
        ['e13_etcut_trkcut_xs30_j10_perf_xe25_6dphi05_mt35', 'L1_EM12_XS20', ['L1_EM12','L1_XS20','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e13_etcut_trkcut","xs30","j10_perf","xe25"]]],
        ['e18_etcut_trkcut_xs50_j10_perf_xe30_6dphi15_mt35', 'L1_EM15_XS30', ['L1_EM15','L1_XS30','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e18_etcut_trkcut","xs50","j10_perf","xe30"]]],
        ['e18_etcut_trkcut_xs50_j10_perf_xe30_6dphi05_mt35', 'L1_EM15_XS30', ['L1_EM15','L1_XS30','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e18_etcut_trkcut","xs50","j10_perf","xe30"]]],
        ['e18_etcut_trkcut_xs60_j10_perf_xe30_6dphi15_mt35', 'L1_EM15_XS30', ['L1_EM15','L1_XS30','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e18_etcut_trkcut","xs60","j10_perf","xe30"]]],
        ['e18_etcut_trkcut_xs60_j10_perf_xe30_6dphi05_mt35', 'L1_EM15_XS30', ['L1_EM15','L1_XS30','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e18_etcut_trkcut","xs60","j10_perf","xe30"]]],
        ['e13_etcut_trkcut_xs50_xe30_mt35', 'L1_EM12_XS20', ['L1_EM12','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e13_etcut_trkcut","xs50","xe30"]]],
        ['e18_etcut_trkcut_xs60_xe30_mt35', 'L1_EM15_XS30', ['L1_EM15','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e18_etcut_trkcut","xs60","xe30"]]],
        ['e24_etcut_trkcut_xs60_xe30_mt35', 'L1_EM15_XS30', ['L1_EM15','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e24_etcut_trkcut","xs60","xe30"]]],
        ['e26_etcut_trkcut_xs40_xe30_mt35', 'L1_EM15_XS30', ['L1_EM15','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e26_etcut_trkcut","xs40","xe30"]]],
        ['e24_etcut_trkcut_xs40_j10_perf_xe35_2dphi05_mt35', 'L1_EM15_XS30', ['L1_EM15','L1_XS30','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e24_etcut_trkcut","xs40","j10_perf","xe35"]]],
        ['e40_etcut_trkcut_xs40_xe30',                       'L1_EM15_XS30', ['L1_EM15','L1_XS30',''],     [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e40_etcut_trkcut","xs40","xe30"]]],

        # L1Topo W T&P triggers for 2016
        ['e13_etcut_trkcut_xs30_j10_perf_xe25_6dphi15_mt35_L1EM12_W-MT35_XS30_W-15DPHI-JXE-0_W-15DPHI-EMXE', 'L1_EM12_W-MT35_XS30_W-15DPHI-JXE-0_W-15DPHI-EMXE', ['L1_EM12','','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e13_etcut_trkcut","xs30","j10_perf","xe25"]]],
        ['e13_etcut_trkcut_xs40_j10_perf_xe25_6dphi05_mt35_L1EM12_W-MT35_XS40_W-05DPHI-JXE-0_W-05DPHI-EMXE', 'L1_EM12_W-MT35_XS40_W-05DPHI-JXE-0_W-05DPHI-EMXE', ['L1_EM12','','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e13_etcut_trkcut","xs40","j10_perf","xe25"]]],
        ['e13_etcut_trkcut_xs30_j10_perf_xe25_6dphi15_mt35_L1EM12_W-MT35_W-90RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EMXE', 'L1_EM12_W-MT35_W-90RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EMXE', ['L1_EM12','','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e13_etcut_trkcut","xs30","j10_perf","xe25"]]],
        ['e13_etcut_trkcut_xs30_j10_perf_xe25_6dphi05_mt35_L1EM12_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EMXE', 'L1_EM12_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EMXE', ['L1_EM12','','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e13_etcut_trkcut","xs30","j10_perf","xe25"]]],
        ['e18_etcut_trkcut_xs50_j10_perf_xe30_6dphi15_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE', 'L1_EM15_W-MT35_W-250RO2-XEHT-0_W-15DPHI-JXE-0_W-15DPHI-EM15XE', ['L1_EM15','','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e18_etcut_trkcut","xs50","j10_perf","xe30"]]],
        ['e18_etcut_trkcut_xs50_j10_perf_xe30_6dphi05_mt35_L1EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE', 'L1_EM15_W-MT35_W-250RO2-XEHT-0_W-05DPHI-JXE-0_W-05DPHI-EM15XE', ['L1_EM15','','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e18_etcut_trkcut","xs50","j10_perf","xe30"]]],
        ['e18_etcut_trkcut_xs60_j10_perf_xe30_6dphi15_mt35_L1EM15_W-MT35_XS60_W-15DPHI-JXE-0_W-15DPHI-EM15XE', 'L1_EM15_W-MT35_XS60_W-15DPHI-JXE-0_W-15DPHI-EM15XE', ['L1_EM15','','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e18_etcut_trkcut","xs60","j10_perf","xe30"]]],
        ['e18_etcut_trkcut_xs60_j10_perf_xe30_6dphi05_mt35_L1EM15_W-MT35_XS60_W-05DPHI-JXE-0_W-05DPHI-EM15XE', 'L1_EM15_W-MT35_XS60_W-05DPHI-JXE-0_W-05DPHI-EM15XE', ['L1_EM15','','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e18_etcut_trkcut","xs60","j10_perf","xe30"]]],
        ['e18_etcut_trkcut_xs50_j10_perf_xe30_6dphi15_mt35_L1EM15_W-MT35_XS40_W-15DPHI-JXE-0_W-15DPHI-EM15XE', 'L1_EM15_W-MT35_XS40_W-15DPHI-JXE-0_W-15DPHI-EM15XE', ['L1_EM15','','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e18_etcut_trkcut","xs50","j10_perf","xe30"]]],
        ['e18_etcut_trkcut_xs50_j10_perf_xe30_6dphi05_mt35_L1EM15_W-MT35_XS40_W-05DPHI-JXE-0_W-05DPHI-EM15XE', 'L1_EM15_W-MT35_XS40_W-05DPHI-JXE-0_W-05DPHI-EM15XE', ['L1_EM15','','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e18_etcut_trkcut","xs50","j10_perf","xe30"]]],
        ['e13_etcut_trkcut_xs50_xe30_mt35_L1EM12_W-MT35_XS50', 'L1_EM12_W-MT35_XS50', ['L1_EM12','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e13_etcut_trkcut","xs50","xe30"]]],
        ['e18_etcut_trkcut_xs60_xe30_mt35_L1EM15_W-MT35_XS60', 'L1_EM15_W-MT35_XS60', ['L1_EM15','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e18_etcut_trkcut","xs60","xe30"]]],
        ['e13_etcut_trkcut_xs50_xe30_mt35_L1EM10VH_W-MT35_XS50', 'L1_EM10VH_W-MT35_XS50', ['L1_EM10VH','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e13_etcut_trkcut","xs50","xe30"]]],
        ['e18_etcut_trkcut_xs60_xe30_mt35_L1EM15VH_W-MT35_XS60', 'L1_EM15VH_W-MT35_XS60', ['L1_EM15VH','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e18_etcut_trkcut","xs60","xe30"]]],
        ['e24_etcut_trkcut_xs60_xe30_mt35_L1EM20VH_W-MT35_XS60', 'L1_EM20VH_W-MT35_XS60', ['L1_EM20VH','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e24_etcut_trkcut","xs60","xe30"]]],
        ['e26_etcut_trkcut_xs40_xe30_mt35_L1EM22VHI_W-MT35_XS40', 'L1_EM22VHI_W-MT35_XS40', ['L1_EM22VHI','',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e26_etcut_trkcut","xs40","xe30"]]],


        ['e13_etcut_trkcut_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20_xe20_mt25', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', ['L1_EM12',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xe20"]]],
        ['e13_etcut_trkcut_xe20_mt25', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', ['L1_EM12',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xe20"]]],    
        ['e13_etcut_trkcut_xs15_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', ['L1_EM12',''], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1,['serial',-1,["e13_etcut_trkcut","xs15"]]], # commenting out until megrging is fixed centrally.
        ['e13_etcut_trkcut_xe20_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', ['L1_EM12',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","xe20"]]],
        ['e13_etcut_trkcut_xe20', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', ['L1_EM12',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","xe20"]]],
        ['e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS20', ['L1_EM12','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","j20_perf","xe15"]]],
        ['e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25_L1EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', 'L1_EM12_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', ['L1_EM12','',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","j20_perf","xe15"]]],
        
        # photon+tri-jet trigger
        #['g45_tight_2j50noL1',           'L1_EM20VH',  ['L1_EM20VH',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:JET'], -1,['serial',-1,["g45_tight","2j50noL1"]]],
        #['g75_tight_2j25noL1',           'L1_EM20VH',  ['L1_EM20VH',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:JET'], -1,['serial',-1,["g75_tight","2j25noL1"]]],

        ['g45_tight_3j50noL1', 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJET', 'BW:Egamma', 'BW:JET'], -1,['serial',-1,["g45_tight","3j50noL1"]]],
        ['g75_tight_3j50noL1', 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJET', 'BW:Egamma', 'BW:JET'], -1,['serial',-1,["g75_tight","3j50noL1"]]],
        ['g75_tight_3j25noL1_L1EM20VHI', 'L1_EM20VHI', ['L1_EM20VHI',''], [PhysicsStream], ['RATE:EgammaJET', 'BW:Egamma', 'BW:JET'], -1,['serial',-1,["g75_tight","3j25noL1_L1EM20VHI"]]],
        ['g75_tight_3j25noL1_L1EM22VHI', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaJET', 'BW:Egamma', 'BW:JET'], -1,['serial',-1,["g75_tight","3j25noL1_L1EM22VHI"]]],
        ['g75_tight_3j50noL1_L1EM22VHI', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:JET'], -1,['serial',-1,["g75_tight","3j50noL1_L1EM22VHI"]]],

#        ['g45_tight_iloose_3j50noL1_L1EM20VHI', 'L1_EM20VHI', ['L1_EM20VHI',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:JET'], -1,['serial',-1,["g45_tight_iloose","3j50noL1_L1EM20VHI"]]],
#        ['g45_tight_iloose_3j50noL1_L1EM22VHI', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:JET'], -1,['serial',-1,["g45_tight_iloose","3j50noL1_L1EM22VHI"]]],
#
#        ['g75_tight_iloose_3j25noL1_L1EM20VHI', 'L1_EM20VHI', ['L1_EM20VHI',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:JET'], -1,['serial',-1,["g75_tight_iloose","3j25noL1_L1EM20VHI"]]],
#        ['g75_tight_iloose_3j25noL1_L1EM22VHI', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:JET'], -1,['serial',-1,["g75_tight_iloose","3j25noL1_L1EM22VHI"]]],
#



        ['e15_lhtight_iloose_3j20_L1EM13VH_3J20','L1_EM13VH_3J20',  ['L1_EM13VH_3J20',''],  [PhysicsStream], ['RATE:EgammaJET', 'BW:Egamma', 'BW:JET'], -1,['serial',-1,["e15_lhtight_iloose","3j20"]]],

##da levare
  #      ['j60_boffperf_split_j60_L12J40_XE45','L1_2J40_XE45',  [],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1,['serial',-1,["j60_boffperf_split","j60_L12J40_XE45"]]],

        ['j80_boffperf_split_L12J50_XE40','L1_2J50_XE40',  ['L1_2J50_XE40',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],
        ['j80_boffperf_split_xe70_L1J400ETA25_XE50','L1_J40.0ETA25_XE50',  ['L1_J40.0ETA25_XE50',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1,['serial',-1,["j80_boffperf_split","xe70_L1J400ETA25_XE50"]]],
        ['j100_boffperf_split_L1XE60','L1_XE60',  ['L1_XE60',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],

      ['j80_bmv2c2060_split_L12J50_XE40','L1_2J50_XE40',  ['L1_2J50','XE40'],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],

     ['j80_bmv2c2060_split_xe60_L12J50_XE40','L1_2J50_XE40',  [],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1,['serial',-1,["j80_bmv2c2060_split","xe60"]]]           ,

      ['j80_bmv2c2077_split_xe70_L1J400ETA25_XE50','L1_J40.0ETA25_XE50',  ['L1_J40.0ETA25','XE50'],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1,['serial',-1,["j80_bmv2c2077_split","xe70_L1J400ETA25_XE50"]]],
      ['j100_bmv2c2070_split_L1XE60','L1_XE60',  ['L1_2J50',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],



#        ['j80_boffperf_split_L1J40_XE50','L1_J40_XE50',  ['L1_J40_XE50',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],

#        ['j80_boffperf_split_L1J400ETA25_XE50','L1_J40.0ETA25_XE50',  ['L1_J40.0ETA25_XE50',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],

#        ['j80_boffperf_split_L1XE60','L1_XE60',  ['L1_XE60',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],
#        ['2j30_boffperf_split_L12J15_XE55','L1_2J15_XE55',  ['L1_2J15','XE55'],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],
# 

        # 
        #  B-jet + hT
        #
        ['2j55_bmv2c2060_split_ht300_L14J20',              'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:Bjet', 'BW:Jet'],-1,['serial',-1,['2j55_bmv2c2060_split', 'ht300_L14J20']]],

        ['2j55_bperf_ht300_L14J20',  'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:Bjet', 'BW:Jet'],-1,['serial',-1,['2j55_bperf', 'ht300_L14J20']]],
        ['2j55_bperf_split_ht300_L14J20',  'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:Bjet', 'BW:Jet'],-1,['serial',-1,['2j55_bperf_split', 'ht300_L14J20']]],
        ['2j55_boffperf_ht300_L14J20',  'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:Bjet', 'BW:Jet'],-1,['serial',-1,['2j55_boffperf', 'ht300_L14J20']]],
        ['2j55_boffperf_split_ht300_L14J20',  'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:Bjet', 'BW:Jet'],-1,['serial',-1,['2j55_boffperf_split', 'ht300_L14J20']]],

        ['j55_bperf_ht500_L14J20',  'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:Bjet', 'BW:Jet'],-1,['serial',-1,['j55_bperf', 'ht500_L14J20']]],
        ['j55_bperf_split_ht500_L14J20',  'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:Bjet', 'BW:Jet'],-1,['serial',-1,['j55_bperf_split', 'ht500_L14J20']]],
        ['j55_boffperf_ht500_L14J20',  'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:Bjet', 'BW:Jet'],-1,['serial',-1,['j55_boffperf', 'ht500_L14J20']]],
        ['j55_boffperf_split_ht500_L14J20',  'L1_4J20',   ['L1_4J20', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:Bjet', 'BW:Jet'],-1,['serial',-1,['j55_boffperf_split', 'ht500_L14J20']]],

        
        

       ### NOT IN THE LIST ['e5_tight1_g4_etcut',                       'L1_2EM3', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1, ['parallel',-1,[] ]],
        ### NOT IN THE LIST ['e24_medium1_L1EM20VHI_g8_etcut',      'L1_EM20VHI', ['L1_EM20VHI','L1_EM3'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1, ['parallel',-1,[] ]],
        ### NOT IN THE LIST ['e17_loose1_2g8_etcut_L12EM7',         'L1_EM15VH_3EM7',['L1_EM15VH','L1_2EM7'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ### NOT IN THE LIST ['e24_medium1_L1EM15VH_g25_medium1',    'L1_2EM15VH',[], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ### NOT IN THE LIST ['e20_medium1_g35_loose1',              'L1_2EM15VH',[], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ### NOT IN THE LIST ['e20_medium1_2g20_loose1',             'L1_2EM15VH',[], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        
        #MET+X -- with e/g only
        #['e24_medium1_L1EM20VHI_xe100noL1',  'L1_EM20VHI',['L1_EM20VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1,['serial',-1,["e24_medium1_L1EM20VHI","xe100noL1"]]],

        #['mu24_xe100noL1_wEFMu',     'L1_MU20',['L1_MU20',''], [PhysicsStream], ['RATE:METMuon', 'BW:Muon'], -1,['serial',-1,["mu24","xe100noL1_wEFMu"]]],
        #['j80_xe80_dphi1',         'L1_J40_XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j80","xe80"]]],

        # jet+xe alternative triggers
        ['j80_xe100',               'L1_J40_XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets', 'BW:MET'], -1,['serial',-1,["j80","xe100"]]],        
        ['j120_xe80',               'L1_J75_XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets', 'BW:MET'], -1,['serial',-1,["j120","xe80"]]],        
        ['j150_xe80',               'L1_J75_XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets', 'BW:MET'], -1,['serial',-1,["j150","xe80"]]],        
        ['j150_xe80_tc_lcw',        'L1_J75_XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets', 'BW:MET'], -1,['serial',-1,["j150","xe80_tc_lcw"]]],        

        # MET+jet chains with cleaning
        ['j40_cleanT_xe80_L1XE60',    'L1_XE60', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,["j40_cleanT","xe80_L1XE60"]]],
        ['j40_cleanT_xe75_L1XE60',    'L1_XE60', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,["j40_cleanT","xe75_L1XE60"]]],
        ['j30_0eta490_cleanT_xe80_L1XE60',    'L1_XE60', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,["j30_0eta490_cleanT","xe80_L1XE60"]]],
        ['j30_0eta490_cleanT_xe100_tc_lcw_L1XE70', 'L1_XE70', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,["j30_0eta490_cleanT","xe100_tc_lcw_L1XE70"]]],
        ['j60_cleanT_xe80_L1J40_XE50',                    'L1_J40_XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,["j60_cleanT","xe80"]]],
        ['j60_cleanT_xe75_L1J40_XE50',                    'L1_J40_XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,["j60_cleanT","xe75"]]],


        # ATR-11975
        ['j65_0eta490_j40_0eta490_invm250_xe80_L1XE70', 'L1_XE70', ['','',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,["j65_0eta490","j40_0eta490_invm250","xe80_L1XE70"]]],

  

        # TAU+X
        # Ditau Items
      
        #['tau35_medium1_tracktwo_xe70_L1XE45_TAU20-J20', 'L1_XE45_TAU20-J20',['L1_TAU20', 'L1_XE45'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","xe70_L1XE45"]]],
        # tau + met for testing (ATR-10785)
        ['tau35_medium1_tracktwo_L1TAU20_xe70_L1XE45_L1XE45_TAU20-J20', 'L1_XE45_TAU20-J20',['L1_TAU20', 'L1_XE45'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo_L1TAU20","xe70_L1XE45"]]],

  # High-pT DiTau seeding off of single-tau

        # Tau + Electron
        # Low-threshold with jets
         #e+tau chains seeded by L1EM15TAU12I, needed as supporting triggers for the primary chains with either L1 jet or L1topo
 
        ['e17_medium_tau25_medium1_tracktwo_L1EM15HI_2TAU12IM',  'L1_EM15HI_2TAU12IM',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_medium","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_tau25_medium1_tracktwo_L1EM15HI_2TAU12IM',  'L1_EM15HI_2TAU12IM',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_nod0_tau25_medium1_tracktwo_L1EM15HI_2TAU12IM',  'L1_EM15HI_2TAU12IM',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_L1EM15HI_2TAU12IM',  'L1_EM15HI_2TAU12IM',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_nod0_iloose","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1EM15HI_2TAU12IM',  'L1_EM15HI_2TAU12IM',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwo"]]],
        
        #e+tau chains seeded by L1DR-EM15TAU12I which could be the primary e+tau chain with L1topo
        ### cannot do now until l1 item is defined
        ['e17_medium_tau25_medium1_tracktwo_L1DR-EM15TAU12I',  'L1_DR-EM15TAU12I',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_medium","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_tau25_medium1_tracktwo_L1DR-EM15TAU12I',  'L1_DR-EM15TAU12I',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_nod0_tau25_medium1_tracktwo_L1DR-EM15TAU12I',  'L1_DR-EM15TAU12I',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_nod0_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I',  'L1_DR-EM15TAU12I',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_nod0_iloose","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I',  'L1_DR-EM15TAU12I',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwo"]]],
 
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1EM15TAU12I-J25', 'L1_EM15TAU12I-J25',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwo"]]],
        
        # Isolated Variant

        # Those are somehow not working
        ['e17_medium_iloose_tau25_perf_ptonly_L1EM15HI_2TAU12IM',   'L1_EM15HI_2TAU12IM',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_medium_iloose","tau25_perf_ptonly"]]],
        ['e17_lhmedium_iloose_tau25_perf_ptonly_L1EM15HI_2TAU12IM', 'L1_EM15HI_2TAU12IM',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_iloose","tau25_perf_ptonly"]]],

        ['e17_medium_ivarloose_tau25_perf_ptonly_L1EM15HI_2TAU12IM',   'L1_EM15HI_2TAU12IM',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_medium_ivarloose","tau25_perf_ptonly"]]],
        ['e17_lhmedium_ivarloose_tau25_perf_ptonly_L1EM15HI_2TAU12IM', 'L1_EM15HI_2TAU12IM',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_ivarloose","tau25_perf_ptonly"]]],

        # Tau + Muon
        #mu+tau chains seeded by L1MU10TAU20IM, needed as supporting triggers for the primary chains with either L1 jet or L1topo
        ['mu14_tau25_medium1_tracktwo_L1MU10_TAU12IM',  'L1_MU10_TAU12IM',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14","tau25_medium1_tracktwo"]]],
        ['mu14_iloose_tau25_medium1_tracktwo_L1MU10_TAU12IM',  'L1_MU10_TAU12IM',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_iloose","tau25_medium1_tracktwo"]]],
        ['mu14_ivarloose_tau25_medium1_tracktwo_L1MU10_TAU12IM',  'L1_MU10_TAU12IM',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwo"]]],
   
        # ATR-13366

        ['mu14_ivarloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25', 'L1_DR-MU10TAU12I_TAU12I-J25',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwo"]]],
        ['mu14_ivarloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I', 'L1_DR-MU10TAU12I',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwo"]]],
        ['mu14_ivarloose_tau25_medium1_tracktwo_L1MU10_TAU12I-J25', 'L1_MU10_TAU12I-J25',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwo"]]],  
        ['mu14_ivarloose_tau35_medium1_tracktwo',  'L1_MU10_TAU20IM',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_medium1_tracktwo"]]],
        ['mu14_ivarloose_tau25_medium1_tracktwo_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose", "tau25_medium1_tracktwo","xe50"]]],	
	
        # Also somehow not working...
        ['mu14_iloose_tau25_perf_ptonly_L1MU10_TAU12IM',      'L1_MU10_TAU12IM',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_iloose","tau25_perf_ptonly"]]],        
        ['mu14_ivarloose_tau25_perf_ptonly_L1MU10_TAU12IM',      'L1_MU10_TAU12IM',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_perf_ptonly"]]],        

        #tau + photon
        ['g35_medium_tau25_perf_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau25_perf_tracktwo_L1TAU12"]]],
        # photon+pion (ATR-13525)
        ['g35_medium_tau25_singlepion_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau25_singlepion_tracktwo_L1TAU12"]]],
        ['g35_medium_tau25_singlepiontight_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau25_singlepiontight_tracktwo_L1TAU12"]]],

        # muon plus multijets
        ['mu14_iloose_3j20_L1MU10_3J20', 'L1_MU10_3J20', ['L1_MU10_3J20', ''], [PhysicsStream], ['RATE:Muonjet', 'BW:Muon','BW:Jet'], -1,['serial',-1,['mu14_iloose', '3j20'] ],True],
        ['mu14_ivarloose_3j20_L1MU10_3J20', 'L1_MU10_3J20', ['L1_MU10_3J20', ''], [PhysicsStream], ['RATE:Muonjet', 'BW:Muon','BW:Jet'], -1,['serial',-1,['mu14_ivarloose', '3j20'] ],True],

        # 4-jet items (VBF H->bb, hh->bbbb)

        ['mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu4', '3j35', 'j35_bloose_split_antimatchdr05mu'] ]],


        ['mu4_3j35_dr05_j35_boffperf_antimatchdr05mu_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu4', '3j35', 'j35_boffperf_antimatchdr05mu'] ]],
        ['mu4_3j35_dr05_j35_boffperf_split_antimatchdr05mu_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu4', '3j35', 'j35_boffperf_split_antimatchdr05mu'] ]],

        ['mu4_3j35_dr05_j35_boffperf_split_antimatchdr05mu_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j35', 'j35_boffperf_split_antimatchdr05mu'] ], False],
        ['mu4_3j30_dr05_j30_boffperf_split_antimatchdr05mu_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j30', 'j30_boffperf_split_antimatchdr05mu'] ], False],        
        
        #['mu4_4j40_dr05_L14J20', 'L1_4J20', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu4', '4j40'] ]],
        # ['mu4_3j35_dr05_j35_bperfoff_antimatchdr05mu_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu4', '3j35', 'j35_bperfoff_antimatchdr05mu'] ]],
        # ['mu4_3j35_dr05_j35_bperfoff_split_antimatchdr05mu_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu4', '3j35', 'j35_bperfoff_split_antimatchdr05mu'] ]],
        # ['mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j35', 'j35_bloose_split_antimatchdr05mu'] ]],##### these should be bperfoff
        # ['mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j30', 'j30_bmedium_split_antimatchdr05mu'] ]], #### these should be bperfoff


        ['mu4_4j40_dr05_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '4j40'] ],True],
        ['mu4_3j35_dr05_j35_boffperf_split_antimatchdr05mu_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j35', 'j35_boffperf_split_antimatchdr05mu'] ], True],
#        ['mu4_3j30_dr05_j30_boffperf_split_antimatchdr05mu_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j30', 'j30_boffperf_split_antimatchdr05mu'] , True]],
        ['mu4_4j40_dr05_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '4j40'] ], True],
        ['mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j35', 'j35_bloose_split_antimatchdr05mu'] ], True],
        ['mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j30', 'j30_bmedium_split_antimatchdr05mu'] ], True],


#        ['mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu4', '3j35', 'j35_bloose_split_antimatchdr05mu'] ]],
        ['mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j30', 'j30_bmedium_split_antimatchdr05mu'] ]],
        ['mu4_3j30_dr05_j30_boffperf_split_antimatchdr05mu_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j30', 'j30_boffperf_split_antimatchdr05mu'] ]],

        ['mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j35', 'j35_bloose_split_antimatchdr05mu'] ], False],
        ['mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j30', 'j30_bmedium_split_antimatchdr05mu'] , False]],


        ['mu4_3j35_dr05_j35_boffperf_split_antimatchdr05mu_L1MU4_3J15', 'L1_MU4_3J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j35', 'j35_boffperf_split_antimatchdr05mu'] ]],
        ['mu4_3j30_dr05_j30_boffperf_split_antimatchdr05mu_L1MU4_3J15', 'L1_MU4_3J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j30', 'j30_boffperf_split_antimatchdr05mu'] ]],
        ['mu4_3j35_dr05_j35_boffperf_split_antimatchdr05mu_L1MU4_3J20', 'L1_MU4_3J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j35', 'j35_boffperf_split_antimatchdr05mu'] ]],
        ['mu4_3j30_dr05_j30_boffperf_split_antimatchdr05mu_L1MU4_3J20', 'L1_MU4_3J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j30', 'j30_boffperf_split_antimatchdr05mu'] ]],
        ['mu4_2j40_dr05_j40_boffperf_split_antimatchdr05mu_L1MU4_3J15', 'L1_MU4_3J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j40', 'j40_boffperf_split_antimatchdr05mu'] ]],
        ['mu4_2j35_dr05_j35_boffperf_split_antimatchdr05mu_L1MU4_3J15', 'L1_MU4_3J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j35', 'j35_boffperf_split_antimatchdr05mu'] ]],
        ['mu4_2j40_dr05_j40_boffperf_split_antimatchdr05mu_L1MU4_3J20', 'L1_MU4_3J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j40', 'j40_boffperf_split_antimatchdr05mu'] ]],
        ['mu4_2j35_dr05_j35_boffperf_split_antimatchdr05mu_L1MU4_3J20', 'L1_MU4_3J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j35', 'j35_boffperf_split_antimatchdr05mu'] ]],
        

        
        # VBF triggers
        ['g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490_invm700', 'L1_EM18VH', ['L1_EM18VH','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1,['serial',-1,["g20_loose_L1EM18VH","2j40_0eta490","3j25_0eta490_invm700"]]],
        ['g25_loose_2j40_0eta490_3j25_0eta490_invm700', 'L1_EM22VHI', ['L1_EM22VHI','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1,['serial',-1,["g25_loose","2j40_0eta490","3j25_0eta490_invm700"]]],
        ['g25_loose_L1EM20VH_2j40_0eta490_3j25_0eta490_invm700', 'L1_EM20VH', ['L1_EM20VH','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jets'], -1,['serial',-1,["g25_loose_L1EM20VH","2j40_0eta490","3j25_0eta490_invm700"]]],
####	
        ['g25_medium_L1EM22VHI_j35_0eta490_boffperf_split_3j35_0eta490_invm700', 'L1_EM22VHI', ['L1_EM22VHI', '', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Bjet','BW:Egamma'],-1,['serial',-1,['g25_medium_L1EM22VHI', 'j35_0eta490_boffperf_split','3j35_0eta490_invm700']]],
        ['g25_medium_L1EM22VHI_2j35_0eta490_boffperf_split_2j35_0eta490', 'L1_EM22VHI', ['L1_EM22VHI', '', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Bjet','BW:Egamma'],-1,['serial',-1,['g25_medium_L1EM22VHI', '2j35_0eta490_boffperf_split','2j35_0eta490']]],

        ['j60_bmv2c2077_split_j60_L12J40_XE45','L1_2J40_XE45',  [],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1,['serial',-1,["j60_bmv2c2077_split","j60_L12J40_XE45"]]],
        ['j80_bmv2c2077_split_L12J50_XE40','L1_2J50_XE40',  ['L1_2J50_XE40',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],
        ['j80_bmv2c2077_split_L1J40_XE60','L1_J40_XE60',  ['L1_J40_XE60',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],
        ['j80_bmv2c2077_split_L1J40_XE50','L1_J40_XE50',  ['L1_J40_XE50',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],
 
        ['j80_bmv2c2077_split_L1J400ETA25_XE50','L1_J40.0ETA25_XE50',  ['L1_J40.0ETA25_XE50',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],
 
        ['j80_bmv2c2077_split_L1XE60','L1_XE60',  ['L1_XE60',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],
        ['2j30_bmv2c2085_split_L12J15_XE55','L1_2J15_XE55',  ['L1_2J15','XE55'],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],
 
        ['j60_boffperf_split_j60_L12J40_XE45','L1_2J40_XE45',  [],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1,['serial',-1,["j60_boffperf_split","j60_L12J40_XE45"]]],
#        ['j80_boffperf_split_L12J50_XE40','L1_2J50_XE40',  ['L1_2J50_XE40',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],
        ['j80_boffperf_split_L1J40_XE60','L1_J40_XE60',  ['L1_J40_XE60',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],
        ['j80_boffperf_split_L1J40_XE50','L1_J40_XE50',  ['L1_J40_XE50',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],
 
        ['j80_boffperf_split_L1J400ETA25_XE50','L1_J40.0ETA25_XE50',  ['L1_J40.0ETA25_XE50',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],
 
        ['j80_boffperf_split_L1XE60','L1_XE60',  ['L1_XE60',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],
        ['2j30_boffperf_split_L12J15_XE55','L1_2J15_XE55',  ['L1_2J15','XE55'],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],
 
        ['j60_boffperf_j60_L12J40_XE45','L1_2J40_XE45',  [],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1,['serial',-1,["j60_boffperf","j60_L12J40_XE45"]]],
        ['j80_boffperf_L12J50_XE40','L1_2J50_XE40',  ['L1_2J50_XE40',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],
        ['j80_boffperf_L1J40_XE60','L1_J40_XE60',  ['L1_J40_XE60',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],
        ['j80_boffperf_L1J40_XE50','L1_J40_XE50',  ['L1_J40_XE50',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],
 
        ['j80_boffperf_L1J400ETA25_XE50','L1_J40.0ETA25_XE50',  ['L1_J40.0ETA25_XE50',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],
 
        ['j80_boffperf_L1XE60','L1_XE60',  ['L1_XE60',''],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],
        ['2j30_boffperf_L12J15_XE55','L1_2J15_XE55',  ['L1_2J15','XE55'],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:JET'], -1],

        ['2j55_boffperf_split_ht300_L1J100',  'L1_J100',   ['L1_J100', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:Bjet', 'BW:Jet'],-1,['serial',-1,['2j55_boffperf_split', 'ht300_L1J100']]],
        ['j55_boffperf_split_ht500_L1J100',  'L1_J100',   ['L1_J100', ''],   [PhysicsStream], ['RATE:MultiBJet',  'BW:Bjet', 'BW:Jet'],-1,['serial',-1,['j55_boffperf_split', 'ht500_L1J100']]],

        
        #Backup
        ['mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L1MU4_3J15', 'L1_MU4_3J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j35', 'j35_bloose_split_antimatchdr05mu'] ]],
        ['mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L1MU4_3J15', 'L1_MU4_3J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j30', 'j30_bmedium_split_antimatchdr05mu'] ]],
        ['mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L1MU4_3J20', 'L1_MU4_3J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j35', 'j35_bloose_split_antimatchdr05mu'] ]],
        ['mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L1MU4_3J20', 'L1_MU4_3J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j30', 'j30_bmedium_split_antimatchdr05mu'] ]],
        
        ['mu6_ivarloose_2j40_0eta490_invm1000_L1MU6_J30.0ETA49_2J20.0ETA49', 'L1_MU6_J30.0ETA49_2J20.0ETA49', ['L1_MU6',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jets'], -1, ['serial',-1,["mu6_ivarloose","2j40_0eta490_invm1000"]]],

        
        # 3-jet items (bA->bbb)
        ['mu4_3j45_dr05_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j45'] ], False],
        ['mu4_2j40_dr05_j40_bloose_split_antimatchdr05mu_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j40', 'j40_bloose_split_antimatchdr05mu'] ], False],
        ['mu4_2j35_dr05_j35_bmedium_split_antimatchdr05mu_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j35', 'j35_bmedium_split_antimatchdr05mu'] ], False],
        ['mu4_3j45_dr05_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j45'] ], False],
        ['mu4_2j40_dr05_j40_bloose_split_antimatchdr05mu_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j40', 'j40_bloose_split_antimatchdr05mu'] ], False],
        ['mu4_2j35_dr05_j35_bmedium_split_antimatchdr05mu_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j35', 'j35_bmedium_split_antimatchdr05mu'] ], False],

         # 3-jet items (bA->bbb) backup
        ['mu4_2j40_dr05_j40_bloose_split_antimatchdr05mu_L1MU4_3J15', 'L1_MU4_3J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j40', 'j40_bloose_split_antimatchdr05mu'] ]],
        ['mu4_2j35_dr05_j35_bmedium_split_antimatchdr05mu_L1MU4_3J15', 'L1_MU4_3J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j35', 'j35_bmedium_split_antimatchdr05mu'] ]],
        ['mu4_2j40_dr05_j40_bloose_split_antimatchdr05mu_L1MU4_3J20', 'L1_MU4_3J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j40', 'j40_bloose_split_antimatchdr05mu'] ]],
        ['mu4_2j35_dr05_j35_bmedium_split_antimatchdr05mu_L1MU4_3J20', 'L1_MU4_3J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '2j35', 'j35_bmedium_split_antimatchdr05mu'] ]],

        ['j40_0eta490_j30_0eta490_deta25_xe80',    'L1_XE60', ['','',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,["j40_0eta490","j30_0eta490_deta25","xe80"]]],
        ['j40_0eta490_j30_0eta490_deta25_xe80_tc_lcw',     'L1_XE60', ['','',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,["j40_0eta490","j30_0eta490_deta25","xe80_tc_lcw"]]],
        ['j65_0eta490_j40_0eta490_invm250_xe80',   'L1_XE60', ['','',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,["j65_0eta490","j40_0eta490_invm250","xe80"]]],

        ['2j40_0eta490_invm250_xe80',    'L1_XE60', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,["2j40_0eta490_invm250","xe80"]]],

        ['j50_0eta490_j30_0eta490_deta30_xe80_L1J40_XE50',       'L1_J40_XE50',['','',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j50_0eta490","j30_0eta490_deta30","xe80"]]],
        ['j50_0eta490_j30_0eta490_deta30_xe80_L1J30.0ETA49_XE50',       'L1_J30.0ETA49_XE50',['','',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j50_0eta490","j30_0eta490_deta30","xe80"]]],

        ['j40_0eta490_j30_0eta490_deta25_xe60',       'L1_XE60',['','',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j40_0eta490","j30_0eta490_deta25","xe60"]]],


        # testing jet chains from new topo item
        ['j40_0eta490_xe80_1dphi10_L1DPHI-AJ20s2XE50', 'L1_DPHI-AJ20s2XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j40_0eta490","xe80" ]]],
        ['2j30_0eta490_deta25_xe80_L1DPHI-AJ20s2XE50', 'L1_DPHI-AJ20s2XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["2j30_0eta490_deta25","xe80" ]]],
        ['2j30_0eta490_deta35_xe80_tc_lcw_L1DPHI-AJ20s2XE50', 'L1_DPHI-AJ20s2XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["2j30_0eta490_deta35","xe80_tc_lcw" ]]],

        ['2j30_0eta490_deta35_xe60_L1DPHI-AJ20s2XE50', 'L1_DPHI-AJ20s2XE50', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["2j30_0eta490_deta35","xe60" ]]],
        

        ]

    TriggerFlags.MinBiasSlice.signatures = TriggerFlags.MinBiasSlice.signatures() + [
        #HMT
        ['mb_sp2000_trk70_hmt', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],        
        ['mb_sp2000_pusup600_trk70_hmt', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['mb_sp2000_pusup600_trk70_hmt_L1TE60',  'L1_TE60', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['mb_sp2000_pusup600_trk70_hmt_L1TE70',  'L1_TE70', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['mb_sp2000_pusup700_trk70_hmt_L1TE60',  'L1_TE60', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['mb_sp2000_pusup700_trk70_hmt_L1TE70',  'L1_TE70', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['mb_sptrk_vetombts2in_L1RD0_FILLED', 'L1_RD0_FILLED', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	#ATR-9419
        ['mb_sp2000_pusup700_trk60_hmt_L1TE60', 'L1_TE60', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp2500_pusup750_trk90_hmt_L1TE70', 'L1_TE70', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        #['mb_sp3000_pusup800_trk120_hmt_L1TE40', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp2000_pusup700_trk50_sumet70_hmt_L1TE60',   'L1_TE60', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp2000_pusup700_trk50_sumet110_hmt_L1TE70',  'L1_TE70', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        #['mb_sp2000_pusup700_trk50_sumet150_hmt_L1TE40',  'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

	#supporting HMT triggers
	['mb_sp1800_hmtperf_L1TE40', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

        ]

    #Beamspot chanis first try ATR-9847                                                                                                                
    TriggerFlags.BeamspotSlice.signatures = TriggerFlags.BeamspotSlice.signatures() + [                                                                 

    ]

    if TriggerFlags.doFTK():
        TriggerFlags.BeamspotSlice.signatures = TriggerFlags.BeamspotSlice.signatures() + [                                                                 
#            ['beamspot_allTE_FTK',           'L1_4J15', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
#            ['beamspot_activeTE_FTK',        'L1_4J15', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
            ['beamspot_idperf_FTK',          'L1_4J15', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
            ['beamspot_trkFS_FTK',           'L1_4J15', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
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
        ['noalg_L14J20.0ETA49',                'L1_4J20.0ETA49',                [], [PhysicsStream], ['RATE:SeededStreamers', "BW:Jets"], -1],

        # Muon streamers
        #['noalg_L1MU10_MU6',      'L1_MU10_MU6',       [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],


        # LowLumiUnprescaled streamers
        ['noalg_L12EM10VH',                        'L1_2EM10VH',                        [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Egamma'], -1],
        ['noalg_L12MU6',                           'L1_2MU6',                           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L13MU4',                           'L1_3MU4',                           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L1EM15VH_MU10',                    'L1_EM15VH_MU10',                    [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Egamma', 'BW:Muon'], -1],
        ['noalg_L1EM15I_MU4',                       'L1_EM15I_MU4',                       [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Egamma', 'BW:Muon'], -1],
        ['noalg_L1EM7_MU10',                       'L1_EM7_MU10',                       [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Egamma', 'BW:Muon'], -1],
        ['noalg_L12EM8VH_MU10',                    'L1_2EM8VH_MU10',                    [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Egamma', 'BW:Muon'], -1],
        ['noalg_L1TAU20IM_2TAU12IM_J25_2J20_3J12', 'L1_TAU20IM_2TAU12IM_J25_2J20_3J12', [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1TAU20_2J20_XE45',                'L1_TAU20_2J20_XE45',                [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1EM15HI_2TAU12IM_XE35',           'L1_EM15HI_2TAU12IM_XE35',           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1MU10_TAU12IM_XE35',              'L1_MU10_TAU12IM_XE35',              [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1TAU20_2TAU12_XE35',              'L1_TAU20_2TAU12_XE35',              [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1TAU20IM_2TAU12IM_XE35',          'L1_TAU20IM_2TAU12IM_XE35',          [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1EM15HI_2TAU12IM',                'L1_EM15HI_2TAU12IM',                [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1EM15HI_2TAU12IM_J25_3J12',       'L1_EM15HI_2TAU12IM_J25_3J12',       [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1EM15HI_TAU40_2TAU15',            'L1_EM15HI_TAU40_2TAU15',            [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1MU10_TAU20',                     'L1_MU10_TAU20',                     [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1MU10_TAU12_J25_2J12',            'L1_MU10_TAU12_J25_2J12',            [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1MU10_TAU12IM_J25_2J12',          'L1_MU10_TAU12IM_J25_2J12',          [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L1MU10_TAU12IM',                   'L1_MU10_TAU12IM',                   [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Tau'], -1],
        ['noalg_L14J15',                           'L1_4J15',                           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Jets'], -1],
        ['noalg_L13J25.0ETA23',                    'L1_3J25.0ETA23',                    [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Jets'], -1],
        ['noalg_L13J40',                           'L1_3J40',                           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Jets'], -1],
        ['noalg_L12J15_XE55',                      'L1_2J15_XE55',                      [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Jets'], -1],
        ['noalg_L1MU6_J40',                        'L1_MU6_J40',                        [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],

        
        ]

    TriggerFlags.MonitorSlice.signatures = TriggerFlags.MonitorSlice.signatures() + [
        ['mistimemonl1bccorr', '', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], -1],
        ['mistimemonl1bccorrnomu', '', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], -1],
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
    'tau1_cosmic_ptonly_L1MU4_EMPTY',
    'tau1_cosmic_track_L1MU4_EMPTY',
    'tau8_cosmic_ptonly',
    'tau8_cosmic_track',
]
ps_calibmon_list=[
    'alfacalib',
    'alfacalib_L1ALFA_A7L1_OD',
    'alfacalib_L1ALFA_A7R1_OD',
    'alfacalib_L1ALFA_ANY',
    'alfacalib_L1ALFA_B7L1_OD',
    'alfacalib_L1ALFA_B7R1_OD',
    'alfacalib_L1ALFA_ELAS',
    'alfacalib_L1ALFA_ELAST15',
    'alfacalib_L1ALFA_ELAST18',
    'alfacalib_L1ALFA_SYS',
    'alfacalib_L1LHCF_ALFA_ANY_A',
    'alfacalib_L1LHCF_ALFA_ANY_A_UNPAIRED_ISO',
    'alfacalib_L1LHCF_ALFA_ANY_C',
    'alfacalib_L1LHCF_ALFA_ANY_C_UNPAIRED_ISO',
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
    'j40_L1ZB',
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
    'larnoiseburst_L1J40_XE50',
    'larnoiseburst_L1J50',
    'larnoiseburst_L1J75',
    'larnoiseburst_L1J75_XE40',
    'larnoiseburst_L1XE35',
    'larnoiseburst_L1XE50',
    'larnoiseburst_L1XE80',
    'larnoiseburst_loose_L1All',
    'larnoiseburst_loose_rerun',
    'larnoiseburst_rerun',
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
    'lumipeb_vdm_L1MBTS_1',
    'lumipeb_vdm_L1MBTS_1_BGRP11',
    'lumipeb_vdm_L1MBTS_1_BGRP9',
    'lumipeb_vdm_L1MBTS_1_UNPAIRED_ISO',
    'lumipeb_vdm_L1MBTS_2',
    'lumipeb_vdm_L1MBTS_2_BGRP11',
    'lumipeb_vdm_L1MBTS_2_BGRP9',
    'lumipeb_vdm_L1MBTS_2_UNPAIRED_ISO',
    'lumipeb_vdm_L1RD0_BGRP11',
    'lumipeb_vdm_L1RD0_BGRP9',
    'lumipeb_vdm_L1RD0_FILLED',
    'lumipeb_vdm_L1RD0_UNPAIRED_ISO',
    'mu0_muoncalib',
    'mu0_muoncalib_ds3',
    'mu0_muoncalib_ds3_L1MU15',
    'mu0_muoncalib_ds3_L1MU4_EMPTY',
    'mu0_muoncalib_L1MU15',
    'mu0_muoncalib_L1MU4_EMPTY',
    'noalg_bkg_L1Bkg',
    'noalg_bkg_L1J12',
    'noalg_bkg_L1J12_BGRP12',
    'noalg_bkg_L1J30.31ETA49',
    'noalg_bkg_L1J30.31ETA49_BGRP12',
    'noalg_cosmiccalo_L1EM3_EMPTY',
    'noalg_cosmiccalo_L1J12_EMPTY',
    'noalg_cosmiccalo_L1J12_FIRSTEMPTY',
    'noalg_cosmiccalo_L1J3031ETA49_EMPTY',
    'noalg_cosmiccalo_L1J30_EMPTY',
    'noalg_cosmiccalo_L1J30_FIRSTEMPTY',
    'noalg_cosmiccalo_L1RD1_EMPTY',
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
    #'satu20em_l1satmon_L1EM20VHI_FIRSTEMPTY',
    'satu20em_l1satmon_L1EM22VHI_FIRSTEMPTY',
    'satu20em_l1satmon_L1J100',
    'satu20em_l1satmon_L1J100.31ETA49',
    'satu20em_l1satmon_L1J100.31ETA49_FIRSTEMPTY',
    'satu20em_l1satmon_L1J100_FIRSTEMPTY',
    'sct_noise',
    'tilecalib_laser',
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
    ]
ps_fwdphys_list=[
    'lhcfpeb',
    'lhcfpeb_L1LHCF_ALFA_ANY_A',
    'lhcfpeb_L1LHCF_ALFA_ANY_A_UNPAIRED_ISO',
    'lhcfpeb_L1LHCF_ALFA_ANY_C',
    'lhcfpeb_L1LHCF_ALFA_ANY_C_UNPAIRED_ISO',
    'lhcfpeb_L1LHCF_EMPTY',
    'lhcfpeb_L1LHCF_UNPAIRED_ISO',
    'mb_sptrk_vetombts2in_L1ALFA_CEP',
    'mb_sptrk_vetombts2in_L1TRT_ALFA_EINE',
    'mb_sptrk_vetombts2in_L1ALFA_ANY',
    'mb_sptrk_vetombts2in_L1ALFA_ANY_UNPAIRED_ISO',
    'mb_sptrk_vetombts2in_L1TRT_ALFA_ANY',
    'mb_sptrk_vetombts2in_L1TRT_ALFA_ANY_UNPAIRED_ISO',
    'mb_sptrk_vetombts2in_peb_L1ALFA_ANY',
    'mb_sptrk_vetombts2in_peb_L1ALFA_ANY_UNPAIRED_ISO',
    'mb_sptrk_vetombts2in_peb_L1TRT_ALFA_ANY',
    'mb_sptrk_vetombts2in_peb_L1TRT_ALFA_ANY_UNPAIRED_ISO',
    'noalg_L1ALFA_ANY',
    'noalg_L1ALFA_Phys',
    'noalg_L1ALFA_PhysAny',
    'noalg_L1ALFA_SYS',
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
    'mb_perf_L1LUCID',
    'mb_perf_L1LUCID_EMPTY',
    'mb_perf_L1LUCID_UNPAIRED_ISO',
    'mb_perf_L1MBTS_2',
    'mb_perf_L1RD1_FILLED',
    'mb_sp700_hmtperf_L1TE5',
    'mb_sp2100_hmtperf_L1TE10',
    'mb_sp2100_hmtperf_L1TE20',
    'mb_sp2100_hmtperf_L1TE30',
    'mb_sp2100_hmtperf_L1TE40',
    'mb_sp2100_hmtperf_L1TE50',
    'mb_sp2100_hmtperf_L1TE60',
    'mb_sp1500_hmtperf_L1TE50',
    'mb_sp1500_hmtperf_L1TE60',
    'mb_sp1400_trk100_hmt_L1TE40',
    'mb_sp800_pusup400_trk50_hmt_L1TE5',
    'mb_sp900_pusup500_trk60_hmt_L1TE5',
    'mb_sp1200_pusup700_trk70_hmt_L1TE5',
    'mb_sp1400_pusup800_trk80_hmt_L1TE5',
    'mb_sp1600_pusup1100_trk90_hmt_L1TE10',
    'mb_sp2000_pusup1200_trk100_hmt_L1TE10',
    'mb_sp1600_pusup1100_trk90_hmt_L1TE20',
    'mb_sp2000_pusup1200_trk100_hmt_L1TE20',
    'mb_sp2200_pusup1300_trk110_hmt_L1TE20',
    'mb_sp2400_pusup1400_trk120_hmt_L1TE20',
    'mb_sp1600_pusup1100_trk90_hmt_L1TE30',
    'mb_sp2000_pusup1200_trk100_hmt_L1TE30',
    'mb_sp2200_pusup1300_trk110_hmt_L1TE30',
    'mb_sp2400_pusup1400_trk120_hmt_L1TE30',
    'mb_sp1600_pusup1100_trk90_hmt_L1TE40',
    'mb_sp2000_pusup1200_trk100_hmt_L1TE40',
    'mb_sp2200_pusup1300_trk110_hmt_L1TE40',
    'mb_sp2400_pusup1400_trk120_hmt_L1TE40',
    'mb_sp1600_pusup1100_trk90_hmt_L1TE50',
    'mb_sp2000_pusup1200_trk100_hmt_L1TE50',
    'mb_sp2200_pusup1300_trk110_hmt_L1TE50',
    'mb_sp2400_pusup1400_trk120_hmt_L1TE50',
    'mb_sp1600_pusup1100_trk90_hmt_L1TE60',
    'mb_sp2000_pusup1200_trk100_hmt_L1TE60',
    'mb_sp2200_pusup1300_trk110_hmt_L1TE60',
    'mb_sp2400_pusup1400_trk120_hmt_L1TE60',
    'mb_sptrk_vetombts2in_L1ZDC_AND',
    'mb_sptrk_vetombts2in_L1ZDC_A_C',
    'mb_sp1400_trk100_hmt_L1TE40.0ETA24',
    'mb_sp1000_trk70_hmt_L1MBTS_1_1',
    'mb_sp1200_trk75_hmt_L1MBTS_1_1',
    'mb_sp1300_hmtperf_L1TE20',
    'mb_sp1300_hmtperf_L1TE20.0ETA24',
    'mb_sp1300_hmtperf_L1TE30',
    'mb_sp1300_hmtperf_L1TE40',
    'mb_sp1300_hmtperf_L1TE40.0ETA24',
    'mb_sp1300_hmtperf_L1TE5',
    'mb_sp1300_hmtperf_L1TE5.0ETA24',
    'mb_sp1300_hmtperf_L1TE10',
    'mb_sp1300_hmtperf_L1TE10.0ETA24',
    'mb_sp1400_pusup550_trk90_hmt_L1TE20',
    'mb_sp1400_pusup550_trk90_hmt_L1TE20.0ETA24',
    'mb_sp1400_pusup550_trk90_hmt_L1TE30',
    'mb_sp1400_pusup550_trk90_hmt_L1TE40',
    'mb_sp1400_pusup550_trk90_hmt_L1TE40.0ETA24',
    'mb_sp1400_pusup550_trk90_hmt_L1TE10',
    'mb_sp1400_pusup550_trk90_hmt_L1TE10.0ETA24',
    'mb_sp1400_pusup550_trk90_hmt_L1TE50',
    'mb_sp1500_trk100_hmt_L1TE40',
    'mb_sp1500_trk100_hmt_L1TE40.0ETA24',
    'mb_sp1400_trk80_hmt_L1MBTS_1_1',
    'mb_sp1400_trk90_hmt_L1TE20',
    'mb_sp1400_trk90_hmt_L1TE20.0ETA24',
    'mb_sp1400_trk90_hmt_L1TE30',
    'mb_sp1400_trk90_hmt_L1TE40',
    'mb_sp1400_trk90_hmt_L1TE40.0ETA24',
    'mb_sp1400_trk90_hmt_L1TE10',
    'mb_sp1400_trk90_hmt_L1TE10.0ETA24',
    'mb_sp1500_hmtperf_L1TE20',
    'mb_sp1500_hmtperf_L1TE20.0ETA24',
    'mb_sp1500_hmtperf_L1TE30',
    'mb_sp1500_hmtperf_L1TE40',
    'mb_sp1500_hmtperf_L1TE40.0ETA24',
    'mb_sp1500_hmtperf_L1TE10',
    'mb_sp1500_hmtperf_L1TE10.0ETA24',
    'mb_sp1500_pusup700_trk100_hmt_L1TE20',
    'mb_sp1500_pusup700_trk100_hmt_L1TE20.0ETA24',
    'mb_sp1500_pusup700_trk100_hmt_L1TE30',
    'mb_sp1500_pusup700_trk100_hmt_L1TE40',
    'mb_sp1500_pusup700_trk100_hmt_L1TE40.0ETA24',
    'mb_sp1500_pusup700_trk100_hmt_L1TE10',
    'mb_sp1500_pusup700_trk100_hmt_L1TE10.0ETA24',
    'mb_sp1500_pusup700_trk100_hmt_L1TE50',
    'mb_sp1500_trk100_hmt_L1TE20',
    'mb_sp1500_trk100_hmt_L1TE20.0ETA24',
    'mb_sp1500_trk100_hmt_L1TE30',
    'mb_sp1500_trk100_hmt_L1TE40',
    'mb_sp1500_trk100_hmt_L1TE40.0ETA24',
    'mb_sp1500_trk100_hmt_L1TE10',
    'mb_sp1500_trk100_hmt_L1TE10.0ETA24',
    'mb_sp1700_hmtperf_L1TE20',
    'mb_sp1700_hmtperf_L1TE20.0ETA24',
    'mb_sp1700_hmtperf_L1TE30',
    'mb_sp1700_hmtperf_L1TE40',
    'mb_sp1700_hmtperf_L1TE40.0ETA24',
    'mb_sp1700_hmtperf_L1TE10',
    'mb_sp1700_hmtperf_L1TE10.0ETA24',
    'mb_sp1800_hmtperf',
    'mb_sp1800_hmtperf_L1TE40',
    'mb_sp1800_pusup800_trk110_hmt_L1TE20',
    'mb_sp1800_pusup800_trk110_hmt_L1TE20.0ETA24',
    'mb_sp1800_pusup800_trk110_hmt_L1TE30',
    'mb_sp1800_pusup800_trk110_hmt_L1TE40',
    'mb_sp1800_pusup800_trk110_hmt_L1TE40.0ETA24',
    'mb_sp1800_pusup800_trk110_hmt_L1TE10',
    'mb_sp1800_pusup800_trk110_hmt_L1TE10.0ETA24',
    'mb_sp1800_pusup800_trk110_hmt_L1TE50',
    'mb_sp1800_trk110_hmt_L1TE20',
    'mb_sp1800_trk110_hmt_L1TE20.0ETA24',
    'mb_sp1800_trk110_hmt_L1TE30',
    'mb_sp1800_trk110_hmt_L1TE40',
    'mb_sp1800_trk110_hmt_L1TE40.0ETA24',
    'mb_sp1800_trk110_hmt_L1TE10',
    'mb_sp1800_trk110_hmt_L1TE10.0ETA24',
    'mb_sp2100_pusup1000_trk120_hmt_L1TE20',
    'mb_sp2100_pusup1000_trk120_hmt_L1TE30',
    'mb_sp2100_pusup1000_trk120_hmt_L1TE40',
    'mb_sp2100_pusup1000_trk120_hmt_L1TE50',
    'mb_sp2400_pusup1100_trk130_hmt_L1TE20',
    'mb_sp2400_pusup1100_trk130_hmt_L1TE30',
    'mb_sp2400_pusup1100_trk130_hmt_L1TE40',
    'mb_sp2400_pusup1100_trk130_hmt_L1TE50',
    'mb_sp2700_pusup1300_trk140_hmt_L1TE20',
    'mb_sp2700_pusup1300_trk140_hmt_L1TE30',
    'mb_sp2700_pusup1300_trk140_hmt_L1TE40',
    'mb_sp2700_pusup1300_trk140_hmt_L1TE50',
    'mb_sp3000_pusup1400_trk150_hmt_L1TE20',
    'mb_sp3000_pusup1400_trk150_hmt_L1TE30',
    'mb_sp3000_pusup1400_trk150_hmt_L1TE40',
    'mb_sp3000_pusup1400_trk150_hmt_L1TE50',
    'mb_sp3000_pusup1400_trk150_hmt_L1TE60',
    'mb_sp3000_pusup1500_trk160_hmt_L1TE20',
    'mb_sp3000_pusup1500_trk160_hmt_L1TE30',
    'mb_sp3000_pusup1500_trk160_hmt_L1TE40',
    'mb_sp3000_pusup1500_trk160_hmt_L1TE50',
    'mb_sp3000_pusup1500_trk160_hmt_L1TE60',
    'mb_sp2000_pusup600_trk70_hmt',
    'mb_sp2000_pusup600_trk70_hmt_L1TE60',
    'mb_sp2000_pusup600_trk70_hmt_L1TE70',
    'mb_sp2000_pusup700_trk50_sumet110_hmt_L1TE70',
    'mb_sp2000_pusup700_trk50_sumet70_hmt_L1TE60',
    'mb_sp2000_pusup700_trk60_hmt_L1TE60',
    'mb_sp2000_pusup700_trk70_hmt_L1TE60',
    'mb_sp2000_pusup700_trk70_hmt_L1TE70',
    'mb_sp2000_trk70_hmt',
    'mb_sp2500_hmtperf_L1TE40',
    'mb_sp2500_pusup750_trk90_hmt_L1TE70',
    'mb_sp2_hmtperf',
    'mb_sp2_hmtperf_L1MBTS_1_1',
    'mb_sp2_hmtperf_L1TE5',
    'mb_sp2_hmtperf_L1TE5.0ETA24',
    'mb_sp300_trk10_sumet40_hmt_L1MBTS_1_1',
    'mb_sp300_trk10_sumet50_hmt_L1MBTS_1_1',
    'mb_sp300_trk10_sumet50_hmt_L1RD3_FILLED',
    'mb_sp300_trk10_sumet50_hmt_L1TE20',
    'mb_sp300_trk10_sumet60_hmt_L1MBTS_1_1',
    'mb_sp300_trk10_sumet60_hmt_L1TE40',
    'mb_sp300_trk10_sumet70_hmt_L1MBTS_1_1',
    'mb_sp300_trk10_sumet80_hmt_L1MBTS_1_1',
    'mb_sp400_trk40_hmt_L1MBTS_1_1',
    'mb_sp500_hmtperf',
    'mb_sp600_trk10_sumet40_hmt_L1MBTS_1_1',
    'mb_sp600_trk10_sumet50_hmt_L1MBTS_1_1',
    'mb_sp600_trk10_sumet50_hmt_L1RD3_FILLED',
    'mb_sp600_trk10_sumet50_hmt_L1TE20.0ETA24',
    'mb_sp600_trk10_sumet60_hmt_L1MBTS_1_1',
    'mb_sp600_trk10_sumet60_hmt_L1TE40.0ETA24',
    'mb_sp600_trk10_sumet70_hmt_L1MBTS_1_1',
    'mb_sp600_trk10_sumet80_hmt_L1MBTS_1_1',
    'mb_sp600_trk45_hmt_L1MBTS_1_1',
    'mb_sp700_trk50_hmt_L1MBTS_1_1',
    'mb_sp700_trk50_hmt_L1RD3_FILLED',
    'mb_sp700_trk55_hmt_L1MBTS_1_1',
    'mb_sp900_pusup350_trk60_hmt_L1TE20',
    'mb_sp900_pusup350_trk60_hmt_L1TE20.0ETA24',
    'mb_sp900_pusup350_trk60_hmt_L1TE10',
    'mb_sp900_pusup350_trk60_hmt_L1TE10.0ETA24',
    'mb_sp900_trk60_hmt_L1MBTS_1_1',
    'mb_sp900_trk60_hmt_L1TE20',
    'mb_sp900_trk60_hmt_L1TE20.0ETA24',
    'mb_sp900_trk60_hmt_L1TE10',
    'mb_sp900_trk60_hmt_L1TE10.0ETA24',
    'mb_sp900_trk65_hmt_L1MBTS_1_1',
    'mb_sptrk',
    'mb_sptrk_costr',
    'mb_sptrk_costr_L1RD0_EMPTY',
    'mb_sptrk_L1RD0_EMPTY',
    'mb_sptrk_L1RD0_UNPAIRED_ISO',
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
    'mb_sptrk_vetombts2in_L1ALFA_CEP',
    'mb_sptrk_vetombts2in_L1RD0_FILLED',
    'mb_sptrk_vetombts2in_peb_L1ALFA_ANY',
    'mb_sptrk_vetombts2in_peb_L1ALFA_ANY_UNPAIRED_ISO',
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
    'noalg_mb_L1TE5',
    'noalg_mb_L1TE5.0ETA24',
    'noalg_mb_L1TE10',
    'noalg_mb_L1TE10.0ETA24',
    'noalg_mb_L1TE20',
    'noalg_mb_L1TE20.0ETA24',
    'noalg_mb_L1TE30',
    'noalg_mb_L1TE30.0ETA24',
    'noalg_mb_L1TE40',
    'noalg_mb_L1TE40.0ETA24',
    'noalg_mb_L1TE60',
    'noalg_mb_L1TE60.0ETA24',
    'noalg_mb_L1TE70',
    'noalg_mb_L1TE70.0ETA24',
    'noalg_mb_L1TE50',
    'noalg_mb_L1TE50.0ETA24',
    'noalg_mb_L1ZDC_A',
    'noalg_mb_L1ZDC_C',
    'noalg_mb_L1ZDC_AND',
    'noalg_mb_L1ZDC_A_C',
    ]

ps_rerun_list = [
    'xe0noL1_l2fsperf',
    'xe0noL1_l2fsperf_mht',
    'xe0noL1_l2fsperf_pueta',
    'xe0noL1_l2fsperf_pufit',
    'xe0noL1_l2fsperf_tc_em',
    'xe0noL1_l2fsperf_tc_lcw',
    ]
ps_larnoiseburst_rerun_list = [
    'larnoiseburst_loose_rerun',
    'larnoiseburst_rerun',
    ]

ps_perform_list = [
    'tau0_perf_ptonly_L1TAU12',
    'tau0_perf_ptonly_L1TAU60',
    'tau125_medium1_calo',
    'tau125_medium1_track',
    'tau125_perf_ptonly',
    'tau125_r1medium1',
    'tau125_r1perf',
    'tau160_idperf_track',
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
    '2e17_lhloose_2j35_boffperf',
    '2e17_lhloose_2j35_bperf',
    '2e17_loose_2j35_boffperf',
    '2e17_loose_2j35_bperf',
    '2j30_boffperf_L12J15_XE55',
    '2j35_boffperf_2j35_L13J25.0ETA23',
    '2j35_boffperf_2j35_L14J15.0ETA25',
    '2j35_bperf_2j35_L13J25.0ETA23',
    '2j35_bperf_2j35_L14J15.0ETA25',
    '2j45_boffperf_2j45',
    '2j45_boffperf_2j45_L13J25.0ETA23',
    '2j45_boffperf_2j45_L14J15.0ETA25',
    '2j45_bperf_2j45',
    '2j45_bperf_2j45_L13J25.0ETA23',
    '2j45_bperf_2j45_L14J15.0ETA25',
    '2j55_boffperf_2j55',
    '2j55_boffperf_2j55_L13J25.0ETA23',
    '2j55_boffperf_ht300_L14J20',
    '2j55_bperf_2j55',
    '2j55_bperf_2j55_L13J25.0ETA23',
    '2j55_bperf_L13J20_4J20.0ETA49_MJJ-400',
    '2j55_bperf_L13J20_4J20.0ETA49_MJJ-700',
    '2j55_bperf_L13J20_4J20.0ETA49_MJJ-800',
    '2j55_bperf_L13J20_4J20.0ETA49_MJJ-900',
#    '2j55_bperf_L14J20.0ETA49',
    '2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-400',
    '2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-700',
    '2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-800',
    '2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-900',
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
    'e26_lhtight_nod0_ivarloose_2j35_bperf',
    'e26_tight_iloose_2j35_boffperf',
    'e26_tight_iloose_2j35_bperf',
    'j0_perf_bperf_L1J12_EMPTY',
    'j0_perf_bperf_L1MU10',
    'j0_perf_bperf_L1RD0_EMPTY',
    'j100_2j55_boffperf',
    'j100_2j55_bperf',
    'j110_boffperf',
    'j110_bperf',
    'j150_boffperf',
    'j150_boffperf_j50_boffperf',
    'j150_bperf',
    'j150_bperf_j50_bperf',
    'j15_boffperf',
    'j15_bperf',
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
    'j45_boffperf_3j45',
    'j45_boffperf_3j45_L13J15.0ETA25',
    'j45_boffperf_3j45_L13J20',
    'j45_bperf',
    'j45_bperf_3j45',
    'j45_bperf_3j45_L13J15.0ETA25',
    'j45_bperf_3j45_L13J20',
    'j45_bperf_3j45_L14J20',
    'j55_boffperf',
    'j55_boffperf_ht500_L14J20',
    'j55_bperf',
    'j55_bperf_3j55',
    'j55_bperf_ht500_L14J20',
#    'j60_boffperf',
    'j60_boffperf_j60_L12J40_XE45',
    'j65_boffperf_3j65_L13J25.0ETA23',
    'j65_boffperf_3j65_L14J15.0ETA25',
    'j65_bperf_3j65_L13J25.0ETA23',
    'j65_bperf_3j65_L14J15.0ETA25',
    'j70_boffperf_3j70',
    'j70_boffperf_3j70_L13J25.0ETA23',
    'j70_boffperf_3j70_L14J15.0ETA25',
    'j70_bperf_3j70',
    'j70_bperf_3j70_L13J25.0ETA23',
    'j70_bperf_3j70_L14J15.0ETA25',
    'j75_boffperf_3j75',
    'j75_boffperf_3j75_L13J25.0ETA23',
    'j75_bperf_3j75',
    'j75_bperf_3j75_L13J25.0ETA23',
    'j80_boffperf_L12J50_XE40',
    'j80_boffperf_L1J400ETA25_XE50',
    'j80_boffperf_L1J40_XE50',
    'j80_boffperf_L1J40_XE60',
    'j80_boffperf_L1XE60',
    'j85_boffperf',
    'j85_bperf',
    'mu26_imedium_2j35_boffperf',
    'mu26_imedium_2j35_bperf',
    'mu4_j15_dr05' ,
    'mu4_j35_dr05'  ,
    'mu4_j55_dr05' ,
    'mu6_j110_dr05',
    'mu6_j150_dr05',
    'mu6_j175_dr05',
    'mu6_j260_dr05',
    'mu6_j320_dr05',
    'mu6_j400_dr05',
    'mu6_j85_dr05' ,

    ]

ps_Jpsi_list = [
    'e5_tight_e4_etcut_Jpsiee',
    'e5_lhtight_e4_etcut_Jpsiee',
    'e5_lhtight_nod0_e4_etcut_Jpsiee',
    'e5_tight_e4_etcut_Jpsiee_L1JPSI-1M5',
    'e5_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5',
    'e5_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5',
    'e9_tight_e4_etcut_Jpsiee',
    'e9_lhtight_e4_etcut_Jpsiee',
    'e9_lhtight_nod0_e4_etcut_Jpsiee',
    'e9_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7',
    'e9_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7',
    'e9_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM7',
    'e9_etcut_e5_tight_Jpsiee',
    'e9_etcut_e5_lhtight_Jpsiee',
    'e9_etcut_e5_lhtight_nod0_Jpsiee',
    'e9_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM7',
    'e9_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM7',
    'e9_etcut_e5_lhtight_nod0_Jpsiee_L1JPSI-1M5-EM7',
    'e14_tight_e4_etcut_Jpsiee',
    'e14_lhtight_e4_etcut_Jpsiee',
    'e14_lhtight_nod0_e4_etcut_Jpsiee',
    'e14_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12',
    'e14_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12',
    'e14_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM12',
    'e14_etcut_e5_tight_Jpsiee',
    'e14_etcut_e5_lhtight_Jpsiee',
    'e14_etcut_e5_lhtight_nod0_Jpsiee',
    'e14_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM12',
    'e14_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM12',
    'e14_etcut_e5_lhtight_nod0_Jpsiee_L1JPSI-1M5-EM12',
    'e24_lhtight_L1EM20VH_e15_etcut_Zee',
    'e24_tight_L1EM20VH_e15_etcut_Zee',
    'e26_lhtight_e15_etcut_Zee',
    'e26_tight_e15_etcut_Zee',
]

ps_ftk_list = [
]


if TriggerFlags.doFTK():
    ps_ftk_list = [
        'beamspot_idperf_FTK',
        'beamspot_trkFS_FTK',
        'mu6_idperf_FTK',
        'mu24_idperf_FTK',
        'mu6_idperf_FTKRefit',
        'mu24_idperf_FTKRefit',
        'tau0_idperf_FTK',
        'tau0_idperf_FTKRefit',
        'tau0_perf_FTK',
        'tau0_perf_FTKRefit',
        'tau25_idperf_FTK',
        'tau25_idperf_FTKRefit',
        'tau25_perf_FTK',
        'tau25_perf_FTKRefit',
        'tau25_medium1_FTK',
        'tau25_medium1_FTKRefit',
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
        '2j35_bmv2c2070_2j35_L13J25.0ETA23_FTK',
        '2j45_bmv2c2077_2j45_L13J25.0ETA23_FTK',
        'j175_bmv2c2085_FTK',
        '2j35_bmv2c2070_2j35_L13J25.0ETA23_FTKVtx',
        '2j45_bmv2c2077_2j45_L13J25.0ETA23_FTKVtx',
        'j175_bmv2c2085_FTKVtx',
        '2j35_bmv2c2070_2j35_L13J25.0ETA23_FTKRefit',
        '2j45_bmv2c2077_2j45_L13J25.0ETA23_FTKRefit',
        'j175_bmv2c2085_FTKRefit',
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

chain_list=ps_eb_list+ps_fwdphys_list+ps_minb_list+ps_Jpsi_list
if not TriggerFlags.doFTK():
    chain_list+=ps_ftk_list

Prescales.HLTPrescales_tightperf_mc_prescale.update(dict(map(None,chain_list,len(chain_list)*[ [-1, 0,-1] ])))

######################################################
# TIGHT mc prescales
######################################################
Prescales.L1Prescales_tight_mc_prescale  = deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_tight_mc_prescale = deepcopy(Prescales.HLTPrescales_tightperf_mc_prescale)
chain_list=ps_perform_list+ps_Jpsi_list

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
