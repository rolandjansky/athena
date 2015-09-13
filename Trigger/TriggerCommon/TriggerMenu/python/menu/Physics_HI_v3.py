# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
#------------------------------------------------------------------------#
def setupMenu():
    
    from TriggerMenu.menu.TriggerPythonConfig import TriggerPythonConfig
    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenu.menu.Physics_HI_v3.py' )

    from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1 as tcl1
    
    if tcl1.current:
        log.info("L1 items: %s " % tcl1.current.menu.items.itemNames())
    else:
        log.info("ERROR L1 menu has not yet been defined")

    HardProbesStream="HardProbes"
    MinBiasStream="MinBias"

    # INPUT FORMAT FOR CHAINS:
    # ['chainName', 'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep]

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.TestSlice.signatures = [] 
    
    TriggerFlags.MuonSlice.signatures = [
	#ID tracking performance
        ['mu4_idperf',          'L1_MU4', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu6_idperf',          'L1_MU4', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu10_idperf',         'L1_MU6', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],

	#RoI muons
	['mu4',                 'L1_MU4', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu6',                 'L1_MU4', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu10',                'L1_MU6', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu14',                'L1_MU11', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],

	#di-muons RoI based
	['2mu4',                 'L1_2MU4', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],
	['2mu6',                 'L1_2MU4', [], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1],

	#Full scan
        ['mu4_mu4noL1',         'L1_MU4', ['L1_MU4',''], [HardProbesStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial',-1,["mu4","mu4noL1"]]],
        ['mu6_mu4noL1',         'L1_MU4', ['L1_MU4',''], [HardProbesStream], ['RATE:MultiMuon','BW:Muon'], -1, ['serial',-1,["mu6","mu4noL1"]]],
        
        #Full-scan supporting
        ['mu4noL1',         'L1_MU4', ['L1_MU4',''], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1, ],
        ['mu6noL1',         'L1_MU4', ['L1_MU4',''], [HardProbesStream], ['RATE:SingleMuon','BW:Muon'], -1, ],

        # muon calibration
        ['mu0_muoncalib',             'L1_MU20',     [],['Muon_Calibration'],["RATE:Calibration","BW:Muon"],-1],
        ['mu0_muoncalib_L1MU15',      'L1_MU15',     [],['Muon_Calibration'],["RATE:Calibration","BW:Muon"],-1],
        ['mu0_muoncalib_L1MU4_EMPTY', 'L1_MU4_EMPTY',[],['Muon_Calibration'],["RATE:Calibration","BW:Muon"],-1],
        
        
        ]

    TriggerFlags.JetSlice.signatures = [
	#performance
	['j0_perf_L1ZDC_A_C',     'L1_ZDC_A_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jets'], -1],
	['j0_perf_L1RD0_FILLED',     'L1_RD0_FILLED',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jets'], -1],

	#physics
	['j15_L1ZDC_A_C',         'L1_ZDC_A_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jets'], -1],   
	['j20_L1ZDC_A_C',         'L1_ZDC_A_C',[], [HardProbesStream], ['RATE:SingleJet', 'BW:Jets'], -1],
        ]

    TriggerFlags.BjetSlice.signatures = [
        ]

    TriggerFlags.METSlice.signatures = [
        ]

    TriggerFlags.TauSlice.signatures = [
        ]

    TriggerFlags.EgammaSlice.signatures = [
	['e15_etcut',                  'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
       	['g15_etcut',                  'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['g20_etcut',                  'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	#Run 1 PID
	['e15_loose1',                 'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_medium1',                'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_loose',                  'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_medium',                 'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_lhloose',                'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_lhmedium',               'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	#heavy ion instances	
        ['e15_etcut_ion',              'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['g15_etcut_ion',              'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['g20_etcut_ion',              'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	['e15_loose1_ion',             'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_medium1_ion',            'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_loose_ion',              'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_medium_ion',             'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_lhloose_ion',            'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_lhmedium_ion',           'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
 
	#backup for e15
	['e20_etcut_ion',              'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
	['e20_loose_ion',              'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e20_loose1_ion',             'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e20_lhloose_ion',            'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e20_etcut',                  'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e20_loose',                  'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e20_loose1',                 'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e20_lhloose',                'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	#di-electrons , timeouts ATR-10226
	#['2e10_loose_ion',              'L1_2EM5', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	#['2e10_loose1_ion',             'L1_2EM5', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	#['2e10_lhloose_ion',            'L1_2EM5', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	['g15_loose',                  'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['g15_medium',                 'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['g20_loose',                  'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['g20_medium',                 'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	['g15_loose_ion',              'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['g15_medium_ion',             'L1_EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['g20_loose_ion',              'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['g20_medium_ion',             'L1_EM12', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	#di-photon triggers
	['2g15_loose',                  'L1_2EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['2g15_loose_ion',              'L1_2EM10', [], [HardProbesStream], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ]

    TriggerFlags.BphysicsSlice.signatures = [
        ]

    TriggerFlags.CombinedSlice.signatures = [
        ]
   
    TriggerFlags.HeavyIonSlice.signatures = [
	#event shape triggers
        ['hi_v2_th5_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3_th5_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2_th10_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3_th10_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2_th13_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3_th13_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2_th14_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3_th14_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2_th15_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3_th15_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v2_th16_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],
        ['hi_v3_th16_L1TE50',    'L1_TE50',   [], [MinBiasStream],["BW:EventShape", "RATE:EventShape"], 1],

	#ultra-central triggers
	#physics
        ['hi_fcalet3000_ucc_L1TE10000',    'L1_TE10000',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
	['hi_fcalet3306_ucc_L1TE10000',    'L1_TE10000',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
	['hi_fcalet3391_ucc_L1TE10000',    'L1_TE10000',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
	['hi_fcalet3516_ucc_L1TE10000',    'L1_TE10000',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
	#performance/calibration
	['hi_perf_ucc_L1TE10000',    'L1_TE10000',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
	['hi_perf_ucc_L1ZDC_A_C',    'L1_ZDC_A_C',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        ['hi_perf_ucc_L1All',    '',   [], [MinBiasStream],["BW:UltraCentral", "RATE:UltraCentral"], 1],
        ]
    
    TriggerFlags.MinBiasSlice.signatures = [
        ['mb_mbts_L1MBTS_2',    'L1_MBTS_2',   [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_mbts_L1MBTS_1_1',  'L1_MBTS_1_1', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1], 
        ['mb_mbts_L1MBTS_2_2',  'L1_MBTS_2_2', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_mbts_L1MBTS_3_3',  'L1_MBTS_3_3', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_mbts_L1MBTS_4_4',  'L1_MBTS_4_4', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],

	#sptrk
        ['mb_sp',  'L1_RD0_FILLED', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk_ion_L1ZDC_A_C_VTE50',  'L1_ZDC_A_C_VTE50', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_idperf_ion_L1MBTS_1', 'L1_MBTS_1', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        
	['mb_sptrk_ion_L1ZDC_A_C_VTE50_OVERLAY',  'L1_ZDC_A_C_VTE50_OVERLAY', [], ['MinBiasOverlay'],["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],

        #Zdc
	['mb_lg_zdcperf_L1All',  '', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_lg_zdcperf_L1ZDC_A_C',  'L1_ZDC_A_C', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_hg_zdcperf_L1All',  '', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_hg_zdcperf_L1ZDC_A_C',  'L1_ZDC_A_C', [], [MinBiasStream],["BW:MinBias", "RATE:MinBias"], 1],
        ]

    TriggerFlags.CalibSlice.signatures   = [
        ['idcalib_trk9_central',    'L1_TAU', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ['idcalib_trk9_fwd',        'L1_TAU', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ['idcalib_trk16_central',   'L1_TAU', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ['idcalib_trk16_fwd',       'L1_TAU', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ['idcalib_trk29_central',   'L1_TAU', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],
        ['idcalib_trk29_fwd',       'L1_TAU', [], ['IDTracks'], [ 'RATE:IDTracks','BW:Detector'], -1],

        ['lumipeb_L1RD0_EMPTY',        'L1_RD0_EMPTY', [], ['PixelNoise'], ["RATE:Calibration", "RATE:PixelCalibration", "BW:Detector"], -1],
        ['lumipeb_L1RD0_FILLED',         'L1_RD0_FILLED', [], ['PixelBeam'], ["RATE:Calibration", "RATE:PixelBeamCalibration", "BW:Detector"], -1],
        ['lumipeb_L1RD0_UNPAIRED_ISO',    'L1_RD0_UNPAIRED_ISO',    [], ['PixelBeam'], ["RATE:Calibration", "BW:Detector"], -1],
        ['lumipeb_L1RD0_ABORTGAPNOTCALIB',    'L1_RD0_ABORTGAPNOTCALIB',    [], ['PixelBeam'], ["RATE:Calibration", "BW:Detector"], -1],
        
        # Monitoritems for saturated towers
        ['satu20em_l1satmon_L1J100_FIRSTEMPTY',  'L1_J100_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['satu20em_l1satmon_L1J100.31ETA49_FIRSTEMPTY',  'L1_J100.31ETA49_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['satu20em_l1satmon_L1EM20VH_FIRSTEMPTY',  'L1_EM20VH_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        #['satu20em_l1satmon_L1EM20VHI_FIRSTEMPTY',  'L1_EM20VHI_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1], 
        # the one above was replaced with a em22vhi_firstempty seeded one:
        #['satu20em_l1satmon_L1EM22VHI_FIRSTEMPTY',  'L1_EM22VHI_FIRSTEMPTY',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1], # for HI, L1EM20VH one is OK?
        ['satu20em_l1satmon_L1J100',  'L1_J100',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['satu20em_l1satmon_L1J100.31ETA49',  'L1_J100.31ETA49',     [], ['L1Calo'], ['RATE:Calibration', 'BW:Detector'],-1],                 
        # larnoiseburst
        ['larnoiseburst_L1XE35',            'L1_XE35',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1XE50',            'L1_XE50',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        #['larnoiseburst_L1XE80', 'L1_XE80', [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1], # for HI, XE50 is OK?
        ['larnoiseburst_L1J50',             'L1_J50',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1J75',             'L1_J75',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1J100', 'L1_J100', [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_L1J40_XE50',            'L1_J40_XE50',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1], 
        ['larnoiseburst_L1J75_XE40',            'L1_J75_XE40',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1], 

        # larpeb
        ['g3_loose_larpeb',                      'L1_EM3',     [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g12_loose_larpeb',                      'L1_EM10VH',     [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_loose_larpeb_L1EM15',              'L1_EM15', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g40_loose_larpeb',                     'L1_EM20VHI', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g60_loose_larpeb',                     'L1_EM20VHI', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g80_loose_larpeb',                     'L1_EM20VHI', [], ['LArCells'], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['conej40_larpebj',  'L1_J12',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['conej165_larpebj', 'L1_J75',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['conej75_320eta490_larpebj',  'L1_J30.31ETA49',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],
        ['conej140_320eta490_larpebj', 'L1_J75.31ETA49',     [], ['LArCells'], ['RATE:Calibration', 'BW:Detector'],-1],
        
        # larcalib
        ['larcalib_L1EM3_EMPTY',    'L1_EM3_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1EM7_EMPTY',    'L1_EM7_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1TAU8_EMPTY',   'L1_TAU8_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1J12_EMPTY',    'L1_J12_EMPTY',      [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larcalib_L1J3031ETA49_EMPTY',  'L1_J30.31ETA49_EMPTY',     [], ['LArCellsEmpty'], ["RATE:Calibration", "BW:Detector"], -1],
        
        #Special re-run chains
        ['larnoiseburst_rerun',                  '',             [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_loose_rerun',            '',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],          
        
        ]
    
    TriggerFlags.CosmicSlice.signatures  = [        
        ['tilecalib_laser',   'L1_CALREQ2', [], ['Tile'], ["RATE:Calibration", "RATE:Cosmic_TileCalibration", "BW:Detector"], -1], 
        ['sct_noise',          'L1_RD0_EMPTY', [], ['SCTNoise'], ["RATE:Calibration", "RATE:SCTCalibration", "BW:Detector"], -1],
        # CosmicID
        ['id_cosmicid_L1MU4_EMPTY',         'L1_MU4_EMPTY', [],  ['HLT_IDCosmic', 'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],
        ['id_cosmicid_L1MU11_EMPTY',        'L1_MU11_EMPTY', [], ['HLT_IDCosmic', 'express'], ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking', 'BW:Detector'], -1],

        # CosmicCalo
        ['larps_L1EM3_EMPTY',  'L1_EM3_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1EM7_EMPTY',  'L1_EM7_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J12_EMPTY',  'L1_J12_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J30_EMPTY',  'L1_J30_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1TAU8_EMPTY', 'L1_TAU8_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],        
        ['larps_L1J3031ETA49_EMPTY', 'L1_J30.31ETA49_EMPTY', [], ['CosmicCalo'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J12_FIRSTEMPTY', 'L1_J12_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larps_L1J30_FIRSTEMPTY', 'L1_J30_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1], 
        ['larps_L1EM7_FIRSTEMPTY',  'L1_EM7_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1], 
        
        ['larhec_L1J12_FIRSTEMPTY', 'L1_J12_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larhec_L1J30_FIRSTEMPTY', 'L1_J30_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larhec_L1EM7_FIRSTEMPTY', 'L1_EM7_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],
        ['larhec_L1TAU8_FIRSTEMPTY', 'L1_TAU8_FIRSTEMPTY', [], ['LArCellsEmpty'], ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector'], -1],


        
        
        ]
    TriggerFlags.StreamingSlice.signatures = [
	#TE full calo
	['noalg_mb_L1TE5',      'L1_TE5', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE20',      'L1_TE20', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
        ['noalg_mb_L1TE50',      'L1_TE50', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE100',      'L1_TE100', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE200',      'L1_TE200', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE10000',      'L1_TE10000', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE12000',      'L1_TE12000', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE14000',      'L1_TE14000', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],

	#TE restricted calo
        ['noalg_mb_L1TE3.0ETA24',      'L1_TE3.0ETA24', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE5.0ETA24',      'L1_TE5.0ETA24', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE10.0ETA24',      'L1_TE10.0ETA24', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE20.0ETA24',      'L1_TE20.0ETA24', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1TE30.0ETA24',      'L1_TE30.0ETA24', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],

	#min bias
        ['noalg_mb_L1MBTS_2',    'L1_MBTS_2', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
        ['noalg_mb_L1MBTS_1_1',  'L1_MBTS_1_1', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
        ['noalg_mb_L1MBTS_2_2',  'L1_MBTS_2_2', [], [MinBiasStream], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1ZDC_A_C',   'L1_ZDC_A_C', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],

	#beam spot
	['noalg_L1J10_VTE100',   'L1_J10_VTE100', [], [HardProbesStream], ["BW:Jets", "RATE:Jets"], 1],
	['noalg_L1J10_VTE200',   'L1_J10_VTE200', [], [HardProbesStream], ["BW:Jets", "RATE:Jets"], 1],

	#jets
	['noalg_L1J5',   'L1_J5', [], [HardProbesStream], ["BW:Jets", "RATE:Jets"], 1],
	['noalg_L1J10',   'L1_J10', [], [HardProbesStream], ["BW:Jets", "RATE:Jets"], 1],
	['noalg_L1J15',   'L1_J15', [], [HardProbesStream], ["BW:Jets", "RATE:Jets"], 1],
        ['noalg_L1J20',   'L1_J20', [], [HardProbesStream], ["BW:Jets", "RATE:Jets"], 1],
        ['noalg_L1J30',   'L1_J30', [], [HardProbesStream], ["BW:Jets", "RATE:Jets"], 1],
	['noalg_L1J50',   'L1_J50', [], [HardProbesStream], ["BW:Jets", "RATE:Jets"], 1],
	['noalg_L1J75',   'L1_J75', [], [HardProbesStream], ["BW:Jets", "RATE:Jets"], 1],
	['noalg_L1J175',   'L1_J175', [], [HardProbesStream], ["BW:Jets", "RATE:Jets"], 1],

	#data overlay
	['noalg_L1ZDC_A_C_OVERLAY',   'L1_ZDC_A_C_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],
	['noalg_L1TE50_OVERLAY',   'L1_TE50_OVERLAY', [], ['MinBiasOverlay'], ["BW:MinBiasOverlay", "RATE:MinBiasOverlay"], 1],

        ['noalg_L1TGC_BURST',   'L1_TGC_BURST', [], ['TgcNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],

        #idmon streamer
        ['noalg_idmon_L1RD0_EMPTY',          'L1_RD0_EMPTY',        [], ['IDMonitoring','express'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['noalg_idmon_L1RD0_FILLED',         'L1_RD0_FILLED',        [], ['IDMonitoring'], ["RATE:Monitoring", "BW:Detector"], -1],
        ['noalg_idmon_L1RD0_UNPAIRED_ISO',   'L1_RD0_UNPAIRED_ISO', [], ['IDMonitoring'], ["RATE:Monitoring", "BW:Detector"], -1],

        # Cosmic calo streamer
        ['noalg_cosmiccalo_L1EM3_EMPTY',         'L1_EM3_EMPTY',        [], ['CosmicCalo'], ["BW:MinBias", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1RD1_EMPTY',         'L1_RD1_EMPTY',        [], ['CosmicCalo', 'express'], ["RATE:Calibration", "BW:Detector"], -1],
        ['noalg_cosmiccalo_L1J3031ETA49_EMPTY',  'L1_J30.31ETA49_EMPTY',[], ['CosmicCalo'], ["BW:Jets", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1J12_EMPTY',         'L1_J12_EMPTY',        [], ['CosmicCalo', 'express'], ["BW:Jets", "RATE:Cosmic_Calo"], -1],
        ['noalg_cosmiccalo_L1J30_EMPTY',         'L1_J30_EMPTY',        [], ['CosmicCalo', 'express'], ["BW:Jets", "RATE:Cosmic_Calo"], -1],

        #background streamers
        ['noalg_bkg_L1Bkg',               'L1_Bkg',               [], ['Background'], ["RATE:SeededStreamers", "BW:Other"], -1],  

        #DCM level monitoring test chain
        ['noalg_dcmmon_L1RD0_EMPTY',   'L1_RD0_EMPTY',        [], ['monitoring_random'], ["RATE:DISCARD","BW:DISCARD"], -1],
        
        ]

    TriggerFlags.BeamspotSlice.signatures = [
        ['beamspot_activeTE_trkfast_peb',           'L1_J10_VTE100', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_allTE_trkfast_pebTRT',           'L1_J10_VTE100', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_trkFS_trkfast',                  'L1_J10_VTE100', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

	['beamspot_activeTE_trkfast_peb_L1J10_VTE200',           'L1_J10_VTE200', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_allTE_trkfast_pebTRT_L1J10_VTE200',           'L1_J10_VTE200', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
        ['beamspot_trkFS_trkfast_L1J10_VTE200',                  'L1_J10_VTE200', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
	]

    TriggerFlags.MonitorSlice.signatures = [
        ['timeburner', '', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], -1],
        ['robrequest', '', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], -1],
        ['robrequest_L1RD0_EMPTY', 'L1_RD0_EMPTY', [], ['DISCARD'], ["RATE:DISCARD","BW:DISCARD"], -1], 
        
        ['cscmon_L1All', 'L1_All',         [], ['CSC'], ["RATE:Monitoring", "BW:Detector"], -1],

        ['costmonitor', '', [], ['CostMonitoring'], ['RATE:Monitoring','BW:Other'],1],
        
        ]
    #TriggerFlags.GenericSlice.signatures = []
    
class Prescales:
    #   Item name             | Prescale
    #----------------------------------------------------------
    L1Prescales = {

        }
    HLTPrescales = {
        }


# StreamConfig = {
#     'HardProbes':      [ 'egamma',
#                      'comb__ej', 'comb__emet', 'comb__ejmet', 'comb__eg',
#                      'comb__gmet', 
#                      'comb__emu', 'comb__gmu',
#                      'comb_tauej', 
#                      ], 
#     'MinBias':     [ 'minbias' ], 
#     'Background':  [ 'background' ], 

#     'HardProbes':       [ 'muons', 'bphysics',
#                      'comb__muj', 'comb__mumet', 'comb__mujmet',
#                      #'comb__emu', 'comb__gmu',
#                      'comb_taumuj','comb__mue', 'comb__mug',
#                      'comb__multimu',
#                      ], 
#     'HardProbes':   [ 'jets', 'tauEtmiss',
#                         'comb__tauj', 'comb__taumet', 'comb__jmet',
#                         'comb__taujmet', 'comb__taub', 'comb__taue', 
#                         'comb__taue', 'comb__taumu', 'comb__fjj', 
#                         'comb__tauej', 'comb__taumuj', ],
#     # New delayed streams for Jet triggers
#     'HadDelayed':  [ 'haddelayed' ],
#     'JetCalibDelayed':  [ 'jetcalibdelayed' ],
#     'Standby': [ 'standby' ], 
#     'calibration': [ 'calibration' ], 
#     'express':     [ 'express' ],

#     }

