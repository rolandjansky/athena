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
                'AFP_NSC', 'AFP_NSA', 'AFP_FSA_SIT', 'AFP_FSA_TOF', 'AFP_FSA_LOG', 'AFP_FSC_SIT', 'AFP_FSC_LOG', 'AFP_FSC_TOF'   # 2xAFP
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
                'NIMTRT'      # TRT
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
            'eEM3', 'eEM7', 'eEM8', 'eEM8VH', 'eEM10', 'eEM10VH', 'eEM15', 'eEM15VH', 'eEM15VHI',
            'eEM18VHI', 'eEM20', 'eEM20VH', 'eEM20VHI', 'eEM22', 'eEM22VHI',
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
            'jJ12',  'jJ12.0ETA23',  'jJ15',  'jJ15.0ETA25',  'jJ20',  'jJ25',  'jJ25.0ETA23',  'jJ30',
            'jJ35.0ETA23',  'jJ40.0ETA25',  'jJ40',  'jJ50',  'jJ85',  'jJ100',  'jJ15.31ETA49', 
            'jJ20.31ETA49',  'jJ75.31ETA49'
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
                    TopoMenuDef( '0DR25-TAU20abi-TAU12abi',                 outputbits = 0 ),
                    TopoMenuDef( '0DR28-TAU20abi-TAU12abi',                 outputbits = 1 ),
                    TopoMenuDef( '2DISAMB-J25ab-0DR25-TAU20abi-TAU12abi',   outputbits = 2 ),
                    TopoMenuDef( '2DISAMB-J25ab-0DR28-TAU20abi-TAU12abi',   outputbits = 3 ),
                    TopoMenuDef( 'INVM_DPHI_NFF',                           outputbits = (4,7), outputlines = ['400INVM9999-0DPHI26-J30s6-AJ20s6',
                                                                                                               '400INVM9999-0DPHI24-J30s6-AJ20s6',
                                                                                                               '400INVM9999-0DPHI22-J30s6-AJ20s6',
                                                                                                               '400INVM9999-0DPHI20-J30s6-AJ20s6'] ),
                    TopoMenuDef( '0INVM10-3MU4ab',                          outputbits = 9 ),
                ]
            },            

            {
                "fpga" : 0,
                "clock" : 1,
                "algorithms" : [
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
                    TopoMenuDef( 'ZAFB_DPHI',                      outputbits = (6,7), outputlines = [ '60INVM9999-04DPHI32-EM15abhi-FJj15s623ETA49',
                                                                                                       '60INVM9999-25DPHI32-EM15abhi-FJj15s623ETA49'] ),
                    TopoMenuDef( 'CEP_CJ',                         outputbits = (8,9), outputlines = [ 'CEP-CJ50s6',
                                                                                                       'CEP-CJ60s6'] ),
                    TopoMenuDef( 'INVM_EMs6',                      outputbits = (10,11), outputlines = [ '1INVM5-EM7s1-EMs6',
                                                                                                         '1INVM5-EM12s1-EMs6'] ),
                    TopoMenuDef( '27DPHI32-EMs1-EMs6',             outputbits = 12 ),
                ]
            },
            
            {
                "fpga" : 1,
                "clock" : 1,
                "algorithms" : [
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
                    TopoMenuDef( '0DR03-EM7ab-CJ15ab',                       outputbits = 1 ),
                    TopoMenuDef( 'INVM_NFF',                                 outputbits = 2, outputlines = ['500INVM9999-J30s6-AJ20s6']), # 4 bits to 1 bit
                    TopoMenuDef( 'INVM_AJ_HighMass',                         outputbits = 3, outputlines = ['700INVM9999-AJ30s6-AJ20s6'] ), # 4 bits to 1 bit
                    TopoMenuDef( 'INVM_AJ_LowMass',                          outputbits = 4, outputlines = ['300INVM9999-AJ30s6-AJ20s6'] ), # 4 bits to 1 bit
                    TopoMenuDef( 'HT150-J20s5pETA31',                        outputbits = 5 ),
                    TopoMenuDef( '400INVM9999-AJ30s6pETA31-AJ20s6p31ETA49',  outputbits = 6 ),
                    TopoMenuDef( '10MINDPHI-J20s2-XE30',                     outputbits = 7 ),
                    TopoMenuDef( '10MINDPHI-J20s2-XE50',                     outputbits = 8 ),
                    TopoMenuDef( 'SC111-CJ15abpETA26',                       outputbits = 9 ),
                    TopoMenuDef( '0DETA20-J50s1-Js2',                        outputbits = 10 ),
                    TopoMenuDef( '2DISAMB-J25ab-0DR28-EM15his2-TAU12abi',    outputbits = 11 ),
                    TopoMenuDef( '0INVM9-EM7ab-EMab',                        outputbits = 12 ),
                    TopoMenuDef( '2DISAMB-TAU12abi-J25ab',                   outputbits = 13 ),
                    TopoMenuDef( '0DR04-MU4ab-CJ15ab',                       outputbits = 14 ),
                    TopoMenuDef( '0DR04-MU6ab-CJ20ab',                       outputbits = 15 ),
                ]
            },

            {
                "fpga" : 0,
                "clock" : 1,
                "algorithms" : [
                ]
            },

            {
                "fpga" : 1,
                "clock" : 0,
                "algorithms" : [
                    TopoMenuDef( 'DISAMB-30INVM-EM20his2-TAU12ab',       outputbits = 0 ),
                    TopoMenuDef( '100RATIO-0MATCH-TAU30si2-EMall',       outputbits = 1 ),
                    TopoMenuDef( 'NOT-0MATCH-TAU30si1-EMall',            outputbits = 2 ),
                    TopoMenuDef( 'LATE-MU10s1',                          outputbits = 3 ),
                    TopoMenuDef( 'MULT-CMU4ab',                          outputbits = (4,5), outputlines = ['MULT-CMU4ab[0]', 'MULT-CMU4ab[1]'] ), # 2bits
                    TopoMenuDef( 'MULT-CMU6ab',                          outputbits = (6,7), outputlines = ['MULT-CMU6ab[0]', 'MULT-CMU6ab[1]'] ), # 2bits
                    TopoMenuDef( '0DR15-2MU6ab',                         outputbits = 8 ),
                ]
            },

            {
                "fpga" : 1,
                "clock" : 1,
                "algorithms" : [
                    TopoMenuDef( '5DETA99-5DPHI99-2MU4ab',               outputbits = 0 ),
                    TopoMenuDef( '5DETA99-5DPHI99-MU6ab-MU4ab',          outputbits = 1 ), 
                    TopoMenuDef( '5DETA99-5DPHI99-2MU6ab',               outputbits = 2 ),
                    TopoMenuDef( '0DETA04-EM8abi-MU10ab',                outputbits = 3 ),
                    TopoMenuDef( '0DPHI03-EM8abi-MU10ab',                outputbits = 4 ),
                    TopoMenuDef( '0DETA04-EM15abi-MUab',                 outputbits = 5 ),
                    TopoMenuDef( '0DPHI03-EM15abi-MUab',                 outputbits = 6 ),
                    TopoMenuDef( '0INVM70-27DPHI32-EM10his1-EM10his6',   outputbits = 7 ),
                    TopoMenuDef( '0INVM70-27DPHI32-EM12his1-EM12his6',   outputbits = 8 ),
                    TopoMenuDef( '8INVM15-2CMU4ab',                      outputbits = 9 ),
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
            # we are still reflecting the old muon system, until the muon simulation is implemented.
            # the old one starts at bit 1 and has those 6 thresholds
            (None,1),
            'MU4', 'MU6', 'MU10', 'MU11', 'MU20', 'MU21',
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
                    TopoMenuDef( "0DR15-2MU6ab",            outputbits = 0 ),
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
                    (None,2), "ALFA4_B7R1L", "ALFA4_A7R1L", "ALFA4_A7L1L", "ALFA4_B7L1L"
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
