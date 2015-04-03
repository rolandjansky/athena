# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------phy--------------------------------------------------------------#
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

    PhysicsStream="Main"

    #---------------------------------------------------------------------
    # INPUT FORMAT FOR CHAINS:
    # ['chainName',  'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep], OPTIONAL: [mergingStrategy, offset,[merginOrder] ]], topoStartsFrom = False
    #---------------------------------------------------------------------

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.TestSlice.signatures = [
        ]


    TriggerFlags.MuonSlice.signatures = [ 
        ## ADD MU*_EMPTY TO L1 MENU
        ['mu4_cosmic_L1MU4_EMPTY',               'L1_MU4_EMPTY', [],   ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],
        ['mu4_cosmic_L1MU11_EMPTY',              'L1_MU11_EMPTY', [],  ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],
        ['mu4_msonly_cosmic_L1MU4_EMPTY',        'L1_MU4_EMPTY', [],   ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],
        ['mu4_msonly_cosmic_L1MU11_EMPTY',       'L1_MU11_EMPTY', [],  ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],
                            
        ['mu4_cosmicEF_L1MU4_EMPTY',             'L1_MU4_EMPTY', [], ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],
        ['mu4_cosmicEF_L1MU11_EMPTY',            'L1_MU11_EMPTY', [],['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],
        ['mu4_msonly_cosmicEF_L1MU11_EMPTY',     'L1_MU11_EMPTY', [],['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],
        ['mu4_msonly_cosmicEF_L1MU4_EMPTY',      'L1_MU4_EMPTY', [],  ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],
        
        
        ##Streaming name?
        #['mu4_cosmicEF_ds1_L1MU4', 'L1_MU4', [], ['DataScouting_01_CosmicMuons'], ["RATE:Cosmic_Muon_DS", "BW:Muon"], -1],
        #['mu4_cosmicEF_ds2_L1MU4', 'L1_MU4', [], ['DataScouting_01_CosmicMuons','DataScouting_02_CosmicMuons'], ["RATE:Cosmic_Muon_DS", "BW:Muon"], -1],
        
        ['mu0_muoncalib', 'L1_MU4',[],['Muon_Calibration'],["RATE:Calibration","BW:Muon"],-1],
        #['mu0_muoncalib_ds3', 'L1_MU4',[],['DataScouting_03_CosmicMuons'],["RATE:SingleMuon","BW:Muon"],-1],        

        ]
    
    TriggerFlags.JetSlice.signatures = [   
        # Performance chains
        ['j0_perf_ds1_L1All',     'L1_All', [], ['DataScouting_05_Jets'], ['RATE:Cosmic_Jets_DS', 'BW:Jets'], -1],
        ['j0_perf_L1RD0_EMPTY',      'L1_RD0_EMPTY',[], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j0_perf_L1MU10',           'L1_MU10',[], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j0_perf_L1J12',        'L1_J12',[], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['ht0_perf_L1J12',       'L1_J12',[], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        ['j0_L1J12',               'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j0_jes_L1J12',           'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j0_lcw_jes_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j0_sub_L1J12',           'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j0_nojcalib_L1J12',      'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j0_jes_PS_L1J12',        'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        
        ['j0_a10_nojcalib_L1J12',  'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j0_a10_lcw_sub_L1J12',   'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j0_a10r_lcw_sub_L1J12',  'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        ['ht0_L1J12',              'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        ['j0_a10_nojcalib_L1RD0_EMPTY', 'L1_RD0_EMPTY', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j0_a10r_lcw_sub_L1RD0_EMPTY', 'L1_RD0_EMPTY', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        ['j15_L1J12',              'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        ]

    TriggerFlags.BjetSlice.signatures = [
        ['j0_perf_bperf_L1RD0_EMPTY',  'L1_RD0_EMPTY',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j0_perf_bperf_L1MU10',       'L1_MU10',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],
        ['j0_perf_bperf_L1J12',        'L1_J12',[], [PhysicsStream], ['RATE:SingleBJet', 'BW:Jets'], -1],

        ]
    
    TriggerFlags.METSlice.signatures = [
        ['xe0_L1All',                              'L1_All' ,[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0_L1XE35',                             'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0_tc_lcw_L1All',                       'L1_All' ,[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0_tc_lcw_L1XE35',                      'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0_tc_em_L1All',                        'L1_All' ,[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0_tc_em_L1XE35',                       'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0_tc_pueta_L1All',                     'L1_All' ,[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0_tc_pueta_L1XE35',                    'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0_tc_pufit_L1All',                     'L1_All' ,[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0_tc_pufit_L1XE35',                    'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0_tc_mht_L1All',                       'L1_All' ,[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0_tc_mht_L1XE35',                      'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0_L2FS_L1All',                         'L1_All' ,[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0_L2FS_L1XE35',                        'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0_l2fsperf_wEFMuFEB_wEFMu_L1All',      'L1_All' ,[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe0_l2fsperf_wEFMuFEB_wEFMu_L1XE35',     'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['xe35',                                   'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_tc_lcw',                            'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_tc_em',                             'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_pueta',                             'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_pufit',                             'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_mht',                               'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_L2FS',                              'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe35_l2fsperf_wEFMuFEB_wEFMu',           'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ['te20',                                   'L1_TE20',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['te20_tc_lcw',                            'L1_TE20',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        
        ['xs30',                                   'L1_XS30',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xs30_tc_lcw',                            'L1_XS30',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        ]
    
    TriggerFlags.TauSlice.signatures = [
        ['tau8_cosmic_track', 'L1_TAU8_EMPTY', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau8_cosmic_ptonly', 'L1_TAU8_EMPTY', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau1_cosmic_track_L1MU4_EMPTY',  'L1_MU4_EMPTY', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau1_cosmic_ptonly_L1MU4_EMPTY', 'L1_MU4_EMPTY', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ]

    TriggerFlags.EgammaSlice.signatures = [
        ### NOT IN THE LIST ['e5_loose1',                        'L1_EM3', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        #['e5_etcut_L1EM3_EMPTY',              'L1_EM3_EMPTY', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        #['g5_etcut',                          'L1_EM3', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        #['g5_etcut_L1EM3_EMPTY',              'L1_EM3_EMPTY', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 

        ['g0_perf_L1EM3HI', 'L1_EM3HI', [], [PhysicsStream], ['RATE:SinglePhoton',   'BW:Egamma'], -1],
        ['e0_perf_L1EM3HI', 'L1_EM3HI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],        
        
        ['e3_loose',             'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e3_lhloose',           'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e3_etcut',             'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['e3_etcut_L1EM3_EMPTY', 'L1_EM3_EMPTY', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['g3_etcut',             'L1_EM3',       [], [PhysicsStream], ['RATE:SinglePhoton',   'BW:Egamma'], -1], 
        ['g3_etcut_L1EM3_EMPTY', 'L1_EM3_EMPTY', [], [PhysicsStream], ['RATE:SinglePhoton',   'BW:Egamma'], -1], 
        ['e0_perf_L1EM3',        'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e0_L2Star_perf_L1EM3', 'L1_EM3',       [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['g0_perf_L1EM3',        'L1_EM3',       [], [PhysicsStream], ['RATE:SinglePhoton',   'BW:Egamma'], -1],

        #Single electron/photon chains for Calo sequence optimization (ATR-10336)
        ['e3_loose_L2EFCalo', 'L1_EM3', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e3_lhloose_L2EFCalo', 'L1_EM3', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e3_loose_HLTCalo', 'L1_EM3', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e3_lhloose_HLTCalo', 'L1_EM3', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e3_loose_etisem', 'L1_EM3', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g3_loose_HLTCalo', 'L1_EM3', [], [PhysicsStream], ['RATE:SinglePhoton','BW:Egamma'],-1],


        ##Calibration chains
        ['g3_loose_larpeb',                     'L1_EM3',[], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g40_loose_larpeb',                    'L1_EM20VHI',[], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g60_loose_larpeb',                    'L1_EM20VHI',[], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g80_loose_larpeb',                    'L1_EM20VHI',[], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],

        ]

    TriggerFlags.BphysicsSlice.signatures = [
        ['2mu4_bDimu',              'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1], 
        ['2mu4_bDimu_novtx_noos',   'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],  
        ['2mu6_bDimu',              'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1], 
        ['2mu6_bDimu_novtx_noos',   'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],  

        ]

    TriggerFlags.CombinedSlice.signatures = [
        ]

    TriggerFlags.HeavyIonSlice.signatures = [
        ]

 #Beamspot chanis first try ATR-9847                                                                                                               
    TriggerFlags.BeamspotSlice.signatures = [
        ['beamspot_allTE_L2StarB_L1TRT',           'L1_TRT', [], [PhysicsStream], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_allTE_trkfast_L1TRT',           'L1_TRT', [], [PhysicsStream], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

        ['beamspot_allTE_L2StarB_L1TRT_FILLED',           'L1_TRT_FILLED', [], [PhysicsStream], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_allTE_trkfast_L1TRT_FILLED',           'L1_TRT_FILLED', [], [PhysicsStream], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

        ['beamspot_allTE_L2StarB_L1TRT_EMPTY',           'L1_TRT_EMPTY', [], [PhysicsStream], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_allTE_trkfast_L1TRT_EMPTY',           'L1_TRT_EMPTY', [], [PhysicsStream], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ]   

    TriggerFlags.MinBiasSlice.signatures = [
        # LS1 chains
        ['mb_sptrk',                     'L1_RD0_FILLED', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_sptrk_L1RD0_UNPAIRED_ISO',  'L1_RD0_UNPAIRED_ISO', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_sptrk_L1RD0_EMPTY',         'L1_RD0_EMPTY', [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['mb_sptrk_noisesup',            'L1_RD0_FILLED', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_sptrk_noisesup_L1RD0_UNPAIRED_ISO',    'L1_RD0_UNPAIRED_ISO', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_sptrk_noisesup_L1RD0_EMPTY',   'L1_RD0_EMPTY', [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ##['mb_sp2000_trk70_hmt',        'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:HMT"], -1], #disabled for M4 see https://savannah.cern.ch/bugs/?104744

        ['mb_perf_L1LUCID_UNPAIRED_ISO', 'L1_LUCID_UNPAIRED_ISO',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_perf_L1LUCID_EMPTY',        'L1_LUCID_EMPTY',        [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['mb_perf_L1LUCID',              'L1_LUCID',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],

        ['mb_sptrk_costr',               'L1_RD0_FILLED', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_sptrk_costr_L1RD0_EMPTY',   'L1_RD0_EMPTY', [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],

        ['mb_perf_L1MBTS_2',             'L1_MBTS_2', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_perf_L1RD1_FILLED',         'L1_RD1_FILLED', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
 
        ['mb_mbts_L1MBTS_1',             'L1_MBTS_1', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_mbts_L1MBTS_2',             'L1_MBTS_2', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1], 
        ['mb_mbts_L1MBTS_1_1',           'L1_MBTS_1_1', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_mbts_L1MBTS_1_EMPTY',       'L1_MBTS_1_EMPTY', [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['mb_mbts_L1MBTS_2_EMPTY',       'L1_MBTS_2_EMPTY', [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['mb_mbts_L1MBTS_1_1_EMPTY',     'L1_MBTS_1_1_EMPTY', [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['mb_mbts_L1MBTS_1_UNPAIRED_ISO',             'L1_MBTS_1_UNPAIRED_ISO', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_mbts_L1MBTS_2_UNPAIRED_ISO',             'L1_MBTS_2_UNPAIRED_ISO', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['mb_mbts_L1MBTS_1_1_UNPAIRED_ISO',           'L1_MBTS_1_1_UNPAIRED_ISO', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ]

    TriggerFlags.CalibSlice.signatures   = [
        ['larnoiseburst_L1All',                  'L1_All',       [], ['LArNoiseBurst','CosmicCalo'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_loose_L1All',             'L1_All',       [], ['LArNoiseBurst','CosmicCalo'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1RD0_EMPTY',            'L1_RD0_EMPTY', [], ['LArNoiseBurst','CosmicCalo'], ["RATE:Calibration", "BW:Detector"], -1],        
        
        #Special re-run chains
        ['larnoiseburst_rerun',                  '',             [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_loose_rerun',            '',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        

        ['l1calocalib',             'L1_RD0_FILLED',     [], ['L1CaloCalib'], ["RATE:Calibration", "RATE:L1CaloCalib", "BW:Detector"], -1],
        ['l1calocalib_L1BGRP7',     'L1_BGRP7',          [], ['L1CaloCalib'], ["RATE:Calibration", "RATE:L1CaloCalib", "BW:Detector"], -1], 
        
        ['larcalib_L1EM3_EMPTY',    'L1_EM3_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1TAU8_EMPTY',   'L1_TAU8_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1J12_EMPTY',    'L1_J12_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1J3032ETA49_EMPTY',  'L1_J30.32ETA49_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],

        ['tilelarcalib_L1EM3_EMPTY',     'L1_EM3_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['tilelarcalib_L1TAU8_EMPTY',    'L1_TAU8_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['tilelarcalib_L1J12_EMPTY',     'L1_J12_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['tilelarcalib_L1J3032ETA49_EMPTY',   'L1_J30.32ETA49_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],

        #ALFA
        #['alfa_alfacalib',   'L1_ALFA_EMPTY', [], ['ALFACalib'], [ 'RATE:ALFACalibration','BW:Detector'], -1],        


        #IBL
        ['ibllumi_L1RD0_EMPTY',    'L1_RD0_EMPTY', [], ['IBLLumi'], [ 'RATE:IBLLumi','BW:Detector'], -1],
        ['ibllumi_L1RD0_FILLED',    'L1_RD0_FILLED', [], ['IBLLumi'], [ 'RATE:IBLLumi','BW:Detector'], -1],
        ['ibllumi_L1RD0_UNPAIRED_ISO',    'L1_RD0_UNPAIRED_ISO', [], ['IBLLumi'], [ 'RATE:IBLLumi','BW:Detector'], -1],
        ]

    TriggerFlags.CosmicSlice.signatures  = [ 
        ['tilecalib_laser',   'L1_CALREQ2', [], ['Tile'], ["RATE:Calibration", "RATE:Cosmic_TileCalibration", "BW:Detector"], -1],
        ['pixel_noise',        'L1_RD0_EMPTY', [], ['PixelNoise'], ["RATE:Calibration", "RATE:PixelCalibration", "BW:Detector"], -1],
        ['pixel_beam',         'L1_RD0_FILLED', [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1],
        ['sct_noise',          'L1_RD0_EMPTY', [], ['SCTNoise'], ["RATE:Calibration", "RATE:SCTCalibration", "BW:Detector"], -1],
        
        # CosmicCalo
        ['larps_L1EM3_EMPTY',  'L1_EM3_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1EM7_EMPTY',  'L1_EM7_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J12_EMPTY',  'L1_J12_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J30_EMPTY',  'L1_J30_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1TAU8_EMPTY', 'L1_TAU8_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],

        #JetTauEtmiss 
        ['larps_L1J12_FIRSTEMPTY', 'L1_J12_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J30_FIRSTEMPTY', 'L1_J30_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],

        #['larhec_L1J12_FIRSTEMPTY', 'L1_J12_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],

        #CosmicID
        ['id_cosmicid_L1MU4',         'L1_MU4', [],  ['HLT_IDCosmic', 'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],
        ['id_cosmicid_L1MU11',        'L1_MU11', [], ['HLT_IDCosmic', 'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],

        ['id_cosmicid',               '', [], ['HLT_IDCosmic', 'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],
        ['id_cosmicid_trtxk',         '', [], ['HLT_IDCosmic'],            ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],
        ['id_cosmicid_trtxk_central', '', [], ['HLT_IDCosmic'],            ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],

        # ['id_cosmicid_ds',            '', [], ['DataScouting_04_IDCosmic'], ['RATE:CosmicSlice', 'BW:Detector'], -1],        

        ]
    TriggerFlags.StreamingSlice.signatures = [

        ['noalg_L1CALREQ2',   'L1_CALREQ2', [], ['Tile'], ["RATE:Calibration", "RATE:Cosmic_TileCalibration", "BW:Detector"], -1],

        # beam splash streamer
        ['noalg_L1EM20A',   'L1_EM20A',        [], ['MinBias', 'express'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_L1EM20C',   'L1_EM20C',        [], ['MinBias', 'express'], ["RATE:MinBias", "BW:MinBias"], -1],
        
        ['noalg_L1J75A',   'L1_J75A',          [], ['MinBias', 'express'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_L1J75C',   'L1_J75C',          [], ['MinBias', 'express'], ["RATE:MinBias", "BW:MinBias"], -1],
        
        ['noalg_L1TAU20A',   'L1_TAU20A',      [], ['MinBias', 'express'], ["RATE:MinBias", "BW:MinBias"], -1], 
        ['noalg_L1TAU20C',   'L1_TAU20C',      [], ['MinBias', 'express'], ["RATE:MinBias", "BW:MinBias"], -1], 

        # ALFA_ANY streamer
        ['noalg_L1ALFA_ANY',   'L1_ALFA_ANY',          [], ['ALFACalib'], ["RATE:SeededStreamers", "BW:Other"], -1], 

        # 
        ['noalg_L1TGC_BURST',   'L1_TGC_BURST', [], ['TgcNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        
        #DCM level monitoring test chain
        ['noalg_dcmmon_L1RD0_EMPTY',   'L1_RD0_EMPTY',        [], ['monitoring_random'], ["RATE:DISCARD","BW:DISCARD"], -1], 

        #background streamers
        ['noalg_bkg_L1Bkg',              'L1_Bkg',              [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_bkg_L1RD0_UNPAIRED_ISO', 'L1_RD0_UNPAIRED_ISO', [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_bkg_L1RD0_EMPTY',        'L1_RD0_EMPTY',        [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_bkg_L1RD1_FILLED',       'L1_RD1_FILLED',       [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1], 

        ['noalg_L1Standby',              'L1_Standby',          [], ['Standby'],    ["RATE:SeededStreamers", "BW:Other"], -1],         
        ['noalg_idcosmic_L1TRT',         'L1_TRT',              [], ['IDCosmic'],   ["RATE:SeededStreamers", "BW:Other"], -1],        
        ['noalg_idcosmic_L1TRT_EMPTY',          'L1_TRT_EMPTY',              [], ['IDCosmic'],   ["RATE:SeededStreamers", "BW:Other"], -1],        
        ['noalg_idcosmic_L1TRT_FILLED',         'L1_TRT_FILLED',              [], ['IDCosmic'],   ["RATE:SeededStreamers", "BW:Other"], -1],        

        # standby streamer
        # disabled (ATR-9101) ['noalg_L1Standby',  'L1_Standby',          [], ['Standby'],    ["RATE:SeededStreamers", "BW:Other"], -1], 
        #
        # L1 streamers
        #disabled see #104204   ['noalg_L1Muon',                'L1_Muon',             [], ['L1Muon'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
        #disabled see #104204   ['noalg_L1Muon_EMPTY',          'L1_Muon_EMPTY',       [], ['L1Muon'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
        
        ['noalg_L1Calo',                  'L1_Calo',             [], ['L1Calo'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_L1MinBias',               'L1_MinBias',          [], ['L1MinBias'],  ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_L1Calo_EMPTY',            'L1_Calo_EMPTY',       [], ['L1Calo'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
        # #['noalg_L1MinBias_EMPTY',      'L1_MinBias_EMPTY', [], ['L1MinBias'], [], -1], 

        # #minbias streamer
        ['noalg_mb_L1MBTS_1',             'L1_MBTS_1',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1MBTS_1_EMPTY',       'L1_MBTS_1_EMPTY',        [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1MBTS_1_UNPAIRED_ISO','L1_MBTS_1_UNPAIRED_ISO',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1MBTS_2',             'L1_MBTS_2',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1MBTS_2_EMPTY',       'L1_MBTS_2_EMPTY',        [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1MBTS_2_UNPAIRED_ISO',  'L1_MBTS_2_UNPAIRED_ISO',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1MBTS_1_1',             'L1_MBTS_1_1',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1MBTS_1_1_EMPTY',       'L1_MBTS_1_1_EMPTY',        [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1MBTS_1_1_UNPAIRED_ISO','L1_MBTS_1_1_UNPAIRED_ISO',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
	#beam splashes
        ['noalg_mb_L1MBTS_4_A',            'L1_MBTS_4_A',        [], ['MinBias', 'express'], ["RATE:MinBias", "BW:MinBias"], -1],
	['noalg_mb_L1MBTS_4_C',            'L1_MBTS_4_C',        [], ['MinBias', 'express'], ["RATE:MinBias", "BW:MinBias"], -1],

	#others
        ['noalg_mb_L1LUCID',             'L1_LUCID',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1LUCID_EMPTY',       'L1_LUCID_EMPTY',        [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1LUCID_UNPAIRED_ISO','L1_LUCID_UNPAIRED_ISO',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1RD0_EMPTY',          'L1_RD0_EMPTY',        [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1RD0_FILLED',         'L1_RD0_FILLED',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1RD0_UNPAIRED_ISO',   'L1_RD0_UNPAIRED_ISO', [], ['MinBias'], ["RATE:Cosmic_MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1RD1_EMPTY',          'L1_RD1_EMPTY',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_mb_L1RD1_FILLED',         'L1_RD1_FILLED',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        # L1 seed doesn't exits ['noalg_mb_L1RD1_UNPAIRED_ISO',  'L1_RD1_UNPAIRED_ISO', [], ['MinBias'], ["BW:Unpaired_MinBias", "RATE:Cosmic_MinBias"], -1],

        # LHCF
        ['noalg_L1LHCF',             'L1_LHCF',        [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_L1LHCF_MBTS_1',      'L1_LHCF_MBTS_1', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        ['noalg_L1LHCF_MBTS_2',      'L1_LHCF_MBTS_2', [], ['MinBias'], ["RATE:MinBias", "BW:MinBias"], -1],
        
        # id cosmic streamer
        #['noalg_cosmicid_L1TRT',    'L1_TRT',              [], ['IDCosmic'], ["BW:MinBias", "RATE:MinBias"], -1],

        #Zero bias streamer
        ['noalg_zb_L1ZB','L1_ZB', [], ['ZeroBias'], ["BW:ZeroBias", "RATE:ZeroBias"], -1],
        #Zero bias plus HLT jet
        ['j40_L1ZB','L1_ZB', [], ['ZeroBias'], ["BW:ZeroBias", "RATE:ZeroBias"], -1],
        

        # Cosmic calo streamer
        ['noalg_cosmiccalo_L1EM3_EMPTY',         'L1_EM3_EMPTY',        [], ['CosmicCalo'], ["BW:MinBias", "RATE:Cosmic_Calo"], -1],        
        ['noalg_cosmiccalo_L1RD1_EMPTY',         'L1_RD1_EMPTY',        [], ['CosmicCalo'], ["BW:MinBias", "RATE:Cosmic_Calo"], -1],        
        ['noalg_cosmiccalo_L1J3032ETA49_EMPTY',  'L1_J30.32ETA49_EMPTY',[], ['CosmicCalo'], ["BW:Jets", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1J12_EMPTY',         'L1_J12_EMPTY',        [], ['CosmicCalo', 'express'], ["BW:Jets", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1J30_EMPTY',         'L1_J30_EMPTY',        [], ['CosmicCalo', 'express'], ["BW:Jets", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1J12_FIRSTEMPTY',    'L1_J12_FIRSTEMPTY',   [], ['CosmicCalo', 'express'], ["BW:Jets", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1J30_FIRSTEMPTY',    'L1_J30_FIRSTEMPTY',   [], ['CosmicCalo', 'express'], ["BW:Jets", "RATE:Cosmic_Calo"], -1],
        
        #HLT pass through
        ['noalg_L1All',                  'L1_All',              [], ['HLTPassthrough'], ["RATE:SeededStreamers", "BW:Detector"], -1],
                
        # Enhanced bias item
        ['noalg_eb_L1BGRP7',                  'L1_BGRP7',              [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        #['noalg_eb_L1RD1_FILLED',            'L1_RD1_FILLED',        [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1RD0_EMPTY',             'L1_RD0_EMPTY',         [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        #['noalg_eb_L1RD0_FIRSTEMPTY',         'L1_RD0_FIRSTEMPTY',    [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        #['noalg_eb_L1RD0_UNPAIRED_ISO',        'L1_RD0_UNPAIRED_ISO',  [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        #['noalg_eb_L1Physics_noPS'   ,        'L1_Physics_noPS',      [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        
        #idmon streamer
        ['noalg_idmon_L1RD0_EMPTY',          'L1_RD0_EMPTY',        [], ['IDMonitoring','express'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['noalg_idmon_L1RD0_FILLED',         'L1_RD0_FILLED',        [], ['IDMonitoring'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['noalg_idmon_L1RD0_UNPAIRED_ISO',   'L1_RD0_UNPAIRED_ISO', [], ['IDMonitoring'], ["RATE:Monitoring", "BW:Detector"], -1],
        
        #cosmicmuon streamer
        ['noalg_cosmicmuons_L1MU4_EMPTY',   'L1_MU4_EMPTY',        [], ['CosmicMuons','express'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],        
        ['noalg_cosmicmuons_L1MU11_EMPTY',  'L1_MU11_EMPTY',        [], ['CosmicMuons','express'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],        

        # L1Topo streamers for M8
        ['noalg_L1MJJ-4',   'L1_MJJ-4',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1MJJ-3',   'L1_MJJ-3',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1MJJ-2',   'L1_MJJ-2',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1MJJ-1',   'L1_MJJ-1',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        
        ['noalg_L1MJJ-350-0',   'L1_MJJ-350-0',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1MJJ-300-0',   'L1_MJJ-300-0',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1MJJ-250-0',   'L1_MJJ-250-0',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1MJJ-200-0',   'L1_MJJ-200-0',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ], 
        
        ['noalg_L1HT200-J20s5.ETA49', 'L1_HT200-J20s5.ETA49', [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1HT190-J20s5.ETA49', 'L1_HT190-J20s5.ETA49', [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1HT1-J0.ETA49',   'L1_HT1-J0.ETA49',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        
        ['noalg_L1JPSI-1M5-EMs',  'L1_JPSI-1M5-EMs',  [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1JPSI-1M5-EM6s', 'L1_JPSI-1M5-EM6s', [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1JPSI-1M5-EM12s','L1_JPSI-1M5-EM12s',[], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],

        ['noalg_L1DETA-JJ',         'L1_DETA-JJ',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        
        # M9 L1Topo streamers
        ['noalg_L1LFV-MU',          'L1_LFV-MU',          [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1DY-DR-2MU4',      'L1_DY-DR-2MU4',      [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1BPH-DR-2MU4',     'L1_BPH-DR-2MU4',     [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],   
        ['noalg_L1BPH-DR-2MU6',     'L1_BPH-DR-2MU6',     [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1BPH-DR-MU6MU4',   'L1_BPH-DR-MU6MU4',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1BPH-2M-2MU4',     'L1_BPH-2M-2MU4',     [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1BPH-2M-2MU6',     'L1_BPH-2M-2MU6',     [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1BPH-2M-MU6MU4',   'L1_BPH-2M-MU6MU4',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1BPH-4M8-2MU4',    'L1_BPH-4M8-2MU4',    [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1BPH-4M8-2MU6',    'L1_BPH-4M8-2MU6',    [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1BPH-4M8-MU6MU4',  'L1_BPH-4M8-MU6MU4',  [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1DY-BOX-2MU4',     'L1_DY-BOX-2MU4',     [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1DY-BOX-MU6MU4',   'L1_DY-BOX-MU6MU4',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1DY-BOX-2MU6',     'L1_DY-BOX-2MU6',     [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1DR-TAU20ITAU12I', 'L1_DR-TAU20ITAU12I', [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ['noalg_L1DR-MU10TAU12I',   'L1_DR-MU10TAU12I',   [], [PhysicsStream], ["RATE:L1TopoStreamers", "BW:Other"], -1 ],
        ]

    TriggerFlags.MonitorSlice.signatures = [
        ## # enhancedbias
        ['timeburner', '', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], -1],
        ['robrequest', '', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], -1],
        ['robrequest_L1RD0_EMPTY', 'L1_RD0_EMPTY', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], -1],   
        
        ['costmonitor', '', [], ['CostMonitoring'], ['RATE:Monitoring','BW:Other'],1],

        ['cscmon_L1MU10',   'L1_MU10',     [], ['CSC'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['cscmon_L1EM3',    'L1_EM3',     [], ['CSC'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['cscmon_L1J12',    'L1_J12',     [], ['CSC'], ["RATE:Monitoring", "BW:Detector"], -1],

        
        ## check L1 ['idmon_trkFS_L14J15', 'L1_4J15', [], [PhysicsStream], ['RATE:InDetTracking', 'BW:Detector'], -1],

        ]

    TriggerFlags.EnhancedBiasSlice.signatures = [
        #['eb_physics_L1RD1_FILLED',              'L1_RD1_FILLED',         [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        #['eb_low_L1RD0_FILLED',                  'L1_RD0_FILLED',         [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        #['eb_high_L1RD0_FILLED',                 'L1_RD0_FILLED',         [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['eb_empty_L1RD0_EMPTY',                  'L1_RD0_EMPTY',         [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        #['eb_firstempty_L1RD0_FIRSTEMPTY',       'L1_RD0_FIRSTEMPTY',         [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        # ['eb_unpairediso_L1RD0_UNPAIRED_ISO',   'L1_RD0_UNPAIRED_ISO',         [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ]

    #TriggerFlags.GenericSlice.signatures = []
    
class Prescales:
    #   Item name             | Prescale
    #----------------------------------------------------------
    L1Prescales = {}

    #   Signature name        |
    #      [ HLTprescale, HLTpass-through, rerun]
    #   - Prescale values should be a positive integer (default=1)
    #   - If the current pass_through value is non-zero,
    #     the value given here will be used as pass_through rate
    #     Assuming that pass through chains are configured so
    #     in the slice files and won't change. Also prescale
    #     and pass_through will not be used together.
    #   - If only the first value is specified,
    #     the default value of pass-through (=0) will be used
    #----------------------------------------------------------
    HLTPrescales = {
        'larnoiseburst_rerun' : [ 1, 0 , 1, "LArNoiseBurst"],
        'larnoiseburst_loose_rerun' : [ 1, 0 , 1, "LArNoiseBurst"],
        }

#StreamConfig = {}

