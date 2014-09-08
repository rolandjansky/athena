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

    # ------------------------------------------------
    # INPUT FORMAT FOR CHAINS:
    # ------------------------------------------------
    # ['chainName', chainCounter, 'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep]


    TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
        #rerun chain
        ['mu0noL1_fsperf' ,     59,  ''      , [], ['Muon'], ['Rate:SingleMuon', 'BW:Muon'], 1],

       # #FS muons
        #['mu20_mu8noL1',         727, 'L1_MU20', ['L1_MU20',''], ['Muon'], ['RATE:MultiMuon','BW:Muon'], 1,['serial',-1,["mu20","mu8noL1"]]],        
        #['mu20_imedium_mu8noL1', 726, 'L1_MU20', ['L1_MU20',""], ['Muon'], ['RATE:MultiMuon','BW:Muon'], 1,['serial',-1,['mu20_imedium','mu8noL1']]],
        #['mu20_2mu4noL1',        728, 'L1_MU20', ['L1_MU20',""], ['Muon'], ['RATE:MultiMuon','BW:Muon'], 1,['serial',-1,['mu20','2mu4noL1']]],
        #['mu20_iloose_mu8noL1',  744, 'L1_MU20', ['L1_MU20',''], ['Muon'], ['RATE:MultiMuon','BW:Muon'], 1,['serial',-1,['mu20_iloose','mu8noL1']]],
        #['mu20_iloose_2mu4noL1', 745, 'L1_MU20', ['L1_MU20',''], ['Muon'], ['RATE:MultiMuon','BW:Muon'], 1,['serial',-1,['mu20_iloose','2mu4noL1']]],

        # #T&P chains
        # #['mu24_imedium_mu20noL1_msonly', 729, 'L1_MU20', [], ['Muon'], ['RATE:MultiMuon','BW:Muon'], 1],        
        # #['mu24_imedium_mu20noL1_msonly', 730, 'L1_MU20', [], ['Muon'], ['RATE:MultiMuon','BW:Muon'], 1],       
        ]

    TriggerFlags.JetSlice.signatures = TriggerFlags.JetSlice.signatures() + [
        ]

    TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [
        #Lower priority items: 
        #['j175_bmedium_j60_bmedium',     14,    'L1_J100',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],# <- J100 
        ]

    TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + [        
       ####PU suppression####
        #['xe50_pueta',                            364,    'L1_XE35',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],
        #--- Run L2 algos
        ['xe50_l2fsperf_pueta',                   365,    'L1_XE35',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],
        ##Muon corrections to FEB MET-use EF_mu8
        ['xe50_l2fsperf_wEFMuFEB_pueta',           366,    'L1_XE35',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],
        
        ##Other thresolds:
        ['xe70_pueta',                            367,    'L1_XE50',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],
        ['xe70_l2fsperf_pueta',                   368,    'L1_XE50',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],    
        
        ['xe100_pueta',                           369,    'L1_XE70',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],
        ['xe100_l2fsperf_pueta',                  370,    'L1_XE70',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],
        
        ####--- cut on FEB MET
        ['xe50_L2FS',                        2011,    'L1_XE35',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],
        ]

    TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [
        ]

    TriggerFlags.EgammaSlice.signatures = TriggerFlags.EgammaSlice.signatures() + [
       # chainname: [chaincounter, stream, group, bw, prescale, pass through, rerun (to be set to 0 for all), eBstep]
        # single electron chains
        # Et cut only chains
        ['e5_etcut',                120,          'L1_EM3', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        #['e15_etcut',               121,          'L1_EM12', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e15_etcut_L1EM7V',          121,          'L1_EM7V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e25_etcut',               122,          'L1_EM20V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e35_etcut',               123,          'L1_EM20V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e45_etcut',               124,          'L1_EM20V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e55_etcut',               125,          'L1_EM50', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e65_etcut',               126,          'L1_EM50', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e80_etcut',               127,          'L1_EM50', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e100_etcut',              128,          'L1_EM50', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e120_etcut',              129,          'L1_EM50', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 

        # Cut-based Run1 PID chains
        # Loose chains with etcut variations
        ['e5_loose1',               130,          'L1_EM3', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['e15_loose1',              131,          'L1_EM12', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e15_loose1_L1EM7V',         131,          'L1_EM7V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e25_loose1',              132,          'L1_EM20V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e35_loose1',              133,          'L1_EM20V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e45_loose1',              134,          'L1_EM20V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e55_loose1',              135,          'L1_EM50', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e65_loose1',              136,          'L1_EM50', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e80_loose1',              137,          'L1_EM50', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e100_loose1',             138,          'L1_EM50', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], #needed for exotics background estimation --> needs lower prescale than the rest
        ['e120_loose1',             139,          'L1_EM50', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        
        # Single electron Run1 PID
        ['e24_medium1_iloose',       140, 'L1_EM18VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e24_loose1_L1EM20V',       141, 'L1_EM20V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e24_medium1_L1EM20V',      142, 'L1_EM20V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e24_tight1_L1EM20V',       143, 'L1_EM20V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e24_loose1',               144, 'L1_EM18VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['e24_loose1_L1EM18VHI',     145, 'L1_EM18VHI', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['e24_loose1_L1EM18VHIrel',  146, 'L1_EM18VHIrel', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e28_tight1_iloose',        147, 'L1_EM24VHI', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['e28_tight1_iloose_L1EM22VHI',    148, 'L1_EM22VHI', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['e28_tight1_iloose_L1EM20VHI',    149, 'L1_EM20VHI', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['e26_tight1_iloose',        150, 'L1_EM22VHI', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['e24_tight1_iloose_L1EM20VHI',    151, 'L1_EM20VHI', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e60_medium1',              152, 'L1_EM50', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e60_loose1',               153, 'L1_EM50', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 

        # Rerun mode triggers to support Run1 PID chains
        ['e18_loose1',           155,          'L1_EM15VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e17_loose1',           156,          'L1_EM15VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e12_loose1',           157,          'L1_EM8VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['e9_loose1',            158,          'L1_EM6', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e9_loose1_L1EM7V',       158,          'L1_EM7V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e7_medium1',           159,          'L1_EM3', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e12_medium1',          160,          'L1_EM8VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e17_medium1',          161,          'L1_EM15VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e5_tight1',            162,          'L1_EM3', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['e9_tight1',            163,          'L1_EM6', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['e14_tight1',           164,          'L1_EM12', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e9_tight1_L1EM3',      163,          'L1_EM3', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e14_tight1_L1EM7V',    164,          'L1_EM7V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e4_etcut',             165,          'L1_EM3', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['e9_etcut',             166,          'L1_EM6', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['e14_etcut',            167,          'L1_EM12', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e9_etcut_L1EM3',       166,          'L1_EM3', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e14_etcut_L1EM7V',     167,          'L1_EM7V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e28_loose1_iloose',    168,          'L1_EM24VHI', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],

        # Tracking tune chains for Run1 PID
        ['e28_tight1_iloose_L2StarA',        169, 'L1_EM24VHI', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e28_tight1_iloose_L2StarB',        172, 'L1_EM24VHI', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e28_tight1_iloose_L2StarC',        173, 'L1_EM24VHI', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e28_tight1_L2Star_idperf',         174, 'L1_EM24VHI', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e28_tight1_idperf',                175, 'L1_EM24VHI', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        
        # Rerun mode triggers to support LH PID chains
        ['e18_lhloose',           176,          'L1_EM15VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e17_lhloose',           177,          'L1_EM15VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e12_lhloose',           178,          'L1_EM8VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['e9_lhloose',            178,          'L1_EM6', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e9_lhloose_L1EM7V',     179,          'L1_EM7V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e28_lhloose_iloose',    180,          'L1_EM24VHI', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        
        # Single electron Run2 cut-based PID
        #['e24_medium_iloose',       170, 'L1_EM18VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], #In Physics Menu 
        ['e24_loose_L1EM20V',       181, 'L1_EM20V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e24_medium_L1EM20V',      182, 'L1_EM20V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e24_tight_L1EM20V',       183, 'L1_EM20V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e28_tight_iloose',        184, 'L1_EM24VHI', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], #Run2 baseline 
        ['e28_tight_iloose_L2StarA',185, 'L1_EM24VHI', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e60_medium',              186, 'L1_EM50', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        
        # /single electron LH ID chains
        ['e24_lhmedium_iloose',       187, 'L1_EM18VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e24_lhloose_L1EM20V',       188, 'L1_EM20V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], #L1_EM20V
        ['e24_lhmedium_L1EM20V',      189, 'L1_EM20V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], #L1_EM20V
        ['e24_lhtight_L1EM20V',       190, 'L1_EM20V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], #L1_EM20V
        ['e28_lhtight_iloose',        191, 'L1_EM24VHI', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], #L1_EM18VH
        ['e28_lhtight_iloose_L2StarA',192, 'L1_EM24VHI', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], #L1_EM18VH
        ['e28_lhtight_iloose_L2StarB',193, 'L1_EM24VHI', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e28_lhtight_iloose_L2StarC',194, 'L1_EM24VHI', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e28_lhtight_L2Star_idperf', 195, 'L1_EM24VHI', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e28_lhtight_idperf',        196, 'L1_EM24VHI', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e60_lhmedium',              197, 'L1_EM50', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], #L1_EM50
        
        # multi-electron chains
        # Run1 PID       ['2e12_loose1',             201, 'L1_2EM8VH', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['2e17_loose1',             200, 'L1_2EM15VH', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ###['e28_loose1_iloose_e12_loose1_L12EM3',  199, 'L1_EM24VHI', ['L1_EM24VHI','L1_2EM3'], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1], ### CHECK L1 Config, if correct L1 item needs to be added
        ###['e18_loose1_2e9_loose1_L13EM7V',   200, 'L1_EM15VH_3EM7V', ['L1_EM15VH','L1_3EM7V'], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1], ### CHECK L1 Config, if correct L1 item needs to be added
                                                                                   
        # LH PID
        ['2e17_lhloose',            201, 'L1_2EM15VH', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ###['e28_lhloose_iloose_e12_lhloose_L12EM3',202, 'L1_EM24VHI_2EM3', ['L1_EM24VHI','L1_2EM3'], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1], ### CHECK L1 Config, if correct L1 item needs to be added
        ###['e18_lhloose_2e9_lhloose_L13EM7V', 203, 'L1_EM15VH_3EM7V', ['L1_EM15VH','L1_3EM7V'], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1], ### CHECK L1 Config, if correct L1 item needs to be added
        # Multielectron chains with m_ee cut for Run1 PID
        ['e5_tight1_e4_etcut',        210,      'L1_2EM3', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['e5_tight1_e4_etcut_Jpsiee', 211,      'L1_2EM3', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['e5_tight1_L2StarA_e4_etcut_L2StarA_Jpsiee', 212,      'L1_2EM3', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['e5_tight1_L2StarB_e4_etcut_L2StarB_Jpsiee', 213,      'L1_2EM3', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['e5_tight1_L2StarC_e4_etcut_L2StarC_Jpsiee', 214,      'L1_2EM3', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['e5_tight1_L2Star_idperf_e4_etcut_L2Star_idperf_Jpsiee', 215,      'L1_2EM3', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['e5_tight1_idperf_e4_etcut_idperf_Jpsiee',   216,      'L1_2EM3', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        #['e9_tight1_e4_etcut_Jpsiee', 217,      'L1_2EM3_EM6', ['L1_EM6','L1_2EM3'], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        #['e5_tight1_e9_etcut_Jpsiee', 218,      'L1_2EM3_EM6', ['L1_EM3','L1_EM6'], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        #['e14_tight1_e4_etcut_Jpsiee', 219,      'L1_2EM3_EM12', ['L1_EM12','L1_2EM3'], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        #['e5_tight1_e14_etcut_Jpsiee', 220,      'L1_2EM3_EM12', ['L1_EM12','L1_2EM3'], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['e9_tight1_L1EM3_e4_etcut_L1EM3_Jpsiee', 217,      'L1_2EM3', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['e5_tight1_L1EM3_e9_etcut_L1EM3_Jpsiee', 218,      'L1_2EM3', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ###['e14_tight1_L1EM7V_e4_etcut_L12EM3_Jpsiee', 219,      'L1_EM7V_2EM3', ['L1_EM7V','L1_2EM3'], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1], ### CHECK L1 Config, if correct L1 item needs to be added
        ###['e5_tight1_L1EM7V_e14_etcut_L12EM3_Jpsiee', 220,      'L1_EM7V_2EM3', ['L1_EM7V','L1_2EM3'], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1], ### CHECK L1 Config, if correct L1 item needs to be added

        # Multielectron chains with m_ee cut for Run1 PID
        ['e5_lhtight_e4_etcut_Jpsiee',                 221,      'L1_2EM3', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['e5_lhtight_L2StarA_e4_etcut_L2StarA_Jpsiee', 222,      'L1_2EM3', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['e5_lhtight_L2StarB_e4_etcut_L2StarB_Jpsiee', 223,      'L1_2EM3', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['e5_lhtight_L2StarC_e4_etcut_L2StarC_Jpsiee', 224,      'L1_2EM3', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['e5_lhtight_L2Star_idperf_e4_etcut_L2Star_idperf_Jpsiee', 225,      'L1_2EM3', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ['e5_lhtight_idperf_e4_etcut_idperf_Jpsiee',   226,      'L1_2EM3', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],

        # Et cut only photon chains
        #['g12_etcut',               230,          'L1_EM7V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['g20_etcut_L1EM15V',        231,          'L1_EM15V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['g25_etcut',               232,          'L1_EM20V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['g35_etcut',               233,          'L1_EM20V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['g40_etcut',               234,          'L1_EM20V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['g50_etcut',               235,          'L1_EM20V', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['g60_etcut',               236,          'L1_EM50', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['g80_etcut',               237,          'L1_EM50', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['g100_etcut',              238,          'L1_EM50', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['g120_etcut',              239,          'L1_EM50', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],


        # Photon cut-based Run1 PID chains
        # Loose chains for low ET physics and background estimation
        ###['g12_loose1_L1EM7V',        240,          'L1_EM7V', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],### waiting for egamma changes
        ###['g20_loose1_L1EM15V',       241,          'L1_EM15V', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],### waiting for egamma changes
        ###['g25_loose1_L1EM15V',       242,          'L1_EM15V', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],### waiting for egamma changes
        ###['g35_loose1_L1EM15V',       243,          'L1_EM15V', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],### waiting for egamma changes
        ###['g40_loose1',               244,          'L1_EM20V', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],### waiting for egamma changes
        ###['g50_loose1',               245,          'L1_EM20V', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],### waiting for egamma changes
        ###['g60_loose1',               246,          'L1_EM50', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],### waiting for egamma changes  ### needed also in rerun mode!!!!
        ###['g80_loose1',               247,          'L1_EM50', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],### waiting for egamma changes
        ###['g100_loose1',              248,          'L1_EM50', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],### waiting for egamma changes
        
        # Run 2 Baseline chains
        ###['g120_loose1',              250,  'L1_EM50', [], ['Egamma'], ['RATE:SinglePhoton','BW:Egamma'], 1],### waiting for egamma changes
        ###['g140_loose1',              251,  'L1_EM50', [], ['Egamma'], ['RATE:SinglePhoton','BW:Egamma'], 1],### waiting for egamma changes

        # multi-photon chains
        ###['g35_loose1_g25_loose1',    252, 'L1_2EM15VH', [], ['Egamma'], ['RATE:MultiPhoton','BW:Egamma'], 1],### waiting for egamma changes
        ###['g35_medium1_g25_medium1',  253, 'L1_2EM15VH', [], ['Egamma'], ['RATE:MultiPhoton','BW:Egamma'], 1],### waiting for egamma changes
        ###['2g20_tight1',              254, 'L1_2EM15VH', [], ['Egamma'], ['RATE:MultiPhoton','BW:Egamma'], 1],### waiting for egamma changes
        ['3g20_loose1',              255, 'L1_2EM15VH', [], ['Egamma'], ['RATE:MultiPhoton','BW:Egamma'], 1],


        # Rerun mode trigger to support combined chains
        ['g4_etcut',                 260,          'L1_EM3', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ['g8_etcut',                 261,          'L1_EM3', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ['g8_etcut_L1EM7V',          262,          'L1_EM7V', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        #['g9_etcut',                 263,          'L1_EM6', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        #['g14_etcut',                264,          'L1_EM12', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        #['g9_etcut_L1EM3',           265,          'L1_EM3', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        #['g14_etcut_L1EM7V',         266,          'L1_EM7V', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],
        ###['g25_loose1',               265,          'L1_EM15VH', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],### waiting for egamma changes
        ###['g25_medium1',              266,          'L1_EM15VH', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],### waiting for egamma changes
        ###['g35_loose1',               267,          'L1_EM15VH', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],### waiting for egamma changes
        ###['g35_medium1',              268,          'L1_EM15VH', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],### waiting for egamma changes
        ###['g12_loose1',               269,          'L1_EM8VH', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],### waiting for egamma changes
        ###['g20_loose1',               270,          'L1_EM15VH', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],### waiting for egamma changes
        ###['g20_tight1',               271,          'L1_EM15VH', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],### waiting for egamma changes

        # e/g Combined + mass cut Chains ### should these become instead of 'Egamma' --> 'Combined' ???
        # J/psi
        ###['e5_tight1_g4_etcut', 280,      'L1_2EM3', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],### added to combined chains
        #['e5_tight1_g4_etcut_Jpsiee', 280,      'L1_2EM3', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        #['e9_tight1_g4_etcut_Jpsiee', 281,      'L1_2EM3_EM6', ['L1_EM6','L1_2EM3'], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        #['e5_tight1_g9_etcut_Jpsiee', 282,      'L1_EM3_EM6', ['L1_EM6','L1_2EM3'], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        #['e14_tight1_g4_etcut_Jpsiee', 283,      'L1_2EM3_EM12', ['L1_EM12','L1_2EM3'], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        #['e5_tight1_g14_etcut_Jpsiee', 284,      'L1_2EM3_EM12', ['L1_EM12','L1_2EM3'], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        
        # Zee ### should these become instead of 'Egamma' --> 'Combined' ???
        #['e28_tight1_iloose_g8_etcut_Zeg', 285, 'L1_EM24VHI', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],
        ###['e28_tight1_iloose_g8_etcut', 286, 'L1_EM24VHI', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],### added to combined chains
        ###['e17_loose1_2g8_etcut', 287, 'L1_EM15VH_3EM7V', ['L1_EM15VH','L1_3EM7V'], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1],### added to combined chains

        # Fall-back if problem with new tracking
        ['e17_etcut',                        290, 'L1_EM15VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e17_etcut_L2StarA',                291, 'L1_EM15VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e17_etcut_L2StarB',                292, 'L1_EM15VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e17_etcut_L2StarC',                293, 'L1_EM15VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e17_etcut_L2Star_idperf',          294, 'L1_EM15VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e17_etcut_idperf',                 295, 'L1_EM15VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        
        # perf chains to study L2/EF options ### need development in ElectronDef.py !!!
        ###['e_perf',               296, 'L1_EM15VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],### waiting for egamma changes
        ###['e_L2Star_perf',        297, 'L1_EM15VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],### waiting for egamma changes
        ###['g_perf',               298, 'L1_EM15VH', [], ['Egamma'], ['RATE:SinglePhoton', 'BW:Egamma'], 1],### waiting for egamma changes
                                                                                   
        # technical chain for forward electrons ### please check!!!! should this become instead of 'Egamma' --> 'Combined' ???
        ###['e17_loose1_L1EM15VHJ1523ETA49',    299,          'L1_EM15VH_J15.23ETA49', ['L1_EM15VH','L1_J15.23ETA49'], ['Egamma'], ['RATE:Combined', 'BW:Egamma'], 1],### waiting for L1 fixes

        ]

    TriggerFlags.BphysicsSlice.signatures = TriggerFlags.BphysicsSlice.signatures() + [
        ]


    TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures() + [
        #['g30_loose_mu8', 66,  'L1_EM30_MU6',['L1_EM30','L1_MU6' ], 'Photon'], [], 1],
        #['e24vhi_medium1_mu6', mu8', 815, 'L1_MU6', [], ['Muon'], [], 1],
        #['e17_loose_2g8_etcut', 816, 'L1_3EM6_EM14VH', [], ['Combined'], [], 1], #EM15VH_3EM7 # 
        ['e5_tight1_g4_etcut', 280,      'L1_2EM3', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1, ['parallel',-1,[] ]],
        ###['e28_tight1_iloose_g8_etcut', 286, 'L1_EM24VHI', [], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1, ['parallel',-1,[] ]],### CHECK L1 Config, if correct L1 item needs to be added
        ###['e17_loose1_2g8_etcut', 287, 'L1_EM15VH_3EM7V', ['L1_EM15VH','L1_3EM7V'], ['Egamma'], ['RATE:MultiElectron', 'BW:Egamma'], 1, ['parallel',-1,[] ]], ### CHECK L1 Config, if correct L1 item needs to be added
        ['e24_medium1_g25_medium1', 890, 'L1_2EM15VH',[], ['Egamma'], ['RATE:ElectronPhoton', 'BW:Egamma'], 1,['parallel',-1,[] ]],
        ['e20_medium1_g35_loose1', 891, 'L1_2EM15VH',[], ['Egamma'], ['RATE:ElectronPhoton', 'BW:Egamma'], 1,['parallel',-1,[] ]],
        ['e20_medium1_2g20_loose1', 892, 'L1_2EM15VH',[], ['Egamma'], ['RATE:ElectronPhoton', 'BW:Egamma'], 1,['parallel',-1,[] ]],
        ]

    TriggerFlags.MinBiasSlice.signatures = TriggerFlags.MinBiasSlice.signatures() + [
        ['mb_perf_L1LUCID', 823, 'L1_LUCID', [], ['MinBias'], [], 1],
        ]


    ## The following slices are only needed for Physics runnings and LS1 menus
    ## To be commented out for DC14
    TriggerFlags.CalibSlice.signatures   = TriggerFlags.CalibSlice.signatures() + []
    TriggerFlags.CosmicSlice.signatures  = TriggerFlags.CosmicSlice.signatures() + []
    TriggerFlags.StreamingSlice.signatures = TriggerFlags.StreamingSlice.signatures() + [
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



    
Prescales = physics_menu.Prescales
StreamConfig = physics_menu.StreamConfig

Prescales.L1Prescales = dict([(ctpid,1) for ctpid in Prescales.L1Prescales])  # setting all L1 prescales to 1

Prescales.L1Prescales_loose_mc_prescale  = Prescales.L1Prescales
Prescales.HLTPrescales_loose_mc_prescale = Prescales.HLTPrescales


Prescales.L1Prescales_tight_mc_prescale  = Prescales.L1Prescales
Prescales.HLTPrescales_tight_mc_prescale = {
    #'mu24_tight'                                : [    -1,    -1,    0,    0,  -1], 
    }

Prescales.L1Prescales_upgrade_mc_prescale  = Prescales.L1Prescales
Prescales.HLTPrescales_upgrade_mc_prescale = deepcopy(Prescales.HLTPrescales_tight_mc_prescale)
# Note: "upgrade" prescales are set with regular expressions at the end
#       of the setupMenu() function above 
