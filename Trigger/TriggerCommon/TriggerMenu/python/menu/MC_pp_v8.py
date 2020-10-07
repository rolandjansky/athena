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

    TriggerFlags.BeamspotSlice.signatures = TriggerFlags.BeamspotSlice.signatures()+ [
        ]

    TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
        #ATR-19452 (muon)
        ['mu4_muonqualL',          'L1_MU4',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu4_muonqualM',          'L1_MU4',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu6_muonqualL',          'L1_MU6',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu6_muonqualM',          'L1_MU6',            [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['2mu12_muonqualL',                  'L1_2MU10',          [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu12_muonqualM',                  'L1_2MU10',          [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu14_muonqualL',                  'L1_2MU10',          [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu14_muonqualM',                  'L1_2MU10',          [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu6_muonqualL',                   'L1_3MU6',           [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu6_muonqualM',                   'L1_3MU6',           [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],


        # ATR-19382
        ['mu10_ivarmedium_mu10',          'L1_2MU10', [], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['parallel',-1,[] ]],
        ['2mu10_AND_mu10_ivarmedium',     'L1_2MU10', ['L1_2MU10','L1_MU10'], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['2mu10_AND', 'mu10_ivarmedium'],False]],

        ['mu10_ivarmedium_mu10_20invm60',          'L1_2MU10', [], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['parallel', -1, [] ]],
        ['mu20_ivarmedium_mu8noL1_20invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu8noL1_20invm60']]],
        ['mu20_ivarmedium_mu6noL1_20invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu6noL1_20invm60']]],
        ['mu20_ivarmedium_mu4noL1_20invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu4noL1_20invm60']]],
        ['mu20_ivarmedium_mu2noL1_20invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu2noL1_20invm60']]],

        ['mu10_ivarmedium_mu10_10invm60',          'L1_2MU10', [], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['parallel', -1, [] ]],
        ['mu20_ivarmedium_mu8noL1_10invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu8noL1_10invm60']]],
        ['mu20_ivarmedium_mu6noL1_10invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu6noL1_10invm60']]],
        ['mu20_ivarmedium_mu4noL1_10invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu4noL1_10invm60']]],
        ['mu20_ivarmedium_mu2noL1_10invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu2noL1_10invm60']]],

        ['mu20_ivarmedium_mu8noL1_10invm70',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu8noL1_10invm70']]],
        ['mu20_ivarmedium_mu6noL1_10invm70',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu6noL1_10invm70']]],
        ['mu20_ivarmedium_mu2noL1_10invm70',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu2noL1_10invm70']]],

        ['mu10_ivarmedium_mu10_0invm60',          'L1_2MU10', [], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['parallel', -1, [] ]],
        ['mu20_ivarmedium_mu8noL1_0invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu8noL1_0invm60']]],
        ['mu20_ivarmedium_mu6noL1_0invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu6noL1_0invm60']]],
        ['mu20_ivarmedium_mu4noL1_0invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu4noL1_0invm60']]],
        ['mu20_ivarmedium_mu2noL1_0invm60',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu2noL1_0invm60']]],

        ['mu10_ivarmedium_mu10_20invm80',          'L1_2MU10', [], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['parallel', -1, [] ]],
        ['mu20_ivarmedium_mu8noL1_20invm80',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu8noL1_20invm80']]],
        ['mu20_ivarmedium_mu6noL1_20invm80',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu6noL1_20invm80']]],
        ['mu20_ivarmedium_mu4noL1_20invm80',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu4noL1_20invm80']]],
        ['mu20_ivarmedium_mu2noL1_20invm80',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu2noL1_20invm80']]],

        ['mu10_ivarmedium_mu10_10invm80',          'L1_2MU10', [], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['parallel', -1, [] ]],
        ['mu20_ivarmedium_mu8noL1_10invm80',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu8noL1_10invm80']]],
        ['mu20_ivarmedium_mu6noL1_10invm80',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu6noL1_10invm80']]],
        ['mu20_ivarmedium_mu4noL1_10invm80',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu4noL1_10invm80']]],
        ['mu20_ivarmedium_mu2noL1_10invm80',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial', -1, ['mu20_ivarmedium', 'mu2noL1_10invm80']]],

        #ATR-19579
        ['2mu14_L12MU11',           'L1_2MU10',          [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu14_L1MU11_2MU10',      'L1_MU11_2MU10',     ['L1_2MU10'], [PhysicsStream], ['RATE:MultiMuon',  'BW:Muon'], -1],



        #ATR-19267
        ['mu15',                   'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu18',                   'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],

        #ATR-19382
        ['mu20_ivarmedium_mu6noL1',    'L1_MU20MU21',  ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_ivarmedium','mu6noL1']]],
        ['mu20_ivarmedium_mu4noL1',    'L1_MU20MU21',  ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_ivarmedium','mu4noL1']]],
        ['mu20_ivarmedium_mu2noL1',    'L1_MU20MU21',  ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_ivarmedium','mu2noL1']]],

        ['mu22_ivarmedium',	     'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],

        ['3mu4_L1BPH-0M10-3MU4',               'L1_BPH-0M10-3MU4', ['L1_3MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1], #ATR-19355

# ATR-20505
        ['2mu50_msonly',           'L1_MU20', ['L1_MU20','L1_MU20'],    [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu40_msonly_3layersEC',           'L1_MU20',  ['L1_MU20','L1_MU20'],       [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['mu50_msonly_mu40noL1_msonly',           'L1_MU20', ['L1_MU20',''],    [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu50_msonly','mu40noL1_msonly']]],
        ['mu60_msonly_mu50noL1_msonly',           'L1_MU20', ['L1_MU20',''],    [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu60_msonly','mu50noL1_msonly']]],
       ['mu50_msonly_3layersEC_mu40noL1_msonly',           'L1_MU20', ['L1_MU20',''],    [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu50_msonly_3layersEC','mu40noL1_msonly']]],
        
        # ATR-20573
        ['mu18_mu10_L12MU10',          'L1_2MU10', [], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['parallel',-1,[] ]],
        ['mu15_mu12_L12MU10',          'L1_2MU10', [], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['parallel',-1,[] ]],

        ]

    TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + [
        ['j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ-400-NFF-0DPHI26', 'L1_MJJ-400-NFF-0DPHI26',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ-400-NFF-0DPHI24', 'L1_MJJ-400-NFF-0DPHI24',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ-400-NFF-0DPHI22', 'L1_MJJ-400-NFF-0DPHI22',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ-400-NFF-0DPHI20', 'L1_MJJ-400-NFF-0DPHI20',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j70_j50_0eta490_invm900j50_dphi20_deta40_L1MJJ-400-NFF-0DPHI26', 'L1_MJJ-400-NFF-0DPHI26', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j70_j50_0eta490_invm900j50_dphi20_deta40_L1MJJ-400-NFF-0DPHI24', 'L1_MJJ-400-NFF-0DPHI24', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j70_j50_0eta490_invm900j50_dphi20_deta40_L1MJJ-400-NFF-0DPHI22', 'L1_MJJ-400-NFF-0DPHI22', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j70_j50_0eta490_invm900j50_dphi20_deta40_L1MJJ-400-NFF-0DPHI20', 'L1_MJJ-400-NFF-0DPHI20', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
#        ['j70_j50_0eta490_invm900j50_dphi20_deta40_L1MJJ-400-NFF', 'L1_MJJ-400-NFF', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1], #ATR-19377
        ['j70_j50_0eta490_invm1000j50_dphi20_deta40_L1MJJ-400-NFF-0DPHI26', 'L1_MJJ-400-NFF-0DPHI26', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j70_j50_0eta490_invm1100j50_dphi20_deta40_L1MJJ-400-NFF-0DPHI26', 'L1_MJJ-400-NFF-0DPHI26', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],

#        ['j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ-400-0DPHI26', 'L1_MJJ-400-0DPHI26',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
#        ['j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ-400-0DPHI24', 'L1_MJJ-400-0DPHI24',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
#        ['j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ-400-0DPHI22', 'L1_MJJ-400-0DPHI22',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
#        ['j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ-400-0DPHI20', 'L1_MJJ-400-0DPHI20',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j70_j50_0eta490_invm1000j70_dphi20_deta45_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j70_j50_0eta490_invm1000j70_dphi20_deta40_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j70_j50_0eta490_invm1100j50_dphi20_deta40_L1MJJ-500-NFF', 'L1_MJJ-500-NFF',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j70_j50_0eta490_invm1000j50_dphi20_deta40_L1MJJ-500-NFF', 'L1_MJJ-500-NFF',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['j70_j50_0eta490_invm900j50_dphi20_deta40_L1MJJ-500-NFF', 'L1_MJJ-500-NFF',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],

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
#        ['ht700_L1HT150-J20.ETA31',     'L1_HT150-J20.ETA31', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['ht700_L1HT150-J20s5.ETA31',   'L1_HT150-J20s5.ETA31', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
#        ['ht1000_L1HT190-J15.ETA21',    'L1_HT190-J15.ETA21', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['ht1000_L1HT190-J15s5.ETA21',  'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],

        ['2j45',   'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['4j15',   'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['4j45',   'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        # ATR-19554
        ['j0_perf_L1jJ100',   'L1_jJ100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j0_perf_L14jJ15',   'L1_4jJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j260_L1jJ100',      'L1_jJ100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['6j45_L14jJ15',   'L1_4jJ15', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],

        #ATR-19277
        ['j0_gsc0_boffperf_split_L1RD0_FILLED', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

        #ATR-17320 for AFP
        ['2j100_L1CEP-CJ60',   'L1_CEP-CJ60', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['2j100_L1CEP-CJ50',   'L1_CEP-CJ50', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],

        #ATR-16023
        ['j0_perf_sktc_em_nojcalib_L1RD0_FILLED',   'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j0_perf_tc_em_nojcalib_L1RD0_FILLED',   'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j0_perf_tc_em_sub_L1RD0_FILLED',   'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j0_perf_sktc_lcw_nojcalib_L1RD0_FILLED',   'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j0_perf_tc_lcw_nojcalib_L1RD0_FILLED',   'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j0_perf_tc_lcw_sub_L1RD0_FILLED',   'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        # Test of ftf on cut-down Run 3 HH item
        ['j55_j45_j28_j20_AND_j20_ftf', 'L1_J45.0ETA20_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1, ['serial', -1,["j55","j45","j28","j20_AND","j20_ftf"]]],
        ['2j35_AND_j35_ftf', 'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1, ['serial', -1,["2j35_AND","j35_ftf"]]],
        ['4j35_AND_j35_ftf', 'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1, ['serial', -1,["4j35_AND","j35_ftf"]]],

        # ATR-20408: gFEX
        ['j460_a10t_lcw_jes_L1gLJ100',   'L1_gLJ100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10r_L1gLJ100',   'L1_gLJ100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10_lcw_subjes_L1gLJ100',   'L1_gLJ100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        ['j460_a10t_lcw_jes_L1gLJ120',   'L1_gLJ120', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10r_L1gLJ120',   'L1_gLJ120', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10_lcw_subjes_L1gLJ120',   'L1_gLJ120', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        ['j460_a10t_lcw_jes_L1gLJ140',   'L1_gLJ140', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10r_L1gLJ140',   'L1_gLJ140', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10_lcw_subjes_L1gLJ140',   'L1_gLJ140', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        ['j460_a10t_lcw_jes_L1gLJ160',   'L1_gLJ160', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10r_L1gLJ160',   'L1_gLJ160', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10_lcw_subjes_L1gLJ160',   'L1_gLJ160', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        ['j460_a10t_lcw_jes_L1gLJ180',   'L1_gLJ180', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10r_L1gLJ180',   'L1_gLJ180', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10_lcw_subjes_L1gLJ180',   'L1_gLJ180', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        ['j460_a10t_lcw_jes_L1jLJ100',   'L1_jLJ100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10r_L1jLJ100',   'L1_jLJ100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10_lcw_subjes_L1jLJ100',   'L1_jLJ100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        ['j460_a10t_lcw_jes_L1jLJ120',   'L1_jLJ120', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10r_L1jLJ120',   'L1_jLJ120', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10_lcw_subjes_L1jLJ120',   'L1_jLJ120', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        ['j460_a10t_lcw_jes_L1jLJ140',   'L1_jLJ140', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10r_L1jLJ140',   'L1_jLJ140', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10_lcw_subjes_L1jLJ140',   'L1_jLJ140', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        ['j460_a10t_lcw_jes_L1jLJ160',   'L1_jLJ160', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10r_L1jLJ160',   'L1_jLJ160', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10_lcw_subjes_L1jLJ160',   'L1_jLJ160', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        ['j460_a10t_lcw_jes_L1jLJ180',   'L1_jLJ180', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10r_L1jLJ180',   'L1_jLJ180', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10_lcw_subjes_L1jLJ180',   'L1_jLJ180', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        
        # ATR-20573
        ['2j110_gsc150_boffperf_split_2j60_gsc85_boffperf_split',   'L1_3J50', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['3j60_gsc85_boffperf_split_0eta240_2j45_gsc55_boffperf_split_0eta240_L14J15',   'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['3j85_gsc115_boffperf_split_2j50_gsc70_boffperf_split_L14J15',   'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['3j50_gsc70_boffperf_split_0eta240_3j35_gsc45_boffperf_split_0eta240_L14J15',   'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['3j60_gsc85_boffperf_split_3j45_gsc55_boffperf_split_L14J15',   'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['4j50_gsc70_boffperf_split_3j35_L14J15',   'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['6j45_gsc55_boffperf_split_L14J15',   'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
			 ]

    TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [
        ['j35_boffperf_split_L1J15',  'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j150_boffperf_split_L1J40', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_boffperf_split_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_bmv2c1050_split_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j150_boffperf_split_j50_boffperf_split_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j150_bmv2c1050_split_j50_bmv2c1050_split_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],


        # ATR-19309
        #2b50 x4 L1 seeds
        ['j55_gsc80_boffperf_j45_gsc55_boffperf_j28_j20_AND_j28_b40_j20_b40_L1V1', 'L1_J45.0ETA20_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1, ['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf","j28","j20_AND","j28_b40","j20_b40_L1V1"]]],
        ['j55_gsc80_boffperf_j45_gsc55_boffperf_j28_j20_AND_j28_b50_j20_b50_L1V1', 'L1_J45.0ETA20_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1, ['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf","j28","j20_AND","j28_b50","j20_b50_L1V1"]]],
        ['j55_gsc80_boffperf_j45_gsc55_boffperf_j28_j20_AND_j28_b50_j20_b50_L1V2', 'L1_J50_2J40.0ETA25_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf","j28","j20_AND","j28_b50","j20_b50_L1V2"]]],
        ['j55_gsc80_boffperf_j45_gsc55_boffperf_j28_j20_AND_j28_b50_j20_b50_L1V3', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf","j28","j20_AND","j28_b50","j20_b50_L1V3"]]],
        ['j55_gsc80_boffperf_j45_gsc55_boffperf_j28_j20_AND_j28_b50_j20_b50_L1V4', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf","j28","j20_AND","j28_b50","j20_b50_L1V4"]]],
##### Non-FTK version #2b60 x4 L1 seeds
        ['j55_gsc80_boffperf_j45_gsc55_boffperf_j28_j20_AND_j28_b60_j20_b60_L1V1', 'L1_J45.0ETA20_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1, ['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf","j28","j20_AND","j28_b60","j20_b60_L1V1"]]],
        ['j55_gsc80_boffperf_j45_gsc55_boffperf_j28_j20_AND_j28_b60_j20_b60_L1V2', 'L1_J50_2J40.0ETA25_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf","j28","j20_AND","j28_b60","j20_b60_L1V2"]]],
        ['j55_gsc80_boffperf_j45_gsc55_boffperf_j28_j20_AND_j28_b60_j20_b60_L1V3', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf","j28","j20_AND","j28_b60","j20_b60_L1V3"]]],
        ['j55_gsc80_boffperf_j45_gsc55_boffperf_j28_j20_AND_j28_b60_j20_b60_L1V4', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf","j28","j20_AND","j28_b60","j20_b60_L1V4"]]],
        #3b70 x4 L1 seeds
        ['j55_gsc80_boffperf_j45_gsc55_boffperf_j28_j20_AND_j45_gsc55_b70_j28_b70_j20_b70_L1V2', 'L1_J50_2J40.0ETA25_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf","j28","j20_AND","j45_gsc55_b70","j28_b70","j20_b70_L1V2"]]],
        ['j55_gsc80_boffperf_j45_gsc55_boffperf_j28_j20_AND_j45_gsc55_b70_j28_b70_j20_b70_L1V3', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf","j28","j20_AND","j45_gsc55_b70","j28_b70","j20_b70_L1V3"]]],
        ['j55_gsc80_boffperf_j45_gsc55_boffperf_j28_j20_AND_j45_gsc55_b70_j28_b70_j20_b70_L1V4', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf","j45_gsc55_boffperf","j28","j20_AND","j45_gsc55_b70","j28_b70","j20_b70_L1V4"]]],
        #2b60+4b85 x2 L1 seeds
        ['j55_gsc80_b85_j45_gsc55_b85_j28_b85_j20_b85_AND_j28_b60_j20_b60_L1V1', 'L1_J45.0ETA20_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b85","j45_gsc55_b85","j28_b85","j20_b85_AND","j28_b60","j20_b60_L1V1"]]],
        ['j55_gsc80_b85_j45_gsc55_b85_j28_b85_j20_b85_AND_j28_b60_j20_b60_L1V2', 'L1_J50_2J40.0ETA25_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b85","j45_gsc55_b85","j28_b85","j20_b85_AND","j28_b60","j20_b60_L1V2"]]],
        #2b70+2b85 x4 L1 seeds
        ['j55_gsc80_b85_j45_gsc55_b85_j28_b85_j20_b85_AND_j28_b70_j20_b70_L1V1', 'L1_J45.0ETA20_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b85","j45_gsc55_b85","j28_b85","j20_b85_AND","j28_b70","j20_b70_L1V1"]]],
        ['j55_gsc80_b85_j45_gsc55_b85_j28_b85_j20_b85_AND_j28_b70_j20_b70_L1V2', 'L1_J50_2J40.0ETA25_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b85","j45_gsc55_b85","j28_b85","j20_b85_AND","j28_b70","j20_b70_L1V2"]]],
        ['j55_gsc80_b85_j45_gsc55_b85_j28_b85_j20_b85_AND_j28_b70_j20_b70_L1V3', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b85","j45_gsc55_b85","j28_b85","j20_b85_AND","j28_b70","j20_b70_L1V3"]]],
        ['j55_gsc80_b85_j45_gsc55_b85_j28_b85_j20_b85_AND_j28_b70_j20_b70_L1V4', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b85","j45_gsc55_b85","j28_b85","j20_b85_AND","j28_b70","j20_b70_L1V4"]]],
        #4b77 x4 L1 seeds
        ['j55_gsc80_b77_j45_gsc55_b77_j28_b77_j20_b77_L1V2', 'L1_J50_2J40.0ETA25_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j55_gsc80_b77_j45_gsc55_b77_j28_b77_j20_b77_L1V3', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j55_gsc80_b77_j45_gsc55_b77_j28_b77_j20_b77_L1V4', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

        # ATR-20573
        ['2j55_gsc75_bmv2c1077_split_j45_gsc55_bmv2c1077_split_L13J35.0ETA23', 'L1_3J35.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
	['j60_gsc85_bmv2c1077_split_5j35_gsc45_boffperf_split_0eta240_L14J15',   'L1_4J15', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ]

    TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + [

        #ATR-19270
        ['xe100_trktc_lcw_L1XE50', 'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe110_trktc_lcw_L1XE50', 'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_trktc_lcw_L1XE50', 'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_trktc_lcw', '', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe110_trktc_lcw_muonor_L1XE50', 'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_trktc_lcw_muonor_L1XE50', 'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_trktc_lcw_muonor', '', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        #Different trktc configurations:
        ['xe0noL1_l2fsperf_sktc_lcw', '', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_sktc_lcw_cvf', '', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_tc_lcw_cvf', '', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_tc_lcw_cvf_tst', '', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_tc_lcw_cvf_tstmuonor', '', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe90_pufittrack_lcw_nojcalib_xe105_pufit_xe65_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe105_pufit_xe65_L1XE50','xe90_pufittrack_lcw_nojcalib']]],
        ['xe0noL1_l2fsperf_pufittrack_lcw_nojcalib', '', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe110_trkmht_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_trkmht_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_trkmht', '', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_trkmht_LRT', '', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_trkmht_muonor_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_trkmht_muonor', '', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        # ATR-19554
        ['xe110_pufit_xe65_L1gXERHO50', 'L1_gXERHO50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit', 'xe65_L1gXERHO50']]],
        ['xe110_pufit_xe65_L1gXEPUFIT50', 'L1_gXEPUFIT50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit', 'xe65_L1gXEPUFIT50']]],
        ['xe110_pufit_xe65_L1gXE50', 'L1_gXE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit', 'xe65_L1gXE50']]],
        ['xe110_pufit_xe65_L1jXE50', 'L1_jXE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit', 'xe65_L1jXE50']]],


        ['xe110_trktc_lcw_xe70_L1XE50', 'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1, ['serial',-1,['xe70_L1XE50', 'xe110_trktc_lcw']]],
        ['xe110_trktc_lcw_muonor_xe70_L1XE50', 'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1, ['serial',-1,['xe70_L1XE50', 'xe110_trktc_lcw_muonor']]],
        ['xe110_sktc_lcw_xe70_L1XE50', 'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1, ['serial',-1,['xe70_L1XE50', 'xe110_sktc_lcw']]],
        ['xe110_sktc_lcw_cvf_xe70_L1XE50', 'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1, ['serial',-1,['xe70_L1XE50', 'xe110_sktc_lcw_cvf']]],
        ['xe110_tc_lcw_cvf_xe70_L1XE50', 'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1, ['serial',-1,['xe70_L1XE50', 'xe110_tc_lcw_cvf']]],
        ['xe110_tc_lcw_cvf_tst_xe70_L1XE50', 'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1, ['serial',-1,['xe70_L1XE50', 'xe110_tc_lcw_cvf_tst']]],
        ['xe110_tc_lcw_cvf_muonor_xe70_L1XE50', 'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1, ['serial',-1,['xe70_L1XE50', 'xe110_tc_lcw_cvf_muonor']]],



        ['xe110_trktc_lcw_xe50_L1XE50', 'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1, ['serial',-1,['xe50_L1XE50', 'xe110_trktc_lcw']]],
        ['xe120_trkmht_xe50_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1, ['serial',-1,['xe50_L1XE50', 'xe120_trkmht']]],

        ['xe80_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe80_pufit'] ]],
        ['xe85_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe85_pufit'] ]],
        ['xe90_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe90_pufit'] ]],
        ['xe95_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe95_pufit'] ]],
        ['xe100_pufit_xe65_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe100_pufit'] ]],
        ['xe105_pufit_xe65_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe105_pufit'] ]],

        ['xe95_trkmht_xe80_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe80_pufit','xe95_trkmht'] ]],
        ['xe100_trkmht_xe80_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe80_pufit','xe100_trkmht'] ]],
        ['xe105_trkmht_xe80_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe80_pufit','xe105_trkmht'] ]],
        ['xe110_trkmht_xe80_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe80_pufit','xe110_trkmht'] ]],
        ['xe115_trkmht_xe80_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe80_pufit','xe115_trkmht'] ]],
        ['xe120_trkmht_xe80_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe80_pufit','xe120_trkmht'] ]],
        ['xe100_trkmht_xe85_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe85_pufit','xe100_trkmht'] ]],
        ['xe100_trkmht_muonor_xe85_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe85_pufit','xe100_trkmht_muonor'] ]],
        ['xe105_trkmht_xe85_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe85_pufit','xe105_trkmht'] ]],
        ['xe110_trkmht_xe85_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe85_pufit','xe110_trkmht'] ]],
        ['xe115_trkmht_xe85_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe85_pufit','xe115_trkmht'] ]],
        ['xe120_trkmht_xe85_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe85_pufit','xe120_trkmht'] ]],
        # cellPufit
        ['xe0noL1_l2fsperf_cellpufit', '', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe100_cellpufit_L1XE50', 'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe105_trktc_lcw_L1XE50', 'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe125_trkmht_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe130_trkmht_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe135_trkmht_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
]

    TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [
        # performance chains with BDT to select best tauCore FTF track (MR 27181)
        ['tau25_idperf_tracktwoMVABDT',           'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_tracktwoMVABDT',             'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_mediumRNN_tracktwoMVABDT',        'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau20_mediumRNN_tracktwoMVABDT_L1TAU8', 'L1_TAU8',    [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],

        # check RNN track classification performance
        ['tau25_idperf_tracktwoRNN',                      'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1], 
        ['tau25_perf_tracktwoRNN',                        'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1], 
        ['tau25_mediumRNN_tracktwoRNN',                   'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1], 
        ['tau35_mediumRNN_tracktwoRNN',                   'L1_TAU20IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau60_mediumRNN_tracktwoRNN',                   'L1_TAU40',   [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_mediumRNN_tracktwoRNN_L1TAU100',         'L1_TAU100',  [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau20_mediumRNN_tracktwoRNN_L1TAU8',            'L1_TAU8',    [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],

        # ATR-19359
        ['tau20_mediumRNN_tracktwoMVA_L1TAU5',                   'L1_TAU5', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau20_mediumRNN_tracktwoMVA_L1TAU8',                   'L1_TAU8', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_mediumRNN_tracktwoMVA_L1TAU5',                   'L1_TAU5', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_mediumRNN_tracktwoMVA_L1TAU8',                   'L1_TAU8', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],

        #ATR-19269
        ['tau0_perf_ptonly_L1TAU12',               'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau0_perf_ptonly_L1TAU12IM', 'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau0_perf_ptonly_L1TAU8', 'L1_TAU8', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        # ATR-19554
        ['tau0_idperf_tracktwoMVA_L1eTAU12', 'L1_eTAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        #['tau0_idperf_tracktwoMVA_L1eTAU5', 'L1_eTAU5', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],

        ['tau160_mediumRNN_tracktwoMVA_L1TAU90',      'L1_TAU90', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_mediumRNN_tracktwoMVA_L1J100',       'L1_J100',  [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],

        # MVA TES (ATR-17687)
        ['tau25_idperf_tracktwoEFmvaTES',                'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_tracktwoEFmvaTES',                  'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1NoPt_tracktwoEFmvaTES',           'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_tracktwoEFmvaTES',               'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
			 ]

    TriggerFlags.EgammaSlice.signatures = TriggerFlags.EgammaSlice.signatures() + [

        ['e26_lhtight_nod0_ivarloose_L1EM20VH',        'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhtight_nod0_ivarloose_L1EM20VH',        'L1_EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e22_lhtight_nod0_ivarloose_L1EM15VH',        'L1_EM15VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        #ATR-19268
        ['g20_tight_icaloloose',             'L1_EM18VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['e10_lhmedium_nod0_ivarloose', 'L1_EM8VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g27_medium_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g45_loose_L1EM15VH', 'L1_EM15VH', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['e17_lhmedium_nod0_L1EM15VHI', 'L1_EM15VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_ivarloose', 'L1_EM20VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        # ATR-19554
        ['e26_lhtight_nod0_ivarloose_L1eEM22', 'L1_eEM22', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhvloose_nod0_L1eEM20', 'L1_eEM20', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g25_medium_L1eEM20', 'L1_eEM20', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['e3_etcut_L1eEM3', 'L1_eEM3', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g3_etcut_L1eEM3', 'L1_eEM3', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['g35_medium_g25_medium_L12eEM10', 'L1_2eEM10', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1, ['serial',-1,['g35_medium','g25_medium_L12eEM10']]],
        ['g35_medium_L1eEM10', 'L1_eEM10', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g25_medium_L1eEM10', 'L1_eEM10', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['2g20_tight_icalovloose_L12eEM8', 'L1_2eEM8', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_tight_icalovloose_L1eEM8', 'L1_eEM8', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['e26_lhtight_nod0_ivarloose_L1eEM15', 'L1_eEM15', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['2e17_lhvloose_nod0_L12eEM8', 'L1_2eEM8', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhvloose_nod0_L1eEM8', 'L1_eEM8', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['2g10_loose_L12eEM8', 'L1_2eEM8', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g10_loose_L1eEM8', 'L1_eEM8', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['2e10_lhvloose_nod0_L12eEM8', 'L1_2eEM8', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e10_lhvloose_nod0_L1eEM8', 'L1_eEM8', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],



        # ATR-19302

#        ['2g12_tight_dPhi15_L1DPHI-M70-2EM10I',            'L1_DPHI-M70-2EM10I', ['L1_2EM10VH'], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],
#        ['2g12_tight_icalovloose_L1DPHI-M70-2EM10I',       'L1_DPHI-M70-2EM10I', ['L1_2EM10VH'], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],
#        ['2g12_tight_icalotight_L1DPHI-M70-2EM10I',        'L1_DPHI-M70-2EM10I', ['L1_2EM10VH'], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],
#        ['2g12_tight_icalotight_dPhi15_L1DPHI-M70-2EM10I', 'L1_DPHI-M70-2EM10I', ['L1_2EM10VH'], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],

        ['2g12_tight_dPhi15_L1DPHI-M70-2EM12I',            'L1_DPHI-M70-2EM12I', ['L1_2EM10VH'], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g12_tight_icalovloose_L1DPHI-M70-2EM12I',       'L1_DPHI-M70-2EM12I', ['L1_2EM10VH'], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g12_tight_icalotight_L1DPHI-M70-2EM12I',        'L1_DPHI-M70-2EM12I', ['L1_2EM10VH'], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g12_tight_icalotight_dPhi15_L1DPHI-M70-2EM12I', 'L1_DPHI-M70-2EM12I', ['L1_2EM10VH'], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],

#        ['2g15_tight_dPhi15_L1DPHI-M70-2EM10I',            'L1_DPHI-M70-2EM10I', ['L1_2EM10VH'], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],
#        ['2g15_tight_icalovloose_L1DPHI-M70-2EM10I',       'L1_DPHI-M70-2EM10I', ['L1_2EM10VH'], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],
#        ['2g15_tight_icalotight_L1DPHI-M70-2EM10I',        'L1_DPHI-M70-2EM10I', ['L1_2EM10VH'], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],
#        ['2g15_tight_icalotight_dPhi15_L1DPHI-M70-2EM10I', 'L1_DPHI-M70-2EM10I', ['L1_2EM10VH'], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],

        ['2g15_tight_icalovloose_L1DPHI-M70-2EM12I',       'L1_DPHI-M70-2EM12I', ['L1_2EM10VH'], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g15_tight_icalotight_L1DPHI-M70-2EM12I',        'L1_DPHI-M70-2EM12I', ['L1_2EM10VH'], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g15_tight_icalotight_dPhi15_L1DPHI-M70-2EM12I', 'L1_DPHI-M70-2EM12I', ['L1_2EM10VH'], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],

        #ATR-18824
        ['e17_lhvloose_nod0_L1ZAFB-04DPHI-EM15I',       'L1_ZAFB-04DPHI-EM15I', ['L1_EM15VHI'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhvloose_nod0_L1ZAFB-25DPHI-EM15I',       'L1_ZAFB-25DPHI-EM15I', ['L1_EM15VHI'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e20_lhvloose_nod0_L1ZAFB-25DPHI-EM18I',       'L1_ZAFB-25DPHI-EM18I', ['L1_EM18VHI'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhloose_nod0_L1ZAFB-04DPHI-EM15I',        'L1_ZAFB-04DPHI-EM15I', ['L1_EM15VHI'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhloose_nod0_L1ZAFB-25DPHI-EM15I',        'L1_ZAFB-25DPHI-EM15I', ['L1_EM15VHI'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e20_lhloose_nod0_L1ZAFB-25DPHI-EM18I',        'L1_ZAFB-25DPHI-EM18I', ['L1_EM18VHI'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhmedium_nod0_L1ZAFB-04DPHI-EM15I',       'L1_ZAFB-04DPHI-EM15I', ['L1_EM15VHI'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhmedium_nod0_L1ZAFB-25DPHI-EM15I',       'L1_ZAFB-25DPHI-EM15I', ['L1_EM15VHI'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e20_lhmedium_nod0_L1ZAFB-25DPHI-EM18I',       'L1_ZAFB-25DPHI-EM18I', ['L1_EM18VHI'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhtight_nod0_L1ZAFB-04DPHI-EM15I',        'L1_ZAFB-04DPHI-EM15I', ['L1_EM15VHI'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhtight_nod0_L1ZAFB-25DPHI-EM15I',        'L1_ZAFB-25DPHI-EM15I', ['L1_EM15VHI'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e20_lhtight_nod0_L1ZAFB-25DPHI-EM18I',        'L1_ZAFB-25DPHI-EM18I', ['L1_EM18VHI'], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        # ATR-20573
        ['e26_lhvloose_nod0_e22_lhvloose_nod0', 'L1_2EM20VH', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g22_tight_icalovloose_g17_tight_icalovloose_L12EM15VHI', 'L1_2EM15VHI', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['g24_tight_g20_tight_L12EM15VHI', 'L1_2EM15VHI', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['g27_tight_g22_tight_L12EM20VH', 'L1_2EM20VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['g60_loose_g40_loose_L12EM20VH', 'L1_2EM20VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],

        ]

    TriggerFlags.BphysicsSlice.signatures = TriggerFlags.BphysicsSlice.signatures() + [

        #ATR-19452
        ['2mu4_bJpsimumu_Lxy0_L1BPH-2M9-0DR15-2MU4_muonqualL', 'L1_BPH-2M9-0DR15-2MU4', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bJpsimumu_Lxy0_L1BPH-2M9-0DR15-2MU4_muonqualM', 'L1_BPH-2M9-0DR15-2MU4', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_Lxy0_L1BPH-2M9-0DR15-2MU4_muonqualL',    'L1_BPH-2M9-0DR15-2MU4', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumu_Lxy0_L1BPH-2M9-0DR15-2MU4_muonqualM',    'L1_BPH-2M9-0DR15-2MU4', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BsmumuPhi_L1BPH-2M9-0DR15-2MU4_muonqualL', 'L1_BPH-2M9-0DR15-2MU4', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_bBmumux_BsmumuPhi_L1BPH-2M9-0DR15-2MU4_muonqualM', 'L1_BPH-2M9-0DR15-2MU4', ['L1_2MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],

        #ATR-19510
        ['mu4_ivarloose_mu4_7invm9_noos_L1DY-BOX-2MU4',     'L1_DY-BOX-2MU4', ['L1_MU4','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['mu4_ivarloose_mu4_7invm9_noos_novtx_L1DY-BOX-2MU4',     'L1_DY-BOX-2MU4', ['L1_MU4','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['mu4_ivarloose_mu4_11invm60_noos_L1DY-BOX-2MU4',   'L1_DY-BOX-2MU4', ['L1_MU4','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['mu4_ivarloose_mu4_11invm60_noos_novtx_L1DY-BOX-2MU4',   'L1_DY-BOX-2MU4', ['L1_MU4','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['mu6_ivarloose_mu6_11invm24_noos_L1DY-BOX-2MU6',    'L1_DY-BOX-2MU6', ['L1_MU6','L1_MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['mu6_ivarloose_mu6_11invm24_noos_novtx_L1DY-BOX-2MU6',  'L1_DY-BOX-2MU6', ['L1_MU6','L1_MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['mu6_ivarloose_mu6_24invm60_noos_L1DY-BOX-2MU6', 'L1_DY-BOX-2MU6', ['L1_MU6','L1_MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['mu6_ivarloose_mu6_24invm60_noos_novtx_L1DY-BOX-2MU6', 'L1_DY-BOX-2MU6', ['L1_MU6','L1_MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],

        ['mu4_ivarloose_mu4_7invm9_noos_L12MU4', 'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu4_ivarloose_mu4_7invm9_noos_novtx_L12MU4', 'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu4_ivarloose_mu4_11invm60_noos_L12MU4', 'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu4_ivarloose_mu4_11invm60_noos_novtx_L12MU4', 'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_ivarloose_mu6_11invm24_noos_L12MU6', 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_ivarloose_mu6_11invm24_noos_novtx_L12MU6', 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_ivarloose_mu6_24invm60_noos_L12MU6', 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],
        ['mu6_ivarloose_mu6_24invm60_noos_novtx_L12MU6', 'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],

        ['2mu4_7invm9_noos_novtx_L1DY-BOX-2MU4', 'L1_DY-BOX-2MU4', ['L1_MU4','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu4_11invm60_noos_novtx_L1DY-BOX-2MU4', 'L1_DY-BOX-2MU4', ['L1_MU4','L1_MU4'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_11invm24_noos_novtx_L1DY-BOX-2MU6', 'L1_DY-BOX-2MU6', ['L1_MU6','L1_MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_24invm60_noos_novtx_L1DY-BOX-2MU6', 'L1_DY-BOX-2MU6', ['L1_MU6','L1_MU6'], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1, False],

        ['3mu4_bTau_L1BPH-0M10-3MU4',               'L1_BPH-0M10-3MU4', ['L1_3MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1], #ATR-19355

        ['2mu4_bUpsimumu_L1BPH-8M15-2MU4-BO',       'L1_BPH-8M15-2MU4-BO', ['L1_MU4','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1], #ATR-20174
        ]

    TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures() + [
        ['e5_lhvloose_nod0_j70_j50_0eta490_invm1000j50_dphi28_xe50_pufit_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_EM3', '', '',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["e5_lhvloose_nod0","j70","j50_0eta490","xe50_pufit"]]],
        ['e5_lhvloose_nod0_j70_j50_0eta490_invm1000j50_xe50_pufit_L1MJJ-700', 'L1_MJJ-700', ['L1_EM3', '', '',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["e5_lhvloose_nod0","j70","j50_0eta490","xe50_pufit"]]],
        ['e5_lhvloose_nod0_j70_j50_0eta490_invm1000j50_dphi28_xe50_pufit_L1MJJ-700', 'L1_MJJ-700', ['L1_EM3', '', '',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["e5_lhvloose_nod0","j70","j50_0eta490","xe50_pufit"]]],
        # ATR-19359
        # e+tau
        ['e24_lhmedium_nod0_ivarloose_L1EM22VHI_tau20_mediumRNN_tracktwoMVA_L1TAU5',  'L1_EM22VHI', ['L1_EM22VHI', 'L1_TAU5'], [PhysicsStream], ['RATE:ElectronTau', 'BW:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e24_lhmedium_nod0_ivarloose_L1EM22VHI","tau20_mediumRNN_tracktwoMVA_L1TAU5"]]],
        ['e24_lhmedium_nod0_ivarloose_L1EM22VHI_tau20_mediumRNN_tracktwoMVA_L1TAU8',    'L1_EM22VHI', ['L1_EM22VHI', 'L1_TAU8'], [PhysicsStream], ['RATE:ElectronTau', 'BW:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e24_lhmedium_nod0_ivarloose_L1EM22VHI","tau20_mediumRNN_tracktwoMVA_L1TAU8"]]],
        ['e24_lhmedium_nod0_ivarloose_L1EM22VHI_tau20_mediumRNN_tracktwoMVA_L1TAU5_03dRtt',  'L1_EM22VHI', ['L1_EM22VHI', 'L1_TAU5'], [PhysicsStream], ['RATE:ElectronTau', 'BW:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e24_lhmedium_nod0_ivarloose_L1EM22VHI","tau20_mediumRNN_tracktwoMVA_L1TAU5"]]],
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
        ['mu20_ivarmedium_tau20_mediumRNN_tracktwoMVA_L1TAU5_03dRtt',  'L1_MU20MU21', ['L1_MU20', 'L1_TAU5'], [PhysicsStream], ['RATE:MuonTau', 'BW:MuonTau','BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu20_ivarmedium","tau20_mediumRNN_tracktwoMVA_L1TAU5"]]],
        ['mu20_ivarmedium_tau20_mediumRNN_tracktwoMVA_L1TAU8_03dRtt',  'L1_MU20MU21', ['L1_MU20', 'L1_TAU8'], [PhysicsStream], ['RATE:MuonTau', 'BW:MuonTau','BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu20_ivarmedium","tau20_mediumRNN_tracktwoMVA_L1TAU8"]]],
        # VBF ditau
        ['tau25_mRNN_t2MVA_L1TAU5_tau20_mRNN_t2MVA_L1TAU5_j70_j50_0eta490_invm900j50', 'L1_MJJ-500-NFF', ['L1_TAU5', 'L1_TAU5','',''], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau', 'BW:Jet'], -1,['serial',-1,["tau25_mRNN_t2MVA_L1TAU5", "tau20_mRNN_t2MVA_L1TAU5","j70","j50_0eta490_invm900j50"]]],
        ['tau25_mRNN_t2MVA_L1TAU5_tau20_mRNN_t2MVA_L1TAU5_j70_j50_0eta490_invm900j50_03dRtt', 'L1_MJJ-500-NFF', ['L1_TAU5', 'L1_TAU5','',''], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau', 'BW:Jet'], -1,['serial',-1,["tau25_mRNN_t2MVA_L1TAU5", "tau20_mRNN_t2MVA_L1TAU5","j70","j50_0eta490_invm900j50"]]],
        ['tau25_mRNN_t2MVA_L1TAU8_tau20_mRNN_t2MVA_L1TAU8_j70_j50_0eta490_invm900j50', 'L1_MJJ-500-NFF', ['L1_TAU8', 'L1_TAU8','',''], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau', 'BW:Jet'], -1,['serial',-1,["tau25_mRNN_t2MVA_L1TAU8", "tau20_mRNN_t2MVA_L1TAU8","j70","j50_0eta490_invm900j50"]]],
        #mu+jet
        ['mu26_ivarmedium_j110_gsc150_boffperf_split_j35_boffperf_split_L1MU20_J50', 'L1_MU20_J50',  ['L1_MU20', '', ''], [PhysicsStream], ['RATE:MuonBJet', 'BW:BJet_combined'], -1, ['serial',-1,['mu26_ivarmedium', 'j110_gsc150_boffperf_split', 'j35_boffperf_split'] ] ],
        ['mu14_ivarloose_3j20_L1MU10_3J20', 'L1_MU10_3J20', ['L1_MU10_3J20', ''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon','BW:Jet'], -1,['serial',-1,['mu14_ivarloose', '3j20'] ],True],
        #e+jets
        ['e20_lhtight_nod0_ivarloose_3j20_L1EM18VHI_3J20', 'L1_EM18VHI_3J20',    ['L1_EM18VHI',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1, ['serial',-1,["e20_lhtight_nod0_ivarloose","3j20"] ]],
        ['e60_etcut_trkcut_L1EM24VHIM_xs30_j15_perf_xe30_6dphi15_mt35', 'L1_EM24VHIM', ['L1_EM24VHIM','','',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e60_etcut_trkcut_L1EM24VHIM","xs30","j15_perf","xe30"]]],
        #gamma+jets
        # ATR-20573
        ['g45_loose_3j55_0eta240_3j35_0eta240', 'L1_EM15VH',    ['L1_EM15VH', '', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1, ['serial',-1,["g45_loose", "3j55_0eta240", "3j35_0eta240"] ]],
        ['g25_medium_L1EM22VHI_2j45_0eta490_2j25_0eta490_invm1000', 'L1_EM22VHI',    ['L1_EM22VHI','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1, ['serial',-1,["g25_medium_L1EM22VHI", "2j45_0eta490", "2j25_0eta490_invm1000"] ]],

        #ATR-19317
        ['g35_loose_L1EM30VHI_j0_perf',  'L1_EM30VHI', ['L1_EM30VHI',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g35_loose_L1EM30VHI","j0_perf"]]],
        ['g35_loose_L1EM30VHI_j0_perf_gsc0_boffperf_split',  'L1_EM30VHI', ['L1_EM30VHI',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g35_loose_L1EM30VHI","j0_perf_gsc0_boffperf_split"]]],
        ['g35_loose_L1EM30VHI_3j25',  'L1_EM30VHI', ['L1_EM30VHI',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g35_loose_L1EM30VHI","3j25"]]],
        ['g35_loose_L1EM30VHI_3j15_gsc25_boffperf_split',  'L1_EM30VHI', ['L1_EM30VHI',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g35_loose_L1EM30VHI","3j15_gsc25_boffperf_split"]]],

        ['e9_lhvloose_nod0_mu11_nomucomb_L1LFV-EM8I-MU11',        'L1_LFV-EM8I-MU11',  ['L1_EM7', 'L1_MU11'],   [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e14_lhvloose_nod0_mu6_nomucomb_L1LFV-EM12I-MU6',        'L1_LFV-EM12I-MU6',  ['L1_EM12', 'L1_MU6'],   [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e24_lhmedium_nod0_mu8noL1', 'L1_EM20VH', ['L1_EM20VH', ''], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['e24_lhmedium_nod0','mu8noL1'] ]],

        ['tau50_mediumRNN_tracktwoMVA_xe80_pufit_xe50_L1TAU20IM_2J20_XE50', 'L1_TAU20IM_2J20_XE50',['L1_TAU20IM','',''], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau50_mediumRNN_tracktwoMVA","xe80_pufit","xe50_L1TAU20IM_2J20_XE50"]]], # it should be reordered in athenaMT
        ['g90_loose_xe90noL1',           'L1_EM22VHI',  ['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g90_loose","xe90noL1"]]],
        ['mu14_ivarloose_tau25_mediumRNN_tracktwoMVA_03dRtt',      'L1_MU10_TAU12IM_J25_2J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],
        ['mu14_ivarloose_tau25_mediumRNN_tracktwoMVA_03dRtt_L1MU10_TAU12IM_3J12',      'L1_MU10_TAU12IM_3J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],
        ['mu14_ivarloose_tau25_mediumRNN_tracktwoMVA_xe50_03dRtt', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Legacy:Backup','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose", "tau25_mediumRNN_tracktwoMVA","xe50"]]],
        ['mu14_ivarloose_tau35_mediumRNN_tracktwoMVA_03dRtt_L1MU10_TAU20IM_J25_2J20',      'L1_MU10_TAU20IM_J25_2J20',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['Legacy:Backup','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_mediumRNN_tracktwoMVA"]]],
        ['mu14_ivarloose_tau35_mediumRNN_tracktwoMVA_03dRtt',  'L1_MU10_TAU20IM',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_mediumRNN_tracktwoMVA"]]],
        ['mu14_ivarloose_L1MU11_tau35_mediumRNN_tracktwoMVA_03dRtt_L1MU11_TAU20IM',  'L1_MU11_TAU20IM',['L1_MU11','L1_TAU20IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose_L1MU11","tau35_mediumRNN_tracktwoMVA"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_mediumRNN_tracktwoMVA_03dRtt_L1DR-EM15TAU12I-J25', 'L1_DR-EM15TAU12I-J25',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'],-1,['serial',-1 ,["e17_lhmedium_nod0_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_mediumRNN_tracktwoMVA_xe50_03dRtt', 'L1_EM15VHI_2TAU12IM_XE35',['L1_EM15VHI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Legacy:Backup','RATE:TauMET', 'BW:Tau', 'BW:Egamma'],-1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_mediumRNN_tracktwoMVA", "xe50"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_mediumRNN_tracktwoMVA_03dRtt',  'L1_EM15VHI_2TAU12IM_J25_3J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream],['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_mediumRNN_tracktwoMVA_03dRtt_L1EM15VHI_2TAU12IM_4J12',   'L1_EM15VHI_2TAU12IM_4J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream],['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],
        ['e24_lhmedium_nod0_ivarloose_tau35_mediumRNN_tracktwoMVA_03dRtt',  'L1_EM20VHI_TAU20IM_2TAU20_J25_3J20',['L1_EM20VHI','L1_TAU20IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e24_lhmedium_nod0_ivarloose","tau35_mediumRNN_tracktwoMVA"]]],

        # ATR-20450
        ['tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_L1TAU20IM_2TAU12IM_4J12.0ETA25',   'L1_TAU20IM_2TAU12IM_4J12.0ETA25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_mediumRNN_tracktwoMVA","tau25_mediumRNN_tracktwoMVA"]]],
        ['tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_L1TAU20IM_2TAU12IM_4J12.0ETA28',   'L1_TAU20IM_2TAU12IM_4J12.0ETA28',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_mediumRNN_tracktwoMVA","tau25_mediumRNN_tracktwoMVA"]]],

        #ATR-20505
        ['mu50_msonly_g50_loose',           'L1_EM8VH_MU20', ['L1_MU20','L1_EM20VH'],    [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['serial',-1,['g50_loose','mu50_msonly']]],
        ['mu40_msonly_3layersEC_g40_loose',           'L1_EM8VH_MU20', ['L1_MU20','L1_EM20VH'],    [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['serial',-1,['g40_loose','mu40_msonly_3layersEC']]],

        ['mu50noL1_msonly_g50_loose',           'L1_EM22VHI', ['','L1_EM22VHI'],    [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['serial',-1,['g50_loose','mu50noL1_msonly']]],

        ]

    TriggerFlags.CalibSlice.signatures   = TriggerFlags.CalibSlice.signatures() + []

    TriggerFlags.CosmicSlice.signatures  = TriggerFlags.CosmicSlice.signatures() + []

    TriggerFlags.StreamingSlice.signatures = TriggerFlags.StreamingSlice.signatures() + [
        ['noalg_L1gXERHO50', 'L1_gXERHO50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['noalg_L1gXEPUFIT50', 'L1_gXEPUFIT50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['noalg_L1gXE50', 'L1_gXE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['noalg_L1jXE50', 'L1_jXE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['noalg_L1gLJ100',   'L1_gLJ100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['noalg_L1gLJ120',   'L1_gLJ120', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['noalg_L1gLJ140',   'L1_gLJ140', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['noalg_L1gLJ160',   'L1_gLJ160', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['noalg_L1gLJ180',   'L1_gLJ180', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        ['noalg_L1jLJ100',   'L1_jLJ100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['noalg_L1jLJ120',   'L1_jLJ120', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['noalg_L1jLJ140',   'L1_jLJ140', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['noalg_L1jLJ160',   'L1_jLJ160', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['noalg_L1jLJ180',   'L1_jLJ180', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
    ]

    TriggerFlags.EnhancedBiasSlice.signatures = TriggerFlags.EnhancedBiasSlice.signatures() + []

    if TriggerFlags.doFTK():
        TriggerFlags.BeamspotSlice.signatures = TriggerFlags.BeamspotSlice.signatures()+ [
            ['beamspot_idperf_FTK',      'L1_4J20', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
            ['beamspot_idperf_FTKRefit', 'L1_4J20', [], ["BeamSpot"], ['RATE:BeamSpot', 'BW:BeamSpot'], -1],
            ['beamspot_idperf_FTK_L1All',      '', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
            ['beamspot_idperf_FTKRefit_L1All', '', [], ["BeamSpot"], ['RATE:BeamSpot', 'BW:BeamSpot'], -1]
            ]

        TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
            ['mu24_idperf_FTK',       'L1_MU20MU21', ['L1_MU20'], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
            ['mu6_idperf_FTK',             'L1_MU6',            [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
            ['mu24_idperf_FTKRefit',  'L1_MU20MU21', ['L1_MU20'], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
            ['mu6_idperf_FTKRefit',        'L1_MU6',            [], [PhysicsStream], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
            ['mu22_mu8noL1_FTKFS',    'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','mu8noL1_FTKFS']]],
            ['mu26_ivarmedium_FTK',   'L1_MU20MU21', ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
            ['mu26noL1_FTKFS',            'L1_J100', [], [PhysicsStream],  ['RATE:MuonJet', 'BW:Muon','BW:Jet'], -1],
            ['mu28noL1_FTKFS',            'L1_J100', [], [PhysicsStream],  ['RATE:MuonJet', 'BW:Muon','BW:Jet'], -1],
            ['mu30noL1_FTKFS',            'L1_J100', [], [PhysicsStream],  ['RATE:MuonJet', 'BW:Muon','BW:Jet'], -1],
            ['mu32noL1_FTKFS',            'L1_J100', [], [PhysicsStream],  ['RATE:MuonJet', 'BW:Muon','BW:Jet'], -1],
            ]

        TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + [
            ['j0_perf_ftk_L1RD0_FILLED',   'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
            ['j0_perf_ftkrefit_L1RD0_FILLED',   'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
            ['j0_gsc0_boffperf_split_FTK_L1RD0_FILLED', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
            ['j0_gsc0_boffperf_split_FTKRefit_L1RD0_FILLED', 'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
            ['j0_ftk_jvt015et15',   'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
            ['j0_ftk_jvt015et45',   'L1_J15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
            ['j0_ftk_2jvt011et45',   'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
            ['j0_ftk_2jvt015et45',   'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
            ['j0_ftk_2jvt059et45',   'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
            ['j0_ftk_4jvt011et45',   'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
            ['j0_ftk_4jvt015et15',   'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
            ['j0_ftk_4jvt015et45',   'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
            ['j0_ftk_4jvt059et45',   'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
            ['j0_ftk_6jvt011et45',   'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
            ['j0_ftk_6jvt015et45',   'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
            ['j0_ftk_6jvt059et45',   'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],

    ##  ATR-19574 copy gsc triggers with FTK; only "legacy-primary"
            ['j225_gsc420_boffperf_split_FTKRefit',  'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
            ['4j85_gsc115_boffperf_split_FTKRefit',  'L1_3J50', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
            ['5j60_gsc85_boffperf_split_FTKRefit_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
            ['5j60_gsc85_boffperf_split_FTKRefit_L14J15',         'L1_4J15', [], [PhysicsStream], [ 'RATE:MultiJet',  'BW:Jet'], -1],
            ['5j50_gsc70_boffperf_split_FTKRefit_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
            ['5j50_gsc70_boffperf_split_FTKRefit_0eta240_L14J15', 'L1_4J15', [], [PhysicsStream], [ 'RATE:MultiJet',  'BW:Jet'], -1],
            ['6j50_gsc70_boffperf_split_FTKRefit_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
            ['6j50_gsc70_boffperf_split_FTKRefit_L14J15', 'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
            ['6j45_gsc55_boffperf_split_FTKRefit_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
            ['6j45_gsc55_boffperf_split_FTKRefit_0eta240_L14J15', 'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
            ['7j35_gsc45_boffperf_split_FTKRefit_L14J150ETA25',          'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
            ['7j35_gsc45_boffperf_split_FTKRefit_L14J15',         'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
            ['7j35_gsc45_boffperf_split_FTKRefit_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
            ['7j35_gsc45_boffperf_split_FTKRefit_0eta240_L14J15', 'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

            ['2j225_gsc250_boffperf_split_FTKRefit_0eta240_j85_gsc120_boffperf_split_FTKRefit_0eta240',      'L1_J100', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
            ['ht300_2j40_0eta490_invm700_AND_2j35_gsc45_bmv2c1070_split_FTKRefit','L1_HT150-J20s5.ETA31_MJJ-400-CF', [], [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,[  'ht300', '2j40_0eta490_invm700_AND','2j35_gsc45_bmv2c1070_split_FTKRefit']]],

            ]

        TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [
           ['j35_boffperf_split_FTK_L1J15', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j35_boffperf_split_FTKVtx_L1J15', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j35_boffperf_split_FTKRefit_L1J15', 'L1_J15', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

           ['j150_boffperf_split_FTK_L1J40', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j150_boffperf_split_FTKVtx_L1J40', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j150_boffperf_split_FTKRefit_L1J40', 'L1_J40', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

           ['j225_boffperf_split_FTK_L1J100',    'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j225_boffperf_split_FTKVtx_L1J100',    'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j225_boffperf_split_FTKRefit_L1J100',    'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

           ['j225_bmv2c1050_split_FTK_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j225_bmv2c1050_split_FTKVtx_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j225_bmv2c1050_split_FTKRefit_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

           ['j150_boffperf_split_j50_boffperf_split_FTK_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
           ['j150_boffperf_split_j50_boffperf_split_FTKVtx_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
           ['j150_boffperf_split_j50_boffperf_split_FTKRefit_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

           ['j150_bmv2c1050_split_j50_bmv2c1050_split_FTK_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
           ['j150_bmv2c1050_split_j50_bmv2c1050_split_FTKVtx_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
           ['j150_bmv2c1050_split_j50_bmv2c1050_split_FTKRefit_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

           ['2j35_boffperf_split_FTK_2j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
           ['2j35_boffperf_split_FTKVtx_2j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
           ['2j35_boffperf_split_FTKRefit_2j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

           ['2j35_bmv2c1050_split_FTK_2j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
           ['2j35_bmv2c1050_split_FTKVtx_2j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
           ['2j35_bmv2c1050_split_FTKRefit_2j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

           #ATR-19309
           ##### FTK-refit version #2b60 x4 L1 seeds
           #['j55_gsc80_boffperf_FTK_j45_gsc55_boffperf_FTK_j28_j20_AND_j28_b60_FTK_j20_b60_FTK_L1V1', 'L1_J45.0ETA20_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf_FTK_j45_gsc55_boffperf_FTK_j28_j20_AND","j28_b60_FTK_j20_b60_FTK_L1V1"]]],
           #['j55_gsc80_boffperf_FTK_j45_gsc55_boffperf_FTK_j28_j20_AND_j28_b60_FTK_j20_b60_FTK_L1V2', 'L1_J50_2J40.0ETA25_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf_FTK_j45_gsc55_boffperf_FTK_j28_j20_AND","j28_b60_FTK_j20_b60_FTK_L1V2"]]],
           #['j55_gsc80_boffperf_FTK_j45_gsc55_boffperf_FTK_j28_j20_AND_j28_b60_FTK_j20_b60_FTK_L1V3', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf_FTK_j45_gsc55_boffperf_FTK_j28_j20_AND","j28_b60_FTK_j20_b60_FTK_L1V3"]]],
           #['j55_gsc80_boffperf_FTK_j45_gsc55_boffperf_FTK_j28_j20_AND_j28_b60_FTK_j20_b60_FTK_L1V4', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf_FTK_j45_gsc55_boffperf_FTK_j28_j20_AND","j28_b60_FTK_j20_b60_FTK_L1V4"]]],
           ##3b70 x4 L1 seeds
           #['j55_gsc80_boffperf_FTK_j45_gsc55_boffperf_FTK_j28_j20_AND_j45_gsc55_b70_FTK_j28_b70_FTK_j20_b70_FTK_L1V1', 'L1_J45.0ETA20_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf_FTK_j45_gsc55_boffperf_FTK_j28_j20_AND","j45_gsc55_b70_FTK_j28_b70_FTK_j20_b70_FTK_L1V1"]]],
           #['j55_gsc80_boffperf_FTK_j45_gsc55_boffperf_FTK_j28_j20_AND_j45_gsc55_b70_FTK_j28_b70_FTK_j20_b70_FTK_L1V2', 'L1_J50_2J40.0ETA25_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf_FTK_j45_gsc55_boffperf_FTK_j28_j20_AND","j45_gsc55_b70_FTK_j28_b70_FTK_j20_b70_FTK_L1V2"]]],
           #['j55_gsc80_boffperf_FTK_j45_gsc55_boffperf_FTK_j28_j20_AND_j45_gsc55_b70_FTK_j28_b70_FTK_j20_b70_FTK_L1V3', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf_FTK_j45_gsc55_boffperf_FTK_j28_j20_AND","j45_gsc55_b70_FTK_j28_b70_FTK_j20_b70_FTK_L1V3"]]],
           #['j55_gsc80_boffperf_FTK_j45_gsc55_boffperf_FTK_j28_j20_AND_j45_gsc55_b70_FTK_j28_b70_FTK_j20_b70_FTK_L1V4', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_boffperf_FTK_j45_gsc55_boffperf_FTK_j28_j20_AND","j45_gsc55_b70_FTK_j28_b70_FTK_j20_b70_FTK_L1V4"]]],
           ##2b70+2b85 x4 L1 seeds
           #['j55_gsc80_b85_FTK_j45_gsc55_b85_FTK_j28_b85_FTK_j20_b85_FTK_AND_j28_b70_FTK_j20_b70_FTK_L1V1', 'L1_J45.0ETA20_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b85_FTK_j45_gsc55_b85_FTK_j28_b85_FTK_j20_b85_FTK_AND","j28_b70_FTK_j20_b70_FTK_L1V1"]]],
           #['j55_gsc80_b85_FTK_j45_gsc55_b85_FTK_j28_b85_FTK_j20_b85_FTK_AND_j28_b70_FTK_j20_b70_FTK_L1V2', 'L1_J50_2J40.0ETA25_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b85_FTK_j45_gsc55_b85_FTK_j28_b85_FTK_j20_b85_FTK_AND","j28_b70_FTK_j20_b70_FTK_L1V2"]]],
           #['j55_gsc80_b85_FTK_j45_gsc55_b85_FTK_j28_b85_FTK_j20_b85_FTK_AND_j28_b70_FTK_j20_b70_FTK_L1V3', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b85_FTK_j45_gsc55_b85_FTK_j28_b85_FTK_j20_b85_FTK_AND","j28_b70_FTK_j20_b70_FTK_L1V3"]]],
           #['j55_gsc80_b85_FTK_j45_gsc55_b85_FTK_j28_b85_FTK_j20_b85_FTK_AND_j28_b70_FTK_j20_b70_FTK_L1V4', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1,['serial', -1,["j55_gsc80_b85_FTK_j45_gsc55_b85_FTK_j28_b85_FTK_j20_b85_FTK_AND","j28_b70_FTK_j20_b70_FTK_L1V4"]]],
           #4b77 x4 L1 seeds
           #['j55_gsc80_b77_FTK_j45_gsc55_b77_FTK_j28_b77_FTK_j20_b77_FTK_L1V1', 'L1_J45.0ETA20_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
           #['j55_gsc80_b77_FTK_j45_gsc55_b77_FTK_j28_b77_FTK_j20_b77_FTK_L1V2', 'L1_J50_2J40.0ETA25_3J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
           #['j55_gsc80_b77_FTK_j45_gsc55_b77_FTK_j28_b77_FTK_j20_b77_FTK_L1V3', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
           #['j55_gsc80_b77_FTK_j45_gsc55_b77_FTK_j28_b77_FTK_j20_b77_FTK_L1V4', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],


##  ATR-19574 copy gsc triggers with FTK; only "legacy-primary"
           ['j175_gsc225_bmv2c1040_split_FTKRefit', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j225_gsc275_bmv2c1060_split_FTKRefit', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j225_gsc300_bmv2c1070_split_FTKRefit', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j225_gsc360_bmv2c1077_split_FTKRefit', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

           ## Single jet with hybrid tuning
           ['j225_gsc275_bhmv2c1060_split_FTKRefit',        'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j225_gsc300_bhmv2c1070_split_FTKRefit',        'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j225_gsc360_bhmv2c1077_split_FTKRefit',        'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

           ['j55_gsc75_bmv2c1040_split_3j55_gsc75_boffperf_split_FTKRefit',       'L1_4J20',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j60_gsc85_bmv2c1050_split_3j60_gsc85_boffperf_split_FTKRefit',       'L1_4J20',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j150_gsc175_bmv2c1060_split_j45_gsc60_bmv2c1060_split_FTKRefit', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
           ['j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_FTKRefit_L1J85_3J30', 'L1_J85_3J30', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],


           ['2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_FTKRefit_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
           ['2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_FTKRefit_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

           ['2j35_gsc45_bmv2c1060_split_3j35_gsc45_boffperf_split_FTKRefit',       'L1_5J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

           ['3j50_gsc65_bmv2c1077_split_FTKRefit_L13J35.0ETA23', 'L1_3J35.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

           ['j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_FTKRefit_j45_320eta490', 'L1_J40.0ETA25_2J25_J20.31ETA49', [], [PhysicsStream], ['RATE:MultiBJet','BW:BJet'], -1],


            ['2j35_bmv2c1060_split_FTKRefit_2j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
            ['2j35_bmv2c1060_split_FTKRefit_3j35', 'L1_5J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
            ['3j35_bmv2c1070_split_FTKRefit_j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
            ['4j35_bmv2c1077_split_FTKRefit_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
            ['2j35_bmv2c1070_split_FTKRefit_2j35_bmv2c1085_split_FTKRefit_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
           ]

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

            ['xe100_trkmht_FTK_xe65_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1, ['serial',-1,['xe65_L1XE50','xe100_trkmht_FTK'] ]],
            ['xe105_trkmht_FTK_xe60_pufit_xe60_L1XE50', 'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1, ['serial',-1,['xe60_L1XE50','xe60_pufit','xe105_trkmht_FTK'] ]],
            ['xe90_trkmht_FTK_xe80_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe80_pufit','xe90_trkmht_FTK'] ]],
            ['xe95_trkmht_FTK_xe80_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe80_pufit','xe95_trkmht_FTK'] ]],
            ['xe100_trkmht_FTK_xe80_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe80_pufit','xe100_trkmht_FTK'] ]],
            ['xe90_trkmht_FTK_xe85_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe85_pufit','xe90_trkmht_FTK'] ]],
            ['xe95_trkmht_FTK_xe85_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe85_pufit','xe95_trkmht_FTK'] ]],
            ['xe100_trkmht_FTK_xe85_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe85_pufit','xe100_trkmht_FTK'] ]],

            ['xe95_trkmht_FTK_xe90_pufit_xe65_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x65_L1XE50','xe90_pufit','xe95_trkmht_FTK'] ]],
            ['xe95_trkmht_FTK_xe85_pufit_xe60_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x60_L1XE50','xe85_pufit','xe95_trkmht_FTK'] ]],
            ['xe95_trkmht_FTK_xe85_pufit_xe70_L1XE50',  'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['x70_L1XE50','xe85_pufit','xe95_trkmht_FTK'] ]],

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
            ['tau12_idperf_FTK',              'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
            ['tau12_perf_FTK',                'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
            ['tau12_perf0_FTK',               'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
            ['tau12_perf_FTKNoPrec',          'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
            ['tau12_perf0_FTKNoPrec',         'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
            ['tau12_medium0_FTK',             'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
            ['tau12_medium1_FTK',             'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
            ['tau12_medium0_FTKNoPrec',       'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
            ['tau12_medium1_FTKNoPrec',       'L1_TAU12IM', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],

                           ]

        TriggerFlags.BphysicsSlice.signatures = TriggerFlags.BphysicsSlice.signatures() + [
            ['2mu4_bBmumuxv2_Ftk',                'L1_2MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],

    ]

        TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures() + [

##  ATR-19574 copy gsc triggers with FTK; only "legacy-primary"
            ['j45_gsc55_bmv2c1050_split_FTKRefit_ht700_L1HT190-J15s5.ETA21',  'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet_combined'], -1,['serial',-1,['ht700','j45_gsc55_bmv2c1050_split_FTKRefit']]],
            ['2j45_gsc55_bmv2c1050_split_FTKRefit_ht300_L1HT190-J15s5.ETA21', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet_combined'], -1,['serial',-1,[ 'ht300', '2j45_gsc55_bmv2c1050_split_FTKRefit' ]]],

            ['2j35_gsc45_bmv2c1070_split_FTKRefit_xe85_pufit_xe50_L12J15_XE55',"L1_2J15_XE55",[],[PhysicsStream], ['RATE:MultiBJet', 'BW:BJet_combined', 'BW:MET'], -1, ['serial',-1,['xe85_pufit','xe50_L12J15_XE55','2j35_gsc45_bmv2c1070_split_FTKRefit']]],
            ['j85_gsc100_bmv2c1050_split_FTKRefit_xe85_pufit_xe50_L1XE55',"L1_XE55",[],[PhysicsStream], ['RATE:MultiBJet', 'BW:BJet_combined', 'BW:MET'], -1, ['serial',-1,['xe85_pufit','xe50_L1XE55','j85_gsc100_bmv2c1050_split_FTKRefit']]],

            ['mu26_ivarmedium_j110_gsc150_boffperf_split_FTKRefit_j35_boffperf_split_FTKRefit', 'L1_MU20MU21',    ['L1_MU20',    '', ''], [PhysicsStream], ['RATE:MuonBJet',  'BW:BJet_combined'],  -1, ['serial',-1,['mu26_ivarmedium', 'j110_gsc150_boffperf_split_FTKRefit', 'j35_boffperf_split_FTKRefit']]],
            ['e28_lhtight_nod0_ivarloose_j110_gsc150_boffperf_split_FTKRefit_j35_boffperf_split_FTKRefit',  'L1_EM24VHI', ['L1_EM24VHI', '', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet_combined'],  -1, ['serial',-1,['e28_lhtight_nod0_ivarloose', 'j110_gsc150_boffperf_split_FTKRefit','j35_boffperf_split_FTKRefit']]],

            ['j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split_FTKRefit','L1_J40.0ETA25_2J25_J20.31ETA49', ['','','','',''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet_combined' ], -1, ['serial',-1,['j80_0eta240','j60','j45_320eta490_AND','2j35_gsc45_bmv2c1070_split_FTKRefit'] ]],

            ['j45_gsc55_bmv2c1070_split_FTKRefit_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49','L1_J25.0ETA23_2J15.31ETA49',['',''], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet_combined' ], -1, ['serial',-1,['2j45_320eta490_L1J25.0ETA23_2J15.31ETA49','j45_gsc55_bmv2c1070_split_FTKRefit'] ]],

            ['2mu14_2j35_boffperf_split_FTKRefit',        'L1_2MU10',   ['L1_2MU10',   ''],     [PhysicsStream], ['RATE:MuonBJet',  'BW:BJet_combined'],  -1, ['serial',-1,['2mu14', '2j35_boffperf_split_FTKRefit']]],
            ['2e24_lhvloose_nod0_2j35_boffperf_split_FTKRefit',   'L1_2EM20VH', ['L1_2EM20VH', ''],     [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet_combined'],   -1, ['serial',-1,['2e24_lhvloose_nod0', '2j35_boffperf_split_FTKRefit']]],
            ['e28_lhmedium_nod0_L1EM24VHI_mu8noL1_2j35_boffperf_split_FTKRefit', 'L1_EM24VHI', ['L1_EM24VHI', '', ''], [PhysicsStream], ['RATE:EgammaMuon',  'BW:BJet_combined'], -1, ['serial',-1,['e28_lhmedium_nod0_L1EM24VHI','mu8noL1','2j35_boffperf_split_FTKRefit'] ]],
            ['mu26_ivarmedium_j150_boffperf_split_FTKRefit_j35_boffperf_split_FTKRefit',          'L1_MU20MU21',    ['L1_MU20',    '', ''], [PhysicsStream], ['RATE:MuonBJet',  'BW:BJet_combined'],  -1, ['serial',-1,['mu26_ivarmedium', 'j150_boffperf_split_FTKRefit','j35_boffperf_split_FTKRefit']]],
            ['e28_lhtight_nod0_ivarloose_j150_boffperf_split_FTKRefit_j35_boffperf_split_FTKRefit',               'L1_EM24VHI', ['L1_EM24VHI', '', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet_combined'],  -1, ['serial',-1,['e28_lhtight_nod0_ivarloose', 'j150_boffperf_split_FTKRefit','j35_boffperf_split_FTKRefit']]],
            ['j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ-500-NFF_AND_2j35_bmv2c1070_split_FTKRefit', 'L1_MJJ-500-NFF', [], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial', -1,["j70_0eta490","j50_0eta490","2j35_0eta490_invm1000j50_L1MJJ-500-NFF_AND","2j35_bmv2c1070_split_FTKRefit"]]],
            ['j70_0eta490_j50_0eta490_2j35_0eta490_invm1200j50_L1MJJ-500-NFF_AND_2j35_bmv2c1070_split_FTKRefit', 'L1_MJJ-500-NFF', [], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial', -1,["j70_0eta490","j50_0eta490","2j35_0eta490_invm1200j50_L1MJJ-500-NFF_AND","2j35_bmv2c1070_split_FTKRefit"]]],
            ['j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ-700_AND_2j35_bmv2c1070_split_FTKRefit', 'L1_MJJ-700', [], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial', -1,["j70_0eta490","j50_0eta490","2j35_0eta490_invm1000j50_L1MJJ-700_AND","2j35_bmv2c1070_split_FTKRefit"]]],
            ['j70_0eta490_j50_0eta490_2j35_0eta490_invm1200j50_L1MJJ-700_AND_2j35_bmv2c1070_split_FTKRefit', 'L1_MJJ-700', [], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial', -1,["j70_0eta490","j50_0eta490","2j35_0eta490_invm1200j50_L1MJJ-700_AND","2j35_bmv2c1070_split_FTKRefit"]]],
            ['3j35_bmv2c1077_split_FTKRefit_xe70_pufit_xe50_L13J15.0ETA25_XE40',"L1_3J15.0ETA25_XE40",[],[PhysicsStream], ['RATE:MultiBJet', 'BW:BJet_combined', 'BW:MET'], -1, ['serial',-1,['xe70_pufit','xe50_L13J15.0ETA25_XE40','3j35_bmv2c1077_split_FTKRefit']]],
            ['g20_tight_icaloloose_j35_bmv2c1077_split_FTKRefit_3j35_0eta490_invm500', 'L1_EM18VHI_MJJ-300', ['','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g20_tight_icaloloose","3j35_0eta490_invm500","j35_bmv2c1077_split_FTKRefit"]]],
            ['g27_medium_L1EM24VHI_j35_bmv2c1077_split_FTKRefit_3j35_0eta490_invm700', 'L1_EM24VHI', ['L1_EM24VHI','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g27_medium_L1EM24VHI","3j35_0eta490_invm700","j35_bmv2c1077_split_FTKRefit"]]],
            ['g27_medium_L1EM24VHI_2j35_bmv2c1077_split_FTKRefit_2j35_0eta490', 'L1_EM24VHI', ['L1_EM24VHI','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g27_medium_L1EM24VHI","2j35_0eta490","2j35_bmv2c1077_split_FTKRefit"]]],
            ['g25_medium_L1EM22VHI_j35_0eta490_bmv2c1077_split_FTKRefit_3j35_0eta490_invm700', 'L1_EM22VHI', ['L1_EM22VHI', '', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet_combined','BW:Egamma'],-1,['serial',-1,['g25_medium_L1EM22VHI','3j35_0eta490_invm700','j35_0eta490_bmv2c1077_split_FTKRefit']]],
            ['g25_medium_L1EM22VHI_2j35_0eta490_bmv2c1077_split_FTKRefit_2j35_0eta490', 'L1_EM22VHI', ['L1_EM22VHI', '', ''], [PhysicsStream], ['RATE:EgammaJet', 'BW:BJet_combined','BW:Egamma'],-1,['serial',-1,['g25_medium_L1EM22VHI','2j35_0eta490','2j35_0eta490_bmv2c1077_split_FTKRefit']]],
            ['j80_bmv2c1050_split_FTKRefit_xe60_L12J50_XE40','L1_2J50_XE40',  [],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:BJet_combined'], -1,['serial',-1,["xe60","j80_bmv2c1050_split_FTKRefit"]]],

            #ATR-19317
            ['g35_loose_L1EM30VHI_j0_perf_gsc0_boffperf_split_FTK',  'L1_EM30VHI', ['L1_EM30VHI',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g35_loose_L1EM30VHI","j0_perf_gsc0_boffperf_split_FTK"]]],
            ['g35_loose_L1EM30VHI_3j15_gsc25_boffperf_split_FTK',  'L1_EM30VHI', ['L1_EM30VHI',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g35_loose_L1EM30VHI","3j15_gsc25_boffperf_split_FTK"]]],

            ]

        TriggerFlags.MinBiasSlice.signatures = TriggerFlags.MinBiasSlice.signatures() + [
            ['mb_trk40_hmftk_FTKVtx_L1TE5', 'L1_TE5', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
            ['mb_trk50_hmftk_FTKVtx_L1TE5', 'L1_TE5', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
            ['mb_trk60_hmftk_FTKVtx_L1TE5', 'L1_TE5', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
            ['mb_trk70_hmftk_FTKVtx_L1TE5', 'L1_TE5', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
            ['mb_trk80_hmftk_FTKVtx_L1TE5', 'L1_TE5', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
            ['mb_trk90_hmftk_FTKVtx_L1TE5', 'L1_TE5', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
            ['mb_trk100_hmftk_FTKVtx_L1TE5', 'L1_TE5', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
            ['mb_trk110_hmftk_FTKVtx_L1TE5', 'L1_TE5', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
            ['mb_trk120_hmftk_FTKVtx_L1TE5', 'L1_TE5', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
            ['mb_trk130_hmftk_FTKVtx_L1TE5', 'L1_TE5', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

        ]

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

ps_unconvtrk_list= [
      'xe0noL1_l2fsperf_trkmht_LRT',
]    

if TriggerFlags.doFTK():
    ps_rerun_list += [
      'xe0noL1_l2fsperf_trkmht_FTK',
      'xe0noL1_l2fsperf_trktc_lcw_FTK',
          ]

ps_larnoiseburst_rerun_list = []

ps_perform_list = [
    'j0_a10t_lcw_jes_subj360Iwidth',
    'j0_a10t_lcw_jes_subj360I0width5',
    'j0_a10t_lcw_jes_subj360I0ktdr5',
    'j0_a10t_lcw_jes_subj360Iktdr',
    'j0_a10t_lcw_jes_subj360Iktdr3Iwidth2',
    'j0_a10t_lcw_jes_subj360IktdrI9width',
    'j0_perf_L1RD0_FILLED',
    'j0_perf_sktc_em_nojcalib_L1RD0_FILLED',
    'j0_perf_tc_em_nojcalib_L1RD0_FILLED',
    'j0_perf_tc_em_sub_L1RD0_FILLED',
    'j0_perf_sktc_lcw_nojcalib_L1RD0_FILLED',
    'j0_perf_tc_lcw_nojcalib_L1RD0_FILLED',
    'j0_perf_tc_lcw_sub_L1RD0_FILLED',

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
    'j0_gsc0_boffperf_split_L1RD0_FILLED',
    ]
if TriggerFlags.doFTK():
    ps_perform_list += [
        'j0_perf_ftk_L1RD0_FILLED',
        'j0_perf_ftkrefit_L1RD0_FILLED',
        'j0_gsc0_boffperf_split_FTK_L1RD0_FILLED',
        'j0_gsc0_boffperf_split_FTKRefit_L1RD0_FILLED',
    ]

ps_Jpsi_list = []

ps_ftk_list = []

ps_Bphys_list = []

ps_ftk_beamspot_list = []

ps_hmftk = []

if TriggerFlags.doFTK():
    ps_hmftk = [
        'mb_trk40_hmftk_FTKVtx_L1TE5',
        'mb_trk50_hmftk_FTKVtx_L1TE5',
        'mb_trk60_hmftk_FTKVtx_L1TE5',
        'mb_trk70_hmftk_FTKVtx_L1TE5',
        'mb_trk80_hmftk_FTKVtx_L1TE5',
        'mb_trk90_hmftk_FTKVtx_L1TE5',
        'mb_trk100_hmftk_FTKVtx_L1TE5',
        'mb_trk110_hmftk_FTKVtx_L1TE5',
        'mb_trk120_hmftk_FTKVtx_L1TE5',
        'mb_trk130_hmftk_FTKVtx_L1TE5',
    ]

    ps_ftk_beamspot_list += [
        'beamspot_idperf_FTK_L1All',
        'beamspot_idperf_FTKRefit_L1All'
        ]

    ps_ftk_list += [
        'beamspot_idperf_FTK',
        'beamspot_idperf_FTKRefit',
        'mu6_idperf_FTK',
        'mu24_idperf_FTK',
        'mu6_idperf_FTKRefit',
        'mu24_idperf_FTKRefit',
        'j35_boffperf_split_FTK_L1J15',
        'j150_boffperf_split_FTK_L1J40',
        'j35_boffperf_split_FTKVtx_L1J15',
        'j150_boffperf_split_FTKVtx_L1J40',
        'j35_boffperf_split_FTKRefit_L1J15',
        'j150_boffperf_split_FTKRefit_L1J40',
        'j225_boffperf_split_FTK_L1J100',
        'j225_boffperf_split_FTKVtx_L1J100',
        'j225_boffperf_split_FTKRefit_L1J100',
        ]

#-----------------------------------------------------


chain_list=ps_online_list+ps_cosmic_list+ps_minb_list+ps_unconvtrk_list
if TriggerFlags.doFTK():
    chain_list+=ps_ftk_beamspot_list

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
# TIGHTPERF mc prescales for performance
######################################################
Prescales.L1Prescales_tightperf_mc_prescale= deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_tightperf_mc_prescale = deepcopy(Prescales.HLTPrescales_loose_mc_prescale)

chain_list=ps_eb_list+ps_fwdphys_list+ps_minb_list+ps_Bphys_list#+ps_Jpsi_list # enabling JPSI ee again ATR-15162

Prescales.HLTPrescales_tightperf_mc_prescale.update(dict(map(None,chain_list,len(chain_list)*[ [-1, 0,-1] ])))
Prescales.L1Prescales_TriggerValidation_mc_prescale= deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_TriggerValidation_mc_prescale = deepcopy(Prescales.HLTPrescales_tightperf_mc_prescale)
######################################################
# TIGHT mc prescales
######################################################
Prescales.L1Prescales_tight_mc_prescale  = deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_tight_mc_prescale = deepcopy(Prescales.HLTPrescales_tightperf_mc_prescale)
chain_list=ps_perform_list + ps_ftk_list + ps_hmftk

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
