# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
#------------------------------------------------------------------------#
def setupMenu():

    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenu.menu.Physics_pp_v8.py' )

    from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1 as tcl1
    if tcl1.current:
        log.info("L1 items: %s " % tcl1.current.menu.items.itemNames())
    else:
        log.info("ERROR L1 menu has not yet been defined")

    PhysicsStream="Main"

    # stream, BW and RATE tags for Bphysics items that appear in Muon and Bphysics slice.signatures
    BPhysicsStream     = "BphysLS"
    BMultiMuonStream   = "Main"  
    RATE_BphysTag      = 'RATE:Bphysics'
    RATE_BMultiMuonTag = 'RATE:BMultiMuon'  # can become RATE:BMultiMuon' with one letter change 
    BW_BphysTag        = 'BW:Bphys'
    #RATE_DYTag         = 'RATE:Muon'   
    #BW_DYTag           = 'BW:Muon'   

    #---------------------------------------------------------------------
    # INPUT FORMAT FOR CHAINS:
    # ['chainName',  'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep], OPTIONAL: [mergingStrategy, offset,[merginOrder] ]], topoStartsFrom = False
    #---------------------------------------------------------------------

    #---------------------------------------------------------------------
    # if it's needed to temporary remove almost all the chains from the menu
    # be aware that it is necessary to leave at least one chain in the muon slice
    # otherwise athenaHLT will seg-fault 
    #---------------------------------------------------------------------

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.TestSlice.signatures = []

    TriggerFlags.AFPSlice.signatures = [

        ]

    TriggerFlags.MuonSlice.signatures = [
        ['mu26_ivarmedium',	     'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream, 'express'], ['Primary:20000','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu50',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Primary:17000','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu60_0eta105_msonly',    'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Primary:20000','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu80_msonly_3layersEC',  'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Primary:20000','RATE:SingleMuon', 'BW:Muon'], -1],


        #ATR-17539
        ['mu20_ivarmedium_mu8noL1',    'L1_MU20MU21',  ['L1_MU20',''], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_ivarmedium','mu8noL1']]],

        # multi muons
        ['2mu14',                  'L1_2MU10',          [], [PhysicsStream, 'express'], ['Primary:20000','RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu6',                   'L1_3MU6',           [], [PhysicsStream], ['Primary:20000','RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu6_msonly',            'L1_3MU6',           [], [PhysicsStream], ['Primary:20000','RATE:MultiMuon', 'BW:Muon'], -1],
        ['mu22_mu8noL1',           'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Primary:17000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','mu8noL1']]],
        ['mu20_2mu4noL1',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','2mu4noL1']]],
       
	  # multi-muon TagandProbe chains (ATR-17625)
	  ['mu22_mu8noL1_TagandProbe', 'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Primary:17000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','mu8noL1']]],

        # Primary (multi muon chains)
        ['4mu4',                   'L1_4MU4', ['L1_4MU4',''], [BMultiMuonStream], [RATE_BMultiMuonTag, BW_BphysTag], -1,['serial',-1,['4mu4','']]],

        ['mu20_ivarmedium_L1MU10_2J20', 'L1_MU10_2J20', [], [PhysicsStream], ['Primary:20000','RATE:MuonJet', 'BW:Muon','BW:Jet'], -1],                
        ['mu20_ivarmedium_L1MU10_2J15_J20', 'L1_MU10_2J15_J20', [], [PhysicsStream], ['Primary:17000','RATE:MuonJet', 'BW:Muon','BW:Jet'], -1],  

        #nscan05 triggers rejecting CB muons
        ['mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_XE30', 'L1_MU20_XE30', ['L1_MU20_XE30','L2_mu20_msonly_iloosems'], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_iloosems','mu6noL1_msonly_nscan05']]],
        ['mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_J40', 'L1_MU20_J40',   ['L1_MU20_J40','L2_mu20_msonly_iloosems'], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_iloosems','mu6noL1_msonly_nscan05']]],
        ['mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU6_EMPTY', 'L1_MU6_EMPTY',   ['L1_MU20','L2_mu20_msonly_iloosems'], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_iloosems','mu6noL1_msonly_nscan05']]],
        ['mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU4_UNPAIRED_ISO', 'L1_MU4_UNPAIRED_ISO',   ['L1_MU20','L2_mu20_msonly_iloosems'], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_iloosems','mu6noL1_msonly_nscan05']]],
        ['mu6_dRl1_mu20_msonly_iloosems_mu6noL1_dRl1_msonly','L1_MU11_2MU6', [['L1_MU6','L1_MU6'],'L1_MU20',''], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu6_dRl1','mu20_msonly_iloosems','mu6noL1_dRl1_msonly']]],


        ['mu10_mgonly_L1LATE-MU10_J50',          'L1_LATE-MU10_J50',           [], [PhysicsStream], ['Primary:20000','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10_mgonly_L1LATE-MU10_XE40',         'L1_LATE-MU10_XE40',          [], [PhysicsStream], ['Primary:20000','RATE:SingleMuon', 'BW:Muon'], -1],

			 ]


    if TriggerFlags.doFTK():
        TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
            ]


    TriggerFlags.JetSlice.signatures = [   
        ['j0_perf_ds1_L1J100',     'L1_J100', [], ['DataScouting_05_Jets'], ['RATE:Jets_DS', 'BW:Jet'], -1],
        
        # Standard central jet triggers
        ['j420',                        'L1_J100', [], [PhysicsStream,'express'], ['Primary:17000','RATE:SingleJet', 'BW:Jet'], -1],

        # Standard gsc central jet triggers (primaries)
        ['j225_gsc420_boffperf_split',  'L1_J100', [], [PhysicsStream,'express'], ['Primary:17000','RATE:SingleJet', 'BW:Jet'], -1],

        # Standard forward jet triggers
        ['j260_320eta490',              'L1_J75.31ETA49', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet',  'BW:Jet'], -1],

        # Alternative calibration forward jet triggers
        ['j260_320eta490_sub',          'L1_J75.31ETA49', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet',  'BW:Jet'], -1],
        ['j260_320eta490_nojcalib',     'L1_J75.31ETA49', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet',  'BW:Jet'], -1],
        ['j260_320eta490_lcw_sub',      'L1_J75.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        
        # Standard topocluster large-R jet triggers
        ['j460_a10_lcw_subjes_L1J100',      'L1_J100', [], [PhysicsStream], ['Primary:17000','RATE:SingleJet', 'BW:Jet'], -1],

        # L1topo standard topocluster large-R jet triggers
        ['j460_a10_lcw_subjes_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream,'express'], ['RATE:SingleJet', 'BW:Jet'], -1],

        # Standard reclustered large-R jet triggers
        ['j460_a10r_L1J100',                'L1_J100', [], [PhysicsStream], ['Primary:17000','RATE:SingleJet', 'BW:Jet'], -1],
        
        # L1topo standard reclustered large-R jet triggers
        ['j460_a10r_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream,'express'], ['RATE:SingleJet', 'BW:Jet'], -1],

        # Standard trimmed large-R jet triggers
        ['j460_a10t_lcw_jes_L1J100',        'L1_J100', [], [PhysicsStream], ['Primary:17000','RATE:SingleJet', 'BW:Jet'], -1],

        # L1topo standard trimmed large-R jet triggers
        ['j460_a10t_lcw_jes_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream,'express'], ['RATE:SingleJet', 'BW:Jet'], -1],

        # Trimmed + mass-cut single-jet triggers
        ['j420_a10t_lcw_jes_35smcINF_L1J100',               'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j420_a10t_lcw_jes_35smcINF_L1SC111',              'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        # Trimmed + mass-cut di-jet triggers
        ['2j330_a10t_lcw_jes_35smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['2j330_a10t_lcw_jes_35smcINF_L1SC111',             'L1_SC111-CJ15', [], [PhysicsStream,'express'], ['RATE:SingleJet', 'BW:Jet'], -1],

        # Trimmed + mass-cut asymmetric di-jet triggers (for rate optimization)
        ['j360_a10t_lcw_jes_60smcINF_j360_a10t_lcw_jes_L1SC111',   'L1_SC111-CJ15', [], [PhysicsStream], ['Primary:17000','RATE:SingleJet', 'BW:Jet'], -1],

        # ATR-17691 Trimmed large-R jet trigger analysis specific chains with mass cuts
        ['j370_a10t_lcw_jes_35smcINF_j370_a10t_lcw_jes_L1SC111',   'L1_SC111-CJ15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        #####
        ######################### Multi-jet small-R trigger chains
        #####

        # 3-jet single threshold triggers
        ['3j200',                       'L1_J100', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],

        # 4-jet single threshold triggers
        ['4j120',                       'L1_3J50', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],

        # 4-jet gsc single threshold triggers
        ['4j85_gsc115_boffperf_split',  'L1_3J50', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],

        # 5-jet single threshold triggers
        ['5j85_L14J15',                 'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85_L14J150ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        
        # 5-jet gsc single threshold triggers
        ['5j60_gsc85_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j60_gsc85_boffperf_split_L14J15',         'L1_4J15', [], [PhysicsStream], ['Primary:17000', 'RATE:MultiJet',  'BW:Jet'], -1],
        
        # 5-jet single threshold restricted eta range triggers
        ['5j70_0eta240_L14J15',         'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j70_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
 
        # 5-jet gsc single threshold restricted eta range triggers
        ['5j50_gsc70_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j50_gsc70_boffperf_split_0eta240_L14J15', 'L1_4J15', [], [PhysicsStream], ['Primary:17000', 'RATE:MultiJet',  'BW:Jet'], -1],
        
        # 6-jet single threshold triggers
        ['6j70_L14J15',                 'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j70_L14J150ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        
        # 6-jet gsc single threshold triggers
        ['6j50_gsc70_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_gsc70_boffperf_split_L14J15', 'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],

        # 6-jet single threshold restricted eta range triggers
        ['6j55_0eta240_L14J15',         'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j55_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        
        # 6-jet gsc single threshold restricted eta range triggers
        ['6j45_gsc55_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream,'express'], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc55_boffperf_split_0eta240_L14J15', 'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],

        # 7-jet single-threshold triggers
        ['7j45_L14J15',                 'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j45_L14J150ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        
        # 7-jet gsc single threshold triggers
        ['7j35_gsc45_boffperf_split_L14J150ETA25',          'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc45_boffperf_split_L14J15',         'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        
        # 7-jet single-threshold restricted eta range triggers
        ['7j45_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j45_0eta240_L14J15',                      'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        
        # 7-jet gsc single threshold restricted eta range triggers
        ['7j35_gsc45_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc45_boffperf_split_0eta240_L14J15', 'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],

        # 10-jet single-threshold triggers
        ['10j40_L14J15',                'L1_4J15' , [], [PhysicsStream], ['Primary:17000','RATE:MultiJet', 'BW:Jet'], -1],
        ['10j40_L14J150ETA25',          'L1_4J15.0ETA25' , [], [PhysicsStream], ['Primary:20000','RATE:MultiJet', 'BW:Jet'], -1],


        #####
        ######################### HT trigger chains
        #####

        # HT triggers
        ['ht1000_L1J100',   'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet', 'BW:Jet'], -1],

        #####
        ######################### Analysis-specific jet trigger chains
        #####

        # Analysis-driven multijet triggers
        ['2j225_gsc250_boffperf_split_0eta240_j85_gsc120_boffperf_split_0eta240',      'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],

        # ATR-15044: 2017 VBF Hbb
        ['ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF_AND_2j35_gsc45_bmv2c1070_split','L1_HT150-J20s5.ETA31_MJJ-400-CF', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,[  'ht300', '2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF_AND','2j35_gsc45_bmv2c1070_split']]],

		#VBF Chains
        ['j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ-500-NFF', 'L1_MJJ-500-NFF',[], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
			 ]

    TriggerFlags.BjetSlice.signatures = [
   
        # 
        #  1b / 1j
        # 

        #  gsc
        #  ATR-17811
        ['j175_gsc225_bmv2c1040_split', 'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc275_bmv2c1060_split', 'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc300_bmv2c1070_split', 'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc360_bmv2c1077_split', 'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleBJet', 'BW:BJet'], -1],                   

        ## Single jet with hybrid tuning
        ['j225_gsc275_bhmv2c1060_split',        'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc300_bhmv2c1070_split',        'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc360_bhmv2c1077_split',        'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleBJet', 'BW:BJet'], -1],

        # 
        #  1b / 4j
        # 

        ['j55_gsc75_bmv2c1040_split_3j55_gsc75_boffperf_split',       'L1_4J20',        [], [PhysicsStream], ['Primary:20000','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j60_gsc85_bmv2c1050_split_3j60_gsc85_boffperf_split',       'L1_4J20',        [], [PhysicsStream], ['Primary:20000','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j150_gsc175_bmv2c1060_split_j45_gsc60_bmv2c1060_split', 'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet'], -1],
        ['j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30', 'L1_J85_3J30', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet'], -1],

        ['2j35_bmv2c1060_split_2j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet'], -1],

        #
        #  2b / 5j
        #
        ['2j35_bmv2c1060_split_3j35', 'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j35_gsc45_bmv2c1060_split_3j35_gsc45_boffperf_split',                 'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet'], -1],

        #
        #  3b / 3j
        #
        ['3j50_gsc65_bmv2c1077_split_L13J35.0ETA23', 'L1_3J35.0ETA23', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet'], -1],

        #
        #  3b / 4j
        #
        ['3j35_bmv2c1070_split_j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet'], -1],

        #
        #  4b 
        #
        ['4j35_bmv2c1077_split_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j35_bmv2c1070_split_2j35_bmv2c1085_split_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet'], -1],

        #
        # L1Topo VBF MJJ bjets
        #
        ['j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490', 'L1_J40.0ETA25_2J25_J20.31ETA49', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet','BW:BJet'], -1],

			 ]
   

    if TriggerFlags.doFTK():
        TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [
           ] # Additional FTK for BjetSlise



    TriggerFlags.METSlice.signatures = [

  ###############      
  ## L1_XE50 ####
  ###############
        ['xe110_pufit_xe65_L1XE50', 'L1_XE50', ['L1_XE50','L1_XE50'], [PhysicsStream], ['Primary:20000','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x65_L1XE50'] ]],
			 ]

    if TriggerFlags.doFTK():
        TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + [
            ]

    
    TriggerFlags.TauSlice.signatures = [
        ['tau160_medium1_tracktwo_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['Primary:20000','RATE:SingleTau', 'BW:Tau'], -1],
        # ATR-17807
        ['tau160_medium1_tracktwoEF_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['Primary:20000','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_mediumRNN_tracktwoMVA_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['Primary:20000','RATE:SingleTau', 'BW:Tau'], -1],

        ['tau40_medium1_tracktwo_tau35_medium1_tracktwo',   'L1_TAU25IM_2TAU20IM_2J25_3J20',['L1_TAU25IM','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau40_medium1_tracktwo","tau35_medium1_tracktwo"]]],
        # ATR-17807
        ['tau40_medium1_tracktwoEF_tau35_medium1_tracktwoEF',   'L1_TAU25IM_2TAU20IM_2J25_3J20',['L1_TAU25IM','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau40_medium1_tracktwoEF","tau35_medium1_tracktwoEF"]]],
        ['tau40_mediumRNN_tracktwoMVA_tau35_mediumRNN_tracktwoMVA',   'L1_TAU25IM_2TAU20IM_2J25_3J20',['L1_TAU25IM','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau40_mediumRNN_tracktwoMVA","tau35_mediumRNN_tracktwoMVA"]]],

        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM_4J12.0ETA23',   'L1_TAU20IM_2TAU12IM_4J12.0ETA23',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
        # ATR-17807
        ['tau35_medium1_tracktwoEF_tau25_medium1_tracktwoEF_L1TAU20IM_2TAU12IM_4J12.0ETA23',   'L1_TAU20IM_2TAU12IM_4J12.0ETA23',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwoEF","tau25_medium1_tracktwoEF"]]],
        ['tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_L1TAU20IM_2TAU12IM_4J12.0ETA23',   'L1_TAU20IM_2TAU12IM_4J12.0ETA23',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_mediumRNN_tracktwoMVA","tau25_mediumRNN_tracktwoMVA"]]],

	    # l1topo ditau chains
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
        # ATR-17807
        ['tau35_medium1_tracktwoEF_tau25_medium1_tracktwoEF_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwoEF","tau25_medium1_tracktwoEF"]]],
        ['tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_mediumRNN_tracktwoMVA","tau25_mediumRNN_tracktwoMVA"]]],

        # tau+tau topo_start_from
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_tautsf_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]],True],

        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_03dR30_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
        # ATR-17807
        ['tau35_medium1_tracktwoEF_tau25_medium1_tracktwoEF_03dR30_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwoEF","tau25_medium1_tracktwoEF"]]],
        ['tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_03dR30_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_mediumRNN_tracktwoMVA","tau25_mediumRNN_tracktwoMVA"]]],

        # chains for 2e34 backup (ATR-15225)
        ['tau80_medium1_tracktwo_L1TAU60_tau35_medium1_tracktwo_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I',   'L1_TAU60_DR-TAU20ITAU12I',['L1_TAU60','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_medium1_tracktwo_L1TAU60","tau35_medium1_tracktwo_L1TAU12IM"]]]	,
        # non-L1topo backup (ATR-15757)
        ['tau80_medium1_tracktwo_L1TAU60_tau60_medium1_tracktwo_L1TAU40',   'L1_TAU60_2TAU40',['L1_TAU60','L1_TAU40'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_medium1_tracktwo_L1TAU60","tau60_medium1_tracktwo_L1TAU40"]]]       ,
        # ATR-17807
        ['tau80_medium1_tracktwoEF_L1TAU60_tau35_medium1_tracktwoEF_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I',   'L1_TAU60_DR-TAU20ITAU12I',['L1_TAU60','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_medium1_tracktwoEF_L1TAU60","tau35_medium1_tracktwoEF_L1TAU12IM"]]]	,
        ['tau80_mediumRNN_tracktwoMVA_L1TAU60_tau35_mediumRNN_tracktwoMVA_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I',   'L1_TAU60_DR-TAU20ITAU12I',['L1_TAU60','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_mediumRNN_tracktwoMVA_L1TAU60","tau35_mediumRNN_tracktwoMVA_L1TAU12IM"]]]	,
        ['tau80_medium1_tracktwoEF_L1TAU60_tau60_medium1_tracktwoEF_L1TAU40',   'L1_TAU60_2TAU40',['L1_TAU60','L1_TAU40'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_medium1_tracktwoEF_L1TAU60","tau60_medium1_tracktwoEF_L1TAU40"]]]       ,
        ['tau80_mediumRNN_tracktwoMVA_L1TAU60_tau60_mediumRNN_tracktwoMVA_L1TAU40',   'L1_TAU60_2TAU40',['L1_TAU60','L1_TAU40'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_mediumRNN_tracktwoMVA_L1TAU60","tau60_mediumRNN_tracktwoMVA_L1TAU40"]]]       ,

			 ]

    if TriggerFlags.doFTK():
            TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [
                           ]

    TriggerFlags.EgammaSlice.signatures = [
        ['g300_etcut',                    'L1_EM22VHI', [], [PhysicsStream], ['Primary:17000','RATE:SinglePhoton', 'BW:Egamma'],-1], 
        ['g0_hiptrt_L1EM22VHI',                 'L1_EM22VHI', [], [PhysicsStream], ['Primary:17000','RATE:SinglePhoton', 'BW:Egamma'], -1],
	# ATR-16089
        ['2g20_tight_icalovloose_L12EM15VHI',           'L1_2EM15VHI', [], [PhysicsStream, 'express'], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g22_tight_L12EM15VHI',                   'L1_2EM15VHI', [], [PhysicsStream], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g50_loose_L12EM20VH',                    'L1_2EM20VH', [], [PhysicsStream], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ##Adding tight diphoton triggers (ATR-10762)
        ['2g25_tight_L12EM20VH',                    'L1_2EM20VH', [], [PhysicsStream], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g25_loose_g15_loose',                    'L1_2EM20VH', [], [PhysicsStream], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1], 
        #ATR-16981
        ['e26_lhtight_nod0_e15_etcut_L1EM7_Zee',    	  'L1_EM22VHI', ['L1_EM22VHI','L1_EM7'], [PhysicsStream], ['Primary:17000','RATE:MultiElectron', 'BW:Egamma'],-1 ],
        # Single electron triggers
        ['e26_lhtight_nod0',                  'L1_EM22VHI', [], [PhysicsStream], ['Primary:17000','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhtight_nod0_ivarloose',        'L1_EM22VHI', [], [PhysicsStream], ['Primary:17000','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e60_lhmedium_nod0', 'L1_EM22VHI', [], [PhysicsStream], ['Primary:17000','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g140_loose',        'L1_EM24VHI', [], [PhysicsStream], ['Primary:17000','RATE:SinglePhoton','BW:Egamma'],-1],
        ['e140_lhloose_nod0', 'L1_EM22VHI', [], [PhysicsStream], ['Primary:17000','RATE:SingleElectron', 'BW:Egamma'],-1],

        ['2e17_lhvloose_nod0_L12EM15VHI',         'L1_2EM15VHI', [], [PhysicsStream], ['Primary:20000','RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e24_lhvloose_nod0',                   'L1_2EM20VH', [], [PhysicsStream, 'express'], ['Primary:20000','RATE:MultiElectron', 'BW:Egamma'],-1],

        ['e24_lhvloose_nod0_2e12_lhvloose_nod0_L1EM20VH_3EM10VH', 'L1_EM20VH_3EM10VH', ['L1_EM20VH','L1_3EM10VH'], [PhysicsStream], ['Primary:20000','RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_2g12_loose',                          'L1_EM20VH_3EM10VH', ['L1_EM20VH','L1_3EM10VH'], [PhysicsStream], ['Primary:20000','RATE:ElectronPhoton', 'BW:Egamma'],-1,['parallel',-1,[] ]],
        #Physics (e+g):
        ['e24_lhmedium_nod0_L1EM20VH_g25_medium',  'L1_2EM20VH', [], [PhysicsStream], ['Primary:20000','RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 

        #Diphoton triggers
        ['g35_medium_g25_medium_L12EM20VH',          'L1_2EM20VH', [], [PhysicsStream, 'express'], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1],

        # Et cut only chains
        ['e300_etcut',                           'L1_EM24VHI',  [], [PhysicsStream], ['Primary:20000','RATE:SingleElectron', 'BW:Egamma'],-1], 

        	 ]

    TriggerFlags.BphysicsSlice.signatures = [

        #New for 2018: mu11_mu6 triggers seeded from LFV-MU11
        ['mu11_mu6_bDimu_L1LFV-MU11',                 'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bDimu2700_L1LFV-MU11',             'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bJpsimumu_L1LFV-MU11',             'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bBmumu_L1LFV-MU11',                'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bUpsimumu_L1LFV-MU11',             'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bTau_L1LFV-MU11',                  'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bPhi_L1LFV-MU11',                  'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bBmumuxv2_L1LFV-MU11',             'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bBmumux_BpmumuKp_L1LFV-MU11',      'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bJpsimumu_Lxy0_L1LFV-MU11',        'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bDimu2700_Lxy0_L1LFV-MU11',        'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bDimu_Lxy0_L1LFV-MU11',            'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],

        #ATR-16219
        ['mu11_mu6_bPhi',                 'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1,['serial',-1,['mu11','mu6_bPhi']]],
        ['mu11_mu6noL1_bPhi_L1MU11_2MU6', 'L1_MU11_2MU6', ['L1_MU11','L2_mu6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1,['serial',-1,['mu11','mu6noL1_bPhi']]],

        # ATR-16163
        ['3mu4_bJpsi',                    'L1_3MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['3mu4_bTau',                     'L1_3MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['3mu4_bUpsi',                    'L1_3MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['3mu6_bDimu',                   'L1_3MU6', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu6_2mu4_bJpsi', 'L1_MU6_3MU4', ['L1_MU6','L1_3MU4'], [BPhysicsStream, 'express'], [RATE_BphysTag, BW_BphysTag], -1],
        ['mu6_2mu4_bUpsi', 'L1_MU6_3MU4', ['L1_MU6','L1_3MU4'], [BPhysicsStream], [RATE_BphysTag, BW_BphysTag], -1],
        ['mu6_2mu4_bDimu2700', 'L1_MU6_3MU4', ['L1_MU6','L1_3MU4'], [BPhysicsStream], [RATE_BphysTag, BW_BphysTag], -1],

        ### ATR-18329
        ['4mu4_bDimu6000', 'L1_4MU4', ['L1_4MU4',''], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],

        ['mu13_mu13_idperf_Zmumu',     'L1_2MU10', [], [PhysicsStream, 'express'], ['RATE:IDMonitoring','BW:Muon', 'BW:ID'], -1],

        ['2mu10_bBmumuxv2',               'L1_2MU10', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu10_bJpsimumu',               'L1_2MU10', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu10_bUpsimumu',               'L1_2MU10', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
     
        ['mu11_mu6_bDimu2700',            'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu11_mu6_bDimu_Lxy0',            'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu11_mu6_bDimu2700_Lxy0',            'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        
        ['2mu6_mu4_bUpsi', 'L1_2MU6_3MU4', ['L1_2MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag, BW_BphysTag], -1],

        ['mu11_2mu4noL1_bNocut_L1MU11_2MU6', 'L1_MU11_2MU6', ['L1_MU11','L2_mu11'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1,['serial',-1,['mu11','2mu4noL1']]], 

        ['3mu4_bPhi',                     'L1_3MU4', [], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],

        ['mu11_2mu4noL1_bTau_L1MU11_2MU6', 'L1_MU11_2MU6', ['L1_MU11','L2_mu11'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1,['serial',-1,['mu11','2mu4noL1']]],
        ['mu20_mu6noL1_bTau',              'L1_MU20MU21', ['L1_MU20','L2_mu20'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1,['serial',-1,['mu20','mu6noL1']]],

        ['mu6_2mu4_bTau_L1MU6_3MU4' , 'L1_MU6_3MU4',  ['L1_MU6','L1_2MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['2mu6_mu4_bTau_L12MU6_3MU4', 'L1_2MU6_3MU4', ['L1_2MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],

        ### ATR-14350: L1Topo bphysics chains
        # 96% bckg rejection WP
        ['2mu6_bJpsimumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',  'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',     ['L1_2MU6'], [BPhysicsStream, 'express'], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bUpsimumu_L1BPH-8M15-2MU6_BPH-0DR22-2MU6', 'L1_BPH-8M15-2MU6_BPH-0DR22-2MU6',     ['L1_2MU6'], [PhysicsStream, 'express'], ['Primary:20000','RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bBmumux_BpmumuKp_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',  'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],


        #ATR-16219
        ['2mu6_bBmumu_Lxy0_L1BPH-2M9-2MU6_BPH-2DR15-2MU6', 'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',     ['L1_2MU6'], [BPhysicsStream, 'express'], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bBmumux_BsmumuPhi_L1BPH-2M9-2MU6_BPH-2DR15-2MU6', 'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',     ['L1_2MU6'], [BPhysicsStream, 'express'], [RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bJpsimumu_Lxy0_L1BPH-2M9-2MU6_BPH-2DR15-2MU6', 'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',     ['L1_2MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1, False],
        
        # new baseline triggers, ATR-15503
        ['mu11_mu6_bBmumu',                'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream, 'express'], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu11_mu6_bBmumuxv2',             'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream, 'express'], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu11_mu6_bBmumux_BpmumuKp',      'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu11_mu6_bJpsimumu_Lxy0',        'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu11_mu6_bJpsimumu',             'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream, 'express'], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu11_mu6_bUpsimumu',             'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu11_mu6_bDimu',                 'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream, 'express'], [RATE_BphysTag,BW_BphysTag], -1],

        # ATR-16314
        ['mu11_mu6_bTau',                  'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream, 'express'], [RATE_BphysTag,BW_BphysTag], -1],
        # ATR-17617
        ['mu20_mu6btrk_bTauTight',        'L1_MU20MU21', ['L1_MU20','L2_mu20'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1,['serial',-1,['mu20','mu6btrk']]],
        ['mu20_2mu2btrk_bTauTight',       'L1_MU20MU21', ['L1_MU20','L2_mu20'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1,['serial',-1,['mu20','2mu2btrk']]],
        ['mu11_2mu2btrk_bTauTight_L1MU11_2MU6', 'L1_MU11_2MU6', ['L1_MU11','L2_mu11'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1,['serial',-1,['mu11','2mu2btrk']]],
        
			 ]

    TriggerFlags.CombinedSlice.signatures = [
        
        ['j45_gsc55_bmv2c1050_split_ht700_L1HT190-J15s5.ETA21',  'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['Primary:20000','RATE:SingleBJet', 'BW:BJet_combined'], -1,['serial',-1,['ht700','j45_gsc55_bmv2c1050_split']]],

        ['2j45_gsc55_bmv2c1050_split_ht300_L1HT190-J15s5.ETA21', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet_combined'], -1,['serial',-1,[ 'ht300', '2j45_gsc55_bmv2c1050_split' ]]], 

        ## VBF triggers
        ['e10_lhmedium_nod0_ivarloose_j70_j50_0eta490_invm900j50_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_EM8VH','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["e10_lhmedium_nod0_ivarloose","j70","j50_0eta490_invm900j50"]]],
        ['e5_lhvloose_nod0_j70_j50_0eta490_invm1000j50_xe50_pufit_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_EM3','','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["e5_lhvloose_nod0","j70","j50_0eta490","xe50_pufit"]]],
        ['e5_lhvloose_j70_j50_0eta490_invm1000j50_dphi28_xe50_pufit_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_EM3','','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["e5_lhvloose","j70","j50_0eta490","xe50_pufit"]]],
        ['mu10_ivarmedium_j70_j50_0eta490_invm900j50_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_MU10','',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jet'], -1, ['serial', -1,["mu10_ivarmedium","j70","j50_0eta490_invm900j50"]]],
        ['mu4_j70_j50_0eta490_invm1000j50_xe50_pufit_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_MU4','','',''], [PhysicsStream], ['RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial', -1,["mu4","j70","j50_0eta490","xe50_pufit"]]],
        ['mu4_j70_j50_0eta490_invm1000j50_dphi28_xe50_pufit_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_MU4','','',''], [PhysicsStream], ['RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial', -1,["mu4","j70","j50_0eta490","xe50_pufit"]]],
        ['2mu4_j70_j50_0eta490_invm1000j50_xe50_pufit_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_2MU4','','',''], [PhysicsStream], ['RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial', -1,["2mu4","j70","j50_0eta490","xe50_pufit"]]],
        ['g35_medium_j70_j50_0eta490_invm900j50_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_EM24VHI','',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jet'], -1, ['serial', -1,["g35_medium","j70","j50_0eta490_invm900j50"]]],
        ['j70_j50_0eta490_invm1000j50_dphi24_xe90_pufit_xe50_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1, ['serial', -1,["j70","j50_0eta490_invm1000j50_dphi24","xe90_pufit","xe50"]]],
        ['j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ-500-NFF_AND_2j35_bmv2c1070_split', 'L1_MJJ-500-NFF', [], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial', -1,["j70_0eta490","j50_0eta490","2j35_0eta490_invm1000j50_L1MJJ-500-NFF_AND","2j35_bmv2c1070_split"]]],
        ['j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ-500-NFF_AND_2j35_gsc45_bmv2c1070_split', 'L1_MJJ-500-NFF', [], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial', -1,["j70_0eta490","j50_0eta490","2j35_0eta490_invm1000j50_L1MJJ-500-NFF_AND","2j35_gsc45_bmv2c1070_split"]]],
        ['2e5_lhmedium_nod0_j70_j50_0eta490_invm900j50_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_2EM3','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["2e5_lhmedium_nod0","j70","j50_0eta490_invm900j50"]]],
        ['2mu6_2j50_0eta490_invm900j50_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_2MU6',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jet'], -1, ['serial', -1,["2mu6","2j50_0eta490_invm900j50"]]],
        ['tau25_medium1_tracktwo_tau20_medium1_tracktwo_j70_j50_0eta490_invm900j50_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_TAU12IM','L1_TAU12IM','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["tau25_medium1_tracktwo","tau20_medium1_tracktwo","j70","j50_0eta490_invm900j50"]]],
        ['tau25_mediumRNN_tracktwoMVA_tau20_mediumRNN_tracktwoMVA_j70_j50_0eta490_invm900j50_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_TAU12IM','L1_TAU12IM','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["tau25_mediumRNN_tracktwoMVA","tau20_mediumRNN_tracktwoMVA","j70","j50_0eta490_invm900j50"]]],
        ['e10_lhmedium_nod0_ivarloose_j70_j50_0eta490_invm900j50_L1MJJ-700', 'L1_MJJ-700', ['L1_EM8VH','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["e10_lhmedium_nod0_ivarloose","j70","j50_0eta490_invm900j50"]]],
        ['e5_lhvloose_j70_j50_0eta490_invm1000j50_xe50_pufit_L1MJJ-700', 'L1_MJJ-700', ['L1_EM3','','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["e5_lhvloose","j70","j50_0eta490","xe50_pufit"]]],
        ['e5_lhvloose_j70_j50_0eta490_invm1000j50_dphi28_xe50_pufit_L1MJJ-700', 'L1_MJJ-700', ['L1_EM3','','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["e5_lhvloose","j70","j50_0eta490","xe50_pufit"]]],
        ['mu10_ivarmedium_j70_j50_0eta490_invm900j50_L1MJJ-700', 'L1_MJJ-700', ['L1_MU10','',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jet'], -1, ['serial', -1,["mu10_ivarmedium","j70","j50_0eta490_invm900j50"]]],
        ['mu4_j70_j50_0eta490_invm1000j50_xe50_pufit_L1MJJ-700', 'L1_MJJ-700', ['L1_MU4','','',''], [PhysicsStream], ['RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial', -1,["mu4","j70","j50_0eta490","xe50_pufit"]]],
        ['mu4_j70_j50_0eta490_invm1000j50_dphi28_xe50_pufit_L1MJJ-700', 'L1_MJJ-700', ['L1_MU4','','',''], [PhysicsStream], ['RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial', -1,["mu4","j70","j50_0eta490","xe50_pufit"]]],
        ['2mu4_j70_j50_0eta490_invm1000j50_xe50_pufit_L1MJJ-700', 'L1_MJJ-700', ['L1_2MU4','','',''], [PhysicsStream], ['RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial', -1,["2mu4","j70","j50_0eta490","xe50_pufit"]]],
        ['g35_medium_j70_j50_0eta490_invm900j50_L1MJJ-700', 'L1_MJJ-700', ['L1_EM24VHI','',''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon', 'BW:Jet'], -1, ['serial', -1,["g35_medium","j70","j50_0eta490_invm900j50"]]],
        ['j70_j50_0eta490_invm1000j50_dphi24_xe90_pufit_xe50_L1MJJ-700', 'L1_MJJ-700', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1, ['serial', -1,["j70","j50_0eta490_invm1000j50_dphi24","xe90_pufit","xe50"]]],
        ['j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ-700_AND_2j35_bmv2c1070_split', 'L1_MJJ-700', [], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial', -1,["j70_0eta490","j50_0eta490","2j35_0eta490_invm1000j50_L1MJJ-700_AND","2j35_bmv2c1070_split"]]],
        ['j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ-700_AND_2j35_gsc45_bmv2c1070_split', 'L1_MJJ-700', [], [PhysicsStream], ['RATE:JetMET', 'BW:Jet'], -1, ['serial', -1,["j70_0eta490","j50_0eta490","2j35_0eta490_invm1000j50_L1MJJ-700_AND","2j35_gsc45_bmv2c1070_split"]]],
        ['2e5_lhmedium_nod0_j70_j50_0eta490_invm900j50_L1MJJ-700', 'L1_MJJ-700', ['L1_2EM3','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["2e5_lhmedium_nod0","j70","j50_0eta490_invm900j50"]]],
        ['2mu6_2j50_0eta490_invm900j50_L1MJJ-700', 'L1_MJJ-700', ['L1_2MU6',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["2mu6","2j50_0eta490_invm900j50"]]],
        ['tau25_medium1_tracktwo_tau20_medium1_tracktwo_j70_j50_0eta490_invm900j50_L1MJJ-700', 'L1_MJJ-700', ['L1_TAU12IM','L1_TAU12IM','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["tau25_medium1_tracktwo","tau20_medium1_tracktwo","j70","j50_0eta490_invm900j50"]]],
        ['tau25_mediumRNN_tracktwoMVA_tau20_mediumRNN_tracktwoMVA_j70_j50_0eta490_invm900j50_L1MJJ-700', 'L1_MJJ-700', ['L1_TAU12IM','L1_TAU12IM','',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["tau25_mediumRNN_tracktwoMVA","tau20_mediumRNN_tracktwoMVA","j70","j50_0eta490_invm900j50"]]],

        # b-jet+MET ATR-17678
        ['2j35_gsc45_bmv2c1070_split_xe85_pufit_xe50_L12J15_XE55',"L1_2J15_XE55",[],[PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet_combined', 'BW:MET'], -1, ['serial',-1,['xe85_pufit','xe50_L12J15_XE55','2j35_gsc45_bmv2c1070_split']]],
        ['3j35_bmv2c1077_split_xe70_pufit_xe50_L13J15.0ETA25_XE40',"L1_3J15.0ETA25_XE40",[],[PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet_combined', 'BW:MET'], -1, ['serial',-1,['xe70_pufit','xe50_L13J15.0ETA25_XE40','3j35_bmv2c1077_split']]],
        ['j85_gsc100_bmv2c1050_split_xe85_pufit_xe50_L1XE55',"L1_XE55",[],[PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet_combined', 'BW:MET'], -1, ['serial',-1,['xe85_pufit','xe50_L1XE55','j85_gsc100_bmv2c1050_split']]],


        ['2e12_lhloose_nod0_mu10',  'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e12_lhloose_nod0_2mu10',  'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e17_lhloose_nod0_mu14',   'L1_EM15VH_MU10', ['L1_EM15VH', 'L1_MU10'], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e26_lhmedium_nod0_mu8noL1', 'L1_EM22VHI', ['L1_EM22VHI', ''], [PhysicsStream], ['Primary:17000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['e26_lhmedium_nod0','mu8noL1'] ]],
        ['e70_lhloose_nod0_xe70noL1',    'L1_EM22VHI',['L1_EM22VHI',''], [PhysicsStream], ['Primary:17000','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e70_lhloose_nod0","xe70noL1"]]],

        ['e7_lhmedium_nod0_mu24',        'L1_MU20MU21', ['L1_EM3', 'L1_MU20'], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['g80_loose_icalovloose_xe80noL1',           'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['Primary:17000','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g80_loose_icalovloose","xe80noL1"]]],
        ['g85_tight_icalovloose_L1EM22VHI_3j50noL1',  'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['Primary:17000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g85_tight_icalovloose_L1EM22VHI","3j50noL1"]]],

        #ATR-14196
        ['g80_loose_xe80noL1',           'L1_EM22VHI',  ['L1_EM22VHI',''], [PhysicsStream], ['Primary:17000','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g80_loose","xe80noL1"]]],

        ['g25_medium_mu24',              'L1_MU20MU21', ['L1_EM15VH','L1_MU20'], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],

        ['g15_loose_2mu10_msonly',       'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],

        #ATR-15243 Higgsino
        #di-muon primary
        ['2mu4_invm1_j20_xe40_pufit_2dphi10_L12MU4_J20_XE30_DPHI-J20s2XE30',  'L1_2MU4_J20_XE30_DPHI-J20s2XE30', [], [PhysicsStream], ['Primary:20000','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["2mu4_invm1","j20","xe40_pufit"]]], 
        #di-muon backup
        ['2mu4_invm1_j20_xe60_pufit_2dphi10_L12MU4_J40_XE50',  'L1_2MU4_J40_XE50', ['L1_2MU4','L1_J40','L1_XE50'], [PhysicsStream], ['Primary:20000','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["2mu4_invm1","j20","xe60_pufit"] ]],
        #single muon backup
        ['mu4_j90_xe90_pufit_2dphi10_L1MU4_J50_XE50_DPHI-J20s2XE30',  'L1_MU4_J50_XE50_DPHI-J20s2XE30', [], [PhysicsStream], ['Primary:20000','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["mu4","j90","xe90_pufit"] ]],
        ['mu4_j90_xe90_pufit_2dphi10_L1MU4_XE60',  'L1_MU4_XE60', [], [PhysicsStream], ['Primary:20000','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["mu4","j90","xe90_pufit"] ]],
        # mu-e
        ['e5_lhvloose_nod0_mu4_j30_xe40_pufit_2dphi10_L1MU4_J30_XE40_DPHI-J20s2XE30', 'L1_MU4_J30_XE40_DPHI-J20s2XE30', ['L1_EM3','','',''], [PhysicsStream], ['Primary:20000','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1,['serial',-1,["e5_lhvloose_nod0","mu4","j30","xe40_pufit"]]],
        #mu-e backup
        ['e5_lhmedium_nod0_mu4_j30_xe65_pufit_2dphi10_L1MU4_XE60',  'L1_MU4_XE60', ['L1_EM3','L1_MU4','','L1_XE60'], [PhysicsStream], ['Primary:20000','RATE:MuonJetMET', 'BW:Egamma', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["e5_lhmedium_nod0","mu4","j30","xe65_pufit"] ]], 
        #di-electron
        ['2e5_lhvloose_nod0_j40_xe70_pufit_2dphi10_L1J40_XE50_DPHI-J20s2XE50',  'L1_J40_XE50_DPHI-J20s2XE50', ['L1_EM3','',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJetMET', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["2e5_lhvloose_nod0","j40","xe70_pufit"] ]],
        #di-electron backup
        ['2e5_lhvloose_nod0_j40_xe70_pufit_2dphi10_L1XE60',  'L1_XE60', ['L1_EM3','',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJetMET', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["2e5_lhvloose_nod0","j40","xe70_pufit_L1XE60"] ]],
        #single electron backup 
        ['e5_lhloose_nod0_j50_xe70_pufit_2dphi10_L1J40_XE50_DPHI-J20s2XE50',  'L1_J40_XE50_DPHI-J20s2XE50', ['L1_EM3','',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJetMET', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e5_lhloose_nod0","j50","xe70_pufit"] ]],
        ['e5_lhloose_nod0_j40_xe70_pufit_2dphi10_L1XE60',  'L1_XE60', ['L1_EM3','',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJetMET', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e5_lhloose_nod0","j40","xe70_pufit_L1XE60"] ]],
        #ATR-16089 combined egamma support chains
        ['e60_etcut_trkcut_L1EM24VHIM_xs30_j15_perf_xe30_6dphi15_mt35', 'L1_EM24VHIM', ['L1_EM24VHIM','','',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e60_etcut_trkcut_L1EM24VHIM","xs30","j15_perf","xe30"]]],

        # 2mu
        ['2mu14_2j35_boffperf_split',        'L1_2MU10',   ['L1_2MU10',   ''],     [PhysicsStream], ['Primary:20000','RATE:MuonBJet',  'BW:BJet_combined'],  -1, ['serial',-1,['2mu14', '2j35_boffperf_split']]],

        # 2e 
        ['2e24_lhvloose_nod0_2j35_boffperf_split',   'L1_2EM20VH', ['L1_2EM20VH', ''],     [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:BJet_combined'],   -1, ['serial',-1,['2e24_lhvloose_nod0', '2j35_boffperf_split']]],

        # e-mu
        ['e28_lhmedium_nod0_L1EM24VHI_mu8noL1_2j35_boffperf_split', 'L1_EM24VHI', ['L1_EM24VHI', '', ''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon',  'BW:BJet_combined'], -1, ['serial',-1,['e28_lhmedium_nod0_L1EM24VHI','mu8noL1','2j35_boffperf_split'] ]],

        # mu +asymmetric
        ['mu26_ivarmedium_j150_boffperf_split_j35_boffperf_split',          'L1_MU20MU21',    ['L1_MU20',    '', ''], [PhysicsStream], ['Primary:20000','RATE:MuonBJet',  'BW:BJet_combined'],  -1, ['serial',-1,['mu26_ivarmedium', 'j150_boffperf_split','j35_boffperf_split']]],
        ['mu26_ivarmedium_j110_gsc150_boffperf_split_j35_boffperf_split', 'L1_MU20MU21',    ['L1_MU20',    '', ''], [PhysicsStream], ['Primary:20000','RATE:MuonBJet',  'BW:BJet_combined'],  -1, ['serial',-1,['mu26_ivarmedium', 'j110_gsc150_boffperf_split', 'j35_boffperf_split']]],

        # e
        ['e28_lhtight_nod0_ivarloose_j150_boffperf_split_j35_boffperf_split',               'L1_EM24VHI', ['L1_EM24VHI', '', ''], [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:BJet_combined'],  -1, ['serial',-1,['e28_lhtight_nod0_ivarloose', 'j150_boffperf_split','j35_boffperf_split']]],
        ['e28_lhtight_nod0_ivarloose_j110_gsc150_boffperf_split_j35_boffperf_split',  'L1_EM24VHI', ['L1_EM24VHI', '', ''], [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:BJet_combined'],  -1, ['serial',-1,['e28_lhtight_nod0_ivarloose', 'j110_gsc150_boffperf_split','j35_boffperf_split']]],

        ['2g10_loose_mu20',          'L1_MU20MU21',    ['L1_2EM7', 'L1_MU20'],  [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['2g10_loose','mu20'] ]],

        # LLP trigger seeded by MUON items
        ['j30_muvtx',                    'L1_2MU10',            [], [PhysicsStream], ['Primary:20000','RATE:MuonJet','BW:Muon', 'BW:Jet'], -1],
        ['j30_muvtx_noiso',              'L1_2MU10',            [], [PhysicsStream], ['Primary:20000','RATE:MuonJet','BW:Muon', 'BW:Jet'], -1],

        # LLP trigger seeded by TAU items
        ['j30_jes_cleanLLP_PS_llp_L1TAU100',             'L1_TAU100',             [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j30_jes_cleanLLP_PS_llp_noiso_L1TAU100',       'L1_TAU100',             [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],

        # L1Topo LLP with TAU
        ['j30_jes_cleanLLP_PS_llp_L1LLP-NOMATCH',       'L1_LLP-NOMATCH',       [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1, False],
        ['j30_jes_cleanLLP_PS_llp_noiso_L1LLP-NOMATCH', 'L1_LLP-NOMATCH',       [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1, False],

        ['j30_jes_cleanLLP_PS_llp_L1LLP-RO',       'L1_LLP-RO',       [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1, False],
        ['j30_jes_cleanLLP_PS_llp_noiso_L1LLP-RO', 'L1_LLP-RO',       [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1, False],

 
        #ATR-11623 photon + muon/di-muon chains L1_EM24VHI and L1_EM24VHIM seed
        ['g35_tight_icalotight_L1EM24VHI_mu18noL1',        'L1_EM24VHI', ['L1_EM24VHI', ''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_tight_icalotight_L1EM24VHI','mu18noL1'] ]],
        ['g35_tight_icalotight_L1EM24VHI_mu15noL1_mu2noL1', 'L1_EM24VHI', ['L1_EM24VHI', '',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_tight_icalotight_L1EM24VHI','mu15noL1','mu2noL1']]],

        ['g35_loose_L1EM24VHI_mu18',        'L1_EM24VHI', ['L1_EM24VHI', 'L1_MU11'], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_loose_L1EM24VHI','mu18'] ]],
        ['g35_loose_L1EM24VHI_mu15_mu2noL1', 'L1_EM24VHI', ['L1_EM24VHI', 'L1_MU11',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_loose_L1EM24VHI','mu15','mu2noL1']]],

        # Heg chains
        ['e25_mergedtight_g35_medium_Heg',       'L1_2EM20VH', [], [PhysicsStream], ['Primary:20000','RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],

        # ATR-16089 Heg chains
        ['e25_mergedtight_ivarloose_g35_medium_icalovloose_Heg',       'L1_2EM20VH', [], [PhysicsStream], ['Primary:20000','RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
 
	# new chain for 2e34 backup (ATR-15225)
        ['e24_lhmedium_nod0_ivarloose_tau35_medium1_tracktwo',  'L1_EM20VHI_TAU20IM_2TAU20_J25_3J20',['L1_EM20VHI','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e24_lhmedium_nod0_ivarloose","tau35_medium1_tracktwo"]]],
	                                               
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo',  'L1_EM15VHI_2TAU12IM_J25_3J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream],['Primary:17000','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwo"]]],

        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1EM15VHI_2TAU12IM_4J12',   'L1_EM15VHI_2TAU12IM_4J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream],['Primary:20000','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwo"]]],

	#L1Topo e+tau chains
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25', 'L1_DR-EM15TAU12I-J25',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:ElectronTau', 'BW:Tau'],-1,['serial',-1 ,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwo"]]],
        ['mu14_ivarloose_tau35_medium1_tracktwo',  'L1_MU10_TAU20IM',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['Primary:17000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_medium1_tracktwo"]]],
        ['mu14_ivarloose_L1MU11_tau35_medium1_tracktwo_L1MU11_TAU20IM',  'L1_MU11_TAU20IM',['L1_MU11','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose_L1MU11","tau35_medium1_tracktwo"]]],

        ['mu14_ivarloose_tau25_medium1_tracktwo',      'L1_MU10_TAU12IM_J25_2J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Primary:17000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwo"]]],        
        ['mu14_ivarloose_tau25_medium1_tracktwo_L1MU10_TAU12IM_3J12',      'L1_MU10_TAU12IM_3J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwo"]]],

        # l1topo tau+mu chains
        ['mu14_ivarloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25', 'L1_DR-MU10TAU12I_TAU12I-J25',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwo"]]],
        
        # Tau + MET
        ['tau35_medium1_tracktwo_xe70_L1XE45',  'L1_TAU20IM_2J20_XE45',['L1_TAU20IM','L1_XE45'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","xe70_L1XE45"]]],  
        ['tau60_medium1_tracktwo_tau25_medium1_tracktwo_xe50', 'L1_TAU40_2TAU12IM_XE40',['L1_TAU40','L1_TAU12IM','L1_XE40'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau60_medium1_tracktwo","tau25_medium1_tracktwo","xe50"]]],
 
        # Tau + e + MET
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_xe50', 'L1_EM15VHI_2TAU12IM_XE35',['L1_EM15VHI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau', 'BW:Egamma'],-1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwo", "xe50"]]],
        ['e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50', 'L1_EM15VHI_2TAU12IM_XE35',['L1_EM15VHI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_medium1_tracktwo", "xe50"]]],

        # Tau + mu + MET
        ['mu14_tau25_medium1_tracktwo_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14", "tau25_medium1_tracktwo","xe50"]]],
        ['mu14_ivarloose_tau25_medium1_tracktwo_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose", "tau25_medium1_tracktwo","xe50"]]],

        # ATR-17807
        ['tau60_medium1_tracktwoEF_tau25_medium1_tracktwoEF_xe50', 'L1_TAU40_2TAU12IM_XE40',['L1_TAU40','L1_TAU12IM','L1_XE40'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau60_medium1_tracktwoEF","tau25_medium1_tracktwoEF","xe50"]]],
        ['tau60_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_xe50', 'L1_TAU40_2TAU12IM_XE40',['L1_TAU40','L1_TAU12IM','L1_XE40'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau60_mediumRNN_tracktwoMVA","tau25_mediumRNN_tracktwoMVA","xe50"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwoEF_xe50', 'L1_EM15VHI_2TAU12IM_XE35',['L1_EM15VHI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau', 'BW:Egamma'],-1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwoEF", "xe50"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_mediumRNN_tracktwoMVA_xe50', 'L1_EM15VHI_2TAU12IM_XE35',['L1_EM15VHI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau', 'BW:Egamma'],-1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_mediumRNN_tracktwoMVA", "xe50"]]],
        ['e17_lhmedium_nod0_tau25_medium1_tracktwoEF_xe50', 'L1_EM15VHI_2TAU12IM_XE35',['L1_EM15VHI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_medium1_tracktwoEF", "xe50"]]],
        ['e17_lhmedium_nod0_tau25_mediumRNN_tracktwoMVA_xe50', 'L1_EM15VHI_2TAU12IM_XE35',['L1_EM15VHI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_mediumRNN_tracktwoMVA", "xe50"]]],
        ['mu14_ivarloose_tau25_medium1_tracktwoEF',      'L1_MU10_TAU12IM_J25_2J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Primary:17000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwoEF"]]],        
        ['mu14_ivarloose_tau25_mediumRNN_tracktwoMVA',      'L1_MU10_TAU12IM_J25_2J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Primary:17000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],        
        ['mu14_ivarloose_tau25_medium1_tracktwoEF_L1DR-MU10TAU12I_TAU12I-J25', 'L1_DR-MU10TAU12I_TAU12I-J25',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwoEF"]]],
        ['mu14_ivarloose_tau25_mediumRNN_tracktwoMVA_L1DR-MU10TAU12I_TAU12I-J25', 'L1_DR-MU10TAU12I_TAU12I-J25',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwoEF_L1DR-EM15TAU12I-J25', 'L1_DR-EM15TAU12I-J25',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:ElectronTau', 'BW:Tau'],-1,['serial',-1 ,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwoEF"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_mediumRNN_tracktwoMVA_L1DR-EM15TAU12I-J25', 'L1_DR-EM15TAU12I-J25',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:ElectronTau', 'BW:Tau'],-1,['serial',-1 ,["e17_lhmedium_nod0_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwoEF',  'L1_EM15VHI_2TAU12IM_J25_3J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream],['Primary:17000','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwoEF"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_mediumRNN_tracktwoMVA',  'L1_EM15VHI_2TAU12IM_J25_3J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream],['Primary:17000','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],
        ['e24_lhmedium_nod0_ivarloose_tau35_medium1_tracktwoEF',  'L1_EM20VHI_TAU20IM_2TAU20_J25_3J20',['L1_EM20VHI','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e24_lhmedium_nod0_ivarloose","tau35_medium1_tracktwoEF"]]],
        ['e24_lhmedium_nod0_ivarloose_tau35_mediumRNN_tracktwoMVA',  'L1_EM20VHI_TAU20IM_2TAU20_J25_3J20',['L1_EM20VHI','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e24_lhmedium_nod0_ivarloose","tau35_mediumRNN_tracktwoMVA"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwoEF_L1EM15VHI_2TAU12IM_4J12',   'L1_EM15VHI_2TAU12IM_4J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream],['Primary:20000','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwoEF"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_mediumRNN_tracktwoMVA_L1EM15VHI_2TAU12IM_4J12',   'L1_EM15VHI_2TAU12IM_4J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream],['Primary:20000','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],
        ['mu14_ivarloose_tau25_medium1_tracktwoEF_L1MU10_TAU12IM_3J12',      'L1_MU10_TAU12IM_3J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwoEF"]]],
        ['mu14_ivarloose_tau25_mediumRNN_tracktwoMVA_L1MU10_TAU12IM_3J12',      'L1_MU10_TAU12IM_3J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],
        ['mu14_ivarloose_tau35_medium1_tracktwoEF',  'L1_MU10_TAU20IM',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['Primary:17000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_medium1_tracktwoEF"]]],
        ['mu14_ivarloose_tau35_mediumRNN_tracktwoMVA',  'L1_MU10_TAU20IM',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['Primary:17000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_mediumRNN_tracktwoMVA"]]],
        ['mu14_ivarloose_L1MU11_tau35_medium1_tracktwoEF_L1MU11_TAU20IM',  'L1_MU11_TAU20IM',['L1_MU11','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose_L1MU11","tau35_medium1_tracktwoEF"]]],
        ['mu14_ivarloose_L1MU11_tau35_mediumRNN_tracktwoMVA_L1MU11_TAU20IM',  'L1_MU11_TAU20IM',['L1_MU11','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose_L1MU11","tau35_mediumRNN_tracktwoMVA"]]],
        ['mu14_tau25_medium1_tracktwoEF_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14", "tau25_medium1_tracktwoEF","xe50"]]],
        ['mu14_tau25_mediumRNN_tracktwoMVA_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14", "tau25_mediumRNN_tracktwoMVA","xe50"]]],
        ['mu14_ivarloose_tau25_medium1_tracktwoEF_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose", "tau25_medium1_tracktwoEF","xe50"]]],
        ['mu14_ivarloose_tau25_mediumRNN_tracktwoMVA_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose", "tau25_mediumRNN_tracktwoMVA","xe50"]]],
        ['tau35_medium1_tracktwoEF_xe70_L1XE45',  'L1_TAU20IM_2J20_XE45',['L1_TAU20IM','L1_XE45'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwoEF","xe70_L1XE45"]]],  
        ['tau35_mediumRNN_tracktwoMVA_xe70_L1XE45',  'L1_TAU20IM_2J20_XE45',['L1_TAU20IM','L1_XE45'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_mediumRNN_tracktwoMVA","xe70_L1XE45"]]],  


        # ATR-17807
        #tracktwoEF
        ['g25_medium_L1EM24VHI_tau25_dikaonmass_tracktwoEF_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dikaonmass_tracktwoEF'],False] ],
        ['g25_medium_L1EM24VHI_tau25_dipion1loose_tracktwoEF_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dipion1loose_tracktwoEF'],False] ],
        ['g25_medium_L1EM24VHI_tau25_dipion2_tracktwoEF_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dipion2_tracktwoEF'],False] ],
        ['g25_medium_L1EM24VHI_tau25_kaonpi1_tracktwoEF_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_kaonpi1_tracktwoEF'],False] ],
        ['g25_medium_L1EM24VHI_tau25_singlepion_tracktwoEF_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_singlepion_tracktwoEF'],False] ],
        #tracktwoMVA
        ['g25_medium_L1EM24VHI_tau25_dikaonmass_tracktwoMVA_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dikaonmass_tracktwoMVA'],False] ],
        ['g25_medium_L1EM24VHI_tau25_dipion1loose_tracktwoMVA_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dipion1loose_tracktwoMVA'],False] ],
        ['g25_medium_L1EM24VHI_tau25_dipion2_tracktwoMVA_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dipion2_tracktwoMVA'],False] ],
        ['g25_medium_L1EM24VHI_tau25_kaonpi1_tracktwoMVA_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_kaonpi1_tracktwoMVA'],False] ],
        ['g25_medium_L1EM24VHI_tau25_singlepion_tracktwoMVA_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_singlepion_tracktwoMVA'],False] ],


        # lepton + jets 
        ### ATR-15524
        ['e20_lhtight_nod0_ivarloose_3j20_L1EM18VHI_3J20', 'L1_EM18VHI_3J20',    ['L1_EM18VHI',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1, ['serial',-1,["e20_lhtight_nod0_ivarloose","3j20"] ]],

        #ATR-14196
        ['g25_medium_L1EM22VHI_4j35_0eta490_invm1000', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['Primary:17000','RATE:EgammaJet', 'BW:Egamma'], -1,['serial',-1,["g25_medium_L1EM22VHI","4j35_0eta490_invm1000"]]],

        ['g25_medium_L1EM22VHI_j35_0eta490_bmv2c1077_split_3j35_0eta490_invm700', 'L1_EM22VHI', ['L1_EM22VHI', '', ''], [PhysicsStream], ['Primary:17000','RATE:EgammaJet', 'BW:BJet_combined','BW:Egamma'],-1,['serial',-1,['g25_medium_L1EM22VHI','3j35_0eta490_invm700','j35_0eta490_bmv2c1077_split']]],
        ['g25_medium_L1EM22VHI_2j35_0eta490_bmv2c1077_split_2j35_0eta490', 'L1_EM22VHI', ['L1_EM22VHI', '', ''], [PhysicsStream], ['Primary:17000','RATE:EgammaJet', 'BW:BJet_combined','BW:Egamma'],-1,['serial',-1,['g25_medium_L1EM22VHI','2j35_0eta490','2j35_0eta490_bmv2c1077_split']]],

        #ATR-15062
        ['g20_tight_icaloloose_j35_bmv2c1077_split_3j35_0eta490_invm500', 'L1_EM18VHI_MJJ-300', ['','',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g20_tight_icaloloose","3j35_0eta490_invm500","j35_bmv2c1077_split"]]],
        ['g27_medium_L1EM24VHI_j35_bmv2c1077_split_3j35_0eta490_invm700', 'L1_EM24VHI', ['L1_EM24VHI','',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g27_medium_L1EM24VHI","3j35_0eta490_invm700","j35_bmv2c1077_split"]]],
        ['g27_medium_L1EM24VHI_2j35_bmv2c1077_split_2j35_0eta490', 'L1_EM24VHI', ['L1_EM24VHI','',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g27_medium_L1EM24VHI","2j35_0eta490","2j35_bmv2c1077_split"]]],

        # backups for L1Topo VBF items
        ['j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split','L1_J40.0ETA25_2J25_J20.31ETA49', ['','','','',''], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet_combined' ], -1, ['serial',-1,['j80_0eta240','j60','j45_320eta490_AND','2j35_gsc45_bmv2c1070_split'] ]],

        ['j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49','L1_J25.0ETA23_2J15.31ETA49',['',''], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet_combined' ], -1, ['serial',-1,['2j45_320eta490_L1J25.0ETA23_2J15.31ETA49','j45_gsc55_bmv2c1070_split'] ]],

        # 2017 g+tau chains (ATR-16266)
	# ATR-14947
        ['g25_medium_L1EM24VHI_tau25_dikaonmass_tracktwo_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dikaonmass_tracktwo'],False]],
        ['g25_medium_L1EM24VHI_tau25_singlepion_tracktwo_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_singlepion_tracktwo'],False]],
        ['g25_medium_L1EM24VHI_tau25_dipion1loose_tracktwo_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dipion1loose_tracktwo'],False]],
        ['g25_medium_L1EM24VHI_tau25_dipion2_tracktwo_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dipion2_tracktwo'],False]],
        ['g25_medium_L1EM24VHI_tau25_kaonpi1_tracktwo_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_kaonpi1_tracktwo'],False]],

        ### ATR-14196
       	['g85_tight_L1EM22VHI_3j50noL1', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['Primary:17000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g85_tight_L1EM22VHI","3j50noL1"]]],

     #ATR-14196
        ['j80_bmv2c1050_split_xe60_L12J50_XE40','L1_2J50_XE40',  [],  [PhysicsStream], ['Primary:17000','RATE:JetMET', 'BW:MET', 'BW:BJet_combined'], -1,['serial',-1,["xe60","j80_bmv2c1050_split"]]],
        ['mu14_ivarloose_3j20_L1MU10_3J20', 'L1_MU10_3J20', ['L1_MU10_3J20', ''], [PhysicsStream], ['Primary:20000','RATE:MuonJet', 'BW:Muon','BW:Jet'], -1,['serial',-1,['mu14_ivarloose', '3j20'] ],True],

        ['e9_lhvloose_nod0_mu20_mu8noL1',        'L1_MU20MU21',  ['L1_EM3', 'L1_MU20', ''],   [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['serial',-1,['mu20','mu8noL1','e9_lhvloose_nod0'] ]],
        ['e9_lhvloose_nod0_mu20_mu8noL1_L1EM7_MU20',    'L1_EM7_MU20',  ['L1_EM7','L1_MU20',''],   [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['serial',-1,['mu20','mu8noL1','e9_lhvloose_nod0'] ]],

        #ATR-17462 low pt photon triggers
        ['2g6_tight_icalotight_L1J100', 'L1_J100', ['L1_2EM3'],  [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1],

        # photon + mulit-jets ATR-17502
        ['g45_loose_6j45_0eta240', 'L1_4J15.0ETA25', ['L1_EM15VH','L1_4J15.0ETA25'], [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g45_loose","6j45_0eta240"]]],

        ['e9_lhvloose_nod0_e5_lhvloose_nod0_bBeexM6000t_2mu4_nomucomb_L1BPH-0M9-EM7-EM5_2MU4',  'L1_BPH-0M9-EM7-EM5_2MU4',   ['L1_EM7','L1_EM3','L1_2MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag],-1,['parallel',-1,[] ]],

			 ]


    if TriggerFlags.doFTK():
            TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures() + [
             ]


    TriggerFlags.HeavyIonSlice.signatures = [
        ]


    TriggerFlags.BeamspotSlice.signatures = [
        ]   


    if TriggerFlags.doFTK():
        TriggerFlags.BeamspotSlice.signatures = TriggerFlags.BeamspotSlice.signatures() + [                                                                 
            ]

    TriggerFlags.MinBiasSlice.signatures = [
        ]
        
    TriggerFlags.CalibSlice.signatures   = [
        ]

    TriggerFlags.CosmicSlice.signatures  = [ 
        ]
    TriggerFlags.StreamingSlice.signatures = [
        ]

    if TriggerFlags.doFTK():
        TriggerFlags.StreamingSlice.signatures = TriggerFlags.StreamingSlice.signatures() + [
            ]
     
    TriggerFlags.MonitorSlice.signatures = [
        ]

    TriggerFlags.EnhancedBiasSlice.signatures = [
        ]


    ###############################################################
    #################################################################
    signatureList=[]
    for prop in dir(TriggerFlags):
        if prop[-5:]=='Slice':
            sliceName=prop
            slice=getattr(TriggerFlags,sliceName)
            if slice.signatures():
                signatureList.extend(slice.signatures())
            else:
                log.debug('SKIPPING '+str(sliceName))
    mySigList=[]
    for allInfo in signatureList:
        mySigList.append(allInfo[0])
    mydict={}
    for chain in mySigList:
        mydict[chain]=[-1,0,0]
    mydict.update(Prescales.HLTPrescales_cosmics)
    mydict.update(Prescales.HLTPrescales_unprescaled_only_physics_prescale)
    mydict.update(Prescales.L1Prescales_unprescaled_only_physics_prescale)
    from copy import deepcopy
    Prescales.HLTPrescales_cosmics = deepcopy(mydict)
    Prescales.L1Prescales_unprescaled_only_physics_prescale  = deepcopy(mydict)
    Prescales.HLTPrescales_unprescaled_only_physics_prescale = deepcopy(mydict)


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
        'larnoiseburst_rerun'       : [ 0, 0 , 1, "LArNoiseBurst"],
        'larnoiseburst_loose_rerun' : [ 0, 0 , 1, "LArNoiseBurst"],
        }

    L1Prescales_cosmics  = {}
    HLTPrescales_cosmics = {}
    chain_list=[
        ]
    HLTPrescales_cosmics.update(dict(map(None,chain_list,len(chain_list)*[ [1, 0, 0] ])))

    HLTPrescales_unprescaled_only_physics_prescale = {}
    HLTPrescales_unprescaled_only_physics_prescale.update(dict(map(None,chain_list,len(chain_list)*[ [-1, 0, -1] ])))
    L1Prescales_unprescaled_only_physics_prescale = {}
    L1Prescales_unprescaled_only_physics_prescale.update(dict(map(None,chain_list,len(chain_list)*[ [-1, 0, -1] ])))


    HLT_chain_list = [
        'tau35_medium1_tracktwo_tau25_medium1_tracktwo_tautsf_L1DR-TAU20ITAU12I-J25',
        'e60_etcut_trkcut_L1EM24VHIM_xs30_j15_perf_xe30_6dphi15_mt35',
        '2e24_lhvloose_nod0_2j35_boffperf_split',
        '2j35_bmv2c1070_split_2j35_bmv2c1085_split_L14J15.0ETA25',
        '2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15.0ETA25',
        '2j35_gsc45_bmv2c1060_split_3j35_gsc45_boffperf_split',
        '2j45_gsc55_bmv2c1050_split_ht300_L1HT190-J15s5.ETA21',
        '2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15.0ETA25',
        '2mu14_2j35_boffperf_split',
        '3j35_bmv2c1070_split_j35_L14J15.0ETA25',
        '3j50_gsc65_bmv2c1077_split_L13J35.0ETA23',
        '4j35_bmv2c1077_split_L14J15.0ETA25',
        'e28_lhmedium_nod0_L1EM24VHI_mu8noL1_2j35_boffperf_split',
        'e28_lhtight_nod0_ivarloose_j110_gsc150_boffperf_split_j35_boffperf_split',
        'e28_lhtight_nod0_ivarloose_j150_boffperf_split_j35_boffperf_split',
        'g25_medium_L1EM22VHI_2j35_0eta490_bmv2c1077_split_2j35_0eta490',
        'g25_medium_L1EM22VHI_j35_0eta490_bmv2c1077_split_3j35_0eta490_invm700',
        'ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF_AND_2j35_gsc45_bmv2c1070_split',
        'j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30',
        'j150_gsc175_bmv2c1060_split_j45_gsc60_bmv2c1060_split',
        'j175_gsc225_bmv2c1040_split',
        'j225_gsc275_bmv2c1060_split',
        'j225_gsc300_bmv2c1070_split',
        'j225_gsc360_bmv2c1077_split',
        'j45_gsc55_bmv2c1050_split_ht700_L1HT190-J15s5.ETA21',
        'j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49',
        'j55_gsc75_bmv2c1040_split_3j55_gsc75_boffperf_split',
        'j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490',
        'j60_gsc85_bmv2c1050_split_3j60_gsc85_boffperf_split',
        'j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split',
        'mu26_ivarmedium_j110_gsc150_boffperf_split_j35_boffperf_split',
        'mu26_ivarmedium_j150_boffperf_split_j35_boffperf_split',
        '2mu10_bBmumuxv2',
        '2mu10_bJpsimumu',
        '2mu10_bUpsimumu',
        '2mu6_bBmumu_Lxy0_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
        '2mu6_bBmumux_BpmumuKp_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
        '2mu6_bBmumux_BsmumuPhi_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
        '2mu6_bJpsimumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
        '2mu6_bJpsimumu_Lxy0_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
        '2mu6_bUpsimumu_L1BPH-8M15-2MU6_BPH-0DR22-2MU6',
        '2mu6_mu4_bTau_L12MU6_3MU4',
        '2mu6_mu4_bUpsi',
        '3mu4_bJpsi',
        '3mu4_bPhi',
        '3mu4_bTau',
        '3mu4_bUpsi',
        '4mu4',
        'mu11_2mu4noL1_bTau_L1MU11_2MU6',
        'mu11_mu6_bBmumu',
        'mu11_mu6_bBmumux_BpmumuKp',
        'mu11_mu6_bBmumuxv2',
        'mu11_mu6_bDimu',
        'mu11_mu6_bDimu2700',
        'mu11_mu6_bDimu2700_Lxy0',
        'mu11_mu6_bDimu_Lxy0',
        'mu11_mu6_bJpsimumu',
        'mu11_mu6_bJpsimumu_Lxy0',
        'mu11_mu6_bPhi',
        'mu11_mu6_bTau',
        'mu11_mu6_bUpsimumu',
        'mu11_mu6noL1_bPhi_L1MU11_2MU6',
        'mu20_mu6noL1_bTau',
        'mu6_2mu4_bTau_L1MU6_3MU4',
        'mu6_2mu4_bUpsi',
        #'satu20em_l1satmon_L1J100.31ETA49',
        #'satu20em_l1satmon_L1J100.31ETA49_FIRSTEMPTY',
        '2e12_lhloose_nod0_mu10',
        '2e17_lhvloose_nod0_L12EM15VHI',
        '2e24_lhvloose_nod0',
        '2e5_lhvloose_nod0_j40_xe70_pufit_2dphi10_L1J40_XE50_DPHI-J20s2XE50',
        '2e5_lhvloose_nod0_j40_xe70_pufit_2dphi10_L1XE60',
        '2g10_loose_mu20',
        '2g20_tight_icalovloose_L12EM15VHI',
        '2g22_tight_L12EM15VHI',
        '2g25_loose_g15_loose',
        '2g25_tight_L12EM20VH',
        '2g50_loose_L12EM20VH',
        'e12_lhloose_nod0_2mu10',
        'e140_lhloose_nod0',
        'e17_lhloose_nod0_mu14',
        'e20_lhtight_nod0_ivarloose_3j20_L1EM18VHI_3J20',
        'e24_lhmedium_nod0_2g12_loose',
        'e24_lhmedium_nod0_L1EM20VH_g25_medium',
        'e24_lhvloose_nod0_2e12_lhvloose_nod0_L1EM20VH_3EM10VH',
        'e25_mergedtight_g35_medium_Heg',
        'e25_mergedtight_ivarloose_g35_medium_icalovloose_Heg',
        'e26_lhmedium_nod0_mu8noL1',
        'e26_lhtight_nod0_e15_etcut_L1EM7_Zee',
        'e26_lhtight_nod0_ivarloose',
        'e300_etcut',
        'e5_lhloose_nod0_j40_xe70_pufit_2dphi10_L1XE60',
        'e5_lhloose_nod0_j50_xe70_pufit_2dphi10_L1J40_XE50_DPHI-J20s2XE50',
        'e5_lhmedium_nod0_mu4_j30_xe65_pufit_2dphi10_L1MU4_XE60',
        'e60_lhmedium_nod0',
        'e70_lhloose_nod0_xe70noL1',
        'e7_lhmedium_nod0_mu24',
        'g0_hiptrt_L1EM22VHI',
        'g140_loose',
        'g15_loose_2mu10_msonly',
        'g20_tight_icaloloose_j35_bmv2c1077_split_3j35_0eta490_invm500',
        'g25_medium_L1EM22VHI_4j35_0eta490_invm1000',
        'g25_medium_mu24',
        'g27_medium_L1EM24VHI_2j35_bmv2c1077_split_2j35_0eta490',
        'g27_medium_L1EM24VHI_j35_bmv2c1077_split_3j35_0eta490_invm700',
        'g300_etcut',
        'g35_loose_L1EM24VHI_mu15_mu2noL1',
        'g35_loose_L1EM24VHI_mu18',
        'g35_medium_g25_medium_L12EM20VH',
        'g35_tight_icalotight_L1EM24VHI_mu15noL1_mu2noL1',
        'g35_tight_icalotight_L1EM24VHI_mu18noL1',
        'g80_loose_icalovloose_xe80noL1',
        'g80_loose_xe80noL1',
        'g85_tight_L1EM22VHI_3j50noL1',
        'g85_tight_icalovloose_L1EM22VHI_3j50noL1',
        'e9_lhvloose_nod0_mu20_mu8noL1',
        'e9_lhvloose_nod0_mu20_mu8noL1_L1EM7_MU20',
        '10j40_L14J15',
        '10j40_L14J150ETA25',
        '2j225_gsc250_boffperf_split_0eta240_j85_gsc120_boffperf_split_0eta240',
        '3j200',
        '4j120',
        '4j85_gsc115_boffperf_split',
        '5j50_gsc70_boffperf_split_0eta240_L14J150ETA25',
        '5j60_gsc85_boffperf_split_L14J150ETA25',
        '5j70_0eta240_L14J15',
        '5j70_0eta240_L14J150ETA25',
        '5j85_L14J15',
        '5j85_L14J150ETA25',
        '6j45_gsc55_boffperf_split_0eta240_L14J150ETA25',
        '6j50_gsc70_boffperf_split_L14J150ETA25',
        '6j55_0eta240_L14J15',
        '6j55_0eta240_L14J150ETA25',
        '6j70_L14J15',
        '6j70_L14J150ETA25',
        '7j35_gsc45_boffperf_split_0eta240_L14J150ETA25',
        '7j35_gsc45_boffperf_split_L14J150ETA25',
        '7j45_0eta240_L14J150ETA25',
        '7j45_L14J15',
        '7j45_L14J150ETA25',
        'ht1000_L1J100',
        'j0_perf_ds1_L1J100',
        'j225_gsc420_boffperf_split',
        'j260_320eta490',
        'j260_320eta490_lcw_sub',
        'j260_320eta490_nojcalib',
        'j260_320eta490_sub',
        'j30_jes_cleanLLP_PS_llp_L1LLP-NOMATCH',
        'j30_jes_cleanLLP_PS_llp_L1LLP-RO',
        'j30_jes_cleanLLP_PS_llp_L1TAU100',
        'j30_jes_cleanLLP_PS_llp_noiso_L1LLP-NOMATCH',
        'j30_jes_cleanLLP_PS_llp_noiso_L1LLP-RO',
        'j30_jes_cleanLLP_PS_llp_noiso_L1TAU100',
        #'j360_320eta490',
        'j420',
        'j460_a10_lcw_subjes_L1J100',
        'j460_a10r_L1J100',
        'j460_a10t_lcw_jes_L1J100',
        'j80_bmv2c1050_split_xe60_L12J50_XE40',
        '2mu14',
        '2mu4_invm1_j20_xe40_pufit_2dphi10_L12MU4_J20_XE30_DPHI-J20s2XE30',
        '2mu4_invm1_j20_xe60_pufit_2dphi10_L12MU4_J40_XE50',
        '3mu6',
        '3mu6_msonly',
        'e5_lhvloose_nod0_mu4_j30_xe40_pufit_2dphi10_L1MU4_J30_XE40_DPHI-J20s2XE30',
        'j30_muvtx',
        'j30_muvtx_noiso',
        'mu10_mgonly_L1LATE-MU10_J50',
        'mu10_mgonly_L1LATE-MU10_XE40',
        'mu11_2mu4noL1_bNocut_L1MU11_2MU6',
        'mu13_mu13_idperf_Zmumu',
        'mu14_ivarloose_3j20_L1MU10_3J20',
        'mu20_2mu4noL1',
        'mu20_ivarmedium_L1MU10_2J15_J20',
        'mu20_ivarmedium_L1MU10_2J20',
        'mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_J40',
        'mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_XE30',
        'mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU4_UNPAIRED_ISO',
        'mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU6_EMPTY',
        'mu22_mu8noL1',
        'mu26_ivarmedium',
        'mu4_j90_xe90_pufit_2dphi10_L1MU4_J50_XE50_DPHI-J20s2XE30',
        'mu4_j90_xe90_pufit_2dphi10_L1MU4_XE60',
        'mu50',
        'mu60_0eta105_msonly',
        'e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo',
        'e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',
        'e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1EM15VHI_2TAU12IM_4J12',
        'e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_xe50',
        'e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50',
        'e24_lhmedium_nod0_ivarloose_tau35_medium1_tracktwo',
        'g25_medium_L1EM24VHI_tau25_dikaonmass_tracktwo_50mVis10000',
        'g25_medium_L1EM24VHI_tau25_dipion1loose_tracktwo_50mVis10000',
        'g25_medium_L1EM24VHI_tau25_dipion2_tracktwo_50mVis10000',
        'g25_medium_L1EM24VHI_tau25_kaonpi1_tracktwo_50mVis10000',
        'g25_medium_L1EM24VHI_tau25_singlepion_tracktwo_50mVis10000',
        'mu14_ivarloose_L1MU11_tau35_medium1_tracktwo_L1MU11_TAU20IM',
        'mu14_ivarloose_tau25_medium1_tracktwo',
        'mu14_ivarloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25',
        'mu14_ivarloose_tau25_medium1_tracktwo_L1MU10_TAU12IM_3J12',
        'mu14_ivarloose_tau25_medium1_tracktwo_xe50',
        'mu14_ivarloose_tau35_medium1_tracktwo',
        'mu14_tau25_medium1_tracktwo_xe50',
        'tau160_medium1_tracktwo_L1TAU100',
        'tau35_medium1_tracktwo_tau25_medium1_tracktwo_03dR30_L1DR-TAU20ITAU12I-J25',
        'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I-J25',
        'tau35_medium1_tracktwo_xe70_L1XE45',
        'tau40_medium1_tracktwo_tau35_medium1_tracktwo',
        'tau60_medium1_tracktwo_tau25_medium1_tracktwo_xe50',
        'tau80_medium1_tracktwo_L1TAU60_tau35_medium1_tracktwo_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I',
        'tau80_medium1_tracktwo_L1TAU60_tau60_medium1_tracktwo_L1TAU40',
        ]

    HLTPrescales_unprescaled_only_physics_prescale = {}
    HLTPrescales_unprescaled_only_physics_prescale.update(dict(map(None,HLT_chain_list,len(HLT_chain_list)*[ [1, 0, -1] ])))



######################################################
# TIGHT physics prescales
######################################################
from copy import deepcopy
# enable if desired: # setting all L1 prescales to 1
#Prescales.L1Prescales = dict([(ctpid,1) for ctpid in Prescales.L1Prescales]) 
 
Prescales.L1Prescales_tight_physics_prescale  = deepcopy(Prescales.L1Prescales)
Prescales.HLTPrescales_tight_physics_prescale = deepcopy(Prescales.HLTPrescales)

chain_list=[
    ]
Prescales.HLTPrescales_tight_physics_prescale.update(dict(map(None,chain_list,len(chain_list)*[ [-1, 0,-1] ])))
######################################################
