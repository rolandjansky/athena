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
    # ['chainName', chainCounter, 'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep]

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.MuonSlice.signatures = [
	#ID tracking performance
        ['mu4_idperf',          302, 'L1_MU4', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu6_idperf',          303, 'L1_MU4', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu10_idperf',         306, 'L1_MU6', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'], -1],
 	
	#RoI muons
	['mu6',          	304, 'L1_MU4', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'], -1],
	['mu10',                305, 'L1_MU6', [], ['Muon'], ['RATE:SingleMuon','BW:Muon'], -1],
	
	#Full scan
        ['mu4_mu4noL1',         300, 'L1_MU4', ['L1_MU4',''], ['Muon'], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,["mu4","mu4noL1"]]],
        ['mu6_mu4noL1',         301, 'L1_MU4', ['L1_MU4',''], ['Muon'], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,["mu6","mu4noL1"]]],
        ]

    TriggerFlags.JetSlice.signatures = [
	#performance
	['j0_perf_L1ZDC_A_C', 500,     'L1_ZDC_A_C',[], ['Jet'], ['RATE:SingleJet', 'BW:Jets'], -1],
	#physics
	['j15_L1ZDC_A_C',     501,     'L1_ZDC_A_C',[], ['Jet'], ['RATE:SingleJet', 'BW:Jets'], -1],   
	['j20_L1ZDC_A_C',     502,     'L1_ZDC_A_C',[], ['Jet'], ['RATE:SingleJet', 'BW:Jets'], -1],
        ]

    TriggerFlags.BjetSlice.signatures = [
        ]

    TriggerFlags.METSlice.signatures = [
        ]

    TriggerFlags.TauSlice.signatures = [
        ]

    TriggerFlags.EgammaSlice.signatures = [
	['e15_etcut',         400,          'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	#Run 1 PID
	['e15_loose1',        401,          'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_medium1',       402,          'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_loose',         403,          'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e15_medium',        404,          'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_lhloose',       405,          'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
	['e15_lhmedium',      406,          'L1_EM10', [], ['Egamma'], ['RATE:SingleElectron', 'BW:Egamma'], -1],
        ]

    TriggerFlags.BphysicsSlice.signatures = [
        ]

    TriggerFlags.CombinedSlice.signatures = [
        ]
   
    TriggerFlags.MinBiasSlice.signatures = [
        ['mb_mbts_L1MBTS_2', 100, 'L1_MBTS_2', [], ['MinBias'],["BW:MinBias", "RATE:MinBias"], 1],
        #['mb_mbts_L1MBTS_1_1', 101, 'L1_MBTS_1_1', [], ['MinBias'],["BW:MinBias", "RATE:MinBias"], 1], #does not work, needs to be fixed
        ['mb_mbts_L1MBTS_2_2', 102, 'L1_MBTS_2_2', [], ['MinBias'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_mbts_L1MBTS_3_3', 103, 'L1_MBTS_3_3', [], ['MinBias'],["BW:MinBias", "RATE:MinBias"], 1],
        ['mb_mbts_L1MBTS_4_4', 104, 'L1_MBTS_4_4', [], ['MinBias'],["BW:MinBias", "RATE:MinBias"], 1],
        ]

    TriggerFlags.CalibSlice.signatures   = []
    TriggerFlags.CosmicSlice.signatures  = [ ]
    TriggerFlags.StreamingSlice.signatures = [
        ['noalg_mb_L1TE50', 200, 'L1_TE50', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
        ['noalg_mb_L1MBTS_2', 201, 'L1_MBTS_2', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
        ['noalg_mb_L1MBTS_1_1', 202, 'L1_MBTS_1_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
        ['noalg_mb_L1MBTS_2_2', 203, 'L1_MBTS_2_2', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
	['noalg_mb_L1ZDC_A_C', 204, 'L1_ZDC_A_C', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
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

