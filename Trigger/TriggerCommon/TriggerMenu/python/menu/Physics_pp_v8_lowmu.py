# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
#------------------------------------------------------------------------#
def setupMenu():

    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenu.menu.Physics_pp_v8_lowmu.py' )

    from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1 as tcl1
    if tcl1.current:
        log.info("L1 items: %s " % tcl1.current.menu.items.itemNames())
    else:
        log.info("ERROR L1 menu has not yet been defined")

    PhysicsStream="Main"

    # stream, BW and RATE tags for Bphysics items that appear in Muon and Bphysics slice.signatures
    BPhysicsStream     = "BphysLS"
    #BMultiMuonStream   = "Main"  
    RATE_BphysTag      = 'RATE:Bphysics'
    RATE_BMultiMuonTag = 'RATE:BMultiMuon'  # can become RATE:BMultiMuon' with one letter change 
    BW_BphysTag        = 'BW:Bphys'
    #RATE_DYTag         = 'RATE:Muon'   
    #BW_DYTag           = 'BW:Muon'   

    #---------------------------------------------------------------------
    # INPUT FORMAT FOR CHAINS:
    # ['chainName',  'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep], OPTIONAL: [mergingStrategy, offset,[mergingOrder] ]], topoStartsFrom = False
    #---------------------------------------------------------------------

    #---------------------------------------------------------------------
    # if it's needed to temporary remove almost all the chains from the menu
    # be aware that it is necessary to leave at least one chain in the muon slice
    # otherwise athenaHLT will seg-fault 
    #---------------------------------------------------------------------

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.TestSlice.signatures = []

    TriggerFlags.AFPSlice.signatures = []

    TriggerFlags.MuonSlice.signatures = [
        ['mu26_ivarmedium',	     'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream, 'express'], ['Legacy:Backup','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu50',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Legacy:Backup','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu60',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Legacy:Backup','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu26',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Legacy:Backup','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu22',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Legacy:Backup','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Legacy:Backup','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream, 'express'], ['Legacy:Backup','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20_idperf',            'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream, 'express'], ['Legacy:Support','RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
        ['mu6_idperf',             'L1_MU6',            [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],

        ['mu4_nomucomb',                   'L1_MU4',           [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu6_nomucomb',                   'L1_MU6',           [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleMuon', 'BW:Muon'], -1],

        #Low-mu run triggers
        ['mu14',                   'L1_MU10',           [], [PhysicsStream], ['Legacy:Primary','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu15',     'L1_MU11',           [], [PhysicsStream], ['Legacy:Backup','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu16',     'L1_MU11',           [], [PhysicsStream], ['Legacy:Backup','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu18',     'L1_MU11',           [], [PhysicsStream], ['Legacy:Backup','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu15_L1MU10',                   'L1_MU10',           [], [PhysicsStream,'express'], ['Legacy:Backup','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu18_L1MU10',                   'L1_MU10',           [], [PhysicsStream], ['Legacy:Backup','RATE:SingleMuon', 'BW:Muon'], -1],
        ['2mu4',                 'L1_2MU4', [], [PhysicsStream,'express'], ['Legacy:Primary',RATE_BMultiMuonTag, BW_BphysTag], -1],
        ['2mu6',                 'L1_2MU6', [], [PhysicsStream,'express'], ['Legacy:Backup',RATE_BMultiMuonTag, BW_BphysTag], -1],
        ['mu4_mu2noL1_L1MU4', 'L1_MU4',  ['L1_MU4',''], [PhysicsStream], ['Legacy:Support','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu4','mu2noL1']]],
        ['mu6_mu2noL1_L1MU6', 'L1_MU6',  ['L1_MU6',''], [PhysicsStream], ['Legacy:Support','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu6','mu2noL1']]],
            ]

    TriggerFlags.JetSlice.signatures = [   
        #Low-mu run triggers
        ['j0_perf_ds1_L1J15',      'L1_J15',  [], ['DataScouting_05_Jets'], ['Legacy:Support','RATE:Jets_DS', 'BW:Jet'], -1],
        ['j0_perf_ds1_L1J20',      'L1_J20',  [], ['DataScouting_05_Jets'], ['Legacy:Primary','RATE:Jets_DS', 'BW:Jet'], -1],
        ['j0_perf_ds1_L1J30',      'L1_J30',  [], ['DataScouting_05_Jets'], ['Legacy:Backup','RATE:Jets_DS', 'BW:Jet'], -1],
        ['j0_perf_ds1_L1J40',      'L1_J40',  [], ['DataScouting_05_Jets'], ['Legacy:Backup','RATE:Jets_DS', 'BW:Jet'], -1],
        ['j0_perf_ds1_L1J50',      'L1_J50',  [], ['DataScouting_05_Jets'], ['Legacy:Backup','RATE:Jets_DS', 'BW:Jet'], -1],

        # Standard central jet triggers
        ['j15',                         'L1_RD0_FILLED', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j25',                         'L1_RD0_FILLED', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j35',                         'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j45',                         'L1_J15', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j60',                         'L1_J20',  [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j85',                         'L1_J20',  [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j110',                        'L1_J30',  [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j175',                        'L1_J50',  [], [PhysicsStream], ['Legacy:Primary','RATE:SingleJet', 'BW:Jet'], -1],
        ['j260',                        'L1_J75', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],

        # Standard forward jet triggers
        ['j15_320eta490',               'L1_RD0_FILLED',  [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j25_320eta490',               'L1_RD0_FILLED',  [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j35_320eta490',               'L1_RD0_FILLED',  [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j45_320eta490',               'L1_J15.31ETA49', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j60_320eta490',               'L1_J20.31ETA49', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j85_320eta490',               'L1_J20.31ETA49', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j110_320eta490',              'L1_J30.31ETA49', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleJet',  'BW:Jet'], -1],
        ['j175_320eta490',              'L1_J50.31ETA49', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet',  'BW:Jet'], -1],
        ['j260_320eta490',              'L1_J75.31ETA49', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet',  'BW:Jet'], -1],

        #####
        ######################### Single-jet large-R trigger chains
        #####

        # Standard topocluster large-R jet triggers
        ['j110_a10_lcw_subjes_L1J30',       'L1_J30', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j260_a10_lcw_subjes_L1J75',       'L1_J75', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleJet', 'BW:Jet'], -1],
        ['j360_a10_lcw_subjes_L1J100',     'L1_J100',[], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],

        #ATR-ATR-19271
        ['j15_320eta490_L1TE5',      'L1_TE5', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j25_320eta490_L1TE10',      'L1_TE10', [], [PhysicsStream,'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j30_0eta490_L1TE20',      'L1_TE20', [], [PhysicsStream,'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j35_320eta490_L1TE20',      'L1_TE20', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j40_0eta490_L1TE20',      'L1_TE20', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j55_320eta490',      'L1_J15.31ETA49', [], [PhysicsStream,'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j60_200eta320_L1J20',      'L1_J20', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j75_200eta320_L1J20',      'L1_J20', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j50_L1J15',      'L1_J15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j75_L1J20',      'L1_J20', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j100_L1J20',      'L1_J20', [], [PhysicsStream,'express'], ['RATE:SingleJet', 'BW:Jet'], -1],

			 ]

    TriggerFlags.BjetSlice.signatures = []

    TriggerFlags.METSlice.signatures = [
        ['xe35', 'L1_XE35',[], [PhysicsStream], ['Legacy:Primary','RATE:MET', 'BW:MET'], -1],
        ['xe35_tc_lcw', 'L1_XE35',[], [PhysicsStream], ['Legacy:Primary','RATE:MET', 'BW:MET'], -1],
            ]

    TriggerFlags.TauSlice.signatures = [
        ['tau25_idperf_tracktwo',                  'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:IDMonitoring', 'BW:Tau', 'BW:ID'], -1],
			 ]

    TriggerFlags.EgammaSlice.signatures = [
	  #ATR-17820
        ['2g3_loose_dPhi15_L12EM3_VTE50', 'L1_2EM3_VTE50', [], [PhysicsStream],['Legacy:Support','RATE:MultiPhoton','BW:Egamma'],-1],
        ['2g3_medium_dPhi15_L12EM3_VTE50', 'L1_2EM3_VTE50', [], [PhysicsStream],['Legacy:Support','RATE:MultiPhoton','BW:Egamma'],-1],

        ['g10_loose',                            'L1_EM7',  [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_loose_L1EM15',                     'L1_EM15', [], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'],-1],

        # PS-ed trigger to combined chains
        ['g15_loose_L1EM7',               'L1_EM7',   [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g20_loose_L1EM12',              'L1_EM12',  [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'], -1], # pass through
        ['g40_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g45_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g50_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g80_loose',                     'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'], -1], 
        ['g100_loose',                    'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'], -1], 

        # Rerun mode and PS
        ['g25_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_loose_L1EM15',              'L1_EM15',  [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g60_loose',                     'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g70_loose',                     'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'],-1],

        # Rerun mode
        ['g15_loose_L1EM3',               'L1_EM3',   [], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_loose',                     'L1_EM15VH',[], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g50_loose',                     'L1_EM15VH', [], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g60_loose_L1EM15VH',            'L1_EM15VH', [], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'],-1],

	# ATR-16089
	['g15_loose_L1EM8VH',             'L1_EM8VH', [], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['e28_lhtight_nod0_e15_etcut_L1EM7_Zee',    	  'L1_EM24VHI', ['L1_EM24VHI','L1_EM7'], [PhysicsStream, 'express'], ['Legacy:Primary','RATE:MultiElectron', 'BW:Egamma'],-1 ],

        # Jpsiee T&P LH-based nod0
        ['e5_lhtight_nod0_e4_etcut',         'L1_2EM3',      [], [PhysicsStream], ['Legacy:Support','RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e5_lhtight_nod0_e4_etcut_Jpsiee',  'L1_2EM3',      [], [PhysicsStream], ['Legacy:Support','RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e9_lhtight_nod0_e4_etcut_Jpsiee',  'L1_EM7_2EM3',  ['L1_EM7','L1_EM3'], [PhysicsStream], ['Legacy:Support','RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e5_lhtight_nod0_e9_etcut_Jpsiee',  'L1_EM7_2EM3',  ['L1_EM3','L1_EM7'], [PhysicsStream], ['Legacy:Support','RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e14_lhtight_nod0_e4_etcut_Jpsiee', 'L1_EM12_2EM3', ['L1_EM12','L1_EM3'], [PhysicsStream], ['Legacy:Support','RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e5_lhtight_nod0_e14_etcut_Jpsiee', 'L1_EM12_2EM3', ['L1_EM3','L1_EM12'], [PhysicsStream], ['Legacy:Support','RATE:MultiElectron', 'BW:Egamma'],-1],

        ['g140_loose',        'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton','BW:Egamma'],-1],

        #Low-mu run triggers
        ['e15_lhloose_nod0_L1EM12',  'L1_EM12', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleElectron','BW:Egamma'], -1],

        ['e5_lhtight_nod0',                           'L1_EM3',     [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
		['e5_lhtight_nod0_e4_etcut_Jpsiee_L1RD0_FILLED',       'L1_RD0_FILLED',      ['L1_EM3','L1_EM3'],  [PhysicsStream, 'express'], ['Legacy:Support','RATE:MultiElectron', 'BW:Egamma'],-1, True],

        ['e9_lhtight_nod0',                           'L1_EM7',     [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e14_lhtight_nod0',                          'L1_EM12',    [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],

        ##########
        # Single photon triggers        
        ['g120_loose',                    'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Primary','RATE:SinglePhoton','BW:Egamma'],-1], 


        ['g25_loose',                     'L1_EM15VH',[], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_loose',                     'L1_EM15VH',[], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['e17_lhloose_nod0',                          'L1_EM15VH',  [], [PhysicsStream], ['Legacy:Primary','RATE:SingleElectron', 'BW:Egamma'],-1],

        #Diphoton triggers
        ['g35_loose_g25_loose',                      'L1_2EM15VH', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiPhoton', 'BW:Egamma'],-1],

        	 ]

    TriggerFlags.BphysicsSlice.signatures = [
        ['2mu4_bDimu',                    'L1_2MU4', [], [BPhysicsStream], ['Legacy:Backup',RATE_BphysTag,BW_BphysTag], -1], 
        ['2mu4_bDimu_novtx_noos',         'L1_2MU4', [], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1],  
        ['2mu6_bJpsimumu',                'L1_2MU6', [], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1],
        ['mu10_bJpsi_TrkPEBmon',        'L1_MU10',  [], [BPhysicsStream,'express'], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1],
        ['mu4_mu4_idperf_bJpsimumu_noid',  'L1_2MU4', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:IDMonitoring',BW_BphysTag, 'BW:ID'], -1],  
        ['mu13_mu13_idperf_Zmumu',     'L1_2MU10', [], [PhysicsStream, 'express'], ['Legacy:Primary','RATE:IDMonitoring','BW:Muon', 'BW:ID'], -1],
			 ]

    TriggerFlags.CombinedSlice.signatures = [
        #ATR-17860 L1Topo low Et di-photon triggers
        ['2g3_loose_L1DPHI-2EM3_VTE10', 'L1_DPHI-2EM3_VTE10', ['L1_2EM3'], [PhysicsStream],['Legacy:Primary','RATE:SinglePhoton','BW:Egamma'],-1],
        ['2g3_loose_L1DPHI-2EM7_VTE50', 'L1_DPHI-2EM7_VTE50', ['L1_2EM7'], [PhysicsStream],['Legacy:Backup','RATE:SinglePhoton','BW:Egamma'],-1],

        #ATR-17836 W T&P for low-mu
        ['e15_etcut_trkcut_xe30noL1', 'L1_EM12', ['L1_EM12',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1, ['serial',-1,["e15_etcut_trkcut","xe30noL1"]]],
        ['e15_etcut_trkcut_xe30noL1_mt35', 'L1_EM12', ['L1_EM12',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1, ['serial',-1,["e15_etcut_trkcut","xe30noL1"]]],
        ['e15_etcut_trkcut_xs30_xe30noL1_mt35', 'L1_EM12', ['L1_EM12','',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1, ['serial',-1,["e15_etcut_trkcut","xs30","xe30noL1"]]],
			 ]

    TriggerFlags.HeavyIonSlice.signatures = []
    TriggerFlags.BeamspotSlice.signatures = []   
    TriggerFlags.MinBiasSlice.signatures = []
    TriggerFlags.CalibSlice.signatures   = []
    TriggerFlags.CosmicSlice.signatures  = []
    TriggerFlags.StreamingSlice.signatures = []
    TriggerFlags.MonitorSlice.signatures = []
    TriggerFlags.EnhancedBiasSlice.signatures = []




class Prescales:
    #   Item name             | Prescale
    #----------------------------------------------------------
    L1Prescales = {}

    #   Signature name   | [ HLTprescale, HLTpass-through, rerun]
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
        }

