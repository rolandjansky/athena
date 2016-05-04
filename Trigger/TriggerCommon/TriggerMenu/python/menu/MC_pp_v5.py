# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TriggerMenu.menu.Physics_pp_v5 as physics_menu

from TriggerJobOpts.TriggerFlags          import TriggerFlags
from copy                                 import deepcopy



from AthenaCommon.Logging import logging
log = logging.getLogger( 'MC_pp_v5.py' )


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

        ['mu20_r1extr',            'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10_r1extr',            'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu4_r1extr',             'L1_MU4',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
	
        ['mu20_imedium_mu8noL1',    'L1_MU20',  ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_imedium','mu8noL1']]],
        ['mu20_iloose_mu8noL1',     'L1_MU20',  ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_iloose','mu8noL1']]],
        ['mu20_iloose_2mu4noL1',    'L1_MU20',  ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_iloose','2mu4noL1']]],

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
    

        ]

    TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + [

        #multi-jet
        ['5j65_0eta240', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['5j70_0eta240', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],

        #Forward jets
        #['j60_240eta490',          'L1_J15.23ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        # Test chains, to be removed
        ['j85_cleanLLP',                    'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j85_cleanL',                    'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j85_cleanT',                    'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        ## Additional jet chains(ATR-12772)
        # ['j60_TT', 'L1_J20', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        ['10j40_L16J15', 'L1_6J15' , [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['10j40_L14J20', 'L1_4J20' , [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],

        ['j260_a10_sub_L1J75', 'L1_J75', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j260_a10_lcw_sub_L1J75', 'L1_J75', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j260_a10r_L1J75', 'L1_J75', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j300_a10_sub_L1J75', 'L1_J75', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j300_a10_lcw_sub_L1J75', 'L1_J75', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j300_a10r_L1J75', 'L1_J75', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        ['j40_0eta490_j30_0eta490_deta25_xe80',    'L1_XE60', ['','',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,["j40_0eta490","j30_0eta490_deta25","xe80"]]],
        ['j40_j30_0eta490_deta25_xe80_tc_lcw',     'L1_XE60', ['','',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,["j40","j30_0eta490_deta25","xe80_tc_lcw"]]],
        ['j65_0eta490_j40_0eta490_invm250_xe80',   'L1_XE60', ['','',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,["j65_0eta490","j40_0eta490_invm250","xe80"]]],



  #      ['j50_j30_0eta490_deta3_xe80_L1_J40_XE50',       'L1_J40_XE50',['','',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j50","j30_0eta490_deta3","xe80_L1_J40_XE50"]]],                                                                                                                                                   
 #       ['j50_j30_0eta490_deta3_xe80_L1_J30.0ETA49',       'L1_J30.0ETA49',['','',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j50","j30_0eta490_deta3","xe80_L1_J30.0ETA49"]]],

        ]

    TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [

        ['2j75_bperf_j75', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j70_bperf_j70', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j100_2j55_bperf', 'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j55_bperf_2j55', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j45_bperf_2j45', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j75_bperf_3j75', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j70_bperf_3j70', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['2j55_bperf_2j55_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j45_bperf_2j45_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j75_bperf_3j75_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j70_bperf_3j70_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
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
        ['2j55_bperf_split_2j55_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['2j45_bperf_split_2j45_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Bjet'], -1],
        ['j75_bperf_split_3j75_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
        ['j70_bperf_split_3j70_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Bjet'], -1],
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
        
        ]

    TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [
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


        ]


    TriggerFlags.EgammaSlice.signatures = TriggerFlags.EgammaSlice.signatures() + [

        # technical chains for forward electrons
        #['e17_loose_L1EM15VHJ1523ETA49',       'L1_EM15VH_J15.23ETA49', ['L1_EM15VH','L1_J15.23ETA49'], [PhysicsStream], ['RATE:Combined', 'BW:Egamma'],-1], 
        #['e17_lhloose_L1EM15VHJ1523ETA49',     'L1_EM15VH_J15.23ETA49', ['L1_EM15VH','L1_J15.23ETA49'], [PhysicsStream], ['RATE:Combined', 'BW:Egamma'],-1], 
        ['e17_loose_L1EM15VHJJ1523ETA49',       'L1_EM15VH_JJ15.23ETA49', ['L1_EM15VH','L1_JJ15.23ETA49'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
        ['e17_lhloose_L1EM15VHJJ1523ETA49',     'L1_EM15VH_JJ15.23ETA49', ['L1_EM15VH','L1_JJ15.23ETA49'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
        

        # Supporting triggers with 

        
        ##########        

        # Fall-back if problem with new tracking kept as before)
        ['e17_etcut_L1EM15',                      'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_etcut_idperf_L1EM15',               'L1_EM15', [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1],
        
        # extra id perf chains for HLT ID group (kept as before)

        ]


    ###########################################################################################################
    #   Bphysics
    ###########################################################################################################
    TriggerFlags.BphysicsSlice.signatures = TriggerFlags.BphysicsSlice.signatures() + [

        #['2mu6_bMuMu_L12MU6', 'L1_2MU6', [], [PhysicsStream], [], -1], 

        ['2mu4_bBmumu_noL2',    'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bBmumu_noL2', 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bBmumu_noL2',    'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
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
        
        ['2mu4_bDimu_Lxy0',                    'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_bJpsimumu_Lxy0',                    'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_bBmumu_Lxy0',                    'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bDimu_Lxy0',                    'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bJpsimumu_Lxy0',                    'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bBmumu_Lxy0',                    'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bDimu_Lxy0',                    'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bJpsimumu_Lxy0',                    'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bBmumu_Lxy0',                    'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        
        ]



    TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures() + [

        # all e/g + X triggers are taken from DC14 and L1 thresholds adjusted
        
#        ['g10_loose_mu10_taumass',   'L1_EM7_MU10', ['L1_EM7', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g10_loose","mu10"]] ],
#        ['g20_loose_mu4_taumass',    'L1_EM15_MU4', ['L1_EM15', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g20_loose","mu4"]] ],
#        ['g10_loose_mu10',           'L1_EM7_MU10', ['L1_EM7', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],
#        ['g20_loose_mu4',            'L1_EM15_MU4', ['L1_EM15', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],
#        


        ['g45_tight_2j50noL1',           'L1_EM20VH',  ['L1_EM20VH',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:Jets'], -1,['serial',-1,["g45_tight","2j50noL1"]]],
        ['g75_tight_2j25noL1',           'L1_EM20VH',  ['L1_EM20VH',''],  [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:Jets'], -1,['serial',-1,["g75_tight","2j25noL1"]]],

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
        ['j120_xe80',               'L1_J40_XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets', 'BW:MET'], -1,['serial',-1,["j120","xe80"]]],        
        ['j150_xe80',               'L1_J40_XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets', 'BW:MET'], -1,['serial',-1,["j150","xe80"]]],        

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

        # Isolated Variant

        # Those are somehow not working
        ['e17_medium_iloose_tau25_perf_ptonly_L1EM15HI_2TAU12IM',   'L1_EM15HI_2TAU12IM',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_medium_iloose","tau25_perf_ptonly"]]],
        ['e17_lhmedium_iloose_tau25_perf_ptonly_L1EM15HI_2TAU12IM', 'L1_EM15HI_2TAU12IM',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_iloose","tau25_perf_ptonly"]]],

        # Tau + Muon

        # Also somehow not working...
        ['mu14_iloose_tau25_perf_ptonly_L1MU10_TAU12IM',      'L1_MU10_TAU12IM',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_iloose","tau25_perf_ptonly"]]],        

        #tau + photon
        ['g35_medium_tau25_perf_tracktwo_L1TAU12',     'L1_EM22VHI', ['L1_EM22VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["g35_medium","tau25_perf_tracktwo_L1TAU12"]]],



        # 4-jet items (VBF H->bb, hh->bbbb)
        #['mu4_4j40_dr05_L14J20', 'L1_4J20', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu4', '4j40'] ]],
        ['mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu4', '3j35', 'j35_bloose_split_antimatchdr05mu'] ]],
        ['mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L14J20', 'L1_4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j30', 'j30_bmedium_split_antimatchdr05mu'] ]],


        ['mu4_4j40_dr05_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '4j40'] ],False],
        ['mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j35', 'j35_bloose_split_antimatchdr05mu'] ], False],
        ['mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L13J20_BTAG-MU4J20', 'L1_3J20_BTAG-MU4J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j30', 'j30_bmedium_split_antimatchdr05mu'] , False]],
        ['mu4_4j40_dr05_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '4j40'] ], False],
        ['mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j35', 'j35_bloose_split_antimatchdr05mu'] ], False],
        ['mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L13J15_BTAG-MU4J15', 'L1_3J15_BTAG-MU4J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j30', 'j30_bmedium_split_antimatchdr05mu'] ], False],




        #Backup
        ['mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L1MU4_3J15', 'L1_MU4_3J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j35', 'j35_bloose_split_antimatchdr05mu'] ]],
        ['mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L1MU4_3J15', 'L1_MU4_3J15', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j30', 'j30_bmedium_split_antimatchdr05mu'] ]],
        ['mu4_3j35_dr05_j35_bloose_split_antimatchdr05mu_L1MU4_3J20', 'L1_MU4_3J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j35', 'j35_bloose_split_antimatchdr05mu'] ]],
        ['mu4_3j30_dr05_j30_bmedium_split_antimatchdr05mu_L1MU4_3J20', 'L1_MU4_3J20', ['L1_MU4', '', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet'], -1,['serial',-1,['mu4', '3j30', 'j30_bmedium_split_antimatchdr05mu'] ]],
        
        
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
#        ['beamspot_allTE_L2StarB_pebTRT',           'L1_4J15', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
#        ['beamspot_activeTE_L2StarB_pebTRT',           'L1_4J15', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
#        ['beamspot_trkFS_L2StarB',           'L1_4J15', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
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

    TriggerFlags.MonitorSlice.signatures = TriggerFlags.MonitorSlice.signatures() + []
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
    'beamspot_activeTE_L2StarB_peb',
    'beamspot_activeTE_L2StarB_peb_L1TRT_EMPTY',
    'beamspot_activeTE_L2StarB_peb_L1TRT_FILLED',
#    'beamspot_activeTE_L2StarB_pebTRT',
    'beamspot_activeTE_L2StarB_pebTRT_L1TRT_EMPTY',
    'beamspot_activeTE_L2StarB_pebTRT_L1TRT_FILLED',
    'beamspot_activeTE_trkfast_peb',
    'beamspot_activeTE_trkfast_peb_L1TRT_EMPTY',
    'beamspot_activeTE_trkfast_peb_L1TRT_FILLED',
    'beamspot_activeTE_trkfast_pebTRT',
    'beamspot_activeTE_trkfast_pebTRT_L1TRT_EMPTY',
    'beamspot_activeTE_trkfast_pebTRT_L1TRT_FILLED',
    'beamspot_allTE_L2StarB_peb',
    'beamspot_allTE_L2StarB_peb_L1TRT_EMPTY',
    'beamspot_allTE_L2StarB_peb_L1TRT_FILLED',
#    'beamspot_allTE_L2StarB_pebTRT',
    'beamspot_allTE_L2StarB_pebTRT_L1TRT_EMPTY',
    'beamspot_allTE_L2StarB_pebTRT_L1TRT_FILLED',
    'beamspot_allTE_trkfast_peb',
    'beamspot_allTE_trkfast_peb_L1TRT_EMPTY',
    'beamspot_allTE_trkfast_peb_L1TRT_FILLED',
    'beamspot_allTE_trkfast_pebTRT',
    'beamspot_allTE_trkfast_pebTRT_L1TRT_EMPTY',
    'beamspot_allTE_trkfast_pebTRT_L1TRT_FILLED',
#    'beamspot_trkFS_L2StarB',
    'beamspot_trkFS_L2StarB_L1TRT_EMPTY',
    'beamspot_trkFS_L2StarB_L1TRT_FILLED',
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
    'mb_sp1400_trk100_hmt_L1TE40',
    'mb_sp1400_trk100_hmt_L1TE40.0ETA24',
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
    'mb_sptrk_vetombts2in_L1ZDC_AND',
    'mb_sptrk_vetombts2in_L1ZDC_A_C',
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
    'noalg_mb_L1TE50',
    'noalg_mb_L1TE50.0ETA24',
    'noalg_mb_L1TE60',
    'noalg_mb_L1TE60.0ETA24',
    'noalg_mb_L1TE70',
    'noalg_mb_L1TE70.0ETA24',
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

#-----------------------------------------------------

chain_list=ps_online_list+ps_cosmic_list+ps_minb_list
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
Prescales.HLTPrescales_loose_mc_prescale.update(dict(map(None,chain_list,len(chain_list)*[ [-1, 0,-1] ])))

chain_list=ps_rerun_list
Prescales.HLTPrescales_loose_mc_prescale.update(dict(map(None,chain_list,len(chain_list)*[ [0, 0, 1] ])))
######################################################

######################################################
# TIGHT mc prescales
######################################################
Prescales.L1Prescales_tight_mc_prescale  = deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_tight_mc_prescale = deepcopy(Prescales.HLTPrescales_loose_mc_prescale)

chain_list=ps_eb_list+ps_fwdphys_list+ps_minb_list
Prescales.HLTPrescales_tight_mc_prescale.update(dict(map(None,chain_list,len(chain_list)*[ [-1, 0,-1] ])))
######################################################

######################################################
Prescales.L1Prescales_upgrade_mc_prescale  = deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_upgrade_mc_prescale = deepcopy(Prescales.HLTPrescales_tight_mc_prescale)
# Note: "upgrade" prescales are set with regular expressions at the end
#       of the setupMenu() function above 
######################################################
