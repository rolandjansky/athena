# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""CPS addition  """

__doc__="Addition of coherent prescales to the list of chains"

from TriggerJobOpts.TriggerFlags  import TriggerFlags
from TriggerMenuPython.Lvl1Flags import Lvl1Flags

from AthenaCommon.Logging import logging

log = logging.getLogger('TriggerMenuPython.CPS.py')


def hackedCPSAdder(groups, signatures, chains, level):
#     Emanuel: This is an ugly, ugly hack to deal with mutliple
#     instances of the same chain, corresponding to a signature. A
#     proper fix will require re-imagining the filling of
#     triggerPythonConfig's theL2HLTChains and theEFHLTChains (which
#     I'll do after I get back from vacation. See Savannah #84711 for
#     more details.
    for cpsGroup, chainNames in groups.iteritems():
        for chainName in chainNames:
            if chainName not in signatures:
                log.warning('Signature %s not registered to TriggerPythonConfig' % chainName)
                continue

            if level=='L2':
                chain_name = signatures[chainName][0].chain_name
            elif level=='EF':
                chain_name = signatures[chainName][-1].chain_name

            #Now the ugly part where we make sure each instance has
            #the group added to it.
#            log.info('  dir(chains[0])=%s ' % dir(chains[0]) )
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
                log.warning('Signature %s not registered to TriggerPythonConfig' % chainName)
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
    L2_CPS_Groups= {
        
        "RATE:CPS:L2_L1TAU5" :
        ["hadCalib_trk9phi",
         "hadCalib_trk9",
         "tauNoCut",
         ],

        "RATE:CPS:L2_e24vh_medium1":
        ['e24vh_medium1',
         'e24vh_medium1_L2StarB',
         'e24vh_medium1_L2StarC',
         'e24vh_medium1_IDTrkNoCut',
         ],

        #-----------------
        # mu4T_j chains
        #-----------------                       
        "RATE:CPS:L2_mu4T_j45":
        ['mu4T_j45_a4tchad_matched',      # -> L2_mu4T_j40_c4cchad -> L1_MU4_J10
         'mu4T_j45_a4tchad_L2FS_matched', # -> L2_mu4T_j10_a4TTem  -> L1_MU4_J10
         ],

       "RATE:CPS:L2_mu4T_j55_j65":
        ['mu4T_j55_a4tchad_matched',      # -> L2_mu4T_j50_c4cchad -> L1_MU4_J15
         'mu4T_j65_a4tchad_matched',      # -> L2_mu4T_j60_c4cchad -> L1_MU4_J15
         'mu4T_j55_a4tchad_L2FS_matched', # -> L2_mu4T_j15_a4TTem -> L1_MU4_J15 
         ###'mu4T_j65_a4tchad_L2FS_matched', # -> L2_mu4T_j15_a4TTem -> L1_MU4_J15 taken out of group since same L2 item as above chain!!
         ],
        
        "RATE:CPS:L2_mu4T_j80":
        ['mu4T_j80_a4tchad_L2FS_matched', # -> L2_mu4T_j30_a4TTem  -> L1_MU4_J30
         'mu4T_j80_a4tchad_matched',      # -> L2_mu4T_j75_c4cchad -> L1_MU4_J30
         ],
        
        "RATE:CPS:L2_mu4T_j110":
        ['mu4T_j110_a4tchad_L2FS_matched', # -> L2_mu4T_j50_a4TTem   -> L1_MU4_J50
         'mu4T_j110_a4tchad_matched',      # -> L2_mu4T_j105_c4cchad -> L1_MU4_J50
         ],      

        "RATE:CPS:L2_mu4T_j145_j180_j220_j280":
        ['mu4T_j145_a4tchad_L2FS_matched',   # ->  L2_mu4T_j75_a4TTem -> L1_J75
         ###'mu4T_j180_a4tchad_L2FS_matched',   # ->  L2_mu4T_j75_a4TTem -> L1_J75
         ###'mu4T_j220_a4tchad_L2FS_matched',   # ->  L2_mu4T_j75_a4TTem -> L1_J75
         ###'mu4T_j280_a4tchad_L2FS_matched',   # ->  L2_mu4T_j75_a4TTem -> L1_J75
         'mu4T_j145_a4tchad_matched',        # ->  L2_mu4T_j140_c4cchad -> L1_J75
         'mu4T_j180_a4tchad_matched',        # ->  L2_mu4T_j165_c4cchad -> L1_J75
         ###'mu4T_j220_a4tchad_matched',        # ->  L2_mu4T_j165_c4cchad -> L1_J75
         ###'mu4T_j280_a4tchad_matched',        # ->  L2_mu4T_j165_c4cchad -> L1_J75
         ],


        #-----------------
        # b-jet chains
        #-----------------
       "RATE:CPS:L2_b45_4j45":
        ["b45_NoCut_4j45_a4tchad_L2FS",  # -> L2_b10_NoCut_4j10_a4TTem  -> L1_4J15
         "b45_NoCut_4j45_a4tchad",       # -> L2_b10_NoCut_4j10_c4cchad -> L1_4J15
         "4j45_a4tchad_L2FS",            # -> L2_4j15_a4TTem -> L1_4J15
        ],
        "RATE:CPS:L2_b55_NoCut":
        ["b55_NoCut_j55_a4tchad",        # -> L2_b50_NoCut_j50_c4cchad -> L1_J15
         "b55_NoCut_j55_a4tchad_L2FS",   # -> L2_b15_NoCut_j15_a4TTem  -> L1_J15
         ],
       "RATE:CPS:L2_b80_j80":
        ["b80_NoCut_j80_a4tchad",        # -> L2_b75_NoCut_j75_c4cchad -> L1_J30
         "j80_a4tchad",                  # -> L2_j75_c4cchad -> L1_J30
        ],
        "RATE:CPS:L2_b110_j110":
        ["b110_NoCut_j110_a4tchad",      # -> L2_b105_NoCut_j105_c4cchad -> L1_J50
         "j110_a4tchad",                 # -> L2_j105_c4cchad -> L1_J50
         ],
       "RATE:CPS:L2_b145_j145":
        ["b145_NoCut_j145_a4tchad",      # -> L2_b140_NoCut_j140_c4cchad -> L1_J75
         "j145_a4tchad",                 # -> L2_j140_c4cchad -> L1_J75
        ],


       "RATE:CPS:L2_bj180_220_280_":
        ["b180_NoCut_j180_a4tchad", # -> L2_b175_NoCut_j165_c4cchad -> L1_J75
         "b220_NoCut_j220_a4tchad", # -> L2_b215_NoCut_j165_c4cchad -> L1_J75
         "b280_NoCut_j280_a4tchad", # -> L2_b275_NoCut_j165_c4cchad -> L1_J75
         "j180_a4tchad",            # -> L2_j165_c4cchad -> L1_J75
        ],   
        
        ## comment out these groups and combine them in the group above since they share chain with same L2
       #"RATE:CPS:L2_b180_j180":
       # ["b180_NoCut_j180_a4tchad", # -> L2_b175_NoCut_j165_c4cchad -> L1_J75
       #  "j180_a4tchad",            # -> L2_j165_c4cchad -> L1_J75
       # ],   
       #"RATE:CPS:L2_b220_j220":
       # ["b220_NoCut_j220_a4tchad", # -> L2_b215_NoCut_j165_c4cchad -> L1_J75
       # "j220_a4tchad",             # -> L2_j165_c4cchad -> L1_J75
       # ],
       #"RATE:CPS:L2_b280_j280":
       # ["b280_NoCut_j280_a4tchad", # -> L2_b275_NoCut_j165_c4cchad -> L1_J75
       #  "j280_a4tchad",            # -> L2_j165_c4cchad -> L1_J75
       # ],



        #-----------------
        # Bphys chains
        #-----------------        
        "RATE:CPS:L2_Bphys":
        ["mu4T_Trk_Upsi_FS",       # -> L2_mu4T_Trk_Upsi_FS  -> L1_MU4
         "mu4T_DiMu_FS_noOS",      # -> L2_mu4T_DiMu_FS_noOS -> L1_MU4
         'mu4T_Upsimumu_SiTrk_FS', # -> L2_mu4T_Upsimumu_SiTrk_FS -> L1_MU4
         'mu4T_DiMu_FS',           # -> L2_mu4T_DiMu_FS_noOS -> L1_MU4
         'mu4T_Upsimumu_FS',       # -> L2_mu4T_Upsimumu_FS  -> L1_MU4
         ],

       "RATE:CPS:L2_2mu4T_mumu":
        ["2mu4T_Jpsimumu",
         "2mu4T_Bmumu",
         "2mu4T_Upsimumu",
         "2mu4T_Bmumux",
         "2mu4T_Jpsimumu_L2StarB",
         "2mu4T_Bmumu_L2StarB",
         "2mu4T_Upsimumu_L2StarB",
         "2mu4T_Bmumux_L2StarB",
         ],
       "RATE:CPS:L2_2mu4T_Jpsimumu":
        ["2mu4T_Jpsimumu_prescaled",
         "2mu4T_Jpsimumu_L2StarB_prescaled",
         ],        
        "RATE:CPS:L2_2mu4T_DiMu":
        ['2mu4T_DiMu',         # -> L2_2mu4T_DiMu       -> L1_2MU4
         '2mu4T_DiMu_SiTrk',   # -> L2_2mu4T_DiMu_SiTrk -> L1_2MU4
         "2mu4T_DiMu_L2StarB",      # -> L2_2mu4T_DiMu_L2StarB    -> L1_2MU4
         "2mu4T_DiMu_L2StarC",      # -> L2_2mu4T_DiMu_L2StarC    -> L1_2MU4
         "2mu4T_DiMu_l2muonSA",     # -> L2_2mu4T_DiMu_l2muonSA   -> L1_2MU4
         "2mu4T_DiMu_noVtx_noOS",   # -> L2_2mu4T_DiMu_noVtx_noOS -> L1_2MU4
         ],
        "RATE:CPS:L2_2mu4T_Jpsimumu_Barrel":
        ["2mu4T_Jpsimumu_Barrel_prescaled", 
         "2mu4T_Jpsimumu_Barrel_L2StarB_prescaled",
         ],        
        "RATE:CPS:L2_2mu4T_Jpsimumu_BarrelOnly":
        ["2mu4T_Jpsimumu_BarrelOnly_prescaled", 
         "2mu4T_Jpsimumu_BarrelOnly_L2StarB_prescaled", 
         ],
        

        "RATE:CPS:L2_mu6_Jpsi_Jpsimumu":
        ["mu6_Trk_Jpsi_loose",           # -> L2_mu6_Trk_Jpsi_loose          -> L1_MU6
         "mu6_Trk_Jpsi_loose_L2StarA",   # -> L2_mu6_Trk_Jpsi_loose_L2StarA  -> L1_MU6 
         "mu6_Trk_Jpsi_loose_L2StarB",   # -> L2_mu6_Trk_Jpsi_loose_L2StarB  -> L1_MU6
         "mu6_Jpsimumu_tight_L2StarB",   # -> L2_mu6_Jpsimumu_tight_L2Star   -> L1_MU6
         "mu6_Jpsimumu_L2StarA",         # -> L2_mu6_Jpsimumu_L2StarA        -> L1_MU6
         "mu6_Jpsimumu_tight",           # -> L2_mu6_Jpsimumu_tight          -> L1_MU6
         ],


        "RATE:CPS:L2_2mu6_DiMu":
        ["2mu6_DiMu",
         "2mu6_DiMu_noVtx_noOS",
         ],
        "RATE:CPS:L2_2mu6_Bmumux":
        ["2mu6_Bmumux",
         "2mu6_Bmumux_L2StarA",
         ],

        
        "RATE:CPS:L2_mu10_Jpsimumu":
        ["mu10_Jpsimumu",
         "mu10_Jpsimumu_L2StarA",
        ],
        "RATE:CPS:L2_mu10_Upsimumu_tight_FS": 
        ["mu10_Upsimumu_tight_FS",
         "mu10_Upsimumu_tight_FS_L2StarA",
        ],


        "RATE:CPS:L2_mu4Tmu6_Jpsimumu":        
        ["mu4Tmu6_Jpsimumu_prescaled", 
         "mu4Tmu6_Jpsimumu_L2StarB_prescaled", 
         ],
        "RATE:CPS:L2_mu4Tmu6_Jpsimumu_Barrel":
        ["mu4Tmu6_Jpsimumu_Barrel_prescaled", 
        "mu4Tmu6_Jpsimumu_Barrel_L2StarB_prescaled",
        ],

        "RATE:CPS:L2_trk140_pileupSup":
        ["mbSpTrkVtxMh_hip_trk140_L1TE20", 
         "mbSpTrkVtxMh_hip_trk140_L1TE20_pileupSup",       
        ],

        "RATE:CPS:L2_trk160_pileupSup":
        ["mbSpTrkVtxMh_hip_trk160_L1TE50", 
         "mbSpTrkVtxMh_hip_trk160_L1TE50_pileupSup",       
         ],

        "RATE:CPS:L2_trk180_pileupSup":
        ["mbSpTrkVtxMh_hip_trk180_L1TE50", 
         "mbSpTrkVtxMh_hip_trk180_L1TE50_pileupSup",       
         ],

        "RATE:CPS:L2_trk200_pileupSup":
        ["mbSpTrkVtxMh_hip_trk200_L1TE65", 
         "mbSpTrkVtxMh_hip_trk200_L1TE65_pileupSup",       
         ],

        "RATE:CPS:L2_trk240_pileupSup":
        ["mbSpTrkVtxMh_hip_trk240_L1TE65", 
         "mbSpTrkVtxMh_hip_trk240_L1TE65_pileupSup",       
         ],
        
        "RATE:CPS:L2_trk260_pileupSup":
        ["mbSpTrkVtxMh_hip_trk260_L1TE65", 
         "mbSpTrkVtxMh_hip_trk260_L1TE65_pileupSup",       
         ],
        
        }    
        
    EF_CPS_Groups = {


        "RATE:CPS:EF_mu4T_j145_j180_j220_j280_L2FS_matched":
        ['mu4T_j145_a4tchad_L2FS_matched',   # ->  L2_mu4T_j75_a4TTem -> L1_J75
         'mu4T_j180_a4tchad_L2FS_matched',   # ->  L2_mu4T_j75_a4TTem -> L1_J75
         'mu4T_j220_a4tchad_L2FS_matched',   # ->  L2_mu4T_j75_a4TTem -> L1_J75
         'mu4T_j280_a4tchad_L2FS_matched',   # ->  L2_mu4T_j75_a4TTem -> L1_J75
         ],

        "RATE:CPS:EF_mu4T_j145_j180_j220_j280_L2FS_matched":
        ['mu4T_j180_a4tchad_matched',        # ->  L2_mu4T_j165_c4cchad -> L1_J75
         'mu4T_j220_a4tchad_matched',        # ->  L2_mu4T_j165_c4cchad -> L1_J75
         'mu4T_j280_a4tchad_matched',        # ->  L2_mu4T_j165_c4cchad -> L1_J75
         ],
        
        "RATE:CPS:EF_mu4T_j55-65_L2FS_matched":
         ['mu4T_j55_a4tchad_L2FS_matched', # -> L2_mu4T_j15_a4TTem -> L1_MU4_J15 
          'mu4T_j55_a4tchad_L2FS_matchedZ',# -> L2_mu4T_j15_a4TTem -> L1_MU4_J15
          'mu4T_j65_a4tchad_L2FS_matched', # -> L2_mu4T_j15_a4TTem -> L1_MU4_J15 
          ],

        "RATE:CPS:EF_mu4T_j55_matched":
         ['mu4T_j55_a4tchad_matched',  # -> L2_mu4T_j50_c4cchad -> L1_MU4_J15
          'mu4T_j55_a4tchad_matchedZ', # -> L2_mu4T_j50_c4cchad -> L1_MU4_J15
          ],

        "RATE:CPS:EF_mu4T_j45_matched":
         ['mu4T_j45_a4tchad_matched',  # -> L2_mu4T_j40_c4cchad -> L1_MU4_J10
          'mu4T_j45_a4tchad_matchedZ', # -> L2_mu4T_j40_c4cchad -> L1_MU4_J10
          ],

        "RATE:CPS:EF_mu4T_j45_L2FS_matched":
         ['mu4T_j45_a4tchad_L2FS_matched',  # -> L2_mu4T_j10_a4TTem -> L1_MU4_J10
          'mu4T_j45_a4tchad_L2FS_matchedZ', # -> L2_mu4T_j10_a4TTem -> L1_MU4_J10
          ],


       "RATE:CPS:EF_mu4T_j15-j25-j35":
        ['mu4T_j15_a4tchad_matched',   # -> L2_mu4T -> L1_MU4
         'mu4T_j15_a4tchad_matchedZ',  # -> L2_mu4T -> L1_MU4
         'mu4T_j25_a4tchad_matched',   # -> L2_mu4T -> L1_MU4
         'mu4T_j25_a4tchad_matchedZ',  # -> L2_mu4T -> L1_MU4
         'mu4T_j35_a4tchad_matched',   # -> L2_mu4T -> L1_MU4
         'mu4T_j35_a4tchad_matchedZ',  # -> L2_mu4T -> L1_MU4
         ],

        
        "RATE:CPS:EF_e13_etctuTrk_xs60":
        ['e13_etcutTrk_xs60',              # -> L2_e13_etcutTrk_xs45 -> L1_EM6_XS45
         'e13_etcutTrk_xs60_dphi2j15xe20', # -> L2_e13_etcutTrk_xs45 -> L1_EM6_XS45
         ],

       "RATE:CPS:EF_j15_b15":
        ['b15_NoCut_j15_a4tchad', # -> L2_rd0_filled_NoAlg -> L1_RD0_FILLED
         'j15_a4tchad',           # -> L2_rd0_filled_NoAlg -> L1_RD0_FILLED
         ],

        }

    return (L2_CPS_Groups, EF_CPS_Groups)

def addCPS(triggerPythonConfig):
    #genericCPSAdder(L2_CPS_Groups, triggerPythonConfig.allChains, "L2")
    #genericCPSAdder(EF_CPS_Groups, triggerPythonConfig.allChains, "EF")
    L2_CPS_Groups, EF_CPS_Groups = defineCPSGroups()
    hackedCPSAdder(L2_CPS_Groups, triggerPythonConfig.allChains, triggerPythonConfig.theL2HLTChains, "L2")
    hackedCPSAdder(EF_CPS_Groups, triggerPythonConfig.allChains, triggerPythonConfig.theEFHLTChains, "EF")
