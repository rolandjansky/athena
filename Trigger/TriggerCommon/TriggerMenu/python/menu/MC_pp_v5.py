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


    #-----------------------------------------------------
    # remove some streamers from Physics_pp_v5 for MC menu
    #-----------------------------------------------------
    # list of chains to remove
    chainsToRemove = ['noalg_L1EM20A',
                      'noalg_L1EM20C',
                      'noalg_L1J75A',
                      'noalg_L1J75C',
                      'noalg_L1ALFA_ANY',
                      'noalg_mb_L1MBTS_4_A',
                      'noalg_mb_L1MBTS_4_C',
                      'noalg_L1LFV-MU',
                      'noalg_L1DY-DR-2MU4',
                      'noalg_L1BPH-DR-2MU4',
                      'noalg_L1BPH-DR-2MU6',
                      'noalg_L1BPH-DR-MU6MU4',
                      'noalg_L1BPH-2M-2MU4',
                      'noalg_L1BPH-2M-2MU6',
                      'noalg_L1BPH-2M-MU6MU4',
                      'noalg_L1BPH-4M8-2MU4',
                      'noalg_L1BPH-4M8-2MU6',
                      'noalg_L1BPH-4M8-MU6MU4',
                      'noalg_L1DY-BOX-2MU4',
                      'noalg_L1DY-BOX-MU6MU4',
                      'noalg_L1DY-BOX-2MU6',
                      'noalg_L1DR-TAU20ITAU12I',
                      'noalg_L1DR-MU10TAU12I',
                      'e0_perf_L1EM3HI',
                      'g0_perf_L1EM3HI',
                      'e3_loose_L2EFCalo',
                      'e3_lhloose_L2EFCalo', 
                      'e3_loose_HLTCalo', 
                      'e3_lhloose_HLTCalo', 
                      'e3_loose_etisem', 
                      'g3_loose_HLTCalo',
                      'g40_loose_larpeb',
                      'g60_loose_larpeb',
                      'g80_loose_larpeb',
                      ]

    # remove all L1Topo streamers
    allStreamers = TriggerFlags.StreamingSlice.signatures()
    chainIndices = []
    for chainlist in allStreamers:
        #remove the L1Topo Streamers
        if '-' in chainlist[0]:
            chainIndices.append(allStreamers.index(chainlist))            
        else:
            for chain in chainsToRemove:
                if chainlist[0] == chain:
                    chainIndices.append(allStreamers.index(chainlist))                           

    log.info("Chains to not import from Physics to MC_pp_v5 :")
    for index in reversed(chainIndices):
        log.info("  removing  %s " % allStreamers[index][0])
        del allStreamers[index]

    # remove all chains in the list below 
    egammaChains = TriggerFlags.EgammaSlice.signatures()
            
    chainIndicesEgammaChains = []
    for egchain in egammaChains:
        for chain in chainsToRemove:
            if egchain[0] == chain:
                chainIndicesEgammaChains.append(egammaChains.index(egchain))                           

    for index in reversed(chainIndicesEgammaChains):
        log.info("  removing  %s " % egammaChains[index])
        del egammaChains[index]



    # ---------------------------------------------------------------------------------------
    # INPUT FORMAT FOR CHAINS:
    # ['chainName',  'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep], OPTIONAL: [mergingStrategy, offset,[merginOrder] ]], topoStartsFrom = False
    # ----------------------------------------------------------------------------------------

    TriggerFlags.TestSlice.signatures = TriggerFlags.TestSlice.signatures() + [
        #['TestChain400',    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ]

    TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
        ['mu0_perf',           'L1_MU4',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20_2mu0noL1_JpsimumuFS', 'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','2mu0noL1_JpsimumuFS']]], 
        ['mu18_2mu0noL1_JpsimumuFS', 'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu18','2mu0noL1_JpsimumuFS']]],

        ['mu20_2mu4_JpsimumuL2', 'L1_MU20', ['L1_MU20','L1_2MU4'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['2mu4_JpsimumuL2','mu20']]], 
        ['mu18_2mu4_JpsimumuL2', 'L1_MU15', ['L1_MU15','L1_2MU4'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['2mu4_JpsimumuL2','mu18']]],
        
        # Primary (single muon chains)
        ['mu26_imedium',          'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu50',                  'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu60_0eta105_msonly',   'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],

        # Primary at low lumi (single muon chains)
        ['mu24_imedium',           'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_iloose_L1MU15',     'L1_MU15',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20_iloose_L1MU15',     'L1_MU15',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],

        # Primary (multi muon chains)
        ['2mu14',                  'L1_2MU10',          [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu6',                   'L1_3MU6',           [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu6_msonly',            'L1_3MU6',           [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],

        # Primary at low lumi (multi muon chains)
        ['2mu10',                  'L1_2MU10',          [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],

        # Primary (full scan chains)
        ['mu24_mu8noL1',           'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','mu8noL1']]],
        ['mu24_2mu4noL1',          'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','2mu4noL1']]],

        # Primary at low lumi (full scan chains)
        ['mu22_mu8noL1',           'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','mu8noL1']]],
        ['mu22_2mu4noL1',          'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','2mu4noL1']]],
        ['mu20_mu8noL1',           'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','mu8noL1']]],
        ['mu20_2mu4noL1',          'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','2mu4noL1']]],
        ['mu18_mu8noL1',           'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu18','mu8noL1']]],
        ['mu18_2mu4noL1',          'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu18','2mu4noL1']]],

        # Supporting chains
        ['mu26',                   'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_L1MU15',            'L1_MU15',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu22',                   'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20',                   'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20_L1MU15',            'L1_MU15',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20_r1extr',            'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10_r1extr',            'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu4_r1extr',             'L1_MU4',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],

        # Rerun chains
        ['mu24',                   'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_idperf',            'L1_MU20',           [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon'], -1],
        ['mu20_idperf',            'L1_MU20',           [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon'], -1],
        ['mu20_msonly',            'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu18',                   'L1_MU15',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu14',                   'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10',                   'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10_idperf',            'L1_MU10',           [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon'], -1],
        ['mu10_msonly',            'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu6',                    'L1_MU6',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu6_idperf',             'L1_MU6',            [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon'], -1],
        ['mu6_msonly',             'L1_MU6',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu4',                    'L1_MU4',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu4_idperf',             'L1_MU4',            [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon'], -1],
        ['mu4_msonly',             'L1_MU4',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],

        # Additional chains
        ['mu24_L2Star_idperf',      'L1_MU20',  [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:IDMonitoring'], -1],
        ['mu6_L2Star_idperf',       'L1_MU6',   [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:IDMonitoring'], -1],
        ['mu0noL1_fsperf',          '',         [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'],-1],
        ['mu14_iloose_mu14',        'L1_2MU10', [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu6',                    'L1_2MU6',  [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu4',                    'L1_2MU4',  [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['mu20_imedium_mu8noL1',    'L1_MU20',  ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_imedium','mu8noL1']]],
        ['mu20_iloose_mu8noL1',     'L1_MU20',  ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_iloose','mu8noL1']]],
        ['mu20_iloose_2mu4noL1',    'L1_MU20',  ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_iloose','2mu4noL1']]],

        # T&P chains
        #['mu24_imedium_mu20noL1_msonly',  'L1_MU20', [], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'],-1],        
        #['mu24_imedium_mu20noL1_msonly',  'L1_MU20', [], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'],-1],       

        # Primary (narrow scan chains)
        ['mu11_2mu4noL1_nscan03_L1MU11_2MU6', 'L1_MU11_2MU6', ['L1_MU11_2MU6',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11','2mu4noL1_nscan03']]],        
        ['mu11_2mu4noL1_nscan03_L1MU10_2MU6', 'L1_MU10_2MU6', ['L1_MU10_2MU6',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11','2mu4noL1_nscan03']]],        
        ['mu20_msonly_mu6noL1_msonly_nscan05','L1_MU20',     ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu6noL1_msonly_nscan05']]],
        ['mu11_2mu4noL1_nscan03', 'L1_LFV-MU', ['L1_MU10_2MU6',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11','2mu4noL1_nscan03']],False],
 
        ['mu11_2mu4noL1_nscan03_L1MU11_LFV-MU', 'L1_LFV-MU', ['L1_MU11_2MU6',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu11','2mu4noL1_nscan03']],False],
        ]

    TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + [

        ['j460_a10_sub_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_a4_sub', 'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_a4_jes', 'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],        
        ['3j175',            'L1_J100', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['ht1000_L1J100',    'L1_J100', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j30_muvtx',        'L1_2MU10',[], [PhysicsStream], ['RATE:MuonJet','BW:Muon'], -1],

        # LLP trigger (ATR-9673)
        ['j30_llp_L1TAU30',                'L1_TAU30',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j30_llp_L1LLP-RO',                 'L1_LLP-RO',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j30_llp_L1LLP-NOMATCH',              'L1_LLP-NOMATCH',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        #Partial scan
        ['j200_jes_PS', 'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],         
        ['3j175_jes_PS','L1_J100', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],


        # new chains
        ['j55',                    'L1_J15',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j60',                    'L1_J15',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j85',                    'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j85_jes',                'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j85_lcw',                'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j85_lcw_jes',            'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j100',                   'L1_J25',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j110',                   'L1_J30',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j150',                   'L1_J40',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j175',                   'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j175_jes',               'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j175_lcw',               'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j175_lcw_jes',           'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j200',                   'L1_J50', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j260',                   'L1_J75', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j300',                   'L1_J85', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j320',                   'L1_J85', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j360',                   'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j380',                   'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j380_jes',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j380_lcw',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j380_lcw_jes',           'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400',                   'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_sub',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_jes',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_lcw',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_lcw_jes',           'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420',                   'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_jes',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_lcw',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_lcw_jes',           'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j440',                   'L1_J120', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j460',               
    'L1_J120', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        #multi-jet
        ['4j45',                    'L1_3J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['4j85',                    'L1_3J40', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['4j85_jes',                'L1_3J40', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['4j85_lcw',                'L1_3J40', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['4j85_lcw_jes',            'L1_3J40', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['4j100',                   'L1_3J50', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j45',                    'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j55',                    'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j60',                    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j70',                    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j85',                    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j85_jes',                'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j85_lcw',                'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j85_lcw_jes',            'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],

        ['6j45_0eta240',                  'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['6j45_0eta240_L14J20',           'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['6j45_0eta240_L15J150ETA24',     'L1_5J15.0ETA24', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],

        ['6j50_0eta240_L14J20',           'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['6j50_0eta240_L15J150ETA24',     'L1_5J15.0ETA24', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],

        ['6j55_0eta240_L14J20',           'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['6j55_0eta240_L15J150ETA24',     'L1_5J15.0ETA24', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],

        ['7j45',                          'L1_6J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['7j45_L14J20',                   'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['7j45_0eta240_L14J20',           'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['7j45_0eta240_L15J150ETA24',     'L1_5J15.0ETA24', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],

        #Forward jets
        #['j60_240eta490',          'L1_J15.23ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j60_280eta320',          'L1_J15.29ETA32', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j60_320eta490',          'L1_J15.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j85_280eta320',           'L1_J20.29ETA32', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j85_280eta320_jes',       'L1_J20.29ETA32', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j85_280eta320_lcw',       'L1_J20.29ETA32', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j85_280eta320_lcw_jes',    'L1_J20.29ETA32', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j85_320eta490',           'L1_J20.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j110_320eta490',          'L1_J30.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j175_320eta490',           'L1_J50.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j175_320eta490_jes',       'L1_J50.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j175_320eta490_lcw',       'L1_J50.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j175_320eta490_lcw_jes',    'L1_J50.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
                                                                                   
        ['j260_320eta490',           'L1_J75.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j260_320eta490_jes',       'L1_J75.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j260_320eta490_lcw',       'L1_J75.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j260_320eta490_lcw_jes',    'L1_J75.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j360_320eta490',           'L1_J100.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_320eta490_jes',       'L1_J100.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_320eta490_lcw',       'L1_J100.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_320eta490_lcw_jes',    'L1_J100.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j85_nojcalib',       'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j85_lcw_nojcalib',        'L1_J20',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j175_nojcalib',           'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j175_lcw_nojcalib',        'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j380_nojcalib',           'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j380_lcw_nojcalib',        'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j400_nojcalib',           'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j300_lcw_nojcalib',       'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_nojcalib',           'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j420_lcw_nojcalib',       'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        #multi-jet
        ['4j85_nojcalib',           'L1_3J40', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['4j85_lcw_nojcalib',        'L1_3J40', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j85_nojcalib',            'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j85_lcw_nojcalib',        'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        #Forward jets
        ['j85_280eta320_nojcalib',         'L1_J20.29ETA32', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j85_280eta320_lcw_nojcalib',     'L1_J20.29ETA32', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j175_320eta490_nojcalib',        'L1_J50.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j175_320eta490_lcw_nojcalib',    'L1_J50.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j260_320eta490_nojcalib',        'L1_J75.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j260_320eta490_lcw_nojcalib',    'L1_J75.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_320eta490_nojcalib',        'L1_J100.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_320eta490_lcw_nojcalib',    'L1_J100.32ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],


        
        ## Single jets seeded from RD0        
        ['j15',            'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j25',            'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j35',            'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j35_jes',        'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j35_lcw',        'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j35_lcw_jes',    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j45_L1RD0',      'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j55_L1RD0',      'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j60_L1RD0',      'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j85_L1RD0',      'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['4j25',      'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['5j25',      'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['6j25',      'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],        
        ['7j25',      'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],      

        ['j35_nojcalib',         'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j35_lcw_nojcalib',     'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ##HT seeded chains
        ['j360_a10_nojcalib'                      ,      'L1_HT150-J20s5.ETA30',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_a10_nojcalib_L1HT150-J20.ETA30',      'L1_HT150-J20.ETA30', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_a10_lcw_nojcalib'                      ,  'L1_HT150-J20s5.ETA30',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_a10_lcw_nojcalib_L1HT150-J20.ETA30',  'L1_HT150-J20.ETA30', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j460_a10_nojcalib'                          ,  'L1_HT190-J15s5.ETA20',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j460_a10_nojcalib_L1HT190-J15.ETA20'    ,  'L1_HT190-J15.ETA20',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j460_a10_lcw_nojcalib'                      ,  'L1_HT190-J15s5.ETA20',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j460_a10_lcw_nojcalib_L1HT190-J15.ETA20',  'L1_HT190-J15.ETA20',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
  


        ##HT seeded chains
        ['j360_a10_sub'                      ,  'L1_HT150-J20s5.ETA30',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_a10_sub_L1HT150-J20.ETA30',  'L1_HT150-J20.ETA30', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j360_a10_lcw_sub'                      , 'L1_HT150-J20s5.ETA30',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j360_a10_lcw_sub_L1HT150-J20.ETA30', 'L1_HT150-J20.ETA30', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j460_a10_sub'                          , 'L1_HT190-J15s5.ETA20',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j460_a10_sub_L1HT190-J15.ETA20'    , 'L1_HT190-J15.ETA20',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],

        ['j460_a10_lcw_sub'                      , 'L1_HT190-J15s5.ETA20',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        ['j460_a10_lcw_sub_L1HT190-J15.ETA20', 'L1_HT190-J15.ETA20',    [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jets'], -1],
        
        ['ht400'                       ,    'L1_HT150-J20s5.ETA30',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['ht400_L1HT150-J20.ETA30' ,    'L1_HT150-J20.ETA30', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],

        ['ht550'                       ,    'L1_HT150-J20s5.ETA30',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['ht550_L1HT150-J20.ETA30' ,    'L1_HT150-J20.ETA30', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],

        ['ht700'                          , 'L1_HT190-J15s5.ETA20',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['ht700_L1HT190-J15.ETA20'    , 'L1_HT190-J15.ETA20',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],

        ['ht850'                          , 'L1_HT190-J15s5.ETA20',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['ht850_L1HT190-J15.ETA20'    , 'L1_HT190-J15.ETA20',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],

        ['ht1000'                          , 'L1_HT190-J15s5.ETA20',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],
        ['ht1000_L1HT190-J15.ETA20'    , 'L1_HT190-J15.ETA20',    [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jets'], -1],

        # VBF triggers
        ['2j40_0eta490_invm250_L1XE55', 'L1_XE55',         [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['2j40_0eta490_invm250',        'L1_XE35_MJJ-200', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jets'], -1],


        #reclustering chains
        ['j360_a10r', 'L1_HT190-J15s5.ETA20', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j460_a10r', 'L1_HT150-J20s5.ETA30', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j460_a10r_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        ]

    TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [
        ## All chains
        ['2j75_bmedium_j75', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j75_bperf_j75', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j70_btight_j70', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j70_bperf_j70', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['j100_2j55_bmedium', 'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['j100_2j55_bperf', 'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bmedium_2j55', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bperf_2j55', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j45_btight_2j45', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j45_bperf_2j45', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['j75_bmedium_3j75', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j75_bperf_3j75', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j70_btight_3j70', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j70_bperf_3j70', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['2j55_bmedium_2j55_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bperf_2j55_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j45_btight_2j45_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j45_bperf_2j45_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['j75_bmedium_3j75_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j75_bperf_3j75_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j70_btight_3j70_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j70_bperf_3j70_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j175_bmedium_j60_bmedium', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['j175_bperf_j60_bperf', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['j300_bloose', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j300_bperf', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],

        # low lumi menu
        ['2j70_bmedium_j70', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j65_btight_j65', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j65_bperf_j65', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j45_bmedium_2j45_L14J15.0ETA24', 'L1_4J15.0ETA24', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j45_bperf_2j45_L14J15.0ETA24', 'L1_4J15.0ETA24', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j35_btight_2j35_L14J15.0ETA24', 'L1_4J15.0ETA24', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j35_bperf_2j35_L14J15.0ETA24', 'L1_4J15.0ETA24', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['j70_bmedium_3j70_L14J15.0ETA24', 'L1_4J15.0ETA24', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j70_bperf_3j70_L14J15.0ETA24', 'L1_4J15.0ETA24', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j65_btight_3j65_L14J15.0ETA24', 'L1_4J15.0ETA24', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j65_bperf_3j65_L14J15.0ETA24', 'L1_4J15.0ETA24', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['2j45_bmedium_2j45_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j45_bperf_2j45_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j35_btight_2j35_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j35_bperf_2j35_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['j70_bmedium_3j70_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j70_bperf_3j70_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j65_btight_3j65_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j65_bperf_3j65_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j225_bloose', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j225_bperf', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j150_bmedium_j50_bmedium', 'L1_J100', [], [PhysicsStream], ['RATE:MultBJet', 'BW:Jets'], -1],
        ['j150_bperf_j50_bperf', 'L1_J100', [], [PhysicsStream], ['RATE:MultBJet', 'BW:Jets'], -1],
        ['j175_bmedium', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j175_bperf', 'L1_J50', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],

        # boffperf 
        ['j55_boffperf', 'L1_J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j45_boffperf_3j45', 'L1_3J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j45_boffperf_3j45_L13J15.0ETA24', 'L1_3J15.0ETA24', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j45_boffperf_3j45_L13J20', 'L1_3J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        
        ### split configuration
        # main menu
        ['2j75_bmedium_split_j75', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j75_bperf_split_j75', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j70_btight_split_j70', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j70_bperf_split_j70', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['j100_2j55_bmedium_split', 'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['j100_2j55_bperf_split', 'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bmedium_split_2j55', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bperf_split_2j55', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j45_btight_split_2j45', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j45_bperf_split_2j45', 'L1_4J20', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['j75_bmedium_split_3j75', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j75_bperf_split_3j75', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j70_btight_split_3j70', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j70_bperf_split_3j70', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['2j55_bmedium_split_2j55_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bperf_split_2j55_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j45_btight_split_2j45_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j45_bperf_split_2j45_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['j75_bmedium_split_3j75_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j75_bperf_split_3j75_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j70_btight_split_3j70_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j70_bperf_split_3j70_L13J25.ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j175_bmedium_split_j60_bmedium_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['j175_bperf_split_j60_bperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['j300_bloose_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j300_bperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        
        # low lumi menu
        ['2j70_bmedium_split_j70', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j65_btight_split_j65', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j65_bperf_split_j65', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j45_bmedium_split_2j45_L14J15.0ETA24', 'L1_4J15.0ETA24', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j45_bperf_split_2j45_L14J15.0ETA24', 'L1_4J15.0ETA24', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j35_btight_split_2j35_L14J15.0ETA24', 'L1_4J15.0ETA24', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j35_bperf_split_2j35_L14J15.0ETA24', 'L1_4J15.0ETA24', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['j70_bmedium_split_3j70_L14J15.0ETA24', 'L1_4J15.0ETA24', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j70_bperf_split_3j70_L14J15.0ETA24', 'L1_4J15.0ETA24', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j65_btight_split_3j65_L14J15.0ETA24', 'L1_4J15.0ETA24', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j65_bperf_split_3j65_L14J15.0ETA24', 'L1_4J15.0ETA24', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['2j45_bmedium_split_2j45_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j45_bperf_split_2j45_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j35_btight_split_2j35_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j35_bperf_split_2j35_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['j70_bmedium_split_3j70_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j70_bperf_split_3j70_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j65_btight_split_3j65_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j65_bperf_split_3j65_L13J25.0ETA23', 'L1_3J25.0ETA23', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j225_bloose_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j225_bperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j150_bmedium_split_j50_bmedium_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['j150_bperf_split_j50_bperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['j175_bmedium_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j175_bperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],

        
        #VBF triggers
        
        ['2j55_bloose', 'L1_J30_2J20_4J20.0ETA49_MJJ-400', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bperf', 'L1_J30_2J20_4J20.0ETA49_MJJ-400', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-700', 'L1_J30_2J20_4J20.0ETA49_MJJ-700', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-700', 'L1_J30_2J20_4J20.0ETA49_MJJ-700', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-800', 'L1_J30_2J20_4J20.0ETA49_MJJ-800', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-800', 'L1_J30_2J20_4J20.0ETA49_MJJ-800', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-900', 'L1_J30_2J20_4J20.0ETA49_MJJ-900', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-900', 'L1_J30_2J20_4J20.0ETA49_MJJ-900', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        
        ['2j55_bloose_L13J20_4J20.0ETA49_MJJ-400', 'L1_3J20_4J20.0ETA49_MJJ-400', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bperf_L13J20_4J20.0ETA49_MJJ-400', 'L1_3J20_4J20.0ETA49_MJJ-400', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bloose_L13J20_4J20.0ETA49_MJJ-700', 'L1_3J20_4J20.0ETA49_MJJ-700', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bperf_L13J20_4J20.0ETA49_MJJ-700', 'L1_3J20_4J20.0ETA49_MJJ-700', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bloose_L13J20_4J20.0ETA49_MJJ-800', 'L1_3J20_4J20.0ETA49_MJJ-800', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bperf_L13J20_4J20.0ETA49_MJJ-800', 'L1_3J20_4J20.0ETA49_MJJ-800', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bloose_L13J20_4J20.0ETA49_MJJ-900', 'L1_3J20_4J20.0ETA49_MJJ-900', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bperf_L13J20_4J20.0ETA49_MJJ-900', 'L1_3J20_4J20.0ETA49_MJJ-900', [''], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],

        ['2j55_bmedium', 'L1_3J20_4J20.0ETA49_MJJ-400', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bmedium_L13J20_4J20.0ETA49_MJJ-700', 'L1_3J20_4J20.0ETA49_MJJ-700', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bmedium_L13J20_4J20.0ETA49_MJJ-800', 'L1_3J20_4J20.0ETA49_MJJ-800', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bmedium_L13J20_4J20.0ETA49_MJJ-900', 'L1_3J20_4J20.0ETA49_MJJ-900', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-400', 'L1_J30_2J20_4J20.0ETA49_MJJ-400', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bperf_L1J30_2J20_4J20.0ETA49_MJJ-400', 'L1_J30_2J20_4J20.0ETA49_MJJ-400', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-700', 'L1_J30_2J20_4J20.0ETA49_MJJ-700', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-800', 'L1_J30_2J20_4J20.0ETA49_MJJ-800', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ['2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-900', 'L1_J30_2J20_4J20.0ETA49_MJJ-900', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],


        #bperf
        ['j45_bperf_3j45', 'L1_3J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j45_bperf_3j45_L14J20', 'L1_3J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j45_bperf_3j45_L13J15.0ETA24', 'L1_3J15.0ETA24', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j45_bperf_3j45_L13J20', 'L1_3J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j55_bperf_3j55', 'L1_4J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j15_bperf', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j25_bperf', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j35_bperf', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j45_bperf', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j55_bperf', 'L1_J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j85_bperf', 'L1_J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j110_bperf', 'L1_J30', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j150_bperf', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j260_bperf', 'L1_J75', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j320_bperf', 'L1_J85', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j400_bperf', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j55_bperf_split', 'L1_J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j45_bperf_split_3j45', 'L1_3J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j45_bperf_split_3j45_L13J150ETA24', 'L1_3J15.0ETA24', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j45_bperf_split_3j45_L13J20', 'L1_3J20', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        
        # boffperf chains
        ### ['j55_boffperf_split',                       'L1_J20',[],  [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ### ['j45_boffperf_split_3j45',                  'L1_3J15',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ### ['j45_boffperf_split_3j45_L13J150ETA24',     'L1_3J15.0ETA24',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ### ['j45_boffperf_split_3j45_L13J20',           'L1_3J20',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
       
        ### the existing chains        
        ### ['j175_bmedium',                 'L1_J100',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ### ['j75_bmedium_3j75',             'L1_4J20',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ### ['2j55_bmedium_2j55',            'L1_4J20',[], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ### ['2j45_bmedium_3j45',            'L1_5J15.0ETA24',[], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ### ['j175_bmedium_j60_bmedium',     'L1_J100',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],        
        ### ['j300_bloose',                  'L1_J100',[], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ###
        ### ['j175_bmedium_split',                    'L1_J100',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ### ['j75_bmedium_split_3j75',                'L1_4J20',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ### ['2j55_bmedium_split_2j55',               'L1_4J20',[], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ### ['2j45_bmedium_split_3j45',               'L1_5J15.0ETA24',[], [PhysicsStream], ['RATE:MultiBJet', 'BW:Jets'], -1],
        ###['j175_bmedium_split_j60_bmedium_split',  'L1_J100',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ### ['j300_bloose_split',                     'L1_J100',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        # split configuration:
        ### ['j55_bperf_split',                       'L1_J20',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ### ['j45_bperf_split_3j45',                  'L1_3J15',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ### ['j45_bperf_split_3j45_L13J150ETA24',     'L1_3J15.0ETA24',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ### ['j45_bperf_split_3j45_L13J20',           'L1_3J20',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ]

    TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + [ 

        ##add muon corrections

        ['xe35_wEFMu',                    'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_tc_lcw_wEFMu',             'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_tc_em_wEFMu',              'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_mht_wEFMu',                'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_pueta_wEFMu',              'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_pufit_wEFMu',              'L1_XE35', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe60_wEFMu',                    'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_tc_lcw_wEFMu',             'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_tc_em_wEFMu',              'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_mht_wEFMu',                'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_pueta_wEFMu',              'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_pufit_wEFMu',              'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe70_wEFMu',                    'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_tc_lcw_wEFMu',             'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_tc_em_wEFMu',              'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_mht_wEFMu',                'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_pueta_wEFMu',              'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_pufit_wEFMu',              'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ####defaul chains

        ['xe60',                     'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_tc_lcw',              'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_tc_em',               'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_mht',                 'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_pueta',               'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe60_pufit',               'L1_XE40', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe70',                     'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_tc_lcw',              'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_tc_em',               'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_mht',                 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_pueta',               'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe70_pufit',               'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe80_L1XE50',              'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw_L1XE50',       'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em_L1XE50',        'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht_L1XE50',          'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta_L1XE50',        'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit_L1XE50',        'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe80_wEFMu_L1XE50',        'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw_wEFMu_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em_wEFMu_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht_wEFMu_L1XE50',    'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta_wEFMu_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit_wEFMu_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe80',                     'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw',              'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em',               'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht',                 'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta',               'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit',               'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe80_wEFMu',               'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw_wEFMu',        'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em_wEFMu',         'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht_wEFMu',           'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta_wEFMu',         'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit_wEFMu',         'L1_XE60', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe80_L1XE70',              'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw_L1XE70',       'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em_L1XE70',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht_L1XE70',          'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta_L1XE70',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit_L1XE70',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe80_wEFMu_L1XE70',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_lcw_wEFMu_L1XE70', 'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_tc_em_wEFMu_L1XE70',  'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_mht_wEFMu_L1XE70',    'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pueta_wEFMu_L1XE70',  'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe80_pufit_wEFMu_L1XE70',  'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe100',                    'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_lcw',             'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_em',              'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_mht',                'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pueta',              'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pufit',              'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe100_wEFMu',              'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_lcw_wEFMu',       'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_tc_em_wEFMu',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_mht_wEFMu',          'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pueta_wEFMu',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_pufit_wEFMu',        'L1_XE70', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        # Rerun chains
        ['xe0noL1_l2fsperf',         '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_tc_lcw',  '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_tc_em',   '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_mht',     '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_pueta',   '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_pufit',   '',        [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ]

    TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [
        # Energy calibration chain
        ['tau5_perf_ptonly_L1TAU8',                'L1_TAU8', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau0_perf_ptonly_L1TAU12',               'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau0_perf_ptonly_L1TAU60',               'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        # Run-I comparison
        ['tau20_r1medium1',                        'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau20_r1perf',                           'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau20_r1_idperf',                        'L1_TAU12', [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Tau'], -1],
        ['tau25_r1perf',                           'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau29_r1medium1',                        'L1_TAU20', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau29_r1perf',                           'L1_TAU20', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_r1medium1',                       'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_r1perf',                          'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        # Run-II - No BDT: main track-based items
        ['tau25_perf_tracktwo',                    'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_tracktwo_L1TAU12',            'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_track',                       'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_track_L1TAU12',               'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        # Run-II - No BDT: variations
        ['tau25_perf_calo',                        'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_calo_L1TAU12',                'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_ptonly',                      'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_ptonly_L1TAU12',              'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_trackcalo',                   'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_tracktwocalo',                'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        # Run-II - With BDT: main track-based items
        ['tau25_medium1_tracktwo',                 'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_track',                    'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_tracktwo_L1TAU12',         'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_tracktwo_L1TAU12IL',       'L1_TAU12IL', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_tracktwo_L1TAU12IT',       'L1_TAU12IT', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        # Run-II - With BDT: main calo-based items
        ['tau25_medium1_calo',                     'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_trackcalo',                'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_tracktwocalo',             'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        # Run-II - With BDT: no pre-selection
        ['tau25_medium1_mvonly',                   'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        # Run-II - High-pT variations
        ['tau35_medium1_tracktwo',                 'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_tracktwo_L1TAU20',         'L1_TAU20', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_tracktwo_L1TAU20IL',       'L1_TAU20IL', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_tracktwo_L1TAU20IT',       'L1_TAU20IT', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_track',                    'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_calo',                     'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_ptonly',                   'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_ptonly_L1TAU20',           'L1_TAU20', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_perf_tracktwo',                    'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_perf_ptonly',                      'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_medium1_calo',                     'L1_TAU40', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_medium1_tracktwo',                 'L1_TAU40', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_medium1_track',                    'L1_TAU40', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_medium1_tracktwo_L1TAU60',         'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau50_medium1_tracktwo_L1TAU12',         'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_medium1_calo',                    'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_medium1_tracktwo',                'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_medium1_track',                   'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_perf_tracktwo',                   'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_perf_ptonly',                     'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_medium1_tracktwo',                'L1_TAU60', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ]


    TriggerFlags.EgammaSlice.signatures = TriggerFlags.EgammaSlice.signatures() + [

        ##########        
        # Single electron triggers
        # Run2 cut-based:
        ['e24_medium_iloose_L1EM18VH',           'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_medium_iloose_L1EM20VH',           'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_tight_iloose_L1EM20VH',            'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_tight_iloose',                     'L1_EM20VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e26_tight_iloose',                     'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e60_medium',                           'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e120_loose',                           'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e140_loose',                           'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        
        # likelihood-based:
        ['e24_lhmedium_iloose_L1EM18VH',         'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e24_lhmedium_iloose_L1EM20VH',         'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e24_lhtight_iloose_L1EM20VH',          'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e24_lhtight_iloose',                   'L1_EM20VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhtight_iloose',                   'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e60_lhmedium',                         'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e120_lhloose',                         'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e140_lhloose',                         'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 

        # Supporting triggers
        # pairs of triggers (cut-based -- likelihood) should be in coherent PS
        ['e24_vloose_L1EM18VH',                  'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e24_vloose_L1EM20VH',                  'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e26_vloose_L1EM20VH',                  'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e24_medium_L1EM18VH',                  'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e24_tight_L1EM20VH',                   'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 

        ['e24_lhvloose_L1EM18VH',                'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],  
        ['e24_lhvloose_L1EM20VH',                'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],  
        ['e26_lhvloose_L1EM20VH',                'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],        
        ['e24_lhmedium_L1EM18VH',                'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],  
        ['e24_lhtight_L1EM20VH',                 'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        #
        ['e24_medium_iloose_L2StarA_L1EM20VH',   'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e24_medium_L2Star_idperf_L1EM20VH',    'L1_EM20VH',    [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1], 
        
        ['e24_lhmedium_iloose_L2StarA_L1EM20VH', 'L1_EM20VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e24_lhmedium_L2Star_idperf_L1EM20VH',  'L1_EM20VH',    [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1], 
        #
        ['e5_loose',                             'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e5_loose_L2StarA',                     'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e5_loose_L2Star_idperf',               'L1_EM3',       [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1],
        ['e5_loose_idperf',                     'L1_EM3',       [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1],
        ['e24_medium_idperf_L1EM20VH',          'L1_EM20VH',    [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1], 
        
        ['e5_lhloose',                          'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e5_lhloose_L2StarA',                  'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e5_lhloose_L2Star_idperf',            'L1_EM3',       [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1],
        ['e5_lhloose_idperf',                   'L1_EM3',       [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1],
        ['e24_lhmedium_idperf_L1EM20VH',        'L1_EM20VH',    [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1], 
        #        
        ['e5_vloose',                           'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e10_vloose_L1EM7',                    'L1_EM7',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e15_vloose_L1EM7',                    'L1_EM7',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e20_vloose_L1EM12',                   'L1_EM12',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e25_vloose_L1EM15',                   'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e30_vloose_L1EM15',                   'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e40_vloose_L1EM15',                   'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e50_vloose_L1EM15',                   'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e60_vloose',                          'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e70_vloose',                          'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e80_vloose',                          'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e100_vloose',                         'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e120_vloose',                         'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        
        ['e12_vloose_L1EM10VH',                 'L1_EM10VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e15_vloose_L1EM13VH',                 'L1_EM13VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e17_vloose',                          'L1_EM15VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e20_vloose',                          'L1_EM15VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        #        
        ['e5_lhvloose',                         'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e10_lhvloose_L1EM7',                  'L1_EM7',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e15_lhvloose_L1EM7',                  'L1_EM7',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e20_lhvloose_L1EM12',                 'L1_EM12',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e25_lhvloose_L1EM15',                 'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e30_lhvloose_L1EM15',                 'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e40_lhvloose_L1EM15',                 'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e50_lhvloose_L1EM15',                 'L1_EM15',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e60_lhvloose',                        'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e70_lhvloose',                        'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e80_lhvloose',                        'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e100_lhvloose',                       'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e120_lhvloose',                       'L1_EM22VHI',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 

        ['e12_lhvloose_L1EM10VH',               'L1_EM10VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e15_lhvloose_L1EM13VH',               'L1_EM13VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e17_lhvloose',                        'L1_EM15VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e20_lhvloose',                        'L1_EM15VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        # technical chains for forward electrons
        #['e17_loose_L1EM15VHJ1523ETA49',       'L1_EM15VH_J15.23ETA49', ['L1_EM15VH','L1_J15.23ETA49'], [PhysicsStream], ['RATE:Combined', 'BW:Egamma'],-1], 
        #['e17_lhloose_L1EM15VHJ1523ETA49',     'L1_EM15VH_J15.23ETA49', ['L1_EM15VH','L1_J15.23ETA49'], [PhysicsStream], ['RATE:Combined', 'BW:Egamma'],-1], 
        ['e17_loose_L1EM15VHJJ1523ETA49',       'L1_EM15VH_JJ15.23ETA49', ['L1_EM15VH','L1_JJ15.23ETA49'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
        ['e17_lhloose_L1EM15VHJJ1523ETA49',     'L1_EM15VH_JJ15.23ETA49', ['L1_EM15VH','L1_JJ15.23ETA49'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
        
        # Supporting trigger with PS
                        
        ['e0_perf_L1EM15',              'L1_EM15',[], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e0_L2Star_perf_L1EM15',       'L1_EM15',[], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],         
        
        # Et cut only chains
        ['e5_etcut',                             'L1_EM3',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e10_etcut_L1EM7',                      'L1_EM7',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e15_etcut_L1EM7',                      'L1_EM7',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e20_etcut_L1EM12',                     'L1_EM12',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],         
        ['e25_etcut_L1EM15',                     'L1_EM15',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e30_etcut_L1EM15',                     'L1_EM15',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e40_etcut_L1EM15',                     'L1_EM15',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e50_etcut_L1EM15',                     'L1_EM15',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e60_etcut',                            'L1_EM22VHI',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e80_etcut',                            'L1_EM22VHI',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e100_etcut',                           'L1_EM22VHI',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e120_etcut',                           'L1_EM22VHI',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        
        # Rerun mode
        ['e4_etcut',                             'L1_EM3',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e9_etcut',                             'L1_EM7',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e14_etcut',                            'L1_EM12',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 

        # Rerun mode, cut-based
        ['e9_medium',                            'L1_EM7',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_loose',                            'L1_EM8VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_loose_L1EM10VH',                   'L1_EM8VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e15_loose_L1EM13VH',                   'L1_EM13VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_loose_L1EM15',                     'L1_EM15',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_loose',                            'L1_EM15VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_medium',                           'L1_EM15VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_medium_iloose',                    'L1_EM15VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e20_medium',                           'L1_EM15VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_medium_L1EM15VH',                  'L1_EM15VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e60_loose',                            'L1_EM22VHI',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e70_loose',                            'L1_EM22VHI',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_tight',                             'L1_EM3',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e9_tight',                             'L1_EM7',      [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e14_tight',                            'L1_EM12',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        # Rerun mode, likelihood
        ['e9_lhmedium',                          'L1_EM7',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_lhloose',                          'L1_EM8VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_lhloose_L1EM10VH',                 'L1_EM10VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e15_lhloose_L1EM13VH',                 'L1_EM13VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhloose_L1EM15',                   'L1_EM15',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhloose',                          'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhmedium',                         'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhmedium_iloose',                  'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e20_lhmedium',                         'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_L1EM15VH',                'L1_EM15VH',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e60_lhloose',                          'L1_EM22VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e70_lhloose',                          'L1_EM22VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_lhtight',                           'L1_EM3',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e9_lhtight',                           'L1_EM7',     [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e14_lhtight',                          'L1_EM12',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        # Single electron/photon chains for Calo sequence optimization (kept as before)
        ['e24_tight_iloose_L2EFCalo_L1EM20VH',    'L1_EM20VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhtight_iloose_L2EFCalo_L1EM20VH',  'L1_EM20VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_tight_iloose_HLTCalo_L1EM20VH',     'L1_EM20VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhtight_iloose_HLTCalo_L1EM20VH',   'L1_EM20VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_tight_iloose_etisem_L1EM20VH',      'L1_EM20VH',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g120_loose_HLTCalo',                    'L1_EM22VHI',    [], [PhysicsStream], ['RATE:SinglePhoton','BW:Egamma'],-1],
        ['g35_medium_HLTCalo_g25_medium_HLTCalo', 'L1_2EM15VH',  [], [PhysicsStream], ['RATE:MultiPhoton','BW:Egamma'],-1],
        
        #Single electron/chaing for testing mis-ant robust LH ID
        ['e24_lhmedium_nod0_L1EM18VH',            'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nodphi_L1EM18VH',          'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nodphires_L1EM18VH',       'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_cutd0dphi_L1EM18VH',       'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_iloose_L1EM18VH',            'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nodphi_iloose_L1EM18VH',          'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nodphires_iloose_L1EM18VH',       'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_cutd0dphi_iloose_L1EM18VH',       'L1_EM18VH',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        
        # Loose chains with etcut variations
        ['e120_loose1',  'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        
        # Single electron Run1 PID
        ['e24_medium1_iloose_L1EM18VH',  'L1_EM18VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_medium1_L1EM18VH',            'L1_EM18VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_tight1_iloose_L1EM20VH',      'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_tight1_iloose',               'L1_EM20VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_tight1_iloose',               'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e60_medium1',                     'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ##########
        # Di-electron triggers
        # cut-based
        ['2e12_loose_L12EM10VH',            'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
        ['2e15_loose_L12EM13VH',            'L1_2EM13VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
        ['2e17_loose_L12EM15',              'L1_2EM15',   [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
        ['2e17_loose',                      'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        
        # likelihood
        ['2e12_lhloose_L12EM10VH',          'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
        ['2e15_lhloose_L12EM13VH',          'L1_2EM13VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
        ['2e17_lhloose',                    'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
        ['2e17_lhloose_L12EM15',            'L1_2EM15',   [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1], 

        # Tri-electron triggers
        ['e17_medium_2e9_medium',           'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],  
        ['e17_loose_2e9_loose',             'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],  
        ['e17_lhloose_2e9_lhloose',         'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],  
        ['e17_medium_iloose_2e9_medium',    'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],  
        ['e17_lhmedium_2e9_lhmedium',       'L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],  
        ['e17_lhmedium_iloose_2e9_lhmedium','L1_EM15VH_3EM7', ['L1_EM15VH','L1_3EM7'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],  
        
        # Multi-electron chains for Run1 PID
        ['2e12_loose1_L12EM10VH',            'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e17_loose1',                      'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        
        ##########
        # Multielectron chains with m_ee cut for Run2 PID (supporting L1Topo J/psi)
        # cut-based
        ['e5_tight_e4_etcut',                                'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e5_tight_e4_etcut_Jpsiee',                         'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e9_tight_e4_etcut_Jpsiee',                         'L1_EM7_2EM3',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
        ['e9_etcut_e5_tight_Jpsiee',                         'L1_EM7_2EM3',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],         
        ['e14_tight_e4_etcut_Jpsiee',                        'L1_EM12_2EM3', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e14_etcut_e5_tight_Jpsiee',                        'L1_EM12_2EM3', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        
        # likelihood
        ['e5_lhtight_e4_etcut',                              'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e5_lhtight_e4_etcut_Jpsiee',                       'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e9_lhtight_e4_etcut_Jpsiee',                       'L1_EM7_2EM3',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e9_etcut_e5_lhtight_Jpsiee',                       'L1_EM7_2EM3',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1], 
        ['e14_lhtight_e4_etcut_Jpsiee',                      'L1_EM12_2EM3', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e14_etcut_e5_lhtight_Jpsiee',                      'L1_EM12_2EM3', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],                
        
        # Multielectron chains with m_ee cut for Run1 PID
        ['e5_tight1_e4_etcut',         'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e5_tight1_e4_etcut_Jpsiee',  'L1_2EM3', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],

        #Jpsiee from L1Topo
        ['e5_tight_e4_etcut_L1JPSI-1M5',                                'L1_JPSI-1M5', ['L1_2EM3','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e5_lhtight_e4_etcut_L1JPSI-1M5',                                'L1_JPSI-1M5', ['L1_2EM3','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e5_tight1_e4_etcut_L1JPSI-1M5',                                'L1_JPSI-1M5', ['L1_2EM3','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],

        ['e5_tight_e4_etcut_Jpsiee_L1JPSI-1M5',             'L1_JPSI-1M5', ['L1_2EM3','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, False],
        ['e9_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7',         'L1_JPSI-1M5-EM7',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, False], 
        ['e9_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM7',         'L1_JPSI-1M5-EM7',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, False],         
        ['e14_tight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12',       'L1_JPSI-1M5-EM12', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, False],
        ['e14_etcut_e5_tight_Jpsiee_L1JPSI-1M5-EM12',       'L1_JPSI-1M5-EM12', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, False],
        ['e5_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5',           'L1_JPSI-1M5', ['L1_2EM3','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, False],
        ['e9_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM7',           'L1_JPSI-1M5-EM7',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, False],
        ['e9_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM7',       'L1_JPSI-1M5-EM7',  ['L1_EM7','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, False], 
        ['e14_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5-EM12',      'L1_JPSI-1M5-EM12', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, False],
        ['e14_etcut_e5_lhtight_Jpsiee_L1JPSI-1M5-EM12',     'L1_JPSI-1M5-EM12', ['L1_EM12','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, False],                
        ['e5_tight1_e4_etcut_Jpsiee_L1JPSI-1M5',       'L1_JPSI-1M5', ['L1_2EM3','L1_2EM3'], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1, False],


        # W T&P triggers
        ['e13_etcut_trkcut', 'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0',  ['L1_EM10'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e13_etcut_trkcut_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS30', 'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS30',  ['L1_EM10'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e18_etcut_trkcut', 'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_W-250RO2-XEHT-0', ['L1_EM15'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e18_etcut_trkcut_L1EM15_W-MT35_W-05DPHI-JXE-0_W-15DPHI-EMXE_XS60', 'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-15DPHI-EMXE_XS60', ['L1_EM15'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        #['e5_etcut_L1W-10DPHI-JXE-0',  'L1_W-10DPHI-JXE-0', ['L1_EM3'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        #['e5_etcut_L1W-20DPHI-JXE-1',  'L1_W-20DPHI-JXE-1', ['L1_EM3'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        #['e5_etcut_L1W-20DPHI-JXE-0',  'L1_W-20DPHI-JXE-0', ['L1_EM3'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ### TOPO ['e5_etcut_L1W-10DPHI-EMXE-0',      'L1_W-10DPHI-EMXE-0', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ### TOPO ['e5_etcut_L1W-20DPHI-EMXE-1',      'L1_W-20DPHI-EMXE-1', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ### TOPO ['e5_etcut_L1W-20DPHI-EMXE-0',      'L1_W-20DPHI-EMXE-0', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        #['e5_etcut_L1W-05RO-XEHT-0',     'L1_W-05RO-XEHT-0', ['L1_EM3'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        #['e5_etcut_L1W-08RO-XEHT-1',     'L1_W-08RO-XEHT-1', ['L1_EM3'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        #['e5_etcut_L1W-40RO2-XEHT-0',    'L1_W-40RO2-XEHT-0', ['L1_EM3'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        #['e5_etcut_L1W-90RO2-XEHT-1',    'L1_W-90RO2-XEHT-1', ['L1_EM3'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        #['e5_etcut_L1W-HT20-JJ0.ETA49',  'L1_W-HT20-JJ0.ETA49', ['L1_EM3'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        #['e5_etcut_L1W-NOMATCH',  'L1_W-NOMATCH', ['L1_EM3'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        #['e5_etcut_L1W-NOMATCH_W-05RO-XEEMHT',  'L1_W-NOMATCH_W-05RO-XEEMHT', ['L1_EM3'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        # Z T&P triggers for monitoring
        ['e24_lhtight_L1EM20VH_e15_etcut_Zee', 'L1_EM20VH',    [], [PhysicsStream], ['RATE:SinglePhoton','BW:Egamma'],-1], 
        ['e24_tight_L1EM20VH_e15_etcut_Zee',   'L1_EM20VH',    [], [PhysicsStream], ['RATE:SinglePhoton','BW:Egamma'],-1], 
        ['e26_lhtight_e15_etcut_Zee',          'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton','BW:Egamma'],-1], 
        ['e26_tight_e15_etcut_Zee',            'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton','BW:Egamma'],-1], 

        ##########
        # Single photon triggers        
        ['g120_loose',                    'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton','BW:Egamma'],-1], 
        ['g120_loose1',                   'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton','BW:Egamma'],-1], 
        ['g140_loose',                    'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton','BW:Egamma'],-1],         
        ['g200_etcut',                    'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1], 

        # Supporting triggers with 
        ['g0_perf_L1EM15',                'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1], 

        # PS-ed trigger to supmbined chains
        ['g20_etcut_L1EM12',              'L1_EM12',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1], 
        ['g15_loose_L1EM7',               'L1_EM7',   [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g20_loose_L1EM12',              'L1_EM12',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], # pass through
        ['g40_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g45_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g50_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g80_loose',                     'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g100_loose',                    'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 

        # Rerun mode and PS
        ['g10_loose_L1EM7',               'L1_EM7',   [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g25_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g60_loose',                     'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g70_loose',                     'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],

        # Rerun mode
        ['g10_medium_L1EM7',              'L1_EM7',   [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g10_loose',                     'L1_EM8VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g10_medium',                    'L1_EM8VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g15_loose_L1EM3',               'L1_EM3',   [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g15_loose',                     'L1_EM13VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_loose',                     'L1_EM15VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_tight',                     'L1_EM15VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g25_loose',                     'L1_EM15VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g25_medium',                    'L1_EM15VH',[], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_loose',                     'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_medium',                    'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g40_tight',                     'L1_EM20VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g45_tight',                     'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g50_loose',                     'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g60_loose_L1EM15VH',            'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        
        ##########        
        # Di-photon triggers
        ['g35_loose_L1EM15_g25_loose_L1EM15',       'L1_2EM15', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['g35_loose_g25_loose',                     'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['g35_loose1_g25_loose1',                   'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['g35_medium_g25_medium',                   'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['g35_medium1_g25_medium1',                 'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g20_loose_L12EM15',                      'L1_2EM15', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g20_tight',                              'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g50_loose',                              'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g60_loose_L12EM15VH',                    'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],  

        # Tri-photon triggers
        ['3g15_loose',                              'L1_2EM13VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['g20_loose_2g15_loose_L12EM13VH',          'L1_2EM13VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g20_loose_g15_loose',                    'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['3g20_loose',                              'L1_2EM15VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 

        ##########        
        # Monopole triggers
        ['g0_hiptrt_L1EM18VH',                    'L1_EM18VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g0_hiptrt_L1EM20VH',                    'L1_EM20VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g0_hiptrt_L1EM20VHI',                   'L1_EM20VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g0_hiptrt_L1EM22VHI',                 'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 

        # Fall-back if problem with new tracking kept as before)
        ['e17_etcut_L1EM15',                      'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_etcut_L2StarA_L1EM15',              'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_etcut_L2StarB_L1EM15',              'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_etcut_L2StarC_L1EM15',              'L1_EM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_etcut_L2Star_idperf_L1EM15',        'L1_EM15', [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1],
        ['e17_etcut_idperf_L1EM15',               'L1_EM15', [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1],
        
        # extra id perf chains for HLT ID group (kept as before)
        ['e24_medium1_L2Star_idperf_L1EM18VH',    'L1_EM18VH', [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1],
        ['e5_loose1_L2Star_idperf',               'L1_EM3',    [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1],
        ['e24_medium1_idperf_L1EM18VH',           'L1_EM18VH', [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1],
        ['e5_loose1_idperf',                      'L1_EM3',    [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Egamma'],-1],
        ['e24_medium1_iloose_L2StarA_L1EM18VH',   'L1_EM18VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        
        
        ]


    ###########################################################################################################
    #   Bphysics
    ###########################################################################################################
    TriggerFlags.BphysicsSlice.signatures = TriggerFlags.BphysicsSlice.signatures() + [

        #['2mu6_bMuMu_L12MU6', 'L1_2MU6', [], [PhysicsStream], [], -1], 

        ###########  2MU4  ################
        ['2mu4_bJpsimumu',                                            'L1_2MU4',                         [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_bBmumu',                                               'L1_2MU4',                         [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_bUpsimumu',                                            'L1_2MU4',                         [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_bBmumuxv2',                                            'L1_2MU4',                         [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_bBmumux_BcmumuDsloose',                                'L1_2MU4',                         [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        # #Commented in ATR-8195
        ['2mu4_bJpsimumu_L1BPH-DR-2MU4',                            'L1_BPH-DR-2MU4',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_L1BPH-DR-2MU4',                                'L1_BPH-DR-2MU4',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumuxv2_L1BPH-DR-2MU4',                             'L1_BPH-DR-2MU4',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4',                 'L1_BPH-DR-2MU4',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_L1BPH-DR-2MU4',                                 'L1_BPH-DR-2MU4',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4',                      'L1_BPH-DR-2MU4',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bJpsimumu_L1BPH-2M-2MU4',                          'L1_BPH-2M-2MU4',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_L1BPH-2M-2MU4',                             'L1_BPH-2M-2MU4',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bUpsimumu_L1BPH-2M-2MU4',                          'L1_BPH-2M-2MU4',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumuxv2_L1BPH-2M-2MU4',                          'L1_BPH-2M-2MU4',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-2M-2MU4',              'L1_BPH-2M-2MU4',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_L1BPH-2M-2MU4',                              'L1_BPH-2M-2MU4',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_L1BPH-2M-2MU4',                   'L1_BPH-2M-2MU4',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_L1BPH-4M8-2MU4',                               'L1_BPH-4M8-2MU4',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumuxv2_L1BPH-4M8-2MU4',                            'L1_BPH-4M8-2MU4',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU4',                'L1_BPH-4M8-2MU4',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_L1BPH-4M8-2MU4',                                'L1_BPH-4M8-2MU4',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_L1BPH-4M8-2MU4',                     'L1_BPH-4M8-2MU4',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        
        ['2mu4_bJpsimumu_L1BPH-DR-2MU4-BPH-2M-2MU4',             'L1_BPH-DR-2MU4_BPH-2M-2MU4', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_L1BPH-DR-2MU4-BPH-2M-2MU4',                'L1_BPH-DR-2MU4_BPH-2M-2MU4', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumuxv2_L1BPH-DR-2MU4-BPH-2M-2MU4',             'L1_BPH-DR-2MU4_BPH-2M-2MU4', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BPH-2M-2MU4', 'L1_BPH-DR-2MU4_BPH-2M-2MU4', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_L1BPH-DR-2MU4-BPH-2M-2MU4',                 'L1_BPH-DR-2MU4_BPH-2M-2MU4', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BPH-2M-2MU4',      'L1_BPH-DR-2MU4_BPH-2M-2MU4', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_L1BPH-DR-2MU4-BPH-4M8-2MU4',                  'L1_BPH-DR-2MU4_BPH-4M8-2MU4',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumuxv2_L1BPH-DR-2MU4-BPH-4M8-2MU4',               'L1_BPH-DR-2MU4_BPH-4M8-2MU4',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BPH-4M8-2MU4',   'L1_BPH-DR-2MU4_BPH-4M8-2MU4',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_L1BPH-DR-2MU4-BPH-4M8-2MU4',                   'L1_BPH-DR-2MU4_BPH-4M8-2MU4',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BPH-4M8-2MU4',        'L1_BPH-DR-2MU4_BPH-4M8-2MU4',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        # #--- end from Commented in ATR-8195
        
        #L1_2MU4 seeded
        ['mu4_iloose_mu4_7invm9_noos',     'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu4_mu4_idperf_bJpsimumu_noid',  'L1_2MU4', [], [PhysicsStream], ['RATE:IDMonitoring','BW:Bphys'], -1],
        ['mu4_iloose_mu4_11invm60_noos',   'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        ########## SS dimuon selection ###
        ['2mu4_bDimu_noinvm_novtx_ss',    'L1_2MU4',     [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bDimu_noinvm_novtx_ss',    'L1_2MU6',     [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bDimu_noinvm_novtx_ss', 'L1_MU6_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bDimu_noinvm_novtx_ss',   'L1_2MU10',    [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        ########## B+->J/psi K+ exclusive selection ###
        ['2mu4_bBmumux_BpmumuKp',    'L1_2MU4',     [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bBmumux_BpmumuKp',    'L1_2MU6',     [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bBmumux_BpmumuKp', 'L1_MU6_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bBmumux_BpmumuKp',   'L1_2MU10',    [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        ###########  3MU4  ################
        ['3mu4_bDimu',  'L1_3MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['3mu4_bTau',   'L1_3MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['3mu4_bJpsi',  'L1_3MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['3mu4_bUpsi',  'L1_3MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],


        ######## L1_MU6 seeded#####
        ['mu6_bJpsi_Trkloose',     'L1_MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],


        #######  L1_2MU6 #######
        #in Physics menu ['2mu6_bDimu',            'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1], 
        #n Physics menu  ['2mu6_bDimu_novtx_noos',  'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],  
        # not set up yet? ['mu6_iloose_mu6_18invm60_noos',  'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bBmumu',                 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bBmumuxv2',              'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bBmumux_BcmumuDsloose',  'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bJpsimumu',              'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu6_bUpsimumu',              'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],


        # #Commented in ATR-8195
        ['2mu6_bJpsimumu_L1BPH-DR-2MU6',                             'L1_BPH-DR-2MU6',                 ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bBmumu_L1BPH-DR-2MU6',                                'L1_BPH-DR-2MU6',                 ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bBmumuxv2_L1BPH-DR-2MU6',                             'L1_BPH-DR-2MU6',                 ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bBmumux_BcmumuDsloose_L1BPH-DR-2MU6',                 'L1_BPH-DR-2MU6',                 ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bDimu_L1BPH-DR-2MU6',                                 'L1_BPH-DR-2MU6',                 ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bDimu_novtx_noos_L1BPH-DR-2MU6',                      'L1_BPH-DR-2MU6',                 ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bJpsimumu_L1BPH-2M-2MU6',                          'L1_BPH-2M-2MU6',              ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bBmumu_L1BPH-2M-2MU6',                             'L1_BPH-2M-2MU6',              ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bUpsimumu_L1BPH-2M-2MU6',                          'L1_BPH-2M-2MU6',              ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bBmumuxv2_L1BPH-2M-2MU6',                          'L1_BPH-2M-2MU6',              ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bBmumux_BcmumuDsloose_L1BPH-2M-2MU6',              'L1_BPH-2M-2MU6',              ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bDimu_L1BPH-2M-2MU6',                              'L1_BPH-2M-2MU6',              ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bDimu_novtx_noos_L1BPH-2M-2MU6',                   'L1_BPH-2M-2MU6',              ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bBmumu_L1BPH-4M8-2MU6',                               'L1_BPH-4M8-2MU6',                ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bBmumuxv2_L1BPH-4M8-2MU6',                            'L1_BPH-4M8-2MU6',                ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU6',                'L1_BPH-4M8-2MU6',                ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bDimu_L1BPH-4M8-2MU6',                                'L1_BPH-4M8-2MU6',                ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bDimu_novtx_noos_L1BPH-4M8-2MU6',                     'L1_BPH-4M8-2MU6',                ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        
        ['2mu6_bJpsimumu_L1BPH-DR-2MU6-BPH-2M-2MU6',             'L1_BPH-DR-2MU6_BPH-2M-2MU6', ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bBmumu_L1BPH-DR-2MU6-BPH-2M-2MU6',                'L1_BPH-DR-2MU6_BPH-2M-2MU6', ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bBmumuxv2_L1BPH-DR-2MU6-BPH-2M-2MU6',             'L1_BPH-DR-2MU6_BPH-2M-2MU6', ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bBmumux_BcmumuDsloose_L1BPH-DR-2MU6-BPH-2M-2MU6', 'L1_BPH-DR-2MU6_BPH-2M-2MU6', ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bDimu_L1BPH-DR-2MU6-BPH-2M-2MU6',                 'L1_BPH-DR-2MU6_BPH-2M-2MU6', ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bDimu_novtx_noos_L1BPH-DR-2MU6-BPH-2M-2MU6',      'L1_BPH-DR-2MU6_BPH-2M-2MU6', ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bBmumu_L1BPH-DR-2MU6-BPH-4M8-2MU6',                  'L1_BPH-DR-2MU6_BPH-4M8-2MU6',   ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bBmumuxv2_L1BPH-DR-2MU6-BPH-4M8-2MU6',               'L1_BPH-DR-2MU6_BPH-4M8-2MU6',   ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bBmumux_BcmumuDsloose_L1BPH-DR-2MU6-BPH-4M8-2MU6',   'L1_BPH-DR-2MU6_BPH-4M8-2MU6',   ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bDimu_L1BPH-DR-2MU6-BPH-4M8-2MU6',                   'L1_BPH-DR-2MU6_BPH-4M8-2MU6',   ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bDimu_novtx_noos_L1BPH-DR-2MU6-BPH-4M8-2MU6',        'L1_BPH-DR-2MU6_BPH-4M8-2MU6',   ['L1_2MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],       
        # #----end Commented in ATR-8195

        #########   3MU6 seeded##########
        ['3mu6_bJpsi',                   'L1_3MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['3mu6_bTau',                    'L1_3MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['3mu6_bDimu',                   'L1_3MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['3mu6_bUpsi',                  'L1_3MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_iloose_mu6_11invm60_noos', 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        #not set up yet?? ['mu6_iloose_mu6_18invm60_noos', 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        ########### MU6_2MU4 seeded ###########
        ['mu6_mu4_bJpsimumu',            'L1_MU6_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bBmumu',               'L1_MU6_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bUpsimumu',            'L1_MU6_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bBmumuxv2',            'L1_MU6_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bBmumux_BcmumuDsloose', 'L1_MU6_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bDimu',                'L1_MU6_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_mu4_bDimu_novtx_noos',     'L1_MU6_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        ###########  2MU10 seeded ###########
        ['2mu10_bBmumu',                'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bBmumuxv2',             'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bBmumux_BcmumuDsloose', 'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bJpsimumu',            'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bUpsimumu',            'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bDimu',                 'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu10_bDimu_novtx_noos',      'L1_2MU10', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu13_mu13_idperf_Zmumu',     'L1_2MU10', [], [PhysicsStream], ['RATE:IDMonitoring','BW:Bphys'], -1],  


        # chains with muon overlap removal
        ['2mu4_wOvlpRm_bDimu',                                 'L1_2MU4',                 [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_wOvlpRm_bDimu_novtx_noos',                      'L1_2MU4',                 [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_wOvlpRm_bBmumuxv2',                             'L1_2MU4',                         [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['3mu6_wOvlpRm_bDimu',                   'L1_3MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['2mu4_wOvlpRm_bDimu_noinvm_novtx_ss',    'L1_2MU4',     [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],    

        ['2mu4_bJpsimumu_L1BPH-DR-2MU4-B',                            'L1_BPH-DR-2MU4-B',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_L1BPH-DR-2MU4-B',                                'L1_BPH-DR-2MU4-B',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumuxv2_L1BPH-DR-2MU4-B',                             'L1_BPH-DR-2MU4-B',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-B',                 'L1_BPH-DR-2MU4-B',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_L1BPH-DR-2MU4-B',                                 'L1_BPH-DR-2MU4-B',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-B',                      'L1_BPH-DR-2MU4-B',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bJpsimumu_L1BPH-2M-2MU4-B',                          'L1_BPH-2M-2MU4-B',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_L1BPH-2M-2MU4-B',                             'L1_BPH-2M-2MU4-B',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bUpsimumu_L1BPH-2M-2MU4-B',                          'L1_BPH-2M-2MU4-B',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumuxv2_L1BPH-2M-2MU4-B',                          'L1_BPH-2M-2MU4-B',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-2M-2MU4-B',              'L1_BPH-2M-2MU4-B',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_L1BPH-2M-2MU4-B',                              'L1_BPH-2M-2MU4-B',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_L1BPH-2M-2MU4-B',                   'L1_BPH-2M-2MU4-B',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_L1BPH-4M8-2MU4-B',                               'L1_BPH-4M8-2MU4-B',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumuxv2_L1BPH-4M8-2MU4-B',                            'L1_BPH-4M8-2MU4-B',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU4-B',                'L1_BPH-4M8-2MU4-B',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_L1BPH-4M8-2MU4-B',                                'L1_BPH-4M8-2MU4-B',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_L1BPH-4M8-2MU4-B',                     'L1_BPH-4M8-2MU4-B',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        
        ['2mu4_bJpsimumu_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B',             'L1_BPH-DR-2MU4-B_BPH-2M-2MU4-B', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B',                'L1_BPH-DR-2MU4-B_BPH-2M-2MU4-B', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumuxv2_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B',             'L1_BPH-DR-2MU4-B_BPH-2M-2MU4-B', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B', 'L1_BPH-DR-2MU4-B_BPH-2M-2MU4-B', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B',                 'L1_BPH-DR-2MU4-B_BPH-2M-2MU4-B', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-B-BPH-2M-2MU4-B',      'L1_BPH-DR-2MU4-B_BPH-2M-2MU4-B', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B',                  'L1_BPH-DR-2MU4-B_BPH-4M8-2MU4-B',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumuxv2_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B',               'L1_BPH-DR-2MU4-B_BPH-4M8-2MU4-B',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B',   'L1_BPH-DR-2MU4-B_BPH-4M8-2MU4-B',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B',                   'L1_BPH-DR-2MU4-B_BPH-4M8-2MU4-B',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-B-BPH-4M8-2MU4-B',        'L1_BPH-DR-2MU4-B_BPH-4M8-2MU4-B',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],


        ['2mu4_bJpsimumu_L1BPH-DR-2MU4-BO',                            'L1_BPH-DR-2MU4-BO',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_L1BPH-DR-2MU4-BO',                                'L1_BPH-DR-2MU4-BO',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumuxv2_L1BPH-DR-2MU4-BO',                             'L1_BPH-DR-2MU4-BO',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BO',                 'L1_BPH-DR-2MU4-BO',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_L1BPH-DR-2MU4-BO',                                 'L1_BPH-DR-2MU4-BO',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BO',                      'L1_BPH-DR-2MU4-BO',                 ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bJpsimumu_L1BPH-2M-2MU4-BO',                          'L1_BPH-2M-2MU4-BO',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_L1BPH-2M-2MU4-BO',                             'L1_BPH-2M-2MU4-BO',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bUpsimumu_L1BPH-2M-2MU4-BO',                          'L1_BPH-2M-2MU4-BO',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumuxv2_L1BPH-2M-2MU4-BO',                          'L1_BPH-2M-2MU4-BO',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-2M-2MU4-BO',              'L1_BPH-2M-2MU4-BO',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_L1BPH-2M-2MU4-BO',                              'L1_BPH-2M-2MU4-BO',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_L1BPH-2M-2MU4-BO',                   'L1_BPH-2M-2MU4-BO',              ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_L1BPH-4M8-2MU4-BO',                               'L1_BPH-4M8-2MU4-BO',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumuxv2_L1BPH-4M8-2MU4-BO',                            'L1_BPH-4M8-2MU4-BO',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-2MU4-BO',                'L1_BPH-4M8-2MU4-BO',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_L1BPH-4M8-2MU4-BO',                                'L1_BPH-4M8-2MU4-BO',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_L1BPH-4M8-2MU4-BO',                     'L1_BPH-4M8-2MU4-BO',                ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        
        ['2mu4_bJpsimumu_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO',             'L1_BPH-DR-2MU4-BO_BPH-2M-2MU4-BO', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO',                'L1_BPH-DR-2MU4-BO_BPH-2M-2MU4-BO', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumuxv2_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO',             'L1_BPH-DR-2MU4-BO_BPH-2M-2MU4-BO', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO', 'L1_BPH-DR-2MU4-BO_BPH-2M-2MU4-BO', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO',                 'L1_BPH-DR-2MU4-BO_BPH-2M-2MU4-BO', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BO-BPH-2M-2MU4-BO',      'L1_BPH-DR-2MU4-BO_BPH-2M-2MU4-BO', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO',                  'L1_BPH-DR-2MU4-BO_BPH-4M8-2MU4-BO',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumuxv2_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO',               'L1_BPH-DR-2MU4-BO_BPH-4M8-2MU4-BO',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BcmumuDsloose_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO',   'L1_BPH-DR-2MU4-BO_BPH-4M8-2MU4-BO',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO',                   'L1_BPH-DR-2MU4-BO_BPH-4M8-2MU4-BO',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bDimu_novtx_noos_L1BPH-DR-2MU4-BO-BPH-4M8-2MU4-BO',        'L1_BPH-DR-2MU4-BO_BPH-4M8-2MU4-BO',   ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],

        ['mu6_mu4_bDimu_L1BPH-2M-MU6MU4',   'L1_BPH-2M-MU6MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bDimu_L1BPH-4M8-MU6MU4',  'L1_BPH-4M8-MU6MU4',['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bDimu_L1BPH-BPH-DR-MU6MU4','L1_BPH-DR-MU6MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        
        ['mu6_mu4_bJpsimumu_L1BPH-2M-MU6MU4',   'L1_BPH-2M-MU6MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bJpsimumu_L1BPH-DR-MU6MU4',   'L1_BPH-DR-MU6MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
 
        ['mu6_mu4_bBmumu_L1BPH-2M-MU6MU4',   'L1_BPH-2M-MU6MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bBmumu_L1BPH-4M8-MU6MU4',  'L1_BPH-4M8-MU6MU4',['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bBmumu_L1BPH-DR-MU6MU4',   'L1_BPH-DR-MU6MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
 
        ['mu6_mu4_bUpsimumu_L1BPH-2M-MU6MU4',   'L1_BPH-2M-MU6MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bUpsimumu_L1BPH-DR-MU6MU4',   'L1_BPH-DR-MU6MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
 
        ['mu6_mu4_bBmumuxv2_L1BPH-2M-MU6MU4',   'L1_BPH-2M-MU6MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bBmumuxv2_L1BPH-4M8-MU6MU4',  'L1_BPH-4M8-MU6MU4',['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bBmumuxv2_L1BPH-DR-MU6MU4',   'L1_BPH-DR-MU6MU4', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
 
 
        ['mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-2M-MU6MU4-B',   'L1_BPH-2M-MU6MU4-B', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-MU6MU4-B',  'L1_BPH-4M8-MU6MU4-B',['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-DR-MU6MU4-B',   'L1_BPH-DR-MU6MU4-B', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
 
 
        ['mu6_mu4_bDimu_novtx_noos_L1BPH-2M-MU6MU4-B',   'L1_BPH-2M-MU6MU4-B', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bDimu_novtx_noos_L1BPH-4M8-MU6MU4-B',  'L1_BPH-4M8-MU6MU4-B',['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bDimu_novtx_noos_L1BPH-DR-MU6MU4-B',   'L1_BPH-DR-MU6MU4-B', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
 
 
        ['mu6_mu4_bDimu_L1BPH-2M-MU6MU4-BO',   'L1_BPH-2M-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bDimu_L1BPH-4M8-MU6MU4-BO',  'L1_BPH-4M8-MU6MU4-BO',['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bDimu_L1BPH-BPH-DR-MU6MU4-BO','L1_BPH-DR-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        
        ['mu6_mu4_bJpsimumu_L1BPH-2M-MU6MU4-BO',   'L1_BPH-2M-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bJpsimumu_L1BPH-DR-MU6MU4-BO',   'L1_BPH-DR-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
 
        ['mu6_mu4_bBmumu_L1BPH-2M-MU6MU4-BO',   'L1_BPH-2M-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bBmumu_L1BPH-4M8-MU6MU4-BO',  'L1_BPH-4M8-MU6MU4-BO',['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bBmumu_L1BPH-DR-MU6MU4-BO',   'L1_BPH-DR-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
 
        ['mu6_mu4_bUpsimumu_L1BPH-2M-MU6MU4-BO',   'L1_BPH-2M-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bUpsimumu_L1BPH-DR-MU6MU4-BO',   'L1_BPH-DR-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
 
        ['mu6_mu4_bBmumuxv2_L1BPH-2M-MU6MU4-BO',   'L1_BPH-2M-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bBmumuxv2_L1BPH-4M8-MU6MU4-BO',  'L1_BPH-4M8-MU6MU4-BO',['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bBmumuxv2_L1BPH-DR-MU6MU4-BO',   'L1_BPH-DR-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
 
 
        ['mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-2M-MU6MU4-BO',   'L1_BPH-2M-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-4M8-MU6MU4-BO',  'L1_BPH-4M8-MU6MU4-BO',['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bBmumux_BcmumuDsloose_L1BPH-DR-MU6MU4-BO',   'L1_BPH-DR-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
 
 
        ['mu6_mu4_bDimu_novtx_noos_L1BPH-2M-MU6MU4-BO',   'L1_BPH-2M-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bDimu_novtx_noos_L1BPH-4M8-MU6MU4-BO',  'L1_BPH-4M8-MU6MU4-BO',['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],
        ['mu6_mu4_bDimu_novtx_noos_L1BPH-DR-MU6MU4-BO',   'L1_BPH-DR-MU6MU4-BO', ['L1_MU6','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1,False],

        #Tag and Probe
        ['mu18_bJpsi_Trkloose',     'L1_MU15', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        # low lumi running and commissioning
     	['mu4_bJpsi_Trkloose',      'L1_MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],



    ]



    TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures() + [
        # all e/g + X triggers are taken from DC14 and L1 thresholds adjusted

        ['e17_loose_mu14',           'L1_EM15VH_MU10', ['L1_EM15VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e17_lhloose_mu14',         'L1_EM15VH_MU10', ['L1_EM15VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],

        ['2e12_loose_mu10',          'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['2e12_medium_mu10',         'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['2e12_lhloose_mu10',        'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['2e12_lhmedium_mu10',       'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],

        ['e12_loose_2mu10',          'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e12_medium_2mu10',         'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e12_lhloose_2mu10',        'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e12_lhmedium_2mu10',       'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],

        #['e7_medium1_mu24',         'L1_MU20', ['L1_EM3', 'L1_MU20'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],

        ['g25_medium_mu24',          'L1_MU20', ['L1_EM15VH','L1_MU20'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],


        ['g10_etcut_L1EM7_mu10_taumass', 'L1_EM7_MU10', ['L1_EM7', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g10_etcut_L1EM7","mu10"]] ],
        ['g20_etcut_L1EM15_mu4_taumass', 'L1_EM15_MU4', ['L1_EM15', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g20_etcut_L1EM15","mu4"]] ],
        ['g10_etcut_mu10_L1LFV-EM8I', 'L1_LFV-EM8I', ['L1_EM7', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g10_etcut","mu10"]] ],
        ['g20_etcut_mu4_L1LFV-EM15I', 'L1_LFV-EM15I', ['L1_EM15', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g20_etcut","mu4"]] ],
        ['g10_etcut_mu10_taumass', 'L1_LFV-EM8I', ['L1_EM7', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g10_etcut","mu10"]] ],
        ['g20_etcut_mu4_taumass', 'L1_LFV-EM15I', ['L1_EM15', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g20_etcut","mu4"]] ],
     

#        ['g10_loose_mu10_taumass',   'L1_EM7_MU10', ['L1_EM7', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g10_loose","mu10"]] ],
#        ['g20_loose_mu4_taumass',    'L1_EM15_MU4', ['L1_EM15', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,["g20_loose","mu4"]] ],
#        ['g10_loose_mu10',           'L1_EM7_MU10', ['L1_EM7', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],
#        ['g20_loose_mu4',            'L1_EM15_MU4', ['L1_EM15', 'L1_MU4'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],
#        
        ['g15_loose_2mu10_msonly',   'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],

        ['2g10_loose_mu20',          'L1_MU20', ['L1_2EM7', 'L1_MU20'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['2g10_loose','mu20'] ]],
        ['2g10_medium_mu20',         'L1_MU20', ['L1_2EM7', 'L1_MU20'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['2g10_medium','mu20'] ]],
        #['e24_medium1_L1EM20VHI_mu8noL1',    'L1_EM20VHI', ['L1_EM20VHI', ''], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma'], -1, ['serial',-1,['e24_medium1_L1EM20VHI','mu8noL1'] ]],

        # Di-EM triggers
        # cut-based
        ['e24_medium_L1EM15VH_g25_medium',    'L1_2EM15VH', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        ['e20_medium_g35_loose',              'L1_2EM15VH', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        # likelihood
        ['e24_lhmedium_L1EM15VH_g25_medium',  'L1_2EM15VH', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        ['e20_lhmedium_g35_loose',            'L1_2EM15VH', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 

        # Tri-EM triggers
        ['e20_medium_2g10_loose',             'L1_EM15VH_3EM7', ['L1_EM15VH','L1_2EM7'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        ['e20_medium_2g10_medium',            'L1_EM15VH_3EM7', ['L1_EM15VH','L1_2EM7'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        ['e20_lhmedium_2g10_loose',           'L1_EM15VH_3EM7', ['L1_EM15VH','L1_2EM7'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        ['e20_lhmedium_2g10_medium',          'L1_EM15VH_3EM7', ['L1_EM15VH','L1_2EM7'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        
        ### NOT IN THE LIST ['e5_tight1_g4_etcut',                       'L1_2EM3', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1, ['parallel',-1,[] ]],
        ### NOT IN THE LIST ['e24_medium1_L1EM20VHI_g8_etcut',      'L1_EM20VHI', ['L1_EM20VHI','L1_EM3'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1, ['parallel',-1,[] ]],
        ### NOT IN THE LIST ['e17_loose1_2g8_etcut_L12EM7',         'L1_EM15VH_3EM7',['L1_EM15VH','L1_2EM7'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ### NOT IN THE LIST ['e24_medium1_L1EM15VH_g25_medium1',    'L1_2EM15VH',[], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ### NOT IN THE LIST ['e20_medium1_g35_loose1',              'L1_2EM15VH',[], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ### NOT IN THE LIST ['e20_medium1_2g20_loose1',             'L1_2EM15VH',[], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        
        #MET+X -- with e/g only
        #['e24_medium1_L1EM20VHI_xe100noL1',  'L1_EM20VHI',['L1_EM20VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1,['serial',-1,["e24_medium1_L1EM20VHI","xe100noL1"]]],
        ['g70_loose_xe70noL1',              'L1_EM22VHI',    ['L1_EM22VHI',''],    [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1,['serial',-1,["g70_loose","xe70noL1"]]],
        ['g60_loose_xe60noL1',              'L1_EM22VHI',    ['L1_EM22VHI',''],    [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1,['serial',-1,["g60_loose","xe60noL1"]]],
        ['g45_tight_xe45noL1',              'L1_EM20VH',   ['L1_EM20VH',''],   [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1,['serial',-1,["g45_tight","xe45noL1"]]],
        ['g45_tight_L1EM22VHI_xe45noL1',  'L1_EM22VHI',['L1_EM22VHI',''],[PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1,['serial',-1,["g45_tight_L1EM22VHI","xe45noL1"]]],
        ['g40_tight_xe40noL1',              'L1_EM20VH',['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1,['serial',-1,["g40_tight","xe40noL1"]]],

        ['e70_loose_xe70noL1',              'L1_EM22VHI',['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1,['serial',-1,["e70_loose","xe70noL1"]]],
        ['e60_loose_xe60noL1',              'L1_EM22VHI',['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1,['serial',-1,["e60_loose","xe60noL1"]]],
        ['e70_lhloose_xe70noL1',            'L1_EM22VHI',['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1,['serial',-1,["e70_lhloose","xe70noL1"]]],
        ['e60_lhloose_xe60noL1',            'L1_EM22VHI',['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1,['serial',-1,["e60_lhloose","xe60noL1"]]],

        # W T&P triggers
        ['e13_etcut_trkcut_xe20', 'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_W-90RO2-XEHT-0', ['L1_EM10',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","xe20"]]],
        ['e13_etcut_trkcut_xe20_L1EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS30', 'L1_EM10_W-MT25_W-15DPHI-JXE-0_W-15DPHI-EMXE_XS30', ['L1_EM10',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e13_etcut_trkcut","xe20"]]],
        ['e18_etcut_trkcut_xe35', 'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-05DPHI-EMXE_W-250RO2-XEHT-0',['L1_EM15',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e18_etcut_trkcut","xe35"]]],
        ['e18_etcut_trkcut_xe35_L1EM15_W-MT35_W-05DPHI-JXE-0_W-15DPHI-EMXE_XS60', 'L1_EM15_W-MT35_W-05DPHI-JXE-0_W-15DPHI-EMXE_XS60',['L1_EM15',''],[PhysicsStream],['RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e18_etcut_trkcut","xe35"]]],

        #['mu24_xe100noL1_wEFMu',     'L1_MU20',['L1_MU20',''], [PhysicsStream], ['RATE:METMuon', 'BW:Muon'], -1,['serial',-1,["mu24","xe100noL1_wEFMu"]]],
        ['j100_xe80',              'L1_J40_XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j100","xe80"]]],
        ['j80_xe80',               'L1_J40_XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j80","xe80"]]],
        #['j80_xe80_dphi1',         'L1_J40_XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j80","xe80"]]],
        

        ['j80_xe80_dphi1_L1J40_DPHI-Js2XE50',   'L1_J40_DPHI-Js2XE50',['',''], [PhysicsStream],    ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j80","xe80"]]],
        ['j80_xe80_dphi1_L1J40_DPHI-J20s2XE50', 'L1_J40_DPHI-J20s2XE50',['',''], [PhysicsStream],  ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j80","xe80"]]],
        ['j80_xe80_dphi1_L1J40_DPHI-J20XE50', 'L1_J40_DPHI-J20XE50',['',''], [PhysicsStream],  ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j80","xe80"]]],
        ['j80_xe80_dphi1_L1J40_DPHI-CJ20XE50','L1_J40_DPHI-CJ20XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j80","xe80"]]],
        ['j100_xe80_L1J40_DPHI-Js2XE50',    'L1_J40_DPHI-Js2XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j100","xe80"]]],
        ['j100_xe80_L1J40_DPHI-J20s2XE50',  'L1_J40_DPHI-J20s2XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j100","xe80"]]],
        ['j100_xe80_L1J40_DPHI-J20XE50',  'L1_J40_DPHI-J20XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j100","xe80"]]],
        ['j100_xe80_L1J40_DPHI-CJ20XE50', 'L1_J40_DPHI-CJ20XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1,['serial',-1,["j100","xe80"]]],
        ['j30_xe10_razor170', 'L1_2J15_XE55', ['',''], [PhysicsStream], ['RATE:JetMET', 'BW:Jets'], -1, ['serial',-1,['j30','xe10']]],  
        
        # TAU+X
        # Ditau Items
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo',   'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],

        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM',   'L1_TAU20IM_2TAU12IM',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],

        ['tau35_perf_ptonly_tau25_perf_ptonly_L1TAU20IM_2TAU12IM',   'L1_TAU20IM_2TAU12IM',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_perf_ptonly","tau25_perf_ptonly"]]],

        # Works now
        ['tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_L1TAU20_2TAU12',   'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo_L1TAU20","tau25_medium1_tracktwo_L1TAU12"]]],

        # With L1Topo
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I',   'L1_DR-TAU20ITAU12I',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20ITAU12I-J25',   'L1_TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1BOX-TAU20ITAU12I',   'L1_BOX-TAU20ITAU12I',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],

        # High-pT DiTau seeding off of single-tau
        ['tau80_medium1_tracktwo_L1TAU60_tau50_medium1_tracktwo_L1TAU12', 'L1_TAU60',['L1_TAU60','L1_TAU12'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_medium1_tracktwo_L1TAU60","tau50_medium1_tracktwo_L1TAU12"]]],
        ['tau125_medium1_tracktwo_tau50_medium1_tracktwo_L1TAU12', 'L1_TAU60',['L1_TAU60','L1_TAU12'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau125_medium1_tracktwo","tau50_medium1_tracktwo_L1TAU12"]]],

        # Tau + Electron
        # Low-threshold with jets
        ['e17_medium_tau25_medium1_tracktwo',   'L1_EM15HI_2TAU12IM_J25_3J12',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_medium","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_tau25_medium1_tracktwo',  'L1_EM15HI_2TAU12IM_J25_3J12',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium","tau25_medium1_tracktwo"]]],

        # Isolated Variant
        ['e17_medium_iloose_tau25_medium1_tracktwo',   'L1_EM15HI_2TAU12IM_J25_3J12',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_medium_iloose","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_iloose_tau25_medium1_tracktwo',  'L1_EM15HI_2TAU12IM_J25_3J12',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_iloose","tau25_medium1_tracktwo"]]],

        # Those are somehow not working
        #['e17_medium_iloose_tau25_perf_ptonly_L1EM15HI_2TAU12IM',   'L1_EM15HI_2TAU12IM',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_medium_iloose","tau25_perf_ptonly"]]],
        #['e17_lhmedium_iloose_tau25_perf_ptonly_L1EM15HI_2TAU12IM',  'L1_EM15HI_2TAU12IM',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_iloose","tau25_perf_ptonly"]]],

        # Tau + Electron main L1Topo
        ['e17_medium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',  'L1_DR-EM15TAU12I-J25',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_medium","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',  'L1_DR-EM15TAU12I-J25',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium","tau25_medium1_tracktwo"]]],

        # Isolated variant
        ['e17_medium_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',  'L1_DR-EM15TAU12I-J25',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_medium_iloose","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_iloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',  'L1_DR-EM15TAU12I-J25',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_iloose","tau25_medium1_tracktwo"]]],

        # Tau + electron back-up L1Topo

        ['e17_medium_tau25_medium1_tracktwo_L1EM15TAU12I-J25',  'L1_EM15TAU12I-J25',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_medium","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_tau25_medium1_tracktwo_L1EM15TAU12I-J25',  'L1_EM15TAU12I-J25',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium","tau25_medium1_tracktwo"]]],

        ['e17_medium_tau25_medium1_tracktwo_L1EM15-TAU12I',  'L1_EM15-TAU12I',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_medium","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_tau25_medium1_tracktwo_L1EM15-TAU12I',  'L1_EM15-TAU12I',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium","tau25_medium1_tracktwo"]]],

        # High-threshold
        ['e17_medium_tau80_medium1_tracktwo',   'L1_EM15HI_TAU40_2TAU15',['L1_EM15HI','L1_TAU40'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_medium","tau80_medium1_tracktwo"]]],
        ['e17_lhmedium_tau80_medium1_tracktwo',   'L1_EM15HI_TAU40_2TAU15',['L1_EM15HI','L1_TAU40'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium","tau80_medium1_tracktwo"]]],

        ['e17_medium_tau80_medium1_tracktwo_L1EM15-TAU40',   'L1_EM15-TAU40',['L1_EM15HI','L1_TAU40'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_medium","tau80_medium1_tracktwo"]]],
        ['e17_lhmedium_tau80_medium1_tracktwo_L1EM15-TAU40',   'L1_EM15-TAU40',['L1_EM15HI','L1_TAU40'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium","tau80_medium1_tracktwo"]]],
        
        # Tau + Muon
        ['mu14_tau25_medium1_tracktwo',      'L1_MU10_TAU12IM_J25_2J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14","tau25_medium1_tracktwo"]]],        
        ['mu14_tau35_medium1_tracktwo_L1TAU20',  'L1_MU10_TAU20',['L1_MU10','L1_TAU20'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14","tau35_medium1_tracktwo_L1TAU20"]]],
        ['mu14_tau35_medium1_tracktwo',  'L1_MU10_TAU20IM',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14","tau35_medium1_tracktwo"]]],

        ['mu14_iloose_tau25_medium1_tracktwo',      'L1_MU10_TAU12IM_J25_2J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_iloose","tau25_medium1_tracktwo"]]],        
        ['mu14_iloose_tau35_medium1_tracktwo',  'L1_MU10_TAU20IM',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_iloose","tau35_medium1_tracktwo"]]],
        
        # Also somehow not working...
        #['mu14_iloose_tau25_perf_ptonly_L1MU10_TAU12IM',      'L1_MU10_TAU12IM',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_iloose","tau25_perf_ptonly"]]],        

        # With L1Topo
        ['mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12-J25',  'L1_DR-MU10TAU12I_TAU12I-J25',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14","tau25_medium1_tracktwo"]]],
        ['mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I',  'L1_DR-MU10TAU12I',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14","tau25_medium1_tracktwo"]]],
        
        ['mu14_tau25_medium1_tracktwo_L1MU10_TAU12I-J25',  'L1_MU10_TAU12I-J25',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14","tau25_medium1_tracktwo"]]],

        # Tau + MET
        ['tau35_medium1_tracktwo_xe70_L1XE45',  'L1_TAU20IM_2J20_XE45',['L1_TAU20IM','L1_XE45'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","xe70_L1XE45"]]],  
        ['tau35_medium1_tracktwo_L1TAU20_xe70_L1XE45',  'L1_TAU20_2J20_XE45',['L1_TAU20','L1_XE45'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo_L1TAU20","xe70_L1XE45"]]],
        # Problematic
        # Cannot handle our naming scheme :D
        #['tau35_medium1_tracktwo_L1TAU20_xe70_L1XE45_L1XE45_TAU20-J20',  'L1_XE45_TAU20-J20',['L1_TAU20', 'L1_XE45'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo_L1TAU20","xe70_L1XE45"]]],  
        
        # Ditau + MET
        ['tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_xe50', 'L1_TAU20_2TAU12_XE35',['L1_TAU20','L1_TAU12','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo_L1TAU20","tau25_medium1_tracktwo_L1TAU12","xe50"]]],

        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_xe50', 'L1_TAU20IM_2TAU12IM_XE35',['L1_TAU20IM','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo","xe50"]]],

        
        # Tau + e + MET

        ['e17_medium_tau25_medium1_tracktwo_xe50', 'L1_EM15HI_2TAU12IM_XE35',['L1_EM15HI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["e17_medium","tau25_medium1_tracktwo", "xe50"]]],
        ['e17_lhmedium_tau25_medium1_tracktwo_xe50', 'L1_EM15HI_2TAU12IM_XE35',['L1_EM15HI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium","tau25_medium1_tracktwo", "xe50"]]],

        ['e17_medium_iloose_tau25_medium1_tracktwo_xe50', 'L1_EM15HI_2TAU12IM_XE35',['L1_EM15HI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["e17_medium_iloose","tau25_medium1_tracktwo", "xe50"]]],
        ['e17_lhmedium_iloose_tau25_medium1_tracktwo_xe50', 'L1_EM15HI_2TAU12IM_XE35',['L1_EM15HI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_iloose","tau25_medium1_tracktwo", "xe50"]]],

        # With L1Topo
        
        ['e17_medium_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I', 'L1_XE35_EM15-TAU12I',['L1_EM15HI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["e17_medium","tau25_medium1_tracktwo", "xe50"]]],
        ['e17_lhmedium_tau25_medium1_tracktwo_xe50_L1XE35_EM15-TAU12I', 'L1_XE35_EM15-TAU12I',['L1_EM15HI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium","tau25_medium1_tracktwo", "xe50"]]],

        ['e17_medium_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I', 'L1_XE40_EM15-TAU12I',['L1_EM15HI','L1_TAU12IM','L1_XE40'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["e17_medium","tau25_medium1_tracktwo", "xe50"]]],
        ['e17_lhmedium_tau25_medium1_tracktwo_xe50_L1XE40_EM15-TAU12I', 'L1_XE40_EM15-TAU12I',['L1_EM15HI','L1_TAU12IM','L1_XE40'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium","tau25_medium1_tracktwo", "xe50"]]],

        # Tau + mu + MET

        ['mu14_tau25_medium1_tracktwo_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14", "tau25_medium1_tracktwo","xe50"]]],
        ['mu14_iloose_tau25_medium1_tracktwo_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14_iloose", "tau25_medium1_tracktwo","xe50"]]],
        
        #['e18_lhloose_tau80_medium1_calo',                 'L1_EM15-TAU40',[], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e18_lhloose","tau80_medium1_calo"]]],


        # VBF triggers
        
        ['g15_loose_2j40_0eta490_3j25_0eta490', 'L1_MJJ-400', ['L1_EM13VH','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1,['serial',-1,["g15_loose","2j40_0eta490","3j25_0eta490"]]],
        ['g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-700', 'L1_MJJ-700', ['L1_EM15VH','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1,['serial',-1,["g20_loose","2j40_0eta490","3j25_0eta490"]]],
        ['g20_loose_2j40_0eta490_3j25_0eta490', 'L1_MJJ-800', ['L1_EM15VH','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1,['serial',-1,["g20_loose","2j40_0eta490","3j25_0eta490"]]],
        ['g20_loose_2j40_0eta490_3j25_0eta490_L1MJJ-900', 'L1_MJJ-900', ['L1_EM15VH','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1,['serial',-1,["g20_loose","2j40_0eta490","3j25_0eta490"]]],
        ['g20_loose_L1EM18VH_2j40_0eta490_3j25_0eta490_invm700', 'L1_EM18VH', ['L1_EM18VH','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1,['serial',-1,["g20_loose_L1EM18VH","2j40_0eta490","3j25_0eta490_invm700"]]],
        ['g25_loose_L1EM20VH_2j40_0eta490_3j25_0eta490_invm700', 'L1_EM20VH', ['L1_EM20VH','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1,['serial',-1,["g25_loose_L1EM20VH","2j40_0eta490","3j25_0eta490_invm700"]]],
        ['g25_loose_2j40_0eta490_3j25_0eta490_invm700', 'L1_EM22VHI', ['L1_EM22VHI','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1,['serial',-1,["g25_loose","2j40_0eta490","3j25_0eta490_invm700"]]],
        ['mu6_2j40_0eta490_invm400', 'L1_MU6_MJJ-200', ['L1_MU6',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu6","2j40_0eta490_invm400"]]],
        ['mu6_2j40_0eta490_invm600', 'L1_MU6_MJJ-300', ['L1_MU6',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu6","2j40_0eta490_invm600"]]],
        ['mu6_2j40_0eta490_invm800', 'L1_MU6_MJJ-400', ['L1_MU6',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu6","2j40_0eta490_invm800"]]],
        ['mu6_2j40_0eta490_invm1000', 'L1_MU6_MJJ-500', ['L1_MU6',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu6","2j40_0eta490_invm1000"]]],

        # lepton + jets

        ['e26_vloose_L1EM20VH_3j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e26_vloose_L1EM20VH","3j20noL1"]]],
        ['e26_vloose_L1EM20VH_4j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e26_vloose_L1EM20VH","4j20noL1"] ]],
        ['e26_vloose_L1EM20VH_5j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e26_vloose_L1EM20VH","5j15noL1"] ]],
        ['e26_vloose_L1EM20VH_6j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e26_vloose_L1EM20VH","6j15noL1"] ]],

        ['e26_lhvloose_L1EM20VH_3j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e26_lhvloose_L1EM20VH","3j20noL1"] ]],
        ['e26_lhvloose_L1EM20VH_4j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e26_lhvloose_L1EM20VH","4j20noL1"] ]],
        ['e26_lhvloose_L1EM20VH_5j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e26_lhvloose_L1EM20VH","5j15noL1"] ]],
        ['e26_lhvloose_L1EM20VH_6j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e26_lhvloose_L1EM20VH","6j15noL1"] ]],

        ['e24_vloose_L1EM20VH_3j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_vloose_L1EM20VH","3j20noL1"] ]],
        ['e24_vloose_L1EM20VH_4j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_vloose_L1EM20VH","4j20noL1"] ]],
        ['e24_vloose_L1EM20VH_5j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_vloose_L1EM20VH","5j15noL1"] ]],
        ['e24_vloose_L1EM20VH_6j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_vloose_L1EM20VH","6j15noL1"] ]],

        ['e24_lhvloose_L1EM20VH_3j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_lhvloose_L1EM20VH","3j20noL1"] ]],
        ['e24_lhvloose_L1EM20VH_4j20noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_lhvloose_L1EM20VH","4j20noL1"] ]],
        ['e24_lhvloose_L1EM20VH_5j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_lhvloose_L1EM20VH","5j15noL1"] ]],
        ['e24_lhvloose_L1EM20VH_6j15noL1',  'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_lhvloose_L1EM20VH","6j15noL1"] ]],

        ['e24_vloose_L1EM18VH_3j20noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_vloose_L1EM18VH","3j20noL1"] ]],
        ['e24_vloose_L1EM18VH_4j20noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_vloose_L1EM18VH","4j20noL1"] ]],
        ['e24_vloose_L1EM18VH_5j15noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_vloose_L1EM18VH","5j15noL1"] ]],
        ['e24_vloose_L1EM18VH_6j15noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_vloose_L1EM18VH","6j15noL1"] ]],

        ['e24_lhvloose_L1EM18VH_3j20noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_lhvloose_L1EM18VH","3j20noL1"] ]],
        ['e24_lhvloose_L1EM18VH_4j20noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_lhvloose_L1EM18VH","4j20noL1"] ]],
        ['e24_lhvloose_L1EM18VH_5j15noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_lhvloose_L1EM18VH","5j15noL1"] ]],
        ['e24_lhvloose_L1EM18VH_6j15noL1',  'L1_EM18VH', ['L1_EM18VH',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1, ['serial',-1,["e24_lhvloose_L1EM18VH","6j15noL1"] ]],

        ['mu26_2j20noL1',  'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu26","2j20noL1"] ]],
        ['mu26_3j20noL1',  'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu26","3j20noL1"] ]],
        ['mu26_4j15noL1',  'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu26","4j15noL1"] ]],
        ['mu26_5j15noL1',  'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu26","5j15noL1"] ]],

        ['mu24_2j20noL1',  'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu24","2j20noL1"] ]],
        ['mu24_3j20noL1',  'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu24","3j20noL1"] ]],
        ['mu24_4j15noL1',  'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu24","4j15noL1"] ]],
        ['mu24_5j15noL1',  'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu24","5j15noL1"] ]],

        ['mu20_L1MU15_2j20noL1',  'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu20_L1MU15","2j20noL1"] ]],
        ['mu20_L1MU15_3j20noL1',  'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu20_L1MU15","3j20noL1"] ]],
        ['mu20_L1MU15_4j15noL1',  'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu20_L1MU15","4j15noL1"] ]],
        ['mu20_L1MU15_5j15noL1',  'L1_MU15', ['L1_MU15',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon'], -1, ['serial',-1,["mu20_L1MU15","5j15noL1"] ]],


        #mujet matching chain
        ['mu4_3j55_dr05', 'L1_MU4_3J20', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu4', '3j55'] ]],
        
        ['mu4_j15_bperf_dr05', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu4', 'j15_bperf'] ]],
        ['mu4_j25_bperf_dr05', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu4', 'j25_bperf'] ]],
        ['mu4_j35_bperf_dr05', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu4', 'j35_bperf'] ]],

        ['mu4_j55_bperf_dr05',  'L1_MU4_J12', ['L1_MU4', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu4', 'j55_bperf'] ]], 
        ['mu6_j85_bperf_dr05',  'L1_MU6_J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu6', 'j85_bperf'] ]], 
        ['mu6_j110_bperf_dr05', 'L1_MU6_J20', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu6', 'j110_bperf'] ]],
        ['mu6_j150_bperf_dr05', 'L1_MU6_J40', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu6', 'j150_bperf'] ]],
        ['mu6_j175_bperf_dr05', 'L1_MU6_J40', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu6', 'j175_bperf']]],
        ['mu6_j260_bperf_dr05', 'L1_MU6_J75', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu6', 'j260_bperf'] ]],
        ['mu6_j320_bperf_dr05', 'L1_MU6_J75', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu6', 'j320_bperf'] ]],
        ['mu6_j400_bperf_dr05', 'L1_MU6_J75', ['L1_MU6', ''], [PhysicsStream], ['RATE:MuonBjet', 'BW:Bjet', 'BW:Muon'], -1,['serial',-1,['mu6', 'j400_bperf'] ]],
        
        #bjet combined:
        ['2mu14_2j35_bperf',                'L1_2MU10',          ['L1_2MU10', ''],     [PhysicsStream], ['RATE:MuonBjet', 'BW:Muon'], -1,['serial',-1,['2mu14', '2j35_bperf']]],
        ['2e17_loose_2j35_bperf',           'L1_2EM15VH',        ['L1_2EM15VH', ''],   [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'],-1,['serial',-1,['2e17_loose', '2j35_bperf']]  ],
        ['mu26_imedium_2j35_bperf',         'L1_MU20',           ['L1_MU20', ''],      [PhysicsStream], ['RATE:MuonBjet', 'BW:Muon'], -1,['serial',-1,['mu26_imedium', '2j35_bperf']  ]],
        ['e26_tight_iloose_2j35_bperf',     'L1_EM22VHI',      ['L1_EM22VHI', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'],-1,['serial',-1,['e26_tight_iloose', '2j35_bperf'] ] ], 

        ]

    TriggerFlags.MinBiasSlice.signatures = TriggerFlags.MinBiasSlice.signatures() + [
        #HMT
        ['mb_sp2000_trk70_hmt', 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],        
        ['mb_sp2000_pusup600_trk70_hmt', 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['mb_sp2000_pusup600_trk70_hmt_L1TE30',  'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['mb_sp2000_pusup600_trk70_hmt_L1TE40',  'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['mb_sp2000_pusup700_trk70_hmt_L1TE30',  'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['mb_sp2000_pusup700_trk70_hmt_L1TE40',  'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 

	#ATR-9419
        ['mb_sp2000_pusup700_trk60_hmt_L1TE30', 'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp2500_pusup750_trk90_hmt_L1TE40', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp3000_pusup800_trk120_hmt_L1TE50', 'L1_TE50', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp2000_pusup700_trk50_sumet70_hmt_L1TE30',   'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['mb_sp2000_pusup700_trk50_sumet110_hmt_L1TE40',  'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp2000_pusup700_trk50_sumet150_hmt_L1TE50',  'L1_TE50', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

	#ATR-9822
        ['mb_sp2000_pusup700_trk60_hmt_L1TE30.0ETA24', 'L1_TE30.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp2000_pusup700_trk50_sumet70_hmt_L1TE30.0ETA24',   'L1_TE30.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

	#supporting HMT triggers
	['mb_sp1800_hmtperf_L1TE20', 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp1800_hmtperf_L1TE30.0ETA24', 'L1_TE30.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],



        ]

    #Beamspot chanis first try ATR-9847                                                                                                                
    TriggerFlags.BeamspotSlice.signatures = TriggerFlags.BeamspotSlice.signatures() + [                                                                 
        ['beamspot_allTE_L2StarB',           'L1_4J15', [], [PhysicsStream], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_allTE_trkfast',           'L1_4J15', [], [PhysicsStream], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ]
    ## The following slices are only needed for Physics runnings and LS1 menus
    ## To be commented out for DC14
    TriggerFlags.CalibSlice.signatures   = TriggerFlags.CalibSlice.signatures() + []
    TriggerFlags.CosmicSlice.signatures  = TriggerFlags.CosmicSlice.signatures() + []
    TriggerFlags.StreamingSlice.signatures = TriggerFlags.StreamingSlice.signatures() + [


        #['noalg_L1J400', 'L1_J400', [], [PhysicsStream], ['RATE:SeededStreamers', "BW:Jets"], -1],

        ['noalg_mb_L1TE20',  'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['noalg_mb_L1TE30',  'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['noalg_mb_L1TE40',  'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['noalg_mb_L1TE50',  'L1_TE50', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
	['noalg_mb_L1TE30.0ETA24',  'L1_TE30.0ETA24', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

        #Muon streamers
        ['noalg_L1MU20',           'L1_MU20',           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L1MU15',           'L1_MU15',           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L1MU10',           'L1_MU10',           [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L1MU4_EMPTY',      'L1_MU4_EMPTY',      [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        ['noalg_L1MU4_FIRSTEMPTY', 'L1_MU4_FIRSTEMPTY', [], [PhysicsStream], ['RATE:SeededStreamers', 'BW:Muon'], -1],
        #['noalg_L1MU10_MU6',      'L1_MU10_MU6',       [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],

        # VBF triggers
        ['noalg_L1J30.0ETA49_2J20.0ETA49',    'L1_J30.0ETA49_2J20.0ETA49',    [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1],
        ['noalg_L1MJJ-100', 'L1_MJJ-100', [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1],
                
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

     # Chains in physics_pp_v5, except for:
     #     2mu4_bDimu, 2mu4_bDimu_novtx_noos, 2mu6_bDimu, 2mu6_bDimu_novtx_noos
     #     mb_mbts_L1MBTS_2, mb_sptrk, mb_perf_L1LUCID, mb_sptrk_noisesup
    'beamspot_allTE_L2StarB_L1RDO_FILLED'   : [    -1,    0,   -1],
    'beamspot_allTE_trkfast_L1RDO_FILLED'   : [    -1,    0,   -1],
    'costmonitor'                           : [    -1,    0,   -1],
    'cscmon_L1EM3'                          : [    -1,    0,   -1],
    'cscmon_L1J12'                          : [    -1,    0,   -1],
    'cscmon_L1MU10'                         : [    -1,    0,   -1],
    'e0_L2Star_perf_L1EM3'                  : [    -1,    0,   -1],
    'e0_perf_L1EM3'                         : [    -1,    0,   -1],
    'e3_etcut'                              : [    -1,    0,   -1],
    'e3_etcut_L1EM3_EMPTY'                  : [    -1,    0,   -1],
    'e3_lhloose'                            : [    -1,    0,   -1],
    'e3_loose'                              : [    -1,    0,   -1],
    'eb_empty_L1RD0_EMPTY'                  : [    -1,    0,   -1],
    'g0_perf_L1EM3'                         : [    -1,    0,   -1],
    'g3_etcut'                              : [    -1,    0,   -1],
    'g3_etcut_L1EM3_EMPTY'                  : [    -1,    0,   -1],
    'ht0_L1J12'                             : [    -1,    0,   -1],
    'ht0_perf_L1J12'                        : [    -1,    0,   -1],
    'ibllumi_L1RD0_EMPTY'                   : [    -1,    0,   -1],
    'ibllumi_L1RD0_FILLED'                  : [    -1,    0,   -1],
    'ibllumi_L1RD0_UNPAIRED_ISO'            : [    -1,    0,   -1],
    'id_cosmicid'                           : [    -1,    0,   -1],
    'id_cosmicid_L1MU11'                    : [    -1,    0,   -1],
    'id_cosmicid_L1MU4'                     : [    -1,    0,   -1],
    'id_cosmicid_trtxk'                     : [    -1,    0,   -1],
    'id_cosmicid_trtxk_central'             : [    -1,    0,   -1],
    'j0_L1J12'                              : [    -1,    0,   -1],
    'j0_PS_L1J12'                           : [    -1,    0,   -1],
    'j0_a10_lcw_sub_L1J12'                  : [    -1,    0,   -1],
    'j0_a10_nojcalib_L1J12'                 : [    -1,    0,   -1],
    'j0_a10_nojcalib_L1RD0_EMPTY'           : [    -1,    0,   -1],
    'j0_a10r_lcw_sub_L1J12'                 : [    -1,    0,   -1],
    'j0_a10r_lcw_sub_L1RD0_EMPTY'           : [    -1,    0,   -1],
    'j0_jes_L1J12'                          : [    -1,    0,   -1],
    'j0_lcw_jes_L1J12'                      : [    -1,    0,   -1],
    'j0_nojcalib_L1J12'                     : [    -1,    0,   -1],
    'j0_perf_L1J12'                         : [    -1,    0,   -1],
    'j0_perf_L1MU10'                        : [    -1,    0,   -1],
    'j0_perf_L1RD0_EMPTY'                   : [    -1,    0,   -1],
    'j0_perf_bperf_L1J12'                   : [    -1,    0,   -1],
    'j0_perf_bperf_L1MU10'                  : [    -1,    0,   -1],
    'j0_perf_bperf_L1RD0_EMPTY'             : [    -1,    0,   -1],
    'j0_perf_ds1_L1All'                     : [    -1,    0,   -1],
    'j0_sub_L1J12'                          : [    -1,    0,   -1],
    'j15_L1J12'                             : [    -1,    0,   -1],
    'j40_L1ZB'                              : [    -1,    0,   -1],
    'l1calocalib'                           : [    -1,    0,   -1],
    'l1calocalib_L1BGRP7'                   : [    -1,    0,   -1],
    'larcalib_L1EM3_EMPTY'                  : [    -1,    0,   -1],
    'larcalib_L1J12_EMPTY'                  : [    -1,    0,   -1],
    'larcalib_L1J3032ETA49_EMPTY'           : [    -1,    0,   -1],
    'larcalib_L1TAU8_EMPTY'                 : [    -1,    0,   -1],
    'larhec_L1J12_FIRSTEMPTY'               : [    -1,    0,   -1],
    'larnoiseburst_L1All'                   : [    -1,    0,   -1],
    'larnoiseburst_L1RD0_EMPTY'             : [    -1,    0,   -1],
    'larnoiseburst_rerun'                   : [    -1,    0,   -1],
    'larps_L1EM3_EMPTY'                     : [    -1,    0,   -1],
    'larps_L1EM7_EMPTY'                     : [    -1,    0,   -1],
    'larps_L1J12_EMPTY'                     : [    -1,    0,   -1],
    'larps_L1J12_FIRSTEMPTY'                : [    -1,    0,   -1],
    'larps_L1J30_EMPTY'                     : [    -1,    0,   -1],
    'larps_L1J30_FIRSTEMPTY'                : [    -1,    0,   -1],
    'larps_L1TAU8_EMPTY'                    : [    -1,    0,   -1],
#    'mb_mbts_L1MBTS_1'                      : [    -1,    0,   -1],
#    'mb_mbts_L1MBTS_1_1'                    : [    -1,    0,   -1],
    'mb_mbts_L1MBTS_1_1_EMPTY'              : [    -1,    0,   -1],
    'mb_mbts_L1MBTS_1_1_UNPAIRED_ISO'       : [    -1,    0,   -1],
    'mb_mbts_L1MBTS_1_EMPTY'                : [    -1,    0,   -1],
    'mb_mbts_L1MBTS_1_UNPAIRED_ISO'         : [    -1,    0,   -1],
#    'mb_mbts_L1MBTS_2'                      : [    -1,    0,   -1],
    'mb_mbts_L1MBTS_2_EMPTY'                : [    -1,    0,   -1],
    'mb_mbts_L1MBTS_2_UNPAIRED_ISO'         : [    -1,    0,   -1],
#    'mb_perf_L1LUCID'                       : [    -1,    0,   -1],
    'mb_perf_L1LUCID_EMPTY'                 : [    -1,    0,   -1],
    'mb_perf_L1LUCID_UNPAIRED_ISO'          : [    -1,    0,   -1],
#    'mb_perf_L1MBTS_2'                      : [    -1,    0,   -1],
#    'mb_perf_L1RD1_FILLED'                  : [    -1,    0,   -1],
#    'mb_sptrk'                              : [    -1,    0,   -1],
    'mb_sptrk_L1RD0_EMPTY'                  : [    -1,    0,   -1],
    'mb_sptrk_L1RD0_UNPAIRED_ISO'           : [    -1,    0,   -1],
    'mb_sptrk_costr'                        : [    -1,    0,   -1],
    'mb_sptrk_costr_L1RD0_EMPTY'            : [    -1,    0,   -1],
#    'mb_sptrk_noisesup'                     : [    -1,    0,   -1],
    'mb_sptrk_noisesup_L1RD0_EMPTY'         : [    -1,    0,   -1],
    'mb_sptrk_noisesup_L1RD0_UNPAIRED_ISO'  : [    -1,    0,   -1],
    'mu0_muoncalib'                         : [    -1,    0,   -1],
    'mu4_cosmicEF_L1MU11_EMPTY'             : [    -1,    0,   -1],
    'mu4_cosmicEF_L1MU4_EMPTY'              : [    -1,    0,   -1],
    'mu4_cosmic_L1MU11_EMPTY'               : [    -1,    0,   -1],
    'mu4_cosmic_L1MU4_EMPTY'                : [    -1,    0,   -1],
    'mu4_msonly_cosmicEF_L1MU11_EMPTY'      : [    -1,    0,   -1],
    'mu4_msonly_cosmicEF_L1MU4_EMPTY'       : [    -1,    0,   -1],
    'mu4_msonly_cosmic_L1MU11_EMPTY'        : [    -1,    0,   -1],
    'mu4_msonly_cosmic_L1MU4_EMPTY'         : [    -1,    0,   -1],
#    '2mu4_bDimu'                            : [    -1,    0,   -1],
#    '2mu4_bDimu_novtx_noos'                 : [    -1,    0,   -1],
#    '2mu6_bDimu'                            : [    -1,    0,   -1],
#    '2mu6_bDimu_novtx_noos'                 : [    -1,    0,   -1],
    'noalg_L1All'                           : [    -1,    0,   -1],
    'noalg_L1Calo'                          : [    -1,    0,   -1],
    'noalg_L1Calo_EMPTY'                    : [    -1,    0,   -1],
    'noalg_L1MinBias'                       : [    -1,    0,   -1],
    'noalg_L1Standby'                       : [    -1,    0,   -1],
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
    'noalg_idcosmic_L1TRT'                  : [    -1,    0,   -1],
    'noalg_idmon_L1RD0_EMPTY'               : [    -1,    0,   -1],
    'noalg_idmon_L1RD0_FILLED'              : [    -1,    0,   -1],
    'noalg_idmon_L1RD0_UNPAIRED_ISO'        : [    -1,    0,   -1],
    'noalg_mb_L1LUCID'                      : [    -1,    0,   -1],
    'noalg_mb_L1LUCID_EMPTY'                : [    -1,    0,   -1],
    'noalg_mb_L1LUCID_UNPAIRED_ISO'         : [    -1,    0,   -1],
    'noalg_mb_L1MBTS_1'                     : [    -1,    0,   -1],
    'noalg_mb_L1MBTS_1_1'                   : [    -1,    0,   -1],
    'noalg_mb_L1MBTS_1_1_EMPTY'             : [    -1,    0,   -1],
    'noalg_mb_L1MBTS_1_1_UNPAIRED_ISO'      : [    -1,    0,   -1],
    'noalg_mb_L1MBTS_1_EMPTY'               : [    -1,    0,   -1],
    'noalg_mb_L1MBTS_1_UNPAIRED_ISO'        : [    -1,    0,   -1],
    'noalg_mb_L1MBTS_2'                     : [    -1,    0,   -1],
    'noalg_mb_L1MBTS_2_EMPTY'               : [    -1,    0,   -1],
    'noalg_mb_L1MBTS_2_UNPAIRED_ISO'        : [    -1,    0,   -1],
    'noalg_mb_L1RD0_EMPTY'                  : [    -1,    0,   -1],
    'noalg_mb_L1RD0_FILLED'                 : [    -1,    0,   -1],
    'noalg_mb_L1RD0_UNPAIRED_ISO'           : [    -1,    0,   -1],
    'noalg_mb_L1RD1_EMPTY'                  : [    -1,    0,   -1],
    'noalg_mb_L1RD1_FILLED'                 : [    -1,    0,   -1],
    'noalg_zb_L1ZB'                         : [    -1,    0,   -1],
    'pixel_beam'                            : [    -1,    0,   -1],
    'pixel_noise'                           : [    -1,    0,   -1],
    'robrequest'                            : [    -1,    0,   -1],
    'robrequest_L1RD0_EMPTY'                : [    -1,    0,   -1],
    'sct_noise'                             : [    -1,    0,   -1],
    'tau1_cosmic_ptonly_L1MU4_EMPTY'        : [    -1,    0,   -1],
    'tau1_cosmic_track_L1MU4_EMPTY'         : [    -1,    0,   -1],
    'tau8_cosmic_ptonly'                    : [    -1,    0,   -1],
    'tau8_cosmic_track'                     : [    -1,    0,   -1],
    'te20'                                  : [    -1,    0,   -1],
    'te20_tc_lcw'                           : [    -1,    0,   -1],
    'tilecalib_laser'                       : [    -1,    0,   -1],
    'tilelarcalib_L1EM3_EMPTY'              : [    -1,    0,   -1],
    'tilelarcalib_L1J12_EMPTY'              : [    -1,    0,   -1],
    'tilelarcalib_L1J3032ETA49_EMPTY'       : [    -1,    0,   -1],
    'tilelarcalib_L1TAU8_EMPTY'             : [    -1,    0,   -1],
    'timeburner'                            : [    -1,    0,   -1],
    'xe0_L1All'                             : [    -1,    0,   -1],
    'xe0_L1XE35'                            : [    -1,    0,   -1],
    'xe0_L2FS_L1All'                        : [    -1,    0,   -1],
    'xe0_L2FS_L1XE35'                       : [    -1,    0,   -1],
    'xe0_l2fsperf_wEFMuFEB_wEFMu_L1All'     : [    -1,    0,   -1],
    'xe0_l2fsperf_wEFMuFEB_wEFMu_L1XE35'    : [    -1,    0,   -1],
    'xe0_tc_em_L1All'                       : [    -1,    0,   -1],
    'xe0_tc_em_L1XE35'                      : [    -1,    0,   -1],
    'xe0_tc_lcw_L1All'                      : [    -1,    0,   -1],
    'xe0_tc_lcw_L1XE35'                     : [    -1,    0,   -1],
    'xe0_tc_mht_L1All'                      : [    -1,    0,   -1],
    'xe0_tc_mht_L1XE35'                     : [    -1,    0,   -1],
    'xe0_tc_pueta_L1All'                    : [    -1,    0,   -1],
    'xe0_tc_pueta_L1XE35'                   : [    -1,    0,   -1],
    'xe0_tc_pufit_L1All'                    : [    -1,    0,   -1],
    'xe0_tc_pufit_L1XE35'                   : [    -1,    0,   -1],
    'xe35'                                  : [    -1,    0,   -1],
    'xe35_L2FS'                             : [    -1,    0,   -1],
    'xe35_l2fsperf_wEFMuFEB_wEFMu'          : [    -1,    0,   -1],
    'xe35_mht'                              : [    -1,    0,   -1],
    'xe35_pueta'                            : [    -1,    0,   -1],
    'xe35_pufit'                            : [    -1,    0,   -1],
    'xe35_tc_em'                            : [    -1,    0,   -1],
    'xe35_tc_lcw'                           : [    -1,    0,   -1],
    'xs30'                                  : [    -1,    0,   -1],
    'xs30_tc_lcw'                           : [    -1,    0,   -1],

    # Beamspot chains in MC_pp_v5
    'beamspot_allTE_L2StarB'                : [    -1,    0,   -1],
    'beamspot_allTE_trkfast'                : [    -1,    0,   -1],

    # Streamers in MC_pp_v5
    'noalg_L1J30.0ETA49_2J20.0ETA49'        : [    -1,    0,   -1],
    'noalg_L1MJJ-100'                       : [    -1,    0,   -1],
    'noalg_L1MU10'                          : [    -1,    0,   -1],
    'noalg_L1MU15'                          : [    -1,    0,   -1],
    'noalg_L1MU20'                          : [    -1,    0,   -1],
    'noalg_L1MU4_EMPTY'                     : [    -1,    0,   -1],
    'noalg_L1MU4_FIRSTEMPTY'                : [    -1,    0,   -1],
    'noalg_mb_L1TE20'                       : [    -1,    0,   -1],
    'noalg_mb_L1TE30'                       : [    -1,    0,   -1],
    'noalg_mb_L1TE30.0ETA24'                : [    -1,    0,   -1],
    'noalg_mb_L1TE40'                       : [    -1,    0,   -1],
    'noalg_mb_L1TE50'                       : [    -1,    0,   -1],
 
    # ----------
    # rerun (only for slow ones)
    # ----------
    'xe0noL1_l2fsperf'                      : [    0,    0,   1],
    'xe0noL1_l2fsperf_mht'                  : [    0,    0,   1],
    'xe0noL1_l2fsperf_pueta'                : [    0,    0,   1],
    'xe0noL1_l2fsperf_pufit'                : [    0,    0,   1],
    'xe0noL1_l2fsperf_tc_em'                : [    0,    0,   1],
    'xe0noL1_l2fsperf_tc_lcw'               : [    0,    0,   1],
    'mu0noL1_fsperf'                        : [    0,    0,   1],
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
    'mb_mbts_L1MBTS_1'                      : [    -1,    0,   -1],
    'mb_mbts_L1MBTS_1_1'                    : [    -1,    0,   -1],
    'mb_mbts_L1MBTS_2'                      : [    -1,    0,   -1],
    'mb_perf_L1LUCID'                       : [    -1,    0,   -1],
    'mb_perf_L1MBTS_2'                      : [    -1,    0,   -1],
    'mb_perf_L1RD1_FILLED'                  : [    -1,    0,   -1],
    'mb_sptrk'                              : [    -1,    0,   -1],
    'mb_sptrk_noisesup'                     : [    -1,    0,   -1],

     # Minbias chains in MC_pp_v5
    'mb_sp1800_hmtperf_L1TE20'              : [    -1,    0,   -1],
    'mb_sp1800_hmtperf_L1TE30.0ETA24'       : [    -1,    0,   -1],
    'mb_sp2000_pusup600_trk70_hmt'          : [    -1,    0,   -1],
    'mb_sp2000_pusup600_trk70_hmt_L1TE30'   : [    -1,    0,   -1],
    'mb_sp2000_pusup600_trk70_hmt_L1TE40'   : [    -1,    0,   -1],
    'mb_sp2000_pusup700_trk50_sumet110_hmt_L1TE40': [    -1,    0,   -1],
    'mb_sp2000_pusup700_trk50_sumet150_hmt_L1TE50': [    -1,    0,   -1],
    'mb_sp2000_pusup700_trk50_sumet70_hmt_L1TE30': [    -1,    0,   -1],
    'mb_sp2000_pusup700_trk50_sumet70_hmt_L1TE30.0ETA24': [    -1,    0,   -1],
    'mb_sp2000_pusup700_trk60_hmt_L1TE30'   : [    -1,    0,   -1],
    'mb_sp2000_pusup700_trk60_hmt_L1TE30.0ETA24': [    -1,    0,   -1],
    'mb_sp2000_pusup700_trk70_hmt_L1TE30'   : [    -1,    0,   -1],
    'mb_sp2000_pusup700_trk70_hmt_L1TE40'   : [    -1,    0,   -1],
    'mb_sp2000_trk70_hmt'                   : [    -1,    0,   -1],
    'mb_sp2500_pusup750_trk90_hmt_L1TE40'   : [    -1,    0,   -1],
    'mb_sp3000_pusup800_trk120_hmt_L1TE50'  : [    -1,    0,   -1],
    })
######################################################

######################################################
# Special mc prescale: tight + electron + bjets prescaled
######################################################
Prescales.L1Prescales_special_mc_prescale  = deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_special_mc_prescale = deepcopy(Prescales.HLTPrescales_tight_mc_prescale)
Prescales.HLTPrescales_special_mc_prescale.update({
    'e0_L2Star_perf_L1EM15'                 : [    5,    0,   -1],
    'e0_perf_L1EM15'                        : [    5,    0,   -1],
    'e4_etcut'                              : [    5,    0,   -1],
    'e5_etcut'                              : [    5,    0,   -1],
    'e5_etcut_L1W-05RO-XEHT-0': [    5,    0,   -1],
    'e5_etcut_L1W-08RO-XEHT-1': [    5,    0,   -1],
    'e5_etcut_L1W-10DPHI-JXE-0'      : [    5,    0,   -1],
    'e5_etcut_L1W-20DPHI-JXE-1'      : [    5,    0,   -1],
    'e5_etcut_L1W-20DPHI-JXE-0'        : [    5,    0,   -1],
    'e5_etcut_L1W-40RO2-XEHT-0': [    5,    0,   -1],
    'e5_etcut_L1W-90RO2-XEHT-1': [    5,    0,   -1],
    'e5_etcut_L1HT20-JJ0.ETA49'         : [    5,    0,   -1],
    'e5_etcut_L1W-NOMATCH': [    5,    0,   -1],
    'e5_etcut_L1W-NOMATCH_W-05RO-XEEMHT': [    5,    0,   -1],
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
    'e18_loose1_tau25_medium1_calo'         : [    5,    0,   -1],
    'e18_loose1_tau25_medium1_ptonly'       : [    5,    0,   -1],
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
    'e24_lhmedium_cutd0dphi_L1EM18VH'       : [    5,    0,   -1],
    'e24_lhmedium_cutd0dphi_iloose_L1EM18VH': [    5,    0,   -1],
    'e24_lhmedium_idperf_L1EM20VH'          : [    5,    0,   -1],
    'e24_lhmedium_iloose_L1EM18VH'          : [    5,    0,   -1],
    'e24_lhmedium_iloose_L1EM20VH'          : [    5,    0,   -1],
    'e24_lhmedium_iloose_L2StarA_L1EM20VH'  : [    5,    0,   -1],
    'e24_lhmedium_nod0_L1EM18VH'            : [    5,    0,   -1],
    'e24_lhmedium_nod0_iloose_L1EM18VH'     : [    5,    0,   -1],
    'e24_lhmedium_nodphi_L1EM18VH'          : [    5,    0,   -1],
    'e24_lhmedium_nodphi_iloose_L1EM18VH'   : [    5,    0,   -1],
    'e24_lhmedium_nodphires_L1EM18VH'       : [    5,    0,   -1],
    'e24_lhmedium_nodphires_iloose_L1EM18VH': [    5,    0,   -1],
    'e24_lhtight_L1EM20VH'                  : [    5,    0,   -1],
    'e24_lhtight_L1EM20VH_e15_etcut_Zee'    : [    5,    0,   -1],
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
    'e24_tight_L1EM20VH_e15_etcut_Zee'      : [    5,    0,   -1],
    'e24_tight_iloose'                      : [    5,    0,   -1],
    'e24_tight_iloose_HLTCalo_L1EM20VH'     : [    5,    0,   -1],
    'e24_tight_iloose_L1EM20VH'             : [    5,    0,   -1],
    'e24_tight_iloose_L2EFCalo_L1EM20VH'    : [    5,    0,   -1],
    'e24_tight_iloose_etisem_L1EM20VH'      : [    5,    0,   -1],
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
    'e26_lhtight_e15_etcut_Zee'             : [    5,    0,   -1],
    'e26_lhtight_iloose'                    : [    5,    0,   -1],
    'e26_lhvloose_L1EM20VH'                 : [    5,    0,   -1],
    'e26_lhvloose_L1EM20VH_3j20noL1'        : [    5,    0,   -1],
    'e26_lhvloose_L1EM20VH_4j20noL1'        : [    5,    0,   -1],
    'e26_lhvloose_L1EM20VH_5j15noL1'        : [    5,    0,   -1],
    'e26_lhvloose_L1EM20VH_6j15noL1'        : [    5,    0,   -1],
    'e26_tight1_iloose'                     : [    5,    0,   -1],
    'e26_tight_e15_etcut_Zee'               : [    5,    0,   -1],
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
    'g0_hiptrt_L1EM22VHI'                 : [    5,    0,   -1],
    'g0_perf_L1EM15'                        : [    5,    0,   -1],
    'g10_loose'                             : [    5,    0,   -1],
    'g10_loose_L1EM7'                       : [    5,    0,   -1],
    'g10_loose_mu10'                        : [    5,    0,   -1],
    'g10_loose_mu10_taumass'                : [    5,    0,   -1],
    'g10_medium'                            : [    5,    0,   -1],
    'g10_medium_L1EM7'                      : [    5,    0,   -1],
    'g15_loose'                             : [    5,    0,   -1],
    'g15_loose_2j40_0eta490_5j25_0eta490'   : [    5,    0,   -1],
    'g15_loose_2mu10_msonly'                : [    5,    0,   -1],
    'g15_loose_L1EM3'                       : [    5,    0,   -1],
    'g15_loose_L1EM7'                       : [    5,    0,   -1],
    'g20_etcut_L1EM12'                      : [    5,    0,   -1],
    'g20_loose'                             : [    5,    0,   -1],
    'g20_loose_2g15_loose_L12EM13VH'        : [    5,    0,   -1],
    'g20_loose_2j40_0eta490_5j25_0eta490'   : [    5,    0,   -1],
    'g20_loose_2j40_0eta490_5j25_0eta490_L1MJJ-700': [    5,    0,   -1],
    'g20_loose_2j40_0eta490_5j25_0eta490_L1MJJ-900': [    5,    0,   -1],
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
    'g35_loose1_g25_loose1'                 : [    5,    0,   -1],
    'g35_medium'                            : [    5,    0,   -1],
    'g35_medium_HLTCalo_g25_medium_HLTCalo' : [    5,    0,   -1],
    'g35_medium_g25_medium'                 : [    5,    0,   -1],
    'g35_medium1_g25_medium1'               : [    5,    0,   -1],
    'g40_loose_L1EM15'                      : [    5,    0,   -1],
    'g40_tight'                             : [    5,    0,   -1],
    'g40_tight_xe40noL1'                    : [    5,    0,   -1],
    'g45_loose_L1EM15'                      : [    5,    0,   -1],
    'g45_tight'                             : [    5,    0,   -1],
    'g45_tight_L1EM22VHI_xe45noL1'        : [    5,    0,   -1],
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
    'g120_loose1'                           : [    5,    0,   -1],
    'g120_loose_HLTCalo'                    : [    5,    0,   -1],
    'g140_loose'                            : [    5,    0,   -1],
    'g200_etcut'                            : [    5,    0,   -1],
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
    'j65_btight_3j65_L13J25.0ETA23'         : [    5,    0,   -1],
    'j65_btight_3j65_L14J15.0ETA24'         : [    5,    0,   -1],
    'j65_btight_split_3j65_L13J25.0ETA23'   : [    5,    0,   -1],
    'j65_btight_split_3j65_L14J15.0ETA24'   : [    5,    0,   -1],
    'j70_bmedium_3j70_L13J25.0ETA23'        : [    5,    0,   -1],
    'j70_bmedium_3j70_L14J15.0ETA24'        : [    5,    0,   -1],
    'j70_bmedium_split_3j70_L13J25.0ETA23'  : [    5,    0,   -1],
    'j70_bmedium_split_3j70_L14J15.0ETA24'  : [    5,    0,   -1],
    'j70_btight_3j70'                       : [    5,    0,   -1],
    'j70_btight_3j70_L13J25.ETA23'          : [    5,    0,   -1],
    'j70_btight_split_3j70'                 : [    5,    0,   -1],
    'j70_btight_split_3j70_L13J25.ETA23'    : [    5,    0,   -1],
    'j75_bmedium_3j75'                      : [    5,    0,   -1],
    'j75_bmedium_3j75_L13J25.ETA23'         : [    5,    0,   -1],
    'j75_bmedium_split_3j75'                : [    5,    0,   -1],
    'j75_bmedium_split_3j75_L13J25.ETA23'   : [    5,    0,   -1],
    'j100_2j55_bmedium'                     : [    5,    0,   -1],
    'j100_2j55_bmedium_split'               : [    5,    0,   -1],
    'j225_bloose'                           : [    5,    0,   -1],
    'j225_bloose_split'                     : [    5,    0,   -1],
    'j300_bloose'                           : [    5,    0,   -1],
    'j300_bloose_split'                     : [    5,    0,   -1],
    '2j35_btight_2j35_L13J25.0ETA23'        : [    5,    0,   -1],
    '2j35_btight_2j35_L14J15.0ETA24'        : [    5,    0,   -1],
    '2j35_btight_split_2j35_L13J25.0ETA23'  : [    5,    0,   -1],
    '2j35_btight_split_2j35_L14J15.0ETA24'  : [    5,    0,   -1],
    '2j45_bmedium_2j45_L13J25.0ETA23'       : [    5,    0,   -1],
    '2j45_bmedium_2j45_L14J15.0ETA24'       : [    5,    0,   -1],
    '2j45_bmedium_split_2j45_L13J25.0ETA23' : [    5,    0,   -1],
    '2j45_bmedium_split_2j45_L14J15.0ETA24' : [    5,    0,   -1],
    '2j45_btight_2j45'                      : [    5,    0,   -1],
    '2j45_btight_2j45_L13J25.ETA23'         : [    5,    0,   -1],
    '2j45_btight_split_2j45'                : [    5,    0,   -1],
    '2j45_btight_split_2j45_L13J25.ETA23'   : [    5,    0,   -1],
    '2j55_bloose'                           : [    5,    0,   -1],
    '2j55_bloose_L13J20_4J20.0ETA49_MJJ-400': [    5,    0,   -1],
    '2j55_bloose_L13J20_4J20.0ETA49_MJJ-700': [    5,    0,   -1],
    '2j55_bloose_L13J20_4J20.0ETA49_MJJ-800': [    5,    0,   -1],
    '2j55_bloose_L13J20_4J20.0ETA49_MJJ-900': [    5,    0,   -1],
    '2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-700': [    5,    0,   -1],
    '2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-800': [    5,    0,   -1],
    '2j55_bloose_L1J30_2J20_4J20.0ETA49_MJJ-900': [    5,    0,   -1],
    '2j55_bmedium'                          : [    5,    0,   -1],
    '2j55_bmedium_2j55'                     : [    5,    0,   -1],
    '2j55_bmedium_2j55_L13J25.ETA23'        : [    5,    0,   -1],
    '2j55_bmedium_L13J20_4J20.0ETA49_MJJ_700': [    5,    0,   -1],
    '2j55_bmedium_L13J20_4J20.0ETA49_MJJ-800': [    5,    0,   -1],
    '2j55_bmedium_L13J20_4J20.0ETA49_MJJ-900': [    5,    0,   -1],
    '2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-400': [    5,    0,   -1],
    '2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-700': [    5,    0,   -1],
    '2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-800': [    5,    0,   -1],
    '2j55_bmedium_L1J30_2J20_4J20.0ETA49_MJJ-900': [    5,    0,   -1],
    '2j55_bmedium_split_2j55'               : [    5,    0,   -1],
    '2j55_bmedium_split_2j55_L13J25.ETA23'  : [    5,    0,   -1],
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
