# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
#------------------------------------------------------------------------#
def setupMenu():

    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenu.menu.LS1_v1.py' )

    from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1 as tcl1
    if tcl1.current:
        log.info("L1 items: %s" % tcl1.current.menu.items.itemNames())
    else:
        log.info("ERROR L1 menu has not yet been defined")

    # INPUT FORMAT FOR CHAINS:
    # ['chainName',  'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep]

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.MuonSlice.signatures = [
        ['mu4_cosmic_L1MU4_EMPTY',               'L1_MU4_EMPTY', [], ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],
        ['mu4_cosmic_L1MU11_EMPTY',              'L1_MU11_EMPTY', [], ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],
        ['mu4_msonly_cosmic_L1MU11_EMPTY',       'L1_MU11_EMPTY', [], ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],
        
        ['mu4_cosmicEF_L1MU4_EMPTY',             'L1_MU4_EMPTY', [], ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],
        ['mu4_cosmicEF_L1MU11_EMPTY',            'L1_MU11_EMPTY', [], ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],
        ['mu4_msonly_cosmicEF_L1MU11_EMPTY',     'L1_MU11_EMPTY', [], ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],
        ['mu4_msonly_cosmicEF_L1MU4_EMPTY',      'L1_MU4_EMPTY', [], ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],

        #['mu4_cosmic_ds_L1MU4',                 'L1_MU4',   [], ['DataScouting_01_CosmicMuons'], ["RATE:Cosmic_Muon_DS", "BW:Muon"], -1],
        #['mu4_cosmic_ds_L1MU10',                'L1_MU10',  [], ['DataScouting_01_CosmicMuons'], ["RATE:Cosmic_Muon_DS", "BW:Muon"], -1],
        #['mu4_cosmic_ds_L1MU11',                'L1_MU11',  [], ['DataScouting_01_CosmicMuons'], ["RATE:Cosmic_Muon_DS", "BW:Muon"], -1],
        #['mu4_cosmic_ds_L1MU4_EMPTY',           'L1_MU4_EMPTY',  [], ['DataScouting_01_CosmicMuons'], ["RATE:Cosmic_Muon_DS", "BW:Muon"], -1],
        #['mu4_cosmic_ds_L1MU10_EMPTY',          'L1_MU10_EMPTY',  [], ['DataScouting_01_CosmicMuons'], ["RATE:Cosmic_Muon_DS", "BW:Muon"], -1],
        #['mu4_cosmic_ds_L1MU11_EMPTY',          'L1_MU11_EMPTY',  [], ['DataScouting_01_CosmicMuons'], ["RATE:Cosmic_Muon_DS", "BW:Muon"], -1],

        ['mu4_cosmicEF_ds1_L1MU4',  'L1_MU4', [], ['DataScouting_01_CosmicMuons'], ["RATE:Cosmic_Muon_DS", "BW:Muon"], -1],
        ['mu4_cosmicEF_ds2_L1MU4',  'L1_MU4', [], ['DataScouting_01_CosmicMuons','DataScouting_02_CosmicMuons'], ["RATE:Cosmic_Muon_DS", "BW:Muon"], -1],
        
        ['mu0_muoncalib', 'L1_MU4',[],['Muon_Calibration'],["RATE:SingleMuon","BW:Muon"],-1],

        #['mu0_muoncalib_ds3', 'L1_MU4',[],['DataScouting_03_CosmicMuons'],["RATE:SingleMuon","BW:Muon"],-1],

        ]

    TriggerFlags.JetSlice.signatures = [
        # Performance chains
        ['j0_perf_L1RD0_EMPTY',    'L1_RD0_EMPTY',[], ['Main'], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j0_perf_L1MU10',         'L1_MU10',[], ['Main'], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j0_perf_L1J10',      'L1_J10',[], ['Main'], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['ht0_perf_L1J10',      'L1_J10',[], ['Main'], ['RATE:SingleJet', 'BW:Jets'], -1],
        #['j_perf_lcw_L1RD0_EMPTY', 'L1_RD0_EMPTY',[], ['Main'], ['RATE:SingleJet', 'BW:Jets'], -1],
        #['j_perf_lcw_L1MU10',      'L1_MU10',[], ['Main'], ['RATE:SingleJet', 'BW:Jets'], -1],   
        ['j0_perf_ds1_L1MU4',  'L1_MU4', [], ['DataScouting_05_Jets'], ["RATE:Cosmic_Jets_DS", "BW:Jets"], -1],
        ['j0_perf_ds2_L1MU4',  'L1_MU4', [], ['DataScouting_06_Jets'], ["RATE:Cosmic_Jets_DS", "BW:Jets"], -1],       
        ]

    TriggerFlags.BjetSlice.signatures =  [
        ['j0_perf_bperf_L1RD0_EMPTY', 'L1_RD0_EMPTY',[], ['Main'], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j0_perf_bperf_L1MU10',      'L1_MU10',[], ['Main'], ['RATE:SingleJet', 'BW:Jets'], -1],
        ['j0_perf_bperf_L1J10',       'L1_J10',[], ['Main'], ['RATE:SingleJet', 'BW:Jets'], -1],
        ]

    TriggerFlags.METSlice.signatures = [
        ['xe20',                             'L1_XE20',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        ['xe20_cell',                        'L1_XE20',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        ['xe20_L2FS',                        'L1_XE20',[], ['Main'], ['Rate:MET', 'BW:MET'],  1],
        ['xe20_tc_em',                       'L1_XE20',[], ['Main'], ['Rate:MET', 'BW:MET'], -1], 
        ['xe20_pueta',                       'L1_XE20',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        ['xe20_l2fsperf_wEFMuFEB_wEFMu',     'L1_XE20',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        ['xe25',                             'L1_XE25',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        ['xe25_cell',                        'L1_XE25',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        ['xe25_L2FS',                        'L1_XE25',[], ['Main'], ['Rate:MET', 'BW:MET'],  1],
        ['xe25_tc_em',                       'L1_XE25',[], ['Main'], ['Rate:MET', 'BW:MET'], -1], 
        ['xe25_pueta',                       'L1_XE25',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        ['xe25_l2fsperf_wEFMuFEB_wEFMu',     'L1_XE25',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],

        ['xe0_L1XE20',                              'L1_XE20',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        ['xe0_cell_L1XE20',                         'L1_XE20',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        ['xe0_L2FS_L1XE20',                         'L1_XE20',[], ['Main'], ['Rate:MET', 'BW:MET'],  1],
        ['xe0_tc_em_L1XE20',                        'L1_XE20',[], ['Main'], ['Rate:MET', 'BW:MET'], -1], 
        ['xe0_pueta_L1XE20',                        'L1_XE20',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        ['xe0_l2fsperf_wEFMuFEB_wEFMu_L1XE20',      'L1_XE20',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        ['xe0_L1XE25',                              'L1_XE25',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        ['xe0_cell_L1XE25',                         'L1_XE25',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        ['xe0_L2FS_L1XE25',                         'L1_XE25',[], ['Main'], ['Rate:MET', 'BW:MET'],  1],
        ['xe0_tc_em_L1XE25',                        'L1_XE25',[], ['Main'], ['Rate:MET', 'BW:MET'], -1], 
        ['xe0_pueta_L1XE25',                        'L1_XE25',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        ['xe0_l2fsperf_wEFMuFEB_wEFMu_L1XE25',      'L1_XE25',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],

        ['xe0_L1All',                              'L1_All',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        ['xe0_cell_L1All',                         'L1_All',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        ['xe0_L2FS_L1All',                         'L1_All',[], ['Main'], ['Rate:MET', 'BW:MET'],  1],
        ['xe0_tc_em_L1All',                        'L1_All',[], ['Main'], ['Rate:MET', 'BW:MET'], -1], 
        ['xe0_pueta_L1All',                        'L1_All',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        ['xe0_l2fsperf_wEFMuFEB_wEFMu_L1All',      'L1_All',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],

        ['xs30',                                   'L1_XS30',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        #['xs35',                                   'L1_XS35',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        ['xs30_cell',                              'L1_XS30',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        #['xs35_cell',                              'L1_XS35',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],

        ['te20',                                   'L1_TE20',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        ['te300',                                  'L1_TE300',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        ['te20_cell',                              'L1_TE20',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],
        ['te300_cell',                             'L1_TE300',[], ['Main'], ['Rate:MET', 'BW:MET'], -1],

        ]

    TriggerFlags.TauSlice.signatures = [
        ['tau8_cosmic_track',  'L1_TAU8_EMPTY', [], ['Main'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau8_cosmic_ptonly', 'L1_TAU8_EMPTY', [], ['Main'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau1_cosmic_track_L1MU4_EMPTY',  'L1_MU4_EMPTY', [], ['Main'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau1_cosmic_ptonly_L1MU4_EMPTY', 'L1_MU4_EMPTY', [], ['Main'], ['RATE:SingleTau', 'BW:Tau'], -1],
        ]

    TriggerFlags.EgammaSlice.signatures = [
        ['e5_loose1',                         'L1_EM3', [], ['Main'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e5_lhloose',  'L1_EM3', [], ['Main'], ['RATE:SingleElectron', 'BW:Egamma'], 1],

        ['e5_etcut',                          'L1_EM3', [], ['Main'], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['e5_etcut_L1EM3_EMPTY',              'L1_EM3_EMPTY', [], ['Main'], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['g5_etcut',                          'L1_EM3', [], ['Main'], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
        ['g5_etcut_L1EM3_EMPTY',              'L1_EM3_EMPTY', [], ['Main'], ['RATE:SingleElectron', 'BW:Egamma'], -1], 

        ['e0_perf_L1EM3',            'L1_EM3', [], ['Main'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['e0_L2Star_perf_L1EM3',     'L1_EM3', [], ['Main'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ['g0_perf_L1EM3',            'L1_EM3', [], ['Main'], ['RATE:SingleElectron', 'BW:Egamma'], 1],
        ]

    TriggerFlags.BphysicsSlice.signatures = [
        ['2mu4_bDimu',               'L1_2MU4', [], ['Main'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1], 
        ['2mu4_bDimu_novtx_noos',    'L1_2MU4', [], ['Main'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1],  
        ['2mu6_bDimu',               'L1_2MU6', [], ['Main'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1], 
        ['2mu6_bDimu_novtx_noos',    'L1_2MU6', [], ['Main'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], -1],  
        ]
    TriggerFlags.CombinedSlice.signatures = []


    TriggerFlags.MinBiasSlice.signatures = [
        # LS1 chains
        ['mb_sptrk',                     'L1_RD0_FILLED', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sptrk_noisesup',            'L1_RD0_FILLED', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sptrk_L1RD0_UNPAIRED_ISO',  'L1_RD0_UNPAIRED_ISO', [], ['MinBias'], ["BW:MinBias", "RATE:SeededStreamers"], -1],
        ['mb_sptrk_L1RD0_EMPTY',         'L1_RD0_EMPTY', [], ['MinBias'], ["BW:MinBias", "RATE:Cosmic_Minbias"], -1],
        ##['mb_sp2000_trk70_hmt',        'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:HMT"], -1], #disabled for M4 see https://savannah.cern.ch/bugs/?104744

        ['mb_perf_L1LUCID_UNPAIRED_ISO', 'L1_LUCID_UNPAIRED_ISO',        [], ['MinBias'], ["BW:MinBias", "RATE:SeededStreamers"], -1],
        ['mb_perf_L1LUCID_EMPTY',        'L1_LUCID_EMPTY',        [], ['MinBias'], ["BW:MinBias", "RATE:Cosmic_Minbias"], -1],
        ['mb_perf_L1LUCID',              'L1_LUCID',        [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

        ['mb_sptrk_costr',               'L1_RD0_FILLED', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['mb_sptrk_costr_L1RD0_EMPTY',   'L1_RD0_EMPTY', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],

        ['mb_perf_L1MBTS_2',             'L1_MBTS_2', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 
        ['mb_mbts_L1MBTS_2',             'L1_MBTS_2', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1], 

        ]
    

    TriggerFlags.CalibSlice.signatures   = [
        ['l1calocalib',              'L1_RD0_FILLED',     [], ['L1CaloCalib'], ["RATE:Calibration", "RATE:L1CaloCalib", "BW:Detector"], -1],
        ['l1calocalib_L1BGRP7',      'L1_BGRP7',          [], ['L1CaloCalib'], ["RATE:Calibration", "RATE:L1CaloCalib", "BW:Detector"], -1], 

        ['larcalib_L1EM3_EMPTY',     'L1_EM3_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1TAU8_EMPTY',    'L1_TAU8_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1J10_EMPTY',     'L1_J10_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1FJ30_EMPTY',    'L1_FJ30_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        # DCM level monitoring chain for CSC (#104895, ATR-8082)
        ['larcalib_L1MU10',   'L1_MU10',     [], ['CSC'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['larcalib_L1EM3',    'L1_EM3',     [], ['CSC'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['larcalib_L1J10',    'L1_J10',     [], ['CSC'], ["RATE:Monitoring", "BW:Detector"], -1],

        ['tilelarcalib_L1EM3_EMPTY',    'L1_EM3_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['tilelarcalib_L1TAU8_EMPTY',   'L1_TAU8_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['tilelarcalib_L1J10_EMPTY',    'L1_J10_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['tilelarcalib_L1FJ30_EMPTY',   'L1_FJ30_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],

        #ALFA
        ['alfa_alfacalib',    'L1_ALFA_EMPTY', [], ['ALFACalib'], [ 'RATE:ALFACalibration','BW:Detector'], -1],        
        ]
       

    TriggerFlags.CosmicSlice.signatures  =  [
        ['tilecalib_laser',   'L1_CALREQ2', [], ['Tile'], ["RATE:Calibration", "RATE:Cosmic_TileCalibration", "BW:Detector"], -1],
        ['pixel_noise',       'L1_RD0_EMPTY', [], ['PixelNoise'], ["RATE:Calibration", "RATE:PixelCalibration", "BW:Detector"], -1],
        ['pixel_beam',        'L1_RD0_FILLED', [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1],
        ['sct_noise',         'L1_RD0_EMPTY', [], ['SCTNoise'], ["RATE:Calibration", "RATE:SCTCalibration", "BW:Detector"], -1],
        
        # CosmicCalo
        ['larps_L1EM3_EMPTY',  'L1_EM3_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1EM6_EMPTY',  'L1_EM6_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J10_EMPTY',  'L1_J10_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J30_EMPTY',  'L1_J30_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1TAU8_EMPTY', 'L1_TAU8_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],

        ['larps_L1J10_FIRSTEMPTY', 'L1_J10_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J30_FIRSTEMPTY', 'L1_J30_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],

        #CosmicID
        ['id_cosmic_L1MU4',          'L1_MU4', [], ['HLT_IDCosmic', 'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],
        ['id_cosmic_L1MU11',         'L1_MU11', [],['HLT_IDCosmic', 'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],

        ['id_cosmic',                '', [], ['HLT_IDCosmic', 'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],
        ['id_cosmic_trtxk',          '', [], ['HLT_IDCosmic'], ['RATE:CosmicSlice', 'BW:Detector'], -1],
        ['id_cosmic_trtxk_central',  '', [], ['HLT_IDCosmic'], ['RATE:CosmicSlice', 'BW:Detector'], -1],
        
        ['id_cosmic_ds',             '', [], ['DataScouting_04_IDCosmic'], ['RATE:CosmicSlice_DS', 'RATE:Cosmic_Tracking_DS', 'BW:Detector'], -1],        

        ]


    TriggerFlags.StreamingSlice.signatures =  [
        #DCM level monitoring test chain
        ['noalg_dcmmon_L1RD0_EMPTY',     'L1_RD0_EMPTY',        [], ['express','monitoring_random'], [""], -1], 

        #background streamers
        ['noalg_bkg_L1Bkg',              'L1_Bkg',              [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_bkg_L1RD0_UNPAIRED_ISO', 'L1_RD0_UNPAIRED_ISO', [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1],
        ['noalg_bkg_L1RD0_EMPTY',        'L1_RD0_EMPTY',        [], ['express','Background'], ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_bkg_L1RD1_FILLED',       'L1_RD1_FILLED',       [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1], 

        # standby streamer
        ['noalg_L1Standby',              'L1_Standby',          [], ['Standby'],    ["RATE:SeededStreamers", "BW:Other"], -1], 
        
        # L1 streamers
        #disabled see #104204   ['noalg_L1Muon',                'L1_Muon',             [], ['L1Muon'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
        #disabled see #104204   ['noalg_L1Muon_EMPTY',          'L1_Muon_EMPTY',       [], ['L1Muon'],     ["RATE:SeededStreamers", "BW:Other"], -1], 

        ['noalg_L1Calo',                 'L1_Calo',             [], ['L1Calo'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_L1MinBias',              'L1_MinBias',          [], ['L1MinBias'],  ["RATE:SeededStreamers", "BW:Other"], -1], 
        ['noalg_L1Calo_EMPTY',           'L1_Calo_EMPTY',       [], ['L1Calo'],     ["RATE:SeededStreamers", "BW:Other"], -1], 
        # #['noalg_L1MinBias_EMPTY',  'L1_MinBias_EMPTY', [], ['L1MinBias'], [], -1], 

        # #minbias streamer
        ['noalg_mb_L1MBTS_2',     'L1_MBTS_2',        [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
        ['noalg_mb_L1MBTS_2_UNPAIRED_ISO',          'L1_MBTS_2_UNPAIRED_ISO',        [], ['MinBias'], ["BW:MinBias", "RATE:SeededStreamers"], -1],
        ['noalg_mb_L1RD0_EMPTY',           'L1_RD0_EMPTY',        [], ['express','MinBias'], ["BW:MinBias", "RATE:Cosmic_Minbias"], -1],
        ['noalg_mb_L1RD0_FILLED',          'L1_RD0_FILLED',        [], ['MinBias'], ["BW:MinBias", "RATE:Cosmic_Minbias"], -1],
        ['noalg_mb_L1RD0_UNPAIRED_ISO',    'L1_RD0_UNPAIRED_ISO', [], ['MinBias'], ["BW:Unpaired_Minbias", "RATE:Cosmic_Minbias"], -1],
        ['noalg_mb_L1RD1_EMPTY',           'L1_RD1_EMPTY',        [], ['MinBias'], ["BW:MinBias", "RATE:Cosmic_Minbias"], -1],
        ['noalg_mb_L1RD1_FILLED',          'L1_RD1_FILLED',        [], ['MinBias'], ["BW:MinBias", "RATE:Cosmic_Minbias"], -1],
        # L1 seed doesn't exits ['noalg_mb_L1RD1_UNPAIRED_ISO',  'L1_RD1_UNPAIRED_ISO', [], ['MinBias'], ["BW:Unpaired_Minbias", "RATE:Cosmic_Minbias"], -1],

        # id cosmic streamer
        ['noalg_idcosmic_L1TRT',        'L1_TRT',              [], ['IDCosmic'], ["BW:MinBias", "RATE:Minbias"], -1],

        # Cosmic calo streamer
        ['noalg_cosmiccalo_L1EM3_EMPTY',        'L1_EM3_EMPTY',        [], ['CosmicCalo'], ["BW:MinBias", "RATE:Cosmic_Minbias"], -1],        
        ['noalg_cosmiccalo_L1RD1_EMPTY',        'L1_RD1_EMPTY',        [], ['CosmicCalo'], ["BW:MinBias", "RATE:Cosmic_Minbias"], -1],        
        ['noalg_cosmiccalo_L1FJ30_EMPTY',       'L1_FJ30_EMPTY',       [], ['CosmicCalo'], ["BW:Jets", "RATE:SingleJet"], -1],
        ['noalg_cosmiccalo_L1J10_EMPTY',        'L1_J10_EMPTY',        [], ['CosmicCalo', 'express'], ["BW:Jets", "RATE:SingleJet"], -1],
        ['noalg_cosmiccalo_L1J30_EMPTY',        'L1_J30_EMPTY',        [], ['CosmicCalo', 'express'], ["BW:Jets", "RATE:SingleJet"], -1],
        ['noalg_cosmiccalo_L1J10_FIRSTEMPTY',   'L1_J10_FIRSTEMPTY',   [], ['CosmicCalo', 'express'], ["BW:Jets", "RATE:SingleJet"], -1],
        ['noalg_cosmiccalo_L1J30_FIRSTEMPTY',   'L1_J30_FIRSTEMPTY',   [], ['CosmicCalo', 'express'], ["BW:Jets", "RATE:SingleJet"], -1],

        #HLT pass through
        ['noalg_L1All',                'L1_All',              [], ['HLTPassthrough'], ["Supporting", "Calibration", "BW:Detector"], -1],
        ['noalg_L1J10',                'L1_J10',              [], ['HLTPassthrough'], ["Supporting", "Calibration", "BW:Detector"], -1],
        ['noalg_L1TAU8',               'L1_TAU8',             [], ['HLTPassthrough'], ["Supporting", "Calibration", "BW:Detector"], -1],
        ['noalg_L1EM3',                'L1_EM3',             [],  ['HLTPassthrough'], ["Supporting", "Calibration", "BW:Detector"], -1],        

        # jettauetmiss streamer
        ['noalg_jettauetmiss_L1RD0_FIRSTEMPTY',   'L1_RD0_FIRSTEMPTY',  [], ['Main'], ["Supporting", "Calibration", "BW:Detector"], -1],

        # Enhanced bias Physics item
        ['noalg_eb_L1RD3_FILLED',             'L1_RD3_FILLED',             [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1PhysicsLow_noPS',        'L1_PhysicsLow_noPS',        [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1PhysicsHigh_noPS',       'L1_PhysicsHigh_noPS',       [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ## Enhanced Bias Background ##
        ['noalg_eb_L1RD3_EMPTY',              'L1_RD3_EMPTY',              [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1EMPTY_noPS',             'L1_EMPTY_noPS',             [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1FIRSTEMPTY_noPS',        'L1_FIRSTEMPTY_noPS',        [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1UNPAIRED_ISO_noPS',      'L1_UNPAIRED_noPS',          [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1UNPAIRED_NONISO_noPS',   'L1_UNPAIRED_NONISO_noPS',   [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['noalg_eb_L1ABORTGAPNOTCALIB_noPS',  'L1_ABORTGAPNOTCALIB_noPS',  [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],

        #idmon streamer
        ['noalg_idmon_L1RD0_EMPTY',          'L1_RD0_EMPTY',        [], ['express','IDMonitoring'], ["RATE:DISCARD","BW:DISCARD"], -1],
        ['noalg_idmon_L1RD0_FILLED',         'L1_RD0_FILLED',        [], ['IDMonitoring'], ["RATE:DISCARD","BW:DISCARD"], -1],
        ['noalg_idmon_L1RD0_UNPAIRED_ISO',   'L1_RD0_UNPAIRED_ISO', [], ['IDMonitoring'], ["RATE:DISCARD","BW:DISCARD"], -1],

        #cosmicmuon streamer
        ['noalg_cosmicmuons_L1MU4_EMPTY',   'L1_MU4_EMPTY',        [], ['express','CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],        
        ['noalg_cosmicmuons_L1MU11_EMPTY',  'L1_MU11_EMPTY',        [], ['express','CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], -1],        

        ]

    TriggerFlags.MonitorSlice.signatures =  [
        ##CSC only monitoring chain
        ['cscmon_L1MU10',   'L1_MU10',     [], ['CSC'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['cscmon_L1EM3',    'L1_EM3',     [], ['CSC'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['cscmon_L1J10',    'L1_J10',     [], ['CSC'], ["RATE:Monitoring", "BW:Detector"], -1],

        # # enhancedbias
        ['timeburner',  '', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], -1],
        ['robrequest',  '', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], -1],
        ['robrequest_L1RD0_EMPTY', 'L1_RD0_EMPTY', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], -1],   

        ['costmonitor',  '', [], ['CostMonitoring'], ['Supporting','Calibration','BW:Other'],1],

        #['idmon_trkFS_L14J15', 'L1_4J15', [], ['Main'], ['RATE:InDetTracking', 'BW:Detector'], -1],
        ]
    #TriggerFlags.GenericSlice.signatures =  []

    TriggerFlags.BeamspotSlice.signatures =  [
        ##['beamspot_vtx_trkFS_L2StarB',         'L1_4J15', [], ['beamspot'], ["BW:Detector"], -1],
        ##['beamspot_vtx_activeTE_L2StarB_peb',  'L1_4J15', [], ['beamspot'], ["BW:Detector"], -1],
        ##['beamspot_vtx_allTE_L2StarB_peb',     'L1_4J15', [], ['beamspot'], ["BW:Detector"], -1],
        ]
    
    TriggerFlags.EnhancedBiasSlice.signatures = [
        ['eb_low_L1RD2_FILLED',               'L1_RD2_FILLED',             [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['eb_high_L1RD2_FILLED',              'L1_RD2_FILLED',             [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],    
    ]
    


    
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



