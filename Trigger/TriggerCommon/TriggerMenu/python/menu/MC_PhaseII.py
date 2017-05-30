# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#------------------------------------------------------------------------#
#
# MC_PhaseII menu to be used for studies for PhaseII TDR
# Contacts: Catrin Bernius, Anna Sfyrla, Sarah Demers, Tim Martin
# Date: May 2017
#------------------------------------------------------------------------#
def setupMenu():

    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenu.menu.MC_PhaseII.py' )

    from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1 as tcl1
    if tcl1.current:
        log.info("L1 items: %s " % tcl1.current.menu.items.itemNames())
    else:
        log.warning("L1 menu has not yet been defined")

    PhysicsStream="Main"

    #---------------------------------------------------------------------
    # INPUT FORMAT FOR CHAINS:
    # ['chainName',  'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep], OPTIONAL: [mergingStrategy, offset,[merginOrder] ]], topoStartsFrom = False
    #---------------------------------------------------------------------

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.TestSlice.signatures = []
    TriggerFlags.MuonSlice.signatures = [
        # Current primaries (2017 menu)
        ['mu26_ivarmedium',	     'L1_MU21', 	  [], [PhysicsStream], ['RATE:SingleMuon', 'RATE:UpgradeMuon', 'RATE:UpgradeAll', 'BW:Muon'], -1],
        ['mu60',  'L1_MU21',           [], [PhysicsStream], ['RATE:SingleMuon', 'RATE:UpgradeMuon', 'RATE:UpgradeAll', 'BW:Muon'], -1],
        ['mu24_mu8noL1',           'L1_MU21', ['L1_MU21',''], [PhysicsStream], ['RATE:MultiMuon', 'RATE:UpgradeMuon', 'RATE:UpgradeAll','BW:Muon'], -1,['serial',-1,['mu24','mu8noL1']]],
        ['mu24_2mu4noL1',          'L1_MU21', ['L1_MU21',''], [PhysicsStream], ['RATE:MultiMuon', 'RATE:UpgradeMuon', 'RATE:UpgradeAll', 'BW:Muon'], -1,['serial',-1,['mu24','2mu4noL1']]],
        ['2mu14',                  'L1_2MU10',          [], [PhysicsStream], ['RATE:MultiMuon', 'RATE:UpgradeMuon', 'RATE:UpgradeAll', 'BW:Muon'], -1],
        ['2mu14_nomucomb',         'L1_2MU10',          [], [PhysicsStream], ['RATE:MultiMuon', 'RATE:UpgradeMuon', 'RATE:UpgradeAll', 'BW:Muon'], -1],

        #upgrade study chains
        ['mu20_ivarmedium',	   'L1_MU6', 	  [], [PhysicsStream], ['RATE:SingleMuon', 'RATE:UpgradeMuon', 'RATE:UpgradeAll', 'BW:Muon'], -1],
        ['mu20',			   'L1_MU6',           [], [PhysicsStream], ['RATE:SingleMuon', 'RATE:UpgradeMuon', 'RATE:UpgradeAll', 'BW:Muon'], -1],
        ['mu40_L1MU6',			  'L1_MU6',           [], [PhysicsStream], ['RATE:SingleMuon', 'RATE:UpgradeMuon', 'RATE:UpgradeAll', 'BW:Muon'], -1],
        ['mu20_mu8noL1',          'L1_MU6', ['L1_MU6',''], [PhysicsStream], ['RATE:MultiMuon', 'RATE:UpgradeMuon', 'RATE:UpgradeAll','BW:Muon'], -1,['serial',-1,['mu20','mu8noL1']]],
        ['mu20_2mu4noL1',        'L1_MU6', ['L1_MU6',''], [PhysicsStream], ['RATE:MultiMuon', 'RATE:UpgradeMuon', 'RATE:UpgradeAll','BW:Muon'], -1,['serial',-1,['mu20','2mu4noL1']]],
        ['2mu11_L1MU6',                         'L1_MU6',           [], [PhysicsStream], ['RATE:MultiMuon', 'RATE:UpgradeMuon', 'RATE:UpgradeAll', 'BW:Muon'], -1],

        ]

    TriggerFlags.JetSlice.signatures = [   
        # Current primaries (2017 menu)
        ['j400',                    			    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll', 'BW:Jet'], -1],
        ['j420',                    			    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll', 'BW:Jet'], -1],
        ['j440_a10t_lcw_jes_L1J100',        'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll', 'BW:Jet'], -1],
        ['4j110',                  			    'L1_3J50', [], [PhysicsStream], ['RATE:MultiJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll', 'BW:Jet'], -1],
        ['2j250_j150',         		             'L1_J100', [], [PhysicsStream], ['RATE:MultiJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll',  'BW:Jet'], -1],
        ['3j200',                    			    'L1_J100', [], [PhysicsStream], ['RATE:MultiJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll',  'BW:Jet'], -1],
        ['5j70_0eta240',                		    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll',  'BW:Jet'], -1],
        ['5j70_0eta240_L14J15',       	    'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll',  'BW:Jet'], -1],
        ['5j85_L14J15',                 'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll',  'BW:Jet'], -1],
        ['5j85',                        'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll',  'BW:Jet'], -1],
        ['5j90',                        'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll',  'BW:Jet'], -1],
        ['6j70_L14J15',                 'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll' , 'BW:Jet'], -1],
        ['6j70',                        'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll', 'BW:Jet'], -1],
        ['6j50_0eta240_L14J15',         'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll',  'BW:Jet'], -1],
        ['6j50_0eta240_L14J20',         'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll',  'BW:Jet'], -1],
        
        #upgrade study chains
        ['j400_L1J12',                    			    'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll', 'BW:Jet'], -1],
        ['j420_L1J12',                    			    'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll', 'BW:Jet'], -1],
        ['j440_a10t_lcw_jes_L1J12',        	  		    'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll', 'BW:Jet'], -1],
        ['2j250_j150_L1J12',         		                    'L1_J12', [], [PhysicsStream], ['RATE:MultiJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll',  'BW:Jet'], -1],
        ['3j200_L1J12',                    		            'L1_J12', [], [PhysicsStream], ['RATE:MultiJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll',  'BW:Jet'], -1],
        ['4j110_L1J12',                  		    'L1_J12', [], [PhysicsStream], ['RATE:MultiJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll', 'BW:Jet'], -1],
        ['5j90_L1J12',                		            'L1_J12', [], [PhysicsStream], ['RATE:MultiJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll',  'BW:Jet'], -1],
        
        ['j400_L1RD0_FILLED',                    			    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll', 'BW:Jet'], -1],
        ['j420_L1RD0_FILLED',                    			    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll', 'BW:Jet'], -1],
        ['j440_a10t_lcw_jes_L1RD0_FILLED',        	    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll', 'BW:Jet'], -1],
        ['2j250_j150_L1RD0_FILLED',         		             'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll',  'BW:Jet'], -1],
        ['3j200_L1RD0_FILLED',                    		    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll',  'BW:Jet'], -1],
        ['4j110_L1RD0_FILLED',                  		    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll', 'BW:Jet'], -1],
        ['5j90_L1RD0_FILLED',                		            'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet', 'RATE:UpgradeJet', 'RATE:UpgradeAll',  'BW:Jet'], -1],       
        ]

    TriggerFlags.BjetSlice.signatures = [
        # Current primaries (2017 menu)
        ['j225_bmv2c2040_split',        'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet',  'RATE:UpgradeBJet', 'RATE:UpgradeAll', 'BW:BJet'], -1],
        ['j175_gsc225_bmv2c2040_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet',  'RATE:UpgradeBJet', 'RATE:UpgradeAll', 'BW:BJet'], -1],
        ['j225_gsc275_bmv2c2060_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet', 'RATE:UpgradeBJet', 'RATE:UpgradeAll',  'BW:BJet'], -1],
        ['3j50_gsc65_bmv2c2070_split_L13J35.0ETA23', 'L1_3J35.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet', 'RATE:UpgradeBJet', 'RATE:UpgradeAll',  'BW:BJet'], -1],
        ['j150_gsc175_bmv2c2060_split_j45_gsc60_bmv2c2060_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet', 'RATE:UpgradeBJet', 'RATE:UpgradeAll',  'BW:BJet'], -1],
        ['j110_gsc150_boffperf_split_2j35_gsc55_bmv2c2060_split', 'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet', 'RATE:UpgradeBJet', 'RATE:UpgradeAll',  'BW:BJet'], -1],
        ['j50_gsc65_bmv2c2040_split_3j50_gsc65_boffperf_split',      'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet', 'RATE:UpgradeBJet', 'RATE:UpgradeAll',   'BW:BJet'], -1],
        ['j55_gsc75_bmv2c2040_split_3j55_gsc75_boffperf_split',       'L1_4J20',        [], [PhysicsStream], ['RATE:SingleBJet', 'RATE:UpgradeBJet', 'RATE:UpgradeAll',  'BW:BJet'], -1],
        ['j60_gsc85_bmv2c2050_split_3j60_gsc85_boffperf_split',       'L1_4J20',        [], [PhysicsStream], ['RATE:SingleBJet', 'RATE:UpgradeBJet', 'RATE:UpgradeAll',  'BW:BJet'], -1],
        
        #upgrade study chains
        ['j225_bmv2c2040_split_L1J12',        'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'RATE:UpgradeBJet', 'RATE:UpgradeAll', 'BW:BJet'], -1],
        ['j175_gsc225_bmv2c2040_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'RATE:UpgradeBJet', 'RATE:UpgradeAll', 'BW:BJet'], -1],
        ['j225_gsc275_bmv2c2060_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet', 'RATE:UpgradeBJet', 'RATE:UpgradeAll',  'BW:BJet'], -1],
        ['3j50_gsc65_bmv2c2070_split_L13J15', 'L1_3J15', [], [PhysicsStream], ['RATE:MultiBJet', 'RATE:UpgradeBJet', 'RATE:UpgradeAll',  'BW:BJet'], -1],
        ['j150_gsc175_bmv2c2060_split_j45_gsc60_bmv2c2060_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:MultiBJet', 'RATE:UpgradeBJet', 'RATE:UpgradeAll',  'BW:BJet'], -1],
        ['j110_gsc150_boffperf_split_2j35_gsc55_bmv2c2060_split_L14J15', 'L1_4J15', [], [PhysicsStream], ['RATE:MultiBJet', 'RATE:UpgradeBJet', 'RATE:UpgradeAll',  'BW:BJet'], -1],
        ['j50_gsc65_bmv2c2040_split_3j50_gsc65_boffperf_split_L14J15',      'L1_4J15', [], [PhysicsStream], ['RATE:SingleBJet', 'RATE:UpgradeBJet', 'RATE:UpgradeAll',   'BW:BJet'], -1],
        ['j55_gsc75_bmv2c2040_split_3j55_gsc75_boffperf_split_L14J15',       'L1_4J15',        [], [PhysicsStream], ['RATE:SingleBJet', 'RATE:UpgradeBJet', 'RATE:UpgradeAll',  'BW:BJet'], -1],
        ['j60_gsc85_bmv2c2050_split_3j60_gsc85_boffperf_split_L14J15',       'L1_4J15',        [], [PhysicsStream], ['RATE:SingleBJet', 'RATE:UpgradeBJet', 'RATE:UpgradeAll',  'BW:BJet'], -1],
        
        ]
    
    TriggerFlags.METSlice.signatures = [
        
        # Current primaries (2017 menu)
        ['xe110_L1XE55',                      'L1_XE55',[], [PhysicsStream], ['RATE:MET', 'RATE:UpgradeMET', 'RATE:UpgradeAll', 'BW:MET'], -1],
        ['xe110_pufit_L1XE55',                'L1_XE55',[], [PhysicsStream], ['RATE:MET', 'RATE:UpgradeMET', 'RATE:UpgradeAll', 'BW:MET'], -1],
        ['xe110_pufit_L1XE60',                 'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'RATE:UpgradeMET', 'RATE:UpgradeAll', 'BW:MET'], -1],
        ['xe120_L1XE55',                      'L1_XE55',[], [PhysicsStream], ['RATE:MET', 'RATE:UpgradeMET', 'RATE:UpgradeAll', 'BW:MET'], -1],
        ['xe120_mht_xe70_L1XE55','L1_XE55',  ['L1_XE55','L1_XE55'], [PhysicsStream], ['RATE:MET', 'RATE:UpgradeMET', 'RATE:UpgradeAll', 'BW:MET' ], -1, ['serial',-1,['xe120_mht','xe70_L1XE55'] ]],
        ['xe120_mht_xe80_L1XE50','L1_XE50', ['L1_XE50','L1_XE50'], [PhysicsStream], ['RATE:MET', 'RATE:UpgradeMET', 'RATE:UpgradeAll', 'BW:MET' ], -1, ['serial',-1,['xe120_mht','xe80_L1XE50'] ]],
        
        #upgrade study chains
        ['xe110_L1XE35',                      'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'RATE:UpgradeMET', 'RATE:UpgradeAll', 'BW:MET'], -1],
        ['xe110_pufit_L1XE35',                'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'RATE:UpgradeMET', 'RATE:UpgradeAll', 'BW:MET'], -1],
        ['xe120_mht_L1XE35_xe70_L1XE35','L1_XE35',  ['L1_XE35','L1_XE35'], [PhysicsStream], ['RATE:MET', 'RATE:UpgradeMET', 'RATE:UpgradeAll', 'BW:MET' ], -1, ['serial',-1,['xe120_mht_L1XE35','xe70_L1XE35'] ]],
        ]
    
    TriggerFlags.TauSlice.signatures = [
        # Current primaries (2017 menu)
        ['tau160_medium1_tracktwo_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['RATE:SingleTau', 'RATE:UpgradeTau', 'RATE:UpgradeAll', 'BW:Tau'], -1],
        ['tau80_medium1_tracktwo_L1TAU60_tau60_medium1_tracktwo_L1TAU40',   'L1_TAU60_2TAU40',['L1_TAU60','L1_TAU40'], [PhysicsStream], ['RATE:MultiTau', 'RATE:UpgradeTau', 'RATE:UpgradeAll',  'BW:Tau'], -1,['serial',-1,["tau80_medium1_tracktwo_L1TAU60","tau60_medium1_tracktwo_L1TAU40"]]]       ,
        
        #upgrade study chains
        ['tau125_medium1_tracktwo_L1TAU12',       'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'RATE:UpgradeTau', 'RATE:UpgradeAll', 'BW:Tau'], -1],
        ['tau160_medium1_tracktwo_L1TAU12',       'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'RATE:UpgradeTau', 'RATE:UpgradeAll', 'BW:Tau'], -1],
        ['tau40_medium1_tracktwo_L1TAU20_tau29_medium1_tracktwo_L1TAU12',   'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12'], [PhysicsStream], ['RATE:MultiTau', 'RATE:UpgradeTau', 'RATE:UpgradeAll',  'BW:Tau'], -1,['serial',-1,["tau40_medium1_tracktwo_L1TAU20","tau29_medium1_tracktwo_L1TAU12"]]]       ,
        
        ]
    
    TriggerFlags.EgammaSlice.signatures = [
        # Current primaries (2017 menu)
        ['e28_lhtight_nod0_ivarloose',        'L1_EM24VHI', [], [PhysicsStream,'express'], ['RATE:SingleElectron', 'RATE:UpgradeElectron', 'RATE:UpgradeAll','BW:Egamma'],-1],
        ['e60_lhmedium_nod0_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'RATE:UpgradeElectron', 'RATE:UpgradeAll', 'BW:Egamma'],-1] ,
        ['e60_medium_L1EM24VHI',        'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron',  'RATE:UpgradeElectron', 'RATE:UpgradeAll', 'BW:Egamma'],-1], 
        ['e140_lhloose_nod0_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron',  'RATE:UpgradeElectron', 'RATE:UpgradeAll', 'BW:Egamma'],-1],
        ['2e24_lhvloose_nod0',                   'L1_2EM20VH', [], [PhysicsStream], ['RATE:MultiElectron',  'RATE:UpgradeElectron', 'RATE:UpgradeAll', 'BW:Egamma'],-1],
        
        ['g140_tight',        'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'RATE:UpgradePhoton', 'RATE:UpgradeAll', 'BW:Egamma'],-1],
        ['g200_loose',        'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'RATE:UpgradePhoton', 'RATE:UpgradeAll', 'BW:Egamma'],-1],
        ['2g25_tight_L12EM20VH',                    'L1_2EM20VH', [], [PhysicsStream], ['RATE:MultiPhoton',  'RATE:UpgradePhoton', 'RATE:UpgradeAll', 'BW:Egamma'],-1],
        ['2g50_loose_L12EM20VH',                    'L1_2EM20VH', [], [PhysicsStream], ['RATE:MultiPhoton',  'RATE:UpgradePhoton', 'RATE:UpgradeAll', 'BW:Egamma'],-1], 
        ['g35_medium_g25_medium_L12EM20VH',          'L1_2EM20VH', [], [PhysicsStream], ['RATE:MultiPhoton',  'RATE:UpgradePhoton', 'RATE:UpgradeAll', 'BW:Egamma'],-1],
        
        #upgrade study chains
        ['e22_lhtight_nod0_ivarloose_L1EM15VHI',        'L1_EM15VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'RATE:UpgradeElectron', 'RATE:UpgradeAll','BW:Egamma'],-1],
        ['e45_lhmedium_nod0_L1EM15VHI', 			'L1_EM15VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'RATE:UpgradeElectron', 'RATE:UpgradeAll', 'BW:Egamma'],-1], 
        ['e45_medium_L1EM15VHI',               	 	'L1_EM15VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'RATE:UpgradeElectron', 'RATE:UpgradeAll', 'BW:Egamma'],-1], 
        ['e60_lhloose_nod0_L1EM15VHI', 			'L1_EM15VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'RATE:UpgradeElectron', 'RATE:UpgradeAll', 'BW:Egamma'],-1],
        ['2e15_lhvloose_nod0_L12EM10VH',                 'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron',  'RATE:UpgradeElectron', 'RATE:UpgradeAll', 'BW:Egamma'],-1],
        
        ['g120_tight_L1EM15VHI',        				'L1_EM15VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'RATE:UpgradePhoton', 'RATE:UpgradeAll', 'BW:Egamma'],-1],
        ['g120_loose_L1EM15VHI',        			'L1_EM15VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'RATE:UpgradePhoton', 'RATE:UpgradeAll', 'BW:Egamma'],-1],
        ['g180_loose_L1EM15VHI',        			'L1_EM15VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'RATE:UpgradePhoton', 'RATE:UpgradeAll', 'BW:Egamma'],-1],
        ['g35_medium_g25_medium_L12EM10VH',       'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiPhoton',  'RATE:UpgradePhoton', 'RATE:UpgradeAll', 'BW:Egamma'],-1],
        ['2g50_loose_L12EM10VH',       			'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiPhoton',  'RATE:UpgradePhoton', 'RATE:UpgradeAll', 'BW:Egamma'],-1],
        ['2g25_tight_L12EM10VH',       				'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiPhoton',  'RATE:UpgradePhoton', 'RATE:UpgradeAll', 'BW:Egamma'],-1],
        ]

    TriggerFlags.BphysicsSlice.signatures = [
            ]

    TriggerFlags.CombinedSlice.signatures = [
# Current primaries (2017 menu)
        ['j60_gsc100_bmv2c2050_split_xe80_mht_L1XE60','L1_XE60',[],[PhysicsStream], ['RATE:MultiBJet', 'RATE:UpgradeBJet', 'RATE:UpgradeAll',  'BW:BJet', 'BW:MET'], -1,['serial',-1,['j60_gsc100_bmv2c2050_split','xe80_mht_L1XE60']]],
        ['2j45_bmv2c2070_split_xe80_mht_L12J15_XE55','L1_2J15_XE55',[],[PhysicsStream], ['RATE:MultiBJet', 'RATE:UpgradeBJet', 'RATE:UpgradeAll',  'BW:BJet', 'BW:MET'], -1,['serial',-1,['2j45_bmv2c2070_split','xe80_mht']]],
        ['2j25_gsc45_bmv2c2070_split_xe80_mht_L12J15_XE55','L1_2J15_XE55',[],[PhysicsStream], ['RATE:MultiBJet', 'RATE:UpgradeBJet', 'RATE:UpgradeAll',  'BW:BJet', 'BW:MET'], -1,['serial',-1,['2j25_gsc45_bmv2c2070_split','xe80_mht']]],
        ['3j35_bmv2c2077_split_xe60_mht_L13J15.0ETA25_XE40',"L1_3J15.0ETA25_XE40",[],[PhysicsStream], ['RATE:MultiBJet', 'RATE:UpgradeBJet', 'RATE:UpgradeAll',  'BW:BJet', 'BW:MET'], -1, ['serial',-1,['3j35_bmv2c2077_split','xe60_mht']]],
        ['3j15_gsc35_bmv2c2077_split_xe60_mht_L13J15.0ETA25_XE40',"L1_3J15.0ETA25_XE40",[],[PhysicsStream], ['RATE:MultiBJet', 'RATE:UpgradeBJet', 'RATE:UpgradeAll',  'BW:BJet', 'BW:MET'], -1, ['serial',-1,['3j15_gsc35_bmv2c2077_split','xe60_mht']]],
        ['j100_xe80',                    'L1_J40_XE50',['',''], [PhysicsStream], ['RATE:JetMET', 'RATE:UpgradeJetMET', 'RATE:UpgradeAll',  'BW:Jet', 'BW:MET'], -1,['serial',-1,["j100","xe80"]]],
        ['j80_bmv2c2050_split_xe60_L12J50_XE40','L1_2J50_XE40',  [],  [PhysicsStream], ['RATE:JetMET', 'RATE:UpgradeJetMET', 'RATE:UpgradeAll','BW:MET', 'BW:BJet'], -1,['serial',-1,["j80_bmv2c2050_split","xe60"]]],
        
        #upgrade study chains
        ['j60_gsc100_bmv2c2050_split_xe80_mht_L1XE35','L1_XE35',[],[PhysicsStream], ['RATE:MultiBJet', 'RATE:UpgradeBJet', 'RATE:UpgradeAll',  'BW:BJet', 'BW:MET'], -1,['serial',-1,['j60_gsc100_bmv2c2050_split','xe80_mht_L1XE35']]],
        ['j80_bmv2c2050_split_xe60_L1XE35', 'L1_XE35',  [],  [PhysicsStream], ['RATE:JetMET', 'RATE:UpgradeJetMET', 'RATE:UpgradeAll','BW:MET', 'BW:BJet'], -1,['serial',-1,["j80_bmv2c2050_split","xe60_L1XE35"]]],
        ['j80_xe80_L1XE35',                    'L1_XE35',['',''], [PhysicsStream], ['RATE:JetMET', 'RATE:UpgradeJetMET', 'RATE:UpgradeAll',  'BW:Jet', 'BW:MET'], -1,['serial',-1,["j80","xe80_L1XE35"]]],
        ['j140_xe125_L1XE35',                'L1_XE35',['',''], [PhysicsStream], ['RATE:JetMET', 'RATE:UpgradeJetMET', 'RATE:UpgradeAll',  'BW:Jet', 'BW:MET'], -1,['serial',-1,["j140","xe125_L1XE35"]]],
        
        ]

    TriggerFlags.HeavyIonSlice.signatures = [
        ]

    TriggerFlags.BeamspotSlice.signatures = [
        ]   

    TriggerFlags.MinBiasSlice.signatures = [
        ]
        
    TriggerFlags.CalibSlice.signatures   = [
        ]

    TriggerFlags.CosmicSlice.signatures  = [ 
        ]

    TriggerFlags.StreamingSlice.signatures = [
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
    from copy import deepcopy
    Prescales.HLTPrescales_cosmics = deepcopy(mydict)


#----------------------------------------------------------
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
        #'larnoiseburst_rerun'       : [ 0, 0 , 1, "LArNoiseBurst"],
        }

    L1Prescales_cosmics  = {}
    HLTPrescales_cosmics = {}
    chain_list=[
        #'sct_noise',
        #'tilecalib_laser',
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

chain_list=[]
Prescales.HLTPrescales_tight_physics_prescale.update(dict(map(None,chain_list,len(chain_list)*[ [-1, 0,-1] ])))
######################################################

