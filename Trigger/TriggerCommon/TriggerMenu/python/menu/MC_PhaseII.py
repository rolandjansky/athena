315
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
        ['mu26_ivarmedium',	     'L1_MU21', 	  [], [PhysicsStream], ['RATE:SingleMuon','BW:Muon'], -1],
        ['mu60',  'L1_MU21',                              [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu24_mu8noL1',           'L1_MU21',             ['L1_MU21',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu24','mu8noL1']]],
        ['mu24_2mu4noL1',          'L1_MU21',             ['L1_MU21',''], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1,['serial',-1,['mu24','2mu4noL1']]],
        ['2mu14',                  'L1_2MU10',            [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu14_nomucomb',         'L1_2MU10',            [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],

        #upgrade study chains
        ['mu18_ivarmedium',	   'L1_MU6', 	  [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu20_ivarmedium',	   'L1_MU6', 	  [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],
        ['mu40_L1MU6',			  'L1_MU6',           [], [PhysicsStream], ['RATE:SingleMuon', 'BW:Muon'], -1],

        ['mu20_mu8noL1',          'L1_MU6', ['L1_MU6',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','mu8noL1']]],
        ['mu20_2mu4noL1',        'L1_MU6', ['L1_MU6',''], [PhysicsStream], ['RATE:MultiMuon','BW:Muon'], -1,['serial',-1,['mu20','2mu4noL1']]],

        ['2mu11_L1MU6',                         'L1_MU6',           [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],
        ['2mu10_L1MU6',                         'L1_MU6',           [], [PhysicsStream], ['RATE:MultiMuon', 'BW:Muon'], -1],

        ]

    TriggerFlags.JetSlice.signatures = [   
        # Current primaries (2017 menu)
        ['j175',                        'L1_J50',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        ['j400',                    			    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j420',                    			    'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j440_a10t_lcw_jes_L1J100',        'L1_J100', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['4j65',                        'L1_3J15', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],

        ['4j110',                  			    'L1_3J50', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['2j250_j150',         		             'L1_J100', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['3j200',                    			    'L1_J100', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['5j70_0eta240',                		    'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['5j70_0eta240_L14J15',       	    'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85_L14J15',                 'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['5j85',                        'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['5j90',                        'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j70_L14J15',                 'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet' , 'BW:Jet'], -1],
        ['6j70',                        'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['6j50_0eta240_L14J15',         'L1_4J15', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['6j50_0eta240_L14J20',         'L1_4J20', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        ['ht1000_L1J100',               'L1_J100', [], [PhysicsStream], ['RATE:MultiJet','BW:Jet'], -1],

        
        #upgrade study chains
        ['j100_L1J12',                        'L1_J12',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j110_L1J12',                        'L1_J12',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j120_L1J12',                        'L1_J12',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j175_L1J12',                        'L1_J12',  [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        ['j300_L1J12',                    			    'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j380_L1J12',                    			    'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j400_L1J12',                    			    'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j420_L1J12',                    			    'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j440_a10t_lcw_jes_L1J12',        	  		    'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['2j250_j150_L1J12',         		                    'L1_J12', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['3j200_L1J12',                    		            'L1_J12', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],

        ['4j55_L1J12',                        'L1_J12', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['4j65_L1J12',                        'L1_J12', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['4j110_L1J12',                  		    'L1_J12', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['5j90_L1J12',                		            'L1_J12', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        
        ['j400_L1RD0_FILLED',                    			    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j420_L1RD0_FILLED',                    			    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j440_a10t_lcw_jes_L1RD0_FILLED',        	    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['2j250_j150_L1RD0_FILLED',         		             'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['3j200_L1RD0_FILLED',                    		    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],
        ['4j110_L1RD0_FILLED',                  		    'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['5j90_L1RD0_FILLED',                		            'L1_RD0_FILLED', [], [PhysicsStream], ['RATE:MultiJet',  'BW:Jet'], -1],       

        ['ht1000_L1J12',   'L1_J12', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['ht500_L1J12',   'L1_J12', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],
        ['ht450_L1J12',   'L1_J12', [], [PhysicsStream], ['RATE:MultiJet', 'BW:Jet'], -1],

        # Standard topocluster large-R jet triggers
        ['j300_a10_lcw_subjes_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j370_a10_lcw_subjes_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        # Standard topocluster large-R jet triggers + mass cut 
        ['j375_a10t_lcw_jes_30smcINF_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j350_a10t_lcw_jes_30smcINF_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j300_a10t_lcw_jes_30smcINF_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j275_a10t_lcw_jes_30smcINF_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j250_a10t_lcw_jes_30smcINF_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j200_a10t_lcw_jes_30smcINF_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        ['j375_a10t_lcw_jes_35smcINF_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j350_a10t_lcw_jes_35smcINF_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j300_a10t_lcw_jes_35smcINF_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j275_a10t_lcw_jes_35smcINF_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j250_a10t_lcw_jes_35smcINF_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j200_a10t_lcw_jes_35smcINF_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],

        ['j375_a10t_lcw_jes_40smcINF_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j350_a10t_lcw_jes_40smcINF_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j300_a10t_lcw_jes_40smcINF_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j275_a10t_lcw_jes_40smcINF_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j250_a10t_lcw_jes_40smcINF_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],
        ['j200_a10t_lcw_jes_40smcINF_L1J12',       'L1_J12', [], [PhysicsStream], ['RATE:SingleJet', 'BW:Jet'], -1],


        ]

    TriggerFlags.BjetSlice.signatures = [
        # Current primaries (2017 menu)
        ['j225_bmv2c2040_split',        'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j175_gsc225_bmv2c2040_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j225_gsc275_bmv2c2060_split', 'L1_J100', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['3j50_gsc65_bmv2c2070_split_L13J35.0ETA23', 'L1_3J35.0ETA23', [], [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet'], -1],
        ['j150_gsc175_bmv2c2060_split_j45_gsc60_bmv2c2060_split', 'L1_J100', [], [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet'], -1],
        ['j110_gsc150_boffperf_split_2j35_gsc55_bmv2c2060_split', 'L1_J75_3J20', [], [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet'], -1],
        ['j50_gsc65_bmv2c2040_split_3j50_gsc65_boffperf_split',      'L1_4J15.0ETA25', [], [PhysicsStream], ['RATE:SingleBJet',   'BW:BJet'], -1],
        ['j55_gsc75_bmv2c2040_split_3j55_gsc75_boffperf_split',       'L1_4J20',        [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        #['j60_gsc85_bmv2c2050_split_3j60_gsc85_boffperf_split',       'L1_4J20',        [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        
        #upgrade study chains
        # single b-jets with gsc
        ['j110_gsc150_bmv2c2040_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j110_gsc150_bmv2c2060_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j110_gsc150_bmv2c2070_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j110_gsc150_bmv2c2077_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j110_gsc150_bmv2c2085_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],

        ['j150_gsc175_bmv2c2040_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j150_gsc175_bmv2c2060_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j150_gsc175_bmv2c2070_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j150_gsc175_bmv2c2077_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j150_gsc175_bmv2c2085_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],

        ['j175_gsc225_bmv2c2040_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j175_gsc225_bmv2c2060_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j175_gsc225_bmv2c2070_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j175_gsc225_bmv2c2085_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],

        ['j225_gsc275_bmv2c2060_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j225_gsc275_bmv2c2070_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j225_gsc275_bmv2c2077_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j225_gsc275_bmv2c2085_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],


        # single b-jets no gsc
        ['j175_bmv2c2040_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j175_bmv2c2070_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j175_bmv2c2085_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],

        ['j225_bmv2c2060_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j225_bmv2c2070_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j225_bmv2c2077_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['j225_bmv2c2085_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],

        # 1 b-jet + 3 jets
        ['j25_gsc45_bmv2c1040_split_3j25_gsc45_boffperf_split_L1J12',  'L1_J12', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j25_gsc45_bmv2c1060_split_3j25_gsc45_boffperf_split_L1J12',  'L1_J12', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j25_gsc45_bmv2c1070_split_3j25_gsc45_boffperf_split_L1J12',  'L1_J12', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j25_gsc45_bmv2c1077_split_3j25_gsc45_boffperf_split_L1J12',  'L1_J12', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j25_gsc45_bmv2c1085_split_3j25_gsc45_boffperf_split_L1J12',  'L1_J12', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

        ['j35_gsc55_bmv2c1040_split_3j35_gsc55_boffperf_split_L1J12',  'L1_J12', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j35_gsc55_bmv2c1060_split_3j35_gsc55_boffperf_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j35_gsc55_bmv2c1070_split_3j35_gsc55_boffperf_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j35_gsc55_bmv2c1077_split_3j35_gsc55_boffperf_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],
        ['j35_gsc55_bmv2c1085_split_3j35_gsc55_boffperf_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:MultiBJet', 'BW:BJet'], -1],

        ['j50_gsc65_bmv2c1040_split_3j50_gsc65_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j50_gsc65_bmv2c1060_split_3j50_gsc65_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j50_gsc65_bmv2c1070_split_3j50_gsc65_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j50_gsc65_bmv2c1077_split_3j50_gsc65_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j50_gsc65_bmv2c1085_split_3j50_gsc65_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

        ['j55_gsc75_bmv2c1040_split_3j55_gsc75_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j55_gsc75_bmv2c1060_split_3j55_gsc75_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j55_gsc75_bmv2c1070_split_3j55_gsc75_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j55_gsc75_bmv2c1077_split_3j55_gsc75_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j55_gsc75_bmv2c1085_split_3j55_gsc75_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

        ['j65_gsc85_bmv2c1040_split_3j65_gsc85_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j65_gsc85_bmv2c1050_split_3j65_gsc85_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j65_gsc85_bmv2c1060_split_3j65_gsc85_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j65_gsc85_bmv2c1070_split_3j65_gsc85_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j65_gsc85_bmv2c1077_split_3j65_gsc85_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j65_gsc85_bmv2c1085_split_3j65_gsc85_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

        ['j65_bmv2c1040_split_3j65_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j65_bmv2c1050_split_3j65_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j65_bmv2c1060_split_3j65_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['j65_bmv2c1070_split_3j65_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

        ['2j65_bmv2c1040_split_2j65_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j65_bmv2c1050_split_2j65_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j65_bmv2c1060_split_2j65_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j65_bmv2c1070_split_2j65_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

        # 2 b-jets + 2 jets
        ['2j15_gsc35_bmv2c1040_split_2j15_gsc35_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j15_gsc35_bmv2c1060_split_2j15_gsc35_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j15_gsc35_bmv2c1070_split_2j15_gsc35_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j15_gsc35_bmv2c1077_split_2j15_gsc35_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j15_gsc35_bmv2c1085_split_2j15_gsc35_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

        ['2j25_gsc45_bmv2c1040_split_2j25_gsc45_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j25_gsc45_bmv2c1060_split_2j25_gsc45_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j25_gsc45_bmv2c1070_split_2j25_gsc45_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j25_gsc45_bmv2c1077_split_2j25_gsc45_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j25_gsc45_bmv2c1085_split_2j25_gsc45_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

        ['2j25_gsc50_bmv2c1060_split_2j25_gsc50_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

        ['2j35_gsc55_bmv2c1040_split_2j35_gsc55_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j35_gsc55_bmv2c1060_split_2j35_gsc55_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j35_gsc55_bmv2c1070_split_2j35_gsc55_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j35_gsc55_bmv2c1077_split_2j35_gsc55_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j35_gsc55_bmv2c1085_split_2j35_gsc55_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

        ['2j50_gsc65_bmv2c1040_split_2j50_gsc65_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j50_gsc65_bmv2c1060_split_2j50_gsc65_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j50_gsc65_bmv2c1070_split_2j50_gsc65_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j50_gsc65_bmv2c1077_split_2j50_gsc65_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j50_gsc65_bmv2c1085_split_2j50_gsc65_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

        ['2j55_gsc75_bmv2c1040_split_2j55_gsc75_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j55_gsc75_bmv2c1050_split_2j55_gsc75_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j55_gsc75_bmv2c1060_split_2j55_gsc75_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j55_gsc75_bmv2c1070_split_2j55_gsc75_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j55_gsc75_bmv2c1077_split_2j55_gsc75_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j55_gsc75_bmv2c1085_split_2j55_gsc75_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],

        ['2j65_gsc85_bmv2c1040_split_2j65_gsc85_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j65_gsc85_bmv2c1050_split_2j65_gsc85_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j65_gsc85_bmv2c1060_split_2j65_gsc85_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j65_gsc85_bmv2c1070_split_2j65_gsc85_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j65_gsc85_bmv2c1077_split_2j65_gsc85_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],
        ['2j65_gsc85_bmv2c1085_split_2j65_gsc85_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1],


        #forward jets
        ['j85_320eta490', 'L1_J15.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j60_320eta490', 'L1_J15.31ETA49', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j110_320eta490','L1_J15.31ETA49', [], [PhysicsStream], ['RATE:SingleJet','BW:Jet'], -1],
        ['j175_320eta490','L1_J15.31ETA49', [], [PhysicsStream], ['RATE:SingleJet','BW:Jet'], -1],
        ['j60_320eta490_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j85_320eta490_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j110_320eta490_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],
        ['j175_320eta490_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleJet',  'BW:Jet'], -1],

        

        # other chains
        ['j225_bmv2c2040_split_L1J12',        'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        ['3j50_gsc65_bmv2c2070_split_L13J15', 'L1_3J15', [], [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet'], -1],
        ['j150_gsc175_bmv2c2060_split_j45_gsc60_bmv2c2060_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet'], -1],
        ['j110_gsc150_boffperf_split_2j35_gsc55_bmv2c2060_split_L14J15', 'L1_4J15', [], [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet'], -1],
        ['j50_gsc65_bmv2c2040_split_3j50_gsc65_boffperf_split_L14J15',      'L1_4J15', [], [PhysicsStream], ['RATE:SingleBJet',   'BW:BJet'], -1],
        ['j55_gsc75_bmv2c2040_split_3j55_gsc75_boffperf_split_L14J15',       'L1_4J15',        [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        #['j60_gsc85_bmv2c2050_split_3j60_gsc85_boffperf_split_L14J15',       'L1_4J15',        [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],

        ['j110_gsc150_boffperf_split_2j35_gsc55_bmv2c2060_split_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:MultiBJet',  'BW:BJet'], -1],
        ['j50_gsc65_bmv2c2040_split_3j50_gsc65_boffperf_split_L1J12',      'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet',   'BW:BJet'], -1],
        #['j60_gsc85_bmv2c2050_split_3j60_gsc85_boffperf_split_L1J12',       'L1_J12',        [], [PhysicsStream], ['RATE:SingleBJet',  'BW:BJet'], -1],
        
        ]
    
    TriggerFlags.METSlice.signatures = [
        
        # Current primaries (2017 menu)
        ['xe110_L1XE55',                      'L1_XE55',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe110_pufit_L1XE55',                'L1_XE55',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe110_pufit_L1XE60',                 'L1_XE60',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_L1XE55',                      'L1_XE55',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_mht_xe70_L1XE55','L1_XE55',  ['L1_XE55','L1_XE55'], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe120_mht','xe70_L1XE55'] ]],
        ['xe120_mht_xe80_L1XE50','L1_XE50', ['L1_XE50','L1_XE50'], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe120_mht','xe80_L1XE50'] ]],
        
        #upgrade study chains
        ['xe110_L1XE35',                      'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe110_pufit_L1XE35',                'L1_XE35',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_mht_L1XE35_xe70_L1XE35','L1_XE35',  ['L1_XE35','L1_XE35'], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe120_mht_L1XE35','xe70_L1XE35'] ]],

        ['xe110_L1XE10',                      'L1_XE10',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe110_pufit_L1XE10',                'L1_XE10',[], [PhysicsStream], ['RATE:MET', 'BW:MET'], -1],
        ['xe120_mht_L1XE10_xe70_L1XE10','L1_XE10',  ['L1_XE10','L1_XE10'], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe120_mht_L1XE10','xe70_L1XE10'] ]],

	['xe120_trkmht_L1XE10_xe80_mht_L1XE10','L1_XE10', ['L1_XE10','L1_XE10'], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe120_trkmht_L1XE10','xe80_mht_L1XE10'] ]],
	['xe120_trkmht_xe80_mht_L1XE35','L1_XE35', ['L1_XE35','L1_XE35'], [PhysicsStream], ['RATE:MET', 'BW:MET' ], -1, ['serial',-1,['xe120_trkmht','xe80_mht_L1XE35'] ]],
        ]
    
    TriggerFlags.TauSlice.signatures = [
        # Current primaries (2017 menu)
        ['tau160_medium1_tracktwo_L1TAU100',       'L1_TAU100', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau80_medium1_tracktwo_L1TAU60_tau60_medium1_tracktwo_L1TAU40',   'L1_TAU60_2TAU40',['L1_TAU60','L1_TAU40'], [PhysicsStream], ['RATE:MultiTau',  'BW:Tau'], -1,['serial',-1,["tau80_medium1_tracktwo_L1TAU60","tau60_medium1_tracktwo_L1TAU40"]]]       ,
        
        #upgrade study chains
        ['tau115_medium1_tracktwo_L1TAU12',       'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau125_medium1_tracktwo_L1TAU12',       'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],
        ['tau160_medium1_tracktwo_L1TAU12',       'L1_TAU12', [], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1],

        ['tau20_medium1_tracktwo_L1TAU20_tau29_medium1_tracktwo_L1TAU12',   'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12'], [PhysicsStream], ['RATE:MultiTau',  'BW:Tau'], -1,['serial',-1,["tau20_medium1_tracktwo_L1TAU20","tau29_medium1_tracktwo_L1TAU12"]]],
        ['tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12',   'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12'], [PhysicsStream], ['RATE:MultiTau',  'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo_L1TAU20","tau25_medium1_tracktwo_L1TAU12"]]]       ,
        ['tau40_medium1_tracktwo_L1TAU20_tau29_medium1_tracktwo_L1TAU12',   'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12'], [PhysicsStream], ['RATE:MultiTau',  'BW:Tau'], -1,['serial',-1,["tau40_medium1_tracktwo_L1TAU20","tau29_medium1_tracktwo_L1TAU12"]]]       ,

	['tau35_medium1_tracktwo_tau25_medium1_tracktwo_03dR30',   'L1_TAU20IM_2TAU12IM_J25_2J20_3J12',['L1_TAU20IM','L1_TAU12IM'], [PhysicsStream, 'express'], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo","tau25_medium1_tracktwo"]]],
	['tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_03dR30',   'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12'], [PhysicsStream, 'express'], ['RATE:MultiTau', 'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo_L1TAU20","tau25_medium1_tracktwo_L1TAU12"]]],
        
        ]
    
    TriggerFlags.EgammaSlice.signatures = [
        # Current primaries (2017 menu)
        ['e28_lhtight_nod0_ivarloose',        'L1_EM24VHI', [], [PhysicsStream,'express'], ['RATE:SingleElectron','BW:Egamma'],-1],
        ['e60_lhmedium_nod0_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1] ,
        ['e60_medium_L1EM24VHI',        'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron',  'BW:Egamma'],-1], 
        ['e140_lhloose_nod0_L1EM24VHI', 'L1_EM24VHI', [], [PhysicsStream], ['RATE:SingleElectron',  'BW:Egamma'],-1],
        ['2e24_lhvloose_nod0',                   'L1_2EM20VH', [], [PhysicsStream], ['RATE:MultiElectron',  'BW:Egamma'],-1],
        
        ['g140_tight',        'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g200_loose',        'L1_EM24VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['2g25_tight_L12EM20VH',                    'L1_2EM20VH', [], [PhysicsStream], ['RATE:MultiPhoton',  'BW:Egamma'],-1],
        ['2g50_loose_L12EM20VH',                    'L1_2EM20VH', [], [PhysicsStream], ['RATE:MultiPhoton',  'BW:Egamma'],-1], 
        ['g35_medium_g25_medium_L12EM20VH',          'L1_2EM20VH', [], [PhysicsStream], ['RATE:MultiPhoton',  'BW:Egamma'],-1],
        
        #upgrade study chains
        # Electrons
        ['e20_lhmedium_nod0_ivarloose_L1EM15VHI',        'L1_EM15VHI', [], [PhysicsStream], ['RATE:SingleElectron','BW:Egamma'],-1],
        ['e20_lhtight_nod0_ivarloose_L1EM15VHI',        'L1_EM15VHI', [], [PhysicsStream], ['RATE:SingleElectron','BW:Egamma'],-1],

        ['e22_lhtight_nod0_ivarloose_L1EM15VHI',        'L1_EM15VHI', [], [PhysicsStream], ['RATE:SingleElectron','BW:Egamma'],-1],
        ['e45_lhmedium_nod0_L1EM15VHI', 			'L1_EM15VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e45_medium_L1EM15VHI',               	 	'L1_EM15VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1], 
        ['e60_lhloose_nod0_L1EM15VHI', 			'L1_EM15VHI', [], [PhysicsStream], ['RATE:SingleElectron', 'BW:Egamma'],-1],

        ['e20_lhtight_nod0_ringer_ivarloose_L1EM15VHI',        'L1_EM15VHI', [], [PhysicsStream], ['RATE:SingleElectron','BW:Egamma'],-1],
        ['e22_lhtight_nod0_ringer_ivarloose_L1EM15VHI',        'L1_EM15VHI', [], [PhysicsStream], ['RATE:SingleElectron','BW:Egamma'],-1],


        ['2e10_lhvloose_nod0_L12EM10VH',                 'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron',  'BW:Egamma'],-1],
        ['2e11_lhvloose_nod0_L12EM10VH',                 'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron',  'BW:Egamma'],-1],
        ['2e12_lhvloose_nod0_L12EM10VH',                 'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron',  'BW:Egamma'],-1],
        ['2e13_lhvloose_nod0_L12EM10VH',                 'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron',  'BW:Egamma'],-1],
        ['2e14_lhvloose_nod0_L12EM10VH',                 'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron',  'BW:Egamma'],-1],
        ['2e15_lhvloose_nod0_L12EM10VH',                 'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron',  'BW:Egamma'],-1],
        
        ['2e10_lhvloose_nod0_ringer_L12EM10VH',                 'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron',  'BW:Egamma'],-1],
        ['2e12_lhvloose_nod0_ringer_L12EM10VH',                 'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron',  'BW:Egamma'],-1],
        ['2e15_lhvloose_nod0_ringer_L12EM10VH',                 'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiElectron',  'BW:Egamma'],-1],

        # Photons
        ['g100_loose_L1EM15VHI',        				'L1_EM15VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g100_medium_L1EM15VHI',        				'L1_EM15VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g120_tight_L1EM15VHI',        				'L1_EM15VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g120_loose_L1EM15VHI',        			'L1_EM15VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],
        ['g180_loose_L1EM15VHI',        			'L1_EM15VHI', [], [PhysicsStream], ['RATE:SinglePhoton', 'BW:Egamma'],-1],

        ['g25_loose_g20_loose_L12EM10VH',       'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiPhoton',  'BW:Egamma'],-1],
        ['g25_medium_g20_medium_L12EM10VH',       'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiPhoton',  'BW:Egamma'],-1],
        ['g25_tight_g20_tight_L12EM10VH',       'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiPhoton',  'BW:Egamma'],-1],

        ['g35_medium_g25_medium_L12EM10VH',       'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiPhoton',  'BW:Egamma'],-1],

        ['2g25_loose_L12EM10VH',       				'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiPhoton',  'BW:Egamma'],-1],
        ['2g25_medium_L12EM10VH',       			'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiPhoton',  'BW:Egamma'],-1],
        ['2g25_tight_L12EM10VH',       				'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiPhoton',  'BW:Egamma'],-1],

        ['2g20_loose_L12EM10VH',       				'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiPhoton',  'BW:Egamma'],-1],
        ['2g20_medium_L12EM10VH',       			'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiPhoton',  'BW:Egamma'],-1],
        ['2g20_tight_L12EM10VH',       				'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiPhoton',  'BW:Egamma'],-1],

        ['2g50_loose_L12EM10VH',       			'L1_2EM10VH', [], [PhysicsStream], ['RATE:MultiPhoton',  'BW:Egamma'],-1],
        ]

    TriggerFlags.BphysicsSlice.signatures = [
            ]


    TriggerFlags.CombinedSlice.signatures = [
        # Current primaries (2017 menu)
        ['j60_gsc100_bmv2c2050_split_xe80_mht_L1XE60','L1_XE60',[],[PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:MET'], -1,['serial',-1,['j60_gsc100_bmv2c2050_split','xe80_mht_L1XE60']]],
        ['2j45_bmv2c2070_split_xe80_mht_L12J15_XE55','L1_2J15_XE55',[],[PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:MET'], -1,['serial',-1,['2j45_bmv2c2070_split','xe80_mht']]],
        ['2j25_gsc45_bmv2c2070_split_xe80_mht_L12J15_XE55','L1_2J15_XE55',[],[PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:MET'], -1,['serial',-1,['2j25_gsc45_bmv2c2070_split','xe80_mht']]],
        ['3j35_bmv2c2077_split_xe60_mht_L13J15.0ETA25_XE40',"L1_3J15.0ETA25_XE40",[],[PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:MET'], -1, ['serial',-1,['3j35_bmv2c2077_split','xe60_mht']]],
        ['3j15_gsc35_bmv2c2077_split_xe60_mht_L13J15.0ETA25_XE40',"L1_3J15.0ETA25_XE40",[],[PhysicsStream], ['RATE:MultiBJet',  'BW:BJet', 'BW:MET'], -1, ['serial',-1,['3j15_gsc35_bmv2c2077_split','xe60_mht']]],
        ['j100_xe80',                    'L1_J40_XE50',['',''], [PhysicsStream], ['RATE:JetMET',  'BW:Jet', 'BW:MET'], -1,['serial',-1,["j100","xe80"]]],
        ['j100_xe80_mht',                    'L1_J40_XE50',['',''], [PhysicsStream], ['RATE:JetMET',  'BW:Jet', 'BW:MET'], -1,['serial',-1,["j100","xe80_mht"]]],
        ['j80_bmv2c2050_split_xe60_L12J50_XE40','L1_2J50_XE40',  [],  [PhysicsStream], ['RATE:JetMET', 'BW:MET', 'BW:BJet'], -1,['serial',-1,["j80_bmv2c2050_split","xe60"]]],
        

        #upgrade study chains
        #bjet + met
        ['j15_gsc25_bmv2c1040_split_xe100_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j15_gsc25_bmv2c1040_split', 'xe100_L1XE35']]],
        ['j15_gsc25_bmv2c1060_split_xe100_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j15_gsc25_bmv2c1060_split', 'xe100_L1XE35']]],
        ['j15_gsc25_bmv2c1070_split_xe100_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j15_gsc25_bmv2c1070_split', 'xe100_L1XE35']]],
        ['j15_gsc25_bmv2c1077_split_xe100_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j15_gsc25_bmv2c1077_split', 'xe100_L1XE35']]],
        ['j15_gsc25_bmv2c1085_split_xe100_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j15_gsc25_bmv2c1085_split', 'xe100_L1XE35']]],

        ['j25_gsc45_bmv2c1040_split_xe100_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j25_gsc45_bmv2c1040_split', 'xe100_L1XE35']]],
        ['j25_gsc45_bmv2c1060_split_xe100_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j25_gsc45_bmv2c1060_split', 'xe100_L1XE35']]],
        ['j25_gsc45_bmv2c1070_split_xe100_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j25_gsc45_bmv2c1070_split', 'xe100_L1XE35']]],
        ['j25_gsc45_bmv2c1077_split_xe100_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j25_gsc45_bmv2c1077_split', 'xe100_L1XE35']]],
        ['j25_gsc45_bmv2c1085_split_xe100_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j25_gsc45_bmv2c1085_split', 'xe100_L1XE35']]],

        ['j35_gsc55_bmv2c1040_split_xe100_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j35_gsc55_bmv2c1040_split', 'xe100_L1XE35']]],
        ['j35_gsc55_bmv2c1060_split_xe100_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j35_gsc55_bmv2c1060_split', 'xe100_L1XE35']]],
        ['j35_gsc55_bmv2c1070_split_xe100_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j35_gsc55_bmv2c1070_split', 'xe100_L1XE35']]],
        ['j35_gsc55_bmv2c1077_split_xe100_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j35_gsc55_bmv2c1077_split', 'xe100_L1XE35']]],
        ['j35_gsc55_bmv2c1085_split_xe100_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j35_gsc55_bmv2c1085_split', 'xe100_L1XE35']]],


        ['j35_gsc55_bmv2c1070_split_xe110_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j35_gsc55_bmv2c1070_split', 'xe110_L1XE35']]],
        ['j35_gsc55_bmv2c1070_split_xe120_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j35_gsc55_bmv2c1070_split', 'xe120_L1XE35']]],

        ['j55_gsc75_bmv2c1070_split_xe100_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j55_gsc75_bmv2c1070_split', 'xe100_L1XE35']]],
        ['j55_gsc75_bmv2c1070_split_xe110_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j55_gsc75_bmv2c1070_split', 'xe110_L1XE35']]],
        ['j55_gsc75_bmv2c1070_split_xe120_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j55_gsc75_bmv2c1070_split', 'xe120_L1XE35']]],
        ['j60_gsc100_bmv2c2050_split_xe80_mht_L1XE35','L1_XE35',[],  [PhysicsStream], ['RATE:BJetMET',  'BW:BJet', 'BW:MET'], -1,['serial',-1,['j60_gsc100_bmv2c2050_split','xe80_mht_L1XE35']]],
        ['j80_bmv2c2050_split_xe60_L1XE35', 'L1_XE35',          [],  [PhysicsStream], ['RATE:BJetMET', 'BW:MET', 'BW:BJet'], -1,['serial',-1,["j80_bmv2c2050_split","xe60_L1XE35"]]],


        # b-jet + trkmht
        ['j15_gsc25_bmv2c1040_split_xe120_trkmht_xe80_mht_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j15_gsc25_bmv2c1040_split', 'xe120_trkmht','xe80_mht_L1XE35'] ]],
        ['j15_gsc25_bmv2c1060_split_xe120_trkmht_xe80_mht_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j15_gsc25_bmv2c1060_split', 'xe120_trkmht','xe80_mht_L1XE35'] ]],
        ['j15_gsc25_bmv2c1070_split_xe120_trkmht_xe80_mht_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j15_gsc25_bmv2c1070_split', 'xe120_trkmht','xe80_mht_L1XE35'] ]],
        ['j15_gsc25_bmv2c1077_split_xe120_trkmht_xe80_mht_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j15_gsc25_bmv2c1077_split', 'xe120_trkmht','xe80_mht_L1XE35'] ]],
        ['j15_gsc25_bmv2c1085_split_xe120_trkmht_xe80_mht_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j15_gsc25_bmv2c1085_split', 'xe120_trkmht','xe80_mht_L1XE35'] ]],

        ['j25_gsc45_bmv2c1040_split_xe120_trkmht_xe80_mht_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j25_gsc45_bmv2c1040_split', 'xe120_trkmht','xe80_mht_L1XE35'] ]],
        ['j25_gsc45_bmv2c1060_split_xe120_trkmht_xe80_mht_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j25_gsc45_bmv2c1060_split', 'xe120_trkmht','xe80_mht_L1XE35'] ]],
        ['j25_gsc45_bmv2c1070_split_xe120_trkmht_xe80_mht_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j25_gsc45_bmv2c1070_split', 'xe120_trkmht','xe80_mht_L1XE35'] ]],
        ['j25_gsc45_bmv2c1077_split_xe120_trkmht_xe80_mht_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j25_gsc45_bmv2c1077_split', 'xe120_trkmht','xe80_mht_L1XE35'] ]],
        ['j25_gsc45_bmv2c1085_split_xe120_trkmht_xe80_mht_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j25_gsc45_bmv2c1085_split', 'xe120_trkmht','xe80_mht_L1XE35'] ]],

        ['j35_gsc55_bmv2c1040_split_xe120_trkmht_xe80_mht_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j35_gsc55_bmv2c1040_split', 'xe120_trkmht','xe80_mht_L1XE35'] ]],
        ['j35_gsc55_bmv2c1060_split_xe120_trkmht_xe80_mht_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j35_gsc55_bmv2c1060_split', 'xe120_trkmht','xe80_mht_L1XE35'] ]],
        ['j35_gsc55_bmv2c1070_split_xe120_trkmht_xe80_mht_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j35_gsc55_bmv2c1070_split', 'xe120_trkmht','xe80_mht_L1XE35'] ]],
        ['j35_gsc55_bmv2c1077_split_xe120_trkmht_xe80_mht_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j35_gsc55_bmv2c1077_split', 'xe120_trkmht','xe80_mht_L1XE35'] ]],
        ['j35_gsc55_bmv2c1085_split_xe120_trkmht_xe80_mht_L1XE35', 'L1_XE35',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['j35_gsc55_bmv2c1085_split', 'xe120_trkmht','xe80_mht_L1XE35'] ]],


        #bjet+ht
        ['j35_gsc55_bmv2c1040_split_ht500_L1J12', 'L1_J12',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['ht500_L1J12','j35_gsc55_bmv2c1040_split']]],
        ['j35_gsc55_bmv2c1060_split_ht500_L1J12', 'L1_J12',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['ht500_L1J12','j35_gsc55_bmv2c1060_split']]],
        ['j35_gsc55_bmv2c1070_split_ht500_L1J12', 'L1_J12',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['ht500_L1J12','j35_gsc55_bmv2c1070_split']]],
        ['j35_gsc55_bmv2c1077_split_ht500_L1J12', 'L1_J12',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['ht500_L1J12','j35_gsc55_bmv2c1077_split']]],
        ['j35_gsc55_bmv2c1085_split_ht500_L1J12', 'L1_J12',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['ht500_L1J12','j35_gsc55_bmv2c1085_split']]],

        ['j35_gsc55_bmv2c1040_split_ht450_L1J12', 'L1_J12',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['ht450_L1J12','j35_gsc55_bmv2c1040_split']]],
        ['j35_gsc55_bmv2c1060_split_ht450_L1J12', 'L1_J12',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['ht450_L1J12','j35_gsc55_bmv2c1060_split']]],
        ['j35_gsc55_bmv2c1070_split_ht450_L1J12', 'L1_J12',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['ht450_L1J12','j35_gsc55_bmv2c1070_split']]],
        ['j35_gsc55_bmv2c1077_split_ht450_L1J12', 'L1_J12',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['ht450_L1J12','j35_gsc55_bmv2c1077_split']]],
        ['j35_gsc55_bmv2c1085_split_ht450_L1J12', 'L1_J12',   [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'], -1,['serial',-1,['ht450_L1J12','j35_gsc55_bmv2c1085_split']]],

        ['j55_gsc75_bmv2c1040_split_ht500_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'],  -1,['serial',-1,[ 'ht500_L1J12','j55_gsc75_bmv2c1040_split']]],
        ['j55_gsc75_bmv2c1070_split_ht500_L1J12', 'L1_J12', [], [PhysicsStream], ['RATE:SingleBJet', 'BW:BJet'],  -1,['serial',-1,[ 'ht500_L1J12','j55_gsc75_bmv2c1070_split']]],

        #jet+met
        ['j80_xe80_L1XE35',                    'L1_XE35',   ['',''], [PhysicsStream], ['RATE:JetMET',  'BW:Jet', 'BW:MET'], -1,['serial',-1,["j80","xe80_L1XE35"]]],
        ['j135_xe110_mht_L1XE35',                'L1_XE35',['',''], [PhysicsStream], ['RATE:JetMET',  'BW:Jet', 'BW:MET'], -1,['serial',-1,["j135","xe110_mht_L1XE35"]]],
        ['j135_xe120_mht_L1XE35',                'L1_XE35',['',''], [PhysicsStream], ['RATE:JetMET',  'BW:Jet', 'BW:MET'], -1,['serial',-1,["j135","xe120_mht_L1XE35"]]],
        
        # e+mu
        ['e17_lhloose_nod0_mu14',   'L1_EM15VH_MU10', ['L1_EM15VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon',   'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e17_lhloose_nod0_mu14_L1MU6',   'L1_MU6', ['L1_EM15VH', 'L1_MU10'], [PhysicsStream], ['RATE:EgammaMuon',   'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e12_lhmedium_nod0_2mu10', 'L1_MU6', ['L1_EM8VH', 'L1_MU6'], [PhysicsStream], ['RATE:EgammaMuon',  'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['e22_lhmedium_nod0_L1EM15VHI_mu8noL1', 'L1_EM15VHI', ['L1_EM15VHI', ''], [PhysicsStream], ['RATE:EgammaMuon',  'BW:Egamma', 'BW:Muon'], -1, ['serial',-1,['e22_lhmedium_nod0_L1EM15VHI','mu8noL1'] ]],
        ['e7_lhmedium_nod0_mu24',        'L1_MU6', ['L1_EM3', 'L1_MU6'], [PhysicsStream], ['RATE:EgammaMuon',  'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],
        ['2e12_lhloose_nod0_mu10',  'L1_2EM8VH_MU10', ['L1_2EM8VH', 'L1_MU6'], [PhysicsStream], ['RATE:EgammaMuon', 'BW:Egamma', 'BW:Muon'], -1,['parallel',-1,[] ]],

        #tau+jet
        ['tau20_medium1_tracktwo_L1TAU20_tau29_medium1_tracktwo_L1TAU12_j25noL1', 'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12', ''], [PhysicsStream], ['RATE:MultiTau',  'BW:Tau'], -1,['serial',-1,["tau20_medium1_tracktwo_L1TAU20","tau29_medium1_tracktwo_L1TAU12", "j25noL1"]]]       ,
        ['tau20_medium1_tracktwo_L1TAU20_tau29_medium1_tracktwo_L1TAU12_j35noL1', 'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12', ''], [PhysicsStream], ['RATE:MultiTau',  'BW:Tau'], -1,['serial',-1,["tau20_medium1_tracktwo_L1TAU20","tau29_medium1_tracktwo_L1TAU12", "j35noL1"]]]       ,
        ['tau20_medium1_tracktwo_L1TAU20_tau29_medium1_tracktwo_L1TAU12_j55noL1', 'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12', ''], [PhysicsStream], ['RATE:MultiTau',  'BW:Tau'], -1,['serial',-1,["tau20_medium1_tracktwo_L1TAU20","tau29_medium1_tracktwo_L1TAU12", "j55noL1"]]]       ,
        ['tau20_medium1_tracktwo_L1TAU20_tau29_medium1_tracktwo_L1TAU12_j100noL1', 'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12', ''], [PhysicsStream], ['RATE:MultiTau',  'BW:Tau'], -1,['serial',-1,["tau20_medium1_tracktwo_L1TAU20","tau29_medium1_tracktwo_L1TAU12", "j100noL1"]]]       ,
        ['tau20_medium1_tracktwo_L1TAU20_tau29_medium1_tracktwo_L1TAU12_j125noL1', 'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12', ''], [PhysicsStream], ['RATE:MultiTau',  'BW:Tau'], -1,['serial',-1,["tau20_medium1_tracktwo_L1TAU20","tau29_medium1_tracktwo_L1TAU12", "j125noL1"]]]       ,
        ['tau20_medium1_tracktwo_L1TAU20_tau29_medium1_tracktwo_L1TAU12_j150noL1', 'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12', ''], [PhysicsStream], ['RATE:MultiTau',  'BW:Tau'], -1,['serial',-1,["tau20_medium1_tracktwo_L1TAU20","tau29_medium1_tracktwo_L1TAU12", "j150noL1"]]]       ,

        ['tau20_medium1_tracktwo_L1TAU20_tau29_medium1_tracktwo_L1TAU12_', 'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12', ''], [PhysicsStream], ['RATE:MultiTau',  'BW:Tau'], -1,['serial',-1,["tau20_medium1_tracktwo_L1TAU20","tau29_medium1_tracktwo_L1TAU12", "j25noL1"]]]       ,


        ['tau115_medium1_tracktwo_L1TAU12_j35noL1',      'L1_TAU12', ['L1_TAU12', ''], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1, ['serial',-1,["tau115_medium1_tracktwo_L1TAU12", "j35noL1"]]],
        ['tau115_medium1_tracktwo_L1TAU12_j55noL1',      'L1_TAU12', ['L1_TAU12', ''], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1, ['serial',-1,["tau115_medium1_tracktwo_L1TAU12", "j55noL1"]]],
        ['tau115_medium1_tracktwo_L1TAU12_j80noL1',      'L1_TAU12', ['L1_TAU12', ''], [PhysicsStream], ['RATE:SingleTau', 'BW:Tau'], -1, ['serial',-1,["tau115_medium1_tracktwo_L1TAU12", "j80noL1"]]],

        ['tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_j15_gsc35_boffperf_split_L1J12', 'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12', 'L1_J12'], [PhysicsStream], ['RATE:MultiTau',  'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo_L1TAU20","tau25_medium1_tracktwo_L1TAU12", "j15_gsc35_boffperf_split_L1J12"]]],      
        ['tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_j25_gsc45_boffperf_split_L1J12', 'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12', 'L1_J12'], [PhysicsStream], ['RATE:MultiTau',  'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo_L1TAU20","tau25_medium1_tracktwo_L1TAU12", "j25_gsc45_boffperf_split_L1J12"]]],      
        ['tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_j35_gsc55_boffperf_split_L1J12', 'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12', 'L1_J12'], [PhysicsStream], ['RATE:MultiTau',  'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo_L1TAU20","tau25_medium1_tracktwo_L1TAU12", "j35_gsc55_boffperf_split_L1J12"]]],      
        ['tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_j50_gsc65_boffperf_split_L1J12', 'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12', 'L1_J12'], [PhysicsStream], ['RATE:MultiTau',  'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo_L1TAU20","tau25_medium1_tracktwo_L1TAU12", "j50_gsc65_boffperf_split_L1J12"]]],      
        ['tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_j55_gsc75_boffperf_split_L1J12', 'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12', 'L1_J12'], [PhysicsStream], ['RATE:MultiTau',  'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo_L1TAU20","tau25_medium1_tracktwo_L1TAU12", "j55_gsc75_boffperf_split_L1J12"]]],      

        ['tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_2j15_gsc35_boffperf_split_L1J12', 'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12', 'L1_J12'], [PhysicsStream], ['RATE:MultiTau',  'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo_L1TAU20","tau25_medium1_tracktwo_L1TAU12", "2j15_gsc35_boffperf_split_L1J12"]]],      
        ['tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_2j25_gsc45_boffperf_split_L1J12', 'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12', 'L1_J12'], [PhysicsStream], ['RATE:MultiTau',  'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo_L1TAU20","tau25_medium1_tracktwo_L1TAU12", "2j25_gsc45_boffperf_split_L1J12"]]],      
        ['tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_2j35_gsc55_boffperf_split_L1J12', 'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12', 'L1_J12'], [PhysicsStream], ['RATE:MultiTau',  'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo_L1TAU20","tau25_medium1_tracktwo_L1TAU12", "2j35_gsc55_boffperf_split_L1J12"]]],      
        ['tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_2j50_gsc65_boffperf_split_L1J12', 'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12', 'L1_J12'], [PhysicsStream], ['RATE:MultiTau',  'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo_L1TAU20","tau25_medium1_tracktwo_L1TAU12", "2j50_gsc65_boffperf_split_L1J12"]]],      
        ['tau35_medium1_tracktwo_L1TAU20_tau25_medium1_tracktwo_L1TAU12_2j55_gsc75_boffperf_split_L1J12', 'L1_TAU20_2TAU12',['L1_TAU20','L1_TAU12', 'L1_J12'], [PhysicsStream], ['RATE:MultiTau',  'BW:Tau'], -1,['serial',-1,["tau35_medium1_tracktwo_L1TAU20","tau25_medium1_tracktwo_L1TAU12", "2j55_gsc75_boffperf_split_L1J12"]]],      



        ]

    TriggerFlags.HeavyIonSlice.signatures = [
        ]

    TriggerFlags.BeamspotSlice.signatures = [
        ]

    TriggerFlags.AFPSlice.signatures = [
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
        ['costmonitor', '', [], ['CostMonitoring'], ['RATE:Monitoring','BW:Other'],1],
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

