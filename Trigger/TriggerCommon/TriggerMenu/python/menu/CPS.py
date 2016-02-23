# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""CPS addition  """

__doc__="Addition of coherent prescales to the list of chains"

from TriggerJobOpts.TriggerFlags  import TriggerFlags
from TriggerMenu.l1.Lvl1Flags import Lvl1Flags

from AthenaCommon.Logging import logging

log = logging.getLogger('TriggerMenu.CPS.py')


def hackedCPSAdder(groups, signatures, chains, level,signatureOverwritten):
#     Emanuel: This is an ugly, ugly hack to deal with mutliple
#     instances of the same chain, corresponding to a signature. A
#     proper fix will require re-imagining the filling of
#     triggerPythonConfig's theL2HLTChains and theEFHLTChains (which
#     I'll do after I get back from vacation. See Savannah #84711 for
#     more details.
    
    has_rate=[]
    has_no_rate=[]
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
                log.error("Level of chain not defined")
                
            for c in chains:
                if c.chain_name == chain_name:
                    log.info(' ===> Now in my CPS.py : level=%s, chainName=%s, cpsGroup=%s, chain_name=%s' % (level, chainName, cpsGroup, chain_name))
                    log.info('                       : signatures: [0]=%s, [-1]=%s' % (signatures[chainName][0].chain_name, signatures[chainName][-1].chain_name))
                    log.info('                       : chain: c.chain_name=%s, c.groups=%s, cpsGroup=%s' % (c.chain_name, c.groups, cpsGroup))
                    cpsFound = None
                    for g in c.groups:
                        if g.find('CPS') == 5:
                            cpsFound = g
                    if cpsFound == None:
                        log.info('                       : no CPS group found. group=%s added' % cpsGroup)
                        c.addGroup(cpsGroup)
                    elif cpsFound == cpsGroup:
                        log.info('                       : CPS group with same name already exists (%s). Nothing added' % cpsGroup)
                    else:
                        log.warning('                       : CPS group with different name already exists (%s). Group =%s not added' % (cpsFound, cpsGroup))

            #[c.addGroup(cpsGroup) for c in chains if c.chain_name == chain_name]
            





def genericCPSAdder(groups, chains, level):
    #levelMap = {"L2": 0,
    #            "EF": 1,
    #            }
    #level = levelMap[level]
    #print 'igb level', level
    
    for cpsGroup, chainNames in groups.iteritems():
        for chainName in chainNames:
            if chainName not in chains.keys():
                log.error('Signature %s not registered to TriggerPythonConfig' % chainName)
                continue

            if level=='L2':
                l2chain = chains[chainName][0]
                l2chain.addGroup(cpsGroup)
            elif level=='EF':
                if  len(chains[chainName]) ==2:
                    efchain = chains[chainName][1]
                    efchain.addGroup(cpsGroup)
                elif len(chains[chainName]) ==1:
                    efchain = chains[chainName][0]
                    efchain.addGroup(cpsGroup)

def defineCPSGroups():
    HLT_CPS_Groups = {				       
        #Egamma chains
        "RATE:CPS:HLT_e5_vloose"  :  ['e5_vloose',
                                      'e5_lhvloose',               
                                      'e5_lhvloose_nod0',],
        
        
        "RATE:CPS:HLT_e10_vloose_L1EM7" : ['e10_vloose_L1EM7',
                                           'e10_lhvloose_L1EM7',],
                                           
        
        "RATE:CPS:HLT_e15_vloose_L1EM7" : ['e15_vloose_L1EM7',          
                                           'e15_lhvloose_L1EM7'],



        "RATE:CPS:HLT_e20_vloose_L1EM12" : ['e20_vloose_L1EM12',
                                            'e20_lhvloose_L1EM12',],
                                            
   
        "RATE:CPS:HLT_e25_vloose_L1EM15" : ['e25_vloose_L1EM15',          
                                            'e25_lhvloose_L1EM15'],

        "RATE:CPS:HLT_e30_vloose_L1EM15" : ['e30_vloose_L1EM15',          
                                            'e30_lhvloose_L1EM15'],

        "RATE:CPS:HLT_e40_vloose_L1EM15" : ['e40_vloose_L1EM15',          
                                            'e40_lhvloose_L1EM15'],

        "RATE:CPS:HLT_e50_vloose_L1EM15" : ['e50_vloose_L1EM15',          
                                            'e50_lhvloose_L1EM15'],
        
        "RATE:CPS:HLT_e70_vloose"        : ['e70_vloose',          
                                            'e70_lhvloose'],

        "RATE:CPS:HLT_e80_vloose"        : ['e80_vloose',          
                                            'e80_lhvloose'],
        
        "RATE:CPS:HLT_e100_vloose"        : ['e100_vloose',          
                                             'e100_lhvloose'],
        
        "RATE:CPS:HLT_e120_vloose"        : ['e120_vloose',          
                                             'e120_lhvloose'],
        
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
                                                'e24_medium_L1EM18VH',
                                                'e24_lhmedium_nod0_L1EM18VH', 
                                                'e24_lhmedium_nodeta_L1EM18VH', 
                                                'e24_lhmedium_nodphires_L1EM18VH', 
                                                'e24_lhmedium_cutd0dphideta_L1EM18VH',
                                                ],
        
        "RATE:CPS:HLT_e24_tight_L1EM20VH" :  ['e24_lhtight_L1EM20VH',
                                                'e24_tight_L1EM20VH',
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
                                          'e5_loose_L2Star_idperf',
                                          'e5_lhloose_idperf',],
        
        'RATE:CPS:HLT_e5_tight_idperf':['e5_tight_idperf',
                                        'e5_lhtight_idperf',],
        
        'RATE:CPS:HLT_e10_tight_idperf' :['e10_tight_idperf',
                                          'e10_lhtight_idperf',],
        
        'RATE:CPS:HLT_e24_medium_idperf_L1EM20VH' :[ 'e24_medium_idperf_L1EM20VH',
                                                     'e24_lhmedium_idperf_L1EM20VH',],
        
        'RATE:CPS:HLT_e24_lhvloose_L1EM18VH_3j20noL1' : ['e24_lhvloose_L1EM18VH_3j20noL1',
                                                         'e24_lhvloose_nod0_L1EM18VH_3j20noL1',],
        
        'RATE:CPS:HLT_e24_lhvloose_L1EM18VH_4j20noL1' : ['e24_lhvloose_L1EM18VH_4j20noL1',
                                                         'e24_lhvloose_nod0_L1EM18VH_4j20noL1',],
        
        'RATE:CPS:HLT_e24_lhvloose_L1EM18VH_5j15noL1' : ['e24_lhvloose_L1EM18VH_5j15noL1',
                                                         'e24_lhvloose_nod0_L1EM18VH_5j15noL1',],
        
        'RATE:CPS:HLT_e24_lhvloose_L1EM18VH_6j15noL1' : ['e24_lhvloose_L1EM18VH_6j15noL1',
                                                         'e24_lhvloose_nod0_L1EM18VH_6j15noL1',],
 	       
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
        
        'RATE:CPS:HLT_2e12_vloose_L12EM10VH' :['2e12_vloose_L12EM10VH',
                                               '2e12_lhvloose_L12EM10VH',
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
        #Jet chains
        'RATE:CPS:HLT_j35' : ['j35',
                              'j35_jes',
                              'j35_lcw',
                              'j35_lcw_jes',
                              'j35_lcw_nojcalib',
                              'j35_nojcalib',
                              ],

        'RATE:CPS:HLT_j85' : ['j85',
                              'j85_jes',
                              'j85_lcw',
                              'j85_lcw_jes',
                              'j85_lcw_nojcalib',
                              'j85_nojcalib',
                              ],
        
        'RATE:CPS:HLT_j175' : ['j175',
                               'j175_jes',
                               'j175_lcw',
                               'j175_lcw_jes',
                               'j175_lcw_nojcalib',
                               'j175_nojcalib',
                              ],

        'RATE:CPS:HLT_j380' : ['j380',
                               'j380_jes',
                               'j380_lcw',
                               'j380_lcw_jes',
                               'j380_lcw_nojcalib',
                               'j380_nojcalib',
                              ],

        'RATE:CPS:HLT_j85_280eta320' : ['j85_280eta320',
                                        'j85_280eta320_jes',
                                        'j85_280eta320_lcw',
                                        'j85_280eta320_lcw_jes',
                                        'j85_280eta320_lcw_nojcalib',
                                        'j85_280eta320_nojcalib',
                                        ],

        'RATE:CPS:HLT_j175_320eta490' : ['j175_320eta490',
                                         'j175_320eta490_jes',
                                         'j175_320eta490_lcw',
                                         'j175_320eta490_lcw_jes',
                                         'j175_320eta490_lcw_nojcalib',
                                         'j175_320eta490_nojcalib',
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

        'RATE:CPS:HLT_4j85' : ['4j85',
                               '4j85_jes',
                               '4j85_lcw',
                               '4j85_lcw_jes',
                               '4j85_lcw_nojcalib',
                               '4j85_nojcalib',
                               ],
        #BJet chains                                                                                                                                                    
        'RATE:CPS:HLT_j15b' : ['j15_bperf',
                               'j15_bperf_split',
                               'j15_boffperf',
                               'j15_boffperf_split',
                               ],

        'RATE:CPS:HLT_j25b' : ['j25_bperf',
                               'j25_bperf_split',
                               'j25_boffperf',
                               'j25_boffperf_split',
                               ],

        'RATE:CPS:HLT_j35b' : ['j35_bperf',
                               'j35_bperf_split',
                               'j35_boffperf',
                               'j35_boffperf_split',
                               ],

        'RATE:CPS:HLT_j45b' : ['j45_bperf',
                               'j45_bperf_split',
                               'j45_boffperf',
                               'j45_boffperf_split',
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

        'RATE:CPS:HLT_j110b' : ['j110_bperf',
                                'j110_bperf_split',
                                'j110_boffperf',
                                'j110_boffperf_split',
                               ],

        'RATE:CPS:HLT_j150b' : ['j150_bperf',
                                'j150_bperf_split',
                                'j150_boffperf',
                                'j150_boffperf_split',
                               ],

        'RATE:CPS:HLT_j260b' : ['j260_bperf',
                                'j260_bperf_split',
                                'j260_boffperf',
                                'j260_boffperf_split',
                               ],


        'RATE:CPS:HLT_j320b' : ['j320_bperf',
                                'j320_bperf_split',
                                'j320_boffperf',
                                'j320_boffperf_split',
                               ],

        'RATE:CPS:HLT_j400b' : ['j400_bperf',
                                'j400_bperf_split',
                                'j400_boffperf',
                                'j400_boffperf_split',
                               ],

        'RATE:CPS:HLT_j45b_3j45' : ['j45_bperf_3j45',
                                    'j45_bperf_split_3j45',
                                    'j45_boffperf_3j45',
                                    'j45_boffperf_split_3j45',
                               ],

        'RATE:CPS:HLT_j45b_3j45_L13J20' : ['j45_bperf_3j45_L13J20',
                                           'j45_bperf_split_3j45_L13J20',
                                           'j45_boffperf_3j45_L13J20',
                                           'j45_boffperf_split_3j45_L13J20',
                                           ],

        'RATE:CPS:HLT_j45b_3j45_L13J15.0ETA25' : ['j45_bperf_3j45_L13J15.0ETA25',
                                                 'j45_bperf_split_3j45_L13J15.0ETA25',
                                                 'j45_boffperf_3j45_L13J15.0ETA25',
                                                 'j45_boffperf_split_3j45_L13J15.0ETA25',
                                                 ],
        



        #Met chains
        'RATE:CPS:HLT_xe35' : ['xe35',
                               'xe35_tc_lcw',
                               'xe35_tc_em',
                               'xe35_pueta',
                               'xe35_pufit',
                               'xe35_mht',
                               'xe35_L2FS',
                               'xe35_l2fsperf_wEFMuFEB_wEFMu',
                               'xe35_wEFMu',
                               'xe35_tc_lcw_wEFMu', 
                               'xe35_tc_em_wEFMu',
                               'xe35_mht_wEFMu',
                               'xe35_pueta_wEFMu',
                               'xe35_pufit_wEFMu',
                               ],

        'RATE:CPS:HLT_xe50' : ['xe50',      
                               'xe50_tc_lcw',
                               'xe50_tc_em',
                               'xe50_mht',  
                               'xe50_pueta',
                               'xe50_pufit',
                               ],

        'RATE:CPS:HLT_xe60' : ['xe60',            
                               'xe60_tc_lcw',     
                               'xe60_tc_em',      
                               'xe60_mht',        
                               'xe60_pueta',      
                               'xe60_pufit',      
                               'xe60_wEFMu',      
                               'xe60_tc_lcw_wEFMu',
                               'xe60_tc_em_wEFMu',
                               'xe60_mht_wEFMu',  
                               'xe60_pueta_wEFMu',
                               'xe60_pufit_wEFMu',
                               ],


        'RATE:CPS:HLT_xe70' : ['xe70_wEFMu',        
                               'xe70_tc_lcw_wEFMu', 
                               'xe70_tc_em_wEFMu',  
                               'xe70_mht_wEFMu',    
                               'xe70_pueta_wEFMu',  
                               'xe70_pufit_wEFMu',  
                               'xe70',              
                               'xe70_tc_lcw',       
                               'xe70_tc_em',        
                               'xe70_mht',          
                               'xe70_pueta',        
                               'xe70_pufit', 
                               ],       
        

        'RATE:CPS:HLT_xe80_L1XE50' : ['xe80_L1XE50',             
                                      'xe80_tc_lcw_L1XE50',      
                                      'xe80_tc_em_L1XE50',       
                                      'xe80_mht_L1XE50',         
                                      'xe80_pueta_L1XE50',       
                                      'xe80_pufit_L1XE50',       
                                      'xe80_wEFMu_L1XE50',       
                                      'xe80_tc_lcw_wEFMu_L1XE50',
                                      'xe80_tc_em_wEFMu_L1XE50', 
                                      'xe80_mht_wEFMu_L1XE50',   
                                      'xe80_pueta_wEFMu_L1XE50', 
                                      'xe80_pufit_wEFMu_L1XE50', 
                                      ],

        'RATE:CPS:HLT_xe80' : ['xe80',              
                               'xe80_tc_lcw',       
                               'xe80_tc_em',        
                               'xe80_mht',          
                               'xe80_pueta',        
                               'xe80_pufit',        
                               'xe80_wEFMu',        
                               'xe80_tc_lcw_wEFMu', 
                               'xe80_tc_em_wEFMu',  
                               'xe80_mht_wEFMu',    
                               'xe80_pueta_wEFMu',  
                               'xe80_pufit_wEFMu',  
                               ],


        'RATE:CPS:HLT_xe80_L1XE70' : ['xe80_L1XE70',             
                                      'xe80_tc_lcw_L1XE70',      
                                      'xe80_tc_em_L1XE70',       
                                      'xe80_mht_L1XE70',         
                                      'xe80_pueta_L1XE70',       
                                      'xe80_pufit_L1XE70',       
                                      'xe80_wEFMu_L1XE70',       
                                      'xe80_tc_lcw_wEFMu_L1XE70',
                                      'xe80_tc_em_wEFMu_L1XE70', 
                                      'xe80_mht_wEFMu_L1XE70',   
                                      'xe80_pueta_wEFMu_L1XE70', 
                                      'xe80_pufit_wEFMu_L1XE70', 
                                      ],
        
        'RATE:CPS:HLT_xe90_L1XE50' : ['xe90_L1XE50',
                                      'xe90_tc_lcw_L1XE50', 
                                      'xe90_tc_em_L1XE50', 
                                      'xe90_mht_L1XE50',   
                                      'xe90_pueta_L1XE50', 
                                      'xe90_pufit_L1XE50', 
                                      'xe90_wEFMu_L1XE50',  
                                      'xe90_tc_lcw_wEFMu_L1XE50',
                                      'xe90_tc_em_wEFMu_L1XE50', 
                                      'xe90_mht_wEFMu_L1XE50',   
                                      'xe90_pueta_wEFMu_L1XE50', 
                                      'xe90_pufit_wEFMu_L1XE50', 
                                      ],                            
         
        'RATE:CPS:HLT_xe100_L1XE50' : ['xe100_L1XE50',  
                                       'xe100_tc_lcw_L1XE50', 
                                       'xe100_tc_em_L1XE50',  
                                       'xe100_mht_L1XE50',    
                                       'xe100_pueta_L1XE50',  
                                       'xe100_pufit_L1XE50',  
                                       'xe100_wEFMu_L1XE50',  
                                       'xe100_tc_lcw_wEFMu_L1XE50',
                                       'xe100_tc_em_wEFMu_L1XE50', 
                                       'xe100_mht_wEFMu_L1XE50',   
                                       'xe100_pueta_wEFMu_L1XE50', 
                                       'xe100_pufit_wEFMu_L1XE50', 
                                       ],         
 
        'RATE:CPS:HLT_xe90' : ['xe90',
                               'xe90_tc_lcw',
                               'xe90_tc_em',
                               'xe90_mht',
                               'xe90_pueta',
                               'xe90_pufit',
                               'xe90_wEFMu',
                               'xe90_tc_lcw_wEFMu',
                               'xe90_tc_em_wEFMu',
                               'xe90_mht_wEFMu',
                               'xe90_pueta_wEFMu',
                               'xe90_pufit_wEFMu',
                               ],
        
        #Bphys	
        'RATE:CPS:HLT_2mu4_bBmumuX' : ['2mu4_bBmumuxv2',             
                                      '2mu4_bBmumux_BcmumuDsloose', 
                                      '2mu4_bBmumux_BpmumuKp',
                                      '2mu4_bBmumuxv2_noL2',
                                      '2mu4_bBmumux_BcmumuDsloose_noL2',
                                      '2mu4_bBmumux_BpmumuKp_noL2',
                                      ],     
				      
        "RATE:CPS:HLT_2mu4_bJpsimumu" : ['2mu4_bJpsimumu',
	                                 '2mu4_bJpsimumu_noL2',
                                         '2mu4_l2msonly_bJpsimumu_noL2',
                                         'mu4_mu4_l2msonly_bJpsimumu_noL2',
				        ],

        'RATE:CPS:HLT_2mu4_bDimu' : ['2mu4_bDimu',                
                                     '2mu4_bDimu_novtx_noos',   
                                     '2mu4_bDimu_noL2',
                                     '2mu4_l2msonly_bDimu_noL2',
                                     'mu4_mu4_l2msonly_bDimu_noL2', 
                                     '2mu4_bDimu_noEFbph', 
                                     ],

        "RATE:CPS:HLT_2mu6_bDimu"  :  ['2mu6_bDimu',
                                       '2mu6_bDimu_novtx_noos',
                                       '2mu6_bDimu_noL2',
                                       '2mu6_l2msonly_bDimu_noL2',
                                       'mu6_mu6_l2msonly_bDimu_noL2',
                                       ],
				       
        "RATE:CPS:HLT_2mu6_bJpsimumu" :  ['2mu6_bJpsimumu',
                                          '2mu6_bJpsimumu_noL2',
                                          '2mu6_l2msonly_bJpsimumu_noL2',
                                          'mu6_mu6_l2msonly_bJpsimumu_noL2',
					  ],

        "RATE:CPS:HLT_2mu10_bDimu"  :  ['2mu10_bDimu',
                                        '2mu10_bDimu_novtx_noos',
                                        '2mu10_bDimu_noL2',
                                        '2mu10_l2msonly_bDimu_noL2',
                                        'mu10_mu10_l2msonly_bDimu_noL2',
					],
				       
        "RATE:CPS:HLT_2mu10_bJpsimumu" :  ['2mu10_bJpsimumu',
                                           '2mu10_bJpsimumu_noL2',
                                           '2mu10_l2msonly_bJpsimumu_noL2',
                                           'mu10_mu10_l2msonly_bJpsimumu_noL2',
					   ],

        'RATE:CPS:HLT_mu6_mu4_bBmumuX' : ['mu6_mu4_bBmumuxv2',            
                                          'mu6_mu4_bBmumux_BcmumuDsloose',
                                          'mu6_mu4_bBmumux_BpmumuKp',
                                          'mu6_mu4_bBmumuxv2_noL2',
                                          'mu6_mu4_bBmumux_BcmumuDsloose_noL2',
                                          'mu6_mu4_bBmumux_BpmumuKp_noL2',
                                          ],
        'RATE:CPS:HLT_2mu6_bBmumuX'   :  ['2mu6_bBmumuxv2_noL2',
                                          '2mu6_bBmumux_BcmumuDsloose_noL2',
                                          '2mu6_bBmumux_BpmumuKp_noL2',
                                          '2mu6_bBmumuxv2',
                                          '2mu6_bBmumux_BcmumuDsloose',
                                          '2mu6_bBmumux_BpmumuKp',
                                         ],

        'RATE:CPS:HLT_mu6_mu4_bJpsimumu' : ['mu6_mu4_bJpsimumu', 
                                            'mu6_mu4_bJpsimumu_noL2',
					    'mu6_mu4_l2msonly_bJpsimumu_noL2',
                                           ],

        'RATE:CPS:HLT_mu6_mu4_bDimu' : ['mu6_mu4_bDimu',                
                                        'mu6_mu4_bDimu_novtx_noos',
					'mu6_mu4_bDimu_noL2',
                                        'mu6_mu4_l2msonly_bDimu_noL2',
                                        'mu6_mu4_bDimu_noEFbph',
                                        ],
	'RATE:CPS:HLT_2mu10_bBmumuX' : ['2mu10_bBmumuxv2_noL2',
                                        '2mu10_bBmumux_BcmumuDsloose_noL2',
				        '2mu10_bBmumux_BpmumuKp_noL2',
                                        '2mu10_bBmumuxv2',
                                        '2mu10_bBmumux_BcmumuDsloose',
                                        '2mu10_bBmumux_BpmumuKp',
                                        ],

        'RATE:CPS:HLT_mu10_mu6_bBmumuX' : ['mu10_mu6_bBmumuxv2',               
                                           'mu10_mu6_bBmumux_BcmumuDsloose',
                                           'mu10_mu6_bBmumux_BpmumuKp',
					   'mu10_mu6_bBmumuxv2_noL2',
                                           'mu10_mu6_bBmumux_BcmumuDsloose_noL2',
                                           'mu10_mu6_bBmumux_BpmumuKp_noL2',
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
        'RATE:CPS:HLT_mu_jet_L1MU4' : ['mu4_j15_dr05',
                                       'mu4_j25_dr05',
                                       'mu4_j35_dr05',
                                       'mu4_j15_bperf_split_dr05_dz02',
                                       'mu4_j25_bperf_split_dr05_dz02',
                                       'mu4_j35_bperf_split_dr05_dz02',
				       ],
				       
        'RATE:CPS:HLT_mu_jet_L1MU4_J12' : [
					'mu4_j55_dr05',
					'mu4_j55_bperf_split_dr05_dz02',
				       ],				       
				       
        'RATE:CPS:HLT_mu_jet_L1MU6_J20' : [	
					'mu6_j85_dr05',
					'mu6_j110_dr05',
					'mu6_j85_bperf_split_dr05_dz02',
					'mu6_j110_bperf_split_dr05_dz02',
				       ],
        'RATE:CPS:HLT_mu_jet_L1MU6_J40' : [
					'mu6_j150_dr05',
					'mu6_j175_dr05',
					'mu6_j150_bperf_split_dr05_dz02',
					'mu6_j175_bperf_split_dr05_dz02',	
				       ],
        'RATE:CPS:HLT_mu_jet_L1MU6_J75' : [
						'mu6_j260_dr05',
						'mu6_j320_dr05',
						'mu6_j400_dr05',
						'mu6_j260_bperf_split_dr05_dz02',
						'mu6_j320_bperf_split_dr05_dz02',
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
        'RATE:CPS:HLT_xe80_L1XE70' : ['xe80_L1XE70_BGRP7',
                                      'xe80_tc_lcw_L1XE70_BGRP7',
                                      'xe80_tc_em_L1XE70_BGRP7',  
                                      'xe80_mht_L1XE70_BGRP7',
                                      'xe80_pueta_L1XE70_BGRP7',
                                      'xe80_pufit_L1XE70_BGRP7',
                                      'xe80_wEFMu_L1XE70_BGRP7',
                                      'xe80_tc_lcw_wEFMu_L1XE70_BGRP7',
                                      'xe80_tc_em_wEFMu_L1XE70_BGRP7',
                                      'xe80_mht_wEFMu_L1XE70_BGRP7',
                                      'xe80_pueta_wEFMu_L1XE70_BGRP7',
                                      'xe80_pufit_wEFMu_L1XE70_BGRP7',
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
                                            ],}) 
    if "v6" in TriggerFlags.triggerMenuSetup():
     HLT_CPS_Groups.update({
        'RATE:CPS:HLT_e28_lhvloose_L1EM20VH_3j20noL1' : ['e28_lhvloose_L1EM20VH_3j20noL1',
                                                         'e28_lhvloose_nod0_L1EM20VH_3j20noL1',],
      
        'RATE:CPS:HLT_e28_lhvloose_L1EM20VH_4j20noL1' : ['e28_lhvloose_L1EM20VH_4j20noL1',
                                                         'e28_lhvloose_nod0_L1EM20VH_4j20noL1',],
      
        'RATE:CPS:HLT_e28_lhvloose_L1EM20VH_5j15noL1' : ['e28_lhvloose_L1EM20VH_5j15noL1',
                                                         'e28_lhvloose_nod0_L1EM20VH_5j15noL1',],
      
        'RATE:CPS:HLT_e28_lhvloose_L1EM20VH_6j15noL1' : ['e28_lhvloose_L1EM20VH_6j15noL1',
                                                         'e28_lhvloose_nod0_L1EM20VH_6j15noL1',],

        'RATE:CPS:HLT_3mu6_bDimu' : ['3mu6_bDimu',
                                     '3mu6_bJpsi',
                                     '3mu6_bTau', 
                                     '3mu6_bUpsi',
                                     ],

#        'RATE:CPS:HLT_2mu6_bBmumu' :  ['2mu6_bBmumu',
#                                       '2mu6_bBmumu_noL2',
#                                       ],
        
        'RATE:CPS:HLT_2mu4_bBmumuX_L1BPH-1M19-2MU4-BO_BPH-0DR34-2MU4' : ['2mu4_bBmumuxv2_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4',
                                                                         '2mu4_bBmumuxv2_noL2_L1BPH-1M19-2MU4-B_BPH-0DR34-2MU4',
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

        'RATE:CPS:HLT_2mu6_bBmumuX_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6' : ['2mu6_bBmumuxv2_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',             
                                                                          '2mu6_bBmumux_BcmumuDsloose_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6', 
                                                                          ],

        'RATE:CPS:HLT_2mu6_bJpsimumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6' :  ['2mu6_bJpsimumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
                                                                        '2mu6_bJpsimumu_noL2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
                                                                        ],
        
        'RATE:CPS:HLT_mu6_mu4_bJpsimumu_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4' : ['mu6_mu4_bJpsimumu_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4', 
                                                                                'mu6_mu4_bJpsimumu_noL2_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4',
                                                                                ],

        'RATE:CPS:HLT_mu6_mu4_bBmumu_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4' : ['mu6_mu4_bBmumu_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4', 
                                                                             'mu6_mu4_bBmumu_noL2_L1BPH-2M8-MU6MU4-B_BPH-0DR15-MU6MU4',
                                                                             ],
        
        })

    return (HLT_CPS_Groups)

def addCPS(triggerPythonConfig,signatureOverwritten):
    HLT_CPS_Groups = defineCPSGroups()
    hackedCPSAdder(HLT_CPS_Groups, triggerPythonConfig.allChains, triggerPythonConfig.theHLTChains, "HLT",signatureOverwritten)
