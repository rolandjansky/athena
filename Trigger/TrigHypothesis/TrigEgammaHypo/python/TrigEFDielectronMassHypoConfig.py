# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigEgammaHypo.TrigEgammaHypoConf import TrigEFDielectronMassFex, TrigEFDielectronMassHypo
from AthenaCommon.SystemOfUnits import GeV
from TrigEgammaHypo.TrigEFDielectronMassHypoMonitoring import *

class TrigEFDielectronMassFex_Zee ( TrigEFDielectronMassFex ):
    __slots__ = []
    def __init__(self, name="TrigEFDielectronMassFex_Zee"):
        super(TrigEFDielectronMassFex_Zee, self).__init__(name)

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False

        # require invariant mass within window
        self.LowerMassCut=40.0*GeV
        self.UpperMassCut=200.0*GeV

        self.AthenaMonTools = [ TrigEFDielectronMassOnlineMonitoring(), TrigEFDielectronMassValidationMonitoring_Zee() ] 
        
class TrigEFDielectronMassFexElectronCluster_Zee ( TrigEFDielectronMassFex ):
    __slots__ = []
    def __init__(self, name="TrigEFDielectronMassFexElectronCluster_Zee"):
        super(TrigEFDielectronMassFexElectronCluster_Zee, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False

        # require invariant mass within window
        self.LowerMassCut=40.0*GeV
        self.UpperMassCut=200.0*GeV

        self.AthenaMonTools = [ TrigEFDielectronMassOnlineMonitoring(), TrigEFDielectronMassValidationMonitoring_Zee() ] 

class TrigEFDielectronMassHypo_Zee ( TrigEFDielectronMassHypo ):
    __slots__ = []
    def __init__(self, name="TrigEFDielectronMassHypo_Zee"):
        super(TrigEFDielectronMassHypo_Zee, self).__init__(name)

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        self.useElectronElectron = True
        self.useElectronCluster = False
                
        # require invariant mass within window
        self.LowerMassCut=45.0*GeV
        self.UpperMassCut=180.0*GeV
        
        self.AthenaMonTools = [ TrigEFDielectronMassOnlineMonitoring(), TrigEFDielectronMassValidationMonitoring_Zee() ] 
        
class TrigEFDielectronMassHypoElectronCluster_Zee ( TrigEFDielectronMassHypo ):
    __slots__ = []
    def __init__(self, name="TrigEFDielectronMassHypoElectronCluster_Zee"):
        super(TrigEFDielectronMassHypoElectronCluster_Zee, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        self.useElectronElectron = False
        self.useElectronCluster = True
                
        # require invariant mass within window
        self.LowerMassCut=45.0*GeV
        self.UpperMassCut=180.0*GeV
        self.LowerMassElectronClusterCut=45.0*GeV
        self.UpperMassElectronClusterCut=180.0*GeV
        
        self.AthenaMonTools = [ TrigEFDielectronMassOnlineMonitoring(), TrigEFDielectronMassValidationMonitoring_Zee() ] 

class TrigEFDielectronMassHypo_Zee2 ( TrigEFDielectronMassHypo ):
    __slots__ = []
    def __init__(self, name="TrigEFDielectronMassHypo_Zee2"):
        super(TrigEFDielectronMassHypo_Zee2, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        self.useElectronElectron = True
        self.useElectronCluster = False

        # require invariant mass within window
        self.LowerMassCut=40.0*GeV
        self.UpperMassCut=130.0*GeV
        
        self.AthenaMonTools = [ TrigEFDielectronMassOnlineMonitoring(), TrigEFDielectronMassValidationMonitoring_Zee() ] 

class TrigEFDielectronMassHypo_ZeeTight ( TrigEFDielectronMassHypo ):
    __slots__ = []
    def __init__(self, name="TrigEFDielectronMassHypo_ZeeTight"):
        super(TrigEFDielectronMassHypo_ZeeTight, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        self.useElectronElectron = True
        self.useElectronCluster = False

        # require invariant mass within window
        self.LowerMassCut=80.0*GeV
        self.UpperMassCut=100.0*GeV
        
        self.AthenaMonTools = [ TrigEFDielectronMassOnlineMonitoring(), TrigEFDielectronMassValidationMonitoring_Zee() ] 
        
class TrigEFDielectronMassFex_Jpsi ( TrigEFDielectronMassFex ):
    __slots__ = []
    def __init__(self, name="TrigEFDielectronMassFex_Jpsi"):
        super(TrigEFDielectronMassFex_Jpsi, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        
        # require invariant mass within window
        self.LowerMassCut=0.5*GeV
        self.UpperMassCut=15.0*GeV

        self.AthenaMonTools = [ TrigEFDielectronMassOnlineMonitoring(), TrigEFDielectronMassValidationMonitoring_Jpsi() ] 
        
class TrigEFDielectronMassHypo_Jpsi ( TrigEFDielectronMassHypo ):
    __slots__ = []
    def __init__(self, name="TrigEFDielectronMassHypo_Jpsi"):
        super(TrigEFDielectronMassHypo_Jpsi, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        self.useElectronElectron = False
        self.useElectronCluster = True
                
        # require invariant mass within window
        #self.LowerMassCut=1.0*GeV
        #self.UpperMassCut=5.0*GeV
        # Use Looser cuts for now
        self.LowerMassCut=1.0*GeV
        self.UpperMassCut=5.0*GeV
        self.LowerMassElectronClusterCut=1.0*GeV
        self.UpperMassElectronClusterCut=6.0*GeV
        
        self.AthenaMonTools = [ TrigEFDielectronMassOnlineMonitoring(), TrigEFDielectronMassValidationMonitoring_Jpsi() ] 

class TrigEFDielectronMassFex_Upsi ( TrigEFDielectronMassFex ):
    __slots__ = []
    def __init__(self, name="TrigEFDielectronMassFex_Upsi"):
        super(TrigEFDielectronMassFex_Upsi, self).__init__(name)

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        # require invariant mass within window
        self.LowerMassCut=1.0*GeV
        self.UpperMassCut=30.0*GeV

        self.AthenaMonTools = [ TrigEFDielectronMassOnlineMonitoring(), TrigEFDielectronMassValidationMonitoring_Upsi() ]

class TrigEFDielectronMassHypo_Upsi ( TrigEFDielectronMassHypo ):
    __slots__ = []
    def __init__(self, name="TrigEFDielectronMassHypo_Upsi"):
        super(TrigEFDielectronMassHypo_Upsi, self).__init__(name)

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        self.useElectronElectron = False
        self.useElectronCluster = True

        # require invariant mass within window
        self.LowerMassElectronClusterCut=6.0*GeV
        self.UpperMassElectronClusterCut=20.0*GeV

        self.AthenaMonTools = [ TrigEFDielectronMassOnlineMonitoring(), TrigEFDielectronMassValidationMonitoring_Upsi() ]        

#--------------------------------------------------------------
# Combine electron + cluster with 1 TeV upper mass
#--------------------------------------------------------------
class TrigEFDielectronMassFex_Zeg ( TrigEFDielectronMassFex ):
    __slots__ = []
    def __init__(self, name="TrigEFDielectronMassFex_Zeg"):
        super(TrigEFDielectronMassFex_Zeg, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False

        # require invariant mass within window
        self.LowerMassCut=55.0*GeV
        self.UpperMassCut=100000.0*GeV

        self.AthenaMonTools = [ TrigEFDielectronMassOnlineMonitoring(), TrigEFDielectronMassValidationMonitoring_Zee() ] 

class TrigEFDielectronMassHypo_Zeg ( TrigEFDielectronMassHypo ):
    __slots__ = []
    def __init__(self, name="TrigEFDielectronMassHypo_Zeg"):
        super(TrigEFDielectronMassHypo_Zeg, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        self.useElectronElectron = False
        self.useElectronCluster = True
                
        # require invariant mass within window
        self.LowerMassCut=55.0*GeV
        self.UpperMassCut=100000.0*GeV
        self.LowerMassElectronClusterCut=55.0*GeV
        self.UpperMassElectronClusterCut=100000.0*GeV
        
        self.AthenaMonTools = [ TrigEFDielectronMassOnlineMonitoring(), TrigEFDielectronMassValidationMonitoring_Zee() ] 

#--------------------------------------------------------------
# Higgs rare decay
# mergedtight electron with a cluster (seeded by e/g combined chain)
#--------------------------------------------------------------
class TrigEFDielectronMassFex_Heg ( TrigEFDielectronMassFex ):
    __slots__ = []
    def __init__(self, name="TrigEFDielectronMassFex_Heg"):
        super(TrigEFDielectronMassFex_Heg, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False

        # require invariant mass within window
        self.LowerMassCut=80.0*GeV
        self.UpperMassCut=100000.0*GeV

        self.AthenaMonTools = [ TrigEFDielectronMassOnlineMonitoring(), TrigEFDielectronMassValidationMonitoring_Zee() ] 

class TrigEFDielectronMassHypo_Heg ( TrigEFDielectronMassHypo ):
    __slots__ = []
    def __init__(self, name="TrigEFDielectronMassHypo_Heg"):
        super(TrigEFDielectronMassHypo_Heg, self).__init__(name)
        
        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll=False
        self.useElectronElectron = False
        self.useElectronCluster = True
                
        # require invariant mass within window
        self.LowerMassCut=90.0*GeV
        self.UpperMassCut=100000.0*GeV
        self.LowerMassElectronClusterCut=90.0*GeV
        self.UpperMassElectronClusterCut=100000.0*GeV
        
        self.AthenaMonTools = [ TrigEFDielectronMassOnlineMonitoring(), TrigEFDielectronMassValidationMonitoring_Zee() ] 
