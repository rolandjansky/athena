# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
#------------------------------------------------------------------------#
def setupMenu():

    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenu.menu.DC14.py' )

    from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1 as tcl1
    if tcl1.current:
        log.info("L1 items: %s" % tcl1.current.menu.items.itemNames())
    else:
        log.info("L1 menu has not yet been defined or is being read from the DB!")

    # INPUT FORMAT FOR CHAINS:
    # ['L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep]

    globalStream = 'Main'

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.MuonSlice.signatures = [
        ['mu24_idperf', 'L1_MU20', [], [globalStream], ['RATE:SingleMuon','BW:Muon'], -1],
        ['mu6_idperf',          'L1_MU6', [],   [globalStream], ['RATE:SingleMuon','BW:Muon'], -1],
        #L2Star
        ['mu24_L2Star_idperf',        'L1_MU20', [], [globalStream], ['RATE:SingleMuon','BW:Muon'], -1],
        ['mu6_L2Star_idperf',         'L1_MU6', [],   [globalStream], ['RATE:SingleMuon','BW:Muon'], -1],
        #Single muon chains
        ['mu26',                'L1_MU20', [], [globalStream], ['RATE:SingleMuon','BW:Muon'], -1],
        ['mu26_imedium',       'L1_MU20', [],  [globalStream], ['RATE:SingleMuon','BW:Muon'], -1],

        ['mu24',                'L1_MU20', [], [globalStream], ['RATE:SingleMuon','BW:Muon'], -1],
        ['mu24_imedium',        'L1_MU20', [], [globalStream], ['RATE:SingleMuon','BW:Muon'], -1],
        ['mu50',                'L1_MU20', [], [globalStream], ['RATE:SingleMuon','BW:Muon'], -1],
        ['mu60_0eta105_msonly', 'L1_MU20', [], [globalStream], ['RATE:SingleMuon','BW:Muon'], -1],

        #Multi muon chains
        ['2mu4',               'L1_2MU4', [],   [globalStream], ['RATE:MultiMuon','BW:Muon'], -1],
        ['2mu6',               'L1_2MU6', [],   [globalStream], ['RATE:MultiMuon','BW:Muon'], -1],
        ['2mu10',              'L1_2MU10', [],  [globalStream], ['RATE:MultiMuon','BW:Muon'], -1],
        ['3mu6',               'L1_3MU6', [],   [globalStream], ['RATE:MultiMuon','BW:Muon'], -1],
        ['2mu14',               'L1_2MU10', [], [globalStream], ['RATE:MultiMuon','BW:Muon'], -1],
        ['mu14_iloose_mu14',    'L1_2MU10', [], [globalStream], ['RATE:MultiMuon','BW:Muon'], -1],

        ##Full scan
        #['mu20_mu8noL1',          'L1_MU20', ['L1_MU20',''], [globalStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,["mu20","mu8noL1"]]],        
        #['mu20_imedium_mu8noL1',  'L1_MU20', ['L1_MU20',""], [globalStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_imedium','mu8noL1']]],
        #['mu20_2mu4noL1',         'L1_MU20', ['L1_MU20',""], [globalStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','2mu4noL1']]],
        #['mu20_iloose_mu8noL1',   'L1_MU20', ['L1_MU20',''], [globalStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_iloose','mu8noL1']]],
        #['mu20_iloose_2mu4noL1',  'L1_MU20', ['L1_MU20',''], [globalStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_iloose','2mu4noL1']]],

        ##Full scan
        ['mu20_mu8noL1',          'L1_MU20', ['L1_MU20',''], [globalStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','mu8noL1']]],        
        ['mu20_iloose_mu8noL1',   'L1_MU20', ['L1_MU20',''], [globalStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_iloose','mu8noL1']]],
        ['mu20_imedium_mu8noL1',  'L1_MU20', ['L1_MU20',''], [globalStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_imedium','mu8noL1']]],
        ['mu20_2mu4noL1',         'L1_MU20', ['L1_MU20',''], [globalStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','2mu4noL1']]],
        ['mu20_iloose_2mu4noL1',  'L1_MU20', ['L1_MU20',''], [globalStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_iloose','2mu4noL1']]],
        ['mu20_imedium_2mu4noL1', 'L1_MU20', ['L1_MU20',''], [globalStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_imedium','2mu4noL1']]],
        ['mu24_mu8noL1',          'L1_MU20', ['L1_MU20',''], [globalStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','mu8noL1']]],        
        ['mu24_2mu4noL1',         'L1_MU20', ['L1_MU20',''], [globalStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','2mu4noL1']]],
        

        ]


    TriggerFlags.JetSlice.signatures = [
        #Single jet chains        
        ['j400',                    'L1_J100',[],  [globalStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j460_a10_sub_L1J100',     'L1_J100',[], [globalStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j200_320eta490',          'L1_J100.32ETA49',[], [globalStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j60',                     'L1_J20',[], [globalStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j110',                    'L1_J40',[], [globalStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j150',                    'L1_J75',[], [globalStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j200',                    'L1_J100',[], [globalStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j260',                    'L1_J100',[], [globalStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j330',                    'L1_J100',[], [globalStream], ['RATE:SingleJet', 'BW:Jets'], -1],        
        #Multjet chains
        ['3j175',                   'L1_J100',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['4j100',                   'L1_3J20',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1],        
        ['5j85',                    'L1_4J20',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        #HT chains
        ['j200_ht1000',         'L1_J100', [], [globalStream], ['RATE:SingleJet', 'BW:Jets'], -1, ['serial',-1,['j200','ht1000']]],
        #['ht1000_L1J100',      'L1_J100', [], [globalStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        #
        ['j30_muvtx',             'L1_2MU10', [], [globalStream], ['RATE:SingleMuon','BW:Muon'], -1],

        #Partial scan
        ['j200_PS',  'L1_J100', [], [globalStream], ['RATE:SingleJet', 'BW:Jets'], -1],         
        ['3j175_PS',  'L1_J100', [], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1], 

        ]

    TriggerFlags.BjetSlice.signatures =  [
        ['j55_bperf',                        'L1_J20',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j45_bperf_3j45',                   'L1_3J15',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j45_bperf_3j45_L13J15.ETA24',      'L1_3J15.ETA24',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j45_bperf_3j45_L13J20',            'L1_3J20',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        #['j55_EFID_bperf',              ,    'L1_J20',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j175_bmedium',                     'L1_J100',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1],# <- J100 
        ['j75_bmedium_3j75',                 'L1_4J20',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1], #<- 4J20 
        ['2j55_bmedium_2j55',                'L1_4J20',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1],# <- 4J20 
        ['2j45_bmedium_3j45',                'L1_5J15.ETA24',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j175_bmedium_j60_bmedium',         'L1_J100',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j300_bloose',                      'L1_J100',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        
        # chains with EDIF
        ['j55_bperf_EFID',                   'L1_J20',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j45_bperf_EFID_3j45',              'L1_3J15',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j45_bperf_EFID_3j45_L13J15.ETA24',    'L1_3J15.ETA24',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j45_bperf_EFID_3j45_L13J20',          'L1_3J20',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j175_bmedium_EFID',                   'L1_J100',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1],# <- J100 
        ['j75_bmedium_EFID_3j75',               'L1_4J20',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1], #<- 4J20 
        ['2j55_bmedium_EFID_2j55',              'L1_4J20',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1],# <- 4J20 
        ['2j45_bmedium_EFID_3j45',              'L1_5J15.ETA24',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j175_bmedium_EFID_j60_bmedium_EFID',     'L1_J100',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1],
        ['j300_bloose_EFID',                       'L1_J100',[], [globalStream], ['RATE:MultiJet', 'BW:Jets'], -1],    

        ]

    TriggerFlags.METSlice.signatures = [       
        #['xe50noL1_l2fsperf',                    '',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],
        #['xe50noL1_l2fsperf_',                    '',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],        
        ['xe50',                                'L1_XE35',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],
        ['xe50_cell',                           'L1_XE35',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],        
        ####--- Run L2 algos
        ['xe50_l2fsperf',                       'L1_XE35',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],
        ['xe50_l2fsperf_cell',                  'L1_XE35',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],        
        ###Muon corrections to FEB MET-use L2_mu8
        ['xe50_l2fsperf_wEFMuFEB',              'L1_XE35',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],
        ['xe50_l2fsperf_wEFMuFEB_cell',         'L1_XE35',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],                
        ###Muon correction to FEB and to EF MET -use EF_mu8
        ['xe50_l2fsperf_wEFMuFEB_wEFMu',         'L1_XE35',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],
        ['xe50_l2fsperf_wEFMuFEB_wEFMu_cell',    'L1_XE35',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],

        ##Other thresolds:
        ['xe70',                                'L1_XE50',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],
        ['xe70_cell',                           'L1_XE50',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],    
        ####--- Run L2 algos
        ['xe70_l2fsperf',                       'L1_XE50',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],    
        ['xe70_l2fsperf_cell',                  'L1_XE50',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],        
        ###Muon corrections to FEB MET-use L2_mu8
        ['xe70_l2fsperf_wEFMuFEB',             'L1_XE50',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],
        ['xe70_l2fsperf_wEFMuFEB_cell',        'L1_XE50',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],                
        ###Muon correction to FEB and to EF MET -use EF_mu8
        ['xe70_l2fsperf_wEFMuFEB_wEFMu',         'L1_XE50',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],
        ['xe70_l2fsperf_wEFMuFEB_wEFMu_cell',    'L1_XE50',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],
        
        ['xe100',                               'L1_XE70',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],
        ['xe100_cell',                          'L1_XE70',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],        
        ['xe100_l2fsperf',                      'L1_XE70',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],

        
        ###PU suppression####
        ['xe50_pueta',                              'L1_XE35',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],
        #--- Run L2 algos
        ['xe50_l2fsperf_pueta',                     'L1_XE35',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],
        ##Muon corrections to FEB MET-use EF_mu8
        ['xe50_l2fsperf_wEFMuFEB_pueta',             'L1_XE35',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],
        
        ###Muon correction to FEB and to EF MET -use EF_mu8
        ['xe50_l2fsperf_wEFMuFEB_wEFMu_pueta',       'L1_XE35',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],

        ##Other thresolds:
        ['xe70_pueta',                              'L1_XE50',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],
        ['xe70_l2fsperf_pueta',                     'L1_XE50',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],    
        
        ['xe100_pueta',                             'L1_XE70',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],
        ['xe100_l2fsperf_pueta',                    'L1_XE70',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],

        ##Rerun chains
        ['xe0noL1_l2fsperf',                        '',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_cell',                   '',[], [globalStream], ['Rate:MET', 'BW:MET'], -1],


        ]

    TriggerFlags.TauSlice.signatures = [
        ['tau20_r1medium1',                       'L1_TAU12', [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau20_r1perf',                          'L1_TAU12', [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau20_r1_idperf',                       'L1_TAU12', [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],

        ['tau25_medium1_calo',                    'L1_TAU12', [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_track',                   'L1_TAU12', [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_ptonly',                  'L1_TAU12', [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_caloonly',                'L1_TAU12', [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_trackonly',               'L1_TAU12', [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_mvonly',                  'L1_TAU12', [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_r1perf',                          'L1_TAU12', [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_r1perf_L1TAU6',                   'L1_TAU6',  [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_mvonly_L1TAU6',           'L1_TAU6',  [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf',                            'L1_TAU12', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_r1perf_L1TAU8',                   'L1_TAU8',  [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_mvonly_L1TAU8',           'L1_TAU8',  [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_L1TAU6',                     'L1_TAU6', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_L1TAU8',                     'L1_TAU8', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_ptonly_idperf',                   'L1_TAU12', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], -1],

        ['tau29_r1medium1',                       'L1_TAU20', [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau29_r1perf',                          'L1_TAU20', [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_calo',                    'L1_TAU20', [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_track',                   'L1_TAU20', [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_ptonly',                  'L1_TAU20', [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_r1perf',                          'L1_TAU20', [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_perf',                            'L1_TAU20', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], -1],

        ['tau80_medium1_calo',                    'L1_TAU40', [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_medium1_track',                   'L1_TAU40', [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_r1medium1',                      'L1_TAU60', [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_medium1_calo',                   'L1_TAU60', [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_medium1_track',                  'L1_TAU60', [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_r1perf',                         'L1_TAU60', [], [globalStream], ['RATE:SingleTau', 'BW:Tau'], -1],

        ['tau125_perf',                           'L1_TAU60', [], ['Tau'], ['RATE:SingleTau', 'BW:Tau'], -1],


        ]

    TriggerFlags.EgammaSlice.signatures = [
       # chainname: [chaincounter, stream, group, bw, prescale, pass through, rerun (to be set to 0 for all), eBstep]
        # single electron chains
        # Et cut only chains
        ['e5_etcut',                   'L1_EM3', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        #['e15_etcut',                  'L1_EM12', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_etcut_L1EM7V',             'L1_EM7V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e25_etcut',                  'L1_EM20V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e35_etcut',                  'L1_EM20V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['e45_etcut',                  'L1_EM20V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['e55_etcut',                  'L1_EM50', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['e65_etcut',                  'L1_EM50', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['e80_etcut',                  'L1_EM50', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['e100_etcut',                 'L1_EM50', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['e120_etcut',                 'L1_EM50', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 

        # Cut-based Run1 PID chains
        # Loose chains with etcut variations
        ['e5_loose1',                   'L1_EM3', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        #['e15_loose1',                  'L1_EM12', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_loose1_L1EM7V',             'L1_EM7V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e25_loose1',                  'L1_EM20V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e35_loose1',                  'L1_EM20V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e45_loose1',                  'L1_EM20V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e55_loose1',                  'L1_EM50', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e65_loose1',                  'L1_EM50', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e80_loose1',                  'L1_EM50', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e100_loose1',                 'L1_EM50', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], #needed for exotics background estimation --> needs lower prescale than the rest
        ['e120_loose1',                 'L1_EM50', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        
        # Single electron Run1 PID
        ['e24_medium1_iloose',        'L1_EM18VH', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['e24_loose1_L1EM20V',        'L1_EM20V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['e24_medium1_L1EM20V',       'L1_EM20V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['e24_tight1_L1EM20V',        'L1_EM20V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['e24_loose1',                'L1_EM18VH', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        #['e24_loose1_L1EM18VHI',      'L1_EM18VHI', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        #['e24_loose1_L1EM18VHIrel',   'L1_EM18VHIrel', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e28_tight1_iloose',          'L1_EM24VHI', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        #['e28_tight1_iloose_L1EM22VHI',   'L1_EM22VHI', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        #['e28_tight1_iloose_L1EM20VHI',   'L1_EM20VHI', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        #['e26_tight1_iloose',         'L1_EM22VHI', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        #['e24_tight1_iloose_L1EM20VHI',     'L1_EM20VHI', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e60_medium1',             'L1_EM50', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['e60_loose1',              'L1_EM50', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 

        # Rerun mode triggers to support Run1 PID chains
        ['e18_loose1',                   'L1_EM15VH', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e17_loose1',                   'L1_EM15VH', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e12_loose1_L1EM3',             'L1_EM3', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        #['e9_loose1',                    'L1_EM6', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e9_loose1_L1EM7V',               'L1_EM7V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e7_medium1',                   'L1_EM3', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e12_medium1',                  'L1_EM8VH', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e17_medium1',                  'L1_EM15VH', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e5_tight1',                    'L1_EM3', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        #['e9_tight1',                    'L1_EM6', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        #['e14_tight1',                   'L1_EM12', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e9_tight1_L1EM3',              'L1_EM3', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e14_tight1_L1EM7V',            'L1_EM7V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e4_etcut',                     'L1_EM3', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        #['e9_etcut',                     'L1_EM6', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        #['e14_etcut',                    'L1_EM12', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e9_etcut_L1EM3',               'L1_EM3', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e14_etcut_L1EM7V',             'L1_EM7V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e28_loose1_iloose',            'L1_EM24VHI', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

        # Tracking tune chains for Run1 PID
        ['e28_tight1_iloose_L2StarA',        'L1_EM24VHI', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['e28_tight1_iloose_L2StarB',        'L1_EM24VHI', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['e28_tight1_iloose_L2StarC',        'L1_EM24VHI', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['e28_tight1_L2Star_idperf',         'L1_EM24VHI', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e28_tight1_idperf',                'L1_EM24VHI', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        
        # low-ET single electron triggers in rerun mode
        ['e5_lhtight',  'L1_EM3', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e5_lhtight_idperf',  'L1_EM3', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e5_lhtight_L2Star_idperf', 'L1_EM3', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e5_tight1_idperf',  'L1_EM3', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e5_tight1_L2Star_idperf', 'L1_EM3', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 

        # Rerun mode triggers to support LH PID chains
        ['e18_lhloose',                  'L1_EM15VH', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e17_lhloose',                  'L1_EM15VH', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e12_lhloose_L1EM3',            'L1_EM3', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        #['e9_lhloose',                  'L1_EM6', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e9_lhloose_L1EM7V',            'L1_EM7V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e28_lhloose_iloose',           'L1_EM24VHI', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        
        # Single electron Run2 cut-based PID
        ['e24_medium_iloose',        'L1_EM18VH', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], #In Physics Menu 
        ['e24_loose_L1EM20V',        'L1_EM20V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['e24_medium_L1EM20V',       'L1_EM20V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['e24_tight_L1EM20V',        'L1_EM20V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['e28_tight_iloose',         'L1_EM24VHI', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], #Run2 baseline 
        ['e28_tight_iloose_L2StarA', 'L1_EM24VHI', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e60_medium',               'L1_EM50', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        
        # /single electron LH ID chains
        ['e24_lhmedium_iloose',        'L1_EM18VH', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['e24_lhloose_L1EM20V',        'L1_EM20V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], #L1_EM20V
        ['e24_lhmedium_L1EM20V',       'L1_EM20V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], #L1_EM20V
        ['e24_lhtight_L1EM20V',        'L1_EM20V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], #L1_EM20V
        ['e28_lhtight_iloose',         'L1_EM24VHI', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], #L1_EM18VH
        ['e28_lhtight_iloose_L2StarA', 'L1_EM24VHI', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], #L1_EM18VH
        ['e28_lhtight_iloose_L2StarB', 'L1_EM24VHI', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e28_lhtight_iloose_L2StarC', 'L1_EM24VHI', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e28_lhtight_L2Star_idperf',  'L1_EM24VHI', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e28_lhtight_idperf',         'L1_EM24VHI', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e60_lhmedium',               'L1_EM50', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], #L1_EM50
        
        # multi-electron chains
        # Run1 PID       
        ['2e12_loose1',              'L1_2EM8VH', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        ['2e17_loose1',              'L1_2EM15VH', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        ['2e17_loose1_L12EM15V',     'L1_2EM15V', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        ['e28_loose1_iloose_e12_loose1_L1EM3',  'L1_EM24VHI', ['L1_EM24VHI','L1_EM3'], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1], 
        ['e18_loose1_2e9_loose1_L1EM7V',        'L1_EM15VH_3EM7V', ['L1_EM15VH','L1_2EM7V'], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],                                                      

        # LH PID
        ['2e17_lhloose',  'L1_2EM15VH', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        ['e28_lhloose_iloose_e12_lhloose_L1EM3', 'L1_EM24VHI', ['L1_EM24VHI','L1_EM3'], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1], 
        ['e18_lhloose_2e9_lhloose_L1EM7V',  'L1_EM15VH_3EM7V', ['L1_EM15VH','L1_2EM7V'], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1], 

        # Multielectron chains with m_ee cut for Run1 PID
        ['e5_tight1_e4_etcut',              'L1_2EM3', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        ['e5_tight1_e4_etcut_Jpsiee',       'L1_2EM3', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        ['e5_tight1_L2StarA_e4_etcut_L2StarA_Jpsiee',      'L1_2EM3', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        ['e5_tight1_L2StarB_e4_etcut_L2StarB_Jpsiee',      'L1_2EM3', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        ['e5_tight1_L2StarC_e4_etcut_L2StarC_Jpsiee',      'L1_2EM3', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        ['e5_tight1_L2Star_idperf_e4_etcut_L2Star_idperf_Jpsiee',    'L1_2EM3', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        ['e5_tight1_idperf_e4_etcut_idperf_Jpsiee',       'L1_2EM3', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        #['e9_tight1_e4_etcut_Jpsiee',      'L1_2EM3_EM6', ['L1_EM6','L1_2EM3'], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        #['e5_tight1_e9_etcut_Jpsiee',      'L1_2EM3_EM6', ['L1_EM3','L1_EM6'], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        #['e14_tight1_e4_etcut_Jpsiee',      'L1_2EM3_EM12', ['L1_EM12','L1_2EM3'], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        #['e5_tight1_e14_etcut_Jpsiee',      'L1_2EM3_EM12', ['L1_EM12','L1_2EM3'], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        ['e9_tight1_L1EM3_e4_etcut_Jpsiee',        'L1_2EM3', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        ['e9_etcut_L1EM3_e5_tight1_Jpsiee',        'L1_2EM3', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1], 
        ['e14_tight1_L1EM7V_e4_etcut_Jpsiee',      'L1_EM7V_2EM3', ['L1_EM7V','L1_EM3'], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        ['e14_etcut_L1EM7V_e5_tight1_Jpsiee',      'L1_EM7V_2EM3', ['L1_EM7V','L1_EM3'], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1], 


        # Multielectron chains with m_ee cut for Run1 PID
        ['e5_lhtight_e4_etcut_Jpsiee',                      'L1_2EM3', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        ['e5_lhtight_L2StarA_e4_etcut_L2StarA_Jpsiee',      'L1_2EM3', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        ['e5_lhtight_L2StarB_e4_etcut_L2StarB_Jpsiee',      'L1_2EM3', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        ['e5_lhtight_L2StarC_e4_etcut_L2StarC_Jpsiee',      'L1_2EM3', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        ['e5_lhtight_L2Star_idperf_e4_etcut_L2Star_idperf_Jpsiee',       'L1_2EM3', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        ['e5_lhtight_idperf_e4_etcut_idperf_Jpsiee',        'L1_2EM3', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],

        # Et cut only photon chains
        #['g12_etcut',                       'L1_EM7V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['g20_etcut_L1EM15V',                'L1_EM15V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        #['g25_etcut',                       'L1_EM20V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        #['g35_etcut',                       'L1_EM20V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        #['g40_etcut',                       'L1_EM20V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        #['g50_etcut',                       'L1_EM20V', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        #['g60_etcut',                       'L1_EM50', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        #['g80_etcut',                       'L1_EM50', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        #['g100_etcut',                      'L1_EM50', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        #['g120_etcut',                      'L1_EM50', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],


        # Photon cut-based Run1 PID chains
        # Loose chains for low ET physics and background estimation
        ['g12_loose1_L1EM7V',             'L1_EM7V', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g20_loose1_L1EM15V',            'L1_EM15V', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g25_loose1_L1EM15V',            'L1_EM15V', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g35_loose1_L1EM15V',            'L1_EM15V', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g40_loose1',                    'L1_EM20V', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g50_loose1',                    'L1_EM20V', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g60_loose1',                    'L1_EM50', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],### needed also in rerun mode!!!!
        ['g80_loose1',                    'L1_EM50', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g100_loose1',                   'L1_EM50', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        
        # Run 2 Baseline chains
        ['g120_loose1',             'L1_EM50', [], [globalStream], ['RATE:SinglePhoton','BW:Egamma'], -1],
        ['g140_loose1',             'L1_EM50', [], [globalStream], ['RATE:SinglePhoton','BW:Egamma'], -1],

        # multi-photon chains Run-1 PID
        ['g35_loose1_g25_loose1',           'L1_2EM15VH', [], [globalStream], ['RATE:MultiPhoton','BW:Egamma'], -1],
        ['g35_medium1_g25_medium1',         'L1_2EM15VH', [], [globalStream], ['RATE:MultiPhoton','BW:Egamma'], -1],
        ['g35_medium1_g25_medium1_L12EM15V','L1_2EM15V',  [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1], 
        ['2g20_tight1',                     'L1_2EM15VH', [], [globalStream], ['RATE:MultiPhoton','BW:Egamma'], -1],
        ['3g20_loose1',                     'L1_2EM15VH', [], [globalStream], ['RATE:MultiPhoton','BW:Egamma'], -1],

        # Rerun mode trigger to support combined chains
        ['g4_etcut',                  'L1_EM3', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g8_etcut',                  'L1_EM3', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g8_etcut_L1EM7V',           'L1_EM7V', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        #['g9_etcut',                  'L1_EM6', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        #['g14_etcut',                 'L1_EM12', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        #['g9_etcut_L1EM3',            'L1_EM3', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        #['g14_etcut_L1EM7V',          'L1_EM7V', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g25_loose1',                'L1_EM15VH', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g25_medium1_L1EM3',         'L1_EM3', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g35_loose1',                'L1_EM15VH', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g35_medium1',               'L1_EM15VH', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g12_loose1',                'L1_EM8VH', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g20_loose1',                'L1_EM15VH', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g20_tight1',                'L1_EM15VH', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

        # e/g Combined + mass cut Chains ### should these become instead of 'Egamma' --> 'Combined' ???
        # J/psi
        ###['e5_tight1_g4_etcut',           'L1_2EM3', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],### added to combined chains
        #['e5_tight1_g4_etcut_Jpsiee',      'L1_2EM3', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        #['e9_tight1_g4_etcut_Jpsiee',      'L1_2EM3_EM6', ['L1_EM6','L1_2EM3'], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        #['e5_tight1_g9_etcut_Jpsiee',      'L1_EM3_EM6', ['L1_EM6','L1_2EM3'], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        #['e14_tight1_g4_etcut_Jpsiee',     'L1_2EM3_EM12', ['L1_EM12','L1_2EM3'], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        #['e5_tight1_g14_etcut_Jpsiee',     'L1_2EM3_EM12', ['L1_EM12','L1_2EM3'], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        
        # Zee ### should these become instead of 'Egamma' --> 'Combined' ???
        #['e28_tight1_iloose_g8_etcut_Zeg', 'L1_EM24VHI', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],
        ###['e28_tight1_iloose_g8_etcut', 'L1_EM24VHI', [], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],### added to combined chains
        ###['e17_loose1_2g8_etcut',  'L1_EM15VH_3EM7V', ['L1_EM15VH','L1_3EM7V'], [globalStream], ['RATE:MultiElectron', 'BW:Egamma'], -1],### added to combined chains

        # Fall-back if problem with new tracking
        ['e17_etcut',                      'L1_EM15VH', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e17_etcut_L2StarA',              'L1_EM15VH', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e17_etcut_L2StarB',              'L1_EM15VH', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e17_etcut_L2StarC',              'L1_EM15VH', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e17_etcut_L2Star_idperf',        'L1_EM15VH', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e17_etcut_idperf',               'L1_EM15VH', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        
        # perf chains to study L2/EF options ### need development in ElectronDef.py !!!
        ['e0_perf_L1EM15VH',  'L1_EM15VH', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e0_L2Star_perf_L1EM15VH',  'L1_EM15VH', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['g0_perf_L1EM15VH', 'L1_EM15VH', [], [globalStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 

        # extra id perf chains
        ['e24_medium1_L2Star_idperf', 'L1_EM18VH', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'],1],
        ['e5_loose1_L2Star_idperf',  'L1_EM3', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'],1],
        ['e24_medium1_idperf',  'L1_EM18VH', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'],1],
        ['e5_loose1_idperf', 'L1_EM3', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'],1],
        ['e24_medium1_iloose_L2StarA',  'L1_EM18VH', [], [globalStream], ['RATE:SingleElectron', 'BW:Egamma'],1],
                                                           
        # technical chain for forward electrons ### please check!!!! should this become instead of 'Egamma' --> 'Combined' ???
        ['e17_loose1_L1EM15VHJ1523ETA49',         'L1_EM15VH_J15.23ETA49', ['L1_EM15VH',''], [globalStream], ['RATE:Combined', 'BW:Egamma'], -1],### waiting for L1 fixes
        ]


    TriggerFlags.BphysicsSlice.signatures = [
        #L1_3MU6 seeded
        ['3mu6_bJpsi',                'L1_3MU6', [], [globalStream], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1],
        ['3mu6_bTau',                 'L1_3MU6', [], [globalStream], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1],
        ['3mu6_bDimu',                'L1_3MU6', [], [globalStream], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1],

        #L1Topo seeded  # temporary changed L1_2MU6_02DR-MU6-MU6 into L1_2MU6 until L1Topo is available. (#104272)
        ['2mu6_bBmumu',                'L1_2MU6', [], [globalStream], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1],
        ['2mu6_bBmumuxv2',             'L1_2MU6', [], [globalStream], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1],
        ['2mu6_bBmumux_BcmumuDsloose', 'L1_2MU6', [], [globalStream], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1],
        ['2mu6_bJpsimumu',             'L1_2MU6', [], [globalStream], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1],
        ['2mu6_bUpsimumu',             'L1_2MU6', [], [globalStream], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1],

        #L1_2MU10 seeded
        ['2mu10_bBmumu',                'L1_2MU10', [], [globalStream], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1],
        ['2mu10_bBmumuxv2',             'L1_2MU10', [], [globalStream], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1],
        ['2mu10_bBmumux_BcmumuDsloose', 'L1_2MU10', [], [globalStream], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1],
        ['2mu10_bJpsimumu',             'L1_2MU10', [], [globalStream], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1],
        ['2mu10_bUpsimumu',             'L1_2MU10', [], [globalStream], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1],
        ['mu13_mu13_idperf_Zmumu',      'L1_2MU10', [], [globalStream], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1],

        #L1_2MU4 seeded
        ['mu4_iloose_mu4_7invm9_noos',  'L1_2MU4', [], [globalStream], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1],
        ['mu4_mu4_idperf_bJpsimumu_noid',          'L1_2MU4', [], [globalStream], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1],

        #L1_MU6 seeded
        ['mu6_bJpsi_Trkloose',         'L1_MU6', [], [globalStream], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1],
        
        # L1_2MU6
        ['2mu6_bDimu',              'L1_2MU6', [], [globalStream], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1], 
        ['2mu6_bDimu_novtx_noos',  'L1_2MU6', [], [globalStream], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1],          
        ]

    TriggerFlags.CombinedSlice.signatures = [
        ['e17_medium1_mu12',  'L1_EM15VH_MU10', ['L1_EM15VH', 'L1_MU10'], [globalStream], ['RATE:ElectronMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],#EM24VH
        ['e7_medium1_mu24',   'L1_MU20', ['L1_EM3', 'L1_MU20'], [globalStream], ['RATE:ElectronMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],#EM24VH
        ['2e12_medium1_mu12', 'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [globalStream], ['RATE:ElectronMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],#EM24VH
        ['e12_medium1_2mu8', 'L1_EM8VH_2MU6', ['L1_EM8VH', 'L1_2MU6'], [globalStream], ['RATE:ElectronMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],#EM24VH 

        ['g25_medium1_L1EM3_mu24',       'L1_MU20', ['L1_EM3','L1_MU20'], [globalStream], ['RATE:ElectronMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],
        ['g12_loose1_2mu10_msonly',      'L1_EM8VH_2MU6', ['L1_EM8VH', 'L1_2MU6'], [globalStream], ['RATE:ElectronMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],

        ['2g20_loose1_mu20noL1',       'L1_2EM15VH', ['L1_2EM15VH', ''], [globalStream], ['RATE:ElectronMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['2g20_loose1','mu20noL1'] ]], 
        ['e28_medium1_mu8noL1',    'L1_EM24VHI', ['L1_EM24VHI', ''], [globalStream], ['RATE:ElectronMuon', 'BW:Egamma'], -1, ['serial',-1,['e28_medium1','mu8noL1'] ]],       

        ['e5_tight1_g4_etcut',         'L1_2EM3', [], [globalStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1, ['parallel',-1,[] ]],
        ['e28_medium1_g8_etcut',  'L1_EM24VHI', ['L1_EM24VHI','L1_EM3'], [globalStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1, ['parallel',-1,[] ]],### CHECK L1 Config, if correct L1 item needs to be added
        ['e17_loose1_2g8_etcut_L12EM7V',  'L1_EM15VH_3EM7V',['L1_EM15VH','L1_2EM7V'], [globalStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ['e24_medium1_g25_medium1',       'L1_2EM15VH',[], [globalStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ['e20_medium1_g35_loose1',        'L1_2EM15VH',[], [globalStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ['e20_medium1_2g20_loose1',       'L1_2EM15VH',[], [globalStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],

        

        #MET+X
        ['mu24_xe100noL1_wEFMu',        'L1_MU20',['L1_MU20',''], [globalStream], ['RATE:METMuon', 'BW:Muon'], -1,['serial',-1,["mu24","xe100noL1_wEFMu"]]],
        ['e28_medium1_xe100noL1',       'L1_EM24VHI',['L1_EM24VHI',''], [globalStream], ['RATE:METElectron', 'BW:Egamma'], -1,['serial',-1,["e28_medium1","xe100noL1"]]], 
        ['g60_loose1_xe60noL1',         'L1_EM50',['L1_EM50',''], [globalStream], ['RATE:METElectron', 'BW:Egamma'], -1,['serial',-1,["g60_loose1","xe60noL1"]]],        
        

        ['j100_xe80',               'L1_J40_XE50',['',''], [globalStream], ['RATE:JetMET', 'BW:Jets'], 1, ['serial',-1, ["j100","xe80"]]], 
        ['j80_xe80',                'L1_J40_XE50',['',''], [globalStream], ['RATE:JetMET', 'BW:Jets'], 1, ['serial',-1, ["j80","xe80"]]], 
        ['j80_xe80_dphi1',          'L1_J40_XE50',['',''], [globalStream], ['RATE:JetMET', 'BW:Jets'], 1, ['serial',-1, ["j80","xe80"]]], 


        # TAU+X
        ['tau35_medium1_calo_tau25_medium1_calo',         'L1_TAU20_2TAU12_J25_2J20_3J12',['L1_TAU20','L1_TAU12'], [globalStream], ['RATE:Tau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_calo","tau25_medium1_calo"]]],
        ['e18_loose1_tau25_medium1_calo',                 'L1_EM15VH_2TAU12_J25_2J15_3J12',['L1_EM15VH','L1_TAU12'], [globalStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e18_loose1","tau25_medium1_calo"]]],
        ['e18_lhloose_tau25_medium1_calo',                'L1_EM15VH_2TAU12_J25_2J15_3J12',['L1_EM15VH','L1_TAU12'], [globalStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e18_lhloose","tau25_medium1_calo"]]],
        ['e18_loose1_tau80_medium1_calo',                 'L1_EM15VH_TAU40_2TAU15',['L1_EM15VH','L1_TAU40'], [globalStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e18_loose1","tau80_medium1_calo"]]],
        ['e18_lhloose_tau80_medium1_calo',                'L1_EM15VH_TAU40_2TAU15',['L1_EM15VH','L1_TAU40'], [globalStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e18_lhloose","tau80_medium1_calo"]]],
        ['mu14_tau35_medium1_calo',                       'L1_MU10_TAU20',['L1_MU10','L1_TAU20'], [globalStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14","tau35_medium1_calo"]]],
        ['mu14_tau25_medium1_calo',                       'L1_MU10_TAU12_J25_2J12',['L1_MU10','L1_TAU12'], [globalStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14","tau25_medium1_calo"]]],
        ['tau35_medium1_calo_tau25_medium1_calo_xe50',    'L1_TAU20_2TAU12_XE35',['L1_TAU20','L1_TAU12','L1_XE35'], [globalStream], ['RATE:METTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_calo","tau25_medium1_calo","xe50"]]],
        ['tau35_medium1_calo_xe70_L1XE45',                'L1_TAU20_2J20_XE45',['L1_TAU20','L1_XE45'], [globalStream], ['RATE:METTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_calo","xe70_L1XE45"]]],
        ['tau35_medium1_ptonly_tau25_medium1_ptonly',     'L1_TAU20_2TAU12_J25_2J20_3J12',['L1_TAU20','L1_TAU12'], [globalStream], ['RATE:Tau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_ptonly","tau25_medium1_ptonly"]]],
        ['e18_loose1_tau25_medium1_ptonly',               'L1_EM15VH_2TAU12_J25_2J15_3J12',['L1_EM15VH','L1_TAU12'], [globalStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e18_loose1","tau25_medium1_ptonly"]]],
        ['e18_lhloose_tau25_medium1_ptonly',              'L1_EM15VH_2TAU12_J25_2J15_3J12',['L1_EM15VH','L1_TAU12'], [globalStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e18_lhloose","tau25_medium1_ptonly"]]],
        ['e18_loose1_tau80_medium1_ptonly',               'L1_EM15VH_TAU40_2TAU15',['L1_EM15VH','L1_TAU40'], [globalStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e18_loose1","tau80_medium1_ptonly"]]],
        ['e18_lhloose_tau80_medium1_ptonly',              'L1_EM15VH_TAU40_2TAU15',['L1_EM15VH','L1_TAU40'], [globalStream], ['RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e18_lhloose","tau80_medium1_ptonly"]]],
        ['mu14_tau35_medium1_ptonly',                     'L1_MU10_TAU20',['L1_MU10','L1_TAU20'], [globalStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14","tau35_medium1_ptonly"]]],
        ['mu14_tau25_medium1_ptonly',                     'L1_MU10_TAU12_J25_2J12',['L1_MU10','L1_TAU12'], [globalStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14","tau25_medium1_ptonly"]]],
        ['tau35_medium1_ptonly_tau25_medium1_ptonly_xe50', 'L1_TAU20_2TAU12_XE35',['L1_TAU20','L1_TAU12','L1_XE35'], [globalStream], ['RATE:METTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_ptonly","tau25_medium1_ptonly","xe50"]]],
        ['tau35_medium1_ptonly_xe70_L1XE45',              'L1_TAU20_2J20_XE45',['L1_TAU20','L1_XE45'], [globalStream], ['RATE:METTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_ptonly","xe70_L1XE45"]]],

        ]


    TriggerFlags.MinBiasSlice.signatures = [
        ['mb_sptrk',  'L1_RD0_FILLED', [], [globalStream], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_perf_L1MBTS_1',  'L1_MBTS_1', [], [globalStream],["BW:MinBias", "RATE:MinBias"] , -1],
        ['mb_idperf_L1MBTS_2',  'L1_MBTS_2', [], [globalStream], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sp2000_trk70_hmt', 'L1_TE20', [], [globalStream], ["BW:MinBias", "RATE:MinBias"], -1],        
        
        ['mb_sp2000_pusup600_trk70_hmt', 'L1_TE20', [], [globalStream], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['mb_sp2000_pusup600_trk70_hmt_L1TE30',  'L1_TE30', [], [globalStream], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['mb_sp2000_pusup600_trk70_hmt_L1TE40',  'L1_TE40', [], [globalStream], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['mb_sp2000_pusup700_trk70_hmt_L1TE30',  'L1_TE30', [], [globalStream], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['mb_sp2000_pusup700_trk70_hmt_L1TE40',  'L1_TE40', [], [globalStream], ["BW:MinBias", "RATE:MinBias"], -1], 
 
        ]
    

    ###allL1TAUseeds = ",".join([ x for x in tcl1.current.menu.items.itemNames() if "TAU" in x ])

    TriggerFlags.CalibSlice.signatures   = []

    TriggerFlags.CosmicSlice.signatures  =  []


    TriggerFlags.StreamingSlice.signatures =  [
        ['noalg_mb_L1TE20',  'L1_TE20', [], [globalStream], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['noalg_mb_L1TE30',  'L1_TE30', [], [globalStream], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['noalg_mb_L1TE40',  'L1_TE40', [], [globalStream], ["BW:MinBias", "RATE:MinBias"], -1],         

        ]

    TriggerFlags.MonitorSlice.signatures =  []
    #TriggerFlags.GenericSlice.signatures =  []


    # --- list of signatures to be prescaled out in tight mc prescales ---
    ''' PS definition using pattern-matching NOT WORKING when using with Prescales 
    import re
    
    signatureList=[]
    for prop in dir(TriggerFlags):
        if prop[-5:]=='Slice':
            sliceName=prop
            slice=getattr(TriggerFlags,sliceName)
            if slice.signatures():
                signatureList.extend(slice.signatures())
            else:
                log.debug('SKIPPING '+str(sliceName))
    myChains = []
    for sig in signatureList:
        myChains.append(sig[0])

    disablePatterns=[ re.compile('mb_sp'),                     
                      re.compile('mb_perf'),                     
                      ]

    toDisable=[]
    for signature in myChains:
        for pattern in disablePatterns:
            if re.search(pattern,signature):
                toDisable.append(signature)
                break
    log.debug('DISABLE The following chains are disabled in the DC14 tight presacle menu: '+str(toDisable))
    for signature in toDisable:
        Prescales.HLTPrescales_tight_mc_prescale[signature] = [   -1,    0,   -1]

    '''

    
class Prescales:

    # for L1 prescales 
    #   Item name             | Prescale
    #----------------------------------------------------------
    L1Prescales = { }

    # for HLT prescales
    # Signature name  :  [ HLTprescale, HLTpass-through, rerun]
    #   - Prescale values should be a positive integer (default=1)
    #   - If the current pass_through value is non-zero,
    #     the value given here will be used as pass_through rate
    #     Assuming that pass through chains are configured so
    #     in the slice files and won't change. Also prescale
    #     and pass_through will not be used together.
    #   - The default value of pass-through (=0) will be used
    #----------------------------------------------------------
    HLTPrescales = {}


Prescales.L1Prescales = dict([(ctpid,1) for ctpid in Prescales.L1Prescales])  # setting all L1 prescales to 1

Prescales.L1Prescales_loose_mc_prescale  = Prescales.L1Prescales
Prescales.HLTPrescales_loose_mc_prescale = {
    # egamma (Rerun mode triggers to support Run1 PID chains)
    'e18_loose1'            : [    0,    0,   1],
    'e17_loose1'            : [    0,    0,   1],
    'e12_loose1_L1EM3'      : [    0,    0,   1],
    'e9_loose1_L1EM7V'      : [    0,    0,   1],
    'e7_medium1'            : [    0,    0,   1],
    'e12_medium1'           : [    0,    0,   1],
    'e17_medium1'           : [    0,    0,   1],
    'e5_tight1'             : [    0,    0,   1],
    'e9_tight1_L1EM3'       : [    0,    0,   1],
    'e14_tight1_L1EM7V'     : [    0,    0,   1],
    'e4_etcut'              : [    0,    0,   1],
    'e9_etcut_L1EM3'        : [    0,    0,   1],
    'e14_etcut_L1EM7V'      : [    0,    0,   1],
    'e28_loose1_iloose'     : [    0,    0,   1],
    # egamma (low-ET single electron triggers in rerun mode)
    'e5_lhtight'                 : [    0,    0,   1],
    'e5_lhtight_idperf'          : [    0,    0,   1],
    'e5_lhtight_L2Star_idperf'   : [    0,    0,   1],
    'e5_tight1_idperf'           : [    0,    0,   1],
    'e5_tight1_L2Star_idperf'    : [    0,    0,   1],
    # egamma (Rerun mode triggers to support LH PID chains)
    'e18_lhloose'          : [    0,    0,   1],
    'e17_lhloose'          : [    0,    0,   1],
    'e12_lhloose_L1EM3'    : [    0,    0,   1],
    'e9_lhloose_L1EM7V'    : [    0,    0,   1],
    'e28_lhloose_iloose'   : [    0,    0,   1],
    # egamma (Rerun mode trigger to support combined chains)
    'g4_etcut'             : [    0,    0,   1],
    'g8_etcut'             : [    0,    0,   1],
    'g8_etcut_L1EM7V'      : [    0,    0,   1],
    'g25_loose1'           : [    0,    0,   1],
    'g25_medium1_L1EM3'    : [    0,    0,   1],
    'g35_loose1'           : [    0,    0,   1],
    'g35_medium1'          : [    0,    0,   1],
    'g12_loose1'           : [    0,    0,   1],
    'g20_loose1'           : [    0,    0,   1],
    'g20_tight1'           : [    0,    0,   1],
    # minBias prescale
    'mb_sptrk'                             : [    -1,    0,   -1],
    'mb_perf_L1MBTS_1'                     : [    -1,    0,   -1],
    'mb_idperf_L1MBTS_2'                   : [    -1,    0,   -1],
    'mb_sp2000_trk70_hmt'                  : [    -1,    0,   -1],
    'mb_sp2000_pusup600_trk70_hmt'         : [    -1,    0,   -1],
    'mb_sp2000_pusup600_trk70_hmt_L1TE30'  : [    -1,    0,   -1],
    'mb_sp2000_pusup600_trk70_hmt_L1TE40'  : [    -1,    0,   -1],
    'mb_sp2000_pusup700_trk70_hmt_L1TE30'  : [    -1,    0,   -1],
    'mb_sp2000_pusup700_trk70_hmt_L1TE40'  : [    -1,    0,   -1],
    # L1 streamers prescale
    'noalg_mb_L1TE20'                      : [    -1,    0,   -1],
    'noalg_mb_L1TE30'                      : [    -1,    0,   -1],
    'noalg_mb_L1TE40'                      : [    -1,    0,   -1],
    # MET rerun
    'xe0noL1_l2fsperf'      : [    0,    0,   1],
    'xe0noL1_l2fsperf_cell' : [    0,    0,   1],
}


Prescales.L1Prescales_tight_mc_prescale  = Prescales.L1Prescales

Prescales.HLTPrescales_tight_mc_prescale = {
    # egamma (Rerun mode triggers to support Run1 PID chains)
    'e18_loose1'            : [    0,    0,   1],
    'e17_loose1'            : [    0,    0,   1],
    'e12_loose1_L1EM3'      : [    0,    0,   1],
    'e9_loose1_L1EM7V'      : [    0,    0,   1],
    'e7_medium1'            : [    0,    0,   1],
    'e12_medium1'           : [    0,    0,   1],
    'e17_medium1'           : [    0,    0,   1],
    'e5_tight1'             : [    0,    0,   1],
    'e9_tight1_L1EM3'       : [    0,    0,   1],
    'e14_tight1_L1EM7V'     : [    0,    0,   1],
    'e4_etcut'              : [    0,    0,   1],
    'e9_etcut_L1EM3'        : [    0,    0,   1],
    'e14_etcut_L1EM7V'      : [    0,    0,   1],
    'e28_loose1_iloose'     : [    0,    0,   1],
    # egamma (low-ET single electron triggers in rerun mode)
    'e5_lhtight'                 : [    0,    0,   1],
    'e5_lhtight_idperf'          : [    0,    0,   1],
    'e5_lhtight_L2Star_idperf'   : [    0,    0,   1],
    'e5_tight1_idperf'           : [    0,    0,   1],
    'e5_tight1_L2Star_idperf'    : [    0,    0,   1],
    # egamma (Rerun mode triggers to support LH PID chains)
    'e18_lhloose'          : [    0,    0,   1],
    'e17_lhloose'          : [    0,    0,   1],
    'e12_lhloose_L1EM3'    : [    0,    0,   1],
    'e9_lhloose_L1EM7V'    : [    0,    0,   1],
    'e28_lhloose_iloose'   : [    0,    0,   1],
    # egamma (Rerun mode trigger to support combined chains)
    'g4_etcut'             : [    0,    0,   1],
    'g8_etcut'             : [    0,    0,   1],
    'g8_etcut_L1EM7V'      : [    0,    0,   1],
    'g25_loose1'           : [    0,    0,   1],
    'g25_medium1_L1EM3'    : [    0,    0,   1],
    'g35_loose1'           : [    0,    0,   1],
    'g35_medium1'          : [    0,    0,   1],
    'g12_loose1'           : [    0,    0,   1],
    'g20_loose1'           : [    0,    0,   1],
    'g20_tight1'           : [    0,    0,   1],
    # minBias prescale
    'mb_sptrk'                             : [    -1,    0,   -1],
    'mb_perf_L1MBTS_1'                     : [    -1,    0,   -1],
    'mb_idperf_L1MBTS_2'                   : [    -1,    0,   -1],
    'mb_sp2000_trk70_hmt'                  : [    -1,    0,   -1],
    'mb_sp2000_pusup600_trk70_hmt'         : [    -1,    0,   -1],
    'mb_sp2000_pusup600_trk70_hmt_L1TE30'  : [    -1,    0,   -1],
    'mb_sp2000_pusup600_trk70_hmt_L1TE40'  : [    -1,    0,   -1],
    'mb_sp2000_pusup700_trk70_hmt_L1TE30'  : [    -1,    0,   -1],
    'mb_sp2000_pusup700_trk70_hmt_L1TE40'  : [    -1,    0,   -1],
    # L1 streamers prescale
    'noalg_mb_L1TE20'                      : [    -1,    0,   -1],
    'noalg_mb_L1TE30'                      : [    -1,    0,   -1],
    'noalg_mb_L1TE40'                      : [    -1,    0,   -1],
    # MET prescale
    'xe0noL1_l2fsperf'                     : [   -1,    0,   -1],
    'xe0noL1_l2fsperf_cell'                : [   -1,    0,   -1],
    # XE35 seeded prescale
    'xe50'                               : [   -1,    0,   -1],
    'xe50_cell'                          : [   -1,    0,   -1],
    'xe50_l2fsperf'                      : [   -1,    0,   -1],
    'xe50_l2fsperf_cell'                 : [   -1,    0,   -1],
    'xe50_l2fsperf_wEFMuFEB'             : [   -1,    0,   -1],
    'xe50_l2fsperf_wEFMuFEB_cell'        : [   -1,    0,   -1],
    'xe50_l2fsperf_wEFMuFEB_wEFMu'       : [   -1,    0,   -1],
    'xe50_l2fsperf_wEFMuFEB_wEFMu_cell'  : [   -1,    0,   -1],
    # TAU6 seeded prescale
    'tau25_r1perf_L1TAU6'                : [   -1,    0,   -1],
    'tau25_medium1_mvonly_L1TAU6'        : [   -1,    0,   -1],
    # MU6 seeded rerun
    'mu6_idperf'         : [   0,    0,   1],
    'mu6_L2Star_idperf'  : [   0,    0,   1],
    }

#StreamConfig = { }

