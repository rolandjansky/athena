# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
#------------------------------------------------------------------------#
def setupMenu():

    from TriggerMenu.menu.TriggerPythonConfig import TriggerPythonConfig
    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenu.menu.Physics_pp_v5.py' )

    from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1 as tcl1
    if tcl1.current:
        log.info("L1 items: %s " % tcl1.current.menu.items.itemNames())
    else:
        log.info("ERROR L1 menu has not yet been defined")


    # INPUT FORMAT FOR CHAINS:
    # ['chainName', chainCounter, 'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep]

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.MuonSlice.signatures = [ 
        #Single muon chains
        ['mu24_idperf',         820, 'L1_MU20', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'], 1],
        ['mu6_idperf',          799, 'L1_MU6', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'], 1],
        ['mu24_L2Star_idperf',  850, 'L1_MU20', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'], 1],
        ['mu6_L2Star_idperf',   851, 'L1_MU6', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'], 1],

        ['mu26_imedium',        821, 'L1_MU20', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'], 1],

        ['mu24',                816, 'L1_MU20', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'], 1],
        ['mu24_imedium',        817, 'L1_MU20', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'], 1],
        ['mu50',                718, 'L1_MU20', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'], 1],
        ['mu60_0eta105_msonly',   819, 'L1_MU20', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'], 1],

        #Multi muon chains
        ['2mu4',                721, 'L1_2MU4', [], ['Muon'], ['RATE:MultiMuon','BW:Muon'], 1],
        ['2mu6',                722, 'L1_2MU6', [], ['Muon'], ['RATE:MultiMuon','BW:Muon'], 1],
        ['2mu10',               723, 'L1_2MU10', [], ['Muon'], ['RATE:MultiMuon','BW:Muon'], 1],
        ['3mu6',                725, 'L1_3MU6', [], ['Muon'], ['RATE:MultiMuon','BW:Muon'], 1],
        ['2mu14',               724, 'L1_2MU10', [], ['Muon'], ['RATE:MultiMuon','BW:Muon'], 1],
        ['mu14_iloose_mu14',    818, 'L1_2MU10', [], ['Muon'], ['RATE:MultiMuon','BW:Muon'], 1],


        #Full Scan
        ['mu20_mu8noL1',         727, 'L1_MU20', ['L1_MU20',''], ['Muon'], ['RATE:MultiMuon','BW:Muon'], 1,['serial',-1,["mu20","mu8noL1"]]],        
        ['mu20_imedium_mu8noL1', 726, 'L1_MU20', ['L1_MU20',""], ['Muon'], ['RATE:MultiMuon','BW:Muon'], 1,['serial',-1,['mu20_imedium','mu8noL1']]],
        ['mu20_2mu4noL1',        728, 'L1_MU20', ['L1_MU20',""], ['Muon'], ['RATE:MultiMuon','BW:Muon'], 1,['serial',-1,['mu20','2mu4noL1']]],
        ['mu20_iloose_mu8noL1',  744, 'L1_MU20', ['L1_MU20',''], ['Muon'], ['RATE:MultiMuon','BW:Muon'], 1,['serial',-1,['mu20_iloose','mu8noL1']]],
        ['mu20_iloose_2mu4noL1', 745, 'L1_MU20', ['L1_MU20',''], ['Muon'], ['RATE:MultiMuon','BW:Muon'], 1,['serial',-1,['mu20_iloose','2mu4noL1']]],        
        ]
    

    TriggerFlags.JetSlice.signatures = [   
        #['j80',     104,    'L1_J30',[], ['JetEtMiss'], [], 1],
        ['j400',                100,    'L1_J100',[],  ['Jet'], ['RATE:SingleJet', 'BW:Jets'], 1],
        ['j460_a10_L1J100',     101,    'L1_J100',[], ['Jet'], ['RATE:SingleJet', 'BW:Jets'], 1],
        ['j200_320eta490',      102,    'L1_FJ100',[], ['Jet'], ['RATE:SingleJet', 'BW:Jets'], 1],
        ['j60',                 103,    'L1_J20',[], ['Jet'], ['RATE:SingleJet', 'BW:Jets'], 1],
        ['j110',                105,    'L1_J50',[], ['Jet'], ['RATE:SingleJet', 'BW:Jets'], 1],
        ['j150',                106,    'L1_J75',[], ['Jet'], ['RATE:SingleJet', 'BW:Jets'], 1],
        ['j200',                107,    'L1_J100',[], ['Jet'], ['RATE:SingleJet', 'BW:Jets'], 1],
        ['j260',                108,    'L1_J100',[], ['Jet'], ['RATE:SingleJet', 'BW:Jets'], 1],
        ['j330',                109,    'L1_J100',[], ['Jet'], ['RATE:SingleJet', 'BW:Jets'], 1],        
        ['3j175',               110,    'L1_J100',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],
        ['4j100',               111,    'L1_3J50',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],
        ['5j85',                112,    'L1_4J20',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],
        ['ht1000_L1J100', 99, 'L1_J100', [], ['Jet'], ['RATE:SingleJet', 'BW:Jets'], 1],

        ['j30_muvtx',         1, 'L1_2MU10', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'], 1],

        #Partial scan
        ['j200_PS', 114, 'L1_J100', [], ['Jet'], ['RATE:SingleJet', 'BW:Jets'], 1],         
        ['3j175_PS', 115, 'L1_J100', [], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],

        ]

    TriggerFlags.BjetSlice.signatures = [
        ['j55_bperf',                    10,    'L1_J20',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],
        ['j45_bperf_3j45',               11,    'L1_3J15',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],
        ['j45_bperf_3j45_L13J15.ETA24',  12,    'L1_3J15.ETA24',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],
        ['j45_bperf_3j45_L13J20',        13,    'L1_3J20',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],
        ['j55_EFID_bperf',               14,    'L1_J20',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],
        ['j175_bmedium',                 15,    'L1_J100',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],# <- J100 
        ['j75_bmedium_3j75',             16,    'L1_4J20',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1], #<- 4J20 
        ['2j55_bmedium_2j55',            17,    'L1_4J20',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],# <- 4J20 
        ['2j45_bmedium_3j45',            18,    'L1_5J15.ETA24',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],
        ['j300_bloose',                  30,    'L1_J100',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],
        ['j300_bloose_EFID',             41,    'L1_J100',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],

        # chains with EDIF
        ['j55_bperf_EFID',               32,    'L1_J20',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],
        ['j45_bperf_EFID_3j45',          33,    'L1_3J15',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],
        ['j45_bperf_EFID_3j45_L13J15.ETA24',  34,    'L1_3J15.ETA24',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],
        ['j45_bperf_EFID_3j45_L13J20',        35,    'L1_3J20',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],
        ['j175_bmedium_EFID',                 36,    'L1_J100',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],# <- J100 
        ['j75_bmedium_EFID_3j75',             37,    'L1_4J20',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1], #<- 4J20 
        ['2j55_bmedium_EFID_2j55',            38,    'L1_4J20',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],# <- 4J20 
        ['2j45_bmedium_EFID_3j45',            39,    'L1_5J15.ETA24',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],
        ['j175_bmedium_EFID_j60_bmedium_EFID', 40,    'L1_J100',[], ['Jet'], ['RATE:MultiJet', 'BW:Jets'], 1],
        ]

    TriggerFlags.METSlice.signatures = [
        #
        ['xe50',                            350,    'L1_XE35',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],
        ['xe50_cell',                       351,    'L1_XE35',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],        
        #--- Run L2 algos
        ['xe50_l2fsperf',                   352,    'L1_XE35',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],
        ['xe50_l2fsperf_cell',              353,    'L1_XE35',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],        
        ##Muon corrections to FEB MET-use EF_mu8
        ['xe50_l2fsperf_wEFMuFEB',           354,    'L1_XE35',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],
        ['xe50_l2fsperf_wEFMuFEB_cell',      355,    'L1_XE35',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],                
        ###Muon correction to FEB MET-use L2_mu8, and to EF MET -use EF_mu8
        ['xe50_l2fsperf_wEFMuFEB_wEFMu',     356,    'L1_XE35',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],
        ['xe50_l2fsperf_wEFMuFEB_wEFMu_cell',357,    'L1_XE35',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],
        #w/ Pile-Up suppression
        ['xe50_pueta',                            364,    'L1_XE35',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],

        ##Other thresolds:
        ['xe70',                            358,    'L1_XE50',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],
        ['xe70_cell',                       359,    'L1_XE50',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],    
        ['xe70_l2fsperf',                   360,    'L1_XE50',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],    
        
        ['xe100',                           361,    'L1_XE70',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],
        ['xe100_cell',                      362,    'L1_XE70',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],        
        ['xe100_l2fsperf',                  363,    'L1_XE70',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],
        
        ####PU suppression####
        ##['xe50_pueta',                            364,    'L1_XE35',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],
        ###--- Run L2 algos
        ##['xe50_l2fsperf_pueta',                   365,    'L1_XE35',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],
        ####Muon corrections to FEB MET-use EF_mu8
        ##['xe50_l2fsperf_wEFMuFEB_pueta',           366,    'L1_XE35',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],
        ##
        ####Other thresolds:
        ##['xe70_pueta',                            367,    'L1_XE50',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],
        ##['xe70_l2fsperf_pueta',                   368,    'L1_XE50',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],    
        ##
        ##['xe100_pueta',                           369,    'L1_XE70',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],
        ##['xe100_l2fsperf_pueta',                  370,    'L1_XE70',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],


        ##Rerun-chains
        ['xe0noL1_l2fsperf',                            371,    '',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],
        ['xe0noL1_l2fsperf_cell',                       372,    '',[], ['Met'], ['Rate:MET', 'BW:MET'], 1],
        ]

    TriggerFlags.TauSlice.signatures = [
        ['tau20_r1medium1',              5123,          'L1_TAU12', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau20_r1perf',                 5124,          'L1_TAU12', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau20_r1_idperf',              5125,          'L1_TAU12', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau25_medium1_calo',           5126,          'L1_TAU12', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau25_medium1_track',          5127,          'L1_TAU12', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau25_medium1_ptonly',         5128,          'L1_TAU12', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau25_medium1_caloonly',       5129,          'L1_TAU12', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau25_medium1_trackonly',      5130,          'L1_TAU12', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau25_medium1_mvonly',         5131,          'L1_TAU12', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau25_r1perf',                 5132,          'L1_TAU12', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau25_r1perf_L1TAU6',          5133,          'L1_TAU6',  [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau25_medium1_mvonly_L1TAU6',  5134,          'L1_TAU6',  [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau25_perf',                   5135,          'L1_TAU12', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau25_perf_L1TAU6',            5136,          'L1_TAU6', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau25_ptonly_idperf',          5137,          'L1_TAU12', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau29_r1medium1',              5138,          'L1_TAU20', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau29_r1perf',                 5139,          'L1_TAU20', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau35_medium1_calo',           5140,          'L1_TAU20', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau35_medium1_track',          5141,          'L1_TAU20', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau35_medium1_ptonly',         5142,          'L1_TAU20', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau35_perf',                   5143,          'L1_TAU20', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau80_medium1_calo',           5144,          'L1_TAU40', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau80_medium1_track',          5145,          'L1_TAU40', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau125_r1medium1',             5146,          'L1_TAU60', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau125_medium1_calo',          5147,          'L1_TAU60', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau125_medium1_track',         5148,          'L1_TAU60', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau125_r1perf',                5149,          'L1_TAU60', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        ['tau125_perf',                  5150,          'L1_TAU60', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], 1],
        #['tau160_medium', 123, 'L1_TAU60',[], ['Tau'], [], 1]
        ]

    TriggerFlags.EgammaSlice.signatures = [
        ['e24_medium_iloose',       170, 'L1_EM18VH', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        #['e24_medium_iloose', 178, 'L1_EM18VH', [], ['Electron'], [], 1],
        #['g120_loose',153,  'L1_EM50', [], ['Photon'], [], 1],#EM50
        ]

    TriggerFlags.BphysicsSlice.signatures = [
        #['2mu6_bMuMu_L12MU6', 299, 'L1_2MU6', [], ['Bphysics'], [], 1], 

        #L1_3MU6 seeded
        ['3mu6_bJpsi',                 300,  'L1_3MU6', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1],
        ['3mu6_bTau',                  301,  'L1_3MU6', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1],
        ['3mu6_bDimu',                 302,  'L1_3MU6', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1],

        #L1Topo seeded  # temporary changed L1_2MU6_02DR-MU6-MU6 into L1_2MU6 until L1Topo is available. (#104272)
        ['2mu6_bBmumu',                 303, 'L1_2MU6', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1],
        ['2mu6_bBmumuxv2',              304, 'L1_2MU6', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1],
        ['2mu6_bBmumux_BcmumuDsloose',  336, 'L1_2MU6', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1],
        ['2mu6_bJpsimumu',             305, 'L1_2MU6', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1],
        ['2mu6_bUpsimumu',             306, 'L1_2MU6', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1],

        #L1_2MU10 seeded
        ['2mu10_bBmumu',                337, 'L1_2MU10', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1],
        ['2mu10_bBmumuxv2',             338, 'L1_2MU10', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1],
        ['2mu10_bBmumux_BcmumuDsloose', 339, 'L1_2MU10', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1],
        ['2mu10_bJpsimumu',            330, 'L1_2MU10', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1],
        ['2mu10_bUpsimumu',            331, 'L1_2MU10', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1],
        ['mu13_mu13_idperf_Zmumu',     335, 'L1_2MU10', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1],

        #L1_2MU4 seeded
        ['mu4_iloose_mu4_7invm9_noos', 332, 'L1_2MU4', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1],
        ['mu4_mu4_idperf_bJpsimumu_noid',          333, 'L1_2MU4', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1],

        #L1_MU6 seeded
        ['mu6_bJpsi_Trkloose',         334, 'L1_MU6', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1],
        
        # L1_2MU6
        ['2mu6_bDimu', 340,             'L1_2MU6', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1], 
        ['2mu6_bDimu_novtx_noos', 341, 'L1_2MU6', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1],  
        ]

    TriggerFlags.CombinedSlice.signatures = [
        #['e24_medium_iloose_mu24', 815, 'L1_EM15VH_MU10', ['L1_EM15VH', 'L1_MU10'], ['Combined'], ['RATE:SingleElectron', 'BW:Egamma'], 1,['parallel',-1,[] ]],#EM24VH
        #['e24_medium_iloose_mu24', 815, 'L1_EM15VH_MU10', ['L1_EM15VH', 'L1_MU10'], ['Combined'], ['RATE:SingleElectron', 'BW:Egamma'], 1,['serial',-1,["e24_medium_iloose","mu24"]]],#EM24VH
        ['e17_medium_mu12', 815, 'L1_EM15VH_MU10', ['L1_EM15VH', 'L1_MU10'], ['Combined'], ['RATE:SingleElectron', 'BW:Egamma'], 1,['parallel',-1,[] ]],#EM24VH
        ['e7_medium_mu24', 814, 'L1_EM3_MU20', ['L1_EM3', 'L1_MU20'], ['Combined'], ['RATE:SingleElectron', 'BW:Egamma'], 1,['parallel',-1,[] ]],#EM24VH
        ['2e12_medium_mu12', 813, 'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], ['Combined'], ['RATE:SingleElectron', 'BW:Egamma'], 1,['parallel',-1,[] ]],#EM24VH
        ['e12_medium_2mu8', 812, 'L1_EM8VH_2MU6', ['L1_EM8VH', 'L1_2MU6'], ['Combined'], ['RATE:SingleElectron', 'BW:Egamma'], 1,['parallel',-1,[] ]],#EM24VH        
        ['2g20_loose1_mu20noL1', 807,      'L1_2EM15VH', ['L1_2EM15VH', ''], ['Combined'], ['RATE:ElectronMuon', 'BW:Egamma', 'BW:Muon'], 1, ['serial',-1,['2g20_loose1','mu20noL1'] ]], 
        #['e28_tight1_iloose_mu8noL1', 808,  'L1_EM24VHI', ['L1_EM24VHI', ''], ['Combined'], ['RATE:ElectronMuon', 'BW:Egamma'], 1, ['serial',-1,['e28_tight1_iloose','mu8noL1'] ]],       

        #MET+X
        ['mu24_xe100noL1_wEFMu',       901, 'L1_MU20',['L1_MU20',''], ['Combined'], ['RATE:METMuon', 'BW:Muon'], 1,['serial',-1,["mu24","xe100noL1_wEFMu"]]],
        #['e28_tight1_iloose_xe100noL1', 902, 'L1_EM24VHI',['L1_EM24VHI',''], ['Combined'], ['RATE:METElectron', 'BW:Egamma'], 1,['serial',-1,["e28_tight1_iloose","xe100noL1"]]],
        ['g60_loose1_xe60noL1',         903, 'L1_EM50',['L1_EM50',''], ['Combined'], ['RATE:METElectron', 'BW:Egamma'], 1,['serial',-1,["g60_loose1","xe60noL1"]]],        
        ['j100_xe80noL1',              904, 'L1_J40_XE50',['L1_J40',''], ['Combined'], ['RATE:JetMET', 'BW:Jets'], 1,['parallel',2,["j100","xe80noL1"]]],
        ['j80_xe80noL1',              905, 'L1_J40_XE50',['L1_J40',''], ['Combined'], ['RATE:JetMET', 'BW:Jets'], 1,['parallel',2,["j80","xe80noL1"]]],
        ['j80_xe80noL1_dphi1',        924, 'L1_J40_XE50',['L1_J40',''], ['Combined'], ['RATE:JetMET', 'BW:Jets'], 1,['parallel',2,["j80","xe80noL1"]]],

        # TAU+X
        ['tau35_medium1_calo_tau25_medium1_calo',          906, 'L1_TAU20_2TAU12_J25_2J20_3J12',['L1_TAU20','L1_TAU12'], ['Combined'], ['RATE:Tau', 'BW:Tau'], 1,['serial',-1,["tau35_medium1_calo","tau25_medium1_calo"]]],
        ['e18_loose1_tau25_medium1_calo',                  907, 'L1_EM15VH_2TAU12_J25_2J15_3J12',['L1_EM15VH','L1_TAU12'], ['Combined'], ['RATE:ElectronTau', 'BW:Tau'], 1,['serial',-1,["e18_loose1","tau25_medium1_calo"]]],
        ['e18_lhloose_tau25_medium1_calo',                 908, 'L1_EM15VH_2TAU12_J25_2J15_3J12',['L1_EM15VH','L1_TAU12'], ['Combined'], ['RATE:ElectronTau', 'BW:Tau'], 1,['serial',-1,["e18_lhloose","tau25_medium1_calo"]]],
        ['e18_loose1_tau80_medium1_calo',                  909, 'L1_EM15VH_TAU40_2TAU15',['L1_EM15VH','L1_TAU40'], ['Combined'], ['RATE:ElectronTau', 'BW:Tau'], 1,['serial',-1,["e18_loose1","tau80_medium1_calo"]]],
        ['e18_lhloose_tau80_medium1_calo',                 910, 'L1_EM15VH_TAU40_2TAU15',['L1_EM15VH','L1_TAU40'], ['Combined'], ['RATE:ElectronTau', 'BW:Tau'], 1,['serial',-1,["e18_lhloose","tau80_medium1_calo"]]],
        ['mu14_tau35_medium1_calo',                        911, 'L1_MU10_TAU20',['L1_MU10','L1_TAU20'], ['Combined'], ['RATE:MuonTau', 'BW:Tau'], 1,['serial',-1,["mu14","tau35_medium1_calo"]]],
        ['mu14_tau25_medium1_calo',                        912, 'L1_MU10_TAU12_J25_2J12',['L1_MU10','L1_TAU12'], ['Combined'], ['RATE:MuonTau', 'BW:Tau'], 1,['serial',-1,["mu14","tau25_medium1_calo"]]],
        ['tau35_medium1_calo_tau25_medium1_calo_xe50',     913, 'L1_TAU20_2TAU12_XE35',['L1_TAU20','L1_TAU12','L1_XE35'], ['Combined'], ['RATE:METTau', 'BW:Tau'], 1,['serial',-1,["tau35_medium1_calo","tau25_medium1_calo","xe50"]]],
        ['tau35_medium1_calo_xe70_L1XE45',                 914, 'L1_TAU20_2J20_XE45',['L1_TAU20','L1_XE45'], ['Combined'], ['RATE:METTau', 'BW:Tau'], 1,['serial',-1,["tau35_medium1_calo","xe70_L1XE45"]]],
        ['tau35_medium1_ptonly_tau25_medium1_ptonly',      915, 'L1_TAU20_2TAU12_J25_2J20_3J12',['L1_TAU20','L1_TAU12'], ['Combined'], ['RATE:Tau', 'BW:Tau'], 1,['serial',-1,["tau35_medium1_ptonly","tau25_medium1_ptonly"]]],
        ['e18_loose1_tau25_medium1_ptonly',                916, 'L1_EM15VH_2TAU12_J25_2J15_3J12',['L1_EM15VH','L1_TAU12'], ['Combined'], ['RATE:ElectronTau', 'BW:Tau'], 1,['serial',-1,["e18_loose1","tau25_medium1_ptonly"]]],
        ['e18_lhloose_tau25_medium1_ptonly',               917, 'L1_EM15VH_2TAU12_J25_2J15_3J12',['L1_EM15VH','L1_TAU12'], ['Combined'], ['RATE:ElectronTau', 'BW:Tau'], 1,['serial',-1,["e18_lhloose","tau25_medium1_ptonly"]]],
        ['e18_loose1_tau80_medium1_ptonly',                918, 'L1_EM15VH_TAU40_2TAU15',['L1_EM15VH','L1_TAU40'], ['Combined'], ['RATE:ElectronTau', 'BW:Tau'], 1,['serial',-1,["e18_loose1","tau80_medium1_ptonly"]]],
        ['e18_lhloose_tau80_medium1_ptonly',               919, 'L1_EM15VH_TAU40_2TAU15',['L1_EM15VH','L1_TAU40'], ['Combined'], ['RATE:ElectronTau', 'BW:Tau'], 1,['serial',-1,["e18_lhloose","tau80_medium1_ptonly"]]],
        ['mu14_tau35_medium1_ptonly',                      920, 'L1_MU10_TAU20',['L1_MU10','L1_TAU20'], ['Combined'], ['RATE:MuonTau', 'BW:Tau'], 1,['serial',-1,["mu14","tau35_medium1_ptonly"]]],
        ['mu14_tau25_medium1_ptonly',                      921, 'L1_MU10_TAU12_J25_2J12',['L1_MU10','L1_TAU12'], ['Combined'], ['RATE:MuonTau', 'BW:Tau'], 1,['serial',-1,["mu14","tau25_medium1_ptonly"]]],
        ['tau35_medium1_ptonly_tau25_medium1_ptonly_xe50', 922, 'L1_TAU20_2TAU12_XE35',['L1_TAU20','L1_TAU12','L1_XE35'], ['Combined'], ['RATE:METTau', 'BW:Tau'], 1,['serial',-1,["tau35_medium1_ptonly","tau25_medium1_ptonly","xe50"]]],
        ['tau35_medium1_ptonly_xe70_L1XE45',               923, 'L1_TAU20_2J20_XE45',['L1_TAU20','L1_XE45'], ['Combined'], ['RATE:METTau', 'BW:Tau'], 1,['serial',-1,["tau35_medium1_ptonly","xe70_L1XE45"]]],
        ]
   
    TriggerFlags.MinBiasSlice.signatures = [
        ['mb_sptrk', 806, 'L1_RD0_FILLED', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_perf_L1MBTS_1', 822, 'L1_MBTS_1', [], ['MinBias'], [], 1],
        ['mb_sp2000_trk70_hmt', 824, 'L1_TE20', [], ['MinBias'], [], 1],        

        ['mb_sp2000_pusup600_trk70_hmt', 836, 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1], 
        ['mb_sp2000_pusup600_trk70_hmt_L1TE30', 837, 'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1], 
        ['mb_sp2000_pusup600_trk70_hmt_L1TE40', 838, 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1], 
        ['mb_sp2000_pusup700_trk70_hmt_L1TE30', 839, 'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1], 
        ['mb_sp2000_pusup700_trk70_hmt_L1TE40', 840, 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1], 
        ]

    TriggerFlags.CalibSlice.signatures   = []
    TriggerFlags.CosmicSlice.signatures  = [ ]
    TriggerFlags.StreamingSlice.signatures = [
        ['noalg_mb_L1TE20', 843, 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1], 
        ['noalg_mb_L1TE30', 844, 'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1], 
        ['noalg_mb_L1TE40', 845, 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],         
        ]

    TriggerFlags.MonitorSlice.signatures = []
    #TriggerFlags.GenericSlice.signatures = []
    
class Prescales:
    #   Item name             | Prescale
    #----------------------------------------------------------
    L1Prescales = {

        }
    #   Signature name        |
    #      [ L2prescale, EFprescale, L2pass-through, EF-pass-through, rerun]
    #   - Prescale values should be a positive integer (default=1)
    #   - If the current pass_through value is non-zero,
    #     the value given here will be used as pass_through rate
    #     Assuming that pass through chains are configured so
    #     in the slice files and won't change. Also prescale
    #     and pass_through will not be used together.
    #   - If only the first two values (L2 and EF prescales) are specified,
    #     the default value of pass-through (=0) will be used
    #   - If L2 pass-through > 1, those events selected by L2 pass-through
    #     will automatically accepted at EF if EF pass-through=0 is set
    #----------------------------------------------------------
    HLTPrescales = {
        }


StreamConfig = {
    'Egamma':      [ 'egamma',
                     'comb__ej', 'comb__emet', 'comb__ejmet', 'comb__eg',
                     'comb__gmet', 
                     'comb__emu', 'comb__gmu',
                     'comb_tauej', 
                     ], 
    'MinBias':     [ 'minbias' ], 
    'Background':  [ 'background' ], 

    'Muons':       [ 'muons', 'bphysics',
                     'comb__muj', 'comb__mumet', 'comb__mujmet',
                     #'comb__emu', 'comb__gmu',
                     'comb_taumuj','comb__mue', 'comb__mug',
                     'comb__multimu',
                     ], 
    'JetTauEtmiss':   [ 'jets', 'tauEtmiss',
                        'comb__tauj', 'comb__taumet', 'comb__jmet',
                        'comb__taujmet', 'comb__taub', 'comb__taue', 
                        'comb__taue', 'comb__taumu', 'comb__fjj', 
                        'comb__tauej', 'comb__taumuj', ],
    # New delayed streams for Jet triggers
    'HadDelayed':  [ 'haddelayed' ],
    'JetCalibDelayed':  [ 'jetcalibdelayed' ],
    'Standby': [ 'standby' ], 
    'calibration': [ 'calibration' ], 
    'express':     [ 'express' ],

    }

