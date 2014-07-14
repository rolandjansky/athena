# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file  GenericD3PDMaker/python/GenericTriggerBitsD3PDObject.py
# shamelessly copied by Gustaaf Brooijmans from SUSY:
# @author  Bjorn H. Samset <b.h.samset@fys.uio.no>
# @date    Jan, 2010
# @brief   Define trigger bit blocks for Generic.
#



import D3PDMakerCoreComps
import TriggerD3PDMaker
from TriggerD3PDMaker.defineTriggerBits   import defineTriggerBits
from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags       import D3PDMakerFlags
#from SUSYD3PDMaker.SUSYD3PDFlags                     import SUSYD3PDFlags

# Create the object type.
GenericTriggerBitsD3PDObject = make_Void_D3PDObject( "", "GenericTriggerBitsD3PDObject",
                                                  "GenericTriggerBitsFiller" )

######
# Define blocks.

if D3PDMakerFlags.DoTrigger():
  
    #-- for R&D NTUP_Generic{SKIM}, put level of detail at 10
        defineTriggerBits (GenericTriggerBitsD3PDObject, 10,
                       ['EF_.*'])

        ##Tau bits
        defineTriggerBits (GenericTriggerBitsD3PDObject, 10,
                       ['L1_Tau*',
                        'EF_Tau*',
			'EF_mu4T*',
			'EF_2mu4T*',
			'EF_mu4Tmu6',
                        ])

        ##for TAU group
        defineTriggerBits (GenericTriggerBitsD3PDObject, 10,
			   ['L1_J.*',
			    'L1_2J.*',
			    'L1_3J.*',
			    'L1_4J.*',
			    'EF_g.*',
			    'EF_2mu10.*',
			    'EF_xs.*'
			    ])
			  
        

        ##jet MET bits
        defineTriggerBits (GenericTriggerBitsD3PDObject, 10,
                       [#Level1
                        'L1_J10',
			'L1_J10_win6',
                        'L1_J15',
                        'L1_J20',
                        'L1_J30',
                        'L1_J50',
                        'L1_J75',
                        'L1_J175',
                        'L1_J250',
                        'L1_2J10_J50',
                        'L1_2J10_J75',
                        'L1_3J10',
                        'L1_3J15',
			'L1_3J20',
                        'L1_3J50',
                        'L1_3J10_J50',
                        'L1_4J10',
                        'L1_4J15',
                        'L1_XE20',
                        'L1_XE25',
                        'L1_XE30',
                        'L1_XE35',
                        'L1_XE40',
                        'L1_XE50',
                        'L1_XE60',
    
                        'L1_XS15',
                        'L1_XS20',
                        'L1_XS25',
                        'L1_XS30',
                        'L1_XS35',
                        'L1_XS40',
                        'L1_XS45',
                        'L1_XS50',
                        
                        'L1_JE100',
                        'L1_JE140',
                        'L1_JE200',

                        'L1_2J5',
			'L1_2J10',
			'L1_2J10_win6',
                        'L1_2J10_J30_JE140',
			'L1_2J15',
			'L1_J15_MV',
			'L1_J15_LV',
			'L1_J15_NL',
			
                        'L1_J50_XE20',
                        'L1_J50_XE25',

#			'L1_MBTS_A',
#                        'L1_MBTS_C',
#	                'L1_MBTS_1',
#	                'L1_MBTS_2',
#	                'L1_MBTS_1_1',
#	                'L1_MBTSA0',
#	                'L1_MBTSA1',
#	                'L1_MBTSA2',
#	                'L1_MBTSA3',
#	                'L1_MBTSA4',
#	                'L1_MBTSA5',
#	                'L1_MBTSA6',
#	                'L1_MBTSA7',
#	                'L1_MBTSA8',
#	                'L1_MBTSA9',
#	                'L1_MBTSA10',
#	                'L1_MBTSA11',
#	                'L1_MBTSA12',
#	                'L1_MBTSA13',
#		        'L1_MBTSA14',
#		        'L1_MBTSA15',
#			'L1_MBTSC0',
#                        'L1_MBTSC0',
#                        'L1_MBTSC1',
#                        'L1_MBTSC2',
#                        'L1_MBTSC3',
#                        'L1_MBTSC4',
#	                'L1_MBTSC5',
#	                'L1_MBTSC6',
#	                'L1_MBTSC7',
#	                'L1_MBTSC8',
#                        'L1_MBTSC9',
#                        'L1_MBTSC10',
#                        'L1_MBTSC11',
#                        'L1_MBTSC12',
#                        'L1_MBTSC13',
#                        'L1_MBTSC14',
#                        'L1_MBTSC15',
			'L1_TE150',
			'L1_TE360',
			'L1_JE120',
			'L1_JE280',

                        #Level 2
                        'L2_j25',
                        'L2_j35',
                        'L2_j50',
                        'L2_j70',
                        'L2_j95',

                        'L2_2b10_medium_L1JE100',
			'L2_2b10_medium_L1JE140',
			'L2_2b15_medium_3L1J15',
			'L2_2b20_medium_3L1J20',
			'L2_2b10_medium_4L1J10',
			'L2_2b10_medium_L1_2J10J50',
			'L2_3b10_loose_4L1J10',
			'L2_3b15_loose_4L1J15',

                        'L2_2j25_j70',
                        'L2_2j25_j95',

                        'L2_3j25',
                        'L2_3j25_j70',
                        'L2_3j70',
                        'L2_4j25',
                        'L2_4j25',
                        'L2_5j25',
                        'L2_6j25',

                        'L2_xe10_noMu',
                        
                        'L2_xe30_noMu',
                        'L2_xe35_noMu',
                        'L2_xe40_noMu',
                        'L2_xe50_noMu',
                        'L2_xe60_noMu',
                        'L2_xe70_noMu',


                        'L2_je255',



                        'L2_2j25_j70_anymct100',

                          
                        #Event Filter
			'EF_j10v3',
			'EF_j20v2',
			'EF_j40',
			'EF_j80v2',
			'EF_2j10',
			'EF_2j20',
			'EF_2j40',
			'EF_2j10_deta3_5',
			'EF_2j10_deta5',
			'EF_2j20_deta3_5',
			'EF_2j40_deta3_5',
			
                        'EF_j10_a4_EFFS',
                        'EF_j15_a4_EFFS',
                        'EF_j20_a4_EFFS',
                        'EF_j30_a4_EFFS',
                        'EF_j40_a4_EFFS',
                        'EF_j55_a4_EFFS',
                        'EF_j75_a4_EFFS',
                        'EF_j100_a4_EFFS',
                        'EF_j135_a4_EFFS',
                        'EF_j180_a4_EFFS',
                        'EF_j240_a4_EFFS',

                        'EF_L1J175_NoAlg',
                        'EF_L1J250_NoAlg',

                        'EF_3j30_a4_EFFS',
                        'EF_3j75_a4_EFFS',
                        'EF_4j30_a4_EFFS',
                        'EF_5j30_a4_EFFS',
                        'EF_6j30_a4_EFFS',

                        'EF_xe*',
			'EF_xs*',
                        #'EF_xe20_noMu',
                        #'EF_xe25_noMu',
                        #'EF_xe30_noMu',
                        #'EF_xe40_noMu',
                        #'EF_xe50_noMu',
                        #'EF_xe60_noMu',
                        #'EF_xe70_noMu',
                        #'EF_xe80_noMu',
                        #'EF_xe90_noMu',
                        
                        #'EF_jeXXX',
    
                        'EF_j120_j55_j40_a4_EFFS',
                        
                        'EF_j75_a4_EFFS_jetNoEF_EFxe20_noMu',
                        'EF_j75_a4_EFFS_jetNoEF_EFxe25_noMu',
                        'EF_j75_a4_EFFS_xe40_loose_noMu',
                        'EF_j75_a4_EFFS_xe45_loose_noMu',
                        'EF_j75_a4_EFFS_xe50_loose_noMu',
                        'EF_j75_a4_EFFS_xe55_loose_noMu',
                        'EF_j75_a4_EFFS_xe60_loose_noMu',

                        'EF_j75_2j30_a4_EFFS_HT350',
                        'EF_j75_2j30_a4_EFFS_HT400',
                        'EF_j100_a4_EFFS_HT350',
                        'EF_j100_a4_EFFS_HT400',

                        'EF_j75_j30_a4_EFFS_L2anymct150',
                        'EF_j75_j30_a4_EFFS_L2anymct175',
                        'EF_j75_j30_a4_EFFS_anymct150',
                        'EF_j75_j30_a4_EFFS_anymct175',
                        'EF_j75_j30_a4_EFFS_leadingmct150',

                        'EF_j135_j30_a4_EFFS_dphi04',
                        'EF_j180_j30_a4_EFFS_dphi04',

                        'EF_SeededStreamerL1CaloXS', 

                        # 2010 Items
                        'L1_J10',
                        'L1_J15',
                        'L1_J30',
                        'L1_J55',
                        'L1_J75',
                        'L1_2J10',
                        'L1_2J15',
                        'L1_3J10',
                        'L1_4J10',
                        
                        'L1_J5',
                        'L1_J95',
                        'L1_J115',
                        'L1_2J10_J30',
                        'L1_2J10_J55',
                        'L1_2J15_J75',

                        'L2_2j25_j45_dphi03',
                        'L2_2j25_j70_dphi03',
                        'L2_2j30_j90_dphi03',
                        'L2_2j25_j45_anymct100',
                        'L2_2j25_j45_anymct150',
                        'L2_2j25_j45_anymct175',
                        'L2_2j30_anymct100',
                        'L2_2j30_anymct150',
                        'L2_2j30_anymct175',
                        'L2_2j25_j70_anymct150',
                        'L2_2j25_j70_anymct175',
                        'L2_2j25_j45_leadingmct100',
                        'L2_2j25_j45_leadingmct150',
                        'L2_2j25_j45_leadingmct175',
                        'L2_2j30_leadingmct100',
                        'L2_2j30_leadingmct175',
                        'L2_2j30_leadingmct150',
                        'L2_2j25_j70_leadingmct150',
                        'L2_2j25_j70_leadingmct175',

                        'EF_j75_jetNoEF_EFxe20_noMu',
                        'EF_j75_jetNoEF_EFxe25_noMu',
                        'EF_2j35_jetNoEF',
                        'EF_j35_jetNoEF',
                        'EF_j50_jetNoEF',
                        'EF_j75_jetNoEF',
                        'EF_j95_jetNoEF',
                        'EF_j115_jetNoEF',
                        'EF_j135_jetNoEF',
                        'EF_3j20',

                        'EF_j30_jetNoCut',
                        'EF_j35_jetNoCut',
                        'EF_j95_jetNoCut',
                        'EF_2j30_jetNoCut',
                        'EF_2j50_jetNoCut',
                        'EF_2j75_jetNoCut',
                        'EF_3j30_jetNoCut',
                        'EF_3j35_jetNoCut',
                        'EF_3j50_jetNoCut',
                        'EF_3j75_jetNoCut',
                        'EF_4j30_jetNoCut',
                        'EF_4j35_jetNoCut',
                        'EF_4j50_jetNoCut',
                        'EF_5j30_jetNoCut',
                        
                        'EF_2j30_jetNoEF',
                        'EF_2j50_jetNoEF',
                        'EF_2j75_jetNoEF',
                        'EF_3j30_jetNoEF',
                        'EF_3j35_jetNoEF',
                        'EF_3j50_jetNoEF',
                        'EF_3j75_jetNoEF',
                        'EF_4j30_jetNoEF',
                        'EF_4j35_jetNoEF',
                        'EF_4j50_jetNoEF',
                        'EF_5j30_jetNoEF',
                        
                        'EF_xe40',
                        'EF_xe25',
                        'EF_xe50',
                        'EF_xe80',
                        
                        ])


        ##bjet bits
        defineTriggerBits (GenericTriggerBitsD3PDObject, 10,
                       [# Level 2
        
                         #Event Filter
		        'EF_b*',
			'EF_2b*',
			'EF_3b*',
                        #'EF_b10_tight_4L1J10',
                        #'EF_b10_tight_L1JE140',
                        #'EF_b10_loose_EFxe25_noMu_L1JE140',
                        #'EF_2b10_medium_4L1J10',
                        #'EF_2b10_medium_3L1J10',
                        #'EF_2b10_medium_L1JE140',
                        #'EF_2b20_medium_3L1J20',
                        #'EF_2b15_medium_3L1J15',
                        #'EF_3b10_loose_4L1J10',
                        #'EF_3b15_loose_4L1J15',

                        ])



        ##egamma bits
        defineTriggerBits (GenericTriggerBitsD3PDObject, 10,
                       [#Level 1
                        'L1_EM10',
                        'L1_EM12',
                        'L1_EM14',
                        'L1_EM16',
                        'L1_EM30',
                        
                        'L1_2EM3',
                        'L1_2EM5',
                        'L1_2EM7',
                        'L1_2EM10',
                        'L1_2EM14',
                        'L1_3EM5',
                        'L1_2EM3_EM7',
    
                        # Level 2
                        'L2_e10_medium',
                        'L2_e15_medium', 
                        'L2_e15_tight',
                        'L2_e20_loose', 
                        'L2_e20_medium', 
                        
                        'L2_2e10_medium', 

                        'L2_g10_loose', 
                        'L2_g11_etcut',
                        'L2_g15_loose', 
                        'L2_g20_etcut',
                        'L2_g20_loose', 
                        'L2_g40_loose',
                        'L2_g60_loose', 
                        'L2_g80_loose', 
                        'L2_g150_etcut',
                        
                        'L2_2g15_loose', 
                        'L2_2g20_loose', 
                                                
                        'L2_2e12_medium', 
                        'L2_3e10_medium', 
                        'L2_e60_loose',   
                        'L2_e22_medium',  
                        'L2_e20_medium1',
			'L2_mu*',
			'L2_2mu*',



                        #Event Filter
			'EF_e*',
			'EF_2e*',
			'EF_g*',
			'EF_2g*',
			'EF_2e7T_medium1',
			'EF_e12Tvh_loose1',
			'EF_e12Tvh_medium1',
			'EF_e18vh_medium1',
			'EF_e7T_loose1',
			'EF_2e7T_medium1_L2StarB_mu6',
			'EF_e12Tvh_loose1_vbf_L1TAU11I_EM10VH_FJ15',
			'EF_e12Tvh_loose1_vbf_L1TAU11I_EM10VH_J15CF',
                        #'EF_e10_loose',
                        #'EF_e10_medium',
                        #'EF_e15_medium',
                        #'EF_e15_tight',
                        #'EF_e20_loose',
                        #'EF_e20_medium', 
                        #'EF_e20_medium1',

                        #'EF_2e10_medium',
                        #'EF_2e12_medium',
                        #'EF_3e10_medium',

                        #'EF_g10_loose',
                        #'EF_g11_etcut',
                        #'EF_g15_loose',
                        #'EF_g20_etcut',
                        #'EF_g20_loose',
                        #'EF_g40_loose',
                        #'EF_g40_loose_EFxe40_noMu',
                        #'EF_g40_tight',
                        #'EF_g60_loose',
                        #'EF_g80_loose',
                        #'EF_g150_etcut',

                        #'EF_2g15_loose', 
                        #'EF_2g20_loose',
                        #'EF_g100_etcut_g50_etcut',
                                              
                        

                        # 2010 Items
                        'EF_g17_etcut',
                        'EF_2e5_medium',
                        'L1_EM13I',
                        'L1_EM18',
                        'L1_EM18I',
                        'L1_EM23I',
                        'L1_EM100',
                        

                        
                        ])

        ##muon bits
        defineTriggerBits (GenericTriggerBitsD3PDObject, 10,
                       [#Level 1
                        'L1_MU0',
                        'L1_MU6',
                        'L1_MU10',
                        'L1_MU11',
                        'L1_MU15',
                        'L1_MU20',
                       
                        'L1_2MU0',
                        'L1_2MU4',
                        'L1_2MU6',
                        'L1_2MU10',
                        
                        #Level 2
                        'L2_mu10',
                        'L2_mu15',
                        'L2_mu20',
                        'L2_mu20_MSonly',
                        'L2_mu20_MSOnlyBarrel',
                        'L2_mu20_slow',
                        'L2_2mu4',
                        'L2_2mu10',
			#'L2_2mu13',
			'L2_2mu*',

                        'L2_mu13_MG',
                        'L2_mu13_MG',
                        'L2_mu22_MG',
                        'L2_mu20_MG',
                        'L2_mu18_MG',
                        

                        #Event Filter
			'EF_mu*',
			'EF_2mu*',


			
                        'EF_mu4',
                        'EF_mu6',
                        'EF_mu10',
                        'EF_mu10_MSonly'             
                        'EF_mu13',
                        'EF_mu13_MG',                
                        'EF_mu13_tight',
                        'EF_mu15',
                        'EF_mu15i',
                        'EF_mu15_MG',
                        'EF_mu18',
                        'EF_mu18_MG',
                        'EF_mu20',
                        'EF_mu20i',
                        'EF_mu20_MG',
                        'EF_mu20_MSonly'
                        'EF_mu20_slow',
                        'EF_mu20i',                  
                        'EF_mu22',
                        'EF_mu22_MG',
                        'EF_mu40',                    
                        'EF_mu40_MSonly',            
                        'EF_mu40_MSonly_barrel',     
                        'EF_mu40_MSonly_tight',      
                        'EF_mu40_MSonly_tighter',    
                        
                        'EF_2mu4',
                        'EF_2mu6',
                        'EF_2mu10',
                        'EF_2mu10_loose',
                        'EF_mu15_mu10_EFFS',
                        #'EF_2mu4T_DiMu_Barrel',
			#'EF_2mu4T_DiMu_BarrelOnly',
			#'EF_mu4Tmu6_DiMu_Barrel',
			#'EF_mu4Tmu6_DiMu_noVtx_noOS',

                        'EF_3mu6',
                        'EF_3mu6_MSonly',            
                        
                        
                        # 2010 Items
                                               
                        
                        ])
    
        ##mixed bits
        defineTriggerBits (GenericTriggerBitsD3PDObject, 10,
                   [# Level 1
                    'L1_MU0_J5',
                    'L1_MU0_J10',
                    'L1_MU0_J40',
                    'L1_MU0_J70',
                    
                    'L1_EM10_XE20',

                    # Level 2
                    'L2_mu4_j10',
                    'L2_mu4_j15',
                    'L2_mu4_j10_matched',
                    'L2_mu4_j20_matched',
                    'L2_mu4_j70_matched',

                    'L2_e15_medium_xe30_noMu',

                    'L2_e10_mu6',

                    # Event Filter
                    'EF_mu4_j10',
                    'EF_mu4_j20',
                    'EF_mu4_j10_matched',
                    'EF_mu4_j20_matched',
                    'EF_mu4_j40_matched',
                    'EF_mu4_j70_matched',

                    'EF_2mu6_MSonly_g10_loose',  
                    'EF_e15_medium_xe30',
                  
                    'EF_e10_mu6',
                    ])
  
