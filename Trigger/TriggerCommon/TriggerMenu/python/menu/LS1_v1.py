# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
#------------------------------------------------------------------------#
def setupMenu():

    from TriggerMenu.menu.TriggerPythonConfig import TriggerPythonConfig
    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenu.menu.LS1_v1.py' )

    from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1 as tcl1
    if tcl1.current:
        log.info("L1 items: %s" % tcl1.current.menu.items.itemNames())
    else:
        log.info("ERROR L1 menu has not yet been defined")

    # INPUT FORMAT FOR CHAINS:
    # ['chainName', chainCounter, 'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep]

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.MuonSlice.signatures = [
        ['mu4_cosmic_L1MU4_EMPTY',                743, 'L1_MU4_EMPTY', [], ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], 1],
        ['mu4_cosmic_L1MU11_EMPTY',               744, 'L1_MU11_EMPTY', [], ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], 1],
        ['mu4_msonly_cosmic_L1MU11_EMPTY',        724, 'L1_MU11_EMPTY', [], ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], 1],
        
        ['mu4_cosmicEF_L1MU4_EMPTY',                745, 'L1_MU4_EMPTY', [], ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], 1],
        ['mu4_cosmicEF_L1MU11_EMPTY',               746, 'L1_MU11_EMPTY', [], ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], 1],
        ['mu4_msonly_cosmicEF_L1MU11_EMPTY',        747, 'L1_MU11_EMPTY', [], ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], 1],
        ['mu4_msonly_cosmicEF_L1MU4_EMPTY',        748, 'L1_MU4_EMPTY', [], ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], 1],

        #['mu4_cosmic_ds_L1MU4',                   725, 'L1_MU4',   [], ['DataScouting_01_CosmicMuons'], ["RATE:Cosmic_Muon_DS", "BW:Muon"], 1],
        #['mu4_cosmic_ds_L1MU10',                  726, 'L1_MU10',  [], ['DataScouting_01_CosmicMuons'], ["RATE:Cosmic_Muon_DS", "BW:Muon"], 1],
        #['mu4_cosmic_ds_L1MU11',                  727, 'L1_MU11',  [], ['DataScouting_01_CosmicMuons'], ["RATE:Cosmic_Muon_DS", "BW:Muon"], 1],
        #['mu4_cosmic_ds_L1MU4_EMPTY',             728, 'L1_MU4_EMPTY',  [], ['DataScouting_01_CosmicMuons'], ["RATE:Cosmic_Muon_DS", "BW:Muon"], 1],
        #['mu4_cosmic_ds_L1MU10_EMPTY',            729, 'L1_MU10_EMPTY',  [], ['DataScouting_01_CosmicMuons'], ["RATE:Cosmic_Muon_DS", "BW:Muon"], 1],
        #['mu4_cosmic_ds_L1MU11_EMPTY',            730, 'L1_MU11_EMPTY',  [], ['DataScouting_01_CosmicMuons'], ["RATE:Cosmic_Muon_DS", "BW:Muon"], 1],

        ['mu4_cosmic_ds1_L1MU4', 725, 'L1_MU4', [], ['DataScouting_01_CosmicMuons'], ["RATE:Cosmic_Muon_DS", "BW:Muon"], 1],
        ['mu4_cosmic_ds2_L1MU4', 726, 'L1_MU4', [], ['DataScouting_01_CosmicMuons','DataScouting_02_CosmicMuons'], ["RATE:Cosmic_Muon_DS", "BW:Muon"], 1],
        ]

    TriggerFlags.JetSlice.signatures = [
        # Performance chains
        ['j0_perf_L1RD0_EMPTY', 116,     'L1_RD0_EMPTY',[], ['Jet'], ['RATE:SingleJet', 'BW:Jets'], 1],
        ['j0_perf_L1MU10',      117,     'L1_MU10',[], ['Jet'], ['RATE:SingleJet', 'BW:Jets'], 1],
        ['j0_perf_L1J10',       118, 'L1_J10',[], ['Jet'], ['RATE:SingleJet', 'BW:Jets'], 1],
        #['j_perf_lcw_L1RD0_EMPTY', 118,'L1_RD0_EMPTY',[], ['Jet'], ['RATE:SingleJet', 'BW:Jets'], 1],
        #['j_perf_lcw_L1MU10', 119,     'L1_MU10',[], ['Jet'], ['RATE:SingleJet', 'BW:Jets'], 1],          
        ]

    TriggerFlags.METSlice.signatures = []
    TriggerFlags.TauSlice.signatures = []
    TriggerFlags.EgammaSlice.signatures = [
        ['e5_etcut',                120,          'L1_EM3', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['e5_etcut_L1EM3_EMPTY',    121,          'L1_EM3_EMPTY', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['g5_etcut',                122,          'L1_EM3', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ['g5_etcut_L1EM3_EMPTY',    123,          'L1_EM3_EMPTY', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], 1], 
        ]

    TriggerFlags.BphysicsSlice.signatures = [
        ['2mu4_bDimu',               338, 'L1_2MU4', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1], 
        ['2mu4_bDimu_novtx_noos',    339, 'L1_2MU4', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1],  
        ['2mu6_bDimu',               340, 'L1_2MU6', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1], 
        ['2mu6_bDimu_novtx_noos',    341, 'L1_2MU6', [], ['Bphysics'], ['RATE:Bphysics_MultiMuon','BW:Bphys'], 1],  
        ]
    TriggerFlags.CombinedSlice.signatures = []


    TriggerFlags.MinBiasSlice.signatures = [
        # LS1 chains
        ['mb_sptrk',                     806, 'L1_RD0_FILLED', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk_noisesup',            807, 'L1_RD0_FILLED', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk_L1RD0_UNPAIRED_ISO',  808, 'L1_RD0_UNPAIRED_ISO', [], ['MinBias'], ["BW:MinBias", "RATE:SeededStreamers"], 1],
        ['mb_sptrk_L1RD0_EMPTY',         809, 'L1_RD0_EMPTY', [], ['MinBias'], ["BW:MinBias", "RATE:Cosmic_Minbias"], 1],
        ##['mb_sp2000_trk70_hmt',          810, 'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:HMT"], 1], #disabled for M4 see https://savannah.cern.ch/bugs/?104744

        ['mb_perf_L1LUCID_UNPAIRED_ISO', 800, 'L1_LUCID_UNPAIRED_ISO',        [], ['MinBias'], ["BW:MinBias", "RATE:SeededStreamers"], 1],
        ['mb_perf_L1LUCID_EMPTY',        801, 'L1_LUCID_EMPTY',        [], ['MinBias'], ["BW:MinBias", "RATE:Cosmic_Minbias"], 1],
        ['mb_perf_L1LUCID',              802, 'L1_LUCID',        [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],

        ['mb_sptrk_costr',               836, 'L1_RD0_FILLED', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk_costr_L1RD0_EMPTY',   838, 'L1_RD0_EMPTY', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],

        ['mb_perf_L1MBTS_2',             799, 'L1_MBTS_2', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1], 
        ['mb_mbts_L1MBTS_2',             803, 'L1_MBTS_2', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1], 

        ]
    

    allL1TAUseeds = ",".join([ x for x in tcl1.current.menu.items.itemNames() if "TAU" in x ])

    TriggerFlags.CalibSlice.signatures   = [
        ['l1calocalib',             6416, 'L1_RD0_FILLED',     [], ['L1CaloCalib'], ["RATE:Calibration", "RATE:L1CaloCalib", "BW:Detector"], 1],
        ['l1calocalib_L1BGRP7',     6417, 'L1_BGRP7',          [], ['L1CaloCalib'], ["RATE:Calibration", "RATE:L1CaloCalib", "BW:Detector"], 1], 

        ['larcalib_L1EM3_EMPTY',    6418, 'L1_EM3_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], 1],
        ['larcalib_L1TAU8_EMPTY',   6419, 'L1_TAU8_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], 1],
        ['larcalib_L1J10_EMPTY',    6420, 'L1_J10_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], 1],
        ['larcalib_L1FJ30_EMPTY',   6421, 'L1_FJ30_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], 1],
        # DCM level monitoring chain for CSC (#104895)
        ['larcalib_L1MU10',   6422, 'L1_MU10',     [], ['calibration_CSC'], ["RATE:Calibration", "BW:Detector"], 1],
        ['larcalib_L1EM3',   6423, 'L1_EM3',     [], ['calibration_CSC'], ["RATE:Calibration", "BW:Detector"], 1],
        ['larcalib_L1J10',   6424, 'L1_J10',     [], ['calibration_CSC'], ["RATE:Calibration", "BW:Detector"], 1],

        ['tilelarcalib_L1EM3_EMPTY',    6430, 'L1_EM3_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], 1],
        ['tilelarcalib_L1TAU8_EMPTY',   6431, 'L1_TAU8_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], 1],
        ['tilelarcalib_L1J10_EMPTY',    6432, 'L1_J10_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], 1],
        ['tilelarcalib_L1FJ30_EMPTY',   6433, 'L1_FJ30_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], 1],
        ]
       

    TriggerFlags.CosmicSlice.signatures  =  [
        ['tilecalib_laser',  6500, 'L1_CALREQ2', [], ['Tile'], ["RATE:Calibration", "RATE:Cosmic_TileCalibration", "BW:Detector"], 1],
        ['pixel_noise',      6501, 'L1_RD0_EMPTY', [], ['PixelNoise'], ["RATE:Calibration", "RATE:PixelCalibration", "BW:Detector"], 1],
        ['pixel_beam',       6502, 'L1_RD0_FILLED', [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], 1],
        ['sct_noise',        6503, 'L1_RD0_EMPTY', [], ['SCTNoise'], ["RATE:Calibration", "RATE:SCTCalibration", "BW:Detector"], 1],
        
        # CosmicCalo
        ['larps_L1EM3_EMPTY',  5060, 'L1_EM3_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], 1],
        ['larps_L1EM6_EMPTY',  5061, 'L1_EM6_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], 1],
        ['larps_L1J10_EMPTY',  5062, 'L1_J10_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], 1],
        ['larps_L1J30_EMPTY',  5063, 'L1_J30_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], 1],
        ['larps_L1TAU8_EMPTY', 5064, 'L1_TAU8_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], 1],

        #JetTauEtmiss 
        ['larps_L1J10_FIRSTEMPTY', 5065, 'L1_J10_FIRSTEMPTY', [], ['JetTauEtmiss'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], 1],
        ['larps_L1J30_FIRSTEMPTY', 5066, 'L1_J30_FIRSTEMPTY', [], ['JetTauEtmiss'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], 1],

        #CosmicID
        ['id_cosmic_L1MU4',         5041, 'L1_MU4', [], ['IDCosmic', 'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], 1],
        ['id_cosmic_L1MU11',        5040, 'L1_MU11', [], ['IDCosmic', 'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], 1],

        ['id_cosmic',               5042, '', [], ['IDCosmic', 'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], 1],
        ['id_cosmic_trtxk',         5043, '', [], ['IDCosmic'], ['RATE:CosmicSlice', 'BW:Detector'], 1],
        ['id_cosmic_trtxk_central', 5044, '', [], ['IDCosmic'], ['RATE:CosmicSlice', 'BW:Detector'], 1],
        
        ['id_cosmic_ds',            5045, '', [], ['DataScouting_02_IDCosmic'], ['RATE:CosmicSlice_DS', 'RATE:Cosmic_Tracking_DS', 'BW:Detector'], 1],        

        ]


    TriggerFlags.StreamingSlice.signatures =  [
        #background streamers
        ['noalg_bkg_L1Bkg',              6550, 'L1_Bkg',              [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], 1], 
        ['noalg_bkg_L1RD0_UNPAIRED_ISO', 6551, 'L1_RD0_UNPAIRED_ISO', [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], 1], 
        ['noalg_bkg_L1RD0_EMPTY',        6552, 'L1_RD0_EMPTY',        [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], 1], 
        ['noalg_bkg_L1RD1_FILLED',       6549, 'L1_RD1_FILLED',       [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], 1], 

        # standby streamer
        ['noalg_L1Standby',              6553, 'L1_Standby',          [], ['Standby'],    ["RATE:SeededStreamers", "BW:Other"], 1], 
        
        # L1 streamers
        #disabled see #104204   ['noalg_L1Muon',                 6554, 'L1_Muon',             [], ['L1Muon'],     ["RATE:SeededStreamers", "BW:Other"], 1], 
        #disabled see #104204   ['noalg_L1Muon_EMPTY',           6557, 'L1_Muon_EMPTY',       [], ['L1Muon'],     ["RATE:SeededStreamers", "BW:Other"], 1], 

        ['noalg_L1Calo',                 6555, 'L1_Calo',             [], ['L1Calo'],     ["RATE:SeededStreamers", "BW:Other"], 1], 
        ['noalg_L1MinBias',              6556, 'L1_MinBias',          [], ['L1MinBias'],  ["RATE:SeededStreamers", "BW:Other"], 1], 
        ['noalg_L1Calo_EMPTY',           6558, 'L1_Calo_EMPTY',       [], ['L1Calo'],     ["RATE:SeededStreamers", "BW:Other"], 1], 
        # #['noalg_L1MinBias_EMPTY', 9559, 'L1_MinBias_EMPTY', [], ['L1MinBias'], [], 1], 

        # #minbias streamer
        ['noalg_mb_L1MBTS_2',         804, 'L1_MBTS_2',        [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
        ['noalg_mb_L1MBTS_2_UNPAIRED_ISO',         805, 'L1_MBTS_2_UNPAIRED_ISO',        [], ['MinBias'], ["BW:MinBias", "RATE:SeededStreamers"], 1],
        ['noalg_mb_L1RD0_EMPTY',         824, 'L1_RD0_EMPTY',        [], ['MinBias'], ["BW:MinBias", "RATE:Cosmic_Minbias"], 1],
        ['noalg_mb_L1RD0_FILLED',         825, 'L1_RD0_FILLED',        [], ['MinBias'], ["BW:MinBias", "RATE:Cosmic_Minbias"], 1],
        ['noalg_mb_L1RD0_UNPAIRED_ISO',  837, 'L1_RD0_UNPAIRED_ISO', [], ['MinBias'], ["BW:Unpaired_Minbias", "RATE:Cosmic_Minbias"], 1],
        ['noalg_mb_L1RD1_EMPTY',         829, 'L1_RD1_EMPTY',        [], ['MinBias'], ["BW:MinBias", "RATE:Cosmic_Minbias"], 1],
        ['noalg_mb_L1RD1_FILLED',         830, 'L1_RD1_FILLED',        [], ['MinBias'], ["BW:MinBias", "RATE:Cosmic_Minbias"], 1],
        # L1 seed doesn't exits ['noalg_mb_L1RD1_UNPAIRED_ISO',  831, 'L1_RD1_UNPAIRED_ISO', [], ['MinBias'], ["BW:Unpaired_Minbias", "RATE:Cosmic_Minbias"], 1],

        # id cosmic streamer
        ['noalg_idcosmic_L1TRT',         828, 'L1_TRT',              [], ['IDCosmic'], ["BW:MinBias", "RATE:Minbias"], 1],

        # Cosmic calo streamer
        ['noalg_cosmiccalo_L1RD1_EMPTY', 826, 'L1_RD1_EMPTY',        [], ['CosmicCalo'], ["BW:MinBias", "RATE:Cosmic_Minbias"], 1],        
        ['noalg_cosmiccalo_L1FJ30_EMPTY',   841, 'L1_FJ30_EMPTY',  [], ['CosmicCalo'], ["BW:Jets", "RATE:SingleJet"], 1],
        ['noalg_cosmiccalo_L1J10_EMPTY',   842, 'L1_J10_EMPTY',  [], ['CosmicCalo', 'express'], ["BW:Jets", "RATE:SingleJet"], 1],
        ['noalg_cosmiccalo_L1J30_EMPTY',   843, 'L1_J30_EMPTY',  [], ['CosmicCalo', 'express'], ["BW:Jets", "RATE:SingleJet"], 1],
        ['noalg_cosmiccalo_L1J10_FIRSTEMPTY',   844, 'L1_J10_FIRSTEMPTY',  [], ['CosmicCalo', 'express'], ["BW:Jets", "RATE:SingleJet"], 1],
        ['noalg_cosmiccalo_L1J30_FIRSTEMPTY',   845, 'L1_J30_FIRSTEMPTY',  [], ['CosmicCalo', 'express'], ["BW:Jets", "RATE:SingleJet"], 1],

        #HLT pass through
        ['noalg_L1All',                  834, 'L1_All',              [], ['HLTPassthrough'], ["Supporting", "Calibration", "BW:Detector"], 1],
        ['noalg_L1J10',                  835, 'L1_J10',              [], ['HLTPassthrough'], ["Supporting", "Calibration", "BW:Detector"], 1],
        ['noalg_L1TAU8',                 868, 'L1_TAU8',             [], ['HLTPassthrough'], ["Supporting", "Calibration", "BW:Detector"], 1],
        
        # jettauetmiss streamer
        ['noalg_jettauetmiss_L1RD0_FIRSTEMPTY',   840, 'L1_RD0_FIRSTEMPTY',  [], ['JetTauEtmiss'], ["Supporting", "Calibration", "BW:Detector"], 1],

        # Enhanced bias item
        ['noalg_eb_L1BGRP7',                  846, 'L1_BGRP7',              [], ['EnhancedBias'], ["Supporting", "Calibration", "BW:Detector"], 1],

        #idmon streamer
        ['noalg_idmon_L1RD0_EMPTY',         777, 'L1_RD0_EMPTY',        [], ['IDMonitoring'], ["RATE:DISCARD","BW:DISCARD"], 1],
        ['noalg_idmon_L1RD0_FILLED',         778, 'L1_RD0_FILLED',        [], ['IDMonitoring'], ["RATE:DISCARD","BW:DISCARD"], 1],
        ['noalg_idmon_L1RD0_UNPAIRED_ISO',  779, 'L1_RD0_UNPAIRED_ISO', [], ['IDMonitoring'], ["RATE:DISCARD","BW:DISCARD"], 1],

        #cosmicmuon streamer
        ['noalg_cosmicmuons_L1MU4_EMPTY',  128, 'L1_MU4_EMPTY',        [], ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], 1],        
        ['noalg_cosmicmuons_L1MU11_EMPTY', 129, 'L1_MU11_EMPTY',        [], ['CosmicMuons'], ["RATE:Cosmic_Muon", "BW:Muon"], 1],        

        ]

    TriggerFlags.MonitorSlice.signatures =  [
        # # enhancedbias
        ['timeburner', 6010, '', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], 1],
        ['robrequest', 6011, '', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], 1],
        ['robrequest_L1RD0_EMPTY', 6012, 'L1_RD0_EMPTY', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], 1],   

        ['costmonitor', 6013, '', [], ['CostMonitoring'], ['Supporting','Calibration','BW:Other'],1],

        #['idmon_trkFS_L14J15', 5514, 'L1_4J15', [], ['JetTauEtmiss'], ['RATE:InDetTracking', 'BW:Detector'], 1],
        ]
    #TriggerFlags.GenericSlice.signatures =  []

    TriggerFlags.BeamspotSlice.signatures =  [
        ##['beamspot_vtx_trkFS_L2StarB',        7000, 'L1_4J15', [], ['beamspot'], ["BW:Detector"], 1],
        ##['beamspot_vtx_activeTE_L2StarB_peb', 7001, 'L1_4J15', [], ['beamspot'], ["BW:Detector"], 1],
        ##['beamspot_vtx_allTE_L2StarB_peb',    7002, 'L1_4J15', [], ['beamspot'], ["BW:Detector"], 1],
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

