# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2DielectronMassFex, TrigL2DielectronMassHypo
from AthenaCommon.SystemOfUnits import GeV
from TrigEgammaHypo.TrigL2DielectronMassHypoMonitoring import *

class TrigL2DielectronMassFex_Jpsiee ( TrigL2DielectronMassFex ):
    __slots__ = []
    def __init__(self, name="TrigL2DielectronMassFex_Jpsiee"):
        super(TrigL2DielectronMassFex_Jpsiee, self).__init__(name)
        
        # Use Cluster Mode (use TrigEMCluster as Probe)
        self.UseClusterAsProbe=True
        
        # ElectronMass: used in calculation of invariant mass
        self.ElectronMass=0.511
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        
        # cuts (not used in use cluster mode)
        self.OppositeSign=True
        self.CommonVertex=True
        self.ValidElectron=True
        
        # require invariant mass within window
        self.LowerMassCut=1.0*GeV
        self.UpperMassCut=15.0*GeV

        self.AthenaMonTools = [ TrigL2DielectronMassOnlineMonitoring(), TrigL2DielectronMassValidationMonitoring_Jpsiee() ] 

class TrigL2DielectronMassFex_Upsiee ( TrigL2DielectronMassFex ):
    __slots__ = []
    def __init__(self, name="TrigL2DielectronMassFex_Upsiee"):
        super(TrigL2DielectronMassFex_Upsiee, self).__init__(name)

        # Use Cluster Mode (use TrigEMCluster as Probe)
        self.UseClusterAsProbe=True

        # ElectronMass: used in calculation of invariant mass
        self.ElectronMass=0.511

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False

        # cuts (not used in use cluster mode)
        self.OppositeSign=True
        self.CommonVertex=True
        self.ValidElectron=True

        # require invariant mass within window
        self.LowerMassCut=1.0*GeV
        self.UpperMassCut=30.0*GeV

        self.AthenaMonTools = [ TrigL2DielectronMassOnlineMonitoring(), TrigL2DielectronMassValidationMonitoring_Upsiee() ]

class TrigL2DielectronMassFex_Zee ( TrigL2DielectronMassFex ):
    __slots__ = []
    def __init__(self, name="TrigL2DielectronMassFex_Zee"):
        super(TrigL2DielectronMassFex_Zee, self).__init__(name)
        
        # Use Cluster Mode (use TrigEMCluster as Probe)
        # Change for 20.11.X.Y.Z for 2016
        # Required to prevent timeouts to use EMCluster
        self.UseClusterAsProbe=True
        
        # ElectronMass: used in calculation of invariant mass
        self.ElectronMass=0.511

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        
        # cuts (not used in use cluster mode)
        self.OppositeSign=True
        self.CommonVertex=True
        self.ValidElectron=True
        
        # require invariant mass within window
        self.LowerMassCut=20.0*GeV
        self.UpperMassCut=200.0*GeV

        self.AthenaMonTools = [ TrigL2DielectronMassOnlineMonitoring(), TrigL2DielectronMassValidationMonitoring_Zee() ] 
        
class TrigL2DielectronMassHypo_Jpsiee ( TrigL2DielectronMassHypo ):
    __slots__ = []
    def __init__(self, name="TrigL2DielectronMassHypo_Jpsiee"):
        super(TrigL2DielectronMassHypo_Jpsiee, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        
        # cuts
        self.OppositeSign=True
        self.CommonVertex=True
        self.ValidElectron=True
        
        # require invariant mass within window
        self.LowerMassCut=1*GeV
        self.UpperMassCut=6*GeV
        
        self.AthenaMonTools = [ TrigL2DielectronMassOnlineMonitoring(), TrigL2DielectronMassValidationMonitoring_Jpsiee() ] 

class TrigL2DielectronMassHypo_Upsiee ( TrigL2DielectronMassHypo ):
    __slots__ = []
    def __init__(self, name="TrigL2DielectronMassHypo_Upsiee"):
        super(TrigL2DielectronMassHypo_Upsiee, self).__init__(name)

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False

        # cuts
        self.OppositeSign=True
        self.CommonVertex=True
        self.ValidElectron=True

        # require invariant mass within window
        self.LowerMassCut=6*GeV
        self.UpperMassCut=20*GeV

        self.AthenaMonTools = [ TrigL2DielectronMassOnlineMonitoring(), TrigL2DielectronMassValidationMonitoring_Upsiee() ]
        
class TrigL2DielectronMassHypo_Zee ( TrigL2DielectronMassHypo ):
    __slots__ = []
    def __init__(self, name="TrigL2DielectronMassHypo_Zee"):
        super(TrigL2DielectronMassHypo_Zee, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        
        # cuts
        self.OppositeSign=True
        self.CommonVertex=True
        self.ValidElectron=True
        
        # require invariant mass within window
        self.LowerMassCut=45.0*GeV
        self.UpperMassCut=180.0*GeV
        
        self.AthenaMonTools = [ TrigL2DielectronMassOnlineMonitoring(), TrigL2DielectronMassValidationMonitoring_Zee() ] 
        
class TrigL2DielectronMassHypo_Zeg ( TrigL2DielectronMassHypo ):
    __slots__ = []
    def __init__(self, name="TrigL2DielectronMassHypo_Zeg"):
        super(TrigL2DielectronMassHypo_Zeg, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        
        # cuts
        self.OppositeSign=False
        self.CommonVertex=False
        self.ValidElectron=False
        
        # require invariant mass within window
        self.LowerMassCut=45.0*GeV
        self.UpperMassCut=100000.0*GeV
        
        self.AthenaMonTools = [ TrigL2DielectronMassOnlineMonitoring(), TrigL2DielectronMassValidationMonitoring_Zee() ] 

class TrigL2DielectronMassHypo_Zee2 ( TrigL2DielectronMassHypo ):
    __slots__ = []
    def __init__(self, name="TrigL2DielectronMassHypo_Zee2"):
        super(TrigL2DielectronMassHypo_Zee2, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        
        # cuts
        self.OppositeSign=True
        self.CommonVertex=True
        self.ValidElectron=True
        
        # require invariant mass within window
        self.LowerMassCut=30.0*GeV
        self.UpperMassCut=140.0*GeV
        
        self.AthenaMonTools = [ TrigL2DielectronMassOnlineMonitoring(), TrigL2DielectronMassValidationMonitoring_Zee() ] 
        
class TrigL2DielectronMassHypo_ZeeTight ( TrigL2DielectronMassHypo ):
    __slots__ = []
    def __init__(self, name="TrigL2DielectronMassHypo_ZeeTight"):
        super(TrigL2DielectronMassHypo_ZeeTight, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        
        # cuts
        self.OppositeSign=True
        self.CommonVertex=True
        self.ValidElectron=True
        
        # require invariant mass within window
        self.LowerMassCut=80.0*GeV
        self.UpperMassCut=100.0*GeV
        
        self.AthenaMonTools = [ TrigL2DielectronMassOnlineMonitoring(), TrigL2DielectronMassValidationMonitoring_Zee() ] 
        
