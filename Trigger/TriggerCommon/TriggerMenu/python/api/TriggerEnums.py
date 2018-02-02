# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
__author__  = 'Javier Montejo'
__version__="$Revision: 1.01 $"
__doc__="Enumerations for trigger types and periods"

from enum import IntEnum

class TriggerType(IntEnum):
    el_single  = 1 << 0
    el_multi   = 1 << 1
    mu_single  = 1 << 2
    mu_multi   = 1 << 3
    j_single   = 1 << 4
    j_multi    = 1 << 5
    bj_single  = 1 << 6
    bj_multi   = 1 << 7
    tau_single = 1 << 8
    tau_multi  = 1 << 9
    g_single   = 1 << 10
    g_multi    = 1 << 11
    xe         = 1 << 12
    ht         = 1 << 13
    mu_bphys   = 1 << 14
    exotics    = 1 << 15

    el          = el_single | el_multi
    mu          = mu_single | mu_multi
    j           = j_single  | j_multi
    bj          = bj_single | bj_multi
    tau         = tau_single| tau_multi
    g           = g_single  | g_multi

    ALL         = el | mu | j | bj | tau | g | xe | ht | mu_bphys | exotics
    UNDEFINED  = 0


class TriggerPeriod(IntEnum):
    y2015             = 1 << 0
    y2016periodAD3    = 1 << 1
    y2016periodD4plus = 1 << 2
    y2017periodB1     = 1 << 3
    y2017periodB2B4   = 1 << 4
    y2017periodB5B7   = 1 << 5
    y2017periodB8     = 1 << 6
    y2017periodC      = 1 << 7
    y2017periodD1D5   = 1 << 8
    y2017periodD6     = 1 << 9
    y2017periodE      = 1 << 10
    y2017periodF      = 1 << 11
    y2017periodGHI    = 1 << 12
    y2017periodK      = 1 << 13
    y2017periodN      = 1 << 14

    future1p8e34      = 1 << 16
    future2e34        = 1 << 17
    runNumber         = 1 << 18 #Can't get higher than this, enters the run number domain

    y2017periodB      = y2017periodB1   | y2017periodB2B4 | y2017periodB5B7 | y2017periodB8
    y2017periodD      = y2017periodD1D5 | y2017periodD6
    y2017periodAll    = y2017periodB    | y2017periodC    | y2017periodD    | y2017periodE | y2017periodF | y2017periodGHI | y2017periodK | y2017periodN
    y2017             = y2017periodAll
    y2016             = y2016periodAD3  | y2016periodD4plus
    future            = future1p8e34    | future2e34

class LBexceptions:
    ''' List of LBs to be skipped. 
        Usually due to accidental prescales that will be taken into account in future GRLs or via reduced luminosity.
        Users can modify this if needed, but hopefully there is no need ;)
    '''
    # Run number: [(LB start, LB end), ...] both inclusive
    exceptions = {
       276262: [(72 , 75 )], #toroid off keys 
       281317: [(116, 118)], #toroid off keys 
       299055: [(650, 700)], #Toroid was going down, turned muons off
       301932: [(233, 234)], #Accidentaly moved to MuScan prescales
       302831: [(4  , 10 )], #toroid off keys 
       336506: [(212, 260)], #Regular muscan but the defect is not in sync with the switch of keys
       }
