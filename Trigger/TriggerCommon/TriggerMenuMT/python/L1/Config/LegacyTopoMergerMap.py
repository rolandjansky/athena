# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from collections import namedtuple, OrderedDict as odict

from AthenaCommon.Logging import logging
log = logging.getLogger("L1.Config.LegacyTopoMergerMap")

MC = namedtuple('MC', "target_bit, topo_board, source_bit, signals")

# the following mapping is hardcoded to match the hardwired legacy L1Topo
# merger board based on specifications from Stefan Haas
__mapping = [
    MC(target_bit =  0, topo_board = 0, source_bit =  0, signals = ( "900INVM9999-AJ30s6-AJ20s6", None )),
    MC(target_bit =  1, topo_board = 0, source_bit =  1, signals = ( "800INVM9999-AJ30s6-AJ20s6", None )),
    MC(target_bit =  2, topo_board = 0, source_bit =  2, signals = ( "700INVM9999-AJ30s6-AJ20s6", None )),
    MC(target_bit =  3, topo_board = 0, source_bit =  3, signals = ( "500INVM9999-AJ30s6-AJ20s6", None )),
    MC(target_bit =  4, topo_board = 0, source_bit =  4, signals = ( "400INVM9999-AJ30s6-AJ20s6", "0DR03-EM7ab-CJ15ab" )),
    MC(target_bit =  5, topo_board = 0, source_bit =  5, signals = ( "300INVM9999-AJ30s6-AJ20s6", "10MINDPHI-J20s2-XE30" )),
    MC(target_bit =  6, topo_board = 0, source_bit =  6, signals = ( "200INVM9999-AJ30s6-AJ20s6", "10MINDPHI-J20s2-XE50" )),
    MC(target_bit =  7, topo_board = 0, source_bit =  7, signals = ( "100INVM9999-AJ30s6-AJ20s6", "100RATIO-0MATCH-TAU30si2-EMall" )),
    MC(target_bit =  8, topo_board = 0, source_bit =  8, signals = ( "0INVM9-EM7ab-EMab", "NOT-0MATCH-TAU30si1-EMall" )),
    MC(target_bit =  9, topo_board = 0, source_bit =  9, signals = ( "HT150-J20s5pETA31", None )),
    MC(target_bit = 10, topo_board = 0, source_bit = 10, signals = ( "HT190-J15s5pETA21", None )),
    MC(target_bit = 11, topo_board = 0, source_bit = 11, signals = ( "1INVM5-EMs1-EMs6", None )),
    MC(target_bit = 12, topo_board = 0, source_bit = 12, signals = ( "1INVM5-EM7s1-EMs6", "27DPHI32-EMs1-EMs6" )),
    MC(target_bit = 13, topo_board = 0, source_bit = 13, signals = ( "1INVM5-EM12s1-EMs6", None )),
    MC(target_bit = 14, topo_board = 0, source_bit = 14, signals = ( None, None )),
    MC(target_bit = 15, topo_board = 0, source_bit = 15, signals = ( "400INVM9999-AJ30s6pETA31-AJ20s6p31ETA49", None )),
    MC(target_bit = 16, topo_board = 0, source_bit = 16, signals = ( None, "DISAMB-30INVM-EM20his2-TAU12ab" )),
    MC(target_bit = 17, topo_board = 0, source_bit = 17, signals = ( None, None )),
    MC(target_bit = 18, topo_board = 0, source_bit = 18, signals = ( None, None )),
    MC(target_bit = 19, topo_board = 0, source_bit = 19, signals = ( "1DISAMB-J25ab-0DR28-EM15his2-TAU12abi", None)),
    MC(target_bit = 20, topo_board = 0, source_bit = 28, signals = ( None, "600INVM9999-J30s6-AJ20s6" )),
    MC(target_bit = 21, topo_board = 0, source_bit = 29, signals = ( None, "500INVM9999-J30s6-AJ20s6" )),
    MC(target_bit = 22, topo_board = 0, source_bit = 30, signals = ( None, "400INVM9999-J30s6-AJ20s6" )),
    MC(target_bit = 23, topo_board = 0, source_bit = 31, signals = ( None, "200INVM9999-J30s6-AJ20s6" )),
    MC(target_bit = 24, topo_board = 1, source_bit = 10, signals = ( None, None )),
    MC(target_bit = 25, topo_board = 1, source_bit = 11, signals = ( None, "SC111-CJ15abpETA26" )),
    MC(target_bit = 26, topo_board = 1, source_bit = 18, signals = ( None, "0DR28-TAU20abi-TAU12abi" )),
    MC(target_bit = 27, topo_board = 1, source_bit = 19, signals = ( None, "1DISAMB-J25ab-0DR28-TAU20abi-TAU12abi" )),
    MC(target_bit = 28, topo_board = 1, source_bit = 20, signals = ( None, "1DISAMB-TAU12abi-J25ab" )),
    MC(target_bit = 29, topo_board = 1, source_bit = 21, signals = ( None, None )),
    MC(target_bit = 30, topo_board = 1, source_bit = 28, signals = ( "0DETA20-J50s1-Js2", None )),
    MC(target_bit = 31, topo_board = 1, source_bit = 29, signals = ( None, None )),
]


def createMergerBoard(legacyBoard0, legacyBoard1):

    legacyOccupation = [[32*[None],32*[None]],[32*[None],32*[None]]]
    for boardNumber, board in enumerate([legacyBoard0, legacyBoard1]):
        for conn in board["connectors"][0]["algorithmGroups"]:
            fpga = conn['fpga']
            clock = conn['clock']
            for topoAlgDef in conn['algorithms']:
                lines = topoAlgDef.outputlines
                if len(lines)==1:
                    legacyOccupation[boardNumber][clock][ 16*fpga + topoAlgDef.outputbits] = lines[0]
                else:
                    for bit,line in enumerate(lines):
                        legacyOccupation[boardNumber][clock][ 16*fpga + bit + topoAlgDef.outputbits[0]] = line

    signals = [32*[None], 32*[None]]
    for mc in __mapping:
        for clock in [0,1]:
            algFromMapping = mc.signals[clock]
            if algFromMapping is None:
                continue
            signals[clock][mc.target_bit] = algFromMapping
            algFromLegacyBoardDef = legacyOccupation[mc.topo_board][clock][mc.source_bit]
            if algFromMapping != algFromLegacyBoardDef:
                fpga = mc.source_bit // 16
                bit = mc.source_bit % 16
                msg = f"Legacy board mapping expects line {algFromMapping} on LegacyBoard{mc.topo_board}, fpga {fpga}, " +\
                      f"clock {clock}, bit {bit} but board defines {algFromLegacyBoardDef}"
                log.error(msg)
                raise RuntimeError("Mapping doesn't match LegacyBoard definition")




    board = odict()
    board["connectors"] = []
    board["type"] = "merger"
    board["connectors"].append({
        "name" : "LegacyTopoMerged",
        "format" : "simple",
        "nbitsDefault" : 1,
        "type" : "electrical",
        "legacy" : False,
        "signalGroups" : [
            {
                "clock" : 0,
                "signals" : signals[0]
            },
            {
                "clock" : 1,
                "signals" : signals[1]
            }
        ]
    })
    return odict( [("LegacyTopoMerger", board)] )
