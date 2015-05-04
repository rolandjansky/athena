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


    # INPUT FORMAT FOR CHAINS:
    # ['chainName', 'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep]

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.MuonSlice.signatures = [
	#ID tracking performance
        ['mu4_idperf',          'L1_MU4', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu6_idperf',          'L1_MU4', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu10_idperf',         'L1_MU6', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'], -1],
 	
	#RoI muons
	['mu4',                 'L1_MU4', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu6',                 'L1_MU4', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu10',                'L1_MU6', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'], -1],
	
	#di-muons RoI based
	['2mu4',                 'L1_2MU4', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'], -1],

	#Full scan
        ['mu4_mu4noL1',         'L1_MU4', ['L1_MU4',''], ['Muon'], ['RATE:MultiMuon','BW:Muon'], -1, ['serial',-1,["mu4","mu4noL1"]]],
        ['mu6_mu4noL1',         'L1_MU4', ['L1_MU4',''], ['Muon'], ['RATE:MultiMuon','BW:Muon'], -1, ['serial',-1,["mu6","mu4noL1"]]],
        
        #Full-scan supporting
        ['mu4noL1',         'L1_MU4', ['L1_MU4',''], ['Muon'], ['RATE:SingleMuon','BW:Muon'], -1, ],
        ['mu6noL1',         'L1_MU4', ['L1_MU4',''], ['Muon'], ['RATE:SingleMuon','BW:Muon'], -1, ],

        ]

    TriggerFlags.JetSlice.signatures = [
	#performance
	['j0_perf_L1ZDC_A_C',     'L1_ZDC_A_C',[], ['Jet'], ['RATE:SingleJet', 'BW:Jets'], -1],
	#physics
	['j15_L1ZDC_A_C',         'L1_ZDC_A_C',[], ['Jet'], ['RATE:SingleJet', 'BW:Jets'], -1],   
	['j20_L1ZDC_A_C',         'L1_ZDC_A_C',[], ['Jet'], ['RATE:SingleJet', 'BW:Jets'], -1],
        ]

    TriggerFlags.BjetSlice.signatures = [
        ]

    TriggerFlags.METSlice.signatures = [
        ]

    TriggerFlags.TauSlice.signatures = [
        ]

    TriggerFlags.EgammaSlice.signatures = [
	['e15_etcut',                  'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
       	['g15_etcut',                  'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['g20_etcut',                  'L1_EM12', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	#Run 1 PID
	['e15_loose1',                 'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_medium1',                'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_loose',                  'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_medium',                 'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_lhloose',                'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_lhmedium',               'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	#heavy ion instances	
        ['e15_etcut_ion',              'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['g15_etcut_ion',              'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['g20_etcut_ion',              'L1_EM12', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	['e15_loose1_ion',             'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_medium1_ion',            'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_loose_ion',              'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_medium_ion',             'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_lhloose_ion',            'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_lhmedium_ion',           'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
 
	#backup for e15
	['e20_etcut_ion',              'L1_EM12', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1], 
	['e20_loose_ion',              'L1_EM12', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e20_loose1_ion',             'L1_EM12', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e20_lhloose_ion',            'L1_EM12', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e20_etcut',                  'L1_EM12', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e20_loose',                  'L1_EM12', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e20_loose1',                 'L1_EM12', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e20_lhloose',                'L1_EM12', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	#di-electrons , timeouts ATR-10226
	#['2e10_loose_ion',              'L1_2EM5', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	#['2e10_loose1_ion',             'L1_2EM5', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	#['2e10_lhloose_ion',            'L1_2EM5', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	['g15_loose',                  'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['g15_medium',                 'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['g20_loose',                  'L1_EM12', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['g20_medium',                 'L1_EM12', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	['g15_loose_ion',              'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['g15_medium_ion',             'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['g20_loose_ion',              'L1_EM12', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['g20_medium_ion',             'L1_EM12', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],

	#di-photon triggers
	['2g15_loose',                  'L1_2EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['2g15_loose_ion',              'L1_2EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ]

    TriggerFlags.BphysicsSlice.signatures = [
        ]

    TriggerFlags.CombinedSlice.signatures = [
        ]
   
    TriggerFlags.HeavyIonSlice.signatures = [
        ['hi_v2_th5_L1TE50',    'L1_TE50',   [], ['MinBias'],["BW:HeavyIon", "RATE:HeavyIon"], 1],
        ['hi_v3_th5_L1TE50',    'L1_TE50',   [], ['MinBias'],["BW:HeavyIon", "RATE:HeavyIon"], 1],
        ['hi_v2_th10_L1TE50',    'L1_TE50',   [], ['MinBias'],["BW:HeavyIon", "RATE:HeavyIon"], 1],
        ['hi_v3_th10_L1TE50',    'L1_TE50',   [], ['MinBias'],["BW:HeavyIon", "RATE:HeavyIon"], 1],
        ['hi_v2_th14_L1TE50',    'L1_TE50',   [], ['MinBias'],["BW:HeavyIon", "RATE:HeavyIon"], 1],
        ['hi_v3_th14_L1TE50',    'L1_TE50',   [], ['MinBias'],["BW:HeavyIon", "RATE:HeavyIon"], 1],
        ['hi_v2_th15_L1TE50',    'L1_TE50',   [], ['MinBias'],["BW:HeavyIon", "RATE:HeavyIon"], 1],
        ['hi_v3_th15_L1TE50',    'L1_TE50',   [], ['MinBias'],["BW:HeavyIon", "RATE:HeavyIon"], 1],
        ]
    
    TriggerFlags.MinBiasSlice.signatures = [
        ['mb_mbts_L1MBTS_2',    'L1_MBTS_2',   [], ['MinBias'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_mbts_L1MBTS_1_1',  'L1_MBTS_1_1', [], ['MinBias'],["BW:MinBias", "RATE:MinBias"], 1], 
        ['mb_mbts_L1MBTS_2_2',  'L1_MBTS_2_2', [], ['MinBias'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_mbts_L1MBTS_3_3',  'L1_MBTS_3_3', [], ['MinBias'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_mbts_L1MBTS_4_4',  'L1_MBTS_4_4', [], ['MinBias'],["BW:MinBias", "RATE:MinBias"], 1],

	#sptrk
        ['mb_sp',  'L1_RD0_FILLED', [], ['MinBias'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_sptrk_ion_L1ZDC_A_C_VTE50',  'L1_ZDC_A_C_VTE50', [], ['MinBias'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_idperf_ion_L1MBTS_1', 'L1_MBTS_1', [], ['MinBias'],["BW:MinBias", "RATE:MinBias"], 1],
        
        #Zdc
	['mb_lg_zdcperf',  'L1_RD0_FILLED', [], ['MinBias'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_lg_zdcperf_L1ZDC_A_C',  'L1_ZDC_A_C', [], ['MinBias'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_hg_zdcperf',  'L1_RD0_FILLED', [], ['MinBias'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_hg_zdcperf_L1ZDC_A_C',  'L1_ZDC_A_C', [], ['MinBias'],["BW:MinBias", "RATE:MinBias"], 1],
        ]

    TriggerFlags.CalibSlice.signatures   = []
    TriggerFlags.CosmicSlice.signatures  = [ ]
    TriggerFlags.StreamingSlice.signatures = [
        ['noalg_mb_L1TE50',      'L1_TE50', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
        ['noalg_mb_L1MBTS_2',    'L1_MBTS_2', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
        ['noalg_mb_L1MBTS_1_1',  'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
        ['noalg_mb_L1MBTS_2_2',  'L1_MBTS_2_2', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1ZDC_A_C',   'L1_ZDC_A_C', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
        ]

    TriggerFlags.MonitorSlice.signatures = []
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

