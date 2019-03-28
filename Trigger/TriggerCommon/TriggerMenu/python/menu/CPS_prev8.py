# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


from TriggerJobOpts.TriggerFlags import TriggerFlags
HLT_CPS_Groups = {}

if not "primaries" in TriggerFlags.triggerMenuSetup():
    HLT_CPS_Groups.update({
    "RATE:CPS:HLT_e5_vloose"  :  ['e5_lhvloose_nod0',],

    "RATE:CPS:HLT_2g3" : ['2g3_loose_L12EM3_VTE50','2g3_loose_dPhi15_L12EM3_VTE50','2g3_medium_dPhi15_L12EM3_VTE50'],

    "RATE:CPS:HLT_e20_vloose"        : ['e20_lhvloose_nod0'],

     "RATE:CPS:HLT_e40_vloose"        : ['e40_lhvloose_nod0'],

     "RATE:CPS:HLT_e60_vloose"        : ['e60_lhvloose_nod0'],

     "RATE:CPS:HLT_e24_tight_L1EM20VH" :  [
                                             'e24_lhtight_nod0_L1EM20VH',
                                             ],

     'RATE:CPS:HLT_e18_etcut_trkcut_xs20' :['e18_etcut_trkcut_xs20_mt35',
                                            'e18_etcut_trkcut_j20_perf_xe20_6dphi15_mt35',
                                            'e18_etcut_trkcut_xs20_j20_perf_xe20_6dphi15_mt35',],

     'RATE:CPS:HLT_e13_etcut_trkcut_xs15' :['e13_etcut_trkcut_xs15_mt25',
                                            'e13_etcut_trkcut_j20_perf_xe15_6dphi05_mt25',
                                            'e13_etcut_trkcut_xs15_j20_perf_xe15_6dphi05_mt25',
                                            'e13_etcut_trkcut_j20_perf_xe15_2dphi05_mt25',
                                            'e13_etcut_trkcut_xs15_j20_perf_xe15_2dphi05_mt25',],

     #muon chains
     'RATE:CPS:HLT_muon_idperf'         :['mu20_idperf',
                                          'mu24_idperf',
                                          'mu40_idperf',],

     #tau chains   
     'RATE:CPS:HLT_tau25_express'         :['tau25_idperf_track',
                                            'tau25_perf_tracktwo',
                                            'tau25_idperf_tracktwo',
                                            'tau25_medium1_tracktwo',
                                            'tau25_idperf_tracktwoEF',
                                            'tau25_perf_tracktwoEF',
                                            'tau25_medium1_tracktwoEF',
                                            'tau25_idperf_tracktwoEFmvaTES',
                                            'tau25_perf_tracktwoEFmvaTES',
                                            'tau25_medium1NoPt_tracktwoEFmvaTES',
                                            'tau25_medium1_tracktwoEFmvaTES',
                                            'tau25_idperf_tracktwoMVA',
                                            'tau25_perf_tracktwoMVA',
                                            'tau25_medium1_tracktwoMVA',
                                            'tau25_verylooseRNN_tracktwoMVA',
                                            'tau25_looseRNN_tracktwoMVA',
                                            'tau25_mediumRNN_tracktwoMVA',
                                            'tau25_tightRNN_tracktwoMVA',
                                            'tau25_verylooseRNN_tracktwo',
                                            'tau25_looseRNN_tracktwo',
                                            'tau25_mediumRNN_tracktwo',
                                            'tau25_tightRNN_tracktwo',
                                           ],  
     'RATE:CPS:HLT_tau160_L1TAU100'	     :['tau160_idperf_track_L1TAU100',
                                            'tau160_idperf_tracktwo_L1TAU100',
                                            'tau160_idperf_tracktwoEF_L1TAU100',
                                            'tau160_idperf_tracktwoMVA_L1TAU100',
                                            'tau160_perf_tracktwo_L1TAU100',
                                            'tau160_perf_tracktwoEF_L1TAU100',
                                            'tau160_perf_tracktwoMVA_L1TAU100',],  
     

    'RATE:CPS:HLT_j35b' : [ 'j35_boffperf_split',
                           ],
    'RATE:CPS:HLT_j55b' : [ 'j55_boffperf_split',
                              ],
    'RATE:CPS:HLT_j150b' : [ 'j150_boffperf_split',
                            ],
    'RATE:CPS:HLT_j320b' : [ 'j320_boffperf_split',
                            ],
     #Bphys	

     'RATE:CPS:HLT_2mu4_bDimu' : ['2mu4_bDimu',                
                                  '2mu4_bDimu_novtx_noos',   
                                  ],

     "RATE:CPS:HLT_2mu6_bDimu"  :  ['2mu6_bDimu',
                                    '2mu6_bDimu_novtx_noos',
                                    ],
			       
     'RATE:CPS:HLT_mu6_mu4_bBmumuX' : ['mu6_mu4_bBmumuxv2',            
                                       'mu6_mu4_bBmumux_BpmumuKp',
                                       'mu6_mu4_bJpsimumu', 
                                       ],
					  
     'RATE:CPS:HLT_mu6_mu4_bBmumu'   : ['mu6_mu4_bBmumu',],
			 
     'RATE:CPS:HLT_2mu6_bBmumuX'   :  ['2mu6_bBmumuxv2',
                                       '2mu6_bBmumux_BpmumuKp',
                                       '2mu6_bJpsimumu',
                                      ],

     'RATE:CPS:HLT_mu6_mu4_bDimu' : ['mu6_mu4_bDimu',                
                                     'mu6_mu4_bDimu_novtx_noos',
                                     ],

     'RATE:CPS:HLT_2mu10_bDimu' : ['2mu10_bJpsimumu',
                                   '2mu10_bUpsimumu',
                                   '2mu10_bBmumuxv2',
                                   '2mu10_bDimu',
                                   '2mu10_bDimu_novtx_noos',
                                   ],

     'RATE:CPS:HLT_3mu4_bDimu' : ['3mu4_bDimu',
                                  '3mu4_bJpsi',
                                  '3mu4_bTau', 
                                  '3mu4_bUpsi',
                                  ],
     ### ATR-15050

     'RATE:CPS:HLT_bDimu_L1BPH-2M9-2MU6_2DR15-2MU6' :
                          ['2mu6_bDimu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
                           '2mu6_bJpsimumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
                           '2mu6_bBmumu_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
                           '2mu6_bBmumuxv2_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
                           '2mu6_bBmumu_Lxy0_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
                           '2mu6_bBmumux_BsmumuPhi_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
                           '2mu6_bJpsimumu_Lxy0_L1BPH-2M9-2MU6_BPH-2DR15-2MU6',
                          ],


     # mu+jets chains 
     'RATE:CPS:HLT_mu_jet_L1MU4' : ['mu4_j15_boffperf_split_dr05_dz02',
                                    'mu4_j25_boffperf_split_dr05_dz02',
                                    'mu4_j35_boffperf_split_dr05_dz02',
			       'mu4_j35_boffperf_split',
			       ],
			       
			       
     'RATE:CPS:HLT_mu_jet_L1MU6_J40' : [
				'mu6_j150_boffperf_split_dr05_dz02',
				'mu6_j150_boffperf_split',
				'mu6_j175_boffperf_split_dr05_dz02',	
			       ],
     'RATE:CPS:HLT_mu_jet_L1MU6_J75' : [
					'mu6_j175_gsc260_boffperf_split_dr05_dz02',
					'mu6_j225_gsc320_boffperf_split_dr05_dz02',
					'mu6_j320_boffperf_split',
					'mu6_j400_boffperf_split_dr05_dz02',     
			       ],			    

            })

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
        'RATE:CPS:HLT_mu_jet_L1MU4' : ['mu4_j15_boffperf_split_dr05_dz02',
                                       'mu4_j25_boffperf_split_dr05_dz02',
                                       'mu4_j35_boffperf_split_dr05_dz02',
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

    })

if ( "v7" in TriggerFlags.triggerMenuSetup()  or "v8" in TriggerFlags.triggerMenuSetup() ) and not "primaries" in TriggerFlags.triggerMenuSetup():
    HLT_CPS_Groups.update({
            'RATE:CPS:HLT_e24_medium_L1EM18VHI' : ['e24_lhmedium_L1EM18VHI',
                                                   'e24_lhmedium_nod0_L1EM18VHI',
                                                   ],
             'RATE:CPS:HLT_mu_jet_L1MU6_J75' : ['mu6_j175_gsc260_boffperf_split_dr05_dz02',
                                                'mu6_j225_gsc320_boffperf_split_dr05_dz02',
                                                'mu6_j225_gsc320_boffperf_split_dr05_dz99',
                                                'mu6_j225_gsc320_boffperf_split',
                                                'mu6_j225_gsc400_boffperf_split_dr05_dz02',     
                                                ],
             'RATE:CPS:HLT_mu_jet_L1MU6_J40' : ['mu6_j110_gsc150_boffperf_split_dr05_dz02',
                                                'mu6_j110_gsc150_boffperf_split_dr05_dz99',
                                                'mu6_j110_gsc150_boffperf_split',
                                                'mu6_j150_gsc175_boffperf_split_dr05_dz02',	
                                                ],
             'RATE:CPS:HLT_mu_jet_L1MU4' : ['mu4_j15_boffperf_split_dr05_dz02',
                                            'mu4_j25_boffperf_split_dr05_dz02',
                                            ],

             'RATE:CPS:HLT_mu_jet_L1MU6_J20' : ['mu6_j60_gsc85_boffperf_split_dr05_dz02',
                                                'mu6_j85_gsc110_boffperf_split_dr05_dz02',
                                                ],

             'RATE:CPS:HLT_mu4_bJpsi_Trk' : ['mu4_bJpsi_TrkPEB','mu4_bJpsi_Trkloose','mu4_mu2noL1_L1MU4'],
             'RATE:CPS:HLT_mu6_bJpsi_Trk' : ['mu6_bJpsi_TrkPEB','mu6_bJpsi_lowpt_TrkPEB', 'mu6_bJpsi_Trkloose','mu6_mu2noL1_L1MU6'],
             'RATE:CPS:HLT_mu10_bJpsi_Trk' : ['mu10_bJpsi_TrkPEB','mu10_bJpsi_Trkloose', 'mu10_bJpsi_TrkPEBmon'],
             'RATE:CPS:HLT_mu14_bJpsi_Trk' : ['mu14_bJpsi_TrkPEB','mu14_bJpsi_Trkloose'],
             'RATE:CPS:HLT_2e5_bBeexM600t' : ['e5_lhvloose_nod0_bBeexM6000t','2e5_lhvloose_nod0_bBeexM6000t'],
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
   ],
        
   'RATE:CPS:HLT_j260_a10_sub_L1J75':[ 'j260_a10_sub_L1J75',
                                       'j260_a10_lcw_sub_L1J75',
                                       'j260_a10_lcw_nojcalib_L1J75',
                                       'j260_a10_lcw_L1J75',
                                       'j260_a10_nojcalib_L1J75',
                                       'j260_a10r_L1J75',
   ],
        
    'RATE:CPS:HLT_j360_a10L1J100' : [ 'j360_a10r_L1J100' ,            
                                      'j360_a10_lcw_L1J100',],
    'RATE:CPS:HLT_j380_a10L1J100' : [ 'j380_a10r_L1J100' ,            
                                      'j380_a10_lcw_L1J100',],
    'RATE:CPS:HLT_j400_a10' : ['j400_a10_lcw_L1J100',
                               'j400_a10r_L1J100',
    ],
    'RATE:CPS:HLT_j420_a10' : ['j420_a10_lcw_L1J100',
                               'j420_a10r_L1J100',
    ],
    
    'RATE:CPS:HLT_j440_a10' : ['j440_a10_lcw_L1J100',
                               'j440_a10r_L1J100',
    ],
    'RATE:CPS:HLT_j460_a10L1J100' : [	'j460_a10_sub_L1J100',
    																	'j460_a10_nojcalib_L1J100',
    																	'j460_a10_lcw_nojcalib_L1J100',
    																	'j460_a10_lcw_sub_L1J100',
    																	'j460_a10_lcw_L1J100',],
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
                                   'xe100_wEFMu_L1XE50',  
                                   'xe100_tc_lcw_wEFMu_L1XE50',
                                   'xe100_tc_em_wEFMu_L1XE50', 
                                   'xe100_mht_wEFMu_L1XE50',   
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
if ( "v6" in TriggerFlags.triggerMenuSetup() or "v7" in TriggerFlags.triggerMenuSetup() or "v8" in TriggerFlags.triggerMenuSetup() ) and not "primaries" in TriggerFlags.triggerMenuSetup():
    HLT_CPS_Groups.update({


      "RATE:CPS:HLT_mu6_mu4_bBmumu"   : ['mu6_mu4_bBmumu',
                                    'mu6_mu4_bBmumu_Lxy0',],
 "RATE:CPS:HLT_mu6_2mu4"         : ['mu6_2mu4_bJpsi',
                                         'mu6_2mu4_bUpsi',
                                         'mu6_2mu4',
                                         ],

     'RATE:CPS:HLT_2mu6_bBmumuX'   :  ['2mu6_bBmumuxv2',
                                       '2mu6_bBmumux_BpmumuKp',
                                       '2mu6_bJpsimumu',
                                       '2mu6_bJpsimumu_Lxy0',
                                      ],
     'RATE:CPS:HLT_mu6_mu4_bBmumuX' : ['mu6_mu4_bBmumuxv2',            
                                       'mu6_mu4_bBmumux_BpmumuKp',
                                       'mu6_mu4_bJpsimumu', 
                                       'mu6_mu4_bJpsimumu_Lxy0',
                                       ],

     'RATE:CPS:HLT_e13_etcut_trkcut_xs30' :['e13_etcut_trkcut_xs30_xe30_mt35',
                                            'e13_etcut_trkcut_xs30_j15_perf_xe30_6dphi05_mt35',
                                            'e13_etcut_trkcut_xs30_j15_perf_xe30_6dphi15_mt35',
                                            'e13_etcut_trkcut_xs30_j15_perf_xe30_2dphi05_mt35',
                                            'e13_etcut_trkcut_xs30_j15_perf_xe30_2dphi15_mt35',],

      'RATE:CPS:HLT_e18_etcut_trkcut_xs30' :['e18_etcut_trkcut_xs30_xe30_mt35',
                                             'e18_etcut_trkcut_xs30_j15_perf_xe30_6dphi05_mt35',
                                             'e18_etcut_trkcut_xs30_j15_perf_xe30_6dphi15_mt35',
                                             'e18_etcut_trkcut_xs30_j15_perf_xe30_2dphi05_mt35',
                                             'e18_etcut_trkcut_xs30_j15_perf_xe30_2dphi15_mt35',],
     
     'RATE:CPS:HLT_3mu6_bDimu' : ['3mu6_bDimu',
                                  '3mu6_bJpsi',
                                  '3mu6_bTau', 
                                  '3mu6_bUpsi',
                                  ],

    'RATE:CPS:HLT_2mu6_bBmumu' :  ['2mu6_bBmumu',
                                   '2mu6_bBmumu_Lxy0',
                                    ],


      })

if ( ("v7" in TriggerFlags.triggerMenuSetup() or "v8" in TriggerFlags.triggerMenuSetup()) and not "primaries" in TriggerFlags.triggerMenuSetup()):
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

if ( "v7" in TriggerFlags.triggerMenuSetup()  or "v8" in TriggerFlags.triggerMenuSetup() ) and not "primaries" in TriggerFlags.triggerMenuSetup():
    HLT_CPS_Groups.update({
     'RATE:CPS:HLT_j35' :    [   'j35',
                                 'j35_subjes',
                                 'j35_jes',
                                 'j35_sub',
                                 'j35_nojcalib',
                                 'j35_lcw',
                                 'j35_lcw_subjes',
                                 'j35_lcw_jes',
                                 'j35_lcw_sub',
                                 'j35_lcw_nojcalib',
                             ],
     
     'RATE:CPS:HLT_j85' :    [   'j85',
                                 'j85_cleanT',
                                 'j85_cleanL',
                                 'j85_cleanLLP',
                             ],

     'RATE:CPS:HLT_j450_sub' :   [    'j450_subjes',
                                 'j450_jes',
                                 'j450_sub',
                                 'j450_nojcalib',
                                 'j450_lcw',
                                 'j450_lcw_subjes',
                                 'j450_lcw_jes',
                                 'j450_lcw_sub',
                                 'j450_lcw_nojcalib',
                                      ],

     'RATE:CPS:HLT_j35_320eta490' :  [   'j35_320eta490',
                                         'j35_320eta490_subjes',
                                         'j35_320eta490_jes',
                                         'j35_320eta490_sub',
                                         'j35_320eta490_nojcalib',
                                         'j35_320eta490_lcw',
                                         'j35_320eta490_lcw_subjes',
                                         'j35_320eta490_lcw_jes',
                                         'j35_320eta490_lcw_sub',
                                         'j35_320eta490_lcw_nojcalib',
                                     ],

     'RATE:CPS:HLT_j260_320eta490_subjes' : [        'j260_320eta490_subjes',
                                         'j260_320eta490_jes',

                                         'j260_320eta490_lcw',
                                         'j260_320eta490_lcw_subjes',
                                         'j260_320eta490_lcw_jes',
                                         'j260_320eta490_lcw_sub',
                                         'j260_320eta490_lcw_nojcalib',
                                                     ],

     'RATE:CPS:HLT_j260_a10_L1J75' : [   'j260_a10_sub_L1J75',
                                         'j260_a10_nojcalib_L1J75',
                                         'j260_a10_lcw_subjes_L1J75',
                                         'j260_a10_lcw_sub_L1J75',
                                         'j260_a10_lcw_nojcalib_L1J75',
                                         'j260_a10t_lcw_jes_L1J75',
                                         'j260_a10t_lcw_nojcalib_L1J75',
                                         'j260_a10r_L1J75',
                                     ],


     'RATE:CPS:HLT_j460_a10L1SC111' : [	'j460_a10_lcw_subjes_L1SC111',
                                             'j460_a10r_L1SC111',
                                             'j460_a10t_lcw_jes_L1SC111',],

     'RATE:CPS:HLT_j480_a10_sub_L1J100': [   'j480_a10_sub_L1J100',
                                         'j480_a10_nojcalib_L1J100',
                                         'j480_a10_lcw_subjes_L1J100',
                                         'j480_a10_lcw_sub_L1J100',
                                         'j480_a10_lcw_nojcalib_L1J100',
                                         'j480_a10t_lcw_nojcalib_L1J100',
                                         ],

     'RATE:CPS:HLT_j375_a10t_SMC_L1J100' : [ 'j375_a10t_lcw_jes_30smcINF_L1J100',
                                             'j375_a10t_lcw_jes_L1J100',
                                         ],

     'RATE:CPS:HLT_j375_a10t_SMC_L1SC111': [ 'j375_a10t_lcw_jes_30smcINF_L1SC111',
                                             'j375_a10t_lcw_jes_L1SC111',
                                         ],

     'RATE:CPS:HLT_j375_a10t_SMC_L1HT' : [ 'j375_a10t_lcw_jes_30smcINF_L1HT190-J15.ETA21',
                                             'j375_a10t_lcw_jes_L1HT190-J15.ETA21',
                                         ],

     'RATE:CPS:HLT_j390_a10t_SMC_L1J100' : [ 'j390_a10t_lcw_jes_30smcINF_L1J100',
                                             'j390_a10t_lcw_jes_L1J100',
                                         ],

     'RATE:CPS:HLT_j390_a10t_SMC_L1SC111': [ 'j390_a10t_lcw_jes_30smcINF_L1SC111',
                                             'j390_a10t_lcw_jes_L1SC111',
                                         ],

     'RATE:CPS:HLT_j390_a10t_SMC_L1HT' : [ 'j390_a10t_lcw_jes_30smcINF_L1HT190-J15.ETA21',
                                           'j390_a10t_lcw_jes_L1HT190-J15.ETA21',
                                         ],

     'RATE:CPS:HLT_j420_a10t_SMC_L1J100' : [ 'j420_a10t_lcw_jes_30smcINF_L1J100',
                                             'j420_a10t_lcw_jes_L1J100',
                                         ],

     'RATE:CPS:HLT_j420_a10t_SMC_L1SC111': [ 'j420_a10t_lcw_jes_30smcINF_L1SC111',
                                             'j420_a10t_lcw_jes_L1SC111',
                                         ],

     'RATE:CPS:HLT_j420_a10t_SMC_L1HT' : [ 'j420_a10t_lcw_jes_30smcINF_L1HT190-J15.ETA21',
                                           'j420_a10t_lcw_jes_L1HT190-J15.ETA21',
                                         ],

     'RATE:CPS:HLT_j440_a10t_SMC_L1J100' : [ 'j440_a10t_lcw_jes_30smcINF_L1J100',
                                             'j440_a10t_lcw_jes_L1J100',
                                         ],

     'RATE:CPS:HLT_j440_a10t_SMC_L1SC111': [ 'j440_a10t_lcw_jes_30smcINF_L1SC111',
                                             'j440_a10t_lcw_jes_L1SC111',
                                         ],

     'RATE:CPS:HLT_j440_a10t_SMC_L1HT' : [ 'j440_a10t_lcw_jes_30smcINF_L1HT190-J15.ETA21',
                                           'j440_a10t_lcw_jes_L1HT190-J15.ETA21',
                                         ],

     'RATE:CPS:HLT_2j330_a10t_SMC_L1J100': [ '2j330_a10t_lcw_jes_30smcINF_L1J100',
                                             '2j330_a10t_lcw_jes_L1J100',
                                         ],

     'RATE:CPS:HLT_2j330_a10t_SMC_L1SC111':[ '2j330_a10t_lcw_jes_30smcINF_L1SC111',
                                             '2j330_a10t_lcw_jes_L1SC111',
                                         ],

     'RATE:CPS:HLT_2j330_a10t_SMC_L1HT'  : [ '2j330_a10t_lcw_jes_30smcINF_L1HT190-J15.ETA21',
                                             '2j330_a10t_lcw_jes_L1HT190-J15.ETA21',
                                         ],

     'RATE:CPS:HLT_2j340_a10t_SMC_L1J100': [ '2j340_a10t_lcw_jes_30smcINF_L1J100',
                                             '2j340_a10t_lcw_jes_L1J100',
                                         ],

     'RATE:CPS:HLT_2j340_a10t_SMC_L1SC111':[ '2j340_a10t_lcw_jes_30smcINF_L1SC111',
                                             '2j340_a10t_lcw_jes_L1SC111',
                                         ],

     'RATE:CPS:HLT_2j340_a10t_SMC_L1HT'  : [ '2j340_a10t_lcw_jes_30smcINF_L1HT190-J15.ETA21',
                                             '2j340_a10t_lcw_jes_L1HT190-J15.ETA21',
                                         ],

     'RATE:CPS:HLT_2j350_a10t_SMC_L1J100': [ '2j350_a10t_lcw_jes_30smcINF_L1J100',
                                             '2j350_a10t_lcw_jes_L1J100',
                                         ],

     'RATE:CPS:HLT_2j350_a10t_SMC_L1SC111':[ '2j350_a10t_lcw_jes_30smcINF_L1SC111',
                                             '2j350_a10t_lcw_jes_L1SC111',
                                         ],

     'RATE:CPS:HLT_2j350_a10t_SMC_L1HT'  : [ '2j350_a10t_lcw_jes_30smcINF_L1HT190-J15.ETA21',
                                             '2j350_a10t_lcw_jes_L1HT190-J15.ETA21',
                                         ],

     # Multi-jet chains
     'RATE:CPS:HLT_4j45' :   [   '4j45',
                                 '4j45_subjes',
                                 '4j45_jes',
                                 '4j45_sub',
                                 '4j45_nojcalib',
                                 '4j45_lcw',
                                 '4j45_lcw_subjes',
                                 '4j45_lcw_jes',
                                 '4j45_lcw_sub',
                                 '4j45_lcw_nojcalib',
                             ],
     
     'RATE:CPS:HLT_6j45' :   [   '6j45',
                                 '6j45_subjes',
                                 '6j45_jes',
                                 '6j45_sub',
                                 '6j45_nojcalib',
                                 '6j45_lcw',
                                 '6j45_lcw_subjes',
                                 '6j45_lcw_jes',
                                 '6j45_lcw_sub',
                                 '6j45_lcw_nojcalib',
                             ],

     # End of jet chains


     "RATE:CPS:HLT_e10_vloose_L1EM7" : ['e10_lhvloose_nod0_L1EM7',
                                        'e15_lhvloose_nod0_L1EM7',
                                        'e15_lhvloose_nod0_L1EM7',
                                        ],
     "RATE:CPS:HLT_e20_vloose_L1EM12" : ['e20_lhvloose_nod0_L1EM12',],

     "RATE:CPS:HLT_e25_vloose_L1EM15" : ['e25_lhvloose_nod0_L1EM15'],

     "RATE:CPS:HLT_e30_vloose_L1EM15" : ['e30_lhvloose_nod0_L1EM15'],
                                         
     "RATE:CPS:HLT_e40_vloose_L1EM15" : ['e40_lhvloose_nod0_L1EM15'],

     "RATE:CPS:HLT_e50_vloose_L1EM15" : ['e50_lhvloose_nod0_L1EM15'],
     
     "RATE:CPS:HLT_e70_vloose"        : ['e70_lhvloose_nod0'],

     "RATE:CPS:HLT_e80_vloose"        : ['e80_lhvloose_nod0',],
     
     "RATE:CPS:HLT_e100_vloose"        : ['e100_lhvloose_nod0'],
     
     "RATE:CPS:HLT_e120_vloose"        : ['e120_lhvloose_nod0'],
         
     "RATE:CPS:HLT_e140_vloose"        : ['e140_lhvloose_nod0'],        

     "RATE:CPS:HLT_g60_loose" : ['g60_loose',],

     "RATE:CPS:HLT_g60_loose_L1EM24VHIM" : ['g60_loose_L1EM24VHIM',],

     "RATE:CPS:HLT_jX_L1AFP_A_OR_C_J12" : ['j20_L1AFP_A_OR_C_J12',
                                           'j45_L1AFP_A_OR_C_J12'],
     "RATE:CPS:HLT_jX_L1AFP_A_AND_C_J12" : ['j20_L1AFP_A_AND_C_J12',
                                           'j45_L1AFP_A_AND_C_J12'],
     "RATE:CPS:HLT_jX_L1J12" : ['j20_L1J12',
                                'j45_L1J12'],
 
    })



