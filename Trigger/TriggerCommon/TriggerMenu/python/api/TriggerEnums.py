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
    afp        = 1 << 16

    el          = el_single | el_multi
    mu          = mu_single | mu_multi
    j           = j_single  | j_multi
    bj          = bj_single | bj_multi
    tau         = tau_single| tau_multi
    g           = g_single  | g_multi

    ALL         = el | mu | j | bj | tau | g | xe | ht | mu_bphys | exotics | afp
    UNDEFINED  = 0


class TriggerPeriod(IntEnum):
    y2015             = 1 << 0
    y2016periodA      = 1 << 1
    y2016periodBD3    = 1 << 2
    y2016periodD4plus = 1 << 3
    y2017periodB1     = 1 << 4
    y2017periodB2B4   = 1 << 5
    y2017periodB5B7   = 1 << 6
    y2017periodB8     = 1 << 7
    y2017periodC      = 1 << 8
    y2017periodD1D5   = 1 << 9
    y2017periodD6     = 1 << 10
    y2017periodEF     = 1 << 11
    y2017periodGHIK   = 1 << 12
    y2017lowmu        = 1 << 13
    y2018periodBE     = 1 << 14
    y2018periodFI     = 1 << 15
    y2018lowmu        = 1 << 16
    y2018periodKQ     = 1 << 17

    runNumber         = 1 << 18 #Can't get higher than this, enters the run number domain
    future1p8e34      = 1 << 19 
    future2e34        = 1 << 20

    y2017periodB      = y2017periodB1   | y2017periodB2B4 | y2017periodB5B7 | y2017periodB8
    y2017periodD      = y2017periodD1D5 | y2017periodD6
    y2017periodAll    = y2017periodB    | y2017periodC    | y2017periodD    | y2017periodEF | y2017periodGHIK #low-mu period is not considered 
    y2018             = y2018periodBE   | y2018periodFI   | y2018periodKQ  #low-mu period is not considered 
    y2017             = y2017periodAll
    y2016             = y2016periodA    | y2016periodBD3  | y2016periodD4plus
    future            = future1p8e34    | future2e34

    def isBasePeriod(self):
        return TriggerPeriod.isRunNumber(self) or all([self & x == 0 or self & x == self for x in TriggerPeriod])

    @classmethod
    def isRunNumber(cls, number):
        return (number >= TriggerPeriod.runNumber and number < TriggerPeriod.runNumber*2)

    @classmethod
    def basePeriods(cls):
        return [x for x in TriggerPeriod if x.isBasePeriod()]

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
       341294: [(137, 156)], #Standby keys
       355650: [(117, 117)], #Last LB of emittance scan
       357283: [(117, 117)], #Last LB of emittance scan
       359623: [(129, 129)], #Last LB of emittance scan
       }

class TriggerRenaming:
    ''' Pairs of triggers that have been renamed at some point
        The code will complete each other so that each contains
        luminosity of both
        Assumes that they are orthogonal, i.e. they haven't run both at the same time
    '''
    pairs = [
        ("HLT_mu20_mu8noL1_e9_lhvloose_nod0","HLT_e9_lhvloose_nod0_mu20_mu8noL1"),
        ("HLT_mu20_mu8noL1_e9_lhvloose_nod0_L1EM8VH_MU20","HLT_e9_lhvloose_nod0_mu20_mu8noL1_L1EM8VH_MU20"),
        ("HLT_mu20_mu8noL1_e9_lhvloose_nod0_L1EM7_MU20","HLT_e9_lhvloose_nod0_mu20_mu8noL1_L1EM7_MU20"),
    ]

if __name__ == "__main__":
   print TriggerPeriod.y2015.isBasePeriod() 
   print TriggerPeriod.y2017.isBasePeriod() 
   print TriggerPeriod.basePeriods() 
