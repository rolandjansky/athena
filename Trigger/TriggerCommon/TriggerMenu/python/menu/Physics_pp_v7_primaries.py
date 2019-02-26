# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#----------phy--------------------------------------------------------------#
#------------------------------------------------------------------------#
def setupMenu():

    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenu.menu.Physics_pp_v7.py' )

    from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1 as tcl1
    if tcl1.current:
        log.info("L1 items: %s " % tcl1.current.menu.items.itemNames())
    else:
        log.info("ERROR L1 menu has not yet been defined")

    PhysicsStream="Main"

    # stream, BW and RATE tags for Bphysics items that appear in Muon and Bphysics slice.signatures
    BPhysicsStream     = "BphysLS"
    RATE_BphysTag      = 'RATE:Bphysics'
    BW_BphysTag        = 'BW:Bphys'


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
        # single muon
        ['mu26_ivarmedium',	     'L1_MU20', 	  [], [PhysicsStream, 'express'], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu28_ivarmedium',	     'L1_MU20', 	  [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],

        ['mu80',                  'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu50',                  'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu60',                  'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu60_0eta105_msonly',   'L1_MU20',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],

        # multi muons
        ['2mu14',                  'L1_2MU10',          [], [PhysicsStream, 'express'], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu15_L12MU10',          'L1_2MU10',          [], [PhysicsStream, 'express'], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu6',                   'L1_3MU6',           [], [PhysicsStream, 'express'], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu6_msonly',            'L1_3MU6',           [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['3mu8_msonly',            'L1_3MU6',           [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],

        ['mu24_mu10noL1',          'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','mu10noL1']]],
        ['mu24_2mu4noL1',          'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','2mu4noL1']]],

        ['mu26_mu8noL1',           'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu26','mu8noL1']]],
        ['mu26_mu10noL1',          'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu26','mu10noL1']]],

        ['mu28_mu8noL1',           'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu28','mu8noL1']]],
        ['mu22_mu8noL1',           'L1_MU20', ['L1_MU20',''], [PhysicsStream, 'express'], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','mu8noL1']]],
        ['mu22_2mu4noL1',          'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','2mu4noL1']]],
        ['mu20_2mu4noL1',          'L1_MU20', ['L1_MU20',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','2mu4noL1']]],

        ['mu20_ivarmedium_L1MU10_2J20', 'L1_MU10_2J20', [], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon','BW:Jet'], -1],                
        ['mu20_ivarmedium_L1MU10_2J15_J20', 'L1_MU10_2J15_J20', [], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon','BW:Jet'], -1],  

        #low pt
        ['mu6_nomucomb',                   'L1_MU6',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu8',                    'L1_MU6',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu10',                   'L1_MU10',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],

        ['2mu4',                    'L1_2MU4',  [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],

        ]


    if TriggerFlags.doFTK():
        TriggerFlags.MuonSlice.signatures = TriggerFlags.MuonSlice.signatures() + [

            ['mu6_idperf_FTK_L1MU6_FTK',             'L1_MU6_FTK',            [], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
            ['mu6_idperf_FTKRefit_L1MU6_FTK',        'L1_MU6_FTK',            [], [PhysicsStream, 'express'], ['RATE:IDMonitoring', 'BW:Muon', 'BW:ID'], -1],
#            ['mu22_mu8noL1_FTKFS_L1MU20MU21_FTK',    'L1_MU20MU21_FTK', ['L1_MU20_FTK',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','mu8noL1_FTKFS']]],
          ['mu22_mu8noL1_FTKFS_L1MU20_FTK',    'L1_MU20_FTK', [], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu22','mu8noL1_FTKFS']]],

            ]




    TriggerFlags.JetSlice.signatures = [   
       


        ['j420',                   'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j440',                   'L1_J120', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j460',                   'L1_J120', [], [PhysicsStream, 'express'], ['RATE:SingleJet', 'BW:Jet'], -1],
        
        ['j225_gsc420_boffperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc440_boffperf_split', 'L1_J120', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

        ['j260_320eta490',           'L1_J75.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],

        ['j260_320eta490_nojcalib',        'L1_J75.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],

        ['j460_a10_lcw_subjes_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10_lcw_subjes_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        ['j460_a10r_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j480_a10r_L1J100', 'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        ['3j200',            'L1_J100', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        ['4j120',                   'L1_3J50', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['4j130',                   'L1_3J50', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        ['5j85_L14J15', 'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        ['5j85',                    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        ['5j100',                    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        ['5j70_0eta240_L14J15',          'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        ['5j70_0eta240',            'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        ['5j85_0eta240',            'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        ['6j70_L14J15',                  'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        ['6j70',                          'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        ['6j85',                          'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        ['6j55_0eta240_L14J15',          'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        ['6j55_0eta240_L14J20',           'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        ['6j55_0eta240_L15J150ETA25',     'L1_5J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        ['7j45_L14J15',                   'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        ['7j45',                          'L1_6J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['7j45_L14J20',                   'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        ['7j50',                          'L1_6J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        ['7j45_0eta240_L14J20',           'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['7j45_0eta240_L15J150ETA25',     'L1_5J15.0ETA25', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        ['10j40_L14J20', 'L1_4J20' , [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['10j40_L14J15', 'L1_4J15' , [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],

        ['10j40_L16J15', 'L1_6J15' , [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],

        #low pT
        ['j15',                    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j45',            'L1_J15', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j175',                   'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        ['j45_320eta490', 'L1_J15.31ETA49', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        
    ]

    TriggerFlags.BjetSlice.signatures = [
        ['j175_gsc225_bmv2c2050_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc275_bmv2c2060_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j225_gsc360_bmv2c2085_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],                   
        ['j55_gsc75_bmv2c2060_split_3j55_gsc75_boffperf_split_L14J20',       'L1_4J20',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['3j35_gsc55_bmv2c2070_split_L13J35.0ETA23', 'L1_3J35.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

        #low pT
        ['j35_gsc45_boffperf_split_L1J15', 'L1_J15', [], [PhysicsStream,'express'], ['RATE:SingleBJet', 'BW:BJet'], -1],
#        ['j45_gsc60_bmv2c1085_split_L1J12',      'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
  
        ] # BjetSlice
   

    if TriggerFlags.doFTK():
        TriggerFlags.BjetSlice.signatures = TriggerFlags.BjetSlice.signatures() + [
 
           ['j35_boffperf_split_FTK_L1J15_FTK', 'L1_J15_FTK', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j35_boffperf_split_FTKVtx_L1J15_FTK', 'L1_J15_FTK', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
           ['j35_boffperf_split_FTKRefit_L1J15_FTK', 'L1_J15_FTK', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

           ] # Additional FTK for BjetSlise



    TriggerFlags.METSlice.signatures = [
 	['xe110_pufit_L1XE50',                'L1_XE50',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe110_pufit_L1XE55',                'L1_XE55',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

        #low pT
        ['xe35',                                   'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe90_pufit_L1XE50',               'L1_XE50', [], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],

			 ]

    if TriggerFlags.doFTK():
        TriggerFlags.METSlice.signatures = TriggerFlags.METSlice.signatures() + [
            
            ['xe35_trkmht_FTK', 'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
            
            ]
 
    
    TriggerFlags.TauSlice.signatures = [

        ['tau160_medium1_tracktwo_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau200_medium1_tracktwo_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],

        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],

        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_tautsf_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]],True],

        ['tau35_medium1_tracktwo_tau25_medium1_tracktwo_03dR30_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],

        ['tau35_tight1_tracktwo_tau25_tight1_tracktwo_03dR30_L1DR-TAU20ITAU12I-J25',   'L1_DR-TAU20ITAU12I-J25',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_tight1_tracktwo","tau25_tight1_tracktwo"]]],

        ['tau80_medium1_tracktwo_L1TAU60_tau35_medium1_tracktwo_L1TAU12IM_L1TAU60_DR-TAU20ITAU12I',   'L1_TAU60_DR-TAU20ITAU12I',['L1_TAU60','L1_TAU12IM'], [PhysicsStream], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau80_medium1_tracktwo_L1TAU60","tau35_medium1_tracktwo_L1TAU12IM"]]]	,

        #ATR-19062
        ['tau25_medium1_tracktwo',                 'L1_TAU12IM', [], [PhysicsStream, 'express'], ['RATE:SingleTau', 'BW:Tau'], -1],
			 ]

    if TriggerFlags.doFTK():
            TriggerFlags.TauSlice.signatures = TriggerFlags.TauSlice.signatures() + [
              
                #['tau12_idperf_FTK',              'L1_TAU12IM_FTK', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
                #['tau12_perf_FTKNoPrec',          'L1_TAU12IM_FTK', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],

                ]


    TriggerFlags.EgammaSlice.signatures = [

        ['g300_etcut',                    'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1], 
        ['g300_etcut_L1EM24VHI',          'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1], 
        #['g300_etcut_L1EM26VHI',          'L1_EM26VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1], 

        ['g0_hiptrt_L1EM22VHI',                 'L1_EM22VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],
        ['g0_hiptrt_L1EM24VHI',                 'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'], -1],

        ['2g50_loose_L12EM20VH',                    'L1_2EM20VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 
        ['2g60_loose_L12EM20VH',                    'L1_2EM20VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1], 

        ['2g25_tight_L12EM20VH',                    'L1_2EM20VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],

        ['e26_lhtight_nod0_e15_etcut_L1EM7_Zee',    	  'L1_EM22VHI', ['L1_EM22VHI','L1_EM7'], [PhysicsStream, 'express'], ['RATE:MultiElectron', 'BW:Egamma'],-1 ],
        ['e28_lhtight_nod0_e15_etcut_L1EM7_Zee',    	  'L1_EM24VHI', ['L1_EM24VHI','L1_EM7'], [PhysicsStream, 'express'], ['RATE:MultiElectron', 'BW:Egamma'],-1 ],

        ['e60_lhmedium_nod0', 'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['g140_loose',        'L1_EM22VHI', [], [PhysicsStream, 'express'], ['RATE:SinglePhoton','BW:Egamma'],-1],
        ['g140_tight',        'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton','BW:Egamma'],-1],
        ['g160_loose',        'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton','BW:Egamma'],-1],
        ['g180_loose',        'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton','BW:Egamma'],-1],
        ['g200_loose',        'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton','BW:Egamma'],-1],
        ['e140_lhloose_nod0', 'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e80_lhmedium_nod0',              'L1_EM22VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1 ],
        ['e80_lhmedium_nod0_L1EM24VHI',    'L1_EM24VHI',   [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1 ],

        ['2e24_lhvloose_nod0',                   'L1_2EM20VH', [], [PhysicsStream], ['RATE:MultiElectron', 'BW:Egamma'],-1],
        ['e24_lhmedium_nod0_2g12_loose',                          'L1_EM20VH_3EM10VH', ['L1_EM20VH','L1_3EM10VH'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'],-1,['parallel',-1,[] ]],
        ['e24_lhmedium_nod0_2g12_medium',                         'L1_EM20VH_3EM10VH', ['L1_EM20VH','L1_3EM10VH'], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'],-1,['parallel',-1,[] ]],
        ['e24_lhmedium_nod0_L1EM20VH_g25_medium',  'L1_2EM20VH', [], [PhysicsStream], ['RATE:ElectronPhoton', 'BW:Egamma'], -1,['parallel',-1,[] ]], 
        ['g35_medium_g25_medium_L12EM20VH',          'L1_2EM20VH', [], [PhysicsStream], ['RATE:MultiPhoton', 'BW:Egamma'],-1],

        ['e26_lhtight_nod0',                  'L1_EM22VHI', [], [PhysicsStream,'express'], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e26_lhtight_nod0_ivarloose',        'L1_EM22VHI', [], [PhysicsStream,'express'], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_nod0',                  'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_nod0_ivarloose',        'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e28_lhtight_nod0_ivarloose_L1EM22VHI',  'L1_EM22VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e300_etcut',                           'L1_EM22VHI',  [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 

        ['e60_lhmedium_nod0_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e140_lhloose_nod0_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],
        ['e32_lhtight_nod0_ivarloose',        'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],  

        #low pT
        ['g10_loose',                            'L1_EM7',  [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['e9_lhvloose_nod0_L1EM7',                    'L1_EM7',    [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

			 ]

    TriggerFlags.BphysicsSlice.signatures = [
        # ATR-19062
        ['mu6_mu4_bJpsimumu',             'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],
        ['mu6_mu4_bUpsimumu',             'L1_MU6_2MU4', ['L1_MU6','L1_MU4'], [BPhysicsStream], [RATE_BphysTag,BW_BphysTag], -1],

        ['mu11_mu6_bTau',                  'L1_MU11_2MU6', ['L1_MU11','L1_MU6'], [BPhysicsStream, 'express'], [RATE_BphysTag,BW_BphysTag], -1],

]

    TriggerFlags.CombinedSlice.signatures = [

        ['2e12_lhloose_nod0_mu10',  'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],

        ['2e12_lhmedium_nod0_mu10', 'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],

        ['e12_lhloose_nod0_2mu10',  'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],

        ['e12_lhmedium_nod0_2mu10', 'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],

        ['e17_lhloose_nod0_mu14',   'L1_EM15VH_MU10', ['L1_EM15VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],

        ['e70_lhloose_nod0_xe70noL1',    'L1_EM22VHI',['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e70_lhloose_nod0","xe70noL1"]]],

        ['e80_lhloose_nod0_xe80noL1',    'L1_EM22VHI',['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e80_lhloose_nod0","xe80noL1"]]],

        ['e80_lhloose_nod0_L1EM24VHI_xe80noL1',    'L1_EM24VHI',['L1_EM24VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["e80_lhloose_nod0_L1EM24VHI","xe80noL1"]]],

        ['e7_lhmedium_nod0_mu24',        'L1_MU20', ['L1_EM3', 'L1_MU20'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],

        ['g80_loose_L1EM24VHI_xe80noL1',           'L1_EM24VHI', ['L1_EM24VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g80_loose_L1EM24VHI","xe80noL1"]]],

        ['g80_loose_xe80noL1',           'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaMET', 'BW:Egamma', 'BW:MET'], -1,['serial',-1,["g80_loose","xe80noL1"]]],

        ['g25_medium_mu24',              'L1_MU20', ['L1_EM15VH','L1_MU20'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],

        ['g15_loose_2mu10_msonly',       'L1_2MU10', ['L1_EM8VH', 'L1_2MU10'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['parallel',-1,[] ]],

        ['2mu14_2j35_boffperf_split',                           'L1_2MU10',   ['L1_2MU10', ''],   [PhysicsStream], ['RATE:MuonBJet',  'BW:BJet'],-1,['serial',-1,['2mu14', '2j35_boffperf_split']]],

        ['2g10_loose_mu20',          'L1_MU20',        ['L1_2EM7', 'L1_MU20'],  [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['2g10_loose','mu20'] ]],

        ['2g10_medium_mu20',         'L1_MU20',        ['L1_2EM7', 'L1_MU20'],  [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['2g10_medium','mu20'] ]],

        ['j30_muvtx',                    'L1_2MU10',            [], [PhysicsStream], ['RATE:MuonJet','BW:Muon', 'BW:Jet'], -1],
        ['j30_muvtx_noiso',              'L1_2MU10',            [], [PhysicsStream], ['RATE:MuonJet','BW:Muon', 'BW:Jet'], -1],


        ['j30_jes_cleanLLP_PS_llp_L1LLP-NOMATCH',       'L1_LLP-NOMATCH',       [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1, False],
        ['j30_jes_cleanLLP_PS_llp_noiso_L1LLP-NOMATCH', 'L1_LLP-NOMATCH',       [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1, False],

        ['j30_jes_cleanLLP_PS_llp_L1LLP-RO',       'L1_LLP-RO',       [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1, False],
        ['j30_jes_cleanLLP_PS_llp_noiso_L1LLP-RO', 'L1_LLP-RO',       [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1, False],

        #['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo',  'L1_EM15HI_2TAU12IM_J25_3J12',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream],['RATE:ElectronTau', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwo"]]],

        #['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25', 'L1_DR-EM15TAU12I-J25',['L1_EM15HI','L1_TAU12IM'], [PhysicsStream], ['RATE:ElectronTau', 'BW:Tau'],-1,['serial',-1 ,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwo"]]],

        ['mu14_ivarloose_tau35_medium1_tracktwo',  'L1_MU10_TAU20IM',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_medium1_tracktwo"]]],

        ['mu14_ivarloose_L1MU11_tau35_medium1_tracktwo_L1MU11_TAU20IM',  'L1_MU11_TAU20IM',['L1_MU11','L1_TAU20IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose_L1MU11","tau35_medium1_tracktwo"]]],

        ['mu14_ivarloose_tau35_medium1_tracktwo_L1MU10_TAU20IM_J25_2J20',      'L1_MU10_TAU20IM_J25_2J20',['L1_MU10','L1_TAU20IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau35_medium1_tracktwo"]]],   

        ['mu14_ivarloose_tau25_medium1_tracktwo',      'L1_MU10_TAU12IM_J25_2J12',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau', 'BW:Muon'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwo"]]],        

        ['mu14_ivarloose_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12I-J25', 'L1_DR-MU10TAU12I_TAU12I-J25',['L1_MU10','L1_TAU12IM'], [PhysicsStream], ['RATE:MuonTau', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose","tau25_medium1_tracktwo"]]],

        ['tau35_medium1_tracktwo_xe70_L1XE45',  'L1_TAU20IM_2J20_XE45',['L1_TAU20IM','L1_XE45'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","xe70_L1XE45"]]],  

        ['tau60_medium1_tracktwo_tau25_medium1_tracktwo_xe50', 'L1_TAU40_2TAU12IM_XE40',['L1_TAU40','L1_TAU12IM','L1_XE40'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["tau60_medium1_tracktwo","tau25_medium1_tracktwo","xe50"]]],

        #['e17_lhmedium_nod0_ivarloose_tau25_medium1_tracktwo_xe50', 'L1_EM15HI_2TAU12IM_XE35',['L1_EM15HI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau', 'BW:Egamma'],-1,['serial',-1,["e17_lhmedium_nod0_ivarloose","tau25_medium1_tracktwo", "xe50"]]],

        #['e17_lhmedium_nod0_tau25_medium1_tracktwo_xe50', 'L1_EM15HI_2TAU12IM_XE35',['L1_EM15HI','L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,["e17_lhmedium_nod0","tau25_medium1_tracktwo", "xe50"]]],

        ['mu14_tau25_medium1_tracktwo_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14", "tau25_medium1_tracktwo","xe50"]]],

        ['mu14_ivarloose_tau25_medium1_tracktwo_xe50', 'L1_MU10_TAU12IM_XE35',['L1_MU10', 'L1_TAU12IM','L1_XE35'], [PhysicsStream], ['RATE:TauMET', 'BW:Tau'], -1,['serial',-1,["mu14_ivarloose", "tau25_medium1_tracktwo","xe50"]]],

        ['g25_medium_L1EM22VHI_4j35_0eta490_invm1000', 'L1_EM22VHI', ['L1_EM22VHI',''], [PhysicsStream], ['RATE:EgammaJet', 'BW:Egamma'], -1,['serial',-1,["g25_medium_L1EM22VHI","4j35_0eta490_invm1000"]]],

        ['g25_medium_L1EM24VHI_tau25_dikaonmass_tracktwo_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dikaonmass_tracktwo'],False]],

        ['g25_medium_L1EM24VHI_tau25_dipion1loose_tracktwo_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_dipion1loose_tracktwo'],False]],

        ['g25_medium_L1EM24VHI_tau25_kaonpi1_tracktwo_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_kaonpi1_tracktwo'],False]],

        ['g25_medium_L1EM24VHI_tau25_kaonpi2_tracktwo_50mVis10000','L1_EM24VHI', ['L1_EM24VHI','L1_TAU12'], [PhysicsStream], ['RATE:TauGamma', 'BW:Tau', 'BW:Egamma'], -1,['serial',-1,['g25_medium_L1EM24VHI','tau25_kaonpi2_tracktwo'],False]],

        ['mu14_ivarloose_3j20_L1MU10_3J20', 'L1_MU10_3J20', ['L1_MU10_3J20', ''], [PhysicsStream], ['RATE:MuonJet', 'BW:Muon','BW:Jet'], -1,['serial',-1,['mu14_ivarloose', '3j20'] ],True],

			 ]


    TriggerFlags.HeavyIonSlice.signatures = [
        ]


 #Beamspot chanis first try ATR-9847                                                                                                               
    TriggerFlags.BeamspotSlice.signatures = [

        ['beamspot_allTE_trkfast_peb_L13J15', 'L1_3J15', [], ["BeamSpot"], ['RATE:BeamSpot', 'BW:BeamSpot'], -1],

]   


    if TriggerFlags.doFTK():
        TriggerFlags.BeamspotSlice.signatures = TriggerFlags.BeamspotSlice.signatures() + [

]


    TriggerFlags.MinBiasSlice.signatures = [
        # ATR-19062
        ['mb_sptrk',                     'L1_RD0_FILLED', [], ['MinBias', 'express'], ["RATE:MinBias", "BW:MinBias"], -1],

]
        
    TriggerFlags.CalibSlice.signatures   = [

]

    TriggerFlags.CosmicSlice.signatures  = [

]

    TriggerFlags.StreamingSlice.signatures = [
        ['noalg_L1J400',  'L1_J400',   [], [PhysicsStream, 'express'], ["RATE:SeededStreamers", "BW:Jet"], -1 ],
]

    TriggerFlags.MonitorSlice.signatures = [

]

    # Random Seeded EB chains which select at the HLT based on L1 TBP bits
    TriggerFlags.EnhancedBiasSlice.signatures = [

]

##    #TriggerFlags.GenericSlice.signatures = []
##    TriggerFlags.MuonSlice.signatures = []
##    TriggerFlags.JetSlice.signatures = [   ]
##    TriggerFlags.METSlice.signatures = []    
##    TriggerFlags.TauSlice.signatures = []
##    TriggerFlags.EgammaSlice.signatures = []
##    TriggerFlags.BphysicsSlice.signatures = []
##    TriggerFlags.CombinedSlice.signatures = []
##    TriggerFlags.BjetSlice.signatures = [
##        ['j75_boffperf_split_3j75_L14J15.0ETA25',            'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
##        ['j55_gsc75_boffperf_split_3j75_L14J15.0ETA25',      'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
##        ['j55_gsc75_boffperf_split_3j55_gsc75_boffperf_split_L14J15.0ETA25',      'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
##        ['j55_gsc75_bmv2c2060_split_3j55_gsc75_boffperf_split_L14J15.0ETA25',            'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],       
##        ['j55_gsc75_bmv2c2060_split_3j55_boffperf_split_L14J15.0ETA25',            'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],       
##        ['2j35_bmv2c2050_split_2j35_L14J15.0ETA25',  'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],	
##        ['2j150_bmv2c2050_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
##        ['2j150_bmv2c2050_split_j50_bmv2c2040_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
##        ['2j150_bmv2c2050_split_j50_boffperf_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
##        ['3j150_bmv2c2050_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
##        ['4j150_bmv2c2050_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
##        ]

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
    from copy import deepcopy
    Prescales.HLTPrescales_cosmics = deepcopy(mydict)
    

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
    HLTPrescales = {}

    L1Prescales_cosmics  = {}
    HLTPrescales_cosmics = {}
    chain_list=[
        'e0_perf_L1EM3_EMPTY',
        'g0_perf_L1EM3_EMPTY',
        'ht0_L1J12_EMPTY',
        'id_cosmicid_L1MU11_EMPTY',
        'id_cosmicid_L1MU4_EMPTY',
        'j0_L1J12_EMPTY',
        'j0_perf_bperf_L1J12_EMPTY',
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
        'tau1_cosmic_ptonly_L1MU4_EMPTY',
        'tau1_cosmic_track_L1MU4_EMPTY',
        'tau8_cosmic_ptonly',
        'tau8_cosmic_track',
        'tilecalib_laser',
        ]
    HLTPrescales_cosmics.update(dict(map(None,chain_list,len(chain_list)*[ [1, 0, 0] ])))


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
    'mb_sp900_pusup400_trk50_hmt_L1TE5',
    'mb_sp900_pusup400_trk60_hmt_L1TE5',
    'mb_sp1000_pusup450_trk70_hmt_L1TE5',
    'mb_sp1200_pusup500_trk80_hmt_L1TE5',
    'mb_sp1200_pusup500_trk90_hmt_L1TE5',
    'mb_sp1200_pusup500_trk100_hmt_L1TE5',
    'mb_sp1800_pusup700_trk110_hmt_L1TE5',
    'mb_sp2100_pusup900_trk120_hmt_L1TE5',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE5',
    'mb_sp1000_pusup450_trk70_hmt_L1TE10',
    'mb_sp1200_pusup500_trk80_hmt_L1TE10',
    'mb_sp1400_pusup550_trk90_hmt_L1TE10',
    'mb_sp1600_pusup600_trk100_hmt_L1TE10',
    'mb_sp1800_pusup700_trk110_hmt_L1TE10',
    'mb_sp2100_pusup900_trk120_hmt_L1TE10',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE10',
    'mb_sp2500_pusup1100_trk140_hmt_L1TE10',
    'mb_sp1200_pusup500_trk80_hmt_L1TE15',
    'mb_sp1400_pusup550_trk90_hmt_L1TE15',
    'mb_sp1600_pusup600_trk100_hmt_L1TE15',
    'mb_sp1800_pusup700_trk110_hmt_L1TE15',
    'mb_sp2100_pusup900_trk120_hmt_L1TE15',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE15',
    'mb_sp2500_pusup1100_trk140_hmt_L1TE15',
    'mb_sp1400_pusup550_trk90_hmt_L1TE20',
    'mb_sp1600_pusup600_trk100_hmt_L1TE20',
    'mb_sp1800_pusup700_trk110_hmt_L1TE20',
    'mb_sp2100_pusup900_trk120_hmt_L1TE20',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE20',
    'mb_sp2500_pusup1100_trk140_hmt_L1TE20',
    'mb_sp2700_pusup1200_trk150_hmt_L1TE20',
    'mb_sp1600_pusup600_trk100_hmt_L1TE25',
    'mb_sp1800_pusup700_trk110_hmt_L1TE25',
    'mb_sp2100_pusup900_trk120_hmt_L1TE25',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE25',
    'mb_sp2500_pusup1100_trk140_hmt_L1TE25',
    'mb_sp2700_pusup1200_trk150_hmt_L1TE25',
    'mb_sp2900_pusup1300_trk160_hmt_L1TE25',
    'mb_sp1800_pusup700_trk110_hmt_L1TE30',
    'mb_sp2100_pusup900_trk120_hmt_L1TE30',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE30',
    'mb_sp2500_pusup1100_trk140_hmt_L1TE30',
    'mb_sp2700_pusup1200_trk150_hmt_L1TE30',
    'mb_sp2900_pusup1300_trk160_hmt_L1TE30',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE40',
    'mb_sp2500_pusup1100_trk140_hmt_L1TE40',
    'mb_sp2700_pusup1200_trk150_hmt_L1TE40',
    'mb_sp2900_pusup1300_trk160_hmt_L1TE40',
    'mb_sp900_trk50_hmt_L1TE5',
    'mb_sp900_trk60_hmt_L1TE5',
    'mb_sp1000_trk70_hmt_L1TE5',
    'mb_sp1200_trk80_hmt_L1TE5',
    'mb_sp1200_trk90_hmt_L1TE5',
    'mb_sp1200_trk100_hmt_L1TE5',
    'mb_sp1800_trk110_hmt_L1TE5',
    'mb_sp2100_trk120_hmt_L1TE5',
    'mb_sp2300_trk130_hmt_L1TE5',
    'mb_sp1000_trk70_hmt_L1TE10',
    'mb_sp1200_trk80_hmt_L1TE10',
    'mb_sp1400_trk90_hmt_L1TE10',
    'mb_sp1600_trk100_hmt_L1TE10',
    'mb_sp1800_trk110_hmt_L1TE10',
    'mb_sp2100_trk120_hmt_L1TE10',
    'mb_sp2300_trk130_hmt_L1TE10',
    'mb_sp2500_trk140_hmt_L1TE10',
    'mb_sp1200_trk80_hmt_L1TE15',
    'mb_sp1400_trk90_hmt_L1TE15',
    'mb_sp1600_trk100_hmt_L1TE15',
    'mb_sp1800_trk110_hmt_L1TE15',
    'mb_sp2100_trk120_hmt_L1TE15',
    'mb_sp2300_trk130_hmt_L1TE15',
    'mb_sp2500_trk140_hmt_L1TE15',
    'mb_sp1400_trk90_hmt_L1TE20',
    'mb_sp1600_trk100_hmt_L1TE20',
    'mb_sp1800_trk110_hmt_L1TE20',
    'mb_sp2100_trk120_hmt_L1TE20',
    'mb_sp2300_trk130_hmt_L1TE20',
    'mb_sp2500_trk140_hmt_L1TE20',
    'mb_sp2700_trk150_hmt_L1TE20',
    'mb_sp1600_trk100_hmt_L1TE25',
    'mb_sp1800_trk110_hmt_L1TE25',
    'mb_sp2100_trk120_hmt_L1TE25',
    'mb_sp2300_trk130_hmt_L1TE25',
    'mb_sp2500_trk140_hmt_L1TE25',
    'mb_sp2700_trk150_hmt_L1TE25',
    'mb_sp2900_trk160_hmt_L1TE25',
    'mb_sp1800_trk110_hmt_L1TE30',
    'mb_sp2100_trk120_hmt_L1TE30',
    'mb_sp2300_trk130_hmt_L1TE30',
    'mb_sp2500_trk140_hmt_L1TE30',
    'mb_sp2700_trk150_hmt_L1TE30',
    'mb_sp2900_trk160_hmt_L1TE30',
    'mb_sp2300_trk130_hmt_L1TE40',
    'mb_sp2500_trk140_hmt_L1TE40',
    'mb_sp2700_trk150_hmt_L1TE40',
    'mb_sp2900_trk160_hmt_L1TE40',
    'mb_sp900_pusup400_trk50_hmt_L1TE5.0ETA24',
    'mb_sp900_pusup400_trk60_hmt_L1TE5.0ETA24',
    'mb_sp1000_pusup450_trk70_hmt_L1TE5.0ETA24',
    'mb_sp1200_pusup500_trk80_hmt_L1TE5.0ETA24',
    'mb_sp1200_pusup500_trk90_hmt_L1TE5.0ETA24',
    'mb_sp1200_pusup500_trk100_hmt_L1TE5.0ETA24',
    'mb_sp1800_pusup700_trk110_hmt_L1TE5.0ETA24',
    'mb_sp2100_pusup900_trk120_hmt_L1TE5.0ETA24',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE5.0ETA24',
    'mb_sp1000_pusup450_trk70_hmt_L1TE10.0ETA24',
    'mb_sp1200_pusup500_trk80_hmt_L1TE10.0ETA24',
    'mb_sp1400_pusup550_trk90_hmt_L1TE10.0ETA24',
    'mb_sp1600_pusup600_trk100_hmt_L1TE10.0ETA24',
    'mb_sp1800_pusup700_trk110_hmt_L1TE10.0ETA24',
    'mb_sp2100_pusup900_trk120_hmt_L1TE10.0ETA24',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE10.0ETA24',
    'mb_sp2500_pusup1100_trk140_hmt_L1TE10.0ETA24',
    'mb_sp1200_pusup500_trk80_hmt_L1TE15.0ETA24',
    'mb_sp1400_pusup550_trk90_hmt_L1TE15.0ETA24',
    'mb_sp1600_pusup600_trk100_hmt_L1TE15.0ETA24',
    'mb_sp1800_pusup700_trk110_hmt_L1TE15.0ETA24',
    'mb_sp2100_pusup900_trk120_hmt_L1TE15.0ETA24',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE15.0ETA24',
    'mb_sp2500_pusup1100_trk140_hmt_L1TE15.0ETA24',
    'mb_sp1400_pusup550_trk90_hmt_L1TE20.0ETA24',
    'mb_sp1600_pusup600_trk100_hmt_L1TE20.0ETA24',
    'mb_sp1800_pusup700_trk110_hmt_L1TE20.0ETA24',
    'mb_sp2100_pusup900_trk120_hmt_L1TE20.0ETA24',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE20.0ETA24',
    'mb_sp2500_pusup1100_trk140_hmt_L1TE20.0ETA24',
    'mb_sp2700_pusup1200_trk150_hmt_L1TE20.0ETA24',
    'mb_sp1600_pusup600_trk100_hmt_L1TE25.0ETA24',
    'mb_sp1800_pusup700_trk110_hmt_L1TE25.0ETA24',
    'mb_sp2100_pusup900_trk120_hmt_L1TE25.0ETA24',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE25.0ETA24',
    'mb_sp2500_pusup1100_trk140_hmt_L1TE25.0ETA24',
    'mb_sp2700_pusup1200_trk150_hmt_L1TE25.0ETA24',
    'mb_sp2900_pusup1300_trk160_hmt_L1TE25.0ETA24',
    'mb_sp1800_pusup700_trk110_hmt_L1TE30.0ETA24',
    'mb_sp2100_pusup900_trk120_hmt_L1TE30.0ETA24',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE30.0ETA24',
    'mb_sp2500_pusup1100_trk140_hmt_L1TE30.0ETA24',
    'mb_sp2700_pusup1200_trk150_hmt_L1TE30.0ETA24',
    'mb_sp2900_pusup1300_trk160_hmt_L1TE30.0ETA24',
    'mb_sp2300_pusup1000_trk130_hmt_L1TE40.0ETA24',
    'mb_sp2500_pusup1100_trk140_hmt_L1TE40.0ETA24',
    'mb_sp2700_pusup1200_trk150_hmt_L1TE40.0ETA24',
    'mb_sp2900_pusup1300_trk160_hmt_L1TE40.0ETA24',
    'mb_sp400_trk40_hmt_L1MBTS_1_1',
    'mb_sp600_trk45_hmt_L1MBTS_1_1',
    'mb_sp700_trk55_hmt_L1MBTS_1_1',
    'mb_sp900_trk60_hmt_L1MBTS_1_1',
    'mb_sp900_trk65_hmt_L1MBTS_1_1',
    'mb_sp1000_trk70_hmt_L1MBTS_1_1',
    'mb_sp1200_trk75_hmt_L1MBTS_1_1',
    'mb_sp1400_trk80_hmt_L1MBTS_1_1',
    'mb_sp900_trk50_hmt_L1TE5.0ETA24',
    'mb_sp900_trk60_hmt_L1TE5.0ETA24',
    'mb_sp1000_trk70_hmt_L1TE5.0ETA24',
    'mb_sp1200_trk80_hmt_L1TE5.0ETA24',
    'mb_sp1200_trk90_hmt_L1TE5.0ETA24',
    'mb_sp1200_trk100_hmt_L1TE5.0ETA24',
    'mb_sp1800_trk110_hmt_L1TE5.0ETA24',
    'mb_sp2100_trk120_hmt_L1TE5.0ETA24',
    'mb_sp2300_trk130_hmt_L1TE5.0ETA24',
    'mb_sp1000_trk70_hmt_L1TE10.0ETA24',
    'mb_sp1200_trk80_hmt_L1TE10.0ETA24',
    'mb_sp1400_trk90_hmt_L1TE10.0ETA24',
    'mb_sp1600_trk100_hmt_L1TE10.0ETA24',
    'mb_sp1800_trk110_hmt_L1TE10.0ETA24',
    'mb_sp2100_trk120_hmt_L1TE10.0ETA24',
    'mb_sp2300_trk130_hmt_L1TE10.0ETA24',
    'mb_sp2500_trk140_hmt_L1TE10.0ETA24',
    'mb_sp1200_trk80_hmt_L1TE15.0ETA24',
    'mb_sp1400_trk90_hmt_L1TE15.0ETA24',
    'mb_sp1600_trk100_hmt_L1TE15.0ETA24',
    'mb_sp1800_trk110_hmt_L1TE15.0ETA24',
    'mb_sp2100_trk120_hmt_L1TE15.0ETA24',
    'mb_sp2300_trk130_hmt_L1TE15.0ETA24',
    'mb_sp2500_trk140_hmt_L1TE15.0ETA24',
    'mb_sp1400_trk90_hmt_L1TE20.0ETA24',
    'mb_sp1600_trk100_hmt_L1TE20.0ETA24',
    'mb_sp1800_trk110_hmt_L1TE20.0ETA24',
    'mb_sp2100_trk120_hmt_L1TE20.0ETA24',
    'mb_sp2300_trk130_hmt_L1TE20.0ETA24',
    'mb_sp2500_trk140_hmt_L1TE20.0ETA24',
    'mb_sp2700_trk150_hmt_L1TE20.0ETA24',
    'mb_sp1600_trk100_hmt_L1TE25.0ETA24',
    'mb_sp1800_trk110_hmt_L1TE25.0ETA24',
    'mb_sp2100_trk120_hmt_L1TE25.0ETA24',
    'mb_sp2300_trk130_hmt_L1TE25.0ETA24',
    'mb_sp2500_trk140_hmt_L1TE25.0ETA24',
    'mb_sp2700_trk150_hmt_L1TE25.0ETA24',
    'mb_sp2900_trk160_hmt_L1TE25.0ETA24',
    'mb_sp1800_trk110_hmt_L1TE30.0ETA24',
    'mb_sp2100_trk120_hmt_L1TE30.0ETA24',
    'mb_sp2300_trk130_hmt_L1TE30.0ETA24',
    'mb_sp2500_trk140_hmt_L1TE30.0ETA24',
    'mb_sp2700_trk150_hmt_L1TE30.0ETA24',
    'mb_sp2900_trk160_hmt_L1TE30.0ETA24',
    'mb_sp2300_trk130_hmt_L1TE40.0ETA24',
    'mb_sp2500_trk140_hmt_L1TE40.0ETA24',
    'mb_sp2700_trk150_hmt_L1TE40.0ETA24',
    'mb_sp2900_trk160_hmt_L1TE40.0ETA24',
    'mb_sp800_hmtperf_L1TE5',
    'mb_sp800_hmtperf_L1TE10',
    'mb_sp1200_hmtperf_L1TE10',
    'mb_sp1200_hmtperf_L1TE15',
    'mb_sp1600_hmtperf_L1TE15',
    'mb_sp1600_hmtperf_L1TE20',
    'mb_sp2100_hmtperf_L1TE20',
    'mb_sp2100_hmtperf_L1TE25',
    'mb_sp2100_hmtperf_L1TE30',
    'mb_sp2100_hmtperf_L1TE40',
    'mb_sp800_hmtperf_L1TE5.0ETA24',
    'mb_sp800_hmtperf_L1TE10.0ETA24',
    'mb_sp1200_hmtperf_L1TE10.0ETA24',
    'mb_sp1200_hmtperf_L1TE15.0ETA24',
    'mb_sp1600_hmtperf_L1TE15.0ETA24',
    'mb_sp1600_hmtperf_L1TE20.0ETA24',
    'mb_sp2100_hmtperf_L1TE20.0ETA24',
    'mb_sp2100_hmtperf_L1TE25.0ETA24',
    'mb_sp2100_hmtperf_L1TE30.0ETA24',
    'mb_sp2100_hmtperf_L1TE40.0ETA24',
    'mb_sp2_hmtperf_L1MBTS_1_1',
    'mb_sp2_hmtperf',
    'mb_sp500_hmtperf',
    'mb_sp1800_hmtperf',
    'mb_sptrk',
    'mb_sptrk_L1RD0_EMPTY',
    'mb_sptrk_L1RD0_UNPAIRED_ISO',
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
#ATR-13743    'mb_sptrk_vetombts2in_L1ALFA_CEP',
#    'mb_sptrk_vetombts2in_L1TRT_ALFA_EINE',
#    'mb_sptrk_vetombts2in_L1ALFA_ANY',
#    'mb_sptrk_vetombts2in_L1ALFA_ANY_UNPAIRED_ISO',
#    'mb_sptrk_vetombts2in_L1TRT_ALFA_ANY',
#    'mb_sptrk_vetombts2in_L1TRT_ALFA_ANY_UNPAIRED_ISO',
#    'mb_sptrk_vetombts2in_peb_L1ALFA_ANY',
#    'mb_sptrk_vetombts2in_peb_L1ALFA_ANY_UNPAIRED_ISO',
#    'mb_sptrk_vetombts2in_peb_L1TRT_ALFA_ANY',
#    'mb_sptrk_vetombts2in_peb_L1TRT_ALFA_ANY_UNPAIRED_ISO',
    'mb_sptrk_vetombts2in_L1ZDC_AND',
    'mb_sptrk_vetombts2in_L1ZDC_A_C',
    'mb_sptrk_trk80_L1MBTS_2',
    'mb_sptrk_pt2_L1MBTS_2',
    'mb_sp_L1AFP_C_MBTS_A',
    'mb_sp_L1AFP_C_ANY_MBTS_A',
    'mb_sp_L1AFP_C_AND',          
    'mb_sptrk_pt2_L1AFP_C_AND',  
    'mb_sptrk_pt4_L1AFP_C_AND',  
    'mb_sptrk_pt6_L1AFP_C_AND',  
    'mb_sptrk_pt8_L1AFP_C_AND',  
    'mb_sptrk_trk3_L1AFP_C_AND', 
    'mb_sptrk_trk5_L1AFP_C_AND', 
    'mb_sptrk_trk10_L1AFP_C_AND',
    'mb_sptrk_trk15_L1AFP_C_AND',
    'mb_sptrk_trk80_L1AFP_C_AND',
    'mb_sp_vetospmbts2in_L1AFP_C_AND',
    'mb_sp_vetosp_L1AFP_C_AND',
    #
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
    'noalg_mb_L1TE5',
    'noalg_mb_L1TE5.0ETA24',
    'noalg_mb_L1TE10',
    'noalg_mb_L1TE10.0ETA24',
    'noalg_mb_L1TE20',
    'noalg_mb_L1TE20.0ETA24',
    'noalg_mb_L1TE30',
    'noalg_mb_L1TE30.0ETA24',
    'noalg_mb_L1TE40',
    'noalg_mb_L1TE40.0ETA24',
    ##B'noalg_mb_L1TE50',
    ##B'noalg_mb_L1TE50.0ETA24',
    ##B'noalg_mb_L1TE60',
    ##B'noalg_mb_L1TE60.0ETA24',
    'noalg_mb_L1TE70',
    'noalg_mb_L1TE70.0ETA24',
    'noalg_mb_L1ZDC_A',
    'noalg_mb_L1ZDC_C',
    'noalg_mb_L1ZDC_A_C',
    'noalg_mb_L1ZDC_AND',
    'zdcpeb_L1ZDC_A',  
    'zdcpeb_L1ZDC_C',   
    'zdcpeb_L1ZDC_AND', 
    'zdcpeb_L1ZDC_A_C',
    'timeburner',
    ]
Prescales.HLTPrescales_tight_physics_prescale.update(dict(map(None,chain_list,len(chain_list)*[ [-1, 0,-1] ])))
######################################################

