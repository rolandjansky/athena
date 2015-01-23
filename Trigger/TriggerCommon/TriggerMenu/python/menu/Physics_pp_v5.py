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

    # INPUT FORMAT FOR CHAINS:
    # ['chainName', chainCounter, 'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep]

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.TestSlice.signatures = [
        ]


    TriggerFlags.MuonSlice.signatures = [ 
        ## ADD MU*_EMPTY TO L1 MENU
        ['mu4_cosmic_L1MU4_EMPTY',               'L1_MU4_EMPTY', [],   ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],
        ['mu4_cosmic_L1MU11_EMPTY',              'L1_MU11_EMPTY', [],  ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],
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
        ['j0_perf_L1RD0_EMPTY',      'L1_RD0_EMPTY',[], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j0_perf_L1MU10',           'L1_MU10',[], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j0_perf_L1J12',        'L1_J12',[], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['ht0_perf_L1J12',       'L1_J12',[], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ]

    TriggerFlags.BjetSlice.signatures = [
        ['j0_perf_bperf_L1RD0_EMPTY',  'L1_RD0_EMPTY',[], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j0_perf_bperf_L1MU10',       'L1_MU10',[], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j0_perf_bperf_L1J12',        'L1_J12',[], [PhysicsStream], ['RATE:SingleJet', 'BW:Jets'], -1],

        ]

    TriggerFlags.METSlice.signatures = [
        ]

    TriggerFlags.TauSlice.signatures = [
        ['tau8_cosmic_track', 'L1_TAU8_EMPTY', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau8_cosmic_ptonly', 'L1_TAU8_EMPTY', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau1_cosmic_track_L1MU4_EMPTY',  'L1_MU4_EMPTY', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau1_cosmic_ptonly_L1MU4_EMPTY', 'L1_MU4_EMPTY', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ]

    TriggerFlags.EgammaSlice.signatures = [
        ### NOT IN THE LIST ['e5_loose1',                        'L1_EM3', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e5_etcut_L1EM3_EMPTY',              'L1_EM3_EMPTY', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['g5_etcut',                          'L1_EM3', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g5_etcut_L1EM3_EMPTY',              'L1_EM3_EMPTY', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ]

    TriggerFlags.BphysicsSlice.signatures = [
        ['2mu4_bDimu',              'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1], 
        ['2mu4_bDimu_novtx_noos',   'L1_2MU4', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],  
        ['2mu6_bDimu',              'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1], 
        ['2mu6_bDimu_novtx_noos',   'L1_2MU6', [], [PhysicsStream], ['RATE:Bphysics','BW:Bphys'], -1],  

        ]

    TriggerFlags.CombinedSlice.signatures = [
        ]
   
    TriggerFlags.MinBiasSlice.signatures = [
        # LS1 chains
        ['mb_sptrk',                     'L1_RD0_FILLED', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sptrk_noisesup',            'L1_RD0_FILLED', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sptrk_L1RD0_UNPAIRED_ISO',  'L1_RD0_UNPAIRED_ISO', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sptrk_L1RD0_EMPTY',         'L1_RD0_EMPTY', [], ['MinBias'], ["BW:MinBias", "RATE:Cosmic_Minbias"], -1],
        ##['mb_sp2000_trk70_hmt',        'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:HMT"], -1], #disabled for M4 see https://savannah.cern.ch/bugs/?104744

        ['mb_perf_L1LUCID_UNPAIRED_ISO', 'L1_LUCID_UNPAIRED_ISO',        [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_perf_L1LUCID_EMPTY',        'L1_LUCID_EMPTY',        [], ['MinBias'], ["BW:MinBias", "RATE:Cosmic_Minbias"], -1],
        ['mb_perf_L1LUCID',              'L1_LUCID',        [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

        ['mb_sptrk_costr',               'L1_RD0_FILLED', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sptrk_costr_L1RD0_EMPTY',   'L1_RD0_EMPTY', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

        ['mb_perf_L1MBTS_2',             'L1_MBTS_2', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['mb_mbts_L1MBTS_2',             'L1_MBTS_2', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ]

    TriggerFlags.CalibSlice.signatures   = [
        ['larnoiseburst_L1All',       'L1_All',       [], [PhysicsStream], ["BW:Jets", "RATE:SingleJet"], -1],
        ['larnoiseburst_L1RD0_EMPTY', 'L1_RD0_EMPTY', [], [PhysicsStream], ["BW:Jets", "RATE:SingleJet"], -1],        

        ['l1calocalib',             'L1_RD0_FILLED',     [], ['L1CaloCalib'], ["RATE:Calibration", "RATE:L1CaloCalib", "BW:Detector"], -1],
        ['l1calocalib_L1BGRP7',     'L1_BGRP7',          [], ['L1CaloCalib'], ["RATE:Calibration", "RATE:L1CaloCalib", "BW:Detector"], -1], 
        
        ['larcalib_L1EM3_EMPTY',    'L1_EM3_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1TAU8_EMPTY',   'L1_TAU8_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1J12_EMPTY',    'L1_J12_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1J3032ETA49_EMPTY',  'L1_J30.32ETA49_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],

        # DCM level monitoring chain for CSC (#104895, ATR-8082)
        ['larcalib_L1MU10',   'L1_MU10',     [], ['CSC'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['larcalib_L1EM3',    'L1_EM3',     [], ['CSC'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['larcalib_L1J12',    'L1_J12',     [], ['CSC'], ["RATE:Monitoring", "BW:Detector"], -1],
        
        ['tilelarcalib_L1EM3_EMPTY',     'L1_EM3_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['tilelarcalib_L1TAU8_EMPTY',    'L1_TAU8_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['tilelarcalib_L1J12_EMPTY',     'L1_J12_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['tilelarcalib_L1J3032ETA49_EMPTY',   'L1_J30.32ETA49_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],

        #ALFA
        #['alfa_alfacalib',   'L1_ALFA_EMPTY', [], ['ALFACalib'], [ 'RATE:ALFACalibration','BW:Detector'], -1],        
        ]

    TriggerFlags.CosmicSlice.signatures  = [ 
        #['tilecalib_laser',   'L1_CALREQ2', [], ['Tile'], ["RATE:Calibration", "RATE:Cosmic_TileCalibration", "BW:Detector"], -1],
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

        ['larhec_L1J12_FIRSTEMPTY', 'L1_J12_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],

        #CosmicID
        ['id_cosmicid_L1MU4',         'L1_MU4', [], ['CosmicID', 'express'], ['RATE:CosmicSlice', 'RATE:CosmicSlice', 'BW:Detector'], -1],
        ['id_cosmicid_L1MU11',        'L1_MU11', [], ['CosmicID', 'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],

        ['id_cosmicid',               '', [], ['CosmicID', 'express'], ['RATE:CosmicSlice', 'RATE:CosmicSlice', 'BW:Detector'], -1],
        ['id_cosmicid_trtxk',         '', [], ['CosmicID'], ['RATE:CosmicSlice', 'BW:Detector'], -1],
        ['id_cosmicid_trtxk_central', '', [], ['CosmicID'], ['RATE:CosmicSlice', 'BW:Detector'], -1],
        
        # ['id_cosmicid_ds',            '', [], ['DataScouting_04_IDCosmic'], ['RATE:CosmicSlice', 'BW:Detector'], -1],        

        ]
    TriggerFlags.StreamingSlice.signatures = [
        #DCM level monitoring test chain
        ['noalg_dcmmon_L1RD0_EMPTY',   'L1_RD0_EMPTY',        [], ['monitoring_random'], ["RATE:DISCARD","BW:DISCARD"], -1], 

        #background streamers
        ['noalg_bkg_L1Bkg',              'L1_Bkg',              [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_bkg_L1RD0_UNPAIRED_ISO', 'L1_RD0_UNPAIRED_ISO', [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_bkg_L1RD0_EMPTY',        'L1_RD0_EMPTY',        [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_bkg_L1RD1_FILLED',       'L1_RD1_FILLED',       [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1], 
        
        # standby streamer
        # disabled (ATR-9101) ['noalg_L1Standby',  'L1_Standby',          [], ['Standby'],    ["RATE:SeededStreamers", "BW:Other"], -1], 
        #
        # L1 streamers
        #disabled see #104204   ['noalg_L1Muon',                'L1_Muon',             [], ['L1Muon'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
        #disabled see #104204   ['noalg_L1Muon_EMPTY',          'L1_Muon_EMPTY',       [], ['L1Muon'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
        
        ['noalg_L1Calo',                  'L1_Calo',             [], ['L1Calo'],     ["RATE:L1TopoStreamers", "BW:Other"], -1], 
        ['noalg_L1MinBias',               'L1_MinBias',          [], ['L1MinBias'],  ["RATE:L1TopoStreamers", "BW:Other"], -1], 
        ['noalg_L1Calo_EMPTY',            'L1_Calo_EMPTY',       [], ['L1Calo'],     ["RATE:L1TopoStreamers", "BW:Other"], -1], 
        # #['noalg_L1MinBias_EMPTY',      'L1_MinBias_EMPTY', [], ['L1MinBias'], [], -1], 

        # #minbias streamer
        ['noalg_mb_L1MBTS_2',     'L1_MBTS_2',        [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['noalg_mb_L1MBTS_2_UNPAIRED_ISO',  'L1_MBTS_2_UNPAIRED_ISO',        [], ['MinBias'], ["BW:MinBias", "RATE:SeededStreamers"], -1],
        ['noalg_mb_L1RD0_EMPTY',          'L1_RD0_EMPTY',        [], ['MinBias'], ["BW:MinBias", "RATE:Cosmic_Minbias"], -1],
        ['noalg_mb_L1RD0_FILLED',         'L1_RD0_FILLED',        [], ['MinBias'], ["BW:MinBias", "RATE:Cosmic_Minbias"], -1],
        ['noalg_mb_L1RD0_UNPAIRED_ISO',   'L1_RD0_UNPAIRED_ISO', [], ['MinBias'], ["BW:Unpaired_Minbias", "RATE:Cosmic_Minbias"], -1],
        ['noalg_mb_L1RD1_EMPTY',          'L1_RD1_EMPTY',        [], ['MinBias'], ["BW:MinBias", "RATE:Cosmic_Minbias"], -1],
        ['noalg_mb_L1RD1_FILLED',         'L1_RD1_FILLED',        [], ['MinBias'], ["BW:MinBias", "RATE:Cosmic_Minbias"], -1],
        # L1 seed doesn't exits ['noalg_mb_L1RD1_UNPAIRED_ISO',  'L1_RD1_UNPAIRED_ISO', [], ['MinBias'], ["BW:Unpaired_Minbias", "RATE:Cosmic_Minbias"], -1],
        
        # id cosmic streamer
        #['noalg_idcosmic_L1TRT',    'L1_TRT',              [], ['IDCosmic'], ["BW:MinBias", "RATE:Minbias"], -1],
        
        # Cosmic calo streamer
        ['noalg_cosmiccalo_L1EM3_EMPTY',         'L1_EM3_EMPTY',        [], ['CosmicCalo'], ["BW:MinBias", "RATE:Cosmic_Calo"], -1],        
        ['noalg_cosmiccalo_L1RD1_EMPTY',         'L1_RD1_EMPTY',        [], ['CosmicCalo'], ["BW:MinBias", "RATE:Cosmic_Calo"], -1],        
        ['noalg_cosmiccalo_L1J3032ETA49_EMPTY',  'L1_J30.32ETA49_EMPTY',[], ['CosmicCalo'], ["BW:Jets", "RATE:CosmicCalo"], -1],
        ['noalg_cosmiccalo_L1J12_EMPTY',         'L1_J12_EMPTY',        [], ['CosmicCalo', 'express'], ["BW:Jets", "RATE:CosmicCalo"], -1],
        ['noalg_cosmiccalo_L1J30_EMPTY',         'L1_J30_EMPTY',        [], ['CosmicCalo', 'express'], ["BW:Jets", "RATE:CosmicCalo"], -1],
        ['noalg_cosmiccalo_L1J12_FIRSTEMPTY',    'L1_J12_FIRSTEMPTY',   [], ['CosmicCalo', 'express'], ["BW:Jets", "RATE:CosmicCalo"], -1],
        ['noalg_cosmiccalo_L1J30_FIRSTEMPTY',    'L1_J30_FIRSTEMPTY',   [], ['CosmicCalo', 'express'], ["BW:Jets", "RATE:CosmicCalo"], -1],
        
        #HLT pass through
        ['noalg_L1All',                  'L1_All',              [], ['HLTPassthrough'], ["RATE:SeededStreamers", "BW:Detector"], -1],
        ['noalg_L1J12',                  'L1_J12',              [], ['HLTPassthrough'], ["RATE:SeededStreamers", "BW:Detector"], -1],
        ['noalg_L1TAU8',                 'L1_TAU8',             [], ['HLTPassthrough'], ["RATE:SeededStreamers", "BW:Detector"], -1],
        ['noalg_L1EM3',                  'L1_EM3',             [],  ['HLTPassthrough'], ["RATE:SeededStreamers", "BW:Detector"], -1],
        
        # jettauetmiss streamer
        ['noalg_jettauetmiss_L1RD0_FIRSTEMPTY',  'L1_RD0_FIRSTEMPTY',  [], ['JetTauEtmiss'], ["RATE:SeededStreamers", "BW:Detector"], -1],
        
        # Enhanced bias item
        ['noalg_eb_L1BGRP7',                  'L1_BGRP7',              [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        #['noalg_eb_L1RD1_FILLED',            'L1_RD1_FILLED',        [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1RD0_EMPTY',             'L1_RD0_EMPTY',         [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        #['noalg_eb_L1RD0_FIRSTEMPTY',         'L1_RD0_FIRSTEMPTY',    [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        #['noalg_eb_L1RD0_UNPAIRED_ISO',        'L1_RD0_UNPAIRED_ISO',  [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        #['noalg_eb_L1Physics_noPS'   ,        'L1_Physics_noPS',      [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        
        #idmon streamer
        ['noalg_idmon_L1RD0_EMPTY',          'L1_RD0_EMPTY',        [], ['IDMonitoring'], ["RATE:DISCARD","BW:DISCARD"], -1],
        ['noalg_idmon_L1RD0_FILLED',         'L1_RD0_FILLED',        [], ['IDMonitoring'], ["RATE:DISCARD","BW:DISCARD"], -1],
        ['noalg_idmon_L1RD0_UNPAIRED_ISO',   'L1_RD0_UNPAIRED_ISO', [], ['IDMonitoring'], ["RATE:DISCARD","BW:DISCARD"], -1],
        
        #cosmicmuon streamer
        ['noalg_cosmicmuons_L1MU4_EMPTY',   'L1_MU4_EMPTY',        [], ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],        
        ['noalg_cosmicmuons_L1MU11_EMPTY',  'L1_MU11_EMPTY',        [], ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],        

        ]

    TriggerFlags.MonitorSlice.signatures = [
        ## # enhancedbias
        ['timeburner', '', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], -1],
        ['robrequest', '', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], -1],
        ['robrequest_L1RD0_EMPTY', 'L1_RD0_EMPTY', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], -1],   
        
        ['costmonitor', '', [], ['CostMonitoring'], ['RATE:Monitoring','BW:Other'],1],
        
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
    HLTPrescales = {}


#StreamConfig = {}

