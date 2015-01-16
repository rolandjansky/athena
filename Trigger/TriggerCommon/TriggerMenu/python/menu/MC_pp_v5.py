# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import TriggerMenu.menu.Physics_pp_v5 as physics_menu

from TriggerJobOpts.TriggerFlags          import TriggerFlags
from copy                                 import deepcopy



from AthenaCommon.Logging import logging
log = logging.getLogger( 'MC_pp_v5.py' )


#---------------------------------------------------------------------
# Just copy the MC v3 menu with empty HLT
#---------------------------------------------------------------------
def setupMenu():

    physics_menu.setupMenu()

    PhysicsStream="Main"

    # ------------------------------------------------
    # INPUT FORMAT FOR CHAINS:
    # ------------------------------------------------
    # ['chainName', chainCounter, 'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep]

    TriggerFlags.TestSlice.signatures = TriggerFlags.TestSlice.signatures() + [
        #['TestChain400',                640,    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ]



    TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
        ['mu0_perf',           4500, 'L1_MU4',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20_2mu0noL1_JpsimumuFS', 4729, 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','2mu0noL1_JpsimumuFS']]], 
        ['mu18_2mu0noL1_JpsimumuFS', 4730, 'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu18','2mu0noL1_JpsimumuFS']]],

        
        # Primary (single muon chains)
        ['mu26_imedium',           450, 'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu50',                   451, 'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu60_0eta105_msonly',    452, 'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],

        # Primary at low lumi (single muon chains)
        ['mu24_imedium',           453, 'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_iloose_L1MU15',     454, 'L1_MU15',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20_iloose_L1MU15',     455, 'L1_MU15',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],

        # Primary (multi muon chains)
        ['2mu14',                  456, 'L1_2MU10',          [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu6',                   457, 'L1_3MU6',           [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu6_msonly',            458, 'L1_3MU6',           [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],

        # Primary at low lumi (multi muon chains)
        ['2mu10',                  459, 'L1_2MU10',          [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],

        # Primary (full scan chains)
        ['mu24_mu8noL1',           460, 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','mu8noL1']]],
        ['mu24_2mu4noL1',          461, 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','2mu4noL1']]],

        # Primary at low lumi (full scan chains)
        ['mu22_mu8noL1',           462, 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','mu8noL1']]],
        ['mu22_2mu4noL1',          463, 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','2mu4noL1']]],
        ['mu20_mu8noL1',           464, 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','mu8noL1']]],
        ['mu20_2mu4noL1',          465, 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','2mu4noL1']]],
        ['mu18_mu8noL1',           466, 'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu18','mu8noL1']]],
        ['mu18_2mu4noL1',          467, 'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu18','2mu4noL1']]],

        # Supporting chains
        ['mu26',                   468, 'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_L1MU15',            469, 'L1_MU15',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu22',                   470, 'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20',                   471, 'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20_L1MU15',            472, 'L1_MU15',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20_r1extr',            479, 'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10_r1extr',            480, 'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu4_r1extr',             481, 'L1_MU4',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],

        # Rerun chains
        ['mu24',                   482, 'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_idperf',            483, 'L1_MU20',           [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon'], -1],
        ['mu20_idperf',            484, 'L1_MU20',           [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon'], -1],
        ['mu20_msonly',            485, 'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu18',                   486, 'L1_MU15',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu14',                   487, 'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10',                   488, 'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10_idperf',            489, 'L1_MU10',           [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon'], -1],
        ['mu10_msonly',            490, 'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu6',                    491, 'L1_MU6',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu6_idperf',             492, 'L1_MU6',            [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon'], -1],
        ['mu6_msonly',             493, 'L1_MU6',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu4',                    494, 'L1_MU4',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu4_idperf',             495, 'L1_MU4',            [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon'], -1],
        ['mu4_msonly',             496, 'L1_MU4',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],

        # Additional chains
        ['mu24_L2Star_idperf',     497, 'L1_MU20',  [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:IDMonitoring'], -1],
        ['mu6_L2Star_idperf',      498, 'L1_MU6',   [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:IDMonitoring'], -1],
        ['mu0noL1_fsperf',         499, '',         [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], 1],
        ['mu14_iloose_mu14',       500, 'L1_2MU10', [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu6',                   501, 'L1_2MU6',  [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu4',                   502, 'L1_2MU4',  [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['mu20_imedium_mu8noL1',   503, 'L1_MU20',  ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_imedium','mu8noL1']]],
        ['mu20_iloose_mu8noL1',    504, 'L1_MU20',  ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_iloose','mu8noL1']]],
        ['mu20_iloose_2mu4noL1',   505, 'L1_MU20',  ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_iloose','2mu4noL1']]],

        # T&P chains
        #['mu24_imedium_mu20noL1_msonly', 729, 'L1_MU20', [], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], 1],        
        #['mu24_imedium_mu20noL1_msonly', 730, 'L1_MU20', [], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], 1],       

        # Primary (narrow scan chains)
        ['mu11_2mu4noL1_nscan03_L1MU11_2MU6',   7146, 'L1_MU11_2MU6', ['L1_MU11_2MU6',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11','2mu4noL1_nscan03']]],        
        ['mu11_2mu4noL1_nscan03_L1MU10_2MU6',   7147, 'L1_MU10_2MU6', ['L1_MU10_2MU6',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11','2mu4noL1_nscan03']]],        
        ['mu20_msonly_mu6noL1_msonly_nscan05',  7148, 'L1_MU20',     ['L1_MU20',''],
[PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu6noL1_msonly_nscan05']]],

        ]

    TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + [
        ['j460_a10_sub_L1J100',     101,    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        ['j400_a4_sub', 328, 'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_a4_jes', 329, 'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        
        ['3j175',               110,    'L1_J100', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['ht1000_L1J100',        99,    'L1_J100', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j30_muvtx',             1,    'L1_2MU10',[], [PhysicsStream], ['RATE:MuonJet','BW:Muon'], -1],

        #TEST chains for HLT VBF alg (ATR-9609)
        ['2j55_deta2',                 1980,    'L1_J15',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['2j55_invm400',                 1981,    'L1_J15',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['2j55_deta2_invm400',                 1982,    'L1_J15',  [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],

        # LLP trigger (ATR-9673)
        #['j30_llp_L1TAU30',                 7100,    'L1_TAU30',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        #['j30_llp_L1210RATIO-0MATCH-TAU30si2-EMall',                 7101,    'L1_210RATIO-0MATCH-TAU30si2-EMall',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        #['j30_llp_L1NOT-0MATCH-TAU30si2-EMall',                 7102,    'L1_NOT-0MATCH-TAU30si2-EMall',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        #Partial scan
        ['j200_PS', 114, 'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],         
        ['3j175_PS', 115, 'L1_J100', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],


        # new chains
        ['j55',                 980,    'L1_J15',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j60',                 103,    'L1_J15',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j85',                 971,    'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j85_jes',             972,    'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j85_lcw',             973,    'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j85_lcw_jes',         974,    'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j100',                975,    'L1_J25',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j110',                105,    'L1_J30',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j150',                106,    'L1_J40',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j175',                976,    'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j175_jes',            977,    'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j175_lcw',            978,    'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j175_lcw_jes',        979,    'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j200',                107,    'L1_J50', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j260',                108,    'L1_J75', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j300',                981,    'L1_J85', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j320',                982,    'L1_J85', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j360',                983,    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j380',                984,    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j380_jes',            985,    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j380_lcw',            986,    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j380_lcw_jes',        987,    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400',                100,    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_sub',            988,    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_jes',            989,    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_lcw',            990,    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_lcw_jes',        991,    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420',                992,    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_jes',            993,    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_lcw',            994,    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_lcw_jes',        995,    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j440',                996,    'L1_J120', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j460',                997,    'L1_J120', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        #multi-jet
        ['4j45',                998,    'L1_3J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['4j85',                999,    'L1_3J40', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['4j85_jes',            350,    'L1_3J40', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['4j85_lcw',            351,    'L1_3J40', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['4j85_lcw_jes',        352,    'L1_3J40', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['4j100',               111,    'L1_3J50', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j45',                353,    'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j55',                354,    'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j60',                356,    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j70',                357,    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j85',                112,    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j85_jes',            358,    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j85_lcw',            359,    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j85_lcw_jes',        360,    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],

        ['6j45_0eta240',              361,    'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['6j45_0eta240_L14J20',       362,    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['6j45_0eta240_L15J150ETA24', 363,    'L1_5J15.0ETA24', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],

        ['6j50_0eta240_L14J20',       364,    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['6j50_0eta240_L15J150ETA24', 365,    'L1_5J15.0ETA24', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],

        ['6j55_0eta240_L14J20',       366,    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['6j55_0eta240_L15J150ETA24', 367,    'L1_5J15.0ETA24', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],

        ['7j45',                      368,    'L1_6J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['7j45_L14J20',               369,    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['7j45_0eta240_L14J20',       370,    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['7j45_0eta240_L15J150ETA24', 371,    'L1_5J15.0ETA24', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],

        #Forward jets
        ['j60_240eta490',       372,    'L1_J15.23ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j60_280eta320',       373,    'L1_J15.28ETA32', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j60_320eta490',       374,    'L1_J15.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j85_280eta320',        375,    'L1_J20.28ETA32', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j85_280eta320_jes',    376,    'L1_J20.28ETA32', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j85_280eta320_lcw',    377,    'L1_J20.28ETA32', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j85_280eta320_lcw_jes',378,    'L1_J20.28ETA32', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j85_320eta490',        380,    'L1_J20.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j110_320eta490',       381,    'L1_J30.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j175_320eta490',        382,    'L1_J50.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j175_320eta490_jes',    383,    'L1_J50.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j175_320eta490_lcw',    384,    'L1_J50.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j175_320eta490_lcw_jes',385,    'L1_J50.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
                                                                                        
        ['j260_320eta490',        386,    'L1_J75.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j260_320eta490_jes',    387,    'L1_J75.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j260_320eta490_lcw',    388,    'L1_J75.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j260_320eta490_lcw_jes',389,    'L1_J75.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j360_320eta490',        650,    'L1_J100.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_320eta490_jes',    651,    'L1_J100.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_320eta490_lcw',    652,    'L1_J100.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_320eta490_lcw_jes',653,    'L1_J100.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j85_nojcalib',       3001,    'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j85_lcw_nojcalib',        3002,    'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j175_nojcalib',           3003,    'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j175_lcw_nojcalib',        3004,    'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j380_nojcalib',           3005,    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j380_lcw_nojcalib',        3006,    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_nojcalib',           3007,    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j300_lcw_nojcalib',       3008,    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_nojcalib',           3009,    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_lcw_nojcalib',       3010,    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        #multi-jet
        ['4j85_nojcalib',           3011,    'L1_3J40', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['4j85_lcw_nojcalib',        3012,    'L1_3J40', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j85_nojcalib',            3013,    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j85_lcw_nojcalib',        3014,    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        #Forward jets
        ['j85_280eta320_nojcalib',     3015,    'L1_J20.28ETA32', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j85_280eta320_lcw_nojcalib', 3016,    'L1_J20.28ETA32', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j175_320eta490_nojcalib',    3017,    'L1_J50.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j175_320eta490_lcw_nojcalib',3018,    'L1_J50.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j260_320eta490_nojcalib',    3019,    'L1_J75.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j260_320eta490_lcw_nojcalib',3020,    'L1_J75.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_320eta490_nojcalib',    3021,    'L1_J100.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_320eta490_lcw_nojcalib',3022,    'L1_J100.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],


        
        ## Single jets seeded from RD0        
        ['j15',          660,    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j25',          661,    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j35',          662,    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j35_jes',      663,    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j35_lcw',      664,    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j35_lcw_jes',  665,    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j45_L1RD0',    666,    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j55_L1RD0',    667,    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j60_L1RD0',    668,    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j85_L1RD0',    669,    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['4j25',    670,    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j25',    671,    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['6j25',    672,    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],        
        ['7j25',    673,    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],      

        ['j35_nojcalib',      3023,    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j35_lcw_nojcalib',  3024,    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ##HT seeded chains
        ['j360_a10_nojcalib'                      ,   3025, 'L1_HT150-J20s5.ETA30',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_a10_nojcalib_L1HT150-AJ20all.ETA30',  3026, 'L1_HT150-AJ20all.ETA30', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_a10_lcw_nojcalib'                      ,  3027, 'L1_HT150-J20s5.ETA30',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_a10_lcw_nojcalib_L1HT150-AJ20all.ETA30',  3028, 'L1_HT150-AJ20all.ETA30', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j460_a10_nojcalib'                          ,  3029, 'L1_HT190-J15s5.ETA20',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j460_a10_nojcalib_L1HT190-AJ15all.ETA20'    ,  3030, 'L1_HT190-AJ15all.ETA20',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j460_a10_lcw_nojcalib'                          ,  3031, 'L1_HT190-J15s5.ETA20',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j460_a10_lcw_nojcalib_L1HT190-AJ15all.ETA20'    ,  3032, 'L1_HT190-AJ15all.ETA20',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
  


        ##HT seeded chains
        ['j360_a10_sub'                      ,  674, 'L1_HT150-J20s5.ETA30',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_a10_sub_L1HT150-AJ20all.ETA30',  675, 'L1_HT150-AJ20all.ETA30', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j360_a10_lcw_sub'                      ,  676, 'L1_HT150-J20s5.ETA30',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_a10_lcw_sub_L1HT150-AJ20all.ETA30',  677, 'L1_HT150-AJ20all.ETA30', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j460_a10_sub'                          ,  678, 'L1_HT190-J15s5.ETA20',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j460_a10_sub_L1HT190-AJ15all.ETA20'    ,  679, 'L1_HT190-AJ15all.ETA20',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j460_a10_lcw_sub'                          ,  680, 'L1_HT190-J15s5.ETA20',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j460_a10_lcw_sub_L1HT190-AJ15all.ETA20'    ,  681, 'L1_HT190-AJ15all.ETA20',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        
        ['ht400'                       ,  691, 'L1_HT150-J20s5.ETA30',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['ht400_L1HT150-AJ20all.ETA30' ,  692, 'L1_HT150-AJ20all.ETA30', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],

        ['ht550'                       ,  693, 'L1_HT150-J20s5.ETA30',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['ht550_L1HT150-AJ20all.ETA30' ,  694, 'L1_HT150-AJ20all.ETA30', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],

        ['ht700'                          ,  695, 'L1_HT190-J15s5.ETA20',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['ht700_L1HT190-AJ15all.ETA20'    ,  696, 'L1_HT190-AJ15all.ETA20',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],

        ['ht850'                          ,  697, 'L1_HT190-J15s5.ETA20',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['ht850_L1HT190-AJ15all.ETA20'    ,  698, 'L1_HT190-AJ15all.ETA20',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],

        ['ht1000'                          ,  699, 'L1_HT190-J15s5.ETA20',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['ht1000_L1HT190-AJ15all.ETA20'    ,  700, 'L1_HT190-AJ15all.ETA20',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ]

    TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [

        ### Run1 configuration
        # main menu
        ['2j75_bmedium_j75',	85, 'L1_3J25.0ETA22',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['2j70_btight_j70',	86, 'L1_3J25.0ETA22',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['j100_2j55_bmedium',	87, 'L1_J75_3J20',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], # not working
        ['2j55_bmedium_2j55',	88, 'L1_4J20',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['2j45_btight_2j45',	89, 'L1_4J20',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['j75_bmedium_3j75',	90, 'L1_4J20',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['j70_btight_3j70',	91, 'L1_4J20',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['j175_bmedium_j60_bmedium',	92, 'L1_J100',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['j300_bloose',	        93, 'L1_J100',	[], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        # low lumi menu
        ['2j70_bmedium_j70',	94, 'L1_3J25.0ETA22',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['2j65_btight_j65',	95, 'L1_3J25.0ETA22',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['2j45_bmedium_2j45_L14J15.0ETA24',  96, 'L1_4J15.0ETA24',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['2j35_btight_2j35_L14J15.0ETA24',   97, 'L1_4J15.0ETA24',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['j70_bmedium_3j70_L14J15.0ETA24',   98, 'L1_4J15.0ETA24',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['j65_btight_3j65_L14J15.0ETA24',    20, 'L1_4J15.0ETA24',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['2j45_bmedium_2j45_L13J25.0ETA22',  21, 'L1_3J25.0ETA22',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['2j35_btight_2j35_L13J25.0ETA22',   22, 'L1_3J25.0ETA22',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['j70_bmedium_3j70_L13J25.0ETA22',   23, 'L1_3J25.0ETA22',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['j65_btight_3j65_L13J25.0ETA22',    24	, 'L1_3J25.0ETA22',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['j225_bloose',	                     25, 'L1_J100',	[], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1], 
        ['j150_bmedium_j50_bmedium',	     26, 'L1_J100',	[], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1], 
        ['j175_bmedium',	             27, 'L1_J100',	[], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1], 

        # bperf chains
        ['j55_bperf',                        28,    'L1_J20',[],  [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j45_bperf_3j45',                   29,    'L1_3J15',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j45_bperf_3j45_L13J15.0ETA24',      30,    'L1_3J15.0ETA24',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j45_bperf_3j45_L13J20',            31,    'L1_3J20',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],

        # boffperf chains
        ['j55_boffperf',                     32,    'L1_J20',[],  [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j45_boffperf_3j45',                33,    'L1_3J15',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j45_boffperf_3j45_L13J15.0ETA24',   34,    'L1_3J15.0ETA24',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j45_boffperf_3j45_L13J20',         35,    'L1_3J20',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],

        ### split configuration
        # main menu
        ['2j75_bmedium_split_j75',             	   50, 'L1_3J25.0ETA22',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['2j70_btight_split_j70',                  51, 'L1_3J25.0ETA22',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['j100_2j55_bmedium_split',	           52, 'L1_J75_3J20',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], # not working
        ['2j55_bmedium_split_2j55',	           53, 'L1_4J20',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['2j45_btight_split_2j45',            	   54, 'L1_4J20',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['j75_bmedium_split_3j75',	           55, 'L1_4J20',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['j70_btight_split_3j70',	           56, 'L1_4J20',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['j175_bmedium_split_j60_bmedium_split',   61, 'L1_J100',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['j300_bloose_split',	                   62, 'L1_J100',	[], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        # low lumi menu
        ['2j70_bmedium_split_j70',	           63, 'L1_3J25.0ETA22',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['2j65_btight_split_j65',	           64, 'L1_3J25.0ETA22',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['2j45_bmedium_split_2j45_L14J15.0ETA24',  65, 'L1_4J15.0ETA24',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['2j35_btight_split_2j35_L14J15.0ETA24',   66, 'L1_4J15.0ETA24',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['j70_bmedium_split_3j70_L14J15.0ETA24',   67, 'L1_4J15.0ETA24',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['j65_btight_split_3j65_L14J15.0ETA24',	   68, 'L1_4J15.0ETA24',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['2j45_bmedium_split_2j45_L13J25.0ETA22',  69, 'L1_3J25.0ETA22',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['2j35_btight_split_2j35_L13J25.0ETA22',   70, 'L1_3J25.0ETA22',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['j70_bmedium_split_3j70_L13J25.0ETA22',   71, 'L1_3J25.0ETA22',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['j65_btight_split_3j65_L13J25.0ETA22',    72, 'L1_3J25.0ETA22',	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['j225_bloose_split',	                   73, 'L1_J100',  	[], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1], 
        ['j150_bmedium_split_j50_bmedium_split',   74, 'L1_J100',  	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 
        ['j175_bmedium_split',  	           75, 'L1_J100',  	[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1], 

        # bperf chains
        ['j55_bperf_split',                        76,    'L1_J20',[],  [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j45_bperf_split_3j45',                   77,    'L1_3J15',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j45_bperf_split_3j45_L13J150ETA24',      78,    'L1_3J15.0ETA24',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j45_bperf_split_3j45_L13J20',            79,    'L1_3J20',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],

        # boffperf chains
        ### ['j55_boffperf_split',                     80,    'L1_J20',[],  [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ### ['j45_boffperf_split_3j45',                81,    'L1_3J15',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ### ['j45_boffperf_split_3j45_L13J150ETA24',   82,    'L1_3J15.0ETA24',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ### ['j45_boffperf_split_3j45_L13J20',         83,    'L1_3J20',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
       
        ### the existing chains        
        ### ['j175_bmedium',                 15,    'L1_J100',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ### ['j75_bmedium_3j75',             16,    'L1_4J20',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ### ['2j55_bmedium_2j55',            17,    'L1_4J20',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ### ['2j45_bmedium_3j45',            18,    'L1_5J15.0ETA24',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ### ['j175_bmedium_j60_bmedium',     19,    'L1_J100',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],        
        ### ['j300_bloose',                   9,    'L1_J100',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ###
        ### ['j175_bmedium_split',                 36,    'L1_J100',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ### ['j75_bmedium_split_3j75',             37,    'L1_4J20',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ### ['2j55_bmedium_split_2j55',            38,    'L1_4J20',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ### ['2j45_bmedium_split_3j45',            39,    'L1_5J15.0ETA24',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ###['j175_bmedium_split_j60_bmedium_split',40,   'L1_J100',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ### ['j300_bloose_split',                   41,   'L1_J100',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        # split configuration:
        ### ['j55_bperf_split',                    32,    'L1_J20',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ### ['j45_bperf_split_3j45',               33,    'L1_3J15',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ### ['j45_bperf_split_3j45_L13J150ETA24',  34,    'L1_3J15.0ETA24',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ### ['j45_bperf_split_3j45_L13J20',        35,    'L1_3J20',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ]

    TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + [ 

        ##add muon corrections

        ['xe35_wEFMu',                     8050, 'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_tc_lcw_wEFMu',              8051, 'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_tc_em_wEFMu',               8052, 'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_mht_wEFMu',                 8053, 'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_pueta_wEFMu',               8054, 'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_pufit_wEFMu',               8055, 'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe60_wEFMu',                     8056, 'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_tc_lcw_wEFMu',              8057, 'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_tc_em_wEFMu',               8058, 'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_mht_wEFMu',                 8059, 'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_pueta_wEFMu',               8060, 'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_pufit_wEFMu',               8061, 'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe70_wEFMu',                     8062, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_tc_lcw_wEFMu',              8063, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_tc_em_wEFMu',               8064, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_mht_wEFMu',                 8065, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_pueta_wEFMu',               8066, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_pufit_wEFMu',               8067, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ####defaul chains

        ['xe35',                     520, 'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_tc_lcw',              521, 'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_tc_em',               522, 'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_mht',                 523, 'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_pueta',               524, 'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_pufit',               525, 'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe60',                     526, 'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_tc_lcw',              527, 'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_tc_em',               528, 'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_mht',                 529, 'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_pueta',               530, 'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_pufit',               531, 'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe70',                     532, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_tc_lcw',              533, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_tc_em',               534, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_mht',                 535, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_pueta',               536, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_pufit',               537, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe80_L1XE50',              538, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw_L1XE50',       539, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em_L1XE50',        540, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht_L1XE50',          541, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta_L1XE50',        542, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit_L1XE50',        543, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe80_L1XE50_wEFMu',        544, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw_L1XE50_wEFMu', 545, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em_L1XE50_wEFMu',  546, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht_L1XE50_wEFMu',    547, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta_L1XE50_wEFMu',  548, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit_L1XE50_wEFMu',  549, 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe80',                     550, 'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw',              551, 'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em',               552, 'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht',                 553, 'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta',               554, 'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit',               555, 'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe80_wEFMu',               556, 'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw_wEFMu',        557, 'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em_wEFMu',         558, 'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht_wEFMu',           559, 'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta_wEFMu',         560, 'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit_wEFMu',         561, 'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe80_L1XE70',              562, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw_L1XE70',       563, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em_L1XE70',        564, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht_L1XE70',          565, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta_L1XE70',        566, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit_L1XE70',        567, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe80_L1XE70_wEFMu',        568, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw_L1XE70_wEFMu', 569, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em_L1XE70_wEFMu',  570, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht_L1XE70_wEFMu',    571, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta_L1XE70_wEFMu',  572, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit_L1XE70_wEFMu',  573, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe100',                    574, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_lcw',             575, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_em',              576, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_mht',                577, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pueta',              578, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pufit',              579, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe100_wEFMu',              580, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_lcw_wEFMu',       581, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_em_wEFMu',        582, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_mht_wEFMu',          583, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pueta_wEFMu',        584, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pufit_wEFMu',        585, 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        # Rerun chains
        ['xe0noL1_l2fsperf',         586, '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_tc_lcw',  587, '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_tc_em',   588, '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_mht',     589, '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_pueta',   590, '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_pufit',   591, '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ]

    TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [
        ['tau20_r1medium1',              5123,          'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau20_r1perf',                 5124,          'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau20_r1_idperf',              5125,          'L1_TAU12', [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Tau'], -1],
        ['tau25_medium1_calo',           5126,          'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_track',          5127,          'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_tracktwo',       5151,          'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_ptonly',         5128,          'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_caloonly',       5129,          'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_trackonly',      5130,          'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_mvonly',         5131,          'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_r1perf',                 5132,          'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_r1perf_L1TAU6',          5133,          'L1_TAU6',  [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_mvonly_L1TAU6',  5134,          'L1_TAU6',  [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf',                   5135,          'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_L1TAU6',            5136,          'L1_TAU6',  [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_ptonly_idperf',          5137,          'L1_TAU12', [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Tau'], -1],
        ['tau29_r1medium1',              5138,          'L1_TAU20', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau29_r1perf',                 5139,          'L1_TAU20', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_calo',           5140,          'L1_TAU20', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_track',          5141,          'L1_TAU20', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_ptonly',         5142,          'L1_TAU20', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_perf',                   5143,          'L1_TAU20', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_medium1_calo',           5144,          'L1_TAU40', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_medium1_track',          5145,          'L1_TAU40', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_r1medium1',             5146,          'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_medium1_calo',          5147,          'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_medium1_track',         5148,          'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_r1perf',                5149,          'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_perf',                  5150,          'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        #['tau160_medium', 123, 'L1_TAU60',[], [PhysicsStream], [], -1]
        ]


    TriggerFlags.EgammaSlice.signatures = TriggerFlags.EgammaSlice.signatures() + [

        ##########        
        # Single electron triggers
        # Run2 cut-based:
        ['e24_medium_iloose_L1EM18VH',          180, 'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e24_medium_iloose_L1EM20VH',          181, 'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e24_tight_iloose_L1EM20VH',           182, 'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e24_tight_iloose',                    183, 'L1_EM20VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e26_tight_iloose',                    184, 'L1_EM22VHLIL', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e60_medium',                          185, 'L1_EM50V',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e120_loose',                          186, 'L1_EM50V',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e140_loose',                          187, 'L1_EM50V',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        
        # likelihood-based:
        ['e24_lhmedium_iloose_L1EM18VH',        188, 'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e24_lhmedium_iloose_L1EM20VH',        189, 'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e24_lhtight_iloose_L1EM20VH',         190, 'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e24_lhtight_iloose',                  191, 'L1_EM20VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e26_lhtight_iloose',                  192, 'L1_EM22VHLIL', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e60_lhmedium',                        193, 'L1_EM50V',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e120_lhloose',                        194, 'L1_EM50V',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e140_lhloose',                        195, 'L1_EM50V',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 

        # Supporting triggers
        # pairs of triggers (cut-based -- likelihood) should be in coherent PS
        ['e24_vloose_L1EM18VH',                 196, 'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e24_vloose_L1EM20VH',                 197, 'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e26_vloose_L1EM20VH',                 198, 'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e24_medium_L1EM18VH',                 199, 'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e24_tight_L1EM20VH',                  200, 'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 

        ['e24_lhvloose_L1EM18VH',               201, 'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],  
        ['e24_lhvloose_L1EM20VH',               202, 'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],  
        ['e26_lhvloose_L1EM20VH',               203, 'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],        
        ['e24_lhmedium_L1EM18VH',               204, 'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],  
        ['e24_lhtight_L1EM20VH',                205, 'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        #
        ['e24_medium_iloose_L2StarA_L1EM20VH',  206, 'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e24_medium_L2Star_idperf_L1EM20VH',   207, 'L1_EM20VH',    [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'], 1], 
        
        ['e24_lhmedium_iloose_L2StarA_L1EM20VH',208, 'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e24_lhmedium_L2Star_idperf_L1EM20VH', 209, 'L1_EM20VH',    [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'], 1], 
        #
        ['e5_loose',                            210, 'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e5_loose_L2StarA',                    211, 'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e5_loose_L2Star_idperf',              212, 'L1_EM3',       [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'], 1],
        ['e5_loose_idperf',                    1016, 'L1_EM3',       [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'], 1],
        ['e24_medium_idperf_L1EM20VH',         1017, 'L1_EM20VH',    [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'], 1], 
        
        ['e5_lhloose',                          213, 'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e5_lhloose_L2StarA',                  214, 'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e5_lhloose_L2Star_idperf',            215, 'L1_EM3',       [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'], 1],
        ['e5_lhloose_idperf',                  1018, 'L1_EM3',       [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'], 1],
        ['e24_lhmedium_idperf_L1EM20VH',       1019, 'L1_EM20VH',    [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'], 1], 
        #        
        ['e5_vloose',                           216, 'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e10_vloose_L1EM7',                    217, 'L1_EM7',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e15_vloose_L1EM7',                    218, 'L1_EM7',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e20_vloose_L1EM12',                   219, 'L1_EM12',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e25_vloose_L1EM15',                   220, 'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e30_vloose_L1EM15',                   221, 'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e40_vloose_L1EM15',                   222, 'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e50_vloose_L1EM15',                   223, 'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e60_vloose',                          224, 'L1_EM50V',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e70_vloose',                          225, 'L1_EM50V',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e80_vloose',                          226, 'L1_EM50V',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e100_vloose',                         227, 'L1_EM50V',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e120_vloose',                         228, 'L1_EM50V',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        
        ['e12_vloose_L1EM10VH',                 229, 'L1_EM10VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e15_vloose_L1EM13VH',                 230, 'L1_EM13VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e17_vloose',                          231, 'L1_EM15VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e20_vloose',                          232, 'L1_EM15VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        #        
        ['e5_lhvloose',                         233, 'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e10_lhvloose_L1EM7',                  234, 'L1_EM7',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e15_lhvloose_L1EM7',                  235, 'L1_EM7',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e20_lhvloose_L1EM12',                 236, 'L1_EM12',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e25_lhvloose_L1EM15',                 237, 'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e30_lhvloose_L1EM15',                 238, 'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e40_lhvloose_L1EM15',                 239, 'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e50_lhvloose_L1EM15',                 240, 'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e60_lhvloose',                        241, 'L1_EM50V',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e70_lhvloose',                        242, 'L1_EM50V',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e80_lhvloose',                        243, 'L1_EM50V',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e100_lhvloose',                       244, 'L1_EM50V',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e120_lhvloose',                       245, 'L1_EM50V',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 

        ['e12_lhvloose_L1EM10VH',               246, 'L1_EM10VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e15_lhvloose_L1EM13VH',               247, 'L1_EM13VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e17_lhvloose',                        248, 'L1_EM15VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e20_lhvloose',                        249, 'L1_EM15VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        # technical chains for forward electrons
        #['e17_loose_L1EM15VHJ1523ETA49',        250, 'L1_EM15VH_J15.23ETA49', ['L1_EM15VH','L1_J15.23ETA49'], [PhysicsStream], ['RATE:Combined', 'BW:Egamma'], 1], 
        #['e17_lhloose_L1EM15VHJ1523ETA49',      251, 'L1_EM15VH_J15.23ETA49', ['L1_EM15VH','L1_J15.23ETA49'], [PhysicsStream], ['RATE:Combined', 'BW:Egamma'], 1], 
        ['e17_loose_L1EM15VHJJ1523ETA49',        250, 'L1_EM15VH_JJ15.23ETA49', ['L1_EM15VH','L1_JJ15.23ETA49'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1], 
        ['e17_lhloose_L1EM15VHJJ1523ETA49',      251, 'L1_EM15VH_JJ15.23ETA49', ['L1_EM15VH','L1_JJ15.23ETA49'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1], 
        
        # Supporting trigger with PS
        # Perf chains to study L2/EF options
        ['e0_perf_L1EM3',                  952, 'L1_EM3', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e0_L2Star_perf_L1EM3',           953, 'L1_EM3', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        
        ['e0_perf_L1EM15',                 138, 'L1_EM15',[], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e0_L2Star_perf_L1EM15',          959, 'L1_EM15',[], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],         
        
        # Et cut only chains
        ['e5_etcut',                       948,          'L1_EM3',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e10_etcut_L1EM7',                130,          'L1_EM7',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e15_etcut_L1EM7',                131,          'L1_EM7',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e20_etcut_L1EM12',               132,          'L1_EM12',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],         
        ['e25_etcut_L1EM15',               122,          'L1_EM15',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e30_etcut_L1EM15',               123,          'L1_EM15',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e40_etcut_L1EM15',               124,          'L1_EM15',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e50_etcut_L1EM15',               125,          'L1_EM15',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e60_etcut',                      126,          'L1_EM50V',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e80_etcut',                      127,          'L1_EM50V',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e100_etcut',                     128,          'L1_EM50V',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e120_etcut',                     129,          'L1_EM50V',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        
        # Rerun mode
        ['e4_etcut',                       167,          'L1_EM3',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e9_etcut',                       168,          'L1_EM7',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e14_etcut',                      169,          'L1_EM12',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], 

        # Rerun mode, cut-based
        ['e9_medium',                      133,          'L1_EM7',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e12_loose',                      134,          'L1_EM8VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e12_loose_L1EM10VH',             135,          'L1_EM8VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e15_loose_L1EM13VH',             136,          'L1_EM13VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e17_loose_L1EM15',               137,          'L1_EM15',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e17_loose',                      160,          'L1_EM15VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e17_medium',                     161,          'L1_EM15VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e17_medium_iloose',              162,          'L1_EM15VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e20_medium',                     163,          'L1_EM15VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e24_medium_L1EM15VH',            164,          'L1_EM15VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e60_loose',                      165,          'L1_EM50V',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e70_loose',                      166,          'L1_EM50V',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e5_tight',                       140,          'L1_EM3',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e9_tight',                       141,          'L1_EM7',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e14_tight',                      142,          'L1_EM12',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],

        # Rerun mode, likelihood
        ['e9_lhmedium',                    143,          'L1_EM7',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e12_lhloose',                    144,          'L1_EM8VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e12_lhloose_L1EM10VH',           145,          'L1_EM10VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e15_lhloose_L1EM13VH',           146,          'L1_EM13VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e17_lhloose_L1EM15',             147,          'L1_EM15',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e17_lhloose',                    148,          'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e17_lhmedium',                   149,          'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e17_lhmedium_iloose',            150,          'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e20_lhmedium',                   151,          'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e24_lhmedium_L1EM15VH',          152,          'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e60_lhloose',                    153,          'L1_EM50V',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e70_lhloose',                    154,          'L1_EM50V',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e5_lhtight',                     155,          'L1_EM3',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e9_lhtight',                     156,          'L1_EM7',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e14_lhtight',                    157,          'L1_EM12',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],

        # Single electron/photon chains for Calo sequence optimization (kept as before)
        ['e24_tight_iloose_L2EFCalo_L1EM20VH',     390, 'L1_EM20VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e24_lhtight_iloose_L2EFCalo_L1EM20VH',   391, 'L1_EM20VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e24_tight_iloose_HLTCalo_L1EM20VH',      392, 'L1_EM20VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e24_lhtight_iloose_HLTCalo_L1EM20VH',    393, 'L1_EM20VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e24_tight_iloose_etisem_L1EM20VH',       394, 'L1_EM20VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['g120_loose_HLTCalo',                     395, 'L1_EM50V',    [], [PhysicsStream], ['RATE:SinglePhoton','BW:Egamma'], 1],
        ['g35_medium_HLTCalo_g25_medium_HLTCalo',  396, 'L1_2EM15VH',  [], [PhysicsStream], ['RATE:MultiPhoton','BW:Egamma'], 1],
        
        #Single electron/chaing for testing mis-alignment robust LH ID
        ['e24_lhmedium_nod0_L1EM18VH',             866, 'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e24_lhmedium_nodphi_L1EM18VH',           867, 'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e24_lhmedium_nodphires_L1EM18VH',        868, 'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e24_lhmedium_cutd0dphi_L1EM18VH',        869, 'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e24_lhmedium_nod0_iloose_L1EM18VH',             870, 'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e24_lhmedium_nodphi_iloose_L1EM18VH',           871, 'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e24_lhmedium_nodphires_iloose_L1EM18VH',        872, 'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e24_lhmedium_cutd0dphi_iloose_L1EM18VH',        873, 'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        

        # low-ET single electron triggers in rerun mode
        ### NOT IN THE LIST ['e5_lhtight_idperf',           151, 'L1_EM3',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ### NOT IN THE LIST ['e5_lhtight_L2Star_idperf',    152, 'L1_EM3',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ### NOT IN THE LIST ['e5_tight1_idperf',            153, 'L1_EM3',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ### NOT IN THE LIST ['e5_tight1_L2Star_idperf',     154, 'L1_EM3',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        
        # Tracking tune chains for Run1 PID
        ### NOT IN THE LIST ['e24_tight1_iloose_L2StarA_L1EM18VH',        171, 'L1_EM18VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e24_tight1_iloose_L2StarB_L1EM18VH',        172, 'L1_EM18VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e24_tight1_iloose_L2StarC_L1EM18VH',        173, 'L1_EM18VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e24_tight1_L2Star_idperf_L1EM18VH',         174, 'L1_EM18VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e24_tight1_idperf_L1EM18VH',                175, 'L1_EM18VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],

        # Rerun mode triggers to support LH PID chains
        ### NOT IN THE LIST ['e18_lhloose',           176,          'L1_EM15VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e12_lhloose_L1EM3',     178,          'L1_EM3', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e9_lhloose',            179,          'L1_EM7', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e24_lhloose_iloose',    180,          'L1_EM20VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],        

        # Cut-based Run1 PID chains
        # Loose chains with etcut variations
        ### NOT IN THE LIST ###### ['e5_loose1',               130,          'L1_EM3', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1], # commented because it's in physics
        ### NOT IN THE LIST ['e15_loose1',              131,          'L1_EM7', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e25_loose1',              132,          'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e35_loose1',              133,          'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e45_loose1',              134,          'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e55_loose1',              135,          'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e65_loose1',              136,          'L1_EM50V', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e80_loose1',              137,          'L1_EM50V', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e100_loose1',             138,          'L1_EM50V', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e120_loose1',                     139, 'L1_EM50V', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        
        # Single electron Run1 PID
        ['e24_medium1_iloose_L1EM18VH',     770, 'L1_EM18VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e24_loose1_L1EM18VH',              141, 'L1_EM18VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e24_medium1_L1EM18VH',            771, 'L1_EM18VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e24_tight1_L1EM18VH',              143, 'L1_EM18VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e24_tight1_iloose_L1EM18VH',       144, 'L1_EM18VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e24_tight1_iloose_L1EM20VH',      772, 'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e24_tight1_iloose',               773, 'L1_EM20VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e26_tight1_iloose',               774, 'L1_EM22VHLIL', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e60_medium1',                     775, 'L1_EM50V', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e60_loose1',                       149, 'L1_EM50V', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],

        # Rerun mode triggers to support Run1 PID chains
        ### NOT IN THE LIST ['e18_loose1',           155,          'L1_EM15VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e17_loose1',           156,          'L1_EM15VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e12_loose1_L1EM3',     157,          'L1_EM3', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e9_loose1',            158,          'L1_EM7', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e7_medium1',           159,          'L1_EM3', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e12_medium1_L1EM3',    160,          'L1_EM3', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e17_medium1',          161,          'L1_EM15VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e5_tight1',            162,          'L1_EM3', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e9_tight1',            163,          'L1_EM7', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e14_tight1',           164,          'L1_EM12', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e24_loose1_iloose',    168,          'L1_EM20VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e24_medium1_L1EM15VH', 169,          'L1_EM15VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        
        ##########
        # Di-electron triggers
        # cut-based
        ['2e12_loose_L12EM10VH',             170, 'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1], 
        ['2e15_loose_L12EM13VH',             171, 'L1_2EM13VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1], 
        ['2e17_loose_L12EM15',               172, 'L1_2EM15',   [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1], 
        ['2e17_loose',                       776, 'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        
        # likelihood
        ['2e12_lhloose_L12EM10VH',           173, 'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1], 
        ['2e15_lhloose_L12EM13VH',           174, 'L1_2EM13VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1], 
        ['2e17_lhloose',                     175, 'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1], 
        ['2e17_lhloose_L12EM15',             176, 'L1_2EM15',   [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1], 

        # Tri-electron triggers
        ['e17_medium_2e9_medium',            800, 'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],  
        ['e17_loose_2e9_loose',              801, 'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],  
        ['e17_lhloose_2e9_lhloose',          802, 'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],  
        ['e17_medium_iloose_2e9_medium',     803, 'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],  
        ['e17_lhmedium_2e9_lhmedium',        804, 'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],  
        ['e17_lhmedium_iloose_2e9_lhmedium', 805, 'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],  
        
        # Multi-electron chains for Run1 PID
        ['2e12_loose1_L12EM10VH',             777, 'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['2e17_loose1',                       778, 'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['2e17_loose1_L12EM15',               206, 'L1_2EM15', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e24_loose1_iloose_e12_loose1_L1EM3',207, 'L1_EM20VHI', ['L1_EM20VHI','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e18_loose1_2e9_loose1',             208, 'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        # likelihood
        ### NOT IN THE LIST ['e24_lhloose_iloose_e12_lhloose_L1EM3',202, 'L1_EM20VHI', ['L1_EM20VHI','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e18_lhloose_2e9_lhloose',             203, 'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        
        ##########
        # Multielectron chains with m_ee cut for Run2 PID (supporting L1Topo J/psi)
        # cut-based
        ['e5_tight_e4_etcut',                           825,      'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['e5_tight_e4_etcut_Jpsiee',                    826,      'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['e9_tight_e4_etcut_Jpsiee',                    827,      'L1_EM7_2EM3',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1], 
        ['e9_etcut_e5_tight_Jpsiee',                    828,      'L1_EM7_2EM3',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],         
        ['e14_tight_e4_etcut_Jpsiee',                   829,      'L1_EM12_2EM3', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['e14_etcut_e5_tight_Jpsiee',                   830,      'L1_EM12_2EM3', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        
        # likelihood
        ['e5_lhtight_e4_etcut',                         831,      'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['e5_lhtight_e4_etcut_Jpsiee',                  832,      'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['e9_lhtight_e4_etcut_Jpsiee',                  833,      'L1_EM7_2EM3',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['e9_etcut_e5_lhtight_Jpsiee',                  834,      'L1_EM7_2EM3',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1], 
        ['e14_lhtight_e4_etcut_Jpsiee',                 835,      'L1_EM12_2EM3', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['e14_etcut_e5_lhtight_Jpsiee',                 818,      'L1_EM12_2EM3', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],                
        ### NOT IN THE LIST ['e5_lhtight_L2StarA_e4_etcut_L2StarA_Jpsiee',  830,      'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e5_lhtight_L2StarB_e4_etcut_L2StarB_Jpsiee',  831,      'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e5_lhtight_L2StarC_e4_etcut_L2StarC_Jpsiee',  832,      'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e5_lhtight_L2Star_idperf_e4_etcut_L2Star_idperf_Jpsiee', 833,      'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e5_lhtight_idperf_e4_etcut_idperf_Jpsiee',    834,      'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        
        # Multielectron chains with m_ee cut for Run1 PID
        ['e5_tight1_e4_etcut',                          816,      'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['e5_tight1_e4_etcut_Jpsiee',                   817,      'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e5_tight1_L2StarA_e4_etcut_L2StarA_Jpsiee',   818,      'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e5_tight1_L2StarB_e4_etcut_L2StarB_Jpsiee',   819,      'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e5_tight1_L2StarC_e4_etcut_L2StarC_Jpsiee',   820,      'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e5_tight1_L2Star_idperf_e4_etcut_L2Star_idperf_Jpsiee', 821,      'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e5_tight1_idperf_e4_etcut_idperf_Jpsiee',     823,      'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e9_tight1_e4_etcut_Jpsiee',                   825,      'L1_EM7_2EM3',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e9_etcut_e5_tight1_Jpsiee',                   826,      'L1_EM7_2EM3',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e14_tight1_e4_etcut_Jpsiee',                  827,      'L1_EM12_2EM3', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['e14_etcut_e5_tight1_Jpsiee',                  828,      'L1_EM12_2EM3', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],

        ##########
        # L1Topo chains (kept as before)
        ### TOPO ['e5_tight1_e4_etcut_L11INVM5-EMs2-EMall',           590, 'L1_1INVM5-EMs2-EMall', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ### TOPO ['e5_tight1_e4_etcut_Jpsiee_L11INVM5-EMs2-EMall',    591, 'L1_1INVM5-EMs2-EMall', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ### TOPO ['e9_tight1_e4_etcut_Jpsiee_L11INVM5-EM7s2-EMall',   592, 'L1_1INVM5-EM7s2-EMall', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ### TOPO ['e9_etcut_e5_tight1_Jpsiee_L11INVM5-EM7s2-EMall',   593, 'L1_1INVM5-EM7s2-EMall', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ### TOPO ['e14_tight1_e4_etcut_Jpsiee_L11INVM5-EM12s2-EMall', 594, 'L1_1INVM5-EM12s2-EMall', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ### TOPO ['e14_etcut_e5_tight1_Jpsiee_L11INVM5-EM12s2-EMall', 595, 'L1_1INVM5-EM12s2-EMall', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'], 1],

        # W T&P triggers
        ['e5_etcut_L110MINDPHI-AJj15s2-XE0', 600, 'L1_10MINDPHI-AJj15s2-XE0', ['L1_EM3'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e5_etcut_L120MINDPHI-AJjs6-XE0',   601, 'L1_20MINDPHI-AJjs6-XE0', ['L1_EM3'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e5_etcut_L120MINDPHI-AJj15s2-XE0', 602, 'L1_20MINDPHI-AJj15s2-XE0', ['L1_EM3'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### TOPO ['e5_etcut_L110MINDPHI-EM6s1-XE0',   603, 'L1_10MINDPHI-EM6s1-XE0', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### TOPO ['e5_etcut_L120MINDPHI-EM9s6-XE0',   604, 'L1_20MINDPHI-EM9s6-XE0', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ### TOPO ['e5_etcut_L120MINDPHI-EM6s1-XE0',   605, 'L1_20MINDPHI-EM6s1-XE0', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e5_etcut_L105RATIO-XE0-HT0-AJj15all.ETA49',   606, 'L1_05RATIO-XE0-HT0-AJj15all.ETA49', ['L1_EM3'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e5_etcut_L108RATIO-XE0-HT0-AJj0all.ETA49',    607, 'L1_08RATIO-XE0-HT0-AJj0all.ETA49', ['L1_EM3'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e5_etcut_L140RATIO2-XE0-HT0-AJj15all.ETA49',  608, 'L1_40RATIO2-XE0-HT0-AJj15all.ETA49', ['L1_EM3'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e5_etcut_L190RATIO2-XE0-HT0-AJj0all.ETA49',   609, 'L1_90RATIO2-XE0-HT0-AJj0all.ETA49', ['L1_EM3'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e5_etcut_L1HT20-AJj0all.ETA49',               610, 'L1_HT20-AJj0all.ETA49', ['L1_EM3'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e5_etcut_L1NOT-02MATCH-EM9s1-AJj15all.ETA49', 611, 'L1_NOT-02MATCH-EM9s1-AJj15all.ETA49', ['L1_EM3'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e5_etcut_L1NOT-02MATCH-EM9s1-AJj15all.ETA49_05RATIO-XE0-SUM0-EM9s1-HT0-AJj15all.ETA49', 612, 'L1_NOT-02MATCH-EM9s1-AJj15all.ETA49_05RATIO-XE0-SUM0-EM9s1-HT0-AJj15all.ETA49', ['L1_EM3'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],

        ##########
        # Single photon triggers        
        ['g120_loose',             296,          'L1_EM50V', [], [PhysicsStream], ['RATE:SinglePhoton','BW:Egamma'], 1], 
        ['g140_loose',             297,          'L1_EM50V', [], [PhysicsStream], ['RATE:SinglePhoton','BW:Egamma'], 1],         

        # Supporting triggers with PS
        ['g0_perf_L1EM3',          954,          'L1_EM3',   [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],     
        ['g0_perf_L1EM15',         298,          'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1], 

        # Rerun mode trigger to support combined chains
        ['g20_etcut_L1EM12',       299,          'L1_EM12',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1], 
        ['g15_loose_L1EM7',        260,          'L1_EM7',   [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g20_loose_L1EM12',       261,          'L1_EM12',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], # pass through
        ['g40_loose_L1EM15',       262,          'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g45_loose_L1EM15',       263,          'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g50_loose_L1EM15',       264,          'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g80_loose',              265,          'L1_EM50V', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g100_loose',             266,          'L1_EM50V', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 

        # Rerun mode and PS
        ['g10_loose_L1EM7',        267,          'L1_EM7',   [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ['g25_loose_L1EM15',       268,          'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ['g35_loose_L1EM15',       269,          'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ['g60_loose',              270,          'L1_EM50V', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ['g70_loose',              271,          'L1_EM50V', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],

        # Rerun mode
        ['g10_medium_L1EM7',       252,          'L1_EM7',   [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ['g10_loose',              253,          'L1_EM8VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ['g10_medium',             254,          'L1_EM8VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ['g15_loose_L1EM3',        256,          'L1_EM3',   [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ['g15_loose',              257,          'L1_EM13VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ['g20_loose_L1EM15',       258,          'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ['g20_loose',              259,          'L1_EM15VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ['g20_tight',              273,          'L1_EM15VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ['g25_loose',              274,          'L1_EM15VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ['g25_medium',             275,          'L1_EM15VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ['g35_loose',              276,          'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ['g35_medium',             277,          'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ['g40_tight',              278,          'L1_EM20VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ['g45_tight',              279,          'L1_EM22VHLIL', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ['g50_loose',              280,          'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ['g60_loose_L1EM15VH',     281,          'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        
        # Et cut only photon chains (kept as before)
        #['g12_etcut_L1EM7',         230,          'L1_EM7', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['g25_etcut_L1EM15',        232,          'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['g35_etcut_L1EM15',        233,          'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['g40_etcut',               234,          'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['g50_etcut',               235,          'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['g60_etcut',               236,          'L1_EM50V', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['g80_etcut',               237,          'L1_EM50V', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['g100_etcut',              238,          'L1_EM50V', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['g120_etcut',              239,          'L1_EM50V', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        
        # Photon cut-based Run1 PID chains
        # Loose chains for low ET physics and background estimation
        ### NOT IN THE LIST ['g12_loose1_L1EM7',         240,          'L1_EM7', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['g20_loose1_L1EM12',        241,          'L1_EM12', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ##['g25_loose1_L1EM15',        242,          'L1_EM15', [], ['globalStream'], ['RATE:SinglePhoton', 'BW:Egamma'], 1], #duplicate, need to make sure prescales / rerun set properly
        ### NOT IN THE LIST ## ['g35_loose1_L1EM15',        243,          'L1_EM15', [], ['globalStream'], ['RATE:SinglePhoton', 'BW:Egamma'], 1], #duplicate, need to make sure prescales / rerun set properly
        ### NOT IN THE LIST ['g40_loose1',               244,          'L1_EM15', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['g50_loose1',               245,          'L1_EM15', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['g60_loose1',               246,          'L1_EM50V', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['g80_loose1',               247,          'L1_EM50V', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['g100_loose1',              248,          'L1_EM50V', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        
        # Run 2 Baseline chains
        ### NOT IN THE LIST ['g120_loose1',              250,          'L1_EM50V', [], [PhysicsStream], ['RATE:SinglePhoton','BW:Egamma'], 1],
        ### NOT IN THE LIST ['g140_loose1',              251,          'L1_EM50V', [], [PhysicsStream], ['RATE:SinglePhoton','BW:Egamma'], 1],

        ### NOT IN THE LIST ['g4_etcut',                 260,          'L1_EM3', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['g8_etcut',                 261,          'L1_EM3', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ### NOT IN THE LIST #['g9_etcut',                 263,          'L1_EM7', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ### NOT IN THE LIST #['g14_etcut',                264,          'L1_EM12', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ### NOT IN THE LIST #['g9_etcut_L1EM3',           265,          'L1_EM3', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ### NOT IN THE LIST #['g14_etcut_L1EM7',          266,          'L1_EM7', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['g25_loose1_L1EM15',        265,          'L1_EM15', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['g25_medium1_L1EM3',        266,          'L1_EM3', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['g35_loose1_L1EM15',        267,          'L1_EM15', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['g35_medium1',              268,          'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['g12_loose1',               269,          'L1_EM8VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['g20_loose1',               270,          'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ### NOT IN THE LIST ['g20_tight1',               271,          'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
                
        ##########        
        # Di-photon triggers
        ['g35_loose_L1EM15_g25_loose_L1EM15',      310,    'L1_2EM15', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'], 1], 
        ['g35_loose_g25_loose',                    311,    'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'], 1], 
        ['g35_medium_g25_medium',                  312,    'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'], 1], 
        ['2g20_loose_L12EM15',                     313,    'L1_2EM15', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'], 1], 
        ['2g20_tight',                             314,    'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'], 1], 
        ['2g50_loose',                             315,    'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'], 1], 
        ['2g60_loose_L12EM15VH',                   316,    'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'], 1],  

        # Tri-photon triggers
        ['3g15_loose',                             317,    'L1_2EM13VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'], 1], 
        ['g20_loose_2g15_loose_L12EM13VH',         318,    'L1_2EM13VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'], 1], 
        ['2g20_loose_g15_loose',                   319,    'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'], 1], 
        ['3g20_loose',                             320,    'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'], 1], 

        # multi-photon chains
        ### NOT IN THE LIST ['g35_loose1_g25_loose1',    252, 'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton','BW:Egamma'], 1],
        ### NOT IN THE LIST ['g35_medium1_g25_medium1',  253, 'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton','BW:Egamma'], 1],
        ### NOT IN THE LIST ['2g20_tight1',              254, 'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton','BW:Egamma'], 1],
        ### NOT IN THE LIST ['3g20_loose1',              255, 'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton','BW:Egamma'], 1],
        ### NOT IN THE LIST ['g35_loose1_g25_loose1_L12EM15',    256, 'L1_2EM15', [], [PhysicsStream], ['RATE:MultiPhoton','BW:Egamma'], 1],

        ##########        
        # Monopole triggers
        ['g0_hiptrt_L1EM18VH',                    1004, 'L1_EM18VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g0_hiptrt_L1EM20VH',                    1005, 'L1_EM20VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g0_hiptrt_L1EM20VHI',                   1006, 'L1_EM20VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g0_hiptrt_L1EM22VHLIL',                 1007, 'L1_EM22VHLIL', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g0_hiptrt_L1EM50V',                     1008, 'L1_EM50V', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 

        # Fall-back if problem with new tracking  (kept as before)
        ['e17_etcut_L1EM15',                       290, 'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e17_etcut_L2StarA_L1EM15',               291, 'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e17_etcut_L2StarB_L1EM15',               292, 'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e17_etcut_L2StarC_L1EM15',               293, 'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e17_etcut_L2Star_idperf_L1EM15',         294, 'L1_EM15', [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'], 1],
        ['e17_etcut_idperf_L1EM15',                295, 'L1_EM15', [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'], 1],
        
        # extra id perf chains for HLT ID group (kept as before)
        ['e24_medium1_L2Star_idperf_L1EM18VH',     285, 'L1_EM18VH', [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],1],
        ['e5_loose1_L2Star_idperf',                286, 'L1_EM3',    [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],1],
        ['e24_medium1_idperf_L1EM18VH',            287, 'L1_EM18VH', [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],1],
        ['e5_loose1_idperf',                       288, 'L1_EM3',    [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],1],
        ['e24_medium1_iloose_L2StarA_L1EM18VH',    289, 'L1_EM18VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],1],
        
        ]


    ###########################################################################################################
    #   Bphysics
    ###########################################################################################################
    TriggerFlags.BphysicsSlice.signatures = TriggerFlags.BphysicsSlice.signatures() + [

        #['2mu6_bMuMu_L12MU6', 299, 'L1_2MU6', [], [PhysicsStream], [], -1], 

        ###########  2MU4  ################
        ['2mu4_bJpsimumu',                                            400, 'L1_2MU4',                         [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_bBmumu',                                               401, 'L1_2MU4',                         [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_bUpsimumu',                                            402, 'L1_2MU4',                         [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_bBmumuxv2',                                            403, 'L1_2MU4',                         [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_bBmumux_BcmumuDsloose',                                404, 'L1_2MU4',                         [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        # in Physics menu ['2mu4_bDimu',                                                405, 'L1_2MU4',                         [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        # in Physics menu ['2mu4_bDimu_novtx_noos',                                     406, 'L1_2MU4',                         [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bJpsimumu_L12DR15-2MU4ab',                             407, 'L1_2DR15-2MU4ab',                 [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bBmumu_L12DR15-2MU4ab',                                408, 'L1_2DR15-2MU4ab',                 [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bBmumuxv2_L12DR15-2MU4ab',                             409, 'L1_2DR15-2MU4ab',                 [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bBmumux_BcmumuDsloose_L12DR15-2MU4ab',                 410, 'L1_2DR15-2MU4ab',                 [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bDimu_L12DR15-2MU4ab',                                 411, 'L1_2DR15-2MU4ab',                 [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bDimu_novtx_noos_L12DR15-2MU4ab',                      412, 'L1_2DR15-2MU4ab',                 [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bJpsimumu_L12INVM999-2MU4ab',                          413, 'L1_2INVM999-2MU4ab',              [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bBmumu_L12INVM999-2MU4ab',                             414, 'L1_2INVM999-2MU4ab',              [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bUpsimumu_L12INVM999-2MU4ab',                          415, 'L1_2INVM999-2MU4ab',              [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bBmumuxv2_L12INVM999-2MU4ab',                          416, 'L1_2INVM999-2MU4ab',              [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bBmumux_BcmumuDsloose_L12INVM999-2MU4ab',              417, 'L1_2INVM999-2MU4ab',              [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bDimu_L12INVM999-2MU4ab',                              418, 'L1_2INVM999-2MU4ab',              [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bDimu_novtx_noos_L12INVM999-2MU4ab',                   419, 'L1_2INVM999-2MU4ab',              [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bBmumu_L14INVM8-2MU4ab',                               420, 'L1_4INVM8-2MU4ab',                [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bBmumuxv2_L14INVM8-2MU4ab',                            421, 'L1_4INVM8-2MU4ab',                [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bBmumux_BcmumuDsloose_L14INVM8-2MU4ab',                422, 'L1_4INVM8-2MU4ab',                [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bDimu_L14INVM8-2MU4ab',                                423, 'L1_4INVM8-2MU4ab',                [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bDimu_novtx_noos_L14INVM8-2MU4ab',                     424, 'L1_4INVM8-2MU4ab',                [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
       
        ### TOPO ['2mu4_bJpsimumu_L12DR15-2MU4ab-2INVM999-2MU4ab',             425, 'L1_2DR15-2MU4ab_2INVM999-2MU4ab', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bBmumu_L12DR15-2MU4ab-2INVM999-2MU4ab',                426, 'L1_2DR15-2MU4ab_2INVM999-2MU4ab', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bBmumuxv2_L12DR15-2MU4ab-2INVM999-2MU4ab',             427, 'L1_2DR15-2MU4ab_2INVM999-2MU4ab', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bBmumux_BcmumuDsloose_L12DR15-2MU4ab-2INVM999-2MU4ab', 428, 'L1_2DR15-2MU4ab_2INVM999-2MU4ab', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bDimu_L12DR15-2MU4ab-2INVM999-2MU4ab',                 429, 'L1_2DR15-2MU4ab_2INVM999-2MU4ab', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bDimu_novtx_noos_L12DR15-2MU4ab-2INVM999-2MU4ab',      430, 'L1_2DR15-2MU4ab_2INVM999-2MU4ab', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bBmumu_L12DR15-2MU4ab-4INVM8-2MU4ab',                  431, 'L1_2DR15-2MU4ab_4INVM8-2MU4ab',   [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bBmumuxv2_L12DR15-2MU4ab-4INVM8-2MU4ab',               432, 'L1_2DR15-2MU4ab_4INVM8-2MU4ab',   [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bBmumux_BcmumuDsloose_L12DR15-2MU4ab-4INVM8-2MU4ab',   433, 'L1_2DR15-2MU4ab_4INVM8-2MU4ab',   [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bDimu_L12DR15-2MU4ab-4INVM8-2MU4ab',                   434, 'L1_2DR15-2MU4ab_4INVM8-2MU4ab',   [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu4_bDimu_novtx_noos_L12DR15-2MU4ab-4INVM8-2MU4ab',        435, 'L1_2DR15-2MU4ab_4INVM8-2MU4ab',   [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        #L1_2MU4 seeded
        ['mu4_iloose_mu4_7invm9_noos', 335, 'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu4_mu4_idperf_bJpsimumu_noid',          340, 'L1_2MU4', [], [PhysicsStream], ['RATE:IDMonitoring','BW:Bphys'], -1],
        ['mu4_iloose_mu4_11invm60_noos', 436, 'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],


        ##########


        ###########  3MU4  ################
        ['3mu4_bDimu', 341, 'L1_3MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['3mu4_bTau',  516, 'L1_3MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['3mu4_bJpsi', 517, 'L1_3MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['3mu4_bUpsi', 518, 'L1_3MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],


        ######## L1_MU6 seeded#####
        ['mu6_bJpsi_Trkloose',         519, 'L1_MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],


        #######  L1_2MU6 #######
        #in Physics menu ['2mu6_bDimu', 440,             'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1], 
        #n Physics menu  ['2mu6_bDimu_novtx_noos', 441, 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],  
        # not set up yet? ['mu6_iloose_mu6_18invm60_noos', 332, 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bBmumu',                 303, 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bBmumuxv2',              304, 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bBmumux_BcmumuDsloose',  336, 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bJpsimumu',             305, 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bUpsimumu',             306, 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        ### TOPO ['2mu6_bJpsimumu_L12DR15-2MU6ab',                             472, 'L1_2DR15-2MU6ab',                 [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bBmumu_L12DR15-2MU6ab',                                473, 'L1_2DR15-2MU6ab',                 [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bBmumuxv2_L12DR15-2MU6ab',                             474, 'L1_2DR15-2MU6ab',                 [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bBmumux_BcmumuDsloose_L12DR15-2MU6ab',                 475, 'L1_2DR15-2MU6ab',                 [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bDimu_L12DR15-2MU6ab',                                 476, 'L1_2DR15-2MU6ab',                 [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bDimu_novtx_noos_L12DR15-2MU6ab',                      477, 'L1_2DR15-2MU6ab',                 [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bJpsimumu_L12INVM999-2MU6ab',                          478, 'L1_2INVM999-2MU6ab',              [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bBmumu_L12INVM999-2MU6ab',                             479, 'L1_2INVM999-2MU6ab',              [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bUpsimumu_L12INVM999-2MU6ab',                          480, 'L1_2INVM999-2MU6ab',              [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bBmumuxv2_L12INVM999-2MU6ab',                          481, 'L1_2INVM999-2MU6ab',              [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bBmumux_BcmumuDsloose_L12INVM999-2MU6ab',              482, 'L1_2INVM999-2MU6ab',              [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bDimu_L12INVM999-2MU6ab',                              483, 'L1_2INVM999-2MU6ab',              [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bDimu_novtx_noos_L12INVM999-2MU6ab',                   484, 'L1_2INVM999-2MU6ab',              [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bBmumu_L14INVM8-2MU6ab',                               485, 'L1_4INVM8-2MU6ab',                [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bBmumuxv2_L14INVM8-2MU6ab',                            486, 'L1_4INVM8-2MU6ab',                [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bBmumux_BcmumuDsloose_L14INVM8-2MU6ab',                487, 'L1_4INVM8-2MU6ab',                [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bDimu_L14INVM8-2MU6ab',                                488, 'L1_4INVM8-2MU6ab',                [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bDimu_novtx_noos_L14INVM8-2MU6ab',                     489, 'L1_4INVM8-2MU6ab',                [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        
        ### TOPO ['2mu6_bJpsimumu_L12DR15-2MU6ab-2INVM999-2MU6ab',             490, 'L1_2DR15-2MU6ab_2INVM999-2MU6ab', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bBmumu_L12DR15-2MU6ab-2INVM999-2MU6ab',                491, 'L1_2DR15-2MU6ab_2INVM999-2MU6ab', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bBmumuxv2_L12DR15-2MU6ab-2INVM999-2MU6ab',             492, 'L1_2DR15-2MU6ab_2INVM999-2MU6ab', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bBmumux_BcmumuDsloose_L12DR15-2MU6ab-2INVM999-2MU6ab', 493, 'L1_2DR15-2MU6ab_2INVM999-2MU6ab', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bDimu_L12DR15-2MU6ab-2INVM999-2MU6ab',                 494, 'L1_2DR15-2MU6ab_2INVM999-2MU6ab', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bDimu_novtx_noos_L12DR15-2MU6ab-2INVM999-2MU6ab',      495, 'L1_2DR15-2MU6ab_2INVM999-2MU6ab', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bBmumu_L12DR15-2MU6ab-4INVM8-2MU6ab',                  496, 'L1_2DR15-2MU6ab_4INVM8-2MU6ab',   [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bBmumuxv2_L12DR15-2MU6ab-4INVM8-2MU6ab',               497, 'L1_2DR15-2MU6ab_4INVM8-2MU6ab',   [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bBmumux_BcmumuDsloose_L12DR15-2MU6ab-4INVM8-2MU6ab',   498, 'L1_2DR15-2MU6ab_4INVM8-2MU6ab',   [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bDimu_L12DR15-2MU6ab-4INVM8-2MU6ab',                   499, 'L1_2DR15-2MU6ab_4INVM8-2MU6ab',   [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ### TOPO ['2mu6_bDimu_novtx_noos_L12DR15-2MU6ab-4INVM8-2MU6ab',        471, 'L1_2DR15-2MU6ab_4INVM8-2MU6ab',   [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],       

        #########   3MU6 seeded##########
        ['3mu6_bJpsi',                 300,  'L1_3MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['3mu6_bTau',                  301,  'L1_3MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['3mu6_bDimu',                 302,  'L1_3MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['3mu6_bUpsi',                 307, 'L1_3MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_iloose_mu6_11invm60_noos', 437, 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        #not set up yet?? ['mu6_iloose_mu6_18invm60_noos', 438, 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        ########### MU6_2MU4 seeded ###########
        ['mu6_mu4_bJpsimumu',             342, 'L1_MU6_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bBmumu',                343, 'L1_MU6_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bUpsimumu',             344, 'L1_MU6_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bBmumuxv2',             345, 'L1_MU6_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bBmumux_BcmumuDsloose', 346, 'L1_MU6_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bDimu',                 347, 'L1_MU6_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bDimu_novtx_noos',      348, 'L1_MU6_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        ###########  2MU10 seeded ###########
        ['2mu10_bBmumu',                337, 'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bBmumuxv2',             338, 'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bBmumux_BcmumuDsloose', 339, 'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bJpsimumu',            330, 'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bUpsimumu',            331, 'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bDimu',                 332, 'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bDimu_novtx_noos',      333, 'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu13_mu13_idperf_Zmumu',     334, 'L1_2MU10', [], [PhysicsStream], ['RATE:IDMonitoring','BW:Bphys'], -1],  
        ]



    TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures() + [

        # all e/g + X triggers are taken from DC14 and L1 thresholds adjusted

        ['e17_loose_mu14',          705, 'L1_EM15VH_MU10', ['L1_EM15VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e17_lhloose_mu14',        706, 'L1_EM15VH_MU10', ['L1_EM15VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],

        ['2e12_loose_mu10',         707, 'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['2e12_medium_mu10',        708, 'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['2e12_lhloose_mu10',       709, 'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['2e12_lhmedium_mu10',      710, 'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],

        ['e12_loose_2mu10',         711, 'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e12_medium_2mu10',        712, 'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e12_lhloose_2mu10',       713, 'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e12_lhmedium_2mu10',      714, 'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],

        #['e7_medium1_mu24',         814, 'L1_MU20', ['L1_EM3', 'L1_MU20'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],

        ['g25_medium_mu24',         715, 'L1_MU20', ['L1_EM15VH','L1_MU20'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],
        
        ['g10_loose_mu10_taumass',  716, 'L1_EM7_MU10', ['L1_EM7', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g10_loose","mu10"]] ],
        ['g20_loose_mu4_taumass',   717, 'L1_EM15_MU4', ['L1_EM15', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g20_loose","mu4"]] ],
        ['g10_loose_mu10',          718, 'L1_EM7_MU10', ['L1_EM7', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],
        ['g20_loose_mu4',           719, 'L1_EM15_MU4', ['L1_EM15', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],
        
        ['g15_loose_2mu10_msonly',  720, 'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],

        ['2g10_loose_mu20',         721, 'L1_MU20', ['L1_2EM7', 'L1_MU20'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['2g10_loose','mu20'] ]],
        ['2g10_medium_mu20',        722, 'L1_MU20', ['L1_2EM7', 'L1_MU20'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['2g10_medium','mu20'] ]],
        #['e24_medium1_L1EM20VHI_mu8noL1',       808,  'L1_EM20VHI', ['L1_EM20VHI', ''], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma'], -1, ['serial',-1,['e24_medium1_L1EM20VHI','mu8noL1'] ]],

        # Di-EM triggers
        # cut-based
        ['e24_medium_L1EM15VH_g25_medium',   723, 'L1_2EM15VH', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        ['e20_medium_g35_loose',             724, 'L1_2EM15VH', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        # likelihood
        ['e24_lhmedium_L1EM15VH_g25_medium', 725, 'L1_2EM15VH', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        ['e20_lhmedium_g35_loose',           726, 'L1_2EM15VH', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 

        # Tri-EM triggers
        ['e20_medium_2g10_loose',            727, 'L1_EM15VH_3EM7', ['L1_EM15VH','L1_2EM7'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        ['e20_medium_2g10_medium',           728, 'L1_EM15VH_3EM7', ['L1_EM15VH','L1_2EM7'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        ['e20_lhmedium_2g10_loose',          729, 'L1_EM15VH_3EM7', ['L1_EM15VH','L1_2EM7'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        ['e20_lhmedium_2g10_medium',         730, 'L1_EM15VH_3EM7', ['L1_EM15VH','L1_2EM7'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        
        ### NOT IN THE LIST ['e5_tight1_g4_etcut',                  811,      'L1_2EM3', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1, ['parallel',-1,[] ]],
        ### NOT IN THE LIST ['e24_medium1_L1EM20VHI_g8_etcut',      805, 'L1_EM20VHI', ['L1_EM20VHI','L1_EM3'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1, ['parallel',-1,[] ]],
        ### NOT IN THE LIST ['e17_loose1_2g8_etcut_L12EM7',         804, 'L1_EM15VH_3EM7',['L1_EM15VH','L1_2EM7'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ### NOT IN THE LIST ['e24_medium1_L1EM15VH_g25_medium1',    890, 'L1_2EM15VH',[], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ### NOT IN THE LIST ['e20_medium1_g35_loose1',              891, 'L1_2EM15VH',[], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ### NOT IN THE LIST ['e20_medium1_2g20_loose1',             892, 'L1_2EM15VH',[], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        
        #MET+X -- with e/g only
        #['e24_medium1_L1EM20VHI_xe100noL1', 902, 'L1_EM20VHI',['L1_EM20VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1,['serial',-1,["e24_medium1_L1EM20VHI","xe100noL1"]]],
        ['g70_loose_xe70noL1',             731, 'L1_EM50V',    ['L1_EM50V',''],    [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1,['serial',-1,["g70_loose","xe70noL1"]]],
        ['g60_loose_xe60noL1',             732, 'L1_EM50V',    ['L1_EM50V',''],    [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1,['serial',-1,["g60_loose","xe60noL1"]]],
        ['g45_tight_xe45noL1',             733, 'L1_EM20VH',   ['L1_EM20VH',''],   [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1,['serial',-1,["g45_tight","xe45noL1"]]],
        ['g45_tight_L1EM22VHLIL_xe45noL1', 734, 'L1_EM22VHLIL',['L1_EM22VHLIL',''],[PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1,['serial',-1,["g45_tight_L1EM22VHLIL","xe45noL1"]]],
        ['g40_tight_xe40noL1',             735, 'L1_EM20VH',['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1,['serial',-1,["g40_tight","xe40noL1"]]],

        ['e70_loose_xe70noL1',             736, 'L1_EM50V',['L1_EM50V',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1,['serial',-1,["e70_loose","xe70noL1"]]],
        ['e60_loose_xe60noL1',             737, 'L1_EM50V',['L1_EM50V',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1,['serial',-1,["e60_loose","xe60noL1"]]],
        ['e70_lhloose_xe70noL1',           738, 'L1_EM50V',['L1_EM50V',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1,['serial',-1,["e70_lhloose","xe70noL1"]]],
        ['e60_lhloose_xe60noL1',           739, 'L1_EM50V',['L1_EM50V',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1,['serial',-1,["e60_lhloose","xe60noL1"]]],

        #['mu24_xe100noL1_wEFMu',       901, 'L1_MU20',['L1_MU20',''], [PhysicsStream], ['RATE:METMuon', 'BW:Muon'], -1,['serial',-1,["mu24","xe100noL1_wEFMu"]]],
        ['j100_xe80',             904, 'L1_J40_XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j100","xe80"]]],
        ['j80_xe80',              905, 'L1_J40_XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j80","xe80"]]],
        #['j80_xe80_dphi1',        924, 'L1_J40_XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j80","xe80"]]],
        ['j80_xe80_dphi1_L1J40_10MINDPHI-Js2-XE50', 925, 'L1_J40_10MINDPHI-Js2-XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j80","xe80_L1J40_10MINDPHI-Js2-XE50"]]],
        ['j80_xe80_dphi1_L1J40_10MINDPHI-J20s2-XE50', 926, 'L1_J40_10MINDPHI-J20s2-XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j80","xe80_L1J40_10MINDPHI-J20s2-XE50"]]],
        ['j80_xe80_dphi1_L1J40_10MINDPHI-J20ab-XE50', 927, 'L1_J40_10MINDPHI-J20ab-XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j80","xe80_L1J40_10MINDPHI-J20ab-XE50"]]],
        ['j80_xe80_dphi1_L1J40_10MINDPHI-CJ20ab-XE50', 928, 'L1_J40_10MINDPHI-CJ20ab-XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j80","xe80_L1J40_10MINDPHI-CJ20ab-XE50"]]],
        ['j100_xe80_L1J40_10MINDPHI-Js2-XE50', 683, 'L1_J40_10MINDPHI-Js2-XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j100","xe80_L1J40_10MINDPHI-Js2-XE50"]]],
        ['j100_xe80_L1J40_10MINDPHI-J20s2-XE50', 684, 'L1_J40_10MINDPHI-J20s2-XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j100","xe80_L1J40_10MINDPHI-J20s2-XE50"]]],
        ['j100_xe80_L1J40_10MINDPHI-J20ab-XE50', 685, 'L1_J40_10MINDPHI-J20ab-XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j100","xe80_L1J40_10MINDPHI-J20ab-XE50"]]],
        ['j100_xe80_L1J40_10MINDPHI-CJ20ab-XE50', 686, 'L1_J40_10MINDPHI-CJ20ab-XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j100","xe80_L1J40_10MINDPHI-CJ20ab-XE50"]]],
        ['j30_xe10_razor170', 1015, 'L1_2J15_XE55', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,['j30','xe10']]],  
        
        # TAU+X
        ['tau35_medium1_calo_tau25_medium1_calo',          906, 'L1_TAU20_2TAU12_J25_2J20_3J12',['L1_TAU20','L1_TAU12'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_calo","tau25_medium1_calo"]]],
        ['e18_loose1_tau25_medium1_calo',                  907, 'L1_EM15_2TAU12_J25_2J15_3J12',['L1_EM15','L1_TAU12'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e18_loose1","tau25_medium1_calo"]]],
        ['e18_lhloose_tau25_medium1_calo',                 908, 'L1_EM15_2TAU12_J25_2J15_3J12',['L1_EM15','L1_TAU12'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e18_lhloose","tau25_medium1_calo"]]],
        ['e18_loose1_tau80_medium1_calo',                  909, 'L1_EM15_TAU40_2TAU15',['L1_EM15','L1_TAU40'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e18_loose1","tau80_medium1_calo"]]],
        ['e18_lhloose_tau80_medium1_calo',                 910, 'L1_EM15_TAU40_2TAU15',['L1_EM15','L1_TAU40'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e18_lhloose","tau80_medium1_calo"]]],
        ['mu14_tau35_medium1_calo',                        911, 'L1_MU10_TAU20',['L1_MU10','L1_TAU20'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14","tau35_medium1_calo"]]],
        ['mu14_tau25_medium1_calo',                        912, 'L1_MU10_TAU12_J25_2J12',['L1_MU10','L1_TAU12'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14","tau25_medium1_calo"]]],
        ['tau35_medium1_calo_tau25_medium1_calo_xe50',     913, 'L1_TAU20_2TAU12_XE35',['L1_TAU20','L1_TAU12','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_calo","tau25_medium1_calo","xe50"]]],
        ['tau35_medium1_calo_xe70_L1XE45',                 914, 'L1_TAU20_2J20_XE45',['L1_TAU20','L1_XE45'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_calo","xe70_L1XE45"]]],
        ['tau35_medium1_ptonly_tau25_medium1_ptonly',      915, 'L1_TAU20_2TAU12_J25_2J20_3J12',['L1_TAU20','L1_TAU12'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_ptonly","tau25_medium1_ptonly"]]],
        ['e18_loose1_tau25_medium1_ptonly',                916, 'L1_EM15_2TAU12_J25_2J15_3J12',['L1_EM15','L1_TAU12'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e18_loose1","tau25_medium1_ptonly"]]],
        ['e18_lhloose_tau25_medium1_ptonly',               917, 'L1_EM15_2TAU12_J25_2J15_3J12',['L1_EM15','L1_TAU12'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e18_lhloose","tau25_medium1_ptonly"]]],
        ['e18_loose1_tau80_medium1_ptonly',                918, 'L1_EM15_TAU40_2TAU15',['L1_EM15','L1_TAU40'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e18_loose1","tau80_medium1_ptonly"]]],
        ['e18_lhloose_tau80_medium1_ptonly',               919, 'L1_EM15_TAU40_2TAU15',['L1_EM15','L1_TAU40'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e18_lhloose","tau80_medium1_ptonly"]]],
        ['mu14_tau35_medium1_ptonly',                      920, 'L1_MU10_TAU20',['L1_MU10','L1_TAU20'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14","tau35_medium1_ptonly"]]],
        ['mu14_tau25_medium1_ptonly',                      921, 'L1_MU10_TAU12_J25_2J12',['L1_MU10','L1_TAU12'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14","tau25_medium1_ptonly"]]],
        ['tau35_medium1_ptonly_tau25_medium1_ptonly_xe50', 922, 'L1_TAU20_2TAU12_XE35',['L1_TAU20','L1_TAU12','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_ptonly","tau25_medium1_ptonly","xe50"]]],
        ['tau35_medium1_ptonly_xe70_L1XE45',               923, 'L1_TAU20_2J20_XE45',['L1_TAU20','L1_XE45'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_ptonly","xe70_L1XE45"]]],

        # lepton + jets

        ['e26_vloose_L1EM20VH_3j20noL1', 750, 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e26_vloose_L1EM20VH","3j20noL1"]]],
        ['e26_vloose_L1EM20VH_4j20noL1', 751, 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e26_vloose_L1EM20VH","4j20noL1"] ]],
        ['e26_vloose_L1EM20VH_5j15noL1', 752, 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e26_vloose_L1EM20VH","5j15noL1"] ]],
        ['e26_vloose_L1EM20VH_6j15noL1', 753, 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e26_vloose_L1EM20VH","6j15noL1"] ]],

        ['e26_lhvloose_L1EM20VH_3j20noL1', 754, 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e26_lhvloose_L1EM20VH","3j20noL1"] ]],
        ['e26_lhvloose_L1EM20VH_4j20noL1', 755, 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e26_lhvloose_L1EM20VH","4j20noL1"] ]],
        ['e26_lhvloose_L1EM20VH_5j15noL1', 756, 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e26_lhvloose_L1EM20VH","5j15noL1"] ]],
        ['e26_lhvloose_L1EM20VH_6j15noL1', 757, 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e26_lhvloose_L1EM20VH","6j15noL1"] ]],

        ['e24_vloose_L1EM20VH_3j20noL1', 758, 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_vloose_L1EM20VH","3j20noL1"] ]],
        ['e24_vloose_L1EM20VH_4j20noL1', 759, 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_vloose_L1EM20VH","4j20noL1"] ]],
        ['e24_vloose_L1EM20VH_5j15noL1', 760, 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_vloose_L1EM20VH","5j15noL1"] ]],
        ['e24_vloose_L1EM20VH_6j15noL1', 761, 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_vloose_L1EM20VH","6j15noL1"] ]],

        ['e24_lhvloose_L1EM20VH_3j20noL1', 762, 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_lhvloose_L1EM20VH","3j20noL1"] ]],
        ['e24_lhvloose_L1EM20VH_4j20noL1', 763, 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_lhvloose_L1EM20VH","4j20noL1"] ]],
        ['e24_lhvloose_L1EM20VH_5j15noL1', 764, 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_lhvloose_L1EM20VH","5j15noL1"] ]],
        ['e24_lhvloose_L1EM20VH_6j15noL1', 765, 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_lhvloose_L1EM20VH","6j15noL1"] ]],

        ['e24_vloose_L1EM18VH_3j20noL1', 766, 'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_vloose_L1EM18VH","3j20noL1"] ]],
        ['e24_vloose_L1EM18VH_4j20noL1', 767, 'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_vloose_L1EM18VH","4j20noL1"] ]],
        ['e24_vloose_L1EM18VH_5j15noL1', 768, 'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_vloose_L1EM18VH","5j15noL1"] ]],
        ['e24_vloose_L1EM18VH_6j15noL1', 769, 'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_vloose_L1EM18VH","6j15noL1"] ]],

        ['e24_lhvloose_L1EM18VH_3j20noL1', 780, 'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_lhvloose_L1EM18VH","3j20noL1"] ]],
        ['e24_lhvloose_L1EM18VH_4j20noL1', 781, 'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_lhvloose_L1EM18VH","4j20noL1"] ]],
        ['e24_lhvloose_L1EM18VH_5j15noL1', 782, 'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_lhvloose_L1EM18VH","5j15noL1"] ]],
        ['e24_lhvloose_L1EM18VH_6j15noL1', 783, 'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_lhvloose_L1EM18VH","6j15noL1"] ]],

        ['mu26_2j20noL1', 784, 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu26","2j20noL1"] ]],
        ['mu26_3j20noL1', 785, 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu26","3j20noL1"] ]],
        ['mu26_4j15noL1', 786, 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu26","4j15noL1"] ]],
        ['mu26_5j15noL1', 787, 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu26","5j15noL1"] ]],

        ['mu24_2j20noL1', 788, 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu24","2j20noL1"] ]],
        ['mu24_3j20noL1', 789, 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu24","3j20noL1"] ]],
        ['mu24_4j15noL1', 790, 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu24","4j15noL1"] ]],
        ['mu24_5j15noL1', 791, 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu24","5j15noL1"] ]],

        ['mu20_L1MU15_2j20noL1', 792, 'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu20_L1MU15","2j20noL1"] ]],
        ['mu20_L1MU15_3j20noL1', 793, 'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu20_L1MU15","3j20noL1"] ]],
        ['mu20_L1MU15_4j15noL1', 794, 'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu20_L1MU15","4j15noL1"] ]],
        ['mu20_L1MU15_5j15noL1', 795, 'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu20_L1MU15","5j15noL1"] ]],

        #['mu4_3j55_dr05',        1815, 'L1_MU4_3J20', ['L1_MU4', 'L1_3J20'], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,[] ]],

        ]

    TriggerFlags.MinBiasSlice.signatures = TriggerFlags.MinBiasSlice.signatures() + [
        #['mb_perf_L1LUCID', 823, 'L1_LUCID', [], ['MinBias'], [], 1],
        #['mb_sptrk', 806, 'L1_RD0_FILLED', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_perf_L1MBTS_1', 822, 'L1_MBTS_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp2000_trk70_hmt', 824, 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],        

        ['mb_sp2000_pusup600_trk70_hmt', 836, 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['mb_sp2000_pusup600_trk70_hmt_L1TE30', 837, 'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['mb_sp2000_pusup600_trk70_hmt_L1TE40', 838, 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['mb_sp2000_pusup700_trk70_hmt_L1TE30', 839, 'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['mb_sp2000_pusup700_trk70_hmt_L1TE40', 840, 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 

	#ATR-9419
        ['mb_sp2000_pusup700_trk60_hmt_L1TE30', 841, 'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp2500_pusup750_trk90_hmt_L1TE40', 842, 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp3000_pusup800_trk120_hmt_L1TE50', 852, 'L1_TE50', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp2000_pusup700_trk50_sumet70_hmt_L1TE30', 853, 'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp2000_pusup700_trk50_sumet110_hmt_L1TE40', 854, 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp2000_pusup700_trk50_sumet150_hmt_L1TE50', 855, 'L1_TE50', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	#supporting HMT triggers
	['mb_sp1800_hmtperf_L1TE20', 856, 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ]


    ## The following slices are only needed for Physics runnings and LS1 menus
    ## To be commented out for DC14
    TriggerFlags.CalibSlice.signatures   = TriggerFlags.CalibSlice.signatures() + []
    TriggerFlags.CosmicSlice.signatures  = TriggerFlags.CosmicSlice.signatures() + []
    TriggerFlags.StreamingSlice.signatures = TriggerFlags.StreamingSlice.signatures() + [

        ['noalg_L1J400',       690   , 'L1_J400', [], [PhysicsStream], ['RATE:SeededStreamers', "BW:Jets"], -1],

        ['noalg_mb_L1TE20', 843, 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['noalg_mb_L1TE30', 844, 'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['noalg_mb_L1TE40', 845, 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

        #Muon streamers
        ['noalg_L1MU20',           473, 'L1_MU20',           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L1MU15',           474, 'L1_MU15',           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L1MU10',           475, 'L1_MU10',           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L1MU4_EMPTY',      476, 'L1_MU4_EMPTY',      [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L1MU4_FIRSTEMPTY', 477, 'L1_MU4_FIRSTEMPTY', [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        #['noalg_L1MU10_MU6',       478, 'L1_MU10_MU6',       [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        
        # Streamers for TOPO b-jet
        ['noalg_L10DR04-MU4ab-CJ15ab', 846, 'L1_0DR04-MU4ab-CJ15ab', [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1],
        ['noalg_L10DR04-MU4ab-CJ30ab', 847, 'L1_0DR04-MU4ab-CJ30ab', [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1],
        ['noalg_L10DR04-MU6ab-CJ25ab', 848, 'L1_0DR04-MU6ab-CJ25ab', [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1],
        ['noalg_L10DR04-MU4ab-CJ17ab', 849, 'L1_0DR04-MU4ab-CJ17ab', [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1],
        ['noalg_L10DR04-MU4ab-CJ20ab', 860, 'L1_0DR04-MU4ab-CJ20ab', [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1],

        # Streamers for TOPO HT
        ['noalg_L1HT0-AJ0all.ETA49',   861, 'L1_HT0-AJ0all.ETA49',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1],
        ['noalg_L1HT190-AJ15all.ETA20',   862, 'L1_HT190-AJ15all.ETA20',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1],
        ['noalg_L1HT190-J15s5.ETA20',   863, 'L1_HT190-J15s5.ETA20',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1],
        ['noalg_L1HT150-AJ20all.ETA30',  864, 'L1_HT150-AJ20all.ETA30',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1],
        ['noalg_L1HT150-J20s5.ETA30',   865, 'L1_HT150-J20s5.ETA30',    [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1],

        ['noalg_L1DPhi_AJsAJs',   3100, 'L1_DPhi_AJsAJs',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L1DPhi_EMsTAUs',   3101, 'L1_DPhi_EMsTAUs',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L1DEta_AJabAJab',   3102, 'L1_DEta_AJabAJab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L1DEta_EMabTAUab',   3103, 'L1_DEta_EMabTAUab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L12INVM999-2MU4ab',   3104, 'L1_2INVM999-2MU4ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L12INVM999-CMU4ab-MU4ab',   3105, 'L1_2INVM999-CMU4ab-MU4ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L12INVM999-2CMU4ab',   3106, 'L1_2INVM999-2CMU4ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L12INVM999-MU6ab-MU4ab',   3107, 'L1_2INVM999-MU6ab-MU4ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L12INVM999-CMU6ab-CMU4ab',   3108, 'L1_2INVM999-CMU6ab-CMU4ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L12INVM999-2MU6ab',   3109, 'L1_2INVM999-2MU6ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L14INVM8-2MU4ab',   3110, 'L1_4INVM8-2MU4ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L14INVM8-CMU4ab-MU4ab',   3111, 'L1_4INVM8-CMU4ab-MU4ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L14INVM8-2CMU4ab',   3112, 'L1_4INVM8-2CMU4ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L14INVM8-MU6ab-MU4ab',   3113, 'L1_4INVM8-MU6ab-MU4ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L14INVM8-CMU6ab-CMU4ab',   3114, 'L1_4INVM8-CMU6ab-CMU4ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L14INVM8-2MU6ab',   3115, 'L1_4INVM8-2MU6ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L12DR99-2MU4ab',   3116, 'L1_2DR99-2MU4ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L15DETA99-5DPHI99-2MU4ab',   3117, 'L1_5DETA99-5DPHI99-2MU4ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L15DETA99-5DPHI99-MU6ab-MU4ab',   3118, 'L1_5DETA99-5DPHI99-MU6ab-MU4ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L15DETA99-5DPHI99-2MU6ab',   3119, 'L1_5DETA99-5DPHI99-2MU6ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L10DR10-MU10ab-MU6ab',   3120, 'L1_0DR10-MU10ab-MU6ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L10DETA35-0DPHI25-EM8abi-MU10ab',   3121, 'L1_0DETA35-0DPHI25-EM8abi-MU10ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L10DETA35-0DPHI25-EM15abi-MUab',   3122, 'L1_0DETA35-0DPHI25-EM15abi-MUab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L110MINDPHI-Js2-XE50',   3123, 'L1_10MINDPHI-Js2-XE50',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L110MINDPHI-J20s2-XE50',   3124, 'L1_10MINDPHI-J20s2-XE50',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L110MINDPHI-J20ab-XE50',   3125, 'L1_10MINDPHI-J20ab-XE50',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L110MINDPHI-CJ20ab-XE50',   3126, 'L1_10MINDPHI-CJ20ab-XE50',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L1400INVM9999-AJ30s6-AJ20s6',   3127, 'L1_400INVM9999-AJ30s6-AJ20s6',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L1350INVM9999-AJ30s6-AJ20s6',   3128, 'L1_350INVM9999-AJ30s6-AJ20s6',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L1300INVM9999-AJ30s6-AJ20s6',   3129, 'L1_300INVM9999-AJ30s6-AJ20s6',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L1200INVM9999-AJ30s6-AJ20s6',   3130, 'L1_200INVM9999-AJ30s6-AJ20s6',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L1350INVM9999-J30s6-J20s6',   3131, 'L1_350INVM9999-J30s6-J20s6',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L1300INVM9999-J30s6-J20s6',   3132, 'L1_300INVM9999-J30s6-J20s6',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L1250INVM9999-J30s6-J20s6',   3133, 'L1_250INVM9999-J30s6-J20s6',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L1200INVM9999-J30s6-J20s6',   3134, 'L1_200INVM9999-J30s6-J20s6',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L140DETA99-AJ30s6-AJ20s6',   3135, 'L1_40DETA99-AJ30s6-AJ20s6',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L140DETA99-J30s6-J20s6',   3136, 'L1_40DETA99-J30s6-J20s6',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L1HT150-AJj15all.ETA49',   3137, 'L1_HT150-AJj15all.ETA49',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L1HT20-AJj0all.ETA49',   3138, 'L1_HT20-AJj0all.ETA49',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L10DETA10-Js1-Js2',   3139, 'L1_0DETA10-Js1-Js2',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L10MATCH-4AJ20.ETA32-4AJj15',   3140, 'L1_0MATCH-4AJ20.ETA32-4AJj15',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L11INVM5-EMs2-EMall',   3141, 'L1_1INVM5-EMs2-EMall',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L11INVM5-EM7s2-EMall',   3142, 'L1_1INVM5-EM7s2-EMall',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L11INVM5-EM12s2-EMall',   3143, 'L1_1INVM5-EM12s2-EMall',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L110MINDPHI-AJj15s2-XE0',   3144, 'L1_10MINDPHI-AJj15s2-XE0',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L120MINDPHI-AJjs6-XE0',   3145, 'L1_20MINDPHI-AJjs6-XE0',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L120MINDPHI-AJj15s2-XE0',   3146, 'L1_20MINDPHI-AJj15s2-XE0',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L1NOT-02MATCH-EM9s1-AJj15all.ETA49',   3147, 'L1_NOT-02MATCH-EM9s1-AJj15all.ETA49',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L105RATIO-XE0-SUM0-EM9s1-HT0-AJj15all.ETA49',   3148, 'L1_05RATIO-XE0-SUM0-EM9s1-HT0-AJj15all.ETA49',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L110MINDPHI-EM6s1-XE0',   3149, 'L1_10MINDPHI-EM6s1-XE0',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L120MINDPHI-EM9s6-XE0',   3150, 'L1_20MINDPHI-EM9s6-XE0',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L120MINDPHI-EM6s1-XE0',   3151, 'L1_20MINDPHI-EM6s1-XE0',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L105RATIO-XE0-HT0-AJj15all.ETA49',   3152, 'L1_05RATIO-XE0-HT0-AJj15all.ETA49',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L108RATIO-XE0-HT0-AJj0all.ETA49',   3153, 'L1_08RATIO-XE0-HT0-AJj0all.ETA49',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L140RATIO2-XE0-HT0-AJj15all.ETA49',   3154, 'L1_40RATIO2-XE0-HT0-AJj15all.ETA49',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L190RATIO2-XE0-HT0-AJj0all.ETA49',   3155, 'L1_90RATIO2-XE0-HT0-AJj0all.ETA49',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L1210RATIO-0MATCH-TAU30si2-EMall',   3156, 'L1_210RATIO-0MATCH-TAU30si2-EMall',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L1NOT-0MATCH-TAU30si2-EMall',   3157, 'L1_NOT-0MATCH-TAU30si2-EMall',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L10DR28-MU10ab-TAU12abi',   3158, 'L1_0DR28-MU10ab-TAU12abi',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L11DISAMB-TAU12abi-J25ab',   3159, 'L1_1DISAMB-TAU12abi-J25ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L1DISAMB-EM15abhi-TAU40ab',   3160, 'L1_DISAMB-EM15abhi-TAU40ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L11DISAMB-TAU20ab-J20ab',   3161, 'L1_1DISAMB-TAU20ab-J20ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L1DISAMB-EM15abhi-TAU12abi',   3162, 'L1_DISAMB-EM15abhi-TAU12abi',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L11DISAMB-EM15his2-TAU12abi-J25ab',   3163, 'L1_1DISAMB-EM15his2-TAU12abi-J25ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L11DISAMB-J25ab-0DR28-EM15his2-TAU12abi',   3164, 'L1_1DISAMB-J25ab-0DR28-EM15his2-TAU12abi',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L11DISAMB-TAU20abi-TAU12abi-J25ab',   3165, 'L1_1DISAMB-TAU20abi-TAU12abi-J25ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L10DR28-TAU20abi-TAU12abi',   3166, 'L1_0DR28-TAU20abi-TAU12abi',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L10DETA20-0DPHI20-TAU20abi-TAU12abi',   3167, 'L1_0DETA20-0DPHI20-TAU20abi-TAU12abi',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L11DISAMB-J25ab-0DR28-TAU20abi-TAU12abi',   3168, 'L1_1DISAMB-J25ab-0DR28-TAU20abi-TAU12abi',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L1LAR-EM50s1',   3169, 'L1_LAR-EM50s1',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L1LAR-J100s1',   3170, 'L1_LAR-J100s1',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L12DR15-2MU4ab',   3171, 'L1_2DR15-2MU4ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L12DR15-CMU4ab-MU4ab',   3172, 'L1_2DR15-CMU4ab-MU4ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L12DR15-2CMU4ab',   3173, 'L1_2DR15-2CMU4ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L12DR15-MU6ab-MU4ab',   3174, 'L1_2DR15-MU6ab-MU4ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L12DR15-CMU6ab-CMU4ab',   3175, 'L1_2DR15-CMU6ab-CMU4ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        ['noalg_L12DR15-2MU6ab',   3176, 'L1_2DR15-2MU6ab',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        
        ]

    TriggerFlags.MonitorSlice.signatures = TriggerFlags.MonitorSlice.signatures() + []
    #TriggerFlags.GenericSlice.signatures = TriggerFlags.GenericSlice.signatures() + []
    
    
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
Prescales.HLTPrescales.update({
     # Non physics streams
    'costmonitor'                           : [    -1,    0,   -1],
    'eb_empty_L1RD0_EMPTY'                  : [    -1,    0,   -1],
    'id_cosmicid'                           : [    -1,    0,   -1],
    'id_cosmicid_L1MU11'                    : [    -1,    0,   -1],
    'id_cosmicid_L1MU4'                     : [    -1,    0,   -1],
    'id_cosmicid_ds'                        : [    -1,    0,   -1],
    'id_cosmicid_trtxk'                     : [    -1,    0,   -1],
    'id_cosmicid_trtxk_central'             : [    -1,    0,   -1],
    'l1calocalib'                           : [    -1,    0,   -1],
    'l1calocalib_L1BGRP7'                   : [    -1,    0,   -1],
    'larcalib_L1EM3'                        : [    -1,    0,   -1],
    'larcalib_L1EM3_EMPTY'                  : [    -1,    0,   -1],
    'larcalib_L1J12'                        : [    -1,    0,   -1],
    'larcalib_L1J12_EMPTY'                  : [    -1,    0,   -1],
    'larcalib_L1J3032ETA49_EMPTY'           : [    -1,    0,   -1],
    'larcalib_L1MU10'                       : [    -1,    0,   -1],
    'larcalib_L1TAU8_EMPTY'                 : [    -1,    0,   -1],
    'larps_L1EM3_EMPTY'                     : [    -1,    0,   -1],
    'larps_L1EM7_EMPTY'                     : [    -1,    0,   -1],
    'larps_L1J12_EMPTY'                     : [    -1,    0,   -1],
    'larps_L1J30_EMPTY'                     : [    -1,    0,   -1],
    'larps_L1TAU8_EMPTY'                    : [    -1,    0,   -1],
    'mu0_muoncalib'                         : [    -1,    0,   -1],
    'mu4_cosmicEF_L1MU11_EMPTY'             : [    -1,    0,   -1],
    'mu4_cosmicEF_L1MU4_EMPTY'              : [    -1,    0,   -1],
    'mu4_cosmicEF_ds1_L1MU4'                : [    -1,    0,   -1],
    'mu4_cosmicEF_ds2_L1MU4'                : [    -1,    0,   -1],
    'mu4_cosmic_L1MU11_EMPTY'               : [    -1,    0,   -1],
    'mu4_cosmic_L1MU4_EMPTY'                : [    -1,    0,   -1],
    'mu4_msonly_cosmicEF_L1MU11_EMPTY'      : [    -1,    0,   -1],
    'mu4_msonly_cosmicEF_L1MU4_EMPTY'       : [    -1,    0,   -1],
    'mu4_msonly_cosmic_L1MU11_EMPTY'        : [    -1,    0,   -1],
    'noalg_L1All'                           : [    -1,    0,   -1],
    'noalg_L1Calo'                          : [    -1,    0,   -1],
    'noalg_L1Calo_EMPTY'                    : [    -1,    0,   -1],
    'noalg_L1EM3'                           : [    -1,    0,   -1],
    'noalg_L1J12'                           : [    -1,    0,   -1],
    'noalg_L1MinBias'                       : [    -1,    0,   -1],
    'noalg_L1Standby'                       : [    -1,    0,   -1],
    'noalg_L1TAU8'                          : [    -1,    0,   -1],
    'noalg_bkg_L1Bkg'                       : [    -1,    0,   -1],
    'noalg_bkg_L1RD0_EMPTY'                 : [    -1,    0,   -1],
    'noalg_bkg_L1RD0_UNPAIRED_ISO'          : [    -1,    0,   -1],
    'noalg_bkg_L1RD1_FILLED'                : [    -1,    0,   -1],
    'noalg_cosmiccalo_L1EM3_EMPTY'          : [    -1,    0,   -1],
    'noalg_cosmiccalo_L1J12_EMPTY'          : [    -1,    0,   -1],
    'noalg_cosmiccalo_L1J12_FIRSTEMPTY'     : [    -1,    0,   -1],
    'noalg_cosmiccalo_L1J3032ETA49_EMPTY'   : [    -1,    0,   -1],
    'noalg_cosmiccalo_L1J30_EMPTY'          : [    -1,    0,   -1],
    'noalg_cosmiccalo_L1J30_FIRSTEMPTY'     : [    -1,    0,   -1],
    'noalg_cosmiccalo_L1RD1_EMPTY'          : [    -1,    0,   -1],
    'noalg_cosmicmuons_L1MU11_EMPTY'        : [    -1,    0,   -1],
    'noalg_cosmicmuons_L1MU4_EMPTY'         : [    -1,    0,   -1],
    'noalg_dcmmon_L1RD0_EMPTY'              : [    -1,    0,   -1],
    'noalg_eb_L1BGRP7'                      : [    -1,    0,   -1],
    'noalg_eb_L1RD0_EMPTY'                  : [    -1,    0,   -1],
    'noalg_idmon_L1RD0_EMPTY'               : [    -1,    0,   -1],
    'noalg_idmon_L1RD0_FILLED'              : [    -1,    0,   -1],
    'noalg_idmon_L1RD0_UNPAIRED_ISO'        : [    -1,    0,   -1],
    'noalg_jettauetmiss_L1RD0_FIRSTEMPTY'   : [    -1,    0,   -1],
    'pixel_beam'                            : [    -1,    0,   -1],
    'pixel_noise'                           : [    -1,    0,   -1],
    'robrequest'                            : [    -1,    0,   -1],
    'robrequest_L1RD0_EMPTY'                : [    -1,    0,   -1],
    'sct_noise'                             : [    -1,    0,   -1],
    'tilelarcalib_L1EM3_EMPTY'              : [    -1,    0,   -1],
    'tilelarcalib_L1J12_EMPTY'              : [    -1,    0,   -1],
    'tilelarcalib_L1J3032ETA49_EMPTY'       : [    -1,    0,   -1],
    'tilelarcalib_L1TAU8_EMPTY'             : [    -1,    0,   -1],
    'timeburner'                            : [    -1,    0,   -1],
     # Physics streams but non collision BC items
    'e5_etcut_L1EM3_EMPTY'                  : [    -1,    0,   -1],
    'g5_etcut_L1EM3_EMPTY'                  : [    -1,    0,   -1],
    'j0_perf_L1RD0_EMPTY'                   : [    -1,    0,   -1],
    'j0_perf_bperf_L1RD0_EMPTY'             : [    -1,    0,   -1],
    'larps_L1J12_FIRSTEMPTY'                : [    -1,    0,   -1],
    'larps_L1J30_FIRSTEMPTY'                : [    -1,    0,   -1],
    'mb_perf_L1LUCID_EMPTY'                 : [    -1,    0,   -1],
    'mb_sptrk_L1RD0_EMPTY'                  : [    -1,    0,   -1],
    'mb_sptrk_costr_L1RD0_EMPTY'            : [    -1,    0,   -1],
    'noalg_mb_L1RD0_EMPTY'                  : [    -1,    0,   -1],
    'noalg_mb_L1RD1_EMPTY'                  : [    -1,    0,   -1],
    'mb_perf_L1LUCID_UNPAIRED_ISO'          : [    -1,    0,   -1],
    'mb_sptrk_L1RD0_UNPAIRED_ISO'           : [    -1,    0,   -1],
    'noalg_mb_L1MBTS_2_UNPAIRED_ISO'        : [    -1,    0,   -1],
    'noalg_mb_L1RD0_UNPAIRED_ISO'           : [    -1,    0,   -1],
    'tau1_cosmic_ptonly_L1MU4_EMPTY'        : [    -1,    0,   -1],
    'tau1_cosmic_track_L1MU4_EMPTY'         : [    -1,    0,   -1],
     # Physics streams but Mx commissioning
    'ht0_perf_L1J12'                        : [    -1,    0,   -1],
    'j0_perf_L1J12'                         : [    -1,    0,   -1],
    'j0_perf_L1MU10'                        : [    -1,    0,   -1],
    'j0_perf_bperf_L1J12'                   : [    -1,    0,   -1],
    'j0_perf_bperf_L1MU10'                  : [    -1,    0,   -1],
    'tau8_cosmic_ptonly'                    : [    -1,    0,   -1],
    'tau8_cosmic_track'                     : [    -1,    0,   -1],
    'noalg_mb_L1MBTS_2'                     : [    -1,    0,   -1],
    'noalg_mb_L1RD0_FILLED'                 : [    -1,    0,   -1],
    'noalg_mb_L1RD1_FILLED'                 : [    -1,    0,   -1],
})
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
Prescales.HLTPrescales_loose_mc_prescale = deepcopy(Prescales.HLTPrescales)
Prescales.HLTPrescales_loose_mc_prescale.update({})
######################################################

######################################################
# TIGHT mc prescales
######################################################
Prescales.L1Prescales_tight_mc_prescale  = deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_tight_mc_prescale = deepcopy(Prescales.HLTPrescales)
Prescales.HLTPrescales_tight_mc_prescale.update({
     # Minbias chains in physics_pp_v5
    'mb_perf_L1LUCID'                       : [    -1,    0,   -1],
    'mb_perf_L1MBTS_2'                      : [    -1,    0,   -1],
    'mb_sptrk'                              : [    -1,    0,   -1],
    'mb_sptrk_costr'                        : [    -1,    0,   -1],
    'mb_sptrk_noisesup'                     : [    -1,    0,   -1],
    'mb_mbts_L1MBTS_2'                      : [    -1,    0,   -1],
     # Minbias chains in MC_pp_v5
    'mb_perf_L1MBTS_1'                      : [    -1,    0,   -1],
    'mb_sp2000_trk70_hmt'                   : [    -1,    0,   -1],
    'mb_sp2000_pusup600_trk70_hmt'          : [    -1,    0,   -1],
    'mb_sp2000_pusup600_trk70_hmt_L1TE30'   : [    -1,    0,   -1],
    'mb_sp2000_pusup600_trk70_hmt_L1TE40'   : [    -1,    0,   -1],
    'mb_sp2000_pusup700_trk70_hmt_L1TE30'   : [    -1,    0,   -1],
    'mb_sp2000_pusup700_trk70_hmt_L1TE40'   : [    -1,    0,   -1],
    'mb_sp2000_pusup700_trk60_hmt_L1TE30'   : [    -1,    0,   -1],
    'mb_sp2500_pusup750_trk90_hmt_L1TE40'   : [    -1,    0,   -1],
    'mb_sp3000_pusup800_trk120_hmt_L1TE50'  : [    -1,    0,   -1],
    'mb_sp2000_pusup700_trk50_sumet70_hmt_L1TE30'  : [    -1,    0,   -1],
    'mb_sp2000_pusup700_trk50_sumet110_hmt_L1TE40' : [    -1,    0,   -1],
    'mb_sp2000_pusup700_trk50_sumet150_hmt_L1TE50' : [    -1,    0,   -1],
    'mb_sp1800_hmtperf_L1TE20'              : [    -1,    0,   -1],
    })
######################################################

######################################################
# Special mc prescale: loose+ electron + bjets prescaled
######################################################
Prescales.L1Prescales_special_mc_prescale  = deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_special_mc_prescale = deepcopy(Prescales.HLTPrescales_loose_mc_prescale)
Prescales.HLTPrescales_special_mc_prescale.update({
    'e0_L2Star_perf_L1EM15'                 : [    5,    0,   -1],
    'e0_L2Star_perf_L1EM3'                  : [    5,    0,   -1],
    'e0_perf_L1EM15'                        : [    5,    0,   -1],
    'e0_perf_L1EM3'                         : [    5,    0,   -1],
    'e4_etcut'                              : [    5,    0,   -1],
    'e5_etcut'                              : [    5,    0,   -1],
    'e5_etcut_L105RATIO-XE0-HT0-AJj15all.ETA49': [    5,    0,   -1],
    'e5_etcut_L108RATIO-XE0-HT0-AJj0all.ETA49': [    5,    0,   -1],
    'e5_etcut_L110MINDPHI-AJj15s2-XE0'      : [    5,    0,   -1],
    'e5_etcut_L120MINDPHI-AJj15s2-XE0'      : [    5,    0,   -1],
    'e5_etcut_L120MINDPHI-AJjs6-XE0'        : [    5,    0,   -1],
    'e5_etcut_L140RATIO2-XE0-HT0-AJj15all.ETA49': [    5,    0,   -1],
    'e5_etcut_L190RATIO2-XE0-HT0-AJj0all.ETA49': [    5,    0,   -1],
    'e5_etcut_L1HT20-AJj0all.ETA49'         : [    5,    0,   -1],
    'e5_etcut_L1NOT-02MATCH-EM9s1-AJj15all.ETA49': [    5,    0,   -1],
    'e5_etcut_L1NOT-02MATCH-EM9s1-AJj15all.ETA49_05RATIO-XE0-SUM0-EM9s1-HT0-AJj15all.ETA49': [    5,    0,   -1],
    'e5_lhloose'                            : [    5,    0,   -1],
    'e5_lhloose_L2StarA'                    : [    5,    0,   -1],
    'e5_lhloose_L2Star_idperf'              : [    5,    0,   -1],
    'e5_lhloose_idperf'                     : [    5,    0,   -1],
    'e5_lhtight'                            : [    5,    0,   -1],
    'e5_lhtight_e4_etcut'                   : [    5,    0,   -1],
    'e5_lhtight_e4_etcut_Jpsiee'            : [    5,    0,   -1],
    'e5_lhvloose'                           : [    5,    0,   -1],
    'e5_loose'                              : [    5,    0,   -1],
    'e5_loose1_L2Star_idperf'               : [    5,    0,   -1],
    'e5_loose1_idperf'                      : [    5,    0,   -1],
    'e5_loose_L2StarA'                      : [    5,    0,   -1],
    'e5_loose_L2Star_idperf'                : [    5,    0,   -1],
    'e5_loose_idperf'                       : [    5,    0,   -1],
    'e5_tight'                              : [    5,    0,   -1],
    'e5_tight1_e4_etcut'                    : [    5,    0,   -1],
    'e5_tight1_e4_etcut_Jpsiee'             : [    5,    0,   -1],
    'e5_tight_e4_etcut'                     : [    5,    0,   -1],
    'e5_tight_e4_etcut_Jpsiee'              : [    5,    0,   -1],
    'e5_vloose'                             : [    5,    0,   -1],
    'e9_etcut'                              : [    5,    0,   -1],
    'e9_etcut_e5_lhtight_Jpsiee'            : [    5,    0,   -1],
    'e9_etcut_e5_tight_Jpsiee'              : [    5,    0,   -1],
    'e9_lhmedium'                           : [    5,    0,   -1],
    'e9_lhtight'                            : [    5,    0,   -1],
    'e9_lhtight_e4_etcut_Jpsiee'            : [    5,    0,   -1],
    'e9_medium'                             : [    5,    0,   -1],
    'e9_tight'                              : [    5,    0,   -1],
    'e9_tight_e4_etcut_Jpsiee'              : [    5,    0,   -1],
    'e10_etcut_L1EM7'                       : [    5,    0,   -1],
    'e10_lhvloose_L1EM7'                    : [    5,    0,   -1],
    'e10_vloose_L1EM7'                      : [    5,    0,   -1],
    'e12_lhloose'                           : [    5,    0,   -1],
    'e12_lhloose_2mu10'                     : [    5,    0,   -1],
    'e12_lhloose_L1EM10VH'                  : [    5,    0,   -1],
    'e12_lhmedium_2mu10'                    : [    5,    0,   -1],
    'e12_lhvloose_L1EM10VH'                 : [    5,    0,   -1],
    'e12_loose'                             : [    5,    0,   -1],
    'e12_loose_2mu10'                       : [    5,    0,   -1],
    'e12_loose_L1EM10VH'                    : [    5,    0,   -1],
    'e12_medium_2mu10'                      : [    5,    0,   -1],
    'e12_vloose_L1EM10VH'                   : [    5,    0,   -1],
    'e14_etcut'                             : [    5,    0,   -1],
    'e14_etcut_e5_lhtight_Jpsiee'           : [    5,    0,   -1],
    'e14_etcut_e5_tight_Jpsiee'             : [    5,    0,   -1],
    'e14_lhtight'                           : [    5,    0,   -1],
    'e14_lhtight_e4_etcut_Jpsiee'           : [    5,    0,   -1],
    'e14_tight'                             : [    5,    0,   -1],
    'e14_tight_e4_etcut_Jpsiee'             : [    5,    0,   -1],
    'e15_etcut_L1EM7'                       : [    5,    0,   -1],
    'e15_lhloose_L1EM13VH'                  : [    5,    0,   -1],
    'e15_lhvloose_L1EM13VH'                 : [    5,    0,   -1],
    'e15_lhvloose_L1EM7'                    : [    5,    0,   -1],
    'e15_loose_L1EM13VH'                    : [    5,    0,   -1],
    'e15_vloose_L1EM13VH'                   : [    5,    0,   -1],
    'e15_vloose_L1EM7'                      : [    5,    0,   -1],
    'e17_etcut_L1EM15'                      : [    5,    0,   -1],
    'e17_etcut_L2StarA_L1EM15'              : [    5,    0,   -1],
    'e17_etcut_L2StarB_L1EM15'              : [    5,    0,   -1],
    'e17_etcut_L2StarC_L1EM15'              : [    5,    0,   -1],
    'e17_etcut_L2Star_idperf_L1EM15'        : [    5,    0,   -1],
    'e17_etcut_idperf_L1EM15'               : [    5,    0,   -1],
    'e17_lhloose'                           : [    5,    0,   -1],
    'e17_lhloose_2e9_lhloose'               : [    5,    0,   -1],
    'e17_lhloose_L1EM15'                    : [    5,    0,   -1],
    'e17_lhloose_L1EM15VHJJ1523ETA49'       : [    5,    0,   -1],
    'e17_lhloose_mu14'                      : [    5,    0,   -1],
    'e17_lhmedium'                          : [    5,    0,   -1],
    'e17_lhmedium_2e9_lhmedium'             : [    5,    0,   -1],
    'e17_lhmedium_iloose'                   : [    5,    0,   -1],
    'e17_lhmedium_iloose_2e9_lhmedium'      : [    5,    0,   -1],
    'e17_lhvloose'                          : [    5,    0,   -1],
    'e17_loose'                             : [    5,    0,   -1],
    'e17_loose_2e9_loose'                   : [    5,    0,   -1],
    'e17_loose_L1EM15'                      : [    5,    0,   -1],
    'e17_loose_L1EM15VHJJ1523ETA49'         : [    5,    0,   -1],
    'e17_loose_mu14'                        : [    5,    0,   -1],
    'e17_medium'                            : [    5,    0,   -1],
    'e17_medium_2e9_medium'                 : [    5,    0,   -1],
    'e17_medium_iloose'                     : [    5,    0,   -1],
    'e17_medium_iloose_2e9_medium'          : [    5,    0,   -1],
    'e17_vloose'                            : [    5,    0,   -1],
    'e18_lhloose_tau25_medium1_calo'        : [    5,    0,   -1],
    'e18_lhloose_tau25_medium1_ptonly'      : [    5,    0,   -1],
    'e18_lhloose_tau80_medium1_calo'        : [    5,    0,   -1],
    'e18_lhloose_tau80_medium1_ptonly'      : [    5,    0,   -1],
    'e18_loose1_tau25_medium1_calo'         : [    5,    0,   -1],
    'e18_loose1_tau25_medium1_ptonly'       : [    5,    0,   -1],
    'e18_loose1_tau80_medium1_calo'         : [    5,    0,   -1],
    'e18_loose1_tau80_medium1_ptonly'       : [    5,    0,   -1],
    'e20_etcut_L1EM12'                      : [    5,    0,   -1],
    'e20_lhmedium'                          : [    5,    0,   -1],
    'e20_lhmedium_2g10_loose'               : [    5,    0,   -1],
    'e20_lhmedium_2g10_medium'              : [    5,    0,   -1],
    'e20_lhmedium_g35_loose'                : [    5,    0,   -1],
    'e20_lhvloose'                          : [    5,    0,   -1],
    'e20_lhvloose_L1EM12'                   : [    5,    0,   -1],
    'e20_medium'                            : [    5,    0,   -1],
    'e20_medium_2g10_loose'                 : [    5,    0,   -1],
    'e20_medium_2g10_medium'                : [    5,    0,   -1],
    'e20_medium_g35_loose'                  : [    5,    0,   -1],
    'e20_vloose'                            : [    5,    0,   -1],
    'e20_vloose_L1EM12'                     : [    5,    0,   -1],
    'e24_lhmedium_L1EM15VH'                 : [    5,    0,   -1],
    'e24_lhmedium_L1EM15VH_g25_medium'      : [    5,    0,   -1],
    'e24_lhmedium_L1EM18VH'                 : [    5,    0,   -1],
    'e24_lhmedium_L2Star_idperf_L1EM20VH'   : [    5,    0,   -1],
    'e24_lhmedium_idperf_L1EM20VH'          : [    5,    0,   -1],
    'e24_lhmedium_iloose_L1EM18VH'          : [    5,    0,   -1],
    'e24_lhmedium_iloose_L1EM20VH'          : [    5,    0,   -1],
    'e24_lhmedium_iloose_L2StarA_L1EM20VH'  : [    5,    0,   -1],
    'e24_lhtight_L1EM20VH'                  : [    5,    0,   -1],
    'e24_lhtight_iloose'                    : [    5,    0,   -1],
    'e24_lhtight_iloose_HLTCalo_L1EM20VH'   : [    5,    0,   -1],
    'e24_lhtight_iloose_L1EM20VH'           : [    5,    0,   -1],
    'e24_lhtight_iloose_L2EFCalo_L1EM20VH'  : [    5,    0,   -1],
    'e24_lhvloose_L1EM18VH'                 : [    5,    0,   -1],
    'e24_lhvloose_L1EM18VH_3j20noL1'        : [    5,    0,   -1],
    'e24_lhvloose_L1EM18VH_4j20noL1'        : [    5,    0,   -1],
    'e24_lhvloose_L1EM18VH_5j15noL1'        : [    5,    0,   -1],
    'e24_lhvloose_L1EM18VH_6j15noL1'        : [    5,    0,   -1],
    'e24_lhvloose_L1EM20VH'                 : [    5,    0,   -1],
    'e24_lhvloose_L1EM20VH_3j20noL1'        : [    5,    0,   -1],
    'e24_lhvloose_L1EM20VH_4j20noL1'        : [    5,    0,   -1],
    'e24_lhvloose_L1EM20VH_5j15noL1'        : [    5,    0,   -1],
    'e24_lhvloose_L1EM20VH_6j15noL1'        : [    5,    0,   -1],
    'e24_medium1_L1EM18VH'                  : [    5,    0,   -1],
    'e24_medium1_L2Star_idperf_L1EM18VH'    : [    5,    0,   -1],
    'e24_medium1_idperf_L1EM18VH'           : [    5,    0,   -1],
    'e24_medium1_iloose_L1EM18VH'           : [    5,    0,   -1],
    'e24_medium1_iloose_L2StarA_L1EM18VH'   : [    5,    0,   -1],
    'e24_medium_L1EM15VH'                   : [    5,    0,   -1],
    'e24_medium_L1EM15VH_g25_medium'        : [    5,    0,   -1],
    'e24_medium_L1EM18VH'                   : [    5,    0,   -1],
    'e24_medium_L2Star_idperf_L1EM20VH'     : [    5,    0,   -1],
    'e24_medium_idperf_L1EM20VH'            : [    5,    0,   -1],
    'e24_medium_iloose_L1EM18VH'            : [    5,    0,   -1],
    'e24_medium_iloose_L1EM20VH'            : [    5,    0,   -1],
    'e24_medium_iloose_L2StarA_L1EM20VH'    : [    5,    0,   -1],
    'e24_tight1_iloose'                     : [    5,    0,   -1],
    'e24_tight1_iloose_L1EM20VH'            : [    5,    0,   -1],
    'e24_tight_L1EM20VH'                    : [    5,    0,   -1],
    'e24_tight_iloose'                      : [    5,    0,   -1],
    'e24_tight_iloose_EtIsEM_L1EM20VH'      : [    5,    0,   -1],
    'e24_tight_iloose_HLTCalo_L1EM20VH'     : [    5,    0,   -1],
    'e24_tight_iloose_L1EM20VH'             : [    5,    0,   -1],
    'e24_tight_iloose_L2EFCalo_L1EM20VH'    : [    5,    0,   -1],
    'e24_vloose_L1EM18VH'                   : [    5,    0,   -1],
    'e24_vloose_L1EM18VH_3j20noL1'          : [    5,    0,   -1],
    'e24_vloose_L1EM18VH_4j20noL1'          : [    5,    0,   -1],
    'e24_vloose_L1EM18VH_5j15noL1'          : [    5,    0,   -1],
    'e24_vloose_L1EM18VH_6j15noL1'          : [    5,    0,   -1],
    'e24_vloose_L1EM20VH'                   : [    5,    0,   -1],
    'e24_vloose_L1EM20VH_3j20noL1'          : [    5,    0,   -1],
    'e24_vloose_L1EM20VH_4j20noL1'          : [    5,    0,   -1],
    'e24_vloose_L1EM20VH_5j15noL1'          : [    5,    0,   -1],
    'e24_vloose_L1EM20VH_6j15noL1'          : [    5,    0,   -1],
    'e25_etcut_L1EM15'                      : [    5,    0,   -1],
    'e25_lhvloose_L1EM15'                   : [    5,    0,   -1],
    'e25_vloose_L1EM15'                     : [    5,    0,   -1],
    'e26_lhtight_iloose'                    : [    5,    0,   -1],
    'e26_lhvloose_L1EM20VH'                 : [    5,    0,   -1],
    'e26_lhvloose_L1EM20VH_3j20noL1'        : [    5,    0,   -1],
    'e26_lhvloose_L1EM20VH_4j20noL1'        : [    5,    0,   -1],
    'e26_lhvloose_L1EM20VH_5j15noL1'        : [    5,    0,   -1],
    'e26_lhvloose_L1EM20VH_6j15noL1'        : [    5,    0,   -1],
    'e26_tight1_iloose'                     : [    5,    0,   -1],
    'e26_tight_iloose'                      : [    5,    0,   -1],
    'e26_vloose_L1EM20VH'                   : [    5,    0,   -1],
    'e26_vloose_L1EM20VH_3j20noL1'          : [    5,    0,   -1],
    'e26_vloose_L1EM20VH_4j20noL1'          : [    5,    0,   -1],
    'e26_vloose_L1EM20VH_5j15noL1'          : [    5,    0,   -1],
    'e26_vloose_L1EM20VH_6j15noL1'          : [    5,    0,   -1],
    'e30_etcut_L1EM15'                      : [    5,    0,   -1],
    'e30_lhvloose_L1EM15'                   : [    5,    0,   -1],
    'e30_vloose_L1EM15'                     : [    5,    0,   -1],
    'e40_etcut_L1EM15'                      : [    5,    0,   -1],
    'e40_lhvloose_L1EM15'                   : [    5,    0,   -1],
    'e40_vloose_L1EM15'                     : [    5,    0,   -1],
    'e50_etcut_L1EM15'                      : [    5,    0,   -1],
    'e50_lhvloose_L1EM15'                   : [    5,    0,   -1],
    'e50_vloose_L1EM15'                     : [    5,    0,   -1],
    'e60_etcut'                             : [    5,    0,   -1],
    'e60_lhloose'                           : [    5,    0,   -1],
    'e60_lhloose_xe60noL1'                  : [    5,    0,   -1],
    'e60_lhmedium'                          : [    5,    0,   -1],
    'e60_lhvloose'                          : [    5,    0,   -1],
    'e60_loose'                             : [    5,    0,   -1],
    'e60_loose_xe60noL1'                    : [    5,    0,   -1],
    'e60_medium'                            : [    5,    0,   -1],
    'e60_medium1'                           : [    5,    0,   -1],
    'e60_vloose'                            : [    5,    0,   -1],
    'e70_lhloose'                           : [    5,    0,   -1],
    'e70_lhloose_xe70noL1'                  : [    5,    0,   -1],
    'e70_lhvloose'                          : [    5,    0,   -1],
    'e70_loose'                             : [    5,    0,   -1],
    'e70_loose_xe70noL1'                    : [    5,    0,   -1],
    'e70_vloose'                            : [    5,    0,   -1],
    'e80_etcut'                             : [    5,    0,   -1],
    'e80_lhvloose'                          : [    5,    0,   -1],
    'e80_vloose'                            : [    5,    0,   -1],
    'e100_etcut'                            : [    5,    0,   -1],
    'e100_lhvloose'                         : [    5,    0,   -1],
    'e100_vloose'                           : [    5,    0,   -1],
    'e120_etcut'                            : [    5,    0,   -1],
    'e120_lhloose'                          : [    5,    0,   -1],
    'e120_lhvloose'                         : [    5,    0,   -1],
    'e120_loose'                            : [    5,    0,   -1],
    'e120_loose1'                           : [    5,    0,   -1],
    'e120_vloose'                           : [    5,    0,   -1],
    'e140_lhloose'                          : [    5,    0,   -1],
    'e140_loose'                            : [    5,    0,   -1],
    '2e12_lhloose_L12EM10VH'                : [    5,    0,   -1],
    '2e12_lhloose_mu10'                     : [    5,    0,   -1],
    '2e12_lhmedium_mu10'                    : [    5,    0,   -1],
    '2e12_loose1_L12EM10VH'                 : [    5,    0,   -1],
    '2e12_loose_L12EM10VH'                  : [    5,    0,   -1],
    '2e12_loose_mu10'                       : [    5,    0,   -1],
    '2e12_medium_mu10'                      : [    5,    0,   -1],
    '2e15_lhloose_L12EM13VH'                : [    5,    0,   -1],
    '2e15_loose_L12EM13VH'                  : [    5,    0,   -1],
    '2e17_lhloose'                          : [    5,    0,   -1],
    '2e17_lhloose_L12EM15'                  : [    5,    0,   -1],
    '2e17_loose'                            : [    5,    0,   -1],
    '2e17_loose1'                           : [    5,    0,   -1],
    '2e17_loose_L12EM15'                    : [    5,    0,   -1],
    'g0_hiptrt_L1EM18VH'                    : [    5,    0,   -1],
    'g0_hiptrt_L1EM20VH'                    : [    5,    0,   -1],
    'g0_hiptrt_L1EM20VHI'                   : [    5,    0,   -1],
    'g0_hiptrt_L1EM22VHLIL'                 : [    5,    0,   -1],
    'g0_hiptrt_L1EM50V'                     : [    5,    0,   -1],
    'g0_perf_L1EM15'                        : [    5,    0,   -1],
    'g0_perf_L1EM3'                         : [    5,    0,   -1],
    'g10_loose'                             : [    5,    0,   -1],
    'g10_loose_L1EM7'                       : [    5,    0,   -1],
    'g10_loose_mu10'                        : [    5,    0,   -1],
    'g10_loose_mu10_taumass'                : [    5,    0,   -1],
    'g10_medium'                            : [    5,    0,   -1],
    'g10_medium_L1EM7'                      : [    5,    0,   -1],
    'g15_loose'                             : [    5,    0,   -1],
    'g15_loose_2mu10_msonly'                : [    5,    0,   -1],
    'g15_loose_L1EM3'                       : [    5,    0,   -1],
    'g15_loose_L1EM7'                       : [    5,    0,   -1],
    'g20_etcut_L1EM12'                      : [    5,    0,   -1],
    'g20_loose'                             : [    5,    0,   -1],
    'g20_loose_2g15_loose_L12EM13VH'        : [    5,    0,   -1],
    'g20_loose_L1EM12'                      : [    5,    0,   -1],
    'g20_loose_L1EM15'                      : [    5,    0,   -1],
    'g20_loose_mu4'                         : [    5,    0,   -1],
    'g20_loose_mu4_taumass'                 : [    5,    0,   -1],
    'g20_tight'                             : [    5,    0,   -1],
    'g25_loose'                             : [    5,    0,   -1],
    'g25_loose_L1EM15'                      : [    5,    0,   -1],
    'g25_medium'                            : [    5,    0,   -1],
    'g25_medium_mu24'                       : [    5,    0,   -1],
    'g35_loose'                             : [    5,    0,   -1],
    'g35_loose_L1EM15'                      : [    5,    0,   -1],
    'g35_loose_L1EM15_g25_loose_L1EM15'     : [    5,    0,   -1],
    'g35_loose_g25_loose'                   : [    5,    0,   -1],
    'g35_medium'                            : [    5,    0,   -1],
    'g35_medium_HLTCalo_g25_medium_HLTCalo' : [    5,    0,   -1],
    'g35_medium_g25_medium'                 : [    5,    0,   -1],
    'g40_loose_L1EM15'                      : [    5,    0,   -1],
    'g40_tight'                             : [    5,    0,   -1],
    'g40_tight_xe40noL1'                    : [    5,    0,   -1],
    'g45_loose_L1EM15'                      : [    5,    0,   -1],
    'g45_tight'                             : [    5,    0,   -1],
    'g45_tight_L1EM22VHLIL_xe45noL1'        : [    5,    0,   -1],
    'g45_tight_xe45noL1'                    : [    5,    0,   -1],
    'g50_loose'                             : [    5,    0,   -1],
    'g50_loose_L1EM15'                      : [    5,    0,   -1],
    'g60_loose'                             : [    5,    0,   -1],
    'g60_loose_L1EM15VH'                    : [    5,    0,   -1],
    'g60_loose_xe60noL1'                    : [    5,    0,   -1],
    'g70_loose'                             : [    5,    0,   -1],
    'g70_loose_xe70noL1'                    : [    5,    0,   -1],
    'g80_loose'                             : [    5,    0,   -1],
    'g100_loose'                            : [    5,    0,   -1],
    'g120_loose'                            : [    5,    0,   -1],
    'g120_loose_HLTCalo'                    : [    5,    0,   -1],
    'g140_loose'                            : [    5,    0,   -1],
    '2g10_loose_mu20'                       : [    5,    0,   -1],
    '2g10_medium_mu20'                      : [    5,    0,   -1],
    '2g20_loose_L12EM15'                    : [    5,    0,   -1],
    '2g20_loose_g15_loose'                  : [    5,    0,   -1],
    '2g20_tight'                            : [    5,    0,   -1],
    '2g50_loose'                            : [    5,    0,   -1],
    '2g60_loose_L12EM15VH'                  : [    5,    0,   -1],
    '3g15_loose'                            : [    5,    0,   -1],
    '3g20_loose'                            : [    5,    0,   -1],
    'j45_boffperf_3j45'                     : [    5,    0,   -1],
    'j45_boffperf_3j45_L13J15.0ETA24'       : [    5,    0,   -1],
    'j45_boffperf_3j45_L13J20'              : [    5,    0,   -1],
    'j45_bperf_3j45'                        : [    5,    0,   -1],
    'j45_bperf_3j45_L13J15.0ETA24'          : [    5,    0,   -1],
    'j45_bperf_3j45_L13J20'                 : [    5,    0,   -1],
    'j45_bperf_split_3j45'                  : [    5,    0,   -1],
    'j45_bperf_split_3j45_L13J150ETA24'     : [    5,    0,   -1],
    'j45_bperf_split_3j45_L13J20'           : [    5,    0,   -1],
    'j55_boffperf'                          : [    5,    0,   -1],
    'j55_bperf'                             : [    5,    0,   -1],
    'j55_bperf_split'                       : [    5,    0,   -1],
    'j65_btight_3j65_L13J25.0ETA22'         : [    5,    0,   -1],
    'j65_btight_3j65_L14J15.0ETA24'         : [    5,    0,   -1],
    'j65_btight_split_3j65_L13J25.0ETA22'   : [    5,    0,   -1],
    'j65_btight_split_3j65_L14J15.0ETA24'   : [    5,    0,   -1],
    'j70_bmedium_3j70_L13J25.0ETA22'        : [    5,    0,   -1],
    'j70_bmedium_3j70_L14J15.0ETA24'        : [    5,    0,   -1],
    'j70_bmedium_split_3j70_L13J25.0ETA22'  : [    5,    0,   -1],
    'j70_bmedium_split_3j70_L14J15.0ETA24'  : [    5,    0,   -1],
    'j70_btight_3j70'                       : [    5,    0,   -1],
    'j70_btight_split_3j70'                 : [    5,    0,   -1],
    'j75_bmedium_3j75'                      : [    5,    0,   -1],
    'j75_bmedium_split_3j75'                : [    5,    0,   -1],
    'j150_bmedium_j50_bmedium'              : [    5,    0,   -1],
    'j150_bmedium_split_j50_bmedium_split'  : [    5,    0,   -1],
    'j175_bmedium'                          : [    5,    0,   -1],
    'j175_bmedium_j60_bmedium'              : [    5,    0,   -1],
    'j175_bmedium_split'                    : [    5,    0,   -1],
    'j175_bmedium_split_j60_bmedium_split'  : [    5,    0,   -1],
    'j225_bloose'                           : [    5,    0,   -1],
    'j225_bloose_split'                     : [    5,    0,   -1],
    'j300_bloose'                           : [    5,    0,   -1],
    'j300_bloose_split'                     : [    5,    0,   -1],
    '2j35_btight_2j35_L13J25.0ETA22'        : [    5,    0,   -1],
    '2j35_btight_2j35_L14J15.0ETA24'        : [    5,    0,   -1],
    '2j35_btight_split_2j35_L13J25.0ETA22'  : [    5,    0,   -1],
    '2j35_btight_split_2j35_L14J15.0ETA24'  : [    5,    0,   -1],
    '2j45_bmedium_2j45_L13J25.0ETA22'       : [    5,    0,   -1],
    '2j45_bmedium_2j45_L14J15.0ETA24'       : [    5,    0,   -1],
    '2j45_bmedium_split_2j45_L13J25.0ETA22' : [    5,    0,   -1],
    '2j45_bmedium_split_2j45_L14J15.0ETA24' : [    5,    0,   -1],
    '2j45_btight_2j45'                      : [    5,    0,   -1],
    '2j45_btight_split_2j45'                : [    5,    0,   -1],
    '2j55_bmedium_2j55'                     : [    5,    0,   -1],
    '2j55_bmedium_j100'                     : [    5,    0,   -1],
    '2j55_bmedium_split_2j55'               : [    5,    0,   -1],
    '2j55_bmedium_split_j100'               : [    5,    0,   -1],
    '2j65_btight_j65'                       : [    5,    0,   -1],
    '2j65_btight_split_j65'                 : [    5,    0,   -1],
    '2j70_bmedium_j70'                      : [    5,    0,   -1],
    '2j70_bmedium_split_j70'                : [    5,    0,   -1],
    '2j70_btight_j70'                       : [    5,    0,   -1],
    '2j70_btight_split_j70'                 : [    5,    0,   -1],
    '2j75_bmedium_j75'                      : [    5,    0,   -1],
    '2j75_bmedium_split_j75'                : [    5,    0,   -1],
    })
######################################################



######################################################
Prescales.L1Prescales_upgrade_mc_prescale  = deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_upgrade_mc_prescale = deepcopy(Prescales.HLTPrescales_tight_mc_prescale)
# Note: "upgrade" prescales are set with regular expressions at the end
#       of the setupMenu() function above 
######################################################
