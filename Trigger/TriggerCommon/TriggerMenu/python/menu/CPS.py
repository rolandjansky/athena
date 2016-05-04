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
        "RATE:CPS:HLT_e5_vloose"  :  ['e5_lhvloose',               
                                      'e5_lhvloose_nod0',],
        

        ########################################################################

        "RATE:CPS:HLT_e12_vloose_L1EM10VH"        : ['e12_vloose_L1EM10VH', 
                                                     'e12_lhvloose_L1EM10VH',
                                                     'e12_lhvloose_nod0_L1EM10VH'],
        
        "RATE:CPS:HLT_e15_vloose_L1EM13VH"        : ['e15_vloose_L1EM13VH', 
                                                     'e15_lhvloose_L1EM13VH',
                                                     'e15_lhvloose_nod0_L1EM13VH'],
        
        "RATE:CPS:HLT_e17_vloose"        : ['e17_vloose', 
                                            'e17_lhvloose',
                                            'e17_lhvloose_nod0'],
        
        "RATE:CPS:HLT_e20_vloose"        : ['e20_vloose', 
                                            'e20_lhvloose', 
                                            'e20_lhvloose_nod0'],

        "RATE:CPS:HLT_e40_vloose"        : ['e40_vloose', 
                                            'e40_lhvloose', 
                                            'e40_lhvloose_nod0'],

        "RATE:CPS:HLT_e60_vloose"        : ['e60_vloose', 
                                            'e60_lhvloose', 
                                            'e60_lhvloose_nod0'],


        "RATE:CPS:HLT_e24_medium_L1EM18VH" : ['e24_lhmedium_L1EM18VH',
                                                'e24_lhmedium_nod0_L1EM18VH', 
                                                ],
        
        "RATE:CPS:HLT_e24_tight_L1EM20VH" :  ['e24_lhtight_L1EM20VH',
                                                'e24_lhtight_nod0_L1EM20VH',
                                                ],
        
        'RATE:CPS:HLT_e9_tight_e4_etcut_Jpsiee' :['e9_tight_e4_etcut_Jpsiee',
                                                 'e9_lhtight_e4_etcut_Jpsiee',
                                                 'e9_lhtight_nod0_e4_etcut_Jpsiee'],

        'RATE:CPS:HLT_e9_etcut_e5_tight_Jpsiee': ['e9_etcut_e5_tight_Jpsiee',
                                                 'e9_etcut_e5_lhtight_Jpsiee',
                                                 'e9_etcut_e5_lhtight_nod0_Jpsiee'],
        
        'RATE:CPS:HLT_e14_tight_e4_etcut_Jpsiee': ['e14_tight_e4_etcut_Jpsiee',
                                                  'e14_lhtight_e4_etcut_Jpsiee',
                                                  'e14_lhtight_nod0_e4_etcut_Jpsiee'],
        
        'RATE:CPS:HLT_e14_etcut_e5_tight_Jpsiee' :['e14_etcut_e5_tight_Jpsiee',
                                                  'e14_etcut_e5_lhtight_Jpsiee',
                                                  'e14_etcut_e5_lhtight_nod0_Jpsiee'],

        #'RATE:CPS:HLT_e5_tight_e4_etcut_L1JPSI-1M5' :['e5_tight_e4_etcut_L1JPSI-1M5',
        #                                             'e5_tight_e4_etcut_Jpsiee_L1JPSI-1M5',] ,
        #
        #
        #'RATE:CPS:HLT_e5_lhtight_e4_etcut_L1JPSI-1M5' : ['e5_lhtight_e4_etcut_L1JPSI-1M5',
        #                                                 'e5_lhtight_e4_etcut_Jpsiee_L1JPSI-1M5',],


        'RATE:CPS:HLT_e24_vloose_L1EM18VH': ['e24_vloose_L1EM18VH',
                                             'e24_lhvloose_L1EM18VH',
                                             'e24_lhvloose_nod0_L1EM18VH',],
        
        'RATE:CPS:HLT_e24_vloose_L1EM20VH' :['e24_vloose_L1EM20VH',
                                             'e24_lhvloose_L1EM20VH',
                                             'e24_lhvloose_nod0_L1EM20VH',],
        
        'RATE:CPS:HLT_e26_vloose_L1EM20VH' : ['e26_vloose_L1EM20VH',
                                              'e26_lhvloose_L1EM20VH',
                                              'e26_lhvloose_nod0_L1EM20VH',],
        
        'RATE:CPS:HLT_e5_tight_e4_etcut' : ['e5_tight_e4_etcut',
                                            'e5_lhtight_e4_etcut',
                                            'e5_lhtight_nod0_e4_etcut',],

        'RATE:CPS:HLT_e5_tight_e4_etcut_Jpsiee' : ['e5_tight_e4_etcut_Jpsiee',
                                                   'e5_lhtight_e4_etcut_Jpsiee',
                                                   'e5_lhtight_nod0_e4_etcut_Jpsiee',],
        
        'RATE:CPS:HLT_e5_loose_idperf' : ['e5_loose_idperf',
                                          'e5_lhloose_idperf',],
        
        'RATE:CPS:HLT_e5_tight_idperf':['e5_tight_idperf',
                                        'e5_lhtight_idperf',],
        
        'RATE:CPS:HLT_e10_tight_idperf' :['e10_tight_idperf',
                                          'e10_lhtight_idperf',],
        
        'RATE:CPS:HLT_e24_medium_idperf_L1EM20VH' :[ 'e24_medium_idperf_L1EM20VH',
                                                     'e24_lhmedium_idperf_L1EM20VH',],
        
        
        
        
 	       
        'RATE:CPS:HLT_e24_lhvloose_L1EM20VH_3j20noL1' : ['e24_lhvloose_L1EM20VH_3j20noL1',
                                                         'e24_lhvloose_nod0_L1EM20VH_3j20noL1',],
      
        'RATE:CPS:HLT_e24_lhvloose_L1EM20VH_4j20noL1' : ['e24_lhvloose_L1EM20VH_4j20noL1',
                                                       	 'e24_lhvloose_nod0_L1EM20VH_4j20noL1',],
      
        'RATE:CPS:HLT_e24_lhvloose_L1EM20VH_5j15noL1' : ['e24_lhvloose_L1EM20VH_5j15noL1',
                                                         'e24_lhvloose_nod0_L1EM20VH_5j15noL1',],
      
        'RATE:CPS:HLT_e24_lhvloose_L1EM20VH_6j15noL1' : ['e24_lhvloose_L1EM20VH_6j15noL1',
                                                         'e24_lhvloose_nod0_L1EM20VH_6j15noL1',],
      
        'RATE:CPS:HLT_e26_lhvloose_L1EM20VH_3j20noL1' : ['e26_lhvloose_L1EM20VH_3j20noL1',
                                                         'e26_lhvloose_nod0_L1EM20VH_3j20noL1',],
      
        'RATE:CPS:HLT_e26_lhvloose_L1EM20VH_4j20noL1' : ['e26_lhvloose_L1EM20VH_4j20noL1',
                                                         'e26_lhvloose_nod0_L1EM20VH_4j20noL1',],
      
        'RATE:CPS:HLT_e26_lhvloose_L1EM20VH_5j15noL1' : ['e26_lhvloose_L1EM20VH_5j15noL1',
                                                         'e26_lhvloose_nod0_L1EM20VH_5j15noL1',],
      
        'RATE:CPS:HLT_e26_lhvloose_L1EM20VH_6j15noL1' : ['e26_lhvloose_L1EM20VH_6j15noL1',
                                                         'e26_lhvloose_nod0_L1EM20VH_6j15noL1',],
      
        'RATE:CPS:HLT_e5_loose' : ['e5_loose',
                                   'e5_lhloose',],
        
        'RATE:CPS:HLT_2e12_vloose_L12EM10VH' :['2e12_lhvloose_L12EM10VH',
                                               '2e12_lhvloose_nod0_L12EM10VH',],

        'RATE:CPS:HLT_e18_etcut_trkcut_xs20' :['e18_etcut_trkcut_xs20',
                                               'e18_etcut_trkcut_xs20_mt35',
                                               'e18_etcut_trkcut_j20_perf_xe20_6dphi15',
                                               'e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35',
                                               'e18_etcut_trkcut_xs20_j20_perf_xe20_6dphi15',
                                               'e18_etcut_trkcut_xs20_j20_perf_xe20_6dphi15_mt35',],

        'RATE:CPS:HLT_e13_etcut_trkcut_xs15' :['e13_etcut_trkcut_xs15',
                                               'e13_etcut_trkcut_xs15_mt25',
                                               'e13_etcut_trkcut_j20_perf_xe15_6dphi05',
                                               'e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25',
                                               'e13_etcut_trkcut_xs15_j20_perf_xe15_6dphi05',
                                               'e13_etcut_trkcut_xs15_j20_perf_xe15_6dphi05_mt25',
                                               'e13_etcut_trkcut_j20_perf_xe15_2dphi05',
                                               'e13_etcut_trkcut_j20_perf_xe15_2dphi05_mt25',
                                               'e13_etcut_trkcut_xs15_j20_perf_xe15_2dphi05',
                                               'e13_etcut_trkcut_xs15_j20_perf_xe15_2dphi05_mt25',],

        #tau chains   
        'RATE:CPS:HLT_tau25_express'         :['tau25_idperf_track',
                                               'tau25_perf_tracktwo',
                                               'tau25_idperf_tracktwo',
                                               'tau25_medium1_tracktwo',],  
        'RATE:CPS:HLT_tau160'		     :['tau160_idperf_track',
                                               'tau160_idperf_tracktwo',
                                               'tau160_perf_track',
                                               'tau160_perf_tracktwo',],  
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
        
        #BJet chains                                                                                                                                                    
        'RATE:CPS:HLT_j15b' : ['j15_bperf',
                               'j15_bperf_split',
                               'j15_boffperf',
                               'j15_boffperf_split',
                               ],


        'RATE:CPS:HLT_j35b' : ['j35_boffperf',
                               'j35_boffperf_split',
                               ],

        
        'RATE:CPS:HLT_j55b' : ['j55_bperf',
                               'j55_bperf_split',
                               'j55_boffperf',
                               'j55_boffperf_split',
                               ],

        'RATE:CPS:HLT_j85b' : ['j85_bperf',
                               'j85_bperf_split',
                               'j85_boffperf',
                               'j85_boffperf_split',
                               ],


        'RATE:CPS:HLT_j150b' : ['j150_bperf',
                                'j150_bperf_split',
                                'j150_boffperf',
                                'j150_boffperf_split',
                               ],

        'RATE:CPS:HLT_j320b' : ['j320_bperf',
                                'j320_bperf_split',
                                'j320_boffperf',
                                'j320_boffperf_split',
                               ],


        'RATE:CPS:HLT_j45b_3j45' : ['j45_bperf_3j45',
                                    'j45_bperf_split_3j45',
                                    'j45_boffperf_3j45',
                                    'j45_boffperf_split_3j45',
                               ],


        #Met chains
        'RATE:CPS:HLT_xe35' : ['xe35',
                               'xe35_tc_lcw',
                               'xe35_tc_em',
                               'xe35_pueta',
                               'xe35_pufit',
                               'xe35_mht',
                               'xe35_wEFMu',
                               'xe35_tc_lcw_wEFMu', 
                               'xe35_tc_em_wEFMu',
                               'xe35_mht_wEFMu',
                               'xe35_pueta_wEFMu',
                               'xe35_pufit_wEFMu',
                               ],

        
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
         
        #Bphys	

        'RATE:CPS:HLT_2mu4_bDimu' : ['2mu4_bDimu',                
                                     '2mu4_bDimu_novtx_noos',   
                                     ],

        "RATE:CPS:HLT_2mu6_bDimu"  :  ['2mu6_bDimu',
                                       '2mu6_bDimu_novtx_noos',
                                       ],
				       
        'RATE:CPS:HLT_mu6_mu4_bBmumuX' : ['mu6_mu4_bBmumuxv2',            
                                          'mu6_mu4_bBmumux_BcmumuDsloose',
                                          'mu6_mu4_bBmumux_BpmumuKp',
                                          'mu6_mu4_bJpsimumu', 
                                          ],
						  
        'RATE:CPS:HLT_mu6_mu4_bBmumu'   : ['mu6_mu4_bBmumu',],
				 
        'RATE:CPS:HLT_2mu6_bBmumuX'   :  ['2mu6_bBmumuxv2',
                                          '2mu6_bBmumux_BcmumuDsloose',
                                          '2mu6_bBmumux_BpmumuKp',
                                          '2mu6_bJpsimumu',
                                         ],

        'RATE:CPS:HLT_mu6_mu4_bDimu' : ['mu6_mu4_bDimu',                
                                        'mu6_mu4_bDimu_novtx_noos',
                                        ],


        'RATE:CPS:HLT_3mu4_bDimu' : ['3mu4_bDimu',
                                     '3mu4_bJpsi',
                                     '3mu4_bTau', 
                                     '3mu4_bUpsi',
                                     ],

        'RATE:CPS:HLT_mu20_L1MU15' : ['mu20_L1MU15',
                                      'mu24_L1MU15',
                                      ],
        
        # mu+jets chains 
        'RATE:CPS:HLT_mu_jet_L1MU4' : ['mu4_j15_bperf_split_dr05_dz02',
                                       'mu4_j25_bperf_split_dr05_dz02',
                                       'mu4_j35_bperf_split_dr05_dz02',
				       'mu4_j35_bperf_split_dr05',
				       'mu4_j35_bperf_split',
				       ],
				       
				       
        'RATE:CPS:HLT_mu_jet_L1MU6_J20' : [	
					'mu6_j85_bperf_split_dr05_dz02',
					'mu6_j110_bperf_split_dr05_dz02',
				       ],
        'RATE:CPS:HLT_mu_jet_L1MU6_J40' : [
					'mu6_j150_bperf_split_dr05_dz02',
					'mu6_j150_bperf_split_dr05',
					'mu6_j150_bperf_split',
					'mu6_j175_bperf_split_dr05_dz02',	
				       ],
        'RATE:CPS:HLT_mu_jet_L1MU6_J75' : [
						'mu6_j260_bperf_split_dr05_dz02',
						'mu6_j320_bperf_split_dr05_dz02',
						'mu6_j320_bperf_split_dr05',
						'mu6_j320_bperf_split',
						'mu6_j400_bperf_split_dr05_dz02',     
				       ],			    
        }
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
					    
         'RATE:CPS:HLT_mu_jet_L1MU4' : ['mu4_j15_bperf_split_dr05_dz02',
                                       'mu4_j25_bperf_split_dr05_dz02',
                                       'mu4_j35_bperf_split_dr05_dz02',
				       ],   
        'RATE:CPS:HLT_mu_jet_L1MU6_J40' : [
					'mu6_j150_bperf_split_dr05_dz02',
					'mu6_j175_bperf_split_dr05_dz02',	
				       ],
        'RATE:CPS:HLT_mu_jet_L1MU6_J75' : [
						'mu6_j260_bperf_split_dr05_dz02',
						'mu6_j320_bperf_split_dr05_dz02',
						'mu6_j400_bperf_split_dr05_dz02',     
				       ],
					    }) 

    if "v6" in TriggerFlags.triggerMenuSetup():
        HLT_CPS_Groups.update({


          'RATE:CPS:HLT_mu10_mu6_bBmumuX'   :  ['mu10_mu6_bBmumuxv2',
                                      'mu10_mu6_bBmumux_BcmumuDsloose',
                                      'mu10_mu6_bBmumux_BpmumuKp',
                                      'mu10_mu6_bJpsimumu',
                                      'mu10_mu6_bJpsimumu_Lxy0',
                                     ],

          "RATE:CPS:HLT_razor"   :  [''
                                     'j30_xe10_razor100',
                                     'j30_xe60_razor100',
                                     'j30_xe10_razor170',
                                     'j30_xe10_razor185',
                                     'j30_xe10_razor195',
                                     'j30_xe60_razor170',
                                     'j30_xe60_razor185',
                                     ],	

         "RATE:CPS:HLT_mu6_mu4_bBmumu"   : ['mu6_mu4_bBmumu',
	                                    'mu6_mu4_bBmumu_Lxy0',],
	 "RATE:CPS:HLT_mu6_2mu4"         : ['mu6_2mu4_bJpsi',
                                      'mu6_2mu4_bTau_noL2',
												 					    'mu6_2mu4_bUpsi',
												 					    'mu6_2mu4',
                                            ],

        'RATE:CPS:HLT_2mu6_bBmumuX'   :  ['2mu6_bBmumuxv2',
                                          '2mu6_bBmumux_BcmumuDsloose',
                                          '2mu6_bBmumux_BpmumuKp',
                                          '2mu6_bJpsimumu',
                                          '2mu6_bJpsimumu_Lxy0',
                                         ],
        'RATE:CPS:HLT_mu6_mu4_bBmumuX' : ['mu6_mu4_bBmumuxv2',            
                                          'mu6_mu4_bBmumux_BcmumuDsloose',
                                          'mu6_mu4_bBmumux_BpmumuKp',
                                          'mu6_mu4_bJpsimumu', 
                                          'mu6_mu4_bJpsimumu_Lxy0',
                                          ],
					  					 
         "RATE:CPS:HLT_e10_vloose_L1EM7" : ['e10_lhvloose_L1EM7',
                                           'e10_lhvloose_nod0_L1EM7',
                                           'e15_lhvloose_nod0_L1EM7',
                                           'e15_lhvloose_L1EM7',
                                           'e15_lhvloose_nod0_L1EM7',
                                            ],
                                           
        
#        "RATE:CPS:HLT_e15_vloose_L1EM7" : ['e15_lhvloose_L1EM7',
#                                           'e15_lhvloose_nod0_L1EM7'],


        "RATE:CPS:HLT_e20_vloose_L1EM12" : ['e20_lhvloose_L1EM12',
                                            'e20_lhvloose_nod0_L1EM12',],
                                            
   
        "RATE:CPS:HLT_e25_vloose_L1EM15" : ['e25_lhvloose_L1EM15',
                                            'e25_lhvloose_nod0_L1EM15'],

        "RATE:CPS:HLT_e30_vloose_L1EM15" : ['e30_lhvloose_L1EM15',
                                            'e30_lhvloose_nod0_L1EM15'],

        "RATE:CPS:HLT_e40_vloose_L1EM15" : ['e40_lhvloose_L1EM15',
                                            'e40_lhvloose_nod0_L1EM15'],

        "RATE:CPS:HLT_e50_vloose_L1EM15" : ['e50_lhvloose_L1EM15',
                                            'e50_lhvloose_nod0_L1EM15'],
        
        "RATE:CPS:HLT_e70_vloose"        : ['e70_lhvloose',
                                            'e70_lhvloose_nod0'],

        "RATE:CPS:HLT_e80_vloose"        : ['e80_lhvloose',
                                            'e80_lhvloose_nod0',],
        
        "RATE:CPS:HLT_e100_vloose"        : ['e100_lhvloose',
                                             'e100_lhvloose_nod0'],
        
        "RATE:CPS:HLT_e120_vloose"        : ['e120_lhvloose',
                                             'e120_lhvloose_nod0'],
            
        "RATE:CPS:HLT_e140_vloose"        : ['e140_lhvloose',
                                             'e140_lhvloose_nod0'],
            

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
                                           'j260_a10r_L1J75',
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
                
        'RATE:CPS:HLT_e28_lhvloose_L1EM20VH_3j20noL1' : ['e28_lhvloose_L1EM20VH_3j20noL1',
                                                         'e28_lhvloose_nod0_L1EM20VH_3j20noL1',],
      
        'RATE:CPS:HLT_e28_lhvloose_L1EM20VH_4j20noL1' : ['e28_lhvloose_L1EM20VH_4j20noL1',
                                                         'e28_lhvloose_nod0_L1EM20VH_4j20noL1',],
      
        'RATE:CPS:HLT_e28_lhvloose_L1EM20VH_5j15noL1' : ['e28_lhvloose_L1EM20VH_5j15noL1',
                                                         'e28_lhvloose_nod0_L1EM20VH_5j15noL1',],
      
        'RATE:CPS:HLT_e28_lhvloose_L1EM20VH_6j15noL1' : ['e28_lhvloose_L1EM20VH_6j15noL1',
                                                         'e28_lhvloose_nod0_L1EM20VH_6j15noL1',],

        'RATE:CPS:HLT_j85' : ['j85',
                              'j85_jes',
                              'j85_cleanT',
                              'j85_cleanL',
                              'j85_cleanLLP',
        ],
		

        'RATE:CPS:HLT_4j85' : ['4j85',
															 '4j85_jes',
															 '4j85_lcw',
															 '4j85_lcw_jes',
															 '4j85_lcw_nojcalib',
															 '4j85_nojcalib',
															],

        
        'RATE:CPS:HLT_3mu6_bDimu' : ['3mu6_bDimu',
                                     '3mu6_bJpsi',
                                     '3mu6_bTau', 
                                     '3mu6_bUpsi',
                                     ],

       'RATE:CPS:HLT_2mu6_bBmumu' :  ['2mu6_bBmumu',
                                      '2mu6_bBmumu_Lxy0',
                                       ],

        'RATE:CPS:HLT_xe45' : ['xe45',
 															 'xe45_mht',
 															 'xe45_mht_wEFMu',
 															 'xe45_pueta',
 															 'xe45_pueta_wEFMu',
 															 'xe45_pufit',
 															 'xe45_pufit_wEFMu',
 															 'xe45_tc_em',
 															 'xe45_tc_em_wEFMu',
 															 'xe45_tc_lcw',
 															 'xe45_tc_lcw_wEFMu',
 															 'xe45_wEFMu',
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
        
        

#        'RATE:CPS:HLT_2mu4_bJpsimumu_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4' : ['2mu4_bJpsimumu_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',
#                                                                          '2mu4_bJpsimumu_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4',
#                                                                          '2mu4_bJpsimumu_noL2_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',
#                                                                          '2mu4_bJpsimumu_noL2_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4',
#                                                                          ],

#        'RATE:CPS:HLT_2mu4_bUpsimumu_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-BO' : ['2mu4_bUpsimumu_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-BO',
#                                                                           '2mu4_bUpsimumu_L1BPH-7M15-2MU4_BPH-0DR24-2MU4-B',
#                                                                           ],

#        'RATE:CPS:HLT_2mu4_bBmumu_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4' : ['2mu4_bBmumu_L1BPH-2M8-2MU4-BO_BPH-0DR15-2MU4',
#                                                                       '2mu4_bBmumu_L1BPH-2M8-2MU4-B_BPH-0DR15-2MU4',
#                                                                       ],


        

        
        })

    return (HLT_CPS_Groups)

def addCPS(triggerPythonConfig,signatureOverwritten):
    HLT_CPS_Groups = defineCPSGroups()
    genericCPSAdder(HLT_CPS_Groups, triggerPythonConfig.allChains, triggerPythonConfig.theHLTChains, "HLT",signatureOverwritten)
