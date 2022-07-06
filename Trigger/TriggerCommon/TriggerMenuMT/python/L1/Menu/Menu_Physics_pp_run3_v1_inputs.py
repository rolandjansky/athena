# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from collections import OrderedDict as odict

from ..Base.L1MenuFlags import L1MenuFlags
from ..Base.MenuConfObj import TopoMenuDef


def defineInputsMenu():
    
    ctpinBoards = odict() # Ctpin/Slot9 (CTPCAL, NIM1, NIM2)
    topoBoards = odict()  # Topo1, Topo2, Topo3
    muctpiBoard = odict() # MuCTPi
    alfaBoard = odict() # ALFA


    #-----------------------------------
    # SLOT 9 / CON 1 (CTPCal, NIM1,NIM2)
    # https://twiki.cern.ch/twiki/bin/view/Atlas/LevelOneCentralTriggerSetup#CTPIN_Slot_9
    #-----------------------------------
    ctpinBoards["Ctpin9"] = odict()
    ctpinBoards["Ctpin9"]["connectors"] = []
    ctpinBoards["Ctpin9"]["connectors"] += [
        {
            "name" : "CTPCAL",
            "format" : "multiplicity",
            "nbitsDefault" : 1,
            "type" : "ctpin",
            "legacy" : False,
            "thresholds" : [
                'BCM_AtoC', 'BCM_CtoA', 'BCM_Wide', # 3 x 1-bit BCM
                ('BCM_Comb',3), # 1x 3-bit BCM
                'BCM6', 'BCM7', 'BCM8', # 2-hit BCM, for Run 3. 8 is not used
                (None, 6),
                'BMA0', 'BMA1',  # 2x BMA demonstrator
                'BPTX0','BPTX1', # 2x BPTX
                'LUCID_A', 'LUCID_C', # 2x LUCID 
                (None,4),
                'ZDC_0', 'ZDC_1', 'ZDC_2', # 3x ZDC
                'CAL0','CAL1','CAL2', # 3 x CALREQ
            ]
        },
        {
            "name" : "NIM1",
            "format" : "multiplicity",
            "nbitsDefault" : 1,
            "type" : "ctpin",
            "legacy" : False,
            "thresholds" : [
                'MBTS_A0', 'MBTS_A1', 'MBTS_A2', 'MBTS_A3', 'MBTS_A4'  , 'MBTS_A5', 'MBTS_A6', 'MBTS_A7', 'MBTS_A8', 'MBTS_A9', 'MBTS_A10', 'MBTS_A11',
                'MBTS_A12', 'MBTS_A13', 'MBTS_A14', 'MBTS_A15', # 16x MBTSSI 
                ('MBTS_A',3),         # 1x MBTS_A
                'NIML1A',             # L1A for CTP monitoring itself
                'NIMLHCF',            # LHCF
                'AFP_NSA', 'AFP_FSA', 'AFP_FSA_TOF_T0', 'AFP_FSA_TOF_T1', 'AFP_FSA_TOF_T2', 'AFP_FSA_TOF_T3'   # 2xAFP
            ]
        },
        {
            "name" : "NIM2",
            "format" : "multiplicity",
            "nbitsDefault" : 1,
            "type" : "ctpin",
            "legacy" : False,
            "thresholds" : [
                'MBTS_C0', 'MBTS_C1', 'MBTS_C2', 'MBTS_C3', 'MBTS_C4', 'MBTS_C5', 'MBTS_C6', 'MBTS_C7', 'MBTS_C8', 'MBTS_C9', 'MBTS_C10', 'MBTS_C11', 
                'MBTS_C12', 'MBTS_C13', 'MBTS_C14', 'MBTS_C15', # 16x MBTSSI 
                ('MBTS_C',3), # 1x MBTS_C
                'NIMTGC',     # TGC
                'NIMRPC',     # RPC
                'NIMTRT',     # TRT
                'AFP_NSC', 'AFP_FSC', 'AFP_FSC_TOF_T0', 'AFP_FSC_TOF_T1', 'AFP_FSC_TOF_T2', 'AFP_FSC_TOF_T3'   # 2xAFP
            ]
        }
    ]


    #
    # new topo board for multiplicities
    #
    topoBoards["Topo1"] = odict([("connectors",[])])
    topoBoards["Topo1"]["connectors"].append({ # first optical connector
        "name" : "Topo1Opt0",
        "format" : "multiplicity",
        "nbitsDefault" : 2,
        "type" : "optical",
        "legacy" : False,
        "thresholds" : [  # Topo1A: eFex EM, eFex TAU, gJ, gLJ 
            # eEM thresholds for commissioning
            ('eEM5',3), ('eEM7',3), ('eEM9',3), ('eEM10L',3), 

            (None,3), (None,3), (None,3), (None,3), 

            'eEM12L', 'eEM15', 'eEM18', 'eEM18L', 'eEM18M',
            'eEM22M', 'eEM24L', 
            'eEMSPARE1', 'eEMSPARE2', 'eEMSPARE3',

            # variable eEM  thresholds
            'eEM24VM', 'eEM26', 'eEM26L', 'eEM26M', 'eEM26T', 'eEM28M', 

            # eEM thresholds for production      
            'eEMSPARE4', 'eEMSPARE5', 'eEMSPARE6', 'eEMSPARE7', 
        ],
    })

    topoBoards["Topo1"]["connectors"].append({ # second optical connector
        "name" : "Topo1Opt1",
        "format" : "multiplicity",
        "nbitsDefault" : 2,
        "type" : "optical",
        "fpga" : 0,
        "legacy" : False,
        "thresholds" : [ # Topo1A: eFex EM, eFex TAU, gJ, gLJ
            # eTAU thresholds for commissioning
            ('eTAU12',3), ('eTAU20',3), ('eTAUSPARE1',3), 

            (None,3),

            'eTAU20L', 'eTAU20M', 'eTAU30',
            'eTAU35', 'eTAU60', 'eTAU80', 'eTAU140',
            'eTAU40HM', 
         
            # eTAU thresholds for production
            'eTAUSPARE2', 'eTAUSPARE3', 'eTAUSPARE4', 'eTAUSPARE5', 'eTAUSPARE6', 'eTAUSPARE7',

            None, None, 

            # gLJ thresholds for commissioning
            'gLJ80', 'gLJ100', 'gLJ140', 'gLJ160',

            # gLJ thresholds for production
            'gLJSPARE1', 'gLJSPARE2', 'gLJSPARE3', 'gLJSPARE4',

            None, 

            # gJ thresholds for commissioning
            ('gJ20',3), ('gJ30',3), ('gJ40',3), 

            (None,3),

            'gJ50', 'gJ100', 
            'gJ160',

        ]
    })

    topoBoards["Topo1"]["connectors"].append({ # third optical connector
        "name" : "Topo1Opt2",
        "format" : "multiplicity",
        "nbitsDefault" : 2,
        "type" : "optical",
        "fpga" : 1,
        "legacy" : False,
        "thresholds" : [ # Topo1B: jFex small-R jet, jFex large-R jet, combined eFex/jFex TAU, gFex+jFex EX, gFex+jFex SumET, jFex TAU
            # jJ thresholds for commissioning
            ('jJ20',3), ('jJ30',3), ('jJ30p0ETA25',3), ('jJ40',3), ('jJ40p0ETA25',3),
            ('jJ50',3), ('jJ55',3), ('jJ55p0ETA23',3), ('jJ60',3),
            ('jJSPARE1',3), ('jJSPARE2',3),

            (None,3),

            'jJ70p0ETA23', 'jJ80', 'jJ80p0ETA25', 'jJ85p0ETA21', 'jJ90', 'jJ125',
            'jJ140', 'jJ160', 'jJ180', 'jJ500',

            'jJ40p31ETA49', 'jJ50p31ETA49', 'jJ60p31ETA49', 'jJ90p31ETA49', 'jJ125p31ETA49',

            # jJ thresholds for production
            'jJSPARE3', 'jJSPARE4', 'jJSPARE5', 'jJSPARE6',

            None,None,

            # jLJ thresholds for commissioning
            'jLJ80', 'jLJ120', 'jLJ140', 'jLJ180',

            # jLJ thresholds for production
            'jLJ60', 'jLJ100', 'jLJ160', 'jLJ200',

        ]
    })

    topoBoards["Topo1"]["connectors"].append({ # fourth optical connector
        "name" : "Topo1Opt3",
        "format" : "multiplicity",
        "nbitsDefault" : 2,
        "type" : "optical",
        "fpga" : 1,
        "legacy" : False,
        "thresholds" : [ # Topo1B: jFex small-R jet, jFex large-R jet, combined eFex/jFex TAU, gFex+jFex EX, gFex+jFex SumET, jFex TAU
            # jTAU thresholds for commissioning
            ('jTAU20',3), 

            (None,3),

            'jTAU30', 'jTAU30M',
            # jTAU thresholds for production
            'jTAUSPARE1',

            None, 

            # cTAU thresholds for commissioning
            ('cTAU20M',3), ('cTAUSPARE1',3), 

            (None,3),

            'cTAU30M', 'cTAU35M', 
            # cTAU thresholds for production
            'cTAUSPARE2',

            None,

            # jEM thresholds for commissioning
            'jEM20', 'jEM20M', 
            # jEM thresholds for production
            'jEMSPARE1',
    
            None,

            # energy thresholds
            # commissioning
            # jXE
            ('jXE70',1), ('jXE80',1), ('jXE100',1), ('jXE110',1), ('jXE500',1),
            # gXE
            ('gXERHO70',1), ('gXERHO100',1),
            ('gXENC70',1), ('gXENC100',1),
            ('gXEJWOJ70',1), ('gXEJWOJ80',1), ('gXEJWOJ100',1),
            # gTE
            ('gTE200',1),

            # MHT
            ('gMHT500',1),

            # test thresholds
            ('jXEC100',1),
            ('jTE200',1), ('jTEC200',1), ('jTEFWD100',1), ('jTEFWDA100',1), ('jTEFWDC100',1),

            # spare energy thresholds for commissioning
            ('jXESPARE1',1), ('jXESPARE2',1), ('jXESPARE3',1), ('jXESPARE4',1), ('jXESPARE5',1), ('jXESPARE6',1), ('jXESPARE7',1), ('jXESPARE8',1), ('jXESPARE9',1),

            # production
            ('jXESPARE10',1), ('jXESPARE11',1), ('jXESPARE12',1), ('jXESPARE13',1), 
            ('jXESPARE14',1),
            ('jXESPARE15',1), ('jXESPARE16',1), ('jXESPARE17',1), ('jXESPARE18',1), ('jXESPARE19',1),
            ('jXESPARE20',1), ('jXESPARE21',1), ('jXESPARE22',1), ('jXESPARE23',1), ('jXESPARE24',1),
            ('jXESPARE25',1), ('jXESPARE26',1), ('jXESPARE27',1), ('jXESPARE28',1),

        ]
    })

    topoBoards["Topo2"] = odict()
    topoBoards["Topo2"]["connectors"] = []
    topoBoards["Topo2"]["connectors"].append({
        "name" : "Topo2El",
        "format" : "topological",
        "type" : "electrical",
        "legacy" : False,
        "algorithmGroups" : [
            {
                "fpga" : 0,
                "clock" : 0,
                "algorithms" : [
                    TopoMenuDef( '2INVM9-0DR15-MU5VFab-MU3Vab',          outputbits = 0 ), # BLS 
                    TopoMenuDef( '2INVM9-0DR15-2MU3Vab',                 outputbits = 1 ), # BLS 
                    TopoMenuDef( 'INVM_DR_2MU5VFab',                     outputbits = (2,3), outputlines = [ '2INVM9-2DR15-2MU5VFab',      # BLS
                                                                                                             '8INVM15-0DR22-2MU5VFab' ] ), # BLS
                    TopoMenuDef( '5DETA99-5DPHI99-2MU3Vab',              outputbits = 4 ), # Low-mass DY
                    TopoMenuDef( '5DETA99-5DPHI99-MU5VFab-MU3Vab',       outputbits = 5 ), # Low-mass DY
                    TopoMenuDef( '5DETA99-5DPHI99-2MU5VFab',             outputbits = 6 ), # Low-mass DY
                    TopoMenuDef( 'DR_2MU5VFab',                          outputbits = (7,8) , outputlines = [  '0DR15-2MU5VFab' ,   # LVF
                                                                                                              '10DR99-2MU5VFab'] ), # Msonly Narrow Scan
                    TopoMenuDef( '8INVM15-0DR22-CMU5VFab-CMU3Vab',       outputbits = 9 ), # BLS
                    TopoMenuDef( 'LATE-MU10s1',                          outputbits = 10 ),
                    TopoMenuDef( 'INVM_DR_2MU3VFab',                     outputbits = (11,12), outputlines = ['2INVM9-0DR15-2MU3VFab',
                                                                                                              '7INVM11-25DR99-2MU3VFab'] ), #BLS, ATR-21566
                ]
            },            

            {
                "fpga" : 0,
                "clock" : 1,
                "algorithms" : [
                    TopoMenuDef( '0INVM10-3MU3Vab',                          outputbits = 0 ), # BLS
                    TopoMenuDef( '0DR04-MU3Vab-CjJ40ab',                     outputbits = 1 ), # Bjet, TODO: not a primary
                    TopoMenuDef( '0DR04-MU5VFab-CjJ50ab',                    outputbits = 2 ), # Bjet, TODO: not a primary
                    TopoMenuDef( '2DISAMB_jJ55ab_DR_eTAU_eTAU',              outputbits = (3,4), outputlines = [ '2DISAMB-jJ55ab-0DR25-eTAU30ab-eTAU20ab', 
                                                                                                                 '2DISAMB-jJ55ab-0DR28-eTAU30ab-eTAU20ab'  ]),
                    TopoMenuDef( 'DR_eTAU30ab_eTAU20ab',                     outputbits = (5,6), outputlines = [ '0DR25-eTAU30ab-eTAU20ab', 
                                                                                                                 '0DR28-eTAU30ab-eTAU20ab'  ]),
                    TopoMenuDef( '2DISAMB-jJ55ab-0DR28-eTAU30abm-eTAU20abm', outputbits = 7 ),
                    TopoMenuDef( '0DR28-eTAU30abm-eTAU20abm',                outputbits = 8 ), 
                    TopoMenuDef( '0INVM10-3MU3VFab',                         outputbits = 9 ), # BLS
                    
                ]
            },
            
            {
                "fpga" : 1,
                "clock" : 0,
                "algorithms" : [
                    TopoMenuDef( 'INVM_DPHI_eEMsm6',                     outputbits = (0,1), outputlines = [ '0INVM70-27DPHI32-eEM12sm1-eEM12sm6',
                                                                                                             '0INVM70-27DPHI32-eEM15sm1-eEM15sm6' ] ),
                    
                    TopoMenuDef( 'ZEE-eEM24sm2',                         outputbits = 2 ),
                    TopoMenuDef( '0DR03-eEM9ab-CjJ40ab',                 outputbits = 3 ),
                    TopoMenuDef( 'INVM_eEMs6',                           outputbits = (4,5), outputlines = [ '1INVM5-eEM9s1-eEMs6',
                                                                                                             '1INVM5-eEM15s1-eEMs6'] ),
                    TopoMenuDef( '27DPHI32-eEMs1-eEMs6',                 outputbits = 6 ),
                    TopoMenuDef( '0INVM70-27DPHI32-eEM9s1-eEM9s6',       outputbits = 7 ),
                    TopoMenuDef( '0INVM70-27DPHI32-eEM9sl1-eEM9sl6',     outputbits = 8 ),
                    TopoMenuDef( '0INVM9-eEM9ab-eEMab',                  outputbits = 9 ),
                ]
            },
            
            {
                "fpga" : 1,
                "clock" : 1,
                "algorithms" : [
                    TopoMenuDef( 'jINVM_DPHI_NFF',                             outputbits = (0,3), outputlines = ['400INVM-0DPHI26-jJ60s6-AjJ50s6',
                                                                                                                  '400INVM-0DPHI24-jJ60s6-AjJ50s6',
                                                                                                                  '400INVM-0DPHI22-jJ60s6-AjJ50s6',
                                                                                                                  '400INVM-0DPHI20-jJ60s6-AjJ50s6'] ),
                    TopoMenuDef( 'jINVM',                                       outputbits = (4,7), outputlines = ['300INVM-AjJ60s6-AjJ50s6',
                                                                                                                  '400INVM-AjJ60s6-AjJ50s6',
                                                                                                                  '500INVM-AjJ60s6-AjJ50s6',
                                                                                                                  '700INVM-AjJ60s6-AjJ50s6'] ),
                    TopoMenuDef( 'jINVM_DPHI',                                  outputbits = (8,11), outputlines = ['400INVM-0DPHI26-AjJ60s6-AjJ50s6',
                                                                                                                   '400INVM-0DPHI24-AjJ60s6-AjJ50s6',
                                                                                                                   '400INVM-0DPHI22-AjJ60s6-AjJ50s6',
                                                                                                                   '400INVM-0DPHI20-AjJ60s6-AjJ50s6'] ),
                ]
            }
        ]
    })

    topoBoards["Topo3"] = odict()
    topoBoards["Topo3"]["connectors"] = []
    topoBoards["Topo3"]["connectors"].append({
        "name" : "Topo3El",
        "format" : "topological",
        "type" : "electrical",
        "legacy" : False,
        "algorithmGroups" : [
            {
                "fpga" : 0,
                "clock" : 0,
                "algorithms" : [
                    TopoMenuDef( 'HT190-jJ40s5pETA21',                       outputbits = 0 ),
                    TopoMenuDef( 'jINVM_NFF',                                outputbits = (1,4), outputlines = ['300INVM-jJ60s6-AjJ50s6',
                                                                                                                '400INVM-jJ60s6-AjJ50s6',
                                                                                                                '500INVM-jJ60s6-AjJ50s6',
                                                                                                                '700INVM-jJ60s6-AjJ50s6',] ), # TODO: needed? 
                    TopoMenuDef( 'HT150-jJ50s5pETA31',                       outputbits = 5 ),
                    TopoMenuDef( '400INVM-AjJ60s6pETA31-AjJ50s6p31ETA49',    outputbits = 6 ),
                    TopoMenuDef( 'SC111-CjJ40abpETA26',                      outputbits = 7 ),
                    TopoMenuDef( '0DETA20-jJ90s1-jJs2',                      outputbits = 8 ),
                    TopoMenuDef( '100RATIO-0MATCH-eTAU40si2-eEMall',         outputbits = 9 ),
                    TopoMenuDef( 'NOT-0MATCH-eTAU40si1-eEMall',              outputbits = 10),
                ]
            },

            {
                "fpga" : 0,
                "clock" : 1,
                "algorithms" : [
                    TopoMenuDef( 'KF-jXE-AjJall',                  outputbits = (0,5), outputlines = [ 'KF-jXE40-AjJall',
                                                                                                       'KF-jXE50-AjJall',
                                                                                                       'KF-jXE55-AjJall',
                                                                                                       'KF-jXE60-AjJall',
                                                                                                       'KF-jXE65-AjJall',
                                                                                                       'KF-jXE75-AjJall'] ),
                    TopoMenuDef( 'ZAFB_DPHI',                      outputbits = (6,7), outputlines = [ '60INVM-04DPHI32-eEM18abm-FjJj40s623ETA49',
                                                                                                       '60INVM-25DPHI32-eEM18abm-FjJj40s623ETA49'] ),
                    TopoMenuDef( 'CEP_CjJ',                        outputbits = (8,9), outputlines = [ 'CEP-CjJ90s6',
                                                                                                       'CEP-CjJ100s6'] ),
                ]
            },

            {
                "fpga" : 1,
                "clock" : 0,
                "algorithms" : [
                    TopoMenuDef( 'INVM_DR_eEM_MU',                       outputbits = (0,1), outputlines =  ['0INVM10-0DR15-eEM10abl-MU8Fab',
                                                                                                            '0INVM10-0DR15-eEM15abl-MU5VFab' ]) #LFV
                    
                ]
            },

            {
                "fpga" : 1,
                "clock" : 1,
                "algorithms" : [
                    TopoMenuDef( '7INVM14-MU5VFab-MU3VFab',              outputbits = 0 ), #BLS, ATR-22782
                    TopoMenuDef( '7INVM14-2MU3Vab',                      outputbits = 1 ), #BLS, ATR-22782
                    TopoMenuDef( 'INVM_2MU3VFab',                        outputbits = (2,3), outputlines = ['7INVM14-2MU3VFab', 
                                                                                                            '7INVM22-2MU3VFab' ] ), #BLS, ATR-21566
                    TopoMenuDef( '7INVM22-MU5VFab-MU3VFab',              outputbits = 4 ), #BLS, ATR-21566
                    TopoMenuDef ( '7INVM22_DR_2MU3Vab',                  outputbits = (5,6), outputlines = ['7INVM22-0DR20-2MU3Vab',
                                                                                                            '7INVM22-0DR12-2MU3Vab']),
                    TopoMenuDef( '2INVM9-0DR15-C-MU5VFab-MU3Vab',        outputbits = 7 ), #BLS, test
                    TopoMenuDef( '0INVM10C-3MU3Vab',                     outputbits = 8), #BLS, test
                    TopoMenuDef( '7INVM14-0DR25-MU5VFab-MU3VFab',        outputbits = 9), #BLS 
                    TopoMenuDef( '7INVM22-0DR20-2MU3VFab',               outputbits = 10), #BLS, ATR-21566
                ]
            }
        ]
    })

    muctpiBoard["MuCTPi"] = odict() 
    muctpiBoard["MuCTPi"]["connectors"] = []
    muctpiBoard["MuCTPi"]["connectors"].append({
        "name" : "MuCTPiOpt0",
        "format" : "multiplicity",
        "nbitsDefault" : 2,
        "type" : "optical",
        "legacy" : False,
        "thresholds" : [
            ('MU3V',3), ('MU3VF',3), ('MU3VC',3), ('MU3EOF',3), ('MU5VF',3), 
            'MU8F', 'MU8VF', 'MU8FC', 'MU8FH', 'MU8VFC', 'MU9VF', 'MU9VFC', 'MU12FCH', 
            'MU14FCH', 'MU14FCHR', 'MU15VFCH', 'MU15VFCHR', 'MU18VFCH', 'MU20VFC',
            'MU4BO', 'MU4BOM', 'MU10BO', 'MU10BOM', 'MU12BOM',
            'MU8EOF', 'MU14EOF', 
            # 57 bits for standard muon thresholds
            (None,7),
            # 64th bit for NSW monitoring
            ('NSWMon', 1)
        ]

    })

    muctpiBoard["MuCTPi"]["connectors"].append({
        "name" : "MuCTPiEl",
        "format" : "topological",
        "type" : "electrical",
        "legacy" : False,
        "algorithmGroups" : [
            {
                "fpga" : 0,
                "clock" : 0,
                "algorithms" : []
            },
            {
                "fpga" : 0,
                "clock" : 1,
                "algorithms" : [
                    TopoMenuDef( "MUCTP-0DR15-2MU5VFab",            outputbits = 0 ),
                ]
            },
            {
                "fpga" : 1,
                "clock" : 0,
                "algorithms" : []
            },
            {
                "fpga" : 1,
                "clock" : 1,
                "algorithms" : []
            }
        ]
    })


    alfaBoard["AlfaCtpin"] = odict()
    alfaBoard["AlfaCtpin"]["connectors"] = []
    alfaBoard["AlfaCtpin"]["connectors"].append({
        "name" : "AlfaCtpin",
        "format" : "simple",
        "nbitsDefault" : 1,
        "type" : "electrical",
        "legacy" : False,
        "signalGroups" : [
            {
                "clock" : 0,
                "signals" : [
                    (None,2), "ALFA_B7R1L", "ALFA_A7R1L", "ALFA_A7L1L", "ALFA_B7L1L",
                    (None,2), "ALFA2_B7R1L", "ALFA2_A7R1L", "ALFA2_A7L1L", "ALFA2_B7L1L",
                    (None,2), "ALFA3_B7R1L", "ALFA3_A7R1L", "ALFA3_A7L1L", "ALFA3_B7L1L",
                    (None,2), "ALFA4_B7R1L", "ALFA4_A7R1L", "ALFA4_A7L1L", "ALFA4_B7L1L",
                    (None,2), "ALFA_B7R1L_OD", "ALFA_A7R1L_OD", "ALFA_A7L1L_OD", "ALFA_B7L1L_OD"
                ]
            },
            {
                "clock" : 1,
                "signals" : [
                    (None,2), "ALFA_B7R1U", "ALFA_A7R1U", "ALFA_A7L1U", "ALFA_B7L1U",
                    (None,2), "ALFA2_B7R1U", "ALFA2_A7R1U", "ALFA2_A7L1U", "ALFA2_B7L1U",
                    (None,2), "ALFA3_B7R1U", "ALFA3_A7R1U", "ALFA3_A7L1U", "ALFA3_B7L1U",
                    (None,2), "ALFA4_B7R1U", "ALFA4_A7R1U", "ALFA4_A7L1U", "ALFA4_B7L1U",
                    (None,2), "ALFA_B7R1U_OD", "ALFA_A7R1U_OD", "ALFA_A7L1U_OD", "ALFA_B7L1U_OD"
                ]
            }
        ]
    })


    L1MenuFlags.boards().clear()

    L1MenuFlags.boards().update( topoBoards )   # Topo1, Topo2, Topo3

    L1MenuFlags.boards().update( muctpiBoard )  # MuCTPi

    L1MenuFlags.boards().update( ctpinBoards )  # CTPIN/Slot9 NIM1, NIM2, CALREQ

    L1MenuFlags.boards().update( alfaBoard )  # ALFA

    #----------------------------------------------

    def remapThresholds():
        # remap thresholds. TODO: add checks in case the remap does not fulfill HW constraints?
        for boardName, boardDef in L1MenuFlags.boards().items():
            if "connectors" in boardDef:
                for c in boardDef["connectors"]:
                    if "thresholds" in c:
                        thresholdsToRemove = []
                        for thrIndex, thrName in enumerate(c["thresholds"]):
                            nBits = 0
                            if type(thrName)==tuple:
                                (thrName,nBits) = thrName
                            if thrName in L1MenuFlags.ThresholdMap():
                                if (L1MenuFlags.ThresholdMap()[thrName] != ''):
                                    if nBits > 0:
                                        c["thresholds"][thrIndex] = (L1MenuFlags.ThresholdMap()[thrName],nBits)
                                    else:
                                        c["thresholds"][thrIndex] = L1MenuFlags.ThresholdMap()[thrName]
                                else:
                                    thresholdsToRemove.append(thrIndex) 
                        for i in reversed(thresholdsToRemove):
                            del c["thresholds"][i]
          
    #----------------------------------------------

    remapThresholds()
