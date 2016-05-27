# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# vim: tabstop=4:shiftwidth=4:expandtab
from TrigBphysHypo.TrigBphysHypoConf import TrigEFBMuMuXHypo


class EFBMuMuXHypo_1 (TrigEFBMuMuXHypo):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXHypo_1"):
        super( TrigEFBMuMuXHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # decay modes flags
        self.AcceptBplus = True
        self.AcceptBd = True
        self.AcceptBs = True
        self.AcceptLb = True
        self.AcceptBc = False
        # mass cuts
        # B+
        self.LowerBplusMassCut = 4400.
        self.UpperBplusMassCut = 5800.
        self.BplusChi2Cut = 50.
        # Bd
        self.LowerBdMassCut = 4600.
        self.UpperBdMassCut = 5900.
        self.BdChi2Cut = 60.
        self.LowerKstarMassCut = 700.
        self.UpperKstarMassCut = 1100.
        self.KstarChi2Cut = 400.
        # Bs
        self.LowerBsMassCut = 4800.
        self.UpperBsMassCut = 5800.
        self.BsChi2Cut = 60.
        self.LowerPhi1020MassCut = 940.
        self.UpperPhi1020MassCut = 1100.
        self.Phi1020Chi2Cut = 400.
        # Lb
        self.LowerLbMassCut = 5100.
        self.UpperLbMassCut = 6100.
        self.LbChi2Cut = 100.
        self.LowerLambdaMassCut = 1040.
        self.UpperLambdaMassCut = 1200.
        self.LambdaChi2Cut = 400.
        # Bc
        self.LowerBcMassCut = 5450.
        self.UpperBcMassCut = 7050.
        self.BcChi2Cut = 120.
        self.LowerDsMassCut = 1600.
        self.UpperDsMassCut = 2400.
        self.DsChi2Cut = 90.


        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        

class EFBMuMuXHypo_allModes (EFBMuMuXHypo_1):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXHypo_allModes"):
        EFBMuMuXHypo_1.__init__(self, name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # decay modes flags
        self.AcceptBplus = True
        self.AcceptBd = True
        self.AcceptBs = True
        self.AcceptLb = True
        self.AcceptBc = True
        
class EFBMuMuXHypo_FTK (EFBMuMuXHypo_1):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXHypo_FTK"):
        EFBMuMuXHypo_1.__init__(self, name )

        
class EFBMuMuXHypo_allModes_tightBc (EFBMuMuXHypo_allModes):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXHypo_allModes_tightBc"):
        EFBMuMuXHypo_allModes.__init__(self, name )

        # Bc
        self.LowerBcMassCut = 5450.
        
        
class EFBMuMuXHypo_BplusMuMuKplus (EFBMuMuXHypo_1):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXHypo_BplusMuMuKplus"):
        EFBMuMuXHypo_1.__init__(self, name)
        #super( TrigEFBMuMuXHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # decay modes flags
        self.AcceptBplus = True
        self.AcceptBd = False
        self.AcceptBs = False
        self.AcceptLb = False
        self.AcceptBc = False
        
class EFBMuMuXHypo_BdMuMuKstar (EFBMuMuXHypo_1):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXHypo_BdMuMuKstar"):
        EFBMuMuXHypo_1.__init__(self, name)
        #super( TrigEFBMuMuXHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # decay modes flags
        self.AcceptBplus = False
        self.AcceptBd = True
        self.AcceptBs = False
        self.AcceptLb = False
        self.AcceptBc = False
        
class EFBMuMuXHypo_BsMuMuPhi (EFBMuMuXHypo_1):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXHypo_BsMuMuPhi"):
        EFBMuMuXHypo_1.__init__(self, name)
        #super( TrigEFBMuMuXHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # decay modes flags
        self.AcceptBplus = False
        self.AcceptBd = False
        self.AcceptBs = True
        self.AcceptLb = False
        self.AcceptBc = False
        
class EFBMuMuXHypo_LbMuMuLambda (EFBMuMuXHypo_1):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXHypo_LbMuMuLambda"):
        EFBMuMuXHypo_1.__init__(self, name)
        #super( TrigEFBMuMuXHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # decay modes flags
        self.AcceptBplus = False
        self.AcceptBd = False
        self.AcceptBs = False
        self.AcceptLb = True
        self.AcceptBc = False
        
class EFBMuMuXHypo_BcMuMuDs (EFBMuMuXHypo_1):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXHypo_BcMuMuDs"):
        EFBMuMuXHypo_1.__init__(self, name)
        #super( TrigEFBMuMuXHypo, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        # decay modes flags
        self.AcceptBplus = False
        self.AcceptBd = False
        self.AcceptBs = False
        self.AcceptLb = False
        self.AcceptBc = True
        
class EFBMuMuXHypo_BcMuMuDs_tight (EFBMuMuXHypo_BcMuMuDs):
    __slots__ = []
    def __init__(self, name = "EFBMuMuXHypo_BcMuMuDs_tight"):
        EFBMuMuXHypo_BcMuMuDs.__init__(self, name)
        #super( TrigEFBMuMuXHypo, self ).__init__( name )
        
        # Bc
        self.LowerBcMassCut = 5450.
        
