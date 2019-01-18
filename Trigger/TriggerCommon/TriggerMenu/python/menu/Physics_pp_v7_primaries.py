# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
#------------------------------------------------------------------------#
def setupMenu():

    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenu.menu.Physics_pp_v7_primaries.py' )

    from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1 as tcl1
    if tcl1.current:
        log.info("L1 items: %s " % tcl1.current.menu.items.itemNames())
    else:
        log.info("ERROR L1 menu has not yet been defined")

    PhysicsStream="Main"

    # stream, BW and RATE tags for Bphysics items that appear in Muon and Bphysics slice.signatures
    #BPhysicsStream     = "BphysLS"
    #BMultiMuonStream   = "Main"  
    #RATE_BphysTag      = 'RATE:Bphysics'
    #RATE_BMultiMuonTag = 'RATE:BMultiMuon'  # can become RATE:BMultiMuon' with one letter change 
    #BW_BphysTag        = 'BW:Bphys'
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
#        ['mu26_ivarmedium_zROItest', 'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu28_ivarmedium',	     'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Primary:20000','RATE:SingleMuon', 'BW:Muon'], -1],


        ['mu80',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Primary:20000','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu50',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Primary:17000','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu60',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Primary:20000','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu60_0eta105_msonly',    'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Primary:20000','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu80_msonly_3layersEC',  'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Primary:20000','RATE:SingleMuon', 'BW:Muon'], -1],


        #ATR-17539
        ['mu20_ivarmedium_mu8noL1',    'L1_MU20MU21',  ['L1_MU20',''], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_ivarmedium','mu8noL1']]],

        #ATR-17448
        ['mu26_ivartight',          'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Primary:20000','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu28_ivartight',          'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Primary:20000','RATE:SingleMuon', 'BW:Muon'], -1],


        # multi muons
        ['2mu14',                  'L1_2MU10',          [], [PhysicsStream, 'express'], ['Primary:20000','RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu15_L12MU10',          'L1_2MU10',          [], [PhysicsStream], ['Primary:20000','RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu6',                   'L1_3MU6',           [], [PhysicsStream], ['Primary:20000','RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu6_msonly',            'L1_3MU6',           [], [PhysicsStream], ['Primary:20000','RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu8_msonly',            'L1_3MU6',           [], [PhysicsStream], ['Primary:20000','RATE:MultiMuon', 'BW:Muon'], -1],

        ['mu24_mu10noL1',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','mu10noL1']]],
        ['mu24_mu12noL1' ,         'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','mu12noL1']]],
        ['mu24_2mu4noL1',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','2mu4noL1']]],
        ['mu26_mu8noL1',           'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu26','mu8noL1']]],
        ['mu26_mu10noL1',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu26','mu10noL1']]],
        ['mu28_mu8noL1',           'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu28','mu8noL1']]],
        ['mu22_mu8noL1',           'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Primary:17000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','mu8noL1']]],
        ['mu22_2mu4noL1',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','2mu4noL1']]],
        ['mu20_2mu4noL1',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','2mu4noL1']]],
       
	  # multi-muon TagandProbe chains (ATR-17625)
	  ['mu22_mu8noL1_TagandProbe', 'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Primary:17000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','mu8noL1']]],

        # Primary (multi muon chains)
#				#ATR-15246
        ['mu22_mu8noL1_mu6noL1',   'L1_MU20MU21',       ['L1_MU20','',''], [PhysicsStream], ['Primary:20000','RATE:MultiMuon', 'BW:Muon'],  -1,['serial',-1,['mu22','mu8noL1','mu6noL1']]],
                 

        ['mu20_ivarmedium_L1MU10_2J20', 'L1_MU10_2J20', [], [PhysicsStream], ['Primary:20000','RATE:MuonJet', 'BW:Muon','BW:Jet'], -1],                
        ['mu20_ivarmedium_L1MU10_2J15_J20', 'L1_MU10_2J15_J20', [], [PhysicsStream], ['Primary:17000','RATE:MuonJet', 'BW:Muon','BW:Jet'], -1],  

        #nscan05 triggers rejecting CB muons

        ['mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_XE30', 'L1_MU20_XE30', ['L1_MU20_XE30','L2_mu20_msonly_iloosems'], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_iloosems','mu6noL1_msonly_nscan05']]],
        ['mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_J40', 'L1_MU20_J40',   ['L1_MU20_J40','L2_mu20_msonly_iloosems'], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_iloosems','mu6noL1_msonly_nscan05']]],
        ['mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU6_EMPTY', 'L1_MU6_EMPTY',   ['L1_MU20','L2_mu20_msonly_iloosems'], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_iloosems','mu6noL1_msonly_nscan05']]],
        ['mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU4_UNPAIRED_ISO', 'L1_MU4_UNPAIRED_ISO',   ['L1_MU20','L2_mu20_msonly_iloosems'], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_iloosems','mu6noL1_msonly_nscan05']]],
        ['mu6_dRl1_mu20_msonly_iloosems_mu6noL1_dRl1_msonly','L1_MU11_2MU6', [['L1_MU6','L1_MU6'],'L1_MU20',''], [PhysicsStream], ['Primary:20000','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu6_dRl1','mu20_msonly_iloosems','mu6noL1_dRl1_msonly']]],


        ['mu10_mgonly_L1LATE-MU10_J50',          'L1_LATE-MU10_J50',           [], [PhysicsStream], ['Primary:20000','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10_mgonly_L1LATE-MU10_XE50',         'L1_LATE-MU10_XE50',          [], [PhysicsStream], ['Primary:20000','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10_mgonly_L1LATE-MU10_XE40',         'L1_LATE-MU10_XE40',          [], [PhysicsStream], ['Primary:20000','RATE:SingleMuon', 'BW:Muon'], -1],

        #Low-mu run triggers

			 ]


    if TriggerFlags.doFTK():
        TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [

            ]




    TriggerFlags.JetSlice.signatures = [   

        #####
        ######################### Single-jet small-R trigger chains
        #####
        
        # Standard central jet triggers
        ['j420',                        'L1_J100', [], [PhysicsStream,'express'], ['Primary:17000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j440',                        'L1_J120', [], [PhysicsStream], ['Primary:17000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j450',                        'L1_J120', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j460',                        'L1_J120', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j480',                        'L1_J120', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j500',                        'L1_J120', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j520',                        'L1_J120', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],

        # Standard gsc central jet triggers (primaries)
        ['j225_gsc420_boffperf_split',  'L1_J100', [], [PhysicsStream,'express'], ['Primary:17000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j225_gsc440_boffperf_split',  'L1_J120', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j225_gsc450_boffperf_split',  'L1_J120', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j225_gsc460_boffperf_split',  'L1_J120', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j225_gsc480_boffperf_split',  'L1_J120', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j225_gsc500_boffperf_split',  'L1_J120', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],

        # SoftKiller central jet triggers
        #['j35_sktc_lcw_nojcalib',       'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],

        # Standard forward jet triggers
        ['j260_320eta490',              'L1_J75.31ETA49', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet',  'BW:Jet'], -1],
        #['j360_320eta490',              'L1_J100.31ETA49', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet',  'BW:Jet'], -1],

        # Alternative calibration forward jet triggers
        ['j260_320eta490_sub',          'L1_J75.31ETA49', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet',  'BW:Jet'], -1],
        ['j260_320eta490_nojcalib',     'L1_J75.31ETA49', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet',  'BW:Jet'], -1],
        
        
        
        #####
        ######################### Single-jet large-R trigger chains
        #####

        # Standard topocluster large-R jet triggers
        ['j460_a10_lcw_subjes_L1J100',      'L1_J100', [], [PhysicsStream], ['Primary:17000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10_lcw_subjes_L1J100',      'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j500_a10_lcw_subjes_L1J100',      'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j520_a10_lcw_subjes_L1J100',      'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j540_a10_lcw_subjes_L1J100',      'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],

        # Standard reclustered large-R jet triggers
        ['j460_a10r_L1J100',                'L1_J100', [], [PhysicsStream], ['Primary:17000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10r_L1J100',                'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j500_a10r_L1J100',                'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j520_a10r_L1J100',                'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j540_a10r_L1J100',                'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],


        # Standard trimmed large-R jet triggers
        ['j460_a10t_lcw_jes_L1J100',        'L1_J100', [], [PhysicsStream], ['Primary:17000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10t_lcw_jes_L1J100',        'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j500_a10t_lcw_jes_L1J100',        'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j520_a10t_lcw_jes_L1J100',        'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j540_a10t_lcw_jes_L1J100',        'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],

        ['j420_a10t_lcw_jes_40smcINF_L1J100',               'L1_J100', [], [PhysicsStream], ['Primary:17000','RATE:SingleJet', 'BW:Jet'], -1],
        ['j440_a10t_lcw_jes_40smcINF_L1J100',               'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],

        # Trimmed + mass-cut single-jet support triggers (for mass efficiency bootstrap)
        ['2j330_a10t_lcw_jes_40smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['2j340_a10t_lcw_jes_40smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],
        ['2j350_a10t_lcw_jes_40smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:SingleJet', 'BW:Jet'], -1],

        # Trimmed + mass-cut asymmetric di-jet triggers (for rate optimization)
        ['j360_a10t_lcw_jes_60smcINF_j360_a10t_lcw_jes_L1SC111',   'L1_SC111-CJ15', [], [PhysicsStream], ['Primary:17000','RATE:SingleJet', 'BW:Jet'], -1],


        # ATR-17691 Trimmed large-R jet trigger analysis specific chains with mass cuts
        ['j370_a10t_lcw_jes_60smcINF_j370_a10t_lcw_jes_L1SC111',   'L1_SC111-CJ15', [], [PhysicsStream], ['Primary:17000','RATE:SingleJet', 'BW:Jet'], -1],
 

        #####
        ######################### Multi-jet small-R trigger chains
        #####

        # 3-jet single threshold triggers
        ['3j200',                       'L1_J100', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['3j225',                       'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['3j250',                       'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],

        # 4-jet single threshold triggers
        ['4j120',                       'L1_3J50', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['4j130',                       'L1_3J50', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],

        # 4-jet gsc single threshold triggers
        ['4j85_gsc115_boffperf_split',  'L1_3J50', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['4j85_gsc120_boffperf_split',  'L1_3J50', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['4j85_gsc130_boffperf_split',  'L1_3J50', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],

        # 5-jet single threshold triggers
        ['5j85_L14J15',                 'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85_L14J150ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85',                        'L1_4J20', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j90_L14J150ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j90',                        'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j100_L14J150ETA25',          'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j100',                       'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j90_L14J15',                 'L1_4J15', [], [PhysicsStream], ['Primary:17000', 'RATE:MultiJet',  'BW:Jet'], -1],
        ['5j100_L14J15',                'L1_4J15', [], [PhysicsStream], ['Primary:17000', 'RATE:MultiJet',  'BW:Jet'], -1],
        
        # 5-jet gsc single threshold triggers
        ['5j60_gsc85_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j60_gsc85_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j60_gsc90_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j60_gsc90_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85_gsc100_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85_gsc100_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j60_gsc85_boffperf_split_L14J15',         'L1_4J15', [], [PhysicsStream], ['Primary:17000', 'RATE:MultiJet',  'BW:Jet'], -1],
        ['5j60_gsc90_boffperf_split_L14J15',         'L1_4J15', [], [PhysicsStream], ['Primary:17000', 'RATE:MultiJet',  'BW:Jet'], -1],
        
        # 5-jet single threshold restricted eta range triggers
        ['5j70_0eta240_L14J15',         'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j70_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j70_0eta240',                'L1_4J20', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j75_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j75_0eta240',                'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85_0eta240',                'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j75_0eta240_L14J15',         'L1_4J15', [], [PhysicsStream], ['Primary:17000', 'RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85_0eta240_L14J15',         'L1_4J15', [], [PhysicsStream], ['Primary:17000', 'RATE:MultiJet',  'BW:Jet'], -1],
 
        # 5-jet gsc single threshold restricted eta range triggers
        ['5j50_gsc70_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j50_gsc70_boffperf_split_0eta240',               'L1_4J20', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j55_gsc75_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j55_gsc75_boffperf_split_0eta240',               'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j50_gsc70_boffperf_split_0eta240_L14J15', 'L1_4J15', [], [PhysicsStream], ['Primary:17000', 'RATE:MultiJet',  'BW:Jet'], -1],
        ['5j55_gsc75_boffperf_split_0eta240_L14J15', 'L1_4J15', [], [PhysicsStream], ['Primary:17000', 'RATE:MultiJet',  'BW:Jet'], -1],
        
        # 6-jet single threshold triggers
        ['6j70_L14J15',                 'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j70_L14J150ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j70',                        'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j85_L14J150ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j85',                        'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j85_L14J15',                 'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        
        # 6-jet gsc single threshold triggers
        ['6j50_gsc70_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_gsc70_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_gsc70_boffperf_split_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_gsc85_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_gsc85_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_gsc85_boffperf_split_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_gsc70_boffperf_split_L14J15', 'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_gsc85_boffperf_split_L14J15', 'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],

        # 6-jet single threshold restricted eta range triggers
        ['6j55_0eta240_L14J15',         'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j55_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j55_0eta240_L14J20',         'L1_4J20', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j55_0eta240_L15J150ETA25',   'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_0eta240_L14J20',         'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_0eta240_L15J150ETA25',   'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_0eta240_L14J15',              'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        
        # 6-jet gsc single threshold restricted eta range triggers
        ['6j45_gsc55_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream,'express'], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc55_boffperf_split_0eta240_L14J20',        'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc55_boffperf_split_0eta240_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc60_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc60_boffperf_split_0eta240_L14J20',        'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc60_boffperf_split_0eta240_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc55_boffperf_split_0eta240_L14J15', 'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc60_boffperf_split_0eta240_L14J15', 'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],

        # 6-jet single threshold alternative calibration triggers
        # 7-jet single-threshold triggers
        ['7j45_L14J15',                 'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j45_L14J150ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j45_L14J20',                 'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j45',                        'L1_6J15', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j50_L14J150ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j50_L14J20',                 'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j50',                        'L1_6J15', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j50_L14J15',                              'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        
        # 7-jet gsc single threshold triggers
        ['7j35_gsc45_boffperf_split_L14J150ETA25',          'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc45_boffperf_split_L14J20',                'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc45_boffperf_split_L15J150ETA25',          'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_L14J150ETA25',          'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_L14J20',                'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_L15J150ETA25',          'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc45_boffperf_split_L14J15',         'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_L14J15',         'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        
        # 7-jet single-threshold restricted eta range triggers
        ['7j45_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j45_0eta240_L14J20',         'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j45_0eta240_L15J150ETA25',   'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j45_0eta240',                'L1_6J15', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j45_0eta240_L14J15',                      'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        
        # 7-jet gsc single threshold restricted eta range triggers
        ['7j35_gsc45_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc45_boffperf_split_0eta240_L14J20',        'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc45_boffperf_split_0eta240_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_0eta240_L14J20',        'L1_4J20', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_0eta240_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc45_boffperf_split_0eta240_L14J15', 'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_0eta240_L14J15', 'L1_4J15', [], [PhysicsStream], ['Primary:17000','RATE:MultiJet',  'BW:Jet'], -1],

        # 10-jet single-threshold triggers
        ['10j40_L14J15',                'L1_4J15' , [], [PhysicsStream], ['Primary:17000','RATE:MultiJet', 'BW:Jet'], -1],
        ['10j40_L14J150ETA25',          'L1_4J15.0ETA25' , [], [PhysicsStream], ['Primary:20000','RATE:MultiJet', 'BW:Jet'], -1],
        ['10j40_L14J20',                'L1_4J20' , [], [PhysicsStream], ['Primary:20000','RATE:MultiJet', 'BW:Jet'], -1],
        ['10j40_L16J15',                'L1_6J15' , [], [PhysicsStream], ['Primary:20000','RATE:MultiJet', 'BW:Jet'], -1],

        # L1topo multi-jet triggers


        #####
        ######################### HT trigger chains
        #####

        # HT triggers
 
        # L1topo HT triggers



        #####
        ######################### Analysis-specific jet trigger chains
        #####


        # Dijet invariant mass range plus y* cut triggers, mainly aimed at the dijet trigger-level analysis

        # Analysis-driven multijet triggers
        ['2j225_gsc250_boffperf_split_0eta240_j85_gsc120_boffperf_split_0eta240',      'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],
        ['2j225_gsc250_boffperf_split_0eta240_j85_gsc140_boffperf_split_0eta240',      'L1_J100', [], [PhysicsStream], ['Primary:20000','RATE:MultiJet',  'BW:Jet'], -1],

        # ATR-14356: L1Topo VBF chains
        # ATR-15044: 2017 VBF Hbb
        ['ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF_AND_2j35_gsc45_bmv2c1070_split','L1_HT150-J20s5.ETA31_MJJ-400-CF', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,[  'ht300', '2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF_AND','2j35_gsc45_bmv2c1070_split']]],




        #####
        ######################### Specialty jet trigger chains (mostly for performance studies)
        #####

        # Jet triggers to probe the EMEC IW region
        # This region is know to be more challenging

        # Jet cleaning chains to probe NCB
        # Trigger tower (TT) jet triggers
        # See ATR-14446 for details of the request
 
        #AFP + jet chains ATR-16124
 
 
        #ALFA: ATR-18254
        #['2j10_L1TRT_ALFA_EINE', 'L1_TRT_ALFA_EINE', [], ['MinBias'], [ 'RATE:ALFA','BW:Detector'], -1], 
        #['2j10_L1TRT_ALFA_ANY_PAIRED_UNPAIRED_ISO', 'L1_TRT_ALFA_ANY_PAIRED_UNPAIRED_ISO', [], ['MinBias'], [ 'RATE:ALFA','BW:Detector'], -1], 
        #['2j10_L1ALFA_ELAS', 'L1_ALFA_ELAS', [], ['MinBias'], [ 'RATE:ALFA','BW:Detector'], -1], 
        #['2j10_L1ALFA_SYST', 'L1_ALFA_SYS', [], ['MinBias'], [ 'RATE:ALFA','BW:Detector'], -1], 
        #['2j10_L1MBTS_ALFA', 'L1_MBTS_ALFA', [], ['MinBias'], [ 'RATE:ALFA','BW:Detector'], -1], 
        #['j15_L1ALFA_Jet_Phys', 'L1_ALFA_Jet_Phys', [], ['MinBias'], [ 'RATE:ALFA','BW:Detector'], -1], 

        #Low-mu run triggers
        
        #Jets for 5TeV run ATR-17162
   

		#VBF Chains
 			 ]

    TriggerFlags.BjetSlice.signatures = [
   
 

        # boffperf_split


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

        ['2j35_bmv2c1040_split_2j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j35_bmv2c1050_split_2j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j35_bmv2c1060_split_2j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet'], -1],


        ['2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet'], -1],

        ['2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet'], -1],


        #
        #  2b / 5j
        #
        ['2j35_bmv2c1050_split_3j35', 'L1_5J15.0ETA25', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet'], -1],
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
        ['xe110_pufit_xe70_L1XE50', 'L1_XE50', ['L1_XE50','L1_XE50'], [PhysicsStream], ['Primary:20000','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x70_L1XE50'] ]],
        ['xe110_pufit_xe75_L1XE50', 'L1_XE50', ['L1_XE50','L1_XE50'], [PhysicsStream], ['Primary:20000','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x75_L1XE50'] ]],

  ##################
  ## L1_XE55 #######
  ##################

        ['xe110_pufit_xe65_L1XE55', 'L1_XE55', ['L1_XE55','L1_XE55'], [PhysicsStream], ['Primary:20000','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x65_L1XE55'] ]],
        ['xe110_pufit_xe70_L1XE55', 'L1_XE55', ['L1_XE55','L1_XE55'], [PhysicsStream], ['Primary:20000','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x70_L1XE55'] ]],
        ['xe110_pufit_xe75_L1XE55', 'L1_XE55', ['L1_XE55','L1_XE55'], [PhysicsStream], ['Primary:20000','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x75_L1XE55'] ]],


  #################
  ## L1_XE60 ######
  #################

        ['xe110_pufit_xe65_L1XE60', 'L1_XE60', ['L1_XE60','L1_XE60'], [PhysicsStream], ['Primary:20000','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x65_L1XE60'] ]],
        ['xe110_pufit_xe70_L1XE60', 'L1_XE60', ['L1_XE60','L1_XE60'], [PhysicsStream], ['Primary:20000','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x70_L1XE60'] ]],
        ['xe110_pufit_xe75_L1XE60', 'L1_XE60', ['L1_XE60','L1_XE60'], [PhysicsStream], ['Primary:20000','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x75_L1XE60'] ]],


			 ]
    if TriggerFlags.doFTK():
        TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + [
           # FTK MET chains

            ]

    
    TriggerFlags.TauSlice.signatures = [

        # chains for 2e34 backup (ATR-15225)
        ['tau160_medium1_tracktwo_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['Primary:20000','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau200_medium1_tracktwo_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['Primary:20000','RATE:SingleTau', 'BW:Tau'], -1],
        # ATR-17807
        ['tau160_medium1_tracktwoEF_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['Primary:20000','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_medium1_tracktwoMVA_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['Primary:20000','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_mediumRNN_tracktwoMVA_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['Primary:20000','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau200_medium1_tracktwoEF_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['Primary:20000','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau200_medium1_tracktwoMVA_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['Primary:20000','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau200_mediumRNN_tracktwoMVA_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['Primary:20000','RATE:SingleTau', 'BW:Tau'], -1],

        ['tau40_medium1_tracktwo_tau35_medium1_tracktwo',   'L1_TAU25IM_2TAU20IM_2J25_3J20',['L1_TAU25IM','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau40_medium1_tracktwo","tau35_medium1_tracktwo"]]],
        # ATR-17807
        ['tau40_medium1_tracktwoEF_tau35_medium1_tracktwoEF',   'L1_TAU25IM_2TAU20IM_2J25_3J20',['L1_TAU25IM','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau40_medium1_tracktwoEF","tau35_medium1_tracktwoEF"]]],
        ['tau40_medium1_tracktwoMVA_tau35_medium1_tracktwoMVA',   'L1_TAU25IM_2TAU20IM_2J25_3J20',['L1_TAU25IM','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau40_medium1_tracktwoMVA","tau35_medium1_tracktwoMVA"]]],
        ['tau40_mediumRNN_tracktwoMVA_tau35_mediumRNN_tracktwoMVA',   'L1_TAU25IM_2TAU20IM_2J25_3J20',['L1_TAU25IM','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau40_mediumRNN_tracktwoMVA","tau35_mediumRNN_tracktwoMVA"]]],


        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM_4J12.0ETA23',   'L1_TAU20IM_2TAU12IM_4J12.0ETA23',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
        # ATR-17807
        ['tau35_medium1_tracktwoEF_tau25_medium1_tracktwoEF_L1TAU20IM_2TAU12IM_4J12.0ETA23',   'L1_TAU20IM_2TAU12IM_4J12.0ETA23',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwoEF","tau25_medium1_tracktwoEF"]]],
        ['tau35_medium1_tracktwoMVA_tau25_medium1_tracktwoMVA_L1TAU20IM_2TAU12IM_4J12.0ETA23',   'L1_TAU20IM_2TAU12IM_4J12.0ETA23',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwoMVA","tau25_medium1_tracktwoMVA"]]],
        ['tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_L1TAU20IM_2TAU12IM_4J12.0ETA23',   'L1_TAU20IM_2TAU12IM_4J12.0ETA23',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_mediumRNN_tracktwoMVA","tau25_mediumRNN_tracktwoMVA"]]],

	    # l1topo ditau chains
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
        # ATR-17807
        ['tau35_medium1_tracktwoEF_tau25_medium1_tracktwoEF_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwoEF","tau25_medium1_tracktwoEF"]]],
        ['tau35_medium1_tracktwoMVA_tau25_medium1_tracktwoMVA_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwoMVA","tau25_medium1_tracktwoMVA"]]],
        ['tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_mediumRNN_tracktwoMVA","tau25_mediumRNN_tracktwoMVA"]]],
        ['tau35_tight1_tracktwo_tau25_tight1_tracktwo_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwo","tau25_tight1_tracktwo"]]],

        # tau+tau topo_start_from
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_tautsf_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]],True],

        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_03dR30_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
        # ATR-17807
        ['tau35_medium1_tracktwoEF_tau25_medium1_tracktwoEF_03dR30_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwoEF","tau25_medium1_tracktwoEF"]]],
        ['tau35_medium1_tracktwoMVA_tau25_medium1_tracktwoMVA_03dR30_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwoMVA","tau25_medium1_tracktwoMVA"]]],
        ['tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_03dR30_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_mediumRNN_tracktwoMVA","tau25_mediumRNN_tracktwoMVA"]]],

        # chains for 2e34 backup (ATR-15225)
        ['tau35_tight1_tracktwo_tau25_tight1_tracktwo_03dR30_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwo","tau25_tight1_tracktwo"]]],


        ['tau80_medium1_tracktwo_L1TAU60_tau35_medium1_tracktwo_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I',   'L1_TAU60_DR-TAU20ITAU12I',['L1_TAU60','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_medium1_tracktwo_L1TAU60","tau35_medium1_tracktwo_L1TAU12IM"]]]	,
        # non-L1topo backup (ATR-15757)
        ['tau80_medium1_tracktwo_L1TAU60_tau60_medium1_tracktwo_L1TAU40',   'L1_TAU60_2TAU40',['L1_TAU60','L1_TAU40'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_medium1_tracktwo_L1TAU60","tau60_medium1_tracktwo_L1TAU40"]]]       ,
        # ATR-17807
        ['tau80_medium1_tracktwoEF_L1TAU60_tau35_medium1_tracktwoEF_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I',   'L1_TAU60_DR-TAU20ITAU12I',['L1_TAU60','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_medium1_tracktwoEF_L1TAU60","tau35_medium1_tracktwoEF_L1TAU12IM"]]]	,
        ['tau80_medium1_tracktwoMVA_L1TAU60_tau35_medium1_tracktwoMVA_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I',   'L1_TAU60_DR-TAU20ITAU12I',['L1_TAU60','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_medium1_tracktwoMVA_L1TAU60","tau35_medium1_tracktwoMVA_L1TAU12IM"]]]	,
        ['tau80_mediumRNN_tracktwoMVA_L1TAU60_tau35_mediumRNN_tracktwoMVA_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I',   'L1_TAU60_DR-TAU20ITAU12I',['L1_TAU60','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_mediumRNN_tracktwoMVA_L1TAU60","tau35_mediumRNN_tracktwoMVA_L1TAU12IM"]]]	,
        ['tau80_medium1_tracktwoEF_L1TAU60_tau60_medium1_tracktwoEF_L1TAU40',   'L1_TAU60_2TAU40',['L1_TAU60','L1_TAU40'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_medium1_tracktwoEF_L1TAU60","tau60_medium1_tracktwoEF_L1TAU40"]]]       ,
        ['tau80_medium1_tracktwoMVA_L1TAU60_tau60_medium1_tracktwoMVA_L1TAU40',   'L1_TAU60_2TAU40',['L1_TAU60','L1_TAU40'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_medium1_tracktwoMVA_L1TAU60","tau60_medium1_tracktwoMVA_L1TAU40"]]]       ,
        ['tau80_mediumRNN_tracktwoMVA_L1TAU60_tau60_mediumRNN_tracktwoMVA_L1TAU40',   'L1_TAU60_2TAU40',['L1_TAU60','L1_TAU40'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_mediumRNN_tracktwoMVA_L1TAU60","tau60_mediumRNN_tracktwoMVA_L1TAU40"]]]       ,

        # ATR-17807 di-tau
	  #tracktwoEF
        ['tau35_tight1_tracktwoEF_tau25_tight1_tracktwoEF_03dR30_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwoEF","tau25_tight1_tracktwoEF"]]],
        ['tau35_tight1_tracktwoEF_tau25_tight1_tracktwoEF_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwoEF","tau25_tight1_tracktwoEF"]]],
	  #tracktwoMVA
        ['tau35_tight1_tracktwoMVA_tau25_tight1_tracktwoMVA_03dR30_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwoMVA","tau25_tight1_tracktwoMVA"]]],
        ['tau35_tight1_tracktwoMVA_tau25_tight1_tracktwoMVA_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwoMVA","tau25_tight1_tracktwoMVA"]]],
        #RNN
        ['tau35_tightRNN_tracktwoMVA_tau25_tightRNN_tracktwoMVA_03dR30_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tightRNN_tracktwoMVA","tau25_tightRNN_tracktwoMVA"]]],
        ['tau35_tightRNN_tracktwoMVA_tau25_tightRNN_tracktwoMVA_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tightRNN_tracktwoMVA","tau25_tightRNN_tracktwoMVA"]]],
        ['tau80_tightRNN_tracktwoMVA_L1TAU60_tau60_tightRNN_tracktwoMVA_L1TAU40',   'L1_TAU60_2TAU40',['L1_TAU60','L1_TAU40'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_tightRNN_tracktwoMVA_L1TAU60","tau60_tightRNN_tracktwoMVA_L1TAU40"]]],
        ['tau80_tightRNN_tracktwoMVA_L1TAU60_tau35_tightRNN_tracktwoMVA_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I',   'L1_TAU60_DR-TAU20ITAU12I',['L1_TAU60','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_tightRNN_tracktwoMVA_L1TAU60","tau35_tightRNN_tracktwoMVA_L1TAU12IM"]]],
        ['tau40_tightRNN_tracktwoMVA_tau35_tightRNN_tracktwoMVA',   'L1_TAU25IM_2TAU20IM_2J25_3J20',['L1_TAU20IM', 'L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau40_tightRNN_tracktwoMVA","tau35_tightRNN_tracktwoMVA"]]],
        
			 ]

    if TriggerFlags.doFTK():
            TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [


                           ]

    TriggerFlags.EgammaSlice.signatures = [

        ['g300_etcut',                    'L1_EM22VHI', [], [PhysicsStream], ['Primary:17000','RATE:SinglePhoton', 'BW:Egamma'],-1], 
        ['g300_etcut_L1EM24VHI',          'L1_EM24VHI', [], [PhysicsStream], ['Primary:20000','RATE:SinglePhoton', 'BW:Egamma'],-1], 
        ['g300_etcut_L1EM24VHIM',          'L1_EM24VHIM', [], [PhysicsStream], ['Primary:20000','RATE:SinglePhoton', 'BW:Egamma'],-1],

        ## Aranxtas request, to be run in rerun mode
        ['g0_hiptrt_L1EM22VHI',                 'L1_EM22VHI', [], [PhysicsStream], ['Primary:17000','RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g0_hiptrt_L1EM24VHI',                 'L1_EM24VHI', [], [PhysicsStream], ['Primary:20000','RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g0_hiptrt_L1EM24VHIM',                'L1_EM24VHIM', [], [PhysicsStream], ['Primary:20000','RATE:SinglePhoton', 'BW:Egamma'], -1],

	# ATR-16089
        ['2g20_tight_icalovloose_L12EM15VHI',           'L1_2EM15VHI', [], [PhysicsStream, 'express'], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g20_tight_icalotight_L12EM15VHI',            'L1_2EM15VHI', [], [PhysicsStream], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g22_tight_L12EM15VHI',                   'L1_2EM15VHI', [], [PhysicsStream], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g22_tight_icalovloose_L12EM15VHI',           'L1_2EM15VHI', [], [PhysicsStream], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1],

        ['2g22_tight_icalotight_L12EM15VHI',            'L1_2EM15VHI', [], [PhysicsStream], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g50_loose_L12EM20VH',                    'L1_2EM20VH', [], [PhysicsStream], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g60_loose_L12EM20VH',                    'L1_2EM20VH', [], [PhysicsStream], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ##Adding tight diphoton triggers (ATR-10762)
        ['2g25_tight_L12EM20VH',                    'L1_2EM20VH', [], [PhysicsStream], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g25_loose_g15_loose',                    'L1_2EM20VH', [], [PhysicsStream], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g25_loose_g20_loose',                    'L1_2EM20VH', [], [PhysicsStream], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['3g25_loose',                    'L1_2EM20VH', [], [PhysicsStream], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1],

        #ATR-16981

        ['e26_lhtight_nod0_e15_etcut_L1EM7_Zee',    	  'L1_EM22VHI', ['L1_EM22VHI','L1_EM7'], [PhysicsStream], ['Primary:17000','RATE:MultiElectron', 'BW:Egamma'],-1 ],
        ['e28_lhtight_nod0_e15_etcut_L1EM7_Zee',    	  'L1_EM24VHI', ['L1_EM24VHI','L1_EM7'], [PhysicsStream, 'express'], ['Primary:20000','RATE:MultiElectron', 'BW:Egamma'],-1 ],
        ['e28_lhtight_nod0_L1EM24VHIM_e15_etcut_L1EM7_Zee', 'L1_EM24VHIM',  ['L1_EM24VHIM','L1_EM7'], [PhysicsStream], ['Primary:20000','RATE:MultiElectron', 'BW:Egamma'],-1],


        # L1Topo JPSI prescaled performance:

        # extra id perf chains for TRT HT studies                                                                                               


        ['e60_lhmedium_nod0', 'L1_EM22VHI', [], [PhysicsStream], ['Primary:17000','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g140_loose',        'L1_EM24VHI', [], [PhysicsStream], ['Primary:17000','RATE:SinglePhoton','BW:Egamma'],-1],
        ['g140_tight',        'L1_EM24VHI', [], [PhysicsStream], ['Primary:20000','RATE:SinglePhoton','BW:Egamma'],-1],
        ['g140_tight_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Primary:20000','RATE:SinglePhoton','BW:Egamma'],-1],
        ['g160_loose',        'L1_EM24VHI', [], [PhysicsStream], ['Primary:17000','RATE:SinglePhoton','BW:Egamma'],-1],
        ['g180_loose',        'L1_EM24VHI', [], [PhysicsStream], ['Primary:17000','RATE:SinglePhoton','BW:Egamma'],-1],
        ['g200_loose',        'L1_EM24VHI', [], [PhysicsStream, 'express'], ['Primary:20000','RATE:SinglePhoton','BW:Egamma'],-1],
        ['g200_loose_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Primary:20000','RATE:SinglePhoton','BW:Egamma'],-1],
        ['e140_lhloose_nod0', 'L1_EM22VHI', [], [PhysicsStream], ['Primary:17000','RATE:SingleElectron', 'BW:Egamma'],-1],

        #2016 egamma primaries
        ['e80_lhmedium_nod0',              'L1_EM22VHI',   [], [PhysicsStream], ['Primary:17000','RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e80_lhmedium_nod0_L1EM24VHI',    'L1_EM24VHI',   [], [PhysicsStream], ['Primary:20000','RATE:SingleElectron', 'BW:Egamma'],-1 ],

        #Low-mu run triggers
# Start up menu

        ['2e17_lhvloose_nod0_L12EM15VHI',         'L1_2EM15VHI', [], [PhysicsStream], ['Primary:20000','RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e19_lhvloose_nod0',         'L1_2EM15VHI', [], [PhysicsStream], ['Primary:20000','RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e24_lhvloose_nod0',                   'L1_2EM20VH', [], [PhysicsStream, 'express'], ['Primary:20000','RATE:MultiElectron', 'BW:Egamma'],-1],

        ['e24_lhloose_nod0_2e12_lhloose_nod0_L1EM20VH_3EM10VH',   'L1_EM20VH_3EM10VH', ['L1_EM20VH','L1_3EM10VH'], [PhysicsStream], ['Primary:20000','RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e24_lhvloose_nod0_2e12_lhvloose_nod0_L1EM20VH_3EM10VH', 'L1_EM20VH_3EM10VH', ['L1_EM20VH','L1_3EM10VH'], [PhysicsStream], ['Primary:20000','RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_2g12_loose',                          'L1_EM20VH_3EM10VH', ['L1_EM20VH','L1_3EM10VH'], [PhysicsStream], ['Primary:20000','RATE:ElectronPhoton', 'BW:Egamma'],-1,['parallel',-1,[] ]],
        ['e24_lhmedium_nod0_2g12_medium',                         'L1_EM20VH_3EM10VH', ['L1_EM20VH','L1_3EM10VH'], [PhysicsStream], ['Primary:20000','RATE:ElectronPhoton', 'BW:Egamma'],-1,['parallel',-1,[] ]],


#Physics (e+g):
        ['e24_lhmedium_nod0_L1EM20VH_g25_medium',  'L1_2EM20VH', [], [PhysicsStream], ['Primary:20000','RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 

        #Diphoton triggers
        ['g35_medium_g25_medium_L12EM20VH',          'L1_2EM20VH', [], [PhysicsStream, 'express'], ['Primary:20000','RATE:MultiPhoton', 'BW:Egamma'],-1],

        # LLP
        ['e26_lhtight_nod0',                  'L1_EM22VHI', [], [PhysicsStream], ['Primary:17000','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhtight_nod0_ivarloose',        'L1_EM22VHI', [], [PhysicsStream], ['Primary:17000','RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e28_lhtight_nod0',                  'L1_EM24VHI', [], [PhysicsStream], ['Primary:17000','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_nod0_ivarloose',        'L1_EM24VHI', [], [PhysicsStream,'express'], ['Primary:20000','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_nod0_ivarloose_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Primary:20000','RATE:SingleElectron', 'BW:Egamma'],-1],


        #Supporting
        
        ['e28_lhtight_nod0_ivarloose_L1EM22VHI',  'L1_EM22VHI', [], [PhysicsStream], ['Primary:17000','RATE:SingleElectron', 'BW:Egamma'],-1],

        # Et cut only chains
        ['e300_etcut',                           'L1_EM24VHI',  [], [PhysicsStream], ['Primary:20000','RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e300_etcut_L1EM24VHIM',                 'L1_EM24VHIM',  [], [PhysicsStream], ['Primary:20000','RATE:SingleElectron', 'BW:Egamma'],-1], 

        # 2e34 single items
        ['e60_lhmedium_nod0_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['Primary:20000','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e60_lhmedium_nod0_L1EM24VHIM', 'L1_EM24VHIM', [], [PhysicsStream], ['Primary:20000','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e140_lhloose_nod0_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['Primary:20000','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e140_lhloose_nod0_L1EM24VHIM', 'L1_EM24VHIM', [], [PhysicsStream], ['Primary:20000','RATE:SingleElectron', 'BW:Egamma'],-1],
				
        
        #Chains for testing 2e34 menus
        ['e32_lhtight_nod0_ivarloose',        'L1_EM24VHI', [], [PhysicsStream], ['Primary:20000','RATE:SingleElectron', 'BW:Egamma'],-1],  
        

        #ATR-16089 supporting triggers for MC16c

        ['e80_lhmedium_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Primary:20000','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g140_loose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['Primary:17000','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g160_loose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['Primary:17000','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g180_loose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['Primary:17000','RATE:SinglePhoton', 'BW:Egamma'],-1],

        	 ]

    TriggerFlags.BphysicsSlice.signatures = [
        ### ATR-14350: L1Topo bphysics chains
        # 96% bckg rejection WP


			 ]

    TriggerFlags.CombinedSlice.signatures = [
        ['j45_gsc55_bmv2c1050_split_ht700_L1HT190-J15s5.ETA21',  'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['Primary:20000','RATE:SingleBJet', 'BW:BJet_combined'], -1,['serial',-1,['ht700','j45_gsc55_bmv2c1050_split']]],

        ['2j45_gsc55_bmv2c1050_split_ht300_L1HT190-J15s5.ETA21', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet_combined'], -1,['serial',-1,[ 'ht300', '2j45_gsc55_bmv2c1050_split' ]]], 


        # b-jet+MET ATR-17678
        ['2j35_gsc45_bmv2c1070_split_xe85_pufit_xe50_L12J15_XE55',"L1_2J15_XE55",[],[PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet_combined', 'BW:MET'], -1, ['serial',-1,['xe85_pufit','xe50_L12J15_XE55','2j35_gsc45_bmv2c1070_split']]],
        ['3j35_bmv2c1077_split_xe70_pufit_xe50_L13J15.0ETA25_XE40',"L1_3J15.0ETA25_XE40",[],[PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet_combined', 'BW:MET'], -1, ['serial',-1,['xe70_pufit','xe50_L13J15.0ETA25_XE40','3j35_bmv2c1077_split']]],
        ['j85_gsc100_bmv2c1050_split_xe85_pufit_xe50_L1XE55',"L1_XE55",[],[PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet_combined', 'BW:MET'], -1, ['serial',-1,['xe85_pufit','xe50_L1XE55','j85_gsc100_bmv2c1050_split']]],



        ['2e12_lhloose_nod0_mu10',  'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],


        ['2e12_lhmedium_nod0_mu10', 'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],


        ['e12_lhloose_nod0_2mu10',  'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],


        ['e12_lhmedium_nod0_2mu10', 'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],


        ['e17_lhloose_nod0_mu14',   'L1_EM15VH_MU10', ['L1_EM15VH', 'L1_MU10'], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e26_lhmedium_nod0_mu8noL1', 'L1_EM22VHI', ['L1_EM22VHI', ''], [PhysicsStream], ['Primary:17000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['e26_lhmedium_nod0','mu8noL1'] ]],
        ['e28_lhmedium_nod0_mu8noL1', 'L1_EM24VHI', ['L1_EM24VHI', ''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['e28_lhmedium_nod0','mu8noL1'] ]],
        ['e28_lhmedium_nod0_L1EM24VHIM_mu8noL1', 'L1_EM24VHIM', ['L1_EM24VHIM', ''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['e28_lhmedium_nod0_L1EM24VHIM','mu8noL1'] ]],
        ['e70_lhloose_nod0_xe70noL1',    'L1_EM22VHI',['L1_EM22VHI',''], [PhysicsStream], ['Primary:17000','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e70_lhloose_nod0","xe70noL1"]]],
        ['e70_lhloose_nod0_L1EM24VHIM_xe70noL1',    'L1_EM24VHIM',['L1_EM24VHIM',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e70_lhloose_nod0_L1EM24VHIM","xe70noL1"]]],
        ['e80_lhloose_nod0_xe80noL1',    'L1_EM22VHI',['L1_EM22VHI',''], [PhysicsStream], ['Primary:17000','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e80_lhloose_nod0","xe80noL1"]]],
        ['e80_lhloose_nod0_L1EM24VHI_xe80noL1',    'L1_EM24VHI',['L1_EM24VHI',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e80_lhloose_nod0_L1EM24VHI","xe80noL1"]]],
        ['e80_lhloose_nod0_L1EM24VHIM_xe80noL1',    'L1_EM24VHIM',['L1_EM24VHIM',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e80_lhloose_nod0_L1EM24VHIM","xe80noL1"]]],

        ['e7_lhmedium_nod0_mu24',        'L1_MU20MU21', ['L1_EM3', 'L1_MU20'], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['g80_loose_L1EM24VHI_xe80noL1',           'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['Primary:17000','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g80_loose_L1EM24VHI","xe80noL1"]]],
        ['g80_loose_L1EM24VHIM_xe80noL1',           'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['Primary:17000','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g80_loose_L1EM24VHIM","xe80noL1"]]],
        ['g80_loose_icalovloose_L1EM24VHIM_xe80noL1',           'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g80_loose_icalovloose_L1EM24VHIM","xe80noL1"]]],
        ['g80_loose_icalovloose_xe80noL1',           'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['Primary:17000','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g80_loose_icalovloose","xe80noL1"]]],
        ['g80_loose_icalovloose_L1EM24VHI_xe80noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g80_loose_icalovloose_L1EM24VHI","xe80noL1"]]],
        ['g85_tight_icalovloose_L1EM22VHI_3j50noL1',  'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['Primary:17000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g85_tight_icalovloose_L1EM22VHI","3j50noL1"]]],
        ['g100_tight_icalovloose_L1EM22VHI_3j50noL1', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['Primary:17000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g100_tight_icalovloose_L1EM22VHI","3j50noL1"]]],
        ['g85_tight_icalovloose_L1EM24VHI_3j50noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g85_tight_icalovloose_L1EM24VHI","3j50noL1"]]],
        ['g100_tight_icalovloose_L1EM24VHI_3j50noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g100_tight_icalovloose_L1EM24VHI","3j50noL1"]]],

        #ATR-14196
        ['g80_loose_xe80noL1',           'L1_EM22VHI',  ['L1_EM22VHI',''], [PhysicsStream], ['Primary:17000','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g80_loose","xe80noL1"]]],

        ['g25_medium_mu24',              'L1_MU20MU21', ['L1_EM15VH','L1_MU20'], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],

        ['g15_loose_2mu10_msonly',       'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],


        #ATR-15243 Higgsino
        #di-muon primary
        ['2mu4_invm1_j20_xe40_pufit_2dphi10_L12MU4_J20_XE30_DPHI-J20s2XE30',  'L1_2MU4_J20_XE30_DPHI-J20s2XE30', [], [PhysicsStream], ['Primary:20000','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["2mu4_invm1","j20","xe40_pufit"]]], 

        #di-muon backup
        ['2mu4_invm1_j20_xe60_pufit_2dphi10_L12MU4_J20_XE30_DPHI-J20s2XE30',  'L1_2MU4_J20_XE30_DPHI-J20s2XE30', [], [PhysicsStream], ['Primary:20000','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["2mu4_invm1","j20","xe60_pufit"] ]],
        ['2mu4_invm1_j20_xe60_pufit_2dphi10_L12MU4_J40_XE50',  'L1_2MU4_J40_XE50', ['L1_2MU4','L1_J40','L1_XE50'], [PhysicsStream], ['Primary:20000','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["2mu4_invm1","j20","xe60_pufit"] ]],

        ['mu4_j90_xe90_pufit_2dphi10_L1MU4_J50_XE50_DPHI-J20s2XE30',  'L1_MU4_J50_XE50_DPHI-J20s2XE30', [], [PhysicsStream], ['Primary:20000','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["mu4","j90","xe90_pufit"] ]],
        ['mu4_j90_xe90_pufit_2dphi10_L1MU4_XE60',  'L1_MU4_XE60', [], [PhysicsStream], ['Primary:20000','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["mu4","j90","xe90_pufit"] ]],


  

        # mu-e
        ['e5_lhvloose_nod0_mu4_j30_xe40_pufit_2dphi10_L1MU4_J30_XE40_DPHI-J20s2XE30', 'L1_MU4_J30_XE40_DPHI-J20s2XE30', ['L1_EM3','','',''], [PhysicsStream], ['Primary:20000','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1,['serial',-1,["e5_lhvloose_nod0","mu4","j30","xe40_pufit"]]],

        #mu-e backup
        ['e5_lhloose_nod0_mu4_j30_xe40_pufit_2dphi10_L1MU4_J30_XE40_DPHI-J20s2XE30',  'L1_MU4_J30_XE40_DPHI-J20s2XE30', ['L1_EM3','','',''], [PhysicsStream], ['Primary:20000','RATE:MuonJetMET', 'BW:Egamma', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["e5_lhloose_nod0","mu4","j30","xe40_pufit"] ]],
        ['e5_lhmedium_nod0_mu4_j30_xe65_pufit_2dphi10_L1MU4_XE60',  'L1_MU4_XE60', ['L1_EM3','L1_MU4','','L1_XE60'], [PhysicsStream], ['Primary:20000','RATE:MuonJetMET', 'BW:Egamma', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["e5_lhmedium_nod0","mu4","j30","xe65_pufit"] ]], 


        #di-electron
        ['2e5_lhvloose_nod0_j40_xe70_pufit_2dphi10_L1J40_XE50_DPHI-J20s2XE50',  'L1_J40_XE50_DPHI-J20s2XE50', ['L1_EM3','',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJetMET', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["2e5_lhvloose_nod0","j40","xe70_pufit"] ]],

        #di-electron backup
        ['2e5_lhvloose_nod0_j40_xe70_pufit_2dphi10_L1XE60',  'L1_XE60', ['L1_EM3','',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJetMET', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["2e5_lhvloose_nod0","j40","xe70_pufit_L1XE60"] ]],

        #single electron backup 
        ['e5_lhloose_nod0_j50_xe70_pufit_2dphi10_L1J40_XE50_DPHI-J20s2XE50',  'L1_J40_XE50_DPHI-J20s2XE50', ['L1_EM3','',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJetMET', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e5_lhloose_nod0","j50","xe70_pufit"] ]],
        ['e5_lhmedium_nod0_j50_xe80_pufit_2dphi10_L1J40_XE50_DPHI-J20s2XE50',  'L1_J40_XE50_DPHI-J20s2XE50', ['L1_EM3','',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJetMET', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e5_lhmedium_nod0","j50","xe80_pufit"] ]],
        ['e5_lhloose_nod0_j40_xe70_pufit_2dphi10_L1XE60',  'L1_XE60', ['L1_EM3','',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJetMET', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e5_lhloose_nod0","j40","xe70_pufit_L1XE60"] ]],


        #ATR-16089 combined egamma support chains
        ['e60_etcut_trkcut_L1EM24VHIM_xs30_j15_perf_xe30_6dphi15_mt35', 'L1_EM24VHIM', ['L1_EM24VHIM','','',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e60_etcut_trkcut_L1EM24VHIM","xs30","j15_perf","xe30"]]],


        #
        # BJet signal Perf chains
        #
        # (Critical for BTagging SF Calculation with ttbar !) 

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
        ['2g10_medium_mu20',         'L1_MU20MU21',    ['L1_2EM7', 'L1_MU20'],  [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['2g10_medium','mu20'] ]],

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

        
        ### ATR-14543
        ['g35_tight_icalotight_L1EM24VHIM_mu18noL1',        'L1_EM24VHIM', ['L1_EM24VHIM', ''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_tight_icalotight_L1EM24VHIM','mu18noL1'] ]],
        ['g40_tight_icalotight_L1EM24VHIM_mu18noL1',        'L1_EM24VHIM', ['L1_EM24VHIM', ''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g40_tight_icalotight_L1EM24VHIM','mu18noL1'] ]],
        ['g35_tight_icalotight_L1EM24VHIM_mu15noL1_mu2noL1', 'L1_EM24VHIM', ['L1_EM24VHIM', '',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_tight_icalotight_L1EM24VHIM','mu15noL1','mu2noL1']]],
        ['g40_tight_icalotight_L1EM24VHIM_mu15noL1_mu2noL1', 'L1_EM24VHIM', ['L1_EM24VHIM', '',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g40_tight_icalotight_L1EM24VHIM','mu15noL1','mu2noL1']]],
 
        #ATR-11623 photon + muon/di-muon chains L1_EM24VHI and L1_EM24VHIM seed
        ['g35_tight_icalotight_L1EM24VHI_mu18noL1',        'L1_EM24VHI', ['L1_EM24VHI', ''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_tight_icalotight_L1EM24VHI','mu18noL1'] ]],
        ['g35_tight_icalotight_L1EM24VHI_mu15noL1_mu2noL1', 'L1_EM24VHI', ['L1_EM24VHI', '',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_tight_icalotight_L1EM24VHI','mu15noL1','mu2noL1']]],

        ['g40_tight_icalotight_L1EM24VHI_mu18noL1',        'L1_EM24VHI', ['L1_EM24VHI', ''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g40_tight_icalotight_L1EM24VHI','mu18noL1'] ]],
        ['g40_tight_icalotight_L1EM24VHI_mu15noL1_mu2noL1', 'L1_EM24VHI', ['L1_EM24VHI', '',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g40_tight_icalotight_L1EM24VHI','mu15noL1','mu2noL1']]],

        ['g35_loose_L1EM24VHI_mu18',        'L1_EM24VHI', ['L1_EM24VHI', 'L1_MU11'], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_loose_L1EM24VHI','mu18'] ]],
        ['g35_loose_L1EM24VHI_mu15_mu2noL1', 'L1_EM24VHI', ['L1_EM24VHI', 'L1_MU11',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_loose_L1EM24VHI','mu15','mu2noL1']]],

        ['g35_loose_L1EM24VHIM_mu18',        'L1_EM24VHIM', ['L1_EM24VHIM', 'L1_MU11'], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_loose_L1EM24VHIM','mu18'] ]],
        ['g35_loose_L1EM24VHIM_mu15_mu2noL1', 'L1_EM24VHIM', ['L1_EM24VHIM', 'L1_MU11',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_loose_L1EM24VHIM','mu15','mu2noL1']]],

        # Heg chains
        ['e25_mergedtight_g35_medium_Heg',       'L1_2EM20VH', [], [PhysicsStream], ['Primary:20000','RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ['e30_mergedtight_g35_medium_Heg',       'L1_2EM20VH', [], [PhysicsStream], ['Primary:20000','RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],

        # ATR-16089 Heg chains
        ['e25_mergedtight_ivarloose_g35_medium_icalovloose_Heg',       'L1_2EM20VH', [], [PhysicsStream], ['Primary:20000','RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ['e25_mergedtight_ivarloose_g35_medium_icalotight_Heg',       'L1_2EM20VH', [], [PhysicsStream], ['Primary:20000','RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
 
        ['e30_mergedtight_ivarloose_g35_medium_icalotight_Heg',       'L1_2EM20VH', [], [PhysicsStream], ['Primary:20000','RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
	# new chain for 2e34 backup (ATR-15225)
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo',  'L1_EM15VHI_2TAU12IM_J25_3J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream],['Primary:17000','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwo"]]],

        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1EM15VHI_2TAU12IM_4J12',   'L1_EM15VHI_2TAU12IM_4J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream],['Primary:20000','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwo"]]],

	#L1Topo e+tau chains
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25', 'L1_DR-EM15TAU12I-J25',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:ElectronTau', 'BW:Tau'],-1,['serial',-1 ,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwo"]]],
        ['mu14_ivarloose_tau35_medium1_tracktwo',  'L1_MU10_TAU20IM',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['Primary:17000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_medium1_tracktwo"]]],
        ['mu14_ivarloose_L1MU11_tau35_medium1_tracktwo_L1MU11_TAU20IM',  'L1_MU11_TAU20IM',['L1_MU11','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose_L1MU11","tau35_medium1_tracktwo"]]],
        ['mu14_ivarloose_tau35_medium1_tracktwo_L1MU10_TAU20IM_J25_2J20',      'L1_MU10_TAU20IM_J25_2J20',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_medium1_tracktwo"]]],   

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
        ['tau60_medium1_tracktwoMVA_tau25_medium1_tracktwoMVA_xe50', 'L1_TAU40_2TAU12IM_XE40',['L1_TAU40','L1_TAU12IM','L1_XE40'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau60_medium1_tracktwoMVA","tau25_medium1_tracktwoMVA","xe50"]]],
        ['tau60_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_xe50', 'L1_TAU40_2TAU12IM_XE40',['L1_TAU40','L1_TAU12IM','L1_XE40'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau60_mediumRNN_tracktwoMVA","tau25_mediumRNN_tracktwoMVA","xe50"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwoEF_xe50', 'L1_EM15VHI_2TAU12IM_XE35',['L1_EM15VHI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau', 'BW:Egamma'],-1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwoEF", "xe50"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwoMVA_xe50', 'L1_EM15VHI_2TAU12IM_XE35',['L1_EM15VHI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau', 'BW:Egamma'],-1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwoMVA", "xe50"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_mediumRNN_tracktwoMVA_xe50', 'L1_EM15VHI_2TAU12IM_XE35',['L1_EM15VHI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau', 'BW:Egamma'],-1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_mediumRNN_tracktwoMVA", "xe50"]]],
        ['e17_lhmedium_nod0_tau25_medium1_tracktwoEF_xe50', 'L1_EM15VHI_2TAU12IM_XE35',['L1_EM15VHI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_medium1_tracktwoEF", "xe50"]]],
        ['e17_lhmedium_nod0_tau25_medium1_tracktwoMVA_xe50', 'L1_EM15VHI_2TAU12IM_XE35',['L1_EM15VHI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_medium1_tracktwoMVA", "xe50"]]],
        ['e17_lhmedium_nod0_tau25_mediumRNN_tracktwoMVA_xe50', 'L1_EM15VHI_2TAU12IM_XE35',['L1_EM15VHI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_mediumRNN_tracktwoMVA", "xe50"]]],
        ['mu14_ivarloose_tau25_medium1_tracktwoEF',      'L1_MU10_TAU12IM_J25_2J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Primary:17000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwoEF"]]],        
        ['mu14_ivarloose_tau25_medium1_tracktwoMVA',      'L1_MU10_TAU12IM_J25_2J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Primary:17000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwoMVA"]]],        
        ['mu14_ivarloose_tau25_mediumRNN_tracktwoMVA',      'L1_MU10_TAU12IM_J25_2J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Primary:17000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],        
        ['mu14_ivarloose_tau25_medium1_tracktwoEF_L1DR-MU10TAU12I_TAU12I-J25', 'L1_DR-MU10TAU12I_TAU12I-J25',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwoEF"]]],
        ['mu14_ivarloose_tau25_medium1_tracktwoMVA_L1DR-MU10TAU12I_TAU12I-J25', 'L1_DR-MU10TAU12I_TAU12I-J25',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwoMVA"]]],
        ['mu14_ivarloose_tau25_mediumRNN_tracktwoMVA_L1DR-MU10TAU12I_TAU12I-J25', 'L1_DR-MU10TAU12I_TAU12I-J25',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwoEF_L1DR-EM15TAU12I-J25', 'L1_DR-EM15TAU12I-J25',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:ElectronTau', 'BW:Tau'],-1,['serial',-1 ,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwoEF"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwoMVA_L1DR-EM15TAU12I-J25', 'L1_DR-EM15TAU12I-J25',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:ElectronTau', 'BW:Tau'],-1,['serial',-1 ,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwoMVA"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_mediumRNN_tracktwoMVA_L1DR-EM15TAU12I-J25', 'L1_DR-EM15TAU12I-J25',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:ElectronTau', 'BW:Tau'],-1,['serial',-1 ,["e17_lhmedium_nod0_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwoEF',  'L1_EM15VHI_2TAU12IM_J25_3J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream],['Primary:17000','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwoEF"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwoMVA',  'L1_EM15VHI_2TAU12IM_J25_3J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream],['Primary:17000','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwoMVA"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_mediumRNN_tracktwoMVA',  'L1_EM15VHI_2TAU12IM_J25_3J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream],['Primary:17000','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],
        ['e24_lhmedium_nod0_ivarloose_tau35_medium1_tracktwoEF',  'L1_EM20VHI_TAU20IM_2TAU20_J25_3J20',['L1_EM20VHI','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e24_lhmedium_nod0_ivarloose","tau35_medium1_tracktwoEF"]]],
        ['e24_lhmedium_nod0_ivarloose_tau35_medium1_tracktwoMVA',  'L1_EM20VHI_TAU20IM_2TAU20_J25_3J20',['L1_EM20VHI','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e24_lhmedium_nod0_ivarloose","tau35_medium1_tracktwoMVA"]]],
        ['e24_lhmedium_nod0_ivarloose_tau35_mediumRNN_tracktwoMVA',  'L1_EM20VHI_TAU20IM_2TAU20_J25_3J20',['L1_EM20VHI','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e24_lhmedium_nod0_ivarloose","tau35_mediumRNN_tracktwoMVA"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwoEF_L1EM15VHI_2TAU12IM_4J12',   'L1_EM15VHI_2TAU12IM_4J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream],['Primary:20000','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwoEF"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwoMVA_L1EM15VHI_2TAU12IM_4J12',   'L1_EM15VHI_2TAU12IM_4J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream],['Primary:20000','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwoMVA"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_mediumRNN_tracktwoMVA_L1EM15VHI_2TAU12IM_4J12',   'L1_EM15VHI_2TAU12IM_4J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream],['Primary:20000','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],
        ['mu14_ivarloose_tau35_medium1_tracktwoEF_L1MU10_TAU20IM_J25_2J20',      'L1_MU10_TAU20IM_J25_2J20',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_medium1_tracktwoEF"]]],   
        ['mu14_ivarloose_tau35_medium1_tracktwoMVA_L1MU10_TAU20IM_J25_2J20',      'L1_MU10_TAU20IM_J25_2J20',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_medium1_tracktwoMVA"]]],   
        ['mu14_ivarloose_tau35_mediumRNN_tracktwoMVA_L1MU10_TAU20IM_J25_2J20',      'L1_MU10_TAU20IM_J25_2J20',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_mediumRNN_tracktwoMVA"]]],   
        ['mu14_ivarloose_tau25_medium1_tracktwoEF_L1MU10_TAU12IM_3J12',      'L1_MU10_TAU12IM_3J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwoEF"]]],
        ['mu14_ivarloose_tau25_medium1_tracktwoMVA_L1MU10_TAU12IM_3J12',      'L1_MU10_TAU12IM_3J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwoMVA"]]],
        ['mu14_ivarloose_tau25_mediumRNN_tracktwoMVA_L1MU10_TAU12IM_3J12',      'L1_MU10_TAU12IM_3J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],
        ['mu14_ivarloose_tau35_medium1_tracktwoEF',  'L1_MU10_TAU20IM',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['Primary:17000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_medium1_tracktwoEF"]]],
        ['mu14_ivarloose_tau35_medium1_tracktwoMVA',  'L1_MU10_TAU20IM',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['Primary:17000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_medium1_tracktwoMVA"]]],
        ['mu14_ivarloose_tau35_mediumRNN_tracktwoMVA',  'L1_MU10_TAU20IM',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['Primary:17000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_mediumRNN_tracktwoMVA"]]],
        ['mu14_ivarloose_L1MU11_tau35_medium1_tracktwoEF_L1MU11_TAU20IM',  'L1_MU11_TAU20IM',['L1_MU11','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose_L1MU11","tau35_medium1_tracktwoEF"]]],
        ['mu14_ivarloose_L1MU11_tau35_medium1_tracktwoMVA_L1MU11_TAU20IM',  'L1_MU11_TAU20IM',['L1_MU11','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose_L1MU11","tau35_medium1_tracktwoMVA"]]],
        ['mu14_ivarloose_L1MU11_tau35_mediumRNN_tracktwoMVA_L1MU11_TAU20IM',  'L1_MU11_TAU20IM',['L1_MU11','L1_TAU20IM'], [PhysicsStream], ['Primary:20000','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose_L1MU11","tau35_mediumRNN_tracktwoMVA"]]],
        ['mu14_tau25_medium1_tracktwoEF_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14", "tau25_medium1_tracktwoEF","xe50"]]],
        ['mu14_tau25_medium1_tracktwoMVA_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14", "tau25_medium1_tracktwoMVA","xe50"]]],
        ['mu14_tau25_mediumRNN_tracktwoMVA_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14", "tau25_mediumRNN_tracktwoMVA","xe50"]]],
        ['mu14_ivarloose_tau25_medium1_tracktwoEF_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose", "tau25_medium1_tracktwoEF","xe50"]]],
        ['mu14_ivarloose_tau25_medium1_tracktwoMVA_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose", "tau25_medium1_tracktwoMVA","xe50"]]],
        ['mu14_ivarloose_tau25_mediumRNN_tracktwoMVA_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose", "tau25_mediumRNN_tracktwoMVA","xe50"]]],
        ['tau35_medium1_tracktwoEF_xe70_L1XE45',  'L1_TAU20IM_2J20_XE45',['L1_TAU20IM','L1_XE45'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwoEF","xe70_L1XE45"]]],  
        ['tau35_medium1_tracktwoMVA_xe70_L1XE45',  'L1_TAU20IM_2J20_XE45',['L1_TAU20IM','L1_XE45'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwoMVA","xe70_L1XE45"]]],  
        ['tau35_mediumRNN_tracktwoMVA_xe70_L1XE45',  'L1_TAU20IM_2J20_XE45',['L1_TAU20IM','L1_XE45'], [PhysicsStream], ['Primary:20000','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_mediumRNN_tracktwoMVA","xe70_L1XE45"]]],  


        # ATR-17807
        #tracktwoEF
        ['g25_medium_L1EM24VHI_tau25_dikaonmass_tracktwoEF_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dikaonmass_tracktwoEF'],False] ],
        ['g25_medium_L1EM24VHI_tau25_dipion1loose_tracktwoEF_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dipion1loose_tracktwoEF'],False] ],
        ['g25_medium_L1EM24VHI_tau25_dipion2_tracktwoEF_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dipion2_tracktwoEF'],False] ],
        ['g25_medium_L1EM24VHI_tau25_kaonpi1_tracktwoEF_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_kaonpi1_tracktwoEF'],False] ],
        ['g25_medium_L1EM24VHI_tau25_kaonpi2_tracktwoEF_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_kaonpi2_tracktwoEF'],False] ],
        ['g25_medium_L1EM24VHI_tau25_singlepion_tracktwoEF_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_singlepion_tracktwoEF'],False] ],
	    ['g35_medium_L1EM24VHI_tau25_dipion3_tracktwoEF_60mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g35_medium_L1EM24VHI','tau25_dipion3_tracktwoEF'],False] ],
        #tracktwoMVA
        ['g25_medium_L1EM24VHI_tau25_dikaonmass_tracktwoMVA_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dikaonmass_tracktwoMVA'],False] ],
        ['g25_medium_L1EM24VHI_tau25_dipion1loose_tracktwoMVA_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dipion1loose_tracktwoMVA'],False] ],
        ['g25_medium_L1EM24VHI_tau25_dipion2_tracktwoMVA_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dipion2_tracktwoMVA'],False] ],
        ['g25_medium_L1EM24VHI_tau25_kaonpi1_tracktwoMVA_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_kaonpi1_tracktwoMVA'],False] ],
        ['g25_medium_L1EM24VHI_tau25_kaonpi2_tracktwoMVA_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_kaonpi2_tracktwoMVA'],False] ],
        ['g25_medium_L1EM24VHI_tau25_singlepion_tracktwoMVA_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_singlepion_tracktwoMVA'],False] ],
        ['g35_medium_L1EM24VHI_tau25_dipion3_tracktwoMVA_60mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g35_medium_L1EM24VHI','tau25_dipion3_tracktwoMVA'],False] ],


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


        ['j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split','L1_J40.0ETA25_2J25_J20.31ETA49', ['','','','',''], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet_combined' ], -1, ['serial',-1,['j80_0eta240','j60','j45_320eta490_AND','2j35_gsc45_bmv2c1070_split'] ]],

        ['j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49','L1_J25.0ETA23_2J15.31ETA49',['',''], [PhysicsStream], ['Primary:20000','RATE:MultiBJet', 'BW:BJet_combined' ], -1, ['serial',-1,['2j45_320eta490_L1J25.0ETA23_2J15.31ETA49','j45_gsc55_bmv2c1070_split'] ]],

        # 2017 g+tau chains (ATR-16266)
	# ATR-14947
        ['g25_medium_L1EM24VHIM_tau25_dikaonmass_tracktwo_50mVis10000','L1_EM24VHIM', ['L1_EM24VHIM','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHIM','tau25_dikaonmass_tracktwo'],False]],
        ['g25_medium_L1EM24VHI_tau25_dikaonmass_tracktwo_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dikaonmass_tracktwo'],False]],
        ['g25_medium_L1EM24VHIM_tau25_singlepion_tracktwo_50mVis10000','L1_EM24VHIM', ['L1_EM24VHIM','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHIM','tau25_singlepion_tracktwo'],False]],
        ['g25_medium_L1EM24VHI_tau25_singlepion_tracktwo_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_singlepion_tracktwo'],False]],
        ['g25_medium_L1EM24VHIM_tau25_dipion1loose_tracktwo_50mVis10000','L1_EM24VHIM', ['L1_EM24VHIM','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHIM','tau25_dipion1loose_tracktwo'],False]],
        ['g25_medium_L1EM24VHI_tau25_dipion1loose_tracktwo_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dipion1loose_tracktwo'],False]],
        ['g25_medium_L1EM24VHIM_tau25_dipion2_tracktwo_50mVis10000','L1_EM24VHIM', ['L1_EM24VHIM','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHIM','tau25_dipion2_tracktwo'],False]],
        ['g25_medium_L1EM24VHI_tau25_dipion2_tracktwo_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dipion2_tracktwo'],False]],
        ['g25_medium_L1EM24VHIM_tau25_kaonpi1_tracktwo_50mVis10000','L1_EM24VHIM', ['L1_EM24VHIM','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHIM','tau25_kaonpi1_tracktwo'],False]],
        ['g25_medium_L1EM24VHI_tau25_kaonpi1_tracktwo_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_kaonpi1_tracktwo'],False]],
        ['g25_medium_L1EM24VHIM_tau25_kaonpi2_tracktwo_50mVis10000','L1_EM24VHIM', ['L1_EM24VHIM','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHIM','tau25_kaonpi2_tracktwo'],False]],
        ['g25_medium_L1EM24VHI_tau25_kaonpi2_tracktwo_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_kaonpi2_tracktwo'],False]],	
  
        ### ATR-14196
        ['g85_tight_L1EM24VHIM_3j50noL1', 'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['Primary:17000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g85_tight_L1EM24VHIM","3j50noL1"]]],
        ['g85_tight_icalovloose_L1EM24VHIM_3j50noL1', 'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g85_tight_icalovloose_L1EM24VHIM","3j50noL1"]]],
        ['g100_tight_L1EM24VHIM_3j50noL1', 'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g100_tight_L1EM24VHIM","3j50noL1"]]],
        ['g100_tight_icalovloose_L1EM24VHIM_3j50noL1', 'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g100_tight_icalovloose_L1EM24VHIM","3j50noL1"]]],
       	['g85_tight_L1EM22VHI_3j50noL1', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['Primary:17000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g85_tight_L1EM22VHI","3j50noL1"]]],
        ['g85_tight_L1EM24VHI_3j50noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['Primary:17000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g85_tight_L1EM24VHI","3j50noL1"]]],
        ['g100_tight_L1EM22VHI_3j50noL1', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['Primary:17000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g100_tight_L1EM22VHI","3j50noL1"]]],
        ['g100_tight_L1EM24VHI_3j50noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g100_tight_L1EM24VHI","3j50noL1"]]],


        ['j80_bmv2c1050_split_xe60_L12J50_XE40','L1_2J50_XE40',  [],  [PhysicsStream], ['Primary:17000','RATE:JetMET', 'BW:MET', 'BW:BJet_combined'], -1,['serial',-1,["xe60","j80_bmv2c1050_split"]]],
        ['mu14_ivarloose_3j20_L1MU10_3J20', 'L1_MU10_3J20', ['L1_MU10_3J20', ''], [PhysicsStream], ['Primary:20000','RATE:MuonJet', 'BW:Muon','BW:Jet'], -1,['serial',-1,['mu14_ivarloose', '3j20'] ],True],
        
        # with pi0's (ATR-16600)
        ['g35_medium_L1EM24VHI_tau25_dipion3_tracktwo_60mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Primary:20000','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g35_medium_L1EM24VHI','tau25_dipion3_tracktwo'],False]],

        ## Lepton + jets with unisolated leptons:
        # For above 1.0 e34
        ['e9_lhvloose_nod0_mu20_mu8noL1',        'L1_MU20MU21',  ['L1_EM3', 'L1_MU20', ''],   [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['serial',-1,['mu20','mu8noL1','e9_lhvloose_nod0'] ]],
        ['e9_lhvloose_nod0_mu20_mu8noL1_L1EM8VH_MU20',  'L1_EM8VH_MU20',['L1_EM8VH','L1_MU20',''], [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['serial',-1,['mu20','mu8noL1','e9_lhvloose_nod0'] ]],
        ['e9_lhvloose_nod0_mu20_mu8noL1_L1EM7_MU20',    'L1_EM7_MU20',  ['L1_EM7','L1_MU20',''],   [PhysicsStream], ['Primary:20000','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['serial',-1,['mu20','mu8noL1','e9_lhvloose_nod0'] ]],


        #ATR-17462 low pt photon triggers
        ['2g6_tight_icalotight_L1J100', 'L1_J100', ['L1_2EM3'],  [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1],
        ['2g6_loose_L1J100', 'L1_J100', ['L1_2EM3'],  [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1],


        # photon + mulit-jets ATR-17502
        ['g45_loose_6j45_0eta240', 'L1_4J15.0ETA25', ['L1_EM15VH','L1_4J15.0ETA25'], [PhysicsStream], ['Primary:20000','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g45_loose","6j45_0eta240"]]],


			 ]


    if TriggerFlags.doFTK():
            TriggerFlags.CombinedSlice.signatures = TriggerFlags.CombinedSlice.signatures() + [

        # muon +tau FTK chains for T&P performance measurement
        ['mu26_ivarmedium_tau25_perf_tracktwo_L1TAU12IM_FTK',          'L1_MU20_TAU12IM_FTK',['L1_MU20_FTK','L1_TAU12IM_FTK'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu26_ivarmedium","tau25_perf_tracktwo_L1TAU12IM_FTK"]]],
             ]


    TriggerFlags.HeavyIonSlice.signatures = [
        ]


 #Beamspot chanis first try ATR-9847                                                                                                               
    TriggerFlags.BeamspotSlice.signatures = [

        ##trkFS --> no selection, write out PEB events
        ['beamspot_allTE_trkfast_peb_L1TRT_FILLED',           'L1_TRT_FILLED', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],

        ]   


    if TriggerFlags.doFTK():
        TriggerFlags.BeamspotSlice.signatures = TriggerFlags.BeamspotSlice.signatures() + [                                                                 
            ['beamspot_idperf_FTK_L14J20_FTK',      'L1_4J20_FTK', [], ["BeamSpot"], ['RATE:BeamSpot',  'BW:BeamSpot'], -1],
            ]

    TriggerFlags.MinBiasSlice.signatures = [
        # LS1 chains
        ['mb_sptrk',                     'L1_RD0_FILLED', [], ['MinBias', 'express'], ["RATE:MinBias", "BW:MinBias"], -1],
       
        #HMT without pileup supprestion seeded by L1_TE triggers
        

        
        ]
        
    TriggerFlags.CalibSlice.signatures   = [
        
        ['lhcfpeb',       'L1_LHCF',       [], ['IDFwd'], ["RATE:Calibration", "BW:Detector"], -1],
        #Special re-run chains
        ['larnoiseburst_rerun',                  '',             [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        ['larnoiseburst_loose_rerun',            '',       [], ['LArNoiseBurst'], ["RATE:Calibration", "BW:Detector"], -1],
        


        ]

    TriggerFlags.CosmicSlice.signatures  = [ 
        ['tilecalib_laser',   'L1_CALREQ2', [], ['Tile'], ["RATE:Calibration", "RATE:Cosmic_TileCalibration", "BW:Detector"], -1],
 

        ]
    TriggerFlags.StreamingSlice.signatures = [


        ['noalg_L1J400',  'L1_J400',   [], [PhysicsStream, 'express'], ["Primary:20000","RATE:SeededStreamers", "BW:Jet"], -1 ],
 
        
        ]

    if TriggerFlags.doFTK():
        TriggerFlags.StreamingSlice.signatures = TriggerFlags.StreamingSlice.signatures() + [
            ]
     
    TriggerFlags.MonitorSlice.signatures = [
        ['l1calooverflow','', [], ['L1Calo'], ['RATE:Monitoring','BW:Other'], -1],

        ]

    # Random Seeded EB chains which select at the HLT based on L1 TBP bits
    TriggerFlags.EnhancedBiasSlice.signatures = [
        # Enhanced bias HLT items
        ['eb_low_L1RD2_FILLED',               'L1_RD2_FILLED',             [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
        ['eb_high_L1RD2_FILLED',              'L1_RD2_FILLED',             [], ['EnhancedBias'], ["RATE:EnhancedBias", "BW:Detector"], -1],
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
        'e0_perf_L1EM3_EMPTY',
        'g0_perf_L1EM3_EMPTY',
        'ht0_L1J12_EMPTY',
        'id_cosmicid_L1MU11_EMPTY',
        'id_cosmicid_L1MU4_EMPTY',
        'j0_L1J12_EMPTY',
        'j0_perf_boffperf_L1J12_EMPTY',
        'larcalib_L1EM3_EMPTY',
        'larcalib_L1J12_EMPTY',
        'larcalib_L1J3031ETA49_EMPTY',
        'larcalib_L1TAU8_EMPTY',
        'larps_L1EM3_EMPTY',
        'larps_L1EM7_EMPTY',
        'larps_L1J12_EMPTY',
        'larps_L1J3031ETA49_EMPTY',
        'larps_L1J30_EMPTY',
        'larps_L1TAU8_EMPTY',
        'mb_sptrk_costr',
        'mb_sptrk_costr_L1RD0_EMPTY',
        'mu4_cosmic_L1MU11_EMPTY',
        'mu4_cosmic_L1MU4_EMPTY',
        'mu4_msonly_cosmic_L1MU11_EMPTY',
        'mu4_msonly_cosmic_L1MU4_EMPTY',
        'sct_noise',
        'tilecalib_laser',
        ]
    HLTPrescales_cosmics.update(dict(map(None,chain_list,len(chain_list)*[ [1, 0, 0] ])))

    HLTPrescales_unprescaled_only_physics_prescale = {}
    HLTPrescales_unprescaled_only_physics_prescale.update(dict(map(None,chain_list,len(chain_list)*[ [-1, 0, -1] ])))
    L1Prescales_unprescaled_only_physics_prescale = {}
    L1Prescales_unprescaled_only_physics_prescale.update(dict(map(None,chain_list,len(chain_list)*[ [-1, 0, -1] ])))


    HLT_chain_list = [
        'xe110_pufit_L1XE50',
        'xe110_pufit_wEFMu_L1XE50',
        'xe120_pufit_L1XE50',
        'xe120_pufit_wEFMu_L1XE50',
        'xe120_mht_xe80_L1XE50',
        'tau35_medium1_tracktwo_tau25_medium1_tracktwo_tautsf_L1DR-TAU20ITAU12I-J25',
        'g35_medium_L1EM24VHI_tau25_dipion3_tracktwo_60mVis10000',
        'e60_etcut_trkcut_L1EM24VHIM_j15_perf_xe60_6dphi15_mt35',
        'e60_etcut_trkcut_L1EM24VHIM_xs30_j15_perf_xe30_6dphi15_mt35',
        'xe100_pufit_L1XE55',
        'xe100_pufit_L1XE60',
        '2e24_lhvloose_nod0_2j35_boffperf_split',
        '2j35_bmv2c1040_split_2j35_L14J15.0ETA25',
        '2j35_bmv2c1050_split_3j35',
        '2j35_bmv2c1070_split_2j35_bmv2c1085_split_L14J15.0ETA25',
        '2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15.0ETA25',
        '2j35_gsc45_bmv2c1060_split_3j35_gsc45_boffperf_split',
        '2j35_gsc45_bmv2c1070_split_xe80_mht_L12J15_XE55',
        '2j45_gsc55_bmv2c1050_split_ht300_L1HT190-J15s5.ETA21',
        '2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15.0ETA25',
        '2mu14_2j35_boffperf_split',
        '3j35_bmv2c1070_split_j35_L14J15.0ETA25',
        '3j35_bmv2c1077_split_xe60_mht_L13J15.0ETA25_XE40',
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
        'j85_gsc100_bmv2c1050_split_xe80_mht_L1XE60',
        'j60_gsc85_bmv2c1050_split_3j60_gsc85_boffperf_split',
        'j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split',
        'mu26_ivarmedium_j110_gsc150_boffperf_split_j35_boffperf_split',
        'mu26_ivarmedium_j150_boffperf_split_j35_boffperf_split',
        'mu6_j225_gsc400_boffperf_split_dr05_dz02',
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
        '3mu3_mu3noL1_L13MU4',
        '3mu4_bDimu',
        '3mu4_bJpsi',
        '3mu4_bPhi',
        '3mu4_bTau',
        '3mu4_bUpsi',
        '3mu4_mu2noL1',
        '3mu4_mu4noL1',
        '3mu6_bJpsi',
        '3mu6_bTau',
        '3mu6_bUpsi',
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
        'larcalib_L1J3031ETA49_EMPTY',
        'larhec_L1EM7_FIRSTEMPTY',
        'larhec_L1J12_FIRSTEMPTY',
        'larhec_L1J30_FIRSTEMPTY',
        'larnoiseburst_L1J100',
        'larnoiseburst_L1J40_XE60',
        'larnoiseburst_L1XE60',
        'larnoiseburst_L1XE70',
        'larnoiseburst_L1XE80',
        'larps_L1J3031ETA49_EMPTY',
        'larps_L1J30_FIRSTEMPTY',
        'noalg_L1TGC_BURST',
        'satu20em_l1satmon_L1EM20VH_FIRSTEMPTY',
        'satu20em_l1satmon_L1J100',
        #'satu20em_l1satmon_L1J100.31ETA49',
        #'satu20em_l1satmon_L1J100.31ETA49_FIRSTEMPTY',
        'satu20em_l1satmon_L1J100_FIRSTEMPTY',
        'tilecalib_laser',
        '2e12_lhloose_nod0_mu10',
        '2e12_lhmedium_nod0_mu10',
        '2e17_lhvloose_nod0_L12EM15VHI',
        '2e19_lhvloose_nod0',
        '2e24_lhvloose_nod0',
        '2e5_lhvloose_nod0_j40_xe70_pufit_2dphi10_L1J40_XE50_DPHI-J20s2XE50',
        '2e5_lhvloose_nod0_j40_xe70_pufit_2dphi10_L1XE60',
        '2g10_loose_mu20',
        '2g10_medium_mu20',
        '2g20_tight_icalotight_L12EM15VHI',
        '2g20_tight_icalovloose_L12EM15VHI',
        '2g22_tight_L12EM15VHI',
        '2g22_tight_L1EM7_EMPTY',
        '2g22_tight_L1EM7_UNPAIRED_ISO',
        '2g22_tight_icalotight_L12EM15VHI',
        '2g22_tight_icalovloose_L12EM15VHI',
        '2g25_loose_g15_loose',
        '2g25_loose_g20_loose',
        '2g25_tight_L12EM20VH',
        '2g50_loose_L12EM20VH',
        '2g50_tight_L1EM7_EMPTY',
        '2g50_tight_L1EM7_UNPAIRED_ISO',
        '2g60_loose_L12EM20VH',
        '3g25_loose',
        'e12_lhloose_nod0_2mu10',
        'e12_lhmedium_nod0_2mu10',
        'e140_lhloose_nod0',
        'e140_lhloose_nod0_L1EM24VHI',
        'e140_lhloose_nod0_L1EM24VHIM',
        'e17_lhloose_nod0_mu14',
        'e20_lhtight_nod0_ivarloose_3j20_L1EM18VHI_3J20',
        'e24_lhloose_nod0_2e12_lhloose_nod0_L1EM20VH_3EM10VH',
        'e24_lhmedium_nod0_2g12_loose',
        'e24_lhmedium_nod0_2g12_medium',
        'e24_lhmedium_nod0_L1EM20VH_g25_medium',
        'e24_lhvloose_nod0_2e12_lhvloose_nod0_L1EM20VH_3EM10VH',
        'e25_mergedtight_g35_medium_Heg',
        'e25_mergedtight_ivarloose_g35_medium_icalovloose_Heg',
        'e26_lhmedium_nod0_mu8noL1',
        'e26_lhtight_nod0_e15_etcut_L1EM7_Zee',
        'e26_lhtight_nod0_ivarloose',
        'e28_lhmedium_nod0_L1EM24VHIM_mu8noL1',
        'e28_lhmedium_nod0_mu8noL1',
        'e28_lhtight_nod0_L1EM24VHIM_e15_etcut_L1EM7_Zee',
        'e28_lhtight_nod0_e15_etcut_L1EM7_Zee',
        'e28_lhtight_nod0_ivarloose',
        'e28_lhtight_nod0_ivarloose_L1EM22VHI',
        'e28_lhtight_nod0_ivarloose_L1EM24VHIM',
        'e28_lhtight_nod0_noringer_ivarloose',
        'e28_lhtight_nod0_noringer_ivarloose_L1EM24VHIM',
        'e300_etcut',
        'e300_etcut_L1EM24VHIM',
        'e30_mergedtight_g35_medium_Heg',
        'e32_lhtight_nod0_ivarloose',
        'e5_lhloose_nod0_j40_xe70_pufit_2dphi10_L1XE60',
        'e5_lhloose_nod0_j50_xe70_pufit_2dphi10_L1J40_XE50_DPHI-J20s2XE50',
        'e5_lhloose_nod0_mu4_j30_xe40_pufit_2dphi10_L1MU4_J30_XE40_DPHI-J20s2XE30',
        'e5_lhmedium_nod0_j50_xe80_pufit_2dphi10_L1J40_XE50_DPHI-J20s2XE50',
        'e5_lhmedium_nod0_mu4_j30_xe65_pufit_2dphi10_L1MU4_XE60',
        'e60_lhmedium_nod0',
        'e60_lhmedium_nod0_L1EM24VHI',
        'e60_lhmedium_nod0_L1EM24VHIM',
        'e70_lhloose_nod0_L1EM24VHIM_xe70noL1',
        'e70_lhloose_nod0_xe70noL1',
        'e7_lhmedium_nod0_mu24',
        'e80_lhloose_nod0_L1EM24VHIM_xe80noL1',
        'e80_lhloose_nod0_L1EM24VHI_xe80noL1',
        'e80_lhloose_nod0_xe80noL1',
        'e80_lhmedium_nod0',
        'e80_lhmedium_nod0_L1EM24VHI',
        'e80_lhmedium_nod0_L1EM24VHIM',
        'g0_hiptrt_L1EM22VHI',
        'g0_hiptrt_L1EM24VHI',
        'g0_hiptrt_L1EM24VHIM',
        'g100_tight_L1EM22VHI_3j50noL1',
        'g100_tight_L1EM24VHIM_3j50noL1',
        'g100_tight_L1EM24VHI_3j50noL1',
        'g100_tight_icalovloose_L1EM22VHI_3j50noL1',
        'g100_tight_icalovloose_L1EM24VHIM_3j50noL1',
        'g100_tight_icalovloose_L1EM24VHI_3j50noL1',
        'g140_loose',
        'g140_tight',
        'g140_tight_L1EM24VHIM',
        'g15_loose_2mu10_msonly',
        'g15_loose_2mu10_msonly_L1MU4_UNPAIRED_ISO',
        'g15_loose_2mu10_msonly_L1MU6_EMPTY',
        'g200_loose',
        'g200_loose_L1EM24VHIM',
        'g20_tight_icaloloose_j35_bmv2c1077_split_3j35_0eta490_invm500',
        'g25_medium_L1EM22VHI_4j35_0eta490_invm1000',
        'g25_medium_mu24',
        'g27_medium_L1EM24VHI_2j35_bmv2c1077_split_2j35_0eta490',
        'g27_medium_L1EM24VHI_j35_bmv2c1077_split_3j35_0eta490_invm700',
        'g300_etcut',
        'g300_etcut_L1EM24VHI',
        'g300_etcut_L1EM24VHIM',
        'g35_loose_L1EM24VHIM_mu15_mu2noL1',
        'g35_loose_L1EM24VHIM_mu18',
        'g35_loose_L1EM24VHI_mu15_mu2noL1',
        'g35_loose_L1EM24VHI_mu18',
        'g35_medium_g25_medium_L12EM20VH',
        'g35_medium_g25_medium_L1EM7_EMPTY',
        'g35_medium_g25_medium_L1EM7_UNPAIRED_ISO',
        'g35_tight_icalotight_L1EM24VHIM_mu15noL1_mu2noL1',
        'g35_tight_icalotight_L1EM24VHIM_mu18noL1',
        'g35_tight_icalotight_L1EM24VHI_mu15noL1_mu2noL1',
        'g35_tight_icalotight_L1EM24VHI_mu18noL1',
        'g40_tight_icalotight_L1EM24VHIM_mu15noL1_mu2noL1',
        'g40_tight_icalotight_L1EM24VHIM_mu18noL1',
        'g40_tight_icalotight_L1EM24VHI_mu15noL1_mu2noL1',
        'g40_tight_icalotight_L1EM24VHI_mu18noL1',
        'g80_loose_L1EM24VHIM_xe80noL1',
        'g80_loose_L1EM24VHI_xe80noL1',
        'g80_loose_icalovloose_L1EM24VHIM_xe80noL1',
        'g80_loose_icalovloose_L1EM24VHI_xe80noL1',
        'g80_loose_icalovloose_xe80noL1',
        'g80_loose_xe80noL1',
        'g85_tight_L1EM22VHI_3j50noL1',
        'g85_tight_L1EM24VHIM_3j50noL1',
        'g85_tight_L1EM24VHI_3j50noL1',
        'g85_tight_icalovloose_L1EM22VHI_3j50noL1',
        'g85_tight_icalovloose_L1EM24VHIM_3j50noL1',
        'g85_tight_icalovloose_L1EM24VHI_3j50noL1',
        'e9_lhvloose_nod0_mu20_mu8noL1',
        'e9_lhvloose_nod0_mu20_mu8noL1_L1EM7_MU20',
        'e9_lhvloose_nod0_mu20_mu8noL1_L1EM8VH_MU20',
        '10j40_L14J15',
        '10j40_L14J150ETA25',
        '10j40_L14J20',
        '10j40_L16J15',
        '2j225_gsc250_boffperf_split_0eta240_j85_gsc120_boffperf_split_0eta240',
        '2j225_gsc250_boffperf_split_0eta240_j85_gsc140_boffperf_split_0eta240',
        '2j330_a10t_lcw_jes_40smcINF_L1J100',
        '2j340_a10t_lcw_jes_40smcINF_L1J100',
        '2j350_a10t_lcw_jes_40smcINF_L1J100',
        '3j200',
        '3j225',
        '3j250',
        '4j120',
        '4j130',
        '4j85_gsc115_boffperf_split',
        '4j85_gsc120_boffperf_split',
        '4j85_gsc130_boffperf_split',
        '5j100',
        '5j100_L14J150ETA25',
        '5j50_gsc70_boffperf_split_0eta240',
        '5j50_gsc70_boffperf_split_0eta240_L14J150ETA25',
        '5j55_gsc75_boffperf_split_0eta240',
        '5j55_gsc75_boffperf_split_0eta240_L14J150ETA25',
        '5j85_gsc100_boffperf_split',
        '5j85_gsc100_boffperf_split_L14J150ETA25',
        '5j60_gsc85_boffperf_split',
        '5j60_gsc85_boffperf_split_L14J150ETA25',
        '5j60_gsc90_boffperf_split',
        '5j60_gsc90_boffperf_split_L14J150ETA25',
        '5j70_0eta240',
        '5j70_0eta240_L14J15',
        '5j70_0eta240_L14J150ETA25',
        '5j75_0eta240',
        '5j75_0eta240_L14J150ETA25',
        '5j85',
        '5j85_0eta240',
        '5j85_0eta240_L14J150ETA25',
        '5j85_L14J15',
        '5j85_L14J150ETA25',
        '5j90',
        '5j90_L14J150ETA25',
        '6j45_gsc55_boffperf_split_0eta240_L14J150ETA25',
        '6j45_gsc55_boffperf_split_0eta240_L14J20',
        '6j45_gsc55_boffperf_split_0eta240_L15J150ETA25',
        '6j45_gsc60_boffperf_split_0eta240_L14J150ETA25',
        '6j45_gsc60_boffperf_split_0eta240_L14J20',
        '6j45_gsc60_boffperf_split_0eta240_L15J150ETA25',
        '6j50_gsc70_boffperf_split',
        '6j50_gsc70_boffperf_split_L14J150ETA25',
        '6j50_gsc70_boffperf_split_L15J150ETA25',
        '6j55_0eta240_L14J15',
        '6j55_0eta240_L14J150ETA25',
        '6j55_0eta240_L14J20',
        '6j55_0eta240_L15J150ETA25',
        '6j60_0eta240_L14J150ETA25',
        '6j60_0eta240_L14J20',
        '6j60_0eta240_L15J150ETA25',
        '6j60_gsc85_boffperf_split',
        '6j60_gsc85_boffperf_split_L14J150ETA25',
        '6j60_gsc85_boffperf_split_L15J150ETA25',
        '6j70',
        '6j70_L14J15',
        '6j70_L14J150ETA25',
        '6j85',
        '6j85_L14J150ETA25',
        '7j35_gsc45_boffperf_split_0eta240_L14J150ETA25',
        '7j35_gsc45_boffperf_split_0eta240_L14J20',
        '7j35_gsc45_boffperf_split_0eta240_L15J150ETA25',
        '7j35_gsc45_boffperf_split_L14J150ETA25',
        '7j35_gsc45_boffperf_split_L14J20',
        '7j35_gsc45_boffperf_split_L15J150ETA25',
        '7j35_gsc50_boffperf_split_0eta240_L14J150ETA25',
        '7j35_gsc50_boffperf_split_0eta240_L14J20',
        '7j35_gsc50_boffperf_split_0eta240_L15J150ETA25',
        '7j35_gsc50_boffperf_split_L14J150ETA25',
        '7j35_gsc50_boffperf_split_L14J20',
        '7j35_gsc50_boffperf_split_L15J150ETA25',
        '7j45',
        '7j45_0eta240',
        '7j45_0eta240_L14J150ETA25',
        '7j45_0eta240_L14J20',
        '7j45_0eta240_L15J150ETA25',
        '7j45_L14J15',
        '7j45_L14J150ETA25',
        '7j45_L14J20',
        '7j50',
        '7j50_L14J150ETA25',
        '7j50_L14J20',
        'ht1000_L1J100',
        'ht1200_L1J100',
        'ht1400_L1J100',
        'j0_perf_ds1_L1J15',
        'j0_perf_ds1_L1J20',
        'j0_perf_ds1_L1J30',
        'j0_perf_ds1_L1J40',
        'j0_perf_ds1_L1J50_DETA20-J50J',
        'j0_perf_ds1_L1J50',
        'j0_perf_ds1_L1J75',
        'j0_perf_ds1_L1J100',
        'j225_gsc420_boffperf_split',
        'j225_gsc440_boffperf_split',
        'j225_gsc450_boffperf_split',
        'j225_gsc460_boffperf_split',
        'j225_gsc480_boffperf_split',
        'j225_gsc500_boffperf_split',
        'j260_320eta490',
        'j260_320eta490_jes',
        'j260_320eta490_lcw',
        'j260_320eta490_lcw_jes',
        'j260_320eta490_lcw_nojcalib',
        'j260_320eta490_lcw_sub',
        'j260_320eta490_lcw_subjes',
        'j260_320eta490_nojcalib',
        'j260_320eta490_sub',
        'j260_320eta490_subjes',
        'j30_jes_cleanLLP_PS_llp_L1LLP-NOMATCH',
        'j30_jes_cleanLLP_PS_llp_L1LLP-RO',
        'j30_jes_cleanLLP_PS_llp_L1TAU100',
        'j30_jes_cleanLLP_PS_llp_L1TAU30_EMPTY',
        'j30_jes_cleanLLP_PS_llp_L1TAU30_UNPAIRED_ISO',
        'j30_jes_cleanLLP_PS_llp_noiso_L1LLP-NOMATCH',
        'j30_jes_cleanLLP_PS_llp_noiso_L1LLP-RO',
        'j30_jes_cleanLLP_PS_llp_noiso_L1TAU100',
        'j30_jes_cleanLLP_PS_llp_noiso_L1TAU30_EMPTY',
        'j30_jes_cleanLLP_PS_llp_noiso_L1TAU30_UNPAIRED_ISO',
        #'j360_320eta490',
        'j420',
        'j420_a10t_lcw_jes_40smcINF_L1J100',
        'j440',
        'j440_a10t_lcw_jes_40smcINF_L1J100',
        'j450',
        'j460',
        'j460_a10_lcw_subjes_L1J100',
        'j460_a10r_L1J100',
        'j460_a10t_lcw_jes_L1J100',
        'j480',
        'j480_a10_lcw_subjes_L1J100',
        'j480_a10r_L1J100',
        'j480_a10t_lcw_jes_L1J100',
        'j500',
        'j500_a10_lcw_subjes_L1J100',
        'j500_a10r_L1J100',
        'j500_a10t_lcw_jes_L1J100',
        'j520_a10_lcw_subjes_L1J100',
        'j520_a10r_L1J100',
        'j520_a10t_lcw_jes_L1J100',
        'j540_a10_lcw_subjes_L1J100',
        'j540_a10r_L1J100',
        'j540_a10t_lcw_jes_L1J100',
        'j55_0eta240_xe50_L1J30_EMPTY',
        'noalg_L1J400',
        'noalg_cosmiccalo_L1J3031ETA49_EMPTY',
        'j80_bmv2c1050_split_xe60_L12J50_XE40',
        'xe100_pufit_L1XE55',
        'xe100_pufit_L1XE60',
        'xe110_L1XE55',
        'xe110_pufit_L1XE55',
        'xe110_pufit_L1XE60',
        'xe110_pufit_L1XE70',
        'xe110_pufit_wEFMu_L1XE55',
        'xe110_wEFMu_L1XE55',
        'xe120_L1XE55',
        'xe120_mht_xe80_L1XE55',
        'xe120_mht_xe80_L1XE60',
        'xe120_pufit_L1XE55',
        'xe120_pufit_L1XE60',
        'xe120_pufit_L1XE70',
        'xe120_pufit_wEFMu_L1XE55',
        'xe120_pufit_wEFMu_L1XE60',
        'xe130_L1XE55',
        '2mu14',
        '2mu15_L12MU10',
        '2mu4_invm1_j20_xe40_pufit_2dphi10_L12MU4_J20_XE30_DPHI-J20s2XE30',
        '2mu4_invm1_j20_xe60_pufit_2dphi10_L12MU4_J20_XE30_DPHI-J20s2XE30',
        '2mu4_invm1_j20_xe60_pufit_2dphi10_L12MU4_J40_XE50',
        '2mu4_mu3_mu2noL1_bNocut_L13MU4',
        '3mu3_mu3noL1_bNocut_L13MU4',
        '3mu6',
        '3mu6_msonly',
        '3mu6_msonly_L1MU4_UNPAIRED_ISO',
        '3mu6_msonly_L1MU6_EMPTY',
        '3mu8_msonly',
        '4mu4_bNocut',
        'e5_lhvloose_nod0_mu4_j30_xe40_pufit_2dphi10_L1MU4_J30_XE40_DPHI-J20s2XE30',
        'j30_muvtx',
        'j30_muvtx_L1MU4_UNPAIRED_ISO',
        'j30_muvtx_L1MU6_EMPTY',
        'j30_muvtx_noiso',
        'j30_muvtx_noiso_L1MU4_UNPAIRED_ISO',
        'j30_muvtx_noiso_L1MU6_EMPTY',
        'mu0_muoncalib',
        'mu10_mgonly_L1LATE-MU10_J50',
        'mu10_mgonly_L1LATE-MU10_XE40',
        'mu10_mgonly_L1LATE-MU10_XE50',
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
        'mu22_2mu4noL1',
        'mu22_mu8noL1',
        'mu22_mu8noL1_mu6noL1',
        'mu24_2mu4noL1',
        'mu24_mu10noL1',
        'mu24_mu8noL1',
        'mu26_ivarmedium',
        'mu26_mu10noL1',
        'mu26_mu8noL1',
        'mu28_ivarmedium',
        'mu28_mu8noL1',
        'mu4_j90_xe90_pufit_2dphi10_L1MU4_J50_XE50_DPHI-J20s2XE30',
        'mu4_j90_xe90_pufit_2dphi10_L1MU4_XE60',
        'mu50',
        'mu60',
        'mu60_0eta105_msonly',
        'mu80',
        'noalg_bkg_L1J15.31ETA49_UNPAIRED_ISO',
        'noalg_l1calo_L1J400',
        'e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo',
        'e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25',
        'e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1EM15VHI_2TAU12IM_4J12',
        'e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_xe50',
        'e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50',
        'e24_lhmedium_nod0_ivarloose_tau35_medium1_tracktwo',
        'g25_medium_L1EM24VHIM_tau25_dikaonmass_tracktwo_50mVis10000',
        'g25_medium_L1EM24VHIM_tau25_dipion1loose_tracktwo_50mVis10000',
        'g25_medium_L1EM24VHIM_tau25_dipion2_tracktwo_50mVis10000',
        'g25_medium_L1EM24VHIM_tau25_kaonpi1_tracktwo_50mVis10000',
        'g25_medium_L1EM24VHIM_tau25_kaonpi2_tracktwo_50mVis10000',
        'g25_medium_L1EM24VHIM_tau25_singlepion_tracktwo_50mVis10000',
        'g25_medium_L1EM24VHI_tau25_dikaonmass_tracktwo_50mVis10000',
        'g25_medium_L1EM24VHI_tau25_dipion1loose_tracktwo_50mVis10000',
        'g25_medium_L1EM24VHI_tau25_dipion2_tracktwo_50mVis10000',
        'g25_medium_L1EM24VHI_tau25_kaonpi1_tracktwo_50mVis10000',
        'g25_medium_L1EM24VHI_tau25_kaonpi2_tracktwo_50mVis10000',
        'g25_medium_L1EM24VHI_tau25_singlepion_tracktwo_50mVis10000',
        'mu14_ivarloose_L1MU11_tau35_medium1_tracktwo_L1MU11_TAU20IM',
        'mu14_ivarloose_tau25_medium1_tracktwo',
        'mu14_ivarloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25',
        'mu14_ivarloose_tau25_medium1_tracktwo_L1MU10_TAU12IM_3J12',
        'mu14_ivarloose_tau25_medium1_tracktwo_xe50',
        'mu14_ivarloose_tau35_medium1_tracktwo',
        'mu14_ivarloose_tau35_medium1_tracktwo_L1MU10_TAU20IM_J25_2J20',
        'mu14_tau25_medium1_tracktwo_xe50',
        'tau160_medium1_tracktwo_L1TAU100',
        'tau200_medium1_tracktwo_L1TAU100',
        'tau35_medium1_tracktwo_tau25_medium1_tracktwo_03dR30_L1DR-TAU20ITAU12I-J25',
        'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I-J25',
        'tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM_4J12',
        'tau35_medium1_tracktwo_xe70_L1XE45',
        'tau35_tight1_tracktwo_tau25_tight1_tracktwo_03dR30_L1DR-TAU20ITAU12I-J25',
        'tau35_tight1_tracktwo_tau25_tight1_tracktwo_L1DR-TAU20ITAU12I-J25',
        'tau40_medium1_tracktwo_tau35_medium1_tracktwo',
        'tau60_medium1_tracktwo_tau25_medium1_tracktwo_xe50',
        'tau80_medium1_tracktwo_L1TAU60_tau35_medium1_tracktwo_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I',
        'tau80_medium1_tracktwo_L1TAU60_tau60_medium1_tracktwo_L1TAU40',
        '2tau35_medium1_tracktwo_L12TAU20IM_3J20',
        '2tau35_medium1_tracktwo',
        'tau40_medium1_tracktwo_tau25_medium1_tracktwo',
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
    #
    # ID cosmic
    'id_cosmicid_L1MU11_EMPTY',
    'id_cosmicid_L1MU4_EMPTY',
    #
    # minBias chains
    'mb_mbts_L1MBTS_1',
    'mb_mbts_L1MBTS_1_1',
    'mb_mbts_L1MBTS_1_1_EMPTY',
    'mb_mbts_L1MBTS_1_1_UNPAIRED_ISO',
    'mb_mbts_L1MBTS_1_EMPTY',
    'mb_mbts_L1MBTS_1_UNPAIRED_ISO',
    'mb_mbts_L1MBTS_2',
    'mb_mbts_L1MBTS_2_EMPTY',
    'mb_mbts_L1MBTS_2_UNPAIRED_ISO',
    'mb_idperf_L1MBTS_2',
    'mb_perf_L1LUCID',
    'mb_perf_L1LUCID_EMPTY',
    'mb_perf_L1LUCID_UNPAIRED_ISO',
    'mb_perf_L1MBTS_2',
    'mb_perf_L1RD1_FILLED',
    'mb_sp600_pusup300_trk40_hmt_L1TE5',
    'mb_sp700_pusup350_trk50_hmt_L1TE5',
    'mb_sp900_pusup400_trk50_hmt_L1TE5',
    'mb_sp900_pusup400_trk60_hmt_L1TE5',
    'mb_sp1000_pusup450_trk70_hmt_L1TE5',
    'mb_sp1100_pusup450_trk70_hmt_L1TE5',
    'mb_sp1200_pusup500_trk80_hmt_L1TE5',
    'mb_sp1200_pusup500_trk90_hmt_L1TE5',
    'mb_sp1200_pusup500_trk100_hmt_L1TE5',
    'mb_sp1400_pusup550_trk90_hmt_L1TE5',
    'mb_sp1600_pusup600_trk100_hmt_L1TE5',
    'mb_sp1800_pusup700_trk110_hmt_L1TE5',
    'mb_sp2100_pusup900_trk120_hmt_L1TE5',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE5',
    'mb_sp600_pusup300_trk40_hmt_L1TE10',
    'mb_sp700_pusup350_trk50_hmt_L1TE10',
    'mb_sp900_pusup400_trk60_hmt_L1TE10',
    'mb_sp1000_pusup450_trk70_hmt_L1TE10',
    'mb_sp1100_pusup450_trk70_hmt_L1TE10',
    'mb_sp1200_pusup500_trk80_hmt_L1TE10',
    'mb_sp1400_pusup550_trk90_hmt_L1TE10',
    'mb_sp1600_pusup600_trk100_hmt_L1TE10',
    'mb_sp1700_pusup650_trk110_hmt_L1TE10',
    'mb_sp1800_pusup700_trk110_hmt_L1TE10',
    'mb_sp1900_pusup700_trk120_hmt_L1TE10',
    'mb_sp2100_pusup900_trk120_hmt_L1TE10',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE10',
    'mb_sp2500_pusup1100_trk140_hmt_L1TE10',
    'mb_sp700_pusup350_trk50_hmt_L1TE20',
    'mb_sp900_pusup400_trk60_hmt_L1TE20',
    'mb_sp1100_pusup450_trk70_hmt_L1TE20',
    'mb_sp1200_pusup500_trk80_hmt_L1TE20',
    'mb_sp1400_pusup550_trk90_hmt_L1TE20',
    'mb_sp1600_pusup600_trk100_hmt_L1TE20',
    'mb_sp1700_pusup650_trk110_hmt_L1TE20',
    'mb_sp1800_pusup700_trk110_hmt_L1TE20',
    'mb_sp1900_pusup700_trk120_hmt_L1TE20',
    'mb_sp2100_pusup750_trk130_hmt_L1TE20',
    'mb_sp2100_pusup900_trk120_hmt_L1TE20',
    'mb_sp2200_pusup800_trk140_hmt_L1TE20',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE20',
    'mb_sp2400_pusup850_trk150_hmt_L1TE20',
    'mb_sp2500_pusup1100_trk140_hmt_L1TE20',
    'mb_sp2700_pusup1200_trk150_hmt_L1TE20',
    'mb_sp1100_pusup450_trk70_hmt_L1TE30',
    'mb_sp1200_pusup500_trk80_hmt_L1TE30',
    'mb_sp1400_pusup550_trk90_hmt_L1TE30',
    'mb_sp1600_pusup600_trk100_hmt_L1TE30',
    'mb_sp1700_pusup650_trk110_hmt_L1TE30',
    'mb_sp1800_pusup700_trk110_hmt_L1TE30',
    'mb_sp1900_pusup700_trk120_hmt_L1TE30',
    'mb_sp2100_pusup750_trk130_hmt_L1TE30',
    'mb_sp2100_pusup900_trk120_hmt_L1TE30',
    'mb_sp2200_pusup800_trk140_hmt_L1TE30',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE30',
    'mb_sp2400_pusup850_trk150_hmt_L1TE30',
    'mb_sp2500_pusup1100_trk140_hmt_L1TE30',
    'mb_sp2700_pusup1200_trk150_hmt_L1TE30',
    'mb_sp2900_pusup1300_trk160_hmt_L1TE30',
    'mb_sp1400_pusup550_trk90_hmt_L1TE40',
    'mb_sp1600_pusup600_trk100_hmt_L1TE40',
    'mb_sp1700_pusup650_trk110_hmt_L1TE40',
    'mb_sp1900_pusup700_trk120_hmt_L1TE40',
    'mb_sp2100_pusup750_trk130_hmt_L1TE40',
    'mb_sp2200_pusup800_trk140_hmt_L1TE40',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE40',
    'mb_sp2400_pusup850_trk150_hmt_L1TE40',
    'mb_sp2500_pusup1100_trk140_hmt_L1TE40',
    'mb_sp2700_pusup1200_trk150_hmt_L1TE40',
    'mb_sp2900_pusup1300_trk160_hmt_L1TE40',
    'mb_sp1600_pusup600_trk100_hmt_L1TE50',
    'mb_sp1700_pusup650_trk110_hmt_L1TE50',
    'mb_sp1900_pusup700_trk120_hmt_L1TE50',
    'mb_sp2100_pusup750_trk130_hmt_L1TE50',
    'mb_sp2200_pusup800_trk140_hmt_L1TE50',
    'mb_sp2400_pusup850_trk150_hmt_L1TE50',
    #'mb_sp1700_pusup650_trk110_hmt_L1TE60',
    #'mb_sp1900_pusup700_trk120_hmt_L1TE60',
    #'mb_sp2100_pusup750_trk130_hmt_L1TE60',
    #'mb_sp2200_pusup800_trk140_hmt_L1TE60',
    #'mb_sp2400_pusup850_trk150_hmt_L1TE60',
    #'mb_sp1900_pusup700_trk120_hmt_L1TE70',
    #'mb_sp2100_pusup750_trk130_hmt_L1TE70',
    #'mb_sp2200_pusup800_trk140_hmt_L1TE70',
    #'mb_sp2400_pusup850_trk150_hmt_L1TE70',
    'mb_sp600_trk40_hmt_L1TE5',
    'mb_sp700_trk50_hmt_L1TE5',
    'mb_sp900_trk50_hmt_L1TE5',
    'mb_sp900_trk60_hmt_L1TE5',
    'mb_sp1000_trk70_hmt_L1TE5',
    'mb_sp1100_trk70_hmt_L1TE5',
    'mb_sp1200_trk80_hmt_L1TE5',
    'mb_sp1200_trk90_hmt_L1TE5',
    'mb_sp1200_trk100_hmt_L1TE5',
    'mb_sp1400_trk90_hmt_L1TE5',
    'mb_sp1600_trk100_hmt_L1TE5',
    'mb_sp1800_trk110_hmt_L1TE5',
    'mb_sp2100_trk120_hmt_L1TE5',
    'mb_sp2300_trk130_hmt_L1TE5',
    'mb_sp600_trk40_hmt_L1TE10',
    'mb_sp700_trk50_hmt_L1TE10',
    'mb_sp900_trk60_hmt_L1TE10',
    'mb_sp1000_trk70_hmt_L1TE10',
    'mb_sp1100_trk70_hmt_L1TE10',
    'mb_sp1200_trk80_hmt_L1TE10',
    'mb_sp1400_trk90_hmt_L1TE10',
    'mb_sp1600_trk100_hmt_L1TE10',
    'mb_sp1700_trk110_hmt_L1TE10',
    'mb_sp1800_trk110_hmt_L1TE10',
    'mb_sp1900_trk120_hmt_L1TE10',
    'mb_sp2100_trk120_hmt_L1TE10',
    'mb_sp2300_trk130_hmt_L1TE10',
    'mb_sp2500_trk140_hmt_L1TE10',
    'mb_sp700_trk50_hmt_L1TE20',
    'mb_sp900_trk60_hmt_L1TE20',
    'mb_sp1100_trk70_hmt_L1TE20',
    'mb_sp1200_trk80_hmt_L1TE20',
    'mb_sp1400_trk90_hmt_L1TE20',
    'mb_sp1600_trk100_hmt_L1TE20',
    'mb_sp1700_trk110_hmt_L1TE20',
    'mb_sp1800_trk110_hmt_L1TE20',
    'mb_sp1900_trk120_hmt_L1TE20',
    'mb_sp2100_trk130_hmt_L1TE20',
    'mb_sp2100_trk120_hmt_L1TE20',
    'mb_sp2200_trk140_hmt_L1TE20',
    'mb_sp2300_trk130_hmt_L1TE20',
    'mb_sp2400_trk150_hmt_L1TE20',
    'mb_sp2500_trk140_hmt_L1TE20',
    'mb_sp2700_trk150_hmt_L1TE20',
    'mb_sp1100_trk70_hmt_L1TE30',
    'mb_sp1200_trk80_hmt_L1TE30',
    'mb_sp1400_trk90_hmt_L1TE30',
    'mb_sp1600_trk100_hmt_L1TE30',
    'mb_sp1700_trk110_hmt_L1TE30',
    'mb_sp1800_trk110_hmt_L1TE30',
    'mb_sp1900_trk120_hmt_L1TE30',
    'mb_sp2100_trk130_hmt_L1TE30',
    'mb_sp2100_trk120_hmt_L1TE30',
    'mb_sp2200_trk140_hmt_L1TE30',
    'mb_sp2300_trk130_hmt_L1TE30',
    'mb_sp2400_trk150_hmt_L1TE30',
    'mb_sp2500_trk140_hmt_L1TE30',
    'mb_sp2700_trk150_hmt_L1TE30',
    'mb_sp2900_trk160_hmt_L1TE30',
    'mb_sp1400_trk90_hmt_L1TE40',
    'mb_sp1600_trk100_hmt_L1TE40',
    'mb_sp1700_trk110_hmt_L1TE40',
    'mb_sp1900_trk120_hmt_L1TE40',
    'mb_sp2100_trk130_hmt_L1TE40',
    'mb_sp2200_trk140_hmt_L1TE40',
    'mb_sp2300_trk130_hmt_L1TE40',
    'mb_sp2400_trk150_hmt_L1TE40',
    'mb_sp2500_trk140_hmt_L1TE40',
    'mb_sp2700_trk150_hmt_L1TE40',
    'mb_sp2900_trk160_hmt_L1TE40',
    'mb_sp1600_trk100_hmt_L1TE50',
    'mb_sp1700_trk110_hmt_L1TE50',
    'mb_sp1900_trk120_hmt_L1TE50',
    'mb_sp2100_trk130_hmt_L1TE50',
    'mb_sp2200_trk140_hmt_L1TE50',
    'mb_sp2400_trk150_hmt_L1TE50',
    #'mb_sp1700_trk110_hmt_L1TE60',
    #'mb_sp1900_trk120_hmt_L1TE60',
    #'mb_sp2100_trk130_hmt_L1TE60',
    #'mb_sp2200_trk140_hmt_L1TE60',
    #'mb_sp2400_trk150_hmt_L1TE60',
    #'mb_sp1900_trk120_hmt_L1TE70',
    #'mb_sp2100_trk130_hmt_L1TE70',
    #'mb_sp2200_trk140_hmt_L1TE70',
    #'mb_sp2400_trk150_hmt_L1TE70',
    'mb_sp700_trk50_hmt_L1TE15',
    'mb_sp900_trk60_hmt_L1TE15',
    'mb_sp1100_trk70_hmt_L1TE15',
    'mb_sp1200_trk80_hmt_L1TE15',
    'mb_sp1400_trk90_hmt_L1TE15',
    'mb_sp1600_trk100_hmt_L1TE15',
    'mb_sp1700_trk110_hmt_L1TE15',
    'mb_sp1900_trk120_hmt_L1TE15',
    'mb_sp1100_trk70_hmt_L1TE25',
    'mb_sp1200_trk80_hmt_L1TE25',
    'mb_sp1400_trk90_hmt_L1TE25',
    'mb_sp1600_trk100_hmt_L1TE25',
    'mb_sp1700_trk110_hmt_L1TE25',
    'mb_sp1900_trk120_hmt_L1TE25',
    'mb_sp700_pusup350_trk50_hmt_L1TE15',
    'mb_sp900_pusup400_trk60_hmt_L1TE15',
    'mb_sp1100_pusup450_trk70_hmt_L1TE15',
    'mb_sp1200_pusup500_trk80_hmt_L1TE15',
    'mb_sp1400_pusup550_trk90_hmt_L1TE15',
    'mb_sp1600_pusup600_trk100_hmt_L1TE15',
    'mb_sp1700_pusup650_trk110_hmt_L1TE15',
    'mb_sp1900_pusup700_trk120_hmt_L1TE15',
    'mb_sp1100_pusup450_trk70_hmt_L1TE25',
    'mb_sp1200_pusup500_trk80_hmt_L1TE25',
    'mb_sp1400_pusup550_trk90_hmt_L1TE25',
    'mb_sp1600_pusup600_trk100_hmt_L1TE25',
    'mb_sp1700_pusup650_trk110_hmt_L1TE25',
    'mb_sp1900_pusup700_trk120_hmt_L1TE25',
    #'mb_sp900_pusup400_trk50_hmt_L1TE5.0ETA24',
    #'mb_sp900_pusup400_trk60_hmt_L1TE5.0ETA24',
    #'mb_sp1000_pusup450_trk70_hmt_L1TE5.0ETA24',
    #'mb_sp1200_pusup500_trk80_hmt_L1TE5.0ETA24',
    #'mb_sp1200_pusup500_trk90_hmt_L1TE5.0ETA24',
    #'mb_sp1200_pusup500_trk100_hmt_L1TE5.0ETA24',
    #'mb_sp1800_pusup700_trk110_hmt_L1TE5.0ETA24',
    #'mb_sp2100_pusup900_trk120_hmt_L1TE5.0ETA24',
    #'mb_sp2300_pusup1000_trk130_hmt_L1TE5.0ETA24',
    #'mb_sp1000_pusup450_trk70_hmt_L1TE10.0ETA24',
    #'mb_sp1200_pusup500_trk80_hmt_L1TE10.0ETA24',
    #'mb_sp1400_pusup550_trk90_hmt_L1TE10.0ETA24',
    #'mb_sp1600_pusup600_trk100_hmt_L1TE10.0ETA24',
    #'mb_sp1800_pusup700_trk110_hmt_L1TE10.0ETA24',
    #'mb_sp2100_pusup900_trk120_hmt_L1TE10.0ETA24',
    #'mb_sp2300_pusup1000_trk130_hmt_L1TE10.0ETA24',
    #'mb_sp2500_pusup1100_trk140_hmt_L1TE10.0ETA24',
    #'mb_sp1200_pusup500_trk80_hmt_L1TE15.0ETA24',
    #'mb_sp1400_pusup550_trk90_hmt_L1TE15.0ETA24',
    #'mb_sp1600_pusup600_trk100_hmt_L1TE15.0ETA24',
    #'mb_sp1800_pusup700_trk110_hmt_L1TE15.0ETA24',
    #'mb_sp2100_pusup900_trk120_hmt_L1TE15.0ETA24',
    #'mb_sp2300_pusup1000_trk130_hmt_L1TE15.0ETA24',
    #'mb_sp2500_pusup1100_trk140_hmt_L1TE15.0ETA24',
    #'mb_sp1400_pusup550_trk90_hmt_L1TE20.0ETA24',
    #'mb_sp1600_pusup600_trk100_hmt_L1TE20.0ETA24',
    #'mb_sp1800_pusup700_trk110_hmt_L1TE20.0ETA24',
    #'mb_sp2100_pusup900_trk120_hmt_L1TE20.0ETA24',
    #'mb_sp2300_pusup1000_trk130_hmt_L1TE20.0ETA24',
    #'mb_sp2500_pusup1100_trk140_hmt_L1TE20.0ETA24',
    #'mb_sp2700_pusup1200_trk150_hmt_L1TE20.0ETA24',
    #'mb_sp1600_pusup600_trk100_hmt_L1TE25.0ETA24',
    #'mb_sp1800_pusup700_trk110_hmt_L1TE25.0ETA24',
    #'mb_sp2100_pusup900_trk120_hmt_L1TE25.0ETA24',
    #'mb_sp2300_pusup1000_trk130_hmt_L1TE25.0ETA24',
    #'mb_sp2500_pusup1100_trk140_hmt_L1TE25.0ETA24',
    #'mb_sp2700_pusup1200_trk150_hmt_L1TE25.0ETA24',
    #'mb_sp2900_pusup1300_trk160_hmt_L1TE25.0ETA24',
    #'mb_sp1800_pusup700_trk110_hmt_L1TE30.0ETA24',
    #'mb_sp2100_pusup900_trk120_hmt_L1TE30.0ETA24',
    #'mb_sp2300_pusup1000_trk130_hmt_L1TE30.0ETA24',
    #'mb_sp2500_pusup1100_trk140_hmt_L1TE30.0ETA24',
    #'mb_sp2700_pusup1200_trk150_hmt_L1TE30.0ETA24',
    #'mb_sp2900_pusup1300_trk160_hmt_L1TE30.0ETA24',
    #'mb_sp2300_pusup1000_trk130_hmt_L1TE40.0ETA24',
    #'mb_sp2500_pusup1100_trk140_hmt_L1TE40.0ETA24',
    #'mb_sp2700_pusup1200_trk150_hmt_L1TE40.0ETA24',
    #'mb_sp2900_pusup1300_trk160_hmt_L1TE40.0ETA24',
    'mu4_mb_sp600_trk40_hmt_L1MU4_TE10',
    'mu4_mb_sp700_trk50_hmt_L1MU4_TE10',
    'mu4_mb_sp900_trk60_hmt_L1MU4_TE10',
    'mu4_mb_sp1100_trk70_hmt_L1MU4_TE10',
    'mu4_mb_sp1200_trk80_hmt_L1MU4_TE10',
    'mu4_mb_sp1400_trk90_hmt_L1MU4_TE10',
    'mu4_mb_sp1600_trk100_hmt_L1MU4_TE10',
    'mu4_mb_sp600_pusup300_trk40_hmt_L1MU4_TE10',
    'mu4_mb_sp700_pusup350_trk50_hmt_L1MU4_TE10',
    'mu4_mb_sp900_pusup400_trk60_hmt_L1MU4_TE10',
    'mu4_mb_sp1100_pusup450_trk70_hmt_L1MU4_TE10',
    'mu4_mb_sp1200_pusup500_trk80_hmt_L1MU4_TE10',
    'mu4_mb_sp1400_pusup550_trk90_hmt_L1MU4_TE10',
    'mu4_mb_sp1600_pusup600_trk100_hmt_L1MU4_TE10',
    #'mu4_mb_sp1400_trk90_hmt_L1MU4_TE50',
    #'mu4_mb_sp1600_trk100_hmt_L1MU4_TE50',
    #'mu4_mb_sp1700_trk110_hmt_L1MU4_TE50',
    #'mu4_mb_sp1400_pusup550_trk90_hmt_L1MU4_TE50',
    #'mu4_mb_sp1600_pusup600_trk100_hmt_L1MU4_TE50',
    #'mu4_mb_sp1700_pusup650_trk110_hmt_L1MU4_TE50',
    'j40_mb_sp600_pusup300_trk40_hmt',
    'j40_mb_sp700_pusup350_trk50_hmt',
    'j40_mb_sp900_pusup400_trk60_hmt',
    'j40_mb_sp1100_pusup450_trk70_hmt',
    'j40_mb_sp1200_pusup500_trk80_hmt',
    'j40_mb_sp1400_pusup550_trk90_hmt',
    'j40_mb_sp1600_pusup600_trk100_hmt',
    'j40_mb_sp600_trk40_hmt',
    'j40_mb_sp700_trk50_hmt',
    'j40_mb_sp900_trk60_hmt',
    'j40_mb_sp1100_trk70_hmt',
    'j40_mb_sp1200_trk80_hmt',
    'j40_mb_sp1400_trk90_hmt',
    'j40_mb_sp1600_trk100_hmt',
    'j50_mb_sp600_pusup300_trk40_hmt',
    'j50_mb_sp700_pusup350_trk50_hmt',
    'j50_mb_sp900_pusup400_trk60_hmt',
    'j50_mb_sp1100_pusup450_trk70_hmt',
    'j50_mb_sp1200_pusup500_trk80_hmt',
    'j50_mb_sp1400_pusup550_trk90_hmt',
    'j50_mb_sp1600_pusup600_trk100_hmt',
    'j50_mb_sp600_trk40_hmt',
    'j50_mb_sp700_trk50_hmt',
    'j50_mb_sp900_trk60_hmt',
    'j50_mb_sp1100_trk70_hmt',
    'j50_mb_sp1200_trk80_hmt',
    'j50_mb_sp1400_trk90_hmt',
    'j50_mb_sp1600_trk100_hmt',
    'j60_mb_sp600_pusup300_trk40_hmt',
    'j60_mb_sp700_pusup350_trk50_hmt',
    'j60_mb_sp900_pusup400_trk60_hmt',
    'j60_mb_sp1100_pusup450_trk70_hmt',
    'j60_mb_sp1200_pusup500_trk80_hmt',
    'j60_mb_sp1400_pusup550_trk90_hmt',
    'j60_mb_sp1600_pusup600_trk100_hmt',
    'j60_mb_sp600_trk40_hmt',
    'j60_mb_sp700_trk50_hmt',
    'j60_mb_sp900_trk60_hmt',
    'j60_mb_sp1100_trk70_hmt',
    'j60_mb_sp1200_trk80_hmt',
    'j60_mb_sp1400_trk90_hmt',
    'j60_mb_sp1600_trk100_hmt',
    'j110_mb_sp600_pusup300_trk40_hmt',
    'j110_mb_sp700_pusup350_trk50_hmt',
    'j110_mb_sp900_pusup400_trk60_hmt',
    'j110_mb_sp1100_pusup450_trk70_hmt',
    'j110_mb_sp1200_pusup500_trk80_hmt',
    'j110_mb_sp1400_pusup550_trk90_hmt',
    'j110_mb_sp1600_pusup600_trk100_hmt',
    'j110_mb_sp600_trk40_hmt',
    'j110_mb_sp700_trk50_hmt',
    'j110_mb_sp900_trk60_hmt',
    'j110_mb_sp1100_trk70_hmt',
    'j110_mb_sp1200_trk80_hmt',
    'j110_mb_sp1400_trk90_hmt',
    'j110_mb_sp1600_trk100_hmt',
    'j75_mb_sp600_pusup300_trk40_hmt',
    'j75_mb_sp700_pusup350_trk50_hmt',
    'j75_mb_sp900_pusup400_trk60_hmt',
    'j75_mb_sp1100_pusup450_trk70_hmt',
    'j75_mb_sp1200_pusup500_trk80_hmt',
    'j75_mb_sp1400_pusup550_trk90_hmt',
    'j75_mb_sp1600_pusup600_trk100_hmt',
    'j75_mb_sp600_trk40_hmt',
    'j75_mb_sp700_trk50_hmt',
    'j75_mb_sp900_trk60_hmt',
    'j75_mb_sp1100_trk70_hmt',
    'j75_mb_sp1200_trk80_hmt',
    'j75_mb_sp1400_trk90_hmt',
    'j75_mb_sp1600_trk100_hmt',
    'j85_mb_sp600_pusup300_trk40_hmt',
    'j85_mb_sp700_pusup350_trk50_hmt',
    'j85_mb_sp900_pusup400_trk60_hmt',
    'j85_mb_sp1100_pusup450_trk70_hmt',
    'j85_mb_sp1200_pusup500_trk80_hmt',
    'j85_mb_sp1400_pusup550_trk90_hmt',
    'j85_mb_sp1600_pusup600_trk100_hmt',
    'j85_mb_sp600_trk40_hmt',
    'j85_mb_sp700_trk50_hmt',
    'j85_mb_sp900_trk60_hmt',
    'j85_mb_sp1100_trk70_hmt',
    'j85_mb_sp1200_trk80_hmt',
    'j85_mb_sp1400_trk90_hmt',
    'j85_mb_sp1600_trk100_hmt',
    'mb_sp400_trk40_hmt_L1MBTS_1_1',
    'mb_sp600_trk45_hmt_L1MBTS_1_1',
    'mb_sp700_trk55_hmt_L1MBTS_1_1',
    'mb_sp900_trk60_hmt_L1MBTS_1_1',
    'mb_sp900_trk65_hmt_L1MBTS_1_1',
    'mb_sp1000_trk70_hmt_L1MBTS_1_1',
    'mb_sp1200_trk75_hmt_L1MBTS_1_1',
    'mb_sp1400_trk80_hmt_L1MBTS_1_1',
    #'mb_sp900_trk50_hmt_L1TE5.0ETA24',
    #'mb_sp900_trk60_hmt_L1TE5.0ETA24',
    #'mb_sp1000_trk70_hmt_L1TE5.0ETA24',
    #'mb_sp1200_trk80_hmt_L1TE5.0ETA24',
    #'mb_sp1200_trk90_hmt_L1TE5.0ETA24',
    #'mb_sp1200_trk100_hmt_L1TE5.0ETA24',
    #'mb_sp1800_trk110_hmt_L1TE5.0ETA24',
    #'mb_sp2100_trk120_hmt_L1TE5.0ETA24',
    #'mb_sp2300_trk130_hmt_L1TE5.0ETA24',
    #'mb_sp1000_trk70_hmt_L1TE10.0ETA24',
    #'mb_sp1200_trk80_hmt_L1TE10.0ETA24',
    #'mb_sp1400_trk90_hmt_L1TE10.0ETA24',
    #'mb_sp1600_trk100_hmt_L1TE10.0ETA24',
    #'mb_sp1800_trk110_hmt_L1TE10.0ETA24',
    #'mb_sp2100_trk120_hmt_L1TE10.0ETA24',
    #'mb_sp2300_trk130_hmt_L1TE10.0ETA24',
    #'mb_sp2500_trk140_hmt_L1TE10.0ETA24',
    #'mb_sp1200_trk80_hmt_L1TE15.0ETA24',
    #'mb_sp1400_trk90_hmt_L1TE15.0ETA24',
    #'mb_sp1600_trk100_hmt_L1TE15.0ETA24',
    #'mb_sp1800_trk110_hmt_L1TE15.0ETA24',
    #'mb_sp2100_trk120_hmt_L1TE15.0ETA24',
    #'mb_sp2300_trk130_hmt_L1TE15.0ETA24',
    #'mb_sp2500_trk140_hmt_L1TE15.0ETA24',
    #'mb_sp1400_trk90_hmt_L1TE20.0ETA24',
    #'mb_sp1600_trk100_hmt_L1TE20.0ETA24',
    #'mb_sp1800_trk110_hmt_L1TE20.0ETA24',
    #'mb_sp2100_trk120_hmt_L1TE20.0ETA24',
    #'mb_sp2300_trk130_hmt_L1TE20.0ETA24',
    #'mb_sp2500_trk140_hmt_L1TE20.0ETA24',
    #'mb_sp2700_trk150_hmt_L1TE20.0ETA24',
    #'mb_sp1600_trk100_hmt_L1TE25.0ETA24',
    #'mb_sp1800_trk110_hmt_L1TE25.0ETA24',
    #'mb_sp2100_trk120_hmt_L1TE25.0ETA24',
    #'mb_sp2300_trk130_hmt_L1TE25.0ETA24',
    #'mb_sp2500_trk140_hmt_L1TE25.0ETA24',
    #'mb_sp2700_trk150_hmt_L1TE25.0ETA24',
    #'mb_sp2900_trk160_hmt_L1TE25.0ETA24',
    #'mb_sp1800_trk110_hmt_L1TE30.0ETA24',
    #'mb_sp2100_trk120_hmt_L1TE30.0ETA24',
    #'mb_sp2300_trk130_hmt_L1TE30.0ETA24',
    #'mb_sp2500_trk140_hmt_L1TE30.0ETA24',
    #'mb_sp2700_trk150_hmt_L1TE30.0ETA24',
    #'mb_sp2900_trk160_hmt_L1TE30.0ETA24',
    #'mb_sp2300_trk130_hmt_L1TE40.0ETA24',
    #'mb_sp2500_trk140_hmt_L1TE40.0ETA24',
    #'mb_sp2700_trk150_hmt_L1TE40.0ETA24',
    #'mb_sp2900_trk160_hmt_L1TE40.0ETA24',
    'mb_sp800_hmtperf_L1TE5',
    'mb_sp800_hmtperf_L1TE10',
    'mb_sp1200_hmtperf_L1TE10',
    'mb_sp1600_hmtperf_L1TE20',
    'mb_sp2100_hmtperf_L1TE20',
    'mb_sp2100_hmtperf_L1TE30',
    'mb_sp2100_hmtperf_L1TE40',
    #'mb_sp800_hmtperf_L1TE5.0ETA24',
    #'mb_sp800_hmtperf_L1TE10.0ETA24',
    #'mb_sp1200_hmtperf_L1TE10.0ETA24',
    #'mb_sp1200_hmtperf_L1TE15.0ETA24',
    #'mb_sp1600_hmtperf_L1TE15.0ETA24',
    #'mb_sp1600_hmtperf_L1TE20.0ETA24',
    #'mb_sp2100_hmtperf_L1TE20.0ETA24',
    #'mb_sp2100_hmtperf_L1TE25.0ETA24',
    #'mb_sp2100_hmtperf_L1TE30.0ETA24',
    #'mb_sp2100_hmtperf_L1TE40.0ETA24',
    'mb_sp2_hmtperf_L1MBTS_1_1',
    'mb_sp2_hmtperf',
    'mb_sp500_hmtperf',
    'mb_sp1800_hmtperf',
    'mb_sptrk',
    'mb_sptrk_L1RD0_EMPTY',
    'mb_sptrk_L1RD0_UNPAIRED_ISO',
    'mb_sp_L1RD0_FILLED',
    'mb_sp_L1RD0_UNPAIRED_ISO',
    'mb_sp_ncb_L1RD0_UNPAIRED_ISO',
    'mb_sp_blayer_L1RD0_UNPAIRED_ISO',
    'mb_sptrk_L1RD3_FILLED',
    'mb_sptrk_costr',
    'mb_sptrk_costr_L1RD0_EMPTY',
    'mb_sptrk_noisesup',
    'mb_sptrk_noisesup_L1RD0_EMPTY',
    'mb_sptrk_noisesup_L1RD0_UNPAIRED_ISO',
    'mb_sptrk_noisesup_L1RD3_FILLED',
    'mb_sptrk_pt4_L1MBTS_1',
    'mb_sptrk_pt4_L1MBTS_1_1',
    'mb_sptrk_pt4_L1MBTS_2',
    'mb_sptrk_pt4_L1RD3_FILLED',
    'mb_sptrk_pt6_L1MBTS_1',
    'mb_sptrk_pt6_L1MBTS_1_1',
    'mb_sptrk_pt6_L1MBTS_2',
    'mb_sptrk_pt6_L1RD3_FILLED',
    'mb_sptrk_pt8_L1MBTS_1',
    'mb_sptrk_pt8_L1MBTS_1_1',
    'mb_sptrk_pt8_L1MBTS_2',
    'mb_sptrk_pt8_L1RD3_FILLED',
    'mb_sptrk_vetombts2in_L1ZDC_AND',
    'mb_sptrk_vetombts2in_L1ZDC_A_C',
    'mb_sptrk_trk80_L1MBTS_2',
    'mb_sptrk_pt2_L1MBTS_2',
#    'mb_sptrk_L1MBTS_1',
##     'mb_sptrk_L1MBTS_1_VTE2',
##     'mb_sptrk_L1MBTS_1_VTE3',
##     'mb_sptrk_L1MBTS_1_VTE4',
##     'mb_sptrk_L1MBTS_1_VTE5',
##     'mb_sptrk_L1MBTS_1_VTE10',
##     'mb_sptrk_L1MBTS_2_VTE2',
##     'mb_sptrk_L1MBTS_2_VTE3',
##     'mb_sptrk_L1MBTS_2_VTE4',
##     'mb_sptrk_L1MBTS_2_VTE5',
##     'mb_sptrk_L1MBTS_2_VTE10',
##     'mb_sptrk_L1VTE2',
##     'mb_sptrk_L1VTE3',
##     'mb_sptrk_L1VTE4',
##     'mb_sptrk_L1VTE5',
##     'mb_sptrk_L1VTE10',   
    
    # minBias streamer
    'noalg_mb_L1LUCID',
    'noalg_mb_L1LUCID_EMPTY',
    'noalg_mb_L1LUCID_UNPAIRED_ISO',
    'noalg_mb_L1MBTS_1',
    'noalg_mb_L1MBTS_1_1',
    'noalg_mb_L1MBTS_1_1_EMPTY',
    'noalg_mb_L1MBTS_1_1_UNPAIRED_ISO',
    'noalg_mb_L1MBTS_1_EMPTY',
    'noalg_mb_L1MBTS_1_UNPAIRED_ISO',
    'noalg_mb_L1MBTS_2',
    'noalg_mb_L1MBTS_2_EMPTY',
    'noalg_mb_L1MBTS_2_UNPAIRED_ISO',
    'noalg_mb_L1RD0_EMPTY',
    'noalg_mb_L1RD0_FILLED',
    'noalg_mb_L1RD0_UNPAIRED_ISO',
    'noalg_mb_L1RD1_FILLED',
    'noalg_mb_L1RD2_EMPTY',
    'noalg_mb_L1RD2_FILLED',
    'noalg_mb_L1RD3_EMPTY',
    'noalg_mb_L1RD3_FILLED',
 ##    'noalg_mb_L1TE2',
##     'noalg_mb_L1TE3',
##     'noalg_mb_L1TE4',
    'noalg_mb_L1TE5',
    #'noalg_mb_L1TE5.0ETA24',
    'noalg_mb_L1TE10',
    #'noalg_mb_L1TE10.0ETA24',
    'noalg_mb_L1TE20',
    #'noalg_mb_L1TE20.0ETA24',
    'noalg_mb_L1TE30',
    #'noalg_mb_L1TE30.0ETA24',
    'noalg_mb_L1TE40',
    #'noalg_mb_L1TE40.0ETA24',
    'noalg_mb_L1TE50',
    #'noalg_mb_L1TE60',
    #'noalg_mb_L1TE70',
    'noalg_mb_L1TE15',
    'noalg_mb_L1TE25',
    #'noalg_mb_L1TE70.0ETA24',
    'noalg_mb_L1ZDC_A',
    'noalg_mb_L1ZDC_C',
    'noalg_mb_L1ZDC_A_C',
    'noalg_mb_L1ZDC_AND',
    'zdcpeb_L1ZDC_A',  
    'zdcpeb_L1ZDC_C',   
    'zdcpeb_L1ZDC_AND', 
    'zdcpeb_L1ZDC_A_C',
    'timeburner',
#    'mb_sp_L1MBTS_1_OVERLAY',
#    'mb_sptrk_L1MBTS_1_OVERLAY',
#    'noalg_L1MBTS_1_OVERLAY',
#    'noalg_L1TE20_OVERLAY',
    ]
Prescales.HLTPrescales_tight_physics_prescale.update(dict(map(None,chain_list,len(chain_list)*[ [-1, 0,-1] ])))
######################################################
