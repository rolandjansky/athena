# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

"""
This list defines the set of legacy L1Calo thresholds to be used at the beginning of Run3 (ATR-23241)

"""

legacyThresholds = [

    # multiplicities
    "EM3", "EM7", "EM8VH", "EM10VH", "EM12", "EM15", "EM15VH", "EM15VHI", "EM18VHI", "EM20VH", "EM20VHI", "EM22VH", "EM22VHI", "EM24VHI",

    "HA8", "HA12IM", "HA20IM", "HA25IM", "HA40", "HA60", "HA100",

    "J12", "J12p0ETA25", "J15", "J15p0ETA25", "J15p31ETA49", "J20", "J20p31ETA49", "J25", "J25p0ETA23", "J30", "J30p31ETA49", "J35p0ETA23",
    "J40", "J40p0ETA25",  "J45p0ETA21", "J50", "J50p31ETA49", "J75", "J75p31ETA49", "J85", "J100", "J120", "J400", 

    "TE5", "TE10", "TE20", "TE50",

    "XE30", "XE35", "XE40", "XE45", "XE50", "XE55", "XE60", "XE300",

    # R2 TOPO
    "R2TOPO_0DETA20-J50s1-Js2", 
    "R2TOPO_0DR03-EM7ab-CJ15ab", 
    "R2TOPO_0DR28-TAU20abi-TAU12abi", 
    "R2TOPO_0INVM9-EM7ab-EMab",
    "R2TOPO_100RATIO-0MATCH-TAU30si2-EMall",
    "R2TOPO_1DISAMB-J25ab-0DR28-TAU20abi-TAU12abi",
    "R2TOPO_1INVM5-EM12s1-EMs6",
    "R2TOPO_1INVM5-EM7s1-EMs6",
    "R2TOPO_27DPHI32-EMs1-EMs6",
    "R2TOPO_300INVM9999-AJ30s6-AJ20s6",
    "R2TOPO_400INVM9999-AJ30s6pETA31-AJ20s6p31ETA49",
    "R2TOPO_500INVM9999-J30s6-AJ20s6",
    "R2TOPO_700INVM9999-AJ30s6-AJ20s6",
    "R2TOPO_HT150-J20s5pETA31",
    "R2TOPO_HT190-J15s5pETA21",
    "R2TOPO_NOT-0MATCH-TAU30si1-EMall",
    "R2TOPO_SC111-CJ15abpETA26",
    "R2TOPO_ZEE-EM20shi2",

    # detector thresholds
    "NIMLHCF", "NIMTGC", "NIMTRT", 
    "MBTS_A", "MBTS_C", 
    "CAL1", "CAL2", 
    "BCM_AtoC", "BCM_CtoA", "BCM_Wide", "BCM_Comb",
    "BPTX0", "BPTX1",
    "LUCID_A", "LUCID_C",
    "ZDC_0", "ZDC_1", "ZDC_2",
    "AFP_NSA", "AFP_FSA", "AFP_FSA_TOF_T0", "AFP_FSA_TOF_T1", "AFP_FSA_TOF_T2", "AFP_FSA_TOF_T3",
    "AFP_NSC", "AFP_FSC", "AFP_FSC_TOF_T0", "AFP_FSC_TOF_T1", "AFP_FSC_TOF_T2", "AFP_FSC_TOF_T3",

]
