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
    # ['chainName',  'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep], OPTIONAL: [mergingStrategy, offset,[mergingOrder] ]], topoStartsFrom = False
    #---------------------------------------------------------------------

    #---------------------------------------------------------------------
    # if it's needed to temporary remove almost all the chains from the menu
    # be aware that it is necessary to leave at least one chain in the muon slice
    # otherwise athenaHLT will seg-fault 
    #---------------------------------------------------------------------

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.TestSlice.signatures = []

    TriggerFlags.AFPSlice.signatures = [

        ['afp_jetexc_L1J50',  'L1_J50',  [],  [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['afp_jetexc_L1J75',  'L1_J75',  [],  [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['afp_jetexc_L1J100',  'L1_J100',  [],  [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ]

    TriggerFlags.MuonSlice.signatures = [
        ['mu26_ivarmedium',	     'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream, 'express'], ['Legacy:Primary','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu28_ivarmedium',	     'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Legacy:Backup','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu80',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Legacy:Backup','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu50',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Legacy:Primary','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu60',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Legacy:Backup','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu60_0eta105_msonly',    'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Legacy:Primary','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu80_msonly_3layersEC',  'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Legacy:Primary','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20_ivarmedium_L1MU10', 'L1_MU10',           [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu26',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Legacy:Rerun','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu22',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Legacy:Support','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Legacy:Support','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24',                   'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20_msonly',            'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Legacy:Support','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu14',                   'L1_MU10',           [], [PhysicsStream], ['Legacy:Support','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu14_ivarloose',         'L1_MU10',           [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu6',                    'L1_MU6',            [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu6_msonly',             'L1_MU6',            [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu4',                    'L1_MU4',            [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_idperf',            'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream, 'express'], ['Legacy:Support','RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
        ['mu20_idperf',            'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream, 'express'], ['Legacy:Support','RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
        ['mu40_idperf',            'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream, 'express'], ['Legacy:Support','RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
        ['mu10_idperf',            'L1_MU10',           [], [PhysicsStream], ['Legacy:Support','RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
        ['mu6_idperf',             'L1_MU6',            [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
        ['mu4_idperf',             'L1_MU4',            [], [PhysicsStream], ['Legacy:Support','RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],

        ['mu4_nomucomb',                   'L1_MU4',           [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu6_nomucomb',                   'L1_MU6',           [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10_nomucomb',                   'L1_MU10',           [], [PhysicsStream], ['Legacy:Support','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu11_nomucomb',                   'L1_MU11',           [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleMuon', 'BW:Muon'], -1],

        #ATR-17539
        ['mu20_ivarmedium_mu8noL1',    'L1_MU20MU21',  ['L1_MU20',''], [PhysicsStream], ['Legacy:Primary','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_ivarmedium','mu8noL1']]],

        #ATR-16981
        ['mu24_ivarmedium',	     'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream, 'express'], ['Legacy:End-of-fill','RATE:SingleMuon', 'BW:Muon'], -1],

        #ATR-17448
        ['mu26_ivartight',          'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Legacy:Backup','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu28_ivartight',          'L1_MU20MU21',   ['L1_MU20'], [PhysicsStream], ['Legacy:Backup','RATE:SingleMuon', 'BW:Muon'], -1],


        # multi muons
        ['2mu14',                  'L1_2MU10',          [], [PhysicsStream, 'express'], ['Legacy:Primary','RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu15_L12MU10',          'L1_2MU10',          [], [PhysicsStream], ['Legacy:Backup','RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu6',                   'L1_3MU6',           [], [PhysicsStream], ['Legacy:Primary','RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu6_msonly',            'L1_3MU6',           [], [PhysicsStream], ['Legacy:Primary','RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu8_msonly',            'L1_3MU6',           [], [PhysicsStream], ['Legacy:Backup','RATE:MultiMuon', 'BW:Muon'], -1],
        ['mu24_mu8noL1',           'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream, 'express'], ['Legacy:Backup','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','mu8noL1']]],
        ['mu24_mu10noL1',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Legacy:Backup','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','mu10noL1']]],
        ['mu24_mu12noL1',         'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Legacy:Backup','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','mu12noL1']]],
        ['mu24_2mu4noL1',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Legacy:Backup','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','2mu4noL1']]],
        ['mu26_mu8noL1',           'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Legacy:Backup','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu26','mu8noL1']]],
        ['mu26_mu10noL1',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Legacy:Backup','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu26','mu10noL1']]],
        ['mu28_mu8noL1',           'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Legacy:Backup','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu28','mu8noL1']]],
        ['mu22_mu8noL1',           'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Legacy:Primary','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','mu8noL1']]],
        ['mu22_2mu4noL1',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Legacy:Backup','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','2mu4noL1']]],
        ['mu20_2mu4noL1',          'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Legacy:Primary','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','2mu4noL1']]],
        ['mu20_2mu2noL1_JpsimumuFS', 'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Legacy:Support',RATE_BMultiMuonTag,'BW:Muon'], -1,['serial',-1,['mu20','2mu2noL1_JpsimumuFS']]],  # OI - who owns these triggers? Bphys?
        ['mu20_2mu4_JpsimumuL2',     'L1_MU20MU21', ['L1_MU20','L1_2MU4'], [PhysicsStream], ['Legacy:End-of-fill',RATE_BMultiMuonTag,'BW:Muon'], -1,['serial',-1,['2mu4_JpsimumuL2','mu20']]], # OI - who owns these triggers?
       
	  # multi-muon TagandProbe chains (ATR-17625)
	  ['mu22_mu8noL1_TagandProbe', 'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Legacy:Primary','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','mu8noL1']]],

        # Primary (multi muon chains)
        ['4mu4',                   'L1_4MU4', ['L1_4MU4',''], [BMultiMuonStream], ['Legacy:Primary',RATE_BMultiMuonTag, BW_BphysTag], -1,['serial',-1,['4mu4','']]],

		#ATR-15246
        ['mu22_mu8noL1_mu6noL1',   'L1_MU20MU21',       ['L1_MU20','',''], [PhysicsStream], ['Legacy:Backup','RATE:MultiMuon', 'BW:Muon'],  -1,['serial',-1,['mu22','mu8noL1','mu6noL1']]],

         ### ATR-14377: Emergency additional class of chains instead of mu20_msonly_mu10noL1_msonly_nscan05_noComb*
        ['mu20_msonly_mu15noL1_msonly_nscan05_noComb',   'L1_MU20MU21',   ['L1_MU20','L2_mu20_msonly'], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu15noL1_msonly_nscan05_noComb']]],


        ['mu20_ivarmedium_L1MU10_2J20', 'L1_MU10_2J20', [], [PhysicsStream], ['Legacy:Primary','RATE:MuonJet', 'BW:Muon','BW:Jet'], -1],                
        ['mu20_ivarmedium_L1MU10_2J15_J20', 'L1_MU10_2J15_J20', [], [PhysicsStream], ['Legacy:Primary','RATE:MuonJet', 'BW:Muon','BW:Jet'], -1],  

        #nscan05 triggers rejecting CB muons
        ['mu20_msonly_mu6noL1_msonly_nscan05', 'L1_MU20MU21',      ['L1_MU20','L2_mu20_msonly'], [PhysicsStream], ['Legacy:Support','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly','mu6noL1_msonly_nscan05']]],
        ['mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_XE30', 'L1_MU20_XE30', ['L1_MU20_XE30','L2_mu20_msonly_iloosems'], [PhysicsStream], ['Legacy:Primary','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_iloosems','mu6noL1_msonly_nscan05']]],
        ['mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU20_J40', 'L1_MU20_J40',   ['L1_MU20_J40','L2_mu20_msonly_iloosems'], [PhysicsStream], ['Legacy:Primary','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_iloosems','mu6noL1_msonly_nscan05']]],
        ['mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU6_EMPTY', 'L1_MU6_EMPTY',   ['L1_MU20','L2_mu20_msonly_iloosems'], [PhysicsStream], ['Legacy:Primary','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_iloosems','mu6noL1_msonly_nscan05']]],
        ['mu20_msonly_iloosems_mu6noL1_msonly_nscan05_L1MU4_UNPAIRED_ISO', 'L1_MU4_UNPAIRED_ISO',   ['L1_MU20','L2_mu20_msonly_iloosems'], [PhysicsStream], ['Legacy:Primary','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20_msonly_iloosems','mu6noL1_msonly_nscan05']]],
        ['mu6_dRl1_mu20_msonly_iloosems_mu6noL1_dRl1_msonly','L1_MU11_2MU6', [['L1_MU6','L1_MU6'],'L1_MU20',''], [PhysicsStream], ['Legacy:Primary','RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu6_dRl1','mu20_msonly_iloosems','mu6noL1_dRl1_msonly']]],

        ['mu10',                   'L1_MU10',           [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10_mgonly_L1LATE-MU10_J50',          'L1_LATE-MU10_J50',           [], [PhysicsStream], ['Legacy:Primary','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10_mgonly_L1LATE-MU10_XE50',         'L1_LATE-MU10_XE50',          [], [PhysicsStream], ['Legacy:Backup','RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10_mgonly_L1LATE-MU10_XE40',         'L1_LATE-MU10_XE40',          [], [PhysicsStream], ['Legacy:Primary','RATE:SingleMuon', 'BW:Muon'], -1],

			 ]


    if TriggerFlags.doFTK():
        TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [
            ]


    TriggerFlags.JetSlice.signatures = [   

        #####
        ######################### Performance-style chains
        #####

        # Jet data scouting stream chains
        # These record only the trigger jets, but for every event passing the L1 trigger
        ['j0_perf_ds1_L1J50',      'L1_J50',  [], ['DataScouting_05_Jets'], ['Legacy:Support','RATE:Jets_DS', 'BW:Jet'], -1],
        ['j0_perf_ds1_L1J100',     'L1_J100', [], ['DataScouting_05_Jets'], ['Legacy:Primary','RATE:Jets_DS', 'BW:Jet'], -1],

        #Low-mu run triggers
        ['j0_perf_ds1_L1J40',      'L1_J40',  [], ['DataScouting_05_Jets'], ['Legacy:Support','RATE:Jets_DS', 'BW:Jet'], -1],

        # Performance chains

        #ATR-17198: moved from MC to physics
        ['j0_perf_ds1_L1J50_DETA20-J50J',     'L1_J50_DETA20-J50J',  [], ['DataScouting_05_Jets'], ['Legacy:End-of-fill','RATE:Jets_DS', 'BW:Jet'], -1],

        #####
        ######################### Single-jet small-R trigger chains
        #####
        
        # Standard central jet triggers
        ['j25',                         'L1_RD0_FILLED', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j35',                         'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j45',                         'L1_J15', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j45_L1RD0_FILLED',            'L1_RD0_FILLED', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j60',                         'L1_J20',  [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j60_L1RD0_FILLED',            'L1_RD0_FILLED', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j85',                         'L1_J20',  [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j110',                        'L1_J30',  [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j175',                        'L1_J50',  [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j260',                        'L1_J75', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j360',                        'L1_J100', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j400',                        'L1_J100', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleJet', 'BW:Jet'], -1],
        ['j420',                        'L1_J100', [], [PhysicsStream,'express'], ['Legacy:Primary','RATE:SingleJet', 'BW:Jet'], -1],
        ['j440',                        'L1_J120', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j450',                        'L1_J120', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j460',                        'L1_J120', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j480',                        'L1_J120', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j500',                        'L1_J120', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j520',                        'L1_J120', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],

        # Standard gsc central jet triggers (primaries)
        ['j225_gsc400_boffperf_split',  'L1_J100', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleJet', 'BW:Jet'], -1],
        ['j225_gsc420_boffperf_split',  'L1_J100', [], [PhysicsStream,'express'], ['Legacy:Primary','RATE:SingleJet', 'BW:Jet'], -1],
        ['j225_gsc440_boffperf_split',  'L1_J120', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j225_gsc450_boffperf_split',  'L1_J120', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j225_gsc460_boffperf_split',  'L1_J120', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j225_gsc480_boffperf_split',  'L1_J120', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j225_gsc500_boffperf_split',  'L1_J120', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],

        # Alternative calibration central jet triggers
        ['j35_subjes',                  'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_jes',                     'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_sub',                     'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_nojcalib',                'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_lcw',                     'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_lcw_subjes',              'L1_RD0_FILLED', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_lcw_jes',                 'L1_RD0_FILLED', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_lcw_sub',                 'L1_RD0_FILLED', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_lcw_nojcalib',            'L1_RD0_FILLED', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j450_subjes',                 'L1_J120', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j450_jes',                    'L1_J120', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j450_sub',                    'L1_J120', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j450_nojcalib',               'L1_J120', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j450_lcw',                    'L1_J120', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j450_lcw_subjes',             'L1_J120', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j450_lcw_jes',                'L1_J120', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j450_lcw_sub',                'L1_J120', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j450_lcw_nojcalib',           'L1_J120', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],

        # Standard forward jet triggers
        ['j15_320eta490',               'L1_RD0_FILLED',  [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j25_320eta490',               'L1_RD0_FILLED',  [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j35_320eta490',               'L1_RD0_FILLED',  [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j45_320eta490',               'L1_J15.31ETA49', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j60_320eta490',               'L1_J20.31ETA49', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j85_320eta490',               'L1_J20.31ETA49', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j110_320eta490',              'L1_J30.31ETA49', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j175_320eta490',              'L1_J50.31ETA49', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j260_320eta490',              'L1_J75.31ETA49', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleJet',  'BW:Jet'], -1],

        # Alternative calibration forward jet triggers
        ['j35_320eta490_subjes',        'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_320eta490_jes',           'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_320eta490_sub',           'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_320eta490_nojcalib',      'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_320eta490_lcw',           'L1_RD0_FILLED', [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_320eta490_lcw_subjes',    'L1_RD0_FILLED', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_320eta490_lcw_jes',       'L1_RD0_FILLED', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_320eta490_lcw_sub',       'L1_RD0_FILLED', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j35_320eta490_lcw_nojcalib',  'L1_RD0_FILLED', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j260_320eta490_subjes',       'L1_J75.31ETA49', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j260_320eta490_jes',          'L1_J75.31ETA49', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j260_320eta490_sub',          'L1_J75.31ETA49', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleJet',  'BW:Jet'], -1],
        ['j260_320eta490_nojcalib',     'L1_J75.31ETA49', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleJet',  'BW:Jet'], -1],
        ['j260_320eta490_lcw',          'L1_J75.31ETA49', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j260_320eta490_lcw_subjes',   'L1_J75.31ETA49', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j260_320eta490_lcw_jes',      'L1_J75.31ETA49', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet',  'BW:Jet'], -1],
        ['j260_320eta490_lcw_sub',      'L1_J75.31ETA49', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleJet',  'BW:Jet'], -1],
        ['j260_320eta490_lcw_nojcalib', 'L1_J75.31ETA49', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleJet',  'BW:Jet'], -1],
        
        
        
        #####
        ######################### Single-jet large-R trigger chains
        #####

        # Standard topocluster large-R jet triggers
        ['j110_a10_lcw_subjes_L1J30',       'L1_J30', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j175_a10_lcw_subjes_L1J50',       'L1_J50', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j260_a10_lcw_subjes_L1J75',       'L1_J75', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j440_a10_lcw_subjes_L1J100',      'L1_J100', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10_lcw_subjes_L1J100',      'L1_J100', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10_lcw_subjes_L1J100',      'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j500_a10_lcw_subjes_L1J100',      'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j520_a10_lcw_subjes_L1J100',      'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j540_a10_lcw_subjes_L1J100',      'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],

        # Alternative calibration topocluster large-R jet triggers
        ['j260_a10_sub_L1J75',              'L1_J75', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j260_a10_nojcalib_L1J75',         'L1_J75', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j260_a10_lcw_sub_L1J75',          'L1_J75', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j260_a10_lcw_nojcalib_L1J75',     'L1_J75', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10_sub_L1J100',             'L1_J100', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10_nojcalib_L1J100',        'L1_J100', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10_lcw_sub_L1J100',         'L1_J100', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10_lcw_nojcalib_L1J100',    'L1_J100', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],

        # L1topo standard topocluster large-R jet triggers
        ['j440_a10_lcw_subjes_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10_lcw_subjes_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream,'express'], ['Legacy:Primary','RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10_lcw_subjes_L1SC111',             'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j500_a10_lcw_subjes_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j520_a10_lcw_subjes_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j540_a10_lcw_subjes_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],



        # Standard reclustered large-R jet triggers
        ['j260_a10r_L1J75',                 'L1_J75', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j440_a10r_L1J100',                'L1_J100', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10r_L1J100',                'L1_J100', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10r_L1J100',                'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j500_a10r_L1J100',                'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j520_a10r_L1J100',                'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j540_a10r_L1J100',                'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        
        # L1topo standard reclustered large-R jet triggers
        ['j440_a10r_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10r_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream,'express'], ['Legacy:Primary','RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10r_L1SC111',               'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j500_a10r_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j520_a10r_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j540_a10r_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],



        # Standard trimmed large-R jet triggers
        ['j260_a10t_lcw_jes_L1J75',         'L1_J75', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j420_a10t_lcw_jes_L1J100',        'L1_J100', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j460_a10t_lcw_jes_L1J100',        'L1_J100', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10t_lcw_jes_L1J100',        'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j500_a10t_lcw_jes_L1J100',        'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j520_a10t_lcw_jes_L1J100',        'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j540_a10t_lcw_jes_L1J100',        'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],

        # Alternative calibration trimmed large-R jet triggers
        ['j260_a10t_lcw_nojcalib_L1J75',    'L1_J75', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10t_lcw_nojcalib_L1J100',   'L1_J100', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],

        # L1topo standard trimmed large-R jet triggers
        ['j460_a10t_lcw_jes_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream,'express'], ['Legacy:Primary','RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10t_lcw_jes_L1SC111',               'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j500_a10t_lcw_jes_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j520_a10t_lcw_jes_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j540_a10t_lcw_jes_L1SC111', 'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],


        # Trimmed + mass-cut single-jet triggers
        ['j390_a10t_lcw_jes_30smcINF_L1J100',               'L1_J100', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleJet', 'BW:Jet'], -1],
        ['j420_a10t_lcw_jes_30smcINF_L1J100',               'L1_J100', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleJet', 'BW:Jet'], -1],
        ['j440_a10t_lcw_jes_30smcINF_L1J100',               'L1_J100', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleJet', 'BW:Jet'], -1],

        ['j390_a10t_lcw_jes_35smcINF_L1J100',               'L1_J100', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleJet', 'BW:Jet'], -1],
        ['j420_a10t_lcw_jes_35smcINF_L1J100',               'L1_J100', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleJet', 'BW:Jet'], -1],
        ['j420_a10t_lcw_jes_35smcINF_L1SC111',              'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleJet', 'BW:Jet'], -1],
        ['j440_a10t_lcw_jes_35smcINF_L1J100',               'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j440_a10t_lcw_jes_35smcINF_L1SC111',              'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],

        ['j390_a10t_lcw_jes_40smcINF_L1J100',               'L1_J100', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleJet', 'BW:Jet'], -1],
        ['j420_a10t_lcw_jes_40smcINF_L1J100',               'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j440_a10t_lcw_jes_40smcINF_L1J100',               'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],


        # Trimmed + mass-cut single-jet support triggers (for mass efficiency bootstrap)
        ['j390_a10t_lcw_jes_L1J100',                        'L1_J100', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        # j420 L1J100 is above
        ['j420_a10t_lcw_jes_L1SC111',                       'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        # j440 L1J100 is above

        # Trimmed + mass-cut di-jet triggers
        ['2j330_a10t_lcw_jes_30smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleJet', 'BW:Jet'], -1],

        ['2j330_a10t_lcw_jes_35smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleJet', 'BW:Jet'], -1],
        ['2j330_a10t_lcw_jes_35smcINF_L1SC111',             'L1_SC111-CJ15', [], [PhysicsStream,'express'], ['Legacy:Primary','RATE:SingleJet', 'BW:Jet'], -1],
        ['2j340_a10t_lcw_jes_35smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['2j340_a10t_lcw_jes_35smcINF_L1SC111',             'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['2j350_a10t_lcw_jes_35smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['2j350_a10t_lcw_jes_35smcINF_L1SC111',             'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['2j360_a10t_lcw_jes_35smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['2j370_a10t_lcw_jes_35smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['2j380_a10t_lcw_jes_35smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],

        ['2j330_a10t_lcw_jes_40smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['2j340_a10t_lcw_jes_40smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['2j350_a10t_lcw_jes_40smcINF_L1J100',              'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],

        # Trimmed + mass-cut asymmetric di-jet triggers (for rate optimization)



        ['j325_a10t_lcw_jes_60smcINF_j325_a10t_lcw_jes_L1SC111',   'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleJet', 'BW:Jet'], -1],
        ['j340_a10t_lcw_jes_60smcINF_j340_a10t_lcw_jes_L1SC111',   'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleJet', 'BW:Jet'], -1],
        ['j360_a10t_lcw_jes_60smcINF_j360_a10t_lcw_jes_L1SC111',   'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleJet', 'BW:Jet'], -1],


        # ATR-17691 Trimmed large-R jet trigger analysis specific chains with mass cuts
        ['j370_a10t_lcw_jes_35smcINF_j370_a10t_lcw_jes_L1SC111',   'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleJet', 'BW:Jet'], -1],
        ['j370_a10t_lcw_jes_40smcINF_j370_a10t_lcw_jes_L1SC111',   'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j370_a10t_lcw_jes_50smcINF_j370_a10t_lcw_jes_L1SC111',   'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j370_a10t_lcw_jes_60smcINF_j370_a10t_lcw_jes_L1SC111',   'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j380_a10t_lcw_jes_40smcINF_j380_a10t_lcw_jes_L1SC111',   'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],
        ['j380_a10t_lcw_jes_50smcINF_j380_a10t_lcw_jes_L1SC111',   'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],


        # Trimmed + mass-cut di-jet support triggers (for mass efficiency bootstrap)
        ['2j330_a10t_lcw_jes_L1SC111',                      'L1_SC111-CJ15', [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],

        #####
        ######################### Multi-jet small-R trigger chains
        #####

        # 3-jet single threshold triggers
        ['3j160',                       'L1_J75', [], [PhysicsStream], ['Legacy:Support','RATE:MultiJet',  'BW:Jet'], -1],
        ['3j200',                       'L1_J100', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['3j225',                       'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['3j250',                       'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],

        # 4-jet single threshold triggers
        ['4j25',                        'L1_RD0_FILLED', [], [PhysicsStream], ['Legacy:Support','RATE:MultiJet',  'BW:Jet'], -1],
        ['4j90',                        'L1_3J50', [], [PhysicsStream], ['Legacy:Support','RATE:MultiJet',  'BW:Jet'], -1],
        ['4j120',                       'L1_3J50', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['4j130',                       'L1_3J50', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],

        # 4-jet gsc single threshold triggers
        ['4j85_gsc115_boffperf_split',  'L1_3J50', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['4j85_gsc120_boffperf_split',  'L1_3J50', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['4j85_gsc130_boffperf_split',  'L1_3J50', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],

        # 5-jet single threshold triggers
        ['5j25',                        'L1_RD0_FILLED', [], [PhysicsStream], ['Legacy:Support','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j45',                        'L1_4J15', [], [PhysicsStream], ['Legacy:Support','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j70_L14J15',                 'L1_4J15', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j70_L14J150ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j70',                        'L1_4J20', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85_L14J15',                 'L1_4J15', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85_L14J150ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85',                        'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j90_L14J150ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j90',                        'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j100_L14J150ETA25',          'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j100',                       'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j90_L14J15',                 'L1_4J15', [], [PhysicsStream], ['Legacy:Backup', 'RATE:MultiJet',  'BW:Jet'], -1],
        ['5j100_L14J15',                'L1_4J15', [], [PhysicsStream], ['Legacy:Backup', 'RATE:MultiJet',  'BW:Jet'], -1],
        
        # 5-jet gsc single threshold triggers
        ['5j50_gsc70_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j50_gsc70_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j55_gsc75_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j55_gsc75_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j60_gsc85_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j60_gsc85_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j60_gsc90_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j60_gsc90_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85_gsc100_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85_gsc100_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j60_gsc85_boffperf_split_L14J15',         'L1_4J15', [], [PhysicsStream], ['Legacy:Primary', 'RATE:MultiJet',  'BW:Jet'], -1],
        ['5j60_gsc90_boffperf_split_L14J15',         'L1_4J15', [], [PhysicsStream], ['Legacy:Backup', 'RATE:MultiJet',  'BW:Jet'], -1],
        
        # 5-jet single threshold restricted eta range triggers
        ['5j65_0eta240_L14J15',         'L1_4J15', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j65_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j65_0eta240',                'L1_4J20', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j70_0eta240_L14J15',         'L1_4J15', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j70_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j70_0eta240',                'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j75_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j75_0eta240',                'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85_0eta240',                'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j75_0eta240_L14J15',         'L1_4J15', [], [PhysicsStream], ['Legacy:Backup', 'RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85_0eta240_L14J15',         'L1_4J15', [], [PhysicsStream], ['Legacy:Backup', 'RATE:MultiJet',  'BW:Jet'], -1],
 
        # 5-jet gsc single threshold restricted eta range triggers
        ['5j50_gsc65_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j50_gsc65_boffperf_split_0eta240',               'L1_4J20', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j50_gsc70_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j50_gsc70_boffperf_split_0eta240',               'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j55_gsc75_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j55_gsc75_boffperf_split_0eta240',               'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['5j50_gsc70_boffperf_split_0eta240_L14J15', 'L1_4J15', [], [PhysicsStream], ['Legacy:Primary', 'RATE:MultiJet',  'BW:Jet'], -1],
        ['5j55_gsc75_boffperf_split_0eta240_L14J15', 'L1_4J15', [], [PhysicsStream], ['Legacy:Backup', 'RATE:MultiJet',  'BW:Jet'], -1],
        
        # 6-jet single threshold triggers
        ['6j25',                        'L1_RD0_FILLED', [], [PhysicsStream], ['Legacy:Support','RATE:MultiJet',  'BW:Jet'], -1],        
        ['6j45',                        'L1_4J15', [], [PhysicsStream,'express'], ['Legacy:Support','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_L14J15',                 'L1_4J15', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_L14J150ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60',                        'L1_4J20', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j70_L14J15',                 'L1_4J15', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j70_L14J150ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j70',                        'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j85_L14J150ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j85',                        'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j85_L14J15',                 'L1_4J15', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        
        # 6-jet gsc single threshold triggers
        ['6j45_gsc60_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc60_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc60_boffperf_split_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_gsc65_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_gsc65_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_gsc65_boffperf_split_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_gsc70_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_gsc70_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_gsc70_boffperf_split_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_gsc85_boffperf_split',               'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_gsc85_boffperf_split_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_gsc85_boffperf_split_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_gsc70_boffperf_split_L14J15', 'L1_4J15', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_gsc85_boffperf_split_L14J15', 'L1_4J15', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],

        # 6-jet single threshold restricted eta range triggers
        ['6j45_0eta240',                'L1_4J15', [], [PhysicsStream,'express'], ['Legacy:Support','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_0eta240_L14J15',         'L1_4J15', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_0eta240_L14J20',         'L1_4J20', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_0eta240_L15J150ETA25',   'L1_5J15.0ETA25', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j55_0eta240_L14J15',         'L1_4J15', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j55_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j55_0eta240_L14J20',         'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j55_0eta240_L15J150ETA25',   'L1_5J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_0eta240_L14J20',         'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_0eta240_L15J150ETA25',   'L1_5J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j60_0eta240_L14J15',              'L1_4J15', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        
        # 6-jet gsc single threshold restricted eta range triggers
        ['6j35_gsc45_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Support','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j35_gsc50_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j35_gsc50_boffperf_split_0eta240_L14J20',        'L1_4J20', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j35_gsc50_boffperf_split_0eta240_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc55_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream,'express'], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc55_boffperf_split_0eta240_L14J20',        'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc55_boffperf_split_0eta240_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc60_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc60_boffperf_split_0eta240_L14J20',        'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc60_boffperf_split_0eta240_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc55_boffperf_split_0eta240_L14J15', 'L1_4J15', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_gsc60_boffperf_split_0eta240_L14J15', 'L1_4J15', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],

        # 6-jet single threshold alternative calibration triggers
        ['6j45_subjes',                 'L1_4J15', [], [PhysicsStream], ['Legacy:Support','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_jes',                    'L1_4J15', [], [PhysicsStream], ['Legacy:Support','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_sub',                    'L1_4J15', [], [PhysicsStream], ['Legacy:Support','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_nojcalib',               'L1_4J15', [], [PhysicsStream], ['Legacy:Support','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_lcw',                    'L1_4J15', [], [PhysicsStream], ['Legacy:Support','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_lcw_subjes',             'L1_4J15', [], [PhysicsStream], ['Legacy:Support','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_lcw_jes',                'L1_4J15', [], [PhysicsStream], ['Legacy:Support','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_lcw_sub',                'L1_4J15', [], [PhysicsStream], ['Legacy:Support','RATE:MultiJet',  'BW:Jet'], -1],
        ['6j45_lcw_nojcalib',           'L1_4J15', [], [PhysicsStream], ['Legacy:Support','RATE:MultiJet',  'BW:Jet'], -1],

        # 7-jet single-threshold triggers
        ['7j25',                        'L1_RD0_FILLED', [], [PhysicsStream], ['Legacy:Support','RATE:MultiJet',  'BW:Jet'], -1],      
        ['7j45_L14J15',                 'L1_4J15', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j45_L14J150ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j45_L14J20',                 'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j45',                        'L1_6J15', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j50_L14J150ETA25',           'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j50_L14J20',                 'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j50',                        'L1_6J15', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j50_L14J15',                              'L1_4J15', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        
        # 7-jet gsc single threshold triggers
        ['7j35_gsc45_boffperf_split_L14J150ETA25',          'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc45_boffperf_split_L14J20',                'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc45_boffperf_split_L15J150ETA25',          'L1_5J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_L14J150ETA25',          'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_L14J20',                'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_L15J150ETA25',          'L1_5J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc45_boffperf_split_L14J15',         'L1_4J15', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_L14J15',         'L1_4J15', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        
        # 7-jet single-threshold restricted eta range triggers
        ['7j45_0eta240_L14J150ETA25',   'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j45_0eta240_L14J20',         'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j45_0eta240_L15J150ETA25',   'L1_5J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j45_0eta240',                'L1_6J15', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j45_0eta240_L14J15',                      'L1_4J15', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        
        # 7-jet gsc single threshold restricted eta range triggers
        ['7j35_gsc45_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc45_boffperf_split_0eta240_L14J20',        'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc45_boffperf_split_0eta240_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_0eta240_L14J150ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_0eta240_L14J20',        'L1_4J20', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_0eta240_L15J150ETA25',  'L1_5J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc45_boffperf_split_0eta240_L14J15', 'L1_4J15', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['7j35_gsc50_boffperf_split_0eta240_L14J15', 'L1_4J15', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],

        # 10-jet single-threshold triggers
        ['10j40_L14J15',                'L1_4J15' , [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet', 'BW:Jet'], -1],
        ['10j40_L14J150ETA25',          'L1_4J15.0ETA25' , [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet', 'BW:Jet'], -1],
        ['10j40_L14J20',                'L1_4J20' , [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet', 'BW:Jet'], -1],
        ['10j40_L16J15',                'L1_6J15' , [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet', 'BW:Jet'], -1],


        #####
        ######################### HT trigger chains
        #####

        # HT triggers
        ['ht700_L1J75',     'L1_J75', [], [PhysicsStream], ['Legacy:Support','RATE:MultiJet', 'BW:Jet'], -1], 
        ['ht1000_L1J100',   'L1_J100', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet', 'BW:Jet'], -1],
        ['ht1200_L1J100',   'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet', 'BW:Jet'], -1],
        ['ht1400_L1J100',   'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet', 'BW:Jet'], -1],

        #####
        ######################### Analysis-specific jet trigger chains
        #####


        # Dijet invariant mass range plus y* cut triggers, mainly aimed at the dijet trigger-level analysis
        ['j0_1i2c300m500TLA',               'L1_J100',  [], [PhysicsStream], ['Legacy:Support','RATE:MultiJet', 'BW:Jet'], -1],
        ['j0_0i1c500m900TLA',               'L1_J100',  [], [PhysicsStream], ['Legacy:Support','RATE:MultiJet', 'BW:Jet'], -1],
        ['j0_1i2c200m8000TLA',              'L1_J100',  [], [PhysicsStream], ['Legacy:Support','RATE:MultiJet', 'BW:Jet'], -1],

        ['2j220_j120',                      'L1_J100', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['2j250_j120',                      'L1_J100', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],
        ['2j275_j140',                      'L1_J100', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiJet',  'BW:Jet'], -1],

        # Analysis-driven multijet triggers
        ['j80_0eta240_2j60_320eta490',      'L1_J40.0ETA25_2J15.31ETA49', [], [PhysicsStream], ['Legacy:Support','RATE:MultiJet', 'BW:Jet'], -1],
        ['2j225_gsc250_boffperf_split_0eta240_j85_gsc120_boffperf_split_0eta240',      'L1_J100', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet',  'BW:Jet'], -1],
        ['2j225_gsc250_boffperf_split_0eta240_j85_gsc140_boffperf_split_0eta240',      'L1_J100', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet',  'BW:Jet'], -1],
        ['j80_0eta240_j60_j45_320eta490',   'L1_J40.0ETA25_2J25_J20.31ETA49', [], [PhysicsStream], ['Legacy:Support','RATE:MultiBJet', 'BW:BJet'], -1],

        # ATR-15044: 2017 VBF Hbb
        ['ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF','L1_HT150-J20s5.ETA31_MJJ-400-CF', [], [PhysicsStream], ['Legacy:Support','RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,[  'ht300', '2j40_0eta490_invm700']]],
        ['ht300_2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF_AND_2j35_gsc45_bmv2c1070_split','L1_HT150-J20s5.ETA31_MJJ-400-CF', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiBJet',  'BW:BJet', 'BW:Jet'],-1,['serial',-1,[  'ht300', '2j40_0eta490_invm700_L1HT150-J20s5.ETA31_MJJ-400-CF_AND','2j35_gsc45_bmv2c1070_split']]],

        #####
        ######################### Specialty jet trigger chains (mostly for performance studies)
        #####

        # Jet cleaning chains to probe NCB
        ['j85_cleanL',          'L1_J20',  [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j85_cleanT',          'L1_J20',  [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        ['j85_cleanLLP',        'L1_J20',  [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],

		#VBF Chains
        ['j70_j50_0eta490_invm1100j70_dphi20_deta45_L1MJJ-500-NFF', 'L1_MJJ-500-NFF',[], [PhysicsStream], ['Legacy:Backup','RATE:MultiJet', 'BW:Jet'], -1],
        ['j70_j50_0eta490_invm1100j70_dphi20_deta40_L1MJJ-500-NFF', 'L1_MJJ-500-NFF',[], [PhysicsStream], ['Legacy:Primary','RATE:MultiJet', 'BW:Jet'], -1],

			 ]

    TriggerFlags.BjetSlice.signatures = [
   
        # 
        #  1b / 1j
        # 

        # boffperf_split
        ['j35_boffperf_split',  'L1_J15', [], [PhysicsStream], ['Legacy:Support','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j55_boffperf_split',  'L1_J20', [], [PhysicsStream], ['Legacy:Support','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j150_boffperf_split', 'L1_J40', [], [PhysicsStream], ['Legacy:Support','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j320_boffperf_split', 'L1_J85', [], [PhysicsStream], ['Legacy:Support','RATE:SingleBJet', 'BW:BJet'], -1],


        #  gsc
        #  ATR-17811
        ['j45_gsc55_boffperf_split', 'L1_J15', [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j35_gsc45_boffperf_split_L1J15', 'L1_J15', [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SingleBJet', 'BW:BJet'], -1],
     
        ['j85_gsc110_boffperf_split', 'L1_J20', [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SingleBJet', 'BW:BJet'], -1], 
        ['j175_gsc225_boffperf_split', 'L1_J100', [], [PhysicsStream], ['Legacy:Support','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc275_boffperf_split', 'L1_J75', [], [PhysicsStream], ['Legacy:Support','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc300_boffperf_split', 'L1_J100', [], [PhysicsStream], ['Legacy:Support','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc360_boffperf_split', 'L1_J100', [], [PhysicsStream], ['Legacy:Support','RATE:SingleBJet', 'BW:BJet'], -1],
        
        ['j175_gsc225_bmv2c1040_split', 'L1_J100', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleBJet', 'BW:BJet'], -1],

        ['j225_gsc275_bmv2c1060_split', 'L1_J100', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc275_bmv2c1070_split', 'L1_J100', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleBJet', 'BW:BJet'], -1],

        ['j225_gsc300_bmv2c1070_split', 'L1_J100', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc300_bmv2c1077_split', 'L1_J100', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleBJet', 'BW:BJet'], -1],

        ['j225_gsc360_bmv2c1077_split', 'L1_J100', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleBJet', 'BW:BJet'], -1],                   
        ['j225_gsc360_bmv2c1085_split', 'L1_J100', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleBJet', 'BW:BJet'], -1],          


        ## Single jet with hybrid tuning
        ['j225_gsc275_bhmv2c1060_split',        'L1_J100', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc275_bhmv2c1070_split',        'L1_J100', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc300_bhmv2c1070_split',        'L1_J100', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc300_bhmv2c1077_split',        'L1_J100', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleBJet', 'BW:BJet'], -1],         
        ['j225_gsc360_bhmv2c1077_split',        'L1_J100', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc360_bhmv2c1085_split',        'L1_J100', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleBJet', 'BW:BJet'], -1],         

        # 
        #  1b / 4j
        # 

        ['j50_gsc65_boffperf_split_3j50_gsc65_boffperf_split',      'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Support','RATE:SingleBJet',  'BW:BJet'], -1],
        ['j50_gsc65_bmv2c1040_split_3j50_gsc65_boffperf_split',      'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleBJet',  'BW:BJet'], -1],

        ['j55_gsc75_boffperf_split_3j55_gsc75_boffperf_split',        'L1_4J20',        [], [PhysicsStream], ['Legacy:Support','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j55_gsc75_bmv2c1040_split_3j55_gsc75_boffperf_split',       'L1_4J20',        [], [PhysicsStream], ['Legacy:Primary','RATE:SingleBJet', 'BW:BJet'], -1],

        ['j60_gsc85_boffperf_split_3j60_gsc85_boffperf_split',        'L1_4J20',        [], [PhysicsStream], ['Legacy:Support','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j60_gsc85_bmv2c1050_split_3j60_gsc85_boffperf_split',       'L1_4J20',        [], [PhysicsStream], ['Legacy:Primary','RATE:SingleBJet', 'BW:BJet'], -1],

        ['j150_gsc175_boffperf_split_j45_gsc60_boffperf_split',   'L1_J100', [], [PhysicsStream], ['Legacy:Support','RATE:SingleBJet', 'BW:BJet'], -1],
        ['j150_gsc175_bmv2c1060_split_j45_gsc60_bmv2c1060_split', 'L1_J100', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiBJet', 'BW:BJet'], -1],
        ['j150_gsc175_bmv2c1070_split_j45_gsc60_bmv2c1070_split', 'L1_J100', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiBJet', 'BW:BJet'], -1],

        ['j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1070_split_L1J85_3J30', 'L1_J85_3J30', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiBJet', 'BW:BJet'], -1],
        ['j110_gsc150_boffperf_split_2j45_gsc55_bmv2c1077_split_L1J85_3J30', 'L1_J85_3J30', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiBJet', 'BW:BJet'], -1],
        ['j110_gsc150_boffperf_split_2j45_gsc55_boffperf_split_L1J85_3J30', 'L1_J85_3J30', [], [PhysicsStream], ['Legacy:Support','RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_gsc75_bmv2c1050_split_j55_gsc75_boffperf_split',        'L1_3J25.0ETA23', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j55_gsc75_bmv2c1060_split_j55_gsc75_boffperf_split',        'L1_3J25.0ETA23', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiBJet', 'BW:BJet'], -1],

        ['2j60_gsc85_bmv2c1070_split_j60_gsc85_boffperf_split',        'L1_3J25.0ETA23', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j60_gsc85_bmv2c1077_split_j60_gsc85_boffperf_split',        'L1_3J25.0ETA23', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiBJet', 'BW:BJet'], -1],

        ['2j35_boffperf_split_2j35_L14J15.0ETA25',            'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Support','RATE:MultiBJet',  'BW:BJet'], -1],

        ['2j35_bmv2c1040_split_2j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j35_bmv2c1050_split_2j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j35_bmv2c1060_split_2j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiBJet', 'BW:BJet'], -1],


        ['2j35_gsc45_bmv2c1050_split_2j35_gsc45_boffperf_split_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j35_gsc45_bmv2c1060_split_2j35_gsc45_boffperf_split_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiBJet', 'BW:BJet'], -1],

        ['2j45_gsc55_bmv2c1060_split_2j45_gsc55_boffperf_split_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j45_gsc55_bmv2c1070_split_2j45_gsc55_boffperf_split_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiBJet', 'BW:BJet'], -1],


        #
        #  2b / 5j
        #
        ['2j35_bmv2c1050_split_3j35', 'L1_5J15.0ETA25', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j35_bmv2c1060_split_3j35', 'L1_5J15.0ETA25', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiBJet', 'BW:BJet'], -1],

        ['2j35_gsc45_bmv2c1060_split_3j35_gsc45_boffperf_split',                 'L1_5J15.0ETA25', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiBJet', 'BW:BJet'], -1],
        ['2j35_gsc45_bmv2c1070_split_3j35_gsc45_boffperf_split',                 'L1_5J15.0ETA25', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiBJet', 'BW:BJet'], -1],


        #
        #  3b / 3j
        #
        ['3j65_boffperf_split_L13J35.0ETA23',        'L1_3J35.0ETA23', [], [PhysicsStream], ['Legacy:Support','RATE:MultiBJet', 'BW:BJet'], -1],
        ['3j50_gsc65_boffperf_split_L13J35.0ETA23',  'L1_3J35.0ETA23', [], [PhysicsStream], ['Legacy:Support','RATE:MultiBJet', 'BW:BJet'], -1],
        ['3j50_gsc65_bmv2c1077_split_L13J35.0ETA23', 'L1_3J35.0ETA23', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiBJet', 'BW:BJet'], -1],

        #
        #  3b / 4j
        #
        ['3j35_bmv2c1070_split_j35_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiBJet', 'BW:BJet'], -1],

        #
        #  4b 
        #
        ['4j35_bmv2c1077_split_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiBJet', 'BW:BJet'], -1],

        ['2j35_bmv2c1070_split_2j35_bmv2c1085_split_L14J15.0ETA25', 'L1_4J15.0ETA25', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiBJet', 'BW:BJet'], -1],

        #
        # L1Topo VBF MJJ bjets
        #
        ['j55_gsc80_bmv2c1070_split_j45_gsc60_bmv2c1085_split_j45_320eta490', 'L1_J40.0ETA25_2J25_J20.31ETA49', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiBJet','BW:BJet'], -1],
		
			 ]
   

    if TriggerFlags.doFTK():
        TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [
           ] # Additional FTK for BjetSlise



    TriggerFlags.METSlice.signatures = [
  # Rerun chains
        ['xe0noL1_l2fsperf',         '',        [], [PhysicsStream], ['Legacy:Rerun','RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_tc_lcw',  '',        [], [PhysicsStream], ['Legacy:Rerun','RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_tc_em',   '',        [], [PhysicsStream], ['Legacy:Rerun','RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_mht',     '',        [], [PhysicsStream], ['Legacy:Rerun','RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_mht_em',     '',        [], [PhysicsStream], ['Legacy:Rerun','RATE:MET', 'BW:MET'], -1],
        ['xe0noL1_l2fsperf_pufit',   '',        [], [PhysicsStream], ['Legacy:Rerun','RATE:MET', 'BW:MET'], -1],

        ['xe50', 'L1_XE35',[], [PhysicsStream], ['Legacy:Rerun','RATE:MET', 'BW:MET'], -1],


  ###############      
  ## L1_XE50 ####
  ###############
        ['xe110_pufit_xe65_L1XE50', 'L1_XE50', ['L1_XE50','L1_XE50'], [PhysicsStream], ['Legacy:Primary','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x65_L1XE50'] ]],
        ['xe110_pufit_xe70_L1XE50', 'L1_XE50', ['L1_XE50','L1_XE50'], [PhysicsStream], ['Legacy:Backup','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x70_L1XE50'] ]],
        ['xe110_pufit_xe75_L1XE50', 'L1_XE50', ['L1_XE50','L1_XE50'], [PhysicsStream], ['Legacy:Backup','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x75_L1XE50'] ]],

  ##################
  ## L1_XE55 #######
  ##################
        ['xe110_pufit_xe65_L1XE55', 'L1_XE55', ['L1_XE55','L1_XE55'], [PhysicsStream], ['Legacy:Backup','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x65_L1XE55'] ]],
        ['xe110_pufit_xe70_L1XE55', 'L1_XE55', ['L1_XE55','L1_XE55'], [PhysicsStream], ['Legacy:Backup','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x70_L1XE55'] ]],
        ['xe110_pufit_xe75_L1XE55', 'L1_XE55', ['L1_XE55','L1_XE55'], [PhysicsStream], ['Legacy:Backup','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x75_L1XE55'] ]],

  #################
  ## L1_XE60 ######
  #################
        ['xe110_pufit_xe65_L1XE60', 'L1_XE60', ['L1_XE60','L1_XE60'], [PhysicsStream], ['Legacy:Backup','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x65_L1XE60'] ]],
        ['xe110_pufit_xe70_L1XE60', 'L1_XE60', ['L1_XE60','L1_XE60'], [PhysicsStream], ['Legacy:Backup','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x70_L1XE60'] ]],
        ['xe110_pufit_xe75_L1XE60', 'L1_XE60', ['L1_XE60','L1_XE60'], [PhysicsStream], ['Legacy:Backup','RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe110_pufit','x75_L1XE60'] ]],

			 ]

    if TriggerFlags.doFTK():
        TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + [
            ]

    
    TriggerFlags.TauSlice.signatures = [
        # Energy calibration chain
        # Run-I comparison
        ['tau25_idperf_track',                     'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:IDMonitoring', 'BW:Tau', 'BW:ID'], -1],
        ['tau25_idperf_tracktwo',                  'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:IDMonitoring', 'BW:Tau', 'BW:ID'], -1],
        # Run-II - No BDT: main track-based items
        ['tau25_perf_tracktwo',                    'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_tracktwo_L1TAU12',            'L1_TAU12', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_loose1_tracktwo',                  'L1_TAU12IM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_tracktwo',                 'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_tight1_tracktwo',                  'L1_TAU12IM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_tracktwo_L1TAU12',         'L1_TAU12', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        # TracktwoEF
        ['tau25_idperf_tracktwoEF',                      'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_tracktwoEF',                        'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_tracktwoEF',                     'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        # MVA TES (ATR-17687)
        ['tau25_idperf_tracktwoEFmvaTES',                'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_tracktwoEFmvaTES',                  'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1NoPt_tracktwoEFmvaTES',           'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_tracktwoEFmvaTES',           'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        # expected 2018 config: MVA TES + BDT track counting + RNN TauID
        ['tau25_idperf_tracktwoMVA',                      'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_perf_tracktwoMVA',                        'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_medium1_tracktwoMVA',                        'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_verylooseRNN_tracktwoMVA',                'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_looseRNN_tracktwoMVA',                    'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_mediumRNN_tracktwoMVA',                   'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_tightRNN_tracktwoMVA',                    'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        # ATR-17807
        ['tau25_verylooseRNN_tracktwo',         'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_looseRNN_tracktwo',             'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_mediumRNN_tracktwo',            'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_tightRNN_tracktwo',             'L1_TAU12IM', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_tracktwoMVA_L1TAU12IM',                        'L1_TAU12IM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_tracktwoMVA',                        'L1_TAU20IM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau60_medium1_tracktwoMVA',                        'L1_TAU40', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_medium1_tracktwoMVA_L1TAU60',                        'L1_TAU60', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_mediumRNN_tracktwoMVA_L1TAU12IM',                        'L1_TAU12IM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_mediumRNN_tracktwoMVA',                        'L1_TAU20IM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau60_mediumRNN_tracktwoMVA',                        'L1_TAU40', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_mediumRNN_tracktwoMVA_L1TAU60',                        'L1_TAU60', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],

        # Run-II - High-pT variations
        ['tau35_loose1_tracktwo',                  'L1_TAU20IM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_tracktwo',                 'L1_TAU20IM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau50_medium1_tracktwo_L1TAU12',         'L1_TAU12', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau60_medium1_tracktwo',                 'L1_TAU40', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_medium1_tracktwo',                 'L1_TAU40', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_medium1_tracktwo_L1TAU60',         'L1_TAU60', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_medium1_tracktwo',                'L1_TAU60', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_medium1_tracktwo',                'L1_TAU60', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_idperf_track_L1TAU100',           'L1_TAU100', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:IDMonitoring', 'BW:Tau', 'BW:ID'], -1],

        # chains for 2e34 backup (ATR-15225)
        ['tau160_medium1_tracktwo_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau200_medium1_tracktwo_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleTau', 'BW:Tau'], -1],
        # ATR-17807
        ['tau160_medium1_tracktwoEF_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_mediumRNN_tracktwoMVA_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau200_medium1_tracktwoEF_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau200_mediumRNN_tracktwoMVA_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleTau', 'BW:Tau'], -1],

        ['tau160_idperf_tracktwo_L1TAU100',       'L1_TAU100', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_perf_tracktwo_L1TAU100',       'L1_TAU100', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],

        ['tau160_idperf_tracktwoEF_L1TAU100',       'L1_TAU100', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_perf_tracktwoEF_L1TAU100',       'L1_TAU100', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],

        ['tau160_idperf_tracktwoMVA_L1TAU100',       'L1_TAU100', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_perf_tracktwoMVA_L1TAU100',       'L1_TAU100', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:SingleTau', 'BW:Tau'], -1],

        ['tau35_perf_tracktwo_tau25_perf_tracktwo', 'L1_TAU20IM_2TAU12IM' , ['L1_TAU20IM','L1_TAU12IM'],[PhysicsStream, 'express'], ['Legacy:Support','RATE:MultiTau','BW:Tau'], -1],
        
        ['tau35_loose1_tracktwo_tau25_loose1_tracktwo',   'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Legacy:Support','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_loose1_tracktwo","tau25_loose1_tracktwo"]]],
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo',   'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream, 'express'], ['Legacy:End-of-fill','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
        ['tau40_medium1_tracktwo_tau35_medium1_tracktwo',   'L1_TAU25IM_2TAU20IM_2J25_3J20',['L1_TAU25IM','L1_TAU20IM'], [PhysicsStream], ['Legacy:Primary','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau40_medium1_tracktwo","tau35_medium1_tracktwo"]]],
        # ATR-17807
        ['tau35_medium1_tracktwoEF_tau25_medium1_tracktwoEF',   'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwoEF","tau25_medium1_tracktwoEF"]]],
        ['tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA',   'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_mediumRNN_tracktwoMVA","tau25_mediumRNN_tracktwoMVA"]]],
        ['tau40_medium1_tracktwoEF_tau35_medium1_tracktwoEF',   'L1_TAU25IM_2TAU20IM_2J25_3J20',['L1_TAU25IM','L1_TAU20IM'], [PhysicsStream], ['Legacy:Primary','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau40_medium1_tracktwoEF","tau35_medium1_tracktwoEF"]]],
        ['tau40_mediumRNN_tracktwoMVA_tau35_mediumRNN_tracktwoMVA',   'L1_TAU25IM_2TAU20IM_2J25_3J20',['L1_TAU25IM','L1_TAU20IM'], [PhysicsStream], ['Legacy:Primary','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau40_mediumRNN_tracktwoMVA","tau35_mediumRNN_tracktwoMVA"]]],

        ['tau35_tight1_tracktwo_tau25_tight1_tracktwo',   'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwo","tau25_tight1_tracktwo"]]],
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM_4J12.0ETA23',   'L1_TAU20IM_2TAU12IM_4J12.0ETA23',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
        # ATR-17807
        ['tau35_medium1_tracktwoEF_tau25_medium1_tracktwoEF_L1TAU20IM_2TAU12IM_4J12.0ETA23',   'L1_TAU20IM_2TAU12IM_4J12.0ETA23',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwoEF","tau25_medium1_tracktwoEF"]]],
        ['tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_L1TAU20IM_2TAU12IM_4J12.0ETA23',   'L1_TAU20IM_2TAU12IM_4J12.0ETA23',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_mediumRNN_tracktwoMVA","tau25_mediumRNN_tracktwoMVA"]]],

        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM',   'L1_TAU20IM_2TAU12IM',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Legacy:Support','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],

	    # l1topo ditau chains
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
        # ATR-17807
        ['tau35_medium1_tracktwoEF_tau25_medium1_tracktwoEF_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwoEF","tau25_medium1_tracktwoEF"]]],
        ['tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_mediumRNN_tracktwoMVA","tau25_mediumRNN_tracktwoMVA"]]],
        ['tau35_tight1_tracktwo_tau25_tight1_tracktwo_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Legacy:Backup','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwo","tau25_tight1_tracktwo"]]],

        # tau+tau topo_start_from
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_tautsf_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]],True],

        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_03dR30_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
        # ATR-17807
        ['tau35_medium1_tracktwoEF_tau25_medium1_tracktwoEF_03dR30_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwoEF","tau25_medium1_tracktwoEF"]]],
        ['tau35_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_03dR30_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_mediumRNN_tracktwoMVA","tau25_mediumRNN_tracktwoMVA"]]],

        # chains for 2e34 backup (ATR-15225)
        ['tau35_tight1_tracktwo_tau25_tight1_tracktwo_03dR30',   'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream, 'express'], ['Legacy:End-of-fill','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwo","tau25_tight1_tracktwo"]]],
        ['tau35_tight1_tracktwo_tau25_tight1_tracktwo_03dR30_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Legacy:Backup','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwo","tau25_tight1_tracktwo"]]],


        ['tau80_medium1_tracktwo_L1TAU60_tau35_medium1_tracktwo_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I',   'L1_TAU60_DR-TAU20ITAU12I',['L1_TAU60','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_medium1_tracktwo_L1TAU60","tau35_medium1_tracktwo_L1TAU12IM"]]]	,
        # non-L1topo backup (ATR-15757)
        ['tau80_medium1_tracktwo_L1TAU60_tau60_medium1_tracktwo_L1TAU40',   'L1_TAU60_2TAU40',['L1_TAU60','L1_TAU40'], [PhysicsStream], ['Legacy:Primary','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_medium1_tracktwo_L1TAU60","tau60_medium1_tracktwo_L1TAU40"]]]       ,
        # ATR-17807
        ['tau80_medium1_tracktwoEF_L1TAU60_tau35_medium1_tracktwoEF_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I',   'L1_TAU60_DR-TAU20ITAU12I',['L1_TAU60','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_medium1_tracktwoEF_L1TAU60","tau35_medium1_tracktwoEF_L1TAU12IM"]]]	,
        ['tau80_mediumRNN_tracktwoMVA_L1TAU60_tau35_mediumRNN_tracktwoMVA_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I',   'L1_TAU60_DR-TAU20ITAU12I',['L1_TAU60','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_mediumRNN_tracktwoMVA_L1TAU60","tau35_mediumRNN_tracktwoMVA_L1TAU12IM"]]]	,
        ['tau80_medium1_tracktwoEF_L1TAU60_tau60_medium1_tracktwoEF_L1TAU40',   'L1_TAU60_2TAU40',['L1_TAU60','L1_TAU40'], [PhysicsStream], ['Legacy:Primary','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_medium1_tracktwoEF_L1TAU60","tau60_medium1_tracktwoEF_L1TAU40"]]]       ,
        ['tau80_mediumRNN_tracktwoMVA_L1TAU60_tau60_mediumRNN_tracktwoMVA_L1TAU40',   'L1_TAU60_2TAU40',['L1_TAU60','L1_TAU40'], [PhysicsStream], ['Legacy:Primary','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_mediumRNN_tracktwoMVA_L1TAU60","tau60_mediumRNN_tracktwoMVA_L1TAU40"]]]       ,


        #non-L1topo backup - ATR-16230

        # High-pT DiTau seeding off of single-tau
        ['tau80_medium1_tracktwo_L1TAU60_tau50_medium1_tracktwo_L1TAU12', 'L1_TAU60',['L1_TAU60','L1_TAU12'], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_medium1_tracktwo_L1TAU60","tau50_medium1_tracktwo_L1TAU12"]]],
        ['tau125_medium1_tracktwo_tau50_medium1_tracktwo_L1TAU12', 'L1_TAU60',['L1_TAU60','L1_TAU12'], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau125_medium1_tracktwo","tau50_medium1_tracktwo_L1TAU12"]]],

        ['tau35_perf_tracktwo_tau25_perf_tracktwo_ditauM', 'L1_TAU20IM_2TAU12IM' , ['L1_TAU20IM','L1_TAU12IM'],[PhysicsStream, 'express'], ['Legacy:Rerun','RATE:MultiTau','BW:Tau'], -1],


        # photon+pion chains (ATR-13525) 
        ['tau25_singlepion_tracktwo',                    'L1_TAU12', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_dikaonmass_tracktwo',                    'L1_TAU12', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],

	#tau + photon (ATR-13841)
        ['tau25_dipion1_tracktwo',               'L1_TAU12', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_dipion2_tracktwo',               'L1_TAU12', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_kaonpi1_tracktwo',               'L1_TAU12', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_kaonpi2_tracktwo',               'L1_TAU12', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],

        #ATR-18046
        ['tau35_medium1_tracktwo_L1TAU12IM',                  'L1_TAU12IM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_tracktwoEF_L1TAU12IM',                  'L1_TAU12IM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_medium1_tracktwoEF',                  'L1_TAU20IM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau60_medium1_tracktwoEF',                  'L1_TAU40', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_medium1_tracktwoEF_L1TAU60',                  'L1_TAU60', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],

        # ATR-17807, RNN chains
        ['tau80_tightRNN_tracktwoMVA_L1TAU60',                  'L1_TAU60', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau60_tightRNN_tracktwoMVA',                  'L1_TAU40', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau40_tightRNN_tracktwoMVA',                  'L1_TAU20IM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_tightRNN_tracktwoMVA',                  'L1_TAU20IM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau35_tightRNN_tracktwoMVA_L1TAU12IM',                  'L1_TAU12IM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        ['tau25_tightRNN_tracktwoMVA_L1TAU12',                  'L1_TAU12', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleTau', 'BW:Tau'], -1],
        # ATR-17807 di-tau
	  #tracktwoEF
        ['tau35_tight1_tracktwoEF_tau25_tight1_tracktwoEF_03dR30_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Legacy:Backup','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwoEF","tau25_tight1_tracktwoEF"]]],
        ['tau35_tight1_tracktwoEF_tau25_tight1_tracktwoEF_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['Legacy:Backup','RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwoEF","tau25_tight1_tracktwoEF"]]],
        
			 ]

    if TriggerFlags.doFTK():
            TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [
                           ]

    TriggerFlags.EgammaSlice.signatures = [

        #Performance:
        ['e26_lhtight_idperf',        'L1_EM22VHI',    [], [PhysicsStream], ['Legacy:Support','RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
        ['e28_lhtight_idperf',        'L1_EM24VHI',    [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
        ['e28_lhtight_idperf_L1EM24VHIM',        'L1_EM24VHIM',    [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],

        ['g10_loose',                            'L1_EM7',  [], [PhysicsStream,'express'], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_loose_L1EM15',                     'L1_EM15', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['g10_etcut',                     'L1_EM7',   [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_etcut_L1EM12',              'L1_EM12',  [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['g200_etcut',                    'L1_EM22VHI', [], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'],-1], 
        ['g250_etcut',                    'L1_EM22VHI', [], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'],-1], 
        ['g300_etcut',                    'L1_EM22VHI', [], [PhysicsStream], ['Legacy:Primary','RATE:SinglePhoton', 'BW:Egamma'],-1], 
        ['g300_etcut_L1EM24VHI',          'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Backup','RATE:SinglePhoton', 'BW:Egamma'],-1], 
        ['g300_etcut_L1EM24VHIM',          'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Backup','RATE:SinglePhoton', 'BW:Egamma'],-1],

        ## Aranxtas request, to be run in rerun mode
        ['g22_tight',                             'L1_EM15VH', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g25_medium_L1EM22VHI', 'L1_EM22VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g25_medium_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g25_medium_L1EM24VHIM', 'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_medium_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['g35_loose_L1EM22VHI', 'L1_EM22VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g45_tight_L1EM22VHI', 'L1_EM22VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_loose_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['e140_etcut',  'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e140_etcut_L1EM24VHIM',  'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e140_lhvloose_nod0', 'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],

	['e160_etcut',  'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e160_etcut_L1EM24VHIM',  'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e180_etcut',  'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e180_etcut_L1EM24VHIM',  'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g0_hiptrt_L1EM22VHI',                 'L1_EM22VHI', [], [PhysicsStream], ['Legacy:Primary','RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g0_hiptrt_L1EM24VHI',                 'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Backup','RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g0_hiptrt_L1EM24VHIM',                'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Backup','RATE:SinglePhoton', 'BW:Egamma'], -1],

        # PS-ed trigger to supmbined chains
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
        ['g6_tight_icalotight',                            'L1_EM3',  [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g10_medium',                    'L1_EM7',   [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g15_loose_L1EM3',               'L1_EM3',   [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_loose',                     'L1_EM15VH',[], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g40_tight',                     'L1_EM20VH', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g45_tight',                     'L1_EM20VHI',[], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g50_loose',                     'L1_EM15VH', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g60_loose_L1EM15VH',            'L1_EM15VH', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],

	# ATR-16089
	['g25_tight_L1EM20VH',            'L1_EM20VH',[], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
	['g15_loose_L1EM8VH',             'L1_EM8VH', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['2g20_tight_icalovloose_L12EM15VHI',           'L1_2EM15VHI', [], [PhysicsStream, 'express'], ['Legacy:Primary','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g20_tight_icalotight_L12EM15VHI',            'L1_2EM15VHI', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g22_tight_L12EM15VHI',                   'L1_2EM15VHI', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g22_tight_icalovloose_L12EM15VHI',           'L1_2EM15VHI', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g22_tight_icalovloose',           'L1_2EM15VH', [], [PhysicsStream], ['Legacy:Support','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g22_tight_icalotight_L12EM15VHI',            'L1_2EM15VHI', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g22_tight_icalotight',            'L1_2EM15VH', [], [PhysicsStream], ['Legacy:Support','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g50_loose',                              'L1_2EM15VH', [], [PhysicsStream], ['Legacy:Support','RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g60_loose_L12EM15VH',                    'L1_2EM15VH', [], [PhysicsStream], ['Legacy:Support','RATE:MultiPhoton', 'BW:Egamma'],-1],  
        ['2g50_loose_L12EM20VH',                    'L1_2EM20VH', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g60_loose_L12EM20VH',                    'L1_2EM20VH', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['g50_loose_L1EM20VH',                     'L1_EM20VH', [], [PhysicsStream], ['Legacy:Rerun','RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['g60_loose_L1EM20VH',                     'L1_EM20VH', [], [PhysicsStream], ['Legacy:Rerun','RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ##Adding tight diphoton triggers (ATR-10762)
        ['2g22_tight',                              'L1_2EM15VH', [], [PhysicsStream], ['Legacy:Support','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['2g25_tight_L12EM20VH',                    'L1_2EM20VH', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['3g20_loose',                              'L1_2EM15VH', [], [PhysicsStream], ['Legacy:Support','RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g25_loose_g15_loose',                    'L1_2EM20VH', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g25_loose_g20_loose',                    'L1_2EM20VH', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['3g25_loose',                    'L1_2EM20VH', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiPhoton', 'BW:Egamma'],-1],
        #prescaled
        ['2g20_loose_L12EM15',                      'L1_2EM15', [], [PhysicsStream], ['Legacy:Support','RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g20_loose',                   'L1_2EM15VH', [], [PhysicsStream], ['Legacy:Support','RATE:MultiPhoton', 'BW:Egamma'],-1], 


        ['2g10_loose',                           'L1_2EM7', [], [PhysicsStream,'express'], ['Legacy:Support','RATE:MultiPhoton', 'BW:Egamma'],-1],

        ['e17_lhloose_cutd0dphideta_L1EM15',     'L1_EM15', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhloose_nod0_L1EM15',              'L1_EM15', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhloose_nodeta_L1EM15',            'L1_EM15', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhloose_nodphires_L1EM15',         'L1_EM15', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],

        #nod0 chains:
        ['e26_lhvloose_nod0_L1EM22VH',                'L1_EM22VH',    [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhvloose_nod0_L1EM22VH',                'L1_EM22VH',    [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhmedium_nod0_L1EM22VH',                'L1_EM22VH',    [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhmedium_nod0_L1EM22VH',                'L1_EM22VH',    [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],

        # ATR-17148
        ['e9_lhvloose_nod0',                          'L1_EM3',    [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e9_lhvloose_nod0_L1EM7',                    'L1_EM7',    [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e9_lhvloose_nod0_L1EM8VH',                  'L1_EM8VH',  [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],

        #ATR-16981

        ['e26_lhtight_nod0_e15_etcut_L1EM7_Zee',    	  'L1_EM22VHI', ['L1_EM22VHI','L1_EM7'], [PhysicsStream], ['Legacy:Primary','RATE:MultiElectron', 'BW:Egamma'],-1 ],
        ['e28_lhtight_nod0_e15_etcut_L1EM7_Zee',    	  'L1_EM24VHI', ['L1_EM24VHI','L1_EM7'], [PhysicsStream, 'express'], ['Legacy:Backup','RATE:MultiElectron', 'BW:Egamma'],-1 ],
        ['e28_lhtight_nod0_L1EM24VHIM_e15_etcut_L1EM7_Zee', 'L1_EM24VHIM',  ['L1_EM24VHIM','L1_EM7'], [PhysicsStream], ['Legacy:Backup','RATE:MultiElectron', 'BW:Egamma'],-1],

        # Jpsiee T&P LH-based nod0
        ['e5_lhtight_nod0_e4_etcut',         'L1_2EM3',      [], [PhysicsStream], ['Legacy:Support','RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e5_lhtight_nod0_e4_etcut_Jpsiee',  'L1_2EM3',      [], [PhysicsStream], ['Legacy:Support','RATE:MultiElectron', 'BW:Egamma'],-1],

        # L1Topo JPSI prescaled performance:
        ['e9_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM7',   'L1_JPSI-1M5-EM7',  ['L1_EM7','L1_EM3'],  [PhysicsStream, 'express'], ['Legacy:Support','RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e5_lhtight_nod0_e9_etcut_Jpsiee_L1JPSI-1M5-EM7',   'L1_JPSI-1M5-EM7',  ['L1_EM3','L1_EM7'],  [PhysicsStream], ['Legacy:Support','RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e14_lhtight_nod0_e4_etcut_Jpsiee_L1JPSI-1M5-EM12', 'L1_JPSI-1M5-EM12', ['L1_EM12','L1_EM3'], [PhysicsStream], ['Legacy:Support','RATE:MultiElectron', 'BW:Egamma'],-1, True],
        ['e5_lhtight_nod0_e14_etcut_Jpsiee_L1JPSI-1M5-EM12', 'L1_JPSI-1M5-EM12', ['L1_EM3','L1_EM12'], [PhysicsStream], ['Legacy:Support','RATE:MultiElectron', 'BW:Egamma'],-1, True],

        # Supporting trigger
        ['e5_lhloose_idperf',                   'L1_EM3',       [], [PhysicsStream], ['Legacy:Support','RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],

        # Support di-el combined chains ATR-17820
        ['e5_lhmedium_nod0',  'L1_EM3', ['L1_EM3','',''], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],

        # extra id perf chains for TRT HT studies                                                                                               

        ['e5_lhtight_idperf',                     'L1_EM3',       [], [PhysicsStream], ['Legacy:Support','RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
        ['e10_lhtight_idperf',                     'L1_EM7',       [], [PhysicsStream], ['Legacy:Support','RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],

        ['e24_lhmedium_idperf_L1EM20VH',        'L1_EM20VH',    [], [PhysicsStream], ['Legacy:Support','RATE:IDMonitoring', 'BW:Egamma', 'BW:ID'],-1],
        # Single electron triggers
        ['e17_lhmedium_nod0_L1EM15',                    'L1_EM15',   [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_L1EM18VHI',                'L1_EM18VHI',    [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e60_lhmedium_nod0', 'L1_EM22VHI', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g140_loose',        'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Primary','RATE:SinglePhoton','BW:Egamma'],-1],
        ['g140_tight',        'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Backup','RATE:SinglePhoton','BW:Egamma'],-1],
        ['g140_tight_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Backup','RATE:SinglePhoton','BW:Egamma'],-1],
        ['g160_loose',        'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Backup','RATE:SinglePhoton','BW:Egamma'],-1],
        ['g180_loose',        'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Backup','RATE:SinglePhoton','BW:Egamma'],-1],
        ['g200_loose',        'L1_EM24VHI', [], [PhysicsStream, 'express'], ['Legacy:Backup','RATE:SinglePhoton','BW:Egamma'],-1],
        ['g200_loose_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Backup','RATE:SinglePhoton','BW:Egamma'],-1],
        ['e140_lhloose_nod0', 'L1_EM22VHI', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleElectron', 'BW:Egamma'],-1],

        #2016 egamma primaries
        ['2e17_lhloose_nod0_L12EM15',    									'L1_2EM15',     [], [PhysicsStream], ['Legacy:Support','RATE:MultiElectron', 'BW:Egamma'],-1 ],
        ['e5_lhloose_nod0',             									'L1_EM3',       [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e5_lhloose_nod0_idperf',       									'L1_EM3',       [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e10_lhvloose_nod0_L1EM7',       								'L1_EM7',       [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e15_lhvloose_nod0_L1EM7',       								'L1_EM7',       [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e20_lhvloose_nod0_L1EM12',       								'L1_EM12',      [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e25_lhvloose_nod0_L1EM15',  										'L1_EM15',	    [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e30_lhvloose_nod0_L1EM15',  										'L1_EM15',	    [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e40_lhvloose_nod0_L1EM15',  										'L1_EM15',	    [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e50_lhvloose_nod0_L1EM15',  										'L1_EM15',	    [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e70_lhvloose_nod0',  														'L1_EM24VHI',   [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e80_lhvloose_nod0',  														'L1_EM24VHI',   [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e100_lhvloose_nod0', 														'L1_EM24VHI',   [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e120_lhvloose_nod0', 														'L1_EM24VHI',   [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e80_lhmedium_nod0',              'L1_EM22VHI',   [], [PhysicsStream], ['Legacy:Backup','RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e80_lhmedium_nod0_L1EM24VHI',    'L1_EM24VHI',   [], [PhysicsStream], ['Legacy:Backup','RATE:SingleElectron', 'BW:Egamma'],-1 ],

        # Prescaled triggers
        # Rate = 1 Hz each
        ['e5_etcut',                             'L1_EM3',    [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e10_etcut_L1EM7',                      'L1_EM7',    [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e15_etcut_L1EM7',                      'L1_EM7',    [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e20_etcut_L1EM12',                     'L1_EM12',   [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],         
        ['e25_etcut_L1EM15',                     'L1_EM15',   [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e30_etcut_L1EM15',                     'L1_EM15',   [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e40_etcut_L1EM15',                     'L1_EM15',   [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e50_etcut_L1EM15',                     'L1_EM15',   [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e60_etcut',                            'L1_EM24VHI',  [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e70_etcut',                            'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e80_etcut',                            'L1_EM24VHI',  [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e100_etcut',                           'L1_EM24VHI',  [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e120_etcut',                           'L1_EM24VHI',  [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e200_etcut',                           'L1_EM24VHI',  [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1], 

	['e5_lhvloose_nod0',                         'L1_EM3',       [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1], 
        # use coherent PS with lhvloose and lhvloose_nod0 variants below:


        ['e12_lhvloose_nod0_L1EM10VH',               'L1_EM10VH',    [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e17_lhvloose_nod0',                        'L1_EM15VH',    [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1], 
	['e20_lhvloose_nod0',                        'L1_EM15VH',    [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e40_lhvloose_nod0',                       'L1_EM20VH',      [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e60_lhvloose_nod0',                        'L1_EM22VHI',     [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1], 
        #Single electron/chaing for testing mis-ant robust LH ID
        ['e24_lhmedium_nod0_L1EM18VHI',            'L1_EM18VHI',    [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],

        #New nod0 chains:
        ['e24_lhtight_nod0_L1EM20VH',                 'L1_EM20VH',    [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1], 


	['e7_lhmedium_nod0',                           'L1_EM3',     [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],        
	['e9_lhloose_nod0',                           'L1_EM7',     [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
	['e9_lhmedium_nod0',                          'L1_EM7',     [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_lhmedium_nod0',                        'L1_EM8VH',   [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhmedium_nod0',                         'L1_EM15VH',  [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhmedium_nod0_ivarloose_L1EM15VHI',     'L1_EM15VHI',  [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1], 

        ['e20_lhmedium_nod0',                         'L1_EM15VH',  [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_L1EM15VH',                'L1_EM15VH',  [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e60_lhloose_nod0',                          'L1_EM22VHI',   [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e70_lhloose_nod0',                          'L1_EM22VHI',   [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e80_lhloose_nod0',                          'L1_EM22VHI',   [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e80_lhloose_nod0_L1EM24VHI',                          'L1_EM24VHI',   [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e5_lhtight_nod0',                           'L1_EM3',     [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
				['e5_lhtight_nod0_e4_etcut_Jpsiee_L1RD0_FILLED',       'L1_RD0_FILLED',      ['L1_EM3','L1_EM3'],  [PhysicsStream, 'express'], ['Legacy:Support','RATE:MultiElectron', 'BW:Egamma'],-1, True],

        ['e9_lhtight_nod0',                           'L1_EM7',     [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e14_lhtight_nod0',                          'L1_EM12',    [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhmedium_nod0',              'L1_EM22VHI',    [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],


        ##########
        # Single photon triggers        
        ['g120_loose',                    'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton','BW:Egamma'],-1], 

        # rerun mode
        ['e12_lhloose_cutd0dphideta_L1EM10VH',   'L1_EM10VH',  [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_lhloose_nod0_L1EM10VH',                 'L1_EM10VH',  [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_lhloose_nodeta_L1EM10VH',          'L1_EM10VH',  [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_lhloose_nodphires_L1EM10VH',       'L1_EM10VH',  [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e12_lhloose_nod0',                          'L1_EM8VH',   [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e17_lhloose_cutd0dphideta',            'L1_EM15VH',  [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e17_lhloose_nod0',                          'L1_EM15VH',  [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],

        ['g25_loose',                     'L1_EM15VH',[], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g25_medium',                    'L1_EM15VH',[], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_loose',                     'L1_EM15VH',[], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_medium',                    'L1_EM15VH',[], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g24_loose',                     'L1_EM20VH',[], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['2e17_lhvloose_nod0_L12EM15VHI',         'L1_2EM15VHI', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e19_lhvloose_nod0',         'L1_2EM15VHI', [], [PhysicsStream], ['Legacy:Backup','RATE:MultiElectron', 'BW:Egamma'],-1],
        ['2e24_lhvloose_nod0',                   'L1_2EM20VH', [], [PhysicsStream, 'express'], ['Legacy:Primary','RATE:MultiElectron', 'BW:Egamma'],-1],

        ['e24_lhloose_nod0_2e12_lhloose_nod0_L1EM20VH_3EM10VH',   'L1_EM20VH_3EM10VH', ['L1_EM20VH','L1_3EM10VH'], [PhysicsStream], ['Legacy:Backup','RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e24_lhvloose_nod0_2e12_lhvloose_nod0_L1EM20VH_3EM10VH', 'L1_EM20VH_3EM10VH', ['L1_EM20VH','L1_3EM10VH'], [PhysicsStream], ['Legacy:Primary','RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_2g12_loose',                          'L1_EM20VH_3EM10VH', ['L1_EM20VH','L1_3EM10VH'], [PhysicsStream], ['Legacy:Primary','RATE:ElectronPhoton', 'BW:Egamma'],-1,['parallel',-1,[] ]],
        ['e24_lhmedium_nod0_2g12_medium',                         'L1_EM20VH_3EM10VH', ['L1_EM20VH','L1_3EM10VH'], [PhysicsStream], ['Legacy:Backup','RATE:ElectronPhoton', 'BW:Egamma'],-1,['parallel',-1,[] ]],


        ##########
        # Monopole triggers (or rerun?)
        ['g25_medium_L1EM20VH',           'L1_EM20VH',  [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_medium_L1EM20VH',                  'L1_EM20VH', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_loose_L1EM20VH',            'L1_EM20VH',  [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],

#Physics (e+g):
        ['e24_lhmedium_nod0_L1EM15VH_g25_medium',  'L1_2EM15VH', [], [PhysicsStream], ['Legacy:Support','RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        ['e24_lhmedium_nod0_L1EM20VH_g25_medium',  'L1_2EM20VH', [], [PhysicsStream], ['Legacy:Primary','RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        ['e20_lhmedium_nod0_g35_loose',            'L1_2EM15VH', [], [PhysicsStream], ['Legacy:Support','RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 

        #Diphoton triggers
        ['g35_loose_g25_loose',                      'L1_2EM15VH', [], [PhysicsStream], ['Legacy:Support','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['g35_loose_g25_loose_L12EM20VH',            'L1_2EM20VH', [], [PhysicsStream], ['Legacy:Support','RATE:MultiPhoton', 'BW:Egamma'],-1],
        ['g35_medium_g25_medium_L12EM20VH',          'L1_2EM20VH', [], [PhysicsStream, 'express'], ['Legacy:Primary','RATE:MultiPhoton', 'BW:Egamma'],-1],

        ['e24_lhmedium_nod0_L1EM20VH',           'L1_EM20VH', [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e26_lhtight_nod0',                  'L1_EM22VHI', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhtight_nod0_ivarloose',        'L1_EM22VHI', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e28_lhtight_nod0',                  'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_nod0_ivarloose',        'L1_EM24VHI', [], [PhysicsStream,'express'], ['Legacy:Backup','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_nod0_ivarloose_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleElectron', 'BW:Egamma'],-1],


        #Supporting
        
        ['e28_lhtight_nod0_ivarloose_L1EM22VHI',  'L1_EM22VHI', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleElectron', 'BW:Egamma'],-1],

        # Et cut only chains
        ['e250_etcut',                           'L1_EM24VHI',  [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e300_etcut',                           'L1_EM24VHI',  [], [PhysicsStream], ['Legacy:Primary','RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e300_etcut_L1EM24VHIM',                 'L1_EM24VHIM',  [], [PhysicsStream], ['Legacy:Backup','RATE:SingleElectron', 'BW:Egamma'],-1], 

        # 2e34 single items
        ['e60_lhmedium_nod0_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e60_lhmedium_nod0_L1EM24VHIM', 'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e140_lhloose_nod0_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e140_lhloose_nod0_L1EM24VHIM', 'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleElectron', 'BW:Egamma'],-1],
				
        
        #Chains for testing 2e34 menus
        ['e32_lhtight_nod0_ivarloose',        'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleElectron', 'BW:Egamma'],-1],  
        

        #ATR-16089 supporting triggers for MC16c
        ['e24_lhvloose_nod0_L1EM20VH',                'L1_EM20VH',    [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhvloose_nod0',        'L1_EM22VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhmedium_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e80_lhmedium_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Backup','RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e17_lhvloose_nod0_L1EM15VHI',        'L1_EM15VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e19_lhvloose_nod0',        'L1_EM15VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],

        ['g20_tight_L1EM15VHI',                     'L1_EM15VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_tight_icalovloose_L1EM15VHI',                     'L1_EM15VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g20_tight_icalotight_L1EM15VHI',                     'L1_EM15VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g22_tight_L1EM15VHI',                     'L1_EM15VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g22_tight_icalovloose_L1EM15VHI',                     'L1_EM15VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g22_tight_icalovloose',            'L1_EM15VH', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g22_tight_icalotight_L1EM15VHI',                     'L1_EM15VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g22_tight_icalotight',             'L1_EM15VH', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['g25_loose_L1EM20VH',                     'L1_EM20VH', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['e24_lhloose_nod0_L1EM20VH',        'L1_EM20VH', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g12_loose',                     'L1_EM10VH', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g12_medium',                     'L1_EM10VH', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['e60_lhloose_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e70_lhloose_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e80_lhloose_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],

        ['g70_loose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g80_loose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g80_loose_icalovloose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['e60_etcut_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e70_etcut_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e80_etcut_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e100_etcut_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e120_etcut_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e200_etcut_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e250_etcut_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e70_lhvloose_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e80_lhvloose_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e100_lhvloose_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e120_lhvloose_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e140_lhvloose_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Support','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e160_lhvloose_nod0_L1EM24VHIM',        'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e160_lhvloose_nod0',        'L1_EM24VHI', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleElectron', 'BW:Egamma'],-1],
        
        ['g60_loose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g100_loose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g120_loose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Support','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g140_loose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Backup','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g160_loose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Backup','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g180_loose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Backup','RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['e20_lhmedium_nod0_L1EM18VHI',        'L1_EM18VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e20_lhtight_nod0_ivarloose_L1EM18VHI',        'L1_EM18VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],

        ['g35_loose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_tight_icalotight_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g35_tight_icalotight_L1EM24VHI',                     'L1_EM24VHI',  [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g40_tight_icalotight_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g40_tight_icalotight_L1EM24VHI',                      'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g85_tight_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g85_tight_icalovloose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g85_tight_icalovloose_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g100_tight_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g100_tight_icalovloose_L1EM24VHIM',                     'L1_EM24VHIM', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g100_tight_icalovloose_L1EM24VHI',                     'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:SinglePhoton', 'BW:Egamma'],-1],

        #ATR-16089 test chains
        ['e25_mergedtight_ivarloose',           'L1_EM20VH', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e25_mergedtight',                      'L1_EM20VH', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'], -1],
        ['e30_mergedtight',                      'L1_EM20VH', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'], -1],

        ['g35_medium_icalovloose',           'L1_EM20VH', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],
        ['g35_medium_icalotight',           'L1_EM20VH', [], [PhysicsStream], ['Legacy:Rerun','RATE:SingleElectron', 'BW:Egamma'],-1],

        # ATR-18361
        ['e5_lhvloose_nod0_bBeexM6000t',  '',   ['L1_EM3'], [BPhysicsStream], ['Legacy:End-of-fill',RATE_BphysTag,BW_BphysTag],-1],
        ['2e5_lhvloose_nod0_bBeexM6000t',  '',   ['L1_EM3','L1_EM3'], [BPhysicsStream], ['Legacy:End-of-fill',RATE_BphysTag,BW_BphysTag],-1],

        	 ]

    TriggerFlags.BphysicsSlice.signatures = [

        #New for 2018: mu11_mu6 triggers seeded from LFV-MU11
        ['mu11_mu6_bNocut_L1LFV-MU11',                'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bDimu_noinvm_novtx_ss_L1LFV-MU11', 'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bDimu_novtx_noos_L1LFV-MU11',      'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bDimu_L1LFV-MU11',                 'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bDimu2700_L1LFV-MU11',             'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bJpsimumu_L1LFV-MU11',             'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bBmumu_L1LFV-MU11',                'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bUpsimumu_L1LFV-MU11',             'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bTau_L1LFV-MU11',                  'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bPhi_L1LFV-MU11',                  'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bBmumuxv2_L1LFV-MU11',             'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bBmumux_BpmumuKp_L1LFV-MU11',      'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bJpsimumu_Lxy0_L1LFV-MU11',        'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Backup',RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bDimu2700_Lxy0_L1LFV-MU11',        'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Backup',RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu11_mu6_bDimu_Lxy0_L1LFV-MU11',            'L1_LFV-MU11', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Backup',RATE_BphysTag,BW_BphysTag], -1, False],


        #ATR-16219
        ['3mu4_bDimu2700',                'L1_3MU4', [], [BPhysicsStream],             ['Legacy:End-of-fill',RATE_BphysTag,BW_BphysTag], -1],

        ['mu11_mu6_bPhi',                 'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Backup',RATE_BphysTag,BW_BphysTag], -1,['serial',-1,['mu11','mu6_bPhi']]],
        ['mu11_mu6noL1_bPhi_L1MU11_2MU6', 'L1_MU11_2MU6', ['L1_MU11','L2_mu6'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1,['serial',-1,['mu11','mu6noL1_bPhi']]],

        ['mu6_mu4_bDimu',                 'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1],
	
        ['2mu6_bDimu',                    'L1_2MU6', [], [BPhysicsStream], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1], 

        # ATR-16163
        ['mu10_bJpsi_TrkPEBmon',        'L1_MU10',  [], [BPhysicsStream,'express'], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1],
        
        ['3mu4_bJpsi',                    'L1_3MU4', [], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1],
        ['3mu4_bTau',                     'L1_3MU4', [], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1],
        ['3mu4_bUpsi',                    'L1_3MU4', [], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1],
        
        ['3mu6_bDimu',                   'L1_3MU6', [], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1],
        ['3mu6_bJpsi',                   'L1_3MU6', [], [BPhysicsStream], ['Legacy:Backup',RATE_BphysTag,BW_BphysTag], -1],
        ['3mu6_bTau',                    'L1_3MU6', [], [BPhysicsStream], ['Legacy:Backup',RATE_BphysTag,BW_BphysTag], -1],
        ['3mu6_bUpsi',                   'L1_3MU6', [], [BPhysicsStream], ['Legacy:Backup',RATE_BphysTag,BW_BphysTag], -1],

        ['mu6_2mu4_bJpsi', 'L1_MU6_3MU4', ['L1_MU6','L1_3MU4'], [BPhysicsStream, 'express'], ['Legacy:Primary',RATE_BphysTag, BW_BphysTag], -1],
        ['mu6_2mu4_bUpsi', 'L1_MU6_3MU4', ['L1_MU6','L1_3MU4'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag, BW_BphysTag], -1],
        ['mu6_2mu4_bDimu2700', 'L1_MU6_3MU4', ['L1_MU6','L1_3MU4'], [BPhysicsStream], ['Legacy:Backup',RATE_BphysTag, BW_BphysTag], -1],

        ### ATR-18329
        ['mu6_2mu4_bDimu6000', 'L1_MU6_3MU4', ['L1_MU6','L1_3MU4'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag, BW_BphysTag], -1],
        ['4mu4_bDimu6000', 'L1_4MU4', ['L1_4MU4',''], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1],
        
        ['mu4_mu4_idperf_bJpsimumu_noid',  'L1_2MU4', [], [PhysicsStream, 'express'], ['Legacy:Support','RATE:IDMonitoring',BW_BphysTag, 'BW:ID'], -1],  
        ['mu13_mu13_idperf_Zmumu',     'L1_2MU10', [], [PhysicsStream, 'express'], ['Legacy:Primary','RATE:IDMonitoring','BW:Muon', 'BW:ID'], -1],

        ['2mu10_bBmumuxv2',               'L1_2MU10', [], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1],
        ['2mu10_bJpsimumu',               'L1_2MU10', [], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1],
        ['2mu10_bUpsimumu',               'L1_2MU10', [], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1],
     
        ### ATR-14350
        ['2mu6_bBmumuxv2_L1LFV-MU6',              'L1_LFV-MU6',     ['L1_2MU6'], [BPhysicsStream], ['Legacy:End-of-fill',RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bPhi_L1LFV-MU6',                   'L1_LFV-MU6',     ['L1_2MU6'], [BPhysicsStream], ['Legacy:End-of-fill',RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bDimu_L1LFV-MU6',                  'L1_LFV-MU6',     ['L1_2MU6'], [BPhysicsStream], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bDimu_novtx_noos_L1LFV-MU6',       'L1_LFV-MU6',     ['L1_2MU6'], [BPhysicsStream], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1, False],

        ['2mu4_bBmumu_L1BPH-2M9-2MU4_BPH-0DR15-2MU4',         'L1_BPH-2M9-2MU4_BPH-0DR15-2MU4', ['L1_2MU4'], [BPhysicsStream], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu4_bBmumu_Lxy0_L1BPH-2M9-2MU4_BPH-0DR15-2MU4',    'L1_BPH-2M9-2MU4_BPH-0DR15-2MU4', ['L1_2MU4'], [BPhysicsStream], ['Legacy:End-of-fill',RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu4_bJpsimumu_Lxy0_L1BPH-2M9-2MU4_BPH-0DR15-2MU4', 'L1_BPH-2M9-2MU4_BPH-0DR15-2MU4', ['L1_2MU4'], [BPhysicsStream], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu4_bBmumux_BsmumuPhi_L1BPH-2M9-2MU4_BPH-0DR15-2MU4', 'L1_BPH-2M9-2MU4_BPH-0DR15-2MU4', ['L1_2MU4'], [BPhysicsStream], ['Legacy:End-of-fill',RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu4_bBmumux_BpmumuKp_L1BPH-2M9-2MU4_BPH-0DR15-2MU4', 'L1_BPH-2M9-2MU4_BPH-0DR15-2MU4', ['L1_2MU4'], [BPhysicsStream], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1, False],
       
        ['mu6_mu4_bBmumu_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4','L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bBmumu_Lxy0_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4','L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], ['Legacy:End-of-fill',RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bJpsimumu_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4','L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], ['Legacy:End-of-fill',RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bJpsimumu_Lxy0_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4','L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream, 'express'], ['Legacy:End-of-fill',RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bBmumux_BsmumuPhi_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4','L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], ['Legacy:End-of-fill',RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bBmumux_BpmumuKp_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4','L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bDimu_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4','L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1, False],
        ['mu6_mu4_bDimu_novtx_noos_L1BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4','L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1, False],

        ['mu11_mu6_bDimu2700',            'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1],
        ['mu11_mu6_bDimu_Lxy0',            'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Backup',RATE_BphysTag,BW_BphysTag], -1],
        ['mu11_mu6_bDimu2700_Lxy0',            'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Backup',RATE_BphysTag,BW_BphysTag], -1],
        
        ['2mu6_mu4_bUpsi', 'L1_2MU6_3MU4', ['L1_2MU6','L1_MU4'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag, BW_BphysTag], -1],

        #OI replacement for mu20_nomucomb_mu6noL1_nscan03 and mu11_nomucomb_2mu4noL1_nscan03_L1MU11_2MU6
        ['mu20_mu6noL1_bNocut',             'L1_MU20MU21',      ['L1_MU20','L2_mu20'],        [BPhysicsStream], ['Legacy:Support',RATE_BphysTag,'BW:Muon'], -1,['serial',-1,['mu20','mu6noL1']]],  
        ['mu11_2mu4noL1_bNocut_L1MU11_2MU6', 'L1_MU11_2MU6', ['L1_MU11','L2_mu11'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1,['serial',-1,['mu11','2mu4noL1']]], 

        ['3mu4_bPhi',                     'L1_3MU4', [], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1],


        ['mu11_2mu4noL1_bTau_L1MU11_2MU6', 'L1_MU11_2MU6', ['L1_MU11','L2_mu11'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1,['serial',-1,['mu11','2mu4noL1']]],
        ['mu20_mu6noL1_bTau',              'L1_MU20MU21', ['L1_MU20','L2_mu20'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1,['serial',-1,['mu20','mu6noL1']]],

        ['mu6_2mu4_bTau_L1MU6_3MU4', 'L1_MU6_3MU4',  ['L1_MU6','L1_2MU4'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1],
        ['2mu6_mu4_bTau_L12MU6_3MU4', 'L1_2MU6_3MU4', ['L1_2MU6','L1_MU4'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1],

        ### ATR-14350: L1Topo bphysics chains
        # 96% bckg rejection WP
        ['2mu6_bDimu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',      'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',     ['L1_2MU6'], [BPhysicsStream, 'express'], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bDimu_novtx_noos_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',      'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',     ['L1_2MU6'], [BPhysicsStream], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bJpsimumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',  'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',     ['L1_2MU6'], [BPhysicsStream, 'express'], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bUpsimumu_L1BPH-8M15-2MU6_BPH-0DR22-2MU6', 'L1_BPH-8M15-2MU6_BPH-0DR22-2MU6',     ['L1_2MU6'], [PhysicsStream, 'express'], ['Legacy:Primary','RATE:Bphysics','BW:Bphys'], -1, False],
        ['2mu6_bBmumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',     'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',     ['L1_2MU6'], [BPhysicsStream, 'express'], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bBmumux_BpmumuKp_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',  'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',     ['L1_2MU6'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1, False],


        #ATR-16219
        ['mu6_mu4_bUpsimumu_L1BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4-BO', 'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4-BO',     ['L1_MU6','L1_MU4'], [BPhysicsStream], ['Legacy:End-of-fill',RATE_BphysTag,BW_BphysTag], -1, False],

        ['2mu6_bBmumu_Lxy0_L1BPH-2M9-2MU6_BPH-2DR15-2MU6', 'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',     ['L1_2MU6'], [BPhysicsStream, 'express'], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bBmumux_BsmumuPhi_L1BPH-2M9-2MU6_BPH-2DR15-2MU6', 'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',     ['L1_2MU6'], [BPhysicsStream, 'express'], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1, False],
        ['2mu6_bJpsimumu_Lxy0_L1BPH-2M9-2MU6_BPH-2DR15-2MU6', 'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',     ['L1_2MU6'], [BPhysicsStream], ['Legacy:Backup',RATE_BphysTag,BW_BphysTag], -1, False],
        
        # new baseline triggers, ATR-15503
        ['mu11_mu6_bBmumu',                'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream, 'express'], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1],
        ['mu11_mu6_bBmumuxv2',             'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream, 'express'], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1],
        ['mu11_mu6_bBmumux_BpmumuKp',      'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1],
        ['mu11_mu6_bJpsimumu_Lxy0',        'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Backup',RATE_BphysTag,BW_BphysTag], -1],
        ['mu11_mu6_bJpsimumu',             'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream, 'express'], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1],
        ['mu11_mu6_bUpsimumu',             'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1],
        ['mu11_mu6_bDimu',                 'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream, 'express'], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1],
        ['mu11_mu6_bDimu_noinvm_novtx_ss', 'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1],
        ['mu11_mu6_bDimu_novtx_noos',      'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream, 'express'], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1],

        # ATR-16314
        ['mu11_mu6_bTau',                  'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream, 'express'], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1],
        # ATR-17617
        ['mu20_mu6btrk_bTauTight',        'L1_MU20MU21', ['L1_MU20','L2_mu20'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1,['serial',-1,['mu20','mu6btrk']]],
        ['mu20_2mu2btrk_bTauTight',       'L1_MU20MU21', ['L1_MU20','L2_mu20'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1,['serial',-1,['mu20','2mu2btrk']]],
        ['mu11_2mu2btrk_bTauTight_L1MU11_2MU6', 'L1_MU11_2MU6', ['L1_MU11','L2_mu11'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag], -1,['serial',-1,['mu11','2mu2btrk']]],
        
        # support triggers ATR-15851
        ['mu11_mu6_bNocut',                 'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1],
        ['mu11_mu6noL1_bNocut_L1MU11_2MU6',  'L1_MU11_2MU6', ['L1_MU11','L2_mu6'], [BPhysicsStream], ['Legacy:Support',RATE_BphysTag,BW_BphysTag], -1,['serial',-1,['mu11','mu6noL1']]],
        
			 ]

    TriggerFlags.CombinedSlice.signatures = [
#        # L1HT190-J15s5.ETA21
        ['j45_gsc55_bmv2c1050_split_ht700_L1HT190-J15s5.ETA21',  'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['Legacy:Primary','RATE:SingleBJet', 'BW:BJet_combined'], -1,['serial',-1,['ht700','j45_gsc55_bmv2c1050_split']]],
        ['2j45_gsc55_boffperf_split_ht300_L1HT190-J15s5.ETA21',  'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['Legacy:Support','RATE:MultiBJet', 'BW:BJet_combined'], -1,['serial',-1,[ 'ht300', '2j45_gsc55_boffperf_split'  ]]], 
        ['2j45_gsc55_bmv2c1050_split_ht300_L1HT190-J15s5.ETA21', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['Legacy:Primary','RATE:MultiBJet', 'BW:BJet_combined'], -1,['serial',-1,[ 'ht300', '2j45_gsc55_bmv2c1050_split' ]]], 
        ['2j45_gsc55_bmv2c1060_split_ht300_L1HT190-J15s5.ETA21', 'L1_HT190-J15s5.ETA21', [], [PhysicsStream], ['Legacy:End-of-fill','RATE:MultiBJet', 'BW:BJet_combined'], -1,['serial',-1,[ 'ht300', '2j45_gsc55_bmv2c1060_split' ]]], 

        ['mu4_j45_gsc55_boffperf_split_dr05_dz02', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream, 'express'], ['Legacy:Support','RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu4', 'j45_gsc55_boffperf_split'] ]],

        ## VBF triggers
        ['e10_lhmedium_nod0_ivarloose_j70_j50_0eta490_invm900j50_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_EM8VH','',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["e10_lhmedium_nod0_ivarloose","j70","j50_0eta490_invm900j50"]]],
        ['e5_lhvloose_j70_j50_0eta490_invm1000j50_xe50_pufit_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_EM3','','',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["e5_lhvloose","j70","j50_0eta490","xe50_pufit"]]],
        ['e5_lhvloose_nod0_j70_j50_0eta490_invm1000j50_xe50_pufit_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_EM3','','',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["e5_lhvloose_nod0","j70","j50_0eta490","xe50_pufit"]]],
        ['e5_lhloose_j70_j50_0eta490_invm1000j50_xe50_pufit_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_EM3','','',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["e5_lhloose","j70","j50_0eta490","xe50_pufit"]]],
        ['e5_lhvloose_j70_j50_0eta490_invm1000j50_dphi28_xe50_pufit_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_EM3','','',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["e5_lhvloose","j70","j50_0eta490","xe50_pufit"]]],
        ['mu10_ivarmedium_j70_j50_0eta490_invm900j50_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_MU10','',''], [PhysicsStream], ['Legacy:Primary','RATE:MuonJet', 'BW:Muon', 'BW:Jet'], -1, ['serial', -1,["mu10_ivarmedium","j70","j50_0eta490_invm900j50"]]],
        ['mu4_j70_j50_0eta490_invm1000j50_xe50_pufit_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_MU4','','',''], [PhysicsStream], ['Legacy:Primary','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial', -1,["mu4","j70","j50_0eta490","xe50_pufit"]]],
        ['mu4_j70_j50_0eta490_invm1000j50_dphi28_xe50_pufit_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_MU4','','',''], [PhysicsStream], ['Legacy:Backup','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial', -1,["mu4","j70","j50_0eta490","xe50_pufit"]]],
        ['2mu4_j70_j50_0eta490_invm1000j50_xe50_pufit_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_2MU4','','',''], [PhysicsStream], ['Legacy:Primary','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial', -1,["2mu4","j70","j50_0eta490","xe50_pufit"]]],
        ['g35_medium_j70_j50_0eta490_invm900j50_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_EM24VHI','',''], [PhysicsStream], ['Legacy:Primary','RATE:MuonJet', 'BW:Muon', 'BW:Jet'], -1, ['serial', -1,["g35_medium","j70","j50_0eta490_invm900j50"]]],
        ['j70_j50_0eta490_invm1000j50_dphi24_xe90_pufit_xe50_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', [], [PhysicsStream], ['Legacy:Primary','RATE:MET', 'BW:MET'], -1, ['serial', -1,["j70","j50_0eta490_invm1000j50_dphi24","xe90_pufit","xe50"]]],

        ['j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ-500-NFF_AND_2j35_bmv2c1070_split', 'L1_MJJ-500-NFF', [], [PhysicsStream], ['Legacy:Primary','RATE:JetMET', 'BW:Jet'], -1, ['serial', -1,["j70_0eta490","j50_0eta490","2j35_0eta490_invm1000j50_L1MJJ-500-NFF_AND","2j35_bmv2c1070_split"]]],
        ['j70_0eta490_j50_0eta490_2j35_0eta490_invm1200j50_L1MJJ-500-NFF_AND_2j35_bmv2c1070_split', 'L1_MJJ-500-NFF', [], [PhysicsStream], ['Legacy:Primary','RATE:JetMET', 'BW:Jet'], -1, ['serial', -1,["j70_0eta490","j50_0eta490","2j35_0eta490_invm1200j50_L1MJJ-500-NFF_AND","2j35_bmv2c1070_split"]]],
        ['j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ-500-NFF_AND_2j35_bmv2c1060_split', 'L1_MJJ-500-NFF', [], [PhysicsStream], ['Legacy:Backup','RATE:JetMET', 'BW:Jet'], -1, ['serial', -1,["j70_0eta490","j50_0eta490","2j35_0eta490_invm1000j50_L1MJJ-500-NFF_AND","2j35_bmv2c1060_split"]]],
        ['j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ-500-NFF_AND_2j35_gsc45_bmv2c1070_split', 'L1_MJJ-500-NFF', [], [PhysicsStream], ['Legacy:Backup','RATE:JetMET', 'BW:Jet'], -1, ['serial', -1,["j70_0eta490","j50_0eta490","2j35_0eta490_invm1000j50_L1MJJ-500-NFF_AND","2j35_gsc45_bmv2c1070_split"]]],

        ['2e5_lhmedium_nod0_j70_j50_0eta490_invm900j50_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_2EM3','',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["2e5_lhmedium_nod0","j70","j50_0eta490_invm900j50"]]],
        ['2mu6_2j50_0eta490_invm900j50_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_2MU6',''], [PhysicsStream], ['Legacy:Primary','RATE:MuonJet', 'BW:Muon', 'BW:Jet'], -1, ['serial', -1,["2mu6","2j50_0eta490_invm900j50"]]],

        ['tau25_medium1_tracktwo_tau20_medium1_tracktwo_j70_j50_0eta490_invm900j50_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_TAU12IM','L1_TAU12IM','',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["tau25_medium1_tracktwo","tau20_medium1_tracktwo","j70","j50_0eta490_invm900j50"]]],
        ['tau25_mediumRNN_tracktwoMVA_tau20_mediumRNN_tracktwoMVA_j70_j50_0eta490_invm900j50_L1MJJ-500-NFF', 'L1_MJJ-500-NFF', ['L1_TAU12IM','L1_TAU12IM','',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["tau25_mediumRNN_tracktwoMVA","tau20_mediumRNN_tracktwoMVA","j70","j50_0eta490_invm900j50"]]],


        ['e10_lhmedium_nod0_ivarloose_j70_j50_0eta490_invm900j50_L1MJJ-700', 'L1_MJJ-700', ['L1_EM8VH','',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["e10_lhmedium_nod0_ivarloose","j70","j50_0eta490_invm900j50"]]],
        ['e5_lhvloose_j70_j50_0eta490_invm1000j50_xe50_pufit_L1MJJ-700', 'L1_MJJ-700', ['L1_EM3','','',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["e5_lhvloose","j70","j50_0eta490","xe50_pufit"]]],
        ['e5_lhloose_j70_j50_0eta490_invm1000j50_xe50_pufit_L1MJJ-700', 'L1_MJJ-700', ['L1_EM3','','',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["e5_lhloose","j70","j50_0eta490","xe50_pufit"]]],
        ['e5_lhvloose_j70_j50_0eta490_invm1000j50_dphi28_xe50_pufit_L1MJJ-700', 'L1_MJJ-700', ['L1_EM3','','',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["e5_lhvloose","j70","j50_0eta490","xe50_pufit"]]],
        ['mu10_ivarmedium_j70_j50_0eta490_invm900j50_L1MJJ-700', 'L1_MJJ-700', ['L1_MU10','',''], [PhysicsStream], ['Legacy:Primary','RATE:MuonJet', 'BW:Muon', 'BW:Jet'], -1, ['serial', -1,["mu10_ivarmedium","j70","j50_0eta490_invm900j50"]]],
        ['mu4_j70_j50_0eta490_invm1000j50_xe50_pufit_L1MJJ-700', 'L1_MJJ-700', ['L1_MU4','','',''], [PhysicsStream], ['Legacy:Primary','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial', -1,["mu4","j70","j50_0eta490","xe50_pufit"]]],
        ['mu4_j70_j50_0eta490_invm1000j50_dphi28_xe50_pufit_L1MJJ-700', 'L1_MJJ-700', ['L1_MU4','','',''], [PhysicsStream], ['Legacy:Backup','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial', -1,["mu4","j70","j50_0eta490","xe50_pufit"]]],
        ['2mu4_j70_j50_0eta490_invm1000j50_xe50_pufit_L1MJJ-700', 'L1_MJJ-700', ['L1_2MU4','','',''], [PhysicsStream], ['Legacy:Primary','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial', -1,["2mu4","j70","j50_0eta490","xe50_pufit"]]],
        ['g35_medium_j70_j50_0eta490_invm900j50_L1MJJ-700', 'L1_MJJ-700', ['L1_EM24VHI','',''], [PhysicsStream], ['Legacy:Primary','RATE:MuonJet', 'BW:Muon', 'BW:Jet'], -1, ['serial', -1,["g35_medium","j70","j50_0eta490_invm900j50"]]],
        ['j70_j50_0eta490_invm1000j50_dphi24_xe90_pufit_xe50_L1MJJ-700', 'L1_MJJ-700', [], [PhysicsStream], ['Legacy:Primary','RATE:MET', 'BW:MET'], -1, ['serial', -1,["j70","j50_0eta490_invm1000j50_dphi24","xe90_pufit","xe50"]]],


        ['j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ-700_AND_2j35_bmv2c1070_split', 'L1_MJJ-700', [], [PhysicsStream], ['Legacy:Primary','RATE:JetMET', 'BW:Jet'], -1, ['serial', -1,["j70_0eta490","j50_0eta490","2j35_0eta490_invm1000j50_L1MJJ-700_AND","2j35_bmv2c1070_split"]]],
        ['j70_0eta490_j50_0eta490_2j35_0eta490_invm1200j50_L1MJJ-700_AND_2j35_bmv2c1070_split', 'L1_MJJ-700', [], [PhysicsStream], ['Legacy:Primary','RATE:JetMET', 'BW:Jet'], -1, ['serial', -1,["j70_0eta490","j50_0eta490","2j35_0eta490_invm1200j50_L1MJJ-700_AND","2j35_bmv2c1070_split"]]],
        ['j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ-700_AND_2j35_bmv2c1060_split', 'L1_MJJ-700', [], [PhysicsStream], ['Legacy:Backup','RATE:JetMET', 'BW:Jet'], -1, ['serial', -1,["j70_0eta490","j50_0eta490","2j35_0eta490_invm1000j50_L1MJJ-700_AND","2j35_bmv2c1060_split"]]],
        ['j70_0eta490_j50_0eta490_2j35_0eta490_invm1000j50_L1MJJ-700_AND_2j35_gsc45_bmv2c1070_split', 'L1_MJJ-700', [], [PhysicsStream], ['Legacy:Backup','RATE:JetMET', 'BW:Jet'], -1, ['serial', -1,["j70_0eta490","j50_0eta490","2j35_0eta490_invm1000j50_L1MJJ-700_AND","2j35_gsc45_bmv2c1070_split"]]],


        ['2e5_lhmedium_nod0_j70_j50_0eta490_invm900j50_L1MJJ-700', 'L1_MJJ-700', ['L1_2EM3','',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["2e5_lhmedium_nod0","j70","j50_0eta490_invm900j50"]]],
        ['2mu6_2j50_0eta490_invm900j50_L1MJJ-700', 'L1_MJJ-700', ['L1_2MU6',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial', -1,["2mu6","2j50_0eta490_invm900j50"]]],


        ['tau25_medium1_tracktwo_tau20_medium1_tracktwo_j70_j50_0eta490_invm900j50_L1MJJ-700', 'L1_MJJ-700', ['L1_TAU12IM','L1_TAU12IM','',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["tau25_medium1_tracktwo","tau20_medium1_tracktwo","j70","j50_0eta490_invm900j50"]]],
        ['tau25_mediumRNN_tracktwoMVA_tau20_mediumRNN_tracktwoMVA_j70_j50_0eta490_invm900j50_L1MJJ-700', 'L1_MJJ-700', ['L1_TAU12IM','L1_TAU12IM','',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["tau25_mediumRNN_tracktwoMVA","tau20_mediumRNN_tracktwoMVA","j70","j50_0eta490_invm900j50"]]],


        # b-jet+MET ATR-17678
        ['2j35_gsc45_bmv2c1070_split_xe85_pufit_xe50_L12J15_XE55',"L1_2J15_XE55",[],[PhysicsStream], ['Legacy:Primary','RATE:MultiBJet', 'BW:BJet_combined', 'BW:MET'], -1, ['serial',-1,['xe85_pufit','xe50_L12J15_XE55','2j35_gsc45_bmv2c1070_split']]],
        ['3j35_bmv2c1077_split_xe70_pufit_xe50_L13J15.0ETA25_XE40',"L1_3J15.0ETA25_XE40",[],[PhysicsStream], ['Legacy:Primary','RATE:MultiBJet', 'BW:BJet_combined', 'BW:MET'], -1, ['serial',-1,['xe70_pufit','xe50_L13J15.0ETA25_XE40','3j35_bmv2c1077_split']]],
        ['j85_gsc100_bmv2c1050_split_xe85_pufit_xe50_L1XE55',"L1_XE55",[],[PhysicsStream], ['Legacy:Primary','RATE:MultiBJet', 'BW:BJet_combined', 'BW:MET'], -1, ['serial',-1,['xe85_pufit','xe50_L1XE55','j85_gsc100_bmv2c1050_split']]],


        # Non-L1Topo W T&P triggers
        ['e13_etcut_trkcut_xs30_xe30_mt35', 'L1_EM12_XS20', ['L1_EM12','',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xs30","xe30"]]],
        ['e13_etcut_trkcut_xs30_j15_perf_xe30_6dphi05_mt35', 'L1_EM12_XS20', ['L1_EM12','','',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e13_etcut_trkcut_xs30_j15_perf_xe30_6dphi15_mt35', 'L1_EM12_XS20', ['L1_EM12','','',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e13_etcut_trkcut_xs30_j15_perf_xe30_2dphi05_mt35', 'L1_EM12_XS20', ['L1_EM12','','',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e13_etcut_trkcut_xs30_j15_perf_xe30_2dphi15_mt35', 'L1_EM12_XS20', ['L1_EM12','','',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e13_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e18_etcut_trkcut_xs30_xe30_mt35', 'L1_EM15_XS30', ['L1_EM15','',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e18_etcut_trkcut","xs30","xe30"]]],
        ['e18_etcut_trkcut_xs30_j15_perf_xe30_6dphi05_mt35', 'L1_EM15_XS30', ['L1_EM15','','',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e18_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e18_etcut_trkcut_xs30_j15_perf_xe30_6dphi15_mt35', 'L1_EM15_XS30', ['L1_EM15','','',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e18_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e18_etcut_trkcut_xs30_j15_perf_xe30_2dphi05_mt35', 'L1_EM15_XS30', ['L1_EM15','','',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e18_etcut_trkcut","xs30","j15_perf","xe30"]]],
        ['e18_etcut_trkcut_xs30_j15_perf_xe30_2dphi15_mt35', 'L1_EM15_XS30', ['L1_EM15','','',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e18_etcut_trkcut","xs30","j15_perf","xe30"]]],


        # Non-L1Topo W T&P triggers
        ['e18_etcut_trkcut_xs20_j20_perf_xe20_6dphi15_mt35', 'L1_EM15_XS30', ['L1_EM15','','',''],[PhysicsStream],['Legacy:Support','RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e18_etcut_trkcut","xs20","j20_perf","xe20"]]],
        ['e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35',      'L1_EM15_XS30', ['L1_EM15','',''],[PhysicsStream],['Legacy:Support','RATE:EgammaMET','BW:Egamma'],-1,['serial',-1,["e18_etcut_trkcut","j20_perf","xe20"]]],


        ['2e12_lhloose_nod0_mu10',  'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [PhysicsStream], ['Legacy:Primary','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['2e12_lhmedium_nod0_mu10', 'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e12_lhloose_nod0_2mu10',  'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['Legacy:Primary','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e12_lhmedium_nod0_2mu10', 'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],


        ['e17_lhloose_nod0_mu14',   'L1_EM15VH_MU10', ['L1_EM15VH', 'L1_MU10'], [PhysicsStream], ['Legacy:Primary','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e26_lhmedium_nod0_mu8noL1', 'L1_EM22VHI', ['L1_EM22VHI', ''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['e26_lhmedium_nod0','mu8noL1'] ]],
        ['e28_lhmedium_nod0_mu8noL1', 'L1_EM24VHI', ['L1_EM24VHI', ''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['e28_lhmedium_nod0','mu8noL1'] ]],
        ['e28_lhmedium_nod0_L1EM24VHIM_mu8noL1', 'L1_EM24VHIM', ['L1_EM24VHIM', ''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['e28_lhmedium_nod0_L1EM24VHIM','mu8noL1'] ]],
        ['e28_lhmedium_nod0', 'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1],
        ['e70_lhloose_nod0_xe70noL1',    'L1_EM22VHI',['L1_EM22VHI',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e70_lhloose_nod0","xe70noL1"]]],
        ['e70_lhloose_nod0_L1EM24VHIM_xe70noL1',    'L1_EM24VHIM',['L1_EM24VHIM',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e70_lhloose_nod0_L1EM24VHIM","xe70noL1"]]],
        ['e80_lhloose_nod0_xe80noL1',    'L1_EM22VHI',['L1_EM22VHI',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e80_lhloose_nod0","xe80noL1"]]],
        ['e80_lhloose_nod0_L1EM24VHI_xe80noL1',    'L1_EM24VHI',['L1_EM24VHI',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e80_lhloose_nod0_L1EM24VHI","xe80noL1"]]],
        ['e80_lhloose_nod0_L1EM24VHIM_xe80noL1',    'L1_EM24VHIM',['L1_EM24VHIM',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e80_lhloose_nod0_L1EM24VHIM","xe80noL1"]]],

        ['e7_lhmedium_nod0_mu24',        'L1_MU20MU21', ['L1_EM3', 'L1_MU20'], [PhysicsStream], ['Legacy:Primary','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['g45_tight_L1EM22VHI_xe45noL1', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g45_tight_L1EM22VHI","xe45noL1"]]],
        ['g45_tight_L1EM24VHI_xe45noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g45_tight_L1EM24VHI","xe45noL1"]]],
        ['g45_tight_L1EM24VHIM_xe45noL1', 'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g45_tight_L1EM24VHIM","xe45noL1"]]],
        ['g45_tight_icalovloose_L1EM24VHIM_xe45noL1', 'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g45_tight_icalovloose_L1EM24VHIM","xe45noL1"]]],
        ['g80_loose_L1EM24VHI_xe80noL1',           'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g80_loose_L1EM24VHI","xe80noL1"]]],
        ['g80_loose_L1EM24VHIM_xe80noL1',           'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g80_loose_L1EM24VHIM","xe80noL1"]]],
        ['g80_loose_icalovloose_L1EM24VHIM_xe80noL1',           'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g80_loose_icalovloose_L1EM24VHIM","xe80noL1"]]],
        ['g45_tight_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:ElectronPhoton', 'BW:Egamma'], -1],
        ['g45_tight_icalovloose_L1EM22VHI_xe45noL1', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g45_tight_icalovloose_L1EM22VHI","xe45noL1"]]],
        ['g45_tight_icalovloose_L1EM24VHI_xe45noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g45_tight_icalovloose_L1EM24VHI","xe45noL1"]]],
        ['g80_loose_icalovloose_xe80noL1',           'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g80_loose_icalovloose","xe80noL1"]]],
        ['g80_loose_icalovloose_L1EM24VHI_xe80noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g80_loose_icalovloose_L1EM24VHI","xe80noL1"]]],
        ['g85_tight_icalovloose_L1EM22VHI_3j50noL1',  'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g85_tight_icalovloose_L1EM22VHI","3j50noL1"]]],
        ['g100_tight_icalovloose_L1EM22VHI_3j50noL1', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g100_tight_icalovloose_L1EM22VHI","3j50noL1"]]],
        ['g85_tight_icalovloose_L1EM24VHI_3j50noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g85_tight_icalovloose_L1EM24VHI","3j50noL1"]]],
        ['g100_tight_icalovloose_L1EM24VHI_3j50noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g100_tight_icalovloose_L1EM24VHI","3j50noL1"]]],

        #ATR-14196
        ['g80_loose_xe80noL1',           'L1_EM22VHI',  ['L1_EM22VHI',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g80_loose","xe80noL1"]]],

        ['g25_medium_mu24',              'L1_MU20MU21', ['L1_EM15VH','L1_MU20'], [PhysicsStream], ['Legacy:Primary','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],

        ['g15_loose_2mu10_msonly',       'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['Legacy:Primary','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],


        #ATR-15243 Higgsino
        #di-muon primary
        ['2mu4_invm1_j20_xe40_pufit_2dphi10_L12MU4_J20_XE30_DPHI-J20s2XE30',  'L1_2MU4_J20_XE30_DPHI-J20s2XE30', [], [PhysicsStream], ['Legacy:Primary','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["2mu4_invm1","j20","xe40_pufit"]]], 

        #di-muon backup
        ['2mu4_invm1_j20_xe60_pufit_2dphi10_L12MU4_J20_XE30_DPHI-J20s2XE30',  'L1_2MU4_J20_XE30_DPHI-J20s2XE30', [], [PhysicsStream], ['Legacy:Backup','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["2mu4_invm1","j20","xe60_pufit"] ]],
        ['2mu4_invm1_j20_xe60_pufit_2dphi10_L12MU4_J40_XE50',  'L1_2MU4_J40_XE50', ['L1_2MU4','L1_J40','L1_XE50'], [PhysicsStream], ['Legacy:Primary','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["2mu4_invm1","j20","xe60_pufit"] ]],

        #single muon backup
        ['mu4_j90_xe90_pufit_2dphi10_L1MU4_J50_XE50_DPHI-J20s2XE30',  'L1_MU4_J50_XE50_DPHI-J20s2XE30', [], [PhysicsStream], ['Legacy:Primary','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["mu4","j90","xe90_pufit"] ]],
        ['mu4_j90_xe90_pufit_2dphi10_L1MU4_XE60',  'L1_MU4_XE60', [], [PhysicsStream], ['Legacy:Primary','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["mu4","j90","xe90_pufit"] ]],

        # mu-e
        ['e5_lhvloose_nod0_mu4_j30_xe40_pufit_2dphi10_L1MU4_J30_XE40_DPHI-J20s2XE30', 'L1_MU4_J30_XE40_DPHI-J20s2XE30', ['L1_EM3','','',''], [PhysicsStream], ['Legacy:Primary','RATE:MuonJetMET', 'BW:Muon', 'BW:Jet'], -1,['serial',-1,["e5_lhvloose_nod0","mu4","j30","xe40_pufit"]]],

        #mu-e backup
        ['e5_lhloose_nod0_mu4_j30_xe40_pufit_2dphi10_L1MU4_J30_XE40_DPHI-J20s2XE30',  'L1_MU4_J30_XE40_DPHI-J20s2XE30', ['L1_EM3','','',''], [PhysicsStream], ['Legacy:Backup','RATE:MuonJetMET', 'BW:Egamma', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["e5_lhloose_nod0","mu4","j30","xe40_pufit"] ]],
        ['e5_lhmedium_nod0_mu4_j30_xe65_pufit_2dphi10_L1MU4_XE60',  'L1_MU4_XE60', ['L1_EM3','L1_MU4','','L1_XE60'], [PhysicsStream], ['Legacy:Primary','RATE:MuonJetMET', 'BW:Egamma', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["e5_lhmedium_nod0","mu4","j30","xe65_pufit"] ]], 

        #di-electron
        ['2e5_lhvloose_nod0_j40_xe70_pufit_2dphi10_L1J40_XE50_DPHI-J20s2XE50',  'L1_J40_XE50_DPHI-J20s2XE50', ['L1_EM3','',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJetMET', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["2e5_lhvloose_nod0","j40","xe70_pufit"] ]],

        #di-electron backup
        ['2e5_lhvloose_nod0_j40_xe70_pufit_2dphi10_L1XE60',  'L1_XE60', ['L1_EM3','',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJetMET', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["2e5_lhvloose_nod0","j40","xe70_pufit_L1XE60"] ]],

        #single electron backup 
        ['e5_lhloose_nod0_j50_xe70_pufit_2dphi10_L1J40_XE50_DPHI-J20s2XE50',  'L1_J40_XE50_DPHI-J20s2XE50', ['L1_EM3','',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJetMET', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e5_lhloose_nod0","j50","xe70_pufit"] ]],
        ['e5_lhmedium_nod0_j50_xe80_pufit_2dphi10_L1J40_XE50_DPHI-J20s2XE50',  'L1_J40_XE50_DPHI-J20s2XE50', ['L1_EM3','',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaJetMET', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e5_lhmedium_nod0","j50","xe80_pufit"] ]],
        ['e5_lhloose_nod0_j40_xe70_pufit_2dphi10_L1XE60',  'L1_XE60', ['L1_EM3','',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJetMET', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e5_lhloose_nod0","j40","xe70_pufit_L1XE60"] ]],

        #ATR-16089 combined egamma support chains
        ['e60_etcut_trkcut_j15_perf_xe60_6dphi05_mt35','L1_EM24VHIM', ['L1_EM24VHIM','',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e60_etcut_trkcut","j15_perf","xe60"]]],

        ['e60_etcut_trkcut_L1EM24VHIM_xs30_j15_perf_xe30_6dphi15_mt35', 'L1_EM24VHIM', ['L1_EM24VHIM','','',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaMET', 'BW:Egamma'], -1, ['serial',-1,["e60_etcut_trkcut_L1EM24VHIM","xs30","j15_perf","xe30"]]],

        ['mu4_j15_boffperf_split_dr05_dz02', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream], ['Legacy:Support','RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu4', 'j15_boffperf_split'] ]],
        ['mu4_j25_boffperf_split_dr05_dz02', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream], ['Legacy:Support','RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu4', 'j25_boffperf_split'] ]],
        ['mu4_j35_boffperf_split_dr05_dz99', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream], ['Legacy:Support','RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu4', 'j35_boffperf_split'] ]],
        ['mu6_j110_gsc150_boffperf_split_dr05_dz99', 'L1_MU6_J40',     ['L1_MU6', ''], [PhysicsStream], ['Legacy:Support','RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j110_gsc150_boffperf_split'] ]],
        ['mu6_j225_gsc320_boffperf_split_dr05_dz99', 'L1_MU6_J75',     ['L1_MU6', ''], [PhysicsStream], ['Legacy:Support','RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j225_gsc320_boffperf_split'] ]],

        ['mu4_j35_boffperf_split', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream], ['Legacy:Support','RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu4', 'j35_boffperf_split'] ]],
        ['mu4_j35_boffperf_split_dr05_dz02', 'L1_MU4',     ['L1_MU4', ''], [PhysicsStream], ['Legacy:Support','RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu4', 'j35_boffperf_split'] ]],
        ['mu6_j60_gsc85_boffperf_split_dr05_dz02',  'L1_MU6_J20', ['L1_MU6', ''], [PhysicsStream], ['Legacy:Support','RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j60_gsc85_boffperf_split'] ]],
        ['mu6_j85_gsc110_boffperf_split_dr05_dz02', 'L1_MU6_J20', ['L1_MU6', ''], [PhysicsStream], ['Legacy:Support','RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j85_gsc110_boffperf_split'] ]],
        ['mu6_j110_gsc150_boffperf_split_dr05_dz02', 'L1_MU6_J40', ['L1_MU6', ''], [PhysicsStream], ['Legacy:Support','RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j110_gsc150_boffperf_split'] ]],

        ['mu6_j110_gsc150_boffperf_split', 'L1_MU6_J40',     ['L1_MU6', ''], [PhysicsStream], ['Legacy:Support','RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j110_gsc150_boffperf_split'] ]],
        ['mu6_j150_gsc175_boffperf_split_dr05_dz02', 'L1_MU6_J40', ['L1_MU6', ''], [PhysicsStream], ['Legacy:Support','RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j150_gsc175_boffperf_split']]],
        ['mu6_j175_gsc260_boffperf_split_dr05_dz02', 'L1_MU6_J75', ['L1_MU6', ''], [PhysicsStream], ['Legacy:Support','RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j175_gsc260_boffperf_split'] ]],
        ['mu6_j225_gsc320_boffperf_split_dr05_dz02', 'L1_MU6_J75', ['L1_MU6', ''], [PhysicsStream], ['Legacy:Support','RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j225_gsc320_boffperf_split'] ]],

        ['mu6_j225_gsc320_boffperf_split', 'L1_MU6_J75',     ['L1_MU6', ''], [PhysicsStream], ['Legacy:Support','RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j225_gsc320_boffperf_split'] ]],
        ['mu6_j225_gsc400_boffperf_split_dr05_dz02', 'L1_MU6_J75', ['L1_MU6', ''], [PhysicsStream], ['Legacy:End-of-fill','RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j225_gsc400_boffperf_split'] ]],

        ### ATR-14355: L1Topo bjet chains
        ['mu4_j35_boffperf_split_dr05_dz02_L1BTAG-MU4J15', 'L1_BTAG-MU4J15', ['L1_MU4', ''], [PhysicsStream], ['Legacy:Support','RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu4', 'j35_boffperf_split'] ], False],
        ['mu6_j60_gsc85_boffperf_split_dr05_dz02_L1BTAG-MU6J20', 'L1_BTAG-MU6J20', ['L1_MU6', ''], [PhysicsStream], ['Legacy:Support','RATE:MuonBJet', 'BW:BJet_combined'], -1,['serial',-1,['mu6', 'j60_gsc85_boffperf_split'] ], False],
 

        # 2mu
        ['2mu14_2j35_boffperf_split',        'L1_2MU10',   ['L1_2MU10',   ''],     [PhysicsStream], ['Legacy:Primary','RATE:MuonBJet',  'BW:BJet_combined'],  -1, ['serial',-1,['2mu14', '2j35_boffperf_split']]],

        # 2e 
        ['2e24_lhvloose_nod0_2j35_boffperf_split',   'L1_2EM20VH', ['L1_2EM20VH', ''],     [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:BJet_combined'],   -1, ['serial',-1,['2e24_lhvloose_nod0', '2j35_boffperf_split']]],

        # e-mu
        ['e28_lhmedium_nod0_L1EM24VHI_mu8noL1_2j35_boffperf_split', 'L1_EM24VHI', ['L1_EM24VHI', '', ''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaMuon',  'BW:BJet_combined'], -1, ['serial',-1,['e28_lhmedium_nod0_L1EM24VHI','mu8noL1','2j35_boffperf_split'] ]],

        # mu +asymmetric
        ['mu26_ivarmedium_j150_boffperf_split_j35_boffperf_split',          'L1_MU20MU21',    ['L1_MU20',    '', ''], [PhysicsStream], ['Legacy:Primary','RATE:MuonBJet',  'BW:BJet_combined'],  -1, ['serial',-1,['mu26_ivarmedium', 'j150_boffperf_split','j35_boffperf_split']]],
        ['mu26_ivarmedium_j110_gsc150_boffperf_split_j35_boffperf_split', 'L1_MU20MU21',    ['L1_MU20',    '', ''], [PhysicsStream], ['Legacy:Primary','RATE:MuonBJet',  'BW:BJet_combined'],  -1, ['serial',-1,['mu26_ivarmedium', 'j110_gsc150_boffperf_split', 'j35_boffperf_split']]],

        # mu + symmetric
        ['mu26_ivarmedium_2j35_boffperf_split', 'L1_MU20MU21',    ['L1_MU20',    '', ''], [PhysicsStream], ['Legacy:Support','RATE:MuonBJet',  'BW:BJet_combined'],  -1, ['serial',-1,['mu26_ivarmedium', '2j35_boffperf_split']]],

        # e
        ['e28_lhtight_nod0_ivarloose_j150_boffperf_split_j35_boffperf_split',               'L1_EM24VHI', ['L1_EM24VHI', '', ''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:BJet_combined'],  -1, ['serial',-1,['e28_lhtight_nod0_ivarloose', 'j150_boffperf_split','j35_boffperf_split']]],
        ['e28_lhtight_nod0_ivarloose_j110_gsc150_boffperf_split_j35_boffperf_split',  'L1_EM24VHI', ['L1_EM24VHI', '', ''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:BJet_combined'],  -1, ['serial',-1,['e28_lhtight_nod0_ivarloose', 'j110_gsc150_boffperf_split','j35_boffperf_split']]],
        ['e28_lhtight_nod0_ivarloose_L1EM24VHIM_2j35_boffperf_split',        'L1_EM24VHIM', ['L1_EM24VHIM', ''], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:BJet_combined'],  -1, ['serial',-1,['e28_lhtight_nod0_ivarloose_L1EM24VHIM', '2j35_boffperf_split']]],

        ['2g10_loose_mu20',          'L1_MU20MU21',    ['L1_2EM7', 'L1_MU20'],  [PhysicsStream], ['Legacy:Primary','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['2g10_loose','mu20'] ]],
        ['2g10_medium_mu20',         'L1_MU20MU21',    ['L1_2EM7', 'L1_MU20'],  [PhysicsStream], ['Legacy:Backup','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['2g10_medium','mu20'] ]],

        # LLP trigger seeded by MUON items
        ['j30_muvtx',                    'L1_2MU10',            [], [PhysicsStream], ['Legacy:Backup','RATE:MuonJet','BW:Muon', 'BW:Jet'], -1],
        ['j30_muvtx_noiso',              'L1_2MU10',            [], [PhysicsStream], ['Legacy:Primary','RATE:MuonJet','BW:Muon', 'BW:Jet'], -1],

        # LLP trigger seeded by TAU items
        ['j30_jes_PS_llp_L1TAU60',             'L1_TAU60',             [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleJet', 'BW:Jet'], -1],
        ['j30_jes_PS_llp_noiso_L1TAU60',       'L1_TAU60',             [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleJet', 'BW:Jet'], -1],
        ['j30_jes_cleanLLP_PS_llp_L1TAU60',             'L1_TAU60',             [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleJet', 'BW:Jet'], -1],
        ['j30_jes_cleanLLP_PS_llp_L1TAU100',             'L1_TAU100',             [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1],

        ['j30_jes_cleanLLP_PS_llp_pufix_L1TAU60',       'L1_TAU60',             [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleJet', 'BW:Jet'], -1],
        ['j30_jes_cleanLLP_PS_llp_pufix_noiso_L1TAU60',       'L1_TAU60',             [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleJet', 'BW:Jet'], -1],
        ['j30_jes_cleanLLP_PS_llp_noiso_L1TAU60',       'L1_TAU60',             [], [PhysicsStream], ['Legacy:End-of-fill','RATE:SingleJet', 'BW:Jet'], -1],
        ['j30_jes_cleanLLP_PS_llp_noiso_L1TAU100',       'L1_TAU100',             [], [PhysicsStream], ['Legacy:Primary','RATE:SingleJet', 'BW:Jet'], -1],


        # L1Topo LLP with TAU
        ['j30_jes_cleanLLP_PS_llp_L1LLP-NOMATCH',       'L1_LLP-NOMATCH',       [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1, False],
        ['j30_jes_cleanLLP_PS_llp_noiso_L1LLP-NOMATCH', 'L1_LLP-NOMATCH',       [], [PhysicsStream], ['Legacy:Primary','RATE:SingleJet', 'BW:Jet'], -1, False],

        ['j30_jes_cleanLLP_PS_llp_L1LLP-RO',       'L1_LLP-RO',       [], [PhysicsStream], ['Legacy:Backup','RATE:SingleJet', 'BW:Jet'], -1, False],
        ['j30_jes_cleanLLP_PS_llp_noiso_L1LLP-RO', 'L1_LLP-RO',       [], [PhysicsStream], ['Legacy:Primary','RATE:SingleJet', 'BW:Jet'], -1, False],

        # reversed calo-ratio triggers
        ['j250_jes_cleanLLP_PS_revllp_L1TAU60',             'L1_TAU60',             [], [PhysicsStream], ['Legacy:Support','RATE:SingleJet', 'BW:Jet'], -1],
        
        ### ATR-14543
        ['g35_tight_icalotight_L1EM24VHIM_mu18noL1',        'L1_EM24VHIM', ['L1_EM24VHIM', ''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_tight_icalotight_L1EM24VHIM','mu18noL1'] ]],
        ['g40_tight_icalotight_L1EM24VHIM_mu18noL1',        'L1_EM24VHIM', ['L1_EM24VHIM', ''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g40_tight_icalotight_L1EM24VHIM','mu18noL1'] ]],
        ['g35_tight_icalotight_L1EM24VHIM_mu15noL1_mu2noL1', 'L1_EM24VHIM', ['L1_EM24VHIM', '',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_tight_icalotight_L1EM24VHIM','mu15noL1','mu2noL1']]],
        ['g40_tight_icalotight_L1EM24VHIM_mu15noL1_mu2noL1', 'L1_EM24VHIM', ['L1_EM24VHIM', '',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g40_tight_icalotight_L1EM24VHIM','mu15noL1','mu2noL1']]],
 
        #ATR-11623 photon + muon/di-muon chains L1_EM24VHI and L1_EM24VHIM seed
        ['g35_tight_icalotight_L1EM24VHI_mu18noL1',        'L1_EM24VHI', ['L1_EM24VHI', ''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_tight_icalotight_L1EM24VHI','mu18noL1'] ]],
        ['g35_tight_icalotight_L1EM24VHI_mu15noL1_mu2noL1', 'L1_EM24VHI', ['L1_EM24VHI', '',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_tight_icalotight_L1EM24VHI','mu15noL1','mu2noL1']]],

        ['g40_tight_icalotight_L1EM24VHI_mu18noL1',        'L1_EM24VHI', ['L1_EM24VHI', ''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g40_tight_icalotight_L1EM24VHI','mu18noL1'] ]],
        ['g40_tight_icalotight_L1EM24VHI_mu15noL1_mu2noL1', 'L1_EM24VHI', ['L1_EM24VHI', '',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g40_tight_icalotight_L1EM24VHI','mu15noL1','mu2noL1']]],

        ['g35_loose_L1EM24VHI_mu18',        'L1_EM24VHI', ['L1_EM24VHI', 'L1_MU11'], [PhysicsStream], ['Legacy:Primary','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_loose_L1EM24VHI','mu18'] ]],
        ['g35_loose_L1EM24VHI_mu15_mu2noL1', 'L1_EM24VHI', ['L1_EM24VHI', 'L1_MU11',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_loose_L1EM24VHI','mu15','mu2noL1']]],

        ['g35_loose_L1EM24VHIM_mu18',        'L1_EM24VHIM', ['L1_EM24VHIM', 'L1_MU11'], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_loose_L1EM24VHIM','mu18'] ]],
        ['g35_loose_L1EM24VHIM_mu15_mu2noL1', 'L1_EM24VHIM', ['L1_EM24VHIM', 'L1_MU11',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['g35_loose_L1EM24VHIM','mu15','mu2noL1']]],

        # Heg chains
        ['e25_mergedtight_g35_medium_Heg',       'L1_2EM20VH', [], [PhysicsStream], ['Legacy:Primary','RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
        ['e30_mergedtight_g35_medium_Heg',       'L1_2EM20VH', [], [PhysicsStream], ['Legacy:Backup','RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],

        # ATR-16089 Heg chains
        ['e25_mergedtight_ivarloose_g35_medium_icalovloose_Heg',       'L1_2EM20VH', [], [PhysicsStream], ['Legacy:Backup','RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]],
 
	# new chain for 2e34 backup (ATR-15225)
        ['e24_lhmedium_nod0_ivarloose_tau35_medium1_tracktwo',  'L1_EM20VHI_TAU20IM_2TAU20_J25_3J20',['L1_EM20VHI','L1_TAU20IM'], [PhysicsStream], ['Legacy:Primary','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e24_lhmedium_nod0_ivarloose","tau35_medium1_tracktwo"]]],
	                                               
        ['e17_lhmedium_nod0_tau80_medium1_tracktwo',   'L1_EM15VHI_TAU40_2TAU15',['L1_EM15VHI','L1_TAU40'], [PhysicsStream], ['Legacy:End-of-fill','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0","tau80_medium1_tracktwo"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo',  'L1_EM15VHI_2TAU12IM_J25_3J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream],['Legacy:Primary','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwo"]]],

        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1EM15VHI_2TAU12IM_4J12',   'L1_EM15VHI_2TAU12IM_4J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream],['Legacy:Primary','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwo"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1EM15VHI_2TAU12IM',  'L1_EM15VHI_2TAU12IM',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['Legacy:Support','RATE:ElectronTau', 'BW:Tau'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwo"]]],

	#L1Topo e+tau chains
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25', 'L1_DR-EM15TAU12I-J25',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:ElectronTau', 'BW:Tau'],-1,['serial',-1 ,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwo"]]],
        ['mu14_ivarloose_tau35_medium1_tracktwo',  'L1_MU10_TAU20IM',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['Legacy:Primary','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_medium1_tracktwo"]]],
        ['mu14_ivarloose_L1MU11_tau35_medium1_tracktwo_L1MU11_TAU20IM',  'L1_MU11_TAU20IM',['L1_MU11','L1_TAU20IM'], [PhysicsStream], ['Legacy:Primary','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose_L1MU11","tau35_medium1_tracktwo"]]],
        ['mu14_ivarloose_tau35_medium1_tracktwo_L1MU10_TAU20IM_J25_2J20',      'L1_MU10_TAU20IM_J25_2J20',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['Legacy:Backup','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_medium1_tracktwo"]]],   

        ['mu14_iloose_tau25_perf_tracktwo', 'L1_MU10_TAU12IM',['L1_MU10','L1_TAU12IM'], [PhysicsStream, 'express'], ['Legacy:Support','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_iloose","tau25_perf_tracktwo"]]],
        ['mu14_ivarloose_tau25_medium1_tracktwo',      'L1_MU10_TAU12IM_J25_2J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwo"]]],        
        ['mu14_ivarloose_tau25_medium1_tracktwo_L1MU10_TAU12IM_3J12',      'L1_MU10_TAU12IM_3J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwo"]]],
        ['mu14_ivarloose_tau25_medium1_tracktwo_L1MU10_TAU12IM',  'L1_MU10_TAU12IM',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Legacy:Support','RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwo"]]],

        # l1topo tau+mu chains
        ['mu14_ivarloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25', 'L1_DR-MU10TAU12I_TAU12I-J25',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwo"]]],

        
        # Tau + MET
        ['tau35_medium1_tracktwo_xe70_L1XE45',  'L1_TAU20IM_2J20_XE45',['L1_TAU20IM','L1_XE45'], [PhysicsStream], ['Legacy:Primary','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","xe70_L1XE45"]]],  
        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_xe50', 'L1_TAU20IM_2TAU12IM_XE35',['L1_TAU20IM','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Legacy:End-of-fill','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo","xe50"]]],
        ['tau60_medium1_tracktwo_tau25_medium1_tracktwo_xe50', 'L1_TAU40_2TAU12IM_XE40',['L1_TAU40','L1_TAU12IM','L1_XE40'], [PhysicsStream], ['Legacy:Primary','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau60_medium1_tracktwo","tau25_medium1_tracktwo","xe50"]]],
 
        # Tau + e + MET
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_xe50', 'L1_EM15VHI_2TAU12IM_XE35',['L1_EM15VHI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Legacy:Backup','RATE:TauMET', 'BW:Tau', 'BW:Egamma'],-1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwo", "xe50"]]],
        ['e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50', 'L1_EM15VHI_2TAU12IM_XE35',['L1_EM15VHI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Legacy:Primary','RATE:TauMET', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_medium1_tracktwo", "xe50"]]],


        # Tau + mu + MET
        ['mu14_tau25_medium1_tracktwo_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Legacy:Primary','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14", "tau25_medium1_tracktwo","xe50"]]],
        ['mu14_ivarloose_tau25_medium1_tracktwo_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Legacy:Backup','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose", "tau25_medium1_tracktwo","xe50"]]],

        # ATR-17807
        ['tau60_medium1_tracktwoEF_tau25_medium1_tracktwoEF_xe50', 'L1_TAU40_2TAU12IM_XE40',['L1_TAU40','L1_TAU12IM','L1_XE40'], [PhysicsStream], ['Legacy:Primary','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau60_medium1_tracktwoEF","tau25_medium1_tracktwoEF","xe50"]]],
        ['tau60_mediumRNN_tracktwoMVA_tau25_mediumRNN_tracktwoMVA_xe50', 'L1_TAU40_2TAU12IM_XE40',['L1_TAU40','L1_TAU12IM','L1_XE40'], [PhysicsStream], ['Legacy:Primary','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau60_mediumRNN_tracktwoMVA","tau25_mediumRNN_tracktwoMVA","xe50"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwoEF_xe50', 'L1_EM15VHI_2TAU12IM_XE35',['L1_EM15VHI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Legacy:Backup','RATE:TauMET', 'BW:Tau', 'BW:Egamma'],-1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwoEF", "xe50"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_mediumRNN_tracktwoMVA_xe50', 'L1_EM15VHI_2TAU12IM_XE35',['L1_EM15VHI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Legacy:Backup','RATE:TauMET', 'BW:Tau', 'BW:Egamma'],-1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_mediumRNN_tracktwoMVA", "xe50"]]],
        ['e17_lhmedium_nod0_tau25_medium1_tracktwoEF_xe50', 'L1_EM15VHI_2TAU12IM_XE35',['L1_EM15VHI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Legacy:Primary','RATE:TauMET', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_medium1_tracktwoEF", "xe50"]]],
        ['e17_lhmedium_nod0_tau25_mediumRNN_tracktwoMVA_xe50', 'L1_EM15VHI_2TAU12IM_XE35',['L1_EM15VHI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Legacy:Primary','RATE:TauMET', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_mediumRNN_tracktwoMVA", "xe50"]]],
        ['mu14_ivarloose_tau25_medium1_tracktwoEF',      'L1_MU10_TAU12IM_J25_2J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwoEF"]]],        
        ['mu14_ivarloose_tau25_mediumRNN_tracktwoMVA',      'L1_MU10_TAU12IM_J25_2J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],        
        ['mu14_ivarloose_tau25_medium1_tracktwoEF_L1DR-MU10TAU12I_TAU12I-J25', 'L1_DR-MU10TAU12I_TAU12I-J25',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwoEF"]]],
        ['mu14_ivarloose_tau25_mediumRNN_tracktwoMVA_L1DR-MU10TAU12I_TAU12I-J25', 'L1_DR-MU10TAU12I_TAU12I-J25',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwoEF_L1DR-EM15TAU12I-J25', 'L1_DR-EM15TAU12I-J25',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:ElectronTau', 'BW:Tau'],-1,['serial',-1 ,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwoEF"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_mediumRNN_tracktwoMVA_L1DR-EM15TAU12I-J25', 'L1_DR-EM15TAU12I-J25',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:ElectronTau', 'BW:Tau'],-1,['serial',-1 ,["e17_lhmedium_nod0_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwoEF',  'L1_EM15VHI_2TAU12IM_J25_3J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream],['Legacy:Primary','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwoEF"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_mediumRNN_tracktwoMVA',  'L1_EM15VHI_2TAU12IM_J25_3J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream],['Legacy:Primary','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],
        ['e24_lhmedium_nod0_ivarloose_tau35_medium1_tracktwoEF',  'L1_EM20VHI_TAU20IM_2TAU20_J25_3J20',['L1_EM20VHI','L1_TAU20IM'], [PhysicsStream], ['Legacy:Primary','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e24_lhmedium_nod0_ivarloose","tau35_medium1_tracktwoEF"]]],
        ['e24_lhmedium_nod0_ivarloose_tau35_mediumRNN_tracktwoMVA',  'L1_EM20VHI_TAU20IM_2TAU20_J25_3J20',['L1_EM20VHI','L1_TAU20IM'], [PhysicsStream], ['Legacy:Primary','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e24_lhmedium_nod0_ivarloose","tau35_mediumRNN_tracktwoMVA"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwoEF_L1EM15VHI_2TAU12IM_4J12',   'L1_EM15VHI_2TAU12IM_4J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream],['Legacy:Primary','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwoEF"]]],
        ['e17_lhmedium_nod0_ivarloose_tau25_mediumRNN_tracktwoMVA_L1EM15VHI_2TAU12IM_4J12',   'L1_EM15VHI_2TAU12IM_4J12',['L1_EM15VHI','L1_TAU12IM'], [PhysicsStream],['Legacy:Primary','RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],
        ['mu14_ivarloose_tau35_medium1_tracktwoEF_L1MU10_TAU20IM_J25_2J20',      'L1_MU10_TAU20IM_J25_2J20',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['Legacy:Backup','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_medium1_tracktwoEF"]]],   
        ['mu14_ivarloose_tau35_mediumRNN_tracktwoMVA_L1MU10_TAU20IM_J25_2J20',      'L1_MU10_TAU20IM_J25_2J20',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['Legacy:Backup','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_mediumRNN_tracktwoMVA"]]],   
        ['mu14_ivarloose_tau25_medium1_tracktwoEF_L1MU10_TAU12IM_3J12',      'L1_MU10_TAU12IM_3J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwoEF"]]],
        ['mu14_ivarloose_tau25_mediumRNN_tracktwoMVA_L1MU10_TAU12IM_3J12',      'L1_MU10_TAU12IM_3J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['Legacy:Primary','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_mediumRNN_tracktwoMVA"]]],
        ['mu14_ivarloose_tau35_medium1_tracktwoEF',  'L1_MU10_TAU20IM',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['Legacy:Primary','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_medium1_tracktwoEF"]]],
        ['mu14_ivarloose_tau35_mediumRNN_tracktwoMVA',  'L1_MU10_TAU20IM',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['Legacy:Primary','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_mediumRNN_tracktwoMVA"]]],
        ['mu14_ivarloose_L1MU11_tau35_medium1_tracktwoEF_L1MU11_TAU20IM',  'L1_MU11_TAU20IM',['L1_MU11','L1_TAU20IM'], [PhysicsStream], ['Legacy:Primary','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose_L1MU11","tau35_medium1_tracktwoEF"]]],
        ['mu14_ivarloose_L1MU11_tau35_mediumRNN_tracktwoMVA_L1MU11_TAU20IM',  'L1_MU11_TAU20IM',['L1_MU11','L1_TAU20IM'], [PhysicsStream], ['Legacy:Primary','RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose_L1MU11","tau35_mediumRNN_tracktwoMVA"]]],
        ['mu14_tau25_medium1_tracktwoEF_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Legacy:Primary','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14", "tau25_medium1_tracktwoEF","xe50"]]],
        ['mu14_tau25_mediumRNN_tracktwoMVA_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Legacy:Primary','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14", "tau25_mediumRNN_tracktwoMVA","xe50"]]],
        ['mu14_ivarloose_tau25_medium1_tracktwoEF_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Legacy:Backup','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose", "tau25_medium1_tracktwoEF","xe50"]]],
        ['mu14_ivarloose_tau25_mediumRNN_tracktwoMVA_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['Legacy:Backup','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose", "tau25_mediumRNN_tracktwoMVA","xe50"]]],
        ['tau35_medium1_tracktwoEF_xe70_L1XE45',  'L1_TAU20IM_2J20_XE45',['L1_TAU20IM','L1_XE45'], [PhysicsStream], ['Legacy:Primary','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwoEF","xe70_L1XE45"]]],  
        ['tau35_mediumRNN_tracktwoMVA_xe70_L1XE45',  'L1_TAU20IM_2J20_XE45',['L1_TAU20IM','L1_XE45'], [PhysicsStream], ['Legacy:Primary','RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_mediumRNN_tracktwoMVA","xe70_L1XE45"]]],  


        # ATR-17807
        #tracktwoEF
        ['g25_medium_L1EM24VHI_tau25_dikaonmass_tracktwoEF_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Legacy:Primary','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dikaonmass_tracktwoEF'],False] ],
        ['g25_medium_L1EM24VHI_tau25_dipion1loose_tracktwoEF_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Legacy:Primary','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dipion1loose_tracktwoEF'],False] ],
        ['g25_medium_L1EM24VHI_tau25_dipion2_tracktwoEF_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Legacy:Primary','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dipion2_tracktwoEF'],False] ],
        ['g25_medium_L1EM24VHI_tau25_kaonpi1_tracktwoEF_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Legacy:Primary','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_kaonpi1_tracktwoEF'],False] ],
        ['g25_medium_L1EM24VHI_tau25_kaonpi2_tracktwoEF_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Legacy:Primary','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_kaonpi2_tracktwoEF'],False] ],
        ['g25_medium_L1EM24VHI_tau25_singlepion_tracktwoEF_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Legacy:Primary','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_singlepion_tracktwoEF'],False] ],
	    ['g35_medium_L1EM24VHI_tau25_dipion3_tracktwoEF_60mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Legacy:Primary','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g35_medium_L1EM24VHI','tau25_dipion3_tracktwoEF'],False] ],
        #tracktwoMVA
        ['g25_medium_L1EM24VHI_tau25_dikaonmass_tracktwoMVA_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Legacy:Primary','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dikaonmass_tracktwoMVA'],False] ],
        ['g25_medium_L1EM24VHI_tau25_dipion1loose_tracktwoMVA_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Legacy:Primary','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dipion1loose_tracktwoMVA'],False] ],
        ['g25_medium_L1EM24VHI_tau25_dipion2_tracktwoMVA_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Legacy:Primary','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dipion2_tracktwoMVA'],False] ],
        ['g25_medium_L1EM24VHI_tau25_kaonpi1_tracktwoMVA_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Legacy:Primary','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_kaonpi1_tracktwoMVA'],False] ],
        ['g25_medium_L1EM24VHI_tau25_kaonpi2_tracktwoMVA_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Legacy:Primary','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_kaonpi2_tracktwoMVA'],False] ],
        ['g25_medium_L1EM24VHI_tau25_singlepion_tracktwoMVA_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Legacy:Primary','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_singlepion_tracktwoMVA'],False] ],
        ['g35_medium_L1EM24VHI_tau25_dipion3_tracktwoMVA_60mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Legacy:Primary','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g35_medium_L1EM24VHI','tau25_dipion3_tracktwoMVA'],False] ],

        ['j80_xe80',               'L1_J40_XE50',['',''], [PhysicsStream], ['Legacy:Support','RATE:JetMET', 'BW:Jet', 'BW:MET'], -1,['serial',-1,["j80","xe80"]]], 

        # lepton + jets 
        ['e20_lhtight_nod0_3j20_L1EM18VHI_3J20',      'L1_EM18VHI_3J20',    ['L1_EM18VHI',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1, ['serial',-1,["e20_lhtight_nod0","3j20"] ]],

        ### ATR-15524
        ['e20_lhtight_nod0_ivarloose_3j20_L1EM18VHI_3J20', 'L1_EM18VHI_3J20',    ['L1_EM18VHI',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1, ['serial',-1,["e20_lhtight_nod0_ivarloose","3j20"] ]],
        ['e20_lhmedium_nod0_3j20_L1EM18VHI_3J20', 'L1_EM18VHI_3J20',    ['L1_EM18VHI',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1, ['serial',-1,["e20_lhmedium_nod0","3j20"] ]],
         
        #nod0 chains:
        ['e28_lhvloose_nod0_L1EM22VH_3j20noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e28_lhvloose_nod0_L1EM22VH","3j20noL1"] ]],
        ['e28_lhvloose_nod0_L1EM22VH_4j20noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e28_lhvloose_nod0_L1EM22VH","4j20noL1"] ]],
        ['e28_lhvloose_nod0_L1EM22VH_5j15noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e28_lhvloose_nod0_L1EM22VH","5j15noL1"] ]],
        ['e28_lhvloose_nod0_L1EM22VH_6j15noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e28_lhvloose_nod0_L1EM22VH","6j15noL1"] ]],
        ['e26_lhvloose_nod0_L1EM22VH_3j20noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e26_lhvloose_nod0_L1EM22VH","3j20noL1"] ]],
        ['e26_lhvloose_nod0_L1EM22VH_4j20noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e26_lhvloose_nod0_L1EM22VH","4j20noL1"] ]],
        ['e26_lhvloose_nod0_L1EM22VH_5j15noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e26_lhvloose_nod0_L1EM22VH","5j15noL1"] ]],
        ['e26_lhvloose_nod0_L1EM22VH_6j15noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e26_lhvloose_nod0_L1EM22VH","6j15noL1"] ]],

        ['e28_lhmedium_nod0_L1EM22VH_3j20noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e28_lhmedium_nod0_L1EM22VH","3j20noL1"] ]],
        ['e28_lhmedium_nod0_L1EM22VH_4j20noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e28_lhmedium_nod0_L1EM22VH","4j20noL1"] ]],
        ['e28_lhmedium_nod0_L1EM22VH_5j15noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e28_lhmedium_nod0_L1EM22VH","5j15noL1"] ]],
        ['e28_lhmedium_nod0_L1EM22VH_6j15noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e28_lhmedium_nod0_L1EM22VH","6j15noL1"] ]],
        ['e26_lhmedium_nod0_L1EM22VH_3j20noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e26_lhmedium_nod0_L1EM22VH","3j20noL1"] ]],
        ['e26_lhmedium_nod0_L1EM22VH_4j20noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e26_lhmedium_nod0_L1EM22VH","4j20noL1"] ]],
        ['e26_lhmedium_nod0_L1EM22VH_5j15noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e26_lhmedium_nod0_L1EM22VH","5j15noL1"] ]],
        ['e26_lhmedium_nod0_L1EM22VH_6j15noL1',  'L1_EM22VH', ['L1_EM22VH',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1, ['serial',-1,["e26_lhmedium_nod0_L1EM22VH","6j15noL1"] ]],


        ['mu26_2j20noL1',  'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Legacy:Support','RATE:MuonJet', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["mu26","2j20noL1"] ]],
        ['mu26_3j20noL1',  'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Legacy:Support','RATE:MuonJet', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["mu26","3j20noL1"] ]],
        ['mu26_4j15noL1',  'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Legacy:Support','RATE:MuonJet', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["mu26","4j15noL1"] ]],
        ['mu26_5j15noL1',  'L1_MU20MU21', ['L1_MU20',''], [PhysicsStream], ['Legacy:Support','RATE:MuonJet', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["mu26","5j15noL1"] ]],
         

        # VBF triggers
        ['g25_medium_L1EM22VHI_4j35_0eta490_invm700', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma'], -1,['serial',-1,["g25_medium_L1EM22VHI","4j35_0eta490_invm700"]]],
        #ATR-14196
        ['g25_medium_L1EM22VHI_4j35_0eta490_invm1000', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:Egamma'], -1,['serial',-1,["g25_medium_L1EM22VHI","4j35_0eta490_invm1000"]]],
        ['g25_loose_L1EM20VH_4j35_0eta490', 'L1_EM20VH', ['L1_EM20VH',''], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma'], -1,['serial',-1,["g25_loose_L1EM20VH","4j35_0eta490"]]],
	
        ['g25_medium_L1EM22VHI_j35_0eta490_bmv2c1077_split_3j35_0eta490_invm700', 'L1_EM22VHI', ['L1_EM22VHI', '', ''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:BJet_combined','BW:Egamma'],-1,['serial',-1,['g25_medium_L1EM22VHI','3j35_0eta490_invm700','j35_0eta490_bmv2c1077_split']]],
        ['g25_medium_L1EM22VHI_2j35_0eta490_bmv2c1077_split_2j35_0eta490', 'L1_EM22VHI', ['L1_EM22VHI', '', ''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:BJet_combined','BW:Egamma'],-1,['serial',-1,['g25_medium_L1EM22VHI','2j35_0eta490','2j35_0eta490_bmv2c1077_split']]],


        #ATR-15062
        ['g20_tight_icaloloose_j35_bmv2c1077_split_3j35_0eta490_invm500', 'L1_EM18VHI_MJJ-300', ['','',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g20_tight_icaloloose","3j35_0eta490_invm500","j35_bmv2c1077_split"]]],
        ['g27_medium_L1EM24VHI_j35_bmv2c1077_split_3j35_0eta490_invm700', 'L1_EM24VHI', ['L1_EM24VHI','',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g27_medium_L1EM24VHI","3j35_0eta490_invm700","j35_bmv2c1077_split"]]],
        ['g27_medium_L1EM24VHI_2j35_bmv2c1077_split_2j35_0eta490', 'L1_EM24VHI', ['L1_EM24VHI','',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g27_medium_L1EM24VHI","2j35_0eta490","2j35_bmv2c1077_split"]]],

        # backups for L1Topo VBF items
        ['j80_0eta240_j60_j45_320eta490_AND_2j35_gsc45_bmv2c1070_split','L1_J40.0ETA25_2J25_J20.31ETA49', ['','','','',''], [PhysicsStream], ['Legacy:Primary','RATE:MultiBJet', 'BW:BJet_combined' ], -1, ['serial',-1,['j80_0eta240','j60','j45_320eta490_AND','2j35_gsc45_bmv2c1070_split'] ]],

	    ['j55_0eta240_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49','L1_J25.0ETA23_2J15.31ETA49',['',''], [PhysicsStream], ['Legacy:Support','RATE:MultiBJet', 'BW:BJet_combined' ], -1, ['serial',-1,['j55_0eta240','2j45_320eta490_L1J25.0ETA23_2J15.31ETA49'] ]],
        ['j45_gsc55_bmv2c1070_split_2j45_320eta490_L1J25.0ETA23_2J15.31ETA49','L1_J25.0ETA23_2J15.31ETA49',['',''], [PhysicsStream], ['Legacy:Primary','RATE:MultiBJet', 'BW:BJet_combined' ], -1, ['serial',-1,['2j45_320eta490_L1J25.0ETA23_2J15.31ETA49','j45_gsc55_bmv2c1070_split'] ]],

 
        # 2017 g+tau chains (ATR-16266)
	# ATR-14947
        ['g25_medium_L1EM24VHIM_tau25_dikaonmass_tracktwo_50mVis10000','L1_EM24VHIM', ['L1_EM24VHIM','L1_TAU12'], [PhysicsStream], ['Legacy:Backup','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHIM','tau25_dikaonmass_tracktwo'],False]],
        ['g25_medium_L1EM24VHI_tau25_dikaonmass_tracktwo_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Legacy:Primary','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dikaonmass_tracktwo'],False]],
        ['g25_medium_L1EM24VHIM_tau25_singlepion_tracktwo_50mVis10000','L1_EM24VHIM', ['L1_EM24VHIM','L1_TAU12'], [PhysicsStream], ['Legacy:Backup','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHIM','tau25_singlepion_tracktwo'],False]],
        ['g25_medium_L1EM24VHI_tau25_singlepion_tracktwo_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Legacy:Primary','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_singlepion_tracktwo'],False]],
        ['g25_medium_L1EM24VHIM_tau25_dipion1loose_tracktwo_50mVis10000','L1_EM24VHIM', ['L1_EM24VHIM','L1_TAU12'], [PhysicsStream], ['Legacy:Backup','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHIM','tau25_dipion1loose_tracktwo'],False]],
        ['g25_medium_L1EM24VHI_tau25_dipion1loose_tracktwo_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Legacy:Primary','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dipion1loose_tracktwo'],False]],
        ['g25_medium_L1EM24VHIM_tau25_dipion2_tracktwo_50mVis10000','L1_EM24VHIM', ['L1_EM24VHIM','L1_TAU12'], [PhysicsStream], ['Legacy:Backup','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHIM','tau25_dipion2_tracktwo'],False]],
        ['g25_medium_L1EM24VHI_tau25_dipion2_tracktwo_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Legacy:Primary','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dipion2_tracktwo'],False]],
        ['g25_medium_L1EM24VHIM_tau25_kaonpi1_tracktwo_50mVis10000','L1_EM24VHIM', ['L1_EM24VHIM','L1_TAU12'], [PhysicsStream], ['Legacy:Backup','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHIM','tau25_kaonpi1_tracktwo'],False]],
        ['g25_medium_L1EM24VHI_tau25_kaonpi1_tracktwo_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Legacy:Primary','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_kaonpi1_tracktwo'],False]],
        ['g25_medium_L1EM24VHIM_tau25_kaonpi2_tracktwo_50mVis10000','L1_EM24VHIM', ['L1_EM24VHIM','L1_TAU12'], [PhysicsStream], ['Legacy:Backup','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHIM','tau25_kaonpi2_tracktwo'],False]],
        ['g25_medium_L1EM24VHI_tau25_kaonpi2_tracktwo_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Legacy:Primary','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_kaonpi2_tracktwo'],False]],	
        # new L1topo g+tau chain (ATR-14349)
        ['g25_medium_tau25_dikaonmass_tracktwo_50mVis10000_L130M-EM20ITAU12','L1_30M-EM20ITAU12', ['L1_EM20VHI','L1_TAU12'], [PhysicsStream], ['Legacy:End-of-fill','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium','tau25_dikaonmass_tracktwo'],False]],
        ['g25_medium_tau25_singlepion_tracktwo_50mVis10000_L130M-EM20ITAU12','L1_30M-EM20ITAU12', ['L1_EM20VHI','L1_TAU12'], [PhysicsStream], ['Legacy:End-of-fill','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium','tau25_singlepion_tracktwo'],False]],
        ['g25_medium_tau25_dipion1loose_tracktwo_50mVis10000_L130M-EM20ITAU12','L1_30M-EM20ITAU12', ['L1_EM20VHI','L1_TAU12'], [PhysicsStream], ['Legacy:End-of-fill','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium','tau25_dipion1loose_tracktwo'],False]],
        ['g25_medium_tau25_dipion2_tracktwo_50mVis10000_L130M-EM20ITAU12','L1_30M-EM20ITAU12', ['L1_EM20VHI','L1_TAU12'], [PhysicsStream], ['Legacy:End-of-fill','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium','tau25_dipion2_tracktwo'],False]],
        ['g25_medium_tau25_kaonpi1_tracktwo_50mVis10000_L130M-EM20ITAU12','L1_30M-EM20ITAU12', ['L1_EM20VHI','L1_TAU12'], [PhysicsStream], ['Legacy:End-of-fill','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium','tau25_kaonpi1_tracktwo'],False]],
        ['g25_medium_tau25_kaonpi2_tracktwo_50mVis10000_L130M-EM20ITAU12','L1_30M-EM20ITAU12', ['L1_EM20VHI','L1_TAU12'], [PhysicsStream], ['Legacy:End-of-fill','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium','tau25_kaonpi2_tracktwo'],False]],

        ### ATR-14196
        ['g85_tight_L1EM24VHIM_3j50noL1', 'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g85_tight_L1EM24VHIM","3j50noL1"]]],
        ['g85_tight_icalovloose_L1EM24VHIM_3j50noL1', 'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g85_tight_icalovloose_L1EM24VHIM","3j50noL1"]]],
        ['g100_tight_L1EM24VHIM_3j50noL1', 'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g100_tight_L1EM24VHIM","3j50noL1"]]],
        ['g100_tight_icalovloose_L1EM24VHIM_3j50noL1', 'L1_EM24VHIM', ['L1_EM24VHIM',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g100_tight_icalovloose_L1EM24VHIM","3j50noL1"]]],
       	['g85_tight_L1EM22VHI_3j50noL1', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g85_tight_L1EM22VHI","3j50noL1"]]],
        ['g85_tight_L1EM24VHI_3j50noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g85_tight_L1EM24VHI","3j50noL1"]]],
        ['g100_tight_L1EM22VHI_3j50noL1', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g100_tight_L1EM22VHI","3j50noL1"]]],
        ['g100_tight_L1EM24VHI_3j50noL1', 'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g100_tight_L1EM24VHI","3j50noL1"]]],

        ['g85_tight', 'L1_EM22VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:ElectronPhoton', 'BW:Egamma'], -1],
        ['g85_tight_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:ElectronPhoton', 'BW:Egamma'], -1],
        ['g100_tight', 'L1_EM22VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:ElectronPhoton', 'BW:Egamma'], -1],
        ['g100_tight_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['Legacy:Rerun','RATE:ElectronPhoton', 'BW:Egamma'], -1],
     #ATR-14196
        ['j80_bmv2c1050_split_xe60_L12J50_XE40','L1_2J50_XE40',  [],  [PhysicsStream], ['Legacy:Primary','RATE:JetMET', 'BW:MET', 'BW:BJet_combined'], -1,['serial',-1,["xe60","j80_bmv2c1050_split"]]],
        ['j40_cleanT_xe85_tc_lcw_2dphi05_L1XE50',         'L1_XE50', ['',''], [PhysicsStream], ['Legacy:Support','RATE:JetMET', 'BW:Jet'], -1, ['serial',-1,["j40_cleanT","xe85_tc_lcw_L1XE50"]]],
        ['mu14_ivarloose_3j20_L1MU10_3J20', 'L1_MU10_3J20', ['L1_MU10_3J20', ''], [PhysicsStream], ['Legacy:Primary','RATE:MuonJet', 'BW:Muon','BW:Jet'], -1,['serial',-1,['mu14_ivarloose', '3j20'] ],True],
        ### ATR-15524
        ['mu14_3j20_L1MU10_3J20', 'L1_MU10_3J20', ['L1_MU10_3J20', ''], [PhysicsStream], ['Legacy:Support','RATE:MuonJet', 'BW:Muon','BW:Jet'], -1,['serial',-1,['mu14', '3j20'] ],True],

        
        ['mu6_ivarloose_2j40_0eta490_invm1000_L1MU6_J30.0ETA49_2J20.0ETA49', 'L1_MU6_J30.0ETA49_2J20.0ETA49', ['L1_MU6',''], [PhysicsStream], ['Legacy:Support','RATE:MuonJet', 'BW:Muon', 'BW:Jet'], -1, ['serial',-1,["mu6_ivarloose","2j40_0eta490_invm1000"]]],


        # with pi0's (ATR-16600)
        ['g35_medium_L1EM24VHI_tau25_dipion3_tracktwo_60mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['Legacy:Primary','RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g35_medium_L1EM24VHI','tau25_dipion3_tracktwo'],False]],

        ## Lepton + jets with unisolated leptons:
        # For above 1.0 e34
        ['e26_lhmedium_nod0_5j30_0eta240_L1EM20VH_3J20',    	'L1_EM20VH_3J20',    ['L1_EM20VH',''], [PhysicsStream], ['Legacy:End-of-fill','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1, ['serial',-1,["e26_lhmedium_nod0","5j30_0eta240"] ]],
        ['mu26_5j30_0eta240_L1MU10_3J20',    	'L1_MU10_3J20',    ['L1_MU10',''], [PhysicsStream], ['Legacy:End-of-fill','RATE:MuonJet', 'BW:Muon', 'BW:Jet'],-1, ['serial',-1,["mu26","5j30_0eta240"] ]],
        # For below 1.0 e34
        ['e9_lhvloose_nod0_mu20_mu8noL1',        'L1_MU20MU21',  ['L1_EM3', 'L1_MU20', ''],   [PhysicsStream], ['Legacy:Primary','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['serial',-1,['mu20','mu8noL1','e9_lhvloose_nod0'] ]],
        ['e9_lhvloose_nod0_mu20_mu8noL1_L1EM8VH_MU20',  'L1_EM8VH_MU20',['L1_EM8VH','L1_MU20',''], [PhysicsStream], ['Legacy:Backup','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['serial',-1,['mu20','mu8noL1','e9_lhvloose_nod0'] ]],
        ['e9_lhvloose_nod0_mu20_mu8noL1_L1EM7_MU20',    'L1_EM7_MU20',  ['L1_EM7','L1_MU20',''],   [PhysicsStream], ['Legacy:Primary','RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['serial',-1,['mu20','mu8noL1','e9_lhvloose_nod0'] ]],


        #ATR-17462 low pt photon triggers
        ['2g6_tight_icalotight_L1J100', 'L1_J100', ['L1_2EM3'],  [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1],
        ['2g6_loose_L1J100', 'L1_J100', ['L1_2EM3'],  [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1],

        #ATR-17462
        ['2g6_tight_icalotight_L1J50', 'L1_J50', ['L1_2EM3','L1_J50'], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1],
        ['2g6_loose_L1J50', 'L1_J50', ['L1_2EM3','L1_J50'], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1],
        ['2g6_tight_icalotight_L1J40', 'L1_J40', ['L1_2EM3','L1_J40'], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1],
        ['2g6_loose_L1J40', 'L1_J40', ['L1_2EM3','L1_J40'], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'],-1],


        # photon + mulit-jets ATR-17502
        ['g45_loose_6j45_0eta240', 'L1_4J15.0ETA25', ['L1_EM15VH','L1_4J15.0ETA25'], [PhysicsStream], ['Legacy:Primary','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g45_loose","6j45_0eta240"]]],
        ['g45_loose_5j45_0eta240', 'L1_4J15.0ETA25', ['L1_EM15VH','L1_4J15.0ETA25'], [PhysicsStream], ['Legacy:Support','RATE:EgammaJet', 'BW:Egamma', 'BW:Jet'], -1,['serial',-1,["g45_loose","5j45_0eta240"]]],


        ['e9_lhvloose_nod0_e5_lhvloose_nod0_bBeexM6000t_mu6_nomucomb_L1BPH-0M9-EM7-EM5_MU6',  'L1_BPH-0M9-EM7-EM5_MU6',   ['L1_EM7','L1_EM3','L1_MU6'], [BPhysicsStream], ['Legacy:End-of-fill',RATE_BphysTag,BW_BphysTag],-1,['parallel',-1,[] ]],
        ['e5_lhvloose_nod0_bBeexM6000t_mu6_nomucomb_L1BPH-0DR3-EM7J15_MU6',  'L1_BPH-0DR3-EM7J15_MU6',   ['L1_EM7','L1_MU6'], [BPhysicsStream], ['Legacy:End-of-fill',RATE_BphysTag,BW_BphysTag],-1,['parallel',-1,[] ]],
        ['e9_lhvloose_nod0_e5_lhvloose_nod0_bBeexM6000t_2mu4_nomucomb_L1BPH-0M9-EM7-EM5_2MU4',  'L1_BPH-0M9-EM7-EM5_2MU4',   ['L1_EM7','L1_EM3','L1_2MU4'], [BPhysicsStream], ['Legacy:Primary',RATE_BphysTag,BW_BphysTag],-1,['parallel',-1,[] ]],
        ['e5_lhvloose_nod0_bBeexM6000t_2mu4_nomucomb_L1BPH-0DR3-EM7J15_2MU4',  'L1_BPH-0DR3-EM7J15_2MU4',   ['L1_EM7','L1_2MU4'], [BPhysicsStream], ['Legacy:End-of-fill',RATE_BphysTag,BW_BphysTag],-1,['parallel',-1,[] ]],

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
