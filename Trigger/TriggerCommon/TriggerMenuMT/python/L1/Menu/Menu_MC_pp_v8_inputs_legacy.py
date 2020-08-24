# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from collections import OrderedDict as odict

from ..Base.L1MenuFlags import L1MenuFlags
from ..Base.MenuConfObj import TopoMenuDef

def defineLegacyInputsMenu():

    legacyBoards = odict()
    legacyTopoBoards = odict()

    #----------------------------------------
    # SLOT 7 / CON 0-3 (EM1, EM2, TAU1, TAU2)
    #----------------------------------------
    legacyBoards["Ctpin7"] = odict()
    legacyBoards["Ctpin7"]["legacy"] = True
    legacyBoards["Ctpin7"]["connectors"] = []
    legacyBoards["Ctpin7"]["connectors"] += [
        {
            "name" : "EM1",
            "format" : "multiplicity",
            "nbitsDefault" : 3,
            "type" : "ctpin",
            "legacy" : True,
            "thresholds" : [ 'EM3', 'EM7', 'EM8VH', 'EM8I', 'EM10VH', 'EM12', 'EM15', 'EM15VH'  ],
            "zeroBias" : "ZB_EM15"
        },
        {
            "name" : "EM2",
            "format" : "multiplicity",
            "nbitsDefault" : 3,
            "type" : "ctpin",
            "legacy" : True,
            "thresholds" : [ 'EM15VHI', 'EM18VHI', 'EM20VH', 'EM20VHI', 'EM22VH', 'EM22VHI', 'EM24VHI', 'EM24VHIM' ]
        },
        {
            "name" : "TAU1",
            "format" : "multiplicity",
            "nbitsDefault" : 3,
            "type" : "ctpin",
            "legacy" : True,
            "thresholds" : [ 'HA8', 'HA12', 'HA12IL', 'HA12IM', 'HA12IT', 'HA15', 'HA20', 'HA20IL' ]
        },
        
        {
            "name" : "TAU2",
            "format" : "multiplicity",
            "nbitsDefault" : 3,
            "type" : "ctpin",
            "legacy" : True,
            "thresholds" : [ 'HA20IM', 'HA20IT', 'HA25', 'HA25IT', 'HA30', 'HA40', 'HA60', 'HA100' ]
        }
    ]


    #--------------------------------------
    # SLOT 8 / CON 0 (JET1, JET2, EN1, EN2)
    #--------------------------------------
    legacyBoards["Ctpin8"] = odict()
    legacyBoards["Ctpin8"]["legacy"] = True
    legacyBoards["Ctpin8"]["connectors"] = []
    legacyBoards["Ctpin8"]["connectors"] += [
        {
            # 10 x 3-bit JET (can have multiplicity 4 or more)
            "name" : "JET1",
            "format" : "multiplicity",
            "nbitsDefault" : 3,
            "type" : "ctpin",
            "legacy" : True,
            "thresholds" : [
                'J12', 'J15', 'J15.0ETA25','J20', 'J25','J25.0ETA23','J30', 'J40', # 8 x JETs and central jets
                'J20.0ETA49', 'J30.0ETA49', # 2 x VBF
            ]
        },
        {
            # 15 x 2-bit JET (can have maximum multiplicity of 3) (SLOT 8, CON 1)
            "name" : "JET2",
            "format" : "multiplicity",
            "nbitsDefault" : 2,
            "type" : "ctpin",
            "legacy" : True,
            "thresholds" : [
                'J35.0ETA23','J40.0ETA25', 'J45.0ETA20', # 3 x Central Jet
                'J50', 'J75', 'J85', 'J100', 'J120', 'J400', # 6 Jets
                'J15.31ETA49', 'J20.31ETA49', 'J30.31ETA49', 'J50.31ETA49', 'J75.31ETA49', 'J12.0ETA23', # 6 x FJ              
            ]
        },
        {
            # 24 x 1-bit thresholds
            "name" : "EN1",
            "format" : "multiplicity",
            "nbitsDefault" : 1,
            "type" : "ctpin",
            "legacy" : True,
            "thresholds" : [
                'TE5', 'TE10', 'TE15', 'TE20', 'TE25', 'TE30', 'TE40', 'TE50', # 8 x TE
                'XE10', 'XE20', 'XE25', 'XE30', 'XE35', 'XE40', 'XE45', 'XE50', # 8 x XE
                'XS20', 'XS30', 'XS40', 'XS45', 'XS50', 'XS55', 'XS60', 'XS65',  # 8 x XS
            ]
        },
        {
            # 8 x 1-bit thresholds
            "name" : "EN2",
            "format" : "multiplicity",
            "nbitsDefault" : 1,
            "type" : "ctpin",
            "legacy" : True,
            "thresholds" : [
                'TE5.0ETA24', 'TE10.0ETA24', 'TE15.0ETA24', 'TE20.0ETA24', 'TE25.0ETA24', 'TE30.0ETA24', 'TE40.0ETA24', 'TE70.0ETA24', # 8 x TE
                'XE55', 'XE60', 'XE65', 'XE70', 'XE75', 'XE80', 'XE150', 'XE300', # 8 x XE
            ]
        }
    ]


    #----------------
    # Legacy L1TOPO 0  
    #----------------
    legacyTopoBoards["LegacyTopo0"] = odict()
    legacyTopoBoards["LegacyTopo0"]["legacy"] = True
    legacyTopoBoards["LegacyTopo0"]["connectors"] = [
        {
            "name" : "LegacyTopo0",
            "format" : "topological",
            "type" : "electrical",
            "legacy" : True,
            "algorithmGroups" : [
                {
                "fpga" : 0,
                    "clock" : 0,
                    "algorithms" : [
                        TopoMenuDef( "INVM_AJ_HighMass",     outputbits = (0,3), outputlines = [ "900INVM9999-AJ30s6-AJ20s6",
                                                                                                 "800INVM9999-AJ30s6-AJ20s6",
                                                                                                 "700INVM9999-AJ30s6-AJ20s6",
                                                                                                 "500INVM9999-AJ30s6-AJ20s6" ] ),
                        TopoMenuDef( "INVM_AJ_LowMass",      outputbits = (4,7), outputlines = [ "400INVM9999-AJ30s6-AJ20s6",
                                                                                                 "300INVM9999-AJ30s6-AJ20s6",
                                                                                                 "200INVM9999-AJ30s6-AJ20s6",
                                                                                                 "100INVM9999-AJ30s6-AJ20s6" ] ),
                        TopoMenuDef( "0INVM9-EM7ab-EMab",    outputbits = 8 ),
                        TopoMenuDef( "HT150-J20s5.ETA31",    outputbits = 9 ),
                        TopoMenuDef( "HT190-J15s5.ETA21",    outputbits = 10 ),
                        TopoMenuDef( "INVM_EMs6",            outputbits = (11,13), outputlines = [ "1INVM5-EMs1-EMs6",
                                                                                                   "1INVM5-EM7s1-EMs6",
                                                                                                   "1INVM5-EM12s1-EMs6" ] ),
                        TopoMenuDef( "05MINDPHI-EM12s6-XE0", outputbits = 14 ),
                        TopoMenuDef( "400INVM9999-AJ30s6.ETA31-AJ20s6.31ETA49", outputbits = 15 ),
                    ]
            },
            {
                "fpga" : 0,
                "clock" : 1,
                "algorithms" : [
                    TopoMenuDef( "05MINDPHI-EM15s6-XE0",              outputbits = 0 ),
                    TopoMenuDef( "25MT-EM12s6-XE0",                   outputbits = 1 ),
                    TopoMenuDef( "ZEE-EM20shi2",                      outputbits = 2 ),
                    TopoMenuDef( "35MT-EM15s6-XE0",                   outputbits = 3 ),
                    TopoMenuDef( "0DR03-EM7ab-CJ15ab",                outputbits = 4 ),
                    TopoMenuDef( "10MINDPHI-J20s2-XE30",              outputbits = 5 ),
                    TopoMenuDef( "10MINDPHI-J20s2-XE50",              outputbits = 6 ),
                    TopoMenuDef( "100RATIO-0MATCH-TAU30si2-EMall",    outputbits = 7 ),
                    TopoMenuDef( "NOT-0MATCH-TAU30si1-EMall",         outputbits = 8 ),
                    TopoMenuDef( "LAR-EM20shi1",                      outputbits = 9 ),
                    TopoMenuDef( "LAR-J100s1",                        outputbits = 10 ),
                    TopoMenuDef( "NOT-02MATCH-EM10s1-AJj15all.ETA49", outputbits = 11 ),
                    TopoMenuDef( "27DPHI32-EMs1-EMs6",                outputbits = 12 ),
                    TopoMenuDef( "35MT-EM12s6-XE0",                   outputbits = 13 ),
                    TopoMenuDef( "15MINDPHI-EM12s6-XE0",              outputbits = 14 ),
                    TopoMenuDef( "15MINDPHI-EM15s6-XE0",              outputbits = 15 ),
                ]
            },
            {
                "fpga" : 1,
                "clock" : 0,
                "algorithms" : [
                    TopoMenuDef( "1DISAMB-EM15his2-TAU12abi-J25ab",       outputbits = 2 ),
                    TopoMenuDef( "1DISAMB-J25ab-0DR28-EM15his2-TAU12abi", outputbits = 3 ),
                    TopoMenuDef( "2INVM9-2MU6ab",                         outputbits = 4 ),
                    TopoMenuDef( "2INVM8-ONEBARREL-MU6ab-MU4ab",          outputbits = 6 ),
                    TopoMenuDef( "5DETA99-5DPHI99-MU6ab-MU4ab",           outputbits = 8 ),
                    TopoMenuDef( "5DETA99-5DPHI99-2MU6ab",                outputbits = 9 ),
                    TopoMenuDef( "1DISAMB-TAU20abi-TAU12abi-J25ab",       outputbits = 10 ),
                    TopoMenuDef( "0DR28-MU10ab-TAU12abi",                 outputbits = 11 ),
                    TopoMenuDef( "0DETA20-0DPHI20-TAU20abi-TAU12abi",     outputbits = 12 ),
                    TopoMenuDef( "DISAMB-0DR28-EM15his2-TAU12abi",        outputbits = 15 ),
                ]
            },
            {
                "fpga" : 1,
                "clock" : 1,
                "algorithms" : [
                    TopoMenuDef( "DISAMB-30INVM-EM20his2-TAU12ab", outputbits = 0 ),
                    TopoMenuDef( "0DR22-2MU6ab",                   outputbits = 4 ),
                    TopoMenuDef( "7INVM15-2MU4ab",                 outputbits = 5 ),
                    TopoMenuDef( "0DR22-MU6ab-MU4ab",              outputbits = 6 ),
                    TopoMenuDef( "0DR15-2MU4ab",                   outputbits = 7 ),
                    TopoMenuDef( "0DR24-2MU4ab",                   outputbits = 8 ),
                    TopoMenuDef( "0DR15-2MU6ab",                   outputbits = 9 ),
                    TopoMenuDef( "2INVM9-2MU4ab",                  outputbits = 10 ),
                    TopoMenuDef( "2INVM9-MU6ab-MU4ab",             outputbits = 11 ),
                    TopoMenuDef( "INVM_NFF",                       outputbits = (12,15), outputlines = [ "600INVM9999-J30s6-AJ20s6", 
                                                                                                         "500INVM9999-J30s6-AJ20s6",
                                                                                                         "400INVM9999-J30s6-AJ20s6",
                                                                                                         "200INVM9999-J30s6-AJ20s6" ])
                ]
            }
        ]
    }]

    legacyTopoBoards["LegacyTopo1"] = odict()
    legacyTopoBoards["LegacyTopo1"]["legacy"] = True
    legacyTopoBoards["LegacyTopo1"]["connectors"] = [
        {
            "name" : "LegacyTopo1",
            "format" : "topological",
            "type" : "electrical",
            "legacy" : True,
            "algorithmGroups" : [
                {
                "fpga" : 0,
                    "clock" : 0,
                    "algorithms" : [
                        TopoMenuDef( "05MINDPHI-AJj10s6-XE0", outputbits = 0  ),
                        TopoMenuDef( "10MINDPHI-AJj10s6-XE0", outputbits = 1  ),
                        TopoMenuDef( "15MINDPHI-AJj10s6-XE0", outputbits = 2  ),
                        TopoMenuDef( "0DR04-MU4ab-CJ15ab",    outputbits = 3  ),
                        TopoMenuDef( "0DR04-MU4ab-CJ20ab",    outputbits = 4  ),
                        TopoMenuDef( "0DR04-MU4ab-CJ30ab",    outputbits = 5  ),
                        TopoMenuDef( "0DR04-MU6ab-CJ20ab",    outputbits = 6  ),
                        TopoMenuDef( "0DR04-MU6ab-CJ25ab",    outputbits = 7  ),
                        TopoMenuDef( "10MINDPHI-CJ20ab-XE50", outputbits = 8  ),
                        TopoMenuDef( "0DR24-2CMU4ab",         outputbits = 9  ),
                    ]
                },
                {
                    "fpga" : 0,
                    "clock" : 1,
                    "algorithms" : [
                        TopoMenuDef( "0DETA04-EM8abi-MU10ab", outputbits = 3  ),
                        TopoMenuDef( "0DETA04-EM15abi-MUab",  outputbits = 4  ),
                        TopoMenuDef( "0DR24-CMU4ab-MU4ab",    outputbits = 5  ),
                        TopoMenuDef( "0DPHI03-EM8abi-MU10ab", outputbits = 6  ),
                        TopoMenuDef( "2INVM8-CMU4ab-MU4ab",   outputbits = 7  ),
                        TopoMenuDef( "0DPHI03-EM15abi-MUab",  outputbits = 8  ),
                        TopoMenuDef( "10MINDPHI-AJ20s2-XE50", outputbits = 9  ),
                        TopoMenuDef( "LATE-MU10s1",           outputbits = 10 ),
                        TopoMenuDef( "SC111-CJ15ab.ETA26",    outputbits = 11 ),
                        TopoMenuDef( "SC85-CJ15ab.ETA26",     outputbits = 12 ),
                    ]
                },
                {
                    "fpga" : 1,
                    "clock" : 0,
                    "algorithms" : [
                        TopoMenuDef( "KF-XE-AJall",                     outputbits = (0,5),outputlines = [ "KF-XE40-AJall",
                                                                                                           "KF-XE50-AJall", 
                                                                                                           "KF-XE55-AJall",
                                                                                                           "KF-XE60-AJall",
                                                                                                           "KF-XE65-AJall",
                                                                                                           "KF-XE75-AJall" ] ),
                        TopoMenuDef( "0MATCH-4AJ20.ETA31-4AJj15.ETA31", outputbits = 6  ),
                        TopoMenuDef( "HT190-AJ15all.ETA21",             outputbits = 7  ),
                        TopoMenuDef( "HT150-AJ20all.ETA31",             outputbits = 8  ),
                        TopoMenuDef( "HT150-AJj15all.ETA49",            outputbits = 9  ),
                        TopoMenuDef( "HT20-AJj15all.ETA49",             outputbits = 10 ),
                        TopoMenuDef( "0DETA20-J50s1-Js2",               outputbits = 12 ),
                        TopoMenuDef( "05RATIO-XE0-HT0-AJj15all.ETA49",  outputbits = 13 ),
                        TopoMenuDef( "63DETA127-FJ20s1-FJ20s2",         outputbits = 14 ),
                        TopoMenuDef( "90RATIO2-XE0-HT0-AJj15all.ETA49", outputbits = 15 )
                    ]
                },
                {
                    "fpga" : 1,
                    "clock" : 1,
                    "algorithms" : [
                        TopoMenuDef( "250RATIO2-XE0-HT0-AJj15all.ETA49",      outputbits = 0 ),
                        TopoMenuDef( "10MINDPHI-J20ab-XE50",                  outputbits = 1 ),
                        TopoMenuDef( "0DR28-TAU20abi-TAU12abi",               outputbits = 2 ),
                        TopoMenuDef( "1DISAMB-J25ab-0DR28-TAU20abi-TAU12abi", outputbits = 3 ),
                        TopoMenuDef( "1DISAMB-TAU12abi-J25ab",                outputbits = 4 ),
                        TopoMenuDef( "0DR10-MU10ab-MU6ab",                    outputbits = 5 ),
                        TopoMenuDef( "2DR99-2MU4ab",                          outputbits = 6 ),
                        TopoMenuDef( "0DR34-2MU4ab",                          outputbits = 7 ),
                        TopoMenuDef( "2DR15-2MU6ab",                          outputbits = 8 ),
                        TopoMenuDef( "0DR15-MU6ab-MU4ab",                     outputbits = 9 ),
                        TopoMenuDef( "0DR25-TAU20abi-TAU12abi",               outputbits = 10 ),
                        TopoMenuDef( "1DISAMB-J25ab-0DR25-TAU20abi-TAU12abi", outputbits = 11 ),
                        TopoMenuDef( "8INVM15-MU6ab-MU4ab",                   outputbits = 12 ),
                        TopoMenuDef( "8INVM15-2MU6ab",                        outputbits = 13 ),
                        TopoMenuDef( "2INVM8-2MU4ab",                         outputbits = 14 ),
                        TopoMenuDef( "2INVM8-MU6ab-MU4ab",                    outputbits = 15 ),
                    ]
                }
            ]
        }
    ]


    L1MenuFlags.legacyBoards().clear()

    L1MenuFlags.legacyBoards().update( legacyBoards )  # EM1/2, TAU1/2, JET1/2, EN1/2

    L1MenuFlags.legacyBoards().update( legacyTopoBoards) # LegacyTopo0/1

