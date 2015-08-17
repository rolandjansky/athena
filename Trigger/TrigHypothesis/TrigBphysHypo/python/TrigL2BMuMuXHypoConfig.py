# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
from TrigBphysHypo.TrigBphysHypoConf import TrigL2BMuMuXHypo


class L2BMuMuXHypo_1 (TrigL2BMuMuXHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuXHypo_1"):
        super( TrigL2BMuMuXHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # mass cuts
        # B+
        self.LowerBMassCut = 4400.
        self.UpperBMassCut = 5800.
        # Bd
        self.LowerBdMassCut = 4600.
        self.UpperBdMassCut = 5900.
        self.LowerKstarMassCut = 700.
        self.UpperKstarMassCut = 1100.
        # Bs
        self.LowerBsMassCut = 4700.
        self.UpperBsMassCut = 5800.
        self.LowerPhi1020MassCut = 940.
        self.UpperPhi1020MassCut = 1100.
        # Lb
        self.LowerLbMassCut = 5100.
        self.UpperLbMassCut = 6100.
        self.LowerLambdaMassCut = 1040.
        self.UpperLambdaMassCut = 1200.
        # Bc
        self.LowerBcMassCut = 1800.
        self.UpperBcMassCut = 7100.
        self.LowerDsMassCut = 1600.
        self.UpperDsMassCut = 2400.


        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")


class L2BMuMuXHypo_EF (TrigL2BMuMuXHypo):
    __slots__ = []
    def __init__(self, name = "L2BMuMuXHypo_EF"):
        super( TrigL2BMuMuXHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # mass cuts
        # B+
        self.LowerBMassCut = 4400.
        self.UpperBMassCut = 5900.
        # Bd
        self.LowerBdMassCut = 4500.
        self.UpperBdMassCut = 6000.
        self.LowerKstarMassCut = 650.
        self.UpperKstarMassCut = 1150.
        # Bs
        self.LowerBsMassCut = 4700.
        self.UpperBsMassCut = 6000.
        self.LowerPhi1020MassCut = 900.
        self.UpperPhi1020MassCut = 1140.
        # Lb
        self.LowerLbMassCut = 5000.
        self.UpperLbMassCut = 6200.
        self.LowerLambdaMassCut = 1000.
        self.UpperLambdaMassCut = 1240.
        # Bc
        self.LowerBcMassCut = 1800.
        self.UpperBcMassCut = 7050.
        self.LowerDsMassCut = 1600.
        self.UpperDsMassCut = 2400.


        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
