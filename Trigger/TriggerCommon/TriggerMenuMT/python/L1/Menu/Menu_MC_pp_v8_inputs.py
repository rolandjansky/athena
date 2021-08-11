# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

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
                (None,11),
                'BPTX0','BPTX1', # 2x BPTX
                'LUCID_A', 'LUCID_C', # 2x LUCID 
                (None,4),
                'ZDC_A', 'ZDC_C', 'ZDC_AND', # 3x ZDC
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
                'MBTS_A0', 'MBTS_A1', 'MBTS_A2', 'MBTS_A3', 'MBTS_A4'  , 'MBTS_A5', 'MBTS_A6', 'MBTS_A7', 'MBTS_A8', 'MBTS_A10', 'MBTS_A12', 'MBTS_A14',
                'MBTS_A9', 'MBTS_A11', 'MBTS_A13', 'MBTS_A15', # 16x MBTSSI 
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
                'MBTS_C0', 'MBTS_C1', 'MBTS_C2', 'MBTS_C3', 'MBTS_C4', 'MBTS_C5', 'MBTS_C6', 'MBTS_C7', 'MBTS_C8', 'MBTS_C10', 'MBTS_C12', 'MBTS_C14', 
                'MBTS_C11', 'MBTS_C13', 'MBTS_C9', 'MBTS_C15', # 16x MBTSSI 
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
        "nbitsDefault" : 3,
        "type" : "optical",
        "legacy" : False,
        "thresholds" : [
            'eEM3', 'eEM7', 'eEM8', 'eEM8L', 'eEM10', 'eEM10L', 'eEM15', 'eEM15L', 'eEM15M',
            'eEM18M', 'eEM20', 'eEM20L', 'eEM20M', 'eEM22', 'eEM22M', 'eEM22T',
            'eTAU12', 'eTAU12IM', 'eTAU20', 'eTAU20IM', 'eTAU25IM', 'eTAU40', 'eTAU60', 'eTAU100'
        ]
    })

    topoBoards["Topo1"]["connectors"].append({ # second optical connector
        "name" : "Topo1Opt1",
        "format" : "multiplicity",
        "nbitsDefault" : 3,
        "type" : "optical",
        "fpga" : 0,
        "legacy" : False,
        "thresholds" : [
            'jJ12',  'jJ12p0ETA25',  'jJ15',  'jJ15p0ETA25',  'jJ20',  'jJ25',  'jJ25p0ETA23',  'jJ30',
            'jJ35p0ETA23',  'jJ40p0ETA25',  'jJ40',  'jJ50',  'jJ85',  'jJ100',  'jJ15p31ETA49', 
            'jJ20p31ETA49',  'jJ75p31ETA49'
        ]
    })

    topoBoards["Topo1"]["connectors"].append({ # third optical connector
        "name" : "Topo1Opt2",
        "format" : "multiplicity",
        "nbitsDefault" : 3,
        "type" : "optical",
        "fpga" : 1,
        "legacy" : False,
        "thresholds" : [
            'gXERHO20',  'gXERHO30',  'gXERHO35',  'gXERHO40',  'gXERHO45',  'gXERHO50',
            'gXEPUFIT20',  'gXEPUFIT50',  'gXE50'
        ]
    })

    topoBoards["Topo1"]["connectors"].append({ # fourth optical connector
        "name" : "Topo1Opt3",
        "format" : "multiplicity",
        "nbitsDefault" : 3,
        "type" : "optical",
        "fpga" : 1,
        "legacy" : False,
        "thresholds" : [
            'jXE50'
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
                    TopoMenuDef( '2DISAMB-jJ25ab-0DR25-eTAU20abi-eTAU12abi',   outputbits = 0 ),
                    TopoMenuDef( '2DISAMB-jJ25ab-0DR28-eTAU20abi-eTAU12abi',   outputbits = 1 ),
                    TopoMenuDef( 'INVM_DPHI_jNFF',                             outputbits = (2,5), outputlines = ['400INVM-0DPHI26-jJ30s6-AjJ20s6',
                                                                                                                  '400INVM-0DPHI24-jJ30s6-AjJ20s6',
                                                                                                                  '400INVM-0DPHI22-jJ30s6-AjJ20s6',
                                                                                                                  '400INVM-0DPHI20-jJ30s6-AjJ20s6'] ),
                    TopoMenuDef( '0INVM10-3MU4ab',                             outputbits = 6 ),
                    TopoMenuDef( '2DISAMB-TAU12abi-J25ab',                     outputbits = 7 ),  # not a primary
                    TopoMenuDef( 'LATE-MU10s1',                                outputbits = 8 ),
                    TopoMenuDef( 'MULT-CMU4ab',                                outputbits = (9,10), outputlines = ['MULT-CMU4ab[0]', 'MULT-CMU4ab[1]'] ), # 2bits
                    TopoMenuDef( 'MULT-CMU6ab',                                outputbits = (11,12), outputlines = ['MULT-CMU6ab[0]', 'MULT-CMU6ab[1]'] ), # 2bits
                    TopoMenuDef( '0DR04-MU4ab-CJ15ab',                         outputbits = 13 ),  # not a primary
                    TopoMenuDef( '0DR04-MU6ab-CJ20ab',                         outputbits = 14 ),  # not a primary
                ]
            },            

            {
                "fpga" : 0,
                "clock" : 1,
                "algorithms" : [
                    TopoMenuDef( '0DR25-eTAU20abi-eTAU12abi',                  outputbits = 0 ),
                    TopoMenuDef( '0DR28-eTAU20abi-eTAU12abi',                  outputbits = 1 ), 
                ]
            },
            
            {
                "fpga" : 1,
                "clock" : 0,
                "algorithms" : [
                    TopoMenuDef( 'KF-XE-AJall',                    outputbits = (0,5), outputlines = [ 'KF-XE40-AJall',
                                                                                                       'KF-XE50-AJall',
                                                                                                       'KF-XE55-AJall',
                                                                                                       'KF-XE60-AJall',
                                                                                                       'KF-XE65-AJall',
                                                                                                       'KF-XE75-AJall'] ),
                    TopoMenuDef( 'ZAFB_DPHI',                      outputbits = (6,7), outputlines = [ '60INVM-04DPHI32-eEM15abhi-FJj15s623ETA49',
                                                                                                       '60INVM-25DPHI32-eEM15abhi-FJj15s623ETA49'] ),
                    TopoMenuDef( 'CEP_CJ',                         outputbits = (8,9), outputlines = [ 'CEP-CJ50s6',
                                                                                                       'CEP-CJ60s6'] ),
                    TopoMenuDef( 'INVM_eEMs6',                     outputbits = (10,11), outputlines = [ '1INVM5-eEM7s1-eEMs6',
                                                                                                         '1INVM5-eEM12s1-eEMs6'] ),
                    TopoMenuDef( '27DPHI32-eEMs1-eEMs6',           outputbits = 12 ),
                    TopoMenuDef( '0INVM70-27DPHI32-eEM12s1-eEM12s6',           outputbits = 13 ),
                ]
            },
            
            {
                "fpga" : 1,
                "clock" : 1,
                "algorithms" : [
                    # EMPTY SO FAR...
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
                    TopoMenuDef( 'HT190-J15s5pETA21',                        outputbits = 0 ),
                    TopoMenuDef( '0DR03-eEM7ab-CJ15ab',                      outputbits = 1 ),
                    TopoMenuDef( 'INVM_jNFF',                                outputbits = (2,5), outputlines = ['300INVM-jJ30s6-AjJ20s6',
                                                                                                                '400INVM-jJ30s6-AjJ20s6',
                                                                                                                '500INVM-jJ30s6-AjJ20s6',
                                                                                                                '700INVM-jJ30s6-AjJ20s6',] ), 
                    TopoMenuDef( 'HT150-jJ20s5pETA31',                       outputbits = 6 ),
                    TopoMenuDef( '400INVM-AjJ30s6pETA31-AjJ20s6p31ETA49',    outputbits = 7 ),
                    TopoMenuDef( '10MINDPHI-J20s2-XE30',                     outputbits = 8 ),
                    TopoMenuDef( '10MINDPHI-J20s2-XE50',                     outputbits = 9 ),
                    TopoMenuDef( 'SC111-CJ15abpETA26',                       outputbits = 10 ),
                    TopoMenuDef( '0DETA20-J50s1-Js2',                        outputbits = 11 ),
                    TopoMenuDef( '0INVM9-eEM7ab-eEMab',                      outputbits = 12 ),
                ]
            },

            {
                "fpga" : 0,
                "clock" : 1,
                "algorithms" : [
                    # EMPTY SO FAR...
                ]
            },

            {
                "fpga" : 1,
                "clock" : 0,
                "algorithms" : [
                    TopoMenuDef( '100RATIO-0MATCH-TAU30si2-eEMall',      outputbits = 0 ),
                    TopoMenuDef( 'NOT-0MATCH-TAU30si1-eEMall',           outputbits = 1 ), 
                    TopoMenuDef( '0INVM70-27DPHI32-eEM10his1-eEM10his6', outputbits = 2 ),
                    TopoMenuDef( '0INVM70-27DPHI32-eEM12his1-eEM12his6', outputbits = 3 ),
                    TopoMenuDef( 'ZEE-eEM20shi2',                        outputbits = 4 ), 
                    TopoMenuDef( '10DR99-2MU6ab',                        outputbits = 5 ),
                ]
            },

            {
                "fpga" : 1,
                "clock" : 1,
                "algorithms" : [
                    TopoMenuDef( '8INVM15-0DR22-MU6ab-MU4ab',            outputbits = 0 ),
                    TopoMenuDef( '2INVM9-0DR15-MU6ab-MU4ab',             outputbits = 1 ),  
                    TopoMenuDef( '2INVM9-0DR15-2MU4ab',                  outputbits = 2 ), 
                    TopoMenuDef( '2INVM9-2DR15-2MU6ab',                  outputbits = 3 ),
                    TopoMenuDef( '5DETA99-5DPHI99-2MU4ab',               outputbits = 4 ),
                    TopoMenuDef( '5DETA99-5DPHI99-MU6ab-MU4ab',          outputbits = 5 ), 
                    TopoMenuDef( '5DETA99-5DPHI99-2MU6ab',               outputbits = 6 ),
                    TopoMenuDef( '0DETA04-eEM8abi-MU10ab',               outputbits = 7 ),
                    TopoMenuDef( '0DPHI03-eEM8abi-MU10ab',               outputbits = 8 ),
                    TopoMenuDef( '0DETA04-eEM15abi-MUab',                outputbits = 9 ),
                    TopoMenuDef( '0DPHI03-eEM15abi-MUab',                outputbits = 10 ),
                    TopoMenuDef( '0INVM10-0DR15-eEM8abi-MU10ab',         outputbits = 11 ),
                    TopoMenuDef( '0INVM10-0DR15-eEM12abi-MU6ab',         outputbits = 12 ),
                    TopoMenuDef( '8INVM15-2CMU4ab',                      outputbits = 13 ),
                    TopoMenuDef( '0DR15-2MU6ab',                         outputbits = 14 ),
                    TopoMenuDef( '8INVM15-0DR22-2MU6ab',                 outputbits = 15 ),
                ]
            }
        ]
    })

    muctpiBoard["MuCTPi"] = odict() 
    muctpiBoard["MuCTPi"]["connectors"] = []
    muctpiBoard["MuCTPi"]["connectors"].append({
        "name" : "MuCTPiOpt0",
        "format" : "multiplicity",
        "nbitsDefault" : 3,
        "type" : "optical",
        "legacy" : False,
        "thresholds" : [
            # exactly the first 4 thresholds must be defined with 3 bits, all others use 2 bits automatically
            ('MU4',3), ('MU6',3), ('MU10',3), ('MU11',3), 'MU20', 'MU21', 
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
                    TopoMenuDef( "MUCTP-0DR15-2MU6ab",            outputbits = 0 ),
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
