# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""CPS addition  """

__doc__="Addition of coherent prescales to the list of chains"

from TriggerJobOpts.TriggerFlags import TriggerFlags

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)


def genericCPSAdder(groups, signatures, chains, level, signatureOverwritten):

    for cpsGroup, chainNames in groups.iteritems():
        if "RATE" not in cpsGroup:
            log.error('Following group do not start for RATE [%s]' %cpsGroup)
        for chainName in chainNames:
            if chainName not in signatures:
                if not signatureOverwritten:
                    log.error('Signature %s not registered to TriggerPythonConfig' % chainName)
                else:
                    log.warning('Signature %s not registered to TriggerPythonConfig' % chainName)
                continue

            if level == 'HLT':
                chain_name = signatures[chainName][-1].chain_name
            else:
                log.error("Level of %s not defined", chainName)
                
            for c in chains:
                if c.chain_name == chain_name:
                    log.debug('chain: c.chain_name=%s, c.groups=%s, cpsGroup=%s', c.chain_name, c.groups, cpsGroup)
                    cpsFound = None
                    for g in c.groups:
                        if g.find('CPS') == 5:
                            cpsFound = g
                    if cpsFound == None:
                        log.info('CPS group %s added to %s', cpsGroup, chain_name)
                        c.addGroup(cpsGroup)
                    elif cpsFound == cpsGroup:
                        log.debug('CPS group %s already assigned to %s. Nothing added', cpsGroup, chain_name)
                    else:
                        log.warning('CPS group with different name (%s) already exists for %s. Group %s not added', cpsFound, chain_name, cpsGroup)

            #[c.addGroup(cpsGroup) for c in chains if c.chain_name == chain_name]



def defineCPSGroups():
    HLT_CPS_Groups = {				       
        #Egamma chains
                                      
        ########################################################################


#        "RATE:CPS:HLT_e24_medium_L1EM18VH" : ['e24_lhmedium_L1EM18VH',
#                                                'e24_lhmedium_nod0_L1EM18VH', 
#                                                ],
        
        
        #'RATE:CPS:HLT_e5_tight_e4_etcut_L1JPSI-1M5' :['e5_tight_e4_etcut_L1JPSI-1M5',
        #                                             'e5_tight_e4_etcut_Jpsiee_L1JPSI-1M5',] ,
        #
        #
        #'RATE:CPS:HLT_e5_lhtight_e4_etcut_L1JPSI-1M5' : ['e5_lhtight_e4_etcut_L1JPSI-1M5',
        #                                                 'e5_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5',],


 #       'RATE:CPS:HLT_e24_medium_L1EM18VH': ['e24_lhmedium_L1EM18VH',
 #                                            'e24_lhmedium_nod0_L1EM18VH',],
        

        #BJet chains                                                                                                                                                    
        'RATE:CPS:HLT_j35b' : [#'j35_bperf',
#                               'j35_boffperf',
                               #'j35_bperf_split',
                               ],

        
        'RATE:CPS:HLT_j55b' : [#'j55_bperf',
                               #'j55_bperf_split',
#                               'j55_boffperf',
                               ],


        'RATE:CPS:HLT_j150b' : [#'j150_bperf',
                                #'j150_bperf_split',
#                                'j150_boffperf',
                               ],

        'RATE:CPS:HLT_j320b' : [#'j320_bperf',
                                #'j320_bperf_split',
#                                'j320_boffperf',
                               ],         

        }

    if "v6"  in TriggerFlags.triggerMenuSetup():
        HLT_CPS_Groups.update({
            'RATE:CPS:HLT_mu_jet_L1MU6_J75' : [
                'mu6_j260_boffperf_split_dr05_dz02',
                'mu6_j320_boffperf_split_dr05_dz02',
                'mu6_j320_boffperf_split',
                'mu6_j400_boffperf_split_dr05_dz02',     
            ],
            'RATE:CPS:HLT_mu_jet_L1MU6_J40' : [
                'mu6_j150_boffperf_split_dr05_dz02',
                'mu6_j150_boffperf_split',
                'mu6_j175_boffperf_split_dr05_dz02',	
            ],
            'RATE:CPS:HLT_mu_jet_L1MU4' : ['mu4_j35_boffperf_split_dr05_dz02',
                                           'mu4_j35_boffperf_split',
				       ],
        'RATE:CPS:HLT_e24_medium_L1EM18VH' : ['e24_lhmedium_L1EM18VH',
                                                'e24_lhmedium_nod0_L1EM18VH',
		],

         "RATE:CPS:HLT_e10_vloose_L1EM7" : ['e10_lhvloose_L1EM7',
                                           'e10_lhvloose_nod0_L1EM7',
                                           'e15_lhvloose_nod0_L1EM7',
                                           'e15_lhvloose_L1EM7',
                                           'e15_lhvloose_nod0_L1EM7',
                                            ],

        "RATE:CPS:HLT_e20_vloose_L1EM12" : ['e20_lhvloose_L1EM12',
                                            'e20_lhvloose_nod0_L1EM12',],
                                            
   
        "RATE:CPS:HLT_e25_vloose_L1EM15" : ['e25_lhvloose_L1EM15',
                                            'e25_lhvloose_nod0_L1EM15'],

        "RATE:CPS:HLT_e30_vloose_L1EM15" : ['e30_lhvloose_L1EM15',
                                            ],

        "RATE:CPS:HLT_e40_vloose_L1EM15" : ['e40_lhvloose_L1EM15',
                                            'e40_lhvloose_nod0_L1EM15'],

        "RATE:CPS:HLT_e50_vloose_L1EM15" : ['e50_lhvloose_L1EM15',
                                            'e50_lhvloose_nod0_L1EM15'],
        
        "RATE:CPS:HLT_e70_vloose"        : ['e70_lhvloose',
                                            'e70_lhvloose_nod0'],

        "RATE:CPS:HLT_e80_vloose"        : ['e80_lhvloose',
                                            ],
        
        "RATE:CPS:HLT_e100_vloose"        : ['e100_lhvloose',
                                             'e100_lhvloose_nod0'],
        
        "RATE:CPS:HLT_e120_vloose"        : ['e120_lhvloose',
                                             ],
            
        "RATE:CPS:HLT_e140_vloose"        : ['e140_lhvloose',
                                             ],

        })
    if "v7" in TriggerFlags.triggerMenuSetup():
        HLT_CPS_Groups.update({



#        'RATE:CPS:HLT_xe70_L1XE50' : ['xe70_L1XE50',
#                                      'xe75_L1XE50',
#                                      'xe80_L1XE50',
#                                      'xe90_L1XE50',
#                                      'xe110_mht_xe70_L1XE50',
#                                      'xe110_mht_xe75_L1XE50',
#                                      'xe110_mht_xe80_L1XE50', 
#                                      ],
                })
    if "v5" in TriggerFlags.triggerMenuSetup():
     HLT_CPS_Groups.update({
        'RATE:CPS:HLT_xe35_BGRP7' : ['xe35_L1XE35_BGRP7',
                                     'xe35_tc_lcw_L1XE35_BGRP7',
                                     'xe35_tc_em_L1XE35_BGRP7',
                                     'xe35_pueta_L1XE35_BGRP7',
                                     'xe35_pufit_L1XE35_BGRP7',
                                     'xe35_mht_L1XE35_BGRP7',
                                     'xe35_L2FS_L1XE35_BGRP7',
                                     'xe35_l2fsperf_wEFMuFEB_wEFMu_L1XE35_BGRP7',
                                     'xe35_wEFMu_L1XE35_BGRP7',
                                     'xe35_tc_lcw_wEFMu_L1XE35_BGRP7',
                                     'xe35_tc_em_wEFMu_L1XE35_BGRP7',
                                     'xe35_mht_wEFMu_L1XE35_BGRP7',
                                     'xe35_pueta_wEFMu_L1XE35_BGRP7',
                                     'xe35_pueta_wEFMu_L1XE35_BGRP7',
                                     'xe35_pufit_wEFMu_L1XE35_BGRP7',
                                     ],
        'RATE:CPS:HLT_xe50_BGRP7' : ['xe50_L1XE35_BGRP7',
                                     'xe50_tc_lcw_L1XE35_BGRP7',
                                     'xe50_tc_em_L1XE35_BGRP7',   
                                     'xe50_mht_L1XE35_BGRP7',
                                     'xe50_pueta_L1XE35_BGRP7',
                                     'xe50_pufit_L1XE35_BGRP7',
                                     ],
        'RATE:CPS:HLT_xe60_BGRP7' : ['xe60_L1XE40_BGRP7',
                                     'xe60_tc_lcw_L1XE40_BGRP7',
                                     'xe60_tc_em_L1XE40_BGRP7',   
                                     'xe60_mht_L1XE40_BGRP7',
                                     'xe60_pueta_L1XE40_BGRP7',
                                     'xe60_pufit_L1XE40_BGRP7',
                                     'xe60_wEFMu_L1XE40_BGRP7',
                                     'xe60_tc_lcw_wEFMu_L1XE40_BGRP7',
                                     'xe60_tc_em_wEFMu_L1XE40_BGRP7',
                                     'xe60_mht_wEFMu_L1XE40_BGRP7',
                                     'xe60_pueta_wEFMu_L1XE40_BGRP7',
                                     'xe60_pufit_wEFMu_L1XE40_BGRP7',
                                     ],
        'RATE:CPS:HLT_xe70_L1XE50' : ['xe70_wEFMu_L1XE50_BGRP7',
                                      'xe70_tc_lcw_wEFMu_L1XE50_BGRP7',
                                      'xe70_tc_em_wEFMu_L1XE50_BGRP7',
                                      'xe70_mht_wEFMu_L1XE50_BGRP7',
                                      'xe70_pueta_wEFMu_L1XE50_BGRP7',
                                      'xe70_pufit_wEFMu_L1XE50_BGRP7',
                                      'xe70_L1XE50_BGRP7',
                                      'xe70_tc_lcw_L1XE50_BGRP7',
                                      'xe70_tc_em_L1XE50_BGRP7',
                                      'xe70_mht_L1XE50_BGRP7',
                                      'xe70_pueta_L1XE50_BGRP7',
                                      'xe70_pufit_L1XE50_BGRP7',
                                      ],
        'RATE:CPS:HLT_xe80_L1XE50_BGRP7' : ['xe80_L1XE50_BGRP7',
                                            'xe80_tc_lcw_L1XE50_BGRP7',
                                            'xe80_tc_em_L1XE50_BGRP7',
                                            'xe80_mht_L1XE50_BGRP7',
                                            'xe80_pueta_L1XE50_BGRP7',
                                            'xe80_pufit_L1XE50_BGRP7',
                                            'xe80_wEFMu_L1XE50_BGRP7',
                                            'xe80_tc_lcw_wEFMu_L1XE50_BGRP7',
                                            'xe80_tc_em_wEFMu_L1XE50_BGRP7',
                                            'xe80_mht_wEFMu_L1XE50_BGRP7',
                                            'xe80_pueta_wEFMu_L1XE50_BGRP7',
                                            'xe80_pufit_wEFMu_L1XE50_BGRP7',
                                            ],
        'RATE:CPS:HLT_xe80_L1XE60_BGRP7' : ['xe80_L1XE60_BGRP7',
                                            'xe80_tc_lcw_L1XE60_BGRP7',
                                            'xe80_tc_em_L1XE60_BGRP7',
                                            'xe80_mht_L1XE60_BGRP7',
                                            'xe80_pueta_L1XE60_BGRP7',
                                            'xe80_pufit_L1XE60_BGRP7',
                                            'xe80_wEFMu_L1XE60_BGRP7',
                                            'xe80_tc_lcw_wEFMu_L1XE60_BGRP7',
                                            'xe80_tc_em_wEFMu_L1XE60_BGRP7',
                                            'xe80_mht_wEFMu_L1XE60_BGRP7',
                                            'xe80_pueta_wEFMu_L1XE60_BGRP7',
                                            'xe80_pufit_wEFMu_L1XE60_BGRP7',
                                            ],
        'RATE:CPS:HLT_xe90_L1XE70_BGRP7' : ['xe90_L1XE70_BGRP7',
                                            'xe90_tc_lcw_L1XE70_BGRP7',
                                            'xe90_tc_em_L1XE70_BGRP7',
                                            'xe90_mht_L1XE70_BGRP7',
                                            'xe90_pueta_L1XE70_BGRP7',
                                            'xe90_pufit_L1XE70_BGRP7',
                                            'xe90_wEFMu_L1XE70_BGRP7',
                                            'xe90_tc_lcw_wEFMu_L1XE70_BGRP7',
                                            'xe90_tc_em_wEFMu_L1XE70_BGRP7',
                                            'xe90_mht_wEFMu_L1XE70_BGRP7',
                                            'xe90_pueta_wEFMu_L1XE70_BGRP7',
                                            'xe90_pufit_wEFMu_L1XE70_BGRP7',
                                            ],
					    
         'RATE:CPS:HLT_mu_jet_L1MU4' : ['mu4_j15_boffperf_split_dr05_dz02',
                                       'mu4_j25_boffperf_split_dr05_dz02',
                                       'mu4_j35_boffperf_split_dr05_dz02',
				       ],   
        'RATE:CPS:HLT_mu_jet_L1MU6_J40' : [
					'mu6_j150_boffperf_split_dr05_dz02',
					'mu6_j175_boffperf_split_dr05_dz02',	
				       ],
        'RATE:CPS:HLT_mu_jet_L1MU6_J75' : [
						'mu6_j260_boffperf_split_dr05_dz02',
						'mu6_j320_boffperf_split_dr05_dz02',
						'mu6_j400_boffperf_split_dr05_dz02',     
				       ],      
					    }) 
    if "v6" in TriggerFlags.triggerMenuSetup():
        HLT_CPS_Groups.update({
        #Jet chains
        'RATE:CPS:HLT_j35' : ['j35',
                              'j35_jes',
                              'j35_lcw',
                              'j35_lcw_jes',
                              'j35_lcw_nojcalib',
                              'j35_nojcalib',
                              ],

        'RATE:CPS:HLT_j400' : ['j400',
                               'j400_jes',
                               'j400_lcw',
                               'j400_lcw_jes',
                               'j400_nojcalib',
                               'j400_sub',
                          
        ],
        'RATE:CPS:HLT_j420' : ['j420',
                               'j420_jes',
                               'j420_lcw',
                               'j420_lcw_jes',
                               'j420_lcw_nojcalib',
                             ],

        'RATE:CPS:HLT_j85_280eta320' : ['j85_280eta320',
                                        'j85_280eta320_jes',
                                        'j85_280eta320_lcw',
                                        'j85_280eta320_lcw_jes',
                                        'j85_280eta320_lcw_nojcalib',
                                        'j85_280eta320_nojcalib',
                                        ],

        'RATE:CPS:HLT_j260_320eta490' : ['j260_320eta490',
                                         'j260_320eta490_jes',
                                         'j260_320eta490_lcw',
                                         'j260_320eta490_lcw_jes',
                                         'j260_320eta490_lcw_nojcalib',
                                         'j260_320eta490_nojcalib',
                                        ],

        'RATE:CPS:HLT_j360_320eta490' : ['j360_320eta490',
                                         'j360_320eta490_jes',
                                         'j360_320eta490_lcw',
                                         'j360_320eta490_lcw_jes',
                                         'j360_320eta490_lcw_nojcalib',
                                         'j360_320eta490_nojcalib',
                                        ],

        'RATE:CPS:HLT_j175_320eta490' : ['j175_320eta490',
                                         'j175_320eta490_jes',
                                         'j175_320eta490_lcw',
                                         'j175_320eta490_lcw_jes',
                                         'j175_320eta490_lcw_nojcalib',
                                         'j175_320eta490_nojcalib',
                                        ],

        'RATE:CPS:HLT_5j85' : ['5j85',
                               '5j85_jes',
                               '5j85_lcw',
                               '5j85_lcw_jes',
                               '5j85_lcw_nojcalib',
                               '5j85_nojcalib',
                              ],
       
       'RATE:CPS:HLT_j300_a10_sub_L1J75':[ 'j300_a10_sub_L1J75',
                                           'j300_a10_lcw_L1J75',
                                           'j300_a10r_L1J75',
                                          # 'j300_a10_lcw_sub_L1SC85'  ## can this work??
                                          # 'j300_a10_lcw_L1SC85'  ## can this work??
       ],
            
       'RATE:CPS:HLT_j260_a10_sub_L1J75':[ 'j260_a10_sub_L1J75',
                                           'j260_a10_lcw_sub_L1J75',
                                           'j260_a10_lcw_nojcalib_L1J75',
                                           'j260_a10_lcw_L1J75',
                                           'j260_a10_nojcalib_L1J75',
                                          # 'j260_a10_lcw_sub_L1SC85'  ## can this work??
                                          # 'j260_a10_lcw_L1SC85'  ## can this work??
       ],
            
        'RATE:CPS:HLT_j360_a10L1J100' : [ 'j360_a10r_L1J100' ,            
                                          'j360_a10_lcw_L1J100',],
        'RATE:CPS:HLT_j380_a10L1J100' : [ 'j380_a10r_L1J100' ,            
                                          'j380_a10_lcw_L1J100',],
        'RATE:CPS:HLT_j400_a10' : ['j400_a10_lcw_L1J100',
                                   #'j400_a10_lcw_sub_L1J100',
                                   #'j400_a10_sub_L1J100',
                                   'j400_a10r_L1J100',
        ],
        'RATE:CPS:HLT_j420_a10' : ['j420_a10_lcw_L1J100',
                                   #'j420_a10_lcw_sub_L1J100',
                                   #'j420_a10_sub_L1J100',
                                   'j420_a10r_L1J100',
        ],
        
        'RATE:CPS:HLT_j440_a10' : ['j440_a10_lcw_L1J100',
                                   #'j440_a10_lcw_sub_L1J100',
                                   'j440_a10r_L1J100',
        ],
        'RATE:CPS:HLT_j460_a10L1J100' : [	'j460_a10_sub_L1J100',
        																	'j460_a10_nojcalib_L1J100',
        																	'j460_a10_lcw_nojcalib_L1J100',
        																	'j460_a10_lcw_sub_L1J100',
        																	'j460_a10_lcw_L1J100',],
        'RATE:CPS:HLT_j85' : ['j85',
                              'j85_jes',
        ],
		

        'RATE:CPS:HLT_4j85' : ['4j85',
															 '4j85_jes',
															 '4j85_lcw',
															 '4j85_lcw_jes',
															 '4j85_lcw_nojcalib',
															 '4j85_nojcalib',
															],

        #Met chains
        'RATE:CPS:HLT_xe35' : ['xe35',
                               'xe35_pueta',
                               'xe35_wEFMu',
                               'xe35_tc_lcw_wEFMu', 
                               'xe35_tc_em_wEFMu',
                               'xe35_mht_wEFMu',
                               'xe35_pueta_wEFMu',
                               'xe35_pufit_wEFMu',
                               ],

        'RATE:CPS:HLT_xe70_L1XE50' : ['xe70_L1XE50',
                                      'xe75_L1XE50',
                                      'xe80_L1XE50',
                                      'xe90_L1XE50',
                                      'xe110_mht_L1XE50_AND_xe70_L1XE50',
                                      'xe110_mht_L1XE50_AND_xe75_L1XE50',
                                      'xe110_mht_L1XE50_AND_xe80_L1XE50', 
                                      ],

        'RATE:CPS:HLT_xe75_L1XE55' : ['xe75_L1XE55',
                                      'xe80_L1XE55',
                                      'xe90_L1XE55',],

        
        'RATE:CPS:HLT_xe90_L1XE50' : ['xe90_tc_lcw_L1XE50', 
                                      'xe90_mht_L1XE50',   
                                      'xe90_tc_lcw_wEFMu_L1XE50',
                                      'xe90_mht_wEFMu_L1XE50',   
                                      ],                            
         
        'RATE:CPS:HLT_xe100_L1XE50' : ['xe100_L1XE50',  
                                       'xe100_tc_lcw_L1XE50', 
                                       'xe100_tc_em_L1XE50',  
                                       'xe100_mht_L1XE50',    
   #                                    'xe100_pueta_L1XE50',  
   #                                    'xe100_pufit_L1XE50',  
                                       'xe100_wEFMu_L1XE50',  
                                       'xe100_tc_lcw_wEFMu_L1XE50',
                                       'xe100_tc_em_wEFMu_L1XE50', 
                                       'xe100_mht_wEFMu_L1XE50',   
   #                                    'xe100_pueta_wEFMu_L1XE50', 
   #                                    'xe100_pufit_wEFMu_L1XE50', 
                                       ],         

        'RATE:CPS:HLT_xe100_L1XE35' : ['xe100_L1XE35',  
                                       'xe100_tc_lcw_L1XE35', 
                                       'xe100_tc_em_L1XE35',  
                                       'xe100_mht_L1XE35',    
                                       'xe100_tc_lcw_wEFMu_L1XE35',
                                       'xe100_tc_em_wEFMu_L1XE35', 
                                       'xe100_mht_wEFMu_L1XE35',   
                                       'xe100_wEFMu_L1XE35',   
                                       ],   
        })
    if "v6" in TriggerFlags.triggerMenuSetup() or "v7" in TriggerFlags.triggerMenuSetup():
        HLT_CPS_Groups.update({

          #Moved from physics to MC menu: ATR-17795
          #"RATE:CPS:HLT_razor"   :  [''
           #                          'j30_xe10_razor100',
           #                          'j30_xe60_razor100',
           #                          'j30_xe10_razor170',
           #                          'j30_xe10_razor185',
           #                          'j30_xe10_razor195',
           #                          'j30_xe60_razor170',
           #                          'j30_xe60_razor185',
           #                          ],	

					  					 
#        "RATE:CPS:HLT_e15_vloose_L1EM7" : ['e15_lhvloose_L1EM7',
#                                           'e15_lhvloose_nod0_L1EM7'],



        })

    if "v7" in TriggerFlags.triggerMenuSetup():
        HLT_CPS_Groups.update({

        # Single-jet chains
        
        'RATE:CPS:HLT_j450' :   [   'j450',
                                ],
        

        'RATE:CPS:HLT_j260_320eta490' : [   'j260_320eta490',
                                            'j260_320eta490_sub',
                                            'j260_320eta490_nojcalib',
                                        ],
        

        # Large-R jet chains


        'RATE:CPS:HLT_j480_a10_L1J100': [   'j480_a10_lcw_subjes_L1J100',
                                            'j480_a10t_lcw_jes_L1J100',
                                            'j480_a10r_L1J100',
                                        ],




                                            
        })
    return (HLT_CPS_Groups)

def addCPS(triggerPythonConfig,signatureOverwritten):
    HLT_CPS_Groups = defineCPSGroups()
    genericCPSAdder(HLT_CPS_Groups, triggerPythonConfig.allChains, triggerPythonConfig.theHLTChains, "HLT",signatureOverwritten)
